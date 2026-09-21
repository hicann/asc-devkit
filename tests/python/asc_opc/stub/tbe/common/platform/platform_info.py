#! /usr/bin/env python3
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
def set_current_compile_soc_info(soc_version, core_type="AiCore", aicore_num=None, l1_fusion=None):
    print("Set current compile soc info")
    return


def intrinsic_check_support(var):
    return False


platform_info_dict = {"core_type": "VectorCore", "cube_vector_combine": "split", "SHORT_SOC_VERSION": "ascend910"}


def get_soc_spec(key):
    return platform_info_dict.get(key, None)


def set_soc_spec(dict):
    return platform_info_dict.update(dict)
