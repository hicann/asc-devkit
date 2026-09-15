# CeilDivision

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T11:11:55.796Z pushedAt=2026-09-01T01:37:10.363Z -->

## Applicable Products

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: Not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: Not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Not supported
<!-- end id14 -->

## Description

Computes the result of rounding up the quotient of two integers **num1** and **num2**.

## Prototype

```cpp
__aicore__ constexpr inline int32_t CeilDivision(int32_t num1, int32_t num2)
```

## Parameters

**Table 1** Parameters

| Parameter | Description |
| --- | --- |
| **num1** | Parameter 1, the dividend. |
| **num2** | Parameter 2, the divisor. |

## Return Value

Returns the result of dividing two integers and rounding up.

## Constraints

- When **num2** is 0, the result is 0.
- This API can be used only when both **num1** and **num2** are positive numbers.

## Example

In this example, **CeilDivision** is used to calculate the number of iterations **repeatTimes**. By performing ceiling division on the data volume **count** and the data volume processed per iteration, it ensures that all data (including the tail block) is fully processed.

```cpp
template <typename T>
__aicore__ inline void AddCustomImpl(__local_mem__ T *dst, __local_mem__ T *src0, __local_mem__ T *src1,
    uint32_t count)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(T);
    uint16_t repeatTime = AscendC::CeilDivision(count, oneRepeatSize);
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0 + i * oneRepeatSize );
        AscendC::Reg::LoadAlign(srcReg1, src1 + i * oneRepeatSize );
        AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dst + i * oneRepeatSize, dstReg, mask);
    }
}
```
