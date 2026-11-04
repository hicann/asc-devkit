# SetDeqScale<a name="ZH-CN_TOPIC_0000002573904763"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:57:39.727Z pushedAt=2026-10-28T03:14:39.601Z nl=2 -->

## Applicable Products<a name="zh-cn_topic_0000002563051145_section815614567413"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/type_conversion_aux_config/SetDeqScale_res.md#id1 -->


## Description<a name="zh-cn_topic_0000002563051145_section748346451"></a>

The header file path is `"kernel_operator_vec_vconv_intf.h"`.

This API is used to set the value of the DEQSCALE register. The DEQSCALE register is 64 bits wide and is used for quantization computation on the Vector computation unit. The parameters stored in the register have different meanings in different scenarios:

> [!NOTE]Note
> In this document, scenario abbreviations such as `s322fp16` and `s162b8` follow the form "source data type abbreviation + digit 2 + destination data type abbreviation", where the digit 2 means "convert to". For example, `s322fp16` can be split into `s32`, `2`, and `fp16`, indicating the scenario of converting `int32_t` to `half`; `s162b8` indicates the scenario of converting `int16_t` to an 8-bit data type. For the data type abbreviation rules, see [Built-in Data Types](../../data_structures/builtin_data_types.md#datatype-abbreviation).

**Table 1**  Bit meaning mapping of the DEQSCALE register

| Mode | Bit Range | Variable Name | Meaning |
| :--- | :------- | :----- | :--- |
| s322fp16 scenario of [AddDeqRelu](../composite_compute/AddDeqRelu.md), [Cast](../type_conversion/Cast.md), and [CastDequant](../composite_compute/CastDequant.md) | 0~15 | scale | A half type of data. |
| s162b8 scenario where CastDequant does not enable vector quantization | 0~31 | scale | A float type of data M (the hardware treats it as the (1,8,10) format, that is, 1 sign bit, 8 exponent bits, and 10 mantissa bits during computation). |
| s162b8 scenario where CastDequant does not enable vector quantization | 37~45 | offset | A signed 9-bit integer. |
| s162b8 scenario where CastDequant does not enable vector quantization | 46 | signMode | Indicates whether the quantization result is signed (0 indicates unsigned, and 1 indicates signed). |
| s162b8 scenario where CastDequant enables vector quantization | 0~13 | vdeqAddr | Treated by the hardware as an address pointing to a 128-byte space in UB. The space is divided into 16 64-bit segments, each of which can be used for one quantization computation. |

## Prototype<a name="zh-cn_topic_0000002563051145_section21861260618"></a>

- Used for the s322fp16 scenario of AddDeqRelu, Cast, and CastDequant.<a id="func1"></a>

  ```cpp
  __aicore__ inline void SetDeqScale(half scale)
  ```

- Used for the s162b8 scenario of CastDequant without vector quantization enabled.

  ```cpp
  __aicore__ inline void SetDeqScale(float scale, int16_t offset, bool signMode)
  ```

- Used for the s162b8 scenario of CastDequant with vector quantization enabled.

  ```cpp
  template <typename T>
  __aicore__ inline void SetDeqScale(const LocalTensor<T>& vdeq, const VdeqInfo& vdeqInfo)
  ```

## Parameters<a name="zh-cn_topic_0000002563051145_section138547361666"></a>

**Table 2** Template parameter description

| Parameter | Description |
| :-- | :-- |
| T | Data type of the input quantized tensor. The supported data type is uint64_t. |

**Table 3**  Parameter Description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| scale | Input | scale quantization parameter.<br>In the s322fp16 scenario of AddDeqRelu, Cast, and CastDequant, the data type is half.<br>In the CastDequant (isVecDeq=false) scenario, the data type is float. |
| offset | Input | offset quantization parameter of the int16_t type, with only the first 9 bits valid.<br>Used in the CastDequant (isVecDeq=false) scenario to set offset. |
| signMode | Input | bool type, indicating whether the quantization result is signed.<br>Used in the CastDequant (isVecDeq=false) scenario to set signMode. |
| vdeq | Input | Type LocalTensor, with supported TPosition values VECIN/VECCALC/VECOUT.<br>Used in the CastDequant (isVecDeq=true) scenario to input the quantized tensor, with a size of 128 bytes. |
| vdeqInfo | Input | [VdeqInfo](#table4) type, used in the CastDequant (isVecDeq=true) scenario. The structure contains 16 groups of quantization parameters in the quantized tensor. The API reads the parameters in the structure and fills them into vdeq. |

**Table 4**  Parameter description of the VdeqInfo structure<a id="table4"></a>

| Parameter | Description |
| :--- | :--- |
| vdeqScale | Array of the float type, used to store the scale parameters scale<sub>0</sub>-scale<sub>15</sub> in the quantized tensor. |
| vdeqOffset | Array of the int16_t type, used to store the offset parameters offset<sub>0</sub>-offset<sub>15</sub> in the quantized tensor. |
| vdeqSignMode | Array of the bool type, used to store the signMode parameters signMode<sub>0</sub>-signMode<sub>15</sub> in the quantized tensor. |

## Constraints<a name="zh-cn_topic_0000002563051145_section1323412155712"></a>

- offset quantization parameter, int16\_t type, with only the first 9 bits valid.
- For vdeq address alignment constraints, see [Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples<a name="zh-cn_topic_0000002563051145_section5349145316712"></a>

- SetDeqScale(half scale).

  ```cpp
  // Used in the s322fp16 scenario with Cast.
  // dstLocal is a LocalTensor of the half type, and srcLocal is a LocalTensor of the int32_t type.
  uint32_t srcSize = 256; // Number of elements involved in the computation.
  half scale = 1.0; // Quantization parameter is 1.
  AscendC::SetDeqScale(scale);
  // dst = src
  AscendC::Cast(dstLocal, srcLocal, AscendC::RoundMode::CAST_NONE, srcSize);
  ```

- SetDeqScale\(float scale, int16\_t offset, bool signMode\).

  ```cpp
  // Used in the CastDequant (isVecDeq=false) scenario.
  // dstLocal is a LocalTensor of the int8_t type, and srcLocal is a LocalTensor of the int16_t type.
  uint32_t srcSize = 256; // Number of elements involved in the computation.
  float scale = 1.0; // Quantization parameter is 1.
  int16_t offset = 0; // No offset.
  bool signMode = true; // dstLocal is of the int8_t type and is a signed number.
  AscendC::SetDeqScale(scale, offset, signMode);
  // dst = src
  AscendC::CastDequant<int8_t, int16_t, false, false>(dstLocal, srcLocal, srcSize);
  ```

- SetDeqScale\(const LocalTensor<T\>& vdeq, const VdeqInfo& vdeqInfo\).

  ```cpp
  // Used with the CastDequant (isVecDeq=true) scenario.
  // dstLocal is a LocalTensor of the int8_t type, and srcLocal is a LocalTensor of the int16_t type.
  uint32_t srcSize = 256; // Number of elements involved in the computation.
  float vdeqScale[16] = { 0 };
  int16_t vdeqOffset[16] = { 0 };
  bool vdeqSignMode[16] = { 0 };
  for (int i = 0; i < 16; i++) {
      vdeqScale[i] = 1.0; // Quantization parameter is 1.
      vdeqOffset[i] = 0; // No offset.
      vdeqSignMode[i] = true; // dstLocal is of the int8_t type and is a signed number.
  }
  AscendC::VdeqInfo vdeqInfo(vdeqScale, vdeqOffset, vdeqSignMode);
  AscendC::SetDeqScale<uint64_t>(tmpBuffer, vdeqInfo);
  // dst = src
  AscendC::CastDequant<int8_t, int16_t, true, true>(dstLocal, srcLocal, srcSize);
  ```
