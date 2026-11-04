# SubC<a name="ZH-CN_TOPIC_0000002080143865"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T15:59:02.767Z pushedAt=2026-10-26T07:41:44.481Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/basic_arithmetic/SubC_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/reg_compute/kernel_reg_compute_vec_binary_intf.h"`.

This API performs element-wise subtraction on the source operands **srcReg0** and **srcReg1** and the input carry **carrySrc** based on **mask**. The subtraction is implemented at the hardware level through two's complement addition. The result is written to the destination operand **dstReg**, and the carry result of each element is written to **carry** (a **MaskReg** register that stores the carry).

The carry flag (carry/borrow flag) indicates an addition carry or no borrow in subtraction. If the bitwise addition of **srcReg0**, ~**srcReg1**, and **carrySrc** produces a carry at the most significant bit, 1 is set in every 4 bits at the corresponding position in **carry**; otherwise, 0 is written.

The calculation formula is as follows:

$$
\{carry_i, dstReg_i\} = \{1'b0, srcReg0_i\} + \{1'b0, \sim srcReg1_i\} + \{32'b0,carrySrc_i\}
$$

![](../../../../figures/reg_subc_1.png)

Taking the calculation of -1 - 8 = -9 with int64_t data as an example, the applicable scenarios of the Sub/SubC APIs are shown in the following figure:

![](../../../../figures/reg_subc_2.png)

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U>
__simd_callee__ inline void SubC(MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& carrySrc, MaskReg& mask)
```

## Parameters<a name="section389364115374"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [data type](#data-type). |
| U | RegTensor type of the source operand and destination operand, for example, RegTensor&lt;half&gt;. It is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| carry | Output | Destination operand. Outputs the carry value.<br>Type is [MaskReg](../register_data_types/MaskReg.md). |
| dstReg | Output | Destination operand.<br>Type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg0 | Input | Source operand.<br>Type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg1 | Input | Source operand.<br>Type is [RegTensor](../register_data_types/RegTensor.md). |
| carrySrc | Input | Source operand. Inputs the carry value.<br>Type is [MaskReg](../register_data_types/MaskReg.md). |
| mask | Input | Valid indicator for element-wise operations on the source operand. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Data Type

The data type of the destination operand must be consistent with that of the source operand. Supported data types are **int32_t** and **uint32_t**.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section232816306478"></a>

```cpp
template <typename T>
__simd_vf__ inline void SubC(__ubuf__ T* dst0Addr, __ubuf__ T* dst1Addr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint16_t repeatTimes, uint16_t oneRepeatSize){

    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg0;
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::MaskReg maskZero = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALLF>();
    AscendC::Reg::MaskReg carry = AscendC::Reg::CreateMask<uint8_t>();
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::SubC(carry, dstReg0, srcReg0, srcReg1, maskZero, mask);
        // 8*4B=32B align
        AscendC::Reg::StoreAlign<uint32_t, AscendC::Reg::MaskDist::DIST_NORM>((__ubuf__ uint32_t*)dst1Addr + i * 8, carry);
        AscendC::Reg::StoreAlign(dst0Addr + i * oneRepeatSize, dstReg0, mask);
    }
}
```
