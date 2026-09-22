#!/usr/bin/python3
# coding=utf-8
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

import hashlib
import os
from pathlib import Path
import re
import sys
import tempfile
import unittest
from unittest import mock

TOP_PATH = os.path.join(os.path.dirname(os.path.realpath(__file__)), "../../../")
FRAMEWORK_PATH = os.path.join(TOP_PATH, "tools/build/")
if FRAMEWORK_PATH not in sys.path:
    sys.path.insert(0, FRAMEWORK_PATH)

from asc_op_compile_base.asc_op_compiler import static_compile_resource_id as MODULE
from asc_op_compile_base.asc_op_compiler.ascendc_common_utility import CommonUtility, CompileStage


class TestStaticCompileResourceId(unittest.TestCase):
    def setUp(self):
        self.temp_dir = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp_dir.cleanup)
        self.object_path = os.path.join(self.temp_dir.name, "kernel.o")
        Path(self.object_path).write_bytes(b"\x7fELFkernel")

    def test_resource_id_is_the_digest_of_the_object(self):
        expected = hashlib.sha256(Path(self.object_path).read_bytes()).hexdigest()
        resource_id = MODULE.calculate_resource_id(self.object_path)
        self.assertEqual(resource_id, expected)
        self.assertEqual(len(resource_id), MODULE._RESOURCE_ID_VALUE_SIZE)

    def test_resource_id_follows_the_object_contents(self):
        baseline = MODULE.calculate_resource_id(self.object_path)
        Path(self.object_path).write_bytes(b"\x7fELFkernel-rebuilt")
        self.assertNotEqual(MODULE.calculate_resource_id(self.object_path), baseline)

    def test_metadata_layout_matches_kernel_utils_macros_header(self):
        header = Path(os.path.join(TOP_PATH, "impl/basic_api/utils/kernel_utils_macros.h")).read_text(encoding="utf-8")
        self.assertIn(f"B_TYPE_SPECIALIZATION_RESOURCE_ID = {MODULE._RESOURCE_ID_TYPE}", header)
        base_tlv = re.search(r"struct BaseTlv \{(.*?)\}", header, re.S)
        self.assertIsNotNone(base_tlv)
        self.assertEqual(re.findall(r"unsigned short (\w+);", base_tlv.group(1)), ["type", "len"])
        entry = re.search(r"struct BinaryMetaSpecializationResourceId \{(.*?)\}", header, re.S)
        self.assertIsNotNone(entry)
        self.assertIn("BaseTlv head;", entry.group(1))
        self.assertIn(f"char value[{MODULE._RESOURCE_ID_VALUE_SIZE}];", entry.group(1))

    def test_symlink_input_is_rejected(self):
        link_path = os.path.join(self.temp_dir.name, "link.o")
        os.symlink(self.object_path, link_path)
        with self.assertRaisesRegex(MODULE.ResourceIdError, "must not be a symlink"):
            MODULE.calculate_resource_id(link_path)

    def test_generates_metadata_object_and_keeps_inputs_unchanged(self):
        output_path = os.path.join(self.temp_dir.name, "resource_id.o")
        commands, stages, log_paths, sources = [], [], [], []
        original = Path(self.object_path).read_bytes()

        def fake_run_cmd_inner(cmds, stage, compile_log_path=None):
            commands.append(cmds)
            stages.append(stage)
            log_paths.append(compile_log_path)
            sources.append(Path(cmds[cmds.index("-xcce") + 1]).read_text(encoding="utf-8"))
            Path(cmds[cmds.index("-o") + 1]).write_bytes(b"\x7fELF")

        with mock.patch.object(CommonUtility, "run_cmd_inner", side_effect=fake_run_cmd_inner):
            resource_id = MODULE.generate_resource_id_object(
                self.object_path, output_path, ["fake-ccec"], "dav-c220-cube", "compile.log"
            )
        self.assertEqual(len(resource_id), MODULE._RESOURCE_ID_VALUE_SIZE)
        self.assertEqual(commands[0][:4], ["fake-ccec", "-c", "-O3", "-xcce"])
        self.assertIn("--cce-aicore-arch=dav-c220-cube", commands[0])
        self.assertIn("-std=c++17", commands[0])
        self.assertEqual(commands[0][-1], output_path)
        self.assertEqual(stages, [CompileStage.SPECIALIZATION])
        self.assertEqual(log_paths, ["compile.log"])
        source = sources[0]
        self.assertIn('#include "basic_api/kernel_tensor.h"', source)
        self.assertIn('__attribute__((used, section(".ascend.meta")))', source)
        self.assertIn(
            "static const BinaryMetaSpecializationResourceId "
            "g_ascend_resource_id_section = "
            "{{B_TYPE_SPECIALIZATION_RESOURCE_ID, %d}, {%s}};"
            % (MODULE._RESOURCE_ID_VALUE_SIZE, ", ".join(f"'{char}'" for char in resource_id)),
            source,
        )
        self.assertTrue([arg for arg in commands[0] if arg.startswith("-I")])
        self.assertEqual(Path(self.object_path).read_bytes(), original)
        self.assertEqual(Path(output_path).read_bytes(), b"\x7fELF")

    def test_stale_output_is_removed_before_compiling(self):
        # A leftover object must not survive into the link as if it were fresh.
        output_path = os.path.join(self.temp_dir.name, "resource_id.o")
        Path(output_path).write_bytes(b"stale object")
        with mock.patch.object(
            CommonUtility, "run_cmd_inner", side_effect=lambda cmds, stage, compile_log_path=None: None
        ):
            MODULE.generate_resource_id_object(self.object_path, output_path, ["fake-ccec"], "dav-c220-cube")
        self.assertFalse(Path(output_path).exists())


if __name__ == "__main__":
    unittest.main()
