# Compares<a name="ZH-CN_TOPIC_0000001956986933"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:01:22.235Z pushedAt=2026-10-26T07:41:44.485Z nl=2 -->

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
- Atlas 200I/500 A2 inference product: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/compare_and_select/Compares_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_vec_cmpsel_intf.h"`.

This API compares each element of the source operand with a scalar element-wise, and determines whether the result is true according to the comparison mode (LT/GT/GE/EQ/NE/LE) specified by the template parameter. If true, the corresponding bit of the output `MaskReg` is set to 1; otherwise, it is set to 0.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, CMPMODE mode = CMPMODE::EQ, typename U, typename S>
__simd_callee__ inline void Compares(MaskReg& dst, U& srcReg, S scalarValue, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :----- | :--- |
| T | Data type of the vector source operand. |
| mode | Comparison mode. The following values are supported:<br>• LT: less than<br>• GT: greater than<br>• GE: greater than or equal to<br>• EQ: equal to<br>• NE: not equal to<br>• LE: less than or equal to |
| U | `RegTensor` type of the vector source operand, which is automatically deduced by the compiler and does not need to be specified by the user. |
| S | Data type of the scalar operand. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dst | Output | [MaskReg](../register_data_types/MaskReg.md) type, destination operand. |
| srcReg | Input | Source operand.<br>Type: [RegTensor](../register_data_types/RegTensor.md). |
| scalarValue | Input | Source operand.<br>Type: scalar. |
| mask | Input | Valid indicator for the operation on source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Return Value<a name="section640mcpsimp"></a>

None

## Data Type

**Table 3** Data type combinations

| srcReg and scalarValue data type | dst data type |
| :-------------------------- | :---------- |
| int8_t | MaskReg |
| uint8_t | MaskReg |
| int16_t | MaskReg |
| uint16_t | MaskReg |
| half | MaskReg |
| bfloat16_t | MaskReg |
| int32_t | MaskReg |
| uint32_t | MaskReg |
| float | MaskReg |
| int64_t | MaskReg |
| uint64_t | MaskReg |

## Constraints<a name="section633mcpsimp"></a>

Unselected elements controlled by the **mask** parameter are set to zero in the destination operand.

## Example<a name="section12223121385116"></a>

```cpp
template<typename T>
__simd_vf__ inline void ComparesVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, T scalarValue, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::MaskReg cmpMaskReg;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);       
        AscendC::Reg::Compares<T, AscendC::CMPMODE::EQ>(cmpMaskReg, srcReg0, scalarValue, mask);
        AscendC::Reg::Select(dstReg, srcReg0, srcReg1, cmpMaskReg);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
