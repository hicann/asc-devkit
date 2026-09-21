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
from ..ascendc_constants import InferChannelParams


class CompileInfoGeneratorConfig:
    def __init__(
        self,
        pre_compile_type,
        set_default_code_channel,
        need_find_kernel_type,
        enable_kernel_type_mix_n,
        enable_kernel_type_mix,
        enable_simt,
        dump_size_multiples,
        chip_version,
        decode_mode_v310,
    ):
        self.pre_compile_type = pre_compile_type
        self.set_default_code_channel = set_default_code_channel
        self.need_find_kernel_type = need_find_kernel_type
        self.enable_kernel_type_mix_n = enable_kernel_type_mix_n
        self.enable_kernel_type_mix = enable_kernel_type_mix
        self.enable_simt = enable_simt
        self.dump_size_multiples = dump_size_multiples
        self.chip_version = chip_version
        self.decode_mode_v310 = decode_mode_v310


class CompileInfoGeneratorParams:
    def __init__(self, kernel_file, kernel_func_name, op_info, compile_option_tuple, cmd_compiler):
        pass


class CompileInfoGenerator(object):
    def __init__(self, params: CompileInfoGeneratorParams, config: CompileInfoGeneratorConfig):
        pass

    def collect_by_precompile(self):
        pass

    def get_compile_info(self):
        pass

    def collect_by_tiling_compile(self, configuration, tiling_info):
        pass
