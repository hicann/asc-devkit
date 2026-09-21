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
import os
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from presmoke.cli import (
    detect_cpu_count,
    main,
    resolve_cpu_build_jobs,
    resolve_cpu_run_slots,
    resolve_jobs,
    resolve_make_jobs,
)
from presmoke.model import Cell, Command, ExampleSpec
from presmoke.scheduler import (
    custom_op_dependency_violation_s,
    export_schedule_file,
    schedule_cells,
    ScheduleOptions,
    simulate_pipeline_npu_idle,
    simulate_pipeline_makespan,
)


def cell(tmp: Path, name: str) -> Cell:
    spec = ExampleSpec(tmp / name, name, [Command(":", "build")], ["dav-2201"], ["npu"], "test")
    spec.path.mkdir(parents=True, exist_ok=True)
    return Cell(spec, "dav-2201", "npu", spec.commands, spec.path / "build_npu")


def write_timing_report(path: Path, timings: dict[str, tuple[float, float]]) -> None:
    path.write_text(
        json.dumps(
            {
                "results": [
                    {
                        "example": example,
                        "steps": [{"kind": "build", "duration_s": build_s}, {"kind": "run", "duration_s": run_s}],
                    }
                    for example, (build_s, run_s) in timings.items()
                ]
            }
        ),
        encoding="utf-8",
    )


