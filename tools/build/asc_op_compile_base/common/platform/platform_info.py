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
import functools

from asc_op_compile_base.c_api import (
    _get_soc_spec,
    _set_soc_spec,
    _init_soc_spec,
    _te_update_version,
    _set_platform_info_res,
    _set_core_num_by_core_type,
)

# def the cce vector intrinsic params
VECTOR_INST_BLOCK_WIDTH = 256


# represent 5 soc, currently contains in tik
ASCEND_310 = "Ascend310"
ASCEND_310B = "Ascend310B"
AS31XM1 = "AS31XM1"
ASCEND_031 = "Ascend031"
ASCEND_035 = "Ascend035"
ASCEND_035A = "Ascend035A"
ASCEND_035B = "Ascend035B"
ASCEND_910 = "Ascend910"
ASCEND_910H = "Ascend910B"
ASCEND_910B = "Ascend910B"
ASCEND_910_93 = "Ascend910_93"
ASCEND_950 = "Ascend950"
ASCEND_350 = "Ascend350"
ASCEND_910M = "Ascend910A"
ASCEND_910P = "Ascend910ProA"
MC62 = "MC62"
HI3796CV300ES = "Hi3796CV300ES"
HI3796CV300CS = "Hi3796CV300CS"
SD3403 = "SD3403"
ASCEND_610 = "Ascend610"
ASCEND_610LITE = "Ascend610Lite"
BS9SX2A = "BS9SX2A"
MC61AM21A = "MC61AM21A"
ASCEND_310P = "Ascend310P"
BS9SX1A = "BS9SX1A"
ASCEND_610B = "Ascend610B"
ASCEND_SD = "SD3403"
KIRIN_X90 = "KirinX90"
KIRIN_9030 = "Kirin9030"
_AIC_ENGINE = "AiCore"
_VEC_ENGINE = "VectorCore"
MC32DM11A = "MC32DM11A"

AIC_310P = ASCEND_310P + _AIC_ENGINE
VEC_310P = ASCEND_310P + _VEC_ENGINE
AIC_610 = ASCEND_610 + _AIC_ENGINE
VEC_610 = ASCEND_610 + _VEC_ENGINE
VEC_BS9SX1A = BS9SX1A + _VEC_ENGINE
AIC_BS9SX1A = BS9SX1A + _AIC_ENGINE
AIC_610B = ASCEND_610B + _AIC_ENGINE
VEC_610B = ASCEND_610B + _VEC_ENGINE
AIC_310B = ASCEND_310B + _AIC_ENGINE
VEC_310B = ASCEND_310B + _VEC_ENGINE
AIC_AS31XM1 = AS31XM1 + _AIC_ENGINE
VEC_AS31XM1 = AS31XM1 + _VEC_ENGINE
HI3796CV300ESAIC = HI3796CV300ES + _AIC_ENGINE
HI3796CV300CSAIC = HI3796CV300CS + _AIC_ENGINE
SD3403AIC = SD3403 + _AIC_ENGINE
ASCEND_910BAIC = ASCEND_910B + _AIC_ENGINE
ASCEND_910BVEC = ASCEND_910B + _VEC_ENGINE
ASCEND_910_93AIC = ASCEND_910_93 + _AIC_ENGINE
ASCEND_910_93VEC = ASCEND_910_93 + _VEC_ENGINE
ASCEND_SD_AIC = "SD3403" + _AIC_ENGINE

# Example: pylint: disable=invalid-name
scope_cbuf = "local.L1"

SOC_VERSION = "SOC_VERSION"
FULL_SOC_VERSION = "FULL_SOC_VERSION"
SHORT_SOC_VERSION = "SHORT_SOC_VERSION"
AICORE_TYPE = "AICORE_TYPE"
CORE_NUM = "CORE_NUM"
UB_SIZE = "UB_SIZE"
L2_SIZE = "L2_SIZE"
L1_SIZE = "L1_SIZE"
CUBE_SIZE = "CUBE_SIZE"
L0A_SIZE = "L0A_SIZE"
L0B_SIZE = "L0B_SIZE"
L0C_SIZE = "L0C_SIZE"
SMASK_SIZE = "SMASK_SIZE"
UNZIP = "UNZIP"
VREG_SIZE = "VREG_SIZE"
AREG_SIZE = "AREG_SIZE"
PREG_SIZE = "PREG_SIZE"
UREG_SIZE = "UREG_SIZE"
WREG_SIZE = "WREG_SIZE"
WIDE_REG_WIDTH = "WIDE_REG_WIDTH"
CUBE_VECTOR_SPLIT = "CUBE_VECTOR_SPLIT"
COMPILER_ARCH = "Compiler_arch"
FB_SIZE = "FB_SIZE"
FB0_SIZE = "FB0_SIZE"
FB1_SIZE = "FB1_SIZE"
FB2_SIZE = "FB2_SIZE"
FB3_SIZE = "FB3_SIZE"
BT_SIZE = "BT_SIZE"
CORE_TYPE_LIST = "CORE_TYPE_LIST"
L0A_LAYOUT_IS_zN = "L0A_LAYOUT_IS_zN"
UB_BLOCK_SIZE = "ubblock_size"


