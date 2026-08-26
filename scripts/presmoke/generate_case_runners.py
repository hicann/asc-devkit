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
import json
import logging
import re
import shlex
from dataclasses import asdict, dataclass, field
from pathlib import Path
from typing import Iterable, List

from presmoke.discover import discover_examples
from presmoke.model import Cell, Command, ExampleSpec
from presmoke.planner import is_cmake_configure
from presmoke.planner import build_cells, rewrite_command
from presmoke.readme_spec import parse_readme


LOG = logging.getLogger(__name__)


SPECIAL_PREFIXES = (
    "01_simd_cpp_api/02_features/00_",
    "01_simd_cpp_api/02_features/01_",
    "01_simd_cpp_api/02_features/99_",
)

CUSTOM_OP_PACKAGE_CASE = (
    "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op"
)
PARALLEL_OPS_PACKAGE_CASE = (
    "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/parallel_ops_package"
)
TILING_SINK_PROGRAMMING_CASE = (
    "04_aicpu/02_features/00_framework/00_pytorch/tiling_sink_programming"
)
TILING_SINK_GENERATE_TASK_PATTERN_PREFIX = (
    "GenerateTaskForSinkOp:Node [AddCustomTilingSink, AddCustomTilingSink] "
    "starts to generate tasks"
)
TILING_SINK_GENERATE_TASK_PATTERN_SUFFIX = " for the tiling sink, sk_flag [0]."
TILING_SINK_GENERATE_TASK_PATTERN = (
    TILING_SINK_GENERATE_TASK_PATTERN_PREFIX + TILING_SINK_GENERATE_TASK_PATTERN_SUFFIX
)

CUSTOM_OP_PACKAGE_DEPENDENTS = {
    "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclnn_invocation",
    "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclop_invocation",
    "01_simd_cpp_api/02_features/00_framework/02_onnx/onnx_plugin",
    TILING_SINK_PROGRAMMING_CASE,
}

# These projects either build host-only apps, superbuild subprojects, or C API
# samples with their own explicit --npu-arch option. Passing
# CMAKE_ASC_ARCHITECTURES to their top-level CMake only produces an unused
# variable warning.
NO_CMAKE_ARCH_INJECTION_CASES = {
    "01_simd_cpp_api/02_features/00_framework/02_onnx/onnx_plugin",
    "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op_static_lib",
    "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/parallel_ops_package",
    "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclnn_invocation",
    "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclop_invocation",
    "02_simd_c_api/00_introduction/00_quickstart/hello_world_npu",
    "02_simd_c_api/00_introduction/01_add/c_api_delicacy_async_add",
    "02_simd_c_api/00_introduction/01_add/c_api_sync_add",
    "02_simd_c_api/02_features/03_c_api/00_vector_compute/00_sync_add",
    "02_simd_c_api/02_features/03_c_api/00_vector_compute/01_async_add",
    "02_simd_c_api/02_features/03_c_api/00_vector_compute/02_c_api_delicacy_async_add",
}

TENSORFLOW_SKIP_REASON = "requires TensorFlow 2.6.5 environment; skipped by presmoke"
MATMUL_L2CACHE_SKIP_REASON = (
    "data size (~308M) causes overtime in cpu mode; cpu mode skipped by presmoke"
)
LONG_TIME_REASON = "long execution time in cpu mode; cpu mode skipped by presmoke"
WORKAROUND_SKIP_REASON = "temporarily disabled by presmoke workaround"


@dataclass
class SkipConfig:
    reason: str = ""
    modes: List[str] = field(default_factory=list)
    archs: List[str] = field(default_factory=list)


@dataclass
class _SkipContext:
    all_modes_skipped: bool
    skip_reason: str
    custom_op_package_case: bool
    custom_op_dependency: bool


@dataclass
class _SkipResult:
    confidence: str
    reasons: List[str]
    build_cmds: List[Command]
    run_cmds: List[Command]
    verify_cmds: List[Command]


SKIP_CONFIG = {
    "01_simd_cpp_api/02_features/00_framework/01_tensorflow/tensorflow_builtin": SkipConfig(
        reason=TENSORFLOW_SKIP_REASON,
        modes=["npu"],
    ),
    "01_simd_cpp_api/02_features/00_framework/01_tensorflow/tensorflow_custom": SkipConfig(
        reason=TENSORFLOW_SKIP_REASON,
        modes=["npu"],
    ),
    "01_simd_cpp_api/04_advanced_api/00_matmul/matmul_l2cache": SkipConfig(
        reason=MATMUL_L2CACHE_SKIP_REASON,
        modes=["cpu"],
    ),
    "01_simd_cpp_api/05_best_practices/00_vector_compute/add_high_performance": SkipConfig(
        reason=LONG_TIME_REASON,
        modes=["cpu"],
    ),
    "01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_eltwise_high_performance": SkipConfig(
        reason=LONG_TIME_REASON,
        modes=["cpu"],
        archs=["dav-3510"],
    ),
    "01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_high_performance": SkipConfig(
        reason=LONG_TIME_REASON,
        modes=["cpu"],
        archs=["dav-3510"],
    ),
    "01_simd_cpp_api/05_best_practices/04_memory_access/bank_conflict_nd2nz": SkipConfig(
        reason=LONG_TIME_REASON,
        modes=["cpu"],
        archs=["dav-3510"],
    ),
    "01_simd_cpp_api/03_basic_api/05_sync_control/cross_core_set_wait_flag": SkipConfig(
        reason=WORKAROUND_SKIP_REASON,
        modes=["npu", "cpu", "sim"],
    ),
    "01_simd_cpp_api/03_basic_api/05_sync_control/data_sync_barrier": SkipConfig(
        reason=WORKAROUND_SKIP_REASON,
        modes=["npu", "cpu", "sim"],
    ),
    "01_simd_cpp_api/02_features/00_framework/03_ge/triple_chevron_notation": SkipConfig(
        reason=WORKAROUND_SKIP_REASON,
        modes=["npu", "cpu", "sim"],
    ),
}

