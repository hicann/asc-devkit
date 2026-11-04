# SetAippFunctions<a name="ZH-CN_TOPIC_0000001913015092"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T12:54:20.601Z pushedAt=2026-10-26T07:41:44.143Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR & 950DT products: supported
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
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_load_aux_config/SetAippFunctions_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Sets parameters related to image pre-processing (AIPP, AI Core pre-process). It is used together with the [LoadImageToLocal](LoadImageToLocal.md) API. After configuration, calling the [LoadImageToLocal](LoadImageToLocal.md) API completes image pre-processing during data transfer, including data padding, channel swap, single-row read, data type conversion, channel padding, and color space conversion. When calling the **SetAippFunctions** API, you need to pass in the matrix with the source image in Global Memory and the image format of the source image.

- **Padding:** Pads the image in the HW direction. The following modes are supported:
    - Mode 0: Constant padding mode. Each position in the padding area is filled with a constant, and the constant for each channel can be configured. In this mode, only left and right padding is supported, and top and bottom padding is not supported.

        **Figure 1** Constant padding mode (the green area in the middle of the image represents the original data, and the rest is padding data)<a name="fig56681157121916"></a>

        ![](../../../../figures/constant_padding_mode.png "Constant padding mode (the green area in the middle of the image represents the original data, and the rest is padding data)")

    - Mode 1: Row-column padding mode. Each position in the padding area is filled with the data of the nearest source image position in the row/column.

        **Figure 2** Row-column padding mode (the green area in the middle of the image represents the original data, and the rest is padding data)<a name="fig4239143712713"></a>

        ![](../../../../figures/row_col_padding_mode.png "Row-column padding mode (the green area in the middle of the image represents the original data, and the rest is padding data)")

    - Mode 2: Block padding mode. Based on the padding width and height, copies data blocks from the source image to fill the padding area.

        **Figure 3** Block padding mode (the green area in the middle of the image indicates the original data, and the rest is padding data)<a name="fig38821202817"></a>

        ![](../../../../figures/block_padding_mode.png "Block padding mode (the green area in the middle of the image indicates the original data - the rest is padding data)")

    - Mode 3: Mirror block padding mode. Based on the padding width and height, the mirror of the data block is copied from the source image to fill the padding area.

        **Figure 4** Mirror block padding mode (the green area in the middle of the image indicates the original data, and the rest is padding data)<a name="fig14371141920261"></a>

        ![](../../../../figures/mirror_block_padding_mode.png "Mirror block padding mode (the green area in the middle of the image indicates the original data - the rest is padding data)")

- **Channel Swap:** Swaps the image channels.

    - For the RGB888 format, swapping the R and B channels is supported.
    - For the YUV420SP format, swapping the U and V channels is supported.
    - For the XRGB8888 format, shifting the X channel backward (XRGB→RGBX) and swapping the R and B channels are supported.

- **Single-Row Read:** Reads only one row from the source image.

    > [!NOTE]
    > When calling the data movement API, if single-row read is enabled, the configured destination image height parameter is invalid, such as **loadImageToLocalParams.vertSize** of the [LoadImageToLocal](LoadImageToLocal.md) API.

- **Data Type Conversion:** Converts the data type of pixels, supporting conversion from uint8\_t to int8\_t or half. When uint8\_t is converted to int8\_t, the output data range is limited to \[-128, 127\].

    ```cpp
    // Example 1: Implement uint8_t -> int8_t type conversion and zero-mean normalization: set the mean value of each channel to the average of all data in that channel (the min and var values are invalid and do not need to be set).
    output[i][j][k] = input[i][j][k] - mean[k]
    // Example 2: Implement uint8_t -> fp16 type conversion and normalization: set the mean value of each channel to the average of all data in that channel, the min value to the minimum value of all data in that channel after zero-mean normalization, and the var value to the reciprocal of the maximum value minus the minimum value of all data in that channel.
    uint8_t -> fp16:  output[i][j][k] = (input[i][j][k] - mean[k] - min[k]) * var[k]
    ```

    > [!NOTE]
    > The converted data type is determined by the template parameter U. When U is uint8\_t, the data type conversion function does not take effect.
    > When calling the data movement API, the data type of the destination Tensor must be consistent of the output data type of this API, such as the data type of the **dstLocal** parameter of [LoadImageToLocal](LoadImageToLocal.md).

