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
import tempfile
import unittest
from pathlib import Path

from presmoke.orchestrate_report import shard_examples


class OrchestrateShardingTest(unittest.TestCase):
    def test_custom_op_dependents_and_parallel_package_stay_on_one_card(self) -> None:
        dependency_group = [
            "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op_static_lib",
            "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op",
            "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclnn_invocation",
            "01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclop_invocation",
            "01_simd_cpp_api/02_features/00_framework/02_onnx/onnx_plugin",
            "04_aicpu/02_features/00_framework/00_pytorch/tiling_sink_programming",
            "01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/parallel_ops_package",
        ]
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            report = write_plan_report(root, [*dependency_group, "case/a", "case/b", "case/c"])
            assignments = dict((example, card) for card, example in shard_examples(report, ["0", "1", "2"]))

        self.assertEqual(len({assignments[example] for example in dependency_group}), 1)

    def test_sharding_can_split_same_source_case_across_cards(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            report = write_plan_report(root, ["case/source__scenario_1", "case/source__scenario_2", "case/c", "case/d"])

            assignments = dict((example, card) for card, example in shard_examples(report, ["0", "1"]))

        self.assertNotEqual(assignments["case/source__scenario_1"], assignments["case/source__scenario_2"])

    def test_sharding_uses_duration_report_to_balance_cards(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            report = write_plan_report(root, ["case/a", "case/b", "case/c"])
            timings = root / "ALL_CASE_TIMINGS.tsv"
            timings.write_text(
                "\n".join(
                    [
                        "run\texample\tarch\tmode\tstatus\trc\tduration_s",
                        "full\tcase/a\tdav-2201\tnpu\tPASS\t0\t1",
                        "full\tcase/b\tdav-2201\tnpu\tPASS\t0\t100",
                        "full\tcase/c\tdav-2201\tnpu\tPASS\t0\t1",
                    ]
                )
                + "\n",
                encoding="utf-8",
            )

            assignments = dict(
                (example, card) for card, example in shard_examples(report, ["0", "1"], schedule_report=timings)
            )

        self.assertNotEqual(assignments["case/a"], assignments["case/b"])
        self.assertEqual(assignments["case/a"], assignments["case/c"])

    def test_sharding_uses_stage_timings_to_balance_pipeline_makespan(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            report = write_plan_report(root, ["case/a", "case/b", "case/c"])
            timings = root / "ALL_CASE_TIMINGS.tsv"
            timings.write_text(
                "\n".join(
                    [
                        "example\tbuild_s\trun_s\tverify_s\tduration_s",
                        "case/a\t100\t1\t1\t102",
                        "case/b\t90\t1\t1\t92",
                        "case/c\t1\t1\t1\t3",
                    ]
                )
                + "\n",
                encoding="utf-8",
            )

            assignments = dict(
                (example, card) for card, example in shard_examples(report, ["0", "1"], schedule_report=timings, jobs=2)
            )

        self.assertNotEqual(assignments["case/a"], assignments["case/b"])
        self.assertIn(assignments["case/c"], {"0", "1"})

    def test_fixed_shards_map_logical_shards_to_detected_cards(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            report = write_plan_report(root, ["case/a", "case/b", "case/c"])
            shards = root / "shards"
            shards.mkdir()
            (shards / "card_0.txt").write_text("case/b\ncase/a\n", encoding="utf-8")
            (shards / "card_1.txt").write_text("case/c\n", encoding="utf-8")

            assignments = shard_examples(report, ["4", "6"], fixed_shards=shards)

        self.assertEqual(assignments, [("4", "case/b"), ("4", "case/a"), ("6", "case/c")])

    def test_fixed_shards_reject_incomplete_case_coverage(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            report = write_plan_report(root, ["case/a", "case/b"])
            shards = root / "shards"
            shards.mkdir()
            (shards / "card_0.txt").write_text("case/a\n", encoding="utf-8")
            (shards / "card_1.txt").write_text("", encoding="utf-8")

            with self.assertRaisesRegex(ValueError, "missing planned cases.*case/b"):
                shard_examples(report, ["0", "1"], fixed_shards=shards)


def write_plan_report(root: Path, examples: list[str]) -> Path:
    report = root / "report.json"
    report.write_text(
        json.dumps({"results": [{"example": example, "duration_s": 0} for example in examples]}), encoding="utf-8"
    )
    return report


if __name__ == "__main__":
    unittest.main()
