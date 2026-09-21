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

import argparse
import logging
import os
import shutil
import subprocess
import sys
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import List

from .arch import detect_arch
from .case_runners import CaseRunnerOptions, build_case_runner_cells_with_skips
from .model import RunReport, RunResult, Suggestion
from .report import print_console, write_json, write_markdown, write_suggestions
from .runner import CPU_RUN_TIMEOUT, PipelineOptions, run_cells_pipeline_with_options
from .scheduler import export_schedule_file, read_schedule_file, schedule_cells, ScheduleOptions
from .suggest import dedupe_suggestions
from .werror import enabled_from_env as werror_enabled_from_env


LOG = logging.getLogger(__name__)


@dataclass
class RuntimeConfig:
    modes: List[str]
    jobs: int
    make_jobs: int
    cpu_run_slots: int
    cpu_run_timeout: int
    project_root: Path
    host_arch: str
    results_dir: Path
    log_dir: Path
    started_at: str
    parallel_config: dict[str, int]


@dataclass
class CaseSelection:
    cells: list
    suggestions: List[Suggestion]
    skipped_results: List[RunResult]


def main(argv: list[str] | None = None) -> int:
    logging.basicConfig(level=logging.INFO, format="%(message)s", stream=sys.stdout)
    args = parse_args(argv)
    config = build_runtime_config(args)
    if not config.host_arch:
        LOG.error("cannot detect host arch; pass --arch or PRESMOKE_ARCH")
        return 2
    configure_parallel_env(config.project_root, config.make_jobs)
    selected = collect_cases(args, config)
    try:
        cells = schedule_selected_cases(selected, args, config)
    except (FileNotFoundError, ValueError) as err:
        LOG.error("%s", err)
        return 2
    if args.export_schedule:
        export_schedule_file(cells, Path(args.export_schedule).resolve())
    if args.dry_run:
        return emit_dry_run(args, config, cells, selected)
    return run_and_emit(args, config, cells, selected)


def build_runtime_config(args: argparse.Namespace) -> RuntimeConfig:
    modes = parse_modes(args.modes)
    jobs = resolve_jobs(args.jobs, modes=modes)
    make_jobs = resolve_make_jobs(args.make_jobs, jobs)
    cpu_run_slots = resolve_cpu_run_slots(args.cpu_run_slots)
    cpu_run_timeout = resolve_positive_int(args.cpu_run_timeout)
    project_root = Path(os.environ.get("PRESMOKE_PROJECT_ROOT", Path.cwd())).resolve()
    results_dir = Path(args.results).resolve()
    return RuntimeConfig(
        modes=modes,
        jobs=jobs,
        make_jobs=make_jobs,
        cpu_run_slots=cpu_run_slots,
        cpu_run_timeout=cpu_run_timeout,
        project_root=project_root,
        host_arch=detect_arch(args.arch),
        results_dir=results_dir,
        log_dir=Path(args.log_dir).resolve() if args.log_dir else results_dir / "logs",
        started_at=now_iso(),
        parallel_config=parallel_config(args, jobs, make_jobs, cpu_run_slots, cpu_run_timeout),
    )


def parallel_config(
    args: argparse.Namespace, jobs: int, make_jobs: int, cpu_run_slots: int, cpu_run_timeout: int
) -> dict[str, int]:
    return {
        "jobs": jobs,
        "npu_slots": args.npu_slots,
        "make_jobs": make_jobs,
        "cpu_run_slots": cpu_run_slots,
        "cpu_run_timeout": cpu_run_timeout,
        "werror": int(bool(args.werror)),
    }


def collect_cases(args: argparse.Namespace, config: RuntimeConfig) -> CaseSelection:
    cells, suggestions, skipped_results = build_case_runner_cells_with_skips(
        config.project_root,
        CaseRunnerOptions(
            arch=config.host_arch,
            modes=config.modes,
            includes=args.filter,
            excludes=args.exclude,
            exact_includes=args.exact_filter,
            werror=args.werror,
        ),
    )
    return CaseSelection(cells, dedupe_suggestions(suggestions), skipped_results)


def schedule_selected_cases(selected: CaseSelection, args: argparse.Namespace, config: RuntimeConfig) -> list:
    schedule_file = resolve_schedule_file(
        config.project_root, config.host_arch, config.modes, args.schedule, args.schedule_file
    )
    validate_fixed_schedule_coverage(selected, args, schedule_file)
    return schedule_cells(
        selected.cells,
        ScheduleOptions(
            schedule=args.schedule,
            schedule_report=Path(args.schedule_report).resolve() if args.schedule_report else None,
            schedule_file=schedule_file,
            frontload_count=args.schedule_frontload_count,
            jobs=config.jobs,
        ),
    )


