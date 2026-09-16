# LoadData (Convolution Data Transfer)<a id="ZH-CN_TOPIC_0000002538071224"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-09-01T06:53:53.648Z -->

## Product Support<a id="zh-cn_topic_0000002512171652_section796754519912"></a>

### Load3Dv1 API

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->
### Load3Dv2 and Load3Dv2Pro APIs

<!-- npu="950" id10 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas inference products: Supported
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products AI Core: Supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products Vector Core: Not supported
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas training products: Not supported
<!-- end id16 -->
## Function Description<a id="zh-cn_topic_0000002512171652_section106841136114319"></a>

The header file path is: `"basic_api/kernel_operator_mm_intf.h"`.

Load3D is essentially used to perform Image to Column expansion on the Feature Map in NC1HWC0 format, and then select a specified data block from the expanded two-dimensional cube and move it to the corresponding memory location. See Figure 1.

Because Load3D moves data in units of 512-byte data fractals during the transfer process, Load3D can also be used to move 2D format data required for ordinary cube computation. For how to use Load3D to move 2D format data, see [Key Feature Description](#zh-cn_topic_0000002512171652_section102629572045).

Load3D supports only the following data paths: L1 Buffer->L0A Buffer and L1 Buffer->L0B Buffer.

For the implementation principle, see the pseudocode: [Load3D Pseudocode](https://gitcode.com/cann/asc-devkit/blob/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0/scripts/load3d.py).

**Figure 1** Load3D to L0A Buffer function diagram<a id="zh-cn_topic_0000002512171652_fig54450833715"></a>  

![](../../../../figures/load3d_l1tol0a_demo.png "Load3D-to-L0A Buffer function diagram")

## Function Prototype<a id="zh-cn_topic_0000002512171652_section82039854412"></a>

**Load3Dv1 API:**

```cpp
template <typename T, const IsResetLoad3dConfig &defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG, typename U = PrimT<T>, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV1<U>& loadDataParams)
```

**Load3Dv2 API:**

```cpp
template <typename T, const IsResetLoad3dConfig& defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG, typename U = PrimT<T>, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2<U>& loadDataParams)
```

**Load3Dv2Pro API:**

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2Pro& loadDataParams)
```

## Parameter Description<a id="zh-cn_topic_0000002512171652_section16128134420472"></a>

**Table 1** Template parameter description

| Parameter Name | Description |
| ---------- | ------ |
| T | Data type of the source operand and destination operand. For supported data types, see [Data Type](#zh-cn_topic_0000002512171652_section4219135304818). |
| defaultConfig | Controls whether related attributes are set inside the Load3Dv1/Load3Dv2 API. Type: IsResetLoad3dConfig. The IsResetLoad3dConfig structure is defined as follows:<br>struct IsResetLoad3dConfig {<br>   bool isSetFCube = true;<br>   bool isSetPadding = true;<br>};<br>When isSetFCube is set to true, the attribute description of FeatureMap (including l1H, l1W, and padList; for parameter introduction, see [Table 3](#zh-cn_topic_0000002512171652_table679014222918) and [Table 4](#zh-cn_topic_0000002512171652_table193501032193419)) is set inside the API. When it is set to false, the attribute description of FeatureMap passed to this API does not take effect, and developers need to set it through SetFcube.<br>When isSetPadding is set to true, the Pad attribute description (that is, the padValue parameter; for parameter introduction, see [Table 3](#zh-cn_topic_0000002512171652_table679014222918) and [Table 4](#zh-cn_topic_0000002512171652_table193501032193419)) is set inside the API. When it is set to false, the Pad attribute passed to this API does not take effect, and developers need to set it through SetLoadDataPaddingValue. For details, see the SetFcube call example.<br>The default value of this parameter is as follows:<br>constexpr IsResetLoad3dConfig IS_RESER_LOAD3D_DEFAULT_CONFIG = {true, true};<br>For feature details, see [Feature Map and Pad Attribute Description Register Settings](#zh-cn_topic_0000002512171652_section1881795134015). |
| U | Data type of padValue in LoadData3DParamsV1/LoadData3DParamsV2.<br>&nbsp;&nbsp;&bull;When dst and src use basic data types, U must be consistent with the data type T of dst and src; otherwise, compilation fails.<br>&nbsp;&nbsp;&bull;When dst and src use the TensorTrait type, the LiteType of U and the data type T of dst and src must be consistent; otherwise, compilation fails.<br>The last template parameter is used only for the preceding data type check, and users do not need to pay attention to it. |

**Table 2** Common parameter description

| Parameter Name | Input/Output | Description |
| ---------- | ----------- | ------ |
| dst | Output | Destination operand, of the LocalTensor type.<br>The contiguous arrangement order of data is determined by the physical storage location of the destination operand. For format constraints of different product models, see [Constraint Description](#zh-cn_topic_0000002512171652_dst_layout). |
| src | Input | Source operand, of the LocalTensor type.<br>The contiguous arrangement order of data is determined by the physical storage location of the destination operand:<br>&nbsp;&nbsp;&bull; L1 Buffer (TPosition: A1/B1): NC1HWC0 format. |
| loadDataParams | Input | LoadData parameter structure, of the following types:<br>&nbsp;&nbsp;&bull; LoadData3DParamsV1. For details, see [Table 3](#zh-cn_topic_0000002512171652_table679014222918).<br>&nbsp;&nbsp;&bull; LoadData3DParamsV2. For details, see [Table 4](#zh-cn_topic_0000002512171652_table193501032193419).<br>&nbsp;&nbsp;&bull; LoadData3DParamsV2Pro. For details, see [Table 5](#zh-cn_topic_0000002512171652_table118027314415).<br>For the definitions of the preceding structure parameters, see \$\{INSTALL\_DIR\}/include/ascendc/basic\_api/API/kernel\_struct\_mm.h. Replace \$\{INSTALL\_DIR\} with the file storage path after the CANN software is installed. |


**Table 3** Parameter description of the LoadData3DParamsV1 structure<a id="zh-cn_topic_0000002512171652_table679014222918"></a>

| Parameter Name | Description |
| ---------- | ------ |
| padList | Padding list [padding_left, padding_right, padding_top, padding_bottom]. Value range of each element: [0, 255]. Default value: {0, 0, 0, 0}. |
| l1H | Height of the source operand. Value range: l1H∈[1, 32767]. |
| l1W | Width of the source operand. Value range: l1W∈[1, 32767]. |
| c1Index | Start point of this instruction in the C1 dimension of the source tensor. Value range: c1Index∈[0, 4095]. Default value: 0. |
| fetchFilterW | Start position of this instruction in the w dimension of the convolution kernel. Value range: fetchFilterW∈[0, 254]. Default value: 0. |
| fetchFilterH | Start position of this instruction in the h dimension of the filter. Value range: fetchFilterH∈[0, 254]. Default value: 0. |
| leftTopW | Start point of this instruction in the w dimension of the source operand. Value range: leftTopW∈[-255, 32767]. Default value: 0. If padding_left = a, leftTopW is set to -a. |
| leftTopH | Start point of this instruction in the h dimension of the source operand. Value range: leftTopH∈[-255, 32767]. Default value: 0. If padding_top = a, leftTopH is set to -a. |
| strideW | Step by which the convolution kernel slides in the w dimension of the source operand. Value range: strideW∈[1, 63]. |
| strideH | Step by which the convolution kernel slides in the h dimension of the source operand. Value range: strideH∈[1, 63]. |
| filterW | Width of the convolution kernel. Value range: filterW∈[1, 255]. |
| filterH | Height of the convolution kernel. Value range: filterH∈[1, 255]. |
| dilationFilterW | Dilation coefficient of the convolution kernel width. Value range: dilationFilterW∈[1, 255]. |
| dilationFilterH | Dilation coefficient of the convolution kernel height. Value range: dilationFilterH∈[1, 255]. |
| jumpStride | Step of the start address of the destination operand between iterations. Value range: jumpStride∈[1, 127]. |
| repeatMode | Iteration mode.<br>&nbsp;&nbsp;&bull;Mode 0: In each iteration, the point in the convolution kernel window is increased, corresponding to growth in the w dimension direction on the destination cube.<br>&nbsp;&nbsp;&bull;Mode 1: In each iteration, the upper-left coordinate of the sliding window is increased, corresponding to growth in the h dimension direction on the destination cube.<br>Value range: repeatMode∈[0, 1]. Default value: 0. |
| repeatTime | Number of iterations. In each iteration, the addresses of the source operand and destination operand change. Value range: repeatTime∈[1, 255]. |
| cSize | Configures whether to enable the cSize = 4 (b16) / cSize = 8 (b8) optimization. Value range: cSize∈[0, 1]. Default value: 0. |
| padValue | Value of the Pad padding. The data type must be consistent with src. Default value: 0. If padding is not required, set padList to all zeros. |


**Table 4** Parameter description of the LoadData3DParamsV2 structure<a id="zh-cn_topic_0000002512171652_table193501032193419"></a>

| Parameter Name | Description |
| ---------- | ------ |
| padList | Padding list [padding_left, padding_right, padding_top, padding_bottom]. Value range of each element: [0, 255]. Default value: {0, 0, 0, 0}. |
| l1H | Height of the source operand. Value range: l1H∈[0, 32767]. Default value: 0.<br>**l1H = 0 indicates that no data movement is performed, and this API is treated as a NOP (no operation).** |
| l1W | Width of the source operand. Value range: l1W∈[0, 32767]. Default value: 0.<br>**l1W = 0 indicates that no data movement is performed, and this API is treated as a NOP (no operation).** |
| channelSize | Number of channels of the source operand. Value range: channelSize∈[0, 65535]. Default value: 0. Different product models support different channelSize values. For details, see [Constraint Description](#zh-cn_topic_0000002512171652_channelsize_constraint).<br>**Note: channelSize = 0 indicates that no data movement is performed, and this API is treated as a NOP (no operation).** |
| kExtension | Transfer length of this instruction in the width dimension of the destination operand. If the rightmost fractal is not covered, it must be a multiple of 8 for the b32 type, a multiple of 16 for the b16 type, a multiple of 32 for the b8 type, and a multiple of 64 for the b4 type. If the rightmost fractal is covered, no multiple requirement applies to any data type. Value range: kExtension∈[0, 65535]. Default value: 0.<br>**Note: kExtension = 0 indicates that no data movement is performed, and this API is treated as a NOP (no operation).** |
| mExtension | Transfer length of this instruction in the height dimension of the destination operand. If the bottommost fractal is not covered, it must be a multiple of 16 for the b4, b8, and b16 types, and no requirement applies to the b32 type. If the bottommost fractal is covered, no multiple requirement applies to any data type. Value range: mExtension∈[0, 65535]. Default value: 0.<br>**Note: mExtension = 0 indicates that no data movement is performed, and this API is treated as a NOP (no operation).** |
| kStartPt | Start point of this instruction in the width dimension of the destination operand. It must be a multiple of 8 for the b32 type, a multiple of 16 for the b16 type, a multiple of 32 for the b8 type, and a multiple of 64 for the b4 type. Value range: [0, 65535]. Default value: 0. |
| mStartPt | Start point of this instruction in the height dimension of the destination operand. Value range: [0, 32767]. Default value: 0. |
| strideW | Step by which the convolution kernel slides in the width dimension of the source operand. Value range: strideW∈[0, 63]. Default value: 1. |
| strideH | Step by which the convolution kernel slides in the height dimension of the source operand. Value range: strideH∈[0, 63]. Default value: 1. |
| filterW | Width of the convolution kernel. Value range: filterW∈[0, 255]. Default value: 1.<br>**Note: filterW=0 and filterSizeW=false indicate that no data movement is performed, and this API is treated as a NOP (no operation).** |
| filterH | Height of the convolution kernel. Value range: filterH∈[0, 255]. Default value: 1.<br>**Note: filterH=0 and filterSizeH=false indicate that no data movement is performed, and this API is treated as a NOP (no operation).** |
| dilationFilterW | Dilation coefficient of the convolution kernel width. Value range: dilationFilterW∈[0, 255]. Default value: 1. |
| dilationFilterH | Dilation coefficient of the convolution kernel height. Value range: dilationFilterH∈[0, 255]. Default value: 1. |
| enTranspose | Whether to enable the transpose function to transpose the entire destination cube. Supported data type: bool. Default value: false.<br>&nbsp;&nbsp;&bull; true: enabled<br>&nbsp;&nbsp;&bull; false: disabled<br>The valid conditions of enTranspose vary by product model. For details, see [Constraint Description](#zh-cn_topic_0000002512171652_entranspose_constraint).<br>**Note: This parameter is invalid when the physical storage location of the destination operand is L0B Buffer (TPosition: B2).** |
| enSmallK | Whether to enable the small k feature. The size of each fractal cube is 16*4. Supported data type: bool. Default value: false. In the current product form, this feature is no longer supported. |
| padValue | Value of the Pad padding. The data type must be consistent with src. If padding is not required, set padList to all zeros. Default value: 0. |
| filterSizeW | Whether to increase the convolution kernel width by 256 elements based on filterW. true: increase; false: do not increase. Default value: false. |
| filterSizeH | Whether to increase the convolution kernel height by 256 elements based on filterH. true: increase; false: do not increase. Default value: false. |
| fCubeCtrl | Indicates whether the Load3DV2 instruction obtains the attribute description of FeatureMap from the left cube or the right cube. It is used together with SetFcube. Default value: false. Currently, only false is supported.<br>&nbsp;&nbsp;&bull; true: obtain the attribute description of FeatureMap from the right cube.<br>&nbsp;&nbsp;&bull; false: obtain the attribute description of FeatureMap from the left cube. |

**Table 5** Parameter description of the LoadData3DParamsV2Pro structure<a id="zh-cn_topic_0000002512171652_table118027314415"></a>

| Parameter Name | Description |
| ---------- | ------ |
| channelSize | Number of channels of the source operand. Value range: channelSize∈[0, 65535].<br>For half, the remainder of channelSize divided by 16 must be 0, 4, or 8.<br>For int8_t and uint8_t, the remainder of channelSize divided by 32 must be 0, 4, 8, or 16.<br>For int4b_t, channelSize is 8, 16, 32, N\*64, N\*64+8, N\*64+16, or N\*64+32, where N is a positive integer. |
| enTranspose | Whether to enable the transpose function, which transposes the entire destination cube. The supported data type is bool. It takes effect only when the destination TPosition is A2 and the source operand is of the half type. The default value is false.<br>&nbsp;&nbsp;&bull; true: enabled;<br>&nbsp;&nbsp;&bull; false: disabled. |
| enSmallK | Whether to enable the small k feature, where each fractal cube is 16\*4 in size. The supported data type is bool. The default value is false.<br>**Note: This feature is no longer supported in the current product form.** |
| filterSizeW | Whether to increase the convolution kernel width by 256 elements on the basis of filterW. true: increase; false: do not increase. |
| filterSizeH | Whether to increase the convolution kernel height by 256 elements on the basis of filterH. true: increase; false: do not increase. |
| fCubeCtrl | Indicates whether the Load3DV2 instruction obtains the attribute description of the FeatureMap from the left cube or the right cube. It is used together with SetFcube. Currently, only false is supported. The default value is false.<br>&nbsp;&nbsp;&bull; true: obtain the attribute description of the FeatureMap from the right cube;<br>&nbsp;&nbsp;&bull; false: obtain the attribute description of the FeatureMap from the left cube. |
| extConfig | Combined parameter (uint64_t type). The default value is 0.<br>extConfig = ((uint64_t)mStartPt << 48)<br>    &#124; ((uint64_t)kStartPt << 32)<br>    &#124; ((uint64_t)mExtension << 16)<br>    &#124; (uint64_t)kExtension;<br>|
| filterConfig | Combined parameter (uint64_t type). The default value is 0X10101010101.<br>filterConfig = ((uint64_t)dilationFilterH << 40)<br>    &#124; ((uint64_t)dilationFilterW << 32)<br>    &#124; ((uint64_t)filterH << 24)<br>    &#124; ((uint64_t)filterW << 16)<br>    &#124; ((uint64_t)strideH << 8)<br>    &#124; (uint64_t)strideW;<br>|

## Data Type<a id="zh-cn_topic_0000002512171652_section4219135304818"></a>

<!-- npu="310p,910" id19 -->
**Load3Dv1 API:**

<!-- npu="310p" id20 -->
- Atlas inference products AI Core, supported data types: int8_t, uint8_t, half.
<!-- end id20 -->

<!-- npu="910" id21 -->
- Atlas training products, supported data types: int8_t, uint8_t, half.
<!-- end id21 -->
<!-- end id19 -->

<!-- npu="950,A3,910b,310b,310p" id22 -->
**Load3Dv2 API and Load3Dv2Pro API:**

<!-- npu="950" id23 -->
- Ascend 950PR/Ascend 950DT, supported data types: int8_t, uint8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id23 -->

<!-- npu="A3" id24 -->
- Atlas A3 training products/Atlas A3 inference products:
    - When TPosition is A1/A2, supported data types: int4b_t, int8_t, uint8_t, half, bfloat16_t, int32_t, uint32_t, float.
    - When TPosition is B1/B2, supported data types: half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id24 -->

<!-- npu="910b" id25 -->
- Atlas A2 training products/Atlas A2 inference products:
    - When TPosition is A1/A2, the supported data types are: int4b_t, int8_t, uint8_t, half, bfloat16_t, int32_t, uint32_t, float.
    - When TPosition is B1/B2, the supported data types are: half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id25 -->

<!-- npu="310b" id26 -->
- Atlas inference products:
    - When TPosition is A1/A2, the supported data types are: int4b_t, int8_t, uint8_t, half, bfloat16_t, int32_t, uint32_t, float.
    - When TPosition is B1/B2, the supported data types are: half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id26 -->

<!-- npu="310p" id27 -->
- For the AI Core of Atlas inference products, the supported data types are: int4b_t, int8_t, uint8_t, half.
<!-- end id27 -->

<!-- end id22 -->

## Return Value Description<a id="zh-cn_topic_0000002512171652_section640mcpsimp"></a>

None

## Constraint Description<a id="zh-cn_topic_0000002512171652_section2045914466492"></a>

- In the non-transpose scenario, the L1 Buffer->L0B Buffer path is not supported. In the L1 Buffer->L0B Buffer path, transpose is performed automatically, and there is no need to configure enTranspose. In this case, the enTranspose parameter is invalid.

<!-- npu="950" id29 -->
- For Ascend 950PR/Ascend 950DT: You must use the auxiliary configuration API SetLoadDataRepeat to configure the dstStride parameter. If the repeat mode is not enabled, the repeat-related parameters in the API use the default constructed values.
<!-- end id29 -->

<!-- npu="A3,910b" id30 -->
- For the following product models:

    <!-- npu="A3" id31 -->
    Atlas A3 training products/Atlas A3 inference products;
    <!-- end id31 -->
    <!-- npu="910b" id32 -->
    Atlas A2 training products/Atlas A2 inference products;
    <!-- end id32 -->

    The L1 Buffer->L0B Buffer path does not support b4/b8 data. In the transpose scenario, the L1 Buffer->L0A Buffer path does not support the b8 data type.
<!-- end id30 -->

- When any one of l1H/l1W/channelSize/kExtension/mExtension is 0, this instruction is not executed.
- When filterW is 0 and filterSizeW is false, or when filterH is 0 and filterSizeH is false, this instruction is not executed.
- When the destination address is in L0A Buffer/L0B Buffer, the address must be 512-byte aligned. When the source address is in L1 Buffer, the address must be 32-byte aligned. The pipeline occupied by instruction execution is PIPE_MTE1.
<!-- npu="910,310p" id33 -->
- To enable the cSize feature of LoadData3DParamsV1, ensure that the feature map in A1/B1 is 4-channel aligned.
<!-- end id33 -->

- The data contiguous arrangement order is determined by the physical storage location of the destination operand. The specific constraints are as follows: <a id="zh-cn_topic_0000002512171652_dst_layout"></a>

    <!-- npu="A3,910b" id34 -->
    - For Atlas A3 training products/Atlas A3 inference products and Atlas A2 training products/Atlas A2 inference products:
        - L0A Buffer (TPosition: A2): ZZ format;
        - L0B Buffer (TPosition: B2): ZN format.
    <!-- end id34 -->

    <!-- npu="950" id35 -->
    - For Ascend 950PR/Ascend 950DT:
        - L0A Buffer (TPosition: A2): NZ format;
        - L0B Buffer (TPosition: B2): ZN format.
    <!-- end id35 -->

- The value range constraints for channelSize in the LoadData3DParamsV2 structure are as follows: <a id="zh-cn_topic_0000002512171652_channelsize_constraint"></a>

    <!-- npu="310p" id36 -->
    - For Atlas inference products AI Core: for half, channelSize can be 4, 8, 16, N\*16+4, N\*16+8; for int8_t/uint8_t, channelSize can be 4, 8, 16, 32, N\*32+4, N\*32+8, N\*32+16; for int4b_t, channelSize can be 8, 16, 32, N\*64, N\*64+8, N\*64+16, N\*64+32. N is a positive integer.
    <!-- end id36 -->

    <!-- npu="950,A3,910b,310b" id37 -->
    - For Ascend 950PR/Ascend 950DT, Atlas A3 training products/Atlas A3 inference products, Atlas A2 training products/Atlas A2 inference products, and Atlas inference products: for uint32_t/int32_t/float, channelSize can be 4, N\*8, or N\*8+4; for half/bfloat16, channelSize can be 4, 8, N\*16, N\*16+4, or N\*16+8; for int8_t/uint8_t, channelSize can be 4, 8, 16, 32\*N, N\*32+4, N\*32+8, or N\*32+16; for int4b_t, channelSize can be 8, 16, 32, N\*64, N\*64+8, N\*64+16, or N\*64+32. N is a positive integer.
    <!-- end id37 -->

- The valid conditions for enTranspose in the LoadData3DParamsV2 structure are as follows: <a id="zh-cn_topic_0000002512171652_entranspose_constraint"></a>

    <!-- npu="A3,910b" id38 -->
    - For Atlas A3 training products/Atlas A3 inference products and Atlas A2 training products/Atlas A2 inference products: valid when the physical storage location of the destination operand is L0A Buffer (TPosition: A2) and the source operand is of type b16/b32.
    <!-- end id38 -->

    <!-- npu="950" id39 -->
    - For Ascend 950PR/Ascend 950DT: valid when the physical storage location of the destination operand is L0A Buffer (TPosition: A2) and the source operand is of type b8/b16/b32.
    <!-- end id39 -->

## Key Feature Description<a id="zh-cn_topic_0000002512171652_section102629572045"></a>

### Using Load3D to Transfer 2D Format Data Required for Cube Computation<a id="zh-cn_topic_0000002512171652_section86392366416"></a>

For data in NC1HWC0 format, when N is 1, the convolution kernel width and height are 1, padding is 0, the convolution kernel slides with a stride of 1 in both the width and height dimensions of the source operand, and the dilation coefficients of the convolution kernel width and height are 1, the data layout after image to column expansion can be regarded as the NZ fractal layout on the L1 Buffer during cube computation. Therefore, in this scenario, the Load3D API can be used to transfer the 2D format data required for cube computation. The following figure shows the schematic diagram:

![](../../../../figures/load3d_3d22d_demo.png)

#### Non-transpose Scenario

First, the L1 Buffer -> L0B Buffer path does not support the non-transpose scenario. For the L1 Buffer -> L0A Buffer path, different fractal arrangement conversions are performed depending on the product form:

<!-- npu="A3,910b" id40 -->
- For Atlas A3 training products/Atlas A3 inference products and Atlas A2 training products/Atlas A2 inference products:

    Load3D automatically completes the fractal conversion, converting the NZ fractal arrangement on the L1 Buffer into the ZZ fractal arrangement required in the L0A Buffer, as shown in the following diagram for the b16 type scenario:

    ![](../../../../figures/load3d_l1tol0a_b16_nontrans.png)
<!-- end id40 -->

<!-- npu="950" id41 -->
- For Ascend 950PR/Ascend 950DT:

    The fractal arrangement on the L1 Buffer is exactly the NZ fractal arrangement required in the L0A Buffer, and Load3D does not perform fractal arrangement conversion, as shown in the following diagram for the b16 type scenario:

    ![](../../../../figures/load3d_l1tol0a_b16_nontrans_950.png)
<!-- end id41 -->

#### Transpose Scenario

<!-- npu="A3,910b" id42 -->
- For Atlas A3 training products/Atlas A3 inference products and Atlas A2 training products/Atlas A2 inference products:

    Load3D supports transpose (enTranspose=True) only for the b16/b32 data types. The details by type are as follows:

    - For the b16 scenario: each fractal size is 16×16.

        On the L1 Buffer->L0A Buffer path, in the transpose scenario, in addition to each fractal cube itself being transposed, the position of each fractal cube in the entire 2D feature map is also transposed. The transpose diagram is as follows:

        ![](../../../../figures/load3d_l1tol0a_b16_trans.png)

        On the L1 Buffer->L0B Buffer path, the Load3D API automatically completes the transpose, so there is no need to configure the enTranspose parameter, which is invalid in this case. The transpose diagram is as follows:

        ![](../../../../figures/load3d_l1tol0b_b16_trans.png)

    - For the b32 scenario: each fractal size is 16×8.

        On the L1 Buffer->L0A Buffer path, you need to configure the enTranspose parameter to enable the transpose function. The transpose diagram is as follows:

        ![](../../../../figures/load3d_l1tol0a_b32_trans.png)

        On the L1 Buffer->L0B Buffer path, the Load3D API automatically completes the transpose, so there is no need to configure the enTranspose parameter, which is invalid in this case. The transpose diagram is as follows:

        ![](../../../../figures/load3d_l1tol0b_b32_trans.png)
<!-- end id42 -->

<!-- npu="950" id43 -->
- For Ascend 950PR/Ascend 950DT:

    Load3D supports transpose for the b8/b16/b32 data types. On the L1 Buffer->L0A Buffer path, transpose is enabled by setting the enTranspose parameter. On the L1 Buffer->L0B Buffer path, transpose is completed automatically, so there is no need to configure the enTranspose parameter, which is invalid in this case. The details are described by type below.

    - For the b8 scenario: each fractal size is 16×32.

        On the L1 Buffer->L0A Buffer path, in the transpose scenario, two consecutive 16×32 fractals are concatenated into a 32×32 block cube, which is then transposed and split into two 16×32 fractals. The transpose diagram is as follows:

        ![](../../../../figures/load3d_l1tol0a_b8_trans_950.png)

        On the L1 Buffer->L0B Buffer path, the Load3D API automatically completes the transpose, so there is no need to configure the enTranspose parameter, which is invalid in this case. The transpose diagram is as follows:

        ![](../../../../figures/load3d_l1tol0b_b8_trans_950.png)

    - For the b16 scenario: each fractal size is 16×16.

        On the L1 Buffer->L0A Buffer path, in the transpose scenario, in addition to each fractal cube itself being transposed, the position of each fractal cube in the entire 2D feature map is also transposed. The transpose diagram is as follows:

        ![](../../../../figures/load3d_l1tol0a_b16_trans_950.png)

        On the L1 Buffer->L0B Buffer path, the Load3D API automatically completes the transpose, so there is no need to configure the enTranspose parameter, which is invalid in this case. The transpose diagram is as follows:

        ![](../../../../figures/load3d_l1tol0b_b16_trans_950.png)

    - For the b32 scenario: each fractal size is 16×8.

        On the L1 Buffer->L0A Buffer path, the transpose scenario diagram is as follows:

        ![](../../../../figures/load3d_l1tol0a_b32_trans_950.png)

        On the L1 Buffer->L0B Buffer path, the Load3D API automatically completes the transpose, so there is no need to configure the enTranspose parameter, which is invalid in this case. The transpose diagram is as follows:

        ![](../../../../figures/load3d_l1tol0b_b32_trans_950.png)
<!-- end id43 -->

### Repeat Mode<a id="zh-cn_topic_0000002512171652_section131671145123912"></a>

Load3D can complete data block transfer in multiple directions by configuring mExtension and kExtension. We regard the transfer of multiple data blocks here as one iteration. Load3D supports repeat transfer in the M direction (height) or the K direction (width) by configuring the repeat mode. That is, calling the Load3D API once can complete data transfer of multiple iterations, as shown in the following figure. The repeatStride, repeatTime, and the direction along which to repeat are configured through [SetLoadDataRepeat](../cube_load_aux_config/SetLoadDataRepeat.md).

<!-- npu="A3,910b" id44 -->
For the following product models:

<!-- npu="A3" id45 -->
Atlas A3 training products/Atlas A3 inference products;
<!-- end id45 -->

<!-- npu="910b" id46 -->
Atlas A2 training products/Atlas A2 inference products;
<!-- end id46 -->

Schematic diagram of repeat in the M direction:

![](../../../../figures/load3d_repeatmode.png)

It should be noted that when we call the Load3D instruction to perform repeat transfer in the M direction, if we enable transpose, all block matrices in the repetition process will be regarded as a whole large cube and processed according to the transpose scenario described above. Taking the b32 scenario as an example, the following figure shows the transpose schematic diagram when performing repeat transfer in the M direction, where repeatMode=0, repeatStride=2, and repeatTime=3.

![](../../../../figures/load3d_repeatmode_m.png)

Schematic diagram of repeat in the K direction:

![](../../../../figures/load3d_repeatmode_k.png)

When we call the Load3D instruction to perform repeat transfer in the K direction, if transpose is enabled, all block matrices in the repetition process are also treated as a whole large cube and processed according to the transpose scenario described above. Taking the b32 scenario as an example, the following figure shows the transpose diagram for repeat transfer in the K direction, where repeatMode=1, repeatStride=2, repeatTime=3.

![](../../../../figures/load3d_repeatmode_k_trans.png)
<!-- end id44 -->

<!-- npu="950" id47 -->
For Ascend 950PR/Ascend 950DT:

**M-direction repeat diagram:**

![](../../../../figures/load3d_repeatmode_m_950.png)

When we call the Load3D instruction to perform repeat transfer in the M direction, if transpose is enabled, all block matrices in the repetition process are treated as a whole large cube and processed according to the transpose scenario described above. Taking the b32 scenario as an example, the following figure shows the transpose diagram for repeat transfer in the M direction, where repeatMode=0, repeatStride=2, repeatTime=3, dstStride=3.

![](../../../../figures/load3d_repeatmode_m_trans_950.png)

**K-direction repeat diagram:**

![](../../../../figures/load3d_repeatmode_k_950.png)

When we call the Load3D instruction to perform repeat transfer in the K direction, if we enable transpose, all block matrices in the repetition process will also be treated as a whole large cube and processed according to the transpose scenario described earlier. Taking the b32 scenario as an example, the following figure shows the transpose diagram when performing repeat transfer in the K direction, where repeatMode=1, repeatStride=2, repeatTime=3, and dstStride=3.

![](../../../../figures/load3d_repeatmode_k_trans_950.png)
<!-- end id47 -->

### Feature Map and Pad Attribute Description Register Settings<a id="zh-cn_topic_0000002512171652_section1881795134015"></a>

Load3D has two special registers: the Feature Map attribute description register and the Pad attribute description register, which serve as parameters for Image to Column expansion. When performing cube computation, for high-performance data movement, we often move a large block of data from GM to the L1 Buffer, that is, cache a relatively large block of data in the L1 Buffer, and then use a for loop to move data from the L1 Buffer to the L0 Buffer and perform Mmad computation. When moving data from the L1 Buffer to the L0 Buffer through Load3D, we usually only need to set the Feature Map attribute and Pad attribute in Load3D once, without setting them repeatedly. Therefore, we provide the IsResetLoad3dConfig template parameter to manually manage these two attributes, reducing the instruction performance overhead caused by repeated settings.

The IsResetLoad3dConfig structure is defined as follows:

```cpp
struct IsResetLoad3dConfig {
   bool isSetFCube = true;
   bool isSetPadding = true;
}; 
```

When isSetFCube is set to true, it indicates that the FeatureMap attribute description (including l1H, l1W, and padList; for parameter introduction, see [Table 4](#zh-cn_topic_0000002512171652_table193501032193419)) is set inside the Load3D API. When it is set to false, it indicates that the FeatureMap attribute description passed to this API does not take effect, and developers need to manually manage the settings through [SetFcube](../cube_load_aux_config/SetFmatrix.md).

When isSetPadding is set to true, it indicates that the Pad attribute description (that is, the padValue parameter; for parameter introduction, see [Table 4](#zh-cn_topic_0000002512171652_table193501032193419)) is set inside the API. When it is set to false, it indicates that the Pad attribute passed to this API does not take effect, and developers need to set it through [SetLoadDataPaddingValue](../cube_load_aux_config/SetLoadDataPaddingValue.md).

### Load3D Data Format Description<a id="zh-cn_topic_0000002512171652_section726316123184"></a>

The input feature map and filter must be in the [NC1HWC0](../cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md#zh-cn_topic_0000002545089965_section217615301084) format, where C0 is the lowest dimension and is fixed at 16 (32 for the u8/s8 types), and C1=C/C0.

To simplify the scenario, the following scenario assumes that the input feature map has 4 channels, that is, Ci=4. The shape of the input feature maps in A1 is (Hi,Wi,Ci). After Load3D processing, the data shape in A2 is (Wo\*Ho, Hk\*Wk\*Ci), where Wo and Ho are the output shape after convolution, and Hk and Wk are the shape of the filter.

Intuitively, the img2col process is the process in which the filter sweeps across the feature map and expands the corresponding feature map data into each row of the output data. The filter first slides Wo steps in the W direction, then moves one step in the H direction and repeats the above process, ultimately outputting Wo\*Ho rows of data. In the following figure, the red and yellow data represent the first row and the second row, respectively. The numbers indicate the association among the original input data, the filter, and the output data. It can be seen that Load3D first moves the 4 numbers corresponding to 00 in the Ci dimension of the input data, and then moves the four numbers corresponding to 01. Ultimately, the size of this row is Hk\*Wk\*Ci, that is, 3\*3\*4=36 numbers.

The corresponding feature map format is shown in the following figure:

**Figure 2** feature map format<a id="zh-cn_topic_0000002512171652_fig8137131714319"></a>  

![](../../../../figures/feature_map_format.png "feature-map format")

The corresponding filter format is shown in the following figure:

Here, n is the number of filters. It can be seen that the dimension arrangement is (Hk,Wk,Ci,n). However, note that the format in the following figure still needs to be converted according to the format of the B cube in Mmad.

**Figure 3** filter format<a id="zh-cn_topic_0000002512171652_fig1395194710496"></a>  

![](../../../../figures/filter_format.png "filter format")

In actual operations, due to limitations in storage space or computing capability, we usually divide the entire convolution computation into blocks, moving and computing only a small block of data at a time.

**Figure 4** Convolution computation block division<a id="zh-cn_topic_0000002512171652_fig5278110624"></a>  

![](../../../../figures/conv_tiling_block.png "convolution computation block division")

For the feature map of A2, there are two schemes: horizontal block division and vertical block division, corresponding to repeatMode values 0 and 1 respectively.

Note: The fractal cube in the following figures is 4x4 in size, but it should actually be 16x16 (16x32 for u8/s8 types).

When repeatMode = 0, each repeat changes the position of the data point read in the filter window, and then jumps to the position of the next C0.

**Figure 5** Filter window when repeatMode = 0<a id="zh-cn_topic_0000002512171652_fig11710143294513"></a>  

![](../../../../figures/repeat_mode_0_filter_window.png "filter window when repeatMode = 0")

When repeatMode = 1, the position of the data read in the filter window remains unchanged, and each repeat advances by C0 elements in the feature map.

**Figure 6** Filter window when repeatMode = 1<a id="zh-cn_topic_0000002512171652_fig1648315981717"></a>  

![](../../../../figures/repeat_mode_1_filter_window.png "Filter window when repeatMode = 1")

## Calling Example<a id="zh-cn_topic_0000002512171652_section088124295117"></a>

In the following example: cube A is transposed, with shape [k, m]. Under the b32 data type, the data layouts from GM -> L1 Buffer -> L0A Buffer are ND, NZ, and ZZ, respectively. However, when moving from L1 Buffer to L0A Buffer, the LoadDataWithTranspose instruction cannot be called to transpose it, because two consecutive fractals along the K axis cannot be merged into a single 16\*16 block. The schematic diagram is as follows:

![](../../../../figures/load3d_l1tol0a_b32demo.png)

In this case, the Load3D API can be called to implement the transposition of cube A. When the Load3DV2 instruction is called, before writing to the L0A Buffer, the height and width axes of cube A are first aligned to 16 and 8, respectively. Then the instruction transposes the entire cube A and also transposes each fractal, so that the cube A finally written to the L0A Buffer is in ZZ layout.

The sample code snippet is as follows, showing only part of the code in the sample. For the complete sample, see [load_data_l12l0 sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0).

```cpp
// The Load3Dv2 instruction completes the img2col process. It can be seen that after img2col, the height of cube A is ho * wo. According to the calculation formulas of ho and wo, substituting parameters such as the convolution kernel width, convolution kernel sliding stride, and convolution kernel dilation coefficient, it can be seen that the height of cube A is CeilAlign(k, fractalShape[0]). After img2col, the width of cube A is ci * kh * kw. Substituting kh=1 and kw=1, it can be seen that the width of cube A is CeilAlign(m, fractalShape[1]). Finally, configure loadDataParams.enTranspose = true to transpose the entire cube A and also transpose each fractal within it.
// Use the Load3D API to implement NZ2ZZ.
AscendC::LoadData3DParamsV2<T> loadDataParams;
// Source operand height.
loadDataParams.l1H = 1;
// Source operand width.
loadDataParams.l1W = CeilAlign(k, fractalShape[0]);
// Number of channels of the source operand,
// The height of the img2col result cube is ho * wo. According to the calculation formulas of ho and wo, substituting parameters such as the convolution kernel width, convolution kernel sliding stride, and convolution kernel dilation coefficient shows that: ho * wo = loadDataParams.l1H * loadDataParams.l1W
// The width of the img2col result cube is ci * kh * kw. Substituting kh=1 and kw=1 shows that the width of the result cube is ci=loadDataParams.channelSize = m
loadDataParams.channelSize = CeilAlign(m, fractalShape[1]);
// The transfer length of this instruction in the width dimension of the destination operand. If the rightmost fractal is not covered, it must be a multiple of 16 for the half type and a multiple of 32 for int8_t/uint8_t; if covered, there is no multiple requirement.
loadDataParams.kExtension = CeilAlign(m, fractalShape[1]);
// The transfer length of this instruction in the height dimension of the destination operand. If the bottommost fractal is not covered, it must be a multiple of 16 for half/int8_t/uint8_t; if covered, there is no multiple requirement.
loadDataParams.mExtension = CeilAlign(k, fractalShape[1] * fractalNum);
// The stride of the convolution kernel sliding in the width dimension of the source operand
loadDataParams.strideW = 1;
// The stride of the convolution kernel sliding in the height dimension of the source operand
loadDataParams.strideH = 1;
// Convolution kernel width
loadDataParams.filterW = 1;
// Convolution kernel height
loadDataParams.filterH = 1;
// Dilation coefficient of the convolution kernel width
loadDataParams.dilationFilterW = 1;
// Dilation coefficient of the convolution kernel height
loadDataParams.dilationFilterH = 1;
loadDataParams.filterSizeW = false;
loadDataParams.filterSizeH = false;
loadDataParams.enTranspose = true;
loadDataParams.fCubeCtrl = false;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```

For information about how to use Load3D for 2D data transfer, see the corresponding scenario in [load_data_l12l0 sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0).
