# Add<a name="ZH-CN_TOPIC_0000001956827093"></a>

<!-- md-trans-meta sourceCommit=af3f6adafe94d121d34283378bfbe7e348767ea3 translatedAt=2026-11-05T02:18:58.728Z pushedAt=2026-11-18T01:25:30.795Z nl=2 -->

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
- Kirin 9050 Processor products: Supported
<!-- end id8 -->
<!-- npu="kirin1" id9 -->
- KirinDev0001 Processor products: Supported
<!-- end id9 -->
<!-- npu="kirin2" id10 -->
- KirinDev0002 Processor products: Supported
<!-- end id10 -->
<!-- npu="kirin3" id11 -->
- KirinDev0003 Processor products: Supported
<!-- end id11 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/basic_arithmetic/Add_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/reg_compute/kernel_reg_compute_vec_binary_intf.h"`.

This API performs an element-wise addition operation on the source operands **srcReg0** and **srcReg1** according to the mask and writes the result to the destination operand **dstReg**.

The carry flag indicates an addition carry or a subtraction without borrow. This API can mark in carry (a MaskReg register) whether each addition produces a carry. If a carry is generated at the most significant bit after the bitwise addition of srcReg0 and srcReg1, 1 is written to the lowest bit of each 4-bit group at the corresponding position in the MaskReg carry; otherwise, 0 is written.

The calculation result does not retain the carry. The calculation formula is as follows:

$$
dstReg_i = srcReg0_i + srcReg1_i
$$

The calculation result retains the carry. The calculation formula is as follows:

$$
\{carry_i, dstReg_i\} = srcReg0_i + srcReg1_i
$$

![](../../../../figures/reg_add.png)

For the applicable scenarios of the output carry, see [AddC Function Description](AddC.md#description).

## Prototype<a name="section620mcpsimp"></a>

- The calculation result does not retain the carry.

    ```cpp
    template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
    __simd_callee__ inline void Add(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
    ```

- The calculation result retains the carry.

    ```cpp
    template <typename T = DefaultType, typename U>
    __simd_callee__ inline void Add(MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [Data Type](#data-type). |
| mode | [MaskMergeMode](../aux_data_types/MaskMergeMode.md) enumeration type, which selects the MERGING mode or the ZEROING mode.<br>&bull; In the ZEROING mode, elements not filtered by the mask are set to zero in dstReg.<br>&bull; In the MERGING mode, elements not filtered by the mask retain their original values in dstReg. |
| U | RegTensor type of the destination operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstReg | Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg0 | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg1 | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| mask | Input | Valid indicator for the operation on the source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |
| carry | Output | Destination operand. Outputs the carry value.<br>Type is [MaskReg](../register_data_types/MaskReg.md). |

## Data Type

The data types of the destination operand and the source operand must be consistent.
- When the calculation result is Carry Not Retained, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, complex64.
- When the calculation result is Carry Retained, the supported data types are: int32_t, uint32_t.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

- Carry Not Retained

    ```cpp
    template <typename T>
    __simd_vf__ inline void AddVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg0;
        AscendC::Reg::RegTensor<T> srcReg1;
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
            AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
            AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```

- Carry Retained

    ```cpp
    template <typename T>
    static __simd_vf__ inline void AddVF(__ubuf__ T* dst0Addr, __ubuf__ T* dst1Addr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint16_t repeatTimes, uint16_t oneRepeatSize){
        AscendC::Reg::RegTensor<T> srcReg0;
        AscendC::Reg::RegTensor<T> srcReg1;
        AscendC::Reg::RegTensor<T> dstReg0;
        AscendC::Reg::MaskReg mask;
        AscendC::Reg::MaskReg carry = AscendC::Reg::CreateMask<uint8_t>();
        for (uint16_t i = 0; i < repeatTimes; i++) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
            AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
            AscendC::Reg::Add(carry, dstReg0, srcReg0, srcReg1, mask);
            // 8*4B=32B align
            AscendC::Reg::StoreAlign<uint32_t, AscendC::Reg::MaskDist::DIST_NORM>(dst1Addr + i * 8, carry);
            AscendC::Reg::StoreAlign(dst0Addr + i * oneRepeatSize, dstReg0, mask);
        }
    }
    ```