ARCH_OVERRIDES: dict[str, List[str]] = {}

ARCH_ENV_KEYS = {
    "ARCH",
    "ASC_ARCH",
    "ASCEND_ARCH",
    "CMAKE_ASC_ARCHITECTURES",
    "NPU_ARCH",
}

BASH_LICENSE_HEADER = [
    "#!/bin/bash",
    "# ----------------------------------------------------------------------------------------------------------",
    "# Copyright (c) 2026 Huawei Technologies Co., Ltd.",
    "# This program is free software, you can redistribute it and/or modify it under the terms and conditions of",
    '# CANN Open Software License Agreement Version 2.0 (the "License").',
    "# Please refer to the License for details. You may not use this file except in compliance with the License.",
    '# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,',
    "# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.",
    "# See LICENSE in the root of the software repository for the full text of the License.",
    "# ----------------------------------------------------------------------------------------------------------",
]


@dataclass
class CaseReport:
    case: str
    runner: str
    source: str
    confidence: str
    source_case: str = ""
    supported_archs: List[str] = field(default_factory=list)
    supported_modes: List[str] = field(default_factory=list)
    target_runnable: bool = False
    skip: bool = False
    skip_reason: str = ""
    skip_modes: List[str] = field(default_factory=list)
    skip_archs: List[str] = field(default_factory=list)
    reasons: List[str] = field(default_factory=list)
    commands: List[str] = field(default_factory=list)


@dataclass
class RunnerRenderSpec:
    rel: str
    build_cmds: List[Command]
    run_cmds: List[Command]
    verify_cmds: List[Command]
    source_rel: str = ""
    scenario_num: int | None = None
    custom_op_dependency: bool = False
    custom_op_package_case: bool = False
    skip_reason: str = ""
    skip_modes: List[str] = field(default_factory=list)
    skip_archs: List[str] = field(default_factory=list)


def main() -> int:
    logging.basicConfig(level=logging.INFO, format="%(message)s")
    args = parse_args()
    project_root = Path(args.project_root).resolve()
    examples_root = project_root / "examples"
    runners_root = project_root / "scripts" / "presmoke" / "cases"
    reports_root = project_root / "scripts" / "presmoke" / "reports"
    runners_root.mkdir(parents=True, exist_ok=True)
    reports_root.mkdir(parents=True, exist_ok=True)

    specs = [
        parse_readme(path, examples_root) for path in discover_examples(examples_root)
    ]
    apply_case_overrides(specs)
    cells, _ = build_cells(specs, args.arch, [args.mode])
    cells = expand_scenario_cells(cells, project_root)
    cells = [
        cell
        for cell in cells
        if args.arch in cell.example.archs and args.mode in cell.example.modes
    ]
    runnable_by_rel = {cell.example.rel_path: cell for cell in cells}
    all_cells = [cell_for_spec(spec, args.arch, args.mode) for spec in specs]
    all_cells = expand_scenario_cells(all_cells, project_root)
    reports: List[CaseReport] = []
    for cell in all_cells:
        report = write_runner(
            project_root,
            runners_root,
            cell,
            runnable_on_target=cell.example.rel_path in runnable_by_rel,
        )
        reports.append(report)

    remove_stale_runners(runners_root, reports)
    write_manifest(reports_root, reports)
    LOG.info("generated_runners=%s", len(reports))
    LOG.info(
        "low_confidence=%s", sum(1 for item in reports if item.confidence != "high")
    )
    LOG.info("reports_dir=%s", reports_root)
    return 0


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Generate per-case presmoke run.sh wrappers"
    )
    parser.add_argument("--project-root", default=Path(__file__).resolve().parents[2])
    parser.add_argument("--arch", default="dav-2201")
    parser.add_argument("--mode", default="npu")
    return parser.parse_args()


def cell_for_spec(spec, arch: str, mode: str) -> Cell:
    commands = [rewrite_command(command, arch, mode) for command in spec.commands]
    build_dir = spec.path / f"build_{mode}"
    return Cell(spec, arch, mode, commands, build_dir)


def apply_case_overrides(specs) -> None:
    for spec in specs:
        if spec.rel_path in ARCH_OVERRIDES:
            spec.archs = ARCH_OVERRIDES[spec.rel_path]


def write_runner(
    project_root: Path, runners_root: Path, cell, runnable_on_target: bool
) -> CaseReport:
    rel = cell.example.rel_path
    runner_dir = runners_root / rel
    runner_dir.mkdir(parents=True, exist_ok=True)
    runner = runner_dir / "run.sh"
    confidence, reasons = runner_confidence(cell, runnable_on_target)
    render_spec, confidence = build_runner_render_spec(cell, confidence, reasons)
    text = render_runner(render_spec)
    runner.write_text(text, encoding="utf-8")
    runner.chmod(0o755)
    skip_reason = render_spec.skip_reason
    skip_modes = render_spec.skip_modes
    skip_archs = render_spec.skip_archs
    all_modes_skipped = bool(
        skip_reason and all(mode in skip_modes for mode in cell.example.modes)
    )
    return CaseReport(
        case=rel,
        runner=runner.relative_to(project_root).as_posix(),
        source_case=source_rel_for_cell(project_root, cell),
        source=cell.example.source,
        confidence=confidence,
        supported_archs=cell.example.archs,
        supported_modes=cell.example.modes,
        target_runnable=runnable_on_target,
        skip=all_modes_skipped,
        skip_reason=skip_reason,
        skip_modes=skip_modes,
        skip_archs=skip_archs,
        reasons=reasons,
        commands=[command.raw for command in cell.commands if command.raw],
    )


