# Load<a name="ZH-CN_TOPIC_0000002581346834"></a>

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-05T03:20:16.463Z pushedAt=2026-11-18T01:25:30.921Z nl=2 -->

## Applicable Products<a name="section129404119114"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reg_data_load/Load_res.md#id1 -->

## Description<a name="section12921612161519"></a>

The header file path is: `"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`.

Reg vector data copy API that copies data from the source address srcAddr, which is not 32-byte aligned, in the Unified Buffer (UB) to a RegTensor. When the RegTensor template parameter regTrait is RegTraitNumOne, the copy size is VL (256B); when regTrait is RegTraitNumTwo, the copy size is 2*VL (512B). For continuous copy, you need to manually update the srcAddr address.

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
| T | Operand data type. For supported data types, see [Data Types](#data-types). |
| U | RegTensor type of the target operand, for example RegTensor\<half>, which is automatically deduced by the compiler and does not need to be filled in by the user. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstReg | Output | Destination operand, of type [RegTensor](../register_data_types/RegTensor.md). |
| srcAddr | Input | Source operand, the UB start address, which does not need to be 32-byte aligned. |

## Data Types

The data types of the destination operand and the source operand must be consistent.

- When the **RegTensor** template parameter **regTrait** is **RegTraitNumOne**, the supported data types are: b8, b16, b32, b64.
- When the **RegTensor** template parameter **regTrait** is **RegTraitNumTwo**, the supported data types are: complex32, b64.

## Return Value

None

## Constraints

- When the RegTensor template parameter regTrait is RegTraitNumTwo, the srcAddr start position must have at least 2*VL (512B) of readable data.
- The API internally defines an [UnalignRegForLoad](../register_data_types/UnalignRegForLoad-UnalignRegForStore.md), whose register count is capped at 4. Exceeding this limit causes a compiler error.

## Example

```cpp
template<typename T>
__simd_vf__ inline void LoadStoreVF(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t count, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::Load(srcReg, srcAddr + i * count);
        AscendC::Reg::Store(dstAddr + i * count, srcReg);
    }
}
```