class SchedulerTest(unittest.TestCase):
    def test_default_schedule_keeps_original_order(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            cells = [cell(root, "a"), cell(root, "b"), cell(root, "c")]

            scheduled = schedule_cells(cells)

        self.assertEqual([item.example.rel_path for item in scheduled], ["a", "b", "c"])

    def test_custom_op_static_lib_is_always_before_custom_op(self) -> None:
        custom_op = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op"
        static_lib = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op_static_lib"
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            cells = [cell(root, "a"), cell(root, custom_op), cell(root, "b"), cell(root, static_lib)]

            scheduled = schedule_cells(cells)

        names = [item.example.rel_path for item in scheduled]
        self.assertLess(names.index(static_lib), names.index(custom_op))

    def test_parallel_ops_package_is_always_after_custom_op_static_lib(self) -> None:
        static_lib = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op_static_lib"
        parallel_ops = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/parallel_ops_package"
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            cells = [cell(root, parallel_ops), cell(root, "a"), cell(root, static_lib)]

            scheduled = schedule_cells(cells)

        names = [item.example.rel_path for item in scheduled]
        self.assertLess(names.index(static_lib), names.index(parallel_ops))

    def test_required_order_survives_build_desc_schedule(self) -> None:
        custom_op = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op"
        static_lib = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op_static_lib"
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            report = root / "report.json"
            report.write_text(
                json.dumps(
                    {
                        "results": [
                            {"example": custom_op, "steps": [{"kind": "build", "duration_s": 100}]},
                            {"example": static_lib, "steps": [{"kind": "build", "duration_s": 1}]},
                        ]
                    }
                ),
                encoding="utf-8",
            )
            cells = [cell(root, custom_op), cell(root, "a"), cell(root, static_lib)]

            scheduled = schedule_cells(cells, ScheduleOptions(schedule="build-desc", schedule_report=report))

        names = [item.example.rel_path for item in scheduled]
        self.assertLess(names.index(static_lib), names.index(custom_op))

    def test_custom_op_dependents_are_always_after_custom_op(self) -> None:
        custom_op = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op"
        dependents = [
            "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclnn_invocation",
            "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclop_invocation",
            "01_simd_cpp_api/02_features/00_framework/01_tensorflow/tensorflow_builtin",
            "01_simd_cpp_api/02_features/00_framework/01_tensorflow/tensorflow_custom",
            "01_simd_cpp_api/02_features/00_framework/02_onnx/onnx_plugin",
            "04_aicpu/02_features/00_framework/00_pytorch/tiling_sink_programming",
        ]
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            cells = [
                cell(root, dependents[0]),
                cell(root, "a"),
                cell(root, dependents[1]),
                cell(root, custom_op),
                cell(root, dependents[2]),
                cell(root, dependents[3]),
                cell(root, dependents[4]),
                cell(root, dependents[5]),
            ]

            scheduled = schedule_cells(cells)

        names = [item.example.rel_path for item in scheduled]
        for dependent in dependents:
            self.assertLess(names.index(custom_op), names.index(dependent))

    def test_npu_idle_min_preserves_custom_op_required_order(self) -> None:
        custom_op = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op"
        static_lib = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op_static_lib"
        dependent = "01_simd_cpp_api/02_features/00_framework/02_onnx/onnx_plugin"
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            report = root / "report.json"
            write_timing_report(report, {dependent: (1, 30), custom_op: (2, 1), static_lib: (100, 1)})
            cells = [cell(root, dependent), cell(root, custom_op), cell(root, static_lib)]

            scheduled = schedule_cells(cells, ScheduleOptions(schedule="npu-idle-min", schedule_report=report, jobs=2))

        names = [item.example.rel_path for item in scheduled]
        self.assertLess(names.index(static_lib), names.index(custom_op))
        self.assertLess(names.index(custom_op), names.index(dependent))

    def test_npu_idle_min_delays_custom_op_dependents_until_custom_op_build_ready(self) -> None:
        custom_op = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op"
        static_lib = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op_static_lib"
        dependent = "04_aicpu/02_features/00_framework/00_pytorch/tiling_sink_programming"
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            report = root / "report.json"
            write_timing_report(
                report,
                {
                    static_lib: (1, 1),
                    custom_op: (30, 1),
                    dependent: (1, 10),
                    "short-a": (1, 10),
                    "short-b": (1, 10),
                    "short-c": (1, 10),
                    "short-d": (1, 10),
                    "short-e": (1, 10),
                },
            )
            cells = [
                cell(root, static_lib),
                cell(root, custom_op),
                cell(root, dependent),
                cell(root, "short-a"),
                cell(root, "short-b"),
                cell(root, "short-c"),
                cell(root, "short-d"),
                cell(root, "short-e"),
            ]

            scheduled = schedule_cells(cells, ScheduleOptions(schedule="npu-idle-min", schedule_report=report, jobs=4))

        self.assertEqual(custom_op_dependency_violation_s(scheduled, report, jobs=4), 0.0)

    def test_build_desc_schedule_uses_historical_build_duration(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            report = root / "report.json"
            report.write_text(
                json.dumps(
                    {
                        "results": [
                            {
                                "example": "a",
                                "steps": [{"kind": "clean", "duration_s": 1}, {"kind": "build", "duration_s": 10}],
                            },
                            {"example": "b", "steps": [{"kind": "build", "duration_s": 2}]},
                        ]
                    }
                ),
                encoding="utf-8",
            )
            cells = [cell(root, "c"), cell(root, "b"), cell(root, "a")]

            scheduled = schedule_cells(cells, ScheduleOptions(schedule="build-desc", schedule_report=report))

        self.assertEqual([item.example.rel_path for item in scheduled], ["a", "b", "c"])
        self.assertIsInstance(scheduled[0], Cell)

    def test_frontload_build_desc_keeps_rest_in_original_order(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            report = root / "report.json"
            report.write_text(
                json.dumps(
                    {
                        "results": [
                            {"example": "slow", "steps": [{"kind": "build", "duration_s": 30}]},
                            {"example": "medium", "steps": [{"kind": "build", "duration_s": 20}]},
                        ]
                    }
                ),
                encoding="utf-8",
            )
            cells = [cell(root, "fast-a"), cell(root, "slow"), cell(root, "fast-b"), cell(root, "medium")]

            scheduled = schedule_cells(
                cells, ScheduleOptions(schedule="frontload-build-desc", schedule_report=report, frontload_count=1)
            )

        self.assertEqual([item.example.rel_path for item in scheduled], ["slow", "fast-a", "fast-b", "medium"])

    def test_fixed_schedule_uses_schedule_file_and_appends_missing_cases(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            schedule_file = root / "schedule.txt"
            schedule_file.write_text("b\n# comment\nunknown\n\na\n", encoding="utf-8")
            cells = [cell(root, name) for name in ["a", "b", "c"]]

            scheduled = schedule_cells(cells, ScheduleOptions(schedule="fixed", schedule_file=schedule_file))

        self.assertEqual([item.example.rel_path for item in scheduled], ["b", "a", "c"])

    def test_fixed_schedule_preserves_custom_op_required_order(self) -> None:
        custom_op = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op"
        static_lib = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op_static_lib"
        dependent = "01_simd_cpp_api/02_features/00_framework/02_onnx/onnx_plugin"
        parallel_ops = "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/parallel_ops_package"
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            schedule_file = root / "schedule.txt"
            schedule_file.write_text(f"{dependent}\n{custom_op}\n{parallel_ops}\n{static_lib}\n", encoding="utf-8")
            cells = [cell(root, dependent), cell(root, custom_op), cell(root, parallel_ops), cell(root, static_lib)]

            scheduled = schedule_cells(cells, ScheduleOptions(schedule="fixed", schedule_file=schedule_file))

        names = [item.example.rel_path for item in scheduled]
        self.assertLess(names.index(static_lib), names.index(custom_op))
        self.assertLess(names.index(static_lib), names.index(parallel_ops))
        self.assertLess(names.index(custom_op), names.index(dependent))

    def test_fixed_schedule_requires_existing_schedule_file(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            cells = [cell(root, "a")]

            with self.assertRaises(FileNotFoundError):
                schedule_cells(cells, ScheduleOptions(schedule="fixed", schedule_file=root / "missing.txt"))

    def test_export_schedule_file_writes_selected_order(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            report = root / "report.json"
            write_timing_report(report, {"slow-a": (100, 1), "fast-a": (1, 30), "fast-b": (1, 20)})
            out = root / "fixed.txt"
            cells = [cell(root, name) for name in ["slow-a", "fast-a", "fast-b"]]

            export_schedule_file(cells, out)

            self.assertEqual(out.read_text(encoding="utf-8").splitlines(), ["slow-a", "fast-a", "fast-b"])

    def test_cli_applies_schedule_before_dry_run(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            for name in ["a", "b", "c"]:
                runner = root / "scripts" / "presmoke" / "cases" / name / "run.sh"
                runner.parent.mkdir(parents=True, exist_ok=True)
                runner.write_text("#!/usr/bin/env bash\nexit 0\n", encoding="utf-8")
            manifest = root / "scripts" / "presmoke" / "reports" / "case_runner_manifest.json"
            manifest.parent.mkdir(parents=True, exist_ok=True)
            manifest.write_text(
                json.dumps(
                    [
                        {"case": "c", "supported_archs": ["dav-2201"], "supported_modes": ["npu"]},
                        {"case": "b", "supported_archs": ["dav-2201"], "supported_modes": ["npu"]},
                        {"case": "a", "supported_archs": ["dav-2201"], "supported_modes": ["npu"]},
                    ]
                ),
                encoding="utf-8",
            )
            schedule_report = root / "schedule_report.json"
            schedule_report.write_text(
                json.dumps(
                    {
                        "results": [
                            {"example": "a", "steps": [{"kind": "build", "duration_s": 30}]},
                            {"example": "b", "steps": [{"kind": "build", "duration_s": 10}]},
                        ]
                    }
                ),
                encoding="utf-8",
            )
            results_dir = root / "out"

            with mock.patch.dict(os.environ, {"PRESMOKE_PROJECT_ROOT": str(root)}):
                rc = main(
                    [
                        "--arch",
                        "dav-2201",
                        "--modes",
                        "npu",
                        "--dry-run",
                        "--schedule",
                        "build-desc",
                        "--schedule-report",
                        str(schedule_report),
                        "--report-format",
                        "json",
                        "--results",
                        str(results_dir),
                    ]
                )

            payload = json.loads((results_dir / "report.json").read_text(encoding="utf-8"))

        self.assertEqual(rc, 0)
        self.assertEqual([item["example"] for item in payload["results"]], ["a", "b", "c"])

    def test_cli_fixed_schedule_uses_builtin_schedule_file(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            for name in ["a", "b", "c"]:
                runner = root / "scripts" / "presmoke" / "cases" / name / "run.sh"
                runner.parent.mkdir(parents=True, exist_ok=True)
                runner.write_text("#!/usr/bin/env bash\nexit 0\n", encoding="utf-8")
            manifest = root / "scripts" / "presmoke" / "reports" / "case_runner_manifest.json"
            manifest.parent.mkdir(parents=True, exist_ok=True)
            manifest.write_text(
                json.dumps(
                    [
                        {"case": "a", "supported_archs": ["dav-2201"], "supported_modes": ["npu"]},
                        {"case": "b", "supported_archs": ["dav-2201"], "supported_modes": ["npu"]},
                        {"case": "c", "supported_archs": ["dav-2201"], "supported_modes": ["npu"]},
                    ]
                ),
                encoding="utf-8",
            )
            schedule_file = root / "scripts" / "presmoke" / "schedules" / "dav-2201_npu.txt"
            schedule_file.parent.mkdir(parents=True, exist_ok=True)
            schedule_file.write_text("b\na\n", encoding="utf-8")
            results_dir = root / "out"

            with mock.patch.dict(os.environ, {"PRESMOKE_PROJECT_ROOT": str(root)}):
                rc = main(
                    [
                        "--arch",
                        "dav-2201",
                        "--modes",
                        "npu",
                        "--dry-run",
                        "--schedule",
                        "fixed",
                        "--report-format",
                        "json",
                        "--results",
                        str(results_dir),
                    ]
                )

            payload = json.loads((results_dir / "report.json").read_text(encoding="utf-8"))

        self.assertEqual(rc, 0)
        self.assertEqual([item["example"] for item in payload["results"]], ["b", "a", "c"])

    def test_cli_strict_fixed_schedule_rejects_schedule_only_cases(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            for name in ["a", "b"]:
                runner = root / "scripts" / "presmoke" / "cases" / name / "run.sh"
                runner.parent.mkdir(parents=True, exist_ok=True)
                runner.write_text("#!/usr/bin/env bash\nexit 0\n", encoding="utf-8")
            manifest = root / "scripts" / "presmoke" / "reports" / "case_runner_manifest.json"
            manifest.parent.mkdir(parents=True, exist_ok=True)
            manifest.write_text(
                json.dumps(
                    [
                        {"case": "a", "supported_archs": ["dav-2201"], "supported_modes": ["npu"]},
                        {"case": "b", "supported_archs": ["dav-2201"], "supported_modes": ["npu"]},
                    ]
                ),
                encoding="utf-8",
            )
            schedule_file = root / "scripts" / "presmoke" / "schedules" / "dav-2201_npu.txt"
            schedule_file.parent.mkdir(parents=True, exist_ok=True)
            schedule_file.write_text("a\nb\nmissing\n", encoding="utf-8")

            with mock.patch.dict(os.environ, {"PRESMOKE_PROJECT_ROOT": str(root)}):
                rc = main(
                    [
                        "--arch",
                        "dav-2201",
                        "--modes",
                        "npu",
                        "--dry-run",
                        "--schedule",
                        "fixed",
                        "--strict-schedule",
                        "--report-format",
                        "json",
                        "--results",
                        str(root / "out"),
                    ]
                )

        self.assertEqual(rc, 2)

    def test_cli_strict_fixed_schedule_rejects_unscheduled_planned_cases(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            for name in ["a", "b", "new-case"]:
                runner = root / "scripts" / "presmoke" / "cases" / name / "run.sh"
                runner.parent.mkdir(parents=True, exist_ok=True)
                runner.write_text("#!/usr/bin/env bash\nexit 0\n", encoding="utf-8")
            manifest = root / "scripts" / "presmoke" / "reports" / "case_runner_manifest.json"
            manifest.parent.mkdir(parents=True, exist_ok=True)
            manifest.write_text(
                json.dumps(
                    [
                        {"case": "a", "supported_archs": ["dav-2201"], "supported_modes": ["npu"]},
                        {"case": "b", "supported_archs": ["dav-2201"], "supported_modes": ["npu"]},
                        {"case": "new-case", "supported_archs": ["dav-2201"], "supported_modes": ["npu"]},
                    ]
                ),
                encoding="utf-8",
            )
            schedule_file = root / "scripts" / "presmoke" / "schedules" / "dav-2201_npu.txt"
            schedule_file.parent.mkdir(parents=True, exist_ok=True)
            schedule_file.write_text("a\nb\n", encoding="utf-8")

            with mock.patch.dict(os.environ, {"PRESMOKE_PROJECT_ROOT": str(root)}):
                rc = main(
                    [
                        "--arch",
                        "dav-2201",
                        "--modes",
                        "npu",
                        "--dry-run",
                        "--schedule",
                        "fixed",
                        "--strict-schedule",
                        "--report-format",
                        "json",
                        "--results",
                        str(root / "out"),
                    ]
                )

        self.assertEqual(rc, 2)

    def test_cli_fixed_schedule_falls_back_when_builtin_cpu_schedule_is_missing(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            for name in ["a", "b"]:
                runner = root / "scripts" / "presmoke" / "cases" / name / "run.sh"
                runner.parent.mkdir(parents=True, exist_ok=True)
                runner.write_text("#!/usr/bin/env bash\nexit 0\n", encoding="utf-8")
            manifest = root / "scripts" / "presmoke" / "reports" / "case_runner_manifest.json"
            manifest.parent.mkdir(parents=True, exist_ok=True)
            manifest.write_text(
                json.dumps(
                    [
                        {"case": "a", "supported_archs": ["dav-2201"], "supported_modes": ["cpu"]},
                        {"case": "b", "supported_archs": ["dav-2201"], "supported_modes": ["cpu"]},
                    ]
                ),
                encoding="utf-8",
            )
            results_dir = root / "out"

            with mock.patch.dict(os.environ, {"PRESMOKE_PROJECT_ROOT": str(root)}):
                rc = main(
                    [
                        "--arch",
                        "dav-2201",
                        "--modes",
                        "cpu",
                        "--dry-run",
                        "--schedule",
                        "fixed",
                        "--report-format",
                        "json",
                        "--results",
                        str(results_dir),
                    ]
                )

            payload = json.loads((results_dir / "report.json").read_text(encoding="utf-8"))

        self.assertEqual(rc, 0)
        self.assertEqual([item["example"] for item in payload["results"]], ["a", "b"])

    def test_builtin_910b_npu_schedule_matches_manifest(self) -> None:
        project_root = Path(__file__).resolve().parents[3]
        manifest_path = project_root / "scripts" / "presmoke" / "reports" / "case_runner_manifest.json"
        schedule_path = project_root / "scripts" / "presmoke" / "schedules" / "dav-2201_npu.txt"
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        planned = {
            item["case"]
            for item in manifest
            if item.get("target_runnable", True)
            and "dav-2201" in item.get("supported_archs", [])
            and "npu" in item.get("supported_modes", [])
        }
        scheduled = {
            line.strip()
            for line in schedule_path.read_text(encoding="utf-8").splitlines()
            if line.strip() and not line.lstrip().startswith("#")
        }

        self.assertEqual(scheduled - planned, set())
        self.assertEqual(planned - scheduled, set())
        missing_runners = [
            name
            for name in scheduled
            if not (project_root / "scripts" / "presmoke" / "cases" / name / "run.sh").is_file()
        ]
        self.assertEqual(missing_runners, [])

    def test_cli_schedule_file_overrides_builtin_schedule_file(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            for name in ["a", "b", "c"]:
                runner = root / "scripts" / "presmoke" / "cases" / name / "run.sh"
                runner.parent.mkdir(parents=True, exist_ok=True)
                runner.write_text("#!/usr/bin/env bash\nexit 0\n", encoding="utf-8")
            manifest = root / "scripts" / "presmoke" / "reports" / "case_runner_manifest.json"
            manifest.parent.mkdir(parents=True, exist_ok=True)
            manifest.write_text(
                json.dumps(
                    [
                        {"case": "a", "supported_archs": ["dav-2201"], "supported_modes": ["npu"]},
                        {"case": "b", "supported_archs": ["dav-2201"], "supported_modes": ["npu"]},
                        {"case": "c", "supported_archs": ["dav-2201"], "supported_modes": ["npu"]},
                    ]
                ),
                encoding="utf-8",
            )
            builtin = root / "scripts" / "presmoke" / "schedules" / "dav-2201_npu.txt"
            builtin.parent.mkdir(parents=True, exist_ok=True)
            builtin.write_text("a\nb\n", encoding="utf-8")
            explicit = root / "explicit.txt"
            explicit.write_text("c\nb\n", encoding="utf-8")
            results_dir = root / "out"

            with mock.patch.dict(os.environ, {"PRESMOKE_PROJECT_ROOT": str(root)}):
                rc = main(
                    [
                        "--arch",
                        "dav-2201",
                        "--modes",
                        "npu",
                        "--dry-run",
                        "--schedule",
                        "fixed",
                        "--schedule-file",
                        str(explicit),
                        "--report-format",
                        "json",
                        "--results",
                        str(results_dir),
                    ]
                )

            payload = json.loads((results_dir / "report.json").read_text(encoding="utf-8"))

        self.assertEqual(rc, 0)
        self.assertEqual([item["example"] for item in payload["results"]], ["c", "b", "a"])

    def test_cli_export_schedule_writes_current_order(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            for name in ["a", "b", "c"]:
                runner = root / "scripts" / "presmoke" / "cases" / name / "run.sh"
                runner.parent.mkdir(parents=True, exist_ok=True)
                runner.write_text("#!/usr/bin/env bash\nexit 0\n", encoding="utf-8")
            manifest = root / "scripts" / "presmoke" / "reports" / "case_runner_manifest.json"
            manifest.parent.mkdir(parents=True, exist_ok=True)
            manifest.write_text(
                json.dumps(
                    [
                        {"case": "a", "supported_archs": ["dav-2201"], "supported_modes": ["npu"]},
                        {"case": "b", "supported_archs": ["dav-2201"], "supported_modes": ["npu"]},
                        {"case": "c", "supported_archs": ["dav-2201"], "supported_modes": ["npu"]},
                    ]
                ),
                encoding="utf-8",
            )
            schedule_file = root / "schedule.txt"
            schedule_file.write_text("c\na\n", encoding="utf-8")
            export_path = root / "exported.txt"

            with mock.patch.dict(os.environ, {"PRESMOKE_PROJECT_ROOT": str(root)}):
                rc = main(
                    [
                        "--arch",
                        "dav-2201",
                        "--modes",
                        "npu",
                        "--dry-run",
                        "--schedule",
                        "fixed",
                        "--schedule-file",
                        str(schedule_file),
                        "--export-schedule",
                        str(export_path),
                        "--report-format",
                        "json",
                        "--results",
                        str(root / "out"),
                    ]
                )
            exported_lines = export_path.read_text(encoding="utf-8").splitlines()

        self.assertEqual(rc, 0)
        self.assertEqual(exported_lines, ["c", "a", "b"])

    def test_npu_idle_min_schedule_is_not_worse_than_build_desc_after_frontload(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            report = root / "report.json"
            write_timing_report(
                report, {"slow-a": (100, 1), "slow-b": (90, 1), "fast-a": (1, 30), "fast-b": (1, 30), "fast-c": (1, 30)}
            )
            cells = [cell(root, name) for name in ["slow-a", "slow-b", "fast-a", "fast-b", "fast-c"]]

            build_desc = schedule_cells(cells, ScheduleOptions(schedule="build-desc", schedule_report=report, jobs=2))
            idle_min = schedule_cells(cells, ScheduleOptions(schedule="npu-idle-min", schedule_report=report, jobs=2))

            idle_min_s = simulate_pipeline_npu_idle(idle_min, report, jobs=2)
            build_desc_idle_s = simulate_pipeline_npu_idle(build_desc, report, jobs=2)
            idle_min_makespan_s = simulate_pipeline_makespan(idle_min, report, jobs=2)
            build_desc_makespan_s = simulate_pipeline_makespan(build_desc, report, jobs=2)

        self.assertLessEqual(idle_min_s, build_desc_idle_s)
        self.assertLessEqual(idle_min_makespan_s, build_desc_makespan_s)

    def test_npu_idle_min_schedule_depends_on_jobs(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            report = root / "report.json"
            write_timing_report(
                report,
                {"c0": (100, 10), "c1": (100, 10), "c2": (1, 5), "c3": (20, 10), "c4": (10, 100), "c5": (100, 5)},
            )
            cells = [cell(root, name) for name in ["c0", "c1", "c2", "c3", "c4", "c5"]]

            one_job = schedule_cells(cells, ScheduleOptions(schedule="npu-idle-min", schedule_report=report, jobs=1))
            two_jobs = schedule_cells(cells, ScheduleOptions(schedule="npu-idle-min", schedule_report=report, jobs=2))

        self.assertNotEqual([item.example.rel_path for item in one_job], [item.example.rel_path for item in two_jobs])

    def test_cli_accepts_npu_idle_min_schedule_before_dry_run(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            for name in ["slow-a", "fast-a", "fast-b"]:
                runner = root / "scripts" / "presmoke" / "cases" / name / "run.sh"
                runner.parent.mkdir(parents=True, exist_ok=True)
                runner.write_text("#!/usr/bin/env bash\nexit 0\n", encoding="utf-8")
            manifest = root / "scripts" / "presmoke" / "reports" / "case_runner_manifest.json"
            manifest.parent.mkdir(parents=True, exist_ok=True)
            manifest.write_text(
                json.dumps(
                    [
                        {"case": "slow-a", "supported_archs": ["dav-2201"], "supported_modes": ["npu"]},
                        {"case": "fast-a", "supported_archs": ["dav-2201"], "supported_modes": ["npu"]},
                        {"case": "fast-b", "supported_archs": ["dav-2201"], "supported_modes": ["npu"]},
                    ]
                ),
                encoding="utf-8",
            )
            schedule_report = root / "schedule_report.json"
            write_timing_report(schedule_report, {"slow-a": (100, 1), "fast-a": (1, 30), "fast-b": (1, 20)})
            results_dir = root / "out"

            with mock.patch.dict(os.environ, {"PRESMOKE_PROJECT_ROOT": str(root)}):
                rc = main(
                    [
                        "--arch",
                        "dav-2201",
                        "--modes",
                        "npu",
                        "--dry-run",
                        "--schedule",
                        "npu-idle-min",
                        "--schedule-report",
                        str(schedule_report),
                        "--jobs",
                        "2",
                        "--report-format",
                        "json",
                        "--results",
                        str(results_dir),
                    ]
                )

            payload = json.loads((results_dir / "report.json").read_text(encoding="utf-8"))

        self.assertEqual(rc, 0)
        self.assertEqual([item["example"] for item in payload["results"]], ["slow-a", "fast-a", "fast-b"])

    def test_auto_jobs_scales_with_available_cpu_count(self) -> None:
        self.assertEqual(resolve_jobs("auto", cpu_count=192), 12)
        self.assertEqual(resolve_jobs("auto", cpu_count=88), 8)
        self.assertEqual(resolve_jobs("auto", cpu_count=40), 6)
        self.assertEqual(resolve_jobs("auto", cpu_count=16), 4)
        self.assertEqual(resolve_jobs("7", cpu_count=192), 7)

    def test_cpu_auto_jobs_use_generic_cpu_count_formula(self) -> None:
        self.assertEqual(resolve_cpu_build_jobs(192), 48)
        self.assertEqual(resolve_cpu_build_jobs(88), 22)
        self.assertEqual(resolve_cpu_build_jobs(40), 10)
        self.assertEqual(resolve_cpu_build_jobs(16), 4)
        self.assertEqual(resolve_cpu_build_jobs(320), 64)
        self.assertEqual(resolve_jobs("auto", cpu_count=192, modes=["cpu"]), 48)
        self.assertEqual(resolve_jobs("auto", cpu_count=88, modes=["cpu"]), 22)

    def test_cpu_parallel_defaults_use_host_cpu_count(self) -> None:
        self.assertEqual(resolve_cpu_run_slots("auto", cpu_count=192), 192)
        self.assertEqual(resolve_cpu_run_slots("5", cpu_count=192), 5)
        self.assertEqual(resolve_make_jobs("auto", build_jobs=48, cpu_count=192), 4)
        self.assertEqual(resolve_make_jobs("auto", build_jobs=22, cpu_count=88), 4)
        self.assertEqual(resolve_make_jobs("3", build_jobs=12, cpu_count=192), 3)

    def test_cli_report_includes_resolved_parallel_config(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            project = root / "project"
            runner = project / "scripts/presmoke/cases/cpu-case/run.sh"
            runner.parent.mkdir(parents=True)
            runner.write_text("#!/usr/bin/env bash\n", encoding="utf-8")
            manifest = project / "scripts/presmoke/reports/case_runner_manifest.json"
            manifest.parent.mkdir(parents=True)
            manifest.write_text(
                '[{"case":"cpu-case","supported_archs":["dav-2201"],"supported_modes":["cpu"]}]', encoding="utf-8"
            )
            results_dir = root / "results"
            with mock.patch.dict(os.environ, {"PRESMOKE_PROJECT_ROOT": str(project)}):
                with mock.patch("presmoke.cli.detect_cpu_count", return_value=88):
                    rc = main(
                        [
                            "--arch",
                            "dav-2201",
                            "--modes",
                            "cpu",
                            "--dry-run",
                            "--report-format",
                            "json",
                            "--results",
                            str(results_dir),
                            "--jobs",
                            "auto",
                            "--cpu-run-slots",
                            "auto",
                            "--make-jobs",
                            "auto",
                        ]
                    )
            payload = json.loads((results_dir / "report.json").read_text(encoding="utf-8"))

        self.assertEqual(rc, 0)
        self.assertEqual(
            payload["parallel_config"],
            {"jobs": 22, "npu_slots": 1, "cpu_run_slots": 88, "make_jobs": 4, "cpu_run_timeout": 300, "werror": 0},
        )

    def test_detect_cpu_count_prefers_lscpu(self) -> None:
        with mock.patch("presmoke.cli.shutil.which", return_value="/usr/bin/lscpu"):
            with mock.patch("presmoke.cli.subprocess.run") as run:
                run.return_value.stdout = "CPU(s):                             192\n"
                run.return_value.returncode = 0

                self.assertEqual(detect_cpu_count(), 192)

        run.assert_called_once()

    def test_npu_idle_min_does_not_frontload_long_builds_when_it_increases_idle(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            report = root / "report.json"
            timings = {
                "slow-a": (500, 1),
                "slow-b": (130, 1),
                "slow-c": (120, 1),
                "slow-d": (110, 1),
                "fast-a": (1, 200),
                "fast-b": (1, 100),
                "fast-c": (1, 80),
                "fast-d": (1, 60),
            }
            write_timing_report(report, timings)
            cells = [
                cell(root, name)
                for name in ["fast-a", "fast-b", "fast-c", "fast-d", "slow-a", "slow-b", "slow-c", "slow-d"]
            ]

            scheduled = schedule_cells(cells, ScheduleOptions(schedule="npu-idle-min", schedule_report=report, jobs=4))
            scheduled_idle = simulate_pipeline_npu_idle(scheduled, report, jobs=4)

        names = [item.example.rel_path for item in scheduled]
        self.assertLessEqual(scheduled_idle, 58.0)
        self.assertLess(names.index("fast-a"), names.index("slow-b"))

    def test_npu_idle_min_prefers_lower_makespan_after_idle(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            report = root / "report.json"
            write_timing_report(
                report, {"long-build": (100, 1), "long-run-a": (1, 80), "long-run-b": (1, 70), "short-run": (1, 1)}
            )
            cells = [cell(root, name) for name in ["short-run", "long-run-a", "long-build", "long-run-b"]]

            scheduled = schedule_cells(cells, ScheduleOptions(schedule="npu-idle-min", schedule_report=report, jobs=3))
            build_desc = schedule_cells(cells, ScheduleOptions(schedule="build-desc", schedule_report=report, jobs=3))

            scheduled_idle = simulate_pipeline_npu_idle(scheduled, report, jobs=3)
            build_desc_idle = simulate_pipeline_npu_idle(build_desc, report, jobs=3)
            scheduled_makespan = simulate_pipeline_makespan(scheduled, report, jobs=3)
            build_desc_makespan = simulate_pipeline_makespan(build_desc, report, jobs=3)

        self.assertLessEqual(scheduled_idle, build_desc_idle)
        self.assertLessEqual(scheduled_makespan, build_desc_makespan)

    def test_npu_idle_min_accepts_tsv_stage_timing_report(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            report = root / "timings.tsv"
            report.write_text(
                "\n".join(
                    ["example\tbuild_s\trun_s\tverify_s\tduration_s", "long-verify\t1\t1\t20\t22", "short\t1\t1\t0\t2"]
                )
                + "\n",
                encoding="utf-8",
            )
            cells = [cell(root, "short"), cell(root, "long-verify")]

            scheduled = schedule_cells(cells, ScheduleOptions(schedule="npu-idle-min", schedule_report=report, jobs=2))

            self.assertEqual([item.example.rel_path for item in scheduled][0], "long-verify")
            self.assertEqual(simulate_pipeline_npu_idle(scheduled, report, jobs=2), 20.0)


if __name__ == "__main__":
    unittest.main()
