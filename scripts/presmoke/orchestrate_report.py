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
import csv
import datetime
import json
import logging
import re
import statistics
import sys
import xml.etree.ElementTree as ET
from dataclasses import dataclass
from pathlib import Path
from typing import Any
from xml.dom import minidom

_XML_INVALID_CHARS = re.compile(r"[\x00-\x08\x0b\x0c\x0e-\x1f]")
_JUNIT_LOG_TAIL_BYTES = 100_000
LOG = logging.getLogger(__name__)

CUSTOM_OP_DEPENDENCY_GROUP = {
    "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op_static_lib",
    "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op",
    "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclnn_invocation",
    "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclop_invocation",
    "01_simd_cpp_api/02_features/00_framework/01_tensorflow/tensorflow_builtin",
    "01_simd_cpp_api/02_features/00_framework/01_tensorflow/tensorflow_custom",
    "01_simd_cpp_api/02_features/00_framework/02_onnx/onnx_plugin",
    "04_aicpu/02_features/00_framework/00_pytorch/tiling_sink_programming",
    "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/parallel_ops_package",
}


@dataclass(frozen=True)
class MarkdownRenderContext:
    root: Path
    primary: tuple[str, Path, dict[str, str], dict[str, Any]] | None
    retry_by_example: dict[str, list[tuple[str, dict[str, str], dict[str, Any]]]]
    effective_failures: list[dict[str, Any]]
    timings_path: Path
    failures_path: Path


def read_json(path: Path) -> dict[str, Any] | None:
    if not path.exists():
        return None
    with path.open(encoding="utf-8") as handle:
        return json.load(handle)


def read_meta(path: Path) -> dict[str, str]:
    meta: dict[str, str] = {}
    if path.exists():
        for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
            if "=" in line:
                key, value = line.split("=", 1)
                meta[key] = value
    return meta


def timeout_examples(report_path: Path) -> list[str]:
    data = read_json(report_path)
    if not data:
        return []
    return [
        result.get("example", "")
        for result in data.get("results", [])
        if result.get("status") == "FAIL" and result.get("reason") == "timeout"
    ]


def list_examples(report_path: Path) -> list[str]:
    data = read_json(report_path)
    if not data:
        return []
    return [str(result.get("example", "")) for result in data.get("results", []) if result.get("example")]


def shard_examples(
    report_path: Path,
    cards: list[str],
    schedule_report: Path | None = None,
    excludes: set[str] | None = None,
    jobs: int | None = None,
    fixed_shards: Path | None = None,
) -> list[tuple[str, str]]:
    examples = [example for example in list_examples(report_path) if example not in (excludes or set())]
    if not cards or not examples:
        return []
    if fixed_shards is not None:
        return load_fixed_shards(examples, cards, fixed_shards)

    schedule_report = schedule_report or builtin_timing_report(report_path)
    stage_timings = load_pipeline_stage_seconds(schedule_report) if schedule_report else {}
    if stage_timings:
        return shard_examples_by_pipeline_makespan(
            examples, cards, stage_timings, resolve_shard_jobs(report_path, jobs)
        )

    weights = load_duration_seconds(schedule_report)
    default_weight = default_duration_weight(weights, examples)
    groups = group_examples_for_sharding(examples)
    order_index = {example: index for index, example in enumerate(examples)}
    group_items = []
    for source, group_examples in groups.items():
        group_weight = sum(weights.get(example, default_weight) for example in group_examples)
        first_index = min(order_index[example] for example in group_examples)
        group_items.append((group_weight, first_index, source, group_examples))

    card_loads = {card: 0.0 for card in cards}
    card_groups: dict[str, list[list[str]]] = {card: [] for card in cards}
    for group_weight, _, _, group_examples in sorted(group_items, key=lambda item: (-item[0], item[1])):
        card = min(cards, key=lambda item: (card_loads[item], cards.index(item)))
        card_loads[card] += group_weight
        card_groups[card].append(group_examples)

    assignments = []
    for card in cards:
        for example in interleave_group_examples(card_groups[card], order_index):
            assignments.append((card, example))
    return assignments


def load_fixed_shards(examples: list[str], cards: list[str], shard_dir: Path) -> list[tuple[str, str]]:
    assignments: list[tuple[str, str]] = []
    assigned_examples: set[str] = set()
    duplicate_examples: set[str] = set()
    for index, card in enumerate(cards):
        shard_file = shard_dir / f"card_{index}.txt"
        if not shard_file.is_file():
            raise ValueError(f"fixed shard file is missing: {shard_file}")
        for raw_line in shard_file.read_text(encoding="utf-8").splitlines():
            example = raw_line.strip()
            if not example or example.startswith("#"):
                continue
            if example in assigned_examples:
                duplicate_examples.add(example)
            assigned_examples.add(example)
            assignments.append((card, example))

    planned_examples = set(examples)
    missing_examples = [example for example in examples if example not in assigned_examples]
    extra_examples = sorted(assigned_examples - planned_examples)
    errors = []
    if missing_examples:
        errors.append("missing planned cases: " + ", ".join(missing_examples))
    if extra_examples:
        errors.append("unplanned cases: " + ", ".join(extra_examples))
    if duplicate_examples:
        errors.append("duplicate cases: " + ", ".join(sorted(duplicate_examples)))
    if errors:
        raise ValueError(f"fixed shard coverage mismatch in {shard_dir}: " + "; ".join(errors))
    return assignments


