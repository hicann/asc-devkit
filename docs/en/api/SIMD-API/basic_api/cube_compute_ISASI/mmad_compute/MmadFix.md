# MmadFix<a name="ZH-CN_TOPIC_000000253823118103"></a>

<!-- md-trans-meta sourceCommit=971bc6393be4cb3e0f8244913e605c5710bb2cc9 translatedAt=2026-11-19T06:18:41.903Z pushedAt=2026-11-19T06:25:40.292Z nl=2 -->

## Supported Products

### Prototype Without Bias

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadFix_res.md#id1 -->

### Prototype With bias

<!-- npu="950" id12 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id12 -->
<!-- npu="A3" id13 -->
- Atlas A3 products: Not supported
<!-- end id13 -->
<!-- npu="910b" id14 -->
- Atlas A2 products: Not supported
<!-- end id14 -->
<!-- npu="310b" id15 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id15 -->
<!-- npu="310p" id16 -->
- Atlas inference products AI Core: Not supported
<!-- end id16 -->
<!-- npu="310p" id17 -->
- Atlas inference products Vector Core: Not supported
<!-- end id17 -->
<!-- npu="910" id18 -->
- Atlas training products: Not supported
<!-- end id18 -->
<!-- npu="x90" id19 -->
- KirinX90 processor products: not supported
<!-- end id19 -->
<!-- npu="9020" id20 -->
- Kirin9020 processor products: not supported
<!-- end id20 -->
<!-- npu="9030" id21 -->
- Kirin9030 processor products: not supported
<!-- end id21 -->
<!-- npu="9050" id22 -->
- Kirin9050 processor products: supported
<!-- end id22 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadFix_res.md#id2 -->

## Function Description

The header file path is `"basic_api/kernel_operator_macro_intf.h"`.

The MmadFix API is the core cube computation API of Ascend C for Ascend AI chips and Kirin processor products. It is designed for high-performance operator development, encapsulates cube computation capabilities, and supports the fusion of post-processing functions such as quantization (Quant), activation (ReLU), and eltwise. It is widely used in operator development for neural network layers (such as fully connected layers) and quantized inference scenarios.

The MmadFix API implements cube computation capabilities. Its mathematical expression is:

$$
C = A \times B + Bias
$$

<!-- npu="9050" id23 -->
For Kirin9050 processor products:

**Table 1** Description of cubes A, B, and C in cube computation

| Cube computation logic | Physical location of cube computation | Dimension | Input/output data format | Data Type |
| --- | --- | --- | --- | --- |
| A | L1 Buffer | M x K | Nz | <a href="#zh_cn_topic_mmadfix_section5">Data type</a> |
| B | L0B Buffer | K x N | Zn | <a href="#zh_cn_topic_mmadfix_section5">Data type</a> |
| C | L1 Buffer | M x N, can be initialized with the bias cube Bias | Nz | <a href="#zh_cn_topic_mmadfix_section5">Data type</a> |

The M and K dimensions of the matrix multiplication are configured through the SetMatrixPara API (MatrixParaParams parameter), and the N dimension is passed in via the n field of the mmadFixParams parameter. The computation result of the MmadFix API is written directly to the L1 Buffer, eliminating the process of moving the result out of the L0C Buffer in traditional matrix multiplication.
<!-- end id23 -->


## Prototype

- Without bias

    ```cpp
    template <typename T, typename U, typename S>
    __aicore__ inline void MmadFix(const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right, const MmadFixBitModeParams& mmadFixParams)

    template <typename T, typename U, typename S>
    __aicore__ inline void MmadFix(const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right, const MmadFixParams& mmadFixParams)
    ```

- With bias

    ```cpp
    template <typename T, typename U, typename S, typename V>
    __aicore__ inline void MmadFix(const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right, const LocalTensor<V>& bias, const MmadFixBitModeParams& mmadFixParams)

    template <typename T, typename U, typename S, typename V>
    __aicore__ inline void MmadFix(const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right, const LocalTensor<V>& bias, const MmadFixParams& mmadFixParams)
    ```

## Parameters<a id="zh_cn_topic_mmadfix_section4"></a>

