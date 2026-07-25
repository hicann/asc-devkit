# 算子自验证说明

本文档面向Ascend C算子开发者，提供单算子融合进SuperKernel的验证样例。算子开发完成后，可通过本文档中的样例自行验证算子是否支持融合进SuperKernel，包括功能正确性与精度对齐。

当前SuperKernel特性有两种开启方式：npugraph_ex后端和GE图模式，两种方式在前端开启方式上有所不同。开发者通常只需选择其中一种进行验证；如需在两种方式下均验证算子兼容性，可以分别参考下文样例。

## 验证思路

通用的单算子自验证思路如下：

1. **构造一个仅包含待验证算子的`torch.nn.Module`**：构造一个原始版本（不开启SuperKernel）和一个SuperKernel版本（在算子调用处标定SuperKernel范围）。
2. **分别用图模式编译并执行两个模型**：原始版本作为基准，SuperKernel版本作为目标。
3. **比对两个版本的输出**：若数值完全相等（或在可接受误差范围内），则证明算子在SuperKernel下功能与精度正确。
4. **补充多算子联合验证**：单算子验证能够排查大部分SuperKernel适配问题，是验证流程中必要且高效的第一步。但部分问题（如Cache一致性导致的精度差异）在单算子场景下通常不会暴露，需要在多个算子融合后的实际模型中才会显现。因此，单算子验证通过后，建议进一步构建多算子联合用例或在实际模型中验证，以确保算子在整网SuperKernel场景下的兼容性。

>[!NOTE]说明
>SuperKernel范围内即使只包含一个算子，也会按SuperKernel路径进行编译和执行，可以充分检验算子是否支持SuperKernel融合。如需验证多个算子的融合效果，排查融合后对其他算子的影响，可以将待验证算子与其他Ascend C算子一起放入SuperKernel范围内。

## npugraph_ex后端验证样例

npugraph_ex后端开启SuperKernel融合时，编译后端需使用`"npugraph_ex"`，并在`options`中显式传入`super_kernel_optimize=True`。如需控制融合范围，可通过`torch.npu.super_kernel_scope_begin`/`torch.npu.super_kernel_scope_end`一对接口进行标定。

### 示例代码

以下示例以`torch_npu.npu_moe_gating_top_k_softmax`为例，演示在npugraph_ex后端下的SuperKernel单算子验证：

```python
import torch
import numpy as np
import torch_npu

if __name__ == "__main__":
    token_num = 864
    expert_num = 2048
    top_k = 1024

    # 定义模型model
    class ModelOrigin(torch.nn.Module):
        def __init__(self, top_k):
            super().__init__()
            self.top_k = top_k

        def forward(self, gating_input, smooth_scales):
            moe_gating_res = torch_npu.npu_moe_gating_top_k_softmax(gating_input, None, self.top_k)
            dynamic_quant_res = torch_npu.npu_dynamic_quant(
                moe_gating_res[0], smooth_scales=smooth_scales
            )
            return dynamic_quant_res, moe_gating_res

    class ModelSuperKernel(torch.nn.Module):
        def __init__(self, top_k):
            super().__init__()
            self.top_k = top_k

        def forward(self, gating_input, smooth_scales):
            # 仅将npu_moe_gating_top_k_softmax融合为SuperKernel，标记为sp1
            torch.npu.super_kernel_scope_begin("sp1")
            moe_gating_res = torch_npu.npu_moe_gating_top_k_softmax(gating_input, None, self.top_k)
            torch.npu.super_kernel_scope_end("sp1")
            dynamic_quant_res = torch_npu.npu_dynamic_quant(
                moe_gating_res[0], smooth_scales=smooth_scales
            )
            return dynamic_quant_res, moe_gating_res

    gating_input = torch.randn((token_num, expert_num), dtype=torch.bfloat16)
    smooth_scales = torch.randn((top_k,), dtype=torch.bfloat16)
    # 使用图模式后端编译模型
    model_sk = torch.compile(
        ModelSuperKernel(top_k),
        backend="npugraph_ex",
        options={"static_kernel_compile": True, "super_kernel_optimize": True},
        dynamic=False,
    )
    print("-------------------- run sk -----------------------------------")
    dynamic_quant_res_sk, _ = model_sk(gating_input.npu(), smooth_scales.npu())
    model_no_sk = torch.compile(ModelOrigin(top_k), backend="npugraph_ex", dynamic=False)
    print("-------------------- run no sk -----------------------------------")
    dynamic_quant_res_origin, _ = model_no_sk(gating_input.npu(), smooth_scales.npu())
    res = np.array_equal(dynamic_quant_res_origin[0].cpu().numpy(), dynamic_quant_res_sk[0].cpu().numpy())
    res = res and np.array_equal(dynamic_quant_res_origin[1].cpu().numpy(), dynamic_quant_res_sk[1].cpu().numpy())
    if res:
        print("Precision ====== Success!!!")
    else:
        print("Precision ====== Failed.")
```

