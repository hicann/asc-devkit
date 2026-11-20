# Or<a name="ZH-CN_TOPIC_0000001929668268"></a>

<!-- md-trans-meta sourceCommit=af3f6adafe94d121d34283378bfbe7e348767ea3 translatedAt=2026-11-05T03:03:27.592Z pushedAt=2026-11-18T01:25:30.887Z nl=2 -->

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
<!-- npu="9050" id8 -->
- Kirin 9050 Processor Family Product Line: Supported
<!-- end id8 -->
<!-- npu="kirin1" id9 -->
- KirinDev0001 Processor Family Product Line: Supported
<!-- end id9 -->
<!-- npu="kirin2" id10 -->
- KirinDev0002 Processor Family Product Line: Supported
<!-- end id10 -->
<!-- npu="kirin3" id11 -->
- KirinDev0003 Processor Family Product Line: Supported
<!-- end id11 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/logical_compute/Or_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file paths are: `"basic_api/reg_compute/kernel_reg_compute_vec_binary_intf.h"` (for `RegTensor` operations) and `"basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h"` (for `MaskReg` operations).

This section describes two APIs, which are used to operate on [RegTensor](../register_data_types/RegTensor.md) and [MaskReg](../register_data_types/MaskReg.md) respectively:

- For `RegTensor` operations:

  Performs a bitwise OR (|) operation on the input data srcReg0 and srcReg1 based on mask, and writes the result to dstReg.

- For `MaskReg` operations:

  Performs a bitwise OR (|) operation on the two input `MaskReg` data src0 and src1 based on mask, and writes the result to dst.

## Prototype<a name="section620mcpsimp"></a>

- For `RegTensor` operations

  ```cpp
  template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
  __simd_callee__ inline void Or(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
  ```

- For `MaskReg` operations

  ```cpp
  __simd_callee__ inline void Or(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask)
  ```

## Parameters<a name="section622mcpsimp"></a>

- For `RegTensor` operations

  **Table 1** Template parameter description

  | Parameter | Description |
  | :----- | :--- |
  | T | Operand data type. |
  | mode | [MaskMergeMode](../aux_data_types/MaskMergeMode.md) enumeration type. Selects the MERGING mode or the ZEROING mode.<br>• In ZEROING mode, elements not filtered by the mask are set to zero in dstReg.<br>• The MERGING mode is currently not supported. |
  | U | `RegTensor` type of the destination operand and source operands, for example, `RegTensor<uint32_t>`. It is automatically deduced by the compiler and does not need to be specified by the user. |

  **Table 2**  Parameters

  | Parameter | Input/Output | Description |
  | :----- | :-------- | :--- |
  | dstReg | Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
  | srcReg0 | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md).<br>The data type must be consistent with the destination operand. |
  | srcReg1 | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md).<br>The data type must be consistent with the destination operand. |
  | mask | Input | Valid indicator for the operation on the source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

- For `MaskReg` operations

  **Table 3**  Parameter Description

  | Parameter | Description |
  | :----- | :--- |
  | dst    | Destination operand. |
  | src0   | Source operand. |
  | src1   | Source operand. |
  | mask   | Indicates which bits are valid during computation. |

## Return Value<a name="section640mcpsimp"></a>

None

## Data Type

Supported data types: bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t.

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

- For `RegTensor` operations

  ```cpp
  template <typename T>
  __simd_vf__ inline void OrVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
  {
      AscendC::Reg::RegTensor<T> srcReg0;
      AscendC::Reg::RegTensor<T> srcReg1;
      AscendC::Reg::RegTensor<T> dstReg;
      AscendC::Reg::MaskReg mask;
      for (uint16_t i = 0; i < repeatTimes; i++) {
          AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
          AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
          mask = AscendC::Reg::UpdateMask<T>(count);
          AscendC::Reg::Or(dstReg, srcReg0, srcReg1, mask);
          AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
      }
  }
  ```

- For `MaskReg` operations

  ```cpp
  template <typename T>
  __simd_vf__ inline void OrVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
  {
      AscendC::Reg::RegTensor<T> srcReg;
      AscendC::Reg::MaskReg src0 = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALLF>();
      AscendC::Reg::MaskReg src1 = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
      AscendC::Reg::MaskReg dst;
      AscendC::Reg::MaskReg mask;
      for (uint16_t i = 0; i < repeatTimes; ++i) {
          mask = AscendC::Reg::UpdateMask<T>(count);
          AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
          AscendC::Reg::Or(dst, src0, src1, mask);
          AscendC::Reg::Adds(srcReg, srcReg, 0, dst);
          AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, srcReg, mask);
      }
  }
  ```
