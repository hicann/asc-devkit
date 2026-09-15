# LoadImageToLocal<a name="ZH-CN_TOPIC_0000001945534165"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:24:44.559Z -->

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

Loads image data from Global Memory to Local Memory. During the loading process, image preprocessing operations can be performed, including image flipping, image resizing (cropping, edge trimming, scaling, and stretching), color space conversion, and type conversion. The parameters related to image preprocessing are configured through [SetAippFunctions](SetAippFunctions.md).

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void LoadImageToLocal(const LocalTensor<T>& dst, const LoadImageToLocalParams& loadDataParams)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameter description

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| dst | Output | Destination operand, of type **LocalTensor**.<br>The start address of **LocalTensor** must be 32-byte aligned. |
| loadDataParams | Input | LoadData parameter structure, of type **LoadImageToLocalParams**.<br>For the specific definition, see $\{INSTALL_DIR\}/include/ascendc/basic_api/API/kernel_struct_mm.h. Replace $\{INSTALL_DIR\} with the path where the CANN software is stored after installation.<br>For parameter description, see [Table 2](#table8955841508). |

<a name="table8955841508"></a>
**Table 2** Parameter description of the LoadImageToLocalParams structure

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| horizSize | Input | Horizontal width of the image loaded from the source image, in pixels. Value range: horizSize∈[2, 4095]. |
| vertSize | Input | Vertical height of the image loaded from the source image, in pixels. Value range: vertSize∈[2, 4095]. |
| horizStartPos | Input | Horizontal start address of the loaded image on the source image, in pixels. Value range: horizStartPos∈[0, 4095]. The default value is 0.<br>**Note: When the input image is in YUV420SP, XRGB8888, RGB888, or YUV400 format, this parameter must be an even number.** |
| vertStartPos | Input | Vertical start address of the loaded image on the source image, in pixels. Value range: vertStartPos∈[0, 4095]. The default value is 0.<br>**Note: When the input image is in YUV420SP format, this parameter must be an even number.** |
| srcHorizSize | Input | Horizontal width of the source image, in pixels. Value range: srcHorizSize∈[2, 4095].<br>**Note: When the input image is in YUV420SP format, this parameter must be an even number.** |
| topPadSize | Input | Pixel count padded at the top of the destination image. Value range: topPadSize∈[0, 32]. The default value is 0. Used for data padding. You need to first call **SetAippFunctions** to configure the padding value through **AippPaddingParams**, and then configure the padding range through **topPadSize**, **botPadSize**, **leftPadSize**, and **rightPadSize**. |
| botPadSize | Input | Pixel count padded at the bottom of the destination image. Value range: botPadSize∈[0, 32]. The default value is 0. |
| leftPadSize | Input | Pixel count padded on the left of the destination image. Value range: leftPadSize∈[0, 32]. The default value is 0. |
| rightPadSize | Input | Pixel count padded on the right of the destination image. Value range: rightPadSize∈[0, 32]. The default value is 0. |
| sid | Input | Reserved parameter. Reserved for future functions. Developers do not need to pay attention to it for now and can use the default value. |

## Data Type

<!-- npu="950" id8 -->Ascend 950PR/Ascend 950DT: Supported data types are int8_t, uint8_t, and half.<!-- end id8 --><br>
<!-- npu="A3" id9 -->Atlas A3 training products/Atlas A3 inference products: Supported data types are int8_t and half.<!-- end id9 --><br>
<!-- npu="910b" id10 -->Atlas A2 training products/Atlas A2 inference products: Supported data types are int8_t and half.<!-- end id10 --><br>
<!-- npu="310b" id11 -->Atlas 200I/500 A2 inference products: Supported data types are int8_t, uint8_t, and half.<!-- end id11 --><br>
<!-- npu="310p" id12 -->Atlas inference products AI Core: Supported data types are int8_t, uint8_t, and half.<!-- end id12 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For the address alignment requirements of operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- The size of the image loaded to **dst** plus the padding size must be less than or equal to the size of the storage space where it resides.
- When the padding mode is configured as block padding mode or mirrored block padding mode through [SetAippFunctions](SetAippFunctions.md), because the padding data comes from the cropped image, the left and right padding lengths (**leftPadSize** and **rightPadSize**) must be less than or equal to the horizontal length of the crop (**horizSize**), and the top and bottom padding lengths (**topPadSize** and **botPadSize**) must be less than or equal to the vertical length of the crop (**vertSize**).
- The supported physical storage locations are: L1 Buffer (**TPosition**: A1/B1).
    <!-- npu="950" id13 -->
    - Specifically for Ascend 950PR/Ascend 950DT, the supported physical storage locations are: **VECIN**, **VECCALC**, and **VECOUT**.
    <!-- end id13 -->

## Example<a name="section22811728184217"></a>

<!-- npu="310p" id14 -->
The running platform supported by this example is the AI Core of Atlas inference products, and the example image format is YUV420SP.

```cpp
constexpr uint16_t imageWidth = 32;
constexpr uint16_t imageHeight = 32;
constexpr uint32_t yPlaneSize = imageWidth * imageHeight;
constexpr uint32_t inputSize = yPlaneSize * 3 / 2;
constexpr uint32_t outputChannels = 32;
constexpr uint32_t dstElemCount = imageWidth * imageHeight * outputChannels;

// Source operand: a 32x32 YUV420SP image in GM, where the first 1024B is the Y plane and the last 512B is the UV plane.
AscendC::GlobalTensor<uint8_t> fmGlobal;
fmGlobal.SetGlobalBuffer((__gm__ uint8_t *)src, inputSize);

// Destination operand: L1 Buffer.
AscendC::LocalTensor<int8_t> featureMapA1(AscendC::TPosition::A1, a1Addr, dstElemCount);

// format = YUV420SP_U8, indicating that the input is a YUV420 Semi-Planar image of the uint8_t type.
AscendC::AippInputFormat inputFormat = AscendC::AippInputFormat::YUV420SP_U8;

AscendC::AippParams<int8_t> aippConfig;
// This example does not perform HW padding, channel swap, single-line read, mean/scale, or color space conversion. These sub-parameters use the default values of the structure.
// When the output U = int8_t, pad the channels to 32 channels, that is, each pixel outputs 32B.
aippConfig.cPaddingParams.cPaddingMode = 0;
// Pad the channels with 0, and write 0 to the extra channels of the destination Tensor.
aippConfig.cPaddingParams.cPaddingValue = static_cast<int8_t>(0);

// src0 is the start address of the Y plane; src1 is the start address of the UV plane. The UV offset of a 32x32 YUV420SP image is 32 * 32 = 1024 uint8_t.
AscendC::SetAippFunctions(fmGlobal, fmGlobal[yPlaneSize], inputFormat, aippConfig);

AscendC::LoadImageToLocalParams loadParams;
// Load a width of 32 pixels from the source image, which equals the width of the entire 32x32 example image.
loadParams.horizSize = 32;
// Load 32 pixels in height from the source image, which equals the height of the entire 32x32 sample image.
loadParams.vertSize = 32;
// The horizontal start position is pixel 0; the YUV420SP scenario requires an even number, and 0 satisfies the constraint.
loadParams.horizStartPos = 0;
// The vertical start position is row 0; the YUV420SP scenario requires an even number, and 0 satisfies the constraint.
loadParams.vertStartPos = 0;
// The width of each row in the source image is 32 pixels; the YUV420SP scenario requires an even number, and 32 satisfies the constraint.
loadParams.srcHorizSize = 32;
// Do not apply padding in the HW direction at the top.
loadParams.topPadSize = 0;
// Do not apply padding in the HW direction at the bottom.
loadParams.botPadSize = 0;
// Do not apply padding in the HW direction on the left.
loadParams.leftPadSize = 0;
// Do not apply padding in the HW direction on the right.
loadParams.rightPadSize = 0;
// Reserved parameter, fixed to 0.
loadParams.sid = 0;

AscendC::LoadImageToLocal(featureMapA1, loadParams);
```
<!-- end id14 -->
