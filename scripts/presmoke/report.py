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
import logging
from dataclasses import asdict
from pathlib import Path
from typing import Iterable, List

from .model import RunReport, RunResult, Suggestion


LOG = logging.getLogger(__name__)


def write_json(report: RunReport, path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    payload = asdict(report)
    payload["summary"] = summarize(report.results)
    if report.npu_stats:
        payload["npu_stats"]["utilization"] = report.npu_stats.utilization
    path.write_text(json.dumps(payload, ensure_ascii=False, indent=2), encoding="utf-8")


def write_markdown(report: RunReport, path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    lines = markdown_header(report)
    lines.extend(markdown_result_table(report.results))
    if report.suggestions:
        lines.extend(markdown_suggestions(report.suggestions))
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def markdown_header(report: RunReport) -> List[str]:
    summary = summarize(report.results)
    lines = [
        "# Presmoke Report",
        "",
        f"- host_arch: `{report.host_arch}`",
        f"- modes: `{','.join(report.modes)}`",
        f"- started_at: `{report.started_at}`",
        f"- finished_at: `{report.finished_at}`",
        f"- pass/fail/skip: `{summary['PASS']}/{summary['FAIL']}/{summary['SKIP']}`",
    ]
    if report.parallel_config:
        lines.extend(
            [
                f"- resolved_jobs: `{report.parallel_config.get('jobs', '')}`",
                f"- resolved_npu_slots: `{report.parallel_config.get('npu_slots', '')}`",
                f"- resolved_cpu_run_slots: `{report.parallel_config.get('cpu_run_slots', '')}`",
                f"- resolved_make_jobs: `{report.parallel_config.get('make_jobs', '')}`",
            ]
        )
    if report.npu_stats:
        metric_prefix = "cpu_run_queue" if report.npu_stats.queue_model == "pipeline-cpu" else "npu"
        lines.extend(
            [
                f"- {metric_prefix}_slots: `{report.npu_stats.slots}`",
                f"- {metric_prefix}_model: `{report.npu_stats.queue_model}`",
                f"- {metric_prefix}_busy_s: `{report.npu_stats.busy_s:.3f}`",
                f"- {metric_prefix}_idle_s: `{report.npu_stats.idle_s:.3f}`",
                f"- {metric_prefix}_utilization: `{report.npu_stats.utilization:.3f}`",
                f"- {metric_prefix}_max_slot_idle_s: `{report.npu_stats.max_slot_idle_s:.3f}`",
            ]
        )
    return lines


def markdown_result_table(results: Iterable[RunResult]) -> List[str]:
    lines = [
        "",
        "| example | arch | mode | status | rc | duration_s | npu_wait_s | reason | steps | log | stage_logs |",
        "|---|---|---|---:|---:|---:|---:|---|---|---|---|",
    ]
    for result in sorted(results, key=lambda r: (r.example, r.mode)):
        npu_wait = sum(step.wait_s for step in result.steps)
        steps = "<br>".join(
            f"{_md_escape(step.kind)}:{step.duration_s:.1f}s"
            f"{'/npu' if step.npu_slot else ''}"
            f"{'/wait=' + format(step.wait_s, '.1f') + 's' if step.wait_s else ''}"
            f"{'/rc=' + str(step.rc) if step.rc not in (0, None) else ''}"
            for step in result.steps
        )
        lines.append(
            f"| {result.example} | {result.arch} | {result.mode} | {result.status} | "
            f"{'' if result.rc is None else result.rc} | {result.duration_s:.1f} | {npu_wait:.1f} | "
            f"{result.reason} | {steps} | {result.log_file} | {'<br>'.join(result.stage_log_files)} |"
        )
    return lines


def markdown_suggestions(suggestions: Iterable[Suggestion]) -> List[str]:
    lines = ["", "## Suggestions", "", "| example | severity | category | message | hint |", "|---|---|---|---|---|"]
    for item in suggestions:
        lines.append(f"| {item.example} | {item.severity} | {item.category} | {item.message} | {item.hint} |")
    return lines


def write_suggestions(suggestions: Iterable[Suggestion], path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    lines = [
        "# Presmoke Suggestions",
        "",
        "| example | severity | category | message | hint |",
        "|---|---|---|---|---|",
    ]
    for item in suggestions:
        lines.append(f"| {item.example} | {item.severity} | {item.category} | {item.message} | {item.hint} |")
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def print_console(report: RunReport) -> None:
    summary = summarize(report.results)
    LOG.info("Presmoke v2: arch=%s modes=%s", report.host_arch, ",".join(report.modes))
    LOG.info("PASS=%s FAIL=%s SKIP=%s", summary["PASS"], summary["FAIL"], summary["SKIP"])
    if report.parallel_config:
        LOG.info(
            "PARALLEL jobs=%s npu_slots=%s cpu_run_slots=%s make_jobs=%s",
            report.parallel_config.get("jobs", ""),
            report.parallel_config.get("npu_slots", ""),
            report.parallel_config.get("cpu_run_slots", ""),
            report.parallel_config.get("make_jobs", ""),
        )
    if report.npu_stats:
        metric_label = "CPU_RUN_QUEUE" if report.npu_stats.queue_model == "pipeline-cpu" else "NPU"
        LOG.info(
            "%s slots=%s model=%s busy_s=%.3f idle_s=%.3f utilization=%.3f",
            metric_label,
            report.npu_stats.slots,
            report.npu_stats.queue_model,
            report.npu_stats.busy_s,
            report.npu_stats.idle_s,
            report.npu_stats.utilization,
        )
    failures = [r for r in report.results if r.status == "FAIL"]
    if failures:
        LOG.info("Failed cells:")
        for result in sorted(failures, key=lambda r: (r.example, r.mode)):
            LOG.info(
                "  %s [%s] rc=%s reason=%s step=%s",
                result.example,
                result.mode,
                result.rc,
                result.reason,
                result.failing_step,
            )
    if report.suggestions:
        LOG.info("Suggestions: %s written to suggestions.md", len(report.suggestions))


def summarize(results: Iterable[RunResult]) -> dict[str, int]:
    summary = {"PASS": 0, "FAIL": 0, "SKIP": 0}
    for result in results:
        summary[result.status] = summary.get(result.status, 0) + 1
    return summary


def _md_escape(value: str) -> str:
    return value.replace("|", "\\|")
