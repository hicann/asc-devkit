# SetAtomicAdd<a name="ZH-CN_TOPIC_0000001487536650"></a>

<!-- md-trans-meta sourceCommit=ab2c72ba23cbc7229f8b141e516d96812390984a translatedAt=2026-08-27T14:12:00.423Z -->

## Applicable Products

<!-- npu="950" id13 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id13 -->
<!-- npu="A3" id14 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id14 -->
<!-- npu="910b" id15 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id15 -->
<!-- npu="310b" id16 -->
- Atlas 200I/500 A2 inference product: Supported
<!-- end id16 -->
<!-- npu="310p" id17 -->
- Atlas inference products AI Core: Supported
<!-- end id17 -->
<!-- npu="310p" id18 -->
- Atlas inference products Vector Core: Not supported
<!-- end id18 -->
<!-- npu="910" id19 -->
- Atlas training products: Supported
<!-- end id19 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_set_atomic_intf.h"`.

Enables atomic accumulation for subsequent data transfers whose destination address is GM. The atomic accumulation process sums the content to be copied with the existing content in GM, and then writes the summation result to GM. The **SetAtomicAdd** API can set different accumulation data types through template parameters.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void SetAtomicAdd()
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

| Parameter | Description |
|--------|------|
| T | Sets the data type for different atomic accumulation operations. |

## Data Types

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: supported data type is int8_t, int16_t, half, bfloat16_t, int32_t, float.
<!-- end id1 -->

<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: supported data type is int8_t, int16_t, half, bfloat16_t, int32_t, float.
<!-- end id2 -->

<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: supported data type is int8_t, int16_t, half, bfloat16_t, int32_t, float.
<!-- end id3 -->

<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: supported data type is int16_t, half, int32_t, float.
<!-- end id4 -->

<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported data type is int16_t, half, float.
<!-- end id5 -->

<!-- npu="910" id6 -->
- Atlas training products: supported data type is float.
<!-- end id6 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

<!-- npu="950" id7 -->
- Ascend 950PR/Ascend 950DT: The supported data path is UB/L0C Buffer->GM.
<!-- end id7 -->

<!-- npu="A3" id8 -->
- Atlas A3 training products/Atlas A3 inference products: The supported data path is UB/L0C Buffer/L1 Buffer->GM.
<!-- end id8 -->

<!-- npu="910b" id9 -->
- Atlas A2 training products/Atlas A2 inference products: The supported data path is UB/L0C Buffer/L1 Buffer->GM.
<!-- end id9 -->

<!-- npu="310b" id10 -->
- Atlas 200I/500 A2 inference product: The supported data path is UB/L0C Buffer/L1 Buffer->GM.
<!-- end id10 -->

<!-- npu="310p" id11 -->
- Atlas inference products AI Core: The supported data path is UB->GM.
<!-- end id11 -->

<!-- npu="910" id12 -->
- Atlas training products: The supported data path is UB->GM.
<!-- end id12 -->

- After the accumulation operation is complete, it is recommended to disable atomic accumulation by calling [DisableDmaAtomic](DisableDmaAtomic.md) to avoid affecting subsequent related instruction functions.
- This API does not automatically clear the GM data before execution. Developers need to determine whether to clear it based on the operator logic. If clearing is required, perform it manually before execution.
- **SetAtomicAdd** already integrates the same functionality as [SetAtomicType](SetAtomicType.md). It is recommended that when calling the above API, you explicitly specify the data type of the atomic operation by setting the template parameter, without the need to additionally call the **SetAtomicType** API.

## Example<a name="section177231425115410"></a>

```cpp
AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
AscendC::LocalTensor<T> srcLocal = ubAllocator.Alloc<T, SIZE>();

AscendC::DisableDmaAtomic();
AscendC::DataCopy(srcLocal, srcGlobal, SIZE);
AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::SyncAll();

// Enable atomic accumulation to atomically accumulate UB data into GM.
AscendC::SetAtomicAdd<T>();
AscendC::DataCopy(dstGlobal, srcLocal, SIZE);

// Disable atomic accumulation.
AscendC::DisableDmaAtomic();
```

For the complete sample, see [DataMovementWithAtomicOperations sample](https://gitcode.com/cann/asc-devkit/blob/9.1.0/examples/01_simd_cpp_api/03_basic_api/06_atomic/data_movement_with_atomic_operations/data_movement_with_atomic_operations.asc).