def expand_scenario_cells(cells: Iterable[Cell], project_root: Path) -> List[Cell]:
    expanded: List[Cell] = []
    for cell in cells:
        cmake_file = cell.example.path / "CMakeLists.txt"
        values = parse_scenario_values_from_cmake(cmake_file)
        if not values:
            values = parse_scenario_values_from_readme(cell.example.path / "README.md")
        if len(values) <= 1:
            expanded.append(cell)
            continue
        default_value = default_scenario_value(cell, cmake_file)
        if default_value is None or default_value not in values:
            expanded.append(cell)
        for value in values:
            expanded.append(make_scenario_cell(cell, project_root, value))
    return expanded


def parse_scenario_values_from_cmake(cmake_file: Path) -> List[int]:
    if not cmake_file.exists():
        return []
    text = cmake_file.read_text(encoding="utf-8", errors="ignore")
    if "SCENARIO_NUM" not in text and "SCENARIO" not in text:
        return []

    values: set[int] = set()
    for match in re.finditer(
        r"set\s*\(\s*SCENARIO_NUM\s+[^)]*CACHE\s+STRING\s+\"([^\"]*)\"", text
    ):
        values.update(parse_scenario_values_from_text(match.group(1)))
    for match in re.finditer(
        r"set\s*\(\s*SCENARIO\s+[^)]*CACHE\s+STRING\s+\"([^\"]*)\"", text
    ):
        values.update(parse_scenario_values_from_text(match.group(1)))
    for match in re.finditer(r"SCENARIO_NUM[^\n\r\"]*\"([^\"]*)\"", text):
        values.update(parse_scenario_values_from_text(match.group(1)))
    for match in re.finditer(r"set\s*\(\s*VALID_SCENARIOS\s+([^)]+)\)", text):
        values.update(parse_scenario_values_from_text(match.group(1)))
    for match in re.finditer(
        r"SCENARIO_NUM[^\n\r]*(?:must be|Valid values are|specify)[^\n\r]*", text
    ):
        values.update(parse_scenario_values_from_text(match.group(0)))
    return sorted(value for value in values if 0 <= value <= 32)


def parse_scenario_values_from_readme(readme_file: Path) -> List[int]:
    if not readme_file.exists():
        return []
    text = readme_file.read_text(encoding="utf-8", errors="ignore")
    values: set[int] = set()
    values.update(parse_scenario_values_from_html_tables(text))
    for line in text.splitlines():
        if "SCENARIO" not in line:
            continue
        table_match = re.search(r"\|\s*`?SCENARIO`?\s*\|\s*([^|]+)\|", line)
        if table_match:
            values.update(parse_scenario_values_from_text(table_match.group(1)))
            continue
        if re.search(r"(?:取值|Scenario|场景|default|默认)", line):
            values.update(parse_scenario_values_from_text(line))
    return sorted(value for value in values if 0 <= value <= 32)


def parse_scenario_values_from_html_tables(text: str) -> List[int]:
    values: set[int] = set()
    for table in re.findall(r"<table\b.*?</table>", text, flags=re.S | re.I):
        if "SCENARIO" not in table:
            continue
        for row in re.findall(r"<tr\b.*?</tr>", table, flags=re.S | re.I):
            cells = re.findall(r"<td\b[^>]*>(.*?)</td>", row, flags=re.S | re.I)
            if not cells:
                continue
            first_cell = re.sub(r"<.*?>", "", cells[0]).strip()
            if re.fullmatch(r"\d+", first_cell):
                values.add(int(first_cell))
    return sorted(value for value in values if 0 <= value <= 32)


def parse_scenario_values_from_text(text: str) -> List[int]:
    values: set[int] = set()
    text = collect_regex_character_class_scenarios(text, values)
    for start, end in re.findall(r"(?<!\d)(\d+)\s*-\s*(\d+)(?!\d)", text):
        first = int(start)
        last = int(end)
        if 0 <= first <= last <= 32:
            values.update(range(first, last + 1))
    for number in re.findall(r"(?<!\d)(\d+)(?!\d)", text):
        value = int(number)
        if 0 <= value <= 32:
            values.add(value)
    return sorted(values)


def collect_regex_character_class_scenarios(text: str, values: set[int]) -> str:
    def replace(match: re.Match) -> str:
        for char in match.group(1):
            values.add(int(char))
        return " "

    return re.sub(r"\^\[([0-9]+)\]\$", replace, text)


def default_scenario_value(cell: Cell, cmake_file: Path) -> int | None:
    for command in cell.commands:
        value = scenario_value_from_command(command)
        if value is not None:
            return value
    if not cmake_file.exists():
        return None
    text = cmake_file.read_text(encoding="utf-8", errors="ignore")
    match = re.search(r"set\s*\(\s*SCENARIO_NUM\s+\"?(\d+)\"?", text)
    return int(match.group(1)) if match else None


