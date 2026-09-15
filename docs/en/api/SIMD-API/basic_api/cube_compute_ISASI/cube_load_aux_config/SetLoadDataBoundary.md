# SetLoadDataBoundary<a name="ZH-CN_TOPIC_0000001787861430"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:37:57.954Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

Sets the L1 Buffer (TPosition: A1/B1) boundary value required by the [LoadData (convolution data loading)](../cube computation loading in/Load3D.md) API.

If, when the [LoadData (convolution data loading)](../cube computation loading in/Load3D.md) instruction processes the source operand, the address of the source operand on A1/B1 exceeds the set boundary, data is read starting from the start address of A1/B1.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetLoadDataBoundary(uint32_t boundaryValue)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| boundaryValue | Input | Boundary value, in bytes.<br>&nbsp;&nbsp;&bull; Load3Dv1 instruction: in units of 32 bytes.<br>&nbsp;&nbsp;&bull; Load3Dv2 instruction: in units of bytes. |

## Return Value

None

## Constraints<a name="section633mcpsimp"></a>

- If the boundary value is set using the **SetLoadDataBoundary** API, when used together with the **Load3D** instruction, the initial addresses of A1/B1 of the **Load3D** instruction must be within the set boundary.
- If **boundaryValue** is set to 0, it indicates no boundary, and the entire A1/B1 can be used.
- When used together with the [LoadData (convolution data loading)](../cube_compute_load/Load3D.md) instruction, the minimum boundary value is 1024. Setting a value from 1 to 1023 results in undefined behavior.
- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Example<a name="section642mcpsimp"></a>

See [Example](SetFmatrix.md#section642mcpsimp).

This API is used together with the [LoadData (convolution data loading in)](../cube_compute_load/Load3D.md) API. The following shows a code snippet:

```cpp
// The Load3D instruction completes the img2col process. After img2col, the height of cube A is ho * wo. According to the formulas for ho and wo, substituting parameters such as the convolution kernel width, convolution kernel sliding stride, and convolution kernel dilation coefficient, the height of cube A is CeilAlign(k, fractalShape[0]). After img2col, the width of cube A is ci * kh * kw. Substituting kh=1 and kw=1, the width of cube A is CeilAlign(m, fractalShape[1]). Finally, set loadDataParams.enTranspose = true to transpose the entire cube A and transpose each fractal within it.
// Use the load3d API to implement NZ2ZZ.
AscendC::LoadData3DParamsV2<T> loadDataParams;
// Set the parameters related to loadDataParams.
...

// Set SetLoadDataBoundary to 0 to use the entire L1 Buffer.
AscendC::SetLoadDataBoundary(0);

AscendC::LoadData(a2Local, a1Local, loadDataParams);
```
