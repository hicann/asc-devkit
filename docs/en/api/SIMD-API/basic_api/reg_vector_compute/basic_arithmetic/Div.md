# Div<a name="ZH-CN_TOPIC_0000001929668264"></a>

<!-- md-trans-meta sourceCommit=af3f6adafe94d121d34283378bfbe7e348767ea3 translatedAt=2026-11-05T02:21:42.112Z pushedAt=2026-11-18T01:25:30.801Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/basic_arithmetic/Div_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/reg_compute/kernel_reg_compute_vec_binary_intf.h"`.

This API performs an element-wise division operation on the source operands srcReg0 and srcReg1 according to the mask and writes the result to the destination operand dstReg. The calculation formula is as follows:

$$
dstReg_i = srcReg0_i / srcReg1_i
$$

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Div(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [Data Type](#data-type). |
| mode | Configurable as a [MaskMergeMode](../aux_data_types/MaskMergeMode.md) enumeration value or a pointer to the DivSpecificMode structure.<br>&bull; Configure MaskMergeMode: Select the MERGING mode or the ZEROING mode.<br>&nbsp;&nbsp;&bull; In ZEROING mode, elements not selected by the mask are set to zero in dstReg.<br>&nbsp;&nbsp;&bull; The MERGING mode is not supported currently.<br>&bull; Configure DivSpecificMode, which is defined as follows:<br><code>enum class DivAlgo {<br>    INTRINSIC = 0,<br>    DIFF_COMPENSATION,<br>    PRECISION_1ULP_FTZ_TRUE,<br>    PRECISION_0ULP_FTZ_TRUE,<br>    PRECISION_0ULP_FTZ_FALSE,<br>    PRECISION_1ULP_FTZ_FALSE<br>};<br>struct DivSpecificMode {<br>    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;<br>    bool precisionMode = false;<br>    DivAlgo algo = DivAlgo::INTRINSIC;<br>};</code><br>&bull; precisionMode: Used to configure high-precision Div computation. When set to true, the difference compensation algorithm is used, the maximum precision error is 0 ulp, and the float and complex64 types are supported.<br>&bull; algo: Used to select the Div algorithm and configure the Subnormal mode. For details, see [Key Features](#key-features).<br>&nbsp;&nbsp;&bull; DivAlgo::INTRINSIC: Default algorithm, with a maximum precision error of 1 ulp. For the half and float types, Subnormal handling is controlled by the --cce-ftz compilation option (default value: true).<br>&nbsp;&nbsp;&bull; DivAlgo::PRECISION_1ULP_FTZ_TRUE: Uses a single instruction for computation, with a maximum precision error of 1 ulp.<br>&nbsp;&nbsp;&bull; DivAlgo::DIFF_COMPENSATION: Uses the difference compensation algorithm, with a maximum precision error of 0 ulp. For the float and complex64 types, Subnormal handling is controlled by the --cce-ftz compilation option (default value: true).<br>&nbsp;&nbsp;&bull; DivAlgo::PRECISION_0ULP_FTZ_TRUE: Uses the difference compensation algorithm, with a maximum precision error of 0 ulp.<br>&nbsp;&nbsp;&bull; DivAlgo::PRECISION_0ULP_FTZ_FALSE: Uses the difference compensation algorithm, supports Subnormal data computation, with a maximum precision error of 0 ulp.<br>&nbsp;&nbsp;&bull; DivAlgo::PRECISION_1ULP_FTZ_FALSE: Supports Subnormal data computation, with a maximum precision error of 1 ulp. |
| U | RegTensor type of the source operands and the destination operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstReg | Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg0 | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg1 | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| mask | Input | Valid indicator for the operation on the source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Data Type

The data types of the destination operand and the source operand must be consistent.

- When mode is DivAlgo::INTRINSIC or DivAlgo::PRECISION_1ULP_FTZ_TRUE, the supported data types are int16_t, uint16_t, half, int32_t, uint32_t, float, complex32, int64_t, uint64_t, and complex64.
- When mode is DivAlgo::DIFF_COMPENSATION or DivAlgo::PRECISION_0ULP_FTZ_TRUE, the supported data types are float and complex64.
- When mode is DivAlgo::PRECISION_0ULP_FTZ_FALSE, the supported data type is float.
- When mode is DivAlgo::PRECISION_1ULP_FTZ_FALSE, the supported data types are half and float.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Key Features

### Maximum Precision Error

- When precisionMode is set to true, the difference compensation algorithm is used for high-precision computation, with a maximum precision error of 0 ulp.
- When precisionMode is set to false, the maximum precision error is 1 ulp for DivAlgo::INTRINSIC, DivAlgo::PRECISION_1ULP_FTZ_TRUE, and DivAlgo::PRECISION_1ULP_FTZ_FALSE, and 0 ulp for DivAlgo::DIFF_COMPENSATION, DivAlgo::PRECISION_0ULP_FTZ_TRUE, and DivAlgo::PRECISION_0ULP_FTZ_FALSE.

### Configuring Subnormal Mode

FTZ (Flush-To-Zero): A floating-point computation mode in which a Subnormal result is directly flushed to zero (approximated as 0) instead of retaining its exact tiny value.

#### Default Algorithm

DivAlgo::INTRINSIC supports integer, floating-point, and complex types. For the supported range, see [Data Type](#data-type). For half and float types, when --cce-ftz=false, Subnormal values are retained, which is equivalent to DivAlgo::PRECISION_1ULP_FTZ_FALSE; when --cce-ftz=true (the default), the FTZ mode is used, which is equivalent to DivAlgo::PRECISION_1ULP_FTZ_TRUE. For integer types, the computation result does not involve Subnormal values, so --cce-ftz has no effect on them. For complex32 and complex64 types, the real and imaginary parts are half and float respectively, and Subnormal handling follows the --cce-ftz configuration of the corresponding element type.

#### Explicitly Specified Algorithms

- DivAlgo::DIFF_COMPENSATION supports float and complex64 types. When --cce-ftz=false, Subnormal values are retained; when --cce-ftz=true (the default), the FTZ mode is used.
- Algorithms whose names contain **FTZ_FALSE** always preserve Subnormal.
- Algorithms whose names contain **FTZ_TRUE** always use FTZ mode.

#### Usage Recommendations

For general scenarios, use DivAlgo::PRECISION_1ULP_FTZ_TRUE or DivAlgo::PRECISION_0ULP_FTZ_TRUE, which offer better performance. For scenarios that require accurate Subnormal output (for example, when the algorithm has special requirements on data precision or needs to avoid division-by-zero errors), set the algorithm to DivAlgo::PRECISION_0ULP_FTZ_FALSE or DivAlgo::PRECISION_1ULP_FTZ_FALSE.

**Table 3**  Div Subnormal examples

| Dividend input | Divisor input | Output (with Subnormal mode configured) | Output (without Subnormal mode configured) |
| --- | --- | --- | --- |
| 1 (0x3F80 0000) | -5.01923249665106e-39 (0x8036 A794) | -1.9923364304124947e+38 (0xFF15 E301) | -inf (0xFF80 0000) |
| 1.8611191411098982e-39 (0x0014 440B) | 7.70406850625423e-39 (0x0053 E3CF) | 0.24157613515853882 (0x3E77 5FBC) | NaN (0x7FFF FFFF) |

## Example<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void DivVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    // High-precision mode.
    // static constexpr AscendC::Reg::DivSpecificMode mode = {AscendC::Reg::MaskMergeMode::ZEROING, true};
    // 0 ulp precision and Subnormal mode.
    // static constexpr AscendC::Reg::DivSpecificMode mode = {
    //     AscendC::Reg::MaskMergeMode::ZEROING, true, AscendC::DivAlgo::PRECISION_0ULP_FTZ_FALSE};
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Div(dstReg, srcReg0, srcReg1, mask);
        // High-precision mode/Subnormal mode.
        // AscendC::Reg::Div<T, &mode>(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
