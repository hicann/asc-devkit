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

from tbe.dsl import classify


class _op:
    def __init__(self):
        self.value = []
        self.idx = []


class NormClassifyFusion:
    def __init__(self, op_list, pattern, mode="norm"):
        self.op_list = op_list
        self.pattern = pattern
        self.mode = mode
        self.ins_list = None
        self.ins_with_attr_list = None
        self.null_shape_dict = None
        self.op_classify = None
        self.axes = []
        self.norm_output_desc = None
        self.norm_op = None

        self.placeholder_op = _op()
        self.init()

    @staticmethod
    def _handle_input_range(input_desc_vec):
        for input_desc in input_desc_vec:
            shape_range_vec = input_desc.get("range")
            if shape_range_vec is None:
                return
            for range_item in shape_range_vec:
                if len(range_item) == 2:
                    if range_item[1] == -1:
                        range_item[1] = None

    @staticmethod
    def _is_input_dynamic_shape(input_desc_vec):
        for input_desc in input_desc_vec:
            if input_desc.get("shape") is None:
                continue
            if [dim for dim in input_desc["shape"] if dim < 0]:
                return True
        return False

    @staticmethod
    def _get_input_desc_key(input_desc):
        key = "input_"
        if input_desc.get("format") is not None:
            key += input_desc.get("format")
        if input_desc.get("dtype") is not None:
            key += input_desc.get("dtype")
        if input_desc.get("shape") is not None:
            for dim in input_desc.get("shape"):
                key += str(dim)
        return key

    @staticmethod
    def _add_disable_fuse_axes(norm_format, shape_size, axes, extra_params):
        if norm_format == "NC1HWC0" and 1 in axes and 4 in axes:
            extra_params["disable_fuse_axes"] = [1, 4]
        if norm_format == "NDC1HWC0" and 2 in axes and 5 in axes:
            extra_params["disable_fuse_axes"] = [2, 5]
        if norm_format == "FRACTAL_NZ":
            disable_fuse_axes_val = []
            if shape_size - 1 in axes and shape_size - 4 in axes:
                disable_fuse_axes_val.append(shape_size - 4)
                disable_fuse_axes_val.append(shape_size - 1)
            if shape_size - 2 in axes and shape_size - 3 in axes:
                disable_fuse_axes_val.append(shape_size - 3)
                disable_fuse_axes_val.append(shape_size - 2)
            if len(disable_fuse_axes_val) > 0:
                extra_params["disable_fuse_axes"] = disable_fuse_axes_val

    def get_attrs_and_options(self):
        attrs = []
        options = []
        for op_node in self.placeholder_op.value:
            if op_node["type"] == "conv2d_data_rm":
                continue
            attr_dict = {}
            attr_val = []
            attr_dict["name"] = op_node.get("name")
            if "attr_desc" in op_node:
                for attr_desc in op_node["attr_desc"]:
                    attr_val.append(attr_desc)
            attr_dict["val"] = attr_val
            option = {}
            option["name"] = op_node.get("name")
            option["options"] = op_node.get("options")
            attrs.append(attr_dict)
            options.append(option)
        return attrs, options

    def assemble_extra_params(self, input_desc_vec, extra_params):
        input_shape_size = []
        input_desc_dict = {}
        for index, input_desc in enumerate(input_desc_vec):
            if input_desc.get("shape") is None:
                input_shape_size.append(1)
                continue
            shape_size = 1
            for dim in input_desc["shape"]:
                shape_size = shape_size * dim
            input_shape_size.append(shape_size)

            input_key = self._get_input_desc_key(input_desc)
            if input_key in input_desc_dict:
                input_desc_dict.get(input_key).append(index)
            else:
                input_desc_dict[input_key] = [index]

        extra_params["same_input_shape_group"] = list(input_desc_dict.values())

        max_shape_index = []
        max_shape_size = 0
        for index, input_size in enumerate(input_shape_size):
            if input_size > max_shape_size:
                max_shape_size = input_size
                max_shape_index = [index]
            if input_size == max_shape_size:
                max_shape_index.append(index)

        norm_format = ""
        norm_shape_size = 0
        input_shape_type = []
        for index, input_desc in enumerate(input_desc_vec):
            if index in max_shape_index:
                input_shape_type.append(0)
                norm_format = input_desc.get("format")
                norm_shape_size = len(input_desc.get("shape"))
            else:
                input_shape_type.append(1)

        extra_params["input_shape_type"] = input_shape_type
        self._add_disable_fuse_axes(norm_format, norm_shape_size, self.axes, extra_params)

    def _update_axes(self):
        if self.norm_op is None:
            return
        shape_size = len(self.norm_op.get("input_desc")[0].get("ori_shape"))
        if self.norm_op.get("input_desc")[0].get("format") != "FRACTAL_NZ":
            return

        new_axes = []
        for index, val in enumerate(self.axes):
            if val < 0:
                val = val + shape_size
            if val == shape_size - 1:
                new_axes.append(val - 1)
                new_axes.append(val + 2)
            elif val == shape_size - 2:
                new_axes.append(val + 1)
                new_axes.append(val + 2)
            else:
                new_axes.append(val)
        self.axes = new_axes

    def builtin_classify(self, input_desc_vec):
        if not self.axes or len(input_desc_vec) == 0:
            self.ins_list = []
            return

        self._update_axes()

        is_dynamic_shape = self._is_input_dynamic_shape(input_desc_vec)
        if is_dynamic_shape and len(input_desc_vec) > 1:
            self.ins_list = []
            return

        extra_params = {}
        if is_dynamic_shape:
            self._add_disable_fuse_axes(
                input_desc_vec[0].get("format"), len(input_desc_vec[0].get("shape")), self.axes, extra_params
            )
            # 0:complete shape, 1:partial shape, need broadcast
            extra_params["input_shape_type"] = [0]
        else:
            self.assemble_extra_params(input_desc_vec, extra_params)

        input_desc_vec.append(self.axes)
        self.ins_list = classify(inputs_desc_list, self.mode, extra_params)

    def init(self):
        norm_op_type = None
        norm_input_name = ""
        for node in self.op_list:
            if node.get("pattern") == self.pattern:
                self.norm_op = node
                norm_op_type = node.get("type")
                input_descs = node.get("input_desc")
                if input_descs is not None and len(input_descs) > 0:
                    norm_input_name = input_descs[0].get("name")
                if node.get("attr_desc") is not None:
                    if len(node.get("attr_desc")) > 0 and isinstance(node.get("attr_desc")[-1], list):
                        self.axes = node.get("attr_desc")[-1]

        input_desc_vec = []
        for key, node in enumerate(self.op_list):
            if node.get("type") == "Data":
                self.placeholder_op.value.append(node)
                self.placeholder_op.idx.append(key)
                output_desc = node.get("output_desc")[0]
                if output_desc["name"] == norm_input_name:
                    self.norm_output_desc = output_desc
                input_desc_vec.append(output_desc)

        self._handle_input_range(input_desc_vec)

        self.builtin_classify(input_desc_vec)