def shard_examples_by_pipeline_makespan(
    examples: list[str], cards: list[str], stage_timings: dict[str, tuple[float, float, float]], jobs: int
) -> list[tuple[str, str]]:
    groups = group_examples_for_sharding(examples)
    order_index = {example: index for index, example in enumerate(examples)}
    default_timing = default_stage_timing(stage_timings, examples)
    group_items = []
    for source, group_examples in groups.items():
        group_weight = sum(sum(stage_timings.get(example, default_timing)) for example in group_examples)
        first_index = min(order_index[example] for example in group_examples)
        group_items.append((group_weight, first_index, source, group_examples))

    card_groups: dict[str, list[list[str]]] = {card: [] for card in cards}
    card_makespans = {card: 0.0 for card in cards}
    for _, first_index, _, group_examples in sorted(group_items, key=lambda item: (-item[0], item[1])):
        best_card = min(
            cards,
            key=lambda card: (
                estimate_card_makespan(card_groups[card] + [group_examples], order_index, stage_timings, jobs),
                card_makespans[card],
                cards.index(card),
            ),
        )
        card_groups[best_card].append(group_examples)
        card_makespans[best_card] = estimate_card_makespan(card_groups[best_card], order_index, stage_timings, jobs)

    assignments = []
    for card in cards:
        for example in interleave_group_examples(card_groups[card], order_index):
            assignments.append((card, example))
    return assignments


def estimate_card_makespan(
    groups: list[list[str]],
    order_index: dict[str, int],
    stage_timings: dict[str, tuple[float, float, float]],
    jobs: int,
) -> float:
    examples = interleave_group_examples(groups, order_index)
    default_timing = default_stage_timing(stage_timings, examples)
    return simulate_pipeline_makespan_for_examples(examples, stage_timings, default_timing, jobs)


def simulate_pipeline_makespan_for_examples(
    examples: list[str],
    stage_timings: dict[str, tuple[float, float, float]],
    default_timing: tuple[float, float, float],
    jobs: int,
) -> float:
    worker_available = [0.0 for _ in range(max(jobs, 1))]
    build_finishes = []
    for index, example in enumerate(examples):
        worker = min(range(len(worker_available)), key=lambda idx: worker_available[idx])
        start = worker_available[worker]
        build_s = stage_timings.get(example, default_timing)[0]
        worker_available[worker] = start + build_s
        build_finishes.append((worker_available[worker], index, example))

    npu_available = 0.0
    verify_ready = []
    for ready_at, index, example in sorted(build_finishes, key=lambda item: (item[0], item[1])):
        npu_available = max(npu_available, ready_at) + stage_timings.get(example, default_timing)[1]
        verify_ready.append((npu_available, index, example))

    verify_workers = [0.0 for _ in range(max(jobs, 1))]
    for ready_at, _, example in sorted(verify_ready, key=lambda item: (item[0], item[1])):
        worker = min(range(len(verify_workers)), key=lambda idx: verify_workers[idx])
        verify_workers[worker] = max(verify_workers[worker], ready_at) + stage_timings.get(example, default_timing)[2]
    return max(npu_available, max(verify_workers) if verify_workers else 0.0)


def default_stage_timing(
    stage_timings: dict[str, tuple[float, float, float]], examples: list[str]
) -> tuple[float, float, float]:
    known = [stage_timings[example] for example in examples if example in stage_timings]
    if not known:
        return (1.0, 1.0, 0.0)
    return tuple(statistics.median(timing[index] for timing in known) for index in range(3))


def builtin_timing_report(report_path: Path) -> Path | None:
    data = read_json(report_path)
    if not data:
        return None
    arch = data.get("arch")
    modes = data.get("modes") or []
    if not arch or not modes:
        for result in data.get("results", []):
            arch = arch or result.get("arch")
            if not modes and result.get("mode"):
                modes = [result.get("mode")]
            if arch and modes:
                break
    if not arch or not modes:
        return None
    mode = str(modes[0])
    candidate_root = Path(__file__).resolve().parents[2]
    for root in [Path.cwd(), candidate_root]:
        candidate = root / "scripts" / "presmoke" / "schedules" / f"{arch}_{mode}_timings.tsv"
        if candidate.exists():
            return candidate
    return None


