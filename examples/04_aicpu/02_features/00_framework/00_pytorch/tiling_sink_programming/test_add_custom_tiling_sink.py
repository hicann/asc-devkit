#!/usr/bin/python3
# coding=utf-8

# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------


from typing import Any

import torch
import torch_npu
import torchair
from torchair.ge import Tensor
from torch_npu.testing.testcase import TestCase, run_tests

# Register a custom op in torch so it can be captured into the graph.
m = torch.library.Library("ascendc_ops", "FRAGMENT")
m.define("add_custom_tiling_sink(Tensor x, Tensor y) -> Tensor")


@torch.library.impl(m, "add_custom_tiling_sink", "Meta")
def add_custom_tiling_sink_meta(x, y):
    return torch.empty_like(x)


def add_custom_tiling_sink_impl(x, y):
    raise NotImplementedError("torch.ops.ascendc_ops.add_custom_tiling_sink is not implemented!")


torch.library.impl(m, "add_custom_tiling_sink", "CPU")(add_custom_tiling_sink_impl)
torch.library.impl(m, "add_custom_tiling_sink", "PrivateUse1")(add_custom_tiling_sink_impl)


# Map the Torch op to GE so torchair can lower it.
@torchair.register_fx_node_ge_converter(torch.ops.ascendc_ops.add_custom_tiling_sink.default)
def convert_npu_add_custom(x: Tensor, y: Tensor, z: Tensor = None, meta_outputs: Any = None):
    return torchair.ge.custom_op("AddCustomTilingSink", inputs={"x": x, "y": y}, outputs=["z"])


class SingleOpModel(torch.nn.Module):
    def __init__(self):
        super(SingleOpModel, self).__init__()

    def forward(self, x, y):
        z = torch.ops.ascendc_ops.add_custom_tiling_sink.default(x, y)
        return z


class TestCustomAdd(TestCase):
    def test_add_custom_ops(self):
        config = torchair.CompilerConfig()
        config.experimental_config.tiling_schedule_optimize = True
        npu_backend = torchair.get_npu_backend(compiler_config=config)
        model = torch.compile(SingleOpModel().npu(), fullgraph=True, backend=npu_backend, dynamic=False)

        length = [8, 48]
        x = torch.rand(length, device="cpu", dtype=torch.float)
        y = torch.rand(length, device="cpu", dtype=torch.float)
        output = model(x.npu(), y.npu()).cpu()

        cpuout = torch.add(x, y)
        self.assertRtolEqual(output, cpuout)


if __name__ == "__main__":
    run_tests()
