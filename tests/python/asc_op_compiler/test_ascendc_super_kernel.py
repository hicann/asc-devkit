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
import shutil
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
from asc_op_compile_base.common import register
from asc_op_compile_base.common import buildcfg
from asc_op_compile_base.asc_op_compiler.get_op_tiling import *
from asc_op_compile_base.asc_op_compiler.ascendc_common_utility import CompileInfo
from asc_op_compile_base.asc_op_compiler.compile_op import _json_except_info
from asc_op_compile_base.asc_op_compiler.super_kernel_op_infos import *
from asc_op_compile_base.asc_op_compiler.super_kernel_sub_op_infos import *
from asc_op_compile_base.asc_op_compiler.super_kernel import *
from asc_op_compile_base.asc_op_compiler.ascendc_super_kernel import (
    ascendc_super_kernel_plus,
)
from asc_op_compile_base.asc_op_compiler.super_kernel_constants import (
    SuperKernelLinkMode,
)
from asc_op_compile_base.asc_op_compiler.super_kernel_op_compile import (
    gen_super_kernel_link_obj_sequence,
)
from asc_op_compile_base.asc_op_compiler.super_kernel_utility import (
    get_wait_flag_for_chip,
)
import importlib
from asc_op_compile_base.asc_op_compiler.global_storage import global_var_storage


def SetCurrentSocInfo(soc: str):
    set_current_compile_soc_info(soc)
    global_var_storage.set_variable(
        "ascendc_short_soc_version", get_soc_spec("SHORT_SOC_VERSION")
    )


op_json = {
    "binFileName": "te_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684",
    "binFileSuffix": ".o",
    "blockDim": 36,
    "kernelName": "te_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684__kernel0",
    "sha256": "82b640327d7f75745413c68646640a172565ceed2b3c88050c53ee3f50578e3e",
    "workspace": {"num": 1, "size": [32], "type": [0]},
    "sub_operator_params": [
        "x_in__",
        "weight_scale_in__",
        "activation_scale_in__",
        "quant_scale_in__",
        "group_index_in__",
        "y_out_",
        "scale_out_",
        "workspace",
    ],
    "sub_operator_kernel_type": "KERNEL_TYPE_AIV_ONLY",
    "sub_operator_kernel_name": {
        "AiCore": {"func_name": "test", "obj_files": "test.o"},
        "dav-c220-cube": {"func_name": "test_mix_aic", "obj_files": "test_mix_aic.o"},
        "dav-c220-vec": {"func_name": "test_mix_aiv", "obj_files": "test_mix_aiv.o"},
    },
    "sub_operator_early_start_set_flag": False,
    "sub_operator_early_start_wait_flag": False,
    "timestamp_option": False,
    "debugBufSize": 0,
    "debugOptions": "",
}

op_json1 = {
    "binFileName": "te_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684",
    "binFileSuffix": ".o",
    "blockDim": 36,
    "kernelName": "te_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684__kernel0",
    "sha256": "82b640327d7f75745413c68646640a172565ceed2b3c88050c53ee3f50578e3e",
    "workspace": {"num": 1, "size": [32], "type": [0]},
    "debugOptions": "timestamp",
    "sub_operator_params": [
        "x_in__",
        "weight_scale_in__",
        "activation_scale_in__",
        "quant_scale_in__",
        "group_index_in__",
        "y_out_",
        "scale_out_",
        "workspace",
    ],
    "sub_operator_kernel_type": "KERNEL_TYPE_AIV_ONLY",
    "sub_operator_kernel_name": {
        "AiCore": {"func_name": "test", "obj_files": "test.o"},
        "dav-c220-cube": {"func_name": "test_mix_aic", "obj_files": "test_mix_aic.o"},
        "dav-c220-vec": {"func_name": "test_mix_aiv", "obj_files": "test_mix_aiv.o"},
    },
    "sub_operator_early_start_set_flag": True,
    "sub_operator_early_start_wait_flag": True,
    "timestamp_option": True,
    "debugBufSize": 78643200,
    "debugOptions": "printf",
}

B_op_json = {
    "binFileName": "B_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684",
    "binFileSuffix": ".o",
    "blockDim": 24,
    "kernelName": "B_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684__kernel0",
    "sha256": "82b640327d7f75745413c68646640a172565ceed2b3c88050c53ee3f50578e3e",
    "workspace": {"num": 1, "size": [32], "type": [0]},
    "sub_operator_params": [
        "x_in__",
        "weight_scale_in__",
        "activation_scale_in__",
        "quant_scale_in__",
        "group_index_in__",
        "y_out_",
        "scale_out_",
        "workspace",
    ],
    "sub_operator_kernel_type": "KERNEL_TYPE_MIX_AIC_1_2",
    "sub_operator_kernel_name": {
        "dynamic_func_names": {
            "1234": {
                "kernel_type": "KERNEL_TYPE_MIX_AIC_1_2",
                "dav-c220-cube": "Btest_mix_aic",
                "dav-c220-vec": "Btest_mix_aiv",
            },
            "5678": {
                "kernel_type": "KERNEL_TYPE_MIX_AIC_1_2",
                "dav-c220-cube": "Btest_mix_aic5678",
                "dav-c220-vec": "Btest_mix_aiv5678",
            },
        },
        "AiCore": {"func_name": "testC", "obj_files": "testC.o"},
        "dav-c220-cube": {
            "func_name": "B_test_mix_aic",
            "obj_files": "B_test_mix_aic.o",
        },
        "dav-c220-vec": {
            "func_name": "B_test_mix_aiv",
            "obj_files": "B_test_mix_aiv.o",
        },
    },
    "sub_operator_early_start_set_flag": False,
    "sub_operator_early_start_wait_flag": False,
}


C_op_json = {
    "binFileName": "C_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684",
    "binFileSuffix": ".o",
    "blockDim": 12,
    "kernelName": "C_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684__kernel0",
    "sha256": "82b640327d7f75745413c68646640a172565ceed2b3c88050c53ee3f50578e3e",
    "workspace": {"num": 1, "size": [32], "type": [0]},
    "sub_operator_params": [
        "x_in__",
        "weight_scale_in__",
        "activation_scale_in__",
        "quant_scale_in__",
        "group_index_in__",
        "y_out_",
        "scale_out_",
        "workspace",
    ],
    "sub_operator_kernel_type": "KERNEL_TYPE_AIV_ONLY",
    "sub_operator_kernel_name": {
        "AiCore": {"func_name": "testC", "obj_files": "testC.o"},
        "dav-c220-cube": {
            "func_name": "C_test_mix_aic",
            "obj_files": "C_test_mix_aic.o",
        },
        "dav-c220-vec": {
            "func_name": "C_test_mix_aiv",
            "obj_files": "C_test_mix_aiv.o",
        },
    },
    "sub_operator_early_start_set_flag": True,
    "sub_operator_early_start_wait_flag": True,
}

D_op_json = {
    "binFileName": "D_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684",
    "binFileSuffix": ".o",
    "blockDim": 24,
    "kernelName": "D_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684__kernel0",
    "sha256": "82b640327d7f75745413c68646640a172565ceed2b3c88050c53ee3f50578e3e",
    "workspace": {"num": 1, "size": [32], "type": [0]},
    "sub_operator_params": [
        "x_in__",
        "weight_scale_in__",
        "activation_scale_in__",
        "quant_scale_in__",
        "group_index_in__",
        "y_out_",
        "scale_out_",
        "workspace",
    ],
    "sub_operator_kernel_type": "KERNEL_TYPE_AIV_ONLY",
    "sub_operator_kernel_name": {
        "AiCore": {"func_name": "testD", "obj_files": "testD.o"},
        "dav-c220-cube": {"func_name": "test_mix_aic", "obj_files": "test_mix_aic.o"},
        "dav-c220-vec": {"func_name": "test_mix_aiv", "obj_files": "test_mix_aiv.o"},
    },
    "sub_operator_early_start_set_flag": False,
    "sub_operator_early_start_wait_flag": False,
}


F_op_json = {
    "binFileName": "F_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684",
    "binFileSuffix": ".o",
    "blockDim": 36,
    "kernelName": "F_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684__kernel0",
    "sha256": "82b640327d7f75745413c68646640a172565ceed2b3c88050c53ee3f50578e3e",
    "workspace": {"num": 1, "size": [32], "type": [0]},
    "sub_operator_params": [
        "x_in__",
        "weight_scale_in__",
        "activation_scale_in__",
        "quant_scale_in__",
        "group_index_in__",
        "y_out_",
        "scale_out_",
        "workspace",
    ],
    "sub_operator_kernel_type": "KERNEL_TYPE_AIV_ONLY",
    "sub_operator_kernel_name": {
        "AiCore": {"func_name": "testF", "obj_files": "testF.o"},
        "dav-c220-cube": {"func_name": "test_mix_aic", "obj_files": "test_mix_aic.o"},
        "dav-c220-vec": {"func_name": "test_mix_aiv", "obj_files": "test_mix_aiv.o"},
        "dynamic_func_names": {
            "1": {
                "AiCore": "AddCustom_ab1b6750d7f510985325b603cb06dc8b_1",
                "kernel_type": "KERNEL_TYPE_AIV_ONLY",
            },
            "2": {
                "AiCore": "AddCustom_ab1b6750d7f510985325b603cb06dc8b_2",
                "kernel_type": "KERNEL_TYPE_AIV_ONLY",
            },
            "3": {
                "AiCore": "AddCustom_ab1b6750d7f510985325b603cb06dc8b_3",
                "kernel_type": "KERNEL_TYPE_AIV_ONLY",
            },
        },
    },
    "sub_operator_early_start_set_flag": False,
    "sub_operator_early_start_wait_flag": False,
}

E_op_json = {
    "binFileName": "E_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684",
    "binFileSuffix": ".o",
    "blockDim": 9,
    "kernelName": "E_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684__kernel0",
    "sha256": "82b640327d7f75745413c68646640a172565ceed2b3c88050c53ee3f50578e3e",
    "workspace": {"num": 1, "size": [32], "type": [0]},
    "sub_operator_params": [
        "x_in__",
        "weight_scale_in__",
        "activation_scale_in__",
        "quant_scale_in__",
        "group_index_in__",
        "y_out_",
        "scale_out_",
        "workspace",
    ],
    "sub_operator_kernel_type": "KERNEL_TYPE_MIX_AIC_1_2",
    "sub_operator_kernel_name": {
        "AiCore": {"func_name": "testE", "obj_files": "testE.o"},
        "dav-c220-cube": {"func_name": "test_Emix_aic", "obj_files": "test_Emix_aic.o"},
        "dav-c220-vec": {"func_name": "test_Emix_aiv", "obj_files": "test_Emix_aiv.o"},
    },
    "sub_operator_early_start_set_flag": False,
    "sub_operator_early_start_wait_flag": False,
}

G_op_json = {
    "binFileName": "G_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684",
    "binFileSuffix": ".o",
    "blockDim": 36,
    "kernelName": "G_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684__kernel0",
    "sha256": "82b640327d7f75745413c68646640a172565ceed2b3c88050c53ee3f50578e3e",
    "workspace": {"num": 1, "size": [32], "type": [0]},
    "sub_operator_params": [
        "x_in__",
        "weight_scale_in__",
        "activation_scale_in__",
        "quant_scale_in__",
        "group_index_in__",
        "y_out_",
        "scale_out_",
        "workspace",
    ],
    "sub_operator_kernel_type": "KERNEL_TYPE_AIV_ONLY",
    "sub_operator_kernel_name": {
        "AiCore": {"func_name": "testG", "obj_files": "testG.o"},
        "dav-c220-cube": {"func_name": "testG_mix_aic", "obj_files": "testG_mix_aic.o"},
        "dav-c220-vec": {"func_name": "testG_mix_aiv", "obj_files": "testG_mix_aiv.o"},
    },
    "sub_operator_early_start_set_flag": False,
    "sub_operator_early_start_wait_flag": False,
}

H_op_json = {
    "binFileName": "H_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684",
    "binFileSuffix": ".o",
    "blockDim": 36,
    "kernelName": "H_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684__kernel0",
    "sha256": "82b640327d7f75745413c68646640a172565ceed2b3c88050c53ee3f50578e3e",
    "workspace": {"num": 1, "size": [32], "type": [0]},
    "sub_operator_params": [
        "x_in__",
        "weight_scale_in__",
        "activation_scale_in__",
        "quant_scale_in__",
        "group_index_in__",
        "y_out_",
        "scale_out_",
        "workspace",
    ],
    "sub_operator_kernel_type": "KERNEL_TYPE_AIV_ONLY",
    "sub_operator_kernel_name": {
        "AiCore": {"func_name": "testH", "obj_files": "testH.o"},
        "dav-c220-cube": {"func_name": "test_Hmix_aic", "obj_files": "test_Hmix_aic.o"},
        "dav-c220-vec": {"func_name": "test_Hmix_aiv", "obj_files": "test_Hmix_aiv.o"},
    },
    "sub_operator_early_start_set_flag": False,
    "sub_operator_early_start_wait_flag": False,
}

I_op_json = {
    "binFileName": "I_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684",
    "binFileSuffix": ".o",
    "blockDim": 9,
    "kernelName": "I_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684__kernel0",
    "sha256": "82b640327d7f75745413c68646640a172565ceed2b3c88050c53ee3f50578e3e",
    "workspace": {"num": 1, "size": [32], "type": [0]},
    "sub_operator_params": [
        "x_in__",
        "weight_scale_in__",
        "activation_scale_in__",
        "quant_scale_in__",
        "group_index_in__",
        "y_out_",
        "scale_out_",
        "workspace",
    ],
    "sub_operator_kernel_type": "KERNEL_TYPE_AIV_ONLY",
    "sub_operator_kernel_name": {
        "AiCore": {"func_name": "testI", "obj_files": "testI.o"},
        "dav-c220-cube": {"func_name": "test_Imix_aic", "obj_files": "test_Imix_aic.o"},
        "dav-c220-vec": {"func_name": "test_Imix_aiv", "obj_files": "test_Imix_aiv.o"},
    },
    "sub_operator_early_start_set_flag": False,
    "sub_operator_early_start_wait_flag": False,
}


J_op_json = {
    "binFileName": "J_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684",
    "binFileSuffix": ".o",
    "blockDim": 9,
    "kernelName": "J_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684__kernel0",
    "sha256": "82b640327d7f75745413c68646640a172565ceed2b3c88050c53ee3f50578e3e",
    "workspace": {"num": 1, "size": [32], "type": [0]},
    "sub_operator_params": [
        "x_in__",
        "weight_scale_in__",
        "activation_scale_in__",
        "quant_scale_in__",
        "group_index_in__",
        "y_out_",
        "scale_out_",
        "workspace",
    ],
    "sub_operator_kernel_type": "KERNEL_TYPE_AIV_ONLY",
    "sub_operator_kernel_name": {
        "AiCore": {"func_name": "testJ", "obj_files": "testJ.o"},
        "dav-c220-cube": {"func_name": "test_Jmix_aic", "obj_files": "test_Jmix_aic.o"},
        "dav-c220-vec": {"func_name": "test_Jmix_aiv", "obj_files": "test_Jmix_aiv.o"},
    },
    "sub_operator_early_start_set_flag": False,
    "sub_operator_early_start_wait_flag": False,
}

