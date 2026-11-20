# SetMatrixPara<a name="ZH-CN_TOPIC_00000027121725633"></a>

<!-- md-trans-meta sourceCommit=971bc6393be4cb3e0f8244913e605c5710bb2cc9 translatedAt=2026-11-19T02:39:39.900Z pushedAt=2026-11-19T05:58:49.100Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute_aux_config/SetMatrixPara_res.md#id1 -->

## Function Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_macro_intf.h"`.

Before calling the [MmadFix](../mmad_compute/MmadFix.md) API to perform matrix multiplication computation, call this API to set the M dimension (left cube Height), K dimension (left cube Width/right cube Height), and groupK (number of K groups of grouped matrix multiplication) parameters of matrix multiplication. The N dimension (right cube Width) is passed in through the n field of the MmadFixParams parameter.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetMatrixPara(const MatrixParaBitModeParams& params)

__aicore__ inline void SetMatrixPara(const MatrixParaParams& params)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameter description

| Parameter | Input/Output | Meaning |
| ---------- | ---------- | ---------- |
| params | Input | Matrix multiplication dimension parameter, whose type is MatrixParaParams or MatrixParaBitModeParams.<br>For the specific definition of MatrixParaParams, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_macro.h`. For the specific definition of MatrixParaBitModeParams, see `${INSTALL_DIR}/asc/include/basic_api/kernel_operator_macro_bitmode_intf.h`. Replace `${INSTALL_DIR}` with the file storage path after CANN software installation.<br>For the description of MatrixParaParams parameters, see [Table 2](#table2matrixpara). For the description of MatrixParaBitModeParams parameters, see [Table 3](#table3matrixpara). |

<a name="table2matrixpara"></a>
**Table 2** Parameter description of the MatrixParaParams structure

| Parameter | Meaning |
| ---------- | ---------- |
| m | Left cube Height (M dimension), uint16\_t type, default value 0. |
| k | Left cube Width/right cube Height (K dimension), uint16\_t type, default value 0. |
| groupK | Number of K groups of grouped matrix multiplication, uint16\_t type, default value 0. Configured as 0 in the non-grouping scenario. |

<a name="table3matrixpara"></a>
**Table 3** Parameter description of the MatrixParaBitModeParams class

| Parameter | Meaning |
| ---------- | ---------- |
| config0 | uint64\_t type, forms a union with the MatrixParaBitModeConfig0 bit field structure type parameter config0BitMode, initialized to 0, and its value can be obtained by using the GetConfig0() function of the class object. |
| config0BitMode | MatrixParaBitModeConfig0 bit field structure type. For parameters, see [Table 4](#table4matrixpara). Forms a union with config0. |

MatrixParaBitModeParams optimizes the construction of the MatrixParaParams structure. This API is applicable to the scenario where the scalar pipeline becomes a performance optimization bottleneck. The MatrixParaBitModeParams class uses the union and bit field methods, expresses parameter types with bits, uses the bit field structure to automatically handle the bit count of input parameters, and uses the union feature to implement multi-parameter fused passing. Only one input parameter needs to be passed to contain all required information, and the corresponding underlying API only needs to receive one parameter. Meanwhile, when the value of a certain bit in a parameter needs to be modified, it can be implemented only through loops and bit operations without re-passing the parameter, which reduces scalar computation and improves performance.

The MatrixParaBitModeParams class can be directly initialized using a MatrixParaParams structure type object:

```cpp
__aicore__ inline MatrixParaBitModeParams(const MatrixParaParams& matrixParaParams);
```

The Set functions of each parameter can also be used to modify parameter values. In addition, because a union is used, config0 can also be directly modified bit by bit to modify parameters.

<a name="table4matrixpara"></a>
**Table 4** Parameter description of the MatrixParaBitModeConfig0 structure

| Parameter | Meaning |
| ---------- | ---------- |
| m | Left cube Height (M dimension), uint16\_t type.<br>This parameter is the lowest-bit parameter of the bit field structure, occupying 16 bits. Its value can be set by using the SetM() function of the MatrixParaBitModeParams class object and obtained by using the GetM() function. |
| k | Left cube Width/right cube Height (K dimension), uint16\_t type.<br>This parameter is the second-lowest-bit parameter of the bit field structure, occupying 16 bits. Its value can be set by using the SetK() function of the MatrixParaBitModeParams class object and obtained by using the GetK() function. |
| groupK | Number of K groups of grouped matrix multiplication, uint16\_t type.<br>This parameter is the highest-bit parameter of the bit field structure, occupying 16 bits. Its value can be set by using the SetGroupK() function of the MatrixParaBitModeParams class object and obtained by using the GetGroupK() function. |

## Data Type

The supported data types are uint16\_t and uint64\_t.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- This API must be used together with the [MmadFix](../mmad_compute/MmadFix.md) API and must be called before the MmadFix API.
- The M and K dimensions of matrix multiplication are configured through this API, and the N dimension is passed in through the n field of the MmadFixParams parameter.
- The registers involved in this section maintain their state within the same kernel function (Kernel) until they are manually changed, and do not inherit the previous state across different kernel functions (Kernel).

## Example<a name="section642mcpsimp"></a>

```cpp
constexpr uint16_t m = 16;    // Left cube Height (M dimension)
constexpr uint16_t k = 128;   // Left cube Width/right cube Height (K dimension)
constexpr uint16_t groupK = 0;  // Non-grouping scenario, groupK configured as 0

// Method 1: Use the MatrixParaParams structure
AscendC::MatrixParaParams matrixParaParams(m, k, groupK);
AscendC::SetMatrixPara(matrixParaParams);

// Method 2: Use the MatrixParaBitModeParams class (applicable to the scenario where the scalar pipeline becomes a performance optimization bottleneck)
AscendC::MatrixParaBitModeParams matrixParaBitModeParams(matrixParaParams);
AscendC::SetMatrixPara(matrixParaBitModeParams);
```
