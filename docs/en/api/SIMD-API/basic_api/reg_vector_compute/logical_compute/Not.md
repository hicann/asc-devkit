# Not<a name="ZH-CN_TOPIC_0000001929668260"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:20:02.365Z pushedAt=2026-10-26T07:41:44.530Z nl=2 -->

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
- Atlas 200I/500 A2 Inference Products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas Inference Products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas Inference Products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas Training Products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/logical_compute/Not_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file paths are: `"basic_api/reg_compute/kernel_reg_compute_vec_unary_intf.h"` (for operations on `RegTensor`) and `"basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h"` (for operations on `MaskReg`).

This section describes two APIs, which are used to operate on [RegTensor](../register_data_types/RegTensor.md) and [MaskReg](../register_data_types/MaskReg.md), respectively:

- For operations on `RegTensor`:

  Performs a bitwise NOT operation on the input data **srcReg** based on the mask, and writes the result to **dstReg**.

- For operations on `MaskReg`:

    Performs a bitwise NOT operation on the input data **src** based on the mask, and writes the result to **dst**.

## Prototype<a name="section620mcpsimp"></a>

- Operates on `RegTensor`.

  ```cpp
  template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
  __simd_callee__ inline void Not(U& dstReg, U& srcReg, MaskReg& mask)
  ```

- Operates on `MaskReg`.

  ```cpp
  __simd_callee__ inline void Not(MaskReg& dst, MaskReg& src, MaskReg& mask)
  ```

## Parameters<a name="section622mcpsimp"></a>

- Operation on `RegTensor`

  **Table 1**  Template parameter description

  | Parameter | Description |
  | :----- | :--- |
  | T | Data type of the operand. |
  | mode | Enumeration of [MaskMergeMode](../aux_data_types/MaskMergeMode.md). Selects the MERGING mode or the ZEROING mode.<br>• In ZEROING mode, elements not selected by mask are set to zero in dstReg.<br>• The MERGING mode is not supported currently. |
  | U | `RegTensor` type of the destination operand and source operand, for example, `RegTensor<half>`. It is automatically deduced by the compiler and does not need to be specified by the user. |

  **Table 2**  Parameter Description

  | Parameter | Input/Output | Description |
  | :----- | :-------- | :--- |
  | dstReg | Output | Destination Operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
  | srcReg | Input | Source Operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md).<br>The data type of the source operand must be consistent with that of the destination operand. |
  | mask | Input | Indicates the validity of the source operand element operation. For details, see [MaskReg](../register_data_types/MaskReg.md). |

- Operation on `MaskReg`

  **Table 3**  Parameter Description

  | Parameter | Description |
  | :----- | :--- |
  | dst | Destination Operand. |
  | src | Source Operand. |
  | mask | Indicates which bits are valid during computation. |

## Return Value<a name="section640mcpsimp"></a>

None

## Data Types

Supported data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t.

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

- Operate on `RegTensor`.

  ```cpp
  template <typename T>
  __simd_vf__ inline void NotVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, 
   uint32_t oneRepeatSize, uint16_t repeatTimes)
  {
      AscendC::Reg::RegTensor<T> srcReg;
      AscendC::Reg::RegTensor<T> dstReg;
      AscendC::Reg::MaskReg mask;
      for (uint16_t i = 0; i < repeatTimes; i++) {
          mask = AscendC::Reg::UpdateMask<T>(count);
          AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
          AscendC::Reg::Not(dstReg, srcReg, mask);
          AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
      }
  }
  ```

- Operate on `MaskReg`.

  ```cpp
  template <typename T>
  __simd_vf__ inline void NotVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
  {
      AscendC::Reg::RegTensor<T> srcReg;
      AscendC::Reg::MaskReg src = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALLF>();
      AscendC::Reg::MaskReg dst;
      AscendC::Reg::MaskReg mask;
      for (uint16_t i = 0; i < repeatTimes; ++i) {
          mask = AscendC::Reg::UpdateMask<T>(count);
          AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
          AscendC::Reg::Not(dst, src, mask);
          AscendC::Reg::Adds(srcReg, srcReg, 0, dst);
          AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, srcReg, mask);
      }
  }
  ```
