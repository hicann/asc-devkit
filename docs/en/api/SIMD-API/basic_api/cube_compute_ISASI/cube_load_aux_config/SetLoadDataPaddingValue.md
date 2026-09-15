# SetLoadDataPaddingValue<a name="ZH-CN_TOPIC_0000001834740625"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:33:23.373Z -->

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

Used to set the pad padding value when calling the [LoadData (convolution data movement)](../cube_compute_load/Load3D.md) API. When the template parameter **isSetPadding** of **Load3D** is set to **true**, you need to set the pad padding value through this API; when it is set to **false**, the padding value set by this API does not take effect.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void SetLoadDataPaddingValue(const T padValue)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| **padValue** | Input | Numeric value of the Pad padding value. |

## Data Types

<!-- npu="950" id10 -->Ascend 950PR/Ascend 950DT: Supported data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.<!-- end id10 --><br>
<!-- npu="310p" id11 -->Atlas inference products AI Core: Supported data types: int8_t, uint8_t, int16_t, uint16_t, half.<!-- end id11 --><br>
<!-- npu="910b" id12 -->Atlas A2 training products/Atlas A2 inference products: Supported data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.<!-- end id12 --><br>
<!-- npu="A3" id13 -->Atlas A3 training products/Atlas A3 inference products: Supported data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.<!-- end id13 --><br>
<!-- npu="310b" id14 -->Atlas 200I/500 A2 inference products: Supported data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.<!-- end id14 --><br>

## Return Value

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section642mcpsimp"></a>

See [Example](SetFmatrix.md#section642mcpsimp)

The following shows a code snippet:

```cpp
// The Load3Dv2 instruction completes the img2col process. After img2col, the height of cube A is ho * wo. By substituting the convolution kernel width, convolution kernel sliding stride, convolution kernel dilation coefficient, and other parameters into the formulas for ho and wo, the height of cube A is CeilAlign(k, fractalShape[0]). After img2col, the width of cube A is ci * kh * kw. By substituting kh=1 and kw=1, the width of cube A is CeilAlign(m, fractalShape[1]). Finally, set loadDataParams.enTranspose = true to transpose the entire cube A and transpose each fractal within it.
// Use the load3d API to implement NZ2ZZ.
AscendC::LoadData3DParamsV2<T> loadDataParams;
// Set the parameters related to loadDataParams.
...
// Do not set the related attributes inside the Load3D API. Instead, set them explicitly by calling an external API.
static constexpr AscendC::IsResetLoad3dConfig LOAD3D_CONFIG = {false, false};

// Use the SetLoadDataRepeat API to set the repeat parameter of the Load3Dv2 API.
AscendC::SetLoadDataPaddingValue(0);

AscendC::LoadData<T, LOAD3D_CONFIG>(a2Local, a1Local, loadDataParams);
```
