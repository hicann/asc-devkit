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


class CodeGeneratorConfig:
    def __init__(
        self,
        transform_kernel_type,
        dfx_gen_meta_info,
        dfx_param_byte_unit,
        need_clear_workspace,
        gen_code_for_l2_cache,
        write_back_overflow,
        needs_ffts,
        support_dci,
        support_raw_aic_only_dump_tensor,
        support_mix,
        dci_macro,
        gen_isolation_macro_type,
        cube_core_type_macro,
        vec_core_type_macro,
        support_workspace_offset,
    ):
        self.transform_kernel_type = transform_kernel_type
        self.dfx_gen_meta_info = dfx_gen_meta_info
        self.dfx_param_byte_unit = dfx_param_byte_unit
        self.need_clear_workspace = need_clear_workspace
        self.gen_code_for_l2_cache = gen_code_for_l2_cache
        self.write_back_overflow = write_back_overflow
        self.needs_ffts = need_ffts
        self.support_dci = support_dci
        self.support_raw_aic_only_dump_tensor = support_raw_aic_only_dump_tensor
        self.support_mix = support_mix
        self.dci_macro = dci_macro
        self.gen_isolation_macro_type = gen_isolation_macro_type
        self.cube_core_type_macro = cube_core_type_macro
        self.vec_core_type_macro = vec_core_type_macro
        self.support_workspace_offset = support_workspace_offset


class CodeGeneratorParams:
    def __init__(self, compile_info, op_info, compile_option_tuple, tiling_info, configuration, kernel_func_name):
        pass


class CodeGenerator:
    def __init__(self, params: CodeGeneratorParams, config: CodeGeneratorConfig):
        pass

    def generate(self):
        pass