- **Channel Padding:** Pads in the channel direction of the image. The default is mode 0.

    - Mode 0: Pads the channel to 32 bytes. That is, when the output data type is uint8\_t/int8\_t, it pads to 32 channels; when the output data type is fp16, it pads to 16 channels.

    - Mode 1: Pads the channel to 4 channels.

- **Color Space Conversion:** Converts RGB format to YUV format, or YUV format to RGB format.

    ![](../../../../figures/zh-cn_formulaimage_0000001938335310.png)

    ![](../../../../figures/zh-cn_formulaimage_0000001938176178.png)

## Function Prototype<a name="section620mcpsimp"></a>

- Input image format is YUV400, RGB888, or XRGB8888

    ```cpp
    template<typename T, typename U>
    __aicore__ inline void SetAippFunctions(const GlobalTensor<T>& src0, AippInputFormat format, AippParams<U> config)
    ```

- Input image format is YUV420 Semi-Planar

    ```cpp
    template<typename T, typename U>
    __aicore__ inline void SetAippFunctions(const GlobalTensor<T>& src0, const GlobalTensor<T>& src1, AippInputFormat format, AippParams<U> config)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Input data type, which must be consistent with the data type set in **format**. |
| U | Output data type. The same data type must be configured in the copy API, for example, the data type of the **dstLocal** parameter of [LoadImageToLocal](LoadImageToLocal.md).<br>&nbsp;&nbsp;&nbsp;&nbsp;&bull;If the data type conversion function is disabled, it must be consistent with the input type.<br>&nbsp;&nbsp;&nbsp;&nbsp;&bull;If the data type conversion function is enabled, it must be consistent with the expected converted type. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src0 | Input | Matrix of the source image in Global Memory.<br>When the source image format is YUV420SP, it indicates the matrix of the Y dimension in Global Memory. |
| src1 | Input | When the source image format is YUV420SP, it indicates the matrix of the UV dimension in Global Memory.<br>When the source image format is another format, this parameter is invalid. |
| format | Input | Source image format. **AippInputFormat** is an enumeration type with the following values:<br>**AippInputFormat::YUV420SP_U8**: Image format is YUV420 Semi-Planar, and the data type is **uint8_t**.<br>**AippInputFormat::XRGB8888_U8**: Image format is XRGB8888, and the data type is **uint8_t**.<br>**AippInputFormat::RGB888_U8**: Image format is RGB888, and the data type is **uint8_t**.<br>**AippInputFormat::YUV400_U8**: Image format is YUV400, and the data type is **uint8_t**.<br>enum class AippInputFormat : uint8_t {<br>    YUV420SP_U8 = 0,<br>    XRGB8888_U8 = 1,<br>    RGB888_U8 = 4,<br>    YUV400_U8 = 9,<br>}; |
| config | Input | Image preprocessing parameters of type **AippParams**. The structure is defined as follows:<br>template \<typename T\><br>struct AippParams {<br>    AippPaddingParams\<T\> paddingParams;<br>    AippSwapParams swapParams;<br>    AippSingleLineParams singleLineParams;<br>    AippDataTypeConvParams dtcParams;<br>    AippChannelPaddingParams\<T\> cPaddingParams;<br>    AippColorSpaceConvParams cscParams;<br>};<br>The sub-structures within the **AippParams** structure are defined as follows:<br>&nbsp;&nbsp;&bull;Parameters related to the data padding function. See Table 3.<br>template \<typename T\><br>struct AippPaddingParams {<br>    uint32_t paddingMode;<br>    T paddingValueCh0;<br>    T paddingValueCh1;<br>    T paddingValueCh2;<br>    T paddingValueCh3;<br>};<br>&nbsp;&nbsp;&bull;Parameters related to the channel swap function. See Table 4.<br>struct AippSwapParams {<br>    bool isSwapRB;<br>    bool isSwapUV;<br>    bool isSwapAX;<br>};<br>&nbsp;&nbsp;&bull;Parameters related to the single-row read function. See Table 5.<br>struct AippSingleLineParams {<br>    bool isSingleLineCopy;<br>};<br>&nbsp;&nbsp;&bull;Parameters related to the data type conversion function. See Table 6.<br>struct AippDataTypeConvParams {<br>    uint8_t dtcMeanCh0{ 0 };<br>    uint8_t dtcMeanCh1{ 0 };<br>    uint8_t dtcMeanCh2{ 0 };<br>    half dtcMinCh0{ 0 };<br>    half dtcMinCh1{ 0 };<br>    half dtcMinCh2{ 0 };<br>    half dtcVarCh0{ 1.0 };<br>    half dtcVarCh1{ 1.0 };<br>    half dtcVarCh2{ 1.0 };<br>    uint32_t dtcRoundMode{ 0 };<br>};<br>&nbsp;&nbsp;&bull;Parameters related to the channel padding function. See Table 7.<br>template \<typename T\><br>struct AippChannelPaddingParams {<br>    uint32_t cPaddingMode;<br>    T cPaddingValue;<br>};<br>&nbsp;&nbsp;&bull;Parameters related to the color space conversion function. See Table 8.<br>struct AippColorSpaceConvParams {<br>    bool isEnableCsc;<br>    int16_t cscMatrixR0C0;<br>    int16_t cscMatrixR0C1;<br>    int16_t cscMatrixR0C2;<br>    int16_t cscMatrixR1C0;<br>    int16_t cscMatrixR1C1;<br>    int16_t cscMatrixR1C2;<br>    int16_t cscMatrixR2C0;<br>    int16_t cscMatrixR2C1;<br>    int16_t cscMatrixR2C2;<br>    uint8_t cscBiasIn0;<br>    uint8_t cscBiasIn1;<br>    uint8_t cscBiasIn2;<br>    uint8_t cscBiasOut0;<br>    uint8_t cscBiasOut1;<br>    uint8_t cscBiasOut2;<br>}; |

**Table 3** Parameter description within the AippPaddingParams structure<a name="table8955841508"></a>

| Parameter | Input/Output | Description |
| --- | --- | --- |
| paddingMode | Input | Padding mode, with a value range of [0, 3] and a default value of 0.<br>**0**: Constant padding mode, which supports only left and right padding.<br>**1**: Row-column copy mode.<br>**2**: Block copy mode.<br>**3**: Mirror block copy mode. |
| paddingValueCh0 | Input | Data filled in channel0 of the padding area. Valid only in constant padding mode. The data type is **T**, and the default value is 0. |
| paddingValueCh1 | Input | Data filled in channel1 of the padding area. Valid only in constant padding mode. The data type is **T**, and the default value is 0. |
| paddingValueCh2 | Input | Data filled in channel2 of the padding area. Valid only in constant padding mode. The data type is **T**, and the default value is 0. |
| paddingValueCh3 | Input | Data filled in channel3 of the padding area. Valid only in constant padding mode. The data type is **T**, and the default value is 0. |

**Table 4** Parameter description within the AippSwapParams structure<a name="table679014222918"></a>

| Parameter | Input/Output | Description |
| --- | --- | --- |
| isSwapRB | Input | For RGB888 and XRGB8888 formats, whether to swap the R and B channels. The default value is **false**. |
| isSwapUV | Input | For the YUV420SP format, whether to swap the U and V channels. The default value is **false**. |
| isSwapAX | Input | For the XRGB8888 format, whether to shift the X channel backward, that is, XRGB→RGBX. The default value is **false**. |

**Table 5** Parameter description within the AippSingleLineParams structure<a name="table193501032193419"></a>

| Parameter | Input/Output | Description |
| --- | --- | --- |
| isSingleLineCopy | Input | Whether to enable single-row read mode. When enabled, only one row is read from the source image. Default value: false. |

**Table 6** Parameter description of the AippDataTypeConvParams structure<a name="table14611192613519"></a>

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dtcMeanCh0 | Input | Mean value in the calculation formula, channel0, of the uint8_t data type. Default value: 0. |
| dtcMeanCh1 | Input | Mean value in the calculation formula, channel1, of the uint8_t data type. Default value: 0. |
| dtcMeanCh2 | Input | Mean value in the calculation formula, channel2, of the uint8_t data type. Default value: 0. |
| dtcMinCh0 | Input | Min value in the calculation formula, channel0, of the half data type. Default value: 0.<br>Atlas 200I/500 A2 inference products do not support configuring this parameter. |
| dtcMinCh1 | Input | Min value in the calculation formula, channel1, of the half data type. Default value: 0.<br>Atlas 200I/500 A2 inference products do not support configuring this parameter. |
| dtcMinCh2 | Input | Min value in the calculation formula, channel2, of the half data type. Default value: 0.<br>Atlas 200I/500 A2 inference products do not support configuring this parameter. |
| dtcVarCh0 | Input | Var value in the calculation formula, channel0, of the half data type. Default value: 1.0. |
| dtcVarCh1 | Input | Var value in the calculation formula, channel1, of the half data type. Default value: 1.0. |
| dtcVarCh2 | Input | Var value in the calculation formula, channel2, of the half data type. Default value: 1.0. |
| dtcRoundMode | Input | Controls the mode of data type conversion performed by dtc, of the uint32_t data type. Default value: 0.<br>0: Rounds to the nearest integer (C round).<br>1: Rounds to the nearest even number (C rint). |

> [!NOTE]
> The dtcRoundMode parameter can be configured only on Atlas 200I/500 A2 inference products.

**Table 7** Parameter description of the AippChannelPaddingParams structure<a name="table163681812917"></a>

| Parameter | Input/Output | Description |
| --- | --- | --- |
| cPaddingMode | Input | Type of channel padding. Value range: [0, 1]. Default value: 0.<br>0: Pad to 32B. That is, when the output data type U is int8_t or uint8_t, pad to 32 channels; when it is half, pad to 16 channels.<br>1: Pad to 4 channels. |
| cPaddingValue | Input | Value used for channel padding, of the T data type. Default value: 0. |

**Table 8** Parameter description of the AippColorSpaceConvParams structure<a name="table7858175271018"></a>

| Parameter | Input/Output | Description |
| --- | --- | --- |
| isEnableCsc | Input | Whether to enable color space conversion. Default value: false. |
| cscMatrixR0C0 | Input | Color space conversion matrix cscMatrix[0][0]. |
| cscMatrixR0C1 | Input | Color space conversion matrix cscMatrix[0][1]. |
| cscMatrixR0C2 | Input | Color space conversion matrix cscMatrix[0][2]. |
| cscMatrixR1C0 | Input | Color space conversion matrix cscMatrix[1][0]. |
| cscMatrixR1C1 | Input | Color space conversion matrix cscMatrix[1][1]. |
| cscMatrixR1C2 | Input | Color space conversion matrix cscMatrix[1][2]. |
| cscMatrixR2C0 | Input | Color space conversion matrix cscMatrix[2][0]. |
| cscMatrixR2C1 | Input | Color space conversion matrix cscMatrix[2][1]. |
| cscMatrixR2C2 | Input | Color space conversion matrix cscMatrix[2][2]. |
| cscBiasIn0 | Input | RGB-to-YUV bias cscBiasIn[0]. Invalid for YUV-to-RGB conversion. |
| cscBiasIn1 | Input | RGB-to-YUV bias cscBiasIn[1]. Invalid for YUV-to-RGB conversion. |
| cscBiasIn2 | Input | RGB-to-YUV bias cscBiasIn[2]. Invalid for YUV-to-RGB conversion. |
| cscBiasOut0 | Input | YUV-to-RGB bias cscBiasOut0[0]. Invalid for RGB-to-YUV conversion. |
| cscBiasOut1 | Input | YUV-to-RGB bias cscBiasOut1[1]. Invalid for RGB-to-YUV conversion. |
| cscBiasOut2 | Input | YUV-to-RGB bias cscBiasOut2[2]. Invalid for RGB-to-YUV conversion. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The address alignment requirements for **src0** and **src1** in Global Memory are as follows:

    <a name="table9587253131611"></a>

| Image Format | src0 | src1 |
| --- | --- | --- |
| YUV420SP | Must be 2-byte aligned | Must be 2-byte aligned |
| XRGB8888 | Must be 4-byte aligned | - |
| RGB888 | No alignment requirement | - |
| YUV400 | No alignment requirement | - |

- For data in the XRGB input format, the chip discards the fourth channel by default and outputs data in the RGB format. If X is in channel0, the X Channel Shift function must be enabled to convert the input channel to RGBX; conversely, if X is in channel3, the X Channel Shift function must be disabled to output data in the RGB format.

<!-- npu="310p" id8 -->
## Example<a name="section6461234123118"></a>

**SetAippFunctions** must be used together with the [LoadImageToLocal](LoadImageToLocal.md) API. For a complete example (including the data transfer process), see [Example of LoadImageToLocal](LoadImageToLocal.md#section22811728184217).
<!-- end id8 -->
