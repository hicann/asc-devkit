#!/usr/bin/python3
# coding=utf-8

# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

import os
from typing import Any

import torch
import torch_npu
import torchair
from torchair.ge import Tensor
from torch_npu.testing.testcase import TestCase, run_tests


LIB_PATH = os.environ.get("ASCENDC_OPS_LIB", "libascendc_ops.so")
# 从当前构建目录加载编译生成的动态库，使PyTorch侧注册的ascendc_ops::ascendc_add接口生效
torch.ops.load_library(LIB_PATH)


# 注册TorchAir converter：torch.compile捕获到ascendc_add节点后，由该函数转换成GE AddCustom节点
@torchair.register_fx_node_ge_converter(torch.ops.ascendc_ops.ascendc_add.default)
def convert_ascendc_add(x: Tensor, y: Tensor, z: Tensor = None, meta_outputs: Any = None):
    return torchair.ge.custom_op("AddCustom", inputs={"x": x, "y": y}, outputs=["z"])


class AddCustomModel(torch.nn.Module):
    def forward(self, x, y):
        # 模型中直接调用PyTorch侧注册的接口，后续由torch.compile捕获该调用
        return torch.ops.ascendc_ops.ascendc_add(x, y)


class TestGeTorchairAdd(TestCase):
    def test_add_custom_graph_mode(self):
        shape = [8, 2048]
        torch.manual_seed(0)
        # 在CPU侧构造输入和标准结果，便于最后和NPU图模式执行结果做数值对比
        x_cpu = torch.rand(shape, device="cpu", dtype=torch.float32)
        y_cpu = torch.rand(shape, device="cpu", dtype=torch.float32)

        model = AddCustomModel().npu()
        config = torchair.CompilerConfig()
        npu_backend = torchair.get_npu_backend(compiler_config=config)
        # 使用TorchAir后端编译模型，fullgraph=True确保forward中的ascendc_add被整图捕获
        opt_model = torch.compile(model, fullgraph=True, backend=npu_backend, dynamic=False)

        # 在NPU上执行编译后的图，执行链路会经过TorchAir转换出的GE AddCustom节点
        output = opt_model(x_cpu.npu(), y_cpu.npu()).cpu()
        golden = torch.add(x_cpu, y_cpu)
        # 校验图模式输出与CPU标准加法结果一致，证明本样例的接入链路和计算结果符合预期
        torch.testing.assert_close(output, golden, rtol=1e-4, atol=1e-4)


if __name__ == "__main__":
    run_tests()
