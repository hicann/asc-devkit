#!/usr/bin/env python
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
"""
opc api
"""

import sys
from asc_op_compile_base.common.utils import log as logger
from opc import OpcOptionParser
from op_compilation import OpCompilation
from op_info_store import load_op_info_store
from constant import OpcOptions, OpcCompileMode


def compile_op(op_params, build_options):
    """
    Provides API compile operator for external callers.
    """
    opt_parser = OpcOptionParser()
    res = opt_parser.parse_build_options(build_options)
    if not res:
        logger.error("parse_build_options is failed.")
    opt_parser.set_option(OpcOptions.OP_COMPILE_MODE, OpcCompileMode.SINGLE_OP_DICT_MODE)

    if not opt_parser.check_input_params():
        logger.error("Opc tool compile failed.")
        return False

    load_op_info_store(opt_parser.get_option(OpcOptions.SOC_VERSION))
    opt_parser.set_option(OpcOptions.OP_PARAMS, op_params)
    op_compile = OpCompilation(opt_parser.get_all_options())
    if op_compile.op_compilation():
        logger.info("Opc tool compile success.")
        return True
    else:
        logger.error("Opc tool compile failed.")
        return False
