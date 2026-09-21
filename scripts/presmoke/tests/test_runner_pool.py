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

import contextlib
import io
import tempfile
import threading
import time
import unittest
from pathlib import Path

from presmoke.case_runners import CaseRunnerOptions, build_case_runner_cells_with_skips
from presmoke.cli import parse_args
from presmoke.model import Cell, Command, ExampleSpec, RunReport
from presmoke.report import write_json, write_markdown
from presmoke.runner import NpuSlotPool, PipelineOptions, RunOptions
from presmoke.runner import run_cell_with_options
from presmoke.runner import run_cells_pipeline_with_options


def cell(tmp: Path, name: str, mode: str = "npu", commands=None) -> Cell:
    spec = ExampleSpec(tmp / name, name, commands or [], ["dav-2201"], [mode], "test")
    spec.path.mkdir(parents=True, exist_ok=True)
    return Cell(spec, "dav-2201", mode, commands or [], spec.path / f"build_{mode}")


def source_cell(tmp: Path, source: str, name: str, mode: str = "npu", commands=None) -> Cell:
    source_path = tmp / source
    source_path.mkdir(parents=True, exist_ok=True)
    spec = ExampleSpec(source_path, name, commands or [], ["dav-2201"], [mode], "case-runner")
    return Cell(spec, "dav-2201", mode, commands or [], source_path / f"build_{mode}")