**Table 2** Parameters

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| dst | Output | Destination operand, result cube C, of type LocalTensor. The supported physical storage location is L1 Buffer (TPosition:C1). |
| left | Input | Source operand, left cube A, of type LocalTensor. The supported physical storage location is L1 Buffer (TPosition:A1). |
| right | Input | Source operand, right cube B, of type LocalTensor. The supported physical storage location is L0B Buffer (TPosition:B2). |
| bias | Input | Source operand, Bias cube, of type LocalTensor. The storage location and dimension of Bias are determined by the biasMatrixBroadcast parameter in mmadFixParams: when biasMatrixBroadcast=true, the Bias cube is located at BT Buffer (TPosition:C2) with a dimension of 1 x N and is broadcast along the M direction; when biasMatrixBroadcast=false, the Bias cube is located at L1 Buffer (TPosition:C1) with a dimension of M x N and participates in computation as the initial value of cube C. |
| mmadFixParams | Input | Matrix multiplication related parameters, of type MmadFixParams or MmadFixBitModeParams.<br>For the specific definition of MmadFixParams, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_macro.h`. For the specific definition of MmadFixBitModeParams, see `${INSTALL_DIR}/asc/include/basic_api/kernel_operator_macro_bitmode_intf.h`. Replace `${INSTALL_DIR}` with the file storage path after CANN software installation.<br>For the MmadFixParams parameter description, see [Table 3](#zh_cn_topic_mmadfix_section4_table3). For the MmadFixBitModeParams parameter description, see [Table 4](#zh_cn_topic_mmadfix_section4_table4). |

**Table 3** Parameter description of the MmadFixParams structure<a id="zh_cn_topic_mmadfix_section4_table3"></a>

| Parameter | Meaning |
| --- | --- |
| biasMatrixInitVal | Whether to initialize result cube C to 0. The default value is false.<br>&nbsp;&nbsp;&bull; true: The initial value of cube C is 0, and the computation result is C = A \* B (+ Bias);<br>&nbsp;&nbsp;&bull; false: Cube C is not initialized and retains existing data, and the computation result is C += A \* B (+ Bias). |
| biasMatrixBroadcast | Whether to enable broadcasting of the Bias cube along the M direction. The default value is false.<br>&nbsp;&nbsp;&bull; true: The Bias cube has a dimension of 1 x N, is located at BT Buffer (TPosition:C2), and is broadcast along the M direction, meaning that each row of the result cube uses the same Bias;<br>&nbsp;&nbsp;&bull; false: The Bias cube has a dimension of M x N, is located at L1 Buffer (TPosition:C1), and participates in computation as the initial value of cube C. |
| preQuantMode | Quantization mode before matrix multiplication, of pre_quant_t enumeration type. The default value is No_Quant. Values include: No_Quant (no quantization), REQS8_Vector, REQS8_Scalar, DEQF16_Vector, DEQF16_Scalar, DEQS16_Vector, DEQS16_Scalar, DEQS32_Vector, DEQS32_Scalar, REQS4_Vector, REQS4_Scalar, QS322FP8_E4M3_INT_Vector, QS322FP8_E4M3_INT_Scalar. |
| preReluMode | ReLU activation mode before matrix multiplication, of pre_relu_t enumeration type. The default value is NoRelu (activation not enabled). |
| postQuantMode | Quantization mode after matrix multiplication, of post_quant_t enumeration type. The default value is No_Quant (no quantization). |
| actPostMode | Activation mode after matrix multiplication, of uint8_t type. The default value is 0. |
| preClipReluMode | Clip ReLU mode before matrix multiplication, of uint8_t type. The default value is 0. |
| eltwiseOp | eltwise operation type, of eltwise_op_t enumeration type. The default value is No_Eltwise (eltwise operation not enabled). |
| eltwiseAntiqEnable | Whether to enable eltwise dequantization. The default value is false. |
| eltwiseBroadcastEnable | Whether to enable eltwise broadcast. The default value is false. |
| itaEnable | Whether to enable the ITA function. The default value is false. After it is enabled, the maximum value statistics related address must be configured through the SetItaMaxAddr API. |
| biasS32Enable | Whether to enable Bias in S32 (int32_t) format. The default value is false. |
| hmaxEnable | Whether to enable the HMAX (maximum value statistics) function. The default value is false. After it is enabled, the related address must be configured through the SetItaMaxAddr API. |
| lsbMask | Mask control parameter, of lsb_mask_t enumeration type. The default value is Disable. |
| gemvCtrl | GEMV mode control parameter. The default value is false. In the matrix-vector multiplication scenario where M = 1, it is used to control the GEMV mode. |
| groupMode | Grouping mode, of uint8_t type. The default value is 0. |
| dependEnable | Whether to enable inter-instruction dependency control. The default value is false. |
| instrId | Instruction number, of instr_id_t enumeration type. The default value is ID_0. |
| postProcHalfEnable | Whether to enable half precision post-processing. The default value is false. |
| layoutCtrl | Layout control parameter. The default value is false. |
| n | Right cube Width (N dimension), of uint16_t type. The default value is 0. |

**Table 4** Parameter description of the MmadFixBitModeParams class<a id="zh_cn_topic_mmadfix_section4_table4"></a>

| Parameter | Meaning |
| --- | --- |
| config0 | uint64_t type, forms a union with the MmadFixBitModeConfig0 bit field structure type parameter config0BitMode, initialized to 0, and its value can be obtained by using the GetConfig0() function of the class object. |
| config0BitMode | MmadFixBitModeConfig0 bit field structure type. For parameters, see [Table 5](#zh_cn_topic_mmadfix_section4_table5). It forms a union with config0. |

MmadFixBitModeParams optimizes the construction of the MmadFixParams structure. This API is suitable for scenarios where the scalar pipeline becomes a performance optimization bottleneck. The MmadFixBitModeParams class uses the union and bit field methods, expresses parameter types with bits, uses the bit field structure to automatically process the bit count of input parameters, and leverages the characteristics of the union to implement multi-parameter fusion passing. Only one input parameter needs to be passed to contain all required information, and the corresponding underlying API only needs to receive one parameter. In addition, when the value of a certain bit in a parameter needs to be modified, it can be implemented only through loops and bit operations without re-passing the parameter, which reduces scalar computation and improves performance.

The MmadFixBitModeParams class can be initialized directly using an object of the MmadFixParams structure type:

```cpp
__aicore__ inline MmadFixBitModeParams(const MmadFixParams& mmadFixParams);
```

The Set functions of each parameter can also be used to modify parameter values. In addition, because a union is used, config0 can also be directly modified bit by bit to modify parameters.

**Table 5** Parameter description of the MmadFixBitModeConfig0 structure<a id="zh_cn_topic_mmadfix_section4_table5"></a>

| Parameter | Meaning |
| --- | --- |
| biasMatrixInitVal | Whether to initialize result cube C to 0. The value is true/false.<br>This parameter is the lowest-bit parameter of the bit field structure, occupying 1 bit. Its value can be set by using the SetBiasMatrixInitVal() function of the MmadFixBitModeParams class object and obtained by using the GetBiasMatrixInitVal() function. |
| biasMatrixBroadcast | Whether to enable broadcasting of the Bias cube along the M direction. The value is true/false.<br>This parameter is the second-lowest-bit parameter of the bit field structure, occupying 1 bit. Its value can be set by using the SetBiasMatrixBroadcast() function of the MmadFixBitModeParams class object and obtained by using the GetBiasMatrixBroadcast() function. |
| preQuantMode | Quantization mode before matrix multiplication, of pre_quant_t enumeration type.<br>This parameter is the third-lowest-bit parameter of the bit field structure, occupying 5 bits. Its value can be set by using the SetPreQuantMode() function of the MmadFixBitModeParams class object and obtained by using the GetPreQuantMode() function. |
| preReluMode | ReLU activation mode before matrix multiplication, of pre_relu_t enumeration type.<br>This parameter occupies 3 bits. Its value can be set by using the SetPreReluMode() function of the MmadFixBitModeParams class object and obtained by using the GetPreReluMode() function. |
| postQuantMode | Quantization mode after matrix multiplication, of post_quant_t enumeration type.<br>This parameter occupies 5 bits. Its value can be set by using the SetPostQuantMode() function of the MmadFixBitModeParams class object and obtained by using the GetPostQuantMode() function. |
| actPostMode | Activation mode after matrix multiplication, of uint8_t type.<br>This parameter occupies 3 bits. Its value can be set by using the SetactPostMode() function of the MmadFixBitModeParams class object and obtained by using the GetactPostMode() function. |
| preClipReluMode | Clip ReLU mode before matrix multiplication, of uint8_t type.<br>This parameter occupies 1 bit. Its value can be set by using the SetPreClipReluMode() function of the MmadFixBitModeParams class object and obtained by using the GetPreClipReluMode() function. |
| eltwiseOp | eltwise operation type, of eltwise_op_t enumeration type.<br>This parameter occupies 3 bits. Its value can be set by using the SetEltwiseOp() function of the MmadFixBitModeParams class object and obtained by using the GetEltwiseOp() function. |
| eltwiseAntiqEnable | Whether to enable eltwise dequantization. The value is true/false.<br>This parameter occupies 1 bit. Its value can be set by using the SetEltwiseAntiqEnable() function of the MmadFixBitModeParams class object and obtained by using the GetEltwiseAntiqEnable() function. |
| eltwiseBroadcastEnable | Whether to enable eltwise broadcast. The value is true/false.<br>This parameter occupies 1 bit. Its value can be set by using the SetEltwiseBroadcastEnable() function of the MmadFixBitModeParams class object and obtained by using the GetEltwiseBroadcastEnable() function. |
| itaEnable | Whether to enable the ITA function. The value is true/false.<br>This parameter occupies 1 bit. Its value can be set by using the SetItaEnable() function of the MmadFixBitModeParams class object and obtained by using the GetItaEnable() function. |
| biasS32Enable | Whether to enable Bias in S32 (int32_t) format. The value is true/false.<br>This parameter occupies 1 bit. Its value can be set by using the SetBiasS32Enable() function of the MmadFixBitModeParams class object and obtained by using the GetBiasS32Enable() function. |
| hmaxEnable | Whether to enable the HMAX (maximum value statistics) function. The value is true/false.<br>This parameter occupies 1 bit. Its value can be set by using the SetHmaxEnable() function of the MmadFixBitModeParams class object and obtained by using the GetHmaxEnable() function. |
| lsbMask | Mask control parameter, of lsb_mask_t enumeration type.<br>This parameter occupies 3 bits. Its value can be set by using the SetLsbMask() function of the MmadFixBitModeParams class object and obtained by using the GetLsbMask() function. |
| gemvCtrl | GEMV mode control parameter. The value is true/false.<br>This parameter occupies 1 bit. Its value can be set by using the SetGemvCtrl() function of the MmadFixBitModeParams class object and obtained by using the GetGemvCtrl() function. |
| groupMode | Grouping mode, of uint8_t type.<br>This parameter occupies 3 bits. Its value can be set by using the SetGroupMode() function of the MmadFixBitModeParams class object and obtained by using the GetGroupMode() function. |
| dependEnable | Whether to enable inter-instruction dependency control. The value is true/false.<br>This parameter occupies 1 bit. Its value can be set by using the SetDependEnable() function of the MmadFixBitModeParams class object and obtained by using the GetDependEnable() function. |
| instrId | Instruction number, of instr_id_t enumeration type.<br>This parameter occupies 2 bits. Its value can be set by using the SetInstrId() function of the MmadFixBitModeParams class object and obtained by using the GetInstrId() function. |
| postProcHalfEnable | Whether to enable half precision post-processing. The value is true/false.<br>This parameter occupies 1 bit. Its value can be set by using the SetPostProcHalfEnable() function of the MmadFixBitModeParams class object and obtained by using the GetPostProcHalfEnable() function. |
| layoutCtrl | Layout control parameter. The value is true/false.<br>This parameter occupies 1 bit. Its value can be set by using the SetLayoutCtrl() function of the MmadFixBitModeParams class object and obtained by using the GetLayoutCtrl() function. |
| n | Right cube Width (N dimension), of uint16_t type.<br>This parameter is the highest-bit parameter of the bit field structure, occupying 16 bits. Its value can be set by using the SetN() function of the MmadFixBitModeParams class object and obtained by using the GetN() function. |
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadFix_res.md#id3 -->

## Data Type<a id="zh_cn_topic_mmadfix_section5"></a>

**Table 6** Supported precision type combinations for dst, left, right, and bias (Kirin9050 processor products)

| Left cube left type | Right cube right type | bias type | Result cube dst type |
| --- | --- | --- | --- |
| half | half | half | half |
| int16_t | int8_t | int32_t | int32_t |
| int8_t | int8_t | int32_t | int32_t |

## Return Value

None

## Constraints

- The result cube C supports only the physical storage location L1 Buffer (TPosition:C1), the left cube A supports only the physical storage location L1 Buffer (TPosition:A1), and the right cube B supports only the physical storage location L0B Buffer (TPosition:B2).
- The M and K dimensions of the matrix multiplication are configured through the SetMatrixPara API, and the N dimension is passed in via the n parameter of mmadFixParams.
- In the prototype with bias input, the storage location and dimension of the Bias cube are determined by the biasMatrixBroadcast parameter in mmadFixParams, and must be consistent with the actual storage location of the bias LocalTensor.
- When post-processing functions such as quantization, activation, and eltwise are enabled, the corresponding parameters (such as the quantization parameter address and the ReLU parameter address) must be configured through MacroCube series APIs such as SetFixPipeConfig.
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadFix_res.md#id4 -->

## Example

```cpp
// Matrix multiplication example for Kirin9050 processor products: C = A x B + Bias
// Input: left cube A (M x K) and right cube B (K x N) of int8_t type, Bias (1 x N) of int32_t type
// Output: result cube C (M x N) of int32_t type
constexpr uint16_t m = 16;    // Left cube Height (M dimension)
constexpr uint16_t k = 128;   // Left cube Width and right cube Height (K dimension)
constexpr uint16_t n = 64;    // Right cube Width (N dimension)

