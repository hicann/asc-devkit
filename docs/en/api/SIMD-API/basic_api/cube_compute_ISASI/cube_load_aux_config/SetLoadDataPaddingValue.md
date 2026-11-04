# SetLoadDataPaddingValue<a name="ZH-CN_TOPIC_0000001834740625"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T12:56:28.231Z pushedAt=2026-10-26T07:41:44.145Z nl=2 -->

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
- Atlas 200I/500 A2 inference product: supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_load_aux_config/SetLoadDataPaddingValue_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

Used to set the padding value when calling the [LoadData (convolution data movement)](../cube_compute_load/LoadData_3D.md) API. When the template parameter **isSetPadding** of LoadData (convolution data movement) is set to **false**, you need to set the padding value through this API. When it is set to **true**, the padding value set by this API does not take effect.

## Function Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void SetLoadDataPaddingValue(const T padValue)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| padValue | Input | The padding value. |

## Data Types

<!-- npu="950" id10 -->Ascend 950PR&950DT supports the following data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.<!-- end id10 --><br>
<!-- npu="310p" id11 -->Atlas inference products AI Core support the following data types: int8_t, uint8_t, int16_t, uint16_t, half.<!-- end id11 --><br>
<!-- npu="910b" id12 -->Atlas A2 training products/Atlas A2 inference products support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.<!-- end id12 --><br>
<!-- npu="A3" id13 -->Atlas A3 training products/Atlas A3 inference products support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.<!-- end id13 --><br>
<!-- npu="310b" id14 -->Atlas 200I/500 A2 inference products support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.<!-- end id14 --><br>

## Return Value

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

See [Example](SetFmatrix.md#section642mcpsimp)

The following shows a code snippet:

```cpp
// The LoadData (convolution data movement) v2 instruction completes the img2col process. After img2col, the height of matrix A is ho * wo. According to the calculation formulas of ho and wo, substituting parameters such as the convolution kernel width, convolution kernel sliding stride, and convolution kernel dilation coefficient, the height of matrix A is CeilAlign(k, fractalShape[0]). After img2col, the width of matrix A is ci * kh * kw. Substituting kh=1 and kw=1, the width of matrix A is CeilAlign(m, fractalShape[1]). Finally, set loadDataParams.enTranspose = true to transpose the entire matrix A and transpose each fractal within it.
// Use the load3d API to implement NZ2ZZ.
AscendC::LoadData3DParamsV2<T> loadDataParams;
// Set the loadDataParams related parameters.
...
// Do not set related attributes inside the LoadData (convolution data movement) API; set them explicitly through external APIs.
static constexpr AscendC::IsResetLoad3dConfig LOAD3D_CONFIG = {false, false};

// Use the SetLoadDataPaddingValue API to set the padValue parameter of the LoadData (convolution data movement) API.
AscendC::SetLoadDataPaddingValue(0);

AscendC::LoadData<T, LOAD3D_CONFIG>(a2Local, a1Local, loadDataParams);
```
