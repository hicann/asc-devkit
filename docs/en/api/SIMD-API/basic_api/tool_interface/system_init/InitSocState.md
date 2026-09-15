# InitSocState

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-09-01T06:49:59.901Z -->

## Product Support

<!-- npu="950" id12 -->
- Ascend 950PR/Ascend 950DT: supported
<!-- end id12 -->
<!-- npu="A3" id13 -->
- Atlas A3 training products/Atlas A3 inference products: supported
<!-- end id13 -->
<!-- npu="910b" id14 -->
- Atlas A2 training products/Atlas A2 inference products: supported
<!-- end id14 -->
<!-- npu="310b" id15 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id15 -->
<!-- npu="310p" id16 -->
- Atlas inference products AI Core: supported
<!-- end id16 -->
<!-- npu="310p" id17 -->
- Atlas inference products Vector Core: supported
<!-- end id17 -->
<!-- npu="910" id18 -->
- Atlas training products: not supported
<!-- end id18 -->

## Function Description

The header file path is `"basic_api/kernel_operator_common_intf.h"`.

This API initializes the global state of the AI Core, including the public state and the respective unique states of AIC (Cube Core) and AIV (Vector Core). Because implementations differ across products, the initialization items actually executed also vary.

**All initialization operations involved in the API are summarized as follows:**

| State Category | Initialization Content |
| ---- | ---- |
| Public state | Initializes the [atomic accumulation state](../../atomic_operations/DisableDmaAtomic.md) and disables the data transfer in-line atomic operation function. |
| Public state | Initializes the [Mask working mode](../../memory_vector_compute/SIMD_compute/mask.md#mask-mode) to Normal mode. |
| Public state | Initializes the [scale quantization parameter for the s322fp16 scenario used by AddDeqRelu/CastDequant/Cast](../../memory_vector_compute/type_conversion_aux_config/SetDeqScale.md#func1) to 1. |
| Public state | Initializes the bits other than `CTRL[48]` in the [CTRL register](../../special_register_access/SetCtrlSpr_ISASI.md#tab-950) to default values. |
| Public state | Initializes the [atomic operation enable bit and atomic operation type](../../atomic_operations/SetStoreAtomicConfig_ISASI.md) to an invalid atomic operation type. |
| AIC | Initializes the [padding value used when the Load3D API is called](../../cube_compute_ISASI/cube_load_aux_config/SetLoadDataPaddingValue.md#description) to 0. |
| AIC | Initializes the [boundary value of A1/B1 when Load3D is used](../../cube_compute_ISASI/cube_load_aux_config/SetLoadDataBoundary.md#description) to 0, indicating no boundary. |
| AIV | Configures the [Mask](../../memory_vector_compute/SIMD_compute/mask.md) to all 1s, indicating that all numbers participate in the computation. |
| AIV | [Resets the loop mode parameters](../../memory_vector_compute/data_move_aux_config/ResetLoopModePara.md). |

**The initialization items actually executed by each product are as follows:**

<!-- npu="950" id1 -->
**Table 1** Initialization items executed by this API for Ascend 950PR/Ascend 950DT

| State category | Initialization scope |
| ---- | ---- |
| Public state | Atomic accumulation state |
| Public state | Mask working mode |
| Public state | Scale quantization parameters for the s322fp16 scenario of AddDeqRelu/CastDequant/Cast |
| Public state | CTRL register |
| Public state | Atomic operation enable bit and atomic operation type |
| AIC | Padding value filled when the Load3D API is called |
| AIV | Mask |
| AIV | Parameters of loop mode |
<!-- end id1 -->

<!-- npu="A3" id2 -->
**Table 2** Initialization items executed by this API for Atlas A3 training products/Atlas A3 inference products

| State category | Initialization scope |
| ---- | ---- |
| Public state | Atomic accumulation state |
| Public state | Mask working mode |
| AIC | Padding value filled when the Load3D API is called |
| AIC | Boundary values of A1/B1 when using Load3D |
| AIV | Mask |
<!-- end id2 -->

<!-- npu="910b" id3 -->
**Table 3** Initialization items executed by this API for Atlas A2 training products/Atlas A2 inference products

| State category | Initialization scope |
| ---- | ---- |
| Public state | Atomic accumulation state |
| Public state | Mask working mode |
| AIC | Padding value filled when the Load3D API is called |
| AIC | Boundary value of A1/B1 when using Load3D |
| AIV | Mask |
<!-- end id3 -->

<!-- npu="310p" id4 -->
**Table 4** Initialization items executed by this API for Atlas inference products AI Core and Atlas inference products Vector Core

| State category | Initialization scope |
| ---- | ---- |
| Public state | Atomic accumulation state |
<!-- end id4 -->

## Function Prototype

```cpp
__aicore__ inline void InitSocState()
```

## Parameter Description

None

## Return Value Description

None

## Constraints

- In actual execution, these values may be modified by previously executed operators. If this API is not called for initialization, unexpected values may cause precision errors in the computation results.

  For example, if a previous operator uses Counter mode but does not reset it to Normal mode, when the current operator sets the Mask in the default Normal mode, the Mask setting will not meet expectations, thereby causing precision errors.
- In [TPipe framework programming](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/Programming Model/AI Core SIMD Programming/TPipe TQue Framework Programming/TPipe TQue Framework Programming Principles.md), the initialization process is completed by TPipe, and developers do not need to pay attention to it. In the [static Tensor programming](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/Programming Model/AI Core SIMD Programming/Tensor Based CPP Programming/Static Tensor Programming.md) scenario, users must call this function at the Kernel entry to initialize the AI Core state.

## Calling Example

```cpp
__global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z)
{
    // In static Tensor programming, developers need to manually call the InitSocState() API to initialize the global state registers.
    AscendC::InitSocState();
    KernelAdd op;
    op.Init(x, y, z);
    op.Process();
}
```
