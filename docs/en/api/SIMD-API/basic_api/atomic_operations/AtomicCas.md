# AtomicCas<a name="ZH-CN_TOPIC_0000002568264812"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T10:20:41.375Z pushedAt=2026-10-26T07:41:43.946Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/atomic_operations/AtomicCas_res.md#id1 -->

## Description<a name="section163431416121118"></a>

The header file path is `"basic_api/kernel_operator_atomic_intf.h"`.

After this API is called, an atomic comparison operation is performed on the specified GM address. The old value (`old_value`) at the GM address pointed to by `address` is read and compared with the input scalar value `value1`:

- If they are equal, the input scalar value `value2` is written to the GM address.
- If they are not equal, the value at the GM address remains unchanged.
The value at the address before modification (`old_value`) is returned.

The calculation formula is as follows:

$$
new\_value = (old\_value == value1\ ?\ value2\ :\ old\_value)
$$

## Prototype<a name="section1171112415119"></a>

```cpp
template <typename T>
__aicore__ inline T AtomicCas(__gm__ T *address, T value1, T value2)
```

## Parameters<a name="section086962912112"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| address | Input | Address of the input GM. |
| value1/value2 | Input | Scalar values. The supported data type must be consistent with the data type pointed to by **address**. |

## Data Types<a name="section832119018128"></a>

**address**, **value1**/**value2**, and the return value share the same data type, which can be **uint32_t** or **uint64_t**.

## Return Value<a name="section97313820116"></a>

The data before the atomic operation on the GM address.

## Constraints<a name="section24727811123"></a>

- The Bisheng Compiler can automatically insert synchronization between some APIs belonging to **PIPE_S** and APIs belonging to **PIPE_V**, **PIPE_MTE2**, and **PIPE_MTE3** (for details, see [Usage Constraints](../sync_control/intra_core_sync/key_features.md#usage-constraints-1)). However, scalar atomic operation APIs are not within this scope. Therefore, when **AtomicCas** and data movement APIs (belonging to **PIPE_MTE2** and **PIPE_MTE3**) have data dependencies in reading from and writing to GM, the Bisheng Compiler cannot automatically insert synchronization, and developers need to manually insert synchronization based on the actual situation.
- Scalar atomic operations bypass the DCache. You need to call [DataCacheCleanAndInvalid](../cache_control/DataCacheCleanAndInvalid.md) to ensure consistency between GM and the DCache.

## Example<a name="section191505489122"></a>

```cpp
AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
AscendC::LocalTensor<uint32_t> yLocal = ubAllocator.Alloc<uint32_t>(DATA_SIZE);

AscendC::GlobalTensor<uint32_t> yGlobal;
yGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ uint32_t*>(y), DATA_SIZE);

// AtomicCas: Each of the three cores performs an atomic compare-and-swap on the first element of GM.
uint32_t expected = 1;
uint32_t newValue = 2;
AscendC::AtomicCas(reinterpret_cast<__gm__ uint32_t*>(y), expected, newValue);

// Insert synchronization after the atomic operation.
AscendC::SetFlag<AscendC::HardEvent::S_MTE2>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::S_MTE2>(EVENT_ID0);
AscendC::DataCopy(yLocal, yGlobal, DATA_SIZE);
```

For the complete sample, see [scalar_atomic_operations sample](../../../../../../examples/01_simd_cpp_api/03_basic_api/06_atomic/scalar_atomic_operations).
