# Select<a name="ZH-CN_TOPIC_0000001929668292"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:02:45.399Z pushedAt=2026-10-26T07:41:44.488Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/compare_and_select/Select_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_vec_cmpsel_intf.h"`.

Given two source operands **srcReg0** and **srcReg1**, elements are selected based on the bit values of **mask** to obtain the destination operand **dstReg**. When a bit of **mask** is 1, the element at the corresponding position is selected from **srcReg0**; when the bit is 0, the element at the corresponding position is selected from **srcReg1**.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U>
__simd_callee__ inline void Select(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :----- | :--- |
| T | Operand data type. |
| U | `RegTensor` type of the source operand and destination operand, for example, `RegTensor<half>`, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dstReg | Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg0, srcReg1 | Input | Source operands.<br>The type is [RegTensor](../register_data_types/RegTensor.md).<br>The data types of the two source operands must be consistent with that of the destination operand. |
| mask | Input | Specifies the rule for selecting the source operand: when a bit of mask is 1, srcReg0 is selected; when the bit is 0, srcReg1 is selected. |

## Return Value<a name="section640mcpsimp"></a>

None

## Data Types<a name="zh-cn_topic_0000002598723617_section1962315623118"></a>

The supported data types are: bool, int8\_t, uint8\_t, int16\_t, uint16\_t, half, bfloat16\_t, int32\_t, uint32\_t, float, int64\_t, uint64\_t.

**Table 3**  Data type combinations

| srcReg0 and srcReg1 data type | dst data type |
| :----------------------- | :---------- |
| bool | bool |
| uint8_t | uint8_t |
| int8_t | int8_t |
| uint16_t | uint16_t |
| int16_t | int16_t |
| uint32_t | uint32_t |
| int32_t | int32_t |
| half | half |
| float | float |
| bfloat16_t | bfloat16_t |
| uint64_t | uint64_t |
| int64_t | int64_t |

## Constraints<a name="section633mcpsimp"></a>

Elements not selected by the **mask** parameter retain the value of the source operand instead of being cleared to zero.

## Example<a name="section12223121385116"></a>

```cpp
template<typename T>
__simd_vf__ inline void SelectVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::MaskReg cmpReg;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);        
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);      
        AscendC::Reg::Compare<T, AscendC::CMPMODE::EQ>(cmpReg, srcReg0, srcReg1, mask);
        AscendC::Reg::Select(dstReg, srcReg0, srcReg1, cmpReg);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
