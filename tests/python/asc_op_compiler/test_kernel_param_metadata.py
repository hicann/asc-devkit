# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""Regression tests for metadata generation during the existing CCE compilation."""

from contextlib import ExitStack
import importlib
import re
import struct
from pathlib import Path
import sys
from tempfile import TemporaryDirectory
from types import SimpleNamespace
import unittest
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[3] / "tools/build"))
from asc_op_compile_base.asc_op_compiler import kernel_param_metadata as metadata
from asc_op_compile_base.asc_op_compiler.ascendc_common_utility import CommonUtility, CompileInfo
from asc_op_compile_base.asc_op_compiler.ascendc_constants import CompileOptionTuple, KernelMetaType
from asc_op_compile_base.asc_op_compiler.ascendc_compile_dfx import DFXSectionGenerator
from asc_op_compile_base.asc_op_compiler.get_op_tiling import TilingInfo
from asc_op_compile_base.asc_op_compiler.global_storage import global_var_storage

compile_op = importlib.import_module("asc_op_compile_base.asc_op_compiler.compile_op")


def decode_declaration(source):
    # Read the emitted initializer, independently of the encoder's internal helper.
    initializer = source.split("= {", 1)[1].split("};", 1)[0]
    return bytes(int(value, 16) for value in re.findall(r"0x[0-9a-f]{2}", initializer))


class TestKernelParamMetadata(unittest.TestCase):
    def test_pointer_layout_is_emitted_without_a_second_compile_switch(self):
        description = metadata.build_kernel_param_metadata_from_formals(("GM_ADDR x", "GM_ADDR tiling"))
        source = metadata.generate_param_metadata("add", description)
        self.assertIn('section(".ascend.meta." ASCENDC_PARAM_STRINGIFY(add))', source)
        encoded = decode_declaration(source)
        # Protocol bytes also observed in the native <<<>>> two-pointer ELF.
        expected = bytes.fromhex(
            "10000c00 02000000 1000000000000000 "
            "11002400 00000000 00000000 00000000 08000000 08000200 00000000 00000000 00000000 00000000 "
            "11002400 00000000 01000000 08000000 08000000 08000200 00000000 00000000 00000000 00000000"
        )
        self.assertEqual(encoded, expected)
        self.assertIn("static const unsigned char", source)
        self.assertIn("aligned(4)", source)


class TestGeneratedKernelParamSections(unittest.TestCase):
    def setUp(self):
        self.stack = ExitStack()
        self.addCleanup(self.stack.close)
        self.folder = self.stack.enter_context(TemporaryDirectory())
        self.info = CompileInfo()
        self.info.src_file = "add.cpp"
        self.info.kernel_name = "add"
        self.info.origin_func_name = "add"
        self.info.gen_kernel_func_file = str(Path(self.folder) / "add_kernel.cpp")
        self.info.tiling_key_list = ["7"]
        self.tiling = TilingInfo()
        self.tiling.static_shape_flag = False
        self.op = compile_op.OpInfo(
            kernel_name="add",
            op_type="Add",
            inputs=[{"param_name": "x"}],
            outputs=[{"param_name": "y"}],
            origin_inputs=[],
            mc2_ctx=[],
        )
        DFXSectionGenerator().dfx_info_reset(self.op)
        for name, value in (
            ("ascendc_enable_super_kernel", False),
            ("ascendc_enable_sanitizer", True),
            ("ascendc_tiling_no_register", False),
            ("ascendc_enable_aicore_exception_restart", False),
        ):
            old = global_var_storage.get_variable(name)
            global_var_storage.set_variable(name, value)
            self.addCleanup(global_var_storage.set_variable, name, old)
        self.stack.enter_context(mock.patch.object(CommonUtility, "is_v220", return_value=True))
        self.stack.enter_context(mock.patch.object(CommonUtility, "is_v100", return_value=False))
        self.stack.enter_context(mock.patch.object(CommonUtility, "is_v200", return_value=False))
        self.stack.enter_context(mock.patch.object(CommonUtility, "is_c310", return_value=False))
        self.stack.enter_context(mock.patch.object(CommonUtility, "get_chip_version", return_value="c220"))
        self.stack.enter_context(mock.patch.object(compile_op, "get_current_build_config", return_value=[]))
        self.stack.enter_context(
            mock.patch.object(compile_op, "get_v220_kernel_type_mix_flag", return_value=(False, False))
        )

    def generate_source(self):
        compile_op.gen_kernel_fun(self.info, "add", self.op, self.tiling, CompileOptionTuple([], []))
        return Path(self.info.gen_kernel_func_file).read_text()

    def test_common_generator_places_metadata_after_main_entry(self):
        self.assertNotIn("kernel_param_metadata", vars(self.info))
        source = self.generate_source()
        self.assertNotIn("kernel_param_metadata", vars(self.info))
        entry = source.index('extern "C" __global__ [aicore] void auto_gen_add_kernel(')
        self.assertGreater(source.index("static const unsigned char g_ascendc_param_metadata_"), entry)
        self.assertIn('section(".ascend.meta." ASCENDC_PARAM_STRINGIFY(auto_gen_add_kernel))', source)
        self.assertEqual(struct.unpack_from("<IQ", decode_declaration(source), 4), (4, 32))

    def test_group_metadata_stays_with_each_slave_entry(self):
        self.info.tiling_key_group_map = {"7": ["8"]}
        self.info.tiling_key_kernel_type = {
            "7": KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2,
            "8": KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2,
        }
        source = self.generate_source()
        for core, entry in (("CUBE", "add_8_mix_aic"), ("VEC", "add_8_mix_aiv")):
            guard = f"#if TILING_KEY_VAR == 7UL && defined(__DAV_C220_{core}__)"
            block = source.split(guard, 1)[1].split("\n#if TILING_KEY_VAR", 1)[0]
            self.assertIn(f"void {entry}(", block)
            self.assertIn(f"ASCENDC_PARAM_STRINGIFY({entry})", block)

    def test_encoding_failure_preserves_generated_kernel(self):
        with mock.patch.object(metadata, "_serialize_param_metadata", side_effect=RuntimeError("encoding failure")):
            source = self.generate_source()
        self.assertIn("void auto_gen_add_kernel(", source)
        self.assertNotIn("g_ascendc_param_metadata_", source)

    def test_sk_generation_emits_no_ordinary_metadata(self):
        self.generate_source()
        self.info.super_kernel_info = {"sp_options": {}}
        global_var_storage.set_variable("ascendc_enable_super_kernel", True)
        with mock.patch.object(compile_op, "get_context", return_value=None):
            source = self.generate_source()
        self.assertIn("void auto_gen_add_kernel(uint64_t args_offset)", source)
        self.assertNotIn("kernel_param_metadata", vars(self.info))
        self.assertNotIn("g_ascendc_param_metadata_", source)


