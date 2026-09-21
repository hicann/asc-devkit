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

import json
import csv
import re
import statistics
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, List, Optional

from .model import Cell


CUSTOM_OP_CASE = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op"
CUSTOM_OP_STATIC_LIB_CASE = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op_static_lib"
PARALLEL_OPS_PACKAGE_CASE = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/parallel_ops_package"
CUSTOM_OP_DEPENDENT_CASES = {
    "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclnn_invocation",
    "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclop_invocation",
    "01_simd_cpp_api/02_features/00_framework/01_tensorflow/tensorflow_builtin",
    "01_simd_cpp_api/02_features/00_framework/01_tensorflow/tensorflow_custom",
    "01_simd_cpp_api/02_features/00_framework/02_onnx/onnx_plugin",
    "04_aicpu/02_features/00_framework/00_pytorch/tiling_sink_programming",
}


@dataclass(frozen=True)
class ScheduleOptions:
    schedule: str = "default"
    schedule_report: Optional[Path] = None
    schedule_file: Optional[Path] = None
    frontload_count: int = 1
    jobs: int = 1


def schedule_cells(cells: Iterable[Cell], options: ScheduleOptions = ScheduleOptions()) -> List[Cell]:
    cells_list = list(cells)
    schedule = options.schedule
    if schedule == "default":
        return enforce_required_order(cells_list)
    if schedule == "fixed":
        if options.schedule_file is None:
            return enforce_required_order(cells_list)
        return enforce_required_order(apply_fixed_schedule(cells_list, options.schedule_file))
    if schedule not in {"build-desc", "frontload-build-desc", "npu-idle-min"}:
        raise ValueError(f"unknown schedule: {schedule}")
    if options.schedule_report is None:
        return enforce_required_order(cells_list)

    build_seconds = load_build_seconds(options.schedule_report)
    ranked_pairs = sorted(
        enumerate(cells_list), key=lambda item: (-build_seconds.get(item[1].example.rel_path, 0.0), item[0])
    )
    if schedule == "build-desc":
        return enforce_required_order([cell for _, cell in ranked_pairs])
    if schedule == "npu-idle-min":
        return schedule_npu_idle_min(cells_list, options.schedule_report, options.jobs)

    frontload = [cell for _, cell in ranked_pairs[: max(options.frontload_count, 0)]]
    frontloaded = {cell.key for cell in frontload}
    return enforce_required_order(frontload + [cell for cell in cells_list if cell.key not in frontloaded])


def schedule_npu_idle_min(cells: List[Cell], report_path: Path, jobs: int) -> List[Cell]:
    timings = load_pipeline_stage_seconds(report_path)
    if not timings:
        return enforce_required_order(cells)

    candidates = build_npu_idle_candidates(cells, timings, max(jobs, 1))
    return min(
        candidates,
        key=lambda candidate: (
            custom_op_dependency_violation_s(candidate, report_path, jobs=max(jobs, 1)),
            simulate_pipeline_npu_idle(candidate, report_path, jobs=max(jobs, 1)),
            simulate_pipeline_makespan(candidate, report_path, jobs=max(jobs, 1)),
            [cell.example.rel_path for cell in candidate],
        ),
    )


def apply_fixed_schedule(cells: List[Cell], schedule_file: Path) -> List[Cell]:
    if not schedule_file.exists():
        raise FileNotFoundError(f"schedule file not found: {schedule_file}")
    order = read_schedule_file(schedule_file)
    by_name = {cell.example.rel_path: cell for cell in cells}
    used: set[str] = set()
    scheduled: List[Cell] = []
    for name in order:
        cell = by_name.get(name)
        if cell is None or cell.key in used:
            continue
        scheduled.append(cell)
        used.add(cell.key)
    scheduled.extend(cell for cell in cells if cell.key not in used)
    return scheduled


def read_schedule_file(schedule_file: Path) -> List[str]:
    names: List[str] = []
    for line in schedule_file.read_text(encoding="utf-8").splitlines():
        item = line.split("#", 1)[0].strip()
        if item:
            names.append(item)
    return names


def export_schedule_file(cells: List[Cell], output_path: Path) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text("\n".join(cell.example.rel_path for cell in cells) + "\n", encoding="utf-8")