def resolve_shard_jobs(report_path: Path, jobs: int | None) -> int:
    if jobs and jobs > 0:
        return jobs
    data = read_json(report_path) or {}
    parallel_config = data.get("parallel_config") or {}
    try:
        return max(int(parallel_config.get("jobs") or 1), 1)
    except (TypeError, ValueError):
        return 1


def group_examples_for_sharding(examples: list[str]) -> dict[str, list[str]]:
    groups: dict[str, list[str]] = {}
    for example in examples:
        group = "__custom_op_dependency_group__" if example in CUSTOM_OP_DEPENDENCY_GROUP else example
        groups.setdefault(group, []).append(example)
    return groups


def interleave_group_examples(groups: list[list[str]], order_index: dict[str, int]) -> list[str]:
    queues = [sorted(group, key=lambda item: order_index[item]) for group in groups if group]
    queues.sort(key=lambda group: order_index[group[0]])
    ordered: list[str] = []
    while queues:
        next_queues: list[list[str]] = []
        for group in queues:
            ordered.append(group.pop(0))
            if group:
                next_queues.append(group)
        queues = sorted(next_queues, key=lambda group: order_index[group[0]])
    return ordered


def load_duration_seconds(report_path: Path | None) -> dict[str, float]:
    if report_path is None or not report_path.exists():
        return {}
    if report_path.is_dir():
        timings = report_path / "ALL_CASE_TIMINGS.tsv"
        if timings.exists():
            return load_duration_seconds(timings)
        weights: dict[str, float] = {}
        for report in sorted(report_path.glob("*/results/report.json")):
            weights.update(load_duration_seconds(report))
        return weights
    if report_path.suffix == ".tsv":
        return load_duration_seconds_from_tsv(report_path)
    data = read_json(report_path)
    if not data:
        return {}
    return {
        str(result.get("example")): float(result.get("duration_s") or 0)
        for result in data.get("results", [])
        if result.get("example")
    }


def load_duration_seconds_from_tsv(report_path: Path) -> dict[str, float]:
    weights: dict[str, float] = {}
    with report_path.open(encoding="utf-8", newline="") as handle:
        for row in csv.DictReader(handle, delimiter="\t"):
            example = row.get("example", "")
            if not example:
                continue
            try:
                weights[example] = float(row.get("duration_s") or 0)
            except ValueError:
                weights[example] = 0.0
    return weights


def load_pipeline_stage_seconds(report_path: Path | None) -> dict[str, tuple[float, float, float]]:
    if report_path is None or not report_path.exists():
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
    data = read_json(report_path)
    if not data:
        return {}
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
                build_s += parse_float(step.get("duration_s"))
            elif kind == "run":
                run_s += parse_float(step.get("duration_s"))
            elif kind == "verify":
                verify_s += parse_float(step.get("duration_s"))
        estimates[str(example)] = (build_s, run_s, verify_s)
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


def default_duration_weight(weights: dict[str, float], examples: list[str]) -> float:
    known = [weights[example] for example in examples if weights.get(example, 0) > 0]
    return statistics.median(known) if known else 1.0


def retry_status(report_path: Path) -> str:
    data = read_json(report_path)
    if not data:
        return ""
    results = data.get("results", [])
    return results[0].get("status", "") if results else ""


def print_summary(root: Path) -> None:
    primary = load_primary_run(root)
    log_summary_header(root)
    if not primary:
        LOG.info("  Primary run: missing")
        LOG.info("========================================")
        return
    log_primary_summary(primary)
    LOG.info("========================================")


def log_summary_header(root: Path) -> None:
    effective_rc_path = root / "effective_rc.txt"
    effective_rc = effective_rc_path.read_text(encoding="utf-8").strip() if effective_rc_path.exists() else "1"
    LOG.info("========================================")
    LOG.info("Presmoke Summary:")
    LOG.info("  Output: %s", root)
    LOG.info("  Final Report: %s", root / "FINAL_REPORT.md")
    LOG.info("  Timings: %s", root / "ALL_CASE_TIMINGS.tsv")
    LOG.info("  Failures: %s", root / "FAILURES.tsv")
    LOG.info("  Effective RC: %s", effective_rc)


def log_primary_summary(primary: tuple[str, Path, dict[str, str], dict[str, Any]]) -> None:
    name, _, meta, data = primary
    summary = data.get("summary", {})
    results = data.get("results", [])
    npu_stats = data.get("npu_stats") or {}
    elapsed = meta.get("elapsed_sec", "")
    LOG.info("  Primary run: %s", name)
    LOG.info("  Arch/Card: %s/%s", meta.get("arch", ""), meta.get("card", ""))
    LOG.info("  Mode/Schedule: %s/%s", meta.get("modes", ""), meta.get("schedule", ""))
    LOG.info("  Jobs/NPU slots: %s/%s", meta.get("jobs", ""), meta.get("npu_slots", ""))
    LOG.info("  Elapsed: %ss", elapsed)
    LOG.info(
        "  Cases: total=%s pass=%s fail=%s skip=%s",
        len(results),
        summary.get("PASS", 0),
        summary.get("FAIL", 0),
        summary.get("SKIP", 0),
    )
    log_npu_stats(npu_stats)
    log_failed_cases(results)


