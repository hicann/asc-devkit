# Log10<a name="ZH-CN_TOPIC_0000002044509269"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T15:47:45.656Z pushedAt=2026-10-26T07:41:44.455Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/basic_arithmetic/Log10_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/reg_compute/kernel_reg_compute_vec_unary_intf.h"`.

This API computes the base-10 logarithm of the source operand element by element according to the mask, and writes the result to the destination operand.

The calculation formula is as follows:

$$
dstReg_i = \log_{10}(srcReg_i)
$$

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Log10(U& dstReg, U& srcReg, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [Data Type](#data-type). |
| mode | Configurable as a [MaskMergeMode](../aux_data_types/MaskMergeMode.md) enumeration value or a pointer to the **Log10SpecificMode** structure.<br>&bull; Configure **MaskMergeMode**: Select MERGING mode or ZEROING mode.<br>&nbsp;&nbsp;&bull; In ZEROING mode, elements not filtered by the mask are set to zero in **dstReg**.<br>&nbsp;&nbsp;&bull; MERGING mode is currently not supported.<br>&bull; Configure **Log10SpecificMode**, which is defined as follows:<br><code>enum class Log10Algo {<br>    INTRINSIC = 0,<br>    PRECISION_1ULP_FTZ_TRUE,<br>    PRECISION_1ULP_FTZ_FALSE,<br>};<br>struct Log10SpecificMode {<br>    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;<br>    Log10Algo algo = Log10Algo::INTRINSIC;<br>};</code><br>&bull; **mrgMode**: Select MERGING mode or ZEROING mode.<br>&bull; **algo**: Used to select the Log10 algorithm and configure the Subnormal mode. For details, see [Key Feature Description](#key-feature-description).<br>&nbsp;&nbsp;&bull; **Log10Algo::INTRINSIC**: Default algorithm, the maximum precision error is 1 ulp. For half and float types, Subnormal handling is controlled by the compilation option **--cce-ftz** (default value: **true**).<br>&nbsp;&nbsp;&bull; **Log10Algo::PRECISION_1ULP_FTZ_TRUE**: Uses a single instruction for computation, the maximum precision error is 1 ulp.<br>&nbsp;&nbsp;&bull; **Log10Algo::PRECISION_1ULP_FTZ_FALSE**: Implemented through software simulation, supports Subnormal data computation, the maximum precision error is 1 ulp. |
| U | RegTensor type of the destination operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstReg | Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md).|
| mask | Input | Valid indicator for the operation on source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Data Type

The data types of the destination operand and the source operand must be consistent. Supported data types: **half**, **float**.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Key Feature Description

### Precision Improvement

When the source operand data type is **half**, the **half** value is promoted to **float** for computation, and the result is then converted back to **half** for output.

### Maximum Precision Error

The maximum precision error of Log10Algo::INTRINSIC, Log10Algo::PRECISION_1ULP_FTZ_TRUE, and Log10Algo::PRECISION_1ULP_FTZ_FALSE is 1ulp.

### Configuring Subnormal Mode

FTZ (Flush-To-Zero): A floating-point operation mode in which, when the result is subnormal, it is directly flushed to zero (approximated as 0) instead of retaining its exact tiny value.

#### Default Algorithm

**Log10Algo::INTRINSIC** is the default algorithm. When **--cce-ftz=false**, Subnormal values are preserved; when **--cce-ftz=true** (the default value), FTZ mode is used.

#### Explicitly Specified Algorithm

- **Log10Algo::PRECISION_1ULP_FTZ_TRUE** uses single-instruction computation and always adopts FTZ mode.
- **Log10Algo::PRECISION_1ULP_FTZ_FALSE** is implemented through software simulation and supports Subnormal data computation.

#### Usage Recommendations

Because the computation behavior that preserves Subnormal is implemented through software simulation, when --cce-ftz=true (the default value), it is recommended to use the default **Log10Algo::INTRINSIC** or explicitly select **Log10Algo::PRECISION_1ULP_FTZ_TRUE** in general scenarios to achieve better performance. When precise Subnormal output is required, use **Log10Algo::PRECISION_1ULP_FTZ_FALSE**.

## Example<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void Log10VF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    // 1 ulp precision and Subnormal mode.
    // static constexpr AscendC::Reg::Log10SpecificMode mode = {
    //     AscendC::Reg::MaskMergeMode::ZEROING, AscendC::Log10Algo::PRECISION_1ULP_FTZ_FALSE};
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Log10(dstReg, srcReg, mask);
        // 1 ulp precision and Subnormal mode.
        // AscendC::Reg::Log10<DTYPE, &mode>(dstReg, srcReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
