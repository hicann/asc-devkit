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

import re
import shlex
from pathlib import Path
from typing import Dict, List, Sequence, Tuple

from .model import Command, ExampleSpec, Suggestion


ARCH_RE = re.compile(r"dav-\d+")
MODE_RE = re.compile(r"\b(npu|cpu|sim)\b", re.IGNORECASE)


def parse_readme(example_dir: Path, examples_root: Path) -> ExampleSpec:
    rel_path = example_dir.relative_to(examples_root).as_posix()
    suggestions: List[Suggestion] = []
    text = read_readme_text(example_dir, rel_path, suggestions)
    commands, command_block, source = readme_commands(example_dir, rel_path, text, suggestions)
    archs = infer_archs(text, command_block, example_dir, rel_path, suggestions)
    modes = parse_modes(text, command_block) or ["npu"]
    _static_suggestions(example_dir, rel_path, commands, suggestions)
    return ExampleSpec(example_dir, rel_path, commands, sorted(set(archs)), sorted(set(modes)), source, suggestions)


def read_readme_text(example_dir: Path, rel_path: str, suggestions: List[Suggestion]) -> str:
    readme = example_dir / "README.md"
    if readme.exists():
        return readme.read_text(encoding="utf-8", errors="ignore")
    suggestions.append(
        _suggest(rel_path, "structure", "warn", "README.md is missing", "Add README.md or exclude this directory.")
    )
    return ""


def readme_commands(
    example_dir: Path, rel_path: str, text: str, suggestions: List[Suggestion]
) -> tuple[List[Command], str, str]:
    run_section = _section(text, "编译运行") or _section(text, "运行方式")
    if not run_section:
        suggestions.append(
            _suggest(
                rel_path, "structure", "warn", "README has no 编译运行 section", "Add a runnable 编译运行 section."
            )
        )
    command_block, source = _pick_command_block(run_section)
    if command_block:
        return split_commands(command_block), command_block, source
    suggestions.append(
        _suggest(
            rel_path,
            "structure",
            "warn",
            "No runnable bash block found; using convention commands",
            "Add a 样例执行 bash block.",
        )
    )
    return convention_commands(example_dir), "", "convention"


def infer_archs(
    text: str, command_block: str, example_dir: Path, rel_path: str, suggestions: List[Suggestion]
) -> List[str]:
    archs = parse_archs(text, command_block, example_dir)
    if archs:
        return archs
    suggestions.append(
        _suggest(
            rel_path,
            "structure",
            "warn",
            "Could not infer supported archs; defaulting to dav-2201",
            "Document CMAKE_ASC_ARCHITECTURES.",
        )
    )
    return ["dav-2201"]


def _suggest(example: str, category: str, severity: str, message: str, hint: str = "") -> Suggestion:
    return Suggestion(example=example, category=category, severity=severity, message=message, hint=hint)


def _section(text: str, title: str) -> str:
    lines = text.splitlines()
    start = None
    for idx, line in enumerate(lines):
        if re.match(rf"^##\s*{re.escape(title)}\s*$", line.strip()):
            start = idx + 1
            break
    if start is None:
        return ""
    end = len(lines)
    for idx in range(start, len(lines)):
        if re.match(r"^##\s+", lines[idx].strip()):
            end = idx
            break
    return "\n".join(lines[start:end])


def _section_by_heading(text: str, pattern: str) -> str:
    lines = text.splitlines()
    start = None
    for idx, line in enumerate(lines):
        heading = line.strip()
        if re.match(r"^##+\s+", heading) and re.search(pattern, heading):
            start = idx + 1
            break
    if start is None:
        return ""
    end = len(lines)
    for idx in range(start, len(lines)):
        if re.match(r"^##+\s+", lines[idx].strip()):
            end = idx
            break
    return "\n".join(lines[start:end])


def _bash_blocks(text: str) -> List[str]:
    blocks: List[str] = []
    in_block = False
    lang = ""
    buf: List[str] = []
    for line in text.splitlines():
        stripped = line.strip()
        if not in_block and stripped.startswith("```"):
            lang = stripped[3:].strip().lower()
            in_block = True
            buf = []
            continue
        if in_block and stripped.startswith("```"):
            if lang in ("bash", "sh", "shell", ""):
                blocks.append("\n".join(buf))
            in_block = False
            continue
        if in_block:
            buf.append(line)
    return blocks