def log_npu_stats(npu_stats: dict[str, Any]) -> None:
    if not npu_stats:
        return
    metric = "run_queue" if npu_stats.get("queue_model") == "pipeline-cpu" else "npu"
    LOG.info(
        "  %s: busy=%.3fs idle=%.3fs util=%.3f%%",
        metric,
        float(npu_stats.get("busy_s") or 0),
        float(npu_stats.get("idle_s") or 0),
        float(npu_stats.get("utilization") or 0) * 100,
    )


def log_failed_cases(results: list[dict[str, Any]]) -> None:
    failures = [result for result in results if result.get("status") == "FAIL"]
    if failures:
        LOG.info("  Failed cases:")
        for result in failures[:20]:
            LOG.info(
                "    %s reason=%s rc=%s duration=%.1fs",
                result.get("example", ""),
                result.get("reason", ""),
                result.get("rc", ""),
                float(result.get("duration_s") or 0),
            )
        if len(failures) > 20:
            LOG.info("    ... %s more", len(failures) - 20)


def load_primary_run(root: Path) -> tuple[str, Path, dict[str, str], dict[str, Any]] | None:
    runs = load_primary_runs(root)
    if not runs:
        return None
    if len(runs) == 1:
        return runs[0]
    return merge_primary_runs(root, runs)


def load_primary_runs(root: Path) -> list[tuple[str, Path, dict[str, str], dict[str, Any]]]:
    primary_runs: list[tuple[str, Path, dict[str, str], dict[str, Any]]] = []
    for report in sorted(root.glob("*/results/report.json")):
        run_dir = report.parents[1]
        if not run_dir.name.startswith("full_card"):
            continue
        data = read_json(report)
        if data:
            primary_runs.append((run_dir.name, run_dir, read_meta(run_dir / "meta.txt"), data))
    return primary_runs


def merge_primary_runs(
    root: Path, runs: list[tuple[str, Path, dict[str, str], dict[str, Any]]]
) -> tuple[str, Path, dict[str, str], dict[str, Any]]:
    first_meta = dict(runs[0][2])
    results: list[dict[str, Any]] = []
    busy_s = 0.0
    idle_s = 0.0
    for _, _, _, data in runs:
        results.extend(data.get("results", []))
        npu_stats = data.get("npu_stats") or {}
        busy_s += float(npu_stats.get("busy_s") or 0)
        idle_s += float(npu_stats.get("idle_s") or 0)

    summary = {
        "PASS": sum(1 for result in results if result.get("status") == "PASS"),
        "FAIL": sum(1 for result in results if result.get("status") == "FAIL"),
        "SKIP": sum(1 for result in results if result.get("status") == "SKIP"),
    }
    unique_cards = sorted({meta.get("card", "") for _, _, meta, _ in runs if meta.get("card", "")}, key=int)
    first_meta["card"] = ",".join(unique_cards)
    first_meta["elapsed_sec"] = merged_elapsed_sec(runs)
    first_meta["npu_slots"] = str(len(unique_cards) or len(runs))

    total_s = busy_s + idle_s
    parallel_config = dict(runs[0][3].get("parallel_config") or {})
    parallel_config["npu_slots"] = len(unique_cards) or len(runs)
    merged_data = {
        "summary": summary,
        "results": results,
        "parallel_config": parallel_config,
        "npu_stats": {
            "busy_s": busy_s,
            "idle_s": idle_s,
            "utilization": busy_s / total_s if total_s > 0 else 0,
            "queue_model": "multi-card",
        },
    }
    return "full_multi", root, first_meta, merged_data


def merged_elapsed_sec(runs: list[tuple[str, Path, dict[str, str], dict[str, Any]]]) -> str:
    starts = [parse_meta_time(meta.get("started_at", "")) for _, _, meta, _ in runs]
    finishes = [parse_meta_time(meta.get("finished_at", "")) for _, _, meta, _ in runs]
    starts = [value for value in starts if value]
    finishes = [value for value in finishes if value]
    if starts and finishes:
        return str(int((max(finishes) - min(starts)).total_seconds()))
    elapsed_values = [
        int(meta.get("elapsed_sec") or 0) for _, _, meta, _ in runs if str(meta.get("elapsed_sec") or "").isdigit()
    ]
    return str(max(elapsed_values) if elapsed_values else "")


def parse_meta_time(value: str) -> datetime.datetime | None:
    if not value:
        return None
    try:
        return datetime.datetime.fromisoformat(value)
    except ValueError:
        pass
    try:
        return datetime.datetime.strptime(value, "%Y-%m-%dT%H:%M:%S%z")
    except ValueError:
        return None