// The left cube A is located at L1 Buffer (A1), and the right cube B is located at L0B Buffer (B2).
// The result cube C is located at L1 Buffer (C1), and the Bias cube is located at BT Buffer (C2).
AscendC::TBuf<AscendC::TPosition::A1> leftBuf;
AscendC::TBuf<AscendC::TPosition::B2> rightBuf;
AscendC::TBuf<AscendC::TPosition::C1> dstBuf;
AscendC::TBuf<AscendC::TPosition::C2> biasBuf;

// 1. Configure the M/K dimension information of the matrix multiplication (the N dimension is passed in via the n field of MmadFixParams).
AscendC::MatrixParaParams matrixParaParams(m, k, 0 /* groupK */);
AscendC::SetMatrixPara(matrixParaParams);

// 2. Configure the MmadFix computation parameters.
AscendC::MmadFixParams mmadFixParams;
mmadFixParams.n = n;
mmadFixParams.biasMatrixInitVal = true;    // Initialize the result cube to 0.
mmadFixParams.biasMatrixBroadcast = true;  // Bias is located at BT Buffer and broadcast along the M direction.

// 3. Perform matrix multiplication: C = A x B + Bias
AscendC::MmadFix(dstBuf.Get<int32_t>(), leftBuf.Get<int8_t>(), rightBuf.Get<int8_t>(),
                 biasBuf.Get<int32_t>(), mmadFixParams);
```

Calling method using MmadFixBitModeParams (applicable to scenarios where the scalar pipeline becomes a performance optimization bottleneck):

```cpp
// Method 1: Directly use an MmadFixParams object to construct MmadFixBitModeParams.
AscendC::MmadFixBitModeParams mmadFixBitModeParams(mmadFixParams);

// Method 2: Use Set functions to configure parameters one by one (choose either method 1 or method 2).
// AscendC::MmadFixBitModeParams mmadFixBitModeParams;
// mmadFixBitModeParams.SetN(n);
// mmadFixBitModeParams.SetBiasMatrixInitVal(true);
// mmadFixBitModeParams.SetBiasMatrixBroadcast(true);

AscendC::MmadFix(dstBuf.Get<int32_t>(), leftBuf.Get<int8_t>(), rightBuf.Get<int8_t>(),
                 biasBuf.Get<int32_t>(), mmadFixBitModeParams);
```
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadFix_res.md#id5 -->
