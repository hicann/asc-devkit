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

import unittest
from pathlib import Path

from presmoke.model import Command, ExampleSpec
from presmoke.planner import build_cells_with_skips, rewrite_cmake


class PlannerTest(unittest.TestCase):
    def test_rewrite_cmake_replaces_arch_and_mode(self) -> None:
        cmd = rewrite_cmake(
            "cmake -DSCENARIO_NUM=4 -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DCMAKE_ASC_RUN_MODE=cpu ..", "dav-3510", "sim"
        )
        self.assertIn("-DSCENARIO_NUM=4", cmd)
        self.assertIn("-DCMAKE_ASC_ARCHITECTURES=dav-3510", cmd)
        self.assertIn("-DCMAKE_ASC_RUN_MODE=sim", cmd)
        self.assertNotIn("dav-2201", cmd)
        self.assertNotIn("cpu", cmd)

    def test_rewrite_cmake_omits_npu_mode_flag(self) -> None:
        cmd = rewrite_cmake("cmake ..", "dav-2201", "npu")
        self.assertIn("-DCMAKE_ASC_ARCHITECTURES=dav-2201", cmd)
        self.assertNotIn("CMAKE_ASC_RUN_MODE", cmd)

    def test_rewrite_cmake_appends_only_asc_werror_flag_when_enabled(self) -> None:
        cmd = rewrite_cmake("cmake ..", "dav-2201", "npu", werror=True)

        self.assertNotIn("CMAKE_C_FLAGS", cmd)
        self.assertNotIn("CMAKE_CXX_FLAGS", cmd)
        self.assertIn("-DCMAKE_ASC_FLAGS=-Werror", cmd)

    def test_rewrite_command_sets_host_werror_env_when_enabled(self) -> None:
        command = Command("cmake ..", "cmake", {"CXXFLAGS": "-O2"})

        rewritten = build_cells_with_skips(
            [ExampleSpec(Path("/tmp/example"), "example", [command], ["dav-2201"], ["npu"], "readme")],
            "dav-2201",
            ["npu"],
            werror=True,
        )[0][0].commands[0]

        self.assertEqual(rewritten.env["CFLAGS"], "-Werror")
        self.assertEqual(rewritten.env["CXXFLAGS"], "-O2 -Werror")

    def test_unsupported_arch_is_excluded_not_counted_as_skip(self) -> None:
        spec = ExampleSpec(
            Path("/tmp/example"), "example", [Command("cmake ..", "cmake")], ["dav-3510"], ["npu"], "readme"
        )
        cells, suggestions, skipped = build_cells_with_skips([spec], "dav-2201", ["npu"])
        self.assertEqual(cells, [])
        self.assertEqual(len(suggestions), 1)
        self.assertEqual(skipped, [])
        self.assertIn("Excluded on unsupported arch dav-2201", suggestions[0].message)


if __name__ == "__main__":
    unittest.main()
