# SetMmadStridePara<a name="ZH-CN_TOPIC_00000027121725635"></a>

<!-- md-trans-meta sourceCommit=971bc6393be4cb3e0f8244913e605c5710bb2cc9 translatedAt=2026-11-19T02:39:38.034Z pushedAt=2026-11-19T05:58:49.099Z nl=2 -->

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
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute_aux_config/SetMmadStridePara_res.md#id1 -->

## Function Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_macro_intf.h"`.

Before calling the [MmadFix](../mmad_compute/MmadFix.md) API to perform matrix multiplication calculation, call this API to set the loop4 source stride (loop4SrcStride) and destination stride (loop4DstStride) parameters of the Mmad calculation, which are used to control the stride of the source data and destination data of the loop4 loop in the matrix multiplication calculation.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetMmadStridePara(const MmadStrideParaBitModeParams& params)

__aicore__ inline void SetMmadStridePara(const MmadStrideParaParams& params)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameter description

| Parameter | Input/Output | Meaning |
| ---------- | ---------- | ---------- |
| params | Input | Mmad stride parameter, whose type is MmadStrideParaParams or MmadStrideParaBitModeParams.<br>For the specific definition of MmadStrideParaParams, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_macro.h`. For the specific definition of MmadStrideParaBitModeParams, see `${INSTALL_DIR}/asc/include/basic_api/kernel_operator_macro_bitmode_intf.h`. Replace `${INSTALL_DIR}` with the file storage path after the CANN software is installed.<br>For the parameter description of MmadStrideParaParams, see [Table 2](#table2mmadstridepara). For the parameter description of MmadStrideParaBitModeParams, see [Table 3](#table3mmadstridepara). |

<a name="table2mmadstridepara"></a>
**Table 2** Parameter description of the MmadStrideParaParams structure

| Parameter | Meaning |
| ---------- | ---------- |
| loop4SrcStride | Source stride of the loop4 loop, of the uint16\_t type, with a default value of 0. |
| loop4DstStride | Destination stride of the loop4 loop, of the uint16\_t type, with a default value of 0. |

<a name="table3mmadstridepara"></a>
**Table 3** Parameter description of the MmadStrideParaBitModeParams class

| Parameter | Meaning |
| ---------- | ---------- |
| config0 | Of the uint64\_t type, forms a union with the config0BitMode parameter of the MmadStrideParaBitModeConfig0 bit field structure type, is initialized to 0, and its value can be obtained by using the GetConfig0() function of the class object. |
| config0BitMode | Of the MmadStrideParaBitModeConfig0 bit field structure type. For its parameters, see [Table 4](#table4mmadstridepara). It forms a union with config0. |

MmadStrideParaBitModeParams optimizes the construction of the MmadStrideParaParams structure. This API is applicable to the scenario where the scalar pipeline becomes a performance optimization bottleneck. The MmadStrideParaBitModeParams class uses the union and bit field methods, expresses parameter types with bits, uses the bit field structure to automatically process the bit count of input parameters, and uses the characteristics of the union to implement multi-parameter fused transfer, so that only one input parameter needs to be passed to contain all required information, and the corresponding underlying API only needs to receive one parameter. In addition, when the value of a certain bit in a parameter needs to be modified, it can be implemented only through loops and bit operations without re-passing the parameter, which reduces scalar calculation and improves performance.

The MmadStrideParaBitModeParams class can be initialized directly by using an object of the MmadStrideParaParams structure type:

```cpp
__aicore__ inline MmadStrideParaBitModeParams(const MmadStrideParaParams& mmadStrideParaParams);
```

The Set functions of each parameter can also be used to modify parameter values. In addition, because a union is used, config0 can also be directly modified bit by bit to modify parameters.

<a name="table4mmadstridepara"></a>
**Table 4** Parameter description of the MmadStrideParaBitModeConfig0 structure

| Parameter | Meaning |
| ---------- | ---------- |
| loop4SrcStride | Source stride of the loop4 loop, of the uint16\_t type.<br>This parameter is the lowest-bit parameter of the bit field structure and occupies 16 bits. Its value can be set by using the SetLoop4SrcStride() function of the MmadStrideParaBitModeParams class object and obtained by using the GetLoop4SrcStride() function. |
| loop4DstStride | Destination stride of the loop4 loop, of the uint16\_t type.<br>This parameter is the highest-bit parameter of the bit field structure and occupies 16 bits. Its value can be set by using the SetLoop4DstStride() function of the MmadStrideParaBitModeParams class object and obtained by using the GetLoop4DstStride() function. |

## Data Type

The supported data types are uint16\_t and uint64\_t.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- This API must be used together with the [MmadFix](../mmad_compute/MmadFix.md) API and must be called before the MmadFix API.
- The registers involved in this section maintain their state within the same kernel function (Kernel) until they are manually changed, and do not inherit the previous state across different kernel functions (Kernel).

## Example<a name="section642mcpsimp"></a>

```cpp
uint16_t loop4SrcStride = 16;
uint16_t loop4DstStride = 16;

// Method 1: Use the MmadStrideParaParams structure.
AscendC::MmadStrideParaParams mmadStrideParaParams(loop4SrcStride, loop4DstStride);
AscendC::SetMmadStridePara(mmadStrideParaParams);

// Method 2: Use the MmadStrideParaBitModeParams class (applicable to the scenario where the scalar pipeline becomes a performance optimization bottleneck).
AscendC::MmadStrideParaBitModeParams mmadStrideBitModeParams(mmadStrideParaParams);
AscendC::SetMmadStridePara(mmadStrideBitModeParams);
```
