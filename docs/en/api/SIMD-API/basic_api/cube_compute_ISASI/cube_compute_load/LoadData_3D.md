# LoadData (convolution data movement)<a id="ZH-CN_TOPIC_0000002538071224"></a>

<!-- md-trans-meta sourceCommit=9bb45191e5688a8d85bea8f7a62e273b7704ea8f translatedAt=2026-09-23T11:57:01.466Z pushedAt=2026-10-26T07:41:44.096Z nl=2 -->

## Applicable Products<a id="zh-cn_topic_0000002512171652_section796754519912"></a>

### LoadData (convolution data movement) v1 API

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_3D_res.md#id1 -->

### LoadData (convolution data movement) v2 API and LoadData (convolution data movement) v2 Pro API

<!-- npu="950" id10 -->
- Ascend 950PR&950DT products: supported
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 products: supported
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 products: supported
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 inference product: supported
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products AI Core: supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products Vector Core: not supported
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas training products: not supported
<!-- end id16 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_3D_res.md#id2 -->

## Description<a id="zh-cn_topic_0000002512171652_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

**LoadData** (convolution data movement) is essentially used to perform Image to Column expansion on the Feature Map in NC1HWC0 format, and then select a specified data block from the expanded two-dimensional matrix and move it to the corresponding memory location. See Figure 1.

Because **LoadData** moves data in units of 512-byte data fractals during the movement process, **LoadData** can also be used to move 2D format data required for ordinary matrix computation. For details about how to use **LoadData** to move 2D format data, see [Key Feature Description](#zh-cn_topic_0000002512171652_section102629572045).

**LoadData** (convolution data movement) supports only the following data paths: L1 Buffer -> L0A Buffer and L1 Buffer -> L0B Buffer.

For the implementation principle, see the pseudocode: [LoadData (convolution data movement) pseudocode](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0/scripts/load3d.py).

**Figure 1** LoadData to L0A Buffer functional diagram<a id="zh-cn_topic_0000002512171652_fig54450833715"></a>  

![](../../../../figures/load3d_l1tol0a_demo.png "LoadData-to-L0A Buffer functional diagram")

## Prototype<a id="zh-cn_topic_0000002512171652_section82039854412"></a>

**LoadData (convolution data movement) v1 API:**

```cpp
template <typename T, const IsResetLoad3dConfig &defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG, typename U = PrimT<T>, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV1<U>& loadDataParams)
```

**LoadData (convolution data movement) v2 API:**

```cpp
template <typename T, const IsResetLoad3dConfig& defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG, typename U = PrimT<T>, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2<U>& loadDataParams)
```

**LoadData (convolution data movement) v2 Pro API:**

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2Pro& loadDataParams)
```

## Parameters<a id="zh-cn_topic_0000002512171652_section16128134420472"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ---------- | ------ |
| T | Data type of the source operand and destination operand. For supported data types, see [Data Types](#zh-cn_topic_0000002512171652_section4219135304818). |
| defaultConfig | Controls whether to set related attributes inside the LoadData (convolution data movement) v1/LoadData (convolution data movement) v2 API. Type: IsResetLoad3dConfig. The IsResetLoad3dConfig structure is defined as follows:<br>struct IsResetLoad3dConfig {<br>   bool isSetFMatrix = true;<br>   bool isSetPadding = true;<br>};<br>When **isSetFMatrix** is set to **true**, the attribute description of the Feature Map (including **l1H**, **l1W**, and **padList**; see [Table 3](#zh-cn_topic_0000002512171652_table679014222918) and [Table 4](#zh-cn_topic_0000002512171652_table193501032193419) for parameter introduction) is set inside the API. When set to **false**, the attribute description of the Feature Map passed to this API does not take effect, and developers need to set it through **SetFmatrix**.<br>When **isSetPadding** is set to **true**, the Pad attribute description (that is, the **padValue** parameter; see [Table 3](#zh-cn_topic_0000002512171652_table679014222918) and [Table 4](#zh-cn_topic_0000002512171652_table193501032193419) for parameter introduction) is set inside the API. When set to **false**, the Pad attribute passed to this API does not take effect, and developers need to set it through **SetLoadDataPaddingValue**. For details, see the **SetFmatrix** call example.<br>The default value of this parameter is as follows:<br>constexpr IsResetLoad3dConfig IS_RESER_LOAD3D_DEFAULT_CONFIG = {true, true};<br>For feature details, see [Feature Map and Pad Attribute Description Register Settings](#zh-cn_topic_0000002512171652_section1881795134015). |
| U | Data type of **padValue** in LoadData3DParamsV1/LoadData3DParamsV2.<br>&nbsp;&nbsp;&bull;When **dst** and **src** use basic data types, **U** must be consistent with the data type **T** of **dst** and **src**; otherwise, compilation fails.<br>&nbsp;&nbsp;&bull;When **dst** and **src** use the TensorTrait type, the LiteType of **U** must be consistent with the data type **T** of **dst** and **src**; otherwise, compilation fails.<br>The last template parameter is used only for the data type check described above, and users do not need to pay attention to it. |

**Table 2** General parameter description

| Parameter | Input/Output | Description |
| ---------- | ----------- | ------ |
| dst | Output | Destination operand, of the LocalTensor type.<br>The data contiguous arrangement order is determined by the physical storage location of the destination operand. For the specific format constraints of different product models, see [Constraints](#zh-cn_topic_0000002512171652_dst_layout). |
| src | Input | Source operand, of the LocalTensor type.<br>The contiguous data arrangement order is determined by the physical storage location of the source operand:<br>&nbsp;&nbsp;&bull; L1 Buffer (TPosition: A1/B1): NC1HWC0 format. |
| loadDataParams | Input | LoadData parameter structure, of the following types:<br>&nbsp;&nbsp;&bull; LoadData3DParamsV1. For details, see [Table 3](#zh-cn_topic_0000002512171652_table679014222918).<br>&nbsp;&nbsp;&bull; LoadData3DParamsV2. For details, see [Table 4](#zh-cn_topic_0000002512171652_table193501032193419).<br>&nbsp;&nbsp;&bull; LoadData3DParamsV2Pro. For details, see [Table 5](#zh-cn_topic_0000002512171652_table118027314415).<br>For the definitions of the preceding structure parameters, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_mm.h`. Replace `${INSTALL_DIR}` with the file storage path after the CANN software is installed. |