def write_final_report(root: Path) -> None:
    runs = load_runs(root)
    primary = load_primary_run(root)
    retry_by_example = collect_retry_results(runs)

    timings_path = root / "ALL_CASE_TIMINGS.tsv"
    failures_path = root / "FAILURES.tsv"
    write_timings_tsv(timings_path, primary)
    write_failures_tsv(failures_path, primary, retry_by_example)
    effective_failures = compute_effective_failures(primary, retry_by_example)
    missing_primary = primary is None
    (root / "effective_rc.txt").write_text("1\n" if missing_primary or effective_failures else "0\n", encoding="utf-8")
    markdown = render_markdown(
        MarkdownRenderContext(root, primary, retry_by_example, effective_failures, timings_path, failures_path)
    )
    (root / "FINAL_REPORT.md").write_text("\n".join(markdown) + "\n", encoding="utf-8")
    write_junit_xml(root / "junit.xml", primary)


def load_runs(root: Path) -> list[tuple[str, Path, dict[str, str], dict[str, Any]]]:
    runs: list[tuple[str, Path, dict[str, str], dict[str, Any]]] = []
    for report in sorted(root.glob("*/results/report.json")):
        run_dir = report.parents[1]
        data = read_json(report)
        if data:
            runs.append((run_dir.name, run_dir, read_meta(run_dir / "meta.txt"), data))
    return runs


def collect_retry_results(
    runs: list[tuple[str, Path, dict[str, str], dict[str, Any]]],
) -> dict[str, list[tuple[str, dict[str, str], dict[str, Any]]]]:
    retry_by_example: dict[str, list[tuple[str, dict[str, str], dict[str, Any]]]] = {}
    for name, _, meta, data in runs:
        if not name.startswith("retry_"):
            continue
        results = data.get("results", [])
        if results:
            retry_by_example.setdefault(results[0].get("example", ""), []).append((name, meta, results[0]))
    return retry_by_example


def write_timings_tsv(timings_path: Path, primary: tuple[str, Path, dict[str, str], dict[str, Any]] | None) -> None:
    with timings_path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.writer(handle, delimiter="\t")
        writer.writerow(
            [
                "run",
                "example",
                "arch",
                "mode",
                "status",
                "rc",
                "duration_s",
                "npu_wait_s",
                "reason",
                "failing_step",
                "steps",
                "commands",
                "log_file",
                "stage_log_files",
            ]
        )
        if primary:
            name, _, _, data = primary
            for result in data.get("results", []):
                steps = result.get("steps", [])
                npu_wait = sum(float(step.get("wait_s") or 0) for step in steps)
                step_text = "; ".join(format_step(step) for step in steps)
                commands = " && ".join(str(step.get("command", "")).replace("\n", " ") for step in steps)
                writer.writerow(
                    [
                        name,
                        result.get("example", ""),
                        result.get("arch", ""),
                        result.get("mode", ""),
                        result.get("status", ""),
                        result.get("rc", ""),
                        f"{float(result.get('duration_s') or 0):.3f}",
                        f"{npu_wait:.3f}",
                        result.get("reason", ""),
                        result.get("failing_step", ""),
                        step_text,
                        commands,
                        result.get("log_file", ""),
                        ";".join(result.get("stage_log_files", [])),
                    ]
                )


def write_failures_tsv(
    failures_path: Path,
    primary: tuple[str, Path, dict[str, str], dict[str, Any]] | None,
    retry_by_example: dict[str, list[tuple[str, dict[str, str], dict[str, Any]]]],
) -> None:
    with failures_path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.writer(handle, delimiter="\t")
        writer.writerow(
            [
                "example",
                "first_status",
                "first_reason",
                "first_rc",
                "first_duration_s",
                "first_failing_step",
                "retry_results",
            ]
        )
        for result in primary_failures(primary):
            writer.writerow(failure_row(result, retry_by_example))


def primary_failures(primary: tuple[str, Path, dict[str, str], dict[str, Any]] | None) -> list[dict[str, Any]]:
    if not primary:
        return []
    _, _, _, data = primary
    return [result for result in data.get("results", []) if result.get("status") == "FAIL"]


def failure_row(
    result: dict[str, Any], retry_by_example: dict[str, list[tuple[str, dict[str, str], dict[str, Any]]]]
) -> list[str]:
    retries = retry_texts(retry_by_example.get(result.get("example", ""), []))
    return [
        result.get("example", ""),
        result.get("status", ""),
        result.get("reason", ""),
        result.get("rc", ""),
        f"{float(result.get('duration_s') or 0):.3f}",
        result.get("failing_step", ""),
        " | ".join(retries),
    ]


