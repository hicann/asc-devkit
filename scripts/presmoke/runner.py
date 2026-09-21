#!/usr/bin/python3
# coding=utf-8

# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

from __future__ import annotations

import os
import queue
import shutil
import signal
import subprocess
import concurrent.futures
import threading
import time
from contextlib import contextmanager
from dataclasses import dataclass
from pathlib import Path
from typing import Callable, Iterable, Iterator, List, Optional, Union

from .model import Cell, Command, NpuStats, RunCellsResult, RunResult, StepResult


NpuGate = Callable[[Callable[[], int]], tuple[int, float]]

PRESERVE_BUILD_ARTIFACT_CASES = {"01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op"}
PARALLEL_OPS_PACKAGE_CASE = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/parallel_ops_package"
CUSTOM_OP_CASE = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op"
CUSTOM_OP_STATIC_LIB_CASE = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op_static_lib"
CUSTOM_OP_DEPENDENT_CASES = {
    "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclnn_invocation",
    "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclop_invocation",
    "01_simd_cpp_api/02_features/00_framework/01_tensorflow/tensorflow_builtin",
    "01_simd_cpp_api/02_features/00_framework/01_tensorflow/tensorflow_custom",
    "01_simd_cpp_api/02_features/00_framework/02_onnx/onnx_plugin",
    "04_aicpu/02_features/00_framework/00_pytorch/tiling_sink_programming",
}
RUN_AFTER_CASES = {PARALLEL_OPS_PACKAGE_CASE: CUSTOM_OP_DEPENDENT_CASES | {CUSTOM_OP_CASE}}
BUILD_AFTER_CASES = {
    CUSTOM_OP_CASE: {CUSTOM_OP_STATIC_LIB_CASE},
    **{dependent: {CUSTOM_OP_CASE} for dependent in CUSTOM_OP_DEPENDENT_CASES},
}
CPU_RUN_TIMEOUT = 300


class NpuSlotPool:
    def __init__(self, slots: int) -> None:
        if slots <= 0:
            raise ValueError("npu slots must be positive")
        self._sem = threading.BoundedSemaphore(slots)

    def gate(self, fn: Callable[[], int]) -> tuple[int, float]:
        started = time.monotonic()
        with self._sem:
            wait_s = time.monotonic() - started
            return fn(), wait_s


@dataclass(frozen=True)
class RunOptions:
    log_dir: Path
    timeout: int
    cpu_run_timeout: int = CPU_RUN_TIMEOUT
    keep_artifacts: bool = False
    npu_gate: Optional[NpuGate] = None


@dataclass(frozen=True)
class PipelineOptions:
    log_dir: Path
    timeout: int
    cpu_run_timeout: int = CPU_RUN_TIMEOUT
    keep_artifacts: bool = False
    jobs: int = 1
    npu_slots: int = 1
    cpu_run_slots: Optional[int] = None
    stages: str = "all"


@dataclass(frozen=True)
class StepRecord:
    command: Command
    rc: int
    duration_s: float
    wait_s: float
    uses_npu: bool


@dataclass(frozen=True)
class StageRunOptions:
    log_dir: Path
    timeout: int
    cpu_run_timeout: int
    keep_artifacts: bool
    stage_name: str


@dataclass(frozen=True)
class CommandRunOptions:
    timeout: int
    cpu_run_timeout: int
    npu_gate: Optional[NpuGate] = None
    initial_npu_wait_s: float = 0.0


@dataclass(frozen=True)
class CommandContext:
    cell: Cell
    run_dir: Path
    log: object
    stage_log: object


@dataclass(frozen=True)
class SingleCellRunState:
    log_file: Path
    stage_log_dir: Path
    run_dir: Path
    use_build_dir: bool


def run_cell_with_options(cell: Cell, options: RunOptions) -> RunResult:
    started = time.monotonic()
    state = prepare_single_cell_run(cell, options.log_dir)
    status, reason, failing_step, rc, steps = execute_single_cell_commands(cell, options, state)
    duration = time.monotonic() - started
    if should_remove_build_dir(cell, state.use_build_dir, options.keep_artifacts, status):
        shutil.rmtree(cell.build_dir, ignore_errors=True)
    return RunResult(
        example=cell.example.rel_path,
        arch=cell.arch,
        mode=cell.mode,
        status=status,
        reason=reason,
        failing_step=failing_step,
        rc=rc,
        duration_s=duration,
        log_file=str(state.log_file),
        stage_log_files=stage_log_files_for_cell(options.log_dir, cell),
        source=cell.example.source,
        steps=steps,
    )


