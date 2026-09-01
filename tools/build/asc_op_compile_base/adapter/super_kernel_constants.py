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
super kernel constants
"""

import enum


CALL_INSTS = "CALLR"
FUNC_STR = "func_name"
OBJ_FILES_STR = "obj_files"
AI_CORE_STR = "AiCore"
ERR_CODE = "EB0500"


class SuperKernelDeviceType(enum.Enum):
    """super kernel device type"""

    KERNEL_DEVICE_TYPE_AIV = 0
    KERNEL_DEVICE_TYPE_AIC = 1
    KERNEL_DEVICE_TYPE_MIX = 2
    KERNEL_DEVICE_TYPE_MAX = 3


class SuperKernelEarlyStartMode(enum.Enum):
    """early start mode"""

    EarlyStartDisable = 0
    EarlyStartEnableV1 = 1
    EarlyStartEnableV2 = 2
    EarlyStartV2DisableSubKernel = 3


class SuperKernelFeedSyncAllMode(enum.Enum):
    """feed sync all mode"""

    FeedSyncAllDisable = 0
    FeedSyncAllEnable = 1


class SuperKernelLinkMode(enum.Enum):
    """super kernel link mode"""

    PerVecHerCube = 0
    PerCubeHerVec = 1
    PerCubeHerVecWithSuper = 2


class SuperKernelStreamFusionMode(enum.Enum):
    """stream fusion mode"""

    StreamFusionDisable = 0
    StreamFusionEnable = 1


class SubOperatorType(enum.Enum):
    STATIC_OP = 0
    DYNAMIC_OP = 1
    NOP_OP = 2


class SuperKernelPreLoadMode(enum.Enum):
    """preload Mode"""

    PreLoadStepByStep = 0
    PreLoadByWhole = 1
    PreloadByAdanvanceStep = 2
    PreloadNA = 3


class SuperKernelDebugDcciAllMode(enum.Enum):
    """debug dcci all mode"""

    DebugDcciAllDisable = 0
    DebugDcciAllEnable = 1


class SuperKernelDebugSyncAllMode(enum.Enum):
    """debug sync all mode"""

    DebugSyncAllDisable = 0
    DebugSyncAllEnable = 1


class SuperKernelDataCacheMode(enum.Enum):
    """super kernel link mode"""

    DataCacheLoadAdancanceStep = 0
    DataCacheLoadNA = 1


class SuperKernelProfilingMode(enum.Enum):
    """stream fusion mode"""

    ProfilingDisable = 0
    ProfilingEnable = 1


class SuperKernelKernelType(enum.Enum):
    """super kernel kernel type."""

    KERNEL_TYPE_AIV_ONLY = 0
    KERNEL_TYPE_AIC_ONLY = 1
    KERNEL_TYPE_MIX_AIV_HARD_SYNC = 2  # same as KERNEL_TYPE_MIX_AIV_1_0
    KERNEL_TYPE_MIX_AIC_HARD_SYNC = 3  # same as KERNEL_TYPE_MIX_AIC_1_0
    KERNEL_TYPE_MIX_AIV_1_0 = 4
    KERNEL_TYPE_MIX_AIC_1_0 = 5
    KERNEL_TYPE_MIX_AIC_1_1 = 6
    KERNEL_TYPE_MIX_AIC_1_2 = 7
    KERNEL_TYPE_AICORE = 8
    KERNEL_TYPE_VECTORCORE = 9  # donot use in sk
    KERNEL_TYPE_MIX_AICORE = 10  # donot use in sk
    KERNEL_TYPE_MIX_VECTOR_CORE = 11  # donot use in sk
    KERNEL_TYPE_MAX = 12


STR_TO_SK_KERNEL_TYPE = {
    "KERNEL_TYPE_AIV_ONLY": SuperKernelKernelType.KERNEL_TYPE_AIV_ONLY,
    "KERNEL_TYPE_AIC_ONLY": SuperKernelKernelType.KERNEL_TYPE_AIC_ONLY,
    "KERNEL_TYPE_MIX_AIV_1_0": SuperKernelKernelType.KERNEL_TYPE_MIX_AIV_1_0,
    "KERNEL_TYPE_MIX_AIC_1_0": SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_0,
    "KERNEL_TYPE_MIX_AIC_1_1": SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_1,
    "KERNEL_TYPE_MIX_AIC_1_2": SuperKernelKernelType.KERNEL_TYPE_MIX_AIC_1_2,
}


STR_TO_SUPER_TASK_TYPE = {
    "normal": SubOperatorType.STATIC_OP,
    "dynamic": SubOperatorType.DYNAMIC_OP,
    "nop": SubOperatorType.NOP_OP,
}
