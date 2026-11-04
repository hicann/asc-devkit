# And<a name="ZH-CN_TOPIC_0000001956986921"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:20:03.907Z pushedAt=2026-10-26T07:41:44.531Z nl=2 -->

## Product Support<a name="section1550532418810"></a>

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
- Atlas 200I/500 A2 Inference Product: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/logical_compute/And_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file paths are: `"basic_api/reg_compute/kernel_reg_compute_vec_binary_intf.h"` (for `RegTensor` operations) and `"basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h"` (for `MaskReg` operations).

This section describes two APIs for operating on [RegTensor](../register_data_types/RegTensor.md) and [MaskReg](../register_data_types/MaskReg.md), respectively:

- For `RegTensor` operations:

  Performs a bitwise AND (&) operation on the input data `srcReg0` and `srcReg1` according to the mask, and writes the result to `dstReg`.

- For `MaskReg` operations:

  Performs a bitwise AND (&) operation on the two input `MaskReg` data `src0` and `src1` according to the mask, and writes the result to `dst`.

## Prototype<a name="section620mcpsimp"></a>

- For `RegTensor`

  ```cpp
  template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
  __simd_callee__ inline void And(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
  ```

- For `MaskReg`

  ```cpp
  __simd_callee__ inline void And(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask)
  ```

## Parameter Description<a name="section622mcpsimp"></a>

- For `RegTensor` operations

  **Table 1**  Template parameter description

  | Parameter Name | Description |
  | :----- | :--- |
  | T | Operand data type. |
  | mode | [MaskMergeMode](../aux_data_types/MaskMergeMode.md) enumeration type. Selects the **MERGING** mode or the **ZEROING** mode.<br>• In **ZEROING** mode, elements not filtered by **mask** are set to zero in **dstReg**.<br>• The **MERGING** mode is currently not supported. |
  | U | The `RegTensor` type of the destination operand and source operands, for example, `RegTensor<uint32_t>`. It is automatically deduced by the compiler and does not need to be specified by the user. |

  **Table 2**  Parameter description

  | Parameter Name | Input/Output | Description |
  | :----- | :-------- | :--- |
  | dstReg | Output | Destination Operand.<br>Type: [RegTensor](../register_data_types/RegTensor.md). |
  | srcReg0 | Input | Source Operand.<br>Type: [RegTensor](../register_data_types/RegTensor.md).<br>The data types of the two source operands must be consistent with that of the destination operand. |
  | srcReg1 | Input | Source Operand.<br>Type: [RegTensor](../register_data_types/RegTensor.md).<br>The data types of the two source operands must be consistent with that of the destination operand. |
  | mask | Input | Indicates the validity of the operation on the source operand elements. For details, see [MaskReg document](../register_data_types/MaskReg.md). |

- For `MaskReg` operations

  **Table 3**  Parameter description

  | Parameter Name | Description |
  | :----- | :--- |
  | dst | Destination Operand. |
  | src0 | Source Operand. |
  | src1 | Source Operand. |
  | mask | Indicates which bits are valid during computation. |

## Return Value<a name="section640mcpsimp"></a>

None

## Data Types

Supported data types: bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t.

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

- Operations on `RegTensor`

  ```cpp
  template <typename T>
  __simd_vf__ inline void AndVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
  {
      AscendC::Reg::RegTensor<T> srcReg0;
      AscendC::Reg::RegTensor<T> srcReg1;
      AscendC::Reg::RegTensor<T> dstReg;
      AscendC::Reg::MaskReg mask;
      for (uint16_t i = 0; i < repeatTimes; i++) {
          mask = AscendC::Reg::UpdateMask<T>(count);
          AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
          AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
          AscendC::Reg::And(dstReg, srcReg0, srcReg1, mask);
          AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
      }
  }
  ```

- Operations on `MaskReg`

  ```cpp
  template <typename T>
  __simd_vf__ inline void AndVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
  {
      AscendC::Reg::RegTensor<T> srcReg;
      AscendC::Reg::MaskReg src0 = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALLF>();
      AscendC::Reg::MaskReg src1 = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
      AscendC::Reg::MaskReg dst;
      AscendC::Reg::MaskReg mask;
      for (uint16_t i = 0; i < repeatTimes; ++i) {
          mask = AscendC::Reg::UpdateMask<T>(count);
          AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
          AscendC::Reg::And(dst, src0, src1, mask);
          AscendC::Reg::Adds(srcReg, srcReg, 0, dst);
          AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, srcReg, mask);
      }
  }
  ```