def build_npu_idle_candidates(
    cells: List[Cell], timings: dict[str, tuple[float, float, float]], jobs: int
) -> List[List[Cell]]:
    indexed = list(enumerate(cells))
    build_seconds = with_default_seconds({case: timing[0] for case, timing in timings.items()}, cells)
    run_seconds = with_default_seconds({case: timing[1] for case, timing in timings.items()}, cells)
    verify_seconds = with_default_seconds({case: timing[2] for case, timing in timings.items()}, cells)

    candidates: List[List[Cell]] = [
        cells,
        [cell for _, cell in sorted(indexed, key=lambda item: (-build_seconds[item[1].example.rel_path], item[0]))],
        [cell for _, cell in sorted(indexed, key=lambda item: (build_seconds[item[1].example.rel_path], item[0]))],
        [cell for _, cell in sorted(indexed, key=lambda item: (-run_seconds[item[1].example.rel_path], item[0]))],
        [cell for _, cell in sorted(indexed, key=lambda item: (-verify_seconds[item[1].example.rel_path], item[0]))],
        [
            cell
            for _, cell in sorted(
                indexed,
                key=lambda item: (
                    -(run_seconds[item[1].example.rel_path] + verify_seconds[item[1].example.rel_path]),
                    item[0],
                ),
            )
        ],
    ]
    candidates.append(frontload_long_builds(cells, build_seconds, jobs))
    for long_count in range(1, max(jobs, 1)):
        candidates.append(mix_long_and_short_builds(cells, build_seconds, long_count, jobs - long_count))

    deduped: List[List[Cell]] = []
    seen: set[tuple[str, ...]] = set()
    for candidate in candidates:
        ordered = delay_custom_op_dependents(enforce_required_order(candidate), timings, jobs)
        key = tuple(cell.key for cell in ordered)
        if key not in seen:
            deduped.append(ordered)
            seen.add(key)
    return deduped


def mix_long_and_short_builds(
    cells: List[Cell], build_seconds: dict[str, float], long_count: int, short_count: int
) -> List[Cell]:
    long_cells = sorted(cells, key=lambda cell: (-build_seconds[cell.example.rel_path], cells.index(cell)))
    short_cells = sorted(cells, key=lambda cell: (build_seconds[cell.example.rel_path], cells.index(cell)))
    used: set[str] = set()
    result: List[Cell] = []
    while len(result) < len(cells):
        add_next_from(long_cells, used, result, long_count)
        add_next_from(short_cells, used, result, short_count)
    return result


def frontload_long_builds(cells: List[Cell], build_seconds: dict[str, float], jobs: int) -> List[Cell]:
    first_screen_count = min(max(jobs, 1), len(cells))
    ranked = sorted(cells, key=lambda cell: (-build_seconds[cell.example.rel_path], cells.index(cell)))
    first_screen = ranked[:first_screen_count]
    first_screen_keys = {cell.key for cell in first_screen}
    return first_screen + [cell for cell in cells if cell.key not in first_screen_keys]


def add_next_from(cells: List[Cell], used: set[str], result: List[Cell], count: int) -> None:
    for _ in range(count):
        next_cell = next((cell for cell in cells if cell.key not in used), None)
        if next_cell is None:
            return
        result.append(next_cell)
        used.add(next_cell.key)


def simulate_pipeline_npu_idle(cells: List[Cell], report_path: Path, jobs: int = 1) -> float:
    return simulate_pipeline(cells, report_path, jobs)[0]


def simulate_pipeline_makespan(cells: List[Cell], report_path: Path, jobs: int = 1) -> float:
    return simulate_pipeline(cells, report_path, jobs)[1]


def simulate_pipeline(cells: List[Cell], report_path: Path, jobs: int = 1) -> tuple[float, float]:
    timings = load_pipeline_stage_seconds(report_path)
    build_seconds = with_default_seconds({case: timing[0] for case, timing in timings.items()}, cells)
    run_seconds = with_default_seconds({case: timing[1] for case, timing in timings.items()}, cells)
    verify_seconds = with_default_seconds({case: timing[2] for case, timing in timings.items()}, cells)
    build_finishes = simulate_build_finishes(cells, build_seconds, jobs)

    npu_available = 0.0
    idle_s = 0.0
    verify_ready: list[tuple[float, int, Cell]] = []
    for ready_at, index, cell in sorted(build_finishes, key=lambda item: (item[0], item[1])):
        if npu_available < ready_at:
            idle_s += ready_at - npu_available
            npu_available = ready_at
        npu_available += run_seconds[cell.example.rel_path]
        verify_ready.append((npu_available, index, cell))

    verify_done = simulate_verify_done(verify_ready, verify_seconds, jobs)
    if verify_done > npu_available:
        idle_s += verify_done - npu_available
    return idle_s, max(npu_available, verify_done)