def prepare_single_cell_run(cell: Cell, log_dir: Path) -> SingleCellRunState:
    log_dir.mkdir(parents=True, exist_ok=True)
    clear_stage_logs(log_dir, cell)
    use_build_dir = uses_build_directory(cell)
    run_dir = cell.build_dir if use_build_dir else cell.example.path
    if use_build_dir:
        clean_build_dir(cell.build_dir)
        cell.build_dir.mkdir(parents=True, exist_ok=True)
    else:
        clean_build_dir(cell.build_dir)
    return SingleCellRunState(
        log_file=log_dir / f"{safe_log_name(cell.example.rel_path)}__{cell.mode}.log",
        stage_log_dir=log_dir / "stages",
        run_dir=run_dir,
        use_build_dir=use_build_dir,
    )


def uses_build_directory(cell: Cell) -> bool:
    if cell.example.source == "case-runner":
        return False
    return any(command.kind in {"cmake", "make", "package_run"} for command in cell.commands)


def execute_single_cell_commands(
    cell: Cell, options: RunOptions, state: SingleCellRunState
) -> tuple[str, str, Optional[str], Optional[int], List[StepResult]]:
    steps: List[StepResult] = []
    with state.log_file.open("w", encoding="utf-8", errors="replace") as log:
        log.write(f"example={cell.example.rel_path}\narch={cell.arch}\nmode={cell.mode}\nrun_dir={state.run_dir}\n\n")
        for command in cell.commands:
            if not command.raw or command.kind == "skip":
                continue
            step = run_single_command_stage(command, cell, options, state, log)
            steps.append(step)
            if step.rc != 0:
                reason = "timeout" if step.rc == 124 else "nonzero rc"
                return "FAIL", reason, command.raw, step.rc, steps
    return "PASS", "", None, 0, steps


def run_single_command_stage(
    command: Command, cell: Cell, options: RunOptions, state: SingleCellRunState, log
) -> StepResult:
    stage_name = stage_name_for_command(command)
    stage_log_file = state.stage_log_dir / f"{safe_log_name(cell.example.rel_path)}__{cell.mode}__{stage_name}.log"
    with open_stage_log(stage_log_file, cell, state.run_dir, stage_name) as stage_log:
        log.write(f"\n$ {command.raw}\n")
        log.flush()
        stage_log.write(f"\n$ {command.raw}\n")
        stage_log.flush()
        record = execute_command_step(command, CommandContext(cell, state.run_dir, log, stage_log), options)
        step = step_result_from_record(record)
        write_step_result(record, log, stage_log)
    return step


def step_result_from_record(step: StepRecord) -> StepResult:
    return StepResult(
        command=step.command.raw,
        kind=step.command.kind,
        rc=step.rc,
        duration_s=step.duration_s,
        wait_s=step.wait_s,
        npu_slot=step.uses_npu,
        timeout=step.rc == 124,
    )


def run_cells_pipeline_with_options(cells: Iterable[Cell], options: PipelineOptions) -> RunCellsResult:
    cells_list = list(cells)
    try:
        if options.stages == "build":
            return run_build_only(cells_list, options)
        if options.jobs <= 1:
            npu_pool = NpuSlotPool(options.npu_slots)
            results = [
                run_cell_with_options(
                    cell,
                    RunOptions(
                        options.log_dir, options.timeout, options.cpu_run_timeout, options.keep_artifacts, npu_pool.gate
                    ),
                )
                for cell in cells_list
            ]
            return RunCellsResult(results, NpuStats(slots=options.npu_slots, queue_model="serial"))
        return _PipelineExecutor(cells_list, options).run()
    finally:
        keep_artifacts = options.keep_artifacts or options.stages == "build"
        cleanup_run_artifacts(cells_list, options.log_dir, keep_artifacts)


