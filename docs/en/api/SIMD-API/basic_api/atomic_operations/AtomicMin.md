# AtomicMin<a name="ZH-CN_TOPIC_0000002568105162"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-27T14:10:41.692Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description<a name="section163431416121118"></a>

The header file path is `"basic_api/kernel_operator_atomic_intf.h"`.

This API performs an atomic minimum operation on a specified GM address. It compares the old value (**old_value**) at the GM address pointed to by **address** with the input scalar value (**value**), writes the smaller value (**new_value**) back to the GM address, and returns the value before modification (**old_value**).

The calculation formula is as follows:

$$
new\_value = Min(old\_value, value)
$$

## Prototype<a name="section1171112415119"></a>

```cpp
template <typename T>
__aicore__ inline T AtomicMin(__gm__ T *address, T value)
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
| value | Input | Scalar value. The supported data type is the same as the data type pointed to by **address**. |

## Data Types<a name="section832119018128"></a>

**address**, **value**, and the return value share the same data type. The supported data types are **int32_t**, **uint32_t**, **float**, **int64_t**, and **uint64_t**.

## Return Value<a name="section97313820116"></a>

Returns the data at the GM address before the atomic operation.

## Constraints<a name="section24727811123"></a>

- When the compiler automatic synchronization feature is enabled (see [Automatic Synchronization Usage Constraints](../sync_control/intra_core_sync/key_feature_description.md#automatic-synchronization-constraints)), the compiler can automatically insert synchronization between **PIPE_MTE2**/**PIPE_MTE3** and **PIPE_S** (see [Compiler Automatic Synchronization](../sync_control/intra_core_sync/key_feature_description.md#tpipe-tque-framework-programming-paradigm-and-compiler-automatic-synchronization-support)). However, **AtomicMin** is a scalar operation. When reading from and writing to GM, if there is a data dependency with the transfer units (**MTE2**/**MTE3**), the compiler cannot automatically insert synchronization. In this case, developers need to manually insert synchronization based on the actual situation.
- Scalar atomic operations bypass the DCache. You need to call the [DataCacheCleanAndInvalid](../Cache Control/DataCacheCleanAndInvalid.md) API to ensure consistency between GM and the DCache.

## Example<a name="section191505489122"></a>

```cpp
AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
AscendC::LocalTensor<uint32_t> yLocal = ubAllocator.Alloc<uint32_t>(DATA_SIZE);

AscendC::GlobalTensor<uint32_t> yGlobal;
yGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ uint32_t*>(y), DATA_SIZE);

// AtomicMin: Three cores each atomically compute the minimum value of the first element in GM.
uint32_t value = 1;
AscendC::AtomicMin(reinterpret_cast<__gm__ uint32_t*>(y), value);

// Insert synchronization after the atomic operation.
AscendC::SetFlag<AscendC::HardEvent::S_MTE2>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::S_MTE2>(EVENT_ID0);
AscendC::DataCopy(yLocal, yGlobal, DATA_SIZE);
```

For the complete sample, see [scalar_atomic_operations sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/06_atomic/scalar_atomic_operations).
