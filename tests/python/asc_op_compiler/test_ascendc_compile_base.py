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
import unittest
from unittest import mock

THIS_FILE_NAME = __file__
FILE_PATH = os.path.dirname(os.path.realpath(THIS_FILE_NAME))
TOP_PATH = os.path.join(FILE_PATH, "../../../")
FRAMEWORK_PATH = os.path.join(TOP_PATH, "tools/build/")
sys.path.insert(0, FRAMEWORK_PATH)

import asc_op_compile_base
from asc_op_compile_base.common.platform import (
    set_current_compile_soc_info,
    get_soc_spec,
)
from asc_op_compile_base.common import buildcfg
from asc_op_compile_base.asc_op_compiler.ascendc_compile_base import *
from asc_op_compile_base.asc_op_compiler.ascendc_constants import *
from asc_op_compile_base.asc_op_compiler.ascendc_common_utility import *
from asc_op_compile_base.asc_op_compiler.global_storage import global_var_storage
from asc_op_compile_base.asc_op_compiler.super_kernel_constants import (
    SuperKernelFeedSyncAllMode,
    AI_CORE_STR,
    SuperKernelEarlyStartMode,
    SuperKernelStreamFusionMode,
)
from asc_op_compile_base.asc_op_compiler.super_kernel_utility import (
    check_exist_instrinsic_when_super_kernel,
)
from asc_op_compile_base.asc_op_compiler.super_kernel_op_compile import (
    gen_file_header,
    split_spk_kernel_objs,
    localize_symbol_of_sk,
    localization_sub_op_func_sym,
)
from asc_op_compile_base.asc_op_compiler.super_kernel_sub_op_compile import (
    gen_sub_kernel_name,
    split_kernel_arch_str,
    sp_add_sub_op_feed_sync_all_macro,
    sp_add_sub_op_block_num_macro,
)
from asc_op_compile_base.asc_op_compiler.get_op_tiling import TilingInfo
from asc_op_compile_base.asc_op_compiler.super_kernel_sub_op_compile import (
    split_sub_kernel_objs,
    gen_sub_super_kernel_early_start_compile_options,
    gen_sub_super_kernel_compile_options,
    add_sub_super_kernel_info,
)

import importlib

compile_op_module = importlib.import_module(
    "asc_op_compile_base.asc_op_compiler.compile_op"
)


def SetCurrentSocInfo(soc: str):
    set_current_compile_soc_info(soc)
    global_var_storage.set_variable(
        "ascendc_short_soc_version", get_soc_spec("SHORT_SOC_VERSION")
    )