def simulate_verify_done(
    verify_ready: list[tuple[float, int, Cell]], verify_seconds: dict[str, float], jobs: int
) -> float:
    worker_available = [0.0 for _ in range(max(jobs, 1))]
    for ready_at, _, cell in sorted(verify_ready, key=lambda item: (item[0], item[1])):
        worker = min(range(len(worker_available)), key=lambda idx: worker_available[idx])
        start = max(worker_available[worker], ready_at)
        worker_available[worker] = start + verify_seconds[cell.example.rel_path]
    return max(worker_available) if worker_available else 0.0


def custom_op_dependency_violation_s(cells: List[Cell], report_path: Path, jobs: int = 1) -> float:
    timings = load_stage_seconds(report_path)
    build_seconds = with_default_seconds({case: build_s for case, (build_s, _) in timings.items()}, cells)
    return custom_op_dependency_violation_from_builds(cells, build_seconds, jobs)


def custom_op_dependency_violation_from_builds(cells: List[Cell], build_seconds: dict[str, float], jobs: int) -> float:
    windows = simulate_build_windows(cells, build_seconds, jobs)
    custom_window = windows.get(CUSTOM_OP_CASE)
    if custom_window is None:
        return 0.0
    custom_finish = custom_window[1]
    violation_s = 0.0
    for dependent in CUSTOM_OP_DEPENDENT_CASES:
        dependent_window = windows.get(dependent)
        if dependent_window is not None and dependent_window[0] < custom_finish:
            violation_s += custom_finish - dependent_window[0]
    return violation_s


def delay_custom_op_dependents(
    cells: List[Cell], timings: dict[str, tuple[float, float, float]], jobs: int
) -> List[Cell]:
    names = {cell.example.rel_path for cell in cells}
    if CUSTOM_OP_CASE not in names or not (CUSTOM_OP_DEPENDENT_CASES & names):
        return cells

    build_seconds = with_default_seconds({case: timing[0] for case, timing in timings.items()}, cells)
    if custom_op_dependency_violation_from_builds(cells, build_seconds, jobs) <= 0:
        return cells

    dependents = [cell for cell in cells if cell.example.rel_path in CUSTOM_OP_DEPENDENT_CASES]
    base = [cell for cell in cells if cell.example.rel_path not in CUSTOM_OP_DEPENDENT_CASES]
    custom_index = next((idx for idx, cell in enumerate(base) if cell.example.rel_path == CUSTOM_OP_CASE), len(base))
    best = base + dependents
    best_violation = custom_op_dependency_violation_from_builds(best, build_seconds, jobs)
    for insert_at in range(custom_index + 1, len(base) + 1):
        candidate = base[:insert_at] + dependents + base[insert_at:]
        violation = custom_op_dependency_violation_from_builds(candidate, build_seconds, jobs)
        if violation < best_violation:
            best = candidate
            best_violation = violation
        if violation <= 0:
            return candidate
    return best


def simulate_build_finishes(
    cells: List[Cell], build_seconds: dict[str, float], jobs: int
) -> List[tuple[float, int, Cell]]:
    windows = simulate_build_windows(cells, build_seconds, jobs)
    return [(windows[cell.example.rel_path][1], index, cell) for index, cell in enumerate(cells)]


def simulate_build_windows(
    cells: List[Cell], build_seconds: dict[str, float], jobs: int
) -> dict[str, tuple[float, float]]:
    worker_available = [0.0 for _ in range(max(jobs, 1))]
    windows: dict[str, tuple[float, float]] = {}
    for cell in cells:
        worker = min(range(len(worker_available)), key=lambda idx: worker_available[idx])
        start = worker_available[worker]
        finish = start + build_seconds[cell.example.rel_path]
        worker_available[worker] = finish
        windows[cell.example.rel_path] = (start, finish)
    return windows


def with_default_seconds(values: dict[str, float], cells: List[Cell]) -> dict[str, float]:
    known = [value for value in values.values() if value > 0]
    default = statistics.median(known) if known else 0.0
    return {cell.example.rel_path: values.get(cell.example.rel_path, default) for cell in cells}