def get_soc_spec(key):
    """
    call global func to get soc spec.

    Parameters
    ----------
    key

    Returns
    -------
    value
    """

    value = _get_soc_spec(key)
    if value == "":
        raise RuntimeError("Unsupported Key Value of get_soc_spec(): %s" % key)

    str2int_list = (
        CORE_NUM,
        UB_SIZE,
        L2_SIZE,
        L1_SIZE,
        L0A_SIZE,
        L0B_SIZE,
        L0C_SIZE,
        SMASK_SIZE,
        VREG_SIZE,
        WREG_SIZE,
        WIDE_REG_WIDTH,
        AREG_SIZE,
        PREG_SIZE,
        UREG_SIZE,
        CUBE_VECTOR_SPLIT,
        FB_SIZE,
        FB0_SIZE,
        FB1_SIZE,
        FB2_SIZE,
        FB3_SIZE,
        BT_SIZE,
        UB_BLOCK_SIZE,
    )

    str2bool_list = (L0A_LAYOUT_IS_zN,)

    if key in str2int_list:
        try:
            value = int(value)
        except Exception:
            raise RuntimeError("return value %s is not 'int' type" % value)
    elif key in (CUBE_SIZE, UNZIP):
        value_str_list = value.split(",")
        value_int_list = []
        for i in value_str_list:
            try:
                value_int_list.append(int(i))
            except Exception:
                raise RuntimeError("return value %s is not 'int' type" % value)
        value = value_int_list
    elif key == CORE_TYPE_LIST:
        value_str_list = value.split(",")
        value = value_str_list
    elif key in str2bool_list:
        try:
            # "0" stands for False, "1" stands for True
            int_value = int(value)
            assert 0 <= int_value <= 1
            value = int_value == 1
        except Exception as casting_error:
            raise RuntimeError(
                "return value %s cannot be interpret as 'bool' type" % value
            ) from casting_error
    return value


# Example: pylint: disable=unused-argument
def set_current_compile_soc_info(
    soc_version, core_type="AiCore", aicore_num=None, l1_fusion=None
):
    """
    set version info

    Parameters
    ----------
    soc_version : str
    -    "Ascend310"/"Ascend910"/"Ascend310P1~P4"/"Ascend610" ...
    core_type : str
    -    "AiCore" or "VectorCore"
    aicore_num: int
    -    example: 32
    l1_fusion: bool
    -    example: True/False

    Returns
    -------
    errmsg : str
    -    error message, 'success' for OK.
    """
    if core_type in (None, ""):
        core_type = "AiCore"
    if aicore_num in [None, "0", 0, ""]:
        aicore_num = ""
    else:
        aicore_num = str(aicore_num)
    if l1_fusion is None:
        l1_fusion = ""
    l1_fusion = str(l1_fusion).lower()
    value = _init_soc_spec(soc_version, core_type, aicore_num, l1_fusion)
    if value != "success":
        raise RuntimeError("set_current_compile_soc_info() return error.")

    return value


def set_soc_spec(key):
    """
    set soc_version or core_type or aicore_num or l1_fusion

    Parameters
    ----------
    key

    Returns
    -------
    errmsg : str
    -    error message, 'success' for OK.
    """
    if key is True:
        key = "true"
    elif key is False:
        key = "false"
    else:
        key = str(key)
    value = _set_soc_spec(key)
    if value != "success":
        raise RuntimeError("set_soc_spec() return error.")

    return value


class KernelName:
    """
    store kernel name, to across python > c++ > python
    """

    kernel_name = ""

    @classmethod
    def set_kernel_name(cls, kernel_name):
        """
        set kernel name
        """
        cls.kernel_name = kernel_name

    @classmethod
    def get_kernel_name(cls):
        """
        return kernel name set before
        """
        return cls.kernel_name


def get_block_size():
    """
    get the block size of the currently set chip.
    """
    return get_soc_spec(UB_BLOCK_SIZE)


def te_update_version(
    soc_version=None,
    core_type=None,
    aicore_num=None,
    l1_fusion=None,
    l2_mode=None,
    l2_fusion=None,
    kwargs=None,
):
    """
    set version info

    Parameters
    ----------
    soc_version : str
    -    "Ascend310"/"Ascend910"/"Ascend310P1~P4"/"Ascend610" ...
    core_type : str
    -    "AiCore" or "VectorCore"
    aicore_num: int
    -    example: 32
    l1_fusion: bool
    -    example: True/False

    Returns
    -------
    errmsg : str
    -    error message, 'success' for OK.
    """
    if soc_version is None:
        soc_version = ""
    if core_type in (None, ""):
        core_type = ""
    if aicore_num in [None, "0", 0, ""]:
        aicore_num = ""
    else:
        aicore_num = str(aicore_num)
    if l1_fusion is None:
        l1_fusion = ""
    else:
        l1_fusion = str(l1_fusion).lower()
    value = _te_update_version(soc_version, core_type, aicore_num, l1_fusion)

    if value != "success":
        raise RuntimeError("te_update_version() return error.")

    return value


def set_platform_info_res(device_id, res):
    """
    set platform info
    Parameters
    ----------
    device_id: device id
    res: map contains aic num, aiv num, hbm size, l2 size, memory bw

    Returns
    -------

    """
    if device_id not in ("", None) and res:
        value = _set_platform_info_res(int(device_id), res)
        if not value:
            raise RuntimeError("set_platform_info_res return error!")


def set_core_num_by_core_type(core_type):
    """
    set core number by core type
    Parameters
    ----------
    core_type: core type

    Returns None
    -------

    """
    if not core_type:
        # default val is aic
        core_type = "0"
    value = _set_core_num_by_core_type(core_type)
    if not value:
        raise RuntimeError("set_core_num_by_core_type return error!")
