# Operator Self-Verification Description

<!-- md-trans-meta sourceCommit=dc56b6592be9a41ee18c3bfdf6bd8116a9020128 translatedAt=2026-08-26T12:14:15.695Z pushedAt=2026-09-03T11:14:43.273Z -->

This document is intended for Ascend C operator developers and provides verification examples for fusing a single operator into SuperKernel. After operator development is complete, you can use the examples in this document to verify whether the operator supports fusion into SuperKernel, including functional correctness and precision alignment.

The SuperKernel feature currently supports two enabling methods: the npugraph_ex backend and GE graph mode. The two methods differ in how they are enabled on the frontend. Developers usually need to select only one of them for verification. If you need to verify operator compatibility under both methods, refer to the corresponding examples below.

## Verification Approach

The general single-operator self-verification approach is as follows:

1. **Construct a `torch.nn.Module` that contains only the operator to be verified**: Construct an original version (without SuperKernel enabled) and a SuperKernel version (mark the SuperKernel scope at the operator call site).
2. **Compile and execute the two models in graph mode separately**: Use the original version as the baseline and the SuperKernel version as the target.
3. **Compare the outputs of the two versions**: If the values are exactly equal (or within an acceptable error range), it proves that the operator functions correctly and maintains precision under SuperKernel.
4. **Supplement with multi-operator joint verification**: Single-operator verification can identify most SuperKernel adaptation issues and is a necessary and efficient first step in the verification process. However, some issues (such as precision differences caused by cache consistency) usually do not surface in a single-operator scenario and only appear in an actual model after multiple operators are fused. Therefore, after single-operator verification passes, it is recommended to further build multi-operator joint test cases or verify in an actual model to ensure the operator's compatibility in the full-network SuperKernel scenario.

>[!NOTE] Note
>Even if the SuperKernel scope contains only one operator, it is still compiled and executed through the SuperKernel path, which fully verifies whether the operator supports SuperKernel fusion. To verify the fusion effect of multiple operators and identify the impact on other operators after fusion, you can place the operator to be verified together with other Ascend C operators within the SuperKernel scope.

## npugraph_ex Backend Verification Example

When SuperKernel fusion is enabled on the npugraph_ex backend, the compilation backend must use `"npugraph_ex"`, and `super_kernel_optimize=True` must be explicitly passed in `options`. To control the fusion scope, use the `torch.npu.super_kernel_scope_begin`/`torch.npu.super_kernel_scope_end` pair of APIs to mark it.

### Sample Code

The following example uses `torch_npu.npu_moe_gating_top_k_softmax` to demonstrate single-operator verification of SuperKernel on the npugraph_ex backend:

