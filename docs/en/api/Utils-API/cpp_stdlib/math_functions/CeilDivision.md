# CeilDivision

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:12:30.713Z pushedAt=2026-11-13T08:46:49.209Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/math_functions/CeilDivision_res.md#id1 -->

## Description

Computes the result of rounding up the division of two integers, num1 and num2.

## Prototype

```cpp
__aicore__ constexpr inline int32_t CeilDivision(int32_t num1, int32_t num2)
```

## Parameters

**Table 1**  Parameters

| Parameter | Description |
| --- | --- |
| num1 | Parameter 1, the dividend. |
| num2 | Parameter 2, the divisor. |

## Return Value

Returns the result of rounding up the division of two integers.

## Constraints

- When num2 is 0, the result is 0.
- This API supports only the scenario where num1 and num2 are positive numbers.

## Examples

In this example, CeilDivision is used to compute the number of iterations repeatTimes. By rounding up the division of the data count by the amount of data processed per iteration, it ensures that all data, including the tail block, is fully processed.

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
