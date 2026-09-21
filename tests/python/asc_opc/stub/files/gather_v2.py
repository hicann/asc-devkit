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
"""
gather_v2
"""

import collections
from timeit import repeat
from tbe.common.context import get_context
from impl import constant_util as constant
from impl.util.util_common import is_vector_core
from impl.util.util_common import check_op_impl_mode
from impl.util.util_tik_comm_func import floor_align
from impl.util.platform_adapter import tvm
from impl.util.platform_adapter import tbe
from impl.util.platform_adapter import tik
from impl.util.platform_adapter import para_check
from impl.util.platform_adapter import shape_util
from impl.util.platform_adapter import classify
from impl.util.platform_adapter import OpPatternMode
from impl.util.platform_adapter import register_operator
from impl.util.platform_adapter import error_manager_vector
from impl.util.platform_adapter import OpImplMode
from impl.util.platform_adapter import PlatformApi
from impl.util.platform_adapter import tbe_context
from impl.util.platform_adapter import tbe_platform
from impl.util.platform_adapter import tbe_platform as tbe_platform_adapter


# 'pylint: disable=too-few-public-methods
class Constant:
    """
    The class for constant
    """

    TYPE_LEN_DICT = {
        "bfloat16": 2,
        "float16": 2,
        "float32": 4,
        "int8": 1,
        "uint8": 1,
        "int16": 2,
        "uint16": 2,
        "int32": 4,
        "uint32": 4,
        "int64": 8,
        "uint64": 8,
    }

    # reserved ub size
    RESERVED_UB_SIZE = 2 * 1024
    # 100K, caches input params data
    CACHE_UB_SIZE = 100 * 1024

    PARAMS_SIZE = 2**31 - 1
    INDICES_NUM = 2**31 - 1
    TILING_ARG_NUM = 32

    # Aligned types for params row size
    MODE_LESS_THAN_32B = 0
    MODE_ALIGNED = 1
    MODE_MORE_THAN_32B = 2
    MODE_LARGE_PARAMS_ROW = 3
    MODE_SMALL_PARAMS_ROW = 4

    # Cached types for params
    PARAMS_CACHED_UB = 0
    PARAMS_NOT_CACHED = 1

    # Cached types for indices
    INDICES_CACHED_ALL = 0
    INDICES_CACHED_ONE_ROW = 1
    INDICES_LARGE_ROW = 2
    INDICES_SMALL_ROW = 3

    # UB for loop which impl_mode is high performance
    IMPL_MODE_INNER_LOOP_UB_8K = 2**13

    # TOPN Cache data
    DTYPE_FP32 = "float32"
    DTYPE_INT32 = "int32"
    DTYPE_UINT32 = "uint32"
    DTYPE_INT64 = "int64"
    CACHE_ACT_SIMPLING_BUFF_NUM = 64
    TOPN_CACHE_ACT_SIMPLING_NUM = 32
    TOPN_CACHE_INVALID_PARAMS_INDEX = -1.0

    CMP_GREAT_EQUAL = 0
    CMP_LESS_THAN = 1

    # optimization method tpye
    INPUT_LAST_AXIS_ALIGN_HP_OPT_CACHE_N_NUM = 1
    INPUT_LAST_AXIS_ALIGN_HP_OPT_CACHE_SIMPLING = 2
    ONE_BLOCK_FP32_NUM = 8
    ONE_BLOCK_FP16_NUM = 16
    ONE_BLOCK_INT64_NUM = 4

    CUT_SUB_UB_SIZE = 6
    SIZE_ALIGN_64 = 64

    # define for transpose
    MAX_REPEAT_TIME = 255
    VECTOR_BLOCK_SIZE = 256
    FP16_ALIGN_NUM = 16
    DATA_ONE = 1
    OUT_ALIGN_NUM = 8

    # transpose mode
    LINE_TO_COL = 0
    COL_TO_LINE = 1

    # A. block tiling: indices tiling
    # paramsRowSize < 32
    # params is not cache in UB
    TILING_MODE_1 = 1
    # params is cache in UB
    TILING_MODE_4 = 4
    # params is cache in L1
    TILING_MODE_13 = 13

    # paramsRow is not 32B aligned
    TILING_MODE_2 = 2
    # one paramsRow of data can not store in half UB
    TILING_MODE_5 = 5

    # paramsRow is 32B aligned
    # params is not cache in UB or L1
    TILING_MODE_3 = 3
    # params is cache in UB
    TILING_MODE_6 = 6
    # params is not cache in UB or L1
    TILING_MODE_7 = 7

    # B. block tiling: params_pre tiling
    # paramsRowSize < 32
    # params is not cache in UB
    TILING_MODE_8 = 8
    # params is not cache in UB
    TILING_MODE_9 = 9

    # paramsRow is 32B aligned
    # params is not cache in UB or L1
    TILING_MODE_10 = 10
    # params is cache in UB
    TILING_MODE_11 = 11
    # params is not cache in UB or L1
    TILING_MODE_12 = 12
    # impl_mode is high_performance
    TILING_MODE_14 = 14
    # impl_mode is high_performance_for_cache
    TILING_MODE_15 = 15
    TILING_MODE_16 = 16
    TILING_MODE_17 = 17
    TILING_MODE_18 = 18

    # tiling_mode with batch_dims
    # 1.one params row size is smaller than 32B
    # 1.1 params is cached in UB
    TILING_MODE_20 = 20
    TILING_MODE_21 = 21
    TILING_MODE_22 = 22
    # 1.2 params is not cached in UB
    TILING_MODE_23 = 23
    TILING_MODE_24 = 24
    TILING_MODE_25 = 25

    # 2.one params row size is large than 32B and not align
    TILING_MODE_26 = 26
    TILING_MODE_27 = 27
    TILING_MODE_28 = 28

    # 3.one params row size is align
    # 3.1 params is cached in UB
    TILING_MODE_29 = 29
    TILING_MODE_30 = 30
    TILING_MODE_31 = 31
    # 3.2 params is not cached in UB
    TILING_MODE_32 = 32
    TILING_MODE_33 = 33
    TILING_MODE_34 = 34

    # 4. large params row size
    TILING_MODE_35 = 35
    TILING_MODE_36 = 36
    TILING_MODE_37 = 37

    # 5. small indices row size
    TILING_MODE_38 = 38
    TILING_MODE_39 = 39

    # 6. small params and indices row size
    TILING_MODE_40 = 40
    TILING_MODE_41 = 41


# 'pylint: disable=too-many-public-methods,invalid-name,too-many-arguments,too-many-locals,huawei-too-many-arguments
# 'pylint: disable=too-many-lines,too-many-instance-attributes,too-many-statements,unused-argument
def ceil_value(value, factor):
    """
    if not divide exactly then plus 1

    Parameters
    ----------
    value:  input number
    factor: factor

    Returns
    -------
    ceil value
    """
    return (value + factor - 1) // factor


def align_value(value, factor):
    """
    Alignment based on factor.

    Parameters
    ----------
    value: input number
    factor: alignment base

    Returns
    -------
    res:
    """
    return (value + factor - 1) // factor * factor