```python
import torch
import numpy as np
import torch_npu

if __name__ == "__main__":
    token_num = 864
    expert_num = 2048
    top_k = 1024

    # Define the model
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
            # Fuse only npu_moe_gating_top_k_softmax into SuperKernel and mark it as sp1
            torch.npu.super_kernel_scope_begin("sp1")
            moe_gating_res = torch_npu.npu_moe_gating_top_k_softmax(gating_input, None, self.top_k)
            torch.npu.super_kernel_scope_end("sp1")
            dynamic_quant_res = torch_npu.npu_dynamic_quant(
                moe_gating_res[0], smooth_scales=smooth_scales
            )
            return dynamic_quant_res, moe_gating_res

    gating_input = torch.randn((token_num, expert_num), dtype=torch.bfloat16)
    smooth_scales = torch.randn((top_k,), dtype=torch.bfloat16)
    # Compile the model using the graph mode backend
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

### Key Point Description

- `torch.npu.super_kernel_scope_begin(scope_name)`/`torch.npu.super_kernel_scope_end(scope_name)`: These two APIs must be used in pairs, and the paired `scope_name` values must be identical. The same `scope_name` represents the same fusion scope, which is controlled by the user. If `None` is passed in, the operators within this scope are not fused into a SuperKernel.
- `torch.compile(model, backend="npugraph_ex", options={...}, dynamic=False)`: The graph is delivered through the npugraph_ex backend, which supports only static graphs (`dynamic=False`). `super_kernel_optimize=True` is the mandatory switch for enabling SuperKernel fusion optimization. For other fusion optimization and debugging parameters (such as `super_kernel_optimize_options` and `super_kernel_debug_options`), see [SuperKernel](https://www.hiascend.com/document/detail/en/Pytorch/2610/devguide/TorchAir/docs/en/npugraph_ex/advanced/superkernel.md).
- **Full-core scenario verification**: The number of startup cores of a SuperKernel is determined by the maximum number of startup cores among its internal sub-operators. In an actual full-network scenario, many operators are fused, so a SuperKernel usually starts with full cores (for example, 24 Cube + 48 Vector). However, during single operator verification, the number of startup cores of a single operator may be far below the full-core count, causing the SuperKernel not to start with full cores either. As a result, the core-count-related constraints described in the operator adaptation instructions (such as full-core synchronization and hard synchronization) cannot be fully verified. To address this, the npugraph_ex backend can enable the single-operator full-core verification mode through the `debug_per_op_max_core_num` option in `super_kernel_debug_options`. After this mode is enabled, each operator within the fusion scope becomes an independent SuperKernel and runs with the maximum number of cores on the device (both Cube and Vector start with the maximum number of cores). An example is as follows:

  ```python
  # npugraph_ex backend: Enable the single-operator full-core verification mode.
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

  For details about `debug_per_op_max_core_num`, see [SuperKernel](https://www.hiascend.com/document/detail/en/Pytorch/2610/devguide/TorchAir/docs/en/npugraph_ex/advanced/superkernel.md).

## GE Graph Mode Verification Example

In GE graph mode, the fusion scope is marked through the `torchair.scope.super_kernel` scope, and the compilation backend uses the `npu_backend` returned by `torchair.get_npu_backend()`. When using it, wrap the operators to be fused within the `torchair.scope.super_kernel(scope, options)` scope using a `with` statement block.

### Sample Code

The following example uses `torch_npu.npu_moe_gating_top_k_softmax` to demonstrate single-operator verification of SuperKernel in GE graph mode:

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

    # Define the model
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
            # Fuse only npu_moe_gating_top_k_softmax into SuperKernel and mark it as sp1.
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
    # Compile the model using the graph mode backend.
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

### Key Point Description

- `torchair.scope.super_kernel(scope, options)`: Use a `with` statement block to mark the SuperKernel fusion scope. `scope` indicates the name of the SuperKernel into which the operators within this scope are fused. The same scope represents the same range and is controlled by the user. If `None` is passed, the operators within this scope are not fused into a SuperKernel. `options` indicates the compilation options for fusing the SuperKernel, in the format `"<option1>=<value1>:<option2>=<value2>"`, with multiple options separated by English colons. For details about the compilation options, see the section about [marking the SuperKernel scope in a graph](https://www.hiascend.com/document/detail/en/Pytorch/2610/devguide/TorchAir/docs/en/ascend_ir/features/advanced/super_kernel_scope.md).
- `torch.compile(model, backend=npu_backend, dynamic=False)`: Delivers the graph through GE graph mode, which supports only static graphs (`dynamic=False`).
- **Full-core scenario verification**: The number of startup cores of a SuperKernel is determined by the maximum number of startup cores among its internal sub-operators. In a real full-network scenario, many operators are fused, so a SuperKernel usually starts with full cores (for example, 24 Cube + 48 Vector). However, during single-operator verification, the number of startup cores of a single operator may be far below the full-core count, causing the SuperKernel not to start with full cores either. As a result, the constraints related to the number of cores (such as full-core synchronization and hard synchronization) described in [Operator Adaptation Description](operator_adaptation_description.md) cannot be fully verified. To address this, you can use the `debug-aic-num` and `debug-aiv-num` options in `options` to forcibly specify the number of startup cores of the SuperKernel, simulating a full-core scenario for verification. For example, force the SuperKernel to start with 24 Cube + 48 Vector:

  ```python
  # Force the SuperKernel to start with full cores to simulate a real full-network scenario.
  with torchair.scope.super_kernel("sk1", "debug-aic-num=24:debug-aiv-num=48"):
      y = torch_npu.my_custom_op(x)
  ```

  For details about `debug-aic-num` and `debug-aiv-num`, see the section about [marking the SuperKernel scope in a graph](https://www.hiascend.com/document/detail/en/Pytorch/2610/devguide/TorchAir/docs/en/ascend_ir/features/advanced/super_kernel_scope.md).

## Multi-Operator Fusion Verification

To verify the fusion effect between the operator to be verified and other Ascend C operators, you can place multiple operators together within the SuperKernel scope. For example:

- Place the operator to be verified and another Ascend C operator that is known to support SuperKernel together within the scope.
- Or call the operator to be verified twice or more consecutively within the scope (the inputs can be constructed in a serial connection).

For example:

```python
# npugraph_ex backend: fuse the operator to be verified with other Ascend C operators.
torch.npu.super_kernel_scope_begin("sk1")
y1 = torch_npu.my_custom_op(x)
y2 = torch_npu.npu_dequant_swiglu_quant(y1, weight_scale=weight_scale)
torch.npu.super_kernel_scope_end("sk1")

# GE graph mode: fuse the operator to be verified with other Ascend C operators.
with torchair.scope.super_kernel("sk1", ""):
    y1 = torch_npu.my_custom_op(x)
    y2 = torch_npu.npu_dequant_swiglu_quant(y1, weight_scale=weight_scale)
```
