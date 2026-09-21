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
from ..ascendc_constants import CompileOptionTuple


class InitializerConfig:
    def __init__(self, support_impl_mode: bool, is_support_dfx: bool, support_super_kernel: bool):
        self.support_impl_mode = support_impl_mode
        self.is_support_dfx = is_support_dfx
        self.support_super_kernel = support_super_kernel


class InitializerParams:
    def __init__(
        self,
        kernel_file,
        origin_func_name,
        op_info,
        compile_option_tuple,
        code_channel,
        op_compile_option,
        extend_options,
    ):
        pass


class Initializer(object):
    def __init__(self, params: InitializerParams, config: InitializerConfig):
        pass

    def initialize(self):
        pass
