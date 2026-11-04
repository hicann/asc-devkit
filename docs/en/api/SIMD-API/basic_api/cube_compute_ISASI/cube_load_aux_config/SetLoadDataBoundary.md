# SetLoadDataBoundary<a name="ZH-CN_TOPIC_0000001787861430"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T12:54:26.917Z pushedAt=2026-10-26T07:41:44.140Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_load_aux_config/SetLoadDataBoundary_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

Sets the L1 Buffer (TPosition: A1/B1) boundary value required by the [LoadData (convolution data movement)](../cube_compute_load/LoadData_3D.md) API.

If, when the [LoadData (convolution data movement)](../cube_compute_load/LoadData_3D.md) instruction processes the source operand, the address of the source operand in the L1 Buffer (A1/B1) exceeds the set boundary, data is read starting from the start address of the L1 Buffer (A1/B1).

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetLoadDataBoundary(uint32_t boundaryValue)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| boundaryValue | Input | Boundary value, in bytes.<br>&nbsp;&nbsp;&bull; LoadData (convolution data movement) v1 instruction: the unit is 32 bytes.<br>&nbsp;&nbsp;&bull; LoadData (convolution data movement) v2 instruction: the unit is bytes. |

## Return Value

None

## Constraints<a name="section633mcpsimp"></a>

- If the boundary value is set using the **SetLoadDataBoundary** API, when used together with the **LoadData** (convolution data movement) instruction, the initial address of the L1 Buffer (A1/B1) of the **LoadData** (convolution data movement) instruction must be within the set boundary.
- If **boundaryValue** is set to 0, it indicates no boundary, and the entire L1 Buffer (A1/B1) can be used.
- When used together with the [LoadData (convolution data movement)](../cube_compute_load/LoadData_3D.md) instruction, the minimum boundary value is 1024. Setting a value from 1 to 1023 results in undefined behavior.
- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Example<a name="section642mcpsimp"></a>

See [Example](SetFmatrix.md#section642mcpsimp).

This API is used together with [LoadData (convolution data movement)](../cube_compute_load/LoadData_3D.md). The following code snippet shows an example:

```cpp
// The LoadData (convolution data movement) instruction completes the img2col process. After img2col, the height of matrix A is ho * wo. According to the formulas for ho and wo, substituting the convolution kernel width, convolution kernel sliding stride, convolution kernel dilation coefficient, and other parameters shows that the height of matrix A is CeilAlign(k, fractalShape[0]). After img2col, the width of matrix A is ci * kh * kw. Substituting kh=1 and kw=1 shows that the width of matrix A is CeilAlign(m, fractalShape[1]). Finally, set loadDataParams.enTranspose = true to transpose the entire matrix A and transpose each fractal within it.
// Use the load3d API to implement NZ2ZZ.
AscendC::LoadData3DParamsV2<T> loadDataParams;
// Set the loadDataParams-related parameters.
...

// Set SetLoadDataBoundary to 0 to use the entire L1 Buffer.
AscendC::SetLoadDataBoundary(0);

AscendC::LoadData(a2Local, a1Local, loadDataParams);
```