def scenario_value_from_command(command: Command) -> int | None:
    if "SCENARIO_NUM" in command.env:
        value = command.env["SCENARIO_NUM"]
        if value.isdigit():
            return int(value)
    if "SCENARIO" in command.env:
        value = command.env["SCENARIO"]
        if value.isdigit():
            return int(value)
    match = re.search(r"(?:-DSCENARIO_NUM=|SCENARIO_NUM=)(\d+)", command.raw)
    if match:
        return int(match.group(1))
    match = re.search(r"(?:-DSCENARIO=|SCENARIO=)(\d+)", command.raw)
    return int(match.group(1)) if match else None


def make_scenario_cell(cell: Cell, project_root: Path, scenario_num: int) -> Cell:
    source_rel = source_rel_for_cell(project_root, cell)
    scenario_rel = f"{source_rel}__scenario_{scenario_num}"
    commands = [
        rewrite_command_for_scenario(command, scenario_num) for command in cell.commands
    ]
    archs = supported_archs_for_scenario(
        cell.example.archs, cell.example.path / "CMakeLists.txt", scenario_num
    )
    example = ExampleSpec(
        cell.example.path,
        scenario_rel,
        commands,
        archs,
        cell.example.modes,
        cell.example.source,
        cell.example.suggestions,
    )
    return Cell(
        example,
        cell.arch,
        cell.mode,
        commands,
        cell.example.path / f"build_{cell.mode}_scenario_{scenario_num}",
    )


def rewrite_command_for_scenario(command: Command, scenario_num: int) -> Command:
    raw = command.raw
    if raw:
        raw = replace_scenario_num_arg(raw, scenario_num)
        has_cmake_scenario = "-DSCENARIO_NUM=" in raw or "-DSCENARIO=" in raw
        if (
            command.kind == "cmake"
            and is_cmake_configure(raw)
            and not has_cmake_scenario
        ):
            raw = f"{raw} -DSCENARIO_NUM={scenario_num}"
    env = dict(command.env)
    env["SCENARIO_NUM"] = str(scenario_num)
    if "SCENARIO" in env:
        env["SCENARIO"] = str(scenario_num)
    return Command(raw, command.kind, env)


def replace_scenario_num_arg(command: str, scenario_num: int) -> str:
    value = str(scenario_num)
    patterns = [
        r"(-DSCENARIO_NUM=)(?:\$\{?SCENARIO(?:_NUM)?\}?|\d+)",
        r"(-DSCENARIO=)(?:\$\{?SCENARIO(?:_NUM)?\}?|\d+)",
        r"(\bSCENARIO_NUM=)(?:\$\{?SCENARIO(?:_NUM)?\}?|\d+)",
        r"(\bSCENARIO=)(?:\$\{?SCENARIO(?:_NUM)?\}?|\d+)",
        r"(-scenarioNum(?:=|\s+))(?:\$\{?SCENARIO(?:_NUM)?\}?|\d+)",
        r"(-scenario_num(?:=|\s+))(?:\$\{?SCENARIO(?:_NUM)?\}?|\d+)",
        r"(--scenario(?:=|\s+))(?:\$\{?SCENARIO(?:_NUM)?\}?|\d+)",
        r"(--mode(?:=|\s+))(?:\$\{?SCENARIO(?:_NUM)?\}?|\d+)",
    ]
    for pattern in patterns:
        command = re.sub(pattern, lambda match: match.group(1) + value, command)
    return command


def supported_archs_for_scenario(
    base_archs: List[str], cmake_file: Path, scenario_num: int
) -> List[str]:
    if not cmake_file.exists():
        return base_archs
    text = scenario_arch_text(cmake_file)
    if scenario_only_supports_arch(text, scenario_num, "dav-3510"):
        return [arch for arch in base_archs if arch == "dav-3510"]
    if scenario_only_supports_arch(text, scenario_num, "dav-2201"):
        return [arch for arch in base_archs if arch == "dav-2201"]
    return base_archs


def scenario_arch_text(cmake_file: Path) -> str:
    parts = [cmake_file.read_text(encoding="utf-8", errors="ignore")]
    readme = cmake_file.parent / "README.md"
    if readme.exists():
        parts.append(readme.read_text(encoding="utf-8", errors="ignore"))
    for asc_file in sorted(cmake_file.parent.glob("*.asc")):
        parts.append(asc_file.read_text(encoding="utf-8", errors="ignore"))
    return "\n".join(parts)


def scenario_only_supports_arch(text: str, scenario_num: int, arch: str) -> bool:
    scenario = str(scenario_num)
    if re.search(rf"场景[0-9/、,，\s]*{scenario}[0-9/、,，\s]*仅支持{arch}", text):
        return True
    if re.search(
        rf"<tr>(?:(?!</tr>).)*<td>\s*{scenario}\s*</td>(?:(?!</tr>).)*仅支持\s*{arch}(?:(?!</tr>).)*</tr>",
        text,
        flags=re.S,
    ):
        return True
    if arch == "dav-3510" and re.search(
        rf"(?:场景|Scenario)\s*{scenario}[^\n\r]*(?:仅在|only supports)[^\n\r]*Ascend\s*950",
        text,
        flags=re.IGNORECASE,
    ):
        return True
    if arch == "dav-2201" and re.search(
        rf"SCENARIO_NUM\s*=\s*{scenario}[^\n\r]*(?:不支持|does not support|not support)[^\n\r]*Ascend\s*950",
        text,
        flags=re.IGNORECASE,
    ):
        return True
    if arch == "dav-2201" and re.search(
        rf"SCENARIO_NUM\s*={scenario}`?\s*时[^\n\r]*(?:不支持|does not support|not support)[^\n\r]*Ascend\s*950",
        text,
        flags=re.IGNORECASE,
    ):
        return True
    if re.search(
        rf"{scenario}\s*=[^\n\r,，)]*(?:仅|only supports).*(?:Ascend\s*950|{arch})",
        text,
    ):
        return True
    if arch == "dav-3510" and scenario != "1":
        if re.search(r"A2/A3架构仅支持Scenario\s*1", text, flags=re.IGNORECASE):
            return True

    for match in re.finditer(
        r"if\s*\((.*?)\)(.*?)(?:endif\s*\(\)|$)", text, flags=re.S
    ):
        condition = match.group(1)
        body = match.group(2)
        if not re.search(
            rf"SCENARIO_NUM[^\n\r)]*(?:STREQUAL|EQUAL)\s*\"?{scenario}\"?", condition
        ):
            continue
        combined = condition + "\n" + body
        if arch == "dav-3510" and (
            f'NOT CMAKE_ASC_ARCHITECTURES STREQUAL "{arch}"' in combined
            or f"only supports {arch}" in combined
            or f"仅支持{arch}" in combined
            or "仅Ascend 950" in combined
        ):
            return True
        if arch == "dav-2201" and (
            'CMAKE_ASC_ARCHITECTURES STREQUAL "dav-3510"' in combined
            and "only supports dav-2201" in combined
        ):
            return True
    return False