K_op_json = {
    "binFileName": "K_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684",
    "binFileSuffix": ".o",
    "blockDim": 9,
    "kernelName": "K_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684__kernel0",
    "sha256": "82b640327d7f75745413c68646640a172565ceed2b3c88050c53ee3f50578e3e",
    "workspace": {"num": 1, "size": [32], "type": [0]},
    "sub_operator_params": [
        "x_in__",
        "weight_scale_in__",
        "activation_scale_in__",
        "quant_scale_in__",
        "group_index_in__",
        "y_out_",
        "scale_out_",
        "workspace",
    ],
    "sub_operator_kernel_type": "KERNEL_TYPE_AIV_ONLY",
    "sub_operator_kernel_name": {
        "AiCore": {"func_name": "testK", "obj_files": "testK.o"},
        "dav-c220-cube": {"func_name": "tesKt_mix_aic", "obj_files": "tesKt_mix_aic.o"},
        "dav-c220-vec": {"func_name": "tesKt_mix_aiv", "obj_files": "teKt_mix_aiv.o"},
    },
    "sub_operator_early_start_set_flag": False,
    "sub_operator_early_start_wait_flag": False,
}


class TestAscendSuperKernel(unittest.TestCase):
    def setUp(self):
        # operator before each testcase
        patcher = mock.patch("subprocess.run")
        self.mock_exists = patcher.start()
        print("-------------------SetUp----------------")

    def tearDown(self):
        # operator after each testcase
        mock.patch.stopall()
        print("-------------------TearDown-------------")

    @classmethod
    def tearDownClass(cls):
        # when SuperOperatorInfos inits, dir kernel_meta and follow files/dirs will be created
        # SuperOperatorInfos.init_sub_operators() create dir "kernel_meta/{threading.get_ident()}"
        # "super_kernel{tag}_kernel.cpp" and "super_kernel{tag}.log" (tag is CommonUtility.get_kernel_meta_dir())
        generated_file_path = os.path.join(
            os.path.abspath(os.path.dirname(__file__)), "../../../kernel_meta"
        )
        if os.path.isdir(generated_file_path):
            shutil.rmtree(generated_file_path)

    def test_normalize_nop_ops(self):
        op_list = [
            {
                "bin_path": "",
                "json_path": "",
                "send_event_list": [10],
                "stream_id": 0,
                "task_type": "nop",
            },
            {
                "bin_path": "",
                "json_path": "",
                "send_event_list": [12],
                "stream_id": 0,
                "task_type": "nop",
            },
            {
                "bin_path": "op0.o",
                "json_path": "op0.json",
                "stream_id": 0,
                "task_type": "normal",
            },
            {
                "bin_path": "op1.o",
                "json_path": "op1.json",
                "stream_id": 1,
                "task_type": "normal",
            },
            {
                "bin_path": "",
                "json_path": "",
                "send_event_list": [11],
                "stream_id": 0,
                "task_type": "nop",
            },
            {
                "bin_path": "op2.o",
                "json_path": "op2.json",
                "stream_id": 1,
                "task_type": "normal",
            },
        ]

        normalized = normalize_nop_ops(op_list)

        self.assertEqual(len(normalized), 3)
        self.assertEqual(normalized[0]["notify_before_call_event_list"], [10, 12])
        self.assertEqual(normalized[0]["send_event_list"], [11])
        self.assertNotIn("send_event_list", normalized[1])
        self.assertEqual(op_list[0]["send_event_list"], [10])
        self.assertNotIn("notify_before_call_event_list", op_list[2])

    def test_normalize_nop_ops_rejects_invalid_input(self):
        invalid_op_lists = [
            [
                {
                    "json_path": "",
                    "recv_event_list": [10],
                    "stream_id": 0,
                    "task_type": "nop",
                },
                {"json_path": "op0.json", "stream_id": 0},
            ],
            [
                {"json_path": "op0.json", "stream_id": 0},
                {"json_path": "", "stream_id": 0, "task_type": "nop"},
            ],
            [
                {
                    "json_path": "",
                    "send_event_list": [10],
                    "stream_id": 1,
                    "task_type": "nop",
                },
                {"json_path": "op0.json", "stream_id": 0},
            ],
        ]

        for op_list in invalid_op_lists:
            with self.subTest(op_list=op_list):
                with self.assertRaises(Exception):
                    normalize_nop_ops(op_list)

    def test_nop_notify_before_call(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        with (
            mock.patch("builtins.open", new_callable=mock.mock_open, read_data="{}"),
            mock.patch("json.load", return_value=op_json),
            mock.patch.object(CommonUtility, "dump_compile_log"),
        ):
            sub_op = SubOperatorInfos(
                0,
                {
                    "bin_path": "op0.o",
                    "json_path": "op0.json",
                    "notify_before_call_event_list": [10],
                    "send_event_list": [11],
                    "recv_event_list": [12],
                    "stream_id": 0,
                    "task_type": "normal",
                },
                0,
                op_options,
            )
            sub_op.init_of_sub_operator_info()

        sub_op.param_offset = 2
        sub_op.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY
        sub_op.gen_notify_wait_from_outside(set(), False)

        self.assertIn("ev=10, param_offset=1", sub_op.notify_before_call_block)
        self.assertIn(
            "NotifyFunc<false>(param_base[1])", sub_op.notify_before_call_block
        )
        self.assertIn("ev=11, param_offset=10", sub_op.notify_block)
        self.assertIn("NotifyFunc<false>(param_base[10])", sub_op.notify_block)
        self.assertIn("ev=12, param_offset=11", sub_op.wait_block)
        self.assertEqual(
            sub_op.params_before_kernel,
            ["__ac_notify_lock_0_0"],
        )
        self.assertEqual(
            sub_op.extra_kernel_params,
            ["__ac_notify_lock_0_1", "__ac_wait_lock_0_0"],
        )
        sub_op.gen_notify_before_call(True)
        self.assertEqual(sub_op.get_notify_before_call_block("aic"), "")
        self.assertIn(
            "NotifyFunc<false>(param_base[1])",
            sub_op.get_notify_before_call_block("aiv"),
        )

    def test_nop_notify_before_call_is_emitted_before_real_kernel(self):
        SetCurrentSocInfo("Ascend910B1")
        kernel_infos = {
            "op_list": [
                {
                    "bin_path": "",
                    "json_path": "",
                    "send_event_list": [10],
                    "stream_id": 0,
                    "task_type": "nop",
                },
                {
                    "bin_path": "op0.o",
                    "json_path": "op0.json",
                    "stream_id": 0,
                    "task_type": "normal",
                },
            ]
        }
        with (
            mock.patch(
                "builtins.open", new_callable=mock.mock_open, read_data="{}"
            ) as json_open,
            mock.patch("json.load", return_value=op_json),
            mock.patch.object(CommonUtility, "dump_compile_log"),
            mock.patch.object(SubOperatorInfos, "extract_sub_op_bin_files"),
        ):
            super_op = SuperOperatorInfos(kernel_infos, "super_kernel")

        self.assertEqual(len(super_op.info_base), 1)
        self.assertEqual(super_op.op_list[0]["json_path"], "op0.json")
        self.assertNotIn(mock.call("", "r"), json_open.mock_calls)
        self.assertEqual(super_op.compile_info["send_event_list"], [[]])
        self.assertEqual(super_op.compile_info["notify_before_call_event_list"], [[10]])
        self.assertEqual(super_op.compile_info["notify_before_call_param_offset"], [1])
        self.assertEqual(super_op.compile_info["param_offset"], [2])
        self.assertEqual(super_op.super_kernel_params[0], "__ac_notify_lock_0_0")

        generated_file = mock.mock_open()
        with (
            mock.patch("os.open", return_value=10),
            mock.patch("os.fdopen", generated_file),
        ):
            gen_super_kernel_file(super_op)
        source = generated_file().write.call_args.args[0]
        notify_call = "NotifyFunc<false>(param_base[1])"
        sub_op = super_op.info_base[0]
        kernel_call = f"{sub_op.sub_kernel_names[0]}({sub_op.param_offset});"
        self.assertEqual(source.count(notify_call), 1)
        self.assertLess(source.index(notify_call), source.index(kernel_call))

    def test_nop_notify_offsets_follow_dynamic_extra_params(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        with (
            mock.patch("builtins.open", new_callable=mock.mock_open, read_data="{}"),
            mock.patch("json.load", return_value=op_json),
            mock.patch.object(CommonUtility, "dump_compile_log"),
        ):
            sub_op = SubOperatorInfos(
                0,
                {
                    "bin_path": "op0.o",
                    "json_path": "op0.json",
                    "notify_before_call_event_list": [10],
                    "send_event_list": [11],
                    "recv_event_list": [12],
                    "stream_id": 0,
                    "task_type": "dynamic",
                },
                0,
                op_options,
            )
            sub_op.init_of_sub_operator_info()

        dynamic_params = ["tiling_key", "block_num", "wait_lock"]
        sub_op.param_offset = 2
        sub_op.extra_kernel_params = list(dynamic_params)
        sub_op.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY
        sub_op.gen_notify_wait_from_outside(set(), False)

        self.assertIn(
            "NotifyFunc<false>(param_base[1])", sub_op.notify_before_call_block
        )
        self.assertIn("NotifyFunc<false>(param_base[13])", sub_op.notify_block)
        self.assertIn("WaitFunc<false>(param_base[14])", sub_op.wait_block)
        self.assertEqual(
            sub_op.extra_kernel_params,
            dynamic_params + ["__ac_notify_lock_0_1", "__ac_wait_lock_0_0"],
        )

    def test_non_nop_compile_info_remains_unchanged(self):
        SetCurrentSocInfo("Ascend910B1")
        kernel_infos = {
            "op_list": [
                {
                    "bin_path": "op0.o",
                    "json_path": "op0.json",
                    "send_event_list": [11],
                    "stream_id": 0,
                }
            ]
        }
        with (
            mock.patch("builtins.open", new_callable=mock.mock_open, read_data="{}"),
            mock.patch("json.load", return_value=op_json),
            mock.patch.object(CommonUtility, "dump_compile_log"),
            mock.patch.object(SubOperatorInfos, "extract_sub_op_bin_files"),
        ):
            super_op = SuperOperatorInfos(kernel_infos, "super_kernel")

        self.assertEqual(super_op.compile_info["param_offset"], [1])
        self.assertEqual(super_op.compile_info["notify_param_offset"], [9])
        self.assertEqual(super_op.compile_info["send_event_list"], [[11]])
        self.assertNotIn("notify_before_call_event_list", super_op.compile_info)
        self.assertNotIn("notify_before_call_param_offset", super_op.compile_info)

    def test_nop_notify_offsets_preserve_following_operator_params(self):
        SetCurrentSocInfo("Ascend910B1")
        kernel_infos = {
            "op_list": [
                {
                    "bin_path": "",
                    "json_path": "",
                    "send_event_list": [10],
                    "stream_id": 0,
                    "task_type": "nop",
                },
                {
                    "bin_path": "op0.o",
                    "json_path": "op0.json",
                    "send_event_list": [11],
                    "stream_id": 0,
                    "task_type": "normal",
                },
                {
                    "bin_path": "op1.o",
                    "json_path": "op1.json",
                    "stream_id": 0,
                    "task_type": "normal",
                },
            ]
        }
        with (
            mock.patch("builtins.open", new_callable=mock.mock_open, read_data="{}"),
            mock.patch("json.load", return_value=op_json),
            mock.patch.object(CommonUtility, "dump_compile_log"),
            mock.patch.object(SubOperatorInfos, "extract_sub_op_bin_files"),
        ):
            super_op = SuperOperatorInfos(kernel_infos, "super_kernel")

        self.assertEqual(super_op.compile_info["send_event_list"], [[11], []])
        self.assertEqual(
            super_op.compile_info["notify_before_call_event_list"], [[10], []]
        )
        self.assertEqual(super_op.compile_info["notify_param_offset"], [10, 19])
        self.assertEqual(
            super_op.compile_info["notify_before_call_param_offset"], [1, 0]
        )
        self.assertEqual(super_op.compile_info["param_offset"], [2, 11])
        self.assertEqual(
            super_op.super_kernel_params[:2],
            ["__ac_notify_lock_0_0", "x_in___0"],
        )
        self.assertEqual(super_op.super_kernel_params[9], "__ac_notify_lock_0_1")

        compile_info = CompileInfo()
        compile_info.super_kernel_info = super_op.compile_info
        with (
            mock.patch("builtins.open", new_callable=mock.mock_open, read_data="{}"),
            mock.patch("json.load", return_value=op_json),
        ):
            dfx_info = _json_except_info(compile_info)
        first_sub_op = next(iter(dfx_info["kernelList"].values()))[0]
        self.assertEqual(first_sub_op["notify_before_call_event_list"], [10])
        self.assertEqual(first_sub_op["arg_list"]["notify_before_call_param_offset"], 1)

        generated_file = mock.mock_open()
        with (
            mock.patch("os.open", return_value=10),
            mock.patch("os.fdopen", generated_file),
        ):
            gen_super_kernel_file(super_op)
        source = generated_file().write.call_args.args[0]
        self.assertIn("NotifyFunc<false>(param_base[1])", source)
        self.assertIn("NotifyFunc<false>(param_base[10])", source)

    def test_ascendc_super_kernel_plus(self):
        with mock.patch(
            "asc_op_compile_base.asc_op_compiler.super_kernel.compile_super_kernel"
        ):
            with mock.patch(
                "builtins.open", new_callable=mock.mock_open, read_data="{}"
            ) as mock_open:
                with mock.patch("json.load", return_value=op_json):
                    with mock.patch.object(
                        CommonUtility, "is_support_super_kernel", return_value=True
                    ):
                        ascendc_super_kernel_plus(
                            {
                                "op_list": [
                                    {
                                        "bin_path": "op1.o",
                                        "json_path": "op1.json",
                                        "kernel_name": "op1",
                                    },
                                ]
                            },
                            "super_kernel",
                        )
        with mock.patch(
            "asc_op_compile_base.asc_op_compiler.super_kernel.compile_super_kernel"
        ):
            with mock.patch(
                "builtins.open", new_callable=mock.mock_open, read_data="{}"
            ) as mock_open:
                with mock.patch("json.load", return_value=op_json1):
                    with mock.patch.object(
                        CommonUtility, "is_support_super_kernel", return_value=True
                    ):
                        ascendc_super_kernel_plus(
                            {
                                "op_list": [
                                    {
                                        "bin_path": "op1.o",
                                        "json_path": "op1.json",
                                        "kernel_name": "op1",
                                    },
                                ]
                            },
                            "super_kernel",
                        )
        self.assertRaises(
            Exception,
            ascendc_super_kernel_plus,
            {
                "op_list": [
                    {
                        "bin_path": "op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                    },
                ]
            },
            "super_kernel",
        )

    def test_extract_sub_op_bin_files(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with (
            mock.patch(
                "asc_op_compile_base.asc_op_compiler.super_kernel.compile_super_kernel"
            ),
            mock.patch("builtins.open", new_callable=mock.mock_open, read_data="{}"),
            mock.patch("json.load", return_value=op_json),
            mock.patch("os.fdopen"),
        ):
            with open(os.devnull, "a") as file:
                with mock.patch("os.open", return_value=file):
                    with mock.patch("subprocess.run"):
                        tmp = SubOperatorInfos(
                            0,
                            {
                                "bin_path": "./op1.o",
                                "json_path": "op1.json",
                                "kernel_name": "op1",
                            },
                            100,
                            op_options,
                            compile_log_path,
                        )
                        tmp.init_of_sub_operator_info()
                        tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY
                        tmp.extract_sub_op_bin_files()
                        self.assertEqual(tmp.aic_text_len, 0)
                        tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2
                        tmp.extract_sub_op_bin_files()
                        self.assertEqual("test_mix_aic.o" in tmp.aic_bin, True)

    def test_gen_sub_kernel_declare_and_call_func(self):
        SetCurrentSocInfo("Ascend910B1")
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with (
            mock.patch(
                "asc_op_compile_base.asc_op_compiler.super_kernel.compile_super_kernel"
            ),
            mock.patch("builtins.open", new_callable=mock.mock_open, read_data="{}"),
            mock.patch("json.load", return_value=op_json1),
            mock.patch("os.fdopen"),
        ):
            with open(os.devnull, "a") as file:
                with mock.patch("os.open", return_value=file):
                    with mock.patch("subprocess.run"):
                        tmp = SubOperatorInfos(
                            0,
                            {
                                "bin_path": "./op1.o",
                                "json_path": "op1.json",
                                "kernel_name": "op1",
                            },
                            100,
                            op_options,
                            compile_log_path,
                        )
                        tmp.init_of_sub_operator_info()
                        tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY
                        tmp.gen_sub_kernel_declare_and_call_func()
                        self.assertEqual(tmp.sub_kernel_names, ["test"])
                        tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0
                        tmp.gen_sub_kernel_declare_and_call_func()
                        self.assertEqual(tmp.sub_kernel_names, ["test", "test_mix_aiv"])
                        tmp.sub_kernel_names = []
                        tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0
                        tmp.gen_sub_kernel_declare_and_call_func()
                        self.assertEqual(tmp.sub_kernel_names, ["test_mix_aic"])
                        tmp.sub_kernel_names = []
                        tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
                        tmp.gen_sub_kernel_declare_and_call_func()
                        self.assertEqual(
                            tmp.sub_kernel_names, ["test_mix_aic", "test_mix_aiv"]
                        )
                        tmp.sub_kernel_names = []
                        tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2
                        tmp.gen_sub_kernel_declare_and_call_func()
                        self.assertEqual(
                            tmp.sub_kernel_names, ["test_mix_aic", "test_mix_aiv"]
                        )
                        tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MAX
                        self.assertRaises(
                            Exception, tmp.gen_sub_kernel_declare_and_call_func
                        )

    def test_get_summary_type_and_options(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                tmp = SuperOperatorInfos(
                    {
                        "op_list": [
                            {
                                "bin_path": "op1.o",
                                "json_path": "op1.json",
                                "kernel_name": "op1",
                                "timestamp_option": True,
                                "debug_option": "printf",
                                "debug_size": 1024,
                            },
                        ]
                    },
                    "super_kernel",
                )
                sub_tmp1 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                tmp.info_base = [sub_tmp1]

                sub_tmp1.timestamp_option = True
                sub_tmp1.debug_option = "printf"
                sub_tmp1.debug_size = 78643200
                tmp.get_summary_type_and_options()
                self.assertEqual(tmp.debug_option, "printf")
                self.assertEqual(tmp.debug_size, 78643200)

                sub_tmp1.timestamp_option = True
                sub_tmp1.debug_option = "assert"
                sub_tmp1.debug_size = 78643200
                tmp.get_summary_type_and_options()
                self.assertEqual(tmp.debug_option, "printf,assert")
                self.assertEqual(tmp.debug_size, 78643200)

                sub_tmp1.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY
                tmp.get_summary_type_and_options()
                self.assertEqual(
                    tmp.kernel_type, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0
                )

                sub_tmp1.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0
                tmp.get_summary_type_and_options()
                self.assertEqual(
                    tmp.kernel_type, SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0
                )

                sub_tmp1.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0
                tmp.get_summary_type_and_options()
                self.assertEqual(
                    tmp.kernel_type, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0
                )

                sub_tmp1.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
                tmp.get_summary_type_and_options()
                self.assertEqual(
                    tmp.kernel_type, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
                )

                sub_tmp1.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2
                tmp.get_summary_type_and_options()
                self.assertEqual(
                    tmp.kernel_type, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
                )

    def test_get_text_section_size(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        text_section_value = """
Sections:
Idx Name                              Size     VMA              Type
  0                                   00000000 0000000000000000
  1 .strtab                           0000020b 0000000000000000
  2 .text                             00001270 0000000000000000 TEXT
  3 .rela.text                        00000108 0000000000000000
  4 .ascend.stack.size.record         00000008 0000000000000000
  5 .group                            00000008 0000000000000000
  6 .bl.uninit.g_sysWorkspaceReserved 00000008 0000000000000000 DATA
  7 .group                            00000008 0000000000000000
  8 .bl.uninit.g_vecTPipePtr          00000008 0000000000000000 DATA
  9 .group                            00000008 0000000000000000
 10 .bl.uninit.g_deqValue             00000002 0000000000000000 DATA
 11 .comment                          00000030 0000000000000000
 12 .note.GNU-stack                   00000000 0000000000000000
 13 .llvm_addrsig                     00000000 0000000000000000
 14 .symtab                           00000138 0000000000000000
"""

        class ReturnResult:
            def __init__(self):
                self.returncode = 0
                self.stdout = text_section_value

        result = ReturnResult()
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                tmp = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                with mock.patch("subprocess.run", return_value=result):
                    size = tmp.get_text_section_size("op1.o")
                    self.assertEqual(size, 4720)
                result.stdout = ""
                with mock.patch("subprocess.run", return_value=result):
                    self.assertRaises(Exception, tmp.get_text_section_size, "op1.o")

    def test_extract_sub_bin_file_of_mix_kernel(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                tmp = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                with mock.patch("subprocess.run") as mock_run:
                    mock_run.side_effect = RuntimeError()
                    tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY
                    self.assertRaises(
                        Exception,
                        tmp.extract_sub_bin_file_of_mix_kernel,
                        "./",
                        "b",
                        "c",
                    )

    def test_gen_super_kernel_file(self):
        SetCurrentSocInfo("Ascend950PR_9599")
        with mock.patch(
            "asc_op_compile_base.asc_op_compiler.super_kernel.compile_super_kernel"
        ):
            with mock.patch(
                "builtins.open", new_callable=mock.mock_open, read_data="{}"
            ) as mock_open:
                with mock.patch("json.load", return_value=op_json):
                    with mock.patch("os.fdopen") as mock_fdopen:
                        super_operator = SuperOperatorInfos(
                            {
                                "op_list": [
                                    {
                                        "bin_path": "op1.o",
                                        "json_path": "op1.json",
                                        "kernel_name": "op1",
                                    },
                                ]
                            },
                            "super_kernel",
                        )
                        super_operator.datacache_mpde = (
                            SuperKernelDataCacheMode.DataCacheLoadAdancanceStep
                        )
                        super_operator.preload_mode = (
                            SuperKernelPreLoadMode.PreLoadByWhole
                        )
                        super_operator.datacache_mode = (
                            SuperKernelDataCacheMode.DataCacheLoadAdancanceStep
                        )
                        super_operator.timestamp_option = True
                        mock_fdopen.side_effect = Exception("error")
                        self.assertRaises(
                            Exception, gen_super_kernel_file, super_operator
                        )

    def test_ascendc_super_kernel_plus_multi_ops(self):
        with (
            mock.patch(
                "asc_op_compile_base.asc_op_compiler.super_kernel.compile_super_kernel"
            ),
            mock.patch.object(SubOperatorInfos, "extract_sub_bin_file_of_mix_kernel"),
        ):
            mock_returns = [
                B_op_json,
                C_op_json,
                D_op_json,
                F_op_json,
                E_op_json,
                G_op_json,
                H_op_json,
                I_op_json,
                J_op_json,
                K_op_json,
            ]
            with mock.patch(
                "builtins.open", new_callable=mock.mock_open, read_data="{}"
            ) as mock_open:
                with mock.patch.object(
                    CommonUtility, "is_support_super_kernel", return_value=True
                ):
                    with mock.patch("json.load", side_effect=mock_returns):
                        ascendc_super_kernel_plus(
                            {
                                "op_list": [
                                    {
                                        # B dynamic mix
                                        "bin_path": "./te_dynamicquant_2eccb33cf065efcdff327c523c0c8beeeb5be0c9a15f499c09df0de6cec5c03f.o",
                                        "json_path": "./te_dynamicquant_2eccb33cf065efcdff327c523c0c8beeeb5be0c9a15f499c09df0de6cec5c03f.json",
                                        "send_event_list": [46, 50],
                                        "stream_id": 15,
                                        "task_type": "normal",
                                    },
                                    {
                                        # C Vector
                                        "bin_path": "./te_quantbatchmatmulv3_a4527ebe71488258a04ac5e0770a52a4383207a648eb4d4c270186c516a8ff1c.o",
                                        "json_path": "./te_quantbatchmatmulv3_a4527ebe71488258a04ac5e0770a52a4383207a648eb4d4c270186c516a8ff1c.json",
                                        "recv_event_list": [46],
                                        "stream_id": 0,
                                        "task_type": "normal",
                                    },
                                    {
                                        # D Vector
                                        "bin_path": "/root/atc_data/kernel_cache/Ascend910_9382/te_addrmsnormcast_a7c5214a7871e3ceca1201342d3601f8e8473d56ca0c995988448ff23818f78a.o",
                                        "json_path": "/root/atc_data/kernel_cache/Ascend910_9382/te_addrmsnormcast_a7c5214a7871e3ceca1201342d3601f8e8473d56ca0c995988448ff23818f78a.json",
                                        "recv_event_list": [50],
                                        "send_event_list": [49],
                                        "stream_id": 22,
                                        "task_type": "normal",
                                    },
                                    {
                                        # F
                                        "bin_path": "./te_matmulv3_cb85c153f28c9aad28d1e8b9d718a7717764ca4af6735fcf10ab76a1230447c7.o",
                                        "json_path": "./te_matmulv3_cb85c153f28c9aad28d1e8b9d718a7717764ca4af6735fcf10ab76a1230447c7.json",
                                        "stream_id": 9,
                                        "task_type": "dynamic",
                                    },
                                    {
                                        # E
                                        "bin_path": "/root/atc_data/kernel_cache/Ascend910_9382/te_moegatingtopk_ced16b14837e64bc1b276aaf196502499ee35a00c3e1122f624508ae92a5a6b1.o",
                                        "json_path": "/root/atc_data/kernel_cache/Ascend910_9382/te_moegatingtopk_ced16b14837e64bc1b276aaf196502499ee35a00c3e1122f624508ae92a5a6b1.json",
                                        "send_event_list": [1073742088],
                                        "stream_id": 9,
                                        "task_type": "normal",
                                    },
                                    {
                                        # G
                                        "bin_path": "./te_moedistributedispatch_2146895dbfe06a6712f640d98a87cc66592805ff077481a37674b1734700a0d2.o",
                                        "json_path": "./te_moedistributedispatch_2146895dbfe06a6712f640d98a87cc66592805ff077481a37674b1734700a0d2.json",
                                        "stream_id": 22,
                                        "task_type": "normal",
                                    },
                                ],
                                "super_kernel_options": "stream-fusion=1",
                            },
                            "super_kernel",
                        )

    def test_gen_notify_wait_from_outside(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with (
                mock.patch("json.load", return_value=op_json),
                mock.patch.object(CommonUtility, "dump_compile_log"),
            ):
                tmp = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                        "send_event_list": [46, 50],
                        "recv_event_list": [46, 51],
                        "stream_id": 15,
                        "task_type": "normal",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                tmp.init_of_sub_operator_info()
                tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY
                tmp.gen_notify_wait_from_outside([50, 51], True)
                self.assertEqual(len(tmp.notify_block), 2)
                tmp.gen_notify_wait_from_outside([50, 51], False)
                self.assertNotEqual(len(tmp.notify_block), 1)
                tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY
                tmp.gen_notify_wait_from_outside([50, 51], False)
                self.assertNotEqual(len(tmp.notify_block), 1)

    def test_init_of_sub_operator_info(self):
        op_json_early_start = {
            "binFileName": "te_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684",
            "binFileSuffix": ".o",
            "blockDim": 24,
            "kernelName": "te_dequantswigluquant_e22b2f2842ce6848f5485a744deaefe5c0c8aec04b6bf8e727854abd4c5be684__kernel0",
            "sha256": "82b640327d7f75745413c68646640a172565ceed2b3c88050c53ee3f50578e3e",
            "workspace": {"num": 1, "size": [32], "type": [0]},
            "sub_operator_params": ["x_in__"],
            "sub_operator_kernel_type": "KERNEL_TYPE_AIV_ONLY",
            "sub_operator_kernel_name": {
                "AiCore": {"func_name": "test", "obj_files": "test.o"}
            },
            "split_mode": 4,
            "sub_operator_early_start_set_flag": True,
            "sub_operator_early_start_wait_flag": False,
        }
        op_options = {
            "split-mode": 3,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
            "early-start": SuperKernelEarlyStartMode.EarlyStartDisable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json_early_start):
                tmp = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                        "send_event_list": [46, 50],
                        "recv_event_list": [46, 51],
                        "stream_id": 15,
                        "task_type": "normal",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                self.assertRaises(Exception, tmp.init_of_sub_operator_info)
        op_json_early_start["sub_operator_early_start_set_flag"] = False
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json_early_start):
                tmp = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                        "send_event_list": [46, 50],
                        "recv_event_list": [46, 51],
                        "stream_id": 15,
                        "task_type": "normal",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                self.assertRaises(Exception, tmp.init_of_sub_operator_info)
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json_early_start):
                mock_open.side_effect = RuntimeError()
                tmp = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                        "send_event_list": [46, 50],
                        "recv_event_list": [46, 51],
                        "stream_id": 15,
                        "task_type": "normal",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                self.assertRaises(Exception, tmp.init_of_sub_operator_info)

    def test_gen_switch_case_block_of_dynamic_op(self):
        SetCurrentSocInfo("Ascend910B1")
        op_options = {
            "split-mode": 3,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
            "early-start": SuperKernelEarlyStartMode.EarlyStartDisable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                tmp = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                        "send_event_list": [46, 50],
                        "recv_event_list": [46, 51],
                        "stream_id": 15,
                        "task_type": "normal",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                tmp.init_of_sub_operator_info()
                kernel_info_of_tiling_key = {
                    "AiCore": "AddCustom_ab1b6750d7f510985325b603cb06dc8b_2",
                    "dav-c220-vec": "add_vec",
                    "dav-c220-cube": "add_cube",
                    "kernel_type": "KERNEL_TYPE_AIV_ONLY",
                }
                tiling_key = 1
                kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY
                tmp.gen_switch_case_block_of_dynamic_op(
                    kernel_info_of_tiling_key, tiling_key, kernel_type
                )
                kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0
                tmp.gen_switch_case_block_of_dynamic_op(
                    kernel_info_of_tiling_key, tiling_key, kernel_type
                )
                kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0
                tmp.gen_switch_case_block_of_dynamic_op(
                    kernel_info_of_tiling_key, tiling_key, kernel_type
                )
                kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
                tmp.gen_switch_case_block_of_dynamic_op(
                    kernel_info_of_tiling_key, tiling_key, kernel_type
                )
                kernel_type = SuperKernelKernelType.KERNEL_TYPE_MAX
                self.assertRaises(
                    Exception,
                    tmp.gen_switch_case_block_of_dynamic_op,
                    kernel_info_of_tiling_key,
                    tiling_key,
                    kernel_type,
                )

    def test_dynamic_gen_split_call_code(self):
        op_options = {}
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                tmp = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                        "send_event_list": [46, 50],
                        "recv_event_list": [46, 51],
                        "stream_id": 15,
                        "task_type": "normal",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                tmp.init_of_sub_operator_info()
                tmp.split_mode = 1
                res = tmp.dynamic_gen_split_call_code("test", "x, y")
                self.assertTrue(len(res) > 1, True)

    def test_gen_dynamic_op_call_func(self):
        op_options = {
            "split-mode": 3,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
            "early-start": SuperKernelEarlyStartMode.EarlyStartDisable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=F_op_json):
                tmp = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                        "send_event_list": [46, 50],
                        "recv_event_list": [46, 51],
                        "stream_id": 15,
                        "task_type": "dynamic",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                tmp.init_of_sub_operator_info()
                tmp.early_start_set_flag = True
                self.assertRaises(Exception, tmp.gen_dynamic_op_call_func)
                tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY
                tmp.early_start_set_flag = False
                with mock.patch(
                    "asc_op_compile_base.common.platform.platform_info.get_soc_spec",
                    {"ai_core_cnt": 10},
                ):
                    tmp.process_of_dynamic_op(True)
                    self.assertNotEqual(tmp.block_num, 9999)

    def test_extract_sub_bin_file(self):
        op_options = {
            "split-mode": 1,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
            "early-start": SuperKernelEarlyStartMode.EarlyStartDisable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with (
                mock.patch("json.load", return_value=F_op_json),
                mock.patch("os.chdir"),
                mock.patch("subprocess.run"),
                mock.patch.object(CommonUtility, "dump_compile_log"),
            ):
                tmp = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                        "send_event_list": [46, 50],
                        "recv_event_list": [46, 51],
                        "stream_id": 15,
                        "task_type": "dynamic",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                tmp.init_of_sub_operator_info()
                tmp.extract_sub_bin_file("./tmp/kernel_meta/", "bin.o")
                tmp.split_mode_in_json = 1
                tmp.extract_sub_op_bin_files()
                self.assertTrue("testF.o" in tmp.aiv_bin, True)
                tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY
                tmp.extract_sub_op_bin_files()
                self.assertTrue("testF.o" in tmp.aic_bin, True)

                tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0
                tmp.extract_sub_op_bin_files()
                self.assertTrue("test_mix_aiv.o" in tmp.aiv_bin, True)
                tmp.split_mode_in_json = None
                tmp.extract_sub_op_bin_files()
                self.assertTrue("op1.o" in tmp.aiv_bin, True)

                tmp.split_mode_in_json = 1
                tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0
                tmp.extract_sub_op_bin_files()
                self.assertTrue("test_mix_aic.o" in tmp.aic_bin, True)
                tmp.split_mode_in_json = None
                tmp.extract_sub_op_bin_files()
                self.assertTrue("op1.o" in tmp.aic_bin, True)

    def test_gen_early_start_complement_func(self):
        op_options = {
            "split-mode": 1,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
            "early-start": SuperKernelEarlyStartMode.EarlyStartEnableV1,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with (
                mock.patch("json.load", return_value=F_op_json),
                mock.patch("os.chdir"),
                mock.patch("subprocess.run"),
                mock.patch.object(CommonUtility, "dump_compile_log"),
            ):
                tmp = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                        "stream_id": 15,
                        "task_type": "dynamic",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                res = tmp.gen_early_start_complement_func(
                    "ASCEND_IS_AIV", "block_idx < blockDim", True
                )
                self.assertIn("AscendC::SetNextTaskStart();", res)

    def test_get_task_type(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                tmp = SuperOperatorInfos(
                    {
                        "op_list": [
                            {
                                "bin_path": "op1.o",
                                "json_path": "op1.json",
                                "kernel_name": "op1",
                            },
                        ]
                    },
                    "super_kernel",
                )
                sub_tmp1 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                sub_tmp2 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                sub_tmp1.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY

                res = tmp.get_task_type(sub_tmp1)
                self.assertEqual(res, "cub")
                sub_tmp1.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
                sub_tmp2.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
                res = tmp.gen_sync_name(sub_tmp1, sub_tmp2)
                self.assertEqual(res, "cub:vec;vec:cub")
                sub_tmp2.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0
                res = tmp.gen_sync_name(sub_tmp1, sub_tmp2)
                self.assertEqual(res, "vec:cub")
                sub_tmp1.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY
                sub_tmp2.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
                res = tmp.gen_sync_name(sub_tmp1, sub_tmp2)
                self.assertEqual(res, "cub:vec")

    def test_insert_sync_by_event(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                tmp = SuperOperatorInfos(
                    {
                        "op_list": [
                            {
                                "bin_path": "op1.o",
                                "json_path": "op1.json",
                                "kernel_name": "op1",
                            },
                        ]
                    },
                    "super_kernel",
                )
                sub_tmp1 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                sub_tmp1.send_event_list = [1]
                sub_tmp1.recv_event_list = [1]
                sub_tmp1.recv_info = {"op_test": "cub:vec"}
                sub_tmp1.send_info = {"op_test": "cub:vec"}
                sub_tmp1.kernel_name = "op1"
                sub_tmp1.kernel_name_for_multi_stream = "op1_0"
                tmp.info_base = [sub_tmp1]
                self.assertRaises(Exception, tmp.insert_sync_by_event)
                tmp.remove_info_by_name("op1_0", "op_test", True, "vec:cub")
                tmp.remove_info_by_name("op1_0", "op_test", False, "vec:cub")
                tmp.get_remain_events("vec:cub;cub:vec", "cub:vec")
                tmp.get_idx("op2_1", True)
                self.assertEqual(sub_tmp1.recv_info["op_test"], "vec:cub")
                self.assertEqual(sub_tmp1.send_info["op_test"], "vec:cub")

    def test_judge_remove(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                with mock.patch.object(SuperOperatorInfos, "get_idx") as mock_idx:
                    mock_idx.side_effect = [0, 4, 1, 3]
                    tmp = SuperOperatorInfos(
                        {
                            "op_list": [
                                {
                                    "bin_path": "op1.o",
                                    "json_path": "op1.json",
                                    "kernel_name": "op1",
                                },
                            ]
                        },
                        "super_kernel",
                    )
                    sub_tmp1 = SubOperatorInfos(
                        0,
                        {
                            "bin_path": "./op1.o",
                            "json_path": "op1.json",
                            "kernel_name": "op1",
                        },
                        100,
                        op_options,
                        compile_log_path,
                    )
                    sub_tmp1.recv_info = {"op1": "cub:vec"}
                    tmp.info_base = [sub_tmp1]
                    tmp.vec_op_list = [sub_tmp1, sub_tmp1, sub_tmp1, sub_tmp1, sub_tmp1]
                    res = tmp.judge_remove("op1", "op2", True)
                    self.assertEqual(res, True)
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                with mock.patch.object(SuperOperatorInfos, "get_idx") as mock_idx:
                    mock_idx.side_effect = [0, 4, 1, 3]
                    tmp = SuperOperatorInfos(
                        {
                            "op_list": [
                                {
                                    "bin_path": "op1.o",
                                    "json_path": "op1.json",
                                    "kernel_name": "op1",
                                },
                            ]
                        },
                        "super_kernel",
                    )
                    sub_tmp1 = SubOperatorInfos(
                        0,
                        {
                            "bin_path": "./op1.o",
                            "json_path": "op1.json",
                            "kernel_name": "op1",
                        },
                        100,
                        op_options,
                        compile_log_path,
                    )
                    sub_tmp1.recv_info = {"op1": "vec:cub"}
                    tmp.info_base = [sub_tmp1]
                    tmp.cub_op_list = [sub_tmp1, sub_tmp1, sub_tmp1, sub_tmp1, sub_tmp1]
                    res = tmp.judge_remove("op1", "op2", False)
                    self.assertEqual(res, True)

    def test_remove_crossed_line_sync(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                with mock.patch.object(
                    SuperOperatorInfos, "judge_remove", return_value=True
                ):
                    tmp = SuperOperatorInfos(
                        {
                            "op_list": [
                                {
                                    "bin_path": "op1.o",
                                    "json_path": "op1.json",
                                    "kernel_name": "op1",
                                },
                            ]
                        },
                        "super_kernel",
                    )
                    sub_tmp1 = SubOperatorInfos(
                        0,
                        {
                            "bin_path": "./op1.o",
                            "json_path": "op1.json",
                            "kernel_name": "op1",
                        },
                        100,
                        op_options,
                        compile_log_path,
                    )
                    sub_tmp1.send_info = {"op1": "cub:vec"}
                    tmp.info_base = [sub_tmp1]
                    tmp.cub_op_list = [sub_tmp1, sub_tmp1, sub_tmp1, sub_tmp1, sub_tmp1]
                    tmp.remove_crossed_line_sync()
                    tmp.vec_op_list = [sub_tmp1, sub_tmp1, sub_tmp1, sub_tmp1, sub_tmp1]
                    sub_tmp1.send_info = {"op1": "vec:cub"}
                    tmp.remove_crossed_line_sync()

    def test_remove_multi_send_info(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                with mock.patch.object(SuperOperatorInfos, "get_idx") as mock_idx:
                    mock_idx.side_effect = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
                    tmp = SuperOperatorInfos(
                        {
                            "op_list": [
                                {
                                    "bin_path": "op1.o",
                                    "json_path": "op1.json",
                                    "kernel_name": "op1",
                                },
                            ]
                        },
                        "super_kernel",
                    )
                    sub_tmp1 = SubOperatorInfos(
                        0,
                        {
                            "bin_path": "./op1.o",
                            "json_path": "op1.json",
                            "kernel_name": "op1",
                        },
                        100,
                        op_options,
                        compile_log_path,
                    )
                    sub_tmp1.send_info = {"op1": "vec:cub", "op2": "vec:cub"}
                    sub_tmp1.recv_info = {"op1": "cub:vec", "op2": "cub:vec"}
                    tmp.info_base = [sub_tmp1]
                    tmp.vec_op_list = [sub_tmp1]
                    tmp.cub_op_list = [sub_tmp1]
                    tmp.remove_multi_send_info()
                    tmp.remove_multi_recv_info()
                    sub_tmp1.send_info = {"op1": "cub:vec", "op2": "cub:vec"}
                    sub_tmp1.recv_info = {"op1": "vec:cub", "op2": "vec:cub"}
                    tmp.remove_multi_send_info()
                    tmp.remove_multi_recv_info()

    def test_creat_compile_log(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        super_kernel_op_infos_module = importlib.import_module(
            "asc_op_compile_base.asc_op_compiler.super_kernel_op_infos"
        )
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                with mock.patch.object(
                    super_kernel_op_infos_module,
                    "get_op_debug_config",
                    return_value=["dump_cce"],
                ):
                    tmp = SuperOperatorInfos(
                        {
                            "op_list": [
                                {
                                    "bin_path": "op1.o",
                                    "json_path": "op1.json",
                                    "kernel_name": "op1",
                                },
                            ]
                        },
                        "super_kernel",
                    )

                    tmp.find_all_inner_event_id_set()
                    sub_tmp1 = SubOperatorInfos(
                        0,
                        {
                            "bin_path": "./op1.o",
                            "json_path": "op1.json",
                            "kernel_name": "op1",
                        },
                        100,
                        op_options,
                        compile_log_path,
                    )
                    sub_tmp1.send_event_list = [10]
                    sub_tmp1.recv_event_list = [10]
                    tmp.info_base = [sub_tmp1, sub_tmp1, sub_tmp1]
                    self.assertRaises(Exception, tmp.find_all_inner_event_id_set)
                    tmp.creat_compile_log()
                    self.assertIn("super_kernel", tmp.compile_log_path)

    def test_get_summary_type_and_options_1(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                tmp = SuperOperatorInfos(
                    {
                        "op_list": [
                            {
                                "bin_path": "op1.o",
                                "json_path": "op1.json",
                                "kernel_name": "op1",
                            },
                        ]
                    },
                    "super_kernel",
                )
                sub_tmp1 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                sub_tmp2 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                sub_tmp1.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2
                sub_tmp2.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
                sub_tmp2.block_num = 2
                sub_tmp1.block_num = 1
                tmp.info_base = [sub_tmp1, sub_tmp2]
                tmp.get_summary_type_and_options()
                self.assertTrue(tmp.block_num, 1)

    def test_check_dcci_before_after_op_options(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                super_op = SuperOperatorInfos(
                    {
                        "op_list": [
                            {
                                "bin_path": "op1.o",
                                "json_path": "op1.json",
                                "kernel_name": "op1",
                            },
                        ]
                    },
                    "super_kernel",
                )

                sub_op_dcci_option_list = [
                    {
                        "sub_op_seq": (False, True, True, True, False),
                        "exp_res": [(1, 2, 3)],
                    },
                    {
                        "sub_op_seq": (False, True, True, True, False, True, True),
                        "exp_res": [(1, 2, 3), (5, 6)],
                    },
                    {"sub_op_seq": (False, True, False, True, False), "exp_res": []},
                    {
                        "sub_op_seq": (True, True, False, False, False),
                        "exp_res": [(0, 1)],
                    },
                    {
                        "sub_op_seq": (False, True, True, False, False),
                        "exp_res": [(1, 2)],
                    },
                ]
                expected_log_level = AscendCLogLevel.LOG_WARNING

                for op_seq in sub_op_dcci_option_list:
                    with mock.patch(
                        "asc_op_compile_base.asc_op_compiler.super_kernel_utility.CommonUtility.print_compile_log"
                    ) as mock_print_compile_log:
                        super_op.info_base = []
                        # create sub op list
                        for sub_op_idx, call_dcci_disable_on_kernel in enumerate(
                            op_seq["sub_op_seq"]
                        ):
                            sub_op = SubOperatorInfos(
                                0,
                                {
                                    "bin_path": "./sub_op.o",
                                    "json_path": "sub_op.json",
                                    "kernel_name": "sub_op",
                                },
                                100,
                                op_options,
                                compile_log_path,
                            )
                            sub_op.kernel_name = f"sub_op_{sub_op_idx}"
                            sub_op.call_dcci_before_kernel_start = False
                            sub_op.call_dcci_disable_on_kernel = (
                                call_dcci_disable_on_kernel
                            )
                            sub_op.call_dcci_after_kernel_end = False
                            super_op.info_base.append(sub_op)

                        super_op.check_dcci_before_after_op_options()

                        if len(op_seq["exp_res"]) == 0:
                            mock_print_compile_log.assert_not_called()
                        else:
                            mock_print_compile_log.assert_called()

                        mock_log_id = 0
                        for exp_op_seq in op_seq["exp_res"]:
                            for expected_sub_op_id in exp_op_seq:
                                mock_log_id += 1
                                expected_warning_op_name = (
                                    f"sub_op_{expected_sub_op_id}"
                                )
                                self.assertIn(
                                    expected_warning_op_name,
                                    mock_print_compile_log.call_args_list[
                                        mock_log_id
                                    ].args[1],
                                )
                                self.assertEqual(
                                    expected_log_level,
                                    mock_print_compile_log.call_args_list[
                                        mock_log_id
                                    ].args[2],
                                )

    def test_check_debug_aic_aiv_num_ratio(self):
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                super_op = SuperOperatorInfos(
                    {
                        "op_list": [
                            {
                                "bin_path": "op1.o",
                                "json_path": "op1.json",
                                "kernel_name": "op1",
                            },
                        ],
                        "super_kernel_options": "stream-fusion=1",
                    },
                    "super_kernel",
                )

                pass_debug_aic_aiv_num_pairs = [
                    (0, 0),
                    (10, 0),
                    (12, 12),
                    (14, 28),
                    (24, 48),
                    (0, 20),
                    (0, 48),
                ]

                failed_debug_aic_aiv_num_pairs = [
                    (10, 1),
                    (10, 11),
                    (1, 10),
                    (10, 40),
                ]

                for (
                    test_debug_aic_num,
                    test_debug_aiv_num,
                ) in pass_debug_aic_aiv_num_pairs:
                    super_op.debug_aic_num = test_debug_aic_num
                    super_op.debug_aiv_num = test_debug_aiv_num
                    super_op.check_debug_aic_aiv_num_ratio()

                for (
                    test_debug_aic_num,
                    test_debug_aiv_num,
                ) in failed_debug_aic_aiv_num_pairs:
                    super_op.debug_aic_num = test_debug_aic_num
                    super_op.debug_aiv_num = test_debug_aiv_num
                    self.assertRaises(Exception, super_op.check_debug_aic_aiv_num_ratio)

    def test_check_debug_aic_aiv_num_exceed_platform_num_blocks(self):
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                super_op = SuperOperatorInfos(
                    {
                        "op_list": [
                            {
                                "bin_path": "op1.o",
                                "json_path": "op1.json",
                                "kernel_name": "op1",
                            },
                        ],
                        "super_kernel_options": "stream-fusion=1",
                    },
                    "super_kernel",
                )

                pass_debug_aic_aiv_num_pairs = [
                    (0, 0, 24, 48),
                    (10, 0, 24, 48),
                    (10, 24, 24, 48),
                    (10, 10, 24, 48),
                    (24, 48, 24, 48),
                ]

                failed_debug_aic_aiv_num_pairs = [
                    (100, 1, 24, 48),
                    (10, 100, 24, 48),
                    (24, 100, 24, 48),
                    (10, 100, 24, 0),
                ]

                for (
                    test_debug_aic_num,
                    test_debug_aiv_num,
                    platform_aic_num,
                    platform_aiv_num,
                ) in pass_debug_aic_aiv_num_pairs:

                    def mock_get_soc_spec(input_str: str):
                        return {
                            "ai_core_cnt": platform_aic_num,
                            "vector_core_cnt": platform_aiv_num,
                        }.get(input_str, 0)

                    with mock.patch(
                        "asc_op_compile_base.asc_op_compiler.super_kernel_op_infos.get_soc_spec",
                        new=mock_get_soc_spec,
                    ):
                        super_op.debug_aic_num = test_debug_aic_num
                        super_op.debug_aiv_num = test_debug_aiv_num
                        super_op.check_debug_aic_aiv_num_exceed_platform_num_blocks()

                for (
                    test_debug_aic_num,
                    test_debug_aiv_num,
                    platform_aic_num,
                    platform_aiv_num,
                ) in failed_debug_aic_aiv_num_pairs:

                    def mock_get_soc_spec(input_str: str):
                        return {
                            "ai_core_cnt": platform_aic_num,
                            "vector_core_cnt": platform_aiv_num,
                        }.get(input_str, 0)

                    with mock.patch(
                        "asc_op_compile_base.asc_op_compiler.super_kernel_op_infos.get_soc_spec",
                        new=mock_get_soc_spec,
                    ):
                        super_op.debug_aic_num = test_debug_aic_num
                        super_op.debug_aiv_num = test_debug_aiv_num
                        self.assertRaises(
                            Exception,
                            super_op.check_debug_aic_aiv_num_exceed_platform_num_blocks,
                        )

    def test_check_debug_aic_aiv_num_exceed_sub_op_aic_aiv_num(self):
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                super_op = SuperOperatorInfos(
                    {
                        "op_list": [
                            {
                                "bin_path": "op1.o",
                                "json_path": "op1.json",
                                "kernel_name": "op1",
                            },
                        ],
                        "super_kernel_options": "stream-fusion=1",
                    },
                    "super_kernel",
                )

                pass_debug_aic_aiv_num_pairs = [
                    (0, 0, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0, 10),
                    (10, 0, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0, 10),
                    (15, 0, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0, 10),
                    (0, 0, SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0, 20),
                    (0, 20, SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0, 20),
                    (0, 40, SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0, 20),
                    (0, 0, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1, 20),
                    (20, 20, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1, 20),
                    (20, 40, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1, 20),
                    (0, 0, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2, 20),
                    (20, 40, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2, 20),
                    (24, 48, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2, 20),
                ]

                failed_debug_aic_aiv_num_pairs = [
                    (5, 0, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0, 10),
                    (0, 5, SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0, 10),
                    (0, 20, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1, 20),
                    (10, 10, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1, 20),
                    (10, 20, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1, 20),
                    (0, 40, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2, 20),
                    (20, 20, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2, 20),
                    (20, 38, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2, 20),
                ]

                for (
                    test_debug_aic_num,
                    test_debug_aiv_num,
                    sk_kernel_type,
                    sk_block_num,
                ) in pass_debug_aic_aiv_num_pairs:
                    super_op.debug_aic_num = test_debug_aic_num
                    super_op.debug_aiv_num = test_debug_aiv_num
                    super_op.kernel_type = sk_kernel_type
                    super_op.block_num = sk_block_num
                    super_op.check_debug_aic_aiv_num_exceed_sub_op_aic_aiv_num()

                for (
                    test_debug_aic_num,
                    test_debug_aiv_num,
                    sk_kernel_type,
                    sk_block_num,
                ) in failed_debug_aic_aiv_num_pairs:
                    super_op.debug_aic_num = test_debug_aic_num
                    super_op.debug_aiv_num = test_debug_aiv_num
                    super_op.kernel_type = sk_kernel_type
                    super_op.block_num = sk_block_num
                    self.assertRaises(
                        Exception,
                        super_op.check_debug_aic_aiv_num_exceed_sub_op_aic_aiv_num,
                    )

    def test_update_superkernel_blocknum_by_debug_options(self):
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                super_op = SuperOperatorInfos(
                    {
                        "op_list": [
                            {
                                "bin_path": "op1.o",
                                "json_path": "op1.json",
                                "kernel_name": "op1",
                            },
                        ],
                        "super_kernel_options": "stream-fusion=1",
                    },
                    "super_kernel",
                )

                failed_debug_aic_aiv_num_pairs = [
                    (10, 1, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2, 10),
                    (10, 11, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2, 10),
                    (1, 10, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2, 10),
                    (10, 40, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2, 10),
                    (5, 10, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2, 10),
                    (25, 50, SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2, 10),
                ]

                for (
                    test_debug_aic_num,
                    test_debug_aiv_num,
                    sk_kernel_type,
                    sk_block_num,
                ) in failed_debug_aic_aiv_num_pairs:

                    def mock_get_soc_spec(input_str: str):
                        return {"ai_core_cnt": 24, "vector_core_cnt": 48}.get(
                            input_str, 0
                        )

                    with mock.patch(
                        "asc_op_compile_base.asc_op_compiler.super_kernel_op_infos.get_soc_spec",
                        new=mock_get_soc_spec,
                    ):
                        super_op.debug_aic_num = test_debug_aic_num
                        super_op.debug_aiv_num = test_debug_aiv_num
                        super_op.kernel_type = sk_kernel_type
                        super_op.block_num = sk_block_num
                        self.assertRaises(
                            Exception,
                            super_op.update_superkernel_blocknum_by_debug_options,
                        )

    def test_find_sub_kernel_name(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                tmp = SuperOperatorInfos(
                    {
                        "op_list": [
                            {
                                "bin_path": "op1.o",
                                "json_path": "op1.json",
                                "kernel_name": "op1",
                            },
                        ]
                    },
                    "super_kernel",
                )
                origin_sub_kernel_names = ["test_mix_aic_1", "test_mix_aiv_2"]
                aiv_name, aic_name = tmp.find_sub_kernel_name(origin_sub_kernel_names)
                self.assertEqual(aiv_name, "test_mix_aiv_2")

    def test_split_o_in_super_kernel(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with (
                mock.patch("json.load", return_value=op_json),
                mock.patch("os.path.exists", return_value=True),
            ):
                with mock.patch("subprocess.run") as mock_run:
                    tmp = SuperOperatorInfos(
                        {
                            "op_list": [
                                {
                                    "bin_path": "op1.o",
                                    "json_path": "op1.json",
                                    "kernel_name": "op1",
                                },
                            ]
                        },
                        "super_kernel",
                    )
                    mock_run.side_effect = RuntimeError()
                    self.assertRaises(
                        Exception, tmp.split_o_in_super_kernel, "./op.bin", "op1", 0
                    )

    def test_gen_compile_info(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                with mock.patch("os.remove"):
                    tmp = SuperOperatorInfos(
                        {
                            "op_list": [
                                {
                                    "bin_path": "op1.o",
                                    "json_path": "op1.json",
                                    "kernel_name": "op1",
                                },
                            ]
                        },
                        "super_kernel",
                    )
                    sub_tmp1 = SubOperatorInfos(
                        0,
                        {
                            "bin_path": "./op1.o",
                            "json_path": "op1.json",
                            "kernel_name": "op1",
                        },
                        100,
                        op_options,
                        compile_log_path,
                    )
                    sub_tmp1.dynamic_bin = None
                    sub_tmp1.aiv_bin = "aiv.bin"
                    sub_tmp1.aic_bin = "aic.bin"
                    sub_tmp1.dynamic_bin = "dynamic.bin"
                    sub_tmp1.split_mode = 4
                    sub_tmp1.split_mode_in_json = 4
                    sub_tmp1.sub_kernel_names = ["test_mix_aiv_1"]
                    sub_tmp1.called_kernel_name
                    sub_tmp1.called_kernel_name = {
                        "dynamic_func_names": {
                            "1234": {
                                "kernel_type": "KERNEL_TYPE_MIX_AIC_1_2",
                                "dav-c220-cube": "Btest_mix_aic",
                                "dav-c220-vec": "Btest_mix_aiv",
                            },
                            "5678": {
                                "kernel_type": "KERNEL_TYPE_MIX_AIC_1_2",
                                "dav-c220-cube": "Btest_mix_aic5678",
                                "dav-c220-vec": "Btest_mix_aiv5678",
                            },
                        }
                    }
                    tmp.info_base = [sub_tmp1]
                    tmp.super_kernel_params = ["x"] * 2000
                    tmp.early_start_mode = SuperKernelEarlyStartMode.EarlyStartEnableV1
                    tmp.op_options = {"compile-options": "-g"}
                    tmp.gen_compile_info()
                    self.assertIn(
                        "-D__ASCENDC_SUPERKERNEL_EARLY_START_V1",
                        tmp.compile_info["compile_option"],
                    )
                    self.assertIn(
                        "-D__SUPER_KERNEL_DYNAMIC_BLOCK_NUM__",
                        tmp.compile_info["compile_option"],
                    )

    def test_gen_early_start_config(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        sub_tmp1 = SubOperatorInfos(
            0,
            {"bin_path": "./op1.o", "json_path": "op1.json", "kernel_name": "op1"},
            100,
            op_options,
            compile_log_path,
        )
        sub_tmp2 = SubOperatorInfos(
            0,
            {"bin_path": "./op1.o", "json_path": "op1.json", "kernel_name": "op1"},
            100,
            op_options,
            compile_log_path,
        )
        self.assertRaises(Exception, gen_early_start_config, sub_tmp1, sub_tmp2)
        sub_tmp1.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY
        sub_tmp2.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0
        res = gen_early_start_config(sub_tmp1, sub_tmp2)
        self.assertIn("g_super_kernel_early_start_config = 5;", res)

        sub_tmp1.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY
        sub_tmp2.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0
        res = gen_early_start_config(sub_tmp1, sub_tmp2)
        self.assertIn("g_super_kernel_early_start_config = 0;", res)

        sub_tmp1.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
        sub_tmp2.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
        res = gen_early_start_config(sub_tmp1, sub_tmp2)
        self.assertIn("g_super_kernel_early_start_config = 10;", res)

        sub_tmp2.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MAX
        self.assertRaises(Exception, gen_early_start_config, sub_tmp1, sub_tmp2)

    def test_gen_inter_ops_barrier(self):
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with (
                mock.patch("json.load", return_value=op_json),
                mock.patch("os.path.exists", return_value=True),
            ):
                with mock.patch("subprocess.run") as mock_run:
                    tmp = SuperOperatorInfos(
                        {
                            "op_list": [
                                {
                                    "bin_path": "op1.o",
                                    "json_path": "op1.json",
                                    "kernel_name": "op1",
                                },
                            ]
                        },
                        "super_kernel",
                    )
                    op_options = {
                        "split-mode": 4,
                        "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
                    }
                    compile_log_path = "./tmp/"
                    sub_tmp1 = SubOperatorInfos(
                        0,
                        {
                            "bin_path": "./op1.o",
                            "json_path": "op1.json",
                            "kernel_name": "op1",
                        },
                        100,
                        op_options,
                        compile_log_path,
                    )
                    sub_tmp2 = SubOperatorInfos(
                        0,
                        {
                            "bin_path": "./op1.o",
                            "json_path": "op1.json",
                            "kernel_name": "op1",
                        },
                        100,
                        op_options,
                        compile_log_path,
                    )
                    sub_tmp1.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
                    sub_tmp2.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
                    tmp.early_start_mode = SuperKernelEarlyStartMode.EarlyStartDisable
                    res = gen_inter_ops_barrier(tmp, sub_tmp1, sub_tmp2)
                    self.assertIn(
                        "ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIV_ONLY_ALL));",
                        res,
                    )
                    self.assertIn(
                        f"{get_wait_flag_for_chip('AscendC::SYNC_AIV_ONLY_ALL')}", res
                    )

    def test_gen_op_end_dcci_all(self):
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with (
                mock.patch("json.load", return_value=op_json),
                mock.patch("os.path.exists", return_value=True),
            ):
                with mock.patch("subprocess.run") as mock_run:
                    tmp = SuperOperatorInfos(
                        {
                            "op_list": [
                                {
                                    "bin_path": "op1.o",
                                    "json_path": "op1.json",
                                    "kernel_name": "op1",
                                },
                            ]
                        },
                        "super_kernel",
                    )
                    res = gen_op_end_debug_dcci_all(tmp)
                    self.assertEqual(res, "")
                    tmp.debug_dcci_all_mode = (
                        SuperKernelDebugDcciAllMode.DebugDcciAllEnable
                    )
                    res = gen_op_end_debug_dcci_all(tmp)
                    self.assertIn("pipe_barrier(PIPE_ALL);", res)
                    self.assertIn(
                        "dcci((__gm__ uint64_t*)0, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);",
                        res,
                    )

    def test_gen_op_end_sync_all(self):
        SetCurrentSocInfo("Ascend910B1")
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with (
                mock.patch("json.load", return_value=B_op_json),
                mock.patch("os.path.exists", return_value=True),
            ):
                with mock.patch("subprocess.run") as mock_run:
                    tmp = SuperOperatorInfos(
                        {
                            "op_list": [
                                {
                                    "bin_path": "op1.o",
                                    "json_path": "op1.json",
                                    "kernel_name": "op1",
                                },
                            ]
                        },
                        "super_kernel",
                    )
                    res = gen_op_end_debug_sync_all(tmp)
                    self.assertEqual(res, "")
                    tmp.debug_sync_all_mode = (
                        SuperKernelDebugSyncAllMode.DebugSyncAllEnable
                    )
                    res = gen_op_end_debug_sync_all(tmp)
                    self.assertIn("AscendC::SyncAll<false>();", res)

    def test_gen_2_real_stream_op_end_sync_all_by_arch(self):
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with (
                mock.patch("json.load", return_value=op_json),
                mock.patch("os.path.exists", return_value=True),
            ):
                with mock.patch("subprocess.run") as mock_run:
                    tmp = SuperOperatorInfos(
                        {
                            "op_list": [
                                {
                                    "bin_path": "op1.o",
                                    "json_path": "op1.json",
                                    "kernel_name": "op1",
                                },
                            ]
                        },
                        "super_kernel",
                    )
                    res = gen_2_real_stream_op_end_debug_sync_all_by_arch(tmp, "aic")
                    self.assertEqual(res, "")
                    tmp.debug_sync_all_mode = (
                        SuperKernelDebugSyncAllMode.DebugSyncAllEnable
                    )
                    res = gen_2_real_stream_op_end_debug_sync_all_by_arch(tmp, "aic")
                    self.assertIn(
                        f"pipe_barrier(PIPE_ALL);\n\
ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIC_FLAG));\n\
{get_wait_flag_for_chip('AscendC::SYNC_AIC_FLAG')}",
                        res,
                    )
                    res = gen_2_real_stream_op_end_debug_sync_all_by_arch(tmp, "aiv")
                    self.assertIn(
                        f"pipe_barrier(PIPE_ALL);\n\
ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIV_ONLY_ALL));\n\
{get_wait_flag_for_chip('AscendC::SYNC_AIV_ONLY_ALL')}",
                        res,
                    )

    def test_tpl_of_gen_switch_case_call(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        sub_tmp1 = SubOperatorInfos(
            0,
            {"bin_path": "./op1.o", "json_path": "op1.json", "kernel_name": "op1"},
            100,
            op_options,
            compile_log_path,
        )
        sub_tmp2 = SubOperatorInfos(
            0,
            {"bin_path": "./op1.o", "json_path": "op1.json", "kernel_name": "op1"},
            100,
            op_options,
            compile_log_path,
        )
        sub_tmp1.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY
        res = tpl_of_gen_switch_case_call(0, sub_tmp2, sub_tmp1)
        sub_tmp1.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY
        res = tpl_of_gen_switch_case_call(0, sub_tmp2, sub_tmp1)
        self.assertIn("ASCEND_IS_AIV", res)

    def test_print_params_addr(self):
        res = print_params_addr(["x", "y"])
        self.assertIn("printf", res)

    def test_gen_clear_wait_sync_addr_code(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with (
                mock.patch("json.load", return_value=op_json),
                mock.patch("os.path.exists", return_value=True),
            ):
                with mock.patch("subprocess.run") as mock_run:
                    tmp = SuperOperatorInfos(
                        {
                            "op_list": [
                                {
                                    "bin_path": "op1.o",
                                    "json_path": "op1.json",
                                    "kernel_name": "op1",
                                },
                            ]
                        },
                        "super_kernel",
                    )
                    tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY
                    tmp.inner_event_id_set = [0, 1, 2]
                    sub_tmp1 = SubOperatorInfos(
                        0,
                        {
                            "bin_path": "./op1.o",
                            "json_path": "op1.json",
                            "kernel_name": "op1",
                        },
                        100,
                        op_options,
                        compile_log_path,
                    )
                    sub_tmp1.recv_event_list = [10]
                    tmp.info_base = [sub_tmp1]
                    res = gen_clear_wait_sync_addr_code(tmp)
                    self.assertIn(
                        "*(reinterpret_cast<__gm__ uint64_t*>(param_base[0])) = 0", res
                    )

    def test_gen_2_real_stream_send_code(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with (
                mock.patch("json.load", return_value=op_json),
                mock.patch("os.path.exists", return_value=True),
            ):
                with mock.patch("subprocess.run") as mock_run:
                    sub_tmp1 = SubOperatorInfos(
                        0,
                        {
                            "bin_path": "./op1.o",
                            "json_path": "op1.json",
                            "kernel_name": "op1",
                        },
                        100,
                        op_options,
                        compile_log_path,
                    )
                    sub_tmp1.send_info = {
                        "op1": "cub:cub",
                        "op2": "vec:vec;vec:cub;cub:vec",
                    }
                    sub_tmp2 = SubOperatorInfos(
                        0,
                        {
                            "bin_path": "./op1.o",
                            "json_path": "op1.json",
                            "kernel_name": "op1",
                        },
                        100,
                        op_options,
                        compile_log_path,
                    )
                    sub_tmp2.send_info = {
                        "op1": "cub:cub",
                        "op2": "vec:vec;vec:cub;cub:vec",
                    }
                    tmp = SuperOperatorInfos(
                        {
                            "op_list": [
                                {
                                    "bin_path": "op1.o",
                                    "json_path": "op1.json",
                                    "kernel_name": "op1",
                                },
                            ]
                        },
                        "super_kernel",
                    )
                    tmp.info_base = [sub_tmp1, sub_tmp2]
                    res = gen_2_real_stream_send_code(tmp, sub_tmp1, "aic")
                    self.assertNotIn("sync all C->C kernel_name", res)
                    sub_tmp2.send_info = {}
                    tmp.info_base = [sub_tmp1]
                    tmp.cub_op_list = [sub_tmp1]
                    tmp.vec_op_list = [sub_tmp1]
                    sub_tmp2.index = 1
                    res = gen_2_real_stream_send_code(tmp, sub_tmp2, "aic")
                    self.assertIn("pipe_barrier(PIPE_ALL);", res)
                    res = gen_2_real_stream_send_code(tmp, sub_tmp2, "aiv")
                    self.assertIn("pipe_barrier(PIPE_ALL);", res)

    def test_gen_2_real_stream_code_by_arch(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with (
                mock.patch("json.load", return_value=op_json),
                mock.patch("os.path.exists", return_value=True),
            ):
                with mock.patch("subprocess.run") as mock_run:
                    tmp = SuperOperatorInfos(
                        {
                            "op_list": [
                                {
                                    "bin_path": "op1.o",
                                    "json_path": "op1.json",
                                    "kernel_name": "op1",
                                },
                            ]
                        },
                        "super_kernel",
                    )
                    tmp.datacache_mode = (
                        SuperKernelDataCacheMode.DataCacheLoadAdancanceStep
                    )
                    tmp.preload_mode = SuperKernelPreLoadMode.PreLoadByWhole
                    tmp.profiling_mode = SuperKernelProfilingMode.ProfilingEnable
                    sub_tmp1 = SubOperatorInfos(
                        0,
                        {
                            "bin_path": "./op1.o",
                            "json_path": "op1.json",
                            "kernel_name": "op1",
                        },
                        100,
                        op_options,
                        compile_log_path,
                    )
                    tmp.info_base = [sub_tmp1, sub_tmp1, sub_tmp1]
                    sub_tmp1.kernel_params = ""
                    sub_ops = [sub_tmp1, sub_tmp1, sub_tmp1]
                    tmp.timestamp_option = True
                    gen_2_real_stream_super_kernel_file(tmp)
                    res = gen_2_real_stream_code_by_arch(
                        tmp, "aic", "x, y, z", False, sub_ops
                    )
                    self.assertIn("AscendC::PreLoad(8);", res)
                    tmp.preload_mode = SuperKernelPreLoadMode.PreLoadStepByStep
                    res = gen_2_real_stream_code_by_arch(
                        tmp, "aic", "x, y, z", False, sub_ops
                    )
                    self.assertIn("auto_gen_super_kernel_kernel_aic", res)

                    # test sync all option in two real stream case
                    tmp.debug_sync_all_mode = (
                        SuperKernelDebugSyncAllMode.DebugSyncAllEnable
                    )
                    res = gen_2_real_stream_code_by_arch(
                        tmp, "aic", "x, y, z", False, sub_ops
                    )
                    golden = f"pipe_barrier(PIPE_ALL);\n\
ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIC_FLAG));\n\
{get_wait_flag_for_chip('AscendC::SYNC_AIC_FLAG')}"
                    self.assertEqual(res.count(indent_code_func(golden)), 3)

                    res = gen_2_real_stream_code_by_arch(
                        tmp, "aiv", "x, y, z", False, sub_ops
                    )
                    golden = f"pipe_barrier(PIPE_ALL);\n\
ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIV_ONLY_ALL));\n\
{get_wait_flag_for_chip('AscendC::SYNC_AIV_ONLY_ALL')}"
                    self.assertEqual(res.count(indent_code_func(golden)), 3)

                    # test dcci all option in two real stream case
                    tmp.debug_dcci_all_mode = (
                        SuperKernelDebugDcciAllMode.DebugDcciAllEnable
                    )
                    res = gen_2_real_stream_code_by_arch(
                        tmp, "aic", "x, y, z", False, sub_ops
                    )
                    golden = "pipe_barrier(PIPE_ALL);\n\
dcci((__gm__ uint64_t*)0, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);"
                    self.assertEqual(res.count(indent_code_func(golden)), 3)

    def test_gen_super_kernel_file(self):
        SetCurrentSocInfo("Ascend950PR_9599")
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with (
                mock.patch("json.load", return_value=op_json),
                mock.patch("os.path.exists", return_value=True),
            ):
                with mock.patch("subprocess.run") as mock_run:
                    tmp = SuperOperatorInfos(
                        {
                            "op_list": [
                                {
                                    "bin_path": "op1.o",
                                    "json_path": "op1.json",
                                    "kernel_name": "op1",
                                },
                            ]
                        },
                        "super_kernel",
                    )
                    tmp.datacache_mode = (
                        SuperKernelDataCacheMode.DataCacheLoadAdancanceStep
                    )
                    tmp.preload_mode = SuperKernelPreLoadMode.PreLoadByWhole
                    sub_tmp1 = SubOperatorInfos(
                        0,
                        {
                            "bin_path": "./op1.o",
                            "json_path": "op1.json",
                            "kernel_name": "op1",
                        },
                        100,
                        op_options,
                        compile_log_path,
                    )
                    sub_tmp2 = SubOperatorInfos(
                        0,
                        {
                            "bin_path": "./op1.o",
                            "json_path": "op1.json",
                            "kernel_name": "op1",
                        },
                        100,
                        op_options,
                        compile_log_path,
                    )
                    sub_tmp3 = SubOperatorInfos(
                        0,
                        {
                            "bin_path": "./op1.o",
                            "json_path": "op1.json",
                            "kernel_name": "op1",
                        },
                        100,
                        op_options,
                        compile_log_path,
                    )
                    sub_tmp1.sub_op_task_type = SubOperatorType.DYNAMIC_OP
                    sub_tmp1.recv_event_list = [0, 1]
                    sub_tmp1.send_event_list = [2, 3]
                    tmp.info_base = [sub_tmp1, sub_tmp2, sub_tmp3]
                    tmp.split_mode = 2
                    self.assertRaises(Exception, gen_super_kernel_file, tmp)
                    sub_tmp1.recv_event_list = []
                    self.assertRaises(Exception, gen_super_kernel_file, tmp)
                    sub_tmp1.send_event_list = []
                    sub_tmp2.recv_event_list = [0, 1]
                    sub_tmp2.send_event_list = [2, 3]
                    sub_tmp2.wait_block = "wait block"
                    sub_tmp3.wait_block = "wait block"
                    sub_tmp2.notify_block = "notify block"
                    sub_tmp3.notify_block = "notify block"
                    sub_tmp1.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY
                    sub_tmp2.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY
                    sub_tmp3.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY
                    tmp.profiling_mode = SuperKernelProfilingMode.ProfilingEnable
                    gen_super_kernel_file(tmp)
                    tmp.timestamp_option = True
                    gen_super_kernel_file(tmp)

    def test_insert_sync_for_notify(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                tmp = SuperOperatorInfos(
                    {
                        "op_list": [
                            {
                                "bin_path": "op1.o",
                                "json_path": "op1.json",
                                "kernel_name": "op1",
                            },
                        ]
                    },
                    "super_kernel",
                )
                sub_tmp1 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                sub_tmp1.send_event_list = [1]
                sub_tmp1.recv_event_list = [1]
                sub_tmp1.recv_info = {"op_test_1": "cub:vec"}
                sub_tmp1.send_info = {"op_test_1": "cub:vec"}
                sub_tmp1.kernel_name = "op1"
                sub_tmp1.kernel_name_for_multi_stream = "op1_0"
                sub_tmp1.notify_block["aic"] = "tmp"
                sub_tmp1.notify_block["aiv"] = "tmp"
                sub_tmp1.stream_index = 0
                sub_tmp2 = SubOperatorInfos(
                    1,
                    {
                        "bin_path": "./op2.o",
                        "json_path": "op2.json",
                        "kernel_name": "op2",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                sub_tmp2.send_event_list = [1]
                sub_tmp2.recv_event_list = [1]
                sub_tmp2.recv_info = {"op_test_1": "cub:vec"}
                sub_tmp2.send_info = {"op_test_1": "cub:vec"}
                sub_tmp2.kernel_name = "op_test"
                sub_tmp2.kernel_name_for_multi_stream = "op_test_1"
                sub_tmp2.stream_index = 1
                sub_tmp2.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY
                tmp.info_base = [sub_tmp1, sub_tmp2]
                tmp.insert_sync_for_notify()
                self.assertEqual(sub_tmp2.recv_info, {"op_test_1": "cub:vec"})

    def test_gen_call_func_with_syncall(self):
        op_options = {
            "split-mode": 1,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
            "early-start": SuperKernelEarlyStartMode.EarlyStartDisable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with (
                mock.patch("json.load", return_value=F_op_json),
                mock.patch("os.chdir"),
                mock.patch("subprocess.run"),
                mock.patch.object(CommonUtility, "dump_compile_log"),
            ):
                tmp = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                        "send_event_list": [46, 50],
                        "recv_event_list": [46, 51],
                        "stream_id": 15,
                        "task_type": "dynamic",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                tmp.feed_sync_all_mode = SuperKernelFeedSyncAllMode.FeedSyncAllEnable
                tmp.gen_call_func_with_syncall(
                    ["test_code"], "ASCEND_IS_AIC", "get_block_idx()"
                )
                tmp.gen_call_func_with_syncall(
                    ["test_code", "test_code1"], "ASCEND_IS_AIC", "get_block_idx()"
                )

    def test_feed_sync_all_for_double_stream(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with (
                mock.patch("json.load", return_value=op_json),
                mock.patch("os.path.exists", return_value=True),
            ):
                with mock.patch("subprocess.run") as mock_run:
                    tmp = SuperOperatorInfos(
                        {
                            "op_list": [
                                {
                                    "bin_path": "op1.o",
                                    "json_path": "op1.json",
                                    "kernel_name": "op1",
                                },
                            ]
                        },
                        "super_kernel",
                    )
                    tmp.datacache_mode = (
                        SuperKernelDataCacheMode.DataCacheLoadAdancanceStep
                    )
                    tmp.preload_mode = SuperKernelPreLoadMode.PreLoadByWhole
                    sub_tmp1 = SubOperatorInfos(
                        0,
                        {
                            "bin_path": "./op1.o",
                            "json_path": "op1.json",
                            "kernel_name": "op1",
                        },
                        100,
                        op_options,
                        compile_log_path,
                    )
                    sub_tmp1.kernel_params = ""
                    sub_ops = [sub_tmp1]
                    tmp.block_num = 24
                    tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2
                    tmp.profiling_mode = SuperKernelProfilingMode.ProfilingEnable
                    tmp.info_base = [sub_tmp1]
                    sub_tmp1.block_num = 20
                    sub_tmp1.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0
                    sub_tmp1.with_sync_all = True
                    tmp.info_base = sub_ops
                    tmp.timestamp_option = False
                    gen_2_real_stream_super_kernel_file(tmp)
                    tmp.timestamp_option = True
                    tmp.feed_sync_all_mode = (
                        SuperKernelFeedSyncAllMode.FeedSyncAllEnable
                    )
                    gen_2_real_stream_super_kernel_file(tmp)
                    res = gen_2_real_stream_code_by_arch(
                        tmp, "aic", "x, y, z", False, sub_ops
                    )
                    self.assertIn("AscendC::PreLoad(8);", res)
                    tmp.preload_mode = SuperKernelPreLoadMode.PreLoadStepByStep
                    res = gen_2_real_stream_code_by_arch(
                        tmp, "aic", "x, y, z", False, sub_ops
                    )
                    self.assertIn("auto_gen_super_kernel_kernel_aic", res)

                    sub_tmp1.with_sync_all = False
                    flag = judge_need_feed_sync_all(tmp, sub_tmp1)
                    self.assertEqual(flag, False)
                    code, ret = gen_feed_syncall_var_init_code(tmp, sub_tmp1)
                    self.assertEqual(ret, False)
                    sub_tmp1.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2
                    sub_tmp1.block_num = 24
                    sub_tmp1.with_sync_all = True
                    flag = judge_need_feed_sync_all(tmp, sub_tmp1)
                    self.assertEqual(flag, False)
                    sub_tmp1.block_num = 20
                    tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0
                    flag = judge_need_feed_sync_all(tmp, sub_tmp1)
                    self.assertEqual(flag, True)
                    code, ret = gen_feed_syncall_var_init_code(tmp, sub_tmp1)
                    self.assertEqual(ret, True)
                    tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2
                    sub_tmp1.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY
                    sub_tmp1.block_num = 10
                    tmp.block_num = 24
                    flag = judge_need_feed_sync_all(tmp, sub_tmp1)
                    self.assertEqual(flag, True)

                    sub_tmp1.block_num = 48
                    flag = judge_need_feed_sync_all(tmp, sub_tmp1)
                    self.assertEqual(flag, False)

    def test_gen_clear_syncall_worskspace(self):
        SetCurrentSocInfo("Ascend950PR_9599")
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with (
                mock.patch("json.load", return_value=op_json),
                mock.patch("os.path.exists", return_value=True),
            ):
                with mock.patch("subprocess.run") as mock_run:
                    tmp = SuperOperatorInfos(
                        {
                            "op_list": [
                                {
                                    "bin_path": "op1.o",
                                    "json_path": "op1.json",
                                    "kernel_name": "op1",
                                },
                            ]
                        },
                        "super_kernel",
                    )
                    code = gen_clear_syncall_worskspace(tmp)
                    self.assertEqual(code, "")
                    tmp.feed_sync_all_mode = (
                        SuperKernelFeedSyncAllMode.FeedSyncAllEnable
                    )
                    tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0
                    code = gen_clear_syncall_worskspace(tmp)
                    self.assertIn("copy_cbuf_to_gm", code)
                    tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0
                    code = gen_clear_syncall_worskspace(tmp)
                    self.assertIn("copy_ubuf_to_gm", code)
                    tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2
                    code = gen_clear_syncall_worskspace(tmp)
                    self.assertIn("copy_ubuf_to_gm", code)
                    tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
                    code = gen_clear_syncall_worskspace(tmp)
                    self.assertIn("copy_ubuf_to_gm", code)

    def test_calc_workspace_size(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with (
                mock.patch("json.load", return_value=op_json),
                mock.patch("os.path.exists", return_value=True),
            ):
                with mock.patch("subprocess.run") as mock_run:
                    tmp = SuperOperatorInfos(
                        {
                            "op_list": [
                                {
                                    "bin_path": "op1.o",
                                    "json_path": "op1.json",
                                    "kernel_name": "op1",
                                },
                            ]
                        },
                        "super_kernel",
                    )
                    tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
                    tmp.feed_sync_all_mode = (
                        SuperKernelFeedSyncAllMode.FeedSyncAllEnable
                    )
                    tmp.block_num = 1
                    tmp.calc_workspace_size()
                    self.assertEqual(tmp.workspace_size, 512)
                    tmp.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2
                    tmp.info_base = [1] * 1024
                    tmp.calc_workspace_size()

    def test_gen_prof_code_for_notify_wait(self):
        op_options = {
            "split-mode": 4,
            "stream-fusion": SuperKernelStreamFusionMode.StreamFusionEnable,
        }
        compile_log_path = "./tmp/"
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with (
                mock.patch("json.load", return_value=op_json),
                mock.patch("os.path.exists", return_value=True),
            ):
                with mock.patch("subprocess.run") as mock_run:
                    sub_tmp = SubOperatorInfos(
                        0,
                        {
                            "bin_path": "./op1.o",
                            "json_path": "op1.json",
                            "kernel_name": "op1",
                        },
                        100,
                        op_options,
                        compile_log_path,
                    )
                    sub_tmp.profiling_mode = SuperKernelProfilingMode.ProfilingDisable
                    code = sub_tmp.gen_profiling_for_notify(0, True)
                    self.assertEqual(code, "")
                    code = sub_tmp.gen_profiling_for_wait(0, True)
                    self.assertEqual(code, "")
                    sub_tmp.profiling_mode = SuperKernelProfilingMode.ProfilingEnable
                    code = sub_tmp.gen_profiling_for_notify(0, True)
                    self.assertEqual(code, "RecordProfiling(0, 0x4, false);\n")
                    code = sub_tmp.gen_profiling_for_notify(0, False)
                    self.assertEqual(code, "RecordProfiling(0, 0x4, true);\n")
                    code = sub_tmp.gen_profiling_for_wait(0, True)
                    self.assertEqual(code, "RecordProfiling(0, 12, false);\n")
                    code = sub_tmp.gen_profiling_for_wait(0, False)
                    self.assertEqual(code, "RecordProfiling(0, 12, true);\n")

    def test_gen_sync_and_event_code(self):
        compile_log_path = "./tmp/"
        op_options = {"split-mode": 4}
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                tmp = SuperOperatorInfos(
                    {
                        "op_list": [
                            {
                                "bin_path": "op1.o",
                                "json_path": "op1.json",
                                "kernel_name": "op1",
                            },
                        ]
                    },
                    "super_kernel",
                )
                sub_tmp1 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                sub_tmp1.send_event_list = [1]
                sub_tmp1.recv_event_list = [1]
                sub_tmp1.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
                sub_tmp1.notify_block = "1"
                sub_tmp2 = SubOperatorInfos(
                    1,
                    {
                        "bin_path": "./op2.o",
                        "json_path": "op2.json",
                        "kernel_name": "op2",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                sub_tmp2.send_event_list = [1]
                sub_tmp2.recv_event_list = [1]
                sub_tmp2.wait_block = "2"
                sub_tmp2.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
                sync_and_event_code = gen_sync_and_event_code(tmp, sub_tmp1, sub_tmp2)
                self.assertIn(
                    "for continues notify/wait event", sync_and_event_code.strip()
                )

                sub_tmp1.notify_block = {"aiv": "3", "aic": "4"}
                sub_tmp2.kernel_type = SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
                sync_and_event_code = gen_sync_and_event_code_for_two_stream(
                    tmp, sub_tmp1, sub_tmp2, "aic"
                )
                self.assertIn("SyncAll", sync_and_event_code.strip())

                sub_tmp2.kernel_type = SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY
                sync_and_event_code = gen_sync_and_event_code_for_two_stream(
                    tmp, sub_tmp1, sub_tmp2, "aiv"
                )
                self.assertIn("SYNC_AIC_FLAG", sync_and_event_code.strip())

    def test_gen_wait_block_extra_sync(self):
        compile_log_path = "./tmp/"
        op_options = {"split-mode": 4}
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                super_op = SuperOperatorInfos(
                    {
                        "op_list": [
                            {
                                "bin_path": "op1.o",
                                "json_path": "op1.json",
                                "kernel_name": "op1",
                            },
                        ]
                    },
                    "super_kernel",
                )
                sub_op1 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                sub_op2 = SubOperatorInfos(
                    1,
                    {
                        "bin_path": "./op2.o",
                        "json_path": "op2.json",
                        "kernel_name": "op2",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                sub_op2.recv_event_list = [1]
                sub_op2.wait_block = "sub_op2 wait block\n"

                # wait block extra sync case: aic to aiv, need extra aiv sync
                wait_block_extra_sync_aic_to_aiv_pairs = {
                    (
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0,
                    ),
                    (
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
                        SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY,
                    ),
                    (
                        SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0,
                    ),
                    (
                        SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
                        SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY,
                    ),
                }

                golden_extra_aiv_sync_code = """    sub_op2 wait block
    // extra sync for wait event
    AscendC::SyncAll<true>();
"""

                for (
                    sub_op1_type,
                    sub_op2_type,
                ) in wait_block_extra_sync_aic_to_aiv_pairs:
                    sub_op1.kernel_type = sub_op1_type
                    sub_op2.kernel_type = sub_op2_type
                    sync_and_event_code = gen_sync_and_event_code(
                        super_op, sub_op1, sub_op2
                    )
                    self.assertIn(golden_extra_aiv_sync_code, sync_and_event_code)

                # wait block extra sync case: aic to mix, need extra aiv sync
                wait_block_extra_sync_aic_to_mix_pairs = {
                    (
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1,
                    ),
                    (
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2,
                    ),
                    (
                        SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1,
                    ),
                    (
                        SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2,
                    ),
                }

                for (
                    sub_op1_type,
                    sub_op2_type,
                ) in wait_block_extra_sync_aic_to_mix_pairs:
                    sub_op1.kernel_type = sub_op1_type
                    sub_op2.kernel_type = sub_op2_type
                    sync_and_event_code = gen_sync_and_event_code(
                        super_op, sub_op1, sub_op2
                    )
                    self.assertIn(golden_extra_aiv_sync_code, sync_and_event_code)

                # wait block extra sync case: aiv to aic, need extra aic sync
                wait_block_extra_sync_aiv_to_aic_pairs = {
                    (
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0,
                        SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
                    ),
                    (
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0,
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
                    ),
                    (
                        SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY,
                        SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
                    ),
                    (
                        SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY,
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
                    ),
                }

                golden_extra_aic_sync_code = f"""    sub_op2 wait block

    // extra sync for wait event
    ffts_cross_core_sync(PIPE_FIX, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIC_FLAG));
    {get_wait_flag_for_chip("AscendC::SYNC_AIC_FLAG")}
"""

                for (
                    sub_op1_type,
                    sub_op2_type,
                ) in wait_block_extra_sync_aiv_to_aic_pairs:
                    sub_op1.kernel_type = sub_op1_type
                    sub_op2.kernel_type = sub_op2_type
                    sync_and_event_code = gen_sync_and_event_code(
                        super_op, sub_op1, sub_op2
                    )
                    self.assertIn(golden_extra_aic_sync_code, sync_and_event_code)

                # wait block extra sync case: else, no extra sync
                wait_block_extra_no_sync_pairs = {
                    (
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2,
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2,
                    ),
                    (
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2,
                        SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
                    ),
                    (
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2,
                        SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY,
                    ),
                    (
                        SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY,
                        SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2,
                    ),
                    (
                        SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY,
                        SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY,
                    ),
                    (
                        SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
                        SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
                    ),
                }

                for sub_op1_type, sub_op2_type in wait_block_extra_no_sync_pairs:
                    sub_op1.kernel_type = sub_op1_type
                    sub_op2.kernel_type = sub_op2_type
                    sync_and_event_code = gen_sync_and_event_code(
                        super_op, sub_op1, sub_op2
                    )
                    self.assertNotIn(
                        "// extra sync for wait event", sync_and_event_code
                    )
                    self.assertNotIn("AscendC::SyncAll<true>();", sync_and_event_code)
                    self.assertNotIn("ffts_cross_core_sync", sync_and_event_code)
                    self.assertNotIn("wait_flag_dev", sync_and_event_code)

    def test_get_sync_code_by_kernel_type(self):
        sync_code = get_sync_code_by_kernel_type(
            SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1
        )
        self.assertIn("SyncAll", sync_code.strip())
        sync_code = get_sync_code_by_kernel_type(
            SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY
        )
        self.assertIn("SYNC_AIC_FLAG", sync_code.strip())
        sync_code = get_sync_code_by_kernel_type(
            SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY
        )
        self.assertIn("SYNC_AIV_ONLY_ALL", sync_code.strip())

    def test_gen_super_kernel_link_obj_sequence(self):
        compile_info = CompileInfo()
        compile_log_path = "./tmp/"
        op_options = {"split-mode": 4}
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                tmp = SuperOperatorInfos(
                    {
                        "op_list": [
                            {
                                "bin_path": "op1.o",
                                "json_path": "op1.json",
                                "kernel_name": "op1",
                            },
                        ]
                    },
                    "super_kernel",
                )
                tmp.datacache_mode = SuperKernelDataCacheMode.DataCacheLoadAdancanceStep
                tmp.preload_mode = SuperKernelPreLoadMode.PreLoadByWhole
                sub_tmp1 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                sub_tmp2 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                sub_tmp3 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                sub_tmp1.sub_op_task_type = SubOperatorType.DYNAMIC_OP
                sub_tmp1.recv_event_list = [0, 1]
                sub_tmp1.send_event_list = [2, 3]
                sub_tmp1.sub_kernel_names = "tmp1"
                sub_tmp2.sub_kernel_names = "tmp2"
                sub_tmp3.sub_kernel_names = "tmp3"
                sub_tmp4 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                    },
                    100,
                    op_options,
                    compile_log_path,
                )
                sub_tmp4.sub_kernel_names = "tmp4"
                sub_tmp4.dynamic_bin = None
                sub_tmp4.aiv_bin = "aiv.bin"
                sub_tmp4.aic_bin = "aic.bin"
                sub_tmp4.dynamic_bin = "dynamic.bin"
                sub_tmp4.split_mode = 4
                sub_tmp4.split_mode_in_json = 4
                sub_tmp4.sub_kernel_names = ["test_mix_aiv_1"]
                sub_tmp4.called_kernel_name
                sub_tmp4.called_kernel_name = {
                    "dynamic_func_names": {
                        "1234": {
                            "kernel_type": "KERNEL_TYPE_MIX_AIC_1_2",
                            "dav-c220-cube": "Btest_mix_aic",
                            "dav-c220-vec": "Btest_mix_aiv",
                        },
                        "5678": {
                            "kernel_type": "KERNEL_TYPE_MIX_AIC_1_2",
                            "dav-c220-cube": "Btest_mix_aic5678",
                            "dav-c220-vec": "Btest_mix_aiv5678",
                        },
                    }
                }
                tmp.info_base = [sub_tmp1, sub_tmp2, sub_tmp3, sub_tmp4]
                tmp.gen_compile_info()
                tmp.split_mode = 2
                unique_lst = gen_super_kernel_link_obj_sequence(
                    compile_info,
                    tmp.compile_info["sub_operator"],
                    SuperKernelLinkMode.PerVecHerCube,
                    1,
                )
                unique_lst = gen_super_kernel_link_obj_sequence(
                    compile_info,
                    tmp.compile_info["sub_operator"],
                    SuperKernelLinkMode.PerCubeHerVec,
                    1,
                )
                unique_lst = gen_super_kernel_link_obj_sequence(
                    compile_info,
                    tmp.compile_info["sub_operator"],
                    SuperKernelLinkMode.PerCubeHerVecWithSuper,
                    1,
                )
                self.assertEqual(
                    unique_lst,
                    [
                        "",
                        "aic.bin",
                        "aiv.bin",
                        "dynamic.bin",
                        "./kernel_meta/dynamic.b_split1.o",
                        "./kernel_meta/dynamic.b_split2.o",
                        "./kernel_meta/dynamic.b_split3.o",
                    ],
                )

    def test_dcci_options_by_dynamic_op_type(self):
        """Test DCCI options are set correctly based on dynamic operator type"""
        compile_log_path = "./tmp/"

        # Test case 1: op_type in dcci-before-kernel-start list
        op_options1 = {"split-mode": 4, "dcci-before-kernel-start": "Add,MatMul"}
        json_data1 = {
            "kernelName": "op1",
            "split_mode": 4,
            "blockDim": 1,
            "sub_operator_params": [],
            "sub_operator_kernel_type": "KERNEL_TYPE_AIV_ONLY",
            "sub_operator_kernel_name": {
                "AiCore": {"func_name": "test_func", "obj_files": "test.o"}
            },
            "sub_operator_early_start_set_flag": False,
            "sub_operator_early_start_wait_flag": False,
            "sub_operator_call_dcci_before_kernel_start": False,
            "sub_operator_call_dcci_after_kernel_end": False,
            "sub_operator_call_dcci_disable_on_kernel": False,
            "sub_operator_op_type": "Add",
            "debugOptions": "",
        }
        with mock.patch("builtins.open", new_callable=mock.mock_open, read_data="{}"):
            with mock.patch("json.load", return_value=json_data1):
                sub_op1 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                    },
                    100,
                    op_options1,
                    compile_log_path,
                )
                sub_op1.sub_op_task_type = SubOperatorType.DYNAMIC_OP
                self.assertEqual(
                    sub_op1.dcci_before_kernel_start_op_list, ["Add", "MatMul"]
                )
                sub_op1.init_of_sub_operator_info()
                self.assertTrue(sub_op1.call_dcci_before_kernel_start)

        # Test case 2: op_type in dcci-after-kernel-end list
        op_options2 = {"split-mode": 4, "dcci-after-kernel-end": "Conv,MatMul"}
        json_data2 = {
            "kernelName": "op2",
            "split_mode": 4,
            "blockDim": 1,
            "sub_operator_params": [],
            "sub_operator_kernel_type": "KERNEL_TYPE_AIV_ONLY",
            "sub_operator_kernel_name": {
                "AiCore": {"func_name": "test_func", "obj_files": "test.o"}
            },
            "sub_operator_early_start_set_flag": False,
            "sub_operator_early_start_wait_flag": False,
            "sub_operator_call_dcci_before_kernel_start": False,
            "sub_operator_call_dcci_after_kernel_end": False,
            "sub_operator_call_dcci_disable_on_kernel": False,
            "sub_operator_op_type": "MatMul",
            "debugOptions": "",
        }
        with mock.patch("builtins.open", new_callable=mock.mock_open, read_data="{}"):
            with mock.patch("json.load", return_value=json_data2):
                sub_op2 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op2.o",
                        "json_path": "op2.json",
                        "kernel_name": "op2",
                    },
                    100,
                    op_options2,
                    compile_log_path,
                )
                sub_op2.sub_op_task_type = SubOperatorType.DYNAMIC_OP
                self.assertEqual(
                    sub_op2.dcci_after_kernel_end_op_list, ["Conv", "MatMul"]
                )
                sub_op2.init_of_sub_operator_info()
                self.assertTrue(sub_op2.call_dcci_after_kernel_end)

        # Test case 3: op_type in dcci-disable-on-kernel list
        op_options3 = {"split-mode": 4, "dcci-disable-on-kernel": "Transpose,Softmax"}
        json_data3 = {
            "kernelName": "op3",
            "split_mode": 4,
            "blockDim": 1,
            "sub_operator_params": [],
            "sub_operator_kernel_type": "KERNEL_TYPE_AIV_ONLY",
            "sub_operator_kernel_name": {
                "AiCore": {"func_name": "test_func", "obj_files": "test.o"}
            },
            "sub_operator_early_start_set_flag": False,
            "sub_operator_early_start_wait_flag": False,
            "sub_operator_call_dcci_before_kernel_start": False,
            "sub_operator_call_dcci_after_kernel_end": False,
            "sub_operator_call_dcci_disable_on_kernel": False,
            "sub_operator_op_type": "Softmax",
            "debugOptions": "",
        }
        with mock.patch("builtins.open", new_callable=mock.mock_open, read_data="{}"):
            with mock.patch("json.load", return_value=json_data3):
                sub_op3 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op3.o",
                        "json_path": "op3.json",
                        "kernel_name": "op3",
                    },
                    100,
                    op_options3,
                    compile_log_path,
                )
                sub_op3.sub_op_task_type = SubOperatorType.DYNAMIC_OP
                self.assertEqual(
                    sub_op3.dcci_disable_on_kernel_op_list, ["Transpose", "Softmax"]
                )
                sub_op3.init_of_sub_operator_info()
                self.assertTrue(sub_op3.call_dcci_disable_on_kernel)

        # Test case 4: op_type not in any list
        op_options4 = {
            "split-mode": 4,
            "dcci-before-kernel-start": "Add,MatMul",
            "dcci-after-kernel-end": "Conv,MatMul",
            "dcci-disable-on-kernel": "Transpose,Softmax",
        }
        json_data4 = {
            "kernelName": "op4",
            "split_mode": 4,
            "blockDim": 1,
            "sub_operator_params": [],
            "sub_operator_kernel_type": "KERNEL_TYPE_AIV_ONLY",
            "sub_operator_kernel_name": {
                "AiCore": {"func_name": "test_func", "obj_files": "test.o"}
            },
            "sub_operator_early_start_set_flag": False,
            "sub_operator_early_start_wait_flag": False,
            "sub_operator_call_dcci_before_kernel_start": False,
            "sub_operator_call_dcci_after_kernel_end": False,
            "sub_operator_call_dcci_disable_on_kernel": False,
            "sub_operator_op_type": "UnknownOp",
            "debugOptions": "",
        }
        with mock.patch("builtins.open", new_callable=mock.mock_open, read_data="{}"):
            with mock.patch("json.load", return_value=json_data4):
                sub_op4 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op4.o",
                        "json_path": "op4.json",
                        "kernel_name": "op4",
                    },
                    100,
                    op_options4,
                    compile_log_path,
                )
                sub_op4.sub_op_task_type = SubOperatorType.DYNAMIC_OP
                sub_op4.init_of_sub_operator_info()
                self.assertFalse(sub_op4.call_dcci_before_kernel_start)
                self.assertFalse(sub_op4.call_dcci_after_kernel_end)
                self.assertFalse(sub_op4.call_dcci_disable_on_kernel)

        # Test case 5: op_type in multiple lists (disable takes precedence)
        op_options5 = {
            "split-mode": 4,
            "dcci-before-kernel-start": "Add",
            "dcci-disable-on-kernel": "Add",
        }
        json_data5 = {
            "kernelName": "op5",
            "split_mode": 4,
            "blockDim": 1,
            "sub_operator_params": [],
            "sub_operator_kernel_type": "KERNEL_TYPE_AIV_ONLY",
            "sub_operator_kernel_name": {
                "AiCore": {"func_name": "test_func", "obj_files": "test.o"}
            },
            "sub_operator_early_start_set_flag": False,
            "sub_operator_early_start_wait_flag": False,
            "sub_operator_call_dcci_before_kernel_start": False,
            "sub_operator_call_dcci_after_kernel_end": False,
            "sub_operator_call_dcci_disable_on_kernel": False,
            "sub_operator_op_type": "Add",
            "debugOptions": "",
        }
        with mock.patch("builtins.open", new_callable=mock.mock_open, read_data="{}"):
            with mock.patch("json.load", return_value=json_data5):
                sub_op5 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op5.o",
                        "json_path": "op5.json",
                        "kernel_name": "op5",
                    },
                    100,
                    op_options5,
                    compile_log_path,
                )
                sub_op5.sub_op_task_type = SubOperatorType.DYNAMIC_OP
                sub_op5.init_of_sub_operator_info()
                self.assertFalse(sub_op5.call_dcci_before_kernel_start)
                self.assertTrue(sub_op5.call_dcci_disable_on_kernel)
                # Verify DCCI call blocks respect disable flag
                dcci_before_block = sub_op5.gen_dcci_before_kernel_start_call_block()
                self.assertEqual(dcci_before_block, "")

    def test_dcci_options_parsing_empty_lists(self):
        """Test DCCI options parsing with empty or invalid input"""
        compile_log_path = "./tmp/"

        # Test case 1: Empty string
        op_options1 = {
            "split-mode": 4,
            "dcci-before-kernel-start": "",
            "dcci-after-kernel-end": "",
            "dcci-disable-on-kernel": "",
        }
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                sub_op1 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op1.o",
                        "json_path": "op1.json",
                        "kernel_name": "op1",
                    },
                    100,
                    op_options1,
                    compile_log_path,
                )
                self.assertEqual(sub_op1.dcci_before_kernel_start_op_list, [])
                self.assertEqual(sub_op1.dcci_after_kernel_end_op_list, [])
                self.assertEqual(sub_op1.dcci_disable_on_kernel_op_list, [])

        # Test case 2: Whitespace and empty elements
        op_options2 = {
            "split-mode": 4,
            "dcci-before-kernel-start": "Add, , MatMul,",
        }
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                sub_op2 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op2.o",
                        "json_path": "op2.json",
                        "kernel_name": "op2",
                    },
                    100,
                    op_options2,
                    compile_log_path,
                )
                self.assertEqual(
                    sub_op2.dcci_before_kernel_start_op_list, ["Add", "MatMul"]
                )

        # Test case 3: Option not present (default to empty list)
        op_options3 = {
            "split-mode": 4,
        }
        with mock.patch(
            "builtins.open", new_callable=mock.mock_open, read_data="{}"
        ) as mock_open:
            with mock.patch("json.load", return_value=op_json):
                sub_op3 = SubOperatorInfos(
                    0,
                    {
                        "bin_path": "./op3.o",
                        "json_path": "op3.json",
                        "kernel_name": "op3",
                    },
                    100,
                    op_options3,
                    compile_log_path,
                )
                self.assertEqual(sub_op3.dcci_before_kernel_start_op_list, [])
                self.assertEqual(sub_op3.dcci_after_kernel_end_op_list, [])
                self.assertEqual(sub_op3.dcci_disable_on_kernel_op_list, [])


if __name__ == "__main__":
    unittest.main()