class TestAscendCCompileBase(unittest.TestCase):
    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_link_sk_norm_combine_with_meta_file(self):
        result = mock.Mock(returncode=0, stderr="")
        with mock.patch.object(
            CCECInfo, "get_exe", return_value="ld.lld"
        ), mock.patch.object(
            subprocess, "run", return_value=result
        ) as mock_run, mock.patch.object(
            os, "listdir", return_value=["sk.o"]
        ), mock.patch.object(os, "makedirs"), mock.patch.object(
            os, "getpid", return_value=123
        ):
            link_sk_norm_combine(
                "kernel.o", "norm.o", "sk_bind.o", "meta_info.o", "compile.log"
            )

        relocatable_link_cmd = mock_run.call_args_list[1].args[0]
        self.assertEqual(relocatable_link_cmd.count("meta_info.o"), 1)
        self.assertEqual(
            relocatable_link_cmd[-3:], ["norm.o", "sk_bind.o", "meta_info.o"]
        )

    def test_link_sk_norm_combine_without_meta_file(self):
        result = mock.Mock(returncode=0, stderr="")
        with mock.patch.object(
            CCECInfo, "get_exe", return_value="ld.lld"
        ), mock.patch.object(
            subprocess, "run", return_value=result
        ) as mock_run, mock.patch.object(
            os, "listdir", return_value=["sk.o"]
        ), mock.patch.object(os, "makedirs"), mock.patch.object(
            os, "getpid", return_value=123
        ):
            link_sk_norm_combine("kernel.o", "norm.o", "sk_bind.o", "", "compile.log")

        relocatable_link_cmd = mock_run.call_args_list[1].args[0]
        self.assertEqual(relocatable_link_cmd[-2:], ["norm.o", "sk_bind.o"])

    def test_gen_file_header(self):
        file_header = gen_file_header(KernelMetaType.KERNEL_TYPE_AIV_ONLY, 1)
        self.assertIn("__NPU_ARCH__ == 2201", file_header)

    def test_split_spk_kernel_objs(self):
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            s = "unsupported API".encode("utf-8")
            with mock.patch.object(
                CommonUtility, "run_cmd_ascendc", return_value=[s, 0]
            ):
                with mock.patch.object(
                    CommonUtility, "dump_build_log", return_value=[s, 0]
                ):
                    with mock.patch.object(
                        CommonUtility, "dump_compile_log", return_value=[s, 0]
                    ):
                        with mock.patch.object(
                            CommonUtility, "is_c310", return_value=True
                        ):
                            with mock.patch("os.path.exists", return_value=False):
                                split_mode = 4
                                compile_log_path = "./"
                                sub_op_list = [
                                    "./te_superkernel_1_mix_aic.o",
                                    "./te_superkernel_1_mix_aiv.o",
                                ]
                                kernel_type = KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2
                                sub_objs, sk_new = split_spk_kernel_objs(
                                    sub_op_list,
                                    split_mode,
                                    kernel_type,
                                    compile_log_path,
                                )
                                print(sub_objs)
                                self.assertEqual(sub_objs[0], "./sk_0.o")

    def test_localize_symbol_of_sk(self):
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            s = "unsupported API".encode("utf-8")
            with mock.patch.object(
                CommonUtility, "run_cmd_ascendc", return_value=[s, 0]
            ):
                with mock.patch.object(
                    CommonUtility, "dump_build_log", return_value=[s, 0]
                ):
                    with mock.patch.object(
                        CommonUtility, "dump_compile_log", return_value=[s, 0]
                    ):
                        with mock.patch("os.path.exists", return_value=False):
                            split_mode = 4
                            sks = ["_sk_mix_aic"]
                            spk_dst_file = "./te_superkernel_1_mix_aic.o"
                            compile_log_path = "./"
                            localize_symbol_of_sk(
                                split_mode, sks, spk_dst_file, compile_log_path
                            )
                            self.assertEqual(len(sks), 1)

    def test_sp_add_sub_op_block_num_macro(self):
        compile_option_tuple = CompileOptionTuple([], [])
        tiling_info = TilingInfo()
        sp_add_sub_op_block_num_macro(compile_option_tuple, tiling_info)
        self.assertEqual(len(compile_option_tuple.compile_options), 1)
        self.assertIn(
            "-D__SUPER_KERNEL_DYNAMIC_BLOCK_NUM__", compile_option_tuple.compile_options
        )

        compile_option_tuple = CompileOptionTuple([], [])
        tiling_info = TilingInfo()
        tiling_info.block_num = 10
        sp_add_sub_op_block_num_macro(compile_option_tuple, tiling_info)
        self.assertEqual(len(compile_option_tuple.compile_options), 1)
        self.assertIn(
            "-D__SUPER_KERNEL_STATIC_BLOCK_NUM__=10",
            compile_option_tuple.compile_options,
        )

    def test_gen_sub_super_kernel_compile_options(self):
        # StreamFusionEnable
        with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
            with buildcfg.build_config():
                with mock.patch.object(
                    asc_op_compile_base.common.context.get_context(),
                    "get_addition",
                    return_value={
                        "super_kernel_count": 4,
                        "super_kernel_sub_id": 1,
                        "super_kernel_sub_loc": "middle",
                    },
                ):
                    compile_option_tuple = CompileOptionTuple([], [])
                    tiling_info = TilingInfo()
                    compile_info = CompileInfo()
                    compile_info.super_kernel_info = {
                        "sp_options": {
                            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable
                        }
                    }
                    gen_sub_super_kernel_compile_options(
                        compile_option_tuple, tiling_info, compile_info
                    )
                    self.assertEqual(len(compile_option_tuple.compile_options), 3)
                    self.assertIn(
                        "-D__ASCENDC_ENABLE_SUPER_KERNEL__",
                        compile_option_tuple.compile_options,
                    )
                    self.assertIn(
                        "-D__SUPER_KERNEL_DYNAMIC_BLOCK_NUM__",
                        compile_option_tuple.compile_options,
                    )
                    self.assertIn(
                        "-D__ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__",
                        compile_option_tuple.compile_options,
                    )

        # StreamFusionDisable, dynamic shape no early start v2
        with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
            with buildcfg.build_config():
                with mock.patch.object(
                    asc_op_compile_base.common.context.get_context(),
                    "get_addition",
                    return_value={
                        "super_kernel_count": 4,
                        "super_kernel_sub_id": 1,
                        "super_kernel_sub_loc": "middle",
                    },
                ):
                    compile_option_tuple = CompileOptionTuple([], [])
                    tiling_info = TilingInfo()
                    tiling_info.static_shape_flag = False
                    compile_info = CompileInfo()
                    compile_info.super_kernel_info = {
                        "sp_options": {
                            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionDisable
                        }
                    }
                    gen_sub_super_kernel_compile_options(
                        compile_option_tuple, tiling_info, compile_info
                    )
                    self.assertEqual(len(compile_option_tuple.compile_options), 2)
                    self.assertIn(
                        "-D__ASCENDC_ENABLE_SUPER_KERNEL__",
                        compile_option_tuple.compile_options,
                    )
                    self.assertIn(
                        "-D__SUPER_KERNEL_DYNAMIC_BLOCK_NUM__",
                        compile_option_tuple.compile_options,
                    )

        # StreamFusionDisable, static shape
        with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
            with buildcfg.build_config():
                with mock.patch.object(
                    asc_op_compile_base.common.context.get_context(),
                    "get_addition",
                    return_value={
                        "super_kernel_count": 4,
                        "super_kernel_sub_id": 1,
                        "super_kernel_sub_loc": "middle",
                    },
                ):
                    compile_option_tuple = CompileOptionTuple([], [])
                    tiling_info = TilingInfo()
                    tiling_info.static_shape_flag = True
                    compile_info = CompileInfo()
                    compile_info.super_kernel_info = {
                        "sp_options": {
                            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionDisable
                        }
                    }
                    gen_sub_super_kernel_compile_options(
                        compile_option_tuple, tiling_info, compile_info
                    )
                    self.assertEqual(len(compile_option_tuple.compile_options), 4)
                    self.assertIn(
                        "-D__ASCENDC_ENABLE_SUPER_KERNEL__",
                        compile_option_tuple.compile_options,
                    )
                    self.assertIn(
                        "-D__SUPER_KERNEL_DYNAMIC_BLOCK_NUM__",
                        compile_option_tuple.compile_options,
                    )
                    self.assertIn(
                        "-D__ASCENDC_SUPERKERNEL_EARLY_START_V2",
                        compile_option_tuple.compile_options,
                    )
                    self.assertIn(
                        "-D__ASCENDC_SUPER_KERNEL_ENABLE_GM_GET_SET_VALUE_DCCI__",
                        compile_option_tuple.compile_options,
                    )

    def test_gen_sub_super_kernel_early_start_compile_options(self):
        # early start sub kernel disable
        with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
            with buildcfg.build_config():
                with mock.patch.object(
                    asc_op_compile_base.common.context.get_context(),
                    "get_addition",
                    return_value={
                        "super_kernel_count": 4,
                        "super_kernel_sub_id": 1,
                        "super_kernel_sub_loc": "middle",
                    },
                ):
                    compile_option_tuple = CompileOptionTuple([], [])
                    compile_info = CompileInfo()
                    compile_info.super_kernel_info = {
                        "sp_options": {
                            "early-start": SuperKernelEarlyStartMode.EarlyStartDisable
                        }
                    }
                    gen_sub_super_kernel_early_start_compile_options(
                        compile_option_tuple, compile_info
                    )
                    self.assertEqual(len(compile_option_tuple.compile_options), 0)

                    compile_info.super_kernel_info = {
                        "sp_options": {
                            "early-start": SuperKernelEarlyStartMode.EarlyStartV2DisableSubKernel
                        }
                    }
                    self.assertEqual(len(compile_option_tuple.compile_options), 0)

        # early start sub kernel enable
        with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
            with buildcfg.build_config():
                with mock.patch.object(
                    asc_op_compile_base.common.context.get_context(),
                    "get_addition",
                    return_value={
                        "super_kernel_count": 4,
                        "super_kernel_sub_id": 1,
                        "super_kernel_sub_loc": "middle",
                    },
                ):
                    compile_option_tuple = CompileOptionTuple([], [])
                    compile_info = CompileInfo()
                    compile_info.super_kernel_info = {
                        "sp_options": {
                            "early-start": SuperKernelEarlyStartMode.EarlyStartEnableV2
                        }
                    }
                    gen_sub_super_kernel_early_start_compile_options(
                        compile_option_tuple, compile_info
                    )
                    self.assertEqual(len(compile_option_tuple.compile_options), 1)
                    self.assertEqual(
                        compile_option_tuple.compile_options[0],
                        "-D__ASCENDC_SUPERKERNEL_EARLY_START_V2",
                    )

                    compile_option_tuple = CompileOptionTuple([], [])
                    compile_info = CompileInfo()
                    compile_info.super_kernel_info = {
                        "sp_options": {
                            "early-start": SuperKernelEarlyStartMode.EarlyStartEnableV1
                        }
                    }
                    gen_sub_super_kernel_early_start_compile_options(
                        compile_option_tuple, compile_info
                    )
                    self.assertEqual(len(compile_option_tuple.compile_options), 1)
                    self.assertEqual(
                        compile_option_tuple.compile_options[0],
                        "-D__ASCENDC_SUPERKERNEL_EARLY_START_V1",
                    )

        # early start sub kernel set flag, sub count 4, sub id 1
        with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
            with buildcfg.build_config():
                with mock.patch.object(
                    asc_op_compile_base.common.context.get_context(),
                    "get_addition",
                    return_value={
                        "super_kernel_count": 4,
                        "super_kernel_sub_id": 1,
                        "super_kernel_sub_loc": "middle",
                    },
                ):
                    compile_option_tuple = CompileOptionTuple([], [])
                    compile_info = CompileInfo()
                    compile_info.super_kernel_early_start_set_flag = True
                    compile_info.super_kernel_early_start_wait_flag = True
                    compile_info.super_kernel_info = {
                        "sp_options": {
                            "early-start": SuperKernelEarlyStartMode.EarlyStartEnableV2
                        }
                    }
                    gen_sub_super_kernel_early_start_compile_options(
                        compile_option_tuple, compile_info
                    )
                    self.assertEqual(len(compile_option_tuple.compile_options), 3)
                    self.assertEqual(
                        compile_option_tuple.compile_options[0],
                        "-D__ASCENDC_SUPERKERNEL_EARLY_START_V2",
                    )
                    self.assertEqual(
                        compile_option_tuple.compile_options[1],
                        "-D__ASCENDC_ENABLE_SET_NEXT_TASK_START",
                    )
                    self.assertEqual(
                        compile_option_tuple.compile_options[2],
                        "-D__ASCENDC_ENABLE_WAIT_PRE_TASK_END",
                    )

        # early start sub kernel set flag, sub count 4, sub id 0
        with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
            with buildcfg.build_config():
                with mock.patch.object(
                    asc_op_compile_base.common.context.get_context(),
                    "get_addition",
                    return_value={
                        "super_kernel_count": 4,
                        "super_kernel_sub_id": 0,
                        "super_kernel_sub_loc": "middle",
                    },
                ):
                    compile_option_tuple = CompileOptionTuple([], [])
                    compile_info = CompileInfo()
                    compile_info.super_kernel_early_start_set_flag = True
                    compile_info.super_kernel_early_start_wait_flag = True
                    compile_info.super_kernel_info = {
                        "sp_options": {
                            "early-start": SuperKernelEarlyStartMode.EarlyStartEnableV2
                        }
                    }
                    gen_sub_super_kernel_early_start_compile_options(
                        compile_option_tuple, compile_info
                    )
                    self.assertEqual(len(compile_option_tuple.compile_options), 3)
                    self.assertEqual(
                        compile_option_tuple.compile_options[0],
                        "-D__ASCENDC_SUPERKERNEL_EARLY_START_V2",
                    )
                    self.assertEqual(
                        compile_option_tuple.compile_options[1],
                        "-D__ASCENDC_ENABLE_SET_NEXT_TASK_START",
                    )

        # early start sub kernel set flag, sub count 4, sub id 3
        with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
            with buildcfg.build_config():
                with mock.patch.object(
                    asc_op_compile_base.common.context.get_context(),
                    "get_addition",
                    return_value={
                        "super_kernel_count": 4,
                        "super_kernel_sub_id": 3,
                        "super_kernel_sub_loc": "middle",
                    },
                ):
                    compile_option_tuple = CompileOptionTuple([], [])
                    compile_info = CompileInfo()
                    compile_info.super_kernel_early_start_set_flag = True
                    compile_info.super_kernel_early_start_wait_flag = True
                    compile_info.super_kernel_info = {
                        "sp_options": {
                            "early-start": SuperKernelEarlyStartMode.EarlyStartEnableV2
                        }
                    }
                    gen_sub_super_kernel_early_start_compile_options(
                        compile_option_tuple, compile_info
                    )
                    self.assertEqual(len(compile_option_tuple.compile_options), 3)
                    self.assertEqual(
                        compile_option_tuple.compile_options[0],
                        "-D__ASCENDC_SUPERKERNEL_EARLY_START_V2",
                    )
                    self.assertEqual(
                        compile_option_tuple.compile_options[1],
                        "-D__ASCENDC_ENABLE_SET_NEXT_TASK_START",
                    )

        # aclgraph sub combine path enables V3 runtime-mask path and records flags through global storage.
        global_var_storage.global_storage_reset()
        with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
            with buildcfg.build_config():
                with mock.patch.object(
                    asc_op_compile_base.common.context.get_context(),
                    "get_addition",
                    return_value=True,
                ):
                    compile_option_tuple = CompileOptionTuple([], [])
                    compile_info = CompileInfo()
                    compile_info.super_kernel_info = {
                        "sp_options": {"early-start": "1"}
                    }
                    compile_info.super_kernel_early_start_set_flag = True
                    compile_info.super_kernel_early_start_wait_flag = True
                    gen_sub_super_kernel_early_start_compile_options(
                        compile_option_tuple, compile_info
                    )
                    self.assertEqual(len(compile_option_tuple.compile_options), 3)
                    self.assertEqual(
                        compile_option_tuple.compile_options[0],
                        "-D__ASCENDC_SUPERKERNEL_EARLY_START_V3",
                    )
                    self.assertEqual(
                        compile_option_tuple.compile_options[1],
                        "-D__ASCENDC_ENABLE_SET_NEXT_TASK_START",
                    )
                    self.assertEqual(
                        compile_option_tuple.compile_options[2],
                        "-D__ASCENDC_ENABLE_WAIT_PRE_TASK_END",
                    )
                    self.assertTrue(
                        global_var_storage.get_variable(
                            "ascendc_sub_super_kernel_early_start_set_flag"
                        )
                    )
                    self.assertTrue(
                        global_var_storage.get_variable(
                            "ascendc_sub_super_kernel_early_start_wait_flag"
                        )
                    )
        global_var_storage.global_storage_reset()

        # aclgraph sub combine path emits only set-side macro when only set flag is detected.
        global_var_storage.global_storage_reset()
        with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
            with buildcfg.build_config():
                with mock.patch.object(
                    asc_op_compile_base.common.context.get_context(),
                    "get_addition",
                    return_value=True,
                ):
                    compile_option_tuple = CompileOptionTuple([], [])
                    compile_info = CompileInfo()
                    compile_info.super_kernel_info = {
                        "sp_options": {"early-start": "1"}
                    }
                    compile_info.super_kernel_early_start_set_flag = True
                    compile_info.super_kernel_early_start_wait_flag = False
                    gen_sub_super_kernel_early_start_compile_options(
                        compile_option_tuple, compile_info
                    )
                    self.assertEqual(
                        compile_option_tuple.compile_options,
                        [
                            "-D__ASCENDC_SUPERKERNEL_EARLY_START_V3",
                            "-D__ASCENDC_ENABLE_SET_NEXT_TASK_START",
                        ],
                    )
                    self.assertTrue(
                        global_var_storage.get_variable(
                            "ascendc_sub_super_kernel_early_start_set_flag"
                        )
                    )
                    self.assertFalse(
                        global_var_storage.get_variable(
                            "ascendc_sub_super_kernel_early_start_wait_flag"
                        )
                    )
        global_var_storage.global_storage_reset()

        # aclgraph sub combine path emits only wait-side macro when only wait flag is detected.
        global_var_storage.global_storage_reset()
        with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
            with buildcfg.build_config():
                with mock.patch.object(
                    asc_op_compile_base.common.context.get_context(),
                    "get_addition",
                    return_value=True,
                ):
                    compile_option_tuple = CompileOptionTuple([], [])
                    compile_info = CompileInfo()
                    compile_info.super_kernel_info = {
                        "sp_options": {"early-start": "1"}
                    }
                    compile_info.super_kernel_early_start_set_flag = False
                    compile_info.super_kernel_early_start_wait_flag = True
                    gen_sub_super_kernel_early_start_compile_options(
                        compile_option_tuple, compile_info
                    )
                    self.assertEqual(
                        compile_option_tuple.compile_options,
                        [
                            "-D__ASCENDC_SUPERKERNEL_EARLY_START_V3",
                            "-D__ASCENDC_ENABLE_WAIT_PRE_TASK_END",
                        ],
                    )
                    self.assertFalse(
                        global_var_storage.get_variable(
                            "ascendc_sub_super_kernel_early_start_set_flag"
                        )
                    )
                    self.assertTrue(
                        global_var_storage.get_variable(
                            "ascendc_sub_super_kernel_early_start_wait_flag"
                        )
                    )
        global_var_storage.global_storage_reset()

        # aclgraph sub combine path inherits early-start disable from top options.
        global_var_storage.global_storage_reset()
        with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
            with buildcfg.build_config():
                with mock.patch.object(
                    asc_op_compile_base.common.context.get_context(),
                    "get_addition",
                    return_value=True,
                ):
                    compile_option_tuple = CompileOptionTuple([], [])
                    compile_info = CompileInfo()
                    compile_info.super_kernel_info = {
                        "sp_options": {"early-start": "0"}
                    }
                    compile_info.super_kernel_early_start_set_flag = True
                    compile_info.super_kernel_early_start_wait_flag = True
                    gen_sub_super_kernel_early_start_compile_options(
                        compile_option_tuple, compile_info
                    )
                    self.assertEqual(len(compile_option_tuple.compile_options), 0)
                    self.assertFalse(
                        global_var_storage.get_variable(
                            "ascendc_sub_super_kernel_early_start_set_flag"
                        )
                    )
                    self.assertFalse(
                        global_var_storage.get_variable(
                            "ascendc_sub_super_kernel_early_start_wait_flag"
                        )
                    )
        global_var_storage.global_storage_reset()

        # aclgraph debug-sync-all has higher priority than early-start.
        global_var_storage.global_storage_reset()
        with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
            with buildcfg.build_config():
                with mock.patch.object(
                    asc_op_compile_base.common.context.get_context(),
                    "get_addition",
                    return_value=True,
                ):
                    compile_option_tuple = CompileOptionTuple([], [])
                    compile_info = CompileInfo()
                    compile_info.super_kernel_info = {
                        "sp_options": {"early-start": "1", "debug-sync-all": "1"}
                    }
                    compile_info.super_kernel_early_start_set_flag = True
                    compile_info.super_kernel_early_start_wait_flag = True
                    gen_sub_super_kernel_early_start_compile_options(
                        compile_option_tuple, compile_info
                    )
                    self.assertEqual(len(compile_option_tuple.compile_options), 0)
                    self.assertFalse(
                        global_var_storage.get_variable(
                            "ascendc_sub_super_kernel_early_start_set_flag"
                        )
                    )
                    self.assertFalse(
                        global_var_storage.get_variable(
                            "ascendc_sub_super_kernel_early_start_wait_flag"
                        )
                    )
        global_var_storage.global_storage_reset()

        # super_kernel_sub_info assert
        with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
            with buildcfg.build_config():
                with mock.patch.object(
                    asc_op_compile_base.common.context.get_context(),
                    "get_addition",
                    return_value={
                        "super_kernel_count": 4,
                        "super_kernel_sub_loc": "middle",
                    },
                ):
                    compile_option_tuple = CompileOptionTuple([], [])
                    compile_info = CompileInfo()
                    self.assertRaises(
                        Exception,
                        gen_sub_super_kernel_early_start_compile_options,
                        compile_option_tuple,
                        compile_info,
                    )

    def test_split_kernel_arch_str(self):
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            s = "unsupported API".encode("utf-8")
            with mock.patch.object(
                CommonUtility, "run_cmd_ascendc", return_value=[s, 0]
            ):
                with mock.patch.object(
                    CommonUtility, "dump_build_log", return_value=[s, 0]
                ):
                    with mock.patch.object(
                        CommonUtility, "dump_compile_log", return_value=[s, 0]
                    ):
                        with mock.patch("os.path.exists", return_value=False):
                            sub_operator_kernel_name = {
                                "AiCore": {"func_name": "test", "obj_files": "./aaa.o"}
                            }
                            arch_str = AI_CORE_STR
                            split_mode = 4
                            compile_log_path = "./"
                            split_kernel_arch_str(
                                sub_operator_kernel_name,
                                arch_str,
                                split_mode,
                                compile_log_path,
                            )

                            self.assertEqual(
                                sub_operator_kernel_name[AI_CORE_STR]["obj_files"],
                                "./aaa.o",
                            )
                            self.assertEqual(
                                sub_operator_kernel_name[AI_CORE_STR][
                                    "obj_files_split1"
                                ],
                                "./kernel_meta/aaa_split1.o",
                            )
                            self.assertEqual(
                                sub_operator_kernel_name[AI_CORE_STR][
                                    "obj_files_split2"
                                ],
                                "./kernel_meta/aaa_split2.o",
                            )
                            self.assertEqual(
                                sub_operator_kernel_name[AI_CORE_STR][
                                    "obj_files_split3"
                                ],
                                "./kernel_meta/aaa_split3.o",
                            )

    def test_split_sub_kernel_objs(self):
        global_var_storage.global_storage_reset()
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            s = "unsupported API".encode("utf-8")
            with mock.patch.object(
                CommonUtility, "run_cmd_ascendc", return_value=[s, 0]
            ):
                with mock.patch.object(
                    CommonUtility, "dump_build_log", return_value=[s, 0]
                ):
                    with mock.patch.object(
                        CommonUtility, "dump_compile_log", return_value=[s, 0]
                    ):
                        dst_file = "./dst.o"
                        tiling_info = TilingInfo()
                        compile_info = CompileInfo()
                        compile_info.super_kernel_info = {
                            "sp_options": {"split-mode": 4}
                        }

                        # KERNEL_TYPE_AIV_ONLY
                        global_var_storage.set_variable(
                            "ascendc_enable_super_kernel", True
                        )
                        global_var_storage.set_variable(
                            "ascendc_sub_super_kernel_fun_names",
                            {"AiCore": {"func_name": "test", "obj_files": "./aaa.o"}},
                        )
                        global_var_storage.set_variable(
                            "ascendc_sub_super_kernel_type", "KERNEL_TYPE_AIV_ONLY"
                        )
                        split_sub_kernel_objs(dst_file, tiling_info, compile_info)

                        # KERNEL_TYPE_AIC_ONLY
                        global_var_storage.global_storage_reset()
                        global_var_storage.set_variable(
                            "ascendc_enable_super_kernel", True
                        )
                        global_var_storage.set_variable(
                            "ascendc_sub_super_kernel_fun_names",
                            {"AiCore": {"func_name": "test", "obj_files": "./aaa.o"}},
                        )
                        global_var_storage.set_variable(
                            "ascendc_sub_super_kernel_type", "KERNEL_TYPE_AIC_ONLY"
                        )
                        split_sub_kernel_objs(dst_file, tiling_info, compile_info)

                        # KERNEL_TYPE_MIX_AIC_1_2
                        global_var_storage.global_storage_reset()
                        global_var_storage.set_variable(
                            "ascendc_enable_super_kernel", True
                        )
                        global_var_storage.set_variable(
                            "ascendc_sub_super_kernel_fun_names",
                            {
                                "dav-c220-cube": {
                                    "func_name": "test_aic",
                                    "obj_files": "./aaa_aic.o",
                                },
                                "dav-c220-vec": {
                                    "func_name": "test_aiv",
                                    "obj_files": "./aaa_aiv.o",
                                },
                            },
                        )
                        global_var_storage.set_variable(
                            "ascendc_sub_super_kernel_type", "KERNEL_TYPE_MIX_AIC_1_2"
                        )
                        split_sub_kernel_objs(dst_file, tiling_info, compile_info)

        global_var_storage.global_storage_reset()

    def test_check_exist_instrinsic_when_super_kernel(self):
        data = """
# 211 "/usr/local/Ascend/CANN-7.3/x86_64-linux/tikcpp/tikcfw/impl/kernel_operator_vec_unary_intf_impl.h"
template <typename T>
[aicore] __inline__ __attribute__((always_inline)) void Ln(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const int32_t& calCount)
{

    LnImpl((__attribute__((cce_unif_buff)) T*)dstLocal.GetPhyAddr(), (__attribute__((cce_unif_buff)) T*)srcLocal.GetPhyAddr(), calCount, block_idx);
}

# 27 "/usr/local/Ascend/CANN-7.3/opp/built-in/op_impl/ai_core/tbe/impl/dynamic/../ascendc/common/../conv_forward/conv2d/../conv_common/conv_util.h"
namespace conv {
using namespace AscendC;
const static uint64_t L0A_SIZE = 65536;

static [aicore] __inline__ __attribute__((always_inline)) uint64_t AlignB(uint64_t a, uint64_t b)
{
    return ((a + b - 1) / b) * b;
}
"""
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data=data
        ) as mock_open:
            dst_i_file = "./a.i"
            check_exist_instrinsic_when_super_kernel(dst_i_file)
        data = """
# 211 "/usr/local/Ascend/CANN-7.3/x86_64-linux/tikcpp/tikcfw/impl/kernel_operator_vec_unary_intf_impl.h"
template <typename T>
[aicore] __inline__ __attribute__((always_inline)) void Ln(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const int32_t& calCount)
{

    LnImpl((__attribute__((cce_unif_buff)) T*)dstLocal.GetPhyAddr(), (__attribute__((cce_unif_buff)) T*)srcLocal.GetPhyAddr(), calCount, block_idx);
}

# 27 "/usr/local/Ascend/CANN-7.3/opp/built-in/op_impl/ai_core/tbe/impl/dynamic/../ascendc/common/../conv_forward/conv2d/../conv_common/conv_util.h"
namespace conv {
using namespace AscendC;
static [aicore] __inline__ __attribute__((always_inline)) uint64_t AlignB(uint64_t a, uint64_t b)
{
    return ((a + b - 1) / b) * b;
}
const static uint64_t L0A_SIZE = 65536 * get_block_idx();
"""
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data=data
        ) as mock_open:
            dst_i_file = "./a.i"
            self.assertRaises(
                Exception, check_exist_instrinsic_when_super_kernel, dst_i_file
            )

    def test_gen_sub_kernel_name(self):
        with asc_op_compile_base.common.context.op_context.OpContext() as ctx:
            with buildcfg.build_config():
                with mock.patch.object(
                    asc_op_compile_base.common.context.get_context(),
                    "get_addition",
                    return_value={"super_kernel_sub_loc": "middle"},
                ):
                    global_var_storage.set_variable("ascendc_enable_super_kernel", True)
                    current_kernel_name = "test"
                    arch = "AIC"
                    kernel_type = "kernel_type"
                    obj_files = "test.o"
                    kernel_name = gen_sub_kernel_name(
                        current_kernel_name, arch, kernel_type, obj_files
                    )
                    self.assertEqual(kernel_name, "test_middle")

    def test_localization_sub_op_func_sym(self):
        dst_file = "test.o"
        sub_op_kernel_info = [{"sub_kernel_names": "a"}, {"sub_kernel_names": "b"}]
        with mock.patch("subprocess.run"):
            localization_sub_op_func_sym(dst_file, sub_op_kernel_info)

    def test_sp_add_sub_op_feed_sync_all_macro(self):
        compile_option_tuple = CompileOptionTuple([], [])
        compile_info = CompileInfo()
        compile_info.super_kernel_info = {
            "sp_options": {
                "feed-sync-all": SuperKernelFeedSyncAllMode.FeedSyncAllEnable
            }
        }
        sp_add_sub_op_feed_sync_all_macro(compile_info, compile_option_tuple)
        self.assertIn(
            "-D__ASCENDC_SUPERKERNEL_AUTO_SYNC_ALL__",
            compile_option_tuple.compile_options,
        )

    def test_add_sub_super_kernel_info(self):
        compile_info = CompileInfo()
        global_var_storage.set_variable("ascendc_enable_super_kernel", True)
        compile_info.super_kernel_info = {
            "sp_options": {
                "feed-sync-all": SuperKernelFeedSyncAllMode.FeedSyncAllEnable
            }
        }
        js = {}
        static_shape_flag = True
        add_sub_super_kernel_info(js, static_shape_flag, compile_info)


if __name__ == "__main__":
    unittest.main()