def scenario_num_from_case_rel(rel: str) -> int | None:
    match = re.search(r"__scenario_(\d+)$", rel)
    return int(match.group(1)) if match else None


def source_rel_for_cell(project_root: Path | None, cell: Cell) -> str:
    if project_root is None:
        return re.sub(r"__scenario_\d+$", "", cell.example.rel_path)
    examples_root = project_root / "examples"
    try:
        return cell.example.path.relative_to(examples_root).as_posix()
    except ValueError:
        return re.sub(r"__scenario_\d+$", "", cell.example.rel_path)


def runner_confidence(cell, runnable_on_target: bool) -> tuple[str, List[str]]:
    confidence, reasons = classify_confidence(cell)
    if not runnable_on_target:
        reasons.append("unsupported_on_target_arch_or_mode")
        confidence = downgrade(confidence)
    return confidence, reasons


def build_runner_render_spec(
    cell, confidence: str, reasons: List[str]
) -> tuple[RunnerRenderSpec, str]:
    rel = cell.example.rel_path
    source_rel = source_rel_for_cell(None, cell)
    build_cmds = build_commands_for_runner(cell.commands)
    run_cmds = run_commands_for_runner(cell.commands, build_cmds)
    build_cmds, run_cmds = merge_export_commands_into_run(build_cmds, run_cmds)
    verify_cmds = [
        command for command in cell.commands if command.raw and command.kind == "verify"
    ]
    if not run_cmds:
        run_cmds = [Command(":", "run")]
        if "no_run_step" not in reasons:
            reasons.append("no_run_step")
            confidence = downgrade(confidence)
    if not verify_cmds:
        verify_cmds = [Command(":", "verify")]
    custom_op_package_case = source_rel == CUSTOM_OP_PACKAGE_CASE
    custom_op_dependency = requires_custom_op_package(source_rel)
    skip_reason, skip_modes, skip_archs = explicit_skip_config(source_rel, cell.arch)
    all_modes_skipped = skip_reason and all(
        mode in skip_modes for mode in cell.example.modes
    )
    skip_ctx = _SkipContext(
        all_modes_skipped=all_modes_skipped,
        skip_reason=skip_reason,
        custom_op_package_case=custom_op_package_case,
        custom_op_dependency=custom_op_dependency,
    )
    result = _apply_skip_resolution(
        skip_ctx, confidence, reasons, (build_cmds, run_cmds, verify_cmds)
    )
    return (
        RunnerRenderSpec(
            rel=rel,
            source_rel=source_rel,
            scenario_num=scenario_num_from_case_rel(rel),
            build_cmds=result.build_cmds,
            run_cmds=result.run_cmds,
            verify_cmds=result.verify_cmds,
            custom_op_dependency=custom_op_dependency,
            custom_op_package_case=custom_op_package_case,
            skip_reason=skip_reason,
            skip_modes=skip_modes,
            skip_archs=skip_archs,
        ),
        result.confidence,
    )


def _apply_skip_resolution(
    ctx: _SkipContext,
    confidence: str,
    reasons: List[str],
    stage_cmds: tuple[List[Command], List[Command], List[Command]],
) -> _SkipResult:
    build_cmds, run_cmds, verify_cmds = stage_cmds
    if ctx.all_modes_skipped:
        reasons.append("explicit_skip")
        noop_build, noop_run, noop_verify = noop_stage_commands()
        return _SkipResult("high", reasons, noop_build, noop_run, noop_verify)
    if ctx.skip_reason:
        reasons.append("partial_mode_skip")
        return _SkipResult("high", reasons, build_cmds, run_cmds, verify_cmds)
    if ctx.custom_op_package_case:
        reasons.append("custom_op_package_provider")
        noop_build, noop_run, noop_verify = noop_stage_commands()
        return _SkipResult(confidence, reasons, noop_build, noop_run, noop_verify)
    if ctx.custom_op_dependency:
        reasons.append("requires_custom_op_package")
    return _SkipResult(confidence, reasons, build_cmds, run_cmds, verify_cmds)


def noop_stage_commands() -> tuple[List[Command], List[Command], List[Command]]:
    return [Command(":", "build")], [Command(":", "run")], [Command(":", "verify")]