def run_build_only(cells: List[Cell], options: PipelineOptions) -> RunCellsResult:
    def build(cell: Cell) -> RunResult:
        return run_cell_stage(
            cell,
            StageRunOptions(
                options.log_dir, options.timeout, options.cpu_run_timeout, keep_artifacts=True, stage_name="build"
            ),
            commands=build_stage_commands(cell.commands),
        ).result

    with concurrent.futures.ThreadPoolExecutor(max_workers=options.jobs) as executor:
        results = list(executor.map(build, cells))
    return RunCellsResult(results, NpuStats(slots=0, queue_model="build-only"))


def cleanup_run_artifacts(cells: Iterable[Cell], log_dir: Path, keep_artifacts: bool) -> None:
    if keep_artifacts:
        return
    for cell in cells:
        if cell.example.source != "case-runner":
            shutil.rmtree(cell.build_dir, ignore_errors=True)
            continue
        cleanup_commands = [command for command in cell.commands if command.kind == "clean"]
        if not cleanup_commands:
            shutil.rmtree(cell.build_dir, ignore_errors=True)
            continue
        log_file = log_dir / f"{safe_log_name(cell.example.rel_path)}__{cell.mode}.log"
        with log_file.open("a", encoding="utf-8", errors="replace") as log:
            for command in cleanup_commands:
                log.write(f"\n[CLEANUP] $ {command.raw}\n")
                rc = _run_command(command, cell.example.path, log, timeout=60)
                log.write(f"[CLEANUP] rc={rc}\n")