def _pick_command_block(run_section: str) -> Tuple[str, str]:
    if not run_section:
        return "", "convention"
    sample_pos = run_section.find("样例执行")
    if sample_pos >= 0:
        block = _pick_preferred_block(_bash_blocks(run_section[sample_pos:]))
        if block:
            return block, "readme"
    block = _pick_preferred_block(_bash_blocks(run_section))
    if block:
        return block, "readme"
    return "", "convention"


def _pick_preferred_block(blocks: List[str]) -> str:
    runnable_blocks = [block for block in blocks if _is_runnable_block(block)]
    if not runnable_blocks:
        return ""
    for block in runnable_blocks:
        if "cmake" in block and "make" in block:
            return block
    return runnable_blocks[0]


def _is_runnable_block(block: str) -> bool:
    text = block.strip()
    if not text:
        return False
    if "set_env.sh" in text or text.startswith("pip3 install") or text.startswith("pip install"):
        return False
    return bool(re.search(r"\b(cmake|make|python3|./|msprof)\b", text))


def split_commands(block: str) -> List[Command]:
    env: Dict[str, str] = {}
    commands: List[Command] = []
    for original in _join_line_continuations(block):
        line = _strip_comment(original).strip()
        if not line:
            continue
        for part in _split_shell_line(line):
            part = part.strip()
            if not part:
                continue
            assignments, rest = _consume_assignments(part)
            env.update(assignments)
            if rest:
                commands.append(Command(raw=rest, kind=classify_command(rest), env=dict(env)))
    return commands


def _join_line_continuations(block: str) -> List[str]:
    lines: List[str] = []
    current = ""
    for line in block.splitlines():
        stripped = line.rstrip()
        if stripped.endswith("\\"):
            current += stripped[:-1].rstrip() + " "
            continue
        lines.append(current + line)
        current = ""
    if current:
        lines.append(current.rstrip())
    return lines


def _strip_comment(line: str) -> str:
    in_single = False
    in_double = False
    escaped = False
    for idx, ch in enumerate(line):
        if escaped:
            escaped = False
            continue
        if ch == "\\":
            escaped = True
            continue
        if ch == "'" and not in_double:
            in_single = not in_single
        elif ch == '"' and not in_single:
            in_double = not in_double
        elif ch == "#" and not in_single and not in_double:
            return line[:idx]
    return line


def _split_shell_line(line: str) -> List[str]:
    out: List[str] = []
    current: List[str] = []
    in_single = False
    in_double = False
    i = 0
    while i < len(line):
        ch = line[i]
        if ch == "'" and not in_double:
            in_single = not in_single
        elif ch == '"' and not in_single:
            in_double = not in_double
        if not in_single and not in_double and ch == ";":
            out.append("".join(current))
            current = []
            i += 1
            continue
        if not in_single and not in_double and line[i : i + 2] == "&&":
            out.append("".join(current))
            current = []
            i += 2
            continue
        current.append(ch)
        i += 1
    out.append("".join(current))
    return out


def _consume_assignments(command: str) -> Tuple[Dict[str, str], str]:
    assignments: Dict[str, str] = {}
    try:
        parts = shlex.split(command, posix=True)
    except ValueError:
        return assignments, command
    consumed = 0
    for part in parts:
        if re.match(r"^[A-Za-z_][A-Za-z0-9_]*=.*$", part):
            key, value = part.split("=", 1)
            assignments[key] = value
            consumed += 1
        else:
            break
    if consumed == 0:
        return assignments, command
    return assignments, " ".join(shlex.quote(x) for x in parts[consumed:])


def classify_command(command: str) -> str:
    stripped = command.strip()
    first = stripped.split()[0] if stripped.split() else ""
    if first == "cmake":
        return "cmake"
    if first == "make":
        return "make"
    if "gen_data.py" in stripped:
        return "gen_data"
    if "verify_result.py" in stripped:
        return "verify"
    if first == "msprof":
        return "run"
    if first.startswith("./") or re.match(r"python3\s+.*(_test|test_).*\.py", stripped):
        return "run"
    if first == "python3":
        return "run"
    if stripped.endswith(".run") or ".run" in stripped:
        return "package_run"
    return "shell"


