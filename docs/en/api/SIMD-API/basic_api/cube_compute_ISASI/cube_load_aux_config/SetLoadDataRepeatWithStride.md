# SetLoadDataRepeatWithStride<a id="ZH-CN_TOPIC_0000002583382412"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T13:00:27.148Z pushedAt=2026-10-26T07:41:44.151Z nl=2 -->

## Applicable Products<a id="section1550532418810"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_load_aux_config/SetLoadDataRepeatWithStride_res.md#id1 -->

## Description<a id="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

Sets the **repeat** parameter of the [LoadDataWithStride](../cube_compute_load/LoadDataWithStride.md) API. After the **repeat** parameter is set, multiple iterations of data transfer can be completed by calling the LoadDataWithStride API once. It is also used to configure the offset of the output matrix along the K-axis.

## Function Prototype<a id="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetLoadDataRepeatWithStride(const LoadDataRepeatParamWithStride& repeatParams)
```

## Parameters<a id="section622mcpsimp"></a>

**Table 1** Parameter Description

| Parameter Name | Input/Output | Meaning |
| --- | --- | --- |
| repeatParams | Input | Sets the **repeat** parameter of the [LoadDataWithStride](../cube_compute_load/LoadDataWithStride.md) API. The type is **SetLoadDataRepeatWithStride**.<br>For the specific definition, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_mm.h`. Replace `${INSTALL_DIR}` with the file storage path after CANN software installation.<br>For parameter description, see [Table 2](#table15780447181826). |

**Table 2** LoadDataRepeatParamWithStride structure parameter description<a name="table15780447181826"></a>  

| Parameter Name | Meaning |
| -------- | ---- |
| repeatStride | Distance between the start addresses of the previous iteration and the next iteration in the height/width direction. Value range: repeatStride∈[0, 65535]. Default value: 0.<br>&nbsp;&nbsp;&nbsp;&nbsp;&bull; When **repeatMode** is 0, the unit of **repeatStride** is 16 elements.<br>&nbsp;&nbsp;&nbsp;&nbsp;&bull; When **repeatMode** is 1, the unit of **repeatStride** is 32/sizeof(**data_type**) elements, where **data_type** refers to the data type of the source operand. |
| repeatTime | Number of iterations in the height/width direction. Value range: repeatTime∈[0, 255]. Default value: 1. |
| repeatMode | Controls the direction of **repeat** iteration. Value range: repeatMode∈[0, 1]. Default value: 0.<br>&nbsp;&nbsp;&nbsp;&nbsp;&bull; 0: Iteration along the height direction.<br>&nbsp;&nbsp;&nbsp;&nbsp;&bull; 1: Iteration along the width direction. |
| dstStride | Offset of the output matrix in the K-axis direction, in units of 512-byte fractals. |

## Return Value<a id="section4219135304839"></a>

None

## Constraints<a id="section633mcpsimp"></a>

- **repeatTime**=0 means no data movement is performed, and the **LoadDataWithStride** API is treated as a NOP (no operation).
- When calling the [LoadDataWithStride](../cube_compute_load/LoadDataWithStride.md) API, you must configure the **dstStride** parameter in this API. If the **repeat** mode is not enabled, **repeatTime**, **repeatStride**, and **repeatMode** use their default constructed values.

## Example<a id="section1761417552405"></a>

This API is used together with the LoadDataWithStride API. The following code snippet shows an example:

```cpp
// The LoadDataWithStride instruction completes the img2col process. After img2col, the height of matrix A is ho * wo. According to the formulas for ho and wo, substituting parameters such as the convolution kernel width, convolution kernel sliding stride, and convolution kernel dilation coefficient shows that the height of matrix A is CeilAlign(k, fractalShape[0]). After img2col, the width of matrix A is ci * kh * kw. Substituting kh=1 and kw=1 shows that the width of matrix A is CeilAlign(m, fractalShape[1]). Finally, set loadDataParams.enTranspose = true to transpose the entire matrix A and transpose each fractal within it.
AscendC::LoadData3DParamsV2<T> loadDataParams;

// Set the loadDataParams-related parameters.
...

// Use the SetLoadDataRepeatWithStride API to set the repeat parameter of the LoadDataWithStride API.
AscendC::LoadDataRepeatParamWithStride repeatParams;
repeatParams.repeatTime = 1;  // Number of iterations in the height/width direction.
repeatParams.repeatStride = 1;  // Distance between the start positions of consecutive iterations in the height/width direction.
repeatParams.repeatMode = 0;  // Iteration direction: 0: iterate along height; 1: iterate along width.
repeatParams.dstStride = CeilDivision(m, fractalShape[0]);  // Offset of the output matrix along the K axis.
AscendC::SetLoadDataRepeatWithStride(repeatParams);

AscendC::LoadDataWithStride(a2Local, a1Local, loadDataParams);
```
