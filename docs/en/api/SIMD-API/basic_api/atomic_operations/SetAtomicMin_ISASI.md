# SetAtomicMin\(ISASI\)<a name="ZH-CN_TOPIC_0000001637481306"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T14:13:17.912Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id11 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id11 -->
<!-- npu="A3" id12 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id12 -->
<!-- npu="910b" id13 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id13 -->
<!-- npu="310b" id14 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products AI Core: Not supported
<!-- end id15 -->
<!-- npu="310p" id16 -->
- Atlas inference products Vector Core: Not supported
<!-- end id16 -->
<!-- npu="910" id17 -->
- Atlas training products: Not supported
<!-- end id17 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_set_atomic_intf.h"`.

Sets whether to perform an atomic comparison on the data subsequently copied to GM: compares the content to be copied with the existing content in GM, and then writes the minimum value to GM. The **SetAtomicMin** API can set different data types through template parameters.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void SetAtomicMin() 
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
|--------|------|
| T | Sets the data type for different atomic minimum operations. |

## Data Types

The supported data types are **int8_t**, **int16_t**, **half**, **bfloat16_t**, **int32_t**, and **float**.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: The supported data path is UB/L0C Buffer->GM.
<!-- end id1 -->

<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: The supported data path is UB/L0C Buffer/L1 Buffer->GM.
<!-- end id2 -->

<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: The supported data path is UB/L0C Buffer/L1 Buffer->GM.
<!-- end id3 -->

- After use, it is recommended to disable the atomic minimum operation through [DisableDmaAtomic](DisableDmaAtomic.md) to avoid affecting subsequent related functions.
- This API does not automatically clear the GM data before execution. Developers need to determine whether to clear it based on the operator logic. If clearing is required, perform it manually before execution.
- **SetAtomicMin** has integrated the same functionality as [SetAtomicType](SetAtomicType.md). It is recommended that when calling the above API, you explicitly specify the data type of the atomic operation by setting the template parameter, without the need to additionally call the **SetAtomicType** API.

## Example<a name="section177231425115410"></a>

```cpp
#include "kernel_operator.h"

constexpr uint32_t SIZE = 256;
__aicore__ inline void CopyIn()
{
    AscendC::LocalTensor<T> srcLocal = queueSrc.AllocTensor<T>();
    // Clear the atomic operation state.
    AscendC::DisableDmaAtomic();
    AscendC::DataCopy(srcLocal, srcGlobal, SIZE);
    queueSrc.EnQue(srcLocal);
    // Synchronize across cores.
    AscendC::SyncAll();
}

__aicore__ inline void CopyOut()
{
    AscendC::LocalTensor<T> srcLocal = queueSrc.DeQue<T>();
    // Enable the atomic MIN mode so that the three cores take the minimum value.
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

For the complete sample, see [DataMovementWithAtomicOperations sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/06_atomic/data_movement_with_atomic_operations).
