# Exp<a name="ZH-CN_TOPIC_0000001929810838"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T15:43:47.134Z pushedAt=2026-10-26T07:41:44.448Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/basic_arithmetic/Exp_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/reg_compute/kernel_reg_compute_vec_unary_intf.h"`.

This API performs an element-wise exponential operation on the source operand **srcReg** according to the mask, and writes the result to the destination operand **dstReg**. The calculation formula is as follows:

$$
dstReg_i = e^{srcReg_i}
$$

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Exp(U& dstReg, U& srcReg, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [Data Type](#data-type). |
| mode | Configurable as a [MaskMergeMode](../aux_data_types/MaskMergeMode.md) enumeration value or a pointer to the ExpSpecificMode structure.<br>&bull; Configuring MaskMergeMode: Selects the MERGING mode or the ZEROING mode.<br>&nbsp;&nbsp;&bull; In ZEROING mode, elements not selected by the mask are set to zero in dstReg.<br>&nbsp;&nbsp;&bull; The MERGING mode is currently not supported.<br>&bull; Configuring ExpSpecificMode, defined as follows:<br><code>enum class ExpAlgo {<br>    INTRINSIC = 0,<br>    PRECISION_1ULP_FTZ_TRUE,<br>    PRECISION_1ULP_FTZ_FALSE,<br>};<br>struct ExpSpecificMode {<br>    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;<br>    ExpAlgo algo = ExpAlgo::INTRINSIC;<br>};</code><br>&bull; mrgMode: Selects the MERGING mode or the ZEROING mode.<br>&bull; algo: Selects the Exp algorithm and configures the Subnormal mode. For details, see [Key Feature Description](#key-feature-description).<br>&nbsp;&nbsp;&bull; ExpAlgo::INTRINSIC: Default algorithm, with a maximum precision error of 1 ulp. For half and float types, Subnormal handling is controlled by the --cce-ftz compilation option (default value: true).<br>&nbsp;&nbsp;&bull; ExpAlgo::PRECISION_1ULP_FTZ_TRUE: Computed using a single instruction, with a maximum precision error of 1 ulp.<br>&nbsp;&nbsp;&bull; ExpAlgo::PRECISION_1ULP_FTZ_FALSE: Implemented through software simulation, supporting Subnormal data computation, with a maximum precision error of 1 ulp. |
| U | RegTensor type of the source operand and destination operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstReg | Output | Destination operand.<br>Type: [RegTensor](../register_data_types/RegTensor.md). |
| srcReg | Input | Source operand.<br>Type: [RegTensor](../register_data_types/RegTensor.md). |
| mask | Input | Validity indicator for element operations on the source operand. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Data Type

The data types of the destination operand and the source operand must be consistent. Supported data types are **half** and **float**.

## Return Value Description<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Key Feature Description

### Maximum Precision Error

The maximum precision error of **ExpAlgo::INTRINSIC**, **ExpAlgo::PRECISION_1ULP_FTZ_TRUE**, and **ExpAlgo::PRECISION_1ULP_FTZ_FALSE** is 1 ulp.

### Configuring Subnormal Mode

FTZ (Flush-To-Zero): A floating-point operation mode in which, when a result is subnormal, it is directly flushed to zero (approximated as 0) instead of retaining its precise tiny value.

#### Default Algorithm

**ExpAlgo::INTRINSIC** is the default algorithm. When **--cce-ftz=false**, Subnormal values are preserved; when **--cce-ftz=true** (the default value), FTZ mode is used.

#### Explicitly Specified Algorithm

- **ExpAlgo::PRECISION_1ULP_FTZ_TRUE** uses a single instruction for computation and always adopts the FTZ mode.
- **ExpAlgo::PRECISION_1ULP_FTZ_FALSE** is implemented through software simulation and supports Subnormal data computation.

#### Usage Recommendations

Because the computation behavior that preserves Subnormal is implemented through software simulation, when **--cce-ftz=true** (the default value) is used, it is recommended to use the default **ExpAlgo::INTRINSIC** or explicitly select **ExpAlgo::PRECISION_1ULP_FTZ_TRUE** in general scenarios to achieve better performance. When precise Subnormal output is required, use **ExpAlgo::PRECISION_1ULP_FTZ_FALSE**.

**Table 3**  Exp Subnormal example

| Output | Input | Output Binary | Input Binary | Data Type |
| --- | --- | --- | --- | --- |
| 6.0975551605e-05 | -9.703125 | 0 00000 1111111111 | 1 10010 0011011010 | half |
| 5.9604644775e-08 | -16.640625 | 0 00000 0000000001 | 1 10011 0000101001 | half |
| 1.1754942107e-38 | -87.3365478515625 | 0 00000000 11111111111111111111111 | 1 10000101 01011101010110001010000 | float |
| 1.401298464324817e-45 | -103.2789306640625 | 0 00000000 00000000000000000000001 | 1 10000101 10011101000111011010000 | float |

## Example<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void ExpVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    // 1ulp precision and Subnormal mode.
    // static constexpr AscendC::Reg::ExpSpecificMode mode = {
    //     AscendC::Reg::MaskMergeMode::ZEROING, AscendC::ExpAlgo::PRECISION_1ULP_FTZ_FALSE};
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Exp(dstReg, srcReg, mask);
        // 1ulp precision and Subnormal mode.
        // AscendC::Reg::Exp<T, &mode>(dstReg, srcReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
