# LoadDataWithStride<a id="ZH-CN_TOPIC_0000002659754907"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T12:10:19.775Z pushedAt=2026-10-26T07:41:44.101Z nl=2 -->

## Applicable Products<a id="section796754519912"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadDataWithStride_res.md#id1 -->

## Description<a id="section129811351452"></a>

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

This API essentially implements the same functionality as the [LoadData (convolution data movement)](./LoadData_3D.md) API. It is used to perform Image to Column expansion on a Feature Map in NC1HWC0 format, and then select a specified data block from the expanded two-dimensional matrix and transfer it to the corresponding memory location. The difference from the LoadData (convolution data movement) API is that this API supports only the Ascend 950PR&950DT products, supports configuring the output matrix K-axis offset **dstStride** and requires this parameter to be configured. Before calling it, you must use the auxiliary configuration API [SetLoadDataRepeatWithStride](../cube_load_aux_config/SetLoadDataRepeatWithStride.md) to configure the **dstStride** parameter. In addition, compared with LoadData (convolution data movement), this API does not internally include compatibility implementations for other chip versions, which reduces the extra overhead caused by compatibility and optimizes performance.

The data paths supported by LoadDataWithStride are: L1 Buffer->L0A Buffer and L1 Buffer->L0B Buffer.

## Prototype<a id="section82039854412"></a>

```cpp
template <typename T, const IsResetLoad3dConfig& defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG, typename U = PrimT<T>,typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void LoadDataWithStride(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2<U>& loadDataParams)
```

## Parameters<a id="section19594184564415"></a>

**Table 1** Template parameter description

