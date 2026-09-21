#!/usr/bin/python
# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
"""
ascendc constants
"""

import enum
from collections import namedtuple
from typing import Dict, Optional, Any
from dataclasses import dataclass, asdict, field


"""CORE_TYPE_* is used to set is AscendC-api work
CORE_TYPE_MIX means both AscendC cube and vector api is work, i.e. V100/V200;
CORE_TYPE_CUBE means only AscendC cube api is work, vector api is empty
CORE_TYPE_VEC means only AscendC vector api is work, cube api is empty
"""
CORE_TYPE_MIX = 0
CORE_TYPE_CUBE = 1
CORE_TYPE_VEC = 2
MIX_CORE_MACRO = "__MIX_CORE_MACRO__"
ASCENDC_OOM = "ASCENDC_OOM"
TILING_KEY_MACRO = "TILING_KEY_VAR"


INPUT_OUTPUT_DTYPE_LEN = {
    "float": 4,
    "bool": 1,
    "int32": 4,
    "int64": 8,
    "half": 2,
    "uint32": 4,
    "uint64": 8,
    "int4": 0.5,
    "int8": 1,
    "int16": 2,
    "uint8": 1,
    "uint16": 2,
    "float16": 2,
    "bfloat16": 2,
    "float32": 4,
    "double": 8,
    "float8_e5m2": 1,
    "float8_e4m3fn": 1,
    "hifloat8": 1,
    "float8_e8m0": 1,
    "float4_e2m1": 1,
    "float4_e1m2": 1,
    "complex32": 4,
    "complex64": 8,
    "complex128": 16,
    "uint1": 0.125,
}

InferChannelParamsFromIFile = namedtuple(
    "InferChannelParamsFromIFile",
    [
        "tiling_key_list",
        "code_channel",
        "hard_sync",
        "no_kfc_server_flag",
        "enable_deterministic",
        "tiling_key_kernel_type",
        "no_set_kernel_type",
        "default_kernel_type",
        "dump_info",
        "template_tiling_info",
        "default_tiling_struct",
        "tiling_struct_expr_map",
        "tiling_key_struct_map",
        "register_tiling_struct",
        "tpl_tiling_struct",
        "super_kernel_early_start_set_flag",
        "super_kernel_early_start_wait_flag",
        "tiling_key_deterministic",
        "tiling_key_group_map",
    ],
)
InferChannelParams = namedtuple(
    "InferChannelParams",
    [
        "src_file",
        "dst_file_header",
        "compile_option_tuple",
        "tiling_key",
        "tiling_info",
        "compile_log_path",
        "no_kfc_server_flag",
    ],
)


class KernelMetaType(enum.Enum):
    """function meta type."""

    KERNEL_TYPE_AIV_ONLY = 0
    KERNEL_TYPE_AIC_ONLY = 1
    KERNEL_TYPE_MIX_AIV_HARD_SYNC = 2
    KERNEL_TYPE_MIX_AIC_HARD_SYNC = 3
    KERNEL_TYPE_MIX_AIV_1_0 = 4
    KERNEL_TYPE_MIX_AIC_1_0 = 5
    KERNEL_TYPE_MIX_AIC_1_1 = 6
    KERNEL_TYPE_MIX_AIC_1_2 = 7
    KERNEL_TYPE_AICORE = 8
    KERNEL_TYPE_VECTORCORE = 9
    KERNEL_TYPE_MIX_AICORE = 10
    KERNEL_TYPE_MIX_VECTOR_CORE = 11
    KERNEL_TYPE_MAX = 12


@dataclass
class TilingKeyConfig:
    """
    Necessary information for compile per tiling key.
    """

    kernel_type: KernelMetaType = KernelMetaType.KERNEL_TYPE_MAX
    use_kfc: bool = False
    enable_deterministic: bool = False
    tiling_struct_name: str = ""
    template_tiling_info: Optional[Dict[str, Any]] = field(default_factory=dict)


