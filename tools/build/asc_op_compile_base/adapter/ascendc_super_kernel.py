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
ascendc super kernel
"""

from .super_kernel import compile


def ascendc_super_kernel_plus(kernel_infos, called_kernel_name="ascendc_super_kernel_plus", impl_mode=""):
    """entry of super kernel compile

    Args:
        kernel_infos: infos of sub kernel
            {
                "op_list":
                    [{"op1": {"bin_path": "", "json_path": ""}, "op2": {xxx}}],
                "super_kernel_options": compile_option
            }
        called_kernel_name: super kernel name
    """
    compile(kernel_infos, called_kernel_name, impl_mode)
    return
