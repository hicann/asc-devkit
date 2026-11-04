# SetAtomicMin\(ISASI\)<a name="ZH-CN_TOPIC_0000001637481306"></a>

<!-- md-trans-meta sourceCommit=9438de7fdc69ddeeaffbc929f8de5e675de9504c translatedAt=2026-09-23T10:33:20.018Z pushedAt=2026-10-26T07:41:43.961Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id11 -->
- Ascend 950PR&950DT products: supported
<!-- end id11 -->
<!-- npu="A3" id12 -->
- Atlas A3 products: supported
<!-- end id12 -->
<!-- npu="910b" id13 -->
- Atlas A2 products: supported
<!-- end id13 -->
<!-- npu="310b" id14 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products AI Core: not supported
<!-- end id15 -->
<!-- npu="310p" id16 -->
- Atlas inference products Vector Core: not supported
<!-- end id16 -->
<!-- npu="910" id17 -->
- Atlas training products: not supported
<!-- end id17 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/atomic_operations/SetAtomicMin_ISASI_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_set_atomic_intf.h"`.

Sets whether to perform atomic comparison on the data subsequently copied to GM: compares the content to be copied with the existing content in GM, and then writes the minimum value to GM. The **SetAtomicMin** API can set different data types through template parameters.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void SetAtomicMin() 
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

| Parameter | Description |
|--------|------|
| T | Sets the data type for the atomic minimum operation. |

## Data Types

The supported data types are **int8_t**, **int16_t**, **half**, **bfloat16_t**, **int32_t**, and **float**.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

<!-- npu="950" id1 -->
- For the Ascend 950PR&950DT products, the supported data path is UB/L0C Buffer->GM.
<!-- end id1 -->

<!-- npu="A3" id2 -->
- For the Atlas A3 products, the supported data path is UB/L0C Buffer/L1 Buffer->GM.
<!-- end id2 -->

<!-- npu="910b" id3 -->
- For the Atlas A2 products, the supported data path is UB/L0C Buffer/L1 Buffer->GM.
<!-- end id3 -->

- After use, it is recommended that you call [DisableDmaAtomic](DisableDmaAtomic.md) to disable the atomic minimum operation to avoid affecting subsequent related functions.
- This API does not automatically clear the GM data before execution. Developers need to determine whether to clear it based on the operator logic. If clearing is required, perform it manually before execution.
- **SetAtomicMin** has integrated the same functionality as [SetAtomicType](SetAtomicType.md). When calling the preceding API, it is recommended that you explicitly specify the data type of the atomic operation by setting the template parameter, without the need to additionally call the **SetAtomicType** API.
<!-- npu="A3,910b" id4 -->
- For [NPU architecture version 2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md), integer types and the bfloat16_t data type **are in non-saturation mode by default**. The saturation mode during the atomic minimum operation cannot be set through [SetSaturationStrategy](../special_register_access/SetSaturationStrategy.md) or [SetCtrlSpr](../special_register_access/SetCtrlSpr_ISASI.md). It must be set at the device level through the `npu-smi` command. For details about the setting method, see [SetAtomicAdd Constraints](SetAtomicAdd.md#npu_smi_saturation_mode).
<!-- end id4 -->

## Example<a name="section177231425115410"></a>

```cpp
#include "kernel_operator.h"

constexpr uint32_t SIZE = 256;
__aicore__ inline void CopyIn()
{
    AscendC::LocalTensor<T> srcLocal = queueSrc.AllocTensor<T>();
    // Clear the atomic operation status.
    AscendC::DisableDmaAtomic();
    AscendC::DataCopy(srcLocal, srcGlobal, SIZE);
    queueSrc.EnQue(srcLocal);
    // Synchronize across cores.
    AscendC::SyncAll();
}

__aicore__ inline void CopyOut()
{
    AscendC::LocalTensor<T> srcLocal = queueSrc.DeQue<T>();
    // Enable atomic MIN mode. Three cores take the minimum value.
    AscendC::SetAtomicMin<T>();
    AscendC::DataCopy(dstGlobal, srcLocal, SIZE);
    // Disable the atomic operation.
    AscendC::DisableDmaAtomic();
    queueSrc.FreeTensor(srcLocal);
}

/*
Input data of each core: 
Src0: [1,1,1,1,1,...,1] // 256 ones
Src1: [2,2,2,2,2,...,2] // 256 twos
Final output data: [1,1,1,1,1,...,1] // 256 ones
*/
```

For the complete sample, see [DataMovementWithAtomicOperations sample](../../../../../../examples/01_simd_cpp_api/03_basic_api/06_atomic/data_movement_with_atomic_operations).