class RunnerPoolTest(unittest.TestCase):
    def test_custom_op_starts_after_static_lib_finishes(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            static_finished = root / "static.finished"
            static_lib = cell(
                root,
                "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op_static_lib",
                commands=[Command(":", "build"), Command(":", "run"), Command(f"touch {static_finished}", "verify")],
            )
            custom_op = cell(
                root,
                "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op",
                commands=[Command(f"test -f {static_finished}", "build"), Command(":", "run"), Command(":", "verify")],
            )

            run = run_cells_pipeline_with_options(
                [static_lib, custom_op],
                PipelineOptions(root / "logs", timeout=1, keep_artifacts=True, jobs=2, npu_slots=1),
            )

        by_name = {result.example: result for result in run.results}
        self.assertEqual(by_name[static_lib.example.rel_path].status, "PASS")
        self.assertEqual(by_name[custom_op.example.rel_path].status, "PASS")

    def test_custom_op_dependents_start_after_custom_op_finishes(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            custom_finished = root / "custom.finished"
            custom_op = cell(
                root,
                "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op",
                commands=[Command(":", "build"), Command(":", "run"), Command(f"touch {custom_finished}", "verify")],
            )
            aclnn = cell(
                root,
                "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclnn_invocation",
                commands=[Command(f"test -f {custom_finished}", "build"), Command(":", "run"), Command(":", "verify")],
            )

            run = run_cells_pipeline_with_options(
                [custom_op, aclnn], PipelineOptions(root / "logs", timeout=1, keep_artifacts=True, jobs=2, npu_slots=1)
            )

        by_name = {result.example: result for result in run.results}
        self.assertEqual(by_name[custom_op.example.rel_path].status, "PASS")
        self.assertEqual(by_name[aclnn.example.rel_path].status, "PASS")

    def test_custom_op_failure_skips_dependents_without_running_them(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            dependent_started = root / "dependent.started"
            custom_op = cell(
                root,
                "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op",
                commands=[Command("false", "build")],
            )
            tiling_sink = cell(
                root,
                "04_aicpu/02_features/00_framework/00_pytorch/tiling_sink_programming",
                commands=[Command(f"touch {dependent_started}", "build")],
            )

            run = run_cells_pipeline_with_options(
                [custom_op, tiling_sink],
                PipelineOptions(root / "logs", timeout=1, keep_artifacts=True, jobs=2, npu_slots=1),
            )
            dependent_was_started = dependent_started.exists()

        by_name = {result.example: result for result in run.results}
        self.assertEqual(by_name[custom_op.example.rel_path].status, "FAIL")
        self.assertEqual(by_name[tiling_sink.example.rel_path].status, "SKIP")
        self.assertIn("prerequisite failed", by_name[tiling_sink.example.rel_path].reason)
        self.assertFalse(dependent_was_started)

    def test_parallel_package_runs_after_failed_dependents_are_skipped(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            parallel_started = root / "parallel.started"
            custom_op = cell(
                root,
                "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op",
                commands=[Command("false", "build")],
            )
            aclnn = cell(
                root,
                "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclnn_invocation",
                commands=[Command("false", "build")],
            )
            parallel_ops = cell(
                root,
                "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/parallel_ops_package",
                commands=[Command(":", "build"), Command(f"touch {parallel_started}", "run")],
            )

            run = run_cells_pipeline_with_options(
                [parallel_ops, aclnn, custom_op],
                PipelineOptions(root / "logs", timeout=1, keep_artifacts=True, jobs=3, npu_slots=1),
            )
            parallel_was_started = parallel_started.exists()

        by_name = {result.example: result for result in run.results}
        self.assertEqual(by_name[custom_op.example.rel_path].status, "FAIL")
        self.assertEqual(by_name[aclnn.example.rel_path].status, "SKIP")
        self.assertEqual(by_name[parallel_ops.example.rel_path].status, "PASS")
        self.assertTrue(parallel_was_started)

    def test_parallel_ops_package_waits_only_for_custom_op_dependents(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            dependent_finished = root / "dependent.finished"
            parallel_started = root / "parallel.started"
            unrelated_finished = root / "unrelated.finished"
            dependent = cell(
                root,
                "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclnn_invocation",
                commands=[Command(":", "build"), Command(":", "run"), Command(f"touch {dependent_finished}", "verify")],
            )
            unrelated = cell(
                root,
                "unrelated",
                commands=[
                    Command(":", "build"),
                    Command(":", "run"),
                    Command(
                        f"for i in $(seq 1 50); do [ -f {parallel_started} ] && "
                        f"touch {unrelated_finished} && exit 0; sleep 0.02; done; exit 7",
                        "verify",
                    ),
                ],
            )
            parallel_ops = cell(
                root,
                "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/parallel_ops_package",
                commands=[
                    Command(":", "build"),
                    Command(
                        f"test -f {dependent_finished}; test ! -f {unrelated_finished}; touch {parallel_started}", "run"
                    ),
                    Command(":", "verify"),
                ],
            )

            run = run_cells_pipeline_with_options(
                [parallel_ops, dependent, unrelated],
                PipelineOptions(root / "logs", timeout=1, keep_artifacts=True, jobs=2, npu_slots=1),
            )

        by_name = {result.example: result for result in run.results}
        self.assertEqual(by_name[dependent.example.rel_path].status, "PASS")
        self.assertEqual(by_name[unrelated.example.rel_path].status, "PASS")
        self.assertEqual(by_name[parallel_ops.example.rel_path].status, "PASS")

    def test_parallel_ops_package_waits_for_custom_op_when_dependents_are_absent(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            custom_finished = root / "custom.finished"
            custom_op = cell(
                root,
                "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op",
                commands=[Command(":", "build"), Command(":", "run"), Command(f"touch {custom_finished}", "verify")],
            )
            parallel_ops = cell(
                root,
                "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/parallel_ops_package",
                commands=[Command(":", "build"), Command(f"test -f {custom_finished}", "run"), Command(":", "verify")],
            )

            run = run_cells_pipeline_with_options(
                [parallel_ops, custom_op],
                PipelineOptions(root / "logs", timeout=1, keep_artifacts=True, jobs=2, npu_slots=1),
            )

        self.assertEqual([result.status for result in run.results], ["PASS", "PASS"])

    def test_cli_default_timeout_is_short_for_presmoke(self) -> None:
        args = parse_args([])

        self.assertEqual(args.timeout, 120)
        self.assertEqual(args.cpu_run_timeout, "300")

    def test_cli_accepts_build_only_stage(self) -> None:
        self.assertEqual(parse_args([]).stages, "all")
        self.assertEqual(parse_args(["--stages", "build"]).stages, "build")

    def test_removed_cli_options_are_rejected(self) -> None:
        removed_options = [
            ["--preflight"],
            ["--baseline"],
            ["--update-baseline"],
            ["--allowlist", "x"],
            ["--changed-only"],
            ["--since", "HEAD~1"],
            ["--host-timeout", "1"],
            ["--examples-root", "examples"],
            ["--runner-mode", "readme"],
            ["--suggestions-only"],
        ]

        for option in removed_options:
            with self.subTest(option=option):
                with contextlib.redirect_stderr(io.StringIO()):
                    with self.assertRaises(SystemExit):
                        parse_args(option)

    def test_runner_timeout(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            c = cell(Path(tmp), "x", commands=[Command("sleep 2", "run")])
            result = run_cell_with_options(c, RunOptions(Path(tmp) / "logs", timeout=1, keep_artifacts=True))
        self.assertEqual(result.status, "FAIL")
        self.assertEqual(result.rc, 124)

    def test_host_stages_do_not_timeout(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            c = cell(
                Path(tmp),
                "x",
                commands=[Command("sleep 1.2", "build"), Command(":", "run"), Command("sleep 1.2", "verify")],
            )
            result = run_cell_with_options(c, RunOptions(Path(tmp) / "logs", timeout=1, keep_artifacts=True))

        self.assertEqual(result.status, "PASS")
        self.assertEqual([step.rc for step in result.steps], [0, 0, 0])

    def test_build_only_pipeline_skips_run_and_verify_and_keeps_artifacts(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            c = source_cell(
                root,
                "source",
                "build-only",
                commands=[
                    Command("touch build.done", "build"),
                    Command("touch run.done", "run"),
                    Command("touch verify.done", "verify"),
                ],
            )

            result = run_cells_pipeline_with_options(
                [c], PipelineOptions(root / "logs", timeout=1, jobs=2, stages="build")
            ).results[0]

            self.assertEqual(result.status, "PASS")
            self.assertEqual([step.kind for step in result.steps], ["build"])
            self.assertTrue((c.example.path / "build.done").exists())
            self.assertFalse((c.example.path / "run.done").exists())
            self.assertFalse((c.example.path / "verify.done").exists())

    def test_cpu_run_stage_uses_timeout(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            c = cell(Path(tmp), "x", mode="cpu", commands=[Command("sleep 2", "run")])
            result = run_cell_with_options(c, RunOptions(Path(tmp) / "logs", timeout=1, keep_artifacts=True))

        self.assertEqual(result.status, "PASS")
        self.assertEqual(result.rc, 0)

    def test_cpu_run_timeout_can_be_overridden(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            c = cell(Path(tmp), "x", mode="cpu", commands=[Command("sleep 2", "run")])
            result = run_cell_with_options(
                c, RunOptions(Path(tmp) / "logs", timeout=1, cpu_run_timeout=1, keep_artifacts=True)
            )

        self.assertEqual(result.status, "FAIL")
        self.assertEqual(result.rc, 124)

    def test_npu_gate_limits_concurrency(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            active = 0
            max_active = 0
            lock = threading.Lock()
            pool = NpuSlotPool(2)

            def work(c):
                nonlocal active, max_active

                def gated() -> int:
                    nonlocal active, max_active
                    with lock:
                        active += 1
                        max_active = max(max_active, active)
                    time.sleep(0.05)
                    with lock:
                        active -= 1
                    return 0

                pool.gate(gated)
                return c.key

            cells = [cell(Path(tmp), f"x{i}") for i in range(5)]
            threads = [threading.Thread(target=work, args=(c,)) for c in cells]
            for thread in threads:
                thread.start()
            for thread in threads:
                thread.join()
        self.assertLessEqual(max_active, 2)

    def test_manifest_skip_does_not_create_cell(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            runner = root / "scripts/presmoke/cases/x/run.sh"
            runner.parent.mkdir(parents=True)
            runner.write_text("#!/usr/bin/env bash\n", encoding="utf-8")
            manifest = root / "scripts/presmoke/reports/case_runner_manifest.json"
            manifest.parent.mkdir(parents=True)
            manifest.write_text(
                '[{"case":"x","skip":true,"skip_reason":"needs TensorFlow 2.6.5","supported_archs":["dav-2201"],"supported_modes":["npu"]}]',
                encoding="utf-8",
            )
            cells, suggestions, skipped = build_case_runner_cells_with_skips(
                root, CaseRunnerOptions(arch="dav-2201", modes=["npu"], includes=[], excludes=[])
            )
        self.assertEqual(cells, [])
        self.assertEqual(len(skipped), 1)
        self.assertEqual(skipped[0].status, "SKIP")
        self.assertIn("TensorFlow 2.6.5", skipped[0].reason)
        self.assertEqual(len(suggestions), 1)

    def test_unsupported_arch_is_excluded_not_counted_as_skip(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            runner = root / "scripts/presmoke/cases/x/run.sh"
            runner.parent.mkdir(parents=True)
            runner.write_text("#!/usr/bin/env bash\n", encoding="utf-8")
            manifest = root / "scripts/presmoke/reports/case_runner_manifest.json"
            manifest.parent.mkdir(parents=True)
            manifest.write_text(
                '[{"case":"x","skip":false,"supported_archs":["dav-3510"],"supported_modes":["npu"]}]', encoding="utf-8"
            )
            cells, suggestions, skipped = build_case_runner_cells_with_skips(
                root, CaseRunnerOptions(arch="dav-2201", modes=["npu"], includes=[], excludes=[])
            )
        self.assertEqual(cells, [])
        self.assertEqual(skipped, [])
        self.assertEqual(len(suggestions), 1)
        self.assertIn("Excluded on unsupported arch dav-2201", suggestions[0].message)

    def test_exact_filter_matches_only_full_case_path(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            for case in ["01/add", "01/add_tpipe_tque"]:
                runner = root / "scripts/presmoke/cases" / case / "run.sh"
                runner.parent.mkdir(parents=True)
                runner.write_text("#!/usr/bin/env bash\n", encoding="utf-8")
            manifest = root / "scripts/presmoke/reports/case_runner_manifest.json"
            manifest.parent.mkdir(parents=True)
            manifest.write_text(
                """[
                    {"case":"01/add","skip":false,"supported_archs":["dav-2201"],"supported_modes":["npu"]},
                    {"case":"01/add_tpipe_tque","skip":false,"supported_archs":["dav-2201"],"supported_modes":["npu"]}
                ]""",
                encoding="utf-8",
            )

            cells, _, _ = build_case_runner_cells_with_skips(
                root,
                CaseRunnerOptions(arch="dav-2201", modes=["npu"], includes=[], excludes=[], exact_includes=["01/add"]),
            )

        self.assertEqual([item.example.rel_path for item in cells], ["01/add"])

    def test_scenario_case_runner_uses_source_case_path(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            case = "01/add__scenario_2"
            source_case = "01/add"
            runner = root / "scripts/presmoke/cases" / case / "run.sh"
            runner.parent.mkdir(parents=True)
            runner.write_text("#!/usr/bin/env bash\n", encoding="utf-8")
            manifest = root / "scripts/presmoke/reports/case_runner_manifest.json"
            manifest.parent.mkdir(parents=True)
            manifest.write_text(
                """[
                    {
                        "case":"01/add__scenario_2",
                        "source_case":"01/add",
                        "skip":false,
                        "supported_archs":["dav-2201"],
                        "supported_modes":["npu"]
                    }
                ]""",
                encoding="utf-8",
            )

            cells, _, _ = build_case_runner_cells_with_skips(
                root, CaseRunnerOptions(arch="dav-2201", modes=["npu"], includes=[], excludes=[])
            )

        self.assertEqual(cells[0].example.rel_path, case)
        self.assertEqual(cells[0].example.path, root / "examples" / source_case)

    def test_case_runner_cell_starts_with_clean_step(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            runner = root / "scripts/presmoke/cases/x/run.sh"
            runner.parent.mkdir(parents=True)
            runner.write_text("#!/usr/bin/env bash\n", encoding="utf-8")
            manifest = root / "scripts/presmoke/reports/case_runner_manifest.json"
            manifest.parent.mkdir(parents=True)
            manifest.write_text(
                '[{"case":"x","skip":false,"supported_archs":["dav-2201"],"supported_modes":["npu"]}]', encoding="utf-8"
            )

            cells, _, _ = build_case_runner_cells_with_skips(
                root, CaseRunnerOptions(arch="dav-2201", modes=["npu"], includes=[], excludes=[])
            )

        self.assertEqual([command.kind for command in cells[0].commands], ["clean", "build", "run", "verify"])
        self.assertTrue(cells[0].commands[0].raw.endswith("run.sh clean"))

    def test_single_worker_run_removes_stale_build_dir_before_commands(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            c = cell(root, "stale", commands=[Command("test ! -e build_npu/stale", "build")])
            c.build_dir.mkdir()
            (c.build_dir / "stale").write_text("old", encoding="utf-8")

            result = run_cell_with_options(c, RunOptions(Path(tmp) / "logs", timeout=1, keep_artifacts=True))

        self.assertEqual(result.status, "PASS")

    def test_custom_op_provider_build_dir_is_preserved_for_dependent_cases(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            c = cell(
                root,
                "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op",
                commands=[
                    Command("mkdir -p build_npu && touch build_npu/custom_opp_test.run", "build"),
                    Command(":", "run"),
                    Command(":", "verify"),
                ],
            )

            result = run_cell_with_options(c, RunOptions(root / "logs", timeout=1, keep_artifacts=False))

            self.assertEqual(result.status, "PASS")
            self.assertTrue((c.build_dir / "custom_opp_test.run").exists())

    def test_pipeline_cleans_case_runner_artifacts_after_success(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            c = source_cell(
                root,
                "case",
                "case",
                commands=[
                    Command("rm -rf build_npu", "clean"),
                    Command("mkdir -p build_npu && touch build_npu/artifact", "build"),
                    Command(":", "run"),
                    Command(":", "verify"),
                ],
            )

            result = run_cells_pipeline_with_options(
                [c], PipelineOptions(root / "logs", timeout=1, keep_artifacts=False, jobs=1, npu_slots=1)
            )

            self.assertEqual(result.results[0].status, "PASS")
            self.assertFalse(c.build_dir.exists())

    def test_pipeline_cleans_case_runner_artifacts_after_failure(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            c = source_cell(
                root,
                "case",
                "case",
                commands=[
                    Command("rm -rf build_npu", "clean"),
                    Command("mkdir -p build_npu && touch build_npu/artifact", "build"),
                    Command("false", "run"),
                    Command(":", "verify"),
                ],
            )

            result = run_cells_pipeline_with_options(
                [c], PipelineOptions(root / "logs", timeout=1, keep_artifacts=False, jobs=1, npu_slots=1)
            )

            self.assertEqual(result.results[0].status, "FAIL")
            self.assertFalse(c.build_dir.exists())

    def test_pipeline_keeps_build_workers_running_while_npu_slot_is_busy(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            marker_dir = root / "markers"
            marker_dir.mkdir()
            cells = [
                cell(
                    root,
                    "slow-run",
                    commands=[
                        Command(":", "build"),
                        Command(f"while [ ! -f {marker_dir / 'third_build_started'} ]; do sleep 0.02; done", "run"),
                        Command(":", "verify"),
                    ],
                ),
                cell(
                    root,
                    "npu-queued",
                    commands=[Command(":", "build"), Command("sleep 0.05", "run"), Command(":", "verify")],
                ),
                cell(
                    root,
                    "third-build",
                    commands=[
                        Command(f"touch {marker_dir / 'third_build_started'}", "build"),
                        Command(":", "run"),
                        Command(":", "verify"),
                    ],
                ),
            ]
            run = run_cells_pipeline_with_options(
                cells, PipelineOptions(Path(tmp) / "logs", timeout=2, keep_artifacts=True, jobs=2, npu_slots=1)
            )
            results = run.results

        by_name = {result.example: result for result in results}
        self.assertEqual(by_name["slow-run"].status, "PASS")
        self.assertEqual(by_name["npu-queued"].status, "PASS")
        self.assertEqual(by_name["third-build"].status, "PASS")

    def test_pipeline_allows_same_source_scenarios_to_build_in_parallel(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            first_started = root / "first.started"
            second_started = root / "second.started"
            cells = [
                source_cell(
                    root,
                    "shared-source",
                    "shared-source__scenario_1",
                    commands=[
                        Command(
                            f"touch {first_started}; for i in $(seq 1 50); do "
                            f"[ -f {second_started} ] && exit 0; sleep 0.02; done; exit 17",
                            "build",
                        ),
                        Command(":", "run"),
                        Command(":", "verify"),
                    ],
                ),
                source_cell(
                    root,
                    "shared-source",
                    "shared-source__scenario_2",
                    commands=[
                        Command(
                            f"touch {second_started}; for i in $(seq 1 50); do "
                            f"[ -f {first_started} ] && exit 0; sleep 0.02; done; exit 17",
                            "build",
                        ),
                        Command(":", "run"),
                        Command(":", "verify"),
                    ],
                ),
            ]

            run = run_cells_pipeline_with_options(
                cells, PipelineOptions(root / "logs", timeout=1, keep_artifacts=True, jobs=2, npu_slots=1)
            )

        self.assertEqual([result.status for result in run.results], ["PASS", "PASS"])

    def test_pipeline_does_not_block_next_npu_run_on_verify(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            marker = root / "second_run_started"
            cells = [
                cell(
                    root,
                    "verify-waits-for-next-run",
                    commands=[
                        Command(":", "build"),
                        Command("sleep 0.01", "run"),
                        Command(f"while [ ! -f {marker} ]; do sleep 0.02; done", "verify"),
                    ],
                ),
                cell(
                    root,
                    "touches-verify-marker",
                    commands=[
                        Command("sleep 0.05", "build"),
                        Command(f"touch {marker}", "run"),
                        Command(":", "verify"),
                    ],
                ),
            ]

            run = run_cells_pipeline_with_options(
                cells, PipelineOptions(Path(tmp) / "logs", timeout=1, keep_artifacts=True, jobs=2, npu_slots=1)
            )
            results = run.results

        by_name = {result.example: result for result in results}
        self.assertEqual(by_name["verify-waits-for-next-run"].status, "PASS")
        self.assertEqual(by_name["touches-verify-marker"].status, "PASS")

    def test_cpu_pipeline_runs_run_stages_with_job_parallelism(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            marker = root / "second_cpu_run_started"
            cells = [
                cell(
                    root,
                    "waits-for-other-cpu-run",
                    mode="cpu",
                    commands=[
                        Command(":", "build"),
                        Command(f"for i in $(seq 1 50); do [ -f {marker} ] && exit 0; sleep 0.02; done; exit 7", "run"),
                        Command(":", "verify"),
                    ],
                ),
                cell(
                    root,
                    "starts-other-cpu-run",
                    mode="cpu",
                    commands=[Command(":", "build"), Command(f"touch {marker}", "run"), Command(":", "verify")],
                ),
            ]

            run = run_cells_pipeline_with_options(
                cells, PipelineOptions(Path(tmp) / "logs", timeout=1, keep_artifacts=True, jobs=2, npu_slots=1)
            )
            results = run.results

        by_name = {result.example: result for result in results}
        self.assertEqual(by_name["waits-for-other-cpu-run"].status, "PASS")
        self.assertEqual(by_name["starts-other-cpu-run"].status, "PASS")

    def test_pipeline_reports_npu_idle_and_busy_time(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            cells = [
                cell(
                    root,
                    "slow-build",
                    commands=[Command("sleep 0.05", "build"), Command("sleep 0.05", "run"), Command(":", "verify")],
                )
            ]

            run = run_cells_pipeline_with_options(
                cells, PipelineOptions(Path(tmp) / "logs", timeout=1, keep_artifacts=True, jobs=2, npu_slots=1)
            )

        self.assertEqual(run.npu_stats.slots, 1)
        self.assertEqual(run.npu_stats.queue_model, "pipeline")
        self.assertGreater(run.npu_stats.idle_s, 0)
        self.assertGreater(run.npu_stats.busy_s, 0)
        self.assertGreater(run.npu_stats.utilization, 0)

    def test_report_writes_npu_stats(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            cells = [
                cell(
                    root,
                    "slow-build",
                    commands=[Command("sleep 0.01", "build"), Command("sleep 0.01", "run"), Command(":", "verify")],
                )
            ]
            run = run_cells_pipeline_with_options(
                cells, PipelineOptions(root / "logs", timeout=1, keep_artifacts=True, jobs=2, npu_slots=1)
            )
            report = RunReport("dav-2201", ["npu"], "start", "finish", run.results, [], run.npu_stats)
            json_path = root / "report.json"
            md_path = root / "report.md"

            write_json(report, json_path)
            write_markdown(report, md_path)

            json_text = json_path.read_text(encoding="utf-8")
            md_text = md_path.read_text(encoding="utf-8")

        self.assertIn('"npu_stats"', json_text)
        self.assertIn('"idle_s"', json_text)
        self.assertIn('"utilization"', json_text)
        self.assertIn("npu_idle_s", md_text)
        self.assertIn("npu_utilization", md_text)

    def test_cpu_report_labels_run_queue_stats(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            cells = [
                cell(
                    root,
                    "cpu-stats",
                    mode="cpu",
                    commands=[Command(":", "build"), Command("sleep 0.01", "run"), Command(":", "verify")],
                )
            ]
            run = run_cells_pipeline_with_options(
                cells, PipelineOptions(root / "logs", timeout=1, keep_artifacts=True, jobs=2, npu_slots=1)
            )
            report = RunReport("dav-2201", ["cpu"], "start", "finish", run.results, [], run.npu_stats)
            md_path = root / "report.md"

            write_markdown(report, md_path)
            md_text = md_path.read_text(encoding="utf-8")

        self.assertIn("cpu_run_queue_idle_s", md_text)
        self.assertIn("cpu_run_queue_utilization", md_text)
        self.assertNotIn("npu_idle_s", md_text)

    def test_pipeline_writes_per_stage_logs(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            c = cell(
                root,
                "stage-log-case",
                commands=[
                    Command("echo build-stage", "build"),
                    Command("echo run-stage", "run"),
                    Command("echo verify-stage", "verify"),
                ],
            )

            run = run_cells_pipeline_with_options(
                [c], PipelineOptions(root / "logs", timeout=1, keep_artifacts=True, jobs=2, npu_slots=1)
            )
            result = run.results[0]

            build_log = root / "logs/stages/stage-log-case__npu__build.log"
            run_log = root / "logs/stages/stage-log-case__npu__run.log"
            verify_log = root / "logs/stages/stage-log-case__npu__verify.log"

            self.assertEqual(result.status, "PASS")
            self.assertEqual(set(result.stage_log_files), {str(build_log), str(run_log), str(verify_log)})
            self.assertIn("build-stage", build_log.read_text(encoding="utf-8"))
            self.assertIn("run-stage", run_log.read_text(encoding="utf-8"))
            self.assertIn("verify-stage", verify_log.read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
