# Duplicate<a name="ZH-CN_TOPIC_0000001937477638"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:13:49.777Z pushedAt=2026-10-26T07:41:44.515Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/data_padding/Duplicate_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/reg_compute/kernel_reg_compute_vec_duplicate_intf.h"`.

Both Scalar and Tensor modes are supported:

-   Scalar mode: Broadcasts **scalarValue** to the register and stores it in **dstReg** (if a **mask** is present, stores it in the positions of **dstReg** selected by the **mask**).
-   Tensor mode: Broadcasts the lowest-bit element of **srcReg** to the register and stores it in the positions of **dstReg** selected by the **mask**.

## Prototype<a name="section620mcpsimp"></a>

- Broadcast **scalarValue** to the register, without mask support.

    ```cpp
    template <typename T = DefaultType, typename U, typename S>
    __simd_callee__ inline void Duplicate(S& dstReg, U scalarValue);
    ```

- Broadcast **scalarValue** to the register, with mask support.

    ```cpp
    template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U, typename S>
    __simd_callee__ inline void Duplicate(S& dstReg, U scalarValue, MaskReg& mask);
    ```

- Broadcast the lowest-bit element of **srcReg** to the register.

    ```cpp
    template <typename T = DefaultType, HighLowPart pos = HighLowPart::LOWEST, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
    __simd_callee__ inline void Duplicate(S& dstReg, S& srcReg, MaskReg& mask)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [Data Type](#data-type). |
| U | Data type of scalar. For supported data types, see [Data Type](#data-type). |
| mode | [MaskMergeMode](../aux_data_types/MaskMergeMode.md) enumeration type, which selects the MERGING mode or the ZEROING mode.<br>&bull; In ZEROING mode, elements not selected by the mask are set to zero in **dstReg**.<br>&bull; In MERGING mode, elements not selected by the mask retain the original value of **dstReg**. |
| pos | Defaults to **LOWEST**, indicating that the lowest bit in **srcReg** is broadcast to **dstReg** (other values are not supported for now). |
| S | **RegTensor** type of the destination operand, for example, **RegTensor**<half>, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2**  Function parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstReg | Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| scalarValue | Input | Source operand.<br>The type is scalar. |
| srcReg | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| mask | Input | Indicator of the validity of elements in the source operand. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Data Type

The data types of the destination operand and the source operand must be consistent. The supported data types are: bool, int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, complex64.


## Return Value

None

## Constraints<a name="section177921451558"></a>

None

## Example<a name="section642mcpsimp"></a>

- Example 1

    ```cpp
    template<typename T>
    __simd_vf__ inline void DuplicateVF(__ubuf__ T* dstAddr, T scalarValue, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T>();    
        for (uint16_t i = 0; i < repeatTimes; i++) {
            AscendC::Reg::Duplicate(dstReg, scalarValue);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```

- Example 2

    ```cpp
    template<typename T>
    __simd_vf__ inline void DuplicateVF(__ubuf__ T* dstAddr, T scalarValue, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;    
        for (uint16_t i = 0; i < repeatTimes; i++) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::Duplicate(dstReg, scalarValue, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```

- Example 3

    ```cpp
    template<typename T>
    __simd_vf__ inline void DuplicateVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
            AscendC::Reg::Duplicate(dstReg, srcReg, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```