def retry_texts(retries: list[tuple[str, dict[str, str], dict[str, Any]]]) -> list[str]:
    return [
        f"{retry_name}:card={meta.get('card', '')}:status={retry_result.get('status', '')}:"
        f"reason={retry_result.get('reason', '')}:rc={retry_result.get('rc', '')}:"
        f"duration={float(retry_result.get('duration_s') or 0):.1f}"
        for retry_name, meta, retry_result in retries
    ]


def compute_effective_failures(
    primary: tuple[str, Path, dict[str, str], dict[str, Any]] | None,
    retry_by_example: dict[str, list[tuple[str, dict[str, str], dict[str, Any]]]],
) -> list[dict[str, Any]]:
    effective_failures: list[dict[str, Any]] = []
    if not primary:
        return effective_failures
    _, _, _, data = primary
    for result in data.get("results", []):
        if result.get("status") != "FAIL":
            continue
        retries = retry_by_example.get(result.get("example", ""), [])
        retry_passed = any(retry_result.get("status") == "PASS" for _, _, retry_result in retries)
        if result.get("reason") == "timeout" and retry_passed:
            continue
        effective_failures.append(result)
    return effective_failures


def write_junit_xml(path: Path, primary: tuple[str, Path, dict[str, str], dict[str, Any]] | None) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    suites = ET.Element("testsuites")
    if primary:
        _, _, meta, data = primary
        results = data.get("results", [])
        grouped: dict[tuple[str, str], list[dict[str, Any]]] = {}
        for result in results:
            arch = str(result.get("arch", "") or meta.get("arch", ""))
            mode = str(result.get("mode", ""))
            grouped.setdefault((arch, mode), []).append(result)
        for (arch, mode), group in sorted(grouped.items()):
            append_junit_suite(suites, arch, mode, group)

    rough = ET.tostring(suites, encoding="utf-8")
    pretty = minidom.parseString(rough).toprettyxml(indent="  ", encoding="utf-8")
    path.write_bytes(pretty)


def append_junit_suite(parent: ET.Element, arch: str, mode: str, results: list[dict[str, Any]]) -> None:
    failures = sum(1 for result in results if result.get("status") == "FAIL")
    skipped = sum(1 for result in results if result.get("status") == "SKIP")
    suite = ET.SubElement(
        parent,
        "testsuite",
        {
            "name": f"presmoke.{arch}.{mode}",
            "tests": str(len(results)),
            "failures": str(failures),
            "errors": "0",
            "skipped": str(skipped),
            "time": f"{sum(float(result.get('duration_s') or 0) for result in results):.3f}",
        },
    )
    for result in sorted(results, key=lambda item: item.get("example", "")):
        append_junit_case(suite, result)


def append_junit_case(suite: ET.Element, result: dict[str, Any]) -> None:
    example = str(result.get("example", ""))
    case = ET.SubElement(
        suite,
        "testcase",
        {
            "classname": junit_classname(example),
            "name": f"{junit_case_name(example)} [{result.get('mode', '')}]",
            "time": f"{float(result.get('duration_s') or 0):.3f}",
        },
    )
    status = result.get("status", "")
    reason = str(result.get("reason", "") or "")
    rc = result.get("rc", "")
    failing_step = str(result.get("failing_step", "") or "")
    if status == "FAIL":
        failure = ET.SubElement(
            case, "failure", {"message": sanitize_xml_text(f"{reason} rc={rc}").strip(), "type": "PresmokeFailure"}
        )
        failure.text = sanitize_xml_text(f"reason={reason}\nrc={rc}\nfailing_step={failing_step}\n")
    elif status == "SKIP":
        skipped = ET.SubElement(case, "skipped", {"message": sanitize_xml_text(reason)})
        skipped.text = sanitize_xml_text(reason)
    ET.SubElement(case, "system-out").text = build_junit_system_out(result)


def junit_classname(example: str) -> str:
    parts = [part for part in example.split("/") if part]
    if len(parts) <= 1:
        return "presmoke"
    return ".".join(parts[:-1])


def junit_case_name(example: str) -> str:
    parts = [part for part in example.split("/") if part]
    return parts[-1] if parts else "unknown"


def build_junit_system_out(result: dict[str, Any]) -> str:
    lines = [
        f"example={result.get('example', '')}",
        f"arch={result.get('arch', '')}",
        f"mode={result.get('mode', '')}",
        f"status={result.get('status', '')}",
        f"reason={result.get('reason', '')}",
        f"rc={result.get('rc', '')}",
        f"duration_s={float(result.get('duration_s') or 0):.3f}",
        "",
        "steps:",
    ]
    for step in result.get("steps", []):
        command = str(step.get("command", "")).replace("\n", " ")
        lines.append(
            "  "
            f"{step.get('kind', '')}: "
            f"duration_s={float(step.get('duration_s') or 0):.3f} "
            f"wait_s={float(step.get('wait_s') or 0):.3f} "
            f"npu_slot={int(bool(step.get('npu_slot')))} "
            f"rc={step.get('rc', '')} "
            f"command={command}"
        )
    lines.extend(["", "logs:"])
    append_log_tail(lines, "case_log", str(result.get("log_file", "") or ""))
    for stage_log in result.get("stage_log_files", []):
        append_log_tail(lines, "stage_log", str(stage_log))
    return sanitize_xml_text("\n".join(lines))


