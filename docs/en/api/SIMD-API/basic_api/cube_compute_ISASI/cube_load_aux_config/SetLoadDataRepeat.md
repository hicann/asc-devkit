# SetLoadDataRepeat<a name="ZH-CN_TOPIC_0000001788021082"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T12:58:57.317Z pushedAt=2026-10-26T07:41:44.147Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

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
- Atlas 200I/500 A2 inference products: supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_load_aux_config/SetLoadDataRepeat_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

Used to set the **repeat** parameter of the [LoadData (Convolution Data)](../cube_compute_load/LoadData_3D.md) API. After the **repeat** parameter is set, data transfer for multiple iterations can be completed by calling the LoadData (Convolution Data) API once.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetLoadDataRepeat(const LoadDataRepeatParam& repeatParams)
```

## Parameters<a name="section622mcpsimp"></a>

### API Parameters and Structure Parameters

**Table 1** Parameters

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| **repeatParams** | Input | Sets the **repeat** parameter of the [LoadData (convolution data movement)](../cube_compute_load/LoadData_3D.md) API. The type is **LoadDataRepeatParam**.<br>For the specific definition, see: `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_mm.h`. Replace `${INSTALL_DIR}` with the file storage path after CANN software installation.<br>For parameter description, see [Table 2](#table15780447181917). |

<a name="table15780447181917"></a>
**Table 2** LoadDataRepeatParam structure parameters

| Parameter | Meaning |
| --- | --- |
| **repeatStride** | Distance between the start addresses of the previous iteration and the next iteration in the height/width direction. Value range: repeatStride∈[0, 65535]. Default value: 0.<br>&nbsp;&nbsp;&nbsp;&nbsp;&bull; When **repeatMode** is 0, the unit of **repeatStride** is 16 elements.<br>&nbsp;&nbsp;&nbsp;&nbsp;&bull; When **repeatMode** is 1, the unit of **repeatStride** depends on the specific model. In the following, **data_type** refers to the data type of the source operand in LoadData (convolution data movement) v2. |
| **repeatTime** | Number of iterations in the height/width direction. Value range: repeatTime∈[0, 255]. Default value: 1. |
| **repeatMode** | Controls the direction of repeat iteration. Value range: repeatMode∈[0, 1]. Default value: 0.<br>&nbsp;&nbsp;&nbsp;&nbsp;&bull; 0: Iterates along the height direction.<br>&nbsp;&nbsp;&nbsp;&nbsp;&bull; 1: Iterates along the width direction. |
| **dstStride** | Offset of the output matrix along the K axis, in units of 512B fractal. The support for **dstStride** varies by model. See [dstStride Parameter Support Description](#dststride-parameter-support-description). |

### dstStride Parameter Support Description

<!-- npu="950" id8 -->
- For Ascend 950PR&950DT products, the **dstStride** parameter is added. You must configure the **dstStride** parameter when calling this API.
<!-- end id8 -->

<!-- npu="910b" id9 -->
- For Atlas A2 products, this parameter is not supported.
<!-- end id9 -->

<!-- npu="A3" id10 -->
- For Atlas A3 products, this parameter is not supported.
<!-- end id10 -->

<!-- npu="310b" id11 -->
- For Atlas 200I/500 A2 inference products, this parameter is not supported.
<!-- end id11 -->

## Return Value

None

## Constraints

- When iterating along the height direction, the unit of **repeatStride** is 16 elements; when iterating along the width direction, the unit of **repeatStride** is 32/sizeof\(data\_type\) elements.
- When **repeatTime** is 0, **LoadData** (convolution data movement) does not perform any transfer, and the **LoadData** (convolution data movement) API is treated as a NOP (no operation).

<!-- npu="950" id12 -->
- For Ascend 950PR&950DT products, the **dstStride** parameter in this API must be configured when calling the **LoadData** (convolution data movement) instruction.
<!-- end id12 -->

- The unit of **repeatStride** varies by chip model. For details, see the following:

    <!-- npu="950" id13 -->
    - For Ascend 950PR&950DT products, the unit of **repeatStride** is 32/sizeof(data_type) elements.
    <!-- end id13 -->

    <!-- npu="910b" id14 -->
    - For Atlas A2 products, the unit of **repeatStride** is 32/sizeof(data_type) elements.
    <!-- end id14 -->

    <!-- npu="A3" id15 -->
    - For Atlas A3 products, the unit of **repeatStride** is 32/sizeof(data_type) elements.
    <!-- end id15 -->

    <!-- npu="310b" id16 -->
    - For Atlas 200I/500 A2 inference products, the unit of **repeatStride** is 64/sizeof(data_type) elements.
    <!-- end id16 -->

## Example<a name="section642mcpsimp"></a>

The following code snippet is provided as an example:

```cpp
// The LoadData (Convolution Data transfer) v2 instruction completes the img2col process. After img2col, the height of matrix A is ho * wo. According to the formulas of ho and wo, substituting parameters such as the convolution kernel width, convolution kernel sliding stride, and convolution kernel dilation coefficient, the height of matrix A is CeilAlign(k, fractalShape[0]). After img2col, the width of matrix A is ci * kh * kw. Substituting kh=1 and kw=1, the width of matrix A is CeilAlign(m, fractalShape[1]). Finally, set loadDataParams.enTranspose = true to transpose the entire matrix A and transpose each fractal in it.
// Use the load3d API to implement NZ2ZZ.
AscendC::LoadData3DParamsV2<T> loadDataParams;

// Set the parameters related to loadDataParams.
...

// Use the SetLoadDataRepeat API to set the repeat parameter of the LoadData (Convolution Data transfer) v2 API.
AscendC::LoadDataRepeatParam repeatParams;
repeatParams.repeatTime = 1;  // Number of iterations in the height/width direction.
repeatParams.repeatStride = 1;  // Distance between the start positions of consecutive iterations in the height/width direction.
repeatParams.repeatMode = 0;  // Iteration direction 0: iterate along height; 1: iterate along width.
repeatParams.dstStride = CeilDivision(m, fractalShape[0]);  // Offset of the output matrix along the K axis.
AscendC::SetLoadDataRepeat(repeatParams);

AscendC::LoadData(a2Local, a1Local, loadDataParams);
```
