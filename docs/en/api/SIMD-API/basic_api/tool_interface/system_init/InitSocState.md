# InitSocState

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T19:19:27.551Z pushedAt=2026-10-26T07:41:44.858Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/system_init/InitSocState_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_common_intf.h"`.

This API initializes the global state of the AI Core, including the public state and the respective unique states of the AIC (Cube Core) and AIV (Vector Core). Because implementations differ across products, the initialization items actually executed also differ.

**All initialization operations involved in this API are summarized as follows:**

| state category | initialization scope |
| ---- | ---- |
| public state | Initializes the [atomic accumulation state](../../atomic_operations/DisableDmaAtomic.md) and disables the data movement in-band atomic operation feature. |
| public state | Initializes the [Mask working mode](../../memory_vector_compute/SIMD_compute/mask.md#mask-mode) to Normal mode. |
| public state | Initializes the [scale quantization parameter for the s322fp16 scenario of AddDeqRelu/CastDequant/Cast](../../memory_vector_compute/type_conversion_aux_config/SetDeqScale.md#func1) to 1. |
| public state | Initializes the bits of the [CTRL register](../../special_register_access/SetCtrlSpr_ISASI.md#tab-950) other than `CTRL[48]` to their default values. |
| public state | Initializes the [atomic operation enable bit and atomic operation type](../../atomic_operations/SetStoreAtomicConfig_ISASI.md) to an invalid atomic operation type. |
| AIC | Initializes the [padding value used when the LoadData (convolution data movement) API is called](../../cube_compute_ISASI/cube_load_aux_config/SetLoadDataPaddingValue.md#description) to 0. |
| AIC | Initializes the [boundary value of the L1 Buffer (A1/B1) when LoadData (convolution data movement) is used](../../cube_compute_ISASI/cube_load_aux_config/SetLoadDataBoundary.md#description) to 0, indicating no boundary. |
| AIV | Configures [Mask](../../memory_vector_compute/SIMD_compute/mask.md) to all 1s, indicating that all values participate in computation. |
| AIV | [Resets the loop mode parameters](../../memory_vector_compute/data_move_aux_config/ResetLoopModePara.md). |

**The initialization items actually executed by each product are as follows:**

<!-- npu="950" id8 -->
**Table 1** Initialization items executed by this API for the Ascend 950PR&950DT products

| state category | initialization scope |
| ---- | ---- |
| public state | Atomic accumulation state |
| public state | Mask working mode |
| public state | Scale quantization parameter for the s322fp16 scenario of AddDeqRelu/CastDequant/Cast |
| public state | CTRL register |
| public state | Atomic operation enable bit and atomic operation type |
| AIC | Padding value used when the LoadData (convolution data movement) API is called |
| AIV | Mask |
| AIV | Loop mode parameters |

<!-- end id8 -->

<!-- npu="A3" id9 -->
**Table 2** Initialization items executed by this API for the Atlas A3 products

| state category | initialization scope |
| ---- | ---- |
| public state | Atomic accumulation state |
| public state | Mask working mode |
| AIC | Padding value used when the LoadData (convolution data movement) API is called |
| AIC | Boundary value of the L1 Buffer (A1/B1) when LoadData (convolution data movement) is used |
| AIV | Mask |

<!-- end id9 -->

<!-- npu="910b" id10 -->
**Table 3** Initialization items executed by this API for Atlas A2 products

| state category | initialization scope |
| ---- | ---- |
| public state | atomic accumulation state |
| public state | Mask working mode |
| AIC | Padding value used when calling the LoadData (convolution data movement) API |
| AIC | Boundary value of the L1 Buffer (A1/B1) when using LoadData (convolution data movement) |
| AIV | Mask |

<!-- end id10 -->

<!-- npu="310p" id11 -->
**Table 4** Initialization items executed by this API for the AI Core of Atlas inference products and the Vector Core of Atlas inference products

| state category | initialization scope |
| ---- | ---- |
| public state | atomic accumulation state |

<!-- end id11 -->

## Function Prototype

```cpp
__aicore__ inline void InitSocState()
```

## Parameters

None

## Return Value

None

## Constraints

- In actual execution, these values may be modified by previously executed operators. If this API is not called for initialization, unexpected values may cause precision errors in the computation results.

  For example, if a preceding operator uses Counter mode but does not reset it to Normal mode, setting the Mask in the default Normal mode of the current operator will cause the Mask setting to be unexpected, which in turn leads to precision errors.
- In [TPipe framework programming](../../../../../guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_principles.md), the initialization is performed by TPipe, so developers do not need to pay attention to it. In the [static Tensor programming](../../../../../guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md) scenario, users must call this function at the entry of the kernel function (Kernel) to initialize the AI Core state.

## Example

```cpp
__global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z)
{
    // In static Tensor programming, developers must manually call the InitSocState() API to initialize the global state registers.
    AscendC::InitSocState();
    KernelAdd op;
    op.Init(x, y, z);
    op.Process();
}
```
