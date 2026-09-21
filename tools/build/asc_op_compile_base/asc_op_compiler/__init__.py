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
AscendC adapter APIs package.
"""

from .check_op import check_op_cap
from .check_op import generalize_op_params
from .compile_op import compile_op, compile_op_with_customized_config
from .compile_op import replay_op
from .compile_op import get_code_channel
from .get_op_tiling import OpInfo
from asc_op_compile_base.common.utils.log_utils import LogUtil, AscendCLogLevel
from .global_storage import global_var_storage
from .ascendc_common_utility import CommonUtility
from .ascendc_compile_dfx import DFXSectionGenerator
from .ascendc_compile_v220 import (
    gen_compile_cmd_v220,
    get_v220_kernel_type_mix_flag,
    compile_single_tiling_v220,
    call_bisheng_v220,
    get_ktype_section_variable,
)
from .ascendc_constants import KernelMetaType, STR_TO_KERNEL_TYPE_V220, TilingKeyConfig, CustomizedConfig
from .ascendc_compile_base import compile_multi_tilingkey, link_relocatable, fatbin_objs
from .ascendc_compile_v200 import gen_compile_cmd_v200
from .ascendc_compile_gen_code import get_code_for_l2_cache, gen_global_isolation_macro
