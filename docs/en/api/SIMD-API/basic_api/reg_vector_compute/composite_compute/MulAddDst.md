# MulAddDst<a name="ZH-CN_TOPIC_0000002009338652"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:07:18.499Z pushedAt=2026-10-26T07:41:44.496Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/composite_compute/MulAddDst_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_vec_binary_intf.h"`.

This API multiplies **srcReg0** by **srcReg1** element-wise, adds the result to **dstReg**, and writes the computation result to **dstReg** according to **mask**. The calculation formula is as follows:

$$
dstReg_i = srcReg0_i \times srcReg1_i + dstReg_i
$$

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void MulAddDst(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :----- | :--- |
| T | Operand data type. |
| mode | [MaskMergeMode](../aux_data_types/MaskMergeMode.md) enumeration type. Selects the MERGING mode or the ZEROING mode.<br>• In ZEROING mode, elements not selected by the mask are set to zero in **dstReg**.<br>• The MERGING mode is not supported currently. |
| U | `RegTensor` type of the destination operand and source operands, for example, `RegTensor<half>`. It is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dstReg | Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). Before the computation, it participates in the operation as the addend; after the computation, it stores the result. |
| srcReg0 | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). The data type must be consistent with that of the destination operand. |
| srcReg1 | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). The data type must be consistent with that of the destination operand. |
| mask | Input | Indicates the validity of the source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Return Value<a name="section640mcpsimp"></a>

None

## Data Type

**Table 3**  Data type combinations

| srcReg0    | srcReg1    | dstReg     |
| ---------- | ---------- | ---------- |
| int16_t    | int16_t    | int16_t    |
| uint16_t   | uint16_t   | uint16_t   |
| half       | half       | half       |
| bfloat16_t | bfloat16_t | bfloat16_t |
| int32_t    | int32_t    | int32_t    |
| uint32_t   | uint32_t   | uint32_t   |
| float      | float      | float      |
| int64_t    | int64_t    | int64_t    |
| uint64_t   | uint64_t   | uint64_t   |

## Constraints<a name="section633mcpsimp"></a>

- This API supports register overlap.
- The operands of this API are registers, so address alignment is not involved.
- This API does not modify the values of global registers.

## Key Features

The data types implemented by soft simulation for this API are int64_t and uint64_t. For details, see [Summary of Soft Simulation for Reg Vector Compute APIs](../../../../appendix/reg_vector_compute_interface_soft_simulation_summary.md). For data types that are not implemented by soft simulation, a single instruction performs the fused multiply-add computation at the hardware level, achieving higher precision than executing [Mul](../basic_arithmetic/Mul.md) and [Add](../basic_arithmetic/Add.md) separately.

## Example<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void MulAddDstVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(dstReg, dstAddr + i * oneRepeatSize);
        AscendC::Reg::MulAddDst(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