def build_commands_for_runner(commands: Iterable[Command]) -> List[Command]:
    command_list = list(commands)
    result: List[Command] = []
    for command in command_list:
        if not command.raw:
            continue
        if command.kind in {"cmake", "make", "gen_data", "shell"}:
            result.append(command)
            continue
        if is_atc_prerequisite_command(command, command_list):
            result.append(command)
    return result


def run_commands_for_runner(
    commands: Iterable[Command], build_cmds: List[Command]
) -> List[Command]:
    result: List[Command] = []
    for command in commands:
        if not command.raw or command.kind not in {"run", "package_run"}:
            continue
        if command not in build_cmds:
            result.append(command)
    return result


def merge_export_commands_into_run(
    build_cmds: List[Command],
    run_cmds: List[Command],
) -> tuple[List[Command], List[Command]]:
    export_cmds: List[str] = []
    remaining_build_cmds: List[Command] = []
    for command in build_cmds:
        if is_export_command(command.raw):
            export_cmds.append(command.raw)
        else:
            remaining_build_cmds.append(command)
    if not export_cmds or not run_cmds:
        return build_cmds, run_cmds

    first_run = run_cmds[0]
    merged_run = Command(
        "; ".join([*export_cmds, first_run.raw]), first_run.kind, dict(first_run.env)
    )
    return remaining_build_cmds, [merged_run, *run_cmds[1:]]


def is_export_command(command: str) -> bool:
    return bool(re.match(r"^export\s+[A-Za-z_][A-Za-z0-9_]*=", command.strip()))


def classify_confidence(cell) -> tuple[str, List[str]]:
    reasons: List[str] = []
    confidence = "high"
    rel = cell.example.rel_path
    commands = [command.raw for command in cell.commands if command.raw]
    joined = "\n".join(commands)
    if cell.example.source != "readme":
        reasons.append(f"source_{cell.example.source}")
        confidence = downgrade(confidence)
    if rel.startswith(SPECIAL_PREFIXES):
        reasons.append("special_02_features_00_01_02")
        confidence = "low"
    if re.search(r"\$[A-Za-z_][A-Za-z0-9_]*|\$\{[^}]+}", joined):
        reasons.append("shell_variable_reference")
        confidence = "low"
    if re.search(r"(^|[;\s])(for|while|if|case|function)\b", joined):
        reasons.append("shell_control_flow")
        confidence = "low"
    if "\\\n" in joined or any(command.rstrip().endswith("\\") for command in commands):
        reasons.append("line_continuation")
        confidence = "low"
    if any("atc " in command or command.startswith("atc") for command in commands):
        reasons.append("atc_command")
        confidence = downgrade(confidence)
    if any(
        "tensorflow" in command.lower() or "onnx" in command.lower()
        for command in commands
    ):
        reasons.append("framework_command")
        confidence = downgrade(confidence)
    if not any(command.startswith("cmake") for command in commands):
        reasons.append("no_cmake_step")
        confidence = downgrade(confidence)
    if not any(command.startswith("make") for command in commands):
        reasons.append("no_make_step")
        confidence = downgrade(confidence)
    return confidence, sorted(set(reasons))


def is_atc_prerequisite_command(command: Command, commands: Iterable[Command]) -> bool:
    if command.kind != "run" or not command.raw.startswith("python3 "):
        return False
    command_list = list(commands)
    try:
        index = command_list.index(command)
    except ValueError:
        return False
    return any(item.raw.startswith("atc ") for item in command_list[index + 1 :])


def downgrade(confidence: str) -> str:
    if confidence == "high":
        return "medium"
    return confidence


def requires_custom_op_package(rel: str) -> bool:
    if rel == CUSTOM_OP_PACKAGE_CASE:
        return True
    if rel in CUSTOM_OP_PACKAGE_DEPENDENTS:
        return True
    if "/00_framework/01_tensorflow/" in rel:
        return True
    return False


def explicit_skip_config(rel: str, arch: str) -> tuple[str, List[str], List[str]]:
    config = SKIP_CONFIG.get(rel, SkipConfig())
    if config.archs and arch not in config.archs:
        return "", [], config.archs
    return config.reason, config.modes, config.archs


def render_runner(
    spec: RunnerRenderSpec,
) -> str:
    spec = apply_scenario_to_render_spec(spec)
    if spec.rel == PARALLEL_OPS_PACKAGE_CASE:
        return render_parallel_ops_package_runner(spec)
    if spec.rel == TILING_SINK_PROGRAMMING_CASE:
        return render_tiling_sink_programming_runner(spec)
    build_prefix = custom_op_guard(spec.custom_op_dependency, spec.skip_reason)
    run_prefix = custom_op_guard(spec.custom_op_package_case, spec.skip_reason)
    inject_cmake_arch = spec.rel not in NO_CMAKE_ARCH_INJECTION_CASES
    lines = [
        *runner_header(spec),
        *runner_function(
            "case_build",
            [
                *build_prefix,
                *indent_commands(spec.build_cmds, inject_cmake_arch=inject_cmake_arch),
            ],
        ),
        *runner_function(
            "case_run",
            [*run_prefix, *indent_commands(spec.run_cmds, default_cd_build=True)],
        ),
        *runner_function(
            "case_verify", indent_commands(spec.verify_cmds, default_cd_build=True)
        ),
        "case_clean() {",
        "    presmoke_default_clean",
        "}",
        "",
        'presmoke_case_main "$@"',
        "",
    ]
    return "\n".join(lines)