class GatherV2:
    """
    Function: use to store concat base parameters
    """

    # 'pylint: disable=too-many-arguments,huawei-too-many-arguments
    def __init__(
        self, params_dict, indices_dict, axis_dict, y_dict, batch_dims, is_preprocessed, kernel_name, impl_mode
    ):
        """
        constructor of GatherV2

        Parameters
        ----------
        params_dict: dict
            shape and dtype of input params
        indices_dict: dict
            shape and dtype of input indices
        axis_dict: dict
            shape and dtype of input axis
        y_dict: dict
            shape and dtype of output, should be same dtype as input
        batch_dims: int
            an optional int and defaults to 0
        is_preprocessed: bool
            an optional bool and defaults to false
        kernel_name: str
            kernel name, default value is "GatherV2"
        impl_mode: str. The flag for cache data at index 0

        Returns
        -------
        None
        """
        self.params_dtype = params_dict.get("dtype").lower()
        self.indices_dtype = indices_dict.get("dtype").lower()
        self.axis_dtype = axis_dict.get("dtype").lower()
        self.y_dtype = y_dict.get("dtype").lower()

        if self.params_dtype == "bool":
            self.params_dtype = "int8"

        if self.y_dtype == "bool":
            self.y_dtype = "int8"

        self.tiling_dtype = constant.DATA_TYPE_INT64
        dtype_list = (
            "int8",
            "int16",
            "int32",
            "int64",
            "uint8",
            "uint16",
            "uint32",
            "uint64",
            "float16",
            "float32",
            "bfloat16",
        )
        indices_support_dtype_list = ("int32", "int64")
        para_check.check_dtype(self.params_dtype, dtype_list, param_name="x")
        para_check.check_dtype(self.indices_dtype, indices_support_dtype_list, param_name="indices")
        para_check.check_dtype(self.axis_dtype, (constant.DATA_TYPE_INT32, constant.DATA_TYPE_INT64), param_name="axis")
        if self.y_dtype != self.params_dtype:
            error_manager_vector.raise_err_inputs_dtype_not_equal(
                kernel_name, "y", "x", self.y_dtype, self.params_dtype
            )

        self.ub_size = tbe_platform_adapter.get_soc_spec(tbe_platform_adapter.UB_SIZE)
        self.l1_size = tbe_platform_adapter.get_soc_spec(tbe_platform_adapter.L1_SIZE)
        self.core_num = tbe_platform_adapter.get_soc_spec(tbe_platform_adapter.CORE_NUM)
        self.is_vector_core = is_vector_core()
        self.tik_instance = tik.Tik()
        self.batch_dims = batch_dims
        self.is_preprocessed = is_preprocessed
        self.impl_mode = impl_mode
        self.kernel_name = kernel_name

        self.axis_shape = (1,)
        self.x_shape = (Constant.PARAMS_SIZE,)
        self.indices_shape = (Constant.INDICES_NUM,)
        self.y_shape = (Constant.PARAMS_SIZE,)

        self.params_dsize = Constant.TYPE_LEN_DICT.get(self.params_dtype)
        self.indices_dsize = Constant.TYPE_LEN_DICT.get(self.indices_dtype)
        self.block_elem = constant.BLOCK_SIZE // self.params_dsize

        self.x = None
        self.indices = None
        self.axis = None
        self.tiling_gm = None
        self.y = None

        self.params_pre = None
        self.params_axis = None
        self.params_row = None
        self.indices_num = None

        self.cache_params = None
        self.need_core_num = None
        self.tail_process_core = None
        self.indices_num_each_core = None
        self.indices_num_remaining = None
        self.indices_loop_num = None
        self.indices_row_num_once = None
        self.indices_row_num_last = None

        self.row_num_once_ub = None
        self.row_num_once_tail_ub = None
        self.inner_loop_num = None
        self.row_num_last_ub = None
        self.row_num_last_tail_ub = None
        self.inner_loop_num_last = None

        self.params_total = None
        self.one_row_loop = None
        self.one_row_tail = None
        self.params_pre_each_core = None
        self.params_pre_remaining = None

        self.params_batch = None
        self.indices_batch = None
        self.indices_row = None
        self.params_batch_each_core = None
        self.params_batch_remaining = None
        self.is_remaining = 0
        self.buffers = {"params_ub": None, "indices_ub": None, "res_ub": None}
        self.cached_types = {
            "cached_types_params": Constant.PARAMS_CACHED_UB,
            "cached_types_indices": Constant.INDICES_CACHED_ALL,
            "aligned_types_params": Constant.MODE_LESS_THAN_32B,
        }
        self.opt_config = {"out_of_bound_sync_check": True, "enable_const_fold": True}

        self.impl_mode_indices_ub_size = 2**15
        self.impl_mode_x0_ub_size = 2**16
        self.impl_mode_res_ub_size = 2**17
        if PlatformApi.get_soc_spec(PlatformApi.SHORT_SOC_VERSION) == "Ascend910B":
            self.impl_mode_x0_ub_size = 2**15
            self.impl_mode_res_ub_size = 2**16

    def get_tiling_args(self, tiling_ub):
        """
        get runtime params from tiling

        Parameters
        ----------
        tiling_ub: tensor, runtime params from gather_nd tiling

        Returns
        -------
        None
        """
        self.params_pre = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="params_pre")
        self.params_pre.set_as(tiling_ub[1])
        self.params_axis = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="params_axis")
        self.params_axis.set_as(tiling_ub[2])
        self.params_row = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="params_row")
        self.params_row.set_as(tiling_ub[3])
        self.indices_num = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="indices_num")
        self.indices_num.set_as(tiling_ub[4])

        self.cache_params = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="cache_params")
        self.cache_params.set_as(tiling_ub[5])
        self.need_core_num = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="need_core_num")
        self.need_core_num.set_as(tiling_ub[6])
        self.tail_process_core = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="tail_process_core")
        self.tail_process_core.set_as(tiling_ub[7])
        self.indices_num_each_core = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="indices_num_each_core")
        self.indices_num_each_core.set_as(tiling_ub[8])
        self.indices_num_remaining = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="indices_num_remaining")
        self.indices_num_remaining.set_as(tiling_ub[9])
        self.indices_loop_num = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="indices_loop_num")
        self.indices_loop_num.set_as(tiling_ub[10])
        self.indices_row_num_once = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="indices_row_num_once")
        self.indices_row_num_once.set_as(tiling_ub[11])
        self.indices_row_num_last = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="indices_row_num_last")
        self.indices_row_num_last.set_as(tiling_ub[12])

        self.row_num_once_ub = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="row_num_once_ub")
        self.row_num_once_ub.set_as(tiling_ub[13])
        self.row_num_once_tail_ub = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="row_num_once_tail_ub")
        self.row_num_once_tail_ub.set_as(tiling_ub[14])
        self.inner_loop_num = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="inner_loop_num")
        self.inner_loop_num.set_as(tiling_ub[15])
        self.row_num_last_ub = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="row_num_last_ub")
        self.row_num_last_ub.set_as(tiling_ub[16])
        self.row_num_last_tail_ub = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="row_num_last_tail_ub")
        self.row_num_last_tail_ub.set_as(tiling_ub[17])
        self.inner_loop_num_last = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="inner_loop_num_last")
        self.inner_loop_num_last.set_as(tiling_ub[18])

        self.params_total = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="params_total")
        self.params_total.set_as(tiling_ub[19])
        self.one_row_loop = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="one_row_loop")
        self.one_row_loop.set_as(tiling_ub[20])
        self.one_row_tail = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="one_row_tail")
        self.one_row_tail.set_as(tiling_ub[21])
        self.params_pre_each_core = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="params_pre_each_core")
        self.params_pre_each_core.set_as(tiling_ub[22])
        self.params_pre_remaining = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="params_pre_remaining")
        self.params_pre_remaining.set_as(tiling_ub[23])

        self.indices_row = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="indices_row")
        self.indices_row.set_as(tiling_ub[24])
        self.params_batch_each_core = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="params_batch_each_core")
        self.params_batch_each_core.set_as(tiling_ub[25])
        self.params_batch_remaining = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="params_batch_remaining")
        self.params_batch_remaining.set_as(tiling_ub[26])
        self.params_batch = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="params_batch")
        self.params_batch.set_as(tiling_ub[27])

    def gather_v2_compute_tiling(self):
        """
        Main process of gather_v2

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        half_ub_size = (self.ub_size - 2 * 1024) // 2
        quarter_ub_size = (self.ub_size - 2 * 1024) // 4
        remain_half_ub_size = (self.ub_size - Constant.CACHE_UB_SIZE - Constant.RESERVED_UB_SIZE) // 2
        tik_instance = self.tik_instance

        # get tiling data
        tiling_ub = tik_instance.Tensor(
            self.tiling_dtype, (Constant.TILING_ARG_NUM,), name="tiling_ub", scope=tik.scope_ubuf
        )
        tik_instance.data_move(
            tiling_ub,
            self.tiling_gm,
            0,
            1,
            ceil_value(Constant.TILING_ARG_NUM * Constant.TYPE_LEN_DICT.get(self.tiling_dtype), constant.BLOCK_SIZE),
            0,
            0,
        )

        tiling_mode = self.tik_instance.Scalar(dtype=self.tiling_dtype, name="tiling_mode")
        tiling_mode.set_as(tiling_ub[0])

        # get run info
        self.get_tiling_args(tiling_ub)

        with tik_instance.for_range(0, self.core_num, block_num=self.core_num) as block_id:
            with tik_instance.if_scope(block_id < self.need_core_num):
                with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_1):
                    with tik_instance.new_stmt_scope():
                        self.compute_mode_1(half_ub_size, quarter_ub_size, block_id)
                with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_2):
                    with tik_instance.new_stmt_scope():
                        self.compute_mode_2(half_ub_size, block_id)
                with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_3):
                    with tik_instance.new_stmt_scope():
                        self.compute_mode_3(half_ub_size, block_id)
                with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_4):
                    with tik_instance.new_stmt_scope():
                        self.compute_mode_4(remain_half_ub_size, block_id)
                with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_5):
                    with tik_instance.new_stmt_scope():
                        self.compute_mode_5(half_ub_size, block_id)
                with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_6):
                    with tik_instance.new_stmt_scope():
                        self.compute_mode_6(remain_half_ub_size, block_id)
                with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_7):
                    with tik_instance.new_stmt_scope():
                        self.compute_mode_7(half_ub_size, block_id)
                with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_8):
                    with tik_instance.new_stmt_scope():
                        self.compute_mode_8(remain_half_ub_size, block_id)
                with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_9):
                    with tik_instance.new_stmt_scope():
                        self.compute_mode_9(half_ub_size, quarter_ub_size, block_id)
                with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_10):
                    with tik_instance.new_stmt_scope():
                        self.compute_mode_10(remain_half_ub_size, block_id)
                with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_11):
                    with tik_instance.new_stmt_scope():
                        self.compute_mode_11(half_ub_size, block_id)
                with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_12):
                    with tik_instance.new_stmt_scope():
                        self.compute_mode_12(half_ub_size, block_id)
                with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_13):
                    with tik_instance.new_stmt_scope():
                        self.compute_mode_13(half_ub_size, block_id)
                with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_14):
                    with tik_instance.new_stmt_scope():
                        self.compute_mode_14(block_id)
                with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_15):
                    with tik_instance.new_stmt_scope():
                        self.compute_mode_15(block_id)
                with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_16):
                    with tik_instance.new_stmt_scope():
                        self.compute_mode_16(block_id)
                with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_17):
                    with tik_instance.new_stmt_scope():
                        self.compute_mode_17(block_id)
                with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_18):
                    with tik_instance.new_stmt_scope():
                        self.compute_mode_18(block_id)

                if self.batch_dims != 0:
                    # Tiling mode with batch_dims
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_20):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_LESS_THAN_32B, Constant.PARAMS_CACHED_UB, Constant.INDICES_CACHED_ALL
                            )
                            self.compute_with_batch_dims(remain_half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_21):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_LESS_THAN_32B, Constant.PARAMS_CACHED_UB, Constant.INDICES_CACHED_ONE_ROW
                            )
                            self.compute_with_batch_dims(remain_half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_22):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_LESS_THAN_32B, Constant.PARAMS_CACHED_UB, Constant.INDICES_LARGE_ROW
                            )
                            self.compute_with_batch_dims(remain_half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_23):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_LESS_THAN_32B, Constant.PARAMS_NOT_CACHED, Constant.INDICES_CACHED_ALL
                            )
                            self.compute_with_batch_dims(half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_24):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_LESS_THAN_32B, Constant.PARAMS_NOT_CACHED, Constant.INDICES_CACHED_ONE_ROW
                            )
                            self.compute_with_batch_dims(half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_25):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_LESS_THAN_32B, Constant.PARAMS_NOT_CACHED, Constant.INDICES_LARGE_ROW
                            )
                            self.compute_with_batch_dims(half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_26):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_MORE_THAN_32B, Constant.PARAMS_NOT_CACHED, Constant.INDICES_CACHED_ALL
                            )
                            self.compute_with_batch_dims(half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_27):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_MORE_THAN_32B, Constant.PARAMS_NOT_CACHED, Constant.INDICES_CACHED_ONE_ROW
                            )
                            self.compute_with_batch_dims(half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_28):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_MORE_THAN_32B, Constant.PARAMS_NOT_CACHED, Constant.INDICES_LARGE_ROW
                            )
                            self.compute_with_batch_dims(half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_29):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_ALIGNED, Constant.PARAMS_CACHED_UB, Constant.INDICES_CACHED_ALL
                            )
                            self.compute_with_batch_dims(remain_half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_30):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_ALIGNED, Constant.PARAMS_CACHED_UB, Constant.INDICES_CACHED_ONE_ROW
                            )
                            self.compute_with_batch_dims(remain_half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_31):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_ALIGNED, Constant.PARAMS_CACHED_UB, Constant.INDICES_LARGE_ROW
                            )
                            self.compute_with_batch_dims(remain_half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_32):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_ALIGNED, Constant.PARAMS_NOT_CACHED, Constant.INDICES_CACHED_ALL
                            )
                            self.compute_with_batch_dims(half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_33):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_ALIGNED, Constant.PARAMS_NOT_CACHED, Constant.INDICES_CACHED_ONE_ROW
                            )
                            self.compute_with_batch_dims(half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_34):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_ALIGNED, Constant.PARAMS_NOT_CACHED, Constant.INDICES_LARGE_ROW
                            )
                            self.compute_with_batch_dims(half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_35):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_LARGE_PARAMS_ROW, Constant.PARAMS_NOT_CACHED, Constant.INDICES_CACHED_ALL
                            )
                            self.compute_with_batch_dims(half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_36):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_LARGE_PARAMS_ROW,
                                Constant.PARAMS_NOT_CACHED,
                                Constant.INDICES_CACHED_ONE_ROW,
                            )
                            self.compute_with_batch_dims(half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_37):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_LARGE_PARAMS_ROW, Constant.PARAMS_NOT_CACHED, Constant.INDICES_LARGE_ROW
                            )
                            self.compute_with_batch_dims(half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_38):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_LESS_THAN_32B, Constant.PARAMS_CACHED_UB, Constant.INDICES_SMALL_ROW
                            )
                            self.compute_with_batch_dims(remain_half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_39):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_LESS_THAN_32B, Constant.PARAMS_NOT_CACHED, Constant.INDICES_SMALL_ROW
                            )
                            self.compute_with_batch_dims(half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_40):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_SMALL_PARAMS_ROW,
                                Constant.PARAMS_CACHED_UB,
                                Constant.INDICES_CACHED_ONE_ROW,
                            )
                            self.compute_with_batch_dims(remain_half_ub_size, block_id)
                    with tik_instance.if_scope(tiling_mode == Constant.TILING_MODE_41):
                        with tik_instance.new_stmt_scope():
                            self._set_mode_paras(
                                Constant.MODE_SMALL_PARAMS_ROW,
                                Constant.PARAMS_NOT_CACHED,
                                Constant.INDICES_CACHED_ONE_ROW,
                            )
                            self.compute_with_batch_dims(half_ub_size, block_id)

    def compute_mode_1(self, half_ub_size, quarter_ub_size, block_id):
        """
        compute for tiling mode 1

        Parameters
        ----------
        half_ub_size: bytes of half UB
        block_id: id of ai core

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        indices_ub = tik_instance.Tensor(
            self.indices_dtype, ((half_ub_size + 256) // self.indices_dsize,), name="indices_ub", scope=tik.scope_ubuf
        )
        res_ub = tik_instance.Tensor(
            self.params_dtype,
            ((quarter_ub_size + constant.BLOCK_SIZE) // self.params_dsize,),
            name="res_ub",
            scope=tik.scope_ubuf,
        )
        block_ub = tik_instance.Tensor(
            self.params_dtype,
            ((quarter_ub_size + constant.BLOCK_SIZE) // self.params_dsize,),
            name="block_ub",
            scope=tik.scope_ubuf,
        )

        with tik_instance.for_range(0, self.params_pre) as pre_i:
            # 1. indices_num_each_core: indices_row_num_once * indices_loop_num + indices_row_num_last
            # a. process indices_row_num_once * indices_loop_num
            with tik_instance.for_range(0, self.indices_loop_num) as indices_loop_i:
                indices_num_offset = block_id * self.indices_num_each_core + indices_loop_i * self.indices_row_num_once
                # move indices data from gm to ub
                tik_instance.data_move(
                    indices_ub,
                    self.indices[indices_num_offset],
                    0,
                    1,
                    ceil_value(self.indices_row_num_once * self.indices_dsize, constant.BLOCK_SIZE),
                    0,
                    0,
                )

                with tik_instance.if_scope(self.inner_loop_num > 0):
                    with tik_instance.for_range(0, self.inner_loop_num) as inner_loop_i:
                        inner_indices_offset = inner_loop_i * self.row_num_once_ub
                        ub_tensor_list = [self.row_num_once_ub, indices_ub, res_ub, block_ub]
                        offset_list = [indices_num_offset, inner_indices_offset, pre_i]
                        self.process_mode_1(ub_tensor_list, offset_list, quarter_ub_size, self.x)

                # a2. process row_num_once_tail_ub
                with tik_instance.if_scope(self.row_num_once_tail_ub > 0):
                    inner_indices_offset = self.inner_loop_num * self.row_num_once_ub
                    ub_tensor_list = [self.row_num_once_tail_ub, indices_ub, res_ub, block_ub]
                    offset_list = [indices_num_offset, inner_indices_offset, pre_i]
                    self.process_mode_1(ub_tensor_list, offset_list, quarter_ub_size, self.x)

            with tik_instance.if_scope(self.indices_row_num_last > 0):
                indices_num_offset = (
                    block_id * self.indices_num_each_core + self.indices_loop_num * self.indices_row_num_once
                )
                # copy indices data from gm to ub
                tik_instance.data_move(
                    indices_ub,
                    self.indices[indices_num_offset],
                    0,
                    1,
                    ceil_value(self.indices_row_num_last * self.indices_dsize, constant.BLOCK_SIZE),
                    0,
                    0,
                )

                with tik_instance.if_scope(self.inner_loop_num_last > 0):
                    with tik_instance.for_range(0, self.inner_loop_num_last) as inner_loop_i:
                        inner_indices_offset = inner_loop_i * self.row_num_once_ub
                        ub_tensor_list = [self.row_num_once_ub, indices_ub, res_ub, block_ub]
                        offset_list = [indices_num_offset, inner_indices_offset, pre_i]
                        self.process_mode_1(ub_tensor_list, offset_list, quarter_ub_size, self.x)

                with tik_instance.if_scope(self.row_num_last_tail_ub > 0):
                    inner_indices_offset = self.inner_loop_num_last * self.row_num_last_ub
                    ub_tensor_list = [self.row_num_last_tail_ub, indices_ub, res_ub, block_ub]
                    offset_list = [indices_num_offset, inner_indices_offset, pre_i]
                    self.process_mode_1(ub_tensor_list, offset_list, quarter_ub_size, self.x)

            with tik_instance.if_scope(tik.all(self.indices_num_remaining > 0, block_id == self.tail_process_core)):
                self.process_remaining_tail_mode_1(pre_i, indices_ub, res_ub, self.x)

    def process_mode_1(self, ub_tensor_list, offset_list, quarter_ub_size, x_src):
        """
        process row_num_last indices for tiling mode 1

        Parameters
        ----------
        ub_tensor_list: list [row_num_last, indices_ub, res_ub, block_ub]
        offset_list: list [row_num_last, indices_num_offset, inner_indices_offset]
        row_num_last: the indices num
        indices_num_offset: indices num offset
        inner_indices_offset: inner indices num offset
        quarter_ub_size: a quarter of ub size
        x_src: source params

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        row_num_last, indices_ub, res_ub, block_ub = ub_tensor_list
        indices_num_offset, inner_indices_offset, pre_i = offset_list
        # count num of every loop from result_ub to gm
        res_ub_to_gm_per_loop_num = quarter_ub_size / self.params_dsize
        # loop num from result_ub to gm
        res_ub_to_gm_loop_num = (
            row_num_last * self.params_row + res_ub_to_gm_per_loop_num - 1
        ) // res_ub_to_gm_per_loop_num
        # elements num in block_ub
        block_per_loop_num = quarter_ub_size / constant.BLOCK_SIZE

        res_to_gm_loop_num = tik_instance.Scalar(dtype=self.tiling_dtype, name="res_to_gm_loop_num")
        row_num_last_remaining = tik_instance.Scalar(dtype=self.tiling_dtype, name="row_num_last_remaining")
        res_to_gm_loop_num.set_as(res_ub_to_gm_loop_num)
        row_num_last_remaining.set_as(row_num_last)

        with tik_instance.for_range(0, res_to_gm_loop_num) as row_g:
            with tik_instance.if_scope(res_to_gm_loop_num > 1):
                with tik_instance.if_scope(row_g != res_to_gm_loop_num - 1):
                    row_num_last_remaining.set_as(row_num_last // res_to_gm_loop_num)
                with tik_instance.else_scope():
                    row_num_last_remaining.set_as(row_num_last - (row_num_last // res_to_gm_loop_num) * row_g)

            # loop num from x_src to block_ub
            x_to_block_loop_num = row_num_last_remaining // block_per_loop_num
            # compute output offset of every loop
            burst_len_res = ceil_value(
                row_num_last_remaining * self.params_row * self.params_dsize, constant.BLOCK_SIZE
            )
            output_offset = (
                pre_i * self.indices_num + indices_num_offset + inner_indices_offset
            ) * self.params_row + row_g * (row_num_last // res_to_gm_loop_num) * self.params_row

            with tik_instance.for_range(0, x_to_block_loop_num) as row_h:
                with tik_instance.for_range(0, block_per_loop_num) as row_i:
                    # compute gm offset of x
                    indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
                    indices_value.set_as(
                        indices_ub[
                            inner_indices_offset
                            + row_g * (row_num_last // res_to_gm_loop_num)
                            + (row_h * block_per_loop_num + row_i)
                        ]
                    )
                    gm_offset = (pre_i * self.params_axis + indices_value) * self.params_row

                    # copy params row from gm to block_ub
                    tik_instance.data_move(block_ub[self.block_elem * row_i], x_src[gm_offset], 0, 1, 1, 0, 0)

                with tik_instance.for_range(0, block_per_loop_num) as row_i:
                    # set result to res_ub
                    res_ub_offset = (row_h * block_per_loop_num + row_i) * self.params_row
                    block_ub_offset = row_i * self.block_elem
                    with tik_instance.for_range(0, self.params_row) as i:
                        res_ub[res_ub_offset + i].set_as(block_ub[block_ub_offset + i])

            with tik_instance.new_stmt_scope(disable_sync=True):
                tail_indices = x_to_block_loop_num * block_per_loop_num

                with tik_instance.for_range(tail_indices, row_num_last_remaining) as row_i:
                    # compute gm offset of x
                    indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
                    indices_value.set_as(
                        indices_ub[inner_indices_offset + row_g * (row_num_last // res_to_gm_loop_num) + row_i]
                    )
                    gm_offset = (pre_i * self.params_axis + indices_value) * self.params_row

                    # copy params row from gm to block_ub
                    tik_instance.data_move(
                        block_ub[self.block_elem * (row_i - tail_indices)], x_src[gm_offset], 0, 1, 1, 0, 0
                    )

            with tik_instance.for_range(tail_indices, row_num_last_remaining) as row_i:
                # set result to res_ub
                res_ub_offset = row_i * self.params_row
                block_ub_offset = (row_i - tail_indices) * self.block_elem
                with tik_instance.for_range(0, self.params_row) as i:
                    res_ub[res_ub_offset + i].set_as(block_ub[block_ub_offset + i])

            # move result data from ub to gm
            tail_elem = (row_num_last_remaining * self.params_row) % self.block_elem
            with tik_instance.if_scope(tik.all(tail_elem != 0, burst_len_res > 1)):
                block_ub = tik_instance.Tensor(
                    self.params_dtype, (self.block_elem,), name="block_ub", scope=tik.scope_ubuf
                )
                with tik_instance.for_range(0, self.block_elem) as num_i:
                    block_ub[num_i].set_as(res_ub[row_num_last_remaining * self.params_row - self.block_elem + num_i])

                tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res - 1, 0, 0)
                tik_instance.data_move(
                    self.y[output_offset + (row_num_last_remaining * self.params_row - self.block_elem)],
                    block_ub,
                    0,
                    1,
                    1,
                    0,
                    0,
                )
            with tik_instance.else_scope():
                tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res, 0, 0)

    def process_remaining_tail_mode_1(self, pre_i, indices_ub, res_ub, x_src):
        """
        process remaining tail indices in core 0 for tiling mode 1

        Parameters
        ----------
        pre_i: params_pre
        indices_ub: cache indices data in UB
        res_ub: cache result data in UB
        x_src: source params that cache in UB

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        indices_num_offset = self.need_core_num * self.indices_num_each_core
        # move indices data to ub from gm
        tik_instance.data_move(
            indices_ub,
            self.indices[indices_num_offset],
            0,
            1,
            ceil_value(self.indices_num_remaining * self.indices_dsize, constant.BLOCK_SIZE),
            0,
            0,
        )

        burst_len_res = ceil_value(
            self.indices_num_remaining * self.params_row * self.params_dsize, constant.BLOCK_SIZE
        )
        output_offset = (pre_i * self.indices_num + indices_num_offset) * self.params_row

        with tik_instance.for_range(0, self.indices_num_remaining, thread_num=1) as row_i:
            indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
            indices_value.set_as(indices_ub[row_i])
            gm_offset = (pre_i * self.params_axis + indices_value) * self.params_row

            block_ub = tik_instance.Tensor(self.params_dtype, (self.block_elem,), name="block_ub", scope=tik.scope_ubuf)
            # copy params row from gm to block_ub
            tik_instance.data_move(block_ub, x_src[gm_offset], 0, 1, 1, 0, 0)

            # set result to res_ub
            res_ub_offset = row_i * self.params_row
            with tik_instance.for_range(0, self.params_row) as i:
                res_ub[res_ub_offset + i].set_as(block_ub[i])

        # copy result data from ub to gm
        tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res, 0, 0)

    def compute_mode_13(self, half_ub_size, block_id):
        """
        compute for tiling mode 13

        Parameters
        ----------
        half_ub_size: bytes of half UB
        block_id: id of ai core

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        indices_ub = tik_instance.Tensor(
            self.indices_dtype, ((half_ub_size + 256) // self.indices_dsize,), name="indices_ub", scope=tik.scope_ubuf
        )
        res_ub = tik_instance.Tensor(
            self.params_dtype,
            ((half_ub_size + constant.BLOCK_SIZE) // self.params_dsize,),
            name="res_ub",
            scope=tik.scope_ubuf,
        )

        if tbe_platform_adapter.api_check_support("tik.vcopy") or self.is_vector_core:
            x_cbuf = self.x
        else:
            x_cbuf = tik_instance.Tensor(
                self.params_dtype, (self.l1_size // self.params_dsize,), name="x_l1", scope=tik.scope_cbuf
            )
            # cache params data from gm in L1
            tik_instance.data_move(x_cbuf, self.x, 0, 1, ceil_value(self.params_total, self.block_elem), 0, 0)

        with tik_instance.for_range(0, self.params_pre) as pre_i:
            # 1. indices_num_each_core: indices_row_num_once * indices_loop_num + indices_row_num_last
            # a. process indices_row_num_once * indices_loop_num
            with tik_instance.for_range(0, self.indices_loop_num) as indices_loop_i:
                indices_num_offset = block_id * self.indices_num_each_core + indices_loop_i * self.indices_row_num_once
                # move indices data from gm to ub
                tik_instance.data_move(
                    indices_ub,
                    self.indices[indices_num_offset],
                    0,
                    1,
                    ceil_value(self.indices_row_num_once * self.indices_dsize, constant.BLOCK_SIZE),
                    0,
                    0,
                )

                with tik_instance.if_scope(self.inner_loop_num > 0):
                    self.process_loop_mode_13(
                        self.inner_loop_num, indices_num_offset, pre_i, indices_ub, res_ub, x_cbuf
                    )

                # a2. process row_num_once_tail_ub
                with tik_instance.if_scope(self.row_num_once_tail_ub > 0):
                    inner_indices_offset = self.inner_loop_num * self.row_num_once_ub
                    ub_tensor_list = [self.row_num_once_tail_ub, indices_ub, res_ub]
                    offset_list = [indices_num_offset, inner_indices_offset, pre_i]
                    self.process_last_mode_13(ub_tensor_list, offset_list, x_cbuf)

            with tik_instance.if_scope(self.indices_row_num_last > 0):
                indices_num_offset = (
                    block_id * self.indices_num_each_core + self.indices_loop_num * self.indices_row_num_once
                )
                # copy indices data from gm to ub
                tik_instance.data_move(
                    indices_ub,
                    self.indices[indices_num_offset],
                    0,
                    1,
                    ceil_value(self.indices_row_num_last * self.indices_dsize, constant.BLOCK_SIZE),
                    0,
                    0,
                )

                with tik_instance.if_scope(self.inner_loop_num_last > 0):
                    self.process_loop_mode_13(
                        self.inner_loop_num_last, indices_num_offset, pre_i, indices_ub, res_ub, x_cbuf
                    )

                with tik_instance.if_scope(self.row_num_last_tail_ub > 0):
                    inner_indices_offset = self.inner_loop_num_last * self.row_num_last_ub
                    ub_tensor_list = [self.row_num_last_tail_ub, indices_ub, res_ub]
                    offset_list = [indices_num_offset, inner_indices_offset, pre_i]
                    self.process_last_mode_13(ub_tensor_list, offset_list, x_cbuf)

            with tik_instance.if_scope(tik.all(self.indices_num_remaining > 0, block_id == self.tail_process_core)):
                self.process_remaining_tail_mode_13(pre_i, indices_ub, res_ub, x_cbuf)

    # 'pylint: disable=too-many-arguments,huawei-too-many-arguments
    def process_loop_mode_13(self, loop_num, indices_num_offset, pre_i, indices_ub, res_ub, x_src):
        """
        previous loop_num times process for tiling mode 13

        Parameters
        ----------
        loop_num: loop times
        indices_num_offset: indices num offset
        pre_i: params_pre
        indices_ub: cache indices data in UB
        res_ub: cache result data in UB
        x_src: source params

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        burst_len_res = ceil_value(self.row_num_once_ub * self.params_row * self.params_dsize, constant.BLOCK_SIZE)

        with tik_instance.for_range(0, loop_num) as inner_loop_i:
            inner_indices_offset = inner_loop_i * self.row_num_once_ub
            output_offset = (pre_i * self.indices_num + indices_num_offset + inner_indices_offset) * self.params_row

            with tik_instance.for_range(0, self.row_num_once_ub, thread_num=2) as row_i:
                # compute gm offset of x
                indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
                indices_value.set_as(indices_ub[inner_indices_offset + row_i])
                gm_offset = (pre_i * self.params_axis + indices_value) * self.params_row
                l1_offset = gm_offset // self.block_elem * self.block_elem
                ub_offset = gm_offset % self.block_elem

                block_ub = tik_instance.Tensor(
                    self.params_dtype, (self.block_elem * 2,), name="block_ub", scope=tik.scope_ubuf
                )
                # copy params row from gm to block_ub
                tik_instance.data_move(block_ub, x_src[l1_offset], 0, 1, 2, 0, 0)

                # set result to res_ub
                res_ub_offset = row_i * self.params_row
                with tik_instance.for_range(0, self.params_row) as i:
                    res_ub[res_ub_offset + i].set_as(block_ub[ub_offset + i])

            # copy result data from ub to gm
            tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res, 0, 0)

    def process_last_mode_13(self, ub_tensor_list, offset_list, x_src):
        """
        process row_num_last indices for tiling mode 13

        Parameters
        ----------
        ub_tensor_list: [row_num_last, indices_ub, res_ub]
        offset_list: [indices_num_offset, inner_indices_offset, pre_i]
        x_src: source params

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        row_num_last, indices_ub, res_ub = ub_tensor_list
        indices_num_offset, inner_indices_offset, pre_i = offset_list
        burst_len_res = ceil_value(row_num_last * self.params_row * self.params_dsize, constant.BLOCK_SIZE)
        output_offset = (pre_i * self.indices_num + indices_num_offset + inner_indices_offset) * self.params_row

        with tik_instance.for_range(0, row_num_last, thread_num=2) as row_i:
            # compute gm offset of x
            indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
            indices_value.set_as(indices_ub[inner_indices_offset + row_i])
            gm_offset = (pre_i * self.params_axis + indices_value) * self.params_row
            l1_offset = gm_offset // self.block_elem * self.block_elem
            ub_offset = gm_offset % self.block_elem

            block_ub = tik_instance.Tensor(
                self.params_dtype, (self.block_elem * 2,), name="block_ub", scope=tik.scope_ubuf
            )
            # copy params row from gm to block_ub
            tik_instance.data_move(block_ub, x_src[l1_offset], 0, 1, 2, 0, 0)

            # set result to res_ub
            res_ub_offset = row_i * self.params_row
            with tik_instance.for_range(0, self.params_row) as i:
                res_ub[res_ub_offset + i].set_as(block_ub[ub_offset + i])

        # move result data from ub to gm
        tail_elem = (row_num_last * self.params_row) % self.block_elem
        with tik_instance.if_scope(tik.all(tail_elem != 0, burst_len_res > 1)):
            block_ub = tik_instance.Tensor(self.params_dtype, (self.block_elem,), name="block_ub", scope=tik.scope_ubuf)
            with tik_instance.for_range(0, self.block_elem) as num_i:
                block_ub[num_i].set_as(res_ub[row_num_last * self.params_row - self.block_elem + num_i])

            tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res - 1, 0, 0)
            tik_instance.data_move(
                self.y[output_offset + (row_num_last * self.params_row - self.block_elem)], block_ub, 0, 1, 1, 0, 0
            )
        with tik_instance.else_scope():
            tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res, 0, 0)

    def process_remaining_tail_mode_13(self, pre_i, indices_ub, res_ub, x_src):
        """
        process remaining tail indices in core 0 for tiling mode 13

        Parameters
        ----------
        pre_i: params_pre
        indices_ub: cache indices data in UB
        res_ub: cache result data in UB
        x_src: source params that cache in UB

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        indices_num_offset = self.need_core_num * self.indices_num_each_core
        # move indices data to ub from gm
        tik_instance.data_move(
            indices_ub,
            self.indices[indices_num_offset],
            0,
            1,
            ceil_value(self.indices_num_remaining * self.indices_dsize, constant.BLOCK_SIZE),
            0,
            0,
        )

        burst_len_res = ceil_value(
            self.indices_num_remaining * self.params_row * self.params_dsize, constant.BLOCK_SIZE
        )
        output_offset = (pre_i * self.indices_num + indices_num_offset) * self.params_row

        with tik_instance.for_range(0, self.indices_num_remaining, thread_num=1) as row_i:
            indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
            indices_value.set_as(indices_ub[row_i])
            gm_offset = (pre_i * self.params_axis + indices_value) * self.params_row
            l1_offset = gm_offset // self.block_elem * self.block_elem
            ub_offset = gm_offset % self.block_elem

            block_ub = tik_instance.Tensor(
                self.params_dtype, (self.block_elem * 2,), name="block_ub", scope=tik.scope_ubuf
            )
            # copy params row from gm to block_ub
            tik_instance.data_move(block_ub, x_src[l1_offset], 0, 1, 2, 0, 0)

            # set result to res_ub
            res_ub_offset = row_i * self.params_row
            with tik_instance.for_range(0, self.params_row) as i:
                res_ub[res_ub_offset + i].set_as(block_ub[ub_offset + i])

        # copy result data from ub to gm
        tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res, 0, 0)

    def compute_mode_4(self, half_ub_size, block_id):
        """
        compute for tiling mode 4, params row < 32b, and params data cached in UB

        Parameters
        ----------
        half_ub_size: bytes of half UB
        block_id: id of ai core

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        indices_ub = tik_instance.Tensor(
            self.indices_dtype, ((half_ub_size + 256) // self.indices_dsize,), name="indices_ub", scope=tik.scope_ubuf
        )
        res_ub = tik_instance.Tensor(
            self.params_dtype,
            ((half_ub_size + constant.BLOCK_SIZE) // self.params_dsize,),
            name="res_ub",
            scope=tik.scope_ubuf,
        )
        x_ub = tik_instance.Tensor(
            self.params_dtype, (Constant.CACHE_UB_SIZE // self.params_dsize,), name="x_ub", scope=tik.scope_ubuf
        )

        # cache params data in UB from gm
        tik_instance.data_move(x_ub, self.x, 0, 1, ceil_value(self.params_total, self.block_elem), 0, 0)

        with tik_instance.for_range(0, self.params_pre) as pre_i:
            # 1. indices_num_each_core: indices_row_num_once * indices_loop_num + indices_row_num_last
            # a. process indices_row_num_once * indices_loop_num
            with tik_instance.for_range(0, self.indices_loop_num) as indices_loop_i:
                indices_num_offset = block_id * self.indices_num_each_core + indices_loop_i * self.indices_row_num_once
                # move indices data from gm to ub
                tik_instance.data_move(
                    indices_ub,
                    self.indices[indices_num_offset],
                    0,
                    1,
                    ceil_value(self.indices_row_num_once * self.indices_dsize, constant.BLOCK_SIZE),
                    0,
                    0,
                )

                with tik_instance.if_scope(self.inner_loop_num > 0):
                    self.process_loop_mode_4(self.inner_loop_num, indices_num_offset, pre_i, indices_ub, res_ub, x_ub)

                # a2. process row_num_once_tail_ub
                with tik_instance.if_scope(self.row_num_once_tail_ub > 0):
                    inner_indices_offset = self.inner_loop_num * self.row_num_once_ub
                    ub_tensor_list = [self.row_num_once_tail_ub, indices_ub, res_ub, x_ub]
                    offset_list = [indices_num_offset, inner_indices_offset, pre_i]
                    self.process_last_mode_4(ub_tensor_list, offset_list)

            with tik_instance.if_scope(self.indices_row_num_last > 0):
                indices_num_offset = (
                    block_id * self.indices_num_each_core + self.indices_loop_num * self.indices_row_num_once
                )
                # copy indices data from gm to ub
                tik_instance.data_move(
                    indices_ub,
                    self.indices[indices_num_offset],
                    0,
                    1,
                    ceil_value(self.indices_row_num_last * self.indices_dsize, constant.BLOCK_SIZE),
                    0,
                    0,
                )

                with tik_instance.if_scope(self.inner_loop_num_last > 0):
                    self.process_loop_mode_4(
                        self.inner_loop_num_last, indices_num_offset, pre_i, indices_ub, res_ub, x_ub
                    )

                with tik_instance.if_scope(self.row_num_last_tail_ub > 0):
                    inner_indices_offset = self.inner_loop_num_last * self.row_num_last_ub
                    ub_tensor_list = [self.row_num_last_tail_ub, indices_ub, res_ub, x_ub]
                    offset_list = [indices_num_offset, inner_indices_offset, pre_i]
                    self.process_last_mode_4(ub_tensor_list, offset_list)

            with tik_instance.if_scope(tik.all(self.indices_num_remaining > 0, block_id == self.tail_process_core)):
                self.process_remaining_tail_mode_4(pre_i, indices_ub, res_ub, x_ub)

    # 'pylint: disable=too-many-arguments,huawei-too-many-arguments
    def process_loop_mode_4(self, loop_num, indices_num_offset, pre_i, indices_ub, res_ub, x_ub):
        """
        previous loop_num times process for tiling mode 4

        Parameters
        ----------
        loop_num: loop times
        indices_num_offset: indices num offset
        pre_i: params_pre
        indices_ub: cache indices data in UB
        res_ub: cache result data in UB
        x_ub: source params that cache in UB

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        burst_len_res = ceil_value(self.row_num_once_ub * self.params_row * self.params_dsize, constant.BLOCK_SIZE)

        with tik_instance.for_range(0, loop_num) as inner_loop_i:
            inner_indices_offset = inner_loop_i * self.row_num_once_ub
            output_offset = (pre_i * self.indices_num + indices_num_offset + inner_indices_offset) * self.params_row

            with tik_instance.for_range(0, self.row_num_once_ub, thread_num=2) as row_i:
                # compute gm offset of x
                indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
                indices_value.set_as(indices_ub[inner_indices_offset + row_i])
                gm_offset = (pre_i * self.params_axis + indices_value) * self.params_row

                # set result to res_ub
                res_ub_offset = row_i * self.params_row
                with tik_instance.for_range(0, self.params_row) as i:
                    res_ub[res_ub_offset + i].set_as(x_ub[gm_offset + i])

            # copy result data from ub to gm
            tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res, 0, 0)

    def process_last_mode_4(self, tensor_list, offset_list):
        """
        process row_num_last indices for tiling mode 4

        Parameters
        ----------
        tensor_list: [row_num_last, indices_ub, res_ub, x_ub]
        offset_list: [indices_num_offset, inner_indices_offset, pre_i]

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance

        row_num_last, indices_ub, res_ub, x_ub = tensor_list
        indices_num_offset, inner_indices_offset, pre_i = offset_list

        burst_len_res = ceil_value(row_num_last * self.params_row * self.params_dsize, constant.BLOCK_SIZE)
        output_offset = (pre_i * self.indices_num + indices_num_offset + inner_indices_offset) * self.params_row

        with tik_instance.for_range(0, row_num_last, thread_num=2) as row_i:
            # compute gm offset of x
            indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
            indices_value.set_as(indices_ub[inner_indices_offset + row_i])
            gm_offset = (pre_i * self.params_axis + indices_value) * self.params_row

            # set result to res_ub
            res_ub_offset = row_i * self.params_row
            with tik_instance.for_range(0, self.params_row) as i:
                res_ub[res_ub_offset + i].set_as(x_ub[gm_offset + i])

        # move result data from ub to gm
        tail_elem = (row_num_last * self.params_row) % self.block_elem
        with tik_instance.if_scope(tik.all(tail_elem != 0, burst_len_res > 1)):
            block_ub = tik_instance.Tensor(self.params_dtype, (self.block_elem,), name="block_ub", scope=tik.scope_ubuf)
            with tik_instance.for_range(0, self.block_elem) as num_i:
                block_ub[num_i].set_as(res_ub[row_num_last * self.params_row - self.block_elem + num_i])

            tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res - 1, 0, 0)
            tik_instance.data_move(
                self.y[output_offset + (row_num_last * self.params_row - self.block_elem)], block_ub, 0, 1, 1, 0, 0
            )
        with tik_instance.else_scope():
            tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res, 0, 0)

    def process_remaining_tail_mode_4(self, pre_i, indices_ub, res_ub, x_ub):
        """
        process remaining tail indices in core 0 for tiling mode 2

        Parameters
        ----------
        pre_i: params_pre
        indices_ub: cache indices data in UB
        res_ub: cache result data in UB
        x_ub: source params that cache in UB

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        indices_num_offset = self.need_core_num * self.indices_num_each_core
        # move indices data to ub from gm
        tik_instance.data_move(
            indices_ub,
            self.indices[indices_num_offset],
            0,
            1,
            ceil_value(self.indices_num_remaining * self.indices_dsize, constant.BLOCK_SIZE),
            0,
            0,
        )

        burst_len_res = ceil_value(
            self.indices_num_remaining * self.params_row * self.params_dsize, constant.BLOCK_SIZE
        )
        output_offset = (pre_i * self.indices_num + indices_num_offset) * self.params_row

        with tik_instance.for_range(0, self.indices_num_remaining, thread_num=1) as row_i:
            indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
            indices_value.set_as(indices_ub[row_i])
            gm_offset = (pre_i * self.params_axis + indices_value) * self.params_row

            # set result to res_ub
            res_ub_offset = row_i * self.params_row
            with tik_instance.for_range(0, self.params_row) as i:
                res_ub[res_ub_offset + i].set_as(x_ub[gm_offset + i])

        # copy result data from ub to gm
        tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res, 0, 0)

    def compute_mode_2(self, half_ub_size, block_id):
        """
        compute for tiling mode 2

        Parameters
        ----------
        half_ub_size: bytes of half UB
        block_id: id of ai core

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        indices_dsize = self.indices_dsize
        params_dsize = self.params_dsize

        indices_ub = tik_instance.Tensor(
            self.indices_dtype, (half_ub_size // indices_dsize,), name="indices_ub", scope=tik.scope_ubuf
        )
        res_ub = tik_instance.Tensor(
            self.params_dtype, (half_ub_size // params_dsize,), name="res_ub", scope=tik.scope_ubuf
        )
        with tik_instance.for_range(0, self.params_pre) as pre_i:
            # a. process indices_row_num_once
            with tik_instance.for_range(0, self.indices_loop_num) as indices_loop_i:
                indices_offset = block_id * self.indices_num_each_core + indices_loop_i * self.indices_row_num_once
                tik_instance.data_move(
                    indices_ub,
                    self.indices[indices_offset],
                    0,
                    1,
                    ceil_value(self.indices_row_num_once * indices_dsize, constant.BLOCK_SIZE),
                    0,
                    0,
                )

                self.indices_inner_gather_2(self.indices_row_num_once, indices_offset, pre_i, indices_ub, res_ub)
                # process last one loop in indices_loop_num
                self.indices_inner_gather_tail_2(self.indices_row_num_once, indices_offset, pre_i, indices_ub, res_ub)

            # b. process indices_row_num_last
            with tik_instance.if_scope(self.indices_row_num_last > 0):
                indices_offset = (
                    block_id * self.indices_num_each_core + self.indices_loop_num * self.indices_row_num_once
                )
                tik_instance.data_move(
                    indices_ub,
                    self.indices[indices_offset],
                    0,
                    1,
                    ceil_value(self.indices_row_num_last * indices_dsize, constant.BLOCK_SIZE),
                    0,
                    0,
                )

                # process first (indices_row_num_last - 1) loop
                self.indices_inner_gather_2(self.indices_row_num_last, indices_offset, pre_i, indices_ub, res_ub)
                # process last one loop
                self.indices_inner_gather_tail_2(self.indices_row_num_last, indices_offset, pre_i, indices_ub, res_ub)

            # c. process indices remaining
            with tik_instance.if_scope(tik.all(self.indices_num_remaining > 0, block_id < self.indices_num_remaining)):
                indices_offset = self.indices_num_each_core * self.need_core_num + block_id
                self.indices_inner_gather_remain_2(indices_offset, pre_i, indices_ub, res_ub)

    # 'pylint: disable=too-many-arguments,huawei-too-many-arguments
    def indices_inner_gather_2(self, loop_num, indices_offset, pre_i, indices_ub, res_ub):
        """
        process row_num_once_ub indices for tiling mode 2
        """
        tik_instance = self.tik_instance
        burst_len_row = ceil_value(self.params_row * self.params_dsize, constant.BLOCK_SIZE)

        with tik_instance.for_range(0, loop_num - 1) as row_i:
            indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
            indices_value.set_as(indices_ub[row_i])
            gm_offset = (pre_i * self.params_axis + indices_value) * self.params_row
            output_offset = (pre_i * self.indices_num + indices_offset + row_i) * self.params_row

            tik_instance.data_move(res_ub, self.x[gm_offset], 0, 1, burst_len_row, 0, 0)
            tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_row, 0, 0)

    def indices_inner_gather_tail_2(self, loop_num, indices_offset, pre_i, indices_ub, res_ub):
        """
        process last loop num indices for tiling mode 2
        """
        tik_instance = self.tik_instance
        burst_len_row = ceil_value(self.params_row * self.params_dsize, constant.BLOCK_SIZE)

        indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
        indices_value.set_as(indices_ub[loop_num - 1])
        gm_offset = (pre_i * self.params_axis + indices_value) * self.params_row
        output_offset = (pre_i * self.indices_num + indices_offset + loop_num - 1) * self.params_row

        tik_instance.data_move(res_ub, self.x[gm_offset], 0, 1, burst_len_row, 0, 0)

        # set tail 32B of result to block_ub
        block_ub = tik_instance.Tensor(self.params_dtype, (self.block_elem,), name="block_ub", scope=tik.scope_ubuf)
        with tik_instance.for_range(0, self.block_elem) as num_i:
            block_ub[num_i].set_as(res_ub[self.params_row - self.block_elem + num_i])

        tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_row - 1, 0, 0)
        tik_instance.data_move(self.y[output_offset + (self.params_row - self.block_elem)], block_ub, 0, 1, 1, 0, 0)

    def indices_inner_gather_remain_2(self, indices_offset, pre_i, indices_ub, res_ub):
        """
        process tail indices in previous indices_num_remaining core for tiling mode 2
        """
        tik_instance = self.tik_instance
        burst_len_row = ceil_value(self.params_row * self.params_dsize, constant.BLOCK_SIZE)

        # move one indices data to ub from gm
        tik_instance.data_move(indices_ub, self.indices[indices_offset], 0, 1, 1, 0, 0)

        indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
        indices_value.set_as(indices_ub[0])
        gm_offset = (pre_i * self.params_axis + indices_value) * self.params_row
        output_offset = (pre_i * self.indices_num + indices_offset) * self.params_row

        # copy one params_row from gm to res_ub
        tik_instance.data_move(res_ub, self.x[gm_offset], 0, 1, burst_len_row, 0, 0)

        # set tail 32B of result to block_ub
        block_ub = tik_instance.Tensor(self.params_dtype, (self.block_elem,), name="block_ub", scope=tik.scope_ubuf)
        with tik_instance.for_range(0, self.block_elem) as num_i:
            block_ub[num_i].set_as(res_ub[self.params_row - self.block_elem + num_i])

        tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_row - 1, 0, 0)
        tik_instance.data_move(self.y[output_offset + (self.params_row - self.block_elem)], block_ub, 0, 1, 1, 0, 0)

    def compute_mode_5(self, half_ub_size, block_id):
        """
        compute for tiling mode 5

        Parameters
        ----------
        half_ub_size: bytes of half UB
        block_id: id of ai core

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        indices_ub = tik_instance.Tensor(
            self.indices_dtype, (half_ub_size // self.indices_dsize,), name="indices_ub", scope=tik.scope_ubuf
        )
        res_ub = tik_instance.Tensor(
            self.params_dtype,
            ((half_ub_size + constant.BLOCK_SIZE) // self.params_dsize,),
            name="res_ub",
            scope=tik.scope_ubuf,
        )
        half_ub_params_elem = half_ub_size // self.params_dsize

        with tik_instance.for_range(0, self.params_pre) as pre_i:
            with tik_instance.for_range(0, self.indices_loop_num) as indices_loop_i:
                indices_num_offset = block_id * self.indices_num_each_core + indices_loop_i * self.indices_row_num_once
                # move indices data to ub from gm
                tik_instance.data_move(
                    indices_ub,
                    self.indices[indices_num_offset],
                    0,
                    1,
                    ceil_value(self.indices_row_num_once * self.indices_dsize, constant.BLOCK_SIZE),
                    0,
                    0,
                )

                # process one indices_row_num_once
                self.process_loop_mode_5(
                    self.indices_row_num_once, indices_num_offset, pre_i, indices_ub, res_ub, half_ub_params_elem
                )

            # b. process indices_row_num_last
            with tik_instance.if_scope(self.indices_row_num_last > 0):
                indices_num_offset = (
                    block_id * self.indices_num_each_core + self.indices_loop_num * self.indices_row_num_once
                )
                # copy indices data to ub from gm
                tik_instance.data_move(
                    indices_ub,
                    self.indices[indices_num_offset],
                    0,
                    1,
                    ceil_value(self.indices_row_num_last * self.indices_dsize, constant.BLOCK_SIZE),
                    0,
                    0,
                )

                self.process_loop_mode_5(
                    self.indices_row_num_last, indices_num_offset, pre_i, indices_ub, res_ub, half_ub_params_elem
                )

            with tik_instance.if_scope(tik.all(self.indices_num_remaining > 0, block_id < self.indices_num_remaining)):
                indices_num_offset = self.indices_num_each_core * self.need_core_num + block_id
                self.process_remaining_tail_mode_5(indices_num_offset, pre_i, indices_ub, res_ub, half_ub_params_elem)

    # 'pylint: disable=too-many-arguments,huawei-too-many-arguments
    def process_loop_mode_5(self, loop_num, indices_num_offset, pre_i, indices_ub, res_ub, half_ub_params_elem):
        """
        previous loop_num times process for tiling mode 5

        Parameters
        ----------
        loop_num: loop times
        indices_num_offset: indices num offset
        pre_i: params pre
        indices_ub: cache indices data in UB
        res_ub: cache result data in UB
        half_ub_params_elem: number of params element that can be stored in half UB space

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        block_ub = tik_instance.Tensor(self.params_dtype, (self.block_elem,), name="block_ub", scope=tik.scope_ubuf)
        burst_len_sub_row = ceil_value(half_ub_params_elem, self.block_elem)
        burst_len_sub_row_last = ceil_value(self.one_row_tail, self.block_elem)

        # indices_row_num_once
        with tik_instance.for_range(0, loop_num) as row_i:
            output_offset = (pre_i * self.indices_num + indices_num_offset + row_i) * self.params_row
            indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
            indices_value.set_as(indices_ub[row_i])
            gm_offset = (pre_i * self.params_axis + indices_value) * self.params_row

            # process the front part of one params_row
            with tik_instance.for_range(0, self.one_row_loop) as row_inner_i:
                # move half_ub_params_elem data of one row to res_ub from gm
                tik_instance.data_move(
                    res_ub, self.x[gm_offset + row_inner_i * half_ub_params_elem], 0, 1, burst_len_sub_row, 0, 0
                )
                # copy result data to gm from ub
                tik_instance.data_move(
                    self.y[output_offset + row_inner_i * half_ub_params_elem], res_ub, 0, 1, burst_len_sub_row, 0, 0
                )

            # process of one the tail part of params_row: one_row_tail
            with tik_instance.if_scope(self.one_row_tail > 0):
                # move one_row_tail data to res_ub from gm
                tik_instance.data_move(
                    res_ub,
                    self.x[gm_offset + (self.params_row - self.one_row_tail)],
                    0,
                    1,
                    burst_len_sub_row_last,
                    0,
                    0,
                )

                # copy result data to gm from ub
                with tik_instance.if_scope(tik.all(self.one_row_tail % self.block_elem != 0, loop_num - 1 == row_i)):
                    with tik_instance.for_range(0, self.block_elem) as num_i:
                        block_ub[num_i].set_as(res_ub[self.one_row_tail - self.block_elem + num_i])

                    tik_instance.data_move(
                        self.y[output_offset + (self.params_row - self.one_row_tail)],
                        res_ub,
                        0,
                        1,
                        burst_len_sub_row_last - 1,
                        0,
                        0,
                    )
                    tik_instance.data_move(
                        self.y[output_offset + (self.params_row - self.block_elem)], block_ub, 0, 1, 1, 0, 0
                    )
                with tik_instance.else_scope():
                    tik_instance.data_move(
                        self.y[output_offset + (self.params_row - self.one_row_tail)],
                        res_ub,
                        0,
                        1,
                        burst_len_sub_row_last,
                        0,
                        0,
                    )

    # 'pylint: disable=too-many-arguments,huawei-too-many-arguments
    def process_remaining_tail_mode_5(self, indices_num_offset, pre_i, indices_ub, res_ub, half_ub_params_elem):
        """
        process tail indices in previous indices_num_remaining core for tiling mode 5

        Parameters
        ----------
        indices_num_offset: indices num offset
        pre_i: params_row
        indices_ub: cache indices data in UB
        res_ub: cache result data in UB
        half_ub_params_elem: number of params element that can be stored in half UB space

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        # move one indices data to ub from gm
        tik_instance.data_move(indices_ub, self.indices[indices_num_offset], 0, 1, 1, 0, 0)

        burst_len_sub_row = ceil_value(half_ub_params_elem, self.block_elem)
        burst_len_sub_row_last = ceil_value(self.one_row_tail, self.block_elem)
        output_offset = (pre_i * self.indices_num + indices_num_offset) * self.params_row

        indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
        indices_value.set_as(indices_ub[0])
        gm_offset = (pre_i * self.params_axis + indices_value) * self.params_row

        # process the front part of one params_row: one_row_loop * half_ub_params_elem
        with tik_instance.for_range(0, self.one_row_loop) as row_inner_i:
            # move half_ub_params_elem data of one row to res_ub from gm
            tik_instance.data_move(
                res_ub, self.x[gm_offset + row_inner_i * half_ub_params_elem], 0, 1, burst_len_sub_row, 0, 0
            )
            # copy result data to gm from ub
            tik_instance.data_move(
                self.y[output_offset + row_inner_i * half_ub_params_elem], res_ub, 0, 1, burst_len_sub_row, 0, 0
            )

        # process of one the tail part of params_row: one_row_tail
        with tik_instance.if_scope(self.one_row_tail > 0):
            # move one_row_tail data to res_ub from gm
            tik_instance.data_move(
                res_ub, self.x[gm_offset + (self.params_row - self.one_row_tail)], 0, 1, burst_len_sub_row_last, 0, 0
            )
            # copy result data to gm from ub
            with tik_instance.if_scope(self.one_row_tail % self.block_elem != 0):
                block_ub = tik_instance.Tensor(
                    self.params_dtype, (self.block_elem,), name="block_ub", scope=tik.scope_ubuf
                )
                with tik_instance.for_range(0, self.block_elem) as num_i:
                    block_ub[num_i].set_as(res_ub[self.one_row_tail - self.block_elem + num_i])

                tik_instance.data_move(
                    self.y[output_offset + (self.params_row - self.one_row_tail)],
                    res_ub,
                    0,
                    1,
                    burst_len_sub_row_last - 1,
                    0,
                    0,
                )
                tik_instance.data_move(
                    self.y[output_offset + (self.params_row - self.block_elem)], block_ub, 0, 1, 1, 0, 0
                )
            with tik_instance.else_scope():
                tik_instance.data_move(
                    self.y[output_offset + (self.params_row - self.one_row_tail)],
                    res_ub,
                    0,
                    1,
                    burst_len_sub_row_last,
                    0,
                    0,
                )

    def compute_mode_3(self, half_ub_size, block_id):
        """
        compute for tiling mode 3

        Parameters
        ----------
        half_ub_size: bytes of half UB
        block_id: id of ai core

        Returns
        -------
        None
        """
        self.compute_mode_32b_aligned(half_ub_size, block_id, self.x)

    def compute_mode_6(self, remain_half_ub_size, block_id):
        """
        compute for tiling mode 6

        Parameters
        ----------
        remain_half_ub_size: bytes of half remain UB
        block_id: id of ai core

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        x_ub = tik_instance.Tensor(
            self.params_dtype, (Constant.CACHE_UB_SIZE // self.params_dsize,), name="x_ub", scope=tik.scope_ubuf
        )
        # cache params data from gm in UB
        tik_instance.data_move(x_ub, self.x, 0, 1, ceil_value(self.params_total, self.block_elem), 0, 0)

        self.compute_mode_32b_aligned(remain_half_ub_size, block_id, x_ub)

    def compute_mode_32b_aligned(self, half_ub_size, block_id, x_src):
        """
        compute for tiling mode of 32B aligned

        Parameters
        ----------
        half_ub_size: bytes of half UB
        block_id: id of ai core
        x_src: source x tensor

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        indices_ub = tik_instance.Tensor(
            self.indices_dtype, (half_ub_size // self.indices_dsize,), name="indices_ub", scope=tik.scope_ubuf
        )
        res_ub = tik_instance.Tensor(
            self.params_dtype, (half_ub_size // self.params_dsize,), name="res_ub", scope=tik.scope_ubuf
        )

        with tik_instance.for_range(0, self.params_pre) as pre_i:
            with tik_instance.for_range(0, self.indices_loop_num) as indices_loop_i:
                indices_num_offset = block_id * self.indices_num_each_core + indices_loop_i * self.indices_row_num_once
                # move indices data to ub from gm
                tik_instance.data_move(
                    indices_ub,
                    self.indices[indices_num_offset],
                    0,
                    1,
                    ceil_value(self.indices_row_num_once * self.indices_dsize, constant.BLOCK_SIZE),
                    0,
                    0,
                )

                with tik_instance.if_scope(self.inner_loop_num > 0):
                    self.process_loop_32b_aligned(
                        self.inner_loop_num, indices_num_offset, pre_i, indices_ub, res_ub, x_src
                    )

                # a2. process row_num_once_tail_ub
                with tik_instance.if_scope(self.row_num_once_tail_ub > 0):
                    inner_indices_offset = self.inner_loop_num * self.row_num_once_ub
                    ub_tensor_list = [self.row_num_once_tail_ub, indices_ub, res_ub]
                    offset_list = [indices_num_offset, inner_indices_offset, pre_i]
                    self.process_last_32b_aligned(ub_tensor_list, offset_list, x_src)
            with tik_instance.if_scope(self.indices_row_num_last > 0):
                indices_num_offset = (
                    block_id * self.indices_num_each_core + self.indices_loop_num * self.indices_row_num_once
                )
                # copy indices data to ub from gm
                tik_instance.data_move(
                    indices_ub,
                    self.indices[indices_num_offset],
                    0,
                    1,
                    ceil_value(self.indices_row_num_last * self.indices_dsize, constant.BLOCK_SIZE),
                    0,
                    0,
                )

                with tik_instance.if_scope(self.inner_loop_num_last > 0):
                    self.process_loop_32b_aligned(
                        self.inner_loop_num_last, indices_num_offset, pre_i, indices_ub, res_ub, x_src
                    )

                # b2. process row_num_last_tail_ub
                with tik_instance.if_scope(self.row_num_last_tail_ub > 0):
                    inner_indices_offset = self.inner_loop_num_last * self.row_num_last_ub
                    ub_tensor_list = [self.row_num_last_tail_ub, indices_ub, res_ub]
                    offset_list = [indices_num_offset, inner_indices_offset, pre_i]
                    self.process_last_32b_aligned(ub_tensor_list, offset_list, x_src)

            with tik_instance.if_scope(tik.all(self.indices_num_remaining > 0, block_id < self.indices_num_remaining)):
                indices_num_offset = self.indices_num_each_core * self.need_core_num + block_id
                self.process_remaining_tail_32b_aligned(indices_num_offset, pre_i, indices_ub, res_ub, x_src)

    # 'pylint: disable=too-many-arguments,huawei-too-many-arguments
    def process_loop_32b_aligned(self, loop_num, indices_num_offset, pre_i, indices_ub, res_ub, x_src):
        """
        previous loop_num times process for tiling mode of 32B aligned

        Parameters
        ----------
        loop_num: loop times
        indices_num_offset: indices num offset
        pre_i: params_pre
        indices_ub: cache indices data in UB
        res_ub: cache result data in UB
        x_src: source x tensor

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        burst_len_res = ceil_value(self.row_num_once_ub * self.params_row * self.params_dsize, constant.BLOCK_SIZE)
        burst_len_row = ceil_value(self.params_row * self.params_dsize, constant.BLOCK_SIZE)

        with tik_instance.for_range(0, loop_num) as inner_loop_i:
            inner_indices_offset = inner_loop_i * self.row_num_once_ub
            output_offset = (pre_i * self.indices_num + indices_num_offset + inner_indices_offset) * self.params_row

            with tik_instance.new_stmt_scope(disable_sync=True):
                with tik_instance.for_range(0, self.row_num_once_ub, thread_num=2) as row_i:
                    indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
                    indices_value.set_as(indices_ub[inner_indices_offset + row_i])
                    gm_offset = (pre_i * self.params_axis + indices_value) * self.params_row

                    # move params_row from gm or UB or L1 to res_ub
                    tik_instance.data_move(res_ub[row_i * self.params_row], x_src[gm_offset], 0, 1, burst_len_row, 0, 0)

            # copy result data from ub to gm
            tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res, 0, 0)

    def process_last_32b_aligned(self, ub_tensor_list, offset_list, x_src):
        """
        process last row_num_last indices for tiling mode of 32B aligned

        Parameters
        ----------
        ub_tensor_list: [row_num_last, indices_ub, res_ub]
        offset_list: [indices_num_offset, inner_indices_offset, pre_i]
        x_src: source x tensor

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance

        row_num_last, indices_ub, res_ub = ub_tensor_list
        indices_num_offset, inner_indices_offset, pre_i = offset_list
        burst_len_res = ceil_value(row_num_last * self.params_row * self.params_dsize, constant.BLOCK_SIZE)
        burst_len_row = ceil_value(self.params_row * self.params_dsize, constant.BLOCK_SIZE)
        output_offset = (pre_i * self.indices_num + indices_num_offset + inner_indices_offset) * self.params_row

        with tik_instance.new_stmt_scope(disable_sync=True):
            with tik_instance.for_range(0, row_num_last, thread_num=2) as row_i:
                indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
                indices_value.set_as(indices_ub[inner_indices_offset + row_i])
                gm_offset = (pre_i * self.params_axis + indices_value) * self.params_row

                # move params_row data from gm or UB or L1 to res_ub
                tik_instance.data_move(res_ub[row_i * self.params_row], x_src[gm_offset], 0, 1, burst_len_row, 0, 0)

        # move result data from ub to gm
        tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res, 0, 0)

    # 'pylint: disable=too-many-arguments,huawei-too-many-arguments
    def process_remaining_tail_32b_aligned(self, indices_num_offset, pre_i, indices_ub, res_ub, x_src):
        """
        process tail indices in previous indices_num_remaining core for tiling mode of 32B aligned

        Parameters
        ----------
        indices_num_offset: indices num offset
        indices_ub: cache indices data in UB
        res_ub: cache result data in UB
        x_src: source x tensor

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        # move one indices data from gm to ub
        tik_instance.data_move(indices_ub, self.indices[indices_num_offset], 0, 1, 1, 0, 0)

        burst_len_row = ceil_value(self.params_row * self.params_dsize, constant.BLOCK_SIZE)
        output_offset = (pre_i * self.indices_num + indices_num_offset) * self.params_row

        indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
        indices_value.set_as(indices_ub[0])
        gm_offset = (pre_i * self.params_axis + indices_value) * self.params_row

        # copy one params_row from gm or UB or L1 to res_ub
        tik_instance.data_move(res_ub, x_src[gm_offset], 0, 1, burst_len_row, 0, 0)

        # copy result data from ub to gm
        tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_row, 0, 0)

    def compute_mode_7(self, half_ub_size, block_id):
        """
        compute for tiling mode 7

        Parameters
        ----------
        half_ub_size: bytes of half UB
        block_id: id of ai core

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        if tbe_platform_adapter.api_check_support("tik.vcopy") or self.is_vector_core:
            self.compute_mode_32b_aligned(half_ub_size, block_id, self.x)
        else:
            x_cbuf = tik_instance.Tensor(
                self.params_dtype, (self.l1_size // self.params_dsize,), name="x_l1", scope=tik.scope_cbuf
            )

            # cache params data from gm in UB
            tik_instance.data_move(x_cbuf, self.x, 0, 1, ceil_value(self.params_total, self.block_elem), 0, 0)
            self.compute_mode_32b_aligned(half_ub_size, block_id, x_cbuf)

    def compute_mode_8(self, remain_half_ub_size, block_id):
        """
        compute for tiling mode 8

        Parameters
        ----------
        remain_half_ub_size: bytes of half remain UB
        block_id: id of ai core

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        indices_ub = tik_instance.Tensor(
            self.indices_dtype,
            ((remain_half_ub_size + 256) // self.indices_dsize,),
            name="indices_ub",
            scope=tik.scope_ubuf,
        )
        res_ub = tik_instance.Tensor(
            self.params_dtype,
            ((remain_half_ub_size + constant.BLOCK_SIZE) // self.params_dsize,),
            name="res_ub",
            scope=tik.scope_ubuf,
        )
        x_ub = tik_instance.Tensor(
            self.params_dtype, (Constant.CACHE_UB_SIZE // self.params_dsize,), name="x_ub", scope=tik.scope_ubuf
        )

        # cache params data in UB from gm
        tik_instance.data_move(x_ub, self.x, 0, 1, ceil_value(self.params_total, self.block_elem), 0, 0)

        range_left = block_id * self.params_pre_each_core
        range_right = (block_id + 1) * self.params_pre_each_core
        with tik_instance.for_range(range_left, range_right) as pre_i:
            self.compute_mode_less_than_32b_tiling_params_ub(pre_i, indices_ub, res_ub, x_ub)
        with tik_instance.if_scope(tik.all(self.params_pre_remaining > 0, block_id < self.params_pre_remaining)):
            pre_i = self.need_core_num * self.params_pre_each_core + block_id
            self.compute_mode_less_than_32b_tiling_params_ub(pre_i, indices_ub, res_ub, x_ub)

    def compute_mode_less_than_32b_tiling_params_ub(self, pre_i, indices_ub, res_ub, x_src):
        """
        compute for tiling mode of less than 32B when tiling params_pre

        Parameters
        ----------
        pre_i: params_pre
        indices_ub: cache indices data in UB
        res_ub: cache result data in UB
        x_src: source x tensor

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance

        # 1. indices_num_each_core: indices_row_num_once * indices_loop_num + indices_row_num_last
        # a. process indices_row_num_once * indices_loop_num
        with tik_instance.for_range(0, self.indices_loop_num) as indices_loop_i:
            indices_num_offset = indices_loop_i * self.indices_row_num_once
            # move indices data from gm to ub
            tik_instance.data_move(
                indices_ub,
                self.indices[indices_num_offset],
                0,
                1,
                ceil_value(self.indices_row_num_once * self.indices_dsize, constant.BLOCK_SIZE),
                0,
                0,
            )

            with tik_instance.if_scope(self.inner_loop_num > 0):
                self.process_loop_mode_4(self.inner_loop_num, indices_num_offset, pre_i, indices_ub, res_ub, x_src)

            # a2. process row_num_once_tail_ub
            with tik_instance.if_scope(self.row_num_once_tail_ub > 0):
                inner_indices_offset = self.inner_loop_num * self.row_num_once_ub
                tensor_list = [self.row_num_once_tail_ub, indices_ub, res_ub, x_src]
                offset_list = [indices_num_offset, inner_indices_offset, pre_i]
                self.process_last_mode_4(tensor_list, offset_list)

        with tik_instance.if_scope(self.indices_row_num_last > 0):
            indices_num_offset = self.indices_loop_num * self.indices_row_num_once
            # copy indices data from gm to ub
            tik_instance.data_move(
                indices_ub,
                self.indices[indices_num_offset],
                0,
                1,
                ceil_value(self.indices_row_num_last * self.indices_dsize, constant.BLOCK_SIZE),
                0,
                0,
            )

            with tik_instance.if_scope(self.inner_loop_num_last > 0):
                self.process_loop_mode_4(self.inner_loop_num_last, indices_num_offset, pre_i, indices_ub, res_ub, x_src)

            with tik_instance.if_scope(self.row_num_last_tail_ub > 0):
                inner_indices_offset = self.inner_loop_num_last * self.row_num_last_ub
                tensor_list = [self.row_num_last_tail_ub, indices_ub, res_ub, x_src]
                offset_list = [indices_num_offset, inner_indices_offset, pre_i]
                self.process_last_mode_4(tensor_list, offset_list)

    def compute_mode_9(self, half_ub_size, quarter_ub_size, block_id):
        """
        compute for tiling mode 9

        Parameters
        ----------
        half_ub_size: bytes of half remain UB
        block_id: id of ai core

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        indices_ub = tik_instance.Tensor(
            self.indices_dtype, ((half_ub_size + 256) // self.indices_dsize,), name="indices_ub", scope=tik.scope_ubuf
        )
        res_ub = tik_instance.Tensor(
            self.params_dtype, ((quarter_ub_size + 256) // self.params_dsize,), name="res_ub", scope=tik.scope_ubuf
        )
        block_ub = tik_instance.Tensor(
            self.params_dtype, ((quarter_ub_size + 256) // self.params_dsize,), name="block_ub", scope=tik.scope_ubuf
        )

        range_left = block_id * self.params_pre_each_core
        range_right = (block_id + 1) * self.params_pre_each_core
        with tik_instance.for_range(range_left, range_right) as pre_i:
            self.compute_mode_less_than_32b_tiling_params(pre_i, quarter_ub_size, indices_ub, res_ub, block_ub, self.x)
        with tik_instance.if_scope(tik.all(self.params_pre_remaining > 0, block_id < self.params_pre_remaining)):
            pre_i = self.need_core_num * self.params_pre_each_core + block_id
            self.compute_mode_less_than_32b_tiling_params(pre_i, quarter_ub_size, indices_ub, res_ub, block_ub, self.x)

    # 'pylint: disable=too-many-arguments,huawei-too-many-arguments
    def compute_mode_less_than_32b_tiling_params(self, pre_i, quarter_ub_size, indices_ub, res_ub, block_ub, x_src):
        """
        compute for tiling mode of less than 32B when tiling params_pre

        Parameters
        ----------
        pre_i: params_pre
        indices_ub: cache indices data in UB
        res_ub: cache result data in UB
        x_src: source x tensor

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance

        # 1. indices_num_each_core: indices_row_num_once * indices_loop_num + indices_row_num_last
        # a. process indices_row_num_once * indices_loop_num
        with tik_instance.for_range(0, self.indices_loop_num) as indices_loop_i:
            indices_num_offset = indices_loop_i * self.indices_row_num_once
            # move indices data from gm to ub
            tik_instance.data_move(
                indices_ub,
                self.indices[indices_num_offset],
                0,
                1,
                ceil_value(self.indices_row_num_once * self.indices_dsize, constant.BLOCK_SIZE),
                0,
                0,
            )

            with tik_instance.if_scope(self.inner_loop_num > 0):
                with tik_instance.for_range(0, self.inner_loop_num) as inner_loop_i:
                    inner_indices_offset = inner_loop_i * self.row_num_once_ub
                    ub_tensor_list = [self.row_num_once_ub, indices_ub, res_ub, block_ub]
                    offset_list = [indices_num_offset, inner_indices_offset, pre_i]
                    self.process_mode_1(ub_tensor_list, offset_list, quarter_ub_size, self.x)

            # a2. process row_num_once_tail_ub
            with tik_instance.if_scope(self.row_num_once_tail_ub > 0):
                inner_indices_offset = self.inner_loop_num * self.row_num_once_ub
                ub_tensor_list = [self.row_num_once_tail_ub, indices_ub, res_ub, block_ub]
                offset_list = [indices_num_offset, inner_indices_offset, pre_i]
                self.process_mode_1(ub_tensor_list, offset_list, quarter_ub_size, self.x)

        with tik_instance.if_scope(self.indices_row_num_last > 0):
            indices_num_offset = self.indices_loop_num * self.indices_row_num_once
            # copy indices data from gm to ub
            tik_instance.data_move(
                indices_ub,
                self.indices[indices_num_offset],
                0,
                1,
                ceil_value(self.indices_row_num_last * self.indices_dsize, constant.BLOCK_SIZE),
                0,
                0,
            )

            with tik_instance.if_scope(self.inner_loop_num_last > 0):
                with tik_instance.for_range(0, self.inner_loop_num_last) as inner_loop_i:
                    inner_indices_offset = inner_loop_i * self.row_num_once_ub
                    ub_tensor_list = [self.row_num_once_ub, indices_ub, res_ub, block_ub]
                    offset_list = [indices_num_offset, inner_indices_offset, pre_i]
                    self.process_mode_1(ub_tensor_list, offset_list, quarter_ub_size, self.x)

            with tik_instance.if_scope(self.row_num_last_tail_ub > 0):
                inner_indices_offset = self.inner_loop_num_last * self.row_num_last_ub
                ub_tensor_list = [self.row_num_last_tail_ub, indices_ub, res_ub, block_ub]
                offset_list = [indices_num_offset, inner_indices_offset, pre_i]
                self.process_mode_1(ub_tensor_list, offset_list, quarter_ub_size, self.x)

    def compute_mode_10(self, remain_half_ub_size, block_id):
        """
        compute for tiling mode 10

        Parameters
        ----------
        remain_half_ub_size: bytes of half remain UB
        block_id: id of ai core

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        x_ub = tik_instance.Tensor(
            self.params_dtype, (Constant.CACHE_UB_SIZE // self.params_dsize,), name="x_ub", scope=tik.scope_ubuf
        )
        # cache params data from gm in UB
        tik_instance.data_move(x_ub, self.x, 0, 1, ceil_value(self.params_total, self.block_elem), 0, 0)

        indices_ub = tik_instance.Tensor(
            self.indices_dtype, (remain_half_ub_size // self.indices_dsize,), name="indices_ub", scope=tik.scope_ubuf
        )
        res_ub = tik_instance.Tensor(
            self.params_dtype, (remain_half_ub_size // self.params_dsize,), name="res_ub", scope=tik.scope_ubuf
        )

        range_left = block_id * self.params_pre_each_core
        range_right = (block_id + 1) * self.params_pre_each_core
        with tik_instance.for_range(range_left, range_right) as pre_i:
            self.compute_mode_32b_aligned_tiling_params(pre_i, indices_ub, res_ub, x_ub)

        with tik_instance.if_scope(tik.all(self.params_pre_remaining > 0, block_id < self.params_pre_remaining)):
            pre_i = self.need_core_num * self.params_pre_each_core + block_id
            self.compute_mode_32b_aligned_tiling_params(pre_i, indices_ub, res_ub, x_ub)

    def compute_mode_32b_aligned_tiling_params(self, pre_i, indices_ub, res_ub, x_src):
        """
        compute for tiling mode of 32B aligned when tiling params_pre

        Parameters
        ----------
        pre_i: params_pre
        indices_ub: cache indices data in UB
        res_ub: cache result data in UB
        x_src: source x tensor

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance

        with tik_instance.for_range(0, self.indices_loop_num) as indices_loop_i:
            indices_num_offset = indices_loop_i * self.indices_row_num_once
            # move indices data to ub from gm
            tik_instance.data_move(
                indices_ub,
                self.indices[indices_num_offset],
                0,
                1,
                ceil_value(self.indices_row_num_once * self.indices_dsize, constant.BLOCK_SIZE),
                0,
                0,
            )

            with tik_instance.if_scope(self.inner_loop_num > 0):
                self.process_loop_32b_aligned(self.inner_loop_num, indices_num_offset, pre_i, indices_ub, res_ub, x_src)

            # a2. process row_num_once_tail_ub
            with tik_instance.if_scope(self.row_num_once_tail_ub > 0):
                inner_indices_offset = self.inner_loop_num * self.row_num_once_ub
                ub_tensor_list = [self.row_num_once_tail_ub, indices_ub, res_ub]
                offset_list = [indices_num_offset, inner_indices_offset, pre_i]
                self.process_last_32b_aligned(ub_tensor_list, offset_list, x_src)

        with tik_instance.if_scope(self.indices_row_num_last > 0):
            indices_num_offset = self.indices_loop_num * self.indices_row_num_once
            # copy indices data to ub from gm
            tik_instance.data_move(
                indices_ub,
                self.indices[indices_num_offset],
                0,
                1,
                ceil_value(self.indices_row_num_last * self.indices_dsize, constant.BLOCK_SIZE),
                0,
                0,
            )

            with tik_instance.if_scope(self.inner_loop_num_last > 0):
                self.process_loop_32b_aligned(
                    self.inner_loop_num_last, indices_num_offset, pre_i, indices_ub, res_ub, x_src
                )

            # b2. process row_num_last_tail_ub
            with tik_instance.if_scope(self.row_num_last_tail_ub > 0):
                inner_indices_offset = self.inner_loop_num_last * self.row_num_last_ub
                ub_tensor_list = [self.row_num_last_tail_ub, indices_ub, res_ub]
                offset_list = [indices_num_offset, inner_indices_offset, pre_i]
                self.process_last_32b_aligned(ub_tensor_list, offset_list, x_src)

    def compute_mode_11(self, half_ub_size, block_id):
        """
        compute for tiling mode 11

        Parameters
        ----------
        half_ub_size: bytes of half UB
        block_id: id of ai core

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance
        if tbe_platform_adapter.api_check_support("tik.vcopy") or self.is_vector_core:
            x_cbuf = self.x
        else:
            x_cbuf = tik_instance.Tensor(
                self.params_dtype, (self.l1_size // self.params_dsize,), name="x_l1", scope=tik.scope_cbuf
            )

            # cache params data from gm in L1
            tik_instance.data_move(x_cbuf, self.x, 0, 1, ceil_value(self.params_total, self.block_elem), 0, 0)

        indices_ub = tik_instance.Tensor(
            self.indices_dtype, (half_ub_size // self.indices_dsize,), name="indices_ub", scope=tik.scope_ubuf
        )
        res_ub = tik_instance.Tensor(
            self.params_dtype, (half_ub_size // self.params_dsize,), name="res_ub", scope=tik.scope_ubuf
        )

        range_left = block_id * self.params_pre_each_core
        range_right = (block_id + 1) * self.params_pre_each_core
        with tik_instance.for_range(range_left, range_right) as pre_i:
            self.compute_mode_32b_aligned_tiling_params(pre_i, indices_ub, res_ub, x_cbuf)

        with tik_instance.if_scope(tik.all(self.params_pre_remaining > 0, block_id < self.params_pre_remaining)):
            pre_i = self.need_core_num * self.params_pre_each_core + block_id
            self.compute_mode_32b_aligned_tiling_params(pre_i, indices_ub, res_ub, x_cbuf)

    def compute_mode_12(self, half_ub_size, block_id):
        """
        compute for tiling mode 12

        Parameters
        ----------
        half_ub_size: bytes of half UB
        block_id: id of ai core

        Returns
        -------
        None
        """
        tik_instance = self.tik_instance

        indices_ub = tik_instance.Tensor(
            self.indices_dtype, (half_ub_size // self.indices_dsize,), name="indices_ub", scope=tik.scope_ubuf
        )
        res_ub = tik_instance.Tensor(
            self.params_dtype, (half_ub_size // self.params_dsize,), name="res_ub", scope=tik.scope_ubuf
        )

        range_left = block_id * self.params_pre_each_core
        range_right = (block_id + 1) * self.params_pre_each_core
        with tik_instance.for_range(range_left, range_right) as pre_i:
            self.compute_mode_32b_aligned_tiling_params(pre_i, indices_ub, res_ub, self.x)

        with tik_instance.if_scope(tik.all(self.params_pre_remaining > 0, block_id < self.params_pre_remaining)):
            pre_i = self.need_core_num * self.params_pre_each_core + block_id
            self.compute_mode_32b_aligned_tiling_params(pre_i, indices_ub, res_ub, self.x)

    def compute_mode_14(self, block_id):
        """
        compute for tiling mode 14

        Parameters
        ----------
        block_id: id of ai core

        Returns
        -------
        None
        """
        tik_inst = self.tik_instance

        idx_start = tik_inst.Scalar(
            dtype=self.tiling_dtype, name="idx_start", init_value=block_id * self.indices_num_each_core
        )
        idx_num_cur_core = tik_inst.Scalar(
            dtype=self.tiling_dtype, name="idx_num_cur_core", init_value=self.indices_num_each_core
        )
        with tik_inst.if_scope(block_id >= self.tail_process_core):
            idx_start.set_as(block_id * self.indices_num_remaining + self.tail_process_core)
            idx_num_cur_core.set_as(self.indices_num_remaining)
        indices_value = tik_inst.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
        CurIdxTuple = collections.namedtuple("CurIdxTuple", ["idx_start", "idx_num_cur_core", "indices_value"])
        cur_idx_tuple = CurIdxTuple(idx_start, idx_num_cur_core, indices_value)

        row_inner_loop_elem = self.impl_mode_x0_ub_size // self.params_dsize
        x_pre_offset = tik_inst.Scalar(dtype=self.tiling_dtype, name="x_pre_offset")
        y_pre_offset = tik_inst.Scalar(dtype=self.tiling_dtype, name="y_pre_offset")
        y_offset = tik_inst.Scalar(dtype=self.tiling_dtype, name="y_offset")
        RowOffsetTuple = collections.namedtuple("RowOffsetTuple", ["x_pre_offset", "y_pre_offset", "y_offset"])
        row_offset_tuple = RowOffsetTuple(x_pre_offset, y_pre_offset, y_offset)

        with tik_inst.if_scope(tik.all(self.params_row <= row_inner_loop_elem, self.params_row % self.block_elem == 0)):
            with tik_inst.new_stmt_scope():
                self.compute_mode_14_x32b_le64k(row_offset_tuple, cur_idx_tuple)
        with tik_inst.elif_scope(self.params_row > self.block_elem):
            with tik_inst.new_stmt_scope():
                self.compute_mode_14_gt32b(row_offset_tuple, cur_idx_tuple)
        with tik_inst.else_scope():
            with tik_inst.new_stmt_scope():
                self.compute_mode_14_lt32b(row_offset_tuple, cur_idx_tuple)

    def compute_mode_14_x32b_le64k(self, row_offset_tuple, cur_idx_tuple):
        """
        compute_mode_14 branch for cases which params row is aligned 32B but less than or equal to 64KB
        """
        tik_inst = self.tik_instance
        (x_pre_offset, y_pre_offset, y_offset) = row_offset_tuple
        (idx_start, idx_num_cur_core, indices_value) = cur_idx_tuple

        x0_ub = tik_inst.Tensor(
            self.params_dtype, (self.impl_mode_x0_ub_size // self.params_dsize,), name="x0_ub", scope=tik.scope_ubuf
        )
        res_ub = tik_inst.Tensor(
            self.params_dtype, (self.impl_mode_res_ub_size // self.params_dsize,), name="res_ub", scope=tik.scope_ubuf
        )
        indices_ub = tik_inst.Tensor(
            self.indices_dtype,
            (self.impl_mode_indices_ub_size // self.indices_dsize,),
            name="indices_ub",
            scope=tik.scope_ubuf,
        )

        row_size = tik_inst.Scalar(
            dtype=self.tiling_dtype, name="row_size", init_value=self.params_row * self.params_dsize
        )
        row_burst = tik_inst.Scalar(
            dtype=self.tiling_dtype, name="row_burst", init_value=row_size // constant.BLOCK_SIZE
        )
        self.row_num_once_ub.set_as(self.impl_mode_res_ub_size // row_size)

        idx_num_per_loop = self.impl_mode_indices_ub_size // self.indices_dsize
        idx_num_cur_loop = tik_inst.Scalar(dtype=self.tiling_dtype, name="idx_num_cur_loop")
        idx_num_cur_batch = tik_inst.Scalar(dtype=self.tiling_dtype, name="idx_num_cur_batch")

        # unroll num is 16
        idx_values = tik_inst.ScalarArray(dtype=self.indices_dtype, length=16, name="idx_values", init_value=0)

        def compute_for_once_ub(idx_loop_i, idx_batch_i):
            """
            compute for once ub
            """
            with tik_inst.for_range(0, idx_num_cur_batch // 16) as idx_i:
                for roll_i in range(16):
                    idx_values[roll_i].set_as(indices_ub[idx_batch_i * self.row_num_once_ub + idx_i * 16 + roll_i])
                with tik_inst.if_scope(
                    tik.all(
                        idx_values[0] == 0,
                        idx_values[1] == 0,
                        idx_values[2] == 0,
                        idx_values[3] == 0,
                        idx_values[4] == 0,
                        idx_values[5] == 0,
                        idx_values[6] == 0,
                        idx_values[7] == 0,
                        idx_values[8] == 0,
                        idx_values[9] == 0,
                        idx_values[10] == 0,
                        idx_values[11] == 0,
                        idx_values[12] == 0,
                        idx_values[13] == 0,
                        idx_values[14] == 0,
                        idx_values[15] == 0,
                    )
                ):
                    for roll_i in range(16):
                        tik_inst.data_move(
                            res_ub[(idx_i * 16 + roll_i) * self.params_row], x0_ub, 0, 1, row_burst, 0, 0
                        )
                with tik_inst.else_scope():
                    for roll_i in range(16):
                        with tik_inst.if_scope(idx_values[roll_i] == 0):
                            tik_inst.data_move(
                                res_ub[(idx_i * 16 + roll_i) * self.params_row], x0_ub, 0, 1, row_burst, 0, 0
                            )
                        with tik_inst.else_scope():
                            tik_inst.data_move(
                                res_ub[(idx_i * 16 + roll_i) * self.params_row],
                                self.x[x_pre_offset + idx_values[roll_i] * self.params_row],
                                0,
                                1,
                                row_burst,
                                0,
                                0,
                            )
            with tik_inst.for_range(floor_align(idx_num_cur_batch, 16), idx_num_cur_batch) as idx_i:
                indices_value.set_as(indices_ub[idx_batch_i * self.row_num_once_ub + idx_i])
                with tik_inst.if_scope(indices_value == 0):
                    tik_inst.data_move(res_ub[idx_i * self.params_row], x0_ub, 0, 1, row_burst, 0, 0)
                with tik_inst.else_scope():
                    tik_inst.data_move(
                        res_ub[idx_i * self.params_row],
                        self.x[x_pre_offset + indices_value * self.params_row],
                        0,
                        1,
                        row_burst,
                        0,
                        0,
                    )
            y_offset.set_as(
                y_pre_offset + (idx_loop_i * idx_num_per_loop + idx_batch_i * self.row_num_once_ub) * self.params_row
            )
            tik_inst.data_move(self.y[y_offset], res_ub, 0, 1, idx_num_cur_batch * row_burst, 0, 0)

        def compute_for_one_idx_loop(idx_loop_i):
            """
            compute for one idx loop
            """
            tik_inst.data_move(
                indices_ub,
                self.indices[idx_start + idx_loop_i * idx_num_per_loop],
                0,
                1,
                ceil_value(idx_num_cur_loop * self.indices_dsize, constant.BLOCK_SIZE),
                0,
                0,
            )

            idx_num_cur_batch.set_as(self.row_num_once_ub)
            with tik_inst.for_range(0, idx_num_cur_loop // self.row_num_once_ub) as idx_batch_i:
                compute_for_once_ub(idx_loop_i, idx_batch_i)
            with tik_inst.if_scope(idx_num_cur_loop % self.row_num_once_ub != 0):
                idx_num_cur_batch.set_as(idx_num_cur_loop % self.row_num_once_ub)
                compute_for_once_ub(idx_loop_i, idx_num_cur_loop // self.row_num_once_ub)

        with tik_inst.for_range(0, self.params_pre) as pre_i:
            x_pre_offset.set_as(pre_i * self.params_axis * self.params_row)
            y_pre_offset.set_as((pre_i * self.indices_num + idx_start) * self.params_row)

            tik_inst.data_move(x0_ub, self.x[x_pre_offset], 0, 1, row_burst, 0, 0)

            idx_num_cur_loop.set_as(idx_num_per_loop)
            with tik_inst.for_range(0, idx_num_cur_core // idx_num_per_loop) as idx_loop_i:
                compute_for_one_idx_loop(idx_loop_i)
            with tik_inst.if_scope(idx_num_cur_core % idx_num_per_loop != 0):
                idx_num_cur_loop.set_as(idx_num_cur_core % idx_num_per_loop)
                compute_for_one_idx_loop(idx_num_cur_core // idx_num_per_loop)

    def compute_mode_14_gt32b(self, row_offset_tuple, cur_idx_tuple):
        """
        compute_mode_14 branch for cases which params row is great than 32B
        """
        tik_inst = self.tik_instance
        (x_pre_offset, y_pre_offset, y_offset) = row_offset_tuple
        (idx_start, idx_num_cur_core, indices_value) = cur_idx_tuple

        x0_ub = tik_inst.Tensor(
            self.params_dtype, (self.impl_mode_x0_ub_size // self.params_dsize,), name="x0_ub", scope=tik.scope_ubuf
        )
        res_ub = tik_inst.Tensor(
            self.params_dtype, (self.impl_mode_x0_ub_size // self.params_dsize,), name="res_ub", scope=tik.scope_ubuf
        )
        indices_ub = tik_inst.Tensor(
            self.indices_dtype,
            (self.impl_mode_x0_ub_size // self.indices_dsize,),
            name="indices_ub",
            scope=tik.scope_ubuf,
        )

        row_inner_loop_elem = self.impl_mode_x0_ub_size // self.params_dsize
        row_tail_size = tik_inst.Scalar(
            dtype=self.tiling_dtype,
            name="row_tail_size",
            init_value=self.params_row * self.params_dsize % self.impl_mode_x0_ub_size,
        )

        idx_num_per_loop = self.impl_mode_x0_ub_size // self.indices_dsize
        idx_num_cur_loop = tik_inst.Scalar(dtype=self.tiling_dtype, name="idx_num_cur_loop")

        def compute_for_one_idx_loop(row_inner_offset, row_inner_burst, idx_loop_i):
            """
            compute for one idx loop
            """
            tik_inst.data_move(
                indices_ub,
                self.indices[idx_start + idx_loop_i * idx_num_per_loop],
                0,
                1,
                ceil_value(idx_num_cur_loop * self.indices_dsize, constant.BLOCK_SIZE),
                0,
                0,
            )

            with tik_inst.for_range(0, idx_num_cur_loop) as idx_i:
                indices_value.set_as(indices_ub[idx_i])
                y_offset.set_as(
                    y_pre_offset + (idx_loop_i * idx_num_per_loop + idx_i) * self.params_row + row_inner_offset
                )
                with self.tik_instance.if_scope(indices_value == 0):
                    self.tik_instance.data_move(self.y[y_offset], x0_ub, 0, 1, row_inner_burst, 0, 0)
                with self.tik_instance.else_scope():
                    x_offset = x_pre_offset + indices_value * self.params_row + row_inner_offset
                    self.tik_instance.data_move(res_ub, self.x[x_offset], 0, 1, row_inner_burst, 0, 0)
                    self.tik_instance.data_move(self.y[y_offset], res_ub, 0, 1, row_inner_burst, 0, 0)

        def compute_for_incomplete_row(row_inner_offset, row_inner_burst):
            """
            compute for incomplete row
            """
            self.tik_instance.data_move(x0_ub, self.x[x_pre_offset + row_inner_offset], 0, 1, row_inner_burst, 0, 0)

            idx_num_cur_loop.set_as(idx_num_per_loop)
            with tik_inst.for_range(0, idx_num_cur_core // idx_num_per_loop) as idx_loop_i:
                compute_for_one_idx_loop(row_inner_offset, row_inner_burst, idx_loop_i)
            with tik_inst.if_scope(idx_num_cur_core % idx_num_per_loop != 0):
                idx_num_cur_loop.set_as(idx_num_cur_core % idx_num_per_loop)
                compute_for_one_idx_loop(row_inner_offset, row_inner_burst, idx_num_cur_core // idx_num_per_loop)

        with tik_inst.for_range(0, self.params_pre) as pre_i:
            x_pre_offset.set_as(pre_i * self.params_axis * self.params_row)
            y_pre_offset.set_as((pre_i * self.indices_num + idx_start) * self.params_row)

            with tik_inst.for_range(0, self.params_row // row_inner_loop_elem) as row_inner_loop_i:
                compute_for_incomplete_row(
                    row_inner_loop_i * row_inner_loop_elem, self.impl_mode_x0_ub_size // constant.BLOCK_SIZE
                )
            with tik_inst.if_scope(row_tail_size >= constant.BLOCK_SIZE):
                compute_for_incomplete_row(
                    floor_align(self.params_row, row_inner_loop_elem), row_tail_size // constant.BLOCK_SIZE
                )
            with tik_inst.if_scope(row_tail_size % constant.BLOCK_SIZE > 0):
                compute_for_incomplete_row(self.params_row - self.block_elem, 1)

    def compute_mode_14_lt32b(self, row_offset_tuple, cur_idx_tuple):
        """
        compute_mode_14 branch for cases which params row is less than 32B
        """
        tik_inst = self.tik_instance
        (x_pre_offset, y_pre_offset, y_offset) = row_offset_tuple
        (idx_start, idx_num_cur_core, indices_value) = cur_idx_tuple

        x0_ub = tik_inst.Tensor(self.params_dtype, (self.block_elem,), name="x0_ub", scope=tik.scope_ubuf)
        xi_ub = tik_inst.Tensor(self.params_dtype, (self.block_elem,), name="xi_ub", scope=tik.scope_ubuf)
        res_ub = tik_inst.Tensor(
            self.params_dtype, (self.impl_mode_res_ub_size // self.params_dsize,), name="res_ub", scope=tik.scope_ubuf
        )
        indices_ub = tik_inst.Tensor(
            self.indices_dtype,
            (self.impl_mode_x0_ub_size // self.indices_dsize,),
            name="indices_ub",
            scope=tik.scope_ubuf,
        )

        row_size = tik_inst.Scalar(
            dtype=self.tiling_dtype, name="row_size", init_value=self.params_row * self.params_dsize
        )
        self.row_num_once_ub.set_as(floor_align(self.impl_mode_res_ub_size, row_size * constant.BLOCK_SIZE) // row_size)

        res_tail_offset = tik_inst.Scalar(dtype=self.tiling_dtype, name="res_tail_offset")

        idx_num_per_loop = self.impl_mode_x0_ub_size // self.indices_dsize
        idx_num_cur_loop = tik_inst.Scalar(dtype=self.tiling_dtype, name="idx_num_cur_loop")
        idx_num_cur_batch = tik_inst.Scalar(dtype=self.tiling_dtype, name="idx_num_cur_batch")

        def compute_for_once_ub(idx_loop_i, idx_batch_i):
            """
            compute for once ub
            """
            with self.tik_instance.for_range(0, idx_num_cur_batch) as idx_i:
                indices_value.set_as(indices_ub[idx_batch_i * self.row_num_once_ub + idx_i])
                with self.tik_instance.if_scope(indices_value == 0):
                    with self.tik_instance.for_range(0, self.params_row) as elem_i:
                        res_ub[idx_i * self.params_row + elem_i].set_as(x0_ub[elem_i])
                with self.tik_instance.else_scope():
                    self.tik_instance.data_move(
                        xi_ub, self.x[x_pre_offset + indices_value * self.params_row], 0, 1, 1, 0, 0
                    )
                    with self.tik_instance.for_range(0, self.params_row) as elem_i:
                        res_ub[idx_i * self.params_row + elem_i].set_as(xi_ub[elem_i])
            y_offset.set_as(
                y_pre_offset + (idx_loop_i * idx_num_per_loop + idx_batch_i * self.row_num_once_ub) * self.params_row
            )
            self.tik_instance.data_move(
                self.y[y_offset], res_ub, 0, 1, idx_num_cur_batch * row_size // constant.BLOCK_SIZE, 0, 0
            )

        def compute_for_tail():
            """
            compute for tail 32B
            """
            with self.tik_instance.for_range(0, idx_num_cur_batch) as idx_i:
                res_tail_offset.set_as(self.block_elem * 2 - (idx_i + 1) * self.params_row)
                indices_value.set_as(indices_ub[idx_num_cur_loop - idx_i - 1])
                with self.tik_instance.if_scope(indices_value == 0):
                    with self.tik_instance.for_range(0, self.params_row) as elem_i:
                        res_ub[res_tail_offset + elem_i].set_as(x0_ub[elem_i])
                with self.tik_instance.else_scope():
                    self.tik_instance.data_move(
                        xi_ub, self.x[x_pre_offset + indices_value * self.params_row], 0, 1, 1, 0, 0
                    )
                    with self.tik_instance.for_range(0, self.params_row) as elem_i:
                        res_ub[res_tail_offset + elem_i].set_as(xi_ub[elem_i])
            y_offset.set_as(y_pre_offset + idx_num_cur_core * self.params_row - self.block_elem)
            self.tik_instance.data_move(self.y[y_offset], res_ub[self.block_elem], 0, 1, 1, 0, 0)

        def compute_for_one_idx_loop(idx_loop_i):
            """
            compute for one idx loop
            """
            tik_inst.data_move(
                indices_ub,
                self.indices[idx_start + idx_loop_i * idx_num_per_loop],
                0,
                1,
                ceil_value(idx_num_cur_loop * self.indices_dsize, constant.BLOCK_SIZE),
                0,
                0,
            )

            idx_num_cur_batch.set_as(self.row_num_once_ub)
            with tik_inst.for_range(0, idx_num_cur_loop // self.row_num_once_ub) as idx_batch_i:
                compute_for_once_ub(idx_loop_i, idx_batch_i)
            with tik_inst.if_scope(idx_num_cur_loop % self.row_num_once_ub * row_size >= constant.BLOCK_SIZE):
                idx_num_cur_batch.set_as(idx_num_cur_loop % self.row_num_once_ub)
                compute_for_once_ub(idx_loop_i, idx_num_cur_loop // self.row_num_once_ub)
            with tik_inst.if_scope(idx_num_cur_loop % self.row_num_once_ub * row_size % constant.BLOCK_SIZE != 0):
                idx_num_cur_batch.set_as(ceil_value(constant.BLOCK_SIZE, row_size))
                compute_for_tail()

        with tik_inst.for_range(0, self.params_pre) as pre_i:
            x_pre_offset.set_as(pre_i * self.params_axis * self.params_row)
            y_pre_offset.set_as((pre_i * self.indices_num + idx_start) * self.params_row)

            self.tik_instance.data_move(x0_ub, self.x[x_pre_offset], 0, 1, 1, 0, 0)

            idx_num_cur_loop.set_as(idx_num_per_loop)
            with self.tik_instance.for_range(0, idx_num_cur_core // idx_num_per_loop) as idx_loop_i:
                compute_for_one_idx_loop(idx_loop_i)
            with tik_inst.if_scope(idx_num_cur_core % idx_num_per_loop != 0):
                idx_num_cur_loop.set_as(idx_num_cur_core % idx_num_per_loop)
                compute_for_one_idx_loop(idx_num_cur_core // idx_num_per_loop)

    # 'pylint:disable=E1136
    # 'pylint: disable=too-many-arguments,huawei-too-many-arguments
    def params_row_less_than_32b(self, indices_loop_offset, batch_i, block_id, pre_i, is_last):
        """
        process for params row is less than 32 Bytes
        """
        tik_instance = self.tik_instance

        if is_last:
            loop_num = self.inner_loop_num_last
            tail_num = self.row_num_last_tail_ub
        else:
            loop_num = self.inner_loop_num
            tail_num = self.row_num_once_tail_ub
        indices_ub = self.buffers.get("indices_ub")
        params_ub = self.buffers.get("params_ub")
        res_ub = self.buffers.get("res_ub")
        burst_len_res = ceil_value(self.row_num_once_ub * self.params_row * self.params_dsize, constant.BLOCK_SIZE)
        output_offset_base = batch_i * self.params_pre * self.indices_row
        indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
        batch_base = block_id * self.params_batch_each_core
        if self.cached_types.get("cached_types_indices") == Constant.INDICES_CACHED_ALL and self.is_remaining == 0:
            indices_offset_base = (batch_i - batch_base) * self.indices_row
        else:
            indices_offset_base = 0
        if self.cached_types.get("cached_types_params") == Constant.PARAMS_CACHED_UB:
            params_batch_diff = (batch_i - batch_base) if self.is_remaining == 0 else 0
        else:
            params_batch_diff = batch_i
        params_offset_base = params_batch_diff * self.params_pre * self.params_axis + pre_i * self.params_axis

        with tik_instance.for_range(0, loop_num) as inner_loop_i:
            inner_indices_offset = indices_offset_base + inner_loop_i * self.row_num_once_ub
            output_offset = (
                output_offset_base
                + pre_i * self.indices_row
                + (indices_loop_offset + inner_loop_i * self.row_num_once_ub)
            ) * self.params_row

            with tik_instance.for_range(0, self.row_num_once_ub, thread_num=2) as row_i:
                indices_value.set_as(indices_ub[inner_indices_offset + row_i])
                params_offset = (params_offset_base + indices_value) * self.params_row
                res_ub_offset = row_i * self.params_row
                if self.cached_types.get("cached_types_params") == Constant.PARAMS_CACHED_UB:
                    with tik_instance.for_range(0, self.params_row) as i:
                        res_ub[res_ub_offset + i].set_as(params_ub[params_offset + i])
                else:
                    temp_ub_1 = tik_instance.Tensor(
                        self.params_dtype, (self.block_elem,), name="temp_ub_1", scope=tik.scope_ubuf
                    )
                    tik_instance.data_move(temp_ub_1[0], params_ub[params_offset], 0, 1, 1, 0, 0)
                    with tik_instance.for_range(0, self.params_row) as i:
                        res_ub[res_ub_offset + i].set_as(temp_ub_1[i])

            # copy result data from ub to gm
            tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res, 0, 0)

        with tik_instance.if_scope(tail_num > 0):
            burst_len_res = ceil_value(tail_num * self.params_row * self.params_dsize, constant.BLOCK_SIZE)
            inner_indices_offset = indices_offset_base + loop_num * self.row_num_once_ub
            output_offset = (
                output_offset_base + pre_i * self.indices_row + (indices_loop_offset + loop_num * self.row_num_once_ub)
            ) * self.params_row

            with tik_instance.for_range(0, tail_num, thread_num=2) as row_i:
                # compute gm offset of x
                indices_value.set_as(indices_ub[inner_indices_offset + row_i])
                params_offset = (params_offset_base + indices_value) * self.params_row
                res_ub_offset = row_i * self.params_row
                if self.cached_types.get("cached_types_params") == Constant.PARAMS_CACHED_UB:
                    with tik_instance.for_range(0, self.params_row) as i:
                        res_ub[res_ub_offset + i].set_as(params_ub[params_offset + i])
                else:
                    temp_ub_2 = tik_instance.Tensor(
                        self.params_dtype, (self.block_elem,), name="temp_ub_2", scope=tik.scope_ubuf
                    )
                    tik_instance.data_move(temp_ub_2[0], params_ub[params_offset], 0, 1, 1, 0, 0)
                    with tik_instance.for_range(0, self.params_row) as i:
                        res_ub[res_ub_offset + i].set_as(temp_ub_2[i])

            # move result data from ub to gm
            tail_elem = (tail_num * self.params_row) % self.block_elem
            with tik_instance.if_scope(tik.all(tail_elem != 0, burst_len_res > 1)):
                block_ub = tik_instance.Tensor(
                    self.params_dtype, (self.block_elem,), name="block_ub", scope=tik.scope_ubuf
                )
                with tik_instance.for_range(0, self.block_elem) as num_i:
                    block_ub[num_i].set_as(res_ub[tail_num * self.params_row - self.block_elem + num_i])

                tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res - 1, 0, 0)
                tik_instance.data_move(
                    self.y[output_offset + (tail_num * self.params_row - self.block_elem)], block_ub, 0, 1, 1, 0, 0
                )
            with tik_instance.else_scope():
                tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res, 0, 0)

    # 'pylint:disable=E1136
    # 'pylint: disable=too-many-arguments,huawei-too-many-arguments
    def params_row_more_than_32b(self, indices_loop_offset, batch_i, block_id, pre_i, is_last):
        """
        process for params row is more than 32 Bytes
        """
        tik_instance = self.tik_instance
        loop_num = self.indices_row_num_last if is_last else self.indices_row_num_once
        indices_ub = self.buffers.get("indices_ub")
        res_ub = self.buffers.get("res_ub")
        burst_len_row = ceil_value(self.params_row * self.params_dsize, constant.BLOCK_SIZE)
        indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
        batch_base = block_id * self.params_batch_each_core
        if self.cached_types.get("cached_types_indices") == Constant.INDICES_CACHED_ALL and self.is_remaining == 0:
            indices_offset_base = (batch_i - batch_base) * self.indices_row
        else:
            indices_offset_base = 0
        params_offset_base = batch_i * self.params_pre * self.params_axis + pre_i * self.params_axis
        output_offset_base = batch_i * self.params_pre * self.indices_row

        with tik_instance.for_range(0, loop_num - 1) as row_i:
            indices_value.set_as(indices_ub[indices_offset_base + row_i])
            params_offset = (params_offset_base + indices_value) * self.params_row
            output_offset = (
                output_offset_base + pre_i * self.indices_row + (indices_loop_offset + row_i)
            ) * self.params_row

            tik_instance.data_move(res_ub, self.x[params_offset], 0, 1, burst_len_row, 0, 0)
            tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_row, 0, 0)

        indices_value.set_as(indices_ub[indices_offset_base + loop_num - 1])
        params_offset = (params_offset_base + indices_value) * self.params_row
        output_offset = (
            output_offset_base + pre_i * self.indices_row + (indices_loop_offset + loop_num - 1)
        ) * self.params_row

        tik_instance.data_move(res_ub, self.x[params_offset], 0, 1, burst_len_row, 0, 0)

        # set tail 32B of result to block_ub
        block_ub = tik_instance.Tensor(self.params_dtype, (self.block_elem,), name="block_ub", scope=tik.scope_ubuf)
        with tik_instance.for_range(0, self.block_elem) as num_i:
            block_ub[num_i].set_as(res_ub[self.params_row - self.block_elem + num_i])
        tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_row - 1, 0, 0)
        tik_instance.data_move(self.y[output_offset + (self.params_row - self.block_elem)], block_ub, 0, 1, 1, 0, 0)

    # 'pylint: disable=too-many-arguments,huawei-too-many-arguments
    def get_optimization_mode(
        self,
        idx_batch_i,
        mode,
        simpling_step,
        idx_num_cur_batch,
        cache_max_n_num,
        indices_simpling_ub,
        indices_index_float_ub,
    ):
        """
        compute for optimization mode simpling or cache n number
        """
        if self.indices_dtype == Constant.DTYPE_INT32 and tbe_platform_adapter.api_check_support(
            "tbe.dsl.vexp", Constant.DTYPE_FP32
        ):
            # calc Simplingstep
            simpling_step.set_as(idx_num_cur_batch // Constant.TOPN_CACHE_ACT_SIMPLING_NUM)
            with self.tik_instance.if_scope(simpling_step != 0):
                with self.tik_instance.for_range(0, Constant.TOPN_CACHE_ACT_SIMPLING_NUM) as idx_simpling_i:
                    indices_simpling_ub[idx_simpling_i].set_as(
                        indices_index_float_ub[idx_batch_i * self.row_num_once_ub + idx_simpling_i * simpling_step]
                    )

                max_dup_data = self.tik_instance.Tensor(
                    Constant.DTYPE_FP32,
                    (Constant.CACHE_ACT_SIMPLING_BUFF_NUM,),
                    name="cache_simpling_data",
                    scope=tik.scope_ubuf,
                )
                sel_gt_ub = self.tik_instance.Tensor(
                    Constant.DTYPE_UINT32,
                    (Constant.CACHE_ACT_SIMPLING_BUFF_NUM,),
                    name="sel_gt_ub",
                    scope=tik.scope_ubuf,
                )
                cache_max_n_num_fp = self.tik_instance.Scalar(dtype=Constant.DTYPE_FP32, name="cache_max_n_num_fp")
                cache_max_n_num_fp.set_as(cache_max_n_num)
                sel_gt_ub[0].set_as(0)
                self.tik_instance.vec_dup(Constant.CACHE_ACT_SIMPLING_BUFF_NUM, max_dup_data, cache_max_n_num_fp, 1, 8)
                self.tik_instance.vec_cmpv_gt(sel_gt_ub, indices_simpling_ub, max_dup_data, 1, 8, 8)
                sel_gt_flag = self.tik_instance.Scalar(
                    dtype=Constant.DTYPE_UINT32, name="sel_gt_flag", init_value=sel_gt_ub[0]
                )

                with self.tik_instance.if_scope(sel_gt_flag == 0):
                    mode.set_as(Constant.INPUT_LAST_AXIS_ALIGN_HP_OPT_CACHE_N_NUM)
                with self.tik_instance.else_scope():
                    mode.set_as(Constant.INPUT_LAST_AXIS_ALIGN_HP_OPT_CACHE_SIMPLING)
            with self.tik_instance.else_scope():
                mode.set_as(Constant.INPUT_LAST_AXIS_ALIGN_HP_OPT_CACHE_N_NUM)
        else:
            mode.set_as(Constant.INPUT_LAST_AXIS_ALIGN_HP_OPT_CACHE_N_NUM)

    # 'pylint: disable=too-many-arguments,huawei-too-many-arguments
    def compute_for_simpling_cache_data(
        self,
        idx_batch_i,
        simpling_step,
        indices_ub,
        cached_ub,
        row_size_align,
        params_block_num,
        idx_num_cur_batch,
        indices_simpling_ub,
        indices_index_float_ub,
        indices_temp_float_ub,
        indices_temp_int_ub,
    ):
        """
        compute for cache data
        """
        if tbe_platform_adapter.api_check_support("tbe.dsl.vexp", Constant.DTYPE_FP32):
            indices_value_dup_ub = self.tik_instance.Tensor(
                Constant.DTYPE_FP32, (Constant.CACHE_ACT_SIMPLING_BUFF_NUM,), name="indice_dup_ub", scope=tik.scope_ubuf
            )
            idx_value_dup_ub = self.tik_instance.Tensor(
                Constant.DTYPE_FP32, (Constant.CACHE_ACT_SIMPLING_BUFF_NUM,), name="indice_dup_ub", scope=tik.scope_ubuf
            )
            is_eq_ub = self.tik_instance.Tensor(
                Constant.DTYPE_UINT32, ((self.row_num_once_ub + 7) // 8,), name="is_eq_ub", scope=tik.scope_ubuf
            )
            zero_dup_ub = self.tik_instance.Tensor(
                Constant.DTYPE_FP32, (Constant.CACHE_ACT_SIMPLING_BUFF_NUM,), name="zero_dup_ub", scope=tik.scope_ubuf
            )
            invalid_dup_ub = self.tik_instance.Tensor(
                Constant.DTYPE_FP32,
                (Constant.CACHE_ACT_SIMPLING_BUFF_NUM,),
                name="invalid_dup_ub",
                scope=tik.scope_ubuf,
            )
            mask = self.tik_instance.Scalar(dtype=Constant.DTYPE_INT32, name="mask", init_value=constant.MASK64)

            self.tik_instance.vec_dup(mask, zero_dup_ub, 0.0, 1, 8)
            self.tik_instance.vec_dup(mask, invalid_dup_ub, Constant.TOPN_CACHE_INVALID_PARAMS_INDEX, 1, 8)
            indices_value_fp32 = self.tik_instance.Scalar(dtype=Constant.DTYPE_FP32, name="indices_value_fp32")
            indices_index = self.tik_instance.Scalar(dtype=Constant.DTYPE_INT32, name="indices_index")

            with self.tik_instance.if_scope(simpling_step != 0):
                # get the simpling indice data
                with self.tik_instance.for_range(0, Constant.TOPN_CACHE_ACT_SIMPLING_NUM) as idx_simpling_i:
                    indices_index.set_as(
                        indices_ub[idx_batch_i * self.row_num_once_ub + idx_simpling_i * simpling_step]
                    )
                    self.tik_instance.data_move(
                        cached_ub[(idx_simpling_i + 1) * row_size_align],
                        self.x[indices_index * self.params_row],
                        0,
                        1,
                        params_block_num,
                        0,
                        0,
                    )

                repeat_times = self.tik_instance.Scalar(
                    dtype=Constant.DTYPE_INT32, name="repeat_times", init_value=(idx_num_cur_batch + mask - 1) // mask
                )
                idx_num_cur_batch_tail = self.tik_instance.Scalar(
                    dtype=Constant.DTYPE_INT32, name="idx_num_cur_batch_tail"
                )
                idx_num_cur_batch_tail.set_as(idx_num_cur_batch - idx_num_cur_batch // mask * mask)
                if tbe_platform.api_check_support("tik.data_move_pad", self.params_dtype):
                    with self.tik_instance.for_range(0, Constant.TOPN_CACHE_ACT_SIMPLING_NUM) as idx_simpling_i:
                        indices_value_fp32.set_as(indices_simpling_ub[idx_simpling_i])
                        with self.tik_instance.if_scope(indices_value_fp32 != Constant.TOPN_CACHE_INVALID_PARAMS_INDEX):
                            self.tik_instance.vec_dup(mask, indices_value_dup_ub, indices_value_fp32, 1, 8)
                            self.tik_instance.vec_cmpv_eq(is_eq_ub, indices_value_dup_ub, indices_simpling_ub, 1, 8, 8)
                            self.tik_instance.vec_sel(
                                Constant.TOPN_CACHE_ACT_SIMPLING_NUM,
                                0,
                                indices_simpling_ub,
                                is_eq_ub,
                                invalid_dup_ub,
                                indices_simpling_ub,
                                1,
                                8,
                                8,
                                8,
                            )
                            idx_value = self.tik_instance.Scalar(
                                dtype=Constant.DTYPE_FP32, name="idx_value", init_value=-(idx_simpling_i + 1)
                            )
                            self.tik_instance.vec_dup(mask, idx_value_dup_ub, idx_value, 1, 8)

                            with self.tik_instance.if_scope(repeat_times - 1 != 0):
                                self.tik_instance.vec_cmpv_eq(
                                    is_eq_ub,
                                    indices_value_dup_ub,
                                    indices_index_float_ub[idx_batch_i * self.row_num_once_ub],
                                    repeat_times - 1,
                                    0,
                                    8,
                                )
                                self.tik_instance.vec_sel(
                                    mask,
                                    2,
                                    indices_index_float_ub[idx_batch_i * self.row_num_once_ub],
                                    is_eq_ub,
                                    idx_value_dup_ub,
                                    indices_index_float_ub[idx_batch_i * self.row_num_once_ub],
                                    repeat_times - 1,
                                    8,
                                    0,
                                    8,
                                )

                            with self.tik_instance.if_scope(idx_num_cur_batch_tail != 0):
                                self.tik_instance.vec_cmpv_eq(
                                    is_eq_ub,
                                    indices_value_dup_ub,
                                    indices_index_float_ub[
                                        idx_batch_i * self.row_num_once_ub + mask * (repeat_times - 1)
                                    ],
                                    1,
                                    8,
                                    8,
                                )
                                self.tik_instance.vec_sel(
                                    idx_num_cur_batch_tail,
                                    2,
                                    indices_index_float_ub[
                                        idx_batch_i * self.row_num_once_ub + mask * (repeat_times - 1)
                                    ],
                                    is_eq_ub,
                                    idx_value_dup_ub,
                                    indices_index_float_ub[
                                        idx_batch_i * self.row_num_once_ub + mask * (repeat_times - 1)
                                    ],
                                    1,
                                    8,
                                    8,
                                    8,
                                )
                    self.tik_instance.data_move(
                        indices_temp_float_ub,
                        indices_index_float_ub[idx_batch_i * self.row_num_once_ub],
                        0,
                        1,
                        ceil_value(idx_num_cur_batch * self.indices_dsize, constant.BLOCK_SIZE),
                        0,
                        0,
                    )
                    # cmp
                    self.tik_instance.vec_cmpv_gt(is_eq_ub, indices_temp_float_ub[0], zero_dup_ub, repeat_times, 8, 0)
                    self.tik_instance.vec_sel(
                        mask,
                        2,
                        indices_temp_float_ub[0],
                        is_eq_ub,
                        zero_dup_ub,
                        indices_temp_float_ub[0],
                        repeat_times,
                        8,
                        0,
                        8,
                    )
                    self.tik_instance.vec_abs(mask, indices_temp_float_ub, indices_temp_float_ub, repeat_times, 8, 8)
                    self.tik_instance.h_cast(indices_temp_int_ub, indices_temp_float_ub, "round")
                else:
                    with self.tik_instance.for_range(0, Constant.TOPN_CACHE_ACT_SIMPLING_NUM) as idx_simpling_i:
                        indices_value_fp32.set_as(indices_simpling_ub[idx_simpling_i])
                        with self.tik_instance.if_scope(indices_value_fp32 != Constant.TOPN_CACHE_INVALID_PARAMS_INDEX):
                            self.tik_instance.vec_dup(mask, indices_value_dup_ub, indices_value_fp32, 1, 8)
                            self.tik_instance.vec_cmpv_eq(is_eq_ub, indices_value_dup_ub, indices_simpling_ub, 1, 8, 8)
                            self.tik_instance.vec_sel(
                                Constant.TOPN_CACHE_ACT_SIMPLING_NUM,
                                0,
                                indices_simpling_ub,
                                is_eq_ub,
                                invalid_dup_ub,
                                indices_simpling_ub,
                                1,
                                8,
                                8,
                                8,
                            )
                            idx_value = self.tik_instance.Scalar(
                                dtype=Constant.DTYPE_FP32, name="idx_value", init_value=-(idx_simpling_i + 1)
                            )
                            self.tik_instance.vec_dup(mask, idx_value_dup_ub, idx_value, 1, 8)
                            with self.tik_instance.for_range(0, repeat_times) as idx_repeat:
                                self.tik_instance.vec_cmpv_eq(
                                    is_eq_ub,
                                    indices_value_dup_ub,
                                    indices_index_float_ub[idx_batch_i * self.row_num_once_ub + mask * idx_repeat],
                                    1,
                                    8,
                                    8,
                                )
                                with self.tik_instance.if_scope(
                                    tik.all(idx_num_cur_batch_tail != 0, idx_repeat == repeat_times - 1)
                                ):
                                    self.tik_instance.vec_sel(
                                        idx_num_cur_batch_tail,
                                        0,
                                        indices_index_float_ub[idx_batch_i * self.row_num_once_ub + mask * idx_repeat],
                                        is_eq_ub,
                                        idx_value_dup_ub,
                                        indices_index_float_ub[idx_batch_i * self.row_num_once_ub + mask * idx_repeat],
                                        1,
                                        8,
                                        8,
                                        8,
                                    )
                                with self.tik_instance.else_scope():
                                    self.tik_instance.vec_sel(
                                        mask,
                                        0,
                                        indices_index_float_ub[idx_batch_i * self.row_num_once_ub + mask * idx_repeat],
                                        is_eq_ub,
                                        idx_value_dup_ub,
                                        indices_index_float_ub[idx_batch_i * self.row_num_once_ub + mask * idx_repeat],
                                        1,
                                        8,
                                        8,
                                        8,
                                    )
                    self.tik_instance.data_move(
                        indices_temp_float_ub,
                        indices_index_float_ub[idx_batch_i * self.row_num_once_ub],
                        0,
                        1,
                        ceil_value(idx_num_cur_batch * self.indices_dsize, constant.BLOCK_SIZE),
                        0,
                        0,
                    )
                    # cmp
                    with self.tik_instance.for_range(0, repeat_times) as idx_repeat:
                        self.tik_instance.vec_cmpv_gt(
                            is_eq_ub, indices_temp_float_ub[mask * idx_repeat], zero_dup_ub, 1, 8, 8
                        )
                        self.tik_instance.vec_sel(
                            mask,
                            0,
                            indices_temp_float_ub[mask * idx_repeat],
                            is_eq_ub,
                            zero_dup_ub,
                            indices_temp_float_ub[mask * idx_repeat],
                            1,
                            8,
                            8,
                            8,
                        )
                    self.tik_instance.vec_abs(mask, indices_temp_float_ub, indices_temp_float_ub, repeat_times, 8, 8)
                    self.tik_instance.h_cast(indices_temp_int_ub, indices_temp_float_ub, "round")

    def trans(self, src_ub, dst_ub, trans_mode, length):
        """
        transpose for res data
        """
        loop_times = ((length * Constant.FP16_ALIGN_NUM) // Constant.VECTOR_BLOCK_SIZE) % Constant.MAX_REPEAT_TIME
        if trans_mode == Constant.LINE_TO_COL:
            src_list = [src_ub[Constant.FP16_ALIGN_NUM * i * loop_times] for i in range(Constant.FP16_ALIGN_NUM)]
            dst_list = [dst_ub[Constant.FP16_ALIGN_NUM * i] for i in range(Constant.FP16_ALIGN_NUM)]
            with self.tik_instance.if_scope(loop_times == 1):
                self.tik_instance.vnchwconv(False, False, dst_list, src_list, loop_times, 0, 0)
            with self.tik_instance.else_scope():
                self.tik_instance.vnchwconv(False, False, dst_list, src_list, loop_times, Constant.FP16_ALIGN_NUM, 1)
        elif trans_mode == Constant.COL_TO_LINE:
            src_list = [src_ub[Constant.FP16_ALIGN_NUM * i] for i in range(Constant.FP16_ALIGN_NUM)]
            dst_list = [dst_ub[Constant.FP16_ALIGN_NUM * i * loop_times] for i in range(Constant.FP16_ALIGN_NUM)]
            with self.tik_instance.if_scope(loop_times == 1):
                self.tik_instance.vnchwconv(False, False, dst_list, src_list, loop_times, 0, 0)
            with self.tik_instance.else_scope():
                self.tik_instance.vnchwconv(False, False, dst_list, src_list, loop_times, 1, Constant.FP16_ALIGN_NUM)

    # 'pylint: disable=too-many-arguments,huawei-too-many-arguments
    def transdata_and_move_out(
        self,
        res_ub,
        idx_loop_i,
        idx_batch_i,
        idx_num_cur_batch,
        cached_ub,
        row_size_align,
        row_size,
        y_offset,
        y_pre_offset,
        idx_num_per_loop,
        ub_xi,
    ):
        """
        transpose for res data and data move to GM
        """
        one_block_num = self.tik_instance.Scalar(
            dtype=Constant.DTYPE_INT32, name="one_block_num", init_value=Constant.ONE_BLOCK_FP32_NUM
        )
        trans_type_to_fp16 = self.tik_instance.Scalar(
            dtype=Constant.DTYPE_INT32, name="trans_type_to_fp16", init_value=2
        )
        one_line_num = self.tik_instance.Scalar(dtype=Constant.DTYPE_INT32, name="one_line_num", init_value=8)
        one_repeat_num = self.tik_instance.Scalar(dtype=Constant.DTYPE_INT32, name="one_repeat_num", init_value=128)
        if self.params_dsize == 2:
            one_block_num.set_as(Constant.ONE_BLOCK_FP16_NUM)
            trans_type_to_fp16.set_as(1)
            one_line_num.set_as(16)
            one_repeat_num.set_as(256)
        elif self.params_dsize == 8:
            one_block_num.set_as(Constant.ONE_BLOCK_INT64_NUM)
            trans_type_to_fp16.set_as(4)
            one_line_num.set_as(4)
            one_repeat_num.set_as(64)

        with self.tik_instance.if_scope(self.params_row < 64):
            loop_times = idx_num_cur_batch // one_repeat_num
            mod = idx_num_cur_batch % one_repeat_num
            with self.tik_instance.if_scope(loop_times != 0):
                src_fp16 = res_ub.reinterpret_cast_to("float16")
                dst_fp16 = cached_ub.reinterpret_cast_to("float16")
                self.trans(
                    src_fp16,
                    dst_fp16,
                    Constant.LINE_TO_COL,
                    one_line_num * trans_type_to_fp16 * loop_times * row_size_align,
                )
                block_num = ceil_value(row_size * Constant.FP16_ALIGN_NUM * 2 * trans_type_to_fp16, constant.BLOCK_SIZE)
                src_stride = ceil_value(
                    (row_size_align - row_size) * Constant.FP16_ALIGN_NUM * 2 * trans_type_to_fp16, constant.BLOCK_SIZE
                )
                self.tik_instance.data_move(dst_fp16, dst_fp16, 0, one_line_num * loop_times, block_num, src_stride, 0)
                self.trans(
                    dst_fp16, src_fp16, Constant.COL_TO_LINE, one_line_num * trans_type_to_fp16 * loop_times * row_size
                )

                total_num = one_repeat_num * loop_times * row_size
                y_offset.set_as(
                    y_pre_offset
                    + (idx_loop_i * idx_num_per_loop + idx_batch_i * self.row_num_once_ub) * self.params_row
                )
                self.tik_instance.data_move(self.y[y_offset], res_ub[0], 0, 1, total_num // one_block_num, 0, 0)
                base_offset = total_num - one_block_num
                with self.tik_instance.if_scope(total_num % one_block_num != 0):
                    with self.tik_instance.for_range(0, one_block_num) as elem_i:
                        ub_xi[elem_i].set_as(res_ub[base_offset + elem_i])
                    self.tik_instance.data_move(self.y[y_offset + base_offset], ub_xi[0], 0, 1, 1, 0, 0)

            with self.tik_instance.if_scope(mod != 0):
                with self.tik_instance.if_scope(loop_times != 0):
                    self.tik_instance.data_move(
                        res_ub,
                        res_ub[one_repeat_num * loop_times * row_size_align],
                        0,
                        1,
                        ceil_value(row_size_align * self.params_dsize * mod, constant.BLOCK_SIZE),
                        0,
                        0,
                    )
                src_fp16 = res_ub.reinterpret_cast_to("float16")
                dst_fp16 = cached_ub.reinterpret_cast_to("float16")
                self.trans(src_fp16, dst_fp16, Constant.LINE_TO_COL, one_line_num * trans_type_to_fp16 * row_size_align)
                block_num = ceil_value(row_size * Constant.FP16_ALIGN_NUM * 2 * trans_type_to_fp16, constant.BLOCK_SIZE)
                src_stride = ceil_value(
                    (row_size_align - row_size) * Constant.FP16_ALIGN_NUM * 2 * trans_type_to_fp16, constant.BLOCK_SIZE
                )
                self.tik_instance.data_move(dst_fp16, dst_fp16, 0, one_line_num, block_num, src_stride, 0)
                self.trans(dst_fp16, src_fp16, Constant.COL_TO_LINE, one_line_num * trans_type_to_fp16 * row_size)

                y_offset.set_as(
                    y_pre_offset
                    + (idx_loop_i * idx_num_per_loop + idx_batch_i * self.row_num_once_ub + one_repeat_num * loop_times)
                    * self.params_row
                )
                total_num = mod * row_size
                with self.tik_instance.if_scope(total_num >= one_block_num):
                    self.tik_instance.data_move(self.y[y_offset], res_ub[0], 0, 1, total_num // one_block_num, 0, 0)
                    base_offset = total_num - one_block_num
                    with self.tik_instance.if_scope(total_num % one_block_num != 0):
                        with self.tik_instance.for_range(0, one_block_num) as elem_i:
                            ub_xi[elem_i].set_as(res_ub[base_offset + elem_i])
                        self.tik_instance.data_move(self.y[y_offset + base_offset], ub_xi[0], 0, 1, 1, 0, 0)
                with self.tik_instance.else_scope():
                    y_offset.set_as(y_offset - one_block_num + total_num)
                    self.tik_instance.data_move(ub_xi, self.y[y_offset], 0, 1, 1, 0, 0)
                    with self.tik_instance.for_range(0, total_num) as elem_i:
                        ub_xi[one_block_num - 1 - elem_i].set_as(res_ub[total_num - 1 - elem_i])
                    self.tik_instance.data_move(self.y[y_offset], ub_xi[0], 0, 1, 1, 0, 0)

        with self.tik_instance.else_scope():
            one_line_num.set_as(self.row_num_once_ub // Constant.FP16_ALIGN_NUM)
            src_fp16 = res_ub.reinterpret_cast_to("float16")
            dst_fp16 = cached_ub.reinterpret_cast_to("float16")
            self.trans(src_fp16, dst_fp16, Constant.LINE_TO_COL, one_line_num * trans_type_to_fp16 * row_size_align)
            block_num = ceil_value(row_size * Constant.FP16_ALIGN_NUM * 2 * trans_type_to_fp16, constant.BLOCK_SIZE)
            src_stride = ceil_value(
                (row_size_align - row_size) * Constant.FP16_ALIGN_NUM * 2 * trans_type_to_fp16, constant.BLOCK_SIZE
            )
            self.tik_instance.data_move(dst_fp16, dst_fp16, 0, one_line_num, block_num, src_stride, 0)
            self.trans(dst_fp16, src_fp16, Constant.COL_TO_LINE, one_line_num * trans_type_to_fp16 * row_size_align)

            y_offset.set_as(
                y_pre_offset + (idx_loop_i * idx_num_per_loop + idx_batch_i * self.row_num_once_ub) * self.params_row
            )
            out_loop = idx_num_cur_batch // one_line_num
            mod_num = idx_num_cur_batch % one_line_num
            with self.tik_instance.if_scope(mod_num == 0):
                with self.tik_instance.for_range(0, out_loop) as line_idx:
                    with self.tik_instance.if_scope(line_idx != out_loop - 1):
                        self.tik_instance.data_move(
                            self.y[y_offset + line_idx * one_line_num * row_size],
                            res_ub[line_idx * one_line_num * row_size_align],
                            0,
                            1,
                            ceil_value(one_line_num * row_size, one_block_num),
                            0,
                            0,
                        )
                    with self.tik_instance.else_scope():
                        self.tik_instance.data_move(
                            self.y[y_offset + line_idx * one_line_num * row_size],
                            res_ub[line_idx * one_line_num * row_size_align],
                            0,
                            1,
                            one_line_num * row_size // one_block_num,
                            0,
                            0,
                        )
                        last_data_offset = one_line_num * row_size - one_block_num
                        with self.tik_instance.for_range(0, one_block_num) as elem_i:
                            ub_xi[elem_i].set_as(
                                res_ub[line_idx * one_line_num * row_size_align + last_data_offset + elem_i]
                            )
                        self.tik_instance.data_move(
                            self.y[y_offset + line_idx * one_line_num * row_size + last_data_offset],
                            ub_xi,
                            0,
                            1,
                            1,
                            0,
                            0,
                        )
            with self.tik_instance.else_scope():
                with self.tik_instance.for_range(0, out_loop) as line_idx:
                    self.tik_instance.data_move(
                        self.y[y_offset + line_idx * one_line_num * row_size],
                        res_ub[line_idx * one_line_num * row_size_align],
                        0,
                        1,
                        ceil_value(one_line_num * row_size, one_block_num),
                        0,
                        0,
                    )
                with self.tik_instance.if_scope(mod_num * row_size >= one_block_num):
                    self.tik_instance.data_move(
                        self.y[y_offset + out_loop * one_line_num * row_size],
                        res_ub[out_loop * one_line_num * row_size_align],
                        0,
                        1,
                        mod_num * row_size // one_block_num,
                        0,
                        0,
                    )
                    last_data_offset = mod_num * row_size - one_block_num
                    with self.tik_instance.for_range(0, one_block_num) as elem_i:
                        ub_xi[elem_i].set_as(
                            res_ub[out_loop * one_line_num * row_size_align + last_data_offset + elem_i]
                        )
                    self.tik_instance.data_move(
                        self.y[y_offset + out_loop * one_line_num * row_size + last_data_offset], ub_xi, 0, 1, 1, 0, 0
                    )
                with self.tik_instance.else_scope():
                    self.tik_instance.data_move(
                        ub_xi, self.y[y_offset + out_loop * one_line_num * row_size - one_block_num], 0, 1, 1, 0, 0
                    )
                    last_data_offset = mod_num * row_size
                    with self.tik_instance.for_range(0, last_data_offset) as elem_i:
                        ub_xi[one_block_num - last_data_offset + elem_i].set_as(
                            res_ub[out_loop * one_line_num * row_size_align + elem_i]
                        )
                    self.tik_instance.data_move(
                        self.y[y_offset + out_loop * one_line_num * row_size - one_block_num], ub_xi, 0, 1, 1, 0, 0
                    )

    def compute_mode_15(self, block_id):
        """
        compute for tiling mode 15

        Parameters
        ----------
        block_id: id of ai core

        Returns
        -------
        None
        """
        tik_inst = self.tik_instance

        idx_start = tik_inst.Scalar(
            dtype=self.tiling_dtype, name="idx_start", init_value=block_id * self.indices_num_each_core
        )
        idx_num_cur_core = tik_inst.Scalar(
            dtype=self.tiling_dtype, name="idx_num_cur_core", init_value=self.indices_num_each_core
        )
        with tik_inst.if_scope(block_id >= self.tail_process_core):
            idx_start.set_as(block_id * self.indices_num_remaining + self.tail_process_core)
            idx_num_cur_core.set_as(self.indices_num_remaining)
        indices_value = tik_inst.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
        CurIdxTuple = collections.namedtuple("CurIdxTuple", ["idx_start", "idx_num_cur_core", "indices_value"])
        cur_idx_tuple = CurIdxTuple(idx_start, idx_num_cur_core, indices_value)
        x_pre_offset = tik_inst.Scalar(dtype=self.tiling_dtype, name="x_pre_offset")
        y_pre_offset = tik_inst.Scalar(dtype=self.tiling_dtype, name="y_pre_offset")
        y_offset = tik_inst.Scalar(dtype=self.tiling_dtype, name="y_offset")
        RowOffsetTuple = collections.namedtuple("RowOffsetTuple", ["x_pre_offset", "y_pre_offset", "y_offset"])
        row_offset_tuple = RowOffsetTuple(x_pre_offset, y_pre_offset, y_offset)

        with tik_inst.new_stmt_scope():
            self.compute_mode_15_aixs_eq_0(row_offset_tuple, cur_idx_tuple)

    def compute_mode_15_aixs_eq_0(self, row_offset_tuple, cur_idx_tuple):
        """
        compute_mode_15 branch for cases which aixs is 0
        """
        tik_inst = self.tik_instance
        (x_pre_offset, y_pre_offset, y_offset) = row_offset_tuple
        (idx_start, idx_num_cur_core, indices_value) = cur_idx_tuple

        ub_tensor_size = floor_align((self.ub_size - Constant.IMPL_MODE_INNER_LOOP_UB_8K) // 6, constant.BLOCK_SIZE)

        res_ub = tik_inst.Tensor(
            self.params_dtype, (ub_tensor_size // self.params_dsize,), name="res_ub", scope=tik.scope_ubuf
        )
        indices_ub = tik_inst.Tensor(
            self.indices_dtype, (ub_tensor_size // self.indices_dsize,), name="indices_ub", scope=tik.scope_ubuf
        )
        cached_ub = tik_inst.Tensor(
            self.params_dtype, (ub_tensor_size // self.params_dsize,), name="cached_ub", scope=tik.scope_ubuf
        )
        cached_n_num_ub = tik_inst.Tensor(
            self.params_dtype, (ub_tensor_size // self.params_dsize,), name="cached_n_num_ub", scope=tik.scope_ubuf
        )
        indices_index_float_ub = tik_inst.Tensor(
            Constant.DTYPE_FP32,
            (ub_tensor_size // self.indices_dsize,),
            name="indices_index_float_ub",
            scope=tik.scope_ubuf,
        )
        indices_simpling_ub = tik_inst.Tensor(
            Constant.DTYPE_FP32,
            (Constant.CACHE_ACT_SIMPLING_BUFF_NUM,),
            name="indices_simpling_ub",
            scope=tik.scope_ubuf,
        )
        row_size = tik_inst.Scalar(
            dtype=self.tiling_dtype, name="row_size", init_value=self.params_row * self.params_dsize
        )
        params_block_num = tik_inst.Scalar(dtype=Constant.DTYPE_INT32, name="params_block_num")
        params_block_num.set_as(ceil_value(self.params_row * self.params_dsize, constant.BLOCK_SIZE))

        simpling_step = tik_inst.Scalar(dtype=Constant.DTYPE_INT32, name="simpling_step")
        cache_max_n_num = tik_inst.Scalar(dtype=Constant.DTYPE_INT32, name="cache_max_n_num")
        cache_max_n_num.set_as(ub_tensor_size // (self.params_row * self.params_dsize))
        is_full_cache = tik_inst.Scalar(dtype=Constant.DTYPE_INT32, name="is_full_cache", init_value=0)
        with self.tik_instance.if_scope(cache_max_n_num > self.params_axis):
            cache_max_n_num.set_as(self.params_axis)
            is_full_cache.set_as(1)

        tik_inst.data_move(
            cached_n_num_ub,
            self.x,
            0,
            1,
            ceil_value(cache_max_n_num * self.params_row * self.params_dsize, constant.BLOCK_SIZE),
            0,
            0,
        )

        self.row_num_once_ub.set_as(floor_align(ub_tensor_size, row_size * constant.BLOCK_SIZE) // row_size)

        indices_temp_int_ub = tik_inst.Tensor(
            Constant.DTYPE_INT32, (self.row_num_once_ub,), name="indices_temp_int_ub", scope=tik.scope_ubuf
        )
        indices_temp_float_ub = tik_inst.Tensor(
            Constant.DTYPE_FP32, (self.row_num_once_ub,), name="indices_temp_float_ub", scope=tik.scope_ubuf
        )
        idx_num_per_loop = ub_tensor_size // self.indices_dsize
        idx_num_cur_loop = tik_inst.Scalar(dtype=self.tiling_dtype, name="idx_num_cur_loop")
        idx_num_cur_batch = tik_inst.Scalar(dtype=self.tiling_dtype, name="idx_num_cur_batch")

        def compute_for_once_ub(idx_loop_i, idx_batch_i):
            """
            compute for once ub
            """
            opt_mode = tik_inst.Scalar(dtype=Constant.DTYPE_INT32, name="cache_max_n_num")
            indice_base_offset = tik_inst.Scalar(
                dtype=Constant.DTYPE_INT32, name="indice_base_offset", init_value=idx_batch_i * self.row_num_once_ub
            )

            self.get_optimization_mode(
                idx_batch_i,
                opt_mode,
                simpling_step,
                idx_num_cur_batch,
                cache_max_n_num,
                indices_simpling_ub,
                indices_index_float_ub,
            )
            with self.tik_instance.if_scope(opt_mode == Constant.INPUT_LAST_AXIS_ALIGN_HP_OPT_CACHE_SIMPLING):
                self.compute_for_simpling_cache_data(
                    idx_batch_i,
                    simpling_step,
                    indices_ub,
                    cached_ub,
                    self.params_row,
                    params_block_num,
                    idx_num_cur_batch,
                    indices_simpling_ub,
                    indices_index_float_ub,
                    indices_temp_float_ub,
                    indices_temp_int_ub,
                )
                with self.tik_instance.for_range(0, idx_num_cur_batch) as idx_i:
                    indices_value.set_as(indices_temp_int_ub[idx_i])
                    with self.tik_instance.if_scope(indices_value > 0):
                        tik_inst.data_move(
                            res_ub[idx_i * self.params_row],
                            cached_ub[indices_value * self.params_row],
                            0,
                            1,
                            params_block_num,
                            0,
                            0,
                        )
                    with self.tik_instance.else_scope():
                        indices_value.set_as(indices_ub[indice_base_offset + idx_i])
                        tik_inst.data_move(
                            res_ub[idx_i * self.params_row],
                            self.x[x_pre_offset + indices_value * self.params_row],
                            0,
                            1,
                            params_block_num,
                            0,
                            0,
                        )

            with self.tik_instance.else_scope():
                with tik_inst.if_scope(is_full_cache == 1):
                    unrool_num = 8
                    index_array = self.tik_instance.ScalarArray(
                        dtype=self.tiling_dtype, length=unrool_num, name="index_array"
                    )
                    with self.tik_instance.for_range(0, idx_num_cur_batch // unrool_num) as idx_i:
                        with self.tik_instance.for_range(0, unrool_num) as unroll_idx:
                            index_array[unroll_idx].set_as(
                                indices_ub[indice_base_offset + idx_i * unrool_num + unroll_idx]
                            )

                        with self.tik_instance.for_range(0, unrool_num) as unroll_idx:
                            tik_inst.data_move(
                                res_ub[(idx_i * unrool_num + unroll_idx) * row_size],
                                cached_n_num_ub[index_array[unroll_idx] * row_size],
                                0,
                                1,
                                params_block_num,
                                0,
                                0,
                            )
                    with self.tik_instance.if_scope(idx_num_cur_batch % unrool_num > 0):
                        unrool_offset = idx_num_cur_batch // unrool_num * unrool_num
                        tail_num = idx_num_cur_batch % unrool_num
                        with self.tik_instance.for_range(0, tail_num) as unroll_idx:
                            index_array[unroll_idx].set_as(indices_ub[indice_base_offset + unrool_offset + unroll_idx])

                        with self.tik_instance.for_range(0, tail_num) as unroll_idx:
                            tik_inst.data_move(
                                res_ub[(unrool_offset + unroll_idx) * row_size],
                                cached_n_num_ub[index_array[unroll_idx] * row_size],
                                0,
                                1,
                                params_block_num,
                                0,
                                0,
                            )

                with self.tik_instance.else_scope():
                    with self.tik_instance.for_range(0, idx_num_cur_batch) as idx_i:
                        indices_value.set_as(indices_ub[indice_base_offset + idx_i])
                        with self.tik_instance.if_scope(indices_value < cache_max_n_num):
                            tik_inst.data_move(
                                res_ub[idx_i * self.params_row],
                                cached_n_num_ub[indices_value * self.params_row],
                                0,
                                1,
                                params_block_num,
                                0,
                                0,
                            )
                        with self.tik_instance.else_scope():
                            tik_inst.data_move(
                                res_ub[idx_i * self.params_row],
                                self.x[x_pre_offset + indices_value * self.params_row],
                                0,
                                1,
                                params_block_num,
                                0,
                                0,
                            )

            y_offset.set_as(
                y_pre_offset + (idx_loop_i * idx_num_per_loop + idx_batch_i * self.row_num_once_ub) * self.params_row
            )
            self.tik_instance.data_move(
                self.y[y_offset], res_ub, 0, 1, idx_num_cur_batch * row_size // constant.BLOCK_SIZE, 0, 0
            )

        def compute_for_one_idx_loop(idx_loop_i):
            """
            compute for one idx loop
            """
            tik_inst.data_move(
                indices_ub,
                self.indices[idx_start + idx_loop_i * idx_num_per_loop],
                0,
                1,
                ceil_value(idx_num_cur_loop * self.indices_dsize, constant.BLOCK_SIZE),
                0,
                0,
            )
            if self.indices_dtype == Constant.DTYPE_INT32 and tbe_platform_adapter.api_check_support(
                "tbe.dsl.vexp", Constant.DTYPE_FP32
            ):
                tik_inst.h_cast(indices_index_float_ub, indices_ub, "none")
            idx_num_cur_batch.set_as(self.row_num_once_ub)
            with tik_inst.for_range(0, idx_num_cur_loop // self.row_num_once_ub) as idx_batch_i:
                compute_for_once_ub(idx_loop_i, idx_batch_i)
            with tik_inst.if_scope(idx_num_cur_loop % self.row_num_once_ub * row_size >= constant.BLOCK_SIZE):
                idx_num_cur_batch.set_as(idx_num_cur_loop % self.row_num_once_ub)
                compute_for_once_ub(idx_loop_i, idx_num_cur_loop // self.row_num_once_ub)

        with tik_inst.for_range(0, self.params_pre) as pre_i:
            y_pre_offset.set_as((pre_i * self.indices_num + idx_start) * self.params_row)
            idx_num_cur_loop.set_as(idx_num_per_loop)
            with self.tik_instance.for_range(0, idx_num_cur_core // idx_num_per_loop) as idx_loop_i:
                compute_for_one_idx_loop(idx_loop_i)
            with tik_inst.if_scope(idx_num_cur_core % idx_num_per_loop != 0):
                idx_num_cur_loop.set_as(idx_num_cur_core % idx_num_per_loop)
                compute_for_one_idx_loop(idx_num_cur_core // idx_num_per_loop)

    def compute_mode_16(self, block_id):
        """
        compute for tiling mode 16

        Parameters
        ----------
        block_id: id of ai core

        Returns
        -------
        None
        """
        tik_inst = self.tik_instance

        idx_start = tik_inst.Scalar(
            dtype=self.tiling_dtype, name="idx_start", init_value=block_id * self.indices_num_each_core
        )
        idx_num_cur_core = tik_inst.Scalar(
            dtype=self.tiling_dtype, name="idx_num_cur_core", init_value=self.indices_num_each_core
        )
        with tik_inst.if_scope(block_id >= self.tail_process_core):
            idx_start.set_as(block_id * self.indices_num_remaining + self.tail_process_core)
            idx_num_cur_core.set_as(self.indices_num_remaining)
        indices_value = tik_inst.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
        CurIdxTuple = collections.namedtuple("CurIdxTuple", ["idx_start", "idx_num_cur_core", "indices_value"])
        cur_idx_tuple = CurIdxTuple(idx_start, idx_num_cur_core, indices_value)
        x_pre_offset = tik_inst.Scalar(dtype=self.tiling_dtype, name="x_pre_offset")
        y_pre_offset = tik_inst.Scalar(dtype=self.tiling_dtype, name="y_pre_offset")
        y_offset = tik_inst.Scalar(dtype=self.tiling_dtype, name="y_offset")
        RowOffsetTuple = collections.namedtuple("RowOffsetTuple", ["x_pre_offset", "y_pre_offset", "y_offset"])
        row_offset_tuple = RowOffsetTuple(x_pre_offset, y_pre_offset, y_offset)

        with tik_inst.new_stmt_scope():
            self.compute_mode_16_aixs_eq_0(row_offset_tuple, cur_idx_tuple)

    def compute_mode_16_aixs_eq_0(self, row_offset_tuple, cur_idx_tuple):
        """
        compute_mode_16_aixs_eq_0 branch for cases which aixs is 0
        """
        tik_inst = self.tik_instance
        (x_pre_offset, y_pre_offset, y_offset) = row_offset_tuple
        (idx_start, idx_num_cur_core, indices_value) = cur_idx_tuple

        ub_tensor_size = floor_align((self.ub_size - Constant.RESERVED_UB_SIZE) // 6, constant.BLOCK_SIZE)

        res_ub = tik_inst.Tensor(
            self.params_dtype, (ub_tensor_size // self.params_dsize,), name="res_ub", scope=tik.scope_ubuf
        )
        ub_xi = tik_inst.Tensor(self.params_dtype, (constant.BLOCK_SIZE,), name="ub_xi", scope=tik.scope_ubuf)
        indices_ub = tik_inst.Tensor(
            self.indices_dtype, (ub_tensor_size // self.indices_dsize,), name="indices_ub", scope=tik.scope_ubuf
        )
        cached_ub = tik_inst.Tensor(
            self.params_dtype, (ub_tensor_size // self.params_dsize,), name="cached_ub", scope=tik.scope_ubuf
        )
        cached_n_num_ub = tik_inst.Tensor(
            self.params_dtype, (ub_tensor_size // self.params_dsize,), name="cached_n_num_ub", scope=tik.scope_ubuf
        )
        indices_index_float_ub = tik_inst.Tensor(
            Constant.DTYPE_FP32,
            (ub_tensor_size // self.indices_dsize,),
            name="indices_index_float_ub",
            scope=tik.scope_ubuf,
        )
        indices_simpling_ub = tik_inst.Tensor(
            Constant.DTYPE_FP32,
            (Constant.CACHE_ACT_SIMPLING_BUFF_NUM,),
            name="indices_simpling_ub",
            scope=tik.scope_ubuf,
        )
        row_size = tik_inst.Scalar(dtype=self.tiling_dtype, name="row_size", init_value=self.params_row)
        row_size_align = tik_inst.Scalar(dtype=self.tiling_dtype, name="row_size_align")
        row_size_align.set_as(
            align_value(self.params_row * self.params_dsize, constant.BLOCK_SIZE) // self.params_dsize
        )
        cache_max_n_num = tik_inst.Scalar(dtype=Constant.DTYPE_INT32, name="cache_max_n_num")
        cache_max_n_num.set_as(ub_tensor_size // (row_size_align * self.params_dsize))
        is_full_cache = tik_inst.Scalar(dtype=Constant.DTYPE_INT32, name="is_full_cache", init_value=0)
        with self.tik_instance.if_scope(cache_max_n_num > self.params_axis):
            cache_max_n_num.set_as(self.params_axis)
            is_full_cache.set_as(1)
        params_block_num = tik_inst.Scalar(dtype=Constant.DTYPE_INT32, name="params_block_num")
        params_block_num.set_as(ceil_value(self.params_row * self.params_dsize, constant.BLOCK_SIZE))
        simpling_step = tik_inst.Scalar(dtype=Constant.DTYPE_INT32, name="simpling_step")
        self.row_num_once_ub.set_as(
            floor_align(ub_tensor_size, (row_size_align * Constant.FP16_ALIGN_NUM * self.params_dsize))
            // (row_size_align * self.params_dsize)
        )
        indices_temp_int_ub = tik_inst.Tensor(
            Constant.DTYPE_INT32, (self.row_num_once_ub,), name="indices_temp_int_ub", scope=tik.scope_ubuf
        )
        indices_temp_float_ub = tik_inst.Tensor(
            Constant.DTYPE_FP32, (self.row_num_once_ub,), name="indices_temp_float_ub", scope=tik.scope_ubuf
        )

        idx_num_per_loop = ub_tensor_size // self.indices_dsize
        idx_num_cur_loop = tik_inst.Scalar(dtype=self.tiling_dtype, name="idx_num_cur_loop")
        idx_num_cur_batch = tik_inst.Scalar(dtype=self.tiling_dtype, name="idx_num_cur_batch")

        if tbe_platform.api_check_support("tik.data_move_pad", self.params_dtype):
            self.tik_instance.data_move_pad(
                cached_n_num_ub, self.x, cache_max_n_num, row_size * self.params_dsize, 0, 0, 0, 0, None
            )
        else:
            with self.tik_instance.for_range(0, cache_max_n_num) as idx:
                tik_inst.data_move(
                    cached_n_num_ub[idx * row_size_align],
                    self.x[idx * row_size],
                    0,
                    1,
                    ceil_value(cache_max_n_num * self.params_row * self.params_dsize, constant.BLOCK_SIZE),
                    0,
                    0,
                )

        def compute_for_once_ub(idx_loop_i, idx_batch_i):
            """
            compute for once ub
            """
            opt_mode = tik_inst.Scalar(dtype=Constant.DTYPE_INT32, name="opt_mode")
            indice_base_offset = tik_inst.Scalar(
                dtype=Constant.DTYPE_INT32, name="indice_base_offset", init_value=idx_batch_i * self.row_num_once_ub
            )
            one_block_num = tik_inst.Scalar(
                dtype=Constant.DTYPE_INT32, name="one_block_num", init_value=Constant.ONE_BLOCK_FP32_NUM
            )
            trans_type_to_fp16 = tik_inst.Scalar(dtype=Constant.DTYPE_INT32, name="trans_type_to_fp16", init_value=2)
            if self.params_dsize == 2:
                one_block_num.set_as(Constant.ONE_BLOCK_FP16_NUM)
                trans_type_to_fp16.set_as(1)

            self.get_optimization_mode(
                idx_batch_i,
                opt_mode,
                simpling_step,
                idx_num_cur_batch,
                cache_max_n_num,
                indices_simpling_ub,
                indices_index_float_ub,
            )
            with self.tik_instance.if_scope(opt_mode == Constant.INPUT_LAST_AXIS_ALIGN_HP_OPT_CACHE_SIMPLING):
                self.compute_for_simpling_cache_data(
                    idx_batch_i,
                    simpling_step,
                    indices_ub,
                    cached_ub,
                    row_size_align,
                    params_block_num,
                    idx_num_cur_batch,
                    indices_simpling_ub,
                    indices_index_float_ub,
                    indices_temp_float_ub,
                    indices_temp_int_ub,
                )
                with self.tik_instance.for_range(0, idx_num_cur_batch) as idx_i:
                    indices_value.set_as(indices_temp_int_ub[idx_i])
                    with self.tik_instance.if_scope(indices_value > 0):
                        tik_inst.data_move(
                            res_ub[idx_i * row_size_align],
                            cached_ub[indices_value * row_size_align],
                            0,
                            1,
                            params_block_num,
                            0,
                            0,
                        )
                    with self.tik_instance.else_scope():
                        indices_value.set_as(indices_ub[indice_base_offset + idx_i])
                        tik_inst.data_move(
                            res_ub[idx_i * row_size_align],
                            self.x[x_pre_offset + indices_value * self.params_row],
                            0,
                            1,
                            params_block_num,
                            0,
                            0,
                        )
            with self.tik_instance.else_scope():
                with tik_inst.if_scope(is_full_cache == 1):
                    unrool_num = 8
                    index_array = self.tik_instance.ScalarArray(
                        dtype=self.tiling_dtype, length=unrool_num, name="index_array"
                    )
                    with self.tik_instance.for_range(0, idx_num_cur_batch // unrool_num) as idx_i:
                        with self.tik_instance.for_range(0, unrool_num) as unroll_idx:
                            index_array[unroll_idx].set_as(
                                indices_ub[indice_base_offset + idx_i * unrool_num + unroll_idx]
                            )

                        with self.tik_instance.for_range(0, unrool_num) as unroll_idx:
                            tik_inst.data_move(
                                res_ub[(idx_i * unrool_num + unroll_idx) * row_size_align],
                                cached_n_num_ub[index_array[unroll_idx] * row_size_align],
                                0,
                                1,
                                params_block_num,
                                0,
                                0,
                            )
                    with self.tik_instance.if_scope(idx_num_cur_batch % unrool_num > 0):
                        unrool_offset = idx_num_cur_batch // unrool_num * unrool_num
                        tail_num = idx_num_cur_batch % unrool_num
                        with self.tik_instance.for_range(0, tail_num) as unroll_idx:
                            index_array[unroll_idx].set_as(indices_ub[indice_base_offset + unrool_offset + unroll_idx])

                        with self.tik_instance.for_range(0, tail_num) as unroll_idx:
                            tik_inst.data_move(
                                res_ub[(unrool_offset + unroll_idx) * row_size_align],
                                cached_n_num_ub[index_array[unroll_idx] * row_size_align],
                                0,
                                1,
                                params_block_num,
                                0,
                                0,
                            )

                with self.tik_instance.else_scope():
                    with self.tik_instance.for_range(0, idx_num_cur_batch) as idx_i:
                        indices_value.set_as(indices_ub[indice_base_offset + idx_i])
                        with self.tik_instance.if_scope(indices_value < cache_max_n_num):
                            tik_inst.data_move(
                                res_ub[idx_i * row_size_align],
                                cached_n_num_ub[indices_value * row_size_align],
                                0,
                                1,
                                params_block_num,
                                0,
                                0,
                            )
                        with self.tik_instance.else_scope():
                            tik_inst.data_move(
                                res_ub[idx_i * row_size_align],
                                self.x[x_pre_offset + indices_value * self.params_row],
                                0,
                                1,
                                params_block_num,
                                0,
                                0,
                            )
            if tbe_platform.api_check_support("tik.data_move_pad", self.params_dtype):
                y_offset.set_as(
                    y_pre_offset
                    + (idx_loop_i * idx_num_per_loop + idx_batch_i * self.row_num_once_ub) * self.params_row
                )
                self.tik_instance.data_move_pad(
                    self.y[y_offset], res_ub, idx_num_cur_batch, row_size * self.params_dsize, 0, 0, 0, 0, None
                )
            else:
                self.transdata_and_move_out(
                    res_ub,
                    idx_loop_i,
                    idx_batch_i,
                    idx_num_cur_batch,
                    cached_ub,
                    row_size_align,
                    row_size,
                    y_offset,
                    y_pre_offset,
                    idx_num_per_loop,
                    ub_xi,
                )

        def compute_for_one_idx_loop(idx_loop_i):
            """
            compute for one idx loop
            """
            tik_inst.data_move(
                indices_ub,
                self.indices[idx_start + idx_loop_i * idx_num_per_loop],
                0,
                1,
                ceil_value(idx_num_cur_loop * self.indices_dsize, constant.BLOCK_SIZE),
                0,
                0,
            )
            if self.indices_dtype == Constant.DTYPE_INT32 and tbe_platform_adapter.api_check_support(
                "tbe.dsl.vexp", Constant.DTYPE_FP32
            ):
                tik_inst.h_cast(indices_index_float_ub, indices_ub, "none")
            idx_num_cur_batch.set_as(self.row_num_once_ub)
            with tik_inst.for_range(0, idx_num_cur_loop // self.row_num_once_ub) as idx_batch_i:
                compute_for_once_ub(idx_loop_i, idx_batch_i)
            with tik_inst.if_scope(idx_num_cur_loop % self.row_num_once_ub != 0):
                idx_num_cur_batch.set_as(idx_num_cur_loop % self.row_num_once_ub)
                compute_for_once_ub(idx_loop_i, idx_num_cur_loop // self.row_num_once_ub)

        with tik_inst.for_range(0, self.params_pre) as pre_i:
            y_pre_offset.set_as((pre_i * self.indices_num + idx_start) * self.params_row)
            idx_num_cur_loop.set_as(idx_num_per_loop)
            with self.tik_instance.for_range(0, idx_num_cur_core // idx_num_per_loop) as idx_loop_i:
                compute_for_one_idx_loop(idx_loop_i)
            with tik_inst.if_scope(idx_num_cur_core % idx_num_per_loop != 0):
                idx_num_cur_loop.set_as(idx_num_cur_core % idx_num_per_loop)
                compute_for_one_idx_loop(idx_num_cur_core // idx_num_per_loop)

    def create_pos_matrix(self, number, pos_matrix):
        tik_inst = self.tik_instance
        with self.tik_instance.new_stmt_scope():
            add_offset = tik_inst.Tensor(Constant.DTYPE_INT32, (64,), name="add_offset", scope=tik.scope_ubuf)
            tik_inst.vec_dup(64, add_offset, 64, 1, 8)
            with tik_inst.for_range(0, 64) as idx:
                pos_matrix[idx].set_as(idx)

            rept_time = number // 64
            with tik_inst.for_range(1, rept_time) as idx:
                tik_inst.vec_add(64, pos_matrix[idx * 64], pos_matrix[(idx - 1) * 64], add_offset, 1, 8, 8, 8)

            with tik_inst.if_scope(number % 64 != 0):
                tik_inst.vec_add(
                    number % 64, pos_matrix[rept_time * 64], pos_matrix[(rept_time - 1) * 64], add_offset, 1, 8, 8, 8
                )

    # 'pylint: disable=too-many-arguments,huawei-too-many-arguments
    def date_move_use_unroll(
        self,
        out_indices_cached,
        out_pos_indices_cached,
        src_ub,
        number,
        row_size_align,
        params_row,
        params_block_num,
        res_ub,
    ):
        tik_inst = self.tik_instance
        unrool_num = 8
        index_array = self.tik_instance.ScalarArray(dtype=Constant.DTYPE_INT32, length=unrool_num, name="index_array")
        index_pos_array = self.tik_instance.ScalarArray(
            dtype=Constant.DTYPE_INT32, length=unrool_num, name="index_pos_array"
        )
        with self.tik_instance.new_stmt_scope(disable_sync=True):
            with self.tik_instance.for_range(0, number // unrool_num) as idx_i:
                with self.tik_instance.for_range(0, unrool_num) as unroll_idx:
                    index_array[unroll_idx].set_as(out_indices_cached[idx_i * unrool_num + unroll_idx])
                    index_pos_array[unroll_idx].set_as(out_pos_indices_cached[idx_i * unrool_num + unroll_idx])
                with self.tik_instance.for_range(0, unrool_num) as unroll_idx:
                    tik_inst.data_move(
                        res_ub[index_pos_array[unroll_idx] * row_size_align],
                        src_ub[index_array[unroll_idx] * params_row],
                        0,
                        1,
                        params_block_num,
                        0,
                        0,
                    )
            with self.tik_instance.if_scope(number % unrool_num > 0):
                unrool_offset = number // unrool_num * unrool_num
                tail_num = number % unrool_num
                with self.tik_instance.for_range(0, tail_num) as unroll_idx:
                    index_array[unroll_idx].set_as(out_indices_cached[unrool_offset + unroll_idx])
                    index_pos_array[unroll_idx].set_as(out_pos_indices_cached[unrool_offset + unroll_idx])
                with self.tik_instance.for_range(0, tail_num) as unroll_idx:
                    tik_inst.data_move(
                        res_ub[index_pos_array[unroll_idx] * row_size_align],
                        src_ub[index_array[unroll_idx] * params_row],
                        0,
                        1,
                        params_block_num,
                        0,
                        0,
                    )

    # 'pylint: disable=too-many-arguments,huawei-too-many-arguments
    def calc_indices_for_move_date(
        self,
        indices_ub,
        indices_ub_fp,
        dup_cached_num,
        pos_matrix,
        idx_num_cur_batch,
        out_indices_cached,
        out_pos_indices_cached,
        cmp_mod,
        number,
    ):
        tik_inst = self.tik_instance
        with self.tik_instance.new_stmt_scope():
            ub_mask = tik_inst.Tensor(Constant.DTYPE_UINT32, (512,), name="ub_mask", scope=tik.scope_ubuf)

            rept_time = idx_num_cur_batch // 64 + 1
            tik_inst.vec_dup(64, ub_mask, 0, 8, 8)
            if cmp_mod == Constant.CMP_LESS_THAN:
                tik_inst.vec_cmpv_lt(ub_mask, indices_ub_fp, dup_cached_num, rept_time, 8, 0)
            else:
                tik_inst.vec_cmpv_ge(ub_mask, indices_ub_fp, dup_cached_num, rept_time, 8, 0)

            tik_inst.vreducev2(
                idx_num_cur_batch, out_indices_cached, indices_ub, ub_mask, 1, 1, 8, 0, number, mask_mode="counter"
            )
            tik_inst.vreducev2(
                idx_num_cur_batch, out_pos_indices_cached, pos_matrix, ub_mask, 1, 1, 8, 0, mask_mode="counter"
            )

    def compute_mode_17(self, block_id):
        """
        compute for tiling mode 17

        Parameters
        ----------
        block_id: id of ai core

        Returns
        -------
        None
        """
        tik_inst = self.tik_instance

        idx_start = tik_inst.Scalar(
            dtype=self.tiling_dtype, name="idx_start", init_value=block_id * self.indices_num_each_core
        )
        idx_num_cur_core = tik_inst.Scalar(
            dtype=self.tiling_dtype, name="idx_num_cur_core", init_value=self.indices_num_each_core
        )
        with tik_inst.if_scope(block_id >= self.tail_process_core):
            idx_start.set_as(block_id * self.indices_num_remaining + self.tail_process_core)
            idx_num_cur_core.set_as(self.indices_num_remaining)
        indices_value = tik_inst.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
        CurIdxTuple = collections.namedtuple("CurIdxTuple", ["idx_start", "idx_num_cur_core", "indices_value"])
        cur_idx_tuple = CurIdxTuple(idx_start, idx_num_cur_core, indices_value)
        x_pre_offset = tik_inst.Scalar(dtype=self.tiling_dtype, name="x_pre_offset")
        y_pre_offset = tik_inst.Scalar(dtype=self.tiling_dtype, name="y_pre_offset")
        y_offset = tik_inst.Scalar(dtype=self.tiling_dtype, name="y_offset")
        RowOffsetTuple = collections.namedtuple("RowOffsetTuple", ["x_pre_offset", "y_pre_offset", "y_offset"])
        row_offset_tuple = RowOffsetTuple(x_pre_offset, y_pre_offset, y_offset)

        with tik_inst.new_stmt_scope():
            self.compute_mode_17_aixs_eq_0(row_offset_tuple, cur_idx_tuple)

    def compute_mode_17_aixs_eq_0(self, row_offset_tuple, cur_idx_tuple):
        """
        compute_mode_17_aixs_eq_0 branch for cases which aixs is 0
        """
        tik_inst = self.tik_instance
        (x_pre_offset, y_pre_offset, y_offset) = row_offset_tuple
        (idx_start, idx_num_cur_core, indices_value) = cur_idx_tuple

        ub_tensor_size = floor_align((self.ub_size - Constant.RESERVED_UB_SIZE) // 4, constant.BLOCK_SIZE)
        res_ub = tik_inst.Tensor(
            self.params_dtype, (ub_tensor_size // self.params_dsize,), name="res_ub", scope=tik.scope_ubuf
        )
        res_ub_trans = tik_inst.Tensor(
            self.params_dtype, (ub_tensor_size // self.params_dsize,), name="res_ub_trans", scope=tik.scope_ubuf
        )
        ub_xi = tik_inst.Tensor(self.params_dtype, (constant.BLOCK_SIZE,), name="ub_xi", scope=tik.scope_ubuf)
        indices_ub_size = ub_tensor_size // 6 // 64 * 64
        indices_ub = tik_inst.Tensor(
            self.indices_dtype, (indices_ub_size // self.indices_dsize + 64,), name="indices_ub", scope=tik.scope_ubuf
        )
        indices_ub_fp = tik_inst.Tensor(
            Constant.DTYPE_FP32,
            (indices_ub_size // self.indices_dsize + 64,),
            name="indices_ub_fp",
            scope=tik.scope_ubuf,
        )
        pos_matrix = tik_inst.Tensor(
            self.indices_dtype, (indices_ub_size // self.indices_dsize,), name="pos_matrix", scope=tik.scope_ubuf
        )
        out_indices = tik_inst.Tensor(
            self.indices_dtype,
            (indices_ub_size // self.indices_dsize,),
            name="out_indices_cached",
            scope=tik.scope_ubuf,
        )
        out_pos_indices = tik_inst.Tensor(
            self.indices_dtype,
            (indices_ub_size // self.indices_dsize,),
            name="out_pos_indices_cached",
            scope=tik.scope_ubuf,
        )
        dup_cached_num = tik_inst.Tensor(Constant.DTYPE_FP32, (64,), name="dup_cached_num", scope=tik.scope_ubuf)
        cached_ub = tik_inst.Tensor(
            self.params_dtype, (ub_tensor_size // self.params_dsize,), name="cached_ub", scope=tik.scope_ubuf
        )
        cached_n_number = tik_inst.Scalar(dtype=Constant.DTYPE_INT32, name="cached_n_number")
        row_size = tik_inst.Scalar(dtype=self.tiling_dtype, name="row_size", init_value=self.params_row)
        row_size_align = tik_inst.Scalar(dtype=self.tiling_dtype, name="row_size_align")
        row_size_align.set_as(
            align_value(self.params_row * self.params_dsize, constant.BLOCK_SIZE) // self.params_dsize
        )
        cached_n_number.set_as(ub_tensor_size // (row_size_align * self.params_dsize))
        params_block_num = tik_inst.Scalar(dtype=Constant.DTYPE_INT32, name="params_block_num")
        params_block_num.set_as(ceil_value(self.params_row * self.params_dsize, constant.BLOCK_SIZE))
        self.row_num_once_ub.set_as(
            floor_align(ub_tensor_size, (row_size_align * Constant.FP16_ALIGN_NUM * self.params_dsize))
            // (row_size_align * self.params_dsize)
        )
        self.row_num_once_ub.set_as(self.row_num_once_ub // 64 * 64)
        is_full_cache = tik_inst.Scalar(dtype=Constant.DTYPE_INT32, name="is_full_cache", init_value=0)
        with tik_inst.if_scope(cached_n_number > self.params_axis):
            cached_n_number.set_as(self.params_axis)
            is_full_cache.set_as(1)
        cached_n_number_fp = tik_inst.Scalar(dtype=Constant.DTYPE_FP32, name="cached_n_number_fp")

        idx_num_per_loop = indices_ub_size // self.indices_dsize
        idx_num_cur_loop = tik_inst.Scalar(dtype=self.tiling_dtype, name="idx_num_cur_loop")
        idx_num_cur_batch = tik_inst.Scalar(dtype=self.tiling_dtype, name="idx_num_cur_batch")

        if (
            PlatformApi.get_soc_spec(PlatformApi.SHORT_SOC_VERSION) == "Ascend910B"
            and self.indices_dtype == Constant.DTYPE_INT32
        ):
            self.create_pos_matrix(idx_num_per_loop, pos_matrix)
            tik_inst.scalar_conv("none", cached_n_number_fp, cached_n_number)
            tik_inst.vec_dup(64, dup_cached_num, cached_n_number_fp, 1, 8)

        def compute_for_once_ub_not_align(idx_loop_i, idx_batch_i):
            """
            compute for once ub not align
            """
            indice_base_offset = tik_inst.Scalar(
                dtype=Constant.DTYPE_INT32, name="indice_base_offset", init_value=idx_batch_i * self.row_num_once_ub
            )
            one_block_num = tik_inst.Scalar(
                dtype=Constant.DTYPE_INT32, name="one_block_num", init_value=Constant.ONE_BLOCK_FP32_NUM
            )
            trans_type_to_fp16 = tik_inst.Scalar(dtype=Constant.DTYPE_INT32, name="trans_type_to_fp16", init_value=2)
            if self.params_dsize == 2:
                one_block_num.set_as(Constant.ONE_BLOCK_FP16_NUM)
                trans_type_to_fp16.set_as(1)

            with tik_inst.if_scope(is_full_cache == 1):
                unrool_num = 8
                index_array = self.tik_instance.ScalarArray(
                    dtype=self.tiling_dtype, length=unrool_num, name="index_array"
                )
                with self.tik_instance.for_range(0, idx_num_cur_batch // unrool_num) as idx_i:
                    with self.tik_instance.for_range(0, unrool_num) as unroll_idx:
                        index_array[unroll_idx].set_as(indices_ub[indice_base_offset + idx_i * unrool_num + unroll_idx])

                    with self.tik_instance.for_range(0, unrool_num) as unroll_idx:
                        tik_inst.data_move(
                            res_ub[(idx_i * unrool_num + unroll_idx) * row_size_align],
                            cached_ub[index_array[unroll_idx] * row_size_align],
                            0,
                            1,
                            params_block_num,
                            0,
                            0,
                        )
                with self.tik_instance.if_scope(idx_num_cur_batch % unrool_num > 0):
                    unrool_offset = idx_num_cur_batch // unrool_num * unrool_num
                    tail_num = idx_num_cur_batch % unrool_num
                    with self.tik_instance.for_range(0, tail_num) as unroll_idx:
                        index_array[unroll_idx].set_as(indices_ub[indice_base_offset + unrool_offset + unroll_idx])

                    with self.tik_instance.for_range(0, tail_num) as unroll_idx:
                        tik_inst.data_move(
                            res_ub[(unrool_offset + unroll_idx) * row_size_align],
                            cached_ub[index_array[unroll_idx] * row_size_align],
                            0,
                            1,
                            params_block_num,
                            0,
                            0,
                        )
            with tik_inst.else_scope():
                if (
                    PlatformApi.get_soc_spec(PlatformApi.SHORT_SOC_VERSION) == "Ascend910B"
                    and self.indices_dtype == Constant.DTYPE_INT32
                ):
                    number = tik_inst.Scalar(dtype=Constant.DTYPE_UINT32, name="number")
                    self.calc_indices_for_move_date(
                        indices_ub[indice_base_offset],
                        indices_ub_fp[indice_base_offset],
                        dup_cached_num,
                        pos_matrix,
                        idx_num_cur_batch,
                        out_indices,
                        out_pos_indices,
                        Constant.CMP_LESS_THAN,
                        number,
                    )
                    self.date_move_use_unroll(
                        out_indices,
                        out_pos_indices,
                        cached_ub,
                        number,
                        row_size_align,
                        row_size_align,
                        params_block_num,
                        res_ub,
                    )
                    self.calc_indices_for_move_date(
                        indices_ub[indice_base_offset],
                        indices_ub_fp[indice_base_offset],
                        dup_cached_num,
                        pos_matrix,
                        idx_num_cur_batch,
                        out_indices,
                        out_pos_indices,
                        Constant.CMP_GREAT_EQUAL,
                        number,
                    )
                    self.date_move_use_unroll(
                        out_indices,
                        out_pos_indices,
                        self.x[x_pre_offset],
                        number,
                        row_size_align,
                        self.params_row,
                        params_block_num,
                        res_ub,
                    )
                else:
                    with self.tik_instance.for_range(0, idx_num_cur_batch) as idx_i:
                        indices_value.set_as(indices_ub[indice_base_offset + idx_i])
                        with self.tik_instance.if_scope(indices_value < cached_n_number):
                            tik_inst.data_move(
                                res_ub[idx_i * row_size_align],
                                cached_ub[indices_value * row_size_align],
                                0,
                                1,
                                params_block_num,
                                0,
                                0,
                            )
                        with self.tik_instance.else_scope():
                            tik_inst.data_move(
                                res_ub[idx_i * row_size_align],
                                self.x[x_pre_offset + indices_value * self.params_row],
                                0,
                                1,
                                params_block_num,
                                0,
                                0,
                            )

            if tbe_platform.api_check_support("tik.data_move_pad", self.params_dtype):
                y_offset.set_as(
                    y_pre_offset
                    + (idx_loop_i * idx_num_per_loop + idx_batch_i * self.row_num_once_ub) * self.params_row
                )
                self.tik_instance.data_move_pad(
                    self.y[y_offset], res_ub, idx_num_cur_batch, row_size * self.params_dsize, 0, 0, 0, 0, None
                )
            else:
                self.transdata_and_move_out(
                    res_ub,
                    idx_loop_i,
                    idx_batch_i,
                    idx_num_cur_batch,
                    res_ub_trans,
                    row_size_align,
                    row_size,
                    y_offset,
                    y_pre_offset,
                    idx_num_per_loop,
                    ub_xi,
                )

        def compute_for_one_idx_loop_not_align(idx_loop_i):
            """
            compute for one idx loop
            """
            tik_inst.data_move(
                indices_ub,
                self.indices[idx_start + idx_loop_i * idx_num_per_loop],
                0,
                1,
                ceil_value(idx_num_cur_loop * self.indices_dsize, constant.BLOCK_SIZE),
                0,
                0,
            )
            if (
                PlatformApi.get_soc_spec(PlatformApi.SHORT_SOC_VERSION) == "Ascend910B"
                and self.indices_dtype == Constant.DTYPE_INT32
            ):
                tik_inst.h_cast(indices_ub_fp, indices_ub, "none")
            idx_num_cur_batch.set_as(self.row_num_once_ub)
            with tik_inst.for_range(0, idx_num_cur_loop // self.row_num_once_ub) as idx_batch_i:
                compute_for_once_ub_not_align(idx_loop_i, idx_batch_i)
            with tik_inst.if_scope(idx_num_cur_loop % self.row_num_once_ub != 0):
                idx_num_cur_batch.set_as(idx_num_cur_loop % self.row_num_once_ub)
                compute_for_once_ub_not_align(idx_loop_i, idx_num_cur_loop // self.row_num_once_ub)

        if tbe_platform.api_check_support("tik.data_move_pad", self.params_dtype):
            self.tik_instance.data_move_pad(
                cached_ub, self.x, cached_n_number, row_size * self.params_dsize, 0, 0, 0, 0, None
            )
        else:
            with tik_inst.for_range(0, cached_n_number) as i_index:
                self.tik_instance.data_move(
                    cached_ub[i_index * row_size_align],
                    self.x[i_index * row_size],
                    0,
                    1,
                    ceil_value(row_size * self.params_dsize, constant.BLOCK_SIZE),
                    0,
                    0,
                )

        with tik_inst.for_range(0, self.params_pre) as pre_i:
            y_pre_offset.set_as((pre_i * self.indices_num + idx_start) * self.params_row)
            idx_num_cur_loop.set_as(idx_num_per_loop)
            with self.tik_instance.for_range(0, idx_num_cur_core // idx_num_per_loop) as idx_loop_i:
                compute_for_one_idx_loop_not_align(idx_loop_i)
            with tik_inst.if_scope(idx_num_cur_core % idx_num_per_loop != 0):
                idx_num_cur_loop.set_as(idx_num_cur_core % idx_num_per_loop)
                compute_for_one_idx_loop_not_align(idx_num_cur_core // idx_num_per_loop)

    def compute_mode_18(self, block_id):
        """
        compute for tiling mode 18

        Parameters
        ----------
        block_id: id of ai core

        Returns
        -------
        None
        """
        tik_inst = self.tik_instance

        idx_start = tik_inst.Scalar(
            dtype=self.tiling_dtype, name="idx_start", init_value=block_id * self.indices_num_each_core
        )
        idx_num_cur_core = tik_inst.Scalar(
            dtype=self.tiling_dtype, name="idx_num_cur_core", init_value=self.indices_num_each_core
        )
        with tik_inst.if_scope(block_id >= self.tail_process_core):
            idx_start.set_as(block_id * self.indices_num_remaining + self.tail_process_core)
            idx_num_cur_core.set_as(self.indices_num_remaining)
        indices_value = tik_inst.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
        CurIdxTuple = collections.namedtuple("CurIdxTuple", ["idx_start", "idx_num_cur_core", "indices_value"])
        cur_idx_tuple = CurIdxTuple(idx_start, idx_num_cur_core, indices_value)
        x_pre_offset = tik_inst.Scalar(dtype=self.tiling_dtype, name="x_pre_offset")
        y_pre_offset = tik_inst.Scalar(dtype=self.tiling_dtype, name="y_pre_offset")
        y_offset = tik_inst.Scalar(dtype=self.tiling_dtype, name="y_offset")
        RowOffsetTuple = collections.namedtuple("RowOffsetTuple", ["x_pre_offset", "y_pre_offset", "y_offset"])
        row_offset_tuple = RowOffsetTuple(x_pre_offset, y_pre_offset, y_offset)

        with tik_inst.new_stmt_scope():
            self.compute_mode_18_aixs_eq_0(row_offset_tuple, cur_idx_tuple)

    def compute_mode_18_aixs_eq_0(self, row_offset_tuple, cur_idx_tuple):
        """
        compute_mode_18_aixs_eq_0 branch for cases which aixs is 0
        """
        tik_inst = self.tik_instance
        (x_pre_offset, y_pre_offset, y_offset) = row_offset_tuple
        (idx_start, idx_num_cur_core, indices_value) = cur_idx_tuple

        ub_tensor_size = floor_align((self.ub_size - Constant.RESERVED_UB_SIZE) // 3, constant.BLOCK_SIZE)
        res_ub = tik_inst.Tensor(
            self.params_dtype, (ub_tensor_size // self.params_dsize,), name="res_ub", scope=tik.scope_ubuf
        )
        indices_ub_size = ub_tensor_size // Constant.CUT_SUB_UB_SIZE // Constant.SIZE_ALIGN_64 * Constant.SIZE_ALIGN_64
        indices_ub = tik_inst.Tensor(
            self.indices_dtype,
            (indices_ub_size // self.indices_dsize + Constant.SIZE_ALIGN_64,),
            name="indices_ub",
            scope=tik.scope_ubuf,
        )
        indices_ub_fp = tik_inst.Tensor(
            Constant.DTYPE_FP32,
            (indices_ub_size // self.indices_dsize + Constant.SIZE_ALIGN_64,),
            name="indices_ub_fp",
            scope=tik.scope_ubuf,
        )
        pos_matrix = tik_inst.Tensor(
            self.indices_dtype, (indices_ub_size // self.indices_dsize,), name="pos_matrix", scope=tik.scope_ubuf
        )
        out_indices = tik_inst.Tensor(
            self.indices_dtype,
            (indices_ub_size // self.indices_dsize,),
            name="out_indices_cached",
            scope=tik.scope_ubuf,
        )
        out_pos_indices = tik_inst.Tensor(
            self.indices_dtype,
            (indices_ub_size // self.indices_dsize,),
            name="out_pos_indices_cached",
            scope=tik.scope_ubuf,
        )
        dup_cached_num = tik_inst.Tensor(Constant.DTYPE_FP32, (64,), name="dup_cached_num", scope=tik.scope_ubuf)

        cached_ub = tik_inst.Tensor(
            self.params_dtype, (ub_tensor_size // self.params_dsize,), name="cached_ub", scope=tik.scope_ubuf
        )
        cached_n_number = tik_inst.Scalar(dtype=Constant.DTYPE_INT32, name="cached_n_number")
        row_size = tik_inst.Scalar(dtype=self.indices_dtype, name="row_size", init_value=self.params_row)
        row_size_align = tik_inst.Scalar(dtype=self.indices_dtype, name="row_size_align")
        row_size_align.set_as(
            align_value(self.params_row * self.params_dsize, constant.BLOCK_SIZE) // self.params_dsize
        )
        cached_n_number.set_as(ub_tensor_size // (row_size_align * self.params_dsize))
        params_block_num = tik_inst.Scalar(dtype=Constant.DTYPE_INT32, name="params_block_num")
        params_block_num.set_as(ceil_value(self.params_row * self.params_dsize, constant.BLOCK_SIZE))
        self.row_num_once_ub.set_as(
            floor_align(ub_tensor_size, (row_size_align * self.params_dsize)) // (row_size_align * self.params_dsize)
        )
        self.row_num_once_ub.set_as(self.row_num_once_ub // 64 * 64)
        is_full_cache = tik_inst.Scalar(dtype=Constant.DTYPE_INT32, name="is_full_cache", init_value=0)
        with tik_inst.if_scope(cached_n_number > self.params_axis):
            cached_n_number.set_as(self.params_axis)
            is_full_cache.set_as(1)
        cached_n_number_fp = tik_inst.Scalar(dtype=Constant.DTYPE_FP32, name="cached_n_number_fp")

        idx_num_per_loop = indices_ub_size // self.indices_dsize
        idx_num_cur_loop = tik_inst.Scalar(dtype=self.tiling_dtype, name="idx_num_cur_loop")
        idx_num_cur_batch = tik_inst.Scalar(dtype=self.tiling_dtype, name="idx_num_cur_batch")

        if (
            PlatformApi.get_soc_spec(PlatformApi.SHORT_SOC_VERSION) == "Ascend910B"
            and self.indices_dtype == Constant.DTYPE_INT32
        ):
            self.create_pos_matrix(idx_num_per_loop, pos_matrix)
            tik_inst.scalar_conv("none", cached_n_number_fp, cached_n_number)
            tik_inst.vec_dup(64, dup_cached_num, cached_n_number_fp, 1, 8)

        def compute_for_once_ub_align(idx_loop_i, idx_batch_i):
            """
            compute for once ub align
            """
            indice_base_offset = tik_inst.Scalar(
                dtype=Constant.DTYPE_INT32, name="indice_base_offset", init_value=idx_batch_i * self.row_num_once_ub
            )

            with tik_inst.if_scope(is_full_cache == 1):
                unrool_num = 8
                index_array = self.tik_instance.ScalarArray(
                    dtype=self.tiling_dtype, length=unrool_num, name="index_array"
                )
                with self.tik_instance.for_range(0, idx_num_cur_batch // unrool_num) as idx_i:
                    with self.tik_instance.for_range(0, unrool_num) as unroll_idx:
                        index_array[unroll_idx].set_as(indices_ub[indice_base_offset + idx_i * unrool_num + unroll_idx])

                    with self.tik_instance.for_range(0, unrool_num) as unroll_idx:
                        tik_inst.data_move(
                            res_ub[(idx_i * unrool_num + unroll_idx) * row_size_align],
                            cached_ub[index_array[unroll_idx] * row_size_align],
                            0,
                            1,
                            params_block_num,
                            0,
                            0,
                        )
                with self.tik_instance.if_scope(idx_num_cur_batch % unrool_num > 0):
                    unrool_offset = idx_num_cur_batch // unrool_num * unrool_num
                    tail_num = idx_num_cur_batch % unrool_num
                    with self.tik_instance.for_range(0, tail_num) as unroll_idx:
                        index_array[unroll_idx].set_as(indices_ub[indice_base_offset + unrool_offset + unroll_idx])

                    with self.tik_instance.for_range(0, tail_num) as unroll_idx:
                        tik_inst.data_move(
                            res_ub[(unrool_offset + unroll_idx) * row_size_align],
                            cached_ub[index_array[unroll_idx] * row_size_align],
                            0,
                            1,
                            params_block_num,
                            0,
                            0,
                        )

            with tik_inst.else_scope():
                if (
                    PlatformApi.get_soc_spec(PlatformApi.SHORT_SOC_VERSION) == "Ascend910B"
                    and self.indices_dtype == Constant.DTYPE_INT32
                ):
                    number = tik_inst.Scalar(dtype=Constant.DTYPE_UINT32, name="number")
                    self.calc_indices_for_move_date(
                        indices_ub[indice_base_offset],
                        indices_ub_fp[indice_base_offset],
                        dup_cached_num,
                        pos_matrix,
                        idx_num_cur_batch,
                        out_indices,
                        out_pos_indices,
                        Constant.CMP_LESS_THAN,
                        number,
                    )
                    self.date_move_use_unroll(
                        out_indices,
                        out_pos_indices,
                        cached_ub,
                        number,
                        row_size_align,
                        self.params_row,
                        params_block_num,
                        res_ub,
                    )
                    self.calc_indices_for_move_date(
                        indices_ub[indice_base_offset],
                        indices_ub_fp[indice_base_offset],
                        dup_cached_num,
                        pos_matrix,
                        idx_num_cur_batch,
                        out_indices,
                        out_pos_indices,
                        Constant.CMP_GREAT_EQUAL,
                        number,
                    )
                    self.date_move_use_unroll(
                        out_indices,
                        out_pos_indices,
                        self.x[x_pre_offset],
                        number,
                        row_size_align,
                        self.params_row,
                        params_block_num,
                        res_ub,
                    )
                else:
                    with self.tik_instance.for_range(0, idx_num_cur_batch) as idx_i:
                        indices_value.set_as(indices_ub[indice_base_offset + idx_i])
                        with self.tik_instance.if_scope(indices_value < cached_n_number):
                            tik_inst.data_move(
                                res_ub[idx_i * row_size_align],
                                cached_ub[indices_value * row_size_align],
                                0,
                                1,
                                params_block_num,
                                0,
                                0,
                            )
                        with self.tik_instance.else_scope():
                            tik_inst.data_move(
                                res_ub[idx_i * row_size_align],
                                self.x[x_pre_offset + indices_value * self.params_row],
                                0,
                                1,
                                params_block_num,
                                0,
                                0,
                            )
            y_offset.set_as(
                y_pre_offset + (idx_loop_i * idx_num_per_loop + idx_batch_i * self.row_num_once_ub) * self.params_row
            )
            self.tik_instance.data_move(
                self.y[y_offset],
                res_ub,
                0,
                1,
                (idx_num_cur_batch * row_size_align * self.params_dsize) // constant.BLOCK_SIZE,
                0,
                0,
            )

        def compute_for_one_idx_loop_align(idx_loop_i):
            """
            compute for one idx loop
            """
            tik_inst.data_move(
                indices_ub,
                self.indices[idx_start + idx_loop_i * idx_num_per_loop],
                0,
                1,
                ceil_value(idx_num_cur_loop * self.indices_dsize, constant.BLOCK_SIZE),
                0,
                0,
            )
            if (
                PlatformApi.get_soc_spec(PlatformApi.SHORT_SOC_VERSION) == "Ascend910B"
                and self.indices_dtype == Constant.DTYPE_INT32
            ):
                tik_inst.h_cast(indices_ub_fp, indices_ub, "none")
            idx_num_cur_batch.set_as(self.row_num_once_ub)
            with tik_inst.for_range(0, idx_num_cur_loop // self.row_num_once_ub) as idx_batch_i:
                compute_for_once_ub_align(idx_loop_i, idx_batch_i)
            with tik_inst.if_scope(idx_num_cur_loop % self.row_num_once_ub != 0):
                idx_num_cur_batch.set_as(idx_num_cur_loop % self.row_num_once_ub)
                compute_for_once_ub_align(idx_loop_i, idx_num_cur_loop // self.row_num_once_ub)

        # cache n number to ub
        self.tik_instance.data_move(
            cached_ub[0],
            self.x,
            0,
            1,
            ceil_value(row_size * cached_n_number * self.params_dsize, constant.BLOCK_SIZE),
            0,
            0,
        )
        with tik_inst.for_range(0, self.params_pre) as pre_i:
            y_pre_offset.set_as((pre_i * self.indices_num + idx_start) * self.params_row)
            idx_num_cur_loop.set_as(idx_num_per_loop)
            with self.tik_instance.for_range(0, idx_num_cur_core // idx_num_per_loop) as idx_loop_i:
                compute_for_one_idx_loop_align(idx_loop_i)
            with tik_inst.if_scope(idx_num_cur_core % idx_num_per_loop != 0):
                idx_num_cur_loop.set_as(idx_num_cur_core % idx_num_per_loop)
                compute_for_one_idx_loop_align(idx_num_cur_core // idx_num_per_loop)

    # 'pylint:disable=E1136
    def params_row_aligned(self, indices_loop_offset, batch_i, block_id, pre_i, is_last):
        """
        process for params row is aligned
        """
        tik_instance = self.tik_instance
        if is_last:
            loop_num = self.inner_loop_num_last
            tail_num = self.row_num_last_tail_ub
        else:
            loop_num = self.inner_loop_num
            tail_num = self.row_num_once_tail_ub
        x_src = self.buffers.get("params_ub")
        indices_ub = self.buffers.get("indices_ub")
        res_ub = self.buffers.get("res_ub")
        burst_len_res = ceil_value(self.row_num_once_ub * self.params_row * self.params_dsize, constant.BLOCK_SIZE)
        burst_len_row = ceil_value(self.params_row * self.params_dsize, constant.BLOCK_SIZE)

        indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
        batch_base = block_id * self.params_batch_each_core
        if self.cached_types.get("cached_types_indices") == Constant.INDICES_CACHED_ALL and self.is_remaining == 0:
            indices_offset_base = (batch_i - batch_base) * self.indices_row
        else:
            indices_offset_base = 0
        if self.cached_types.get("cached_types_params") == Constant.PARAMS_CACHED_UB:
            params_batch_diff = (batch_i - batch_base) if self.is_remaining == 0 else 0
        else:
            params_batch_diff = batch_i
        params_offset_base = params_batch_diff * self.params_pre * self.params_axis + pre_i * self.params_axis
        output_offset_base = batch_i * self.params_pre * self.indices_row

        with tik_instance.for_range(0, loop_num) as inner_loop_i:
            inner_indices_offset = indices_offset_base + inner_loop_i * self.row_num_once_ub
            output_offset = (
                output_offset_base
                + pre_i * self.indices_row
                + (indices_loop_offset + inner_loop_i * self.row_num_once_ub)
            ) * self.params_row

            with tik_instance.new_stmt_scope(disable_sync=True):
                with tik_instance.for_range(0, self.row_num_once_ub, thread_num=2) as row_i:
                    indices_value.set_as(indices_ub[inner_indices_offset + row_i])
                    params_offset = (params_offset_base + indices_value) * self.params_row

                    tik_instance.data_move(
                        res_ub[row_i * self.params_row], x_src[params_offset], 0, 1, burst_len_row, 0, 0
                    )

            # move result data from ub to gm
            tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res, 0, 0)

        # process of the remaining part after block split
        with tik_instance.if_scope(tail_num > 0):
            burst_len_res = ceil_value(tail_num * self.params_row * self.params_dsize, constant.BLOCK_SIZE)
            inner_indices_offset = indices_offset_base + loop_num * self.row_num_once_ub
            output_offset = (
                output_offset_base + pre_i * self.indices_row + (indices_loop_offset + loop_num * self.row_num_once_ub)
            ) * self.params_row

            with tik_instance.new_stmt_scope(disable_sync=True):
                with tik_instance.for_range(0, tail_num, thread_num=2) as row_i:
                    indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
                    indices_value.set_as(indices_ub[inner_indices_offset + row_i])
                    params_offset = (params_offset_base + indices_value) * self.params_row

                    tik_instance.data_move(
                        res_ub[row_i * self.params_row], x_src[params_offset], 0, 1, burst_len_row, 0, 0
                    )

            # move result data from ub to gm
            tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res, 0, 0)

    def large_params_row(self, indices_loop_offset, batch_i, block_id, pre_i, is_last):
        """
        process for large params row
        """
        tik_instance = self.tik_instance
        loop_num = self.indices_row_num_last if is_last else self.indices_row_num_once
        indices_ub = self.buffers.get("indices_ub")
        res_ub = self.buffers.get("res_ub")

        half_ub_size = (self.ub_size - 2 * 1024) // 2
        half_ub_params_elem = half_ub_size // self.params_dsize
        burst_len_sub_row = ceil_value(half_ub_params_elem, self.block_elem)
        burst_len_sub_row_last = ceil_value(self.one_row_tail, self.block_elem)
        indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
        batch_base = block_id * self.params_batch_each_core
        if self.cached_types.get("cached_types_indices") == Constant.INDICES_CACHED_ALL and self.is_remaining == 0:
            indices_offset_base = (batch_i - batch_base) * self.indices_row
        else:
            indices_offset_base = 0
        params_offset_base = batch_i * self.params_pre * self.params_axis + pre_i * self.params_axis
        output_offset_base = batch_i * self.params_pre * self.indices_row
        inner_indices_offset = self.indices_row_num_once if is_last else 0

        with tik_instance.for_range(0, loop_num) as row_i:
            indices_value.set_as(indices_ub[indices_offset_base + row_i])
            params_offset = (params_offset_base + indices_value) * self.params_row
            output_offset = (
                output_offset_base + pre_i * self.indices_row + (indices_loop_offset + inner_indices_offset + row_i)
            ) * self.params_row

            # process the front part of one params_row: one_row_loop * half_ub_params_elem
            with tik_instance.for_range(0, self.one_row_loop) as row_inner_i:
                # move half_ub_params_elem data of one row to res_ub from gm
                tik_instance.data_move(
                    res_ub, self.x[params_offset + row_inner_i * half_ub_params_elem], 0, 1, burst_len_sub_row, 0, 0
                )
                # copy result data to gm from ub
                tik_instance.data_move(
                    self.y[output_offset + row_inner_i * half_ub_params_elem], res_ub, 0, 1, burst_len_sub_row, 0, 0
                )

            # process of one the tail part of params_row: one_row_tail
            with tik_instance.if_scope(self.one_row_tail > 0):
                # move one_row_tail data to res_ub from gm
                tik_instance.data_move(
                    res_ub,
                    self.x[params_offset + (self.params_row - self.one_row_tail)],
                    0,
                    1,
                    burst_len_sub_row_last,
                    0,
                    0,
                )
                # copy result data to gm from ub
                with tik_instance.if_scope(self.one_row_tail % self.block_elem != 0):
                    block_ub = tik_instance.Tensor(
                        self.params_dtype, (self.block_elem,), name="block_ub", scope=tik.scope_ubuf
                    )
                    with tik_instance.for_range(0, self.block_elem) as num_i:
                        block_ub[num_i].set_as(res_ub[self.one_row_tail - self.block_elem + num_i])

                    tik_instance.data_move(
                        self.y[output_offset + (self.params_row - self.one_row_tail)],
                        res_ub,
                        0,
                        1,
                        burst_len_sub_row_last - 1,
                        0,
                        0,
                    )
                    tik_instance.data_move(
                        self.y[output_offset + (self.params_row - self.block_elem)], block_ub, 0, 1, 1, 0, 0
                    )
                with tik_instance.else_scope():
                    tik_instance.data_move(
                        self.y[output_offset + (self.params_row - self.one_row_tail)],
                        res_ub,
                        0,
                        1,
                        burst_len_sub_row_last,
                        0,
                        0,
                    )

    # 'pylint:disable=E1136
    def small_indices_row(self, batch_i, block_id, is_last):
        """
        process for small indices row
        """
        tik_instance = self.tik_instance
        loop_num = self.inner_loop_num
        tail_num = self.row_num_once_tail_ub // self.indices_row
        tail_num = (tail_num + self.params_batch_remaining) if is_last else tail_num
        indices_ub = self.buffers.get("indices_ub")
        params_ub = self.buffers.get("params_ub")
        res_ub = self.buffers.get("res_ub")
        burst_len_res = ceil_value(self.row_num_once_ub * self.params_row * self.params_dsize, constant.BLOCK_SIZE)
        output_offset_base = batch_i * self.params_pre * self.indices_row + block_id * self.indices_num_each_core
        indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
        params_batch_diff = batch_i
        indices_row_num = self.row_num_once_ub // self.indices_row

        with tik_instance.for_range(0, loop_num) as inner_loop_i:
            output_offset = (output_offset_base + inner_loop_i * self.row_num_once_ub) * self.params_row
            params_offset_base = (
                params_batch_diff * self.params_pre * self.params_axis
                + block_id * self.indices_num_each_core
                + inner_loop_i * indices_row_num * self.params_axis
            )
            with tik_instance.for_range(0, indices_row_num, thread_num=2) as row_i:
                with tik_instance.for_range(0, self.indices_row) as ele_i:
                    indices_value.set_as(indices_ub[ele_i])
                    params_offset = (params_offset_base + row_i * self.params_axis + indices_value) * self.params_row
                    res_ub_offset = (row_i * self.indices_row + ele_i) * self.params_row

                    if self.cached_types.get("cached_types_params") == Constant.PARAMS_CACHED_UB:
                        with tik_instance.for_range(0, self.params_row) as i:
                            res_ub[res_ub_offset + i].set_as(params_ub[params_offset + i])
                    else:
                        temp_ub_1 = tik_instance.Tensor(
                            self.params_dtype, (self.block_elem,), name="temp_ub_1", scope=tik.scope_ubuf
                        )
                        tik_instance.data_move(temp_ub_1[0], params_ub[params_offset], 0, 1, 1, 0, 0)
                        with tik_instance.for_range(0, self.params_row) as i:
                            res_ub[res_ub_offset + i].set_as(temp_ub_1[i])

            # copy result data from ub to gm
            tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res, 0, 0)

        with tik_instance.if_scope(tail_num > 0):
            burst_len_res = ceil_value(
                tail_num * self.indices_row * self.params_row * self.params_dsize, constant.BLOCK_SIZE
            )
            output_offset = (output_offset_base + loop_num * self.row_num_once_ub) * self.params_row
            params_offset_base = (
                params_batch_diff * self.params_pre * self.params_axis
                + block_id * self.indices_num_each_core
                + loop_num * indices_row_num * self.params_axis
            )
            with tik_instance.for_range(0, tail_num, thread_num=2) as row_i:
                with tik_instance.for_range(0, self.indices_row) as ele_i:
                    indices_value.set_as(indices_ub[ele_i])
                    params_offset = (params_offset_base + row_i * self.params_axis + indices_value) * self.params_row
                    res_ub_offset = (row_i * self.indices_row + ele_i) * self.params_row
                    if self.cached_types.get("cached_types_params") == Constant.PARAMS_CACHED_UB:
                        with tik_instance.for_range(0, self.params_row) as i:
                            res_ub[res_ub_offset + i].set_as(params_ub[params_offset + i])
                    else:
                        temp_ub_1 = tik_instance.Tensor(
                            self.params_dtype, (self.block_elem,), name="temp_ub_1", scope=tik.scope_ubuf
                        )
                        tik_instance.data_move(temp_ub_1[0], params_ub[params_offset], 0, 1, 1, 0, 0)
                        with tik_instance.for_range(0, self.params_row) as i:
                            res_ub[res_ub_offset + i].set_as(temp_ub_1[i])

            tail_elem = (tail_num * self.indices_row * self.params_row) % self.block_elem
            with tik_instance.if_scope(tik.all(tail_elem != 0, burst_len_res > 1)):
                block_ub = tik_instance.Tensor(
                    self.params_dtype, (self.block_elem,), name="block_ub", scope=tik.scope_ubuf
                )
                with tik_instance.for_range(0, self.block_elem) as num_i:
                    block_ub[num_i].set_as(
                        res_ub[tail_num * self.indices_row * self.params_row - self.block_elem + num_i]
                    )

                tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res - 1, 0, 0)
                tik_instance.data_move(
                    self.y[output_offset + (tail_num * self.indices_row * self.params_row - self.block_elem)],
                    block_ub,
                    0,
                    1,
                    1,
                    0,
                    0,
                )
            with tik_instance.else_scope():
                tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res, 0, 0)

    # 'pylint:disable=E1136
    def small_params_row(self, block_id, is_last):
        """
        process for small params row
        """
        tik_instance = self.tik_instance
        loop_num = self.inner_loop_num
        tail_num = self.row_num_once_tail_ub // self.params_pre // self.indices_row
        tail_num = (tail_num + self.params_batch_remaining) if is_last else tail_num
        indices_ub = self.buffers.get("indices_ub")
        params_ub = self.buffers.get("params_ub")
        res_ub = self.buffers.get("res_ub")
        batch_base = block_id * self.params_batch_each_core
        burst_len_res = ceil_value(self.row_num_once_ub * self.params_row * self.params_dsize, constant.BLOCK_SIZE)
        output_offset_base = batch_base * self.params_pre * self.indices_row
        indices_value = tik_instance.Scalar(dtype=self.indices_dtype, name="indices_value", init_value=0)
        if self.cached_types.get("cached_types_params") != Constant.PARAMS_CACHED_UB:
            params_batch_diff = batch_base
        else:
            params_batch_diff = 0
        batch_num = self.row_num_once_ub // self.params_pre // self.indices_row

        with tik_instance.for_range(0, loop_num) as inner_loop_i:
            output_offset = (output_offset_base + inner_loop_i * self.row_num_once_ub) * self.params_row

            with tik_instance.for_range(0, batch_num) as batch_i:
                params_offset_base = (
                    (params_batch_diff + inner_loop_i * batch_num + batch_i) * self.params_pre * self.params_axis
                )
                indices_num_offset = (batch_base + inner_loop_i * batch_num + batch_i) * self.indices_row
                tik_instance.data_move(
                    self.buffers.get("indices_ub"),
                    self.indices[indices_num_offset],
                    0,
                    1,
                    ceil_value(self.indices_row * self.indices_dsize, constant.BLOCK_SIZE),
                    0,
                    0,
                )
                with tik_instance.for_range(0, self.params_pre) as pre_i:
                    with tik_instance.for_range(0, self.indices_row) as ele_i:
                        indices_value.set_as(indices_ub[ele_i])
                        params_offset = (
                            params_offset_base + pre_i * self.params_axis + indices_value
                        ) * self.params_row
                        res_ub_offset = (
                            (batch_i * self.params_pre + pre_i) * self.indices_row + ele_i
                        ) * self.params_row

                        if self.cached_types.get("cached_types_params") == Constant.PARAMS_CACHED_UB:
                            with tik_instance.for_range(0, self.params_row) as i:
                                res_ub[res_ub_offset + i].set_as(params_ub[params_offset + i])
                        else:
                            temp_ub_1 = tik_instance.Tensor(
                                self.params_dtype, (self.block_elem,), name="temp_ub_1", scope=tik.scope_ubuf
                            )
                            tik_instance.data_move(temp_ub_1[0], params_ub[params_offset], 0, 1, 1, 0, 0)
                            with tik_instance.for_range(0, self.params_row) as i:
                                res_ub[res_ub_offset + i].set_as(temp_ub_1[i])

            # copy result data from ub to gm
            tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res, 0, 0)

        with tik_instance.if_scope(tail_num > 0):
            burst_len_res = ceil_value(
                tail_num * self.params_pre * self.indices_row * self.params_row * self.params_dsize, constant.BLOCK_SIZE
            )
            output_offset = (output_offset_base + loop_num * self.row_num_once_ub) * self.params_row
            with tik_instance.for_range(0, tail_num) as batch_i:
                params_offset_base = (
                    (params_batch_diff + loop_num * batch_num + batch_i) * self.params_pre * self.params_axis
                )
                indices_num_offset = (batch_base + loop_num * batch_num + batch_i) * self.indices_row
                tik_instance.data_move(
                    self.buffers.get("indices_ub"),
                    self.indices[indices_num_offset],
                    0,
                    1,
                    ceil_value(self.indices_row * self.indices_dsize, constant.BLOCK_SIZE),
                    0,
                    0,
                )
                with tik_instance.for_range(0, self.params_pre) as pre_i:
                    with tik_instance.for_range(0, self.indices_row) as ele_i:
                        indices_value.set_as(indices_ub[ele_i])
                        params_offset = (
                            params_offset_base + pre_i * self.params_axis + indices_value
                        ) * self.params_row
                        res_ub_offset = (
                            (batch_i * self.params_pre + pre_i) * self.indices_row + ele_i
                        ) * self.params_row
                        if self.cached_types.get("cached_types_params") == Constant.PARAMS_CACHED_UB:
                            with tik_instance.for_range(0, self.params_row) as i:
                                res_ub[res_ub_offset + i].set_as(params_ub[params_offset + i])
                        else:
                            temp_ub_1 = tik_instance.Tensor(
                                self.params_dtype, (self.block_elem,), name="temp_ub_1", scope=tik.scope_ubuf
                            )
                            tik_instance.data_move(temp_ub_1[0], params_ub[params_offset], 0, 1, 1, 0, 0)
                            with tik_instance.for_range(0, self.params_row) as i:
                                res_ub[res_ub_offset + i].set_as(temp_ub_1[i])

            tail_elem = (tail_num * self.params_pre * self.indices_row * self.params_row) % self.block_elem
            with tik_instance.if_scope(tik.all(tail_elem != 0, burst_len_res > 1)):
                block_ub = tik_instance.Tensor(
                    self.params_dtype, (self.block_elem,), name="block_ub", scope=tik.scope_ubuf
                )
                with tik_instance.for_range(0, self.block_elem) as num_i:
                    block_ub[num_i].set_as(
                        res_ub[
                            tail_num * self.params_pre * self.indices_row * self.params_row - self.block_elem + num_i
                        ]
                    )

                tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res - 1, 0, 0)
                tik_instance.data_move(
                    self.y[
                        output_offset
                        + (tail_num * self.params_pre * self.indices_row * self.params_row - self.block_elem)
                    ],
                    block_ub,
                    0,
                    1,
                    1,
                    0,
                    0,
                )
            with tik_instance.else_scope():
                tik_instance.data_move(self.y[output_offset], res_ub, 0, 1, burst_len_res, 0, 0)

    def inner_loop_with_batch_dims(self, indices_loop_offset, batch_i, block_id, pre_i, is_last):
        """
        inner loop of the func:compute_with_batch_dims
        """
        if self.cached_types.get("aligned_types_params") == Constant.MODE_LESS_THAN_32B:
            self.params_row_less_than_32b(indices_loop_offset, batch_i, block_id, pre_i, is_last)
        elif self.cached_types.get("aligned_types_params") == Constant.MODE_ALIGNED:
            self.params_row_aligned(indices_loop_offset, batch_i, block_id, pre_i, is_last)
        elif self.cached_types.get("aligned_types_params") == Constant.MODE_MORE_THAN_32B:
            self.params_row_more_than_32b(indices_loop_offset, batch_i, block_id, pre_i, is_last)
        else:
            self.large_params_row(indices_loop_offset, batch_i, block_id, pre_i, is_last)

    def process_large_indices_row(self, indices_num_offset, batch_i, block_id, pre_i):
        """
        process_large_indices_row
        """
        tik_instance = self.tik_instance
        with tik_instance.for_range(0, self.indices_loop_num) as indices_loop_i:
            indices_loop_offset = indices_loop_i * self.indices_row_num_once
            indices_offset = indices_num_offset + indices_loop_offset
            tik_instance.data_move(
                self.buffers.get("indices_ub"),
                self.indices[indices_offset],
                0,
                1,
                ceil_value(self.indices_row_num_once * self.indices_dsize, constant.BLOCK_SIZE),
                0,
                0,
            )
            self.inner_loop_with_batch_dims(indices_loop_offset, batch_i, block_id, pre_i, 0)
        with tik_instance.if_scope(self.indices_row_num_last > 0):
            indices_loop_offset = self.indices_loop_num * self.indices_row_num_once
            indices_offset = indices_num_offset + indices_loop_offset
            tik_instance.data_move(
                self.buffers.get("indices_ub"),
                self.indices[indices_offset],
                0,
                1,
                ceil_value(self.indices_row_num_last * self.indices_dsize, constant.BLOCK_SIZE),
                0,
                0,
            )
            self.inner_loop_with_batch_dims(indices_loop_offset, batch_i, block_id, pre_i, 1)

    # 'pylint:disable=E1136
    def compute_with_batch_dims(self, avl_ub_size, block_id):
        """
        compute for tiling mode with batch_dims
        """
        tik_instance = self.tik_instance
        self.buffers["indices_ub"] = tik_instance.Tensor(
            self.indices_dtype,
            ((avl_ub_size + constant.BLOCK_SIZE) // self.indices_dsize,),
            name="indices_ub",
            scope=tik.scope_ubuf,
        )
        self.buffers["res_ub"] = tik_instance.Tensor(
            self.params_dtype,
            ((avl_ub_size + constant.BLOCK_SIZE) // self.params_dsize,),
            name="res_ub",
            scope=tik.scope_ubuf,
        )

        indices_num_offset = block_id * self.indices_num_each_core
        if self.cached_types.get("cached_types_indices") == Constant.INDICES_CACHED_ALL:
            tik_instance.data_move(
                self.buffers.get("indices_ub"),
                self.indices[indices_num_offset],
                0,
                1,
                ceil_value(self.indices_num_each_core * self.indices_dsize, constant.BLOCK_SIZE),
                0,
                0,
            )

        if self.cached_types.get("cached_types_params") == Constant.PARAMS_CACHED_UB:
            self.buffers["params_ub"] = tik_instance.Tensor(
                self.params_dtype,
                (Constant.CACHE_UB_SIZE // self.params_dsize,),
                name="params_ub",
                scope=tik.scope_ubuf,
            )
            if self.cached_types.get("cached_types_indices") != Constant.INDICES_SMALL_ROW:
                params_offset = self.params_total * block_id
            else:
                params_offset = 0
            tik_instance.data_move(
                self.buffers.get("params_ub"),
                self.x[params_offset],
                0,
                1,
                ceil_value(self.params_total, self.block_elem),
                0,
                0,
            )
        else:
            self.buffers["params_ub"] = self.x

        if self.cached_types.get("cached_types_indices") == Constant.INDICES_SMALL_ROW:
            with tik_instance.for_range(0, self.params_batch) as batch_i:
                indices_num_offset = batch_i * self.indices_row
                tik_instance.data_move(
                    self.buffers.get("indices_ub"),
                    self.indices[indices_num_offset],
                    0,
                    1,
                    ceil_value(self.indices_row * self.indices_dsize, constant.BLOCK_SIZE),
                    0,
                    0,
                )
                with tik_instance.if_scope(block_id < self.tail_process_core):
                    self.small_indices_row(batch_i, block_id, 0)
                with tik_instance.if_scope(block_id == self.tail_process_core):
                    self.small_indices_row(batch_i, block_id, 1)
            return

        if self.cached_types.get("aligned_types_params") == Constant.MODE_SMALL_PARAMS_ROW:
            with tik_instance.if_scope(block_id < self.tail_process_core):
                self.small_params_row(block_id, 0)
            with tik_instance.if_scope(block_id == self.tail_process_core):
                self.small_params_row(block_id, 1)
            return

        range_left = block_id * self.params_batch_each_core
        range_right = (block_id + 1) * self.params_batch_each_core
        with tik_instance.for_range(range_left, range_right) as batch_i:
            self.is_remaining = 0
            with tik_instance.for_range(0, self.params_pre) as pre_i:
                if self.cached_types.get("cached_types_indices") != Constant.INDICES_LARGE_ROW:
                    if self.cached_types.get("cached_types_indices") == Constant.INDICES_CACHED_ONE_ROW:
                        indices_num_offset = batch_i * self.indices_row
                        tik_instance.data_move(
                            self.buffers.get("indices_ub"),
                            self.indices[indices_num_offset],
                            0,
                            1,
                            ceil_value(self.indices_row * self.indices_dsize, constant.BLOCK_SIZE),
                            0,
                            0,
                        )
                    self.inner_loop_with_batch_dims(0, batch_i, block_id, pre_i, 0)
                else:
                    indices_num_offset = batch_i * self.indices_row
                    self.process_large_indices_row(indices_num_offset, batch_i, block_id, pre_i)
        with tik_instance.if_scope(tik.all(self.params_batch_remaining > 0, block_id < self.params_batch_remaining)):
            self.is_remaining = 1
            batch_i = self.need_core_num * self.params_batch_each_core + block_id
            if self.cached_types.get("cached_types_params") == Constant.PARAMS_CACHED_UB:
                num_per_batch = self.params_pre * self.params_axis * self.params_row
                params_offset = self.params_total * self.need_core_num + block_id * num_per_batch
                tik_instance.data_move(
                    self.buffers.get("params_ub"),
                    self.x[params_offset],
                    0,
                    1,
                    ceil_value(num_per_batch, self.block_elem),
                    0,
                    0,
                )
            else:
                self.buffers["params_ub"] = self.x
            indices_num_offset = self.need_core_num * self.indices_num_each_core + block_id * self.indices_row
            if self.cached_types.get("cached_types_indices") != Constant.INDICES_LARGE_ROW:
                tik_instance.data_move(
                    self.buffers.get("indices_ub"),
                    self.indices[indices_num_offset],
                    0,
                    1,
                    ceil_value(self.indices_row * self.indices_dsize, constant.BLOCK_SIZE),
                    0,
                    0,
                )
                with tik_instance.for_range(0, self.params_pre) as pre_i:
                    self.inner_loop_with_batch_dims(0, batch_i, block_id, pre_i, 0)
            else:
                with tik_instance.for_range(0, self.params_pre) as pre_i:
                    self.process_large_indices_row(indices_num_offset, batch_i, block_id, pre_i)

    def add_gather_compile_info(self):
        """
        add compile info
        """
        self.x = self.tik_instance.Tensor(self.params_dtype, self.x_shape, name="x", scope=tik.scope_gm)
        self.indices = self.tik_instance.Tensor(
            self.indices_dtype, self.indices_shape, name="indices", scope=tik.scope_gm
        )
        self.tiling_gm = self.tik_instance.Tensor(
            self.tiling_dtype, (Constant.TILING_ARG_NUM,), name="ddr_arg", scope=tik.scope_gm
        )
        self.y = self.tik_instance.Tensor(self.y_dtype, shape=self.y_shape, name="y", scope=tik.scope_gm)
        impl_mode_value = 1 if self.impl_mode == OpImplMode.HIGH_PERFORMANCE else 0

        # add compile info
        tbe_context.get_context().add_compile_info(
            "vars",
            {
                "core_num": self.core_num,
                "ub_size": self.ub_size,
                "l1_size": self.l1_size,
                "params_dsize": self.params_dsize,
                "indices_dsize": self.indices_dsize,
                "impl_mode": impl_mode_value,
                "is_preprocessed": self.is_preprocessed,
            },
        )
        # It is used to distinguish between Tik implementation and DSL implementation in the tilling phase
        tbe_context.get_context().add_compile_info("is_tik", True)

        if self.axis:
            self.tik_instance.set_tiling_params([self.x, self.indices, self.axis], self.tiling_gm)
        else:
            self.tik_instance.set_tiling_params([self.x, self.indices], self.tiling_gm)
        self.gather_v2_compute_tiling()

    def gather_v2_compute(self):
        """
        compute of gather_v2

        Parameters
        ----------
        None

        Returns
        -------
        compile info
        """
        self.axis = self.tik_instance.Tensor(self.axis_dtype, self.axis_shape, name="axis", scope=tik.scope_gm)
        self.add_gather_compile_info()
        self.tik_instance.BuildCCE(
            kernel_name=self.kernel_name,
            inputs=(self.x, self.indices, self.axis),
            outputs=(self.y,),
            flowtable=(self.tiling_gm,),
            enable_l2=True,
            config=self.opt_config,
        )

    def gather_compute(self):
        """
        compute of gather

        Parameters
        ----------
        None

        Returns
        -------
        compile info
        """
        self.add_gather_compile_info()
        self.tik_instance.BuildCCE(
            kernel_name=self.kernel_name,
            inputs=(self.x, self.indices),
            outputs=(self.y,),
            flowtable=(self.tiling_gm,),
            enable_l2=True,
            config=self.opt_config,
        )

    def _set_mode_paras(self, aligned_types_params, cached_types_params, cached_types_indices):
        self.cached_types["aligned_types_params"] = aligned_types_params
        self.cached_types["cached_types_params"] = cached_types_params
        self.cached_types["cached_types_indices"] = cached_types_indices


def gather_v2_tik(
    x,
    indices,
    axis,
    y,
    batch_dims=0,
    is_preprocessed=False,
    kernel_name="GatherV2",
    impl_mode=OpImplMode.HIGH_PRECISION,
):
    """
    gather_v2 interface for tik
    """
    tbe_context.get_context().add_compile_info("is_gather_v2", True)
    obj = GatherV2(x, indices, axis, y, batch_dims, is_preprocessed, kernel_name, impl_mode)
    return obj.gather_v2_compute()


# 'pylint: disable=locally-disabled,invalid-name,unused-argument,too-many-arguments
def check_supported(
    x,
    indices,
    axis,
    y,
    batch_dims=0,
    is_preprocessed=False,
    negative_index_support=False,
    kernel_name="GatherV2",
    impl_mode=OpImplMode.HIGH_PRECISION,
):
    """
    Judge whether the current input specification supports
    """
    if tbe_platform_adapter.api_check_support("tik.vcopy"):
        return True, ""

    shape_x = x.get("ori_shape")
    shape_indices = indices.get("ori_shape")

    shape_x_list = [(7709, 512), (17191, 512)]
    shape_indices_list = [(1,), (128,), (64, 128)]

    if shape_x in shape_x_list and shape_indices in shape_indices_list:
        reason = "shape in bad-performance list."
        return False, reason

    return True, ""


# 'pylint: disable=too-many-instance-attributes
def gather_v2_compute(
    x, indices, axis, y, batch_dims=0, negative_index_support=False, kernel_name="GatherV2", impl_mode=None
):
    """
    gather_v2 compute

    Parameters
    ----------
    x: input params shape, dtype and range
    indices: input indices shape, dtype and range
    axis: input axis shape, dtype and range
    y: output shape, dtype and range
    batch_dims: the number of batch dimensions
    kernel_name: kernel name of gather_v2 op
    impl_mode: implement mode

    Returns
    -------
    res: TVM tensor
        the result of gather
    """
    support_out_of_bound_index = True if impl_mode == "support_out_of_bound_index" else False

    res = tbe.gather(x, indices, axis, batch_dims, negative_index_support, support_out_of_bound_index)

    return res


def gather_v2_dsl(
    x, indices, axis, y, batch_dims=0, negative_index_support=False, kernel_name="GatherV2", impl_mode=None
):
    """
    gather_v2 interface for dsl
    """
    check_list_x = (
        "bfloat16",
        "float16",
        "float32",
        "int8",
        "uint8",
        "int32",
        "uint32",
        "int16",
        "uint16",
        "int64",
        "uint64",
        "bool",
    )
    check_list_indices = ("int32", "int64")
    dtype_x = x.get("dtype").lower()
    dtype_indices = indices.get("dtype").lower()
    dtype_axis = axis.get("dtype").lower()
    para_check.check_dtype(dtype_x, check_list_x, param_name="x")
    para_check.check_dtype(dtype_indices, check_list_indices, param_name="indices")
    para_check.check_dtype(dtype_axis, check_list_indices, param_name="axis")

    if "const_value" in axis:
        axis_value = axis.get("const_value")
        if isinstance(axis_value, int):
            real_axis = axis_value
        else:
            real_axis = axis_value[0]
    else:
        real_axis = "unknown"
    batch_dims = "unknown" if batch_dims is None else batch_dims
    tbe_context.get_context().add_compile_info("attr_name", "batch_dims")
    tbe_context.get_context().add_compile_info("batch_dims_attr_idx", 0)

    ins = classify([x, indices, real_axis, batch_dims], OpPatternMode.GATHER, {"gather_type": "gather"})
    schedules, tensors = [], []
    for shape_x, shape_indices, shape_axis, batch_dims_input in ins:
        with tbe.compute():
            x_var, indices_var, axis_dim, batch_dims = shape_util.variable_shape(
                [shape_x, shape_indices, shape_axis, batch_dims_input], "gather"
            )
            x_tensor = tvm.placeholder(x_var, name="x", dtype=dtype_x)
            indices_tensor = tvm.placeholder(indices_var, name="indices", dtype=dtype_indices)
            axis_tensor = tvm.placeholder([1], name="axis", dtype=dtype_axis)
            res = gather_v2_compute(
                x_tensor, indices_tensor, axis_dim, y, batch_dims, negative_index_support, kernel_name, impl_mode
            )
            tensors.append([x_tensor, indices_tensor, axis_tensor, res])
        with tvm.target.cce():
            sch = tbe.auto_schedule(res)
        schedules.append(sch)
    config = {"name": kernel_name, "tensor_list": tensors}
    tbe.build(schedules, config)


def support_l1_buffer(params_shape):
    # Take the tik branch to achieve better performance when params can be cached in L1 buffer.
    support_shapes = [[200000, 1], [184965, 1]]
    if PlatformApi.get_soc_spec(PlatformApi.SHORT_SOC_VERSION) == "Ascend910":
        return params_shape in support_shapes
    return False


@register_operator("GatherV2")
@para_check.check_op_params(
    para_check.REQUIRED_INPUT,
    para_check.REQUIRED_INPUT,
    para_check.REQUIRED_INPUT,
    para_check.REQUIRED_OUTPUT,
    para_check.OPTION_ATTR_INT,
    para_check.OPTION_ATTR_BOOL,
    para_check.OPTION_ATTR_BOOL,
    para_check.KERNEL_NAME,
    para_check.OPTION_ATTR_STR,
)
def gather_v2(
    x,
    indices,
    axis,
    y,
    batch_dims=0,
    is_preprocessed=False,
    negative_index_support=False,
    kernel_name="GatherV2",
    impl_mode=OpImplMode.HIGH_PRECISION,
):
    """
    gather_v2 interface

    Parameters
    ----------
    x: input params shape, dtype and range
    indices: input indices shape, dtype and range
    axis: input axis shape, dtype and range
    y: output shape, dtype and range
    batch_dims: the number of batch dimensions
    is_preprocessed: The flag of cpu preprocess high heat date, false is enble the function
    kernel_name: kernel name of gather_v2 op
    impl_mode: str. The flag for cache data at index 0. No need to add into ops_info file. Tempoarily support
               high_performance, high_precision and support_out_of_bound_index.

    Returns
    -------
    None
    """
    check_op_impl_mode(
        impl_mode,
        [OpImplMode.HIGH_PERFORMANCE, OpImplMode.HIGH_PRECISION, OpImplMode.SUPPORT_OUT_OF_BOUND_INDEX],
        kernel_name,
    )
    op_infos = get_context().get_op_info()

    if x.get("dtype").lower() == "bfloat16":
        gather_v2_dsl(x, indices, axis, y, batch_dims, negative_index_support, kernel_name, impl_mode)
    elif not tbe_platform_adapter.api_check_support("tbe.dsl.vexp", "float32"):
        # 310 use the old path or the performance will down
        is_preprocessed = True
        gather_v2_tik(x, indices, axis, y, batch_dims, is_preprocessed, kernel_name, impl_mode)
    elif support_l1_buffer(list(x.get("shape"))):
        gather_v2_tik(x, indices, axis, y, batch_dims, is_preprocessed, kernel_name, impl_mode)
    elif impl_mode == OpImplMode.HIGH_PERFORMANCE:
        if (is_preprocessed is False and axis.__contains__("const_value") is False) or (
            is_preprocessed is False and axis.__contains__("const_value") is True and axis["const_value"][0] == 0
        ):
            gather_v2_tik(x, indices, axis, y, batch_dims, is_preprocessed, kernel_name, impl_mode)
        elif axis.__contains__("const_value") and axis["const_value"][0] != 0:
            gather_v2_dsl(x, indices, axis, y, batch_dims, negative_index_support, kernel_name, impl_mode)
        else:
            gather_v2_tik(x, indices, axis, y, batch_dims, is_preprocessed, kernel_name, impl_mode)
    elif op_infos:
        for op_info in op_infos:
            if op_info.op_type == "GatherV2":
                if op_info.extra_params.get("op_impl_switch") == "tik":
                    gather_v2_tik(x, indices, axis, y, batch_dims, is_preprocessed, kernel_name, impl_mode)
                    break
                else:
                    gather_v2_dsl(x, indices, axis, y, batch_dims, negative_index_support, kernel_name, impl_mode)
                    break
    else:
        gather_v2_dsl(x, indices, axis, y, batch_dims, negative_index_support, kernel_name, impl_mode)
