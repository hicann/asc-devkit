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
Paser op json info and generate kernel name
"""

import json
import hashlib
import asc_op_compile_base.common.utils.log as logger
from opc_common import LogLevel
from opc_common import opc_log_full
from constant import CompileParam, OpcOptions, OpParamType
from op_manager import get_attr_info_from_opstore


class TensorData:
    """
    class for input or output tensor, def tensor data and process
    """

    def __init__(
        self,
        shape,
        ori_shape,
        cur_format,
        sub_format,
        ori_format,
        dtype,
        cur_range,
        ori_range,
        addr_type,
        const_value,
        use_l1_workspace,
        l1_addr_flag,
        l1_fusion_type,
        split_index,
        l1_workspace_size,
        l1_addr_offset,
        l1_valid_size,
        is_first_layer,
        slice_offset,
        valid_shape,
        total_shape,
    ):
        """
        init
        :param shape: shape value
        :param ori_shape: ori_shape value
        :param cur_format: format value, def cur_format not format for sc
        :param ori_format: ori_format value
        :param dtype: dtype value
        :param cur_range: range value, def cur_range not range for sc
        :param range: ori_range value
        """
        self.__shape = shape
        self.__ori_shape = ori_shape
        self.__format = cur_format
        self.__sub_format = sub_format
        self.__ori_format = ori_format
        self.__dtype = dtype
        self.__range = cur_range
        self.__ori_range = ori_range
        self.__addr_type = addr_type
        self.__const_value = const_value
        self.__use_l1_workspace = use_l1_workspace
        self.__l1_addr_flag = l1_addr_flag
        self.__l1_fusion_type = l1_fusion_type
        self.__split_index = split_index
        self.__l1_workspace_size = l1_workspace_size
        self.__l1_addr_offset = l1_addr_offset
        self.__l1_valid_size = l1_valid_size
        self.__is_first_layer = is_first_layer
        self.__slice_offset = slice_offset
        self.__valid_shape = valid_shape
        self.__total_shape = total_shape

    @staticmethod
    def tensor_range_adjust(cur_range):
        """
        resolve op info range -1 to None, and range is a two dimensional array list
        """
        if cur_range is None:
            logger.debug("Range is None.")
            return

        for row_index, row in enumerate(cur_range):
            for col_index, value in enumerate(row):
                if value == -1:
                    logger.debug("Change range value -1 to None.")
                    cur_range[row_index][col_index] = None
        logger.debug("Resolve op json range: %s.", str(cur_range))
        return

    def update_range_common(self):
        """
        update_range_common
        """
        if not isinstance(self.__shape, list):
            return

        self.__range = self.__range if self.__range is not None else []

        if len(self.__shape) != len(self.__range):
            tmp_range = []
            for dim in self.__shape:
                dim1 = dim if dim > 0 else 1
                dim2 = dim if dim > 0 else None
                tmp_range.append([dim1, dim2])
            self.__range = tmp_range
            logger.debug("update_range to: %s.", str(tmp_range))

    def check_tensor_null(self):
        """
        check tensor data is all none
        """
        return (
            self.__shape is None
            and self.__ori_shape is None
            and self.__format is None
            and self.__sub_format is None
            and self.__ori_format is None
            and self.__dtype is None
            and self.__range is None
            and self.__ori_range is None
            and self.__addr_type is None
            and self.__use_l1_workspace is None
            and self.__l1_addr_flag is None
            and self.__l1_fusion_type is None
            and self.__split_index is None
            and self.__l1_workspace_size is None
            and self.__l1_addr_offset is None
            and self.__l1_valid_size is None
            and self.__is_first_layer is None
            and self.__slice_offset is None
            and self.__valid_shape is None
            and self.__total_shape is None
        ) or (self.__format == "FORMAT_RESERVED" or self.__dtype == "undefined")

    def set_tensor_field_default(self):
        """
        set tensor some fields to default value if is none
        """
        if self.__ori_format is None:
            logger.debug("Ori_format is none, set as format.")
            self.__ori_format = self.__format

        if self.__shape == []:
            logger.debug("Shape is [], set as [1].")
            self.__shape = [1]

        if self.__ori_shape is None:
            logger.debug("Ori_shape is none, set as shape.")
            self.__ori_shape = self.__shape

    def adjust_tensor_field_value(self):
        """
        adjust tensor some fields specify value to others
        """
        self.tensor_range_adjust(self.__range)
        self.tensor_range_adjust(self.__ori_range)
        self.update_range_common()

    @staticmethod
    def set_tensor_dict(tensor_dict, key, value):
        """
        generate tensor dict not none value
        """
        if value is not None:
            tensor_dict[key] = value

    def create_tensor_dict(self):
        """
        generate dict from tensor data
        """
        tensor_dict = {}
        self.set_tensor_dict(tensor_dict, CompileParam.SHAPE, self.__shape)
        self.set_tensor_dict(tensor_dict, CompileParam.ORI_SHAPE, self.__ori_shape)
        self.set_tensor_dict(tensor_dict, CompileParam.FORMAT, self.__format)
        self.set_tensor_dict(tensor_dict, CompileParam.SUB_FORMAT, self.__sub_format)
        self.set_tensor_dict(tensor_dict, CompileParam.ORI_FORMAT, self.__ori_format)
        self.set_tensor_dict(tensor_dict, CompileParam.DTYPE, self.__dtype)
        self.set_tensor_dict(tensor_dict, CompileParam.RANGE, self.__range)
        self.set_tensor_dict(tensor_dict, CompileParam.ORI_RANGE, self.__ori_range)
        self.set_tensor_dict(tensor_dict, CompileParam.ADDR_TYPE, self.__addr_type)
        self.set_tensor_dict(tensor_dict, CompileParam.CONST_VALUE, self.__const_value)
        self.set_tensor_dict(tensor_dict, CompileParam.USE_L1_WORKSPACE, self.__use_l1_workspace)
        self.set_tensor_dict(tensor_dict, CompileParam.L1_ADDR_FLAG, self.__l1_addr_flag)
        self.set_tensor_dict(tensor_dict, CompileParam.L1_FUSION_TYPE, self.__l1_fusion_type)
        self.set_tensor_dict(tensor_dict, CompileParam.SPLIT_INDEX, self.__split_index)
        self.set_tensor_dict(tensor_dict, CompileParam.L1_WORKSPACE_SIZE, self.__l1_workspace_size)
        self.set_tensor_dict(tensor_dict, CompileParam.L1_ADDR_OFFSET, self.__l1_addr_offset)
        self.set_tensor_dict(tensor_dict, CompileParam.L1_VALID_SIZE, self.__l1_valid_size)
        self.set_tensor_dict(tensor_dict, CompileParam.IS_FIRST_LAYER, self.__is_first_layer)
        self.set_tensor_dict(tensor_dict, CompileParam.SLICE_OFFSET, self.__slice_offset)
        self.set_tensor_dict(tensor_dict, CompileParam.VALID_SHAPE, self.__valid_shape)
        self.set_tensor_dict(tensor_dict, CompileParam.TOTAL_SHAPE, self.__total_shape)
        logger.debug("tensor_dict is %s.", str(tensor_dict))
        return tensor_dict


class OpInfoParser:
    """
    class for reslove op info from json and generate kernel name
    """

    def __init__(self, op, op_info_dict, opc_compile_args_dict):
        """
        init
        :param op: op info from json, the data need to be resolve
        :param op_info_dict: type is namedtuple of OP_INFO, defined in opc_common
        :param opc_compile_args_dict: type is namedtuple of COMPILE_PARAS, defined in opc_common
        """
        self.__op = op
        self.__op_info_dict = op_info_dict
        self.__opc_compile_args_dict = opc_compile_args_dict

    def __resolve_json_inputoutput(self, res_type):
        """
        resolve op info input or output from json op
        """

        def _resolve_tensor(tensor):
            # reslove field
            tensor_data = TensorData(
                tensor.get(CompileParam.SHAPE),
                tensor.get(CompileParam.ORI_SHAPE),
                tensor.get(CompileParam.FORMAT),
                tensor.get(CompileParam.SUB_FORMAT),
                tensor.get(CompileParam.ORI_FORMAT),
                tensor.get(CompileParam.DTYPE),
                tensor.get(CompileParam.RANGE),
                tensor.get(CompileParam.ORI_RANGE),
                tensor.get(CompileParam.ADDR_TYPE),
                tensor.get(CompileParam.CONST_VALUE),
                tensor.get(CompileParam.USE_L1_WORKSPACE),
                tensor.get(CompileParam.L1_ADDR_FLAG),
                tensor.get(CompileParam.L1_FUSION_TYPE),
                tensor.get(CompileParam.SPLIT_INDEX),
                tensor.get(CompileParam.L1_WORKSPACE_SIZE),
                tensor.get(CompileParam.L1_ADDR_OFFSET),
                tensor.get(CompileParam.L1_VALID_SIZE),
                tensor.get(CompileParam.IS_FIRST_LAYER),
                tensor.get(CompileParam.SLICE_OFFSET),
                tensor.get(CompileParam.VALID_SHAPE),
                tensor.get(CompileParam.TOTAL_SHAPE),
            )

            if tensor_data.check_tensor_null():
                logger.debug("Tensor is null.")
                return None

            tensor_data.set_tensor_field_default()
            tensor_data.adjust_tensor_field_value()
            return tensor_data.create_tensor_dict()

        logger.debug("Start to resolve op json %s.", res_type)
        try:
            input_output_tuple = ()
            for tensors in self.__op.get(res_type):
                if tensors is None:
                    logger.debug("Tensor is null.")
                    input_output_tuple += (None,)  # null tensor set None
                    continue
                if isinstance(tensors, list):
                    tensor_list = []
                    for item in tensors:
                        tensor_list.append(_resolve_tensor(item))
                    input_output_tuple += (tensor_list,)
                else:
                    input_output_tuple += (_resolve_tensor(tensors),)

            return input_output_tuple
        except Exception as e:
            raise RuntimeError("Exception: Failed to resolve op json %s, exception is %s." % (res_type, str(e))) from e
        finally:
            pass

    @staticmethod
    def create_attr_dict(attr_dict, value, value_list, value_range, range_mode):
        """
        create attr dict
        """
        if value_range:
            attr_dict[CompileParam.VALUE_RANGE] = value_range
            if range_mode:
                attr_dict[CompileParam.RANGE_MODE] = range_mode
            else:
                raise RuntimeError("Attr key:[value_range] exist but [range_mode] not exist in json.")
            return

        if value_list:
            attr_dict[CompileParam.VALUE_LIST] = value_list
            return

        attr_dict[CompileParam.VALUE] = value

    @staticmethod
    def __compare_and_warn_del_attr(op_type, op_attr_list, res_attr_list):
        """
        Warning del attr
        """
        for op_attr in op_attr_list:
            op_attr_find = False
            for attr in res_attr_list:
                if op_attr[CompileParam.NAME] == attr[CompileParam.NAME]:
                    op_attr_find = True
            if not op_attr_find:
                logger.warn(
                    "%s delete input attr [%s]. op info store does not have this attr.",
                    op_type,
                    op_attr[CompileParam.NAME],
                )

    def __get_attr_based_on_op_info(self, op_type):
        """
        Verify op attr attribute based on the op info store modify
        if attr is None, but op info store have attr. Read default value from op info store
        if attr not in op info store. remove attr
        """
        op_attr_list = self.__resolve_json_attr()
        if not op_attr_list:
            return op_attr_list

        attr_info_list = get_attr_info_from_opstore(op_type)
        attrs_list = []
        if attr_info_list is None:
            return op_attr_list

        for _, attr_info in enumerate(attr_info_list):
            curr_attr = None
            for op_attr in op_attr_list:
                if op_attr.get(CompileParam.NAME) == attr_info.name:
                    curr_attr = op_attr
                    continue

            if curr_attr is None:
                if attr_info.param_type == OpParamType.OPT:
                    curr_attr = {}
                    curr_attr[CompileParam.NAME] = attr_info.name
                    curr_attr[CompileParam.DTYPE] = attr_info.type
                    if attr_info.default_value is not None:
                        curr_attr[CompileParam.VALUE] = attr_info.default_value
                    logger.warn("%s and attr %s by op info store.", op_type, str(attr_info.name))

            attrs_list.append(curr_attr)
            logger.debug("Dict attr curr_attr: %s.", str(curr_attr))

        self.__compare_and_warn_del_attr(op_type, op_attr_list, attrs_list)
        return attrs_list

    def __calculate_valid_cnt(self, attr):
        valid_cnt = 0
        if CompileParam.VALUE in attr:
            valid_cnt += 1
        if CompileParam.VALUE_LIST in attr:
            valid_cnt += 1
        if CompileParam.VALUE_RANGE in attr:
            valid_cnt += 1
        return valid_cnt

    def __resolve_json_attr(self):
        """
        resolve op info attr from json op
        """
        logger.debug("Start to resolve json attr.")
        attrs = self.__op.get(CompileParam.ATTRS)
        attrs_list = []
        simplified_key_configured = self.__op.get(OpcOptions.SIMPLE_KEY) not in (None, "")
        for attr in attrs:
            if attr is None:
                logger.debug("Attr is none. Add none to attrs.")
                attrs_list.append(None)
                continue
            # reslove field
            name = attr.get(CompileParam.NAME)
            if name is None:
                raise RuntimeError("Attr key:[name] is not exist in json.")
            dtype = attr.get(CompileParam.DTYPE)
            if dtype is None:
                raise RuntimeError("Attr key:[dtype] is not exist in json.")
            valid_cnt = self.__calculate_valid_cnt(attr)
            if valid_cnt != 1:
                raise KeyError(
                    "Only support one of [value,value_range,value_list] in attr dict, please check input param."
                )

            value = attr.get(CompileParam.VALUE)
            value_list = attr.get(CompileParam.VALUE_LIST)
            value_range = attr.get(CompileParam.VALUE_RANGE)
            range_mode = attr.get(CompileParam.RANGE_MODE)

            # create attr dict
            attr_dict = {}
            attr_dict[CompileParam.NAME] = name
            attr_dict[CompileParam.DTYPE] = dtype
            self.create_attr_dict(attr_dict, value, value_list, value_range, range_mode)
            if (CompileParam.VALUE_RANGE in attr or CompileParam.VALUE_LIST in attr) and not simplified_key_configured:
                raise ValueError(
                    "When configuring [value_range,value_list] in attr dict, simplified_key need be configured."
                )
            logger.debug("Dict attr attr_dict: %s.", str(attr_dict))
            # create attr list
            attrs_list.append(attr_dict)
        return attrs_list

    def __resolve_json_by_key(self, key):
        """
        resolve op info bin_filename/simpplified_key from json op
        """
        logger.debug("Start to resolve json by key %s.", str(key))
        try:
            if self.__op.get(key) is None:
                logger.debug("Has no key %s.", str(key))
                return

            self.__op_info_dict[key] = self.__op.get(key)
        except Exception as e:
            raise RuntimeError("Exception: Failed to resolve op json by key %s, reason:%s." % (key, str(e))) from e
        finally:
            pass

    def __generate_kernel_name_inputoutput(self, inputoutput, res_type):
        """
        get input or output to generate kernel name
        """
        logger.debug("Start to generate kernel name %s.", res_type)
        try:
            inputoutputs = self.__op_info_dict.get(inputoutput)
            tensor_tuple = ()
            for tensor in inputoutputs:
                if tensor is None:
                    logger.debug("Tensor is none.")
                    tensor_tuple += (None,)
                    continue
                tensor_dict = {
                    CompileParam.SHAPE: tensor.get(CompileParam.SHAPE),
                    CompileParam.DTYPE: tensor.get(CompileParam.DTYPE),
                    CompileParam.FORMAT: tensor.get(CompileParam.FORMAT),
                }
                logger.debug("Generate kernel name %s desc %s.", res_type, str(tensor_dict))
                tensor_tuple += (tensor_dict,)
            return tensor_tuple
        except Exception as e:
            raise RuntimeError("Failed to generate kernel name %s, exception is %s." % (res_type, str(e))) from e
        finally:
            pass

    def __generate_kernel_name_attr(self):
        """
        get attr to generate kernel name
        """
        logger.debug("Start to generate kernel name attr.")
        try:
            attrs = self.__op_info_dict.get(CompileParam.ATTRS)
            tensor_tuple = ()
            for attr in attrs:
                if attr is None:
                    logger.debug("Attr is none.")
                    tensor_tuple += (None,)
                    continue
                if attr.get(CompileParam.VALUE) is None:
                    logger.debug("Generate kernel name attr, value is none.")
                    continue
                tensor_dict = {
                    CompileParam.NAME: attr.get(CompileParam.NAME),
                    CompileParam.DTYPE: attr.get(CompileParam.DTYPE),
                    CompileParam.VALUE: attr.get(CompileParam.VALUE),
                }

                logger.debug("Generate kernel name attr desc %s.", str(tensor_dict))
                tensor_tuple += (tensor_dict,)
            return tensor_tuple
        except Exception as e:
            raise RuntimeError("Exception: Failed to generate kernel name attr, reason:%s." % (str(e))) from e
        finally:
            pass

    def __get_impl_mode(self):
        """
        get attr to get impl mode
        """
        if OpcOptions.IMPL_MODE in self.__opc_compile_args_dict:
            return self.__opc_compile_args_dict.get(OpcOptions.IMPL_MODE)
        logger.debug("No impl_mode in _opc_compile_args_dict, set default value high_performance.")
        return "high_performance"  # set default value

    @staticmethod
    def get_param_hash(param_tuple):
        """
        get param hash
        """
        sha256_hash = hashlib.sha256()
        sha256_hash.update((json.dumps(param_tuple)).encode("utf-8"))
        return sha256_hash.hexdigest()

    def get_op_info(self, op_type):
        """
        resolve op info from json op
        """
        logger.debug("Start to resolve op json: ")
        opc_log_full(LogLevel.DEBUG, "op = %s", str(self.__op))
        if self.__op.get(CompileParam.INPUTS) is not None:
            self.__op_info_dict[CompileParam.INPUTS] = self.__resolve_json_inputoutput(CompileParam.INPUTS)

        if self.__op.get(CompileParam.OUTPUTS) is not None:
            self.__op_info_dict[CompileParam.OUTPUTS] = self.__resolve_json_inputoutput(CompileParam.OUTPUTS)

        if self.__op.get(CompileParam.ATTRS) is not None:
            self.__op_info_dict[CompileParam.ATTRS] = self.__get_attr_based_on_op_info(op_type)

        self.__resolve_json_by_key(OpcOptions.BIN_FILENAME)
        self.__resolve_json_by_key(OpcOptions.SIMPLE_KEY)

        logger.debug("Resolve json to dict, input: %s.", str(self.__op_info_dict.get(CompileParam.INPUTS)))
        logger.debug("Resolve json to dict, output: %s.", str(self.__op_info_dict.get(CompileParam.OUTPUTS)))
        logger.debug(
            "Resolve json to dict, attr: %s, bin_filename %s, simplified_key %s.",
            str(self.__op_info_dict.get(CompileParam.ATTRS)),
            str(self.__op_info_dict.get(OpcOptions.BIN_FILENAME)),
            str(self.__op_info_dict.get(OpcOptions.SIMPLE_KEY, "")),
        )

    def generate_kernel_name(self):
        """
        generate kernel name
        """
        opc_log_full(LogLevel.DEBUG, "Start to generate kernel name, op_info_dict is %s.", str(self.__op_info_dict))
        try:
            bin_filename = self.__op_info_dict.get(OpcOptions.BIN_FILENAME)
            if bin_filename is not None:
                logger.debug("Bin_filename is not none, use it as kernel name: %s.", bin_filename)
                return bin_filename

            soc_info_dict = {
                "coreType": self.__opc_compile_args_dict.get(OpcOptions.CORE_TYPE),
                "coreNum": self.__opc_compile_args_dict.get(OpcOptions.AICORE_NUM),
                "implMode": self.__get_impl_mode(),
            }

            param_tuple = (soc_info_dict,)
            param_tuple += self.__generate_kernel_name_inputoutput(CompileParam.INPUTS, "input_desc")
            param_tuple += self.__generate_kernel_name_inputoutput(CompileParam.OUTPUTS, "output_desc")
            param_tuple += self.__generate_kernel_name_attr()

            logger.debug("Generate kernel name tuple %s.", str(param_tuple))

            kernel_name = self.__op_info_dict.get("op_type") + "_" + self.get_param_hash(param_tuple)
            logger.debug("Generate kernel name %s.", kernel_name)
            return kernel_name
        except Exception as e:
            raise RuntimeError("Exception: Failed to generate kernel name, reason:%s" % (str(e))) from e
        finally:
            pass
