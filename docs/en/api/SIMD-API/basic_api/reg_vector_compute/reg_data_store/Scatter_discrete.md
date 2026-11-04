# Scatter<a name="ZH-CN_TOPIC_0000001929080422"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:50:21.226Z pushedAt=2026-10-26T07:41:44.584Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reg_data_store/Scatter_discrete_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`.

This instruction scatters the elements in the source operand **srcReg** into the destination operand Unified Buffer (UB) according to the index value **index**. The scattering process is shown in Figure 1:

**Figure 1** Scatter description

![Figure 1 Scatter description](../../../../figures/reg_scatter.png)

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U = DefaultType, typename S, typename V>
__simd_callee__ inline void Scatter(__ubuf__ T* baseAddr, S& srcReg, V& index, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

| Parameter | Description |
|-----|-----|
| T | Data type of the destination operand. For supported data types, see [Data Types](#data-types). |
| U | Data type of the index value **index**. For supported data types, see [Data Types](#data-types). |
| S | RegTensor type of the source operand, for example RegTensor\<half>, which is automatically inferred by the compiler and does not need to be specified by the user. |
| V | RegTensor type of the index value, for example RegTensor\<uint16_t>, which is automatically inferred by the compiler and does not need to be specified by the user. |

**Table 2**  Parameter description

| Parameter | Input/Output | Description |
|-----|-----|-----|
| baseAddr | Output | Destination operand, the base address in UB, which must be 32-byte aligned. |
| srcReg | Input | Source operand, of type [RegTensor](../register_data_types/RegTensor.md). |
| index | Input | Index value, the position of each element in **srcReg** relative to **baseAddr** in UB, in units of element count. The type is [RegTensor](../register_data_types/RegTensor.md). <br>Note: The values in **index** must be unique. If duplicate **index** values exist, the system retains only one of the corresponding data items and ignores the rest. It is impossible to determine which one is retained, so ensure that the **index** values are not duplicated. |
| mask | Input | Valid indicator for the operation on source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Data Types

**Table 3**  Scatter operand data type mapping table

| destination operand | index value |
|-----|-----|
| int8_t | uint16_t |
| uint8_t | uint16_t |
| int16_t | uint16_t |
| uint16_t | uint16_t |
| half | uint16_t |
| bfloat16_t | uint16_t |
| int32_t | uint32_t |
| uint32_t | uint32_t |
| float | uint32_t |
| int64_t | uint32_t |
| int64_t | uint64_t |
| uint64_t | uint32_t |
| uint64_t | uint64_t |

## Return Value

None

## Constraints<a name="section177921451558"></a>

- The start address in **UB** must be 32B-aligned.
- When **T** is **int8_t** or **uint8_t**, only the even-indexed elements in the source operand are valid. That is, the data at even positions [0, 2, 4, ..., 252, 254] in **srcReg** are scattered to the destination operand.
- The values in **index** must be unique. If duplicate **index** values exist, the system retains only one of the corresponding data items and ignores the rest. Since it is impossible to determine which one is retained, ensure that the **index** values are not duplicated.
- When the data type **T** of the destination operand is **B64**, **T**, **U**, **S**, and **V** support only the following combinations:

    | T | U | S (automatic inference) | V (automatic inference) | Remarks |
    |-----|-----|-----|-----|-----|
    | int64_t | uint32_t | RegTensor<int64_t, RegTraitNumOne> | RegTensor<uint32_t> | The first 32 values of index are valid. |
    | int64_t | uint32_t | RegTensor<int64_t, RegTraitNumTwo> | RegTensor<uint32_t> | - |
    | int64_t | uint64_t | RegTensor<int64_t, RegTraitNumOne> | RegTensor<uint64_t, RegTraitNumOne> | - |
    | int64_t | uint64_t | RegTensor<int64_t, RegTraitNumOne> | RegTensor<uint64_t, RegTraitNumTwo> | The first 32 values of index are valid. |
    | int64_t | uint64_t | RegTensor<int64_t, RegTraitNumTwo> | RegTensor<uint64_t, RegTraitNumTwo> | - |
    | uint64_t | uint32_t | RegTensor<uint64_t, RegTraitNumOne> | RegTensor<uint32_t> | The first 32 values of index are valid. |
    | uint64_t | uint32_t | RegTensor<uint64_t, RegTraitNumTwo> | RegTensor<uint32_t> | - |
    | uint64_t | uint64_t |  RegTensor<uint64_t, RegTraitNumOne> | RegTensor<uint64_t, RegTraitNumOne> | - |
    | uint64_t | uint64_t | RegTensor<uint64_t, RegTraitNumOne> | RegTensor<uint64_t, RegTraitNumTwo> | The first 32 values of index are valid. |
    | uint64_t | uint64_t | RegTensor<uint64_t, RegTraitNumTwo> | RegTensor<uint64_t, RegTraitNumTwo> | - |

## Example<a name="section642mcpsimp"></a>

```cpp
template <typename T, typename U>
__simd_vf__ inline void ScatterVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ U* indexAddr, uint32_t count, uint16_t oneRepeatSize)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<U> indexReg;
    AscendC::Reg::MaskReg mask;
    uint16_t repeatTimes = AscendC::CeilDivision(count, oneRepeatSize);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(indexReg, indexAddr + i * oneRepeatSize);
        AscendC::Reg::Scatter(dstAddr, srcReg, indexReg, mask);
    }
}
```