class _PipelineExecutor:
    def __init__(self, cells: List[Cell], options: PipelineOptions) -> None:
        self.cells = cells
        self.options = options
        uses_npu_queue = cells_use_npu_queue(cells)
        self.run_slots = options.npu_slots if uses_npu_queue else max(options.cpu_run_slots or options.jobs, 1)
        queue_model = "pipeline" if uses_npu_queue else "pipeline-cpu"
        self.npu_pool = NpuSlotPool(options.npu_slots)
        self.ready: "queue.Queue[Union[_PipelineItem, None]]" = queue.Queue()
        self.verify_ready: "queue.Queue[Union[_PipelineItem, None]]" = queue.Queue()
        self.results: List[RunResult] = []
        self.results_lock = threading.Lock()
        self.npu_stats = _NpuPipelineStats(self.run_slots, queue_model)
        self.planned_keys = {cell.key for cell in cells}
        self.completed_keys: set[str] = set()
        self.completed_status: dict[str, str] = {}
        self.deferred_items: List[tuple[_PipelineItem, set[str]]] = []
        self.dependency_lock = threading.Lock()
        self.dependency_condition = threading.Condition(self.dependency_lock)

    @staticmethod
    def _start_threads(npu_threads: List[threading.Thread], verify_threads: List[threading.Thread]) -> None:
        for thread in npu_threads:
            thread.start()
        for thread in verify_threads:
            thread.start()

    def run(self) -> RunCellsResult:
        npu_threads = [
            threading.Thread(target=self.npu_worker, args=(idx,), name=f"presmoke-npu-{idx}")
            for idx in range(self.run_slots)
        ]
        verify_threads = [
            threading.Thread(target=self.verify_worker, name=f"presmoke-verify-{idx}")
            for idx in range(self.options.jobs)
        ]
        self.start_threads(npu_threads, verify_threads)
        try:
            self.run_build_workers()
            while True:
                self.ready.join()
                self.verify_ready.join()
                with self.dependency_lock:
                    deferred = bool(self.deferred_items)
                if self.ready.unfinished_tasks == 0 and self.verify_ready.unfinished_tasks == 0:
                    if deferred:
                        raise RuntimeError("presmoke case dependency cannot be satisfied")
                    break
        finally:
            self.stop_threads(npu_threads, verify_threads)
        return RunCellsResult(self.results, self.npu_stats.snapshot())

    def append_result(self, result: RunResult) -> None:
        with self.results_lock:
            self.results.append(result)
        ready_items: List[_PipelineItem] = []
        result_key = f"{result.example}|{result.arch}|{result.mode}"
        with self.dependency_condition:
            self.completed_keys.add(result_key)
            self.completed_status[result_key] = result.status
            pending: List[tuple[_PipelineItem, set[str]]] = []
            for item, required_keys in self.deferred_items:
                if required_keys <= self.completed_keys:
                    ready_items.append(item)
                else:
                    pending.append((item, required_keys))
            self.deferred_items = pending
            self.dependency_condition.notify_all()
        for item in ready_items:
            self.ready.put(item)

    def required_keys(self, cell: Cell) -> set[str]:
        required_cases = RUN_AFTER_CASES.get(cell.example.rel_path, set())
        required_keys = {f"{case}|{cell.arch}|{cell.mode}" for case in required_cases}
        return required_keys & self.planned_keys

    def wait_for_build_dependencies(self, cell: Cell) -> List[str]:
        required_cases = BUILD_AFTER_CASES.get(cell.example.rel_path, set())
        case_keys = {case: f"{case}|{cell.arch}|{cell.mode}" for case in required_cases}
        required_keys = set(case_keys.values()) & self.planned_keys
        with self.dependency_condition:
            while not required_keys <= self.completed_keys:
                self.dependency_condition.wait()
            return sorted(
                case
                for case, key in case_keys.items()
                if key in required_keys and self.completed_status.get(key) != "PASS"
            )

    def build_worker(self, cell: Cell) -> None:
        failed_dependencies = self.wait_for_build_dependencies(cell)
        if failed_dependencies:
            self.append_result(dependency_skip_result(cell, self.options.log_dir, failed_dependencies))
            return
        item = run_cell_stage(
            cell,
            StageRunOptions(
                self.options.log_dir,
                self.options.timeout,
                self.options.cpu_run_timeout,
                keep_artifacts=True,
                stage_name="build",
            ),
            commands=build_stage_commands(cell.commands),
        )
        if item.result.status == "PASS":
            required_keys = self.required_keys(cell)
            with self.dependency_lock:
                if required_keys <= self.completed_keys:
                    enqueue = True
                else:
                    self.deferred_items.append((item, required_keys))
                    enqueue = False
            if enqueue:
                self.ready.put(item)
        else:
            self.append_result(item.result)

    def npu_worker(self, slot_idx: int) -> None:
        while True:
            wait_started = time.monotonic()
            item = self.ready.get()
            self.npu_stats.add_idle(slot_idx, time.monotonic() - wait_started)
            try:
                if item is None:
                    return
                self.finish_npu_item(item, slot_idx)
            finally:
                self.ready.task_done()

    def finish_npu_item(self, item: "_PipelineItem", slot_idx: int) -> None:
        busy_started = time.monotonic()
        result = finish_cell_after_build(
            item,
            timeout=self.options.timeout,
            cpu_run_timeout=self.options.cpu_run_timeout,
            npu_gate=self.npu_pool.gate,
        )
        self.npu_stats.add_busy(slot_idx, time.monotonic() - busy_started)
        if result.status == "PASS":
            self.verify_ready.put(item)
        else:
            self.append_result(result)

    def verify_worker(self) -> None:
        while True:
            item = self.verify_ready.get()
            try:
                if item is None:
                    return
                result = finish_cell_verify(
                    item,
                    timeout=self.options.timeout,
                    cpu_run_timeout=self.options.cpu_run_timeout,
                    keep_artifacts=self.options.keep_artifacts,
                )
                self.append_result(result)
            finally:
                self.verify_ready.task_done()

    def run_build_workers(self) -> None:
        with concurrent.futures.ThreadPoolExecutor(max_workers=self.options.jobs) as executor:
            futures = [executor.submit(self.build_worker, cell) for cell in self.cells]
            for future in concurrent.futures.as_completed(futures):
                future.result()

    def start_threads(self, npu_threads: List[threading.Thread], verify_threads: List[threading.Thread]) -> None:
        self._start_threads(npu_threads, verify_threads)

    def stop_threads(self, npu_threads: List[threading.Thread], verify_threads: List[threading.Thread]) -> None:
        for _ in npu_threads:
            self.ready.put(None)
        for _ in verify_threads:
            self.verify_ready.put(None)
        for thread in npu_threads:
            thread.join()
        for thread in verify_threads:
            thread.join()


