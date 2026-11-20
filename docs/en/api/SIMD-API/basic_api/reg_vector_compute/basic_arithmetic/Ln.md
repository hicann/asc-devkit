# Ln<a name="ZH-CN_TOPIC_0000002052905730"></a>

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-05T02:23:32.208Z pushedAt=2026-11-18T01:25:30.806Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/basic_arithmetic/Ln_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/reg_compute/kernel_reg_compute_vec_unary_intf.h"`.

This API computes the natural logarithm of the source operand element-wise according to the mask and writes the result to the destination operand. The calculation formula is as follows:

$$
dstReg_i = \ln(srcReg_i)
$$

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Ln(U& dstReg, U& srcReg, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [Data Types](#data-types). |
| mode | Can be configured as a [MaskMergeMode](../aux_data_types/MaskMergeMode.md) enumeration value or a pointer to the LnSpecificMode structure.<br>&bull; Configure MaskMergeMode: select the MERGING mode or the ZEROING mode.<br>&nbsp;&nbsp;&bull; In the ZEROING mode, elements not selected by the mask are set to zero in dstReg.<br>&nbsp;&nbsp;&bull; The MERGING mode is not supported currently.<br>&bull; Configure LnSpecificMode, which is defined as follows:<br><code>enum class LnAlgo {<br>    INTRINSIC = 0,<br>    PRECISION_1ULP_FTZ_TRUE,<br>    PRECISION_1ULP_FTZ_FALSE,<br>};<br>struct LnSpecificMode {<br>    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;<br>    LnAlgo algo = LnAlgo::INTRINSIC;<br>};</code><br>&bull; mrgMode: select the MERGING mode or the ZEROING mode.<br>&bull; algo: used to select the Ln algorithm and configure the Subnormal mode. For details, see [Key Features](#key-features).<br>&nbsp;&nbsp;&bull; LnAlgo::INTRINSIC: default algorithm, with a maximum precision error of 1 ulp. For the half and float types, Subnormal handling is controlled by the --cce-ftz compilation option (default value: true).<br>&nbsp;&nbsp;&bull; LnAlgo::PRECISION_1ULP_FTZ_TRUE: computed with a single instruction, with a maximum precision error of 1 ulp.<br>&nbsp;&nbsp;&bull; LnAlgo::PRECISION_1ULP_FTZ_FALSE: implemented through software emulation, supports Subnormal data computation, with a maximum precision error of 1 ulp. |
| U | RegTensor type of the source operand and the destination operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstReg | Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| mask | Input | Valid indicator for the operation on the source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Data Types

The data types of the destination operand and the source operand must be consistent. Supported data types: half and float.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Key Features

### Maximum Precision Error

The maximum precision error of **LnAlgo::INTRINSIC**, **LnAlgo::PRECISION_1ULP_FTZ_TRUE**, and **LnAlgo::PRECISION_1ULP_FTZ_FALSE** is 1 ulp.

### Configuring Subnormal Mode

FTZ (Flush-To-Zero): A floating-point operation mode in which a Subnormal result is directly set to zero (approximated as 0) instead of retaining its exact tiny value.

#### Default Algorithm

**LnAlgo::INTRINSIC** is the default algorithm. When **--cce-ftz=false**, Subnormal values are preserved; when **--cce-ftz=true** (the default value), FTZ mode is used.

#### Explicitly Specified Algorithms

- LnAlgo::PRECISION_1ULP_FTZ_TRUE uses a single instruction for computation and always adopts the FTZ mode.
- LnAlgo::PRECISION_1ULP_FTZ_FALSE is implemented through software simulation and supports Subnormal data computation.

#### Usage Recommendations

Because the computation behavior that preserves Subnormal is implemented through software simulation, when **--cce-ftz=true** (the default value), it is recommended to use the default **LnAlgo::INTRINSIC** or explicitly select **LnAlgo::PRECISION_1ULP_FTZ_TRUE** in general scenarios to achieve better performance. When Subnormal needs to be output accurately, use **LnAlgo::PRECISION_1ULP_FTZ_FALSE**.

## Example<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void LnVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    // 1ulp precision and Subnormal mode.
    // static constexpr AscendC::Reg::LnSpecificMode mode = {
    //     AscendC::Reg::MaskMergeMode::ZEROING, AscendC::LnAlgo::PRECISION_1ULP_FTZ_FALSE};
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Ln(dstReg, srcReg, mask);
        // 1ulp precision and Subnormal mode.
        // AscendC::Reg::Ln<T, &mode>(dstReg, srcReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
