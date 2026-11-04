# Abs<a name="ZH-CN_TOPIC_0000001956810269"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T15:36:46.765Z pushedAt=2026-10-26T07:41:44.438Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/basic_arithmetic/Abs_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/reg_compute/kernel_reg_compute_vec_unary_intf.h"`.

- For real number types

    Computes the absolute value of each valid element in **srcReg** and writes the result to the corresponding position in **dstReg**. The calculation formula is as follows:

    $$
    dstReg_i = |srcReg_i|
    $$

- For complex number types

    Computes the modulus of each valid element in **srcReg** and writes the result to the corresponding position in **dstReg**. The calculation formula is as follows:

    $$
    dstReg_i = |srcReg_i| = (\alpha^2 + \beta^2)^{1/2}
    $$

    Where $srcReg_i = \alpha + \beta i$, α is the real part of the complex number, and β is the imaginary part of the complex number.

## Prototype<a name="section620mcpsimp"></a>

- Real number computation

    ```cpp
    template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
    __simd_callee__ inline void Abs(U& dstReg, U& srcReg, MaskReg& mask)
    ```

- Complex number computation

    ```cpp
    template <typename T = DefaultType, typename U = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S, typename V>
    __simd_callee__ inline void Abs(S& dstReg, V& srcReg, MaskReg& mask)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Destination operand data type. For supported data types, see [data type](#data-type). |
| U | Source operand data type. For supported data types, see [data type](#data-type). |
| mode | [MaskMergeMode](../aux_data_types/MaskMergeMode.md) enumeration type, which selects the MERGING mode or the ZEROING mode.<br>&bull; In ZEROING mode, elements not filtered by the mask are set to zero in dstReg.<br>&bull; The MERGING mode is currently not supported. |
| S | RegTensor type of the destination operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |
| V | RegTensor type of the source operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstReg | Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| mask | Input | Valid indication of the operation on the source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Data Type

**Table 3**  Data Type Combinations

| srcReg | dstReg |
| --- | --- |
| int8_t | int8_t |
| int16_t | int16_t |
| half | half |
| int32_t | int32_t |
| float | float |
| complex32 | half |
| int64_t | int64_t |
| complex64 | float |

## Return Value Description<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- When the data types of the destination operand and the source operand are inconsistent, the destination operand and the source operand must not overlap.
- If the computation result of integer data exceeds the representable range of the data type, non-saturating truncation is applied. For example, for the **int8_t** type, if **srcReg** is -128, its absolute value 128 is truncated to -128.
<!-- npu="950" id8 -->
- For the Ascend 950PR&950DT products, only the APIs of the **complex32** and **complex64** types involve Subnormal in internal computation, and the handling is controlled by the compilation option **--cce-ftz** (default value: **true**):
    - When **--cce-ftz=false**, Subnormal values are preserved during computation and participate in subsequent computation according to their actual values.
    - When **--cce-ftz=true**, the FTZ (Flush-To-Zero) mode is enabled. Subnormal values generated or involved in computation are treated as 0, which may cause precision differences in the computation result compared with preserving Subnormal values.
<!-- end id8 -->

## Example<a name="section642mcpsimp"></a>

- Real number computation.

    ```cpp
    template<typename T>
    __simd_vf__ inline void AbsVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
            AscendC::Reg::Abs(dstReg, srcReg, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```

- Complex number computation.

    ```cpp
    template<typename T, typename U>
    __simd_vf__ inline void AbsVF(__ubuf__ U* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::RegTensor<U> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
            AscendC::Reg::Abs(dstReg, srcReg, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```