def dependency_skip_result(cell: Cell, log_dir: Path, failed_dependencies: List[str]) -> RunResult:
    reason = "prerequisite failed: " + ", ".join(failed_dependencies)
    log_dir.mkdir(parents=True, exist_ok=True)
    log_file = log_dir / f"{safe_log_name(cell.example.rel_path)}__{cell.mode}.log"
    log_file.write_text(
        f"example={cell.example.rel_path}\narch={cell.arch}\nmode={cell.mode}\n\nSKIP: {reason}\n", encoding="utf-8"
    )
    return RunResult(
        example=cell.example.rel_path,
        arch=cell.arch,
        mode=cell.mode,
        status="SKIP",
        reason=reason,
        rc=0,
        log_file=str(log_file),
        source=cell.example.source,
    )


class _NpuPipelineStats:
    def __init__(self, slots: int, queue_model: str) -> None:
        self._slots = slots
        self._queue_model = queue_model
        self._idle_by_slot = [0.0 for _ in range(slots)]
        self._busy_by_slot = [0.0 for _ in range(slots)]
        self._lock = threading.Lock()

    def add_idle(self, slot_idx: int, duration_s: float) -> None:
        with self._lock:
            self._idle_by_slot[slot_idx] += duration_s

    def add_busy(self, slot_idx: int, duration_s: float) -> None:
        with self._lock:
            self._busy_by_slot[slot_idx] += duration_s

    def snapshot(self) -> NpuStats:
        with self._lock:
            idle_by_slot = list(self._idle_by_slot)
            busy_by_slot = list(self._busy_by_slot)
        return NpuStats(
            slots=self._slots,
            queue_model=self._queue_model,
            idle_s=sum(idle_by_slot),
            busy_s=sum(busy_by_slot),
            max_slot_idle_s=max(idle_by_slot, default=0.0),
            max_slot_busy_s=max(busy_by_slot, default=0.0),
        )


@dataclass
class _PipelineItem:
    cell: Cell
    result: RunResult
    run_dir: Path
    log_file: Path
    started: float
    ready_at: float = 0.0

    def __post_init__(self) -> None:
        if not self.ready_at:
            self.ready_at = time.monotonic()


def run_cell_stage(cell: Cell, options: StageRunOptions, commands: Iterable[Command]) -> _PipelineItem:
    started = time.monotonic()
    options.log_dir.mkdir(parents=True, exist_ok=True)
    log_file = options.log_dir / f"{safe_log_name(cell.example.rel_path)}__{cell.mode}.log"
    use_build_dir = any(command.kind in {"cmake", "make", "package_run"} for command in cell.commands)
    if cell.example.source == "case-runner":
        use_build_dir = False
    run_dir = cell.build_dir if use_build_dir else cell.example.path
    if use_build_dir:
        clean_build_dir(cell.build_dir)
        cell.build_dir.mkdir(parents=True, exist_ok=True)
    elif options.stage_name == "build":
        clean_build_dir(cell.build_dir)
    if options.stage_name == "build":
        clear_stage_logs(options.log_dir, cell)

    result = RunResult(
        example=cell.example.rel_path,
        arch=cell.arch,
        mode=cell.mode,
        status="PASS",
        reason="",
        rc=0,
        log_file=str(log_file),
        stage_log_files=[],
        source=cell.example.source,
        steps=[],
    )
    with log_file.open("w", encoding="utf-8", errors="replace") as log:
        log.write(f"example={cell.example.rel_path}\narch={cell.arch}\nmode={cell.mode}\nrun_dir={run_dir}\n\n")
        run_commands(
            result,
            commands,
            CommandContext(cell, run_dir, log, None),
            CommandRunOptions(options.timeout, options.cpu_run_timeout),
        )
    result.duration_s = time.monotonic() - started
    if should_remove_build_dir(cell, use_build_dir, options.keep_artifacts, result.status):
        shutil.rmtree(cell.build_dir, ignore_errors=True)
    return _PipelineItem(cell, result, run_dir, log_file, started)


def finish_cell_after_build(
    item: _PipelineItem, timeout: int, cpu_run_timeout: int, npu_gate: Optional[NpuGate]
) -> RunResult:
    cell = item.cell
    with item.log_file.open("a", encoding="utf-8", errors="replace") as log:
        if item.result.status == "PASS":
            run_commands(
                item.result,
                run_stage_commands(cell.commands),
                CommandContext(cell, item.run_dir, log, None),
                CommandRunOptions(timeout, cpu_run_timeout, npu_gate, time.monotonic() - item.ready_at),
            )
    item.result.duration_s = time.monotonic() - item.started
    return item.result