def append_log_tail(lines: list[str], label: str, raw_path: str) -> None:
    if not raw_path:
        lines.append(f"  {label}: (missing)")
        return
    path = Path(raw_path)
    lines.append(f"  {label}: {path}")
    if not path.exists():
        lines.append(f"  {path} (missing)")
        return
    try:
        data = path.read_bytes()
        tail = data[-_JUNIT_LOG_TAIL_BYTES:]
        if len(data) > len(tail):
            lines.append(f"  ... log truncated to last {_JUNIT_LOG_TAIL_BYTES} bytes ...")
        lines.append(tail.decode("utf-8", errors="replace"))
    except OSError as exc:
        lines.append(f"  unable to read log: {exc}")


def sanitize_xml_text(value: str) -> str:
    return _XML_INVALID_CHARS.sub("", value or "")


def format_step(step: dict[str, Any]) -> str:
    text = f"{step.get('kind')}:{float(step.get('duration_s') or 0):.3f}s"
    if step.get("npu_slot"):
        text += "/npu"
    if step.get("wait_s"):
        text += f"/wait={float(step.get('wait_s') or 0):.3f}s"
    if step.get("rc") not in (0, None):
        text += f"/rc={step.get('rc')}"
    return text


def render_markdown(context: MarkdownRenderContext) -> list[str]:
    lines = render_markdown_header(context.root)
    if not context.primary:
        lines.extend(
            [
                "",
                "Primary full run report was not found.",
                "",
                "## Effective Status",
                "",
                "`FAIL`: primary report missing.",
            ]
        )
        return lines

    name, run_dir, meta, data = context.primary
    lines.extend(render_primary_run(name, meta, data))
    append_failures(lines, data.get("results", []), context.retry_by_example)
    append_skips(lines, data.get("results", []))
    lines.extend(render_effective_status(context.effective_failures))
    lines.extend(render_artifacts(context.root, run_dir, context.timings_path, context.failures_path))
    return lines


def render_markdown_header(root: Path) -> list[str]:
    generated_at = datetime.datetime.now(datetime.timezone.utc).astimezone().isoformat(timespec="seconds")
    return ["# Presmoke Report", "", f"- root: `{root}`", f"- generated_at: `{generated_at}`"]


def render_primary_run(name: str, meta: dict[str, str], data: dict[str, Any]) -> list[str]:
    summary = data.get("summary", {})
    npu_stats = data.get("npu_stats") or {}
    parallel_config = data.get("parallel_config") or {}
    results = data.get("results", [])
    lines = [
        "",
        "## Primary Full Run",
        "",
        f"- run: `{name}`",
        f"- card: `{meta.get('card', '')}`",
        f"- arch: `{meta.get('arch', '')}`",
        f"- schedule: `{meta.get('schedule', '')}`",
        f"- jobs/npu_slots: `{meta.get('jobs', '')}/{meta.get('npu_slots', '')}`",
        f"- cpu_run_slots/make_jobs: `{meta.get('cpu_run_slots', '')}/{meta.get('make_jobs', '')}`",
        f"- resolved jobs/npu_slots: `{parallel_config.get('jobs', '')}/{parallel_config.get('npu_slots', '')}`",
        "- resolved cpu_run_slots/make_jobs: "
        f"`{parallel_config.get('cpu_run_slots', '')}/{parallel_config.get('make_jobs', '')}`",
        f"- resolved cpu_run_timeout: `{parallel_config.get('cpu_run_timeout', '')}`",
        f"- timeout: `{meta.get('timeout', '')}`",
        f"- cpu_run_timeout: `{meta.get('cpu_run_timeout', '')}`",
        f"- elapsed_sec: `{meta.get('elapsed_sec', '')}`",
        f"- planned_results: `{len(results)}`",
        f"- pass/fail/skip: `{summary.get('PASS', 0)}/{summary.get('FAIL', 0)}/{summary.get('SKIP', 0)}`",
    ]
    if npu_stats:
        metric_prefix = "run_queue" if npu_stats.get("queue_model") == "pipeline-cpu" else "npu"
        lines.extend(
            [
                f"- {metric_prefix}_busy_s: `{float(npu_stats.get('busy_s') or 0):.3f}`",
                f"- {metric_prefix}_idle_s: `{float(npu_stats.get('idle_s') or 0):.3f}`",
                f"- {metric_prefix}_utilization: `{float(npu_stats.get('utilization') or 0):.3f}`",
            ]
        )
    return lines