def validate_fixed_schedule_coverage(
    selected: CaseSelection, args: argparse.Namespace, schedule_file: Path | None
) -> None:
    if args.schedule != "fixed" or schedule_file is None:
        return
    if args.filter or args.exclude:
        return

    schedule_order = read_schedule_file(schedule_file)
    schedule_names = set(schedule_order)
    planned_names = {cell.example.rel_path for cell in selected.cells}
    planned_names.update(result.example for result in selected.skipped_results)

    schedule_only = sorted(schedule_names - planned_names)
    planned_only = sorted(planned_names - schedule_names)
    duplicates = sorted({name for name in schedule_order if schedule_order.count(name) > 1})
    if not schedule_only and not planned_only and not duplicates:
        return

    lines = [f"fixed schedule coverage mismatch: {schedule_file}"]
    if schedule_only:
        lines.append(f"  schedule-only cases ({len(schedule_only)}): {', '.join(schedule_only)}")
    if planned_only:
        lines.append(f"  unscheduled planned cases ({len(planned_only)}): {', '.join(planned_only)}")
    if duplicates:
        lines.append(f"  duplicate schedule cases ({len(duplicates)}): {', '.join(duplicates)}")
    message = "\n".join(lines)
    if args.strict_schedule:
        raise ValueError(message)
    LOG.warning(message)


def emit_dry_run(args: argparse.Namespace, config: RuntimeConfig, cells: list, selected: CaseSelection) -> int:
    dry_results = [
        RunResult(
            cell.example.rel_path, cell.arch, cell.mode, "SKIP", "dry-run", duration_s=0, source=cell.example.source
        )
        for cell in cells
    ]
    report = RunReport(
        config.host_arch,
        config.modes,
        config.started_at,
        now_iso(),
        dry_results + selected.skipped_results,
        selected.suggestions,
        parallel_config=config.parallel_config,
    )
    emit_reports(report, args, config.results_dir)
    return 0


def run_and_emit(args: argparse.Namespace, config: RuntimeConfig, cells: list, selected: CaseSelection) -> int:
    run = run_cells_pipeline_with_options(
        cells,
        PipelineOptions(
            log_dir=config.log_dir,
            timeout=args.timeout,
            cpu_run_timeout=config.cpu_run_timeout,
            keep_artifacts=args.keep_artifacts,
            jobs=config.jobs,
            npu_slots=args.npu_slots,
            cpu_run_slots=config.cpu_run_slots,
            stages=args.stages,
        ),
    )
    results = run.results
    results.extend(selected.skipped_results)
    results = sorted(results, key=lambda result: (result.example, result.mode))
    report = RunReport(
        config.host_arch,
        config.modes,
        config.started_at,
        now_iso(),
        results,
        selected.suggestions,
        run.npu_stats,
        config.parallel_config,
    )
    emit_reports(report, args, config.results_dir)
    return 1 if any(result.status == "FAIL" for result in results) else 0


def parse_args(argv: list[str] | None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="asc-devkit presmoke watcher v2")
    parser.add_argument("--arch")
    parser.add_argument("--modes", default="npu")
    parser.add_argument("--jobs", default="1")
    parser.add_argument("--npu-slots", type=int, default=1)
    parser.add_argument("--cpu-run-slots", default="auto")
    parser.add_argument("--make-jobs", default="auto")
    parser.add_argument("--cpu-run-timeout", default=str(CPU_RUN_TIMEOUT))
    parser.add_argument("--filter", action="append", default=[])
    parser.add_argument("--exact-filter", action="append", default=[])
    parser.add_argument("--exclude", action="append", default=[])
    parser.add_argument("--timeout", type=int, default=120)
    parser.add_argument("--report-format", choices=["console", "json", "markdown", "all"], default="console")
    parser.add_argument("--results", default="presmoke_reports")
    parser.add_argument("--log-dir")
    parser.add_argument("--keep-artifacts", action="store_true")
    parser.add_argument("--stages", choices=["all", "build"], default="all")
    parser.add_argument(
        "--schedule",
        choices=["default", "build-desc", "frontload-build-desc", "npu-idle-min", "fixed"],
        default="default",
    )
    parser.add_argument("--schedule-report")
    parser.add_argument("--schedule-file")
    parser.add_argument("--export-schedule")
    parser.add_argument("--schedule-frontload-count", type=int, default=1)
    parser.add_argument("--strict-schedule", action="store_true")
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("--werror", action="store_true")
    parser.add_argument("-v", "--verbose", action="store_true")
    args = parser.parse_args(argv)
    args.werror = bool(args.werror or werror_enabled_from_env())
    return args


