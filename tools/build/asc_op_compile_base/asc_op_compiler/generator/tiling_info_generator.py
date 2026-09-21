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
from ..get_op_tiling import OpInfo


class TilingInfoGeneratorConfig:
    def __init__(
        self,
        enable_vd: bool,
        with_time_stamp_dynamic: bool,
        with_time_stamp_static: bool,
        tiling_format,
        macro_def,
        need_const_tiling_qualifier,
    ):
        self.enable_vd = enable_vd
        self.with_time_stamp_dynamic = with_time_stamp_dynamic
        self.with_time_stamp_static = with_time_stamp_static
        self.tiling_format = tiling_format
        self.macro_def = macro_def
        self.need_const_tiling_qualifier = need_const_tiling_qualifier


class TilingInfoGeneratorParams:
    def __init__(self, op_info, configuration, extend_options):
        pass


class TilingInfoGenerator(object):
    def __init__(self, params: TilingInfoGeneratorParams, config: TilingInfoGeneratorConfig):
        pass

    def parse(self):
        pass