| Parameter | Meaning |
| -------- | ---- |
| T | Data type of the source operand and the destination operand. For supported data types, see [Data Types](#section4219135304818). |
| defaultConfig | Controls whether related attributes are set inside the API. Type is **IsResetLoad3dConfig**. The **IsResetLoad3dConfig** structure is defined as follows:<br>struct IsResetLoad3dConfig {<br>   bool isSetFMatrix = true;<br>   bool isSetPadding = true;<br>};<br>When **isSetFMatrix** is set to true, the Feature Map attribute description (including **l1H**, **l1W**, and **padList**; see [Table 3](#table193501032193419) for parameter introduction) is set inside the API. When set to false, the Feature Map attribute description passed to this API does not take effect, and developers need to set it through [SetFmatrix](../cube_load_aux_config/SetFmatrix.md).<br>When **isSetPadding** is set to true, the Pad attribute description (that is, the **padValue** parameter; see [Table 3](#table193501032193419) for parameter introduction) is set inside the API. When set to false, the Pad attribute passed to this API does not take effect, and developers need to set it through [SetLoadDataPaddingValue](../cube_load_aux_config/SetLoadDataPaddingValue.md).<br>The default value of this parameter is as follows:<br>constexpr IsResetLoad3dConfig IS_RESER_LOAD3D_DEFAULT_CONFIG = {true, true};<br>For feature details, see [Feature Map and Pad attribute description register settings](./LoadData_3D.md#zh-cn_topic_0000002512171652_section1881795134015). |
| U | Data type of **padValue** in **LoadData3DParamsV2**.<br>When **dst** and **src** use basic data types, **U** must be consistent with the data type **T** of **dst** and **src**; otherwise, compilation fails.<br>The last template parameter is used only for the above data type check, and users do not need to pay attention to it. |

**Table 2** General parameter description

| Parameter | Input/Output | Meaning |
| -------- | --------- | ---- |
| dst | Output | Destination operand, whose type is **LocalTensor**. The continuous data arrangement order is determined by the physical storage location of the destination operand. The specific constraints are as follows:<br>&nbsp;&nbsp;&bull; L0A Buffer (TPosition: A2): NZ format.<br>&nbsp;&nbsp;&bull; L0B Buffer (TPosition: B2): ZN format. |
| src | Input | Source operand, whose type is **LocalTensor**. The continuous data arrangement order is determined by the physical storage location of the source operand: L1 Buffer (TPosition: A1/B1): NC1HWC0 format. |
| loadDataParams | Input | LoadData parameter structure, whose type is **LoadData3DParamsV2**. For details, see [Table 3](#table193501032193419). |

**Table 3** Parameter description in the LoadData3DParamsV2 structure<a id="table193501032193419"></a>  

| Parameter | Meaning |
| ---------- | ------ |
| padList | Padding list [padding_left, padding_right, padding_top, padding_bottom]. The value range of each element is [0, 255]. Default is {0, 0, 0, 0}. |
| l1H | Height of the source operand. Value range: l1H∈[0, 32767]. Default is 0.<br>**l1H = 0 indicates not executing the transfer, and this API will be considered as NOP (no operation).** |
| l1W | Width of the source operand. Value range: l1W∈[0, 32767]. Default is 0.<br>**l1W = 0 indicates not executing the transfer, and this API will be considered as NOP (no operation).** |
| channelSize | Number of channels of the source operand. Value range: channelSize∈[0, 65535]. Default is 0.<br>For uint32_t/int32_t/float, channelSize can take values 4, N\*8, N\*8+4; for half/bfloat16, channelSize can take values 4, 8, N\*16, N\*16 + 4, N\*16+8; for int8_t/uint8_t, channelSize can take values 4, 8, 16, 32\*N, N\*32+4, N\*32+8, N\*32+16; for int4b_t, channelSize can take values 8, 16, 32, N\*64, N\*64+8, N\*64+16, N\*64+32. N is a positive integer.<br>**Note: channelSize = 0 indicates not executing the transfer, and this API will be considered as NOP (no operation).** |
| kExtension | Transfer length of this instruction in the width dimension of the destination operand. If the rightmost fractal is not covered, for the b32 type, it should be a multiple of 8; for the b16 type, it should be a multiple of 16; for the b8 type, it should be a multiple of 32; for the b4 type, it should be a multiple of 64. In the covered case, no multiple requirement applies to any data type. Value range: kExtension∈[0, 65535]. Default is 0.<br>**Note: kExtension = 0 indicates not executing the transfer, and this API will be considered as NOP (no operation).** |
| mExtension | Transfer length of this instruction in the height dimension of the destination operand. If the bottommost fractal is not covered, for the b4, b8, and b16 types, it should be a multiple of 16; the b32 type has no requirement. In the covered case, no multiple requirement applies to any data type. Value range: mExtension∈[0, 65535]. Default is 0.<br>**Note: mExtension = 0 indicates not executing the transfer, and this API will be considered as NOP (no operation).** |
| kStartPt | Starting point of this instruction in the width dimension of the destination operand. For the b32 type, it should be a multiple of 8; for the b16 type, it should be a multiple of 16; for the b8 type, it should be a multiple of 32; for the b4 type, it should be a multiple of 64. Value range: [0, 65535]. Default is 0. |
| mStartPt | Starting point of this instruction in the height dimension of the destination operand. Value range: [0, 32767]. Default is 0. |
| strideW | Stride of the convolution kernel sliding in the width dimension of the source operand. Value range: strideW∈[0, 63]. Default is 1. |
| strideH | Stride of the convolution kernel sliding in the height dimension of the source operand. Value range: strideH∈[0, 63]. Default is 1. |
| filterW | Width of the convolution kernel. Value range: filterW∈[0, 255]. Default is 1.<br>**Note: When filterW=0 and filterSizeW=false, it indicates not executing the transfer, and this API will be considered as NOP (no operation).** |
| filterH | Height of the convolution kernel. Value range: filterH∈[0, 255]. Default is 1.<br>**Note: When filterH=0 and filterSizeH=false, it indicates not executing the transfer, and this API will be considered as NOP (no operation).** |
| dilationFilterW | Dilation factor of the convolution kernel width. Value range: dilationFilterW∈[0, 255]. Default is 1. |
| dilationFilterH | Dilation factor of the convolution kernel height. Value range: dilationFilterH∈[0, 255]. Default is 1. |
| enTranspose | Whether to enable the transpose function to transpose the entire target matrix. The supported data type is bool. Default is false.<br>&nbsp;&nbsp;&bull; true: Enable<br>&nbsp;&nbsp;&bull; false: Disable<br>**Note: This parameter is valid only when the physical storage location of the destination operand is L0A Buffer (TPosition: A2) and the source operand is of the b8/b16/b32 type. This parameter is invalid when the physical storage location of the destination operand is L0B Buffer (TPosition: B2).** |
| enSmallK | Whether to enable the small k feature, where each fractal matrix is 16*4 in size. The supported data type is bool. Default is false. In the current product form, this feature is no longer supported. |
| padValue | Value of the Pad padding value. The data type must be consistent with src. If padding is not required, set padList to all 0s. Default is 0. |
| filterSizeW | Whether to increase the convolution kernel width by 256 elements on the basis of filterW. true: increase; false: do not increase. Default is false. |
| filterSizeH | Whether to increase the convolution kernel height by 256 elements on the basis of filterH. true: increase; false: do not increase. Default is false. |
| fMatrixCtrl | Indicates whether the LoadData (convolution data movement) v2 instruction obtains the Feature Map attribute description from the left matrix or the right matrix. It is used together with SetFmatrix. The default value is false. Currently, only false is supported.<br>&nbsp;&nbsp;&bull; true: Obtain the Feature Map attribute description from the right matrix.<br>&nbsp;&nbsp;&bull; false: Obtain the Feature Map attribute description from the left matrix. |

## Data Type<a id="section4219135304818"></a>

- When the data path is L1 Buffer(TPosition::A1)->L0A Buffer(TPosition::A2), the supported data types are: int8_t, uint8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t, int32_t, uint32_t, float.

- When the data path is L1 Buffer(TPosition::B1)->L0B Buffer(TPosition::B2), the supported data types are: half, bfloat16_t, int32_t, uint32_t, float.

## Return Value<a id="section4219135304919"></a>

None

## Constraints<a id="section2045914466492"></a>

- In the non-transpose scenario, the L1 Buffer-\>L0B Buffer path is not supported. In the L1 Buffer-\>L0B Buffer path, transposition is performed automatically, so **enTranspose** does not need to be configured and is invalid in this case.
- When the destination address is in the L0A Buffer/L0B Buffer, the address must be 512-byte aligned. When the source address is in the L1 Buffer, the address must be 32-byte aligned.
- The auxiliary configuration API [SetLoadDataRepeatWithStride](../cube_load_aux_config/SetLoadDataRepeatWithStride.md) must be used to configure the **dstStride** parameter.

## Example<a id="section14867162784812"></a>

This API must be used together with the SetLoadDataRepeatWithStride API. The following shows a code snippet:

```cpp
// The LoadDataWithStride instruction completes the img2col process. After img2col, the height of matrix A is ho * wo. According to the calculation formulas of ho and wo, substituting parameters such as the convolution kernel width, convolution kernel sliding stride, and convolution kernel dilation coefficient, the height of matrix A is CeilAlign(k, fractalShape[0]). After img2col, the width of matrix A is ci * kh * kw. Substituting kh=1 and kw=1, the width of matrix A is CeilAlign(m, fractalShape[1]). Finally, set loadDataParams.enTranspose = true to transpose the entire matrix A and transpose each fractal within it.
AscendC::LoadData3DParamsV2<T> loadDataParams;

// Set the parameters related to loadDataParams.
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
