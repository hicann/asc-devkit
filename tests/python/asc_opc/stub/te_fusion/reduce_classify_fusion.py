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


class ReduceClassifyFusion:
    def __init__(self, op_list, pattern, mode="reduce"):
        self.op_list = op_list
        self.pattern = pattern
        self.ins_list = []

        self.placeholder_op = _op()
        self.elewise_op = _op()
        self.reduce_op = _op()

        self.label = None
        self.axis_idx = None

        self._init()

    def _init(self):
        # collect common_info
        self._collect_common_info()

        # Assure "before_reduce" or "after_reduce"
        ReduceClassifyFusion._discriminate(self.reduce_op.value[0], self.op_list)

        # Single Reduce or not
        if len(self.reduce_op.value) == 1:
            self._deal_single_reduce()
        else:
            raise RuntimeError("Only Support Single Reduce Node in Fusion")

    @staticmethod
    def _discriminate(reduce_op, _op_list):
        input_list_before_reduce = []
        input_list_after_reduce = []
        input_tensors = reduce_op["input_desc"]
        output_tensors = reduce_op["output_desc"]
        if input_tensors:
            for in_tensor in input_tensors:
                ReduceClassifyFusion._find_input_names_before_reduce(
                    in_tensor["name"], _op_list, input_list_before_reduce
                )
        if output_tensors:
            for out_tensor in output_tensors:
                ReduceClassifyFusion._find_input_names_after_reduce(
                    out_tensor["name"], _op_list, input_list_after_reduce
                )
        for _op in _op_list:
            if _op["type"] == "Data":
                out_tensor_name = _op["output_desc"][0]["name"]
                if out_tensor_name in input_list_before_reduce:
                    _op["output_desc"][0]["rel_pos_to_reduce"] = "before"
                if out_tensor_name in input_list_after_reduce:
                    _op["output_desc"][0]["rel_pos_to_reduce"] = "after"

    @staticmethod
    def _find_input_names_before_reduce(tensor_name, _op_list, input_tensor_name_list):
        input_tensor_name_list.append(tensor_name)
        for _op in _op_list:
            if _op["type"] == "Data":
                continue

            output_desc = _op["output_desc"]
            if not output_desc:
                continue
            output_link = False
            for out_desc in output_desc:
                if out_desc["name"] == tensor_name:
                    output_link = True

            if output_link:
                input_desc = _op["input_desc"]
                if input_desc:
                    for in_desc in input_desc:
                        ReduceClassifyFusion._find_input_names_before_reduce(
                            in_desc["name"], _op_list, input_tensor_name_list
                        )

    @staticmethod
    def _find_input_names_after_reduce(tensor_name, _op_list, input_tensor_name_list):
        for _op in _op_list:
            if _op["type"] == "Data":
                continue

            input_desc = _op["input_desc"]
            if not input_desc:
                continue
            input_link = False
            for in_desc in input_desc:
                if in_desc["name"] == tensor_name:
                    input_link = True

            if input_link:
                for in_desc in input_desc:
                    input_tensor_name_list.append(in_desc["name"])

                output_desc = _op["output_desc"]
                if output_desc:
                    for out_desc in output_desc:
                        ReduceClassifyFusion._find_input_names_after_reduce(
                            out_desc["name"], _op_list, input_tensor_name_list
                        )

    @staticmethod
    def _handle_input_range(inputs):
        for input in inputs:
            shape_range = input.get("range")
            if shape_range is None:
                return
            for range in shape_range:
                if len(range) == 2:
                    if range[1] == -1:
                        range[1] = None

    def _collect_common_info(self):
        for _key, _node in enumerate(self.op_list):
            if _node.get("type") == "Data":
                self.placeholder_op.value.append(_node)
                self.placeholder_op.idx.append(_key)
            elif _node.get("type").find("Reduce") != -1:
                self.reduce_op.value.append(_node)
                self.reduce_op.idx.append(_key)
            else:
                self.elewise_op.value.append(_node)
                self.elewise_op.idx.append(_key)

    def _deal_single_reduce(self):
        # ReduceD or Reduce(Single Reduce)
        self.label = self.reduce_op.value[0].get("type")[-1]
        # classify need keepdims attr
        extra_params = {"keepdims": self.reduce_op.value[0].get("attr_desc")[-1]}
        if self.label in ["D"]:
            # ReduceSumD, ReduceMaxD,...D
            input_shape = self.reduce_op.value[0].get("input_desc")[0].get("shape")
            attr_axis = self.reduce_op.value[0].get("attr_desc")[0]
            shape_len = len(input_shape)

            if not attr_axis:
                attr_axis = range(shape_len)
            if hasattr(attr_axis, "index"):
                attr_axis = list(attr_axis)
            import tbe.common.utils.shape_util as shape_util

            attr_axis = shape_util.axis_check(shape_len, attr_axis)
            dict_axis = {"shape": [len(attr_axis)], "value": attr_axis, "rel_pos_to_reduce": "axis"}

            inputs_desc = [x.get("output_desc")[0] for x in self.placeholder_op.value]
            inputs_desc.append(dict_axis)
            ReduceClassifyFusion._handle_input_range(inputs_desc)
            self.ins_list = classify(inputs_desc, self.pattern, extra_params)
        else:
            # ReduceSum, ReduceMax,...
            # axis is tensor that may be from placeholder or other
            dict_axis = self.reduce_op.value[0].get("input_desc")[1]
            axis_name = dict_axis.get("name").lower()
            if axis_name.find("placeholder") != -1:
                # axis is placeholder
                for _idx, _var in enumerate(self.placeholder_op.value):
                    if _var.get("output_desc")[0].get("name").lower() == axis_name:
                        self.placeholder_op.value[_idx]["output_desc"][0].update({"rel_pos_to_reduce": "axis"})
                        self.placeholder_op.value[_idx]["output_desc"][0].update(
                            {"dtype": self.placeholder_op.value[_idx]["output_desc"][0].get("data_type")}
                        )
                        self.axis_idx = _idx
                        break
                    if _idx == len(self.placeholder_op.value) - 1:
                        raise RuntimeError("Axis is belong to placeholder, but not find in placeholder_op")
                inputs_desc = [x.get("output_desc")[0] for x in self.placeholder_op.value]
                ReduceClassifyFusion._handle_input_range(inputs_desc)
                self.ins_list = classify(inputs_desc, self.pattern, extra_params)
            else:
                # axis is not in placeholder
                dict_axis.update({"rel_pos_to_reduce": "axis"})
                inputs_desc = [x.get("output_desc")[0] for x in self.placeholder_op.value]
                inputs_desc.append(dict_axis)
                ReduceClassifyFusion._handle_input_range(inputs_desc)
                self.axis_idx = len(inputs_desc) - 1
                self.ins_list = classify(inputs_desc, self.pattern, extra_params)