class TestKernelDeclarationMetadata(unittest.TestCase):
    def setUp(self):
        self.tiling = TilingInfo()
        self.tiling.static_shape_flag = False
        self.stack = ExitStack()
        self.addCleanup(self.stack.close)
        for name, value in (("ascendc_enable_super_kernel", False), ("ascendc_short_soc_version", "Ascend910B")):
            old = global_var_storage.get_variable(name)
            global_var_storage.set_variable(name, value)
            self.addCleanup(global_var_storage.set_variable, name, old)

    def test_final_declaration_returns_its_layout_and_sk_returns_none(self):
        op = SimpleNamespace(
            mc2_ctx=["ctx"],
            inputs=[None, {"param_name": "x"}],
            outputs=[{"param_name": "y"}],
            output_shape_depend_on_compute=[0],
            param_type_list=None,
        )
        DFXSectionGenerator().dfx_info_reset(op)
        source, _, _, _, description = compile_op._gen_kernel_func_declare_head(True, False, op, self.tiling)
        expected = (
            "GM_ADDR ffts_addr",
            "GM_ADDR ctx",
            "GM_ADDR x",
            "GM_ADDR y",
            "GM_ADDR __ascendc_output_shape",
            "GM_ADDR workspace",
            "GM_ADDR tiling",
        )
        self.assertIsNotNone(description)
        self.assertEqual((description.count, description.total_size), (7, 56))
        self.assertTrue(source.startswith(", ".join(expected) + ") {"))
        global_var_storage.set_variable("ascendc_enable_super_kernel", True)
        with mock.patch.object(compile_op, "get_context", return_value=None):
            source, _, _, _, description = compile_op._gen_kernel_func_declare_head(True, False, op, self.tiling)
        self.assertTrue(source.startswith("uint64_t args_offset) {"))
        self.assertIsNone(description)

    def test_qualified_tiling_pointer_keeps_all_parameter_records(self):
        op = SimpleNamespace(
            mc2_ctx=[],
            inputs=[{"param_name": "x"}],
            outputs=[{"param_name": "y"}],
            output_shape_depend_on_compute=[],
            param_type_list=None,
        )
        DFXSectionGenerator().dfx_info_reset(op)
        with mock.patch.object(CommonUtility, "is_920r1", return_value=True):
            source, _, _, _, description = compile_op._gen_kernel_func_declare_head(False, False, op, self.tiling)
        self.assertIn("GM_ADDR __early_read_before_pre_task_done__ tiling", source)
        self.assertIsNotNone(description)
        encoded = decode_declaration(metadata.generate_param_metadata("add", description))
        self.assertEqual(struct.unpack_from("<IQ", encoded, 4), (4, 32))
        for ordinal in range(4):
            self.assertEqual(
                struct.unpack_from("<IIIIHHIIII", encoded, 16 + ordinal * 40 + 4),
                (0, ordinal, ordinal * 8, 8, 8, 2, 0, 0, 0, 0),
            )


if __name__ == "__main__":
    unittest.main()
