#!/usr/bin/python3
# coding=utf-8
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
import os
import sys
import shutil
import unittest
from itertools import chain, repeat, starmap
from unittest import mock

THIS_FILE_NAME = __file__
FILE_PATH = os.path.dirname(os.path.realpath(THIS_FILE_NAME))
TOP_PATH = os.path.join(FILE_PATH, "../../../")
FRAMEWORK_PATH = os.path.join(TOP_PATH, "cmake/asc/legacy_modules/util")
sys.path.append(FRAMEWORK_PATH)

STUB_CPP_LICENSE = """/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/
"""  # to compare generated and stub code

from extract_host_stub import *
from update_host_stub import *
from channel import v310_mode
import extract_host_stub


class TestAclrtLaunchKernel(unittest.TestCase):
    def setUp(self):
        # operator before each testcase
        print("-------------------SetUp----------------")
        self.maxDiff = None

    def tearDown(self):
        # operator after each testcase
        print("-------------------TearDown-------------")

    def test_parse_func_signature(self):
        func_sign_str: Tuple[str, str] = (
            "",
            """
            void add_custom(
                bool bool_1,
                bool bool_2,
                uint32_t uint32_1,
                A tmpStruct,
                uint16_t uint16_1,
                __attribute__((cce_global)) uint8_t* x,
                __attribute__((cce_global)) uint8_t* y,
                __attribute__((cce_global)) uint8_t* z,
                AddCustomTilingData tiling) {}
            """,
        )

        structs: Dict[str, StructRaw] = {
            "A": StructRaw(
                name="A",
                content="""
                struct A {
                    uint16_t A_uint16_1;
                    uint64_t A_uint64_1;
                    bool A_bool_1;
                    bool A_bool_2;
                    uint16_t A_uint16_2;
                };
            """,
            ),
            "AddCustomTilingData": StructRaw(
                name="AddCustomTilingData",
                content="""
                struct AddCustomTilingData {
                    uint32_t totalLength;
                    uint32_t tileNum;
                };
            """,
            ),
        }

        func_signs = parse_func_signature(func_sign_str, structs)

        self.assertEqual(func_signs.func_params[3].tiling_struct, True)
        self.assertEqual(func_signs.func_params[8].tiling_struct, True)

    def test_cut_struct(self):
        instr = """
dummy codes
# 13 "add_custom_tiling.h" 2
struct   AddCustomTilingData
{
    uint32_t totalLength;
    uint32_t tileNum;
};

# test unexpected struct code Indentation
        struct A {
    uint16_t A_uint16_1;
    uint64_t A_uint64_1;
        bool A_bool_1;
    bool A_bool_2;
    uint16_t A_uint16_2;
        };
# 11
        """

        golden = [
            StructRaw(
                name="AddCustomTilingData",
                content="""struct   AddCustomTilingData
{
    uint32_t totalLength;
    uint32_t tileNum;
};""",
            ),
            StructRaw(
                name="A",
                content="""        struct A {
    uint16_t A_uint16_1;
    uint64_t A_uint64_1;
        bool A_bool_1;
    bool A_bool_2;
    uint16_t A_uint16_2;
        };""",
            ),
        ]

        result = list(cut_struct(instr))
        self.assertEqual(len(result), len(golden))
        for res, gold_res in zip(result, golden):
            self.assertEqual(res, gold_res)

    def test_gen_host_stub_code(self):
        # dummy file path
        file_path = "add_custom.cpp.o"

        # build signature object
        # extern "C" __global__ __aicore__ void add_custom(
        #   bool bool_1,
        #   bool bool_2,
        #   uint32_t uint32_1,
        #   A tmpStruct,
        #   uint16_t uint16_1,
        #   GM_ADDR x,
        #   GM_ADDR y,
        #   GM_ADDR z,
        #   AddCustomTilingData tiling)
        func_signs = (
            FuncSign(
                return_type="uint32_t",
                func_name="aclrtlaunch_add_custom",
                func_params=(
                    FuncParam(parts=("uint32_t", "numBlocks"), cce_global=False, tiling_struct=False),
                    FuncParam(parts=("aclrtStream", "stream"), cce_global=False, tiling_struct=False),
                    FuncParam(parts=("bool", "bool_1"), cce_global=False, tiling_struct=False),
                    FuncParam(parts=("bool", "bool_2"), cce_global=False, tiling_struct=False),
                    FuncParam(parts=("uint32_t", "uint32_1"), cce_global=False, tiling_struct=False),
                    FuncParam(parts=("A*", "tmpStruct"), cce_global=False, tiling_struct=True),
                    FuncParam(parts=("uint16_t", "uint16_1"), cce_global=False, tiling_struct=False),
                    FuncParam(parts=("void*", "x"), cce_global=True, tiling_struct=False),
                    FuncParam(parts=("void*", "y"), cce_global=True, tiling_struct=False),
                    FuncParam(parts=("void*", "z"), cce_global=True, tiling_struct=False),
                    FuncParam(parts=("AddCustomTilingData*", "tiling"), cce_global=False, tiling_struct=True),
                ),
                func_template_decl="",
                func_template_specialization_args=(),
                func_params_specialization_args=(),
            ),
        )

        dump_info = {"dump_type": "", "dump_size": 1048576}
        mode = CodeMode.AIV
        base_key = 0

        type_definition = """struct A {
    uint16_t A_uint16_1;
    uint64_t A_uint64_1;
    bool A_bool_1;
    bool A_bool_2;
    uint16_t A_uint16_2;
};

struct AddCustomTilingData {
    uint32_t totalLength;
    uint32_t tileNum;
};"""

        func_signs_list = [FuncSignGroupWithModeBase(file_path, func_signs, dump_info, mode, base_key)]
        host_stub_code = generate_host_stub_code(func_signs_list, type_definition)

        soc_version = "ascend910b1"
        target_name = "ascendc_kernels_npu"
        op_type = "aiv"
        obj_size = 172464
        file_size = 172464

        host_stub_code = update_source_section(host_stub_code, soc_version, target_name)
        _file_len_str = f"__replaced_{op_type}_file_len"
        _file_str = f"__replaced_{op_type}_len"
        host_stub_code = re.sub(_file_len_str, str(obj_size), host_stub_code)
        host_stub_code = re.sub(_file_str, str(file_size), host_stub_code)

        with open(
            os.path.join(TOP_PATH, "tests/python/aclrt_launch_kernel/stub_files/test_gen_host_stup_code.cpp"), "r"
        ) as f:
            golden_code = f.read()
            f.close()
        self.assertEqual(STUB_CPP_LICENSE + host_stub_code, golden_code)

        dump_info = {"dump_type": "", "dump_size": 1048576}
        mode = CodeMode.MIX
        func_signs_list = [FuncSignGroupWithModeBase(file_path, func_signs, dump_info, mode, base_key)]
        host_stub_code = generate_host_stub_code(func_signs_list, type_definition)

        dump_info = {"dump_type": "", "dump_size": 1048576}
        mode = CodeMode.MIX_VECTOR_CORE
        func_signs_list = [FuncSignGroupWithModeBase(file_path, func_signs, dump_info, mode, base_key)]
        host_stub_code = generate_host_stub_code(func_signs_list, type_definition)

        func_signs_list = [FuncSignGroupWithModeBase(file_path, func_signs, dump_info, "MODE_MAX", base_key)]
        self.assertRaises(Exception, generate_host_stub_code, func_signs_list, type_definition)

    def test_gen_host_stub_code_template(self):
        # dummy file path
        file_path = "hello_world.cpp.o"

        # build signature object
        # template<int a>
        # __global__ __aicore__ void hello_world()
        func_signs = (
            FuncSign(
                return_type="uint32_t",
                func_name="aclrtlaunch_hello_world",
                func_params=(
                    FuncParam(parts=("uint32_t", "numBlocks"), cce_global=False, tiling_struct=False),
                    FuncParam(parts=("aclrtStream", "stream"), cce_global=False, tiling_struct=False),
                ),
                func_template_decl="int a",
                func_template_specialization_args=["35", "45"],
                func_params_specialization_args=["", ""],
            ),
        )

        dump_info = {"dump_type": "", "dump_size": 1048576}
        mode = CodeMode.AIV
        base_key = 0

        type_definition = ""

        func_signs_list = [FuncSignGroupWithModeBase(file_path, func_signs, dump_info, mode, base_key)]
        host_stub_code = generate_host_stub_code(func_signs_list, type_definition)

        soc_version = "ascend910b1"
        target_name = "ascendc_kernels_npu"
        op_type = "aiv"
        obj_size = 52352
        file_size = 52352

        host_stub_code = update_source_section(host_stub_code, soc_version, target_name)
        _file_len_str = f"__replaced_{op_type}_file_len"
        _file_str = f"__replaced_{op_type}_len"
        host_stub_code = re.sub(_file_len_str, str(obj_size), host_stub_code)
        host_stub_code = re.sub(_file_str, str(file_size), host_stub_code)

        print("-------host_stub_code\n")
        print(host_stub_code)

        with open(
            os.path.join(TOP_PATH, "tests/python/aclrt_launch_kernel/stub_files/test_gen_host_stub_code_template.cpp"),
            "r",
        ) as f:
            golden_code = f.read()
            f.close()

        self.assertEqual(STUB_CPP_LICENSE + host_stub_code, golden_code)

    @mock.patch("os.environ", {"ASCENDC_CCACHE_EXECUTABLE": "/usr/bin/ccache"})
    @mock.patch("extract_host_stub.get_mode_by_ofile")
    @mock.patch("extract_host_stub.get_preprocessed_source_filepath")
    def test_kerneltype(self, mock_shutil, mock_getmode):
        data = """extern "C" __attribute__((cce_kernel)) [aicore] void add_custom(__attribute__((cce_global)) uint8_t* x, __attribute__((cce_global)) uint8_t* y, __attribute__((cce_global)) uint8_t* z, __attribute__((cce_global)) uint8_t* workspace, __attribute__((cce_global)) uint8_t* tiling)
        {
            auto __enable_feature_for_compile_default = KERNEL_TYPE_MIX_AIV_1_0;
            GET_TILING_DATA(tilingData, tiling);
            KernelAdd op;
            op.Init(x, y, z, tilingData.totalLength, tilingData.tileNum);
            op.Process();
        }"""
        path = "preprocess/add_custom.cpp.o"
        build_mode = "c220"
        kernel_type = find_kernel_type_by_source(path, data, build_mode)
        build_mode = "m200"
        kernel_type = find_kernel_type_by_source(path, data, build_mode)
        build_mode = "c310"
        kernel_type = find_kernel_type_by_source(path, data, build_mode)

        data = """extern "C" __attribute__((cce_kernel)) [aicore] void add_custom_kerneltype(__attribute__((cce_global)) uint8_t* x, __attribute__((cce_global)) uint8_t* y, __attribute__((cce_global)) uint8_t* z, __attribute__((cce_global)) uint8_t* workspace, __attribute__((cce_global)) uint8_t* tiling)
        {
            auto __enable_feature_for_compile_default = KERNEL_TYPE_VECTORCORE;
            GET_TILING_DATA(tilingData, tiling);
            KernelAdd op;
            op.Init(x, y, z, tilingData.totalLength, tilingData.tileNum);
            op.Process();
        }"""
        build_mode = "c220"
        kernel_type = find_kernel_type_by_source(path, data, build_mode)
        build_mode = "m200"
        kernel_type = find_kernel_type_by_source(path, data, build_mode)
        build_mode = "c310"
        kernel_type = find_kernel_type_by_source(path, data, build_mode)

        data = """extern "C" __attribute__((cce_kernel)) [aicore] void add_custom_kerneltype(__attribute__((cce_global)) uint8_t* x, __attribute__((cce_global)) uint8_t* y, __attribute__((cce_global)) uint8_t* z, __attribute__((cce_global)) uint8_t* workspace, __attribute__((cce_global)) uint8_t* tiling)
        {
            GET_TILING_DATA(tilingData, tiling);
            KernelAdd op;
            op.Init(x, y, z, tilingData.totalLength, tilingData.tileNum);
            op.Process();
        }"""
        build_mode = "c220"
        kernel_type = find_kernel_type_by_source(path, data, build_mode)
        build_mode = "m200"
        kernel_type = find_kernel_type_by_source(path, data, build_mode)
        build_mode = "c310"
        kernel_type = find_kernel_type_by_source(path, data, build_mode)

        mock_getmode.return_value = CodeMode.KERNEL_TYPE_AIV_ONLY
        get_mode_dynamic("", "", None)
        get_mode_dynamic("", "", CodeMode.KERNEL_TYPE_AIV_ONLY)

        parse_func_signature_group_by_source(path, data, build_mode)

        multiple_kernel_data = (
            """__attribute__((cce_kernel)) [aicore] void add_custom1(){return}"""
            + """__attribute__((cce_kernel)) [aicore] void add_custom2(){return}"""
        )

        parse_func_signature_group_by_source(path, multiple_kernel_data, build_mode)

        func_groups = [
            FuncSignGroupWithModeBase(
                filepath="preprocess/add_custom.cpp.o",
                func_signs=(
                    FuncSign(
                        return_type="void",
                        func_name="add_custom",
                        func_params=(
                            FuncParam(parts=("uint8_t*", "x"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "y"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "z"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "workspace"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "tiling"), cce_global=True, tiling_struct=False),
                        ),
                        func_template_decl="",
                        func_template_specialization_args=(),
                        func_params_specialization_args=(),
                    ),
                    FuncSign("", "", (), "", (), ()),
                ),
                dump_info={"dump_type": "", "dump_size": 1048576},
                mode=CodeMode.KERNEL_TYPE_AIV_ONLY,
                base_key=0,
            ),
            FuncSignGroupWithModeBase(
                filepath="preprocess/add_custom1.cpp.o",
                func_signs=(
                    FuncSign(
                        return_type="void",
                        func_name="add_custom1",
                        func_params=(
                            FuncParam(parts=("uint8_t*", "x"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "y"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "z"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "workspace"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "tiling"), cce_global=True, tiling_struct=False),
                        ),
                        func_template_decl="",
                        func_template_specialization_args=(),
                        func_params_specialization_args=(),
                    ),
                    FuncSign("", "", (), "", (), ()),
                ),
                dump_info={"dump_type": "", "dump_size": 1048576},
                mode=CodeMode.KERNEL_TYPE_AIC_ONLY,
                base_key=0,
            ),
            FuncSignGroupWithModeBase(
                filepath="preprocess/add_custom2.cpp.o",
                func_signs=(
                    FuncSign(
                        return_type="void",
                        func_name="add_custom2",
                        func_params=(
                            FuncParam(parts=("uint8_t*", "x"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "y"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "z"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "workspace"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "tiling"), cce_global=True, tiling_struct=False),
                        ),
                        func_template_decl="",
                        func_template_specialization_args=(),
                        func_params_specialization_args=(),
                    ),
                    FuncSign("", "", (), "", (), ()),
                ),
                dump_info={"dump_type": "", "dump_size": 1048576},
                mode=CodeMode.KERNEL_TYPE_MIX_AIV_1_0,
                base_key=1,
            ),
            FuncSignGroupWithModeBase(
                filepath="preprocess/add_custom3.cpp.o",
                func_signs=(
                    FuncSign(
                        return_type="void",
                        func_name="add_custom3",
                        func_params=(
                            FuncParam(parts=("uint8_t*", "x"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "y"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "z"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "workspace"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "tiling"), cce_global=True, tiling_struct=False),
                        ),
                        func_template_decl="",
                        func_template_specialization_args=(),
                        func_params_specialization_args=(),
                    ),
                    FuncSign("", "", (), "", (), ()),
                ),
                dump_info={"dump_type": "", "dump_size": 1048576},
                mode=CodeMode.KERNEL_TYPE_MIX_AIC_1_0,
                base_key=1,
            ),
            FuncSignGroupWithModeBase(
                filepath="preprocess/add_custom4.cpp.o",
                func_signs=(
                    FuncSign(
                        return_type="void",
                        func_name="add_custom4",
                        func_params=(
                            FuncParam(parts=("uint8_t*", "x"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "y"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "z"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "workspace"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "tiling"), cce_global=True, tiling_struct=False),
                        ),
                        func_template_decl="",
                        func_template_specialization_args=(),
                        func_params_specialization_args=(),
                    ),
                    FuncSign("", "", (), "", (), ()),
                ),
                dump_info={"dump_type": "", "dump_size": 1048576},
                mode=CodeMode.KERNEL_TYPE_MIX_AIC_1_1,
                base_key=0,
            ),
            FuncSignGroupWithModeBase(
                filepath="preprocess/add_custom5.cpp.o",
                func_signs=(
                    FuncSign(
                        return_type="void",
                        func_name="add_custom5",
                        func_params=(
                            FuncParam(parts=("uint8_t*", "x"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "y"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "z"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "workspace"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "tiling"), cce_global=True, tiling_struct=False),
                        ),
                        func_template_decl="",
                        func_template_specialization_args=(),
                        func_params_specialization_args=(),
                    ),
                    FuncSign("", "", (), "", (), ()),
                ),
                dump_info={"dump_type": "", "dump_size": 1048576},
                mode=CodeMode.KERNEL_TYPE_MIX_AIC_1_2,
                base_key=1,
            ),
            FuncSignGroupWithModeBase(
                filepath="preprocess/add_custom.cpp.o",
                func_signs=(
                    FuncSign(
                        return_type="void",
                        func_name="add_custom",
                        func_params=(
                            FuncParam(parts=("uint8_t*", "x"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "y"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "z"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "workspace"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "tiling"), cce_global=True, tiling_struct=False),
                        ),
                        func_template_decl="",
                        func_template_specialization_args=(),
                        func_params_specialization_args=(),
                    ),
                    FuncSign("", "", (), "", (), ()),
                ),
                dump_info={"dump_type": "", "dump_size": 1048576},
                mode=CodeMode.KERNEL_TYPE_MIX_AIV_1_0,
                base_key=0,
            ),
            FuncSignGroupWithModeBase(
                filepath="preprocess/add_custom.cpp.o",
                func_signs=(
                    FuncSign(
                        return_type="void",
                        func_name="add_custom",
                        func_params=(
                            FuncParam(parts=("uint8_t*", "x"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "y"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "z"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "workspace"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "tiling"), cce_global=True, tiling_struct=False),
                        ),
                        func_template_decl="",
                        func_template_specialization_args=(),
                        func_params_specialization_args=(),
                    ),
                    FuncSign("", "", (), "", (), ()),
                ),
                dump_info={"dump_type": "", "dump_size": 1048576},
                mode=CodeMode.KERNEL_TYPE_AIV_ONLY,
                base_key=0,
            ),
        ]
        func_groups_simple = [
            FuncSignGroup(
                filepath="preprocess/add_custom.cpp.o",
                func_signs=(
                    FuncSign(
                        return_type="void",
                        func_name="add_custom",
                        func_params=(
                            FuncParam(parts=("uint8_t*", "x"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "y"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "z"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "workspace"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "tiling"), cce_global=True, tiling_struct=False),
                        ),
                        func_template_decl="",
                        func_template_specialization_args=(),
                        func_params_specialization_args=(),
                    ),
                    FuncSign("", "", (), "", (), ()),
                ),
                dump_info={"dump_type": "", "dump_size": 1048576},
                kernel_type=CodeMode.KERNEL_TYPE_AIV_ONLY,
                structs={},
            )
        ]
        func_groups_template_mix = [
            FuncSignGroupWithModeBase(
                filepath="preprocess/add_custom.cpp.o",
                func_signs=(
                    FuncSign(
                        return_type="void",
                        func_name="add_custom",
                        func_params=(
                            FuncParam(parts=("uint8_t*", "x"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "y"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "z"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "workspace"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "tiling"), cce_global=True, tiling_struct=False),
                        ),
                        func_template_decl="int a",
                        func_template_specialization_args=["20", "30"],
                        func_params_specialization_args=["", ""],
                    ),
                    FuncSign("", "", (), "int a", (), ()),
                ),
                dump_info={"dump_type": "", "dump_size": 1048576},
                mode=CodeMode.KERNEL_TYPE_MIX_AIC_1_2,
                base_key=1,
            )
        ]
        func_groups_template = [
            FuncSignGroupWithModeBase(
                filepath="preprocess/add_custom.cpp.o",
                func_signs=(
                    FuncSign(
                        return_type="void",
                        func_name="add_custom",
                        func_params=(
                            FuncParam(parts=("uint8_t*", "x"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "y"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "z"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "workspace"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "tiling"), cce_global=True, tiling_struct=False),
                        ),
                        func_template_decl="int a",
                        func_template_specialization_args=["20", "30"],
                        func_params_specialization_args=["", ""],
                    ),
                    FuncSign("", "", (), "int a", (), ()),
                ),
                dump_info={"dump_type": "", "dump_size": 1048576},
                mode=CodeMode.KERNEL_TYPE_AIV_ONLY,
                base_key=1,
            )
        ]
        modes = [CodeMode.KERNEL_TYPE_AIV_ONLY]
        base_keys = list(get_base_keys(func_groups_simple, modes))
        aiv_sources = [
            "add_custom.cpp",
            "add_custom1.cpp",
            "add_custom2.cpp",
            "add_custom3.cpp",
            "add_custom4.cpp",
            "add_custom5.cpp",
        ]

        source_mapping = {
            "preprocess/add_custom.cpp.o": "add_custom.cpp",
            "aic/add_custom.cpp.o": "add_custom.cpp",
            "aiv/add_custom.cpp.o": "add_custom.cpp",
            "preprocess/add_custom1.cpp.o": "add_custom1.cpp",
            "aic/add_custom1.cpp.o": "add_custom1.cpp",
            "aiv/add_custom1.cpp.o": "add_custom1.cpp",
            "preprocess/add_custom2.cpp.o": "add_custom2.cpp",
            "aic/add_custom.cpp2.o": "add_custom2.cpp",
            "aiv/add_custom.cpp2.o": "add_custom2.cpp",
            "preprocess/add_custom3.cpp.o": "add_custom3.cpp",
            "aic/add_custom3.cpp.o": "add_custom3.cpp",
            "aiv/add_custom3.cpp.o": "add_custom3.cpp",
            "preprocess/add_custom4.cpp.o": "add_custom4.cpp",
            "aic/add_custom4.cpp.o": "add_custom4.cpp",
            "aiv/add_custom4.cpp.o": "add_custom4.cpp",
            "preprocess/add_custom5.cpp.o": "add_custom5.cpp",
            "aic/add_custom.cpp5.o": "add_custom5.cpp",
            "aiv/add_custom.cpp5.o": "add_custom5.cpp",
        }
        dst_dir = FILE_PATH + "/stub_files/"
        mock_shutil.return_value = "add_custom.cpp"
        extract_host_stub.IS_V220_MODE = True
        extract_host_stub.RUN_MODE = "npu"
        generate_definition = True
        with mock.patch("builtins.open", new_callable=mock.mock_open, read_data="") as mock_open:
            mock_open.side_effect = RuntimeError()
            self.assertRaises(
                Exception,
                save_device_kernel_function,
                func_groups_template,
                source_mapping,
                dst_dir,
                generate_definition,
            )
            self.assertRaises(
                Exception, save_aic_aiv_config_cmake, func_groups_template, source_mapping, dst_dir, generate_definition
            )
        save_device_kernel_function(func_groups_template, source_mapping, dst_dir, generate_definition)
        save_device_kernel_function(func_groups_template_mix, source_mapping, dst_dir, generate_definition)
        save_aic_aiv_config_cmake(func_groups_template, source_mapping, dst_dir, generate_definition)
        save_device_kernel_function(func_groups, source_mapping, dst_dir, generate_definition)
        cpp_path_assert_only = os.path.join(dst_dir, "auto_gen_add_custom.cpp")
        assert os.path.exists(cpp_path_assert_only)
        with open(cpp_path_assert_only, "r", encoding="utf-8") as file:
            content = file.read()
        save_aic_aiv_config_cmake(func_groups, source_mapping, dst_dir, generate_definition)
        extract_host_stub.IS_V220_MODE = False
        generate_definition = False
        func_groups_m200 = [
            FuncSignGroupWithModeBase(
                filepath="preprocess/add_custom.cpp.o",
                func_signs=(
                    FuncSign(
                        return_type="void",
                        func_name="add_custom",
                        func_params=(
                            FuncParam(parts=("uint8_t*", "x"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "y"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "z"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "workspace"), cce_global=True, tiling_struct=False),
                            FuncParam(parts=("uint8_t*", "tiling"), cce_global=True, tiling_struct=False),
                        ),
                        func_template_decl="",
                        func_template_specialization_args=(),
                        func_params_specialization_args=(),
                    ),
                    FuncSign("", "", (), "", (), ()),
                ),
                dump_info={"dump_type": "", "dump_size": 1048576},
                mode=CodeMode.AIV,
                base_key=0,
            )
        ]
        save_device_kernel_function(func_groups_m200, source_mapping, dst_dir, True)
        save_aic_aiv_config_cmake(func_groups_m200, source_mapping, dst_dir, generate_definition)
        cpp_path = os.path.join(dst_dir, "auto_gen_add_custom.cpp")
        aic_cmake_path = os.path.join(dst_dir, "aic_config.cmake")
        aiv_cmake_path = os.path.join(dst_dir, "aiv_config.cmake")
        self.assertEqual(os.path.exists(cpp_path), True)
        self.assertEqual(os.path.exists(aic_cmake_path), True)
        self.assertEqual(os.path.exists(aiv_cmake_path), True)
        assert os.path.exists(cpp_path)
        os.remove(cpp_path)
        os.remove(aic_cmake_path)
        os.remove(aiv_cmake_path)

    def test_generate_host_stub_head_code(self):
        has_mix = True
        has_aic = True
        has_aiv = True
        generate_code = generate_host_stub_head_code(has_mix, has_aic, has_aiv)
        self.assertIn("g_kernel_handle", generate_code)
        self.assertIn("g_kernel_handle_aiv", generate_code)
        self.assertIn("g_kernel_handle_aic", generate_code)

    def test_gen_template_kernel_header(self):
        # build signature object
        # template<int a>
        # __global__ __aicore__ void hello_world()
        func_sign = FuncSign(
            return_type="uint32_t",
            func_name="aclrtlaunch_hello_world",
            func_params=(
                FuncParam(parts=("uint32_t", "numBlocks"), cce_global=False, tiling_struct=False),
                FuncParam(parts=("aclrtStream", "stream"), cce_global=False, tiling_struct=False),
            ),
            func_template_decl="int a",
            func_template_specialization_args=["35", "45"],
            func_params_specialization_args=["", ""],
        )

        golden_res = """
#ifndef HEADER_ACLRTLAUNCH_HELLO_WORLD_HKERNEL_H_
#define HEADER_ACLRTLAUNCH_HELLO_WORLD_HKERNEL_H_



template<int a>
uint32_t aclrtlaunch_hello_world(uint32_t numBlocks, void* stream);

template<int a>
inline uint32_t hello_world(uint32_t numBlocks, void* hold, void* stream)
{
    (void)hold;
    return aclrtlaunch_hello_world<a>(numBlocks, stream);
}

#endif
"""
        self.assertEqual(generate_kernel_header_code(func_sign), golden_res)

    def test_tiling_remove_ref_or_ptr_func_params(self):
        nomal_func_sign = FuncSign(
            return_type="void",
            func_name="add_custom",
            func_params=(
                FuncParam(parts=("uint8_t*", "x"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "y"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "z"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("CustomStruct*", "myStruct"), cce_global=False, tiling_struct=True),
                FuncParam(parts=("uint8_t*", "workspace"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "tiling"), cce_global=True, tiling_struct=False),
            ),
            func_template_decl="",
            func_template_specialization_args=(),
            func_params_specialization_args=(),
        )

        nomal_without_ptr_func_sign = FuncSign(
            return_type="void",
            func_name="add_custom",
            func_params=(
                FuncParam(parts=("uint8_t*", "x"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "y"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "z"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("CustomStruct", "myStruct"), cce_global=False, tiling_struct=True),
                FuncParam(parts=("uint8_t*", "workspace"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "tiling"), cce_global=True, tiling_struct=False),
            ),
            func_template_decl="",
            func_template_specialization_args=(),
            func_params_specialization_args=(),
        )

        template_func_sign = FuncSign(
            return_type="void",
            func_name="add_custom",
            func_params=(
                FuncParam(parts=("uint8_t*", "x"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "y"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "z"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("CustomStruct&", "myStruct"), cce_global=False, tiling_struct=True),
                FuncParam(parts=("uint8_t*", "workspace"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "tiling"), cce_global=True, tiling_struct=False),
            ),
            func_template_decl="int a",
            func_template_specialization_args=(),
            func_params_specialization_args=(),
        )

        template_without_ref_func_sign = FuncSign(
            return_type="void",
            func_name="add_custom",
            func_params=(
                FuncParam(parts=("uint8_t*", "x"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "y"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "z"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("CustomStruct", "myStruct"), cce_global=False, tiling_struct=True),
                FuncParam(parts=("uint8_t*", "workspace"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "tiling"), cce_global=True, tiling_struct=False),
            ),
            func_template_decl="int a",
            func_template_specialization_args=(),
            func_params_specialization_args=(),
        )

        nomal_func_params_res = tiling_remove_ref_or_ptr_func_params(nomal_func_sign, nomal_func_sign.func_params)
        nomal_without_ptr_func_params_res = tiling_remove_ref_or_ptr_func_params(
            nomal_without_ptr_func_sign, nomal_func_sign.func_params
        )
        template_func_params_res = tiling_remove_ref_or_ptr_func_params(
            template_func_sign, template_func_sign.func_params
        )
        template_without_ref_func_params_res = tiling_remove_ref_or_ptr_func_params(
            template_without_ref_func_sign, template_func_sign.func_params
        )
        self.assertIn("CustomStruct", nomal_func_params_res[3].parts[0])
        self.assertIn("CustomStruct", nomal_without_ptr_func_params_res[3].parts[0])
        self.assertIn("CustomStruct", template_func_params_res[3].parts[0])
        self.assertIn("CustomStruct", template_without_ref_func_params_res[3].parts[0])

    def test_tiling_add_ref_or_ptr_func_params(self):
        nomal_func_sign = FuncSign(
            return_type="void",
            func_name="add_custom",
            func_params=(
                FuncParam(parts=("uint8_t*", "x"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "y"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "z"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("CustomStruct", "myStruct"), cce_global=False, tiling_struct=True),
                FuncParam(parts=("uint8_t*", "workspace"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "tiling"), cce_global=True, tiling_struct=False),
            ),
            func_template_decl="",
            func_template_specialization_args=(),
            func_params_specialization_args=(),
        )

        template_func_sign = FuncSign(
            return_type="void",
            func_name="add_custom",
            func_params=(
                FuncParam(parts=("uint8_t*", "x"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "y"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "z"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("CustomStruct", "myStruct"), cce_global=False, tiling_struct=True),
                FuncParam(parts=("uint8_t*", "workspace"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "tiling"), cce_global=True, tiling_struct=False),
            ),
            func_template_decl="int a",
            func_template_specialization_args=(),
            func_params_specialization_args=(),
        )

        nomal_func_params_res = tiling_add_ref_or_ptr_func_params(nomal_func_sign, nomal_func_sign.func_params)
        template_func_params_res = tiling_add_ref_or_ptr_func_params(template_func_sign, template_func_sign.func_params)
        self.assertIn("CustomStruct*", nomal_func_params_res[3].parts[0])
        self.assertIn("CustomStruct&", template_func_params_res[3].parts[0])

    def test_generate_launch_kernel_code(self):
        mode = CodeMode.MIX
        func_key = 0
        num_blocks = "num_blocks"
        stream = "stream"
        generate_code = generate_launch_kernel_code(mode, func_key, num_blocks, stream)
        self.assertIn("g_kernel_handle == nullptr", generate_code)

        mode = CodeMode.AIC
        generate_code = generate_launch_kernel_code(mode, func_key, num_blocks, stream)
        self.assertIn("g_kernel_handle_aic == nullptr", generate_code)

        mode = CodeMode.AIV
        generate_code = generate_launch_kernel_code(mode, func_key, num_blocks, stream)
        self.assertIn("g_kernel_handle_aiv == nullptr", generate_code)

    @mock.patch("extract_host_stub.get_code_channel")
    def test_get_mode_by_ofile(self, mock_channel):
        mock_channel.side_effect = [[1, CODE_DEFAULT, ""], [0, CODE_DEFAULT, ""]]
        self.assertRaises(Exception, get_mode_by_ofile, "aic", "aiv", True)
        mock_channel.side_effect = {"aic": [0, CODE_DEFAULT, ""], "aiv": [1, CODE_DEFAULT, ""]}.get
        self.assertRaises(Exception, get_mode_by_ofile, "aic", "aiv", False)
        mock_channel.side_effect = {"aic": [1, CODE_DEFAULT, ""], "aiv": [1, CODE_DEFAULT, ""]}.get
        self.assertEqual(get_mode_by_ofile("aic", "aiv", True), CodeMode.AIV)
        mock_channel.side_effect = {"aic": [1, CODE_DEFAULT, ""], "aiv": [1, CODE_DEFAULT, ""]}.get
        self.assertEqual(get_mode_by_ofile("aic", "aiv", False), CodeMode.AIV)
        mock_channel.side_effect = [[1, CODE_DEFAULT, ""], [1, CODE_AIV, ""]]
        self.assertEqual(get_mode_by_ofile("aic", "aiv", True), CodeMode.AIV)
        mock_channel.side_effect = [[1, CODE_DEFAULT, ""], [1, CODE_AIV, ""]]
        self.assertEqual(get_mode_by_ofile("aic", "aiv", False), CodeMode.AIV)
        mock_channel.side_effect = {"aic": [1, CODE_AIC, ""], "aiv": [1, CODE_DEFAULT, ""]}.get
        self.assertEqual(get_mode_by_ofile("aic", "aiv", True), CodeMode.AIC)
        mock_channel.side_effect = {"aic": [1, CODE_AIC, ""], "aiv": [1, CODE_DEFAULT, ""]}.get
        self.assertEqual(get_mode_by_ofile("aic", "aiv", False), CodeMode.AIC)
        mock_channel.side_effect = {"aic": [1, CODE_AIC, ""], "aiv": [1, CODE_AIV, ""]}.get
        self.assertEqual(get_mode_by_ofile("aic", "aiv", True), CodeMode.MIX)
        mock_channel.side_effect = {"aic": [1, CODE_AIC, ""], "aiv": [1, CODE_AIV, ""]}.get
        self.assertEqual(get_mode_by_ofile("aic", "aiv", False), CodeMode.MIX)
        mock_channel.side_effect = {"aic": [1, CODE_AIC, ""], "aiv": [1, CODE_AIC, ""]}.get
        self.assertRaises(Exception, get_mode_by_ofile, "aic", "aiv", True)
        mock_channel.side_effect = {"aic": [1, CODE_AIV, ""], "aiv": [1, CODE_AIV, ""]}.get
        self.assertRaises(Exception, get_mode_by_ofile, "aic", "aiv", False)

    def test_get_code_channel(self):
        self.assertRaises(Exception, get_code_channel("", (True, False)))

    def test_v310_mode(self):
        casescube = {
            "abcdefghi": 0,
            "000000f0": 1,
            "000000c0": 1,
            "00004080": 0,
            "00000015": 0,
            "0000f015": 0,
            "28003072": 1,
            "0000806e": 1,
            "0000a070": 0,
            "8a000070": 0,
            "0000006b": 1,
            "000000e5": 1,
        }
        for inst, golden_result in casescube.items():
            result = v310_mode(inst, True)
            self.assertEqual(result, golden_result)
        casesvec = {
            "abcdefghi": 0,
            "000000f0": 0,
            "000000c0": 0,
            "00004080": 0,
            "00000015": 2,
            "0000f015": 2,
            "28003072": 0,
            "0000806e": 0,
            "00004242": 2,
            "0000e015": 2,
            "00000070": 0,
            "000000b0": 0,
        }
        for inst, golden_result in casesvec.items():
            result = v310_mode(inst, False)
            self.assertEqual(result, golden_result)

    def test_get_func_meta_type(self):
        self.assertEqual(get_func_meta_type(FuncMetaType.F_TYPE_KTYPE), "F_TYPE_KTYPE")
        self.assertEqual(get_func_meta_type(FuncMetaType.F_TYPE_CROSS_CORE_SYNC), "F_TYPE_CROSS_CORE_SYNC")
        self.assertEqual(get_func_meta_type(FuncMetaType.F_TYPE_MAX), "F_TYPE_MAX")
        self.assertRaises(Exception, get_func_meta_type, 4)

    def test_load_compile_commands(self):
        data = [
            {
                "directory": "preprocess",
                "command": "mock -o add_custom.cpp.o -DASCENDC_TIME_STAMP_ON",
                "file": "add_custom.cpp",
            },
            {"directory": "preprocess", "command": "mock -o add_custom_1.cpp.o", "file": "add_custom_1.cpp"},
            {
                "directory": "preprocess",
                "command": "mock -DFEATURE=1 -o add_custom_2.cpp.o",
                "file": "add_custom_2.cpp",
            },
        ]

        source_mapping = {
            "preprocess/add_custom.cpp.o": "add_custom.cpp",
            "preprocess/add_custom_1.cpp.o": "add_custom_1.cpp",
            "preprocess/add_custom_2.cpp.o": "add_custom_2.cpp",
        }
        with (
            mock.patch("builtins.open", new_callable=mock.mock_open, read_data="") as mock_open,
            mock.patch("json.load", return_value=data) as mock_load,
        ):
            result, _ = load_compile_commands("compile_commands.json")
            self.assertEqual(result, source_mapping)
            mock_load.side_effect = RuntimeError()
            self.assertRaises(Exception, load_compile_commands, "compile_commands.json")
            mock_open.side_effect = RuntimeError()

    @mock.patch("os.environ", {"ASCENDC_CCACHE_EXECUTABLE": "/usr/bin/ccache"})
    @mock.patch("extract_host_stub.get_mode_by_ofile")
    @mock.patch("extract_host_stub.get_preprocessed_source_filepath")
    def test_main(self, mock_shutil, mock_getmode):
        data = """extern "C" __attribute__((cce_kernel)) [aicore] void add_custom(__attribute__((cce_global)) uint8_t* x, __attribute__((cce_global)) uint8_t* y, __attribute__((cce_global)) uint8_t* z, __attribute__((cce_global)) uint8_t* workspace, __attribute__((cce_global)) uint8_t* tiling)
        {
            auto __enable_feature_for_compile_default = KERNEL_TYPE_MIX_AIV_1_0;
            GET_TILING_DATA(tilingData, tiling);
            KernelAdd op;
            op.Init(x, y, z, tilingData.totalLength, tilingData.tileNum);
            op.Process();
        }"""

        source_mapping = {
            "preprocess/add_custom.cpp.o": "add_custom.cpp",
            "aic/add_custom.cpp.o": "add_custom.cpp",
            "aiv/add_custom.cpp.o": "add_custom.cpp",
        }
        enable_timestamp_mapping = {
            "preprocess/add_custom.cpp.o": True,
            "aic/add_custom.cpp.o": True,
            "aiv/add_custom.cpp.o": True,
        }

        dst_dir = FILE_PATH + "/stub_files/"
        mock_shutil.return_value = "add_custom.cpp"
        mock_getmode.return_value = CodeMode.KERNEL_TYPE_AIV_ONLY
        compile_dir = dst_dir + "compile_commands.json"
        dst_dir = dst_dir + "dst"
        builtin_open = open
        read_list = [
            data,
            """__attribute__((cce_kernel)) [aicore] void add_custom(){return}""",
            """__attribute__((cce_kernel)) [aicore] void add_custom(void){return}""",
            """__attribute__((cce_kernel)) [aicore] add_custom(){return}""",
            """__attribute__((cce_kernel)) [aicore] void add_custom add_custom(){return}""",
        ]

        argv = [
            "preprocess/add_custom.cpp.o",
            "--aiv-o",
            "aiv/add_custom.cpp.o",
            "--aic-o",
            "aic/add_custom.cpp.o",
            "-d",
            dst_dir,
            "-hd",
            dst_dir,
            "--compile-commands",
            compile_dir,
        ]

        for i in range(len(read_list)):

            def mock_open(*args, **kwargs):
                if args[0] == "preprocess/add_custom.cpp.o":
                    return mock.mock_open(read_data=read_list[i])(*args, **kwargs)
                return builtin_open(*args, **kwargs)

            with mock.patch.object(
                extract_host_stub, "load_compile_commands", return_value=[source_mapping, enable_timestamp_mapping]
            ) as mock_compile:
                with mock.patch("builtins.open", mock_open) as m:
                    if i == 3 or i == 4:
                        self.assertRaises(Exception, main_with_except, argv)
                    else:
                        self.assertTrue(main_with_except(argv))

        def mock_open(*args, **kwargs):
            if args[0] == "preprocess/add_custom.cpp.o":
                return mock.mock_open(read_data=read_list[0])(*args, **kwargs)
            return builtin_open(*args, **kwargs)

        with mock.patch.object(
            extract_host_stub, "load_compile_commands", return_value=[source_mapping, enable_timestamp_mapping]
        ) as mock_compile:
            with mock.patch("builtins.open", mock_open) as m:
                main_with_except(argv)
                mock_compile.side_effect = FileNotFoundError()
                self.assertEqual(False, main_with_except(argv))
            with mock.patch("extract_host_stub.parse_func_signature_group_by_source") as mock_extract:
                mock_extract.side_effect = RuntimeError()
                self.assertEqual(False, main_with_except(argv))

        shutil.rmtree(dst_dir)

        with mock.patch.object(
            extract_host_stub, "load_compile_commands", return_value=[source_mapping, enable_timestamp_mapping]
        ):
            with mock.patch("builtins.open", mock_open) as m:
                main_with_except(
                    [
                        "preprocess/add_custom.cpp.o",
                        "--dynamic-mode",
                        "--aiv-o",
                        "aiv/add_custom.cpp.o",
                        "--aic-o",
                        "aic/add_custom.cpp.o",
                        "-d",
                        dst_dir,
                        "-hd",
                        dst_dir,
                        "--compile-commands",
                        compile_dir,
                    ]
                )
                self.assertEqual(
                    False,
                    main_with_except(
                        [
                            "preprocess/add_custom.cpp.o",
                            "--dynamic-mode",
                            "--aic-o",
                            "aic/add_custom.cpp.o",
                            "-d",
                            dst_dir,
                            "-hd",
                            dst_dir,
                            "--compile-commands",
                            compile_dir,
                        ]
                    ),
                )
                self.assertEqual(
                    False,
                    main_with_except(
                        [
                            "preprocess/add_custom.cpp.o",
                            "--dynamic-mode",
                            "--aiv-o",
                            "aiv/add_custom.cpp.o",
                            "-d",
                            dst_dir,
                            "-hd",
                            dst_dir,
                            "--compile-commands",
                            compile_dir,
                        ]
                    ),
                )

        shutil.rmtree(dst_dir)

        mock_shutil.close()
        with mock.patch.object(
            extract_host_stub, "load_compile_commands", return_value=[source_mapping, enable_timestamp_mapping]
        ):
            with mock.patch("builtins.open", mock_open) as m:
                main_with_except(
                    [
                        "preprocess/add_custom.cpp.o",
                        "--build-mode",
                        "m200",
                        "--generate-definition",
                        "--aiv-o",
                        "aiv/add_custom.cpp.o",
                        "--aic-o",
                        "aic/add_custom.cpp.o",
                        "-d",
                        dst_dir,
                        "-hd",
                        dst_dir,
                        "--compile-commands",
                        compile_dir,
                    ]
                )

        shutil.rmtree(dst_dir)

    @mock.patch("os.environ", {"ASCENDC_CCACHE_EXECUTABLE": "/usr/bin/ccache"})
    @mock.patch("extract_host_stub.get_mode_by_ofile")
    @mock.patch("extract_host_stub.get_preprocessed_source_filepath")
    def test_template_main(self, mock_shutil, mock_getmode):
        source_mapping = {
            "preprocess/add_custom.cpp.o": "add_custom.cpp",
            "aic/add_custom.cpp.o": "add_custom.cpp",
            "aiv/add_custom.cpp.o": "add_custom.cpp",
        }
        enable_timestamp_mapping = {
            "preprocess/add_custom.cpp.o": True,
            "aic/add_custom.cpp.o": True,
            "aiv/add_custom.cpp.o": True,
        }

        dst_dir = FILE_PATH + "/stub_files/"
        mock_shutil.return_value = "add_custom.cpp"
        mock_getmode.return_value = CodeMode.KERNEL_TYPE_AIV_ONLY
        compile_dir = dst_dir + "compile_commands.json"
        dst_dir = dst_dir + "dst"
        builtin_open = open

        template_data = """template<int a>
         __attribute__((cce_kernel)) [aicore] void add_custom(__attribute__((cce_global)) uint8_t* x, __attribute__((cce_global)) uint8_t* y, __attribute__((cce_global)) uint8_t* z, __attribute__((cce_global)) uint8_t* workspace, __attribute__((cce_global)) uint8_t* tiling)
        {
            auto __enable_feature_for_compile_default = KERNEL_TYPE_MIX_AIV_1_0;
            GET_TILING_DATA(tilingData, tiling);
            KernelAdd op;
            op.Init(x, y, z, tilingData.totalLength, tilingData.tileNum);
            op.Process();
        }"""
        result = cut_func_signature(template_data)
        for i in result:
            pass

        def template_mock_open(*args, **kwargs):
            if args[0] == "preprocess/add_custom.cpp.o":
                # mocked open for path "foo"
                return mock.mock_open(read_data=template_data)(*args, **kwargs)
            # unpatched version for every other path
            return builtin_open(*args, **kwargs)

        template_data_o = "void add_custom<5>"
        import subprocess

        with (
            mock.patch.object(
                extract_host_stub, "load_compile_commands", return_value=[source_mapping, enable_timestamp_mapping]
            ),
            mock.patch("builtins.open", template_mock_open) as m,
            mock.patch("os.path.isfile", return_value=True) as mock_file,
            mock.patch("subprocess.run") as mock_run,
        ):
            mock_stdout = mock.MagicMock()
            mock_stdout.configure_mock(**{"stdout.decode.return_value": "void add_custom<5>", "returncode": 0})
            argv = [
                "preprocess/add_custom.cpp.o",
                "--aiv-o",
                "aiv/add_custom.cpp.o",
                "--aic-o",
                "aic/add_custom.cpp.o",
                "-d",
                dst_dir,
                "-hd",
                dst_dir,
                "--compile-commands",
                compile_dir,
            ]
            mock_run.return_value = mock_stdout
            main_with_except(argv)
            mock_file.side_effect = RuntimeError()
            self.assertRaises(Exception, main_with_except, argv)

        shutil.rmtree(dst_dir)

    def test_get_func_template_specialization_mangle_name(self):
        import subprocess

        with mock.patch("subprocess.run") as mock_run, mock.patch("os.path.isfile", return_value=True):
            mock_stdout = mock.MagicMock()
            mock_stdout.configure_mock(
                **{
                    "stdout.decode.return_value": """
0000000000000a7d l       .debug_line_str        0000000000000000 $d.23
0000000000000000 l       .text  0000000000000000 .hidden __block_local_data_end
0000000000000000 l       .text  0000000000000000 .hidden __block_local_data_start
000000000001b000 l       .init_array    0000000000000000 .hidden __init_array_start
000000000001b008 l       .init_array    0000000000000000 .hidden __init_array_end
000000000001b008 l       .fini_array    0000000000000000 .hidden __fini_array_start
000000000001b010 l       .fini_array    0000000000000000 .hidden __fini_array_end
0000000000000328 g     F .text	00000000000001b0 void ReduceCustom<float, 20, 20, 1>(unsigned char*, unsigned char*, unsigned char*)
00000000000004d8  w    F .text	000000000001485c void EasyASC::ReduceSum<float, void ReduceCustom<float, 20, 20, 1>(unsigned char*, unsigned char*, unsigned char*)::selectedPolicyX>(unsigned char AS1*, unsigned char AS1*, EasyASC::ReduceParam AS1*)
00000000000004c0  w    O .bl_uninit		0000000000000008 AscendC::g_dumpWorkspaceReserved
0000000000014d34  w    F .text	0000000000003c88 void EasyASC::PrintArgs<void ReduceCustom<float, 20, 20, 1>(unsigned char*, unsigned char*, unsigned char*)::selectedPolicyX>(EasyASC::ReduceParam AS1*)
00000000000004b8  w    O .bl_uninit     0000000000000008 g_vecTPipePtr
00000000000189bc  w    F .text	0000000000001658 void ReduceSumCompute<float, float, false>::Compute<__reducePattern::RA, (__reducePattern::RA*)0>(ReduceOp::Shape<2>&, AscendC::LocalTensor<float> const&, AscendC::LocalTensor<float> const&)
00000000000004c8  w    O .bl_uninit     0000000000000008 AscendC::g_kfcClient
0000000000000000  w    O __CCE_KernelArgSize    0000000000000004 _Z12ReduceCustomIfLi20ELi20ELi1EEvPhS0_S0___
""",
                    "returncode": 0,
                }
            )
            mock_run.return_value = mock_stdout
            ret, tmp_spec_names, err_msg = get_func_template_specialization_mangle_name("dummy_file.o", "ReduceCustom")
            self.assertEqual(ret, True)
            self.assertEqual(len(tmp_spec_names), 1)
            self.assertEqual(tmp_spec_names[0][0], "float, 20, 20, 1")

    def test_others(self):
        func_param_list = ["__attribute__((cce_global))", "const", "int", "A"]
        func_param_error_list = ["__attribute__((cce_global))", "const", "int"]
        self.assertEqual((2, "int"), get_func_param_type_by_parts(func_param_list))
        self.assertEqual((3, "A"), get_func_param_name_by_parts(func_param_list))
        self.assertRaises(Exception, get_func_param_name_by_parts, func_param_error_list)

        func_param_input = FuncParam(parts=("uint32_t", "numBlocks"), cce_global=False, tiling_struct=True)
        self.assertEqual(convert_func_param_cce_param_type(func_param_input), func_param_input)

        self.assertEqual(convert_to_void("int"), "void")

        func_param_output = FuncParam(parts=("uint32_t*", "numBlocks"), cce_global=False, tiling_struct=True)
        self.assertEqual(tiling_add_pointer(func_param_input), func_param_output)
        self.assertEqual(tiling_remove_pointer(func_param_input), func_param_input)
        self.assertEqual(remove_aclrt_prefix_snake("aclrtlaunch_add_custom"), "add_custom")
        self.assertEqual(remove_aclrt_prefix_snake("add_custom"), "add_custom")
        self.assertEqual(get_triple_chevrons_impl_cpp_filepath("path"), "path/triple_chevrons_config.cmake")

        template_func_sign = FuncSign(
            return_type="void",
            func_name="add_custom",
            func_params=(
                FuncParam(parts=("uint8_t*", "x"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "y"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "z"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("CustomStruct", "myStruct"), cce_global=False, tiling_struct=True),
                FuncParam(parts=("uint8_t*", "workspace"), cce_global=True, tiling_struct=False),
                FuncParam(parts=("uint8_t*", "tiling"), cce_global=True, tiling_struct=False),
            ),
            func_template_decl="int a",
            func_template_specialization_args=(),
            func_params_specialization_args=(),
        )
        self.assertFalse(get_func_template_specialization_mangle_name("not_file", "not_kernel")[0])
        with mock.patch("subprocess.run") as mock_run, mock.patch("os.path.isfile", return_value=True):
            mock_stdout = mock.MagicMock()
            mock_stdout.configure_mock(**{"stdout.decode.return_value": " ", "returncode": 1})
            self.assertRaises(Exception, add_template_specialization_args, template_func_sign, "not_file", "not_file")
            with mock.patch.object(
                extract_host_stub, "get_func_template_specialization_mangle_name", return_value=(True, "int b", "")
            ):
                self.assertRaises(
                    Exception, add_template_specialization_args, template_func_sign, "not_file", "not_file"
                )


if __name__ == "__main__":
    unittest.main()