def finish_cell_verify(item: _PipelineItem, timeout: int, cpu_run_timeout: int, keep_artifacts: bool) -> RunResult:
    cell = item.cell
    with item.log_file.open("a", encoding="utf-8", errors="replace") as log:
        if item.result.status == "PASS":
            run_commands(
                item.result,
                verify_stage_commands(cell.commands),
                CommandContext(cell, item.run_dir, log, None),
                CommandRunOptions(timeout, cpu_run_timeout),
            )
    item.result.duration_s = time.monotonic() - item.started
    if should_remove_build_dir(cell, cell.build_dir.exists(), keep_artifacts, item.result.status):
        shutil.rmtree(cell.build_dir, ignore_errors=True)
    return item.result


def run_commands(
    result: RunResult, commands: Iterable[Command], context: CommandContext, options: CommandRunOptions
) -> None:
    pending_npu_wait_s = options.initial_npu_wait_s
    for command in commands:
        if not command.raw or command.kind == "skip":
            continue
        stage_name = stage_name_for_command(command)
        stage_log_file = stage_log_path(context.log.name, context.cell, stage_name)
        if str(stage_log_file) not in result.stage_log_files:
            result.stage_log_files.append(str(stage_log_file))
        with open_stage_log(stage_log_file, context.cell, context.run_dir, stage_name) as stage_log:
            context.log.write(f"\n$ {command.raw}\n")
            context.log.flush()
            stage_log.write(f"\n$ {command.raw}\n")
            stage_log.flush()
            step = execute_command_step(
                command,
                CommandContext(context.cell, context.run_dir, context.log, stage_log),
                RunOptions(
                    Path(context.log.name).parent, options.timeout, options.cpu_run_timeout, npu_gate=options.npu_gate
                ),
            )
            if step.uses_npu and pending_npu_wait_s:
                step = StepRecord(
                    step.command, step.rc, step.duration_s, step.wait_s + pending_npu_wait_s, step.uses_npu
                )
                pending_npu_wait_s = 0.0
            step_rc = step.rc
            append_step_result(result.steps, step, context.log, stage_log)
        if step_rc != 0:
            result.status = "FAIL"
            result.reason = "timeout" if step_rc == 124 else "nonzero rc"
            result.failing_step = command.raw
            result.rc = step_rc
            return


def build_stage_commands(commands: Iterable[Command]) -> List[Command]:
    return [command for command in commands if command.kind not in {"run", "package_run", "verify"}]


def run_stage_commands(commands: Iterable[Command]) -> List[Command]:
    return [command for command in commands if command.kind in {"run", "package_run"}]


def cells_use_npu_queue(cells: Iterable[Cell]) -> bool:
    return any(command_uses_npu(cell, command) for cell in cells for command in run_stage_commands(cell.commands))


def verify_stage_commands(commands: Iterable[Command]) -> List[Command]:
    return [command for command in commands if command.kind == "verify"]


def stage_name_for_command(command: Command) -> str:
    if command.kind in {"run", "package_run"}:
        return "run"
    if command.kind == "verify":
        return "verify"
    return "build"


def stage_log_path(case_log_name: str, cell: Cell, stage_name: str) -> Path:
    log_name = f"{safe_log_name(cell.example.rel_path)}__{cell.mode}__{stage_name}.log"
    return Path(case_log_name).parent / "stages" / log_name


def stage_log_files_for_cell(log_dir: Path, cell: Cell) -> List[str]:
    root = log_dir / "stages"
    prefix = f"{safe_log_name(cell.example.rel_path)}__{cell.mode}__"
    return [str(path) for path in sorted(root.glob(f"{prefix}*.log"))]


def clear_stage_logs(log_dir: Path, cell: Cell) -> None:
    for path in stage_log_files_for_cell(log_dir, cell):
        Path(path).unlink(missing_ok=True)


@contextmanager
def open_stage_log(path: Path, cell: Cell, run_dir: Path, stage_name: str) -> Iterator:
    append = path.exists()
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("a" if append else "w", encoding="utf-8", errors="replace") as log:
        if not append:
            log.write(
                f"example={cell.example.rel_path}\n"
                f"arch={cell.arch}\n"
                f"mode={cell.mode}\n"
                f"stage={stage_name}\n"
                f"run_dir={run_dir}\n\n"
            )
        yield log


