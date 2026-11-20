# SetFixPipeReluAlpha<a name="ZH-CN_TOPIC_0000002738231208"></a>

<!-- md-trans-meta sourceCommit=971bc6393be4cb3e0f8244913e605c5710bb2cc9 translatedAt=2026-11-19T06:05:58.440Z pushedAt=2026-11-19T06:14:22.740Z nl=2 -->

## Supported Products<a name="section18204144912492"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- npu="x90" id8 -->
- KirinX90 processor products: not supported
<!-- end id8 -->
<!-- npu="9020" id9 -->
- Kirin9020 processor products: not supported
<!-- end id9 -->
<!-- npu="9030" id10 -->
- Kirin9030 processor products: not supported
<!-- end id10 -->
<!-- npu="9050" id11 -->
- Kirin9050 processor products: supported
<!-- end id11 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_store_aux_config/SetFixPipeReluAlpha_res.md#id1 -->

## Function Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_macro_intf.h"`.

During MacroCube computation (such as [MmadFix](../mmad_compute/MmadFix.md) and ConvFix), when the activation (ReLU) post-processing function is enabled, this API is called to set the LUT (Look-Up Table) mode and preValue parameter of the activation function.

lut\_mode\_t The enumeration type is defined as follows:

```cpp
enum class lut_mode_t : uint8_t { Gelu, Silu, Sigmoid, Tanh };
```

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetFixPipeReluAlpha(const FixPipeReluAlphaBitModeParams& params)

__aicore__ inline void SetFixPipeReluAlpha(const FixPipeReluAlphaParams& params)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameter description

| Parameter | Input/Output | Meaning |
| ---------- | ---------- | ---------- |
| params | Input | Activation-related parameters, of type FixPipeReluAlphaParams or FixPipeReluAlphaBitModeParams.<br>For the specific definition of FixPipeReluAlphaParams, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_macro.h`. For the specific definition of FixPipeReluAlphaBitModeParams, see `${INSTALL_DIR}/asc/include/basic_api/kernel_operator_macro_bitmode_intf.h`. Replace `${INSTALL_DIR}` with the file storage path after the CANN software is installed.<br>For the parameter description of FixPipeReluAlphaParams, see [Table 2](#table2fixpipereulalpha). For the parameter description of FixPipeReluAlphaBitModeParams, see [Table 3](#table3fixpipereulalpha). |

<a name="table2fixpipereulalpha"></a>
**Table 2** Parameter description of the FixPipeReluAlphaParams structure

| Parameter | Meaning |
| ---------- | ---------- |
| lut | LUT mode of the activation function, lut\_mode\_t enumeration type, with a default value of Gelu. The values include Gelu, Silu, Sigmoid, and Tanh. |
| preValue | Scalar parameter used in activation function computation, uint32\_t type, with a default value of 0. When passed in, the float type parameter must be converted to the uint32\_t type. |

<a name="table3fixpipereulalpha"></a>
**Table 3** Parameter description of the FixPipeReluAlphaBitModeParams class

| Parameter | Meaning |
| ---------- | ---------- |
| config0 | uint64\_t type, forms a union with the FixPipeReluAlphaBitModeConfig0 bit field structure type parameter config0BitMode, initialized to 0, and its value can be obtained by using the GetConfig0() function of the class object. |
| config0BitMode | FixPipeReluAlphaBitModeConfig0 bit field structure type. For the parameters, see [Table 4](#table4fixpipereulalpha). It forms a union with config0. |

FixPipeReluAlphaBitModeParams optimizes the construction of the FixPipeReluAlphaParams structure. This API is suitable for the scenario where the scalar pipeline becomes a performance optimization bottleneck. The FixPipeReluAlphaBitModeParams class uses the union and bit field methods, expresses parameter types with bits, uses the bit field structure to automatically process the bit count of input parameters, and leverages the union feature to implement multi-parameter fused passing. Only one input parameter needs to be passed to contain all required information, and the corresponding underlying API only needs to receive one parameter. In addition, when the value of a certain bit in a parameter needs to be modified, it can be achieved only through loops and bit operations without passing the parameter again, which reduces scalar computation and improves performance.

The FixPipeReluAlphaBitModeParams class can be initialized directly with an object of the FixPipeReluAlphaParams structure type:

```cpp
__aicore__ inline FixPipeReluAlphaBitModeParams(const FixPipeReluAlphaParams& fixPipeReluAlphaParams);
```

The Set functions of each parameter can also be used to modify parameter values. In addition, because a union is used, config0 can also be directly modified bit by bit to modify parameters.

<a name="table4fixpipereulalpha"></a>
**Table 4** Parameter description of the FixPipeReluAlphaBitModeConfig0 structure

| Parameter | Meaning |
| ---------- | ---------- |
| lut | LUT mode of the activation function, lut\_mode\_t enumeration type.<br>This parameter is the lowest-bit parameter of the bit field structure, occupying 3 bits. Its value can be set by using the SetLut() function of the FixPipeReluAlphaBitModeParams class object and obtained by using the GetLut() function. |
| reserved0 | Reserved parameter, occupying 10 bits. |
| preValue | Scalar parameter used in activation function computation.<br>This parameter is the highest-bit parameter of the bit field structure, occupying 19 bits. Its value can be set by using the SetPreValue() function of the FixPipeReluAlphaBitModeParams class object and obtained by using the GetPreValue() function. |

## Data Type

The supported data types are uint32\_t and uint64\_t.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- This API must be used together with MacroCube series computation APIs (such as [MmadFix](../mmad_compute/MmadFix.md) and ConvFix), and must be called before the MacroCube computation API.
- The registers involved in this section maintain their state within the same kernel function (Kernel) until they are manually changed, and do not inherit the previous state across different kernel functions (Kernel).

## Example<a name="section642mcpsimp"></a>

```cpp
// Set the LUT mode and preValue parameter of the activation function.
AscendC::lut_mode_t lutMode = AscendC::lut_mode_t::Gelu;
float tmp = (float)0.5;
// Convert the float type tmp to the uint32_t preValue.
uint32_t preValue = static_cast<uint32_t>(*reinterpret_cast<int32_t*>(&tmp));

// Method 1: Use the FixPipeReluAlphaParams structure.
AscendC::FixPipeReluAlphaParams reluAlphaParams(lutMode, preValue);
AscendC::SetFixPipeReluAlpha(reluAlphaParams);

// Method 2: Use the FixPipeReluAlphaBitModeParams class (applicable to the scenario where the scalar pipeline becomes a performance optimization bottleneck).
AscendC::FixPipeReluAlphaBitModeParams reluAlphaBitModeParams(reluAlphaParams);
AscendC::SetFixPipeReluAlpha(reluAlphaBitModeParams);
```
