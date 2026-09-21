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
related classes of tbe register:
"""

from enum import Enum
from enum import IntEnum


# register function is executed at which stage
# Currently only STAGE_BUILD is supported
class InvokeStage(Enum):
    """
    function executed stage
    """

    STAGE_BUILD = 0


# function executed priority: A smaller value indicates a higher priority.
class Priority(IntEnum):
    """
    function executed priority
    """

    HIGH = 0
    NORMAL = 1
    LOW = 2


# 'pylint: disable=useless-object-inheritance
class FusionPassItem(object):
    """
    UB fusion pass
    """

    def __init__(self, func_name, stage, priority, func):
        self._func_name = func_name
        self._stage = stage
        self._priority = priority
        self._func = func
        self._op_type = None
        self._op_pattern = None
        self._op_type_list = []
        self._op_pattern_list = []

    def get_stage(self):
        """
        :return:
        """
        return self._stage

    def get_priority(self):
        """
        :return:
        """
        return self._priority

    def get_func(self):
        """
        :return:
        """
        return self._func

    def get_func_name(self):
        """
        get func name

        Parameters
        ----------

        Returns
        -------
        func_name : str
            func name
        """
        return self._func_name

    def set_op_type(self, op_type):
        """
        set op type

        Parameters
        ----------
        op_type : str
            op type

        Returns
        -------
        """
        if op_type is not None:
            self._op_type = op_type.strip()

    def get_op_type(self):
        """
        get op type

        Parameters
        ----------

        Returns
        -------
        op_type : str
            op type
        """
        return self._op_type

    def set_op_pattern(self, op_pattern):
        """
        set op pattern

        Parameters
        ----------
        op_pattern : str
            op pattern

        Returns
        -------
        """
        if op_pattern is not None:
            self._op_pattern = op_pattern.strip()

    def get_op_pattern(self):
        """
        get op pattern

        Parameters
        ----------

        Returns
        -------
        op_pattern : str
            op pattern
        """
        return self._op_pattern

    def set_op_type_list(self, op_type_str):
        """
        set op pattern

        Parameters
        ----------
        op_pattern : str
            op pattern

        Returns
        -------
        """
        if op_type_str is not None:
            self._op_type_list = op_type_str.split(",")
            for index, op_type_val in enumerate(self._op_type_list):
                self._op_type_list[index] = op_type_val.strip()

    def get_op_type_list(self):
        """
        get op type list

        Parameters
        ----------

        Returns
        -------
        op_type_list : str
            op type list
        """
        return self._op_type_list

    def set_op_pattern_list(self, op_pattern_str):
        """
        set op pattern list

        Parameters
        ----------
        op_pattern_list : list
            op pattern list

        Returns
        -------
        """
        if op_pattern_str is not None:
            self._op_pattern_list = op_pattern_str.split(",")
            for index, op_pattern_val in enumerate(self._op_pattern_list):
                self._op_pattern_list[index] = op_pattern_val.strip()

    def get_op_pattern_list(self):
        """
        get op pattern list

        Parameters
        ----------

        Returns
        -------
        op_pattern_list : list
            op pattern list
        """
        return self._op_pattern_list


class OpCompute(object):
    """
    op compute func info
    """

    def __init__(self, support_fusion, func):
        self._support_fusion = support_fusion
        self._func = func

    def if_support_fusion(self):
        """
        :return:
        """
        return self._support_fusion

    def get_func(self):
        """
        :return:
        """
        return self._func


# 'pylint: disable=useless-object-inheritance
class Operator(object):
    """
    op info
    """

    def __init__(self, pattern, func):
        self._pattern = pattern
        self._func = func

    def get_pattern(self):
        """
        :return:
        """
        return self._pattern

    def get_func(self):
        """
        :return:
        """
        return self._func


class OpClassify(object):
    """
    op classify_func object
    """

    def __init__(self, key, support_type, func):
        self._key = key
        self._type = support_type
        self._func = func

    def get_key(self):
        """
        :return: registered key, pattern or op_type
        """
        return self._key

    def get_type(self):
        """
        :return: registered type
        """
        return self._type

    def get_func(self):
        """
        :return:return: registered classify func
        """
        return self._func


class TensorInfo(object):
    def __init__(self):
        self._id = None
        self._format = None
        self._dtype = None
        self._shape = []
        self._range = []
        self._ori_format = None
        self._ori_dtype = None
        self._ori_shape = []
        self._ori_range = []
        self._is_shape_null = False
        self._only_update_data = False

    def get_id(self):
        return self._id

    def get_format(self):
        return self._format

    def get_dtype(self):
        return self._dtype

    def get_shape(self):
        return self._shape

    def is_shape_null(self):
        return self._is_shape_null

    def get_range(self):
        return self._range

    def get_ori_format(self):
        return self._ori_format

    def get_ori_dtype(self):
        return self._ori_dtype

    def get_ori_shape(self):
        return self._ori_shape

    def get_ori_range(self):
        return self._ori_range

    def set_format(self, format):
        self._format = format

    def set_dtype(self, format):
        self._dtype = format

    def set_shape(self, shape):
        self._shape = shape

    def set_range(self, range):
        self._range = range

    def set_ori_format(self, ori_format):
        self._ori_format = ori_format

    def set_ori_dtype(self, ori_dtype):
        self._ori_dtype = ori_dtype

    def set_ori_shape(self, ori_shape):
        self._ori_shape = ori_shape

    def set_ori_range(self, ori_range):
        self._ori_range = ori_range

    def set_only_update_data(self):
        self._only_update_data = True

    def is_only_update_data(self):
        return self._only_update_data

    @staticmethod
    def get_shape_str(shape):
        if shape is None or len(shape) == 0:
            return "empty"
        shape_str = ""
        is_first_dim = True
        for dim in shape:
            if is_first_dim:
                is_first_dim = False
            else:
                shape_str = shape_str + ", "
            shape_str = shape_str + str(dim)
        return shape_str

    @staticmethod
    def get_range_str(range):
        if range is None or len(range) == 0:
            return "empty"
        range_str = ""
        is_first_item = True
        for item in range:
            if is_first_item:
                is_first_item = False
            else:
                range_str = range_str + ", "
            range_str = range_str + "(" + TensorInfo.get_shape_str(item) + ")"
        return range_str

    def display(self, is_input=False):
        import asc_op_compile_base.common.utils.log as logger

        prefix = "Input" if is_input else "Output"
        logger.debug("[%s] info: id[%s]", prefix, self._id)
        logger.debug(
            "Data type[%s], format[%s], shape[%s], range[%s]",
            self._dtype,
            self._format,
            TensorInfo.get_shape_str(self._shape),
            TensorInfo.get_range_str(self._range),
        )
        logger.debug(
            "Origin data type[%s], origin format[%s], origin shape[%s], origin range[%s]",
            self._ori_dtype,
            self._ori_format,
            TensorInfo.get_shape_str(self._ori_shape),
            TensorInfo.get_range_str(self._ori_range),
        )


class InputInfo(object):
    def __init__(self):
        self._edge_id = None  # peer output id
        self._edge_type = None  # Data, Operator
        self._edge_list = []
        self._param_name = None

    def get_edge_id(self):
        return self._edge_id

    def get_edge_type(self):
        return self._edge_type

    def is_data_edge(self):
        return self._edge_type == "Data"

    def display(self):
        import asc_op_compile_base.common.utils.log as logger

        logger.debug("Input info: edge id[%s], edge type[%s].", self._edge_id, self._edge_type)


class AttrInfo(object):
    def __init__(self):
        self._name = None
        self._dtype = None
        self._value = None

    def get_name(self):
        return self._name

    def get_dtype(self):
        return self._dtype

    def get_value(self):
        return self._value

    def display(self):
        import asc_op_compile_base.common.utils.log as logger

        logger.debug("Attr info: name[%s], data type[%s], value[%s].", self._name, self._dtype, self._value)


class OpInfo(object):
    def __init__(self):
        self._id = None
        self._op_type = None
        self._pattern = None
        self._impl_mode = None
        self._input_list = []
        self._output_list = []
        self._attr_list = []

    def add_input(self, input_info):
        if input_info is None:
            return
        self._input_list.append(input_info)

    def add_output(self, output_info):
        if output_info is None:
            return
        self._output_list.append(output_info)

    def add_attr(self, attr_info):
        if attr_info is None:
            return
        self._attr_list.append(attr_info)

    def get_id(self):
        return self._id

    def get_op_type(self):
        return self._op_type

    def get_pattern(self):
        return self._pattern

    def get_impl_mode(self):
        return self._impl_mode

    def get_input_list(self):
        return self._input_list

    def get_output_list(self):
        return self._output_list

    def get_attr_list(self):
        return self._attr_list

    def get_attr_by_name(self, attr_name):
        for attr_info in self._attr_list:
            if attr_info.get_name() == attr_name:
                return attr_info
        return None

    def display(self):
        import asc_op_compile_base.common.utils.log as logger

        logger.debug(
            "Op info: id[%s], type[%s], pattern[%s], impl mode[%s]",
            self._id,
            self._op_type,
            self._pattern,
            self._impl_mode,
        )
        if self._input_list is not None and len(self._input_list) > 0:
            for input_info in self._input_list:
                input_info.display()
        if self._output_list is not None and len(self._output_list) > 0:
            for output_info in self._output_list:
                output_info.display()
        if self._attr_list is not None and len(self._attr_list) > 0:
            for attr_info in self._attr_list:
                attr_info.display()


class GraphInfo(object):
    def __init__(self):
        self._input_list = []
        self._output_list = []
        self._op_list = []
        self._is_dynamic_shape = False

    def get_input_list(self):
        return self._input_list

    def get_output_list(self):
        return self._output_list

    def get_op_list(self):
        return self._op_list

    def is_dynamic_shape(self):
        return self._is_dynamic_shape

    def get_peer_output_by_id(self, id):
        if len(self._op_list) == 0 or id is None:
            return None
        for op_info in self._op_list:
            if op_info is None:
                continue
            for output_info in op_info.get_output_list():
                if output_info is None:
                    continue
                if output_info.get_id() == id:
                    return output_info

        return None

    def get_data_output_by_id(self, id):
        if len(self._input_list) == 0 or id is None:
            return None
        for input_info in self._input_list:
            if input_info is None:
                continue
            if input_info.get_id() == id:
                return input_info

        return None

    def display(self):
        import asc_op_compile_base.common.utils.log as logger

        logger.debug("Graph info: is dynamic shape[%s]", self._is_dynamic_shape)
        if self._input_list is not None and len(self._input_list) > 0:
            for input_info in self._input_list:
                input_info.display(True)
        if self._output_list is not None and len(self._output_list) > 0:
            for output_info in self._output_list:
                output_info.display()
        if self._op_list is not None and len(self._op_list) > 0:
            for op_info in self._op_list:
                op_info.display()
