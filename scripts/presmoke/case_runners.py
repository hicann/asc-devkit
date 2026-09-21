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
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, List

from .discover import filter_examples
from .model import Cell, Command, ExampleSpec, RunResult, Suggestion


@dataclass
class CaseRunnerOptions:
    arch: str
    modes: Iterable[str]
    includes: Iterable[str]
    excludes: Iterable[str]
    exact_includes: Iterable[str] | None = None
    werror: bool = False


@dataclass
class CaseRunnerContext:
    project_root: Path
    runners_root: Path
    arch: str
    modes: List[str]
    werror: bool
    cells: List[Cell]
    suggestions: List[Suggestion]
    skipped: List[RunResult]


@dataclass(frozen=True)
class RunnerInfo:
    rel_path: str
    source_case: str
    supported_archs: List[str]
    supported_modes: List[str]
    skip: bool
    skip_reason: str
    skip_modes: List[str]
    confidence: str
    reasons: List[str]


def build_case_runner_cells_with_skips(
    project_root: Path, options: CaseRunnerOptions
) -> tuple[List[Cell], List[Suggestion], List[RunResult]]:
    runners_root = project_root / "scripts" / "presmoke" / "cases"
    manifest = load_manifest(project_root)
    runner_dirs = sorted(path.parent for path in runners_root.rglob("run.sh"))
    runner_dirs = filter_examples(runner_dirs, options.includes, options.excludes, options.exact_includes)
    modes_list = list(options.modes)
    context = CaseRunnerContext(
        project_root=project_root,
        runners_root=runners_root,
        arch=options.arch,
        modes=modes_list,
        werror=options.werror,
        cells=[],
        suggestions=[],
        skipped=[],
    )
    for runner_dir in runner_dirs:
        add_runner_cells(context, runner_dir, manifest)
    return context.cells, context.suggestions, context.skipped


def add_runner_cells(context: CaseRunnerContext, runner_dir: Path, manifest: dict[str, dict]) -> None:
    info = load_runner_info(context, runner_dir, manifest)
    if append_skip_result(context, info):
        return
    append_confidence_suggestion(context, info)
    for mode in context.modes:
        if not mode_supported(context, info, mode):
            continue
        context.cells.append(make_runner_cell(context, runner_dir, info, mode))


def load_runner_info(context: CaseRunnerContext, runner_dir: Path, manifest: dict[str, dict]) -> RunnerInfo:
    rel_path = runner_dir.relative_to(context.runners_root).as_posix()
    info = manifest.get(rel_path, {})
    return RunnerInfo(
        rel_path=rel_path,
        source_case=info.get("source_case") or rel_path,
        supported_archs=list(info.get("supported_archs") or [context.arch]),
        supported_modes=list(info.get("supported_modes") or context.modes or ["npu"]),
        skip=bool(info.get("skip")),
        skip_reason=info.get("skip_reason") or "explicit skip",
        skip_modes=list(info.get("skip_modes", [])),
        confidence=info.get("confidence", ""),
        reasons=list(info.get("reasons", [])),
    )


def append_skip_result(context: CaseRunnerContext, info: RunnerInfo) -> bool:
    if not info.skip:
        return False
    for mode in context.modes:
        context.skipped.append(
            RunResult(info.rel_path, context.arch, mode, "SKIP", info.skip_reason, duration_s=0, source="case-runner")
        )
    context.suggestions.append(Suggestion(info.rel_path, "case-runner", "info", "Explicitly skipped", info.skip_reason))
    return True


def append_confidence_suggestion(context: CaseRunnerContext, info: RunnerInfo) -> None:
    if info.confidence and info.confidence != "high":
        severity = "warn" if info.confidence == "low" else "info"
        context.suggestions.append(
            Suggestion(
                info.rel_path,
                "case-runner",
                severity,
                f"Generated runner confidence is {info.confidence}",
                ", ".join(info.reasons),
            )
        )


def mode_supported(context: CaseRunnerContext, info: RunnerInfo, mode: str) -> bool:
    if mode in info.skip_modes:
        context.skipped.append(
            RunResult(
                info.rel_path,
                context.arch,
                mode,
                "SKIP",
                f"Mode {mode} in skip_modes",
                duration_s=0,
                source="case-runner",
            )
        )
        context.suggestions.append(
            Suggestion(
                info.rel_path,
                "case-runner",
                "info",
                "Excluded on skip_modes list",
                f"Skip modes: {', '.join(info.skip_modes)}",
            )
        )
        return False
    if context.arch not in info.supported_archs:
        context.suggestions.append(
            Suggestion(
                info.rel_path,
                "case-runner",
                "info",
                f"Excluded on unsupported arch {context.arch}",
                f"Supported archs: {', '.join(info.supported_archs)}",
            )
        )
        return False
    if mode not in info.supported_modes:
        context.suggestions.append(
            Suggestion(
                info.rel_path,
                "case-runner",
                "info",
                f"Excluded on unsupported mode {mode}",
                f"Supported modes: {', '.join(info.supported_modes)}",
            )
        )
        return False
    return True


def make_runner_cell(context: CaseRunnerContext, runner_dir: Path, info: RunnerInfo, mode: str) -> Cell:
    runner = runner_dir / "run.sh"
    command_env = {
        "PRESMOKE_PROJECT_ROOT": str(context.project_root),
        "PRESMOKE_ARCH": context.arch,
        "PRESMOKE_MODE": mode,
    }
    if getattr(context, "werror", False):
        command_env["PRESMOKE_WERROR"] = "1"
    commands = [
        Command(f"bash {runner} clean", "clean", command_env),
        Command(f"bash {runner} build", "build", command_env),
        Command(f"bash {runner} run", "run", command_env),
        Command(f"bash {runner} verify", "verify", command_env),
    ]
    spec = ExampleSpec(
        path=context.project_root / "examples" / info.source_case,
        rel_path=info.rel_path,
        commands=commands,
        archs=info.supported_archs,
        modes=info.supported_modes,
        source="case-runner",
        suggestions=[],
    )
    return Cell(spec, context.arch, mode, commands, spec.path / f"build_{mode}")


def load_manifest(project_root: Path) -> dict[str, dict]:
    manifest = project_root / "scripts" / "presmoke" / "reports" / "case_runner_manifest.json"
    if not manifest.exists():
        return {}
    data = json.loads(manifest.read_text(encoding="utf-8"))
    return {item["case"]: item for item in data}