def resolve_schedule_file(
    project_root: Path, host_arch: str, modes: List[str], schedule: str, explicit_schedule_file: str | None
) -> Path | None:
    if explicit_schedule_file:
        return Path(explicit_schedule_file).resolve()
    if schedule != "fixed":
        return None
    mode = modes[0] if modes else "npu"
    schedule_file = project_root / "scripts" / "presmoke" / "schedules" / f"{host_arch}_{mode}.txt"
    return schedule_file if schedule_file.exists() else None


def resolve_jobs(value: str | int, cpu_count: int | None = None, modes: List[str] | None = None) -> int:
    if isinstance(value, int):
        return max(value, 1)
    if value != "auto":
        return max(int(value), 1)

    cpus = cpu_count if cpu_count is not None else detect_cpu_count()
    if modes and all(mode == "cpu" for mode in modes):
        return resolve_cpu_build_jobs(cpus)
    if cpus >= 160:
        return 12
    if cpus >= 80:
        return 8
    if cpus >= 32:
        return 6
    return 4


def resolve_cpu_build_jobs(cpu_count: int) -> int:
    cpus = max(cpu_count, 1)
    return max(min((cpus + 3) // 4, 64), 1)


def resolve_cpu_run_slots(value: str | int, cpu_count: int | None = None) -> int:
    if isinstance(value, int):
        return max(value, 1)
    if value != "auto":
        return max(int(value), 1)
    cpus = cpu_count if cpu_count is not None else detect_cpu_count()
    return max(cpus, 1)


def resolve_make_jobs(value: str | int, build_jobs: int, cpu_count: int | None = None) -> int:
    if isinstance(value, int):
        return max(value, 1)
    if value != "auto":
        return max(int(value), 1)
    cpus = cpu_count if cpu_count is not None else detect_cpu_count()
    return max(cpus // max(build_jobs, 1), 1)


def resolve_positive_int(value: str | int) -> int:
    if isinstance(value, int):
        return max(value, 1)
    return max(int(value), 1)


def configure_parallel_env(project_root: Path, make_jobs: int) -> None:
    wrappers_dir = project_root / "scripts" / "presmoke" / "wrappers"
    if wrappers_dir.is_dir():
        path_entries = os.environ.get("PATH", "").split(os.pathsep)
        wrappers = str(wrappers_dir)
        if wrappers not in path_entries:
            os.environ["PATH"] = os.pathsep.join([wrappers, *path_entries])
    os.environ["PRESMOKE_MAKE_JOBS"] = str(make_jobs)
    os.environ.setdefault("CMAKE_BUILD_PARALLEL_LEVEL", str(make_jobs))


def detect_cpu_count() -> int:
    lscpu = shutil.which("lscpu")
    if not lscpu:
        return os.cpu_count() or 1
    try:
        result = subprocess.run([lscpu], check=False, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True)
    except (OSError, ValueError):
        return os.cpu_count() or 1

    for line in result.stdout.splitlines():
        if line.startswith("CPU(s):"):
            try:
                return max(int(line.split(":", 1)[1].strip()), 1)
            except ValueError:
                break
    return os.cpu_count() or 1


def parse_modes(value: str) -> List[str]:
    modes = [item.strip() for item in value.split(",") if item.strip()]
    return modes or ["npu"]


def emit_reports(report: RunReport, args: argparse.Namespace, results_dir: Path) -> None:
    results_dir.mkdir(parents=True, exist_ok=True)
    fmt = args.report_format
    if fmt in ("console", "all"):
        print_console(report)
    if fmt in ("json", "all"):
        write_json(report, results_dir / "report.json")
    if fmt in ("markdown", "all"):
        write_markdown(report, results_dir / "report.md")
    if report.suggestions:
        write_suggestions(report.suggestions, results_dir / "suggestions.md")


def now_iso() -> str:
    return datetime.now(timezone.utc).astimezone().isoformat(timespec="seconds")
