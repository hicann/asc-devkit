# Load<a name="ZH-CN_TOPIC_0000002581346834"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T22:40:34.767Z -->

## Applicable Products<a name="section129404119114"></a>

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

## Description<a name="section12921612161519"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`.

The Reg vector computation data copy API supports copying data from the source address **srcAddr**, which is not 32-byte aligned in UB, to **RegTensor**, with a copy size of VL (256B). For continuous copying, the user must manually update the **srcAddr** address.

This API encapsulates [LoadUnAlignPre and LoadUnAlign](LoadUnAlign_continuous.md).

## Prototype<a name="section429122319217"></a>

```cpp
template <typename T = DefaultType, typename U>
__simd_callee__ inline void Load(U& dstReg, __ubuf__ T* srcAddr)
```

## Parameters<a name="section99612571362"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [data types](#data-type). |
| U | RegTensor type of the target operand, for example, RegTensor\<half>. It is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstReg | Output | Target operand, of the [RegTensor](../register_data_types/RegTensor.md) type. |
| srcAddr | Input | Source operand, the UB start address, which does not need to be 32-byte aligned. |

## Data Type

The data types of the target operation and the source operation must be consistent. Supported data types: b8, b16, b32, and b64.

## Return Value

None

## Constraints

- **dstReg** does not support **RegTraitNumTwo**.
- The API internally defines an [UnalignRegForLoad](../register_data_types/UnalignRegForLoad-UnalignRegForStore.md), and the maximum number of such registers is 4.

## Example

```cpp
template<typename T>
__simd_vf__ inline void LoadStoreVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t count, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::Load(srcReg, srcAddr + i * count);
        AscendC::Reg::Store(dstAddr + i * count, srcReg);
    }
}
```