def render_effective_status(effective_failures: list[dict[str, Any]]) -> list[str]:
    if effective_failures:
        return ["", "## Effective Status", "", f"`FAIL`: `{len(effective_failures)}` unrecovered failed cases."]
    return ["", "## Effective Status", "", "`PASS`: no unrecovered failed cases."]


def render_artifacts(root: Path, run_dir: Path, timings_path: Path, failures_path: Path) -> list[str]:
    return [
        "",
        "## Artifacts",
        "",
        f"- all case timings: `{timings_path}`",
        f"- failures tsv: `{failures_path}`",
        f"- junit xml: `{root / 'junit.xml'}`",
        f"- primary JSON: `{run_dir / 'results' / 'report.json'}`",
        f"- primary Markdown: `{run_dir / 'results' / 'report.md'}`",
        f"- per-case logs: `{run_dir / 'results' / 'logs'}`",
        f"- per-stage logs: `{run_dir / 'results' / 'logs' / 'stages'}`",
    ]


def append_failures(
    lines: list[str],
    results: list[dict[str, Any]],
    retry_by_example: dict[str, list[tuple[str, dict[str, str], dict[str, Any]]]],
) -> None:
    failures = [result for result in results if result.get("status") == "FAIL"]
    lines.extend(["", "## Failures", ""])
    if not failures:
        lines.append("No failures.")
        return
    lines.append("| example | status | reason | rc | duration_s | failing_step | retry |")
    lines.append("|---|---:|---|---:|---:|---|---|")
    for result in failures:
        retry_text = [
            f"{retry_name} card={retry_meta.get('card', '')} {retry_result.get('status', '')}"
            f" {retry_result.get('reason', '')} rc={retry_result.get('rc', '')}"
            for retry_name, retry_meta, retry_result in retry_by_example.get(result.get("example", ""), [])
        ]
        failing_step = str(result.get("failing_step", "")).replace("|", "\\|")
        lines.append(
            f"| {result.get('example', '')} | {result.get('status', '')} | {result.get('reason', '')} | "
            f"{result.get('rc', '')} | {float(result.get('duration_s') or 0):.1f} | "
            f"{failing_step} | {'<br>'.join(retry_text)} |"
        )


def append_skips(lines: list[str], results: list[dict[str, Any]]) -> None:
    skips = [result for result in results if result.get("status") == "SKIP"]
    lines.extend(["", "## Skips", ""])
    dry_run_skips = [result for result in skips if result.get("reason") == "dry-run"]
    real_skips = [result for result in skips if result.get("reason") != "dry-run"]
    if dry_run_skips:
        lines.append(f"- dry-run planned cases: `{len(dry_run_skips)}`; details are in `ALL_CASE_TIMINGS.tsv`.")
    if real_skips:
        lines.append("")
        lines.append("| example | reason |")
        lines.append("|---|---|")
        for result in real_skips:
            lines.append(f"| {result.get('example', '')} | {result.get('reason', '')} |")
    elif not dry_run_skips:
        lines.append("No skips.")


def main() -> int:
    logging.basicConfig(level=logging.INFO, format="%(message)s", stream=sys.stdout)
    parser = argparse.ArgumentParser(description="Presmoke orchestration report helper")
    subparsers = parser.add_subparsers(dest="command", required=True)
    timeout_parser = subparsers.add_parser("timeout-examples")
    timeout_parser.add_argument("report", type=Path)
    list_parser = subparsers.add_parser("list-examples")
    list_parser.add_argument("report", type=Path)
    shard_parser = subparsers.add_parser("shard-examples")
    shard_parser.add_argument("report", type=Path)
    shard_parser.add_argument("--cards", nargs="+", required=True)
    shard_parser.add_argument("--schedule-report", type=Path)
    shard_parser.add_argument("--exclude", action="append", default=[])
    shard_parser.add_argument("--jobs", type=int)
    shard_parser.add_argument("--fixed-shards", type=Path)
    status_parser = subparsers.add_parser("retry-status")
    status_parser.add_argument("report", type=Path)
    report_parser = subparsers.add_parser("write-final-report")
    report_parser.add_argument("root", type=Path)
    summary_parser = subparsers.add_parser("print-summary")
    summary_parser.add_argument("root", type=Path)
    args = parser.parse_args()

    if args.command == "timeout-examples":
        for example in timeout_examples(args.report):
            LOG.info("%s", example)
    elif args.command == "list-examples":
        for example in list_examples(args.report):
            LOG.info("%s", example)
    elif args.command == "shard-examples":
        for card, example in shard_examples(
            args.report,
            args.cards,
            args.schedule_report,
            set(args.exclude),
            jobs=args.jobs,
            fixed_shards=args.fixed_shards,
        ):
            LOG.info("%s\t%s", card, example)
    elif args.command == "retry-status":
        LOG.info("%s", retry_status(args.report))
    elif args.command == "write-final-report":
        write_final_report(args.root)
    elif args.command == "print-summary":
        print_summary(args.root)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