**Table 3** Parameter description in the LoadData3DParamsV1 structure<a id="zh-cn_topic_0000002512171652_table679014222918"></a>

| Parameter | Description |
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
| jumpStride | Step of the first address of the destination operand between iterations. Value range: jumpStride∈[1, 127]. |
| repeatMode | Iteration mode.<br>&nbsp;&nbsp;&bull;Mode 0: In each iteration, the point in the convolution kernel window is increased, corresponding to growth in the w dimension direction on the destination matrix.<br>&nbsp;&nbsp;&bull;Mode 1: In each iteration, the upper-left coordinate of the sliding window is increased, corresponding to growth in the h dimension direction on the destination matrix.<br>Value range: repeatMode∈[0, 1]. Default value: 0. |
| repeatTime | Number of iterations. In each iteration, the addresses of the source operand and destination operand change. Value range: repeatTime∈[1, 255]. |
| cSize | Configures whether to enable the cSize = 4 (b16) / cSize = 8 (b8) optimization. Value range: cSize∈[0, 1]. Default value: 0. |
| padValue | Value of the Pad padding. The data type must be consistent with **src**. Default value: 0. If padding is not required, set **padList** to all zeros. |


**Table 4** Parameter description in the LoadData3DParamsV2 structure<a id="zh-cn_topic_0000002512171652_table193501032193419"></a>

