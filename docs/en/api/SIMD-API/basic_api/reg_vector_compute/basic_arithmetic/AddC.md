# AddC<a name="ZH-CN_TOPIC_0000002008389964"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T15:38:42.517Z pushedAt=2026-10-26T07:41:44.441Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/basic_arithmetic/AddC_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/reg_compute/kernel_reg_compute_vec_binary_intf.h"`.

This API performs an element-wise sum operation on the source operands **srcReg0**, **srcReg1**, and the input carry **carrySrc** according to the mask, writes the result to the destination operand **dstReg**, and writes the carry result of each element to **carry** (the MaskReg register that stores the carry).

The carry flag (carry/borrow flag) indicates an addition carry or a subtraction without borrow. If the highest bit of the bitwise sum of **srcReg0**, **srcReg1**, and **carrySrc** produces a carry, each 4-bit group at the corresponding position in **carry** is set to 1; otherwise, 0 is written.

The calculation formula is as follows:

$$
\{carry_i, dstReg_i\} = srcReg0_i + srcReg1_i + carrySrc_i
$$

![](../../../../figures/reg_addc_1.png)

Taking the calculation of -1 + 8 = 7 with int64_t data as an example, the applicable scenarios of the **Add**/**AddC** APIs are shown in the following figure:

![](../../../../figures/reg_addc_2.png)

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U>
__simd_callee__ inline void AddC(MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& carrySrc, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [Data Type](#data-type). |
| U | RegTensor type of the destination operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| carry | Output | Destination operand. Outputs the carry value.<br>Type is [MaskReg](../register_data_types/MaskReg.md). |
| dstReg | Output | Destination operand.<br>Type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg0 | Input | Source operand.<br>Type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg1 | Input | Source operand.<br>Type is [RegTensor](../register_data_types/RegTensor.md). |
| carrySrc | Input | Input carry register.<br>Type is [MaskReg](../register_data_types/MaskReg.md). |
| mask | Input | Valid indicator for element-wise operations on the source operand. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Data Type

The data types of the destination operand and the source operand must be consistent. The supported data types are: **int32_t**, **uint32_t**.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

```cpp
template <typename T>
static __simd_vf__ inline void AddVF(__ubuf__ T* dst0Addr, __ubuf__ T* dst1Addr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint16_t repeatTimes, uint16_t oneRepeatSize){
    
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
        AscendC::Reg::AddC(carry, dstReg0, srcReg0, srcReg1, maskZero, mask);
        // 8*4B=32B align
        AscendC::Reg::StoreAlign<uint32_t, AscendC::Reg::MaskDist::DIST_NORM>((__ubuf__ uint32_t*)dst1Addr + i * 8, carry);
        AscendC::Reg::StoreAlign(dst0Addr + i * oneRepeatSize, dstReg0, mask);
    }
}
```