def apply_scenario_to_render_spec(spec: RunnerRenderSpec) -> RunnerRenderSpec:
    if spec.scenario_num is None:
        return spec
    return RunnerRenderSpec(
        rel=spec.rel,
        build_cmds=[
            rewrite_command_for_scenario(command, spec.scenario_num)
            for command in spec.build_cmds
        ],
        run_cmds=[
            rewrite_command_for_scenario(command, spec.scenario_num)
            for command in spec.run_cmds
        ],
        verify_cmds=[
            rewrite_command_for_scenario(command, spec.scenario_num)
            for command in spec.verify_cmds
        ],
        source_rel=spec.source_rel,
        scenario_num=spec.scenario_num,
        custom_op_dependency=spec.custom_op_dependency,
        custom_op_package_case=spec.custom_op_package_case,
        skip_reason=spec.skip_reason,
        skip_modes=spec.skip_modes,
        skip_archs=spec.skip_archs,
    )


def render_tiling_sink_programming_runner(spec: RunnerRenderSpec) -> str:
    build_prefix = custom_op_guard(spec.custom_op_dependency, spec.skip_reason)
    lines = [
        *runner_header(spec),
        *runner_function(
            "case_build", [*build_prefix, *indent_commands(spec.build_cmds)]
        ),
        *runner_function(
            "case_run",
            [
                "    local sink_log_pattern",
                f"    sink_log_pattern={shlex.quote(TILING_SINK_GENERATE_TASK_PATTERN_PREFIX)}",
                f"    sink_log_pattern+={shlex.quote(TILING_SINK_GENERATE_TASK_PATTERN_SUFFIX)}",
                *indent_tiling_sink_run_commands(spec.run_cmds),
                '    presmoke_verify_tiling_sink_task_log_for_pid "$case_pid" "$sink_log_pattern"',
            ],
        ),
        *runner_function(
            "case_verify", indent_commands(spec.verify_cmds, default_cd_build=True)
        ),
        "case_clean() {",
        "    presmoke_default_clean",
        "}",
        "",
        'presmoke_case_main "$@"',
        "",
    ]
    return "\n".join(lines)


def render_parallel_ops_package_runner(spec: RunnerRenderSpec) -> str:
    lines = [
        *runner_header(spec),
        "case_build() {",
        "    local cmake_args=(cmake -S . -B build)",
        '    if [[ -n "${NLOHMANN_JSON_URL:-}" ]]; then',
        '        cmake_args+=("-DNLOHMANN_JSON_URL=$NLOHMANN_JSON_URL")',
        "    fi",
        '    if [[ -n "${RUN_MODE_ARG:-}" ]]; then',
        '        cmake_args+=("$RUN_MODE_ARG")',
        "    fi",
        "",
        '    mkdir -p "$BUILD_DIR"',
        '    (cd "$CASE_DIR" && soc_version=$SOC_VERSION presmoke_run_command "${cmake_args[@]}")',
        '    (cd "$CASE_DIR" && soc_version=$SOC_VERSION presmoke_run_command cmake --build build -j)',
        "}",
        "",
        *runner_function(
            "case_run", indent_commands(spec.run_cmds, default_cd_build=True)
        ),
        *runner_function(
            "case_verify", indent_commands(spec.verify_cmds, default_cd_build=True)
        ),
        "case_clean() {",
        '    rm -rf "$CASE_DIR/build" "$BUILD_DIR"',
        "}",
        "",
        'presmoke_case_main "$@"',
        "",
    ]
    return "\n".join(lines)


def custom_op_guard(enabled: bool, skip_reason: str) -> List[str]:
    return (
        ["    presmoke_ensure_custom_op_package"] if enabled and not skip_reason else []
    )


def runner_header(spec: RunnerRenderSpec) -> List[str]:
    source_rel = spec.source_rel or spec.rel
    return [
        *BASH_LICENSE_HEADER,
        "",
        "set -euo pipefail",
        "",
        f"CASE_REL={shlex.quote(source_rel)}",
        *skip_reason_lines(spec.skip_reason),
        *skip_modes_lines(spec.skip_modes),
        'SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"',
        f'source "$SCRIPT_DIR/{case_entry_relative_path(spec.rel)}"',
        'presmoke_case_init "$CASE_REL"',
        *scenario_build_dir_lines(spec.scenario_num),
        "",
    ]


def scenario_build_dir_lines(scenario_num: int | None) -> List[str]:
    if scenario_num is None:
        return []
    return [
        f'BUILD_DIR="$CASE_DIR/build_${{MODE}}_scenario_{scenario_num}"',
        "export BUILD_DIR",
    ]


def skip_reason_lines(skip_reason: str) -> List[str]:
    if not skip_reason:
        return []
    return [f"SKIP_REASON={shlex.quote(skip_reason)}", ""]


def skip_modes_lines(skip_modes: List[str]) -> List[str]:
    if not skip_modes:
        return []
    modes_str = " ".join(skip_modes)
    return [f"SKIP_MODES=({modes_str})", ""]


def runner_function(name: str, body: List[str]) -> List[str]:
    return [
        f"{name}() {{",
        '    mkdir -p "$BUILD_DIR"',
        *body,
        "}",
        "",
    ]


def case_entry_relative_path(rel: str) -> str:
    depth = len(Path(rel).parts)
    return "../" * depth + "_case_entry.sh"


def indent_commands(
    commands: Iterable[Command],
    default_cd_build: bool = False,
    inject_cmake_arch: bool = True,
) -> List[str]:
    lines: List[str] = []
    for command in commands:
        if isinstance(command, str):
            command = Command(command, "shell")
        lines.append(indent_command(command, default_cd_build, inject_cmake_arch))
    return lines


