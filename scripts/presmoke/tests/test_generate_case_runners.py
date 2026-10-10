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
import re
import unittest
import subprocess
import tempfile
from pathlib import Path

from presmoke.generate_case_runners import (
    NO_CMAKE_ARCH_INJECTION_CASES,
    cell_for_spec,
    command_env_prefix,
    command_workdir,
    explicit_skip_config,
    expand_scenario_cells,
    is_atc_prerequisite_command,
    merge_export_commands_into_run,
    parse_scenario_values_from_cmake,
    parse_scenario_values_from_readme,
    quote_env_value,
    render_runner,
    rewrite_command_for_scenario,
    requires_custom_op_package,
    RunnerRenderSpec,
    supported_archs_for_scenario,
)
from presmoke.model import Command, ExampleSpec
from presmoke.readme_spec import parse_readme


def render_runner_from_parts(
    rel: str,
    build_cmds: list[Command],
    run_cmds: list[Command],
    verify_cmds: list[Command],
    custom_op_dependency: bool = False,
    custom_op_package_case: bool = False,
    skip_reason: str = "",
    skip_modes: list[str] = [],
    source_rel: str = "",
    scenario_num: int | None = None,
) -> str:
    return render_runner(
        RunnerRenderSpec(
            rel=rel,
            source_rel=source_rel,
            scenario_num=scenario_num,
            build_cmds=build_cmds,
            run_cmds=run_cmds,
            verify_cmds=verify_cmds,
            custom_op_dependency=custom_op_dependency,
            custom_op_package_case=custom_op_package_case,
            skip_reason=skip_reason,
            skip_modes=skip_modes,
        )
    )


