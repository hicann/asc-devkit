# SetLoadDataRepeat<a name="ZH-CN_TOPIC_0000001788021082"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:34:22.065Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not Supported
<!-- end id7 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

Used to set the **repeat** parameter of the [LoadData (convolution data movement)](../cube_compute_load/Load3D.md) API. After the **repeat** parameter is set, data movement for multiple iterations can be completed with a single **Load3D** API call.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetLoadDataRepeat(const LoadDataRepeatParam& repeatParams)
```

## Parameters<a name="section622mcpsimp"></a>

### API Parameters and Struct Parameters

**Table 1** Parameter Description

| Parameter Name | Input/Output | Meaning |
| --- | --- | --- |
| **repeatParams** | Input | Sets the **repeat** parameter of the [LoadData (convolution data movement)](../cube_compute_load/Load3D.md) API, of type **LoadDataRepeatParam**.<br>For the specific definition, see: \$\{INSTALL_DIR\}/include/ascendc/basic_api/API/kernel_struct_mm.h, where \$\{INSTALL_DIR\} is the path where the CANN software is installed.<br>For Parameter Description, See Also [Table 2](#table15780447181917). |

<a name="table15780447181917"></a>
**Table 2** LoadDataRepeatParam Struct Parameter Description

| Parameter Name | Meaning |
| --- | --- |
| **repeatStride** | Distance between the start addresses of the previous iteration and the next iteration in the height/width direction. Value Range: n∈[0, 65535]. Default Value: 0.<br>&nbsp;&nbsp;&nbsp;&nbsp;&bull; When **repeatMode** is 0, the unit of **repeatStride** is 16 Elements.<br>&nbsp;&nbsp;&nbsp;&nbsp;&bull; When **repeatMode** is 1, the unit of **repeatStride** depends on the specific model. In the following, **data_type** refers to the data type of the source operand in **Load3Dv2**. |
| **repeatTime** | Number of iterations in the height/width direction. Value Range: repeatTime∈[0, 255]. Default Value: 1. |
| **repeatMode** | Controls the direction of **repeat** iteration. Value Range: k∈[0, 1]. Default Value: 0.<br>&nbsp;&nbsp;&nbsp;&nbsp;&bull; 0: Iteration Along the height direction.<br>&nbsp;&nbsp;&nbsp;&nbsp;&bull; 1: Iteration Along the width direction. |
| **dstStride** | Offset of the output cube along the K axis, in units of 512B fractal. The support for **dstStride** varies by model. See Also [dstStride Parameter Support Degree Description](#dststride-parameter-support). |

### dstStride Parameter Support

<!-- npu="950" id8 -->
- For Ascend 950PR/Ascend 950DT, the **dstStride** parameter is newly added. When calling this API, you must configure the **dstStride** parameter.
<!-- end id8 -->

<!-- npu="910b" id9 -->
- For Atlas A2 training products/Atlas A2 inference products, this parameter is not supported.
<!-- end id9 -->

<!-- npu="A3" id10 -->
- For Atlas A3 training products/Atlas A3 inference products, this parameter is not supported.
<!-- end id10 -->

<!-- npu="310b" id11 -->
- For Atlas 200I/500 A2 inference products, this parameter is not supported.
<!-- end id11 -->

## Return Value

None

## Constraints

- When iterating along the height direction, the unit of **repeatStride** is 16 elements; when iterating along the width direction, the unit of **repeatStride** is 32/sizeof(data_type) elements.
- When **repeatTime** is 0, **Load3D** does not perform data movement, and the **Load3D** API is treated as a NOP (no operation).

<!-- npu="950" id12 -->
- For Ascend 950PR/Ascend 950DT, when calling the **Load3D** instruction, the **dstStride** parameter in this API must be configured.
<!-- end id12 -->

- The unit of **repeatStride** varies by chip model. For details, see the following:

    <!-- npu="950" id13 -->
    - Ascend 950PR/Ascend 950DT: The unit of **repeatStride** is 32/sizeof(data_type) elements.
    <!-- end id13 -->

    <!-- npu="910b" id14 -->
    - Atlas A2 training products/Atlas A2 inference products: The unit of **repeatStride** is 32/sizeof(data_type) elements.
    <!-- end id14 -->

    <!-- npu="A3" id15 -->
    - Atlas A3 training products/Atlas A3 inference products: The unit of **repeatStride** is 32/sizeof(data_type) elements.
    <!-- end id15 -->

    <!-- npu="310b" id16 -->
    - Atlas inference products: The unit of **repeatStride** is 64/sizeof(data_type) elements.
    <!-- end id16 -->

## Example<a name="section642mcpsimp"></a>

The following shows a code snippet:

```cpp
// The Load3Dv2 instruction completes the img2col process. After img2col, the height of cube A is ho * wo. By substituting the convolution kernel width, convolution kernel sliding stride, convolution kernel dilation coefficient, and other parameters into the formulas for ho and wo, the height of cube A is CeilAlign(k, fractalShape[0]). After img2col, the width of cube A is ci * kh * kw. By substituting kh=1 and kw=1, the width of cube A is CeilAlign(m, fractalShape[1]). Finally, set loadDataParams.enTranspose = true to transpose the entire cube A and transpose each fractal within it.
// Use the load3d API to implement NZ2ZZ.
AscendC::LoadData3DParamsV2<T> loadDataParams;

// Set the loadDataParams-related parameters.
...

// Use the SetLoadDataRepeat API to set the repeat parameters of the Load3Dv2 API.
AscendC::LoadDataRepeatParam repeatParams;
repeatParams.repeatTime = 1;  // Number of iterations in the height/width direction.
repeatParams.repeatStride = 1;  // Distance between the starting positions of consecutive iterations in the height/width direction.
repeatParams.repeatMode = 0;  // Iteration direction 0: iterate along height; 1: iterate along width.
repeatParams.dstStride = CeilDivision(m, fractalShape[0]);  // Offset of the output cube along the K axis.
AscendC::SetLoadDataRepeat(repeatParams);

AscendC::LoadData(a2Local, a1Local, loadDataParams);
```
