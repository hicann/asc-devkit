# Sqrt<a name="ZH-CN_TOPIC_0000001929668256"></a>

<!-- md-trans-meta sourceCommit=af3f6adafe94d121d34283378bfbe7e348767ea3 translatedAt=2026-11-05T02:37:57.871Z pushedAt=2026-11-18T01:25:30.831Z nl=2 -->

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
<!-- npu="9050" id8 -->
- Kirin 9050 processor family products: Supported
<!-- end id8 -->
<!-- npu="kirin1" id9 -->
- KirinDev0001 processor family products: Supported
<!-- end id9 -->
<!-- npu="kirin2" id10 -->
- KirinDev0002 processor family products: Supported
<!-- end id10 -->
<!-- npu="kirin3" id11 -->
- KirinDev0003 processor family products: Supported
<!-- end id11 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/basic_arithmetic/Sqrt_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/reg_compute/kernel_reg_compute_vec_unary_intf.h"`.

This API performs an element-wise square root operation on the source operand **srcReg** according to the mask and writes the result to the destination operand **dstReg**. The calculation formula is as follows:

$$
dstReg_i = (srcReg_i)^{1/2}
$$

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Sqrt(U& dstReg, U& srcReg, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [Data Type](#data-type). |
| mode | Can be configured as a [MaskMergeMode](../aux_data_types/MaskMergeMode.md) enumeration value or a pointer to the SqrtSpecificMode structure.<br>&bull; Configure MaskMergeMode: select the MERGING mode or the ZEROING mode.<br>&nbsp;&nbsp;&bull; In the ZEROING mode, elements not selected by the mask are set to zero in dstReg.<br>&nbsp;&nbsp;&bull; The MERGING mode is currently not supported.<br>&bull; Configure SqrtSpecificMode, which is defined as follows:<br><code>enum class SqrtAlgo {<br>    INTRINSIC = 0,<br>    FAST_INVERSE,<br>    PRECISION_1ULP_FTZ_TRUE,<br>    PRECISION_0ULP_FTZ_FALSE,<br>    PRECISION_1ULP_FTZ_FALSE,<br>};<br>struct SqrtSpecificMode {<br>    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;<br>    bool precisionMode = false;<br>    SqrtAlgo algo = SqrtAlgo::INTRINSIC;<br>};</code><br>&bull; mrgMode: select the MERGING mode or the ZEROING mode.<br>&bull; precisionMode: used to configure the precision mode. When precisionMode is true, higher-precision Sqrt computation is enabled, and the result is obtained using the fast inverse algorithm. Currently, this algorithm takes effect only for the float data type.<br>&bull; algo: used to select the Sqrt algorithm and configure the Subnormal mode. For details, see [Key Features](#key-features).<br>&nbsp;&nbsp;&bull; SqrtAlgo::INTRINSIC: default algorithm, with a maximum precision error of 1 ulp. For the half and float types, Subnormal handling is controlled by the compilation option --cce-ftz (default value: true).<br>&nbsp;&nbsp;&bull; SqrtAlgo::PRECISION_1ULP_FTZ_TRUE: uses a single instruction for computation, with a maximum precision error of 1 ulp.<br>&nbsp;&nbsp;&bull; SqrtAlgo::FAST_INVERSE and SqrtAlgo::PRECISION_0ULP_FTZ_FALSE: obtain the result using the fast inverse algorithm. Currently, this algorithm supports only the float type and supports Subnormal data computation in this mode.<br>&nbsp;&nbsp;&bull; SqrtAlgo::PRECISION_1ULP_FTZ_FALSE: maximum precision error is 1 ulp, and supports Subnormal data computation for the half type. |
| U | RegTensor type of the source operand and destination operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstReg | Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| mask | Input | Valid indicator for the operation on the source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Data Type

The data types of the destination operand and the source operand must be consistent. Supported data types: half and float.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- If the input is a negative number, the result is nan.
- If the input is -0, the result is -0.

## Key Features

### Maximum Precision Error

- When precisionMode is set to true, the fast inverse algorithm is used for high-precision computation, and the maximum precision error is 0 ulp.
- When precisionMode is set to false:
  - The maximum precision error is 1 ulp: SqrtAlgo::INTRINSIC, SqrtAlgo::PRECISION_1ULP_FTZ_TRUE, and SqrtAlgo::PRECISION_1ULP_FTZ_FALSE.
  - The maximum precision error is 0 ulp: SqrtAlgo::FAST_INVERSE and SqrtAlgo::PRECISION_0ULP_FTZ_FALSE.

### Configuring Subnormal Mode

FTZ (Flush-To-Zero): A floating-point operation mode in which, when the result is Subnormal, it is directly flushed to zero (approximated as 0) instead of preserving its exact tiny value.

#### Default Algorithm

SqrtAlgo::INTRINSIC is the default algorithm and supports the half and float types. When --cce-ftz=false, Subnormal values are preserved; when --cce-ftz=true (the default value), FTZ mode is used.

#### Explicitly Specified Algorithms

- **SqrtAlgo::PRECISION_1ULP_FTZ_TRUE** uses single-instruction computation and always adopts the FTZ mode.
- **SqrtAlgo::FAST_INVERSE** and **SqrtAlgo::PRECISION_0ULP_FTZ_FALSE** use the fast inverse algorithm, support only the float type, and support Subnormal data computation.
- **SqrtAlgo::PRECISION_1ULP_FTZ_FALSE** supports Subnormal data computation for the half type.

#### Usage Recommendations

Because the computation behavior that preserves Subnormal is implemented through software simulation, when --cce-ftz=true (the default value), it is recommended to use the default SqrtAlgo::INTRINSIC or explicitly select SqrtAlgo::PRECISION_1ULP_FTZ_TRUE in general scenarios to achieve better performance. When precise Subnormal output is required, use SqrtAlgo::FAST_INVERSE, SqrtAlgo::PRECISION_0ULP_FTZ_FALSE, or SqrtAlgo::PRECISION_1ULP_FTZ_FALSE.

**Table 3**  Sqrt Subnormal Examples

| Output | Input | algo |
| --- | --- | --- |
| 1.0471472598529991e-19 (0x1FF7 402B) | 1.0965174496326337e-38 (0x0077 6672) | SqrtAlgo::FAST_INVERSE, SqrtAlgo::PRECISION_0ULP_FTZ_FALSE |
| 1.0471473244764844e-19 (0x1FF7 402C) | 1.0965174496326337e-38 (0x0077 6672) | SqrtAlgo::PRECISION_1ULP_FTZ_FALSE |

## Example<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void SqrtVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    // High-precision mode.
    // static constexpr AscendC::Reg::SqrtSpecificMode mode = {AscendC::Reg::MaskMergeMode::ZEROING, true};
    // 0ulp precision and Subnormal mode.
    // static constexpr AscendC::Reg::SqrtSpecificMode mode = {
    //     AscendC::Reg::MaskMergeMode::ZEROING, true, AscendC::SqrtAlgo::PRECISION_0ULP_FTZ_FALSE};
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Sqrt(dstReg, srcReg, mask);
        // High-precision mode/Subnormal mode.
        // AscendC::Reg::Sqrt<T, &mode>(dstReg, srcReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
