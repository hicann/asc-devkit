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
import unittest
from pathlib import Path

from presmoke.model import Cell, Command, ExampleSpec
from presmoke.runner import FATAL_LOG_MARKERS, PipelineOptions, RunOptions
from presmoke.runner import first_fatal_log_marker, run_cell_with_options, run_cells_pipeline_with_options


def cell(tmp: Path, name: str, commands) -> Cell:
    spec = ExampleSpec(tmp / name, name, commands, ["dav-2201"], ["npu"], "test")
    spec.path.mkdir(parents=True, exist_ok=True)
    return Cell(spec, "dav-2201", "npu", commands, spec.path / "build_npu")


class FatalLogMarkerTest(unittest.TestCase):
    """样例的 main() 丢弃 ACL 返回值并 `return 0`，所以 rc 单独不足以判定通过。"""

    def test_every_marker_is_detected(self) -> None:
        for marker in FATAL_LOG_MARKERS:
            with self.subTest(marker=marker):
                self.assertEqual(first_fatal_log_marker(f"before\n{marker} tail\nafter"), marker)

    def test_healthy_output_has_no_marker(self) -> None:
        self.assertIsNone(first_fatal_log_marker("CompareResult golden.bin success!\ntest pass!\n"))

    def test_compare_result_marker_matches_the_real_output_format(self) -> None:
        # 真实输出中间夹着 golden 文件名。曾经把这条写成 "CompareResult failed" 子串，结果在
        # 728 份 A5 日志上命中 0 次，而同一批日志里这类行有 864 条。
        cases = {
            "CompareResult golden.bin failed. Output[16] is -3.000000, golden is -2.000000": (
                "CompareResult golden.bin failed"
            ),
            "CompareResult golden_PdX.bin failed output is 16.585476, golden is 60.557045": (
                "CompareResult golden_PdX.bin failed"
            ),
        }
        for line, expected in cases.items():
            with self.subTest(line=line):
                self.assertEqual(first_fatal_log_marker(line), expected)

    def test_single_cell_path_fails_on_marker_with_rc_zero(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            target = cell(
                root,
                "01_simd_cpp_api/00_introduction/00_quickstart/hello_world",
                [Command(":", "build"), Command("echo 'test failed!'", "run"), Command(":", "verify")],
            )
            result = run_cell_with_options(target, RunOptions(root / "logs", timeout=10, keep_artifacts=True))

        self.assertEqual(result.status, "FAIL")
        self.assertEqual(result.reason, "fatal log marker: test failed")
        # rc 必须是非零的，否则 orchestrate/JUnit 侧把 rc 当真会报出一条 rc=0 的失败。
        self.assertEqual(result.rc, 1)
        self.assertEqual(result.failing_step, "echo 'test failed!'")

    def test_pipeline_path_fails_on_marker_in_a_gated_run_step(self) -> None:
        # 流水线路径上 run 阶段过 npu_gate，并且会把攒下的等待时间重建进 StepRecord ——
        # 正是那次重建最容易把 fatal_log_marker 丢掉。
        # jobs 必须 >= 2：run_cells_pipeline_with_options 在 jobs<=1 且没有 cards 时会短路到
        # 单用例路径，那样这个测试就跑不到 run_commands 了。
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            target = cell(
                root,
                "01_simd_cpp_api/00_introduction/00_quickstart/hello_world",
                [
                    Command("cmake --version >/dev/null 2>&1 || true", "cmake"),
                    Command("echo 'EE1018 No valid context is bound to the current thread'", "run"),
                    Command(":", "verify"),
                ],
            )
            with contextlib.redirect_stdout(io.StringIO()):
                run = run_cells_pipeline_with_options(
                    [target], PipelineOptions(root / "logs", timeout=10, keep_artifacts=True, jobs=2, npu_slots=1)
                )

        result = run.results[0]
        self.assertEqual(result.status, "FAIL")
        self.assertEqual(result.reason, "fatal log marker: EE1018")
        self.assertEqual(result.rc, 1)

    def test_clean_run_still_passes(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            target = cell(
                root,
                "01_simd_cpp_api/00_introduction/00_quickstart/hello_world",
                [Command(":", "build"), Command("echo 'test pass!'", "run"), Command(":", "verify")],
            )
            result = run_cell_with_options(target, RunOptions(root / "logs", timeout=10, keep_artifacts=True))

        self.assertEqual(result.status, "PASS")
        self.assertEqual(result.rc, 0)


if __name__ == "__main__":
    unittest.main()