def indent_tiling_sink_run_commands(commands: Iterable[Command]) -> List[str]:
    lines: List[str] = ["    local case_pid case_rc=0"]
    for command in commands:
        raw = rewrite_runtime_arch_options(command.raw)
        raw = rewrite_runtime_cmake_options(raw)
        quoted = shlex.quote(f"exec {raw}")
        env_prefix = command_env_prefix(command)
        lines.extend(
            [
                "    (",
                f'        cd "{command_workdir(raw, default_cd_build=True)}"',
                "        exec env \\",
                "            ASCEND_GLOBAL_LOG_LEVEL=1 \\",
                f"            {env_prefix}bash -lc {quoted}",
                "    ) &",
                "    case_pid=$!",
                '    wait "$case_pid" || case_rc=$?',
                "    if (( case_rc != 0 )); then",
                '        return "$case_rc"',
                "    fi",
            ]
        )
    return lines


def indent_command(
    command: Command, default_cd_build: bool = False, inject_cmake_arch: bool = True
) -> str:
    raw = command.raw
    if not raw or raw == ":":
        return "    :"
    raw = rewrite_runtime_arch_options(raw)
    raw = rewrite_runtime_cmake_options(raw, inject_cmake_arch)
    quoted = shlex.quote(raw)
    env_prefix = command_env_prefix(command)
    runner = f"{env_prefix}bash -lc {quoted}"
    return f'    (cd "{command_workdir(raw, default_cd_build)}" && {runner})'


def command_workdir(command: str, default_cd_build: bool = False) -> str:
    if command.startswith("cmake "):
        if is_cmake_configure(command) and (" .." in command or command.endswith("..")):
            return "$BUILD_DIR"
        return "$CASE_DIR"
    if command.startswith("make") or command.startswith("atc "):
        return "$BUILD_DIR"
    if command_runs_from_build_dir(command, default_cd_build):
        return "$BUILD_DIR"
    return "$CASE_DIR"


def command_runs_from_build_dir(command: str, default_cd_build: bool = False) -> bool:
    if re.match(r"^(export\s+[A-Za-z_][A-Za-z0-9_]*=.*;\s*)+\./", command.strip()):
        return True
    if command.startswith("python3 ../"):
        return True
    if "python3 ../" in command and (
        "--application=" in command or command.startswith("msprof ")
    ):
        return True
    return (
        default_cd_build
        and command.startswith("./")
        and not command.startswith("./build/")
    )


def rewrite_runtime_cmake_options(command: str, inject_cmake_arch: bool = True) -> str:
    if not command.startswith("cmake ") or not is_cmake_configure(command):
        return command
    command = re.sub(
        r"-DCMAKE_ASC_ARCHITECTURES=(?:'[^']*'|\"[^\"]*\"|[^ \t;]+)",
        '-DCMAKE_ASC_ARCHITECTURES="$ARCH"' if inject_cmake_arch else "",
        command,
    )
    command = re.sub(
        r"-DCMAKE_ASC_RUN_MODE=(?:'[^']*'|\"[^\"]*\"|[^ \t;]+)",
        "",
        command,
    )
    command = re.sub(r"[ \t]+", " ", command).strip()
    if inject_cmake_arch and "-DCMAKE_ASC_ARCHITECTURES=" not in command:
        command = f'{command} -DCMAKE_ASC_ARCHITECTURES="$ARCH"'
    return f"{command} $RUN_MODE_ARG"


def rewrite_runtime_arch_options(command: str) -> str:
    command = re.sub(
        r"--npu-arch=(?:'[^']*'|\"[^\"]*\"|\$[A-Za-z_][A-Za-z0-9_]*|\$\{[^}]+}|[^ \t;]+)",
        "--npu-arch=$ARCH",
        command,
    )
    command = re.sub(
        r"-DNPU_ARCH=(?:'[^']*'|\"[^\"]*\"|\$[A-Za-z_][A-Za-z0-9_]*|\$\{[^}]+}|[^ \t;]+)",
        '-DNPU_ARCH="$ARCH"',
        command,
    )
    return command


def command_env_prefix(command: Command) -> str:
    env = dict(command.env)
    env.setdefault("soc_version", "$SOC_VERSION")
    for key in ARCH_ENV_KEYS:
        if key in env:
            env[key] = "$ARCH"
    if not env:
        return ""
    parts = [
        f"{key}={quote_env_value(str(value))}" for key, value in sorted(env.items())
    ]
    return " ".join(parts) + " "


def quote_env_value(value: str) -> str:
    if value in {"$ARCH", "$SOC_VERSION"}:
        return value
    if re.search(r"(^|[^\\])\$[A-Za-z_{]", value):
        return value
    return shlex.quote(value)


def write_manifest(reports_root: Path, reports: List[CaseReport]) -> None:
    (reports_root / "case_runner_manifest.json").write_text(
        json.dumps(
            [asdict(report) for report in reports], ensure_ascii=False, indent=2
        ),
        encoding="utf-8",
    )


def remove_stale_runners(runners_root: Path, reports: List[CaseReport]) -> None:
    expected = {item.case for item in reports}
    for runner in list(runners_root.rglob("run.sh")):
        rel = runner.parent.relative_to(runners_root).as_posix()
        if rel in expected:
            continue
        LOG.info("remove_stale_runner=%s", rel)
        runner.unlink()
        remove_empty_parents(runner.parent, runners_root)


def remove_empty_parents(path: Path, stop: Path) -> None:
    while path != stop:
        try:
            path.rmdir()
        except OSError:
            return
        path = path.parent


if __name__ == "__main__":
    raise SystemExit(main())
