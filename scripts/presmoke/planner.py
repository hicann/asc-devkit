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

import shlex
from typing import Iterable, List, Sequence, Tuple

from .model import Cell, Command, ExampleSpec, RunResult, Suggestion
from .werror import cmake_args as werror_cmake_args
from .werror import command_env as werror_command_env


def build_cells(
    specs: Sequence[ExampleSpec], arch: str, modes: Iterable[str], werror: bool = False
) -> Tuple[List[Cell], List[Suggestion]]:
    cells, suggestions, _ = build_cells_with_skips(specs, arch, modes, werror=werror)
    return cells, suggestions


def build_cells_with_skips(
    specs: Sequence[ExampleSpec], arch: str, modes: Iterable[str], werror: bool = False
) -> Tuple[List[Cell], List[Suggestion], List[RunResult]]:
    mode_list = list(dict.fromkeys(modes))
    requested_modes = set(mode_list)
    cells: List[Cell] = []
    suggestions: List[Suggestion] = []
    skipped: List[RunResult] = []
    for spec in specs:
        if arch not in spec.archs:
            suggestions.append(
                Suggestion(
                    spec.rel_path,
                    "planner",
                    "info",
                    f"Excluded on unsupported arch {arch}",
                    f"Supported archs: {', '.join(spec.archs)}",
                )
            )
            continue
        for mode in mode_list or spec.modes:
            if requested_modes and mode not in requested_modes:
                continue
            if mode not in spec.modes:
                suggestions.append(
                    Suggestion(
                        spec.rel_path,
                        "planner",
                        "info",
                        f"Excluded on unsupported mode {mode}",
                        f"Supported modes: {', '.join(spec.modes)}",
                    )
                )
                continue
            rewritten = [rewrite_command(command, arch, mode, werror=werror) for command in spec.commands]
            build_dir = spec.path / f"build_{mode}"
            cells.append(Cell(spec, arch, mode, rewritten, build_dir))
    return cells, suggestions, skipped


def rewrite_command(command: Command, arch: str, mode: str, werror: bool = False) -> Command:
    raw = command.raw.strip()
    kind = command.kind
    if raw.startswith("mkdir ") or raw.startswith("cd "):
        return Command(raw="", kind="skip", env=dict(command.env))
    if kind == "cmake" and is_cmake_configure(raw):
        raw = rewrite_cmake(raw, arch, mode, werror=werror)
        return Command(raw=raw, kind=kind, env=werror_command_env(command.env, werror))
    return Command(raw=raw, kind=kind, env=dict(command.env))


def is_cmake_configure(command: str) -> bool:
    try:
        parts = shlex.split(command)
    except ValueError:
        parts = command.split()
    if not parts or parts[0] != "cmake":
        return False
    return not any(part in {"--build", "--install", "--open", "--workflow", "-E"} for part in parts[1:])


def rewrite_cmake(command: str, arch: str, mode: str, werror: bool = False) -> str:
    if not is_cmake_configure(command):
        return command
    try:
        parts = shlex.split(command)
    except ValueError:
        parts = command.split()
    out: List[str] = []
    for part in parts:
        if part.startswith("-DCMAKE_ASC_ARCHITECTURES="):
            continue
        if part.startswith("-DCMAKE_ASC_RUN_MODE="):
            continue
        if part.startswith("-DNPU_ARCH="):
            continue
        out.append(part)
    if not out:
        out = ["cmake"]
    if out[0] != "cmake":
        out.insert(0, "cmake")
    out.append(f"-DCMAKE_ASC_ARCHITECTURES={arch}")
    if mode != "npu":
        out.append(f"-DCMAKE_ASC_RUN_MODE={mode}")
    out.extend(arg for arg in werror_cmake_args(werror) if not _has_cmake_cache_arg(out, arg))
    return " ".join(_quote_cmake_part(x) for x in out)


def _has_cmake_cache_arg(parts: Sequence[str], arg: str) -> bool:
    key = arg.split("=", 1)[0]
    return any(part.startswith(f"{key}=") for part in parts)


def _quote_cmake_part(part: str) -> str:
    if "$" in part:
        return part
    return shlex.quote(part)
