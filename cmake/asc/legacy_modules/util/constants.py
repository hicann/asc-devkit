#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

import re
from host_stub_util import CodeMode

FUNC_SIGNATURE = re.compile(
    r"(?:template\s*<\s*([^>]+?)\s*>\s*\n?\s*)?"
    r"__attribute__\(\(cce_kernel\)\)\s*"
    r"(?:\[aicore\]|__attribute__\(\(cce_aicore\)\))\s*"
    r"(.+?)\s*\{",
    re.DOTALL,
)
FUNC_PARAMS = re.compile(r"\((.+)\)", re.DOTALL)

STRUCT = re.compile(r"^\s*struct\s+(\w+)\s*\{", re.MULTILINE)

CCE_GLOBAL = "__attribute__((cce_global))"
FUNC_PARAM_SKIPS = ("__restrict__", "__restrict", "struct")

ATTRIBUTE = re.compile(r"__attribute__\(\(\w+\)\)")

COMMENT = re.compile(r"[ \t]*//.*")

TYPE_BASENAME = re.compile(r"\w+")

MIX_CORE_MACRO = "__MIX_CORE_MACRO__"

FUN_TEMPLATE_HASH_TILING_KEY_BASE = 1000000

STR_TO_KERNEL_TYPE_V200 = {
    "KERNEL_TYPE_AICORE": CodeMode.AIC,
    "KERNEL_TYPE_VECTORCORE": CodeMode.AIC,
    "KERNEL_TYPE_MIX_VECTOR_CORE": CodeMode.MIX_VECTOR_CORE,
}

STR_TO_KERNEL_TYPE_V220 = {
    "KERNEL_TYPE_AIV_ONLY": CodeMode.KERNEL_TYPE_AIV_ONLY,
    "KERNEL_TYPE_AIC_ONLY": CodeMode.KERNEL_TYPE_AIC_ONLY,
    "KERNEL_TYPE_MIX_AIV_1_0": CodeMode.KERNEL_TYPE_MIX_AIV_1_0,
    "KERNEL_TYPE_MIX_AIC_1_0": CodeMode.KERNEL_TYPE_MIX_AIC_1_0,
    "KERNEL_TYPE_MIX_AIC_1_1": CodeMode.KERNEL_TYPE_MIX_AIC_1_1,
    "KERNEL_TYPE_MIX_AIC_1_2": CodeMode.KERNEL_TYPE_MIX_AIC_1_2,
}

STR_TO_KERNEL_TYPE_C310 = {
    "KERNEL_TYPE_AIV_ONLY": CodeMode.KERNEL_TYPE_AIV_ONLY,
    "KERNEL_TYPE_AIC_ONLY": CodeMode.KERNEL_TYPE_AIC_ONLY,
    "KERNEL_TYPE_MIX_AIV_1_0": CodeMode.KERNEL_TYPE_MIX_AIV_1_0,
    "KERNEL_TYPE_MIX_AIC_1_0": CodeMode.KERNEL_TYPE_MIX_AIC_1_0,
    "KERNEL_TYPE_MIX_AIC_1_1": CodeMode.KERNEL_TYPE_MIX_AIC_1_1,
    "KERNEL_TYPE_MIX_AIC_1_2": CodeMode.KERNEL_TYPE_MIX_AIC_1_2,
}