def enforce_required_order(cells: List[Cell]) -> List[Cell]:
    return stable_topological_order(
        cells,
        {
            CUSTOM_OP_CASE: {CUSTOM_OP_STATIC_LIB_CASE},
            PARALLEL_OPS_PACKAGE_CASE: {CUSTOM_OP_STATIC_LIB_CASE, CUSTOM_OP_CASE, *CUSTOM_OP_DEPENDENT_CASES},
            **{dependent: {CUSTOM_OP_CASE} for dependent in CUSTOM_OP_DEPENDENT_CASES},
        },
    )


def stable_topological_order(cells: List[Cell], dependencies: dict[str, set[str]]) -> List[Cell]:
    present = {cell.example.rel_path for cell in cells}
    remaining = list(cells)
    ordered: List[Cell] = []
    ordered_names: set[str] = set()

    while remaining:
        moved = False
        for index, cell in enumerate(remaining):
            required = dependencies.get(cell.example.rel_path, set()) & present
            if required <= ordered_names:
                ordered.append(cell)
                ordered_names.add(cell.example.rel_path)
                del remaining[index]
                moved = True
                break
        if not moved:
            ordered.extend(remaining)
            break
    return ordered


def load_build_seconds(report_path: Path) -> dict[str, float]:
    return {case: build_s for case, (build_s, _) in load_stage_seconds(report_path).items()}


def load_stage_seconds(report_path: Path) -> dict[str, tuple[float, float]]:
    return {case: (timing[0], timing[1]) for case, timing in load_pipeline_stage_seconds(report_path).items()}


def load_pipeline_stage_seconds(report_path: Path) -> dict[str, tuple[float, float, float]]:
    if not report_path.exists():
        return {}
    if report_path.is_dir():
        timings = report_path / "ALL_CASE_TIMINGS.tsv"
        if timings.exists():
            return load_pipeline_stage_seconds(timings)
        estimates: dict[str, tuple[float, float, float]] = {}
        for report in sorted(report_path.glob("*/results/report.json")):
            estimates.update(load_pipeline_stage_seconds(report))
        return estimates
    if report_path.suffix == ".tsv":
        return load_pipeline_stage_seconds_from_tsv(report_path)
    data = json.loads(report_path.read_text(encoding="utf-8"))
    estimates: dict[str, tuple[float, float, float]] = {}
    for result in data.get("results", []):
        example = result.get("example")
        if not example:
            continue
        build_s = 0.0
        run_s = 0.0
        verify_s = 0.0
        for step in result.get("steps", []):
            kind = step.get("kind")
            if kind in {"clean", "build"}:
                build_s += float(step.get("duration_s") or 0.0)
            elif kind == "run":
                run_s += float(step.get("duration_s") or 0.0)
            elif kind == "verify":
                verify_s += float(step.get("duration_s") or 0.0)
        estimates[example] = (build_s, run_s, verify_s)
    return estimates


def load_pipeline_stage_seconds_from_tsv(report_path: Path) -> dict[str, tuple[float, float, float]]:
    estimates: dict[str, tuple[float, float, float]] = {}
    with report_path.open(encoding="utf-8", newline="") as handle:
        for row in csv.DictReader(handle, delimiter="\t"):
            example = row.get("example", "")
            if not example:
                continue
            if {"build_s", "run_s", "verify_s"} <= set(row):
                estimates[example] = (
                    parse_float(row.get("build_s")),
                    parse_float(row.get("run_s")),
                    parse_float(row.get("verify_s")),
                )
                continue
            if not row.get("steps"):
                continue
            build_s = parse_stage_duration(row.get("steps", ""), "clean")
            build_s += parse_stage_duration(row.get("steps", ""), "build")
            run_s = parse_stage_duration(row.get("steps", ""), "run")
            verify_s = parse_stage_duration(row.get("steps", ""), "verify")
            estimates[example] = (build_s, run_s, verify_s)
    return estimates


def parse_stage_duration(steps: str, kind: str) -> float:
    match = re.search(r"(?:^|; )%s:([0-9.]+)s" % re.escape(kind), steps)
    return float(match.group(1)) if match else 0.0


def parse_float(value: object) -> float:
    try:
        return float(value or 0.0)
    except (TypeError, ValueError):
        return 0.0