class _TeeLog:
    def __init__(self, *logs) -> None:
        self._logs = logs

    def write(self, value: str) -> None:
        for log in self._logs:
            log.write(value)

    def flush(self) -> None:
        for log in self._logs:
            log.flush()


def clean_build_dir(build_dir: Path) -> None:
    if build_dir.exists():
        shutil.rmtree(build_dir, ignore_errors=True)


def should_remove_build_dir(cell: Cell, use_build_dir: bool, keep_artifacts: bool, status: str) -> bool:
    if not use_build_dir or keep_artifacts or status != "PASS":
        return False
    return cell.example.rel_path not in PRESERVE_BUILD_ARTIFACT_CASES


def command_uses_npu(cell: Cell, command: Command) -> bool:
    if cell.mode != "npu":
        return False
    if command.kind in {"run", "package_run"}:
        return True
    stripped = command.raw.strip()
    if stripped.startswith("./") or stripped.startswith("msprof "):
        return True
    if "mssanitizer" in stripped:
        return True
    return False


def select_timeout(
    cell: Cell, command: Command, uses_npu: bool, npu_run_timeout: int, cpu_run_timeout: int = CPU_RUN_TIMEOUT
) -> Optional[int]:
    if uses_npu:
        return npu_run_timeout
    if cell.mode == "cpu" and command.kind in {"run", "package_run"}:
        return cpu_run_timeout
    return None


def execute_command_step(command: Command, context: CommandContext, options: RunOptions) -> StepRecord:
    command_started = time.monotonic()
    uses_npu = command_uses_npu(context.cell, command)
    step_timeout = select_timeout(context.cell, command, uses_npu, options.timeout, options.cpu_run_timeout)
    if uses_npu and options.npu_gate:
        step_rc, wait_s = options.npu_gate(
            lambda: _run_command(command, context.run_dir, _TeeLog(context.log, context.stage_log), step_timeout)
        )
    else:
        step_rc = _run_command(command, context.run_dir, _TeeLog(context.log, context.stage_log), step_timeout)
        wait_s = 0.0
    return StepRecord(
        command=command, rc=step_rc, duration_s=time.monotonic() - command_started, wait_s=wait_s, uses_npu=uses_npu
    )


def _run_command(command: Command, cwd: Path, log, timeout: Optional[int]) -> int:
    env = os.environ.copy()
    env.update(command.env)
    try:
        proc = subprocess.Popen(
            ["/bin/bash", "-lc", command.raw],
            shell=False,
            cwd=str(cwd),
            env=env,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            preexec_fn=os.setsid if hasattr(os, "setsid") else None,
        )
        try:
            stdout, _ = proc.communicate(timeout=timeout)
        except subprocess.TimeoutExpired:
            _kill_process_group(proc)
            stdout, _ = proc.communicate(timeout=10)
            log.write(stdout or "")
            log.write(f"\n[TIMEOUT] after {timeout}s\n")
            log.flush()
            return 124
    except OSError as exc:
        log.write(f"[ERROR] failed to start command: {exc}\n")
        return 127
    log.write(stdout or "")
    log.flush()
    return proc.returncode


def append_step_result(steps: List[StepResult], step: StepRecord, log, stage_log) -> None:
    steps.append(step_result_from_record(step))
    write_step_result(step, log, stage_log)


def write_step_result(step: StepRecord, log, stage_log) -> None:
    step_line = (
        f"\n[STEP] rc={step.rc} duration_s={step.duration_s:.3f} "
        f"wait_s={step.wait_s:.3f} npu_slot={int(step.uses_npu)}\n"
    )
    log.write(step_line)
    log.flush()
    stage_log.write(step_line)
    stage_log.flush()


def _kill_process_group(proc: subprocess.Popen) -> None:
    try:
        if hasattr(os, "killpg") and proc.pid:
            os.killpg(proc.pid, signal.SIGTERM)
        else:
            proc.terminate()
    except OSError:
        return
    time.sleep(1)
    if proc.poll() is None:
        try:
            if hasattr(os, "killpg") and proc.pid:
                os.killpg(proc.pid, signal.SIGKILL)
            else:
                proc.kill()
        except OSError:
            pass


def safe_log_name(rel_path: str) -> str:
    return rel_path.replace("/", "__")