class GenerateCaseRunnersTest(unittest.TestCase):
    def expected_archs_from_supported_products(self, readme: Path) -> list[str]:
        text = readme.read_text(encoding="utf-8", errors="ignore")
        lines = text.splitlines()
        start = None
        for idx, line in enumerate(lines):
            if re.match(r"^#+\s+.*支持.*产品", line.strip()):
                start = idx + 1
                break
        if start is None:
            return []
        end = len(lines)
        for idx in range(start, len(lines)):
            if re.match(r"^##+\s+", lines[idx].strip()):
                end = idx
                break
        support = "\n".join(lines[start:end])
        archs = []
        if re.search(r"Ascend\s*950|950PR|950DT|dav-3510", support, re.IGNORECASE):
            archs.append("dav-3510")
        if re.search(
            r"Atlas\s*A[23]|Atlas\s*200I/500\s*A2|Atlas\s*200I|Atlas\s*500\s*A2|dav-2201", support, re.IGNORECASE
        ):
            archs.append("dav-2201")
        return archs

    def test_manifest_archs_match_readme_supported_products(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        manifest = json.loads(
            (project_root / "scripts/presmoke/reports/case_runner_manifest.json").read_text(encoding="utf-8")
        )
        mismatches = []
        for item in manifest:
            readme = project_root / "examples" / item["case"] / "README.md"
            if not readme.exists():
                continue
            expected = self.expected_archs_from_supported_products(readme)
            if expected and sorted(item.get("supported_archs")) != sorted(expected):
                mismatches.append((item["case"], expected, item.get("supported_archs")))
        self.assertEqual(mismatches, [])

    def test_manifest_modes_include_readme_cpu_support(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        examples_root = project_root / "examples"
        manifest_by_source: dict[str, list[dict]] = {}
        for item in json.loads(
            (project_root / "scripts/presmoke/reports/case_runner_manifest.json").read_text(encoding="utf-8")
        ):
            manifest_by_source.setdefault(item.get("source_case") or item["case"], []).append(item)
        missing = []
        for readme in examples_root.rglob("README.md"):
            text = readme.read_text(encoding="utf-8", errors="ignore")
            if "CMAKE_ASC_RUN_MODE=cpu" not in text:
                continue
            rel_path = readme.parent.relative_to(examples_root).as_posix()
            items = manifest_by_source.get(rel_path, [])
            if not items or all("cpu" not in item.get("supported_modes", []) for item in items):
                missing.append(rel_path)

        self.assertEqual(missing, [])

    def test_manifest_includes_leaf_cmake_executable_cases(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        examples_root = project_root / "examples"
        manifest_items = json.loads(
            (project_root / "scripts/presmoke/reports/case_runner_manifest.json").read_text(encoding="utf-8")
        )
        manifest_cases = {item["case"] for item in manifest_items}
        manifest_sources = {item.get("source_case") or item["case"] for item in manifest_items}
        missing = []
        for cmake_file in examples_root.rglob("CMakeLists.txt"):
            text = cmake_file.read_text(encoding="utf-8", errors="ignore")
            if not re.search(r"\badd_executable\s*\(", text):
                continue
            rel_path = cmake_file.parent.relative_to(examples_root).as_posix()
            if rel_path in manifest_cases or rel_path in manifest_sources:
                continue
            readme = cmake_file.parent / "README.md"
            has_child_case = any(case.startswith(rel_path + "/") for case in manifest_cases)
            if readme.exists() and not has_child_case:
                missing.append(rel_path)

        self.assertEqual(missing, [])

    def test_softmax_scenario_range_is_discovered_from_cmake(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        cmake_file = (
            project_root
            / "examples/01_simd_cpp_api/05_best_practices/02_reg_compute/softmax_high_performance/CMakeLists.txt"
        )

        self.assertEqual(parse_scenario_values_from_cmake(cmake_file), [0, 1, 2, 3, 4, 5])

    def test_scenario_description_does_not_treat_type_widths_as_scenarios(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            cmake_file = Path(tmp) / "CMakeLists.txt"
            cmake_file.write_text(
                'set(SCENARIO_NUM "1" CACHE STRING "Scenario: 1=half-to-int4b_t, 2=half-to-int32_t")\n',
                encoding="utf-8",
            )

            self.assertEqual(parse_scenario_values_from_cmake(cmake_file), [1, 2])

    def test_scenario_range_can_be_discovered_from_readme_table(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        readme_file = project_root / "examples/01_simd_cpp_api/04_advanced_api/08_transpose/transdata/README.md"

        self.assertEqual(parse_scenario_values_from_readme(readme_file), [1, 2, 3, 4])

    def test_scenario_runner_uses_source_case_and_unique_build_dir(self) -> None:
        script = render_runner_from_parts(
            "01_simd_cpp_api/05_best_practices/02_reg_compute/softmax_high_performance__scenario_5",
            [Command("cmake .. -DSCENARIO_NUM=0", "cmake")],
            [Command("./demo", "run")],
            [],
            source_rel="01_simd_cpp_api/05_best_practices/02_reg_compute/softmax_high_performance",
            scenario_num=5,
        )

        self.assertIn("CASE_REL=01_simd_cpp_api/05_best_practices/02_reg_compute/softmax_high_performance", script)
        self.assertIn('BUILD_DIR="$CASE_DIR/build_${MODE}_scenario_5"', script)
        self.assertIn("SCENARIO_NUM=5", script)
        self.assertIn("-DSCENARIO_NUM=5", script)

    def test_default_scenario_is_named_explicitly(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        case_dir = project_root / "examples/01_simd_cpp_api/05_best_practices/02_reg_compute/softmax_high_performance"
        spec = ExampleSpec(
            case_dir,
            "01_simd_cpp_api/05_best_practices/02_reg_compute/softmax_high_performance",
            [Command("cmake .. -DSCENARIO_NUM=5", "cmake")],
            ["dav-3510"],
            ["npu"],
            "readme",
        )

        expanded = expand_scenario_cells([cell_for_spec(spec, "dav-3510", "npu")], project_root)
        names = [cell.example.rel_path for cell in expanded]

        self.assertIn("01_simd_cpp_api/05_best_practices/02_reg_compute/softmax_high_performance__scenario_5", names)
        self.assertNotIn("01_simd_cpp_api/05_best_practices/02_reg_compute/softmax_high_performance", names)

    def test_exported_scenario_num_is_rewritten(self) -> None:
        command = Command("export SCENARIO_NUM=1; ./demo", "run")

        rewritten = rewrite_command_for_scenario(command, 14)

        self.assertEqual(rewritten.raw, "export SCENARIO_NUM=14; ./demo")
        self.assertEqual(rewritten.env["SCENARIO_NUM"], "14")

    def test_atc_prerequisite_python_stays_in_build_before_atc(self) -> None:
        commands = [
            Command("python3 ../leaky_relu.py", "run"),
            Command("atc --model=./leaky_relu.onnx --soc_version=${soc_version}", "shell"),
            Command("cmake ..", "cmake"),
            Command("make -j", "make"),
            Command("./execute_leaky_relu_op", "run"),
        ]
        build_cmds = [
            command
            for command in commands
            if command.kind in {"cmake", "make", "gen_data", "shell"} or is_atc_prerequisite_command(command, commands)
        ]
        run_cmds = [
            command for command in commands if command.kind in {"run", "package_run"} and command not in build_cmds
        ]
        script = render_runner_from_parts("x", build_cmds, run_cmds, [])
        self.assertLess(script.index("python3 ../leaky_relu.py"), script.index("atc --model=./leaky_relu.onnx"))
        self.assertLess(script.index("atc --model=./leaky_relu.onnx"), script.index("cmake .."))
        self.assertLess(script.index("cmake .."), script.index("make -j"))
        self.assertIn("./execute_leaky_relu_op", script)

    def test_cmake_build_command_is_not_treated_as_configure(self) -> None:
        script = render_runner_from_parts(
            "x", [Command("cmake -S . -B build", "cmake"), Command("cmake --build build -j", "cmake")], [], []
        )
        self.assertIn("cmake -S . -B build -DCMAKE_ASC_ARCHITECTURES", script)
        self.assertIn("cmake --build build -j'", script)
        self.assertNotIn("cmake --build build -j -DCMAKE_ASC_ARCHITECTURES", script)

    def test_parallel_ops_package_does_not_pass_unused_arch_to_top_cmake(self) -> None:
        script = render_runner_from_parts(
            "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/parallel_ops_package",
            [],
            [Command("./build/add_custom/custom_opp_*.run", "package_run")],
            [],
        )
        self.assertIn("local cmake_args=(cmake -S . -B build)", script)
        self.assertNotIn('local cmake_args=(cmake -S . -B build "-DCMAKE_ASC_ARCHITECTURES=$ARCH")', script)
        self.assertIn('rm -rf "$CASE_DIR/build" "$BUILD_DIR"', script)
        self.assertNotIn("presmoke_with_lock", script)
        self.assertNotIn("_case_run_locked", script)

    def test_no_arch_injection_cases_do_not_pass_unused_cmake_arch(self) -> None:
        for rel_path in sorted(
            NO_CMAKE_ARCH_INJECTION_CASES
            - {"01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/parallel_ops_package"}
        ):
            with self.subTest(rel_path=rel_path):
                script = render_runner_from_parts(
                    rel_path,
                    [Command("cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201", "cmake"), Command("make -j", "make")],
                    [Command("./demo", "run")],
                    [],
                )
                self.assertNotIn("CMAKE_ASC_ARCHITECTURES", script)
                self.assertIn("$RUN_MODE_ARG", script)

    def test_msprof_application_with_parent_python_runs_from_build_dir(self) -> None:
        script = render_runner_from_parts(
            "x",
            [],
            [Command('msprof --application="python3 ../torch_library_report_tensor.py" --output="../result"', "run")],
            [],
        )
        self.assertIn('(cd "$BUILD_DIR" && soc_version=$SOC_VERSION bash -lc', script)
        self.assertIn("python3 ../torch_library_report_tensor.py", script)
        self.assertNotIn('(cd "$CASE_DIR" && soc_version=$SOC_VERSION bash -lc', script)

    def test_export_command_is_merged_into_first_run_command(self) -> None:
        build_cmds, run_cmds = merge_export_commands_into_run(
            [
                Command("cmake ..", "cmake"),
                Command('export ASCEND_CUSTOM_OPP_PATH="$(pwd)/output:${ASCEND_CUSTOM_OPP_PATH}"', "shell"),
            ],
            [Command("./demo", "run")],
        )

        self.assertEqual([command.raw for command in build_cmds], ["cmake .."])
        self.assertEqual(
            run_cmds[0].raw, 'export ASCEND_CUSTOM_OPP_PATH="$(pwd)/output:${ASCEND_CUSTOM_OPP_PATH}"; ./demo'
        )
        self.assertEqual(command_workdir(run_cmds[0].raw, default_cd_build=True), "$BUILD_DIR")

    def test_env_value_references_are_not_single_quoted(self) -> None:
        self.assertEqual(quote_env_value("$ARCH"), "$ARCH")
        self.assertEqual(quote_env_value("$SOC_VERSION"), "$SOC_VERSION")
        self.assertEqual(quote_env_value("$SCENARIO"), "$SCENARIO")
        self.assertEqual(quote_env_value("${SCENARIO}"), "${SCENARIO}")
        self.assertEqual(quote_env_value("plain value"), "'plain value'")

    def test_arch_env_assignment_uses_runtime_arch(self) -> None:
        prefix = command_env_prefix(Command("cmake ..", "cmake", {"ASC_ARCH": "dav-2201"}))
        self.assertIn("ASC_ARCH=$ARCH", prefix)
        self.assertNotIn("ASC_ARCH=dav-2201", prefix)

    def test_manual_npu_arch_option_uses_runtime_arch(self) -> None:
        script = render_runner_from_parts(
            "x",
            [Command("bisheng kernel.asc -o kernel.o --npu-arch=dav-2201", "shell")],
            [Command("./demo", "run")],
            [],
        )
        self.assertIn("--npu-arch=$ARCH", script)
        self.assertNotIn("--npu-arch=dav-2201", script)

    def test_custom_op_dependent_runner_installs_package_before_build(self) -> None:
        script = render_runner_from_parts(
            "x", [Command("cmake ..", "cmake")], [Command("./demo", "run")], [], custom_op_dependency=True
        )
        self.assertLess(script.index("presmoke_ensure_custom_op_package"), script.index("cmake .."))

    def test_tiling_sink_runner_validates_sink_task_generation_log(self) -> None:
        script = render_runner_from_parts(
            "04_aicpu/02_features/00_framework/00_pytorch/tiling_sink_programming",
            [],
            [Command("python3 test_add_custom_tiling_sink.py", "run")],
            [],
            custom_op_dependency=True,
        )

        self.assertNotIn("presmoke_clear_plog", script)
        self.assertIn("ASCEND_GLOBAL_LOG_LEVEL=1", script)
        self.assertIn("python3 test_add_custom_tiling_sink.py", script)
        self.assertIn("case_pid=$!", script)
        self.assertIn('presmoke_verify_tiling_sink_task_log_for_pid "$case_pid"', script)
        self.assertIn("GenerateTaskForSinkOp:Node [AddCustomTilingSink, AddCustomTilingSink]", script)
        self.assertIn("for the tiling sink, sk_flag [0].", script)

    def test_tiling_sink_log_verification_only_reads_target_pid(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        case_common = project_root / "scripts/presmoke/case_common.sh"
        marker = (
            "GenerateTaskForSinkOp:Node [AddCustomTilingSink, AddCustomTilingSink] starts to generate tasks "
            "for the tiling sink, sk_flag [0]."
        )
        with tempfile.TemporaryDirectory() as tmp:
            log_root = Path(tmp) / "ascend/log"
            debug_plog = log_root / "debug/plog"
            debug_plog.mkdir(parents=True)
            (debug_plog / "plog-111_20260713120000.log").write_text(marker, encoding="utf-8")
            script = f"""
source {case_common}
PRESMOKE_ASCEND_LOG_ROOT={log_root}
PRESMOKE_PLOG_FLUSH_TIMEOUT=0
presmoke_verify_tiling_sink_task_log_for_pid 222 '{marker}'
"""
            result = subprocess.run(["bash", "-c", script], text=True, capture_output=True, check=False)
            self.assertNotEqual(result.returncode, 0)

            (debug_plog / "plog-222_20260713120001.log").write_text(marker, encoding="utf-8")
            result = subprocess.run(["bash", "-c", script], text=True, capture_output=True, check=False)
            self.assertEqual(result.returncode, 0, result.stderr)

    def test_custom_op_provider_runner_uses_locked_installer_in_run_step(self) -> None:
        script = render_runner_from_parts(
            "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op",
            [Command(":", "build")],
            [Command(":", "run")],
            [],
            custom_op_dependency=True,
            custom_op_package_case=True,
        )
        self.assertEqual(script.count("presmoke_ensure_custom_op_package"), 2)

    def test_dependency_and_tensorflow_skip_rules(self) -> None:
        self.assertTrue(
            requires_custom_op_package("01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op")
        )
        self.assertTrue(
            requires_custom_op_package("01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclnn_invocation")
        )
        self.assertTrue(requires_custom_op_package("01_simd_cpp_api/02_features/00_framework/02_onnx/onnx_plugin"))
        self.assertTrue(
            requires_custom_op_package("01_simd_cpp_api/02_features/00_framework/01_tensorflow/tensorflow_custom")
        )
        reason, modes = explicit_skip_config("01_simd_cpp_api/02_features/00_framework/01_tensorflow/tensorflow_custom")
        self.assertIn("TensorFlow 2.6.5", reason)
        self.assertEqual(modes, ["npu"])
        reason, modes = explicit_skip_config(
            "01_simd_cpp_api/02_features/00_framework/01_tensorflow/tensorflow_builtin"
        )
        self.assertEqual(modes, ["npu"])

    def test_matmul_l2cache_skip_config(self) -> None:
        reason, modes = explicit_skip_config("01_simd_cpp_api/04_advanced_api/00_matmul/matmul_l2cache")
        self.assertIn("308M", reason)
        self.assertEqual(modes, ["cpu"])

    def test_matmul_fp8_is_950_only(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        example_dir = project_root / "examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_fp8"

        spec = parse_readme(example_dir, project_root / "examples")

        self.assertEqual(spec.archs, ["dav-3510"])

    def test_custom_op_provider_cases_support_910b_and_950(self) -> None:
        expected = ["dav-2201", "dav-3510"]
        project_root = Path(__file__).resolve().parents[3]
        examples_root = project_root / "examples"
        for rel_path in [
            "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op_static_lib",
            "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op",
        ]:
            spec = parse_readme(examples_root / rel_path, examples_root)
            self.assertEqual(spec.archs, expected)

    def test_custom_op_dependent_cases_support_910b_and_950(self) -> None:
        expected = ["dav-2201", "dav-3510"]
        project_root = Path(__file__).resolve().parents[3]
        examples_root = project_root / "examples"
        for rel_path in [
            "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclnn_invocation",
            "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclop_invocation",
            "01_simd_cpp_api/02_features/00_framework/01_tensorflow/tensorflow_builtin",
            "01_simd_cpp_api/02_features/00_framework/01_tensorflow/tensorflow_custom",
            "01_simd_cpp_api/02_features/00_framework/02_onnx/onnx_plugin",
            "04_aicpu/02_features/00_framework/00_pytorch/tiling_sink_programming",
        ]:
            spec = parse_readme(examples_root / rel_path, examples_root)
            self.assertEqual(spec.archs, expected)

    def test_scenario_arch_limit_accepts_compact_chinese_text(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        cmake_file = (
            project_root / "examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_gm2l1/CMakeLists.txt"
        )

        archs = supported_archs_for_scenario(["dav-2201", "dav-3510"], cmake_file, 3)

        self.assertEqual(archs, ["dav-3510"])

    def test_scenario_arch_limit_handles_a2_a3_only_scenario_one(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        cmake_file = (
            project_root
            / "examples/01_simd_cpp_api/05_best_practices/03_fusion_compute/"
            / "matmul_gelu_high_performance/CMakeLists.txt"
        )

        archs = supported_archs_for_scenario(["dav-2201", "dav-3510"], cmake_file, 2)

        self.assertEqual(archs, ["dav-3510"])

    def test_scenario_arch_limit_reads_readme_table_only_supports_arch(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        cmake_file = (
            project_root
            / "examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/element_wise_logic/CMakeLists.txt"
        )

        archs = supported_archs_for_scenario(["dav-2201", "dav-3510"], cmake_file, 2)

        self.assertEqual(archs, ["dav-3510"])

    def test_scenario_arch_limit_keeps_supported_rows_when_later_rows_are_950_only(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        cmake_file = (
            project_root
            / "examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_pad_gm2ub_ub2gm/CMakeLists.txt"
        )

        scenario_two_archs = supported_archs_for_scenario(["dav-2201", "dav-3510"], cmake_file, 2)
        scenario_three_archs = supported_archs_for_scenario(["dav-2201", "dav-3510"], cmake_file, 3)

        self.assertEqual(scenario_two_archs, ["dav-2201", "dav-3510"])
        self.assertEqual(scenario_three_archs, ["dav-3510"])

    def test_scenario_arch_limit_reads_readme_heading_only_supports_ascend_950(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        cmake_file = (
            project_root
            / "examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/element_wise_logic/CMakeLists.txt"
        )

        archs = supported_archs_for_scenario(["dav-2201", "dav-3510"], cmake_file, 3)

        self.assertEqual(archs, ["dav-3510"])

    def test_rendered_runner_delegates_common_boilerplate_to_case_common(self) -> None:
        script = render_runner_from_parts(
            "01_simd_cpp_api/00_introduction/00_quickstart/hello_world",
            [Command("cmake ..", "cmake"), Command("make -j", "make")],
            [Command("./demo", "run")],
            [],
        )

        self.assertIn("presmoke_case_init", script)
        self.assertIn("_case_entry.sh", script)
        self.assertIn("case_build()", script)
        self.assertIn("case_run()", script)
        self.assertIn("case_verify()", script)
        self.assertIn('presmoke_case_main "$@"', script)
        self.assertNotIn("COMMON_SH", script)
        self.assertNotIn("SEARCH_DIR", script)
        self.assertNotIn('CASE_DIR="$(presmoke_case_root "$CASE_REL")"', script)
        self.assertNotIn('ACTION="${1:-all}"', script)
        self.assertNotIn('case "$ACTION" in', script)

    def test_rtc_template_add_runner_uses_cmake_target_name(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        runner = project_root / "scripts/presmoke/cases/01_simd_cpp_api/02_features/05_aclrtc/rtc_template_add/run.sh"
        manifest = json.loads((project_root / "scripts/presmoke/reports/case_runner_manifest.json").read_text())
        item = next(entry for entry in manifest if entry["case"].endswith("05_aclrtc/rtc_template_add"))

        self.assertIn("bash -lc ./demo", runner.read_text(encoding="utf-8"))
        self.assertNotIn("bash -lc ./main", runner.read_text(encoding="utf-8"))
        self.assertIn("./demo", item["commands"])
        self.assertNotIn("./main", item["commands"])

    def test_skip_runner_clean_still_removes_build_directory(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        runner = (
            project_root
            / "scripts/presmoke/cases/01_simd_cpp_api/02_features/00_framework/01_tensorflow/tensorflow_builtin/run.sh"
        )
        build_dir = (
            project_root
            / "examples/01_simd_cpp_api/02_features/00_framework/01_tensorflow/tensorflow_builtin/build_npu"
        )

        with tempfile.TemporaryDirectory(dir=project_root) as tmp:
            marker = build_dir / "presmoke_clean_marker"
            build_dir.mkdir(parents=True, exist_ok=True)
            marker.write_text("stale", encoding="utf-8")
            env = {"PRESMOKE_PROJECT_ROOT": str(project_root), **dict(__import__("os").environ)}

            result = subprocess.run(["bash", str(runner), "clean"], cwd=tmp, env=env, text=True, capture_output=True)

        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertFalse(build_dir.exists())

    def test_skip_modes_lines_generates_bash_array(self) -> None:
        script = render_runner_from_parts(
            "01_simd_cpp_api/04_advanced_api/00_matmul/matmul_l2cache",
            [],
            [],
            [],
            skip_reason="test skip",
            skip_modes=["cpu", "sim"],
        )
        self.assertIn("SKIP_MODES=(cpu sim)", script)

    def test_case_common_maps_950_arch_to_supported_soc_version(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        case_common = project_root / "scripts/presmoke/case_common.sh"

        result = subprocess.run(
            ["bash", "-c", f"source {case_common}; presmoke_soc_version dav-3510"],
            text=True,
            capture_output=True,
            check=False,
        )

        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(result.stdout.strip(), "Ascend950PR_9599")

    def test_custom_op_stamp_requires_installed_files_to_be_complete(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        case_common = project_root / "scripts/presmoke/case_common.sh"

        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            custom_op = root / "examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op"
            state = root / ".presmoke_state"
            custom_op.mkdir(parents=True)
            state.mkdir()
            (state / "custom_op_dav-2201_npu.installed").write_text("stale", encoding="utf-8")
            opp = root / "opp"
            build_dynamic = custom_op / "build_npu/op_kernel/ascendc_kernels/binary/dynamic/add_custom"
            build_dynamic.mkdir(parents=True)
            (build_dynamic / "add_custom_tiling.h").write_text("header", encoding="utf-8")
            vendor_dynamic = opp / "vendors/customize/op_impl/ai_core/tbe/customize_impl/dynamic"
            vendor_dynamic.mkdir(parents=True)
            (vendor_dynamic / "add_custom.py").write_text("", encoding="utf-8")
            (vendor_dynamic / "add_custom_tiling_sink.py").write_text("", encoding="utf-8")
            add_custom_dynamic = opp / "vendors/add_custom/op_impl/ai_core/tbe/add_custom_impl/dynamic"
            add_custom_dynamic.mkdir(parents=True)
            (add_custom_dynamic / "add_custom.py").write_text("", encoding="utf-8")
            (add_custom_dynamic / "add_custom_kernel.cpp").write_text("", encoding="utf-8")
            vendor_api = opp / "vendors/customize/op_api"
            (vendor_api / "include").mkdir(parents=True)
            (vendor_api / "lib").mkdir(parents=True)
            (vendor_api / "include/aclnn_add_custom.h").write_text("", encoding="utf-8")
            (vendor_api / "include/aclnn_add_custom_tiling_sink.h").write_text("", encoding="utf-8")
            (vendor_api / "lib/libcust_opapi.so").write_text("", encoding="utf-8")
            vendor_framework = opp / "vendors/customize/framework/onnx"
            vendor_framework.mkdir(parents=True)
            (vendor_framework / "libcust_onnx_parsers.so").write_text("", encoding="utf-8")
            vendor_master = opp / "vendors/customize/op_impl/ai_core/tbe/op_master_device/lib"
            vendor_master.mkdir(parents=True)
            (vendor_master / "libcust_opmaster.so").write_text("", encoding="utf-8")

            script = f"""
set -euo pipefail
source {case_common}
export PRESMOKE_PROJECT_ROOT={root}
export PRESMOKE_STATE_DIR={state}
export ASCEND_OPP_PATH={opp}
export PRESMOKE_ARCH=dav-2201
export PRESMOKE_MODE=npu
presmoke_run_command() {{
    if [[ "$1" == "make" ]]; then
        cat > custom_opp_test.run <<'RUN'
#!/usr/bin/env bash
set -euo pipefail
vendor="$ASCEND_OPP_PATH/vendors/customize"
mkdir -p "$vendor/op_impl/ai_core/tbe/customize_impl/dynamic"
mkdir -p "$vendor/op_api/include" "$vendor/op_api/lib"
mkdir -p "$vendor/framework/onnx" "$vendor/op_impl/ai_core/tbe/op_master_device/lib"
touch "$vendor/op_impl/ai_core/tbe/customize_impl/dynamic/add_custom.py"
touch "$vendor/op_impl/ai_core/tbe/customize_impl/dynamic/add_custom_tiling_sink.py"
touch "$vendor/op_api/include/aclnn_add_custom.h"
touch "$vendor/op_api/include/aclnn_add_custom_tiling_sink.h"
touch "$vendor/op_api/lib/libcust_opapi.so"
touch "$vendor/framework/onnx/libcust_onnx_parsers.so"
touch "$vendor/op_impl/ai_core/tbe/op_master_device/lib/libcust_opmaster.so"
RUN
        chmod +x custom_opp_test.run
        return 0
    fi
    if [[ "$1" == "./custom_opp_test.run" ]]; then
        "$@"
        return
    fi
    return 0
}}
presmoke_ensure_custom_op_package
"""

            result = subprocess.run(["bash", "-c", script], text=True, capture_output=True, check=False)

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn("custom_op package already installed", result.stderr)
            self.assertTrue(
                (
                    opp / "vendors/customize/op_impl/ai_core/tbe/customize_impl/dynamic/add_custom_tiling_sink.py"
                ).exists()
            )
            self.assertTrue((add_custom_dynamic / "add_custom_tiling.h").exists())

    def test_custom_op_package_install_does_not_pass_unused_cmake_arch(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        case_common = project_root / "scripts/presmoke/case_common.sh"

        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            custom_op = root / "examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op"
            custom_op.mkdir(parents=True)
            opp = root / "opp"
            log = root / "cmake_args.log"

            script = f"""
set -euo pipefail
source {case_common}
export PRESMOKE_PROJECT_ROOT={root}
export ASCEND_OPP_PATH={opp}
export PRESMOKE_ARCH=dav-2201
export PRESMOKE_MODE=npu
presmoke_run_command() {{
    printf '%q ' "$@" >> {log}
    printf '\\n' >> {log}
    if [[ "$1" == "make" ]]; then
        cat > custom_opp_test.run <<'RUN'
#!/usr/bin/env bash
set -euo pipefail
vendor="$ASCEND_OPP_PATH/vendors/customize"
mkdir -p "$vendor/op_impl/ai_core/tbe/customize_impl/dynamic"
mkdir -p "$vendor/op_api/include" "$vendor/op_api/lib"
mkdir -p "$vendor/framework/onnx" "$vendor/op_impl/ai_core/tbe/op_master_device/lib"
touch "$vendor/op_impl/ai_core/tbe/customize_impl/dynamic/add_custom.py"
touch "$vendor/op_impl/ai_core/tbe/customize_impl/dynamic/add_custom_tiling_sink.py"
touch "$vendor/op_api/include/aclnn_add_custom.h"
touch "$vendor/op_api/include/aclnn_add_custom_tiling_sink.h"
touch "$vendor/op_api/lib/libcust_opapi.so"
touch "$vendor/framework/onnx/libcust_onnx_parsers.so"
touch "$vendor/op_impl/ai_core/tbe/op_master_device/lib/libcust_opmaster.so"
RUN
        chmod +x custom_opp_test.run
        return 0
    fi
    if [[ "$1" == "./custom_opp_test.run" ]]; then
        "$@"
        return
    fi
    return 0
}}
presmoke_ensure_custom_op_package
"""

            result = subprocess.run(["bash", "-c", script], text=True, capture_output=True, check=False)

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn("cmake ..", log.read_text(encoding="utf-8"))
            self.assertNotIn("CMAKE_ASC_ARCHITECTURES", log.read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