def parse_archs(text: str, command_block: str, example_dir: Path) -> List[str]:
    product_archs = _archs_from_supported_products(text)
    if product_archs:
        return product_archs
    table_archs = _values_from_option_rows(text, "CMAKE_ASC_ARCHITECTURES", ARCH_RE)
    if table_archs:
        return table_archs
    if command_block:
        found = ARCH_RE.findall(command_block)
        if found:
            return found
    cmake = example_dir / "CMakeLists.txt"
    if cmake.exists():
        found = ARCH_RE.findall(cmake.read_text(encoding="utf-8", errors="ignore"))
        if found:
            return found
    return []


def _archs_from_supported_products(text: str) -> List[str]:
    support_section = _section_by_heading(text, r"支持.*产品")
    if not support_section:
        return []

    archs: List[str] = []
    if re.search(r"Ascend\s*950|950PR|950DT|dav-3510", support_section, re.IGNORECASE):
        archs.append("dav-3510")
    if re.search(
        r"Atlas\s*A[23]|Atlas\s*200I/500\s*A2|Atlas\s*200I|Atlas\s*500\s*A2|dav-2201", support_section, re.IGNORECASE
    ):
        archs.append("dav-2201")
    return archs


def parse_modes(text: str, command_block: str) -> List[str]:
    modes = _values_from_option_rows(text, "CMAKE_ASC_RUN_MODE", MODE_RE)
    if modes:
        return [m.lower() for m in modes]
    found = {"npu"}
    if command_block:
        found.update(m.lower() for m in MODE_RE.findall(command_block))
    return sorted(found)


def _values_from_option_rows(text: str, option_name: str, pattern: re.Pattern[str]) -> List[str]:
    values: List[str] = []
    for line in text.splitlines():
        cleaned = _clean_markdown_cell(line)
        if option_name in cleaned:
            values.extend(pattern.findall(cleaned))
    return values


def _clean_markdown_cell(text: str) -> str:
    return text.replace("`", "").replace("　", " ")


def convention_commands(example_dir: Path) -> List[Command]:
    commands: List[Command] = []
    if (example_dir / "CMakeLists.txt").exists():
        commands.extend([Command("cmake ..", "cmake"), Command("make -j", "make")])
    scripts = example_dir / "scripts"
    if (scripts / "gen_data.py").exists():
        commands.append(Command("python3 ../scripts/gen_data.py", "gen_data"))
    if any(example_dir.glob("*_test.py")):
        commands.extend(Command(f"python3 ../{p.name}", "run") for p in sorted(example_dir.glob("*_test.py"))[:1])
    elif (example_dir / "CMakeLists.txt").exists():
        commands.append(Command("./demo", "run"))
    if (scripts / "verify_result.py").exists():
        commands.append(Command("python3 ../scripts/verify_result.py output/output.bin output/golden.bin", "verify"))
    return commands


def _static_suggestions(
    example_dir: Path, rel_path: str, commands: Sequence[Command], suggestions: List[Suggestion]
) -> None:
    if not (example_dir / "README_en.md").exists():
        suggestions.append(
            _suggest(
                rel_path,
                "structure",
                "info",
                "README_en.md is missing",
                "Add README_en.md if this example should be bilingual.",
            )
        )
    for command in commands:
        if "gen_data.py" in command.raw and not (example_dir / "scripts" / "gen_data.py").exists():
            suggestions.append(
                _suggest(
                    rel_path,
                    "structure",
                    "warn",
                    "Command references gen_data.py but file is missing",
                    "Fix README command or add scripts/gen_data.py.",
                )
            )
        if "verify_result.py" in command.raw and not (example_dir / "scripts" / "verify_result.py").exists():
            suggestions.append(
                _suggest(
                    rel_path,
                    "structure",
                    "warn",
                    "Command references verify_result.py but file is missing",
                    "Fix README command or add scripts/verify_result.py.",
                )
            )
    if not any(c.kind == "verify" for c in commands):
        suggestions.append(
            _suggest(
                rel_path,
                "runtime",
                "info",
                "No verify step found; pass/fail depends on command rc only",
                "Add verify_result.py or an explicit validation command.",
            )
        )