@dataclass
class CustomizedConfig:
    """
    Inferred Informations of a given kernel source code.
    """

    default_kernel_type: KernelMetaType = KernelMetaType.KERNEL_TYPE_MAX
    default_tiling_struct_name: str = ""
    tiling_key_infos: Optional[Dict[str, TilingKeyConfig]] = field(default_factory=dict)
    debug_info: Optional[Dict[str, str]] = field(default_factory=dict)


STR_TO_KERNEL_TYPE_V220 = {
    "KERNEL_TYPE_AIV_ONLY": KernelMetaType.KERNEL_TYPE_AIV_ONLY,
    "KERNEL_TYPE_AIC_ONLY": KernelMetaType.KERNEL_TYPE_AIC_ONLY,
    "KERNEL_TYPE_MIX_AIV_1_0": KernelMetaType.KERNEL_TYPE_MIX_AIV_1_0,
    "KERNEL_TYPE_MIX_AIC_1_0": KernelMetaType.KERNEL_TYPE_MIX_AIC_1_0,
    "KERNEL_TYPE_MIX_AIC_1_1": KernelMetaType.KERNEL_TYPE_MIX_AIC_1_1,
    "KERNEL_TYPE_MIX_AIC_1_2": KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2,
}


STR_TO_KERNEL_TYPE_V200 = {
    "KERNEL_TYPE_AICORE": KernelMetaType.KERNEL_TYPE_AICORE,
    "KERNEL_TYPE_VECTORCORE": KernelMetaType.KERNEL_TYPE_VECTORCORE,
    "KERNEL_TYPE_MIX_AICORE": KernelMetaType.KERNEL_TYPE_MIX_AICORE,
    "KERNEL_TYPE_MIX_VECTOR_CORE": KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE,
}

STR_TO_KERNEL_TYPE_L300 = {
    "KERNEL_TYPE_AICORE": KernelMetaType.KERNEL_TYPE_AICORE,
}

STR_TO_KERNEL_TYPE_L311 = {
    "KERNEL_TYPE_AICORE": KernelMetaType.KERNEL_TYPE_AICORE,
}


KERNEL_TYPE_TO_STR = {
    KernelMetaType.KERNEL_TYPE_AIV_ONLY: "KERNEL_TYPE_AIV_ONLY",
    KernelMetaType.KERNEL_TYPE_AIC_ONLY: "KERNEL_TYPE_AIC_ONLY",
    KernelMetaType.KERNEL_TYPE_MIX_AIV_HARD_SYNC: "KERNEL_TYPE_MIX_AIV_HARD_SYNC",
    KernelMetaType.KERNEL_TYPE_MIX_AIC_HARD_SYNC: "KERNEL_TYPE_MIX_AIC_HARD_SYNC",
    KernelMetaType.KERNEL_TYPE_MIX_AIV_1_0: "KERNEL_TYPE_MIX_AIV_1_0",
    KernelMetaType.KERNEL_TYPE_MIX_AIC_1_0: "KERNEL_TYPE_MIX_AIC_1_0",
    KernelMetaType.KERNEL_TYPE_MIX_AIC_1_1: "KERNEL_TYPE_MIX_AIC_1_1",
    KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2: "KERNEL_TYPE_MIX_AIC_1_2",
    KernelMetaType.KERNEL_TYPE_AICORE: "KERNEL_TYPE_AICORE",
    KernelMetaType.KERNEL_TYPE_VECTORCORE: "KERNEL_TYPE_VECTORCORE",
    KernelMetaType.KERNEL_TYPE_MIX_AICORE: "KERNEL_TYPE_MIX_AICORE",
    KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE: "KERNEL_TYPE_MIX_VECTOR_CORE",
    KernelMetaType.KERNEL_TYPE_MAX: "KERNEL_TYPE_MAX",
}


class CompileOptionTuple:
    def __init__(self, compile_options, mllvm_options):
        self.compile_options = compile_options
        self.mllvm_options = mllvm_options