### 关键点说明

- `torch.npu.super_kernel_scope_begin(scope_name)`/`torch.npu.super_kernel_scope_end(scope_name)`：成对出现，配对的`scope_name`必须一致。相同的`scope_name`代表相同的融合范围，由用户控制。若传入`None`，则该范围内的算子不进行SuperKernel融合。
- `torch.compile(model, backend="npugraph_ex", options={...}, dynamic=False)`：通过npugraph_ex后端下发图，仅支持静态图（`dynamic=False`）。`super_kernel_optimize=True`是开启SuperKernel融合优化的必要开关；其他融合优化与调试参数（如`super_kernel_optimize_options`、`super_kernel_debug_options`等）请参见[SuperKernel功能](https://www.hiascend.com/document/detail/zh/Pytorch/latest/devguide/TorchAir/docs/zh/npugraph_ex/advanced/superkernel.md)。
- **满核场景验证**：SuperKernel启动核数由其内部子算子的最大启动核数决定。在实际整网场景中，融合的算子较多，SuperKernel通常会启动满核（如24 Cube + 48 Vector）；但单算子验证时，单个算子的启动核数可能远未达到满核，导致SuperKernel也不会启动满核，从而无法充分验证算子适配说明中与核数相关的约束（如全核同步、硬同步等），为此，npugraph_ex后端可通过`super_kernel_debug_options`中的`debug_per_op_max_core_num`选项开启单算子满核验证模式。开启后，融合范围内的每个算子会独立成为一个SuperKernel，并以设备最大核数启动运行（Cube和Vector均启动最大核数）。示例如下：

  ```python
  # npugraph_ex后端：开启单算子满核验证模式
  model_sk = torch.compile(
      ModelSuperKernel(),
      backend="npugraph_ex",
      options={
          "static_kernel_compile": True,
          "super_kernel_optimize": True,
          "super_kernel_debug_options": {"debug_per_op_max_core_num": 1},
      },
      dynamic=False,
  )
  ```

  `debug_per_op_max_core_num`的详细说明请参见[SuperKernel功能](https://www.hiascend.com/document/detail/zh/Pytorch/latest/devguide/TorchAir/docs/zh/npugraph_ex/advanced/superkernel.md)。

## GE图模式验证样例

GE图模式通过`torchair.scope.super_kernel`作用域标定融合范围，编译后端使用`torchair.get_npu_backend()`返回的`npu_backend`。使用时需通过`with`语句块将待融合算子包裹在`torchair.scope.super_kernel(scope, options)`范围内。

### 示例代码

以下示例以`torch_npu.npu_moe_gating_top_k_softmax`为例，演示在GE图模式下的SuperKernel单算子验证：

```python
import torch
import numpy as np
import torch_npu
import torchair
from torchair.configs.compiler_config import CompilerConfig

if __name__ == "__main__":
    config = CompilerConfig()
    config.debug.graph_dump.type = "pbtxt"
    npu_backend = torchair.get_npu_backend(compiler_config=config)

    # 定义模型model
    class ModelOrigin(torch.nn.Module):
        def __init__(self, top_k):
            super().__init__()
            self.top_k = top_k

        def forward(self, gating_input, smooth_scales):
            moe_gating_res = torch_npu.npu_moe_gating_top_k_softmax(gating_input, None, self.top_k)
            dynamic_quant_res = torch_npu.npu_dynamic_quant(
                moe_gating_res[0], smooth_scales=smooth_scales
            )
            return dynamic_quant_res, moe_gating_res

    class ModelSuperKernel(torch.nn.Module):
        def __init__(self, top_k):
            super().__init__()
            self.top_k = top_k

        def forward(self, gating_input, smooth_scales):
            # 仅将npu_moe_gating_top_k_softmax融合为SuperKernel，标记为sp1
            with torchair.scope.super_kernel("sp1", ""):
                moe_gating_res = torch_npu.npu_moe_gating_top_k_softmax(gating_input, None, self.top_k)
            dynamic_quant_res = torch_npu.npu_dynamic_quant(
                moe_gating_res[0], smooth_scales=smooth_scales
            )
            return dynamic_quant_res, moe_gating_res

    token_num = 864
    expert_num = 2048
    top_k = 1024
    gating_input = torch.randn((token_num, expert_num), dtype=torch.bfloat16)
    smooth_scales = torch.randn((top_k,), dtype=torch.bfloat16)
    # 使用图模式后端编译模型
    model_no_sk = torch.compile(ModelOrigin(top_k), backend=npu_backend, dynamic=False)
    print("-------------------- run no sk -----------------------------------")
    dynamic_quant_res_origin, _ = model_no_sk(gating_input.npu(), smooth_scales.npu())
    model_sk = torch.compile(ModelSuperKernel(top_k), backend=npu_backend, dynamic=False)
    print("-------------------- run sk -----------------------------------")
    dynamic_quant_res_sk, _ = model_sk(gating_input.npu(), smooth_scales.npu())
    res = np.array_equal(dynamic_quant_res_origin[0].cpu().numpy(), dynamic_quant_res_sk[0].cpu().numpy())
    res = res and np.array_equal(dynamic_quant_res_origin[1].cpu().numpy(), dynamic_quant_res_sk[1].cpu().numpy())
    if res:
        print("Precision ====== Success!!!")
    else:
        print("Precision ====== Failed.")
```

### 关键点说明

- `torchair.scope.super_kernel(scope, options)`：用`with`语句块标定SuperKernel融合范围。`scope`表示该范围内算子被融合的SuperKernel名称，相同的scope代表相同的范围，由用户控制。若传入None，表示该范围内的算子不进行SuperKernel融合。`options`表示融合SuperKernel的编译选项，格式形如`"<option1>=<value1>:<option2>=<value2>"`，多个选项用英文冒号分割。编译选项说明请参见[图内标定SuperKernel范围](https://www.hiascend.com/document/detail/zh/Pytorch/latest/devguide/TorchAir/docs/zh/ascend_ir/features/advanced/super_kernel_scope.md)。
- `torch.compile(model, backend=npu_backend, dynamic=False)`：通过GE图模式下发图，仅支持静态图（`dynamic=False`）。
- **满核场景验证**：SuperKernel启动核数由其内部子算子的最大启动核数决定。在实际整网场景中，融合的算子较多，SuperKernel通常会启动满核（如24 Cube + 48 Vector）；但单算子验证时，单个算子的启动核数可能远未达到满核，导致SuperKernel也不会启动满核，从而无法充分验证[算子适配说明](算子适配说明.md)中与核数相关的约束（如全核同步、硬同步等）。为此，可通过`options`中的`debug-aic-num`和`debug-aiv-num`选项强制指定SuperKernel的启动核数，模拟满核场景进行验证。例如，强制SuperKernel以24 Cube + 48 Vector启动：

  ```python
  # 强制SuperKernel以满核启动，模拟真实整网场景
  with torchair.scope.super_kernel("sk1", "debug-aic-num=24:debug-aiv-num=48"):
      y = torch_npu.my_custom_op(x)
  ```

  `debug-aic-num`和`debug-aiv-num`的详细说明请参见[图内标定SuperKernel范围](https://www.hiascend.com/document/detail/zh/Pytorch/latest/devguide/TorchAir/docs/zh/ascend_ir/features/advanced/super_kernel_scope.md)。

## 多算子融合验证

如需验证待验证算子与其他Ascend C算子的融合效果，可以将多个算子一起放入SuperKernel范围内。例如：

- 将待验证算子与另一个已知支持SuperKernel的Ascend C算子一起放入scope内。
- 或者在scope内连续调用待验证算子两次或多次（输入可以构造为前后串联）。

例如：

```python
# npugraph_ex后端：将待验证算子与其他Ascend C算子一起融合
torch.npu.super_kernel_scope_begin("sk1")
y1 = torch_npu.my_custom_op(x)
y2 = torch_npu.npu_dequant_swiglu_quant(y1, weight_scale=weight_scale)
torch.npu.super_kernel_scope_end("sk1")

# GE图模式：将待验证算子与其他Ascend C算子一起融合
with torchair.scope.super_kernel("sk1", ""):
    y1 = torch_npu.my_custom_op(x)
    y2 = torch_npu.npu_dequant_swiglu_quant(y1, weight_scale=weight_scale)
```
