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
tbe register API:
To make it easy to manage operator registration information, TBE provides a set of register APIs.
"""

from asc_op_compile_base.common.register import operation_func_mgr, fusion_build_config_mgr


def register_operator(op_type, pattern=None, trans_bool_to_s8=True):
    """
    register op realization func

    Parameters
    ----------
    op_type : string
        op type
    pattern: string
        op fusion pattern

    Returns
    -------
    decorator : decorator
        decorator to register realization func
    """

    return operation_func_mgr.register_operator(op_type, pattern, trans_bool_to_s8)


def get_operator(op_type):
    """
    get op realization func info

    Parameters
    ----------
    op_type : string
        op_func_name(old process) or op type(new process)

    Returns
    -------
    object of class Operator
    """

    return operation_func_mgr.get_operator(op_type)


def register_param_generalization(op_type):
    """
    register op param generalization func

    Parameters
    ----------
    op_type : string
        op type

    Returns
    -------
    decorator : decorator
        decorator to register realization func
    """

    return operation_func_mgr.register_param_generalization(op_type)


def get_param_generalization(op_type):
    """
    get op param generalization func

    Parameters
    ----------
    op_type : string

    Returns
    -------
    op param generalization func
    """

    return operation_func_mgr.get_param_generalization(op_type)


def get_fusion_buildcfg(op_type=None):
    """
    get fusion build config

    Parameters
    ----------
    op_type : string
        op type

    Returns
    -------
    object of class FusionBuildCfg
    """

    return fusion_build_config_mgr.get_fusion_buildcfg(op_type)


def set_fusion_buildcfg(op_type, build_config):
    """
    register fusion build config

    Parameters
    ----------
    op_type : string
        op type
    config: dict
        build configs

    Returns
    -------
    decorator : decorator
        decorator to set fusion build config
    """

    fusion_build_config_mgr.set_fusion_buildcfg(op_type, build_config)


def register_op_compute(op_type, op_mode="dynamic", support_fusion=True, **kwargs):
    """
    register op compute func

    Parameters
    ----------
    op_type : string
        op_func_name(old process) or op type(new process)
    op_mode: string
        dynamic or static shape
    support_fusion: bool
        support dynamic shape UB fusion
    **kwargus:
        - support_bfp16: support bfloat16 dtype

    Returns
    -------
    decorator : decorator
        decorator to register compute func
    support_bfp16 case:
        decorator to return output_tensor
    """

    return operation_func_mgr.register_op_compute(op_type, op_mode, support_fusion, **kwargs)


def get_op_compute(op_type, op_mode="dynamic"):
    """
    get op compute func info

    Parameters
    ----------
    op_type : string
        op_func_name(old process) or op type(new process)
    op_mode: string
        dynamic or static shape

    Returns
    -------
    object of class OpCompute
    """

    return operation_func_mgr.get_op_compute(op_type, op_mode)