| Parameter | Description |
| ---------- | ------ |
| padList | Padding list [padding_left, padding_right, padding_top, padding_bottom]. Value range of each element: [0, 255]. Default value: {0, 0, 0, 0}. |
| l1H | Height of the source operand. Value range: l1H∈[0, 32767]. Default value: 0.<br>**l1H = 0 indicates not executing the movement, and this API is treated as a NOP (no operation).** |
| l1W | Width of the source operand. Value range: l1W∈[0, 32767]. Default value: 0.<br>**l1W = 0 indicates not executing the movement, and this API is treated as a NOP (no operation).** |
| channelSize | Number of channels of the source operand. Value range: channelSize∈[0, 65535]. Default value: 0. Different product models support different channelSize values. For details, see [Constraints](#zh-cn_topic_0000002512171652_channelsize_constraint).<br>**Note: channelSize = 0 indicates not executing the movement, and this API is treated as a NOP (no operation).** |
| kExtension | Transfer length of this instruction in the width dimension of the destination operand. If the rightmost fractal is not covered, it must be a multiple of 8 for the b32 type, a multiple of 16 for the b16 type, a multiple of 32 for the b8 type, and a multiple of 64 for the b4 type. In the covered case, no multiple requirement applies to any data type. Value range: kExtension∈[0, 65535]. Default value: 0.<br>**Note: kExtension = 0 indicates not executing the movement, and this API is treated as a NOP (no operation).** |
| mExtension | Transfer length of this instruction in the height dimension of the destination operand. If the bottommost fractal is not covered, it must be a multiple of 16 for the b4, b8, and b16 types, and no requirement applies to the b32 type. In the covered case, no multiple requirement applies to any data type. Value range: mExtension∈[0, 65535]. Default value: 0.<br>**Note: mExtension = 0 indicates not executing the movement, and this API is treated as a NOP (no operation).** |
| kStartPt | Start point of this instruction in the width dimension of the destination operand. It must be a multiple of 8 for the b32 type, a multiple of 16 for the b16 type, a multiple of 32 for the b8 type, and a multiple of 64 for the b4 type. Value range: [0, 65535]. Default value: 0. |
| mStartPt | Start point of this instruction in the height dimension of the destination operand. Value range: [0, 32767]. Default value: 0. |
| strideW | Step by which the convolution kernel slides in the width dimension of the source operand. Value range: strideW∈[0, 63]. Default value: 1. |
| strideH | Step by which the convolution kernel slides in the height dimension of the source operand. Value range: strideH∈[0, 63]. Default value: 1. |
| filterW | Width of the convolution kernel. Value range: filterW∈[0, 255]. Default value: 1.<br>**Note: filterW=0 and filterSizeW=false indicate not executing the movement, and this API is treated as a NOP (no operation).** |
| filterH | Height of the convolution kernel. Value range: filterH∈[0, 255]. Default value: 1.<br>**Note: filterH=0 and filterSizeH=false indicate not executing the movement, and this API is treated as a NOP (no operation).** |
| dilationFilterW | Dilation coefficient of the convolution kernel width. Value range: dilationFilterW∈[0, 255]. Default value: 1. |
| dilationFilterH | Dilation coefficient of the convolution kernel height. Value range: dilationFilterH∈[0, 255]. Default value: 1. |
| enTranspose | Whether to enable the transpose function to transpose the entire destination matrix. Supported data type: bool. Default value: false.<br>&nbsp;&nbsp;&bull; true: Enabled.<br>&nbsp;&nbsp;&bull; false: Disabled.<br>The valid conditions of **enTranspose** differ across product models. For details, see [Constraints](#zh-cn_topic_0000002512171652_entranspose_constraint).<br>**Note: This parameter is invalid when the physical storage location of the destination operand is the L0B Buffer (TPosition: B2).** |
| enSmallK | Whether to enable the small k feature. The size of each fractal matrix is 16*4. Supported data type: bool. Default value: false. In the current product form, this feature is no longer supported. |
| padValue | Value of the Pad padding. The data type must be consistent with **src**. If padding is not required, set **padList** to all zeros. Default value: 0. |
| filterSizeW | Whether to increase the convolution kernel width by 256 elements on the basis of **filterW**. true: increase. false: do not increase. Default value: false. |
| filterSizeH | Whether to increase the convolution kernel height by 256 elements on the basis of **filterH**. true: increase. false: do not increase. Default value: false. |
| fMatrixCtrl | Indicates whether the LoadData (convolution data movement) v2 instruction obtains the attribute description of the Feature Map from the left matrix or the right matrix. It is used together with **SetFmatrix**. Default value is false. Currently, only false is supported.<br>&nbsp;&nbsp;&bull; true: Obtain the attribute description of the Feature Map from the right matrix.<br>&nbsp;&nbsp;&bull; false: Obtain the attribute description of the Feature Map from the left matrix. |

**Table 5** Parameter description in the LoadData3DParamsV2Pro structure<a id="zh-cn_topic_0000002512171652_table118027314415"></a>

| Parameter | Description |
| ---------- | ------ |
| **channelSize** | Number of channels of the source operand. Value range: channelSize∈[0, 65535].<br>For half, the remainder of channelSize divided by 16 must be 0, 4, or 8.<br>For int8_t and uint8_t, the remainder of channelSize divided by 32 must be 0, 4, 8, or 16.<br>For int4b_t, channelSize is 8, 16, 32, N\*64, N\*64+8, N\*64+16, N\*64+32, where N is a positive integer. |
| **enTranspose** | Whether to enable the transpose function, which transposes the entire destination matrix. The supported data type is bool. It takes effect only when the destination TPosition is A2 and the source operand is of the half type. The default value is false.<br>&nbsp;&nbsp;&bull; true: Enabled.<br>&nbsp;&nbsp;&bull; false: Not enabled. |
| **enSmallK** | Whether to enable the small k feature. The size of each fractal matrix is 16\*4. The supported data type is bool. The default value is false.<br>**Note: This feature is no longer supported in the current product form.** |
| **filterSizeW** | Whether to increase the convolution kernel width by 256 elements on the basis of filterW. true: Increase. false: Do not increase. |
| **filterSizeH** | Whether to increase the convolution kernel height by 256 elements on the basis of filterH. true: Increase. false: Do not increase. |
| **fMatrixCtrl** | Indicates whether the LoadData (convolution data movement) v2 instruction obtains the attribute description of the FeatureMap from the left matrix or the right matrix. It is used together with SetFmatrix. Currently, only false is supported. The default value is false.<br>&nbsp;&nbsp;&bull; true: Obtain the attribute description of the FeatureMap from the right matrix.<br>&nbsp;&nbsp;&bull; false: Obtain the attribute description of the FeatureMap from the left matrix. |
| **extConfig** | Combined parameter (uint64_t type). The default value is 0.<br>extConfig = ((uint64_t)mStartPt << 48)<br>    &#124; ((uint64_t)kStartPt << 32)<br>    &#124; ((uint64_t)mExtension << 16)<br>    &#124; (uint64_t)kExtension; |
| **filterConfig** | Combined parameter (uint64_t type). The default value is 0X10101010101.<br>filterConfig = ((uint64_t)dilationFilterH << 40)<br>    &#124; ((uint64_t)dilationFilterW << 32)<br>    &#124; ((uint64_t)filterH << 24)<br>    &#124; ((uint64_t)filterW << 16)<br>    &#124; ((uint64_t)strideH << 8)<br>    &#124; (uint64_t)strideW; |

## Data Type<a id="zh-cn_topic_0000002512171652_section4219135304818"></a>

<!-- npu="310p,910" id19 -->
**LoadData (convolution data movement) v1 API:**

<!-- npu="310p" id20 -->
- Atlas inference products AI Core supports the following data types: **int8_t**, **uint8_t**, **half**.
<!-- end id20 -->

<!-- npu="910" id21 -->
- Atlas training products supports the following data types: **int8_t**, **uint8_t**, **half**.
<!-- end id21 -->
<!-- end id19 -->

<!-- npu="950,A3,910b,310b,310p" id22 -->
**LoadData (convolution data movement) v2 API and LoadData (convolution data movement) v2 Pro API:**

<!-- npu="950" id23 -->
- Ascend 950PR&950DT products supports the following data types: **int8_t**, **uint8_t**, **hifloat8_t**, **fp8_e5m2_t**, **fp8_e4m3fn_t**, **half**, **bfloat16_t**, **int32_t**, **uint32_t**, **float**.
<!-- end id23 -->

<!-- npu="A3" id24 -->
- Atlas A3 products:
    - When **TPosition** is A1/A2, the supported data types are: **int4b_t**, **int8_t**, **uint8_t**, **half**, **bfloat16_t**, **int32_t**, **uint32_t**, **float**.
    - When **TPosition** is B1/B2, the supported data types are: **half**, **bfloat16_t**, **int32_t**, **uint32_t**, **float**.
<!-- end id24 -->

<!-- npu="910b" id25 -->
- Atlas A2 products:
    - When TPosition is A1/A2, the supported data types are: int4b_t, int8_t, uint8_t, half, bfloat16_t, int32_t, uint32_t, float.
    - When TPosition is B1/B2, the supported data types are: half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id25 -->

<!-- npu="310b" id26 -->
- Atlas 200I/500 A2 inference product:
    - When TPosition is A1/A2, the supported data types are: int4b_t, int8_t, uint8_t, half, bfloat16_t, int32_t, uint32_t, float.
    - When TPosition is B1/B2, the supported data types are: half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id26 -->

<!-- npu="310p" id27 -->
- Atlas inference products AI Core supports the following data types: int4b_t, int8_t, uint8_t, half.
<!-- end id27 -->

<!-- end id22 -->

## Return Value<a id="zh-cn_topic_0000002512171652_section640mcpsimp"></a>

None

## Constraints<a id="zh-cn_topic_0000002512171652_section2045914466492"></a>

- In non-transpose scenarios, the L1 Buffer->L0B Buffer path is not supported. The L1 Buffer->L0B Buffer path automatically performs transposition, so **enTranspose** does not need to be configured and is invalid in this case.

<!-- npu="950" id29 -->
- For Ascend 950PR&950DT products: The **dstStride** parameter must be configured using the auxiliary configuration API **SetLoadDataRepeat**. If repeat mode is not enabled, the repeat-related parameters in the API use their default constructed values.
<!-- end id29 -->

<!-- npu="A3,910b" id30 -->
- For the following product models:

    <!-- npu="A3" id31 -->
    Atlas A3 products;
    <!-- end id31 -->
    <!-- npu="910b" id32 -->
    Atlas A2 products;
    <!-- end id32 -->

    The L1 Buffer-\>L0B Buffer path does not support b4/b8 data types; in transpose scenarios, the L1 Buffer-\>L0A Buffer path does not support the b8 data type.
<!-- end id30 -->

- When any of **l1H**/**l1W**/**channelSize**/**kExtension**/**mExtension** is 0, the instruction is not executed.
- When **filterW** is 0 and **filterSizeW** is false, or when **filterH** is 0 and **filterSizeH** is false, the instruction is not executed.
- When the destination address is in the L0A Buffer/L0B Buffer, the address must be 512-byte aligned. When the source address is in the L1 Buffer, the address must be 32-byte aligned. The pipeline occupied by instruction execution is PIPE_MTE1.
<!-- npu="910,310p" id33 -->
- To enable the **cSize** feature of **LoadData3DParamsV1**, ensure that the feature map in the L1 Buffer (A1/B1) is 4-channel aligned.
<!-- end id33 -->

- The contiguous data arrangement order is determined by the physical storage location of the destination operand. The specific constraints are as follows:<a id="zh-cn_topic_0000002512171652_dst_layout"></a>

    <!-- npu="A3,910b" id34 -->
    - For Atlas A3 products and Atlas A2 products:
        - L0A Buffer (TPosition: A2): ZZ format.
        - L0B Buffer (TPosition: B2): ZN format.
    <!-- end id34 -->

    <!-- npu="950" id35 -->
    - For Ascend 950PR&950DT products:
        - L0A Buffer (TPosition: A2): NZ format.
        - L0B Buffer (TPosition: B2): ZN format.
    <!-- end id35 -->

- The value range constraints of **channelSize** in the **LoadData3DParamsV2** structure are as follows:<a id="zh-cn_topic_0000002512171652_channelsize_constraint"></a>

    <!-- npu="310p" id36 -->
    - For the Atlas inference products AI Core: for **half**, **channelSize** can be 4, 8, 16, N\*16+4, N\*16+8; for **int8_t**/**uint8_t**, **channelSize** can be 4, 8, 16, 32, N\*32+4, N\*32+8, N\*32+16; for **int4b_t**, **channelSize** can be 8, 16, 32, N\*64, N\*64+8, N\*64+16, N\*64+32. N is a positive integer.
    <!-- end id36 -->

    <!-- npu="950,A3,910b,310b" id37 -->
    - For Ascend 950PR&950DT products, Atlas A3 products, Atlas A2 products, and Atlas 200I/500 A2 inference products: for uint32_t/int32_t/float, **channelSize** can be 4, N\*8, N\*8+4; for half/bfloat16, **channelSize** can be 4, 8, N\*16, N\*16 + 4, N\*16+8; for int8_t/uint8_t, **channelSize** can be 4, 8, 16, 32\*N, N\*32+4, N\*32+8, N\*32+16; for int4b_t, **channelSize** can be 8, 16, 32, N\*64, N\*64+8, N\*64+16, N\*64+32. N is a positive integer.
    <!-- end id37 -->

- The valid conditions for **enTranspose** in the **LoadData3DParamsV2** structure are as follows:<a id="zh-cn_topic_0000002512171652_entranspose_constraint"></a>

    <!-- npu="A3,910b" id38 -->
    - For Atlas A3 products and Atlas A2 products: valid when the physical storage location of the destination operand is L0A Buffer (**TPosition**: A2) and the source operand is of b16/b32 type.
    <!-- end id38 -->

    <!-- npu="950" id39 -->
    - For Ascend 950PR&950DT products: valid when the physical storage location of the destination operand is L0A Buffer (**TPosition**: A2) and the source operand is of b8/b16/b32 type.
    <!-- end id39 -->

## Key Features<a id="zh-cn_topic_0000002512171652_section102629572045"></a>

### Using LoadData (Convolution Data Movement) to Move 2D Format Data Required for Matrix Computation<a id="zh-cn_topic_0000002512171652_section86392366416"></a>

For data in the NC1HWC0 format, when N is 1, the convolution kernel width and height are 1, padding is 0, the convolution kernel slides with a stride of 1 in the width and height dimensions of the source operand, and the dilation coefficients for the convolution kernel width and height are 1, the data arrangement after image-to-column expansion can be regarded as the NZ fractal layout in the L1 Buffer during matrix computation. Therefore, in this scenario, the LoadData (convolution data movement) API can be used to move the 2D-format data required for matrix computation. The following figure shows the schematic diagram:

![](../../../../figures/load3d_3d22d_demo.png)

#### Non-transpose Scenario

First, the L1 Buffer -> L0B Buffer path does not support the non-transpose scenario. For the L1 Buffer -> L0A Buffer path, different fractal layout conversions apply depending on the product form:

<!-- npu="A3,910b" id40 -->
- For Atlas A3 products and Atlas A2 products:

    **LoadData** (convolution data movement) automatically completes the fractal transformation, converting the NZ fractal layout on the L1 Buffer into the ZZ fractal layout required in the L0A Buffer, as shown in the following diagram of the b16 type scenario:

    ![](../../../../figures/load3d_l1tol0a_b16_nontrans.png)
<!-- end id40 -->

<!-- npu="950" id41 -->
- For Ascend 950PR&950DT products:

    The fractal layout on the L1 Buffer is exactly the NZ fractal layout required in the L0A Buffer, and **LoadData** (convolution data movement) does not perform fractal layout conversion, as shown in the following diagram of the b16 type scenario:

    ![](../../../../figures/load3d_l1tol0a_b16_nontrans_950.png)
<!-- end id41 -->

#### Transpose Scenario

<!-- npu="A3,910b" id42 -->
- For Atlas A3 products and Atlas A2 products:

    **LoadData** (convolution data movement) supports transpose (**enTranspose**=**True**) only for the b16/b32 data types. The details by type are as follows:

    - For the b16 scenario: Each fractal size is 16×16.

        On the L1 Buffer->L0A Buffer path, in the transpose scenario, in addition to each fractal matrix itself being transposed, the position of each fractal matrix in the entire 2D feature map is also transposed. The transpose diagram is as follows:

        ![](../../../../figures/load3d_l1tol0a_b16_trans.png)

        On the L1 Buffer->L0B Buffer path, the **LoadData** (convolution data movement) API automatically completes the transpose, so there is no need to configure the **enTranspose** parameter. The **enTranspose** parameter is invalid in this case. The transpose diagram is as follows:

        ![](../../../../figures/load3d_l1tol0b_b16_trans.png)

    - For the b32 scenario: Each fractal size is 16×8.

        On the L1 Buffer->L0A Buffer path, you need to configure the **enTranspose** parameter to enable the transpose function. The transpose diagram is as follows:

        ![](../../../../figures/load3d_l1tol0a_b32_trans.png)

        On the L1 Buffer->L0B Buffer path, the LoadData (convolution data movement) API automatically performs the transpose, so the **enTranspose** parameter does not need to be configured and is invalid in this case. The transpose diagram is as follows:

        ![](../../../../figures/load3d_l1tol0b_b32_trans.png)
<!-- end id42 -->

<!-- npu="950" id43 -->
- For the Ascend 950PR&950DT products:

    LoadData (convolution data movement) supports transpose for the b8/b16/b32 data types. On the L1 Buffer->L0A Buffer path, transpose is enabled by turning on the **enTranspose** parameter. On the L1 Buffer->L0B Buffer path, transpose is performed automatically, so the **enTranspose** parameter does not need to be configured and is invalid in this case. The details are described by type below.

    - For the b8 scenario: each fractal is 16×32 in size.

        On the L1 Buffer->L0A Buffer path, in the transpose scenario, two consecutive 16×32 fractals are concatenated into a 32×32 square matrix, which is then transposed and split back into two 16×32 fractals. The transpose diagram is as follows:

        ![](../../../../figures/load3d_l1tol0a_b8_trans_950.png)

        On the L1 Buffer->L0B Buffer path, the LoadData (convolution data movement) API automatically performs the transpose, so the **enTranspose** parameter does not need to be configured and is invalid in this case. The transpose diagram is as follows:

        ![](../../../../figures/load3d_l1tol0b_b8_trans_950.png)

    - For the b16 scenario: each fractal size is 16×16.

        Under the L1 Buffer->L0A Buffer path, in the transpose scenario, in addition to each fractal matrix itself being transposed, the position of each fractal matrix in the entire 2D feature map is also transposed. The transpose diagram is as follows:

        ![](../../../../figures/load3d_l1tol0a_b16_trans_950.png)

        Under the L1 Buffer->L0B Buffer path, the LoadData (convolution data movement) API automatically completes the transpose, so there is no need to configure the enTranspose parameter. The enTranspose parameter is invalid in this case. The transpose diagram is as follows:

        ![](../../../../figures/load3d_l1tol0b_b16_trans_950.png)

    - For the b32 scenario: each fractal size is 16×8.

        Under the L1 Buffer->L0A Buffer path, the transpose scenario diagram is as follows:

        ![](../../../../figures/load3d_l1tol0a_b32_trans_950.png)

        Under the L1 Buffer->L0B Buffer path, the LoadData (convolution data movement) API automatically completes the transpose, so there is no need to configure the enTranspose parameter. The enTranspose parameter is invalid in this case. The transpose diagram is as follows:

        ![](../../../../figures/load3d_l1tol0b_b32_trans_950.png)
<!-- end id43 -->

### Repeat Mode<a id="zh-cn_topic_0000002512171652_section131671145123912"></a>

The **LoadData** (convolution data movement) API can complete data block movement in multiple directions by configuring **mExtension** and **kExtension**. The movement of multiple data blocks here is regarded as one iteration. By configuring the repeat mode, repeat movement can be implemented in the M direction (height) or the K direction (width). That is, calling the **LoadData** (convolution data movement) API once can complete data movement for multiple iterations, as shown in the following figure. The **repeatStride**, **repeatTime**, and the direction along which to repeat are configured through [SetLoadDataRepeat](../cube_load_aux_config/SetLoadDataRepeat.md).

<!-- npu="A3,910b" id44 -->
For the following product models:

<!-- npu="A3" id45 -->
Atlas A3 products;
<!-- end id45 -->

<!-- npu="910b" id46 -->
Atlas A2 products;
<!-- end id46 -->

Schematic diagram of repeat in the M direction:

![](../../../../figures/load3d_repeatmode.png)

Note that when we call the **LoadData** (convolution data movement) API to perform repeat movement in the M direction, if we enable transpose, all block matrices in the repetition process are treated as one global matrix and processed according to the transpose scenario described earlier. Taking the b32 scenario as an example, the following figure shows the transpose schematic diagram when repeat movement is performed in the M direction, where **repeatMode**=0, **repeatStride**=2, and **repeatTime**=3.

![](../../../../figures/load3d_repeatmode_m.png)

Schematic diagram of repeat in the K direction:

![](../../../../figures/load3d_repeatmode_k.png)

When we call the LoadData (convolution data movement) API to perform repeat movement in the K direction, if we enable transpose, all block matrices in the repetition process are also treated as a global matrix and processed according to the transpose scenario described earlier. Taking the b32 scenario as an example, the following figure shows the transpose diagram for repeat movement in the K direction, where repeatMode=1, repeatStride=2, and repeatTime=3.

![](../../../../figures/load3d_repeatmode_k_trans.png)
<!-- end id44 -->

<!-- npu="950" id47 -->
For the Ascend 950PR&950DT products:

**M-direction repeat diagram:**

![](../../../../figures/load3d_repeatmode_m_950.png)

When we call the LoadData (convolution data movement) API to perform repeat movement in the M direction, if we enable transpose, all block matrices in the repetition process are treated as a global matrix and processed according to the transpose scenario described earlier. Taking the b32 scenario as an example, the following figure shows the transpose diagram for repeat movement in the M direction, where repeatMode=0, repeatStride=2, repeatTime=3, and dstStride=3.

![](../../../../figures/load3d_repeatmode_m_trans_950.png)

**K-direction repeat diagram:**

![](../../../../figures/load3d_repeatmode_k_950.png)

When we call the **LoadData** (convolution data movement) API to perform repeat movement in the K direction, if transpose is enabled, all block matrices in the repetition process are also treated as a global matrix and processed according to the transpose scenario described earlier. Taking the b32 scenario as an example, the following figure shows the transpose diagram when performing repeat movement in the K direction, where **repeatMode**=1, **repeatStride**=2, **repeatTime**=3, and **dstStride**=3.

![](../../../../figures/load3d_repeatmode_k_trans_950.png)
<!-- end id47 -->

### Feature Map and Pad Attribute Description Register Settings<a id="zh-cn_topic_0000002512171652_section1881795134015"></a>

The **LoadData (convolution data movement)** API provides two special-purpose registers: the Feature Map attribute description register and the Pad attribute description register, which serve as parameters for Image to Column expansion. When performing matrix computation, for high-performance movement, a large block of data is often moved from GM to the L1 Buffer, that is, a relatively large block of data is cached in the L1 Buffer, and then a for loop is used to move data from the L1 Buffer to the L0 Buffer and perform Mmad computation. When moving data from the L1 Buffer to the L0 Buffer through the **LoadData (convolution data movement)** API, the Feature Map attributes and Pad attributes usually need to be set only once and do not need to be set repeatedly. Therefore, the **IsResetLoad3dConfig** template parameter is provided to manually manage these two attributes, reducing the instruction performance overhead caused by repeated settings.

The **IsResetLoad3dConfig** structure is defined as follows:

```cpp
struct IsResetLoad3dConfig {
   bool isSetFMatrix = true;
   bool isSetPadding = true;
}; 
```

When **isSetFMatrix** is set to **true**, the FeatureMap attribute description (including **l1H**, **l1W**, and **padList**; see [Table 4](#zh-cn_topic_0000002512171652_table193501032193419) for parameter descriptions) is set inside the **LoadData (convolution data movement)** API. When it is set to **false**, the FeatureMap attribute description passed to the API does not take effect, and developers need to manage it manually through [SetFmatrix](../cube_load_aux_config/SetFmatrix.md).

When **isSetPadding** is set to **true**, the Pad attribute description (that is, the **padValue** parameter; see [Table 4](#zh-cn_topic_0000002512171652_table193501032193419) for parameter descriptions) is set inside the API. When it is set to **false**, the Pad attribute passed to the API does not take effect, and developers need to set it through [SetLoadDataPaddingValue](../cube_load_aux_config/SetLoadDataPaddingValue.md).

### LoadData (convolution data movement) API data format description<a id="zh-cn_topic_0000002512171652_section726316123184"></a>

The input feature map and filter must be in the [NC1HWC0](../cube_compute_fractal_intro/key_fractal_format_details.md#zh-cn_topic_0000002545089965_section217615301084) format, where C0 is the lowest dimension and has a fixed value of 16 (32 for u8/s8 types), and C1=C/C0.

To simplify the scenario, the following scenario assumes that the input feature map has 4 channels, that is, Ci=4. The input feature maps have the shape (Hi,Wi,Ci) in the L1 Buffer (A1), and after LoadData processing, the data in the L0A Buffer (A2) has the shape (Wo\*Ho, Hk\*Wk\*Ci). Here, Wo and Ho are the output dimensions after convolution, and Hk and Wk are the filter dimensions.

Intuitively, the img2col process involves the filter sweeping over the feature map and expanding the corresponding feature map data into each row of the output data. The filter first slides Wo steps in the W direction, then moves one step in the H direction and repeats the above process, finally outputting Wo\*Ho rows of data. In the following figure, the red and yellow data represent the first row and the second row, respectively. The numbers indicate the association among the original input data, the filter, and the output data. It can be seen that LoadData first moves the four numbers corresponding to 00 in the Ci dimension of the input data, then moves the four numbers corresponding to 01, and finally the size of this row is Hk\*Wk\*Ci, that is, 3\*3\*4=36 numbers.

The corresponding feature map format is shown in the following figure:

**Figure 2** feature map format<a id="zh-cn_topic_0000002512171652_fig8137131714319"></a>  

![](../../../../figures/feature_map_format.png "feature map format")

The corresponding filter format is shown in the following figure:

Here, n is the number of filters. It can be seen that the dimension arrangement is (Hk,Wk,Ci,n), but note that the format in the following figure also needs to be converted according to the format of the B matrix in Mmad.

**Figure 3** filter format<a id="zh-cn_topic_0000002512171652_fig1395194710496"></a>  

![](../../../../figures/filter_format.png "filter format")

In practice, due to storage space or computing capability limitations, we usually divide the entire convolution computation into blocks, moving and computing only a small block of data at a time.

**Figure 4** Convolution computation blocking<a id="zh-cn_topic_0000002512171652_fig5278110624"></a>  

![](../../../../figures/conv_compute_block.png "convolution computation blocking")

For the feature map in the L0A Buffer (A2), there are two schemes: horizontal blocking and vertical blocking, corresponding to **repeatMode** values 0 and 1, respectively.

Note: The fractal matrix in the following figures is 4x4, but it should actually be 16x16 (16x32 for u8/s8 types).

When **repeatMode** = 0, each repeat changes the position of the data point read in the filter window, and then jumps to the next C0 position.

**Figure 5** Filter window when repeatMode = 0<a id="zh-cn_topic_0000002512171652_fig11710143294513"></a>  

![](../../../../figures/repeatmode_0_filter_window.png "filter window when repeatMode = 0")

When **repeatMode** = 1, the position of the data read in the filter window remains unchanged, and each repeat advances by C0 elements in the feature map.

**Figure 6** Filter window when repeatMode = 1<a id="zh-cn_topic_0000002512171652_fig1648315981717"></a>  

![](../../../../figures/repeatmode_1_filter_window.png "Filter window when repeatMode = 1")

## Example<a id="zh-cn_topic_0000002512171652_section088124295117"></a>

In the following example, matrix A is transposed with shape [k, m]. Under the b32 data type, the data layouts from GM to L1 Buffer to L0A Buffer are ND, NZ, and ZZ, respectively. However, when moving data from L1 Buffer to L0A Buffer, the LoadDataWithTranspose instruction cannot be used for transposition, because two consecutive fractals along the K axis cannot be merged into a single 16\*16 block. The following figure illustrates this:

![](../../../../figures/load3d_l1tol0a_b32demo.png)

In this case, the LoadData (convolution data movement) API can be used to transpose matrix A. When the LoadData (convolution data movement) v2 API is called, before writing to the L0A Buffer, the height and width axes of matrix A are first aligned to 16 and 8, respectively. Then the instruction transposes the entire matrix A and also transposes each fractal, so that matrix A finally written to the L0A Buffer is in the ZZ layout.

The sample code snippet is as follows, showing only part of the code in the sample. For the complete sample, see [load_data_l12l0 sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0).

```cpp
// The LoadData (convolution data movement) v2 API completes the img2col process. After img2col, the height of matrix A is ho * wo. According to the formulas for ho and wo, substituting parameters such as the convolution kernel width, convolution kernel sliding stride, and convolution kernel dilation coefficient, the height of matrix A is CeilAlign(k, fractalShape[0]). After img2col, the width of matrix A is ci * kh * kw. Substituting kh=1 and kw=1, the width of matrix A is CeilAlign(m, fractalShape[1]). Finally, set loadDataParams.enTranspose = true to transpose the entire matrix A and transpose each fractal within it.
// Use the LoadData (convolution data movement) API to implement NZ2ZZ.
AscendC::LoadData3DParamsV2<T> loadDataParams;
// Source operand height.
loadDataParams.l1H = 1;
// Source operand width.
loadDataParams.l1W = CeilAlign(k, fractalShape[0]);
// Number of channels of the source operand.
// The height of the img2col result matrix is ho * wo. According to the formulas for ho and wo, substituting parameters such as the convolution kernel width, convolution kernel sliding stride, and convolution kernel dilation coefficient yields: ho * wo = loadDataParams.l1H * loadDataParams.l1W
// The width of the img2col result matrix is ci * kh * kw. Substituting kh=1 and kw=1, the width of the result matrix is ci=loadDataParams.channelSize = m
loadDataParams.channelSize = CeilAlign(m, fractalShape[1]);
// The transfer length of this instruction in the width dimension of the destination operand. If the rightmost fractal is not covered, it must be a multiple of 16 for the half type and a multiple of 32 for int8_t/uint8_t; if covered, there is no multiple requirement.
loadDataParams.kExtension = CeilAlign(m, fractalShape[1]);
// The transfer length of this instruction in the height dimension of the destination operand. If the bottommost fractal is not covered, it must be a multiple of 16 for half/int8_t/uint8_t; if covered, there is no multiple requirement.
loadDataParams.mExtension = CeilAlign(k, fractalShape[1] * fractalNum);
// Step by which the convolution kernel slides in the width dimension of the source operand
loadDataParams.strideW = 1;
// Step by which the convolution kernel slides in the height dimension of the source operand
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
loadDataParams.fMatrixCtrl = false;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```

For how to use LoadData (convolution data movement) to perform 2D data movement, see the corresponding scenario in the [load_data_l12l0 sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0).
