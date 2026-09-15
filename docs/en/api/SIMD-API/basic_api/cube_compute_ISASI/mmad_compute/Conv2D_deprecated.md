# Conv2D (Deprecated)

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-08-27T16:44:37.723Z -->

## Applicable Products

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
- Atlas 200I/500 A2 inference products: Not supported
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

## Description

**This API is deprecated and will be removed in a later version. Do not use it.**

Computes the 2-D convolution of the given input tensor and weight tensor, and outputs the result tensor. The **Conv2D** convolution layer is mostly used for image recognition, where filters are used to extract features from images.

## Function Prototype

```cpp
template <typename T, typename U>
__aicore__ inline void Conv2D(const LocalTensor<T>& dst, const LocalTensor<U>& featureMap, const LocalTensor<U>& weight, Conv2dParams& conv2dParams, Conv2dTilling& tilling)
```

The tiling structure in the input parameters must be obtained through the following split scheme calculation API:

```cpp
template <typename T>
__aicore__ inline Conv2dTilling GetConv2dTiling(Conv2dParams& conv2dParams)
```

## Parameters

**Table 1** API parameter description

| Parameter | Type | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br><!-- npu="910" id8 -->Atlas training products, supported TPosition: CO1, CO2<!-- end id8 --><br><!-- npu="310p" id9 -->Atlas inference products AI Core, supported TPosition: CO1, CO2<!-- end id9 --><br><br>The valid tensor format in the result is [Cout/16, Ho, Wo, 16], with a size of Cout \* Ho \* Wo, where Ho and Wo can be calculated from other data.<br>Ho = floor((H + pad_top + pad_bottom - dilation_h \* (Kh - 1) - 1) / stride_h + 1)<br>Wo = floor((W + pad_left + pad_right - dilation_w \* (Kw - 1) - 1) / stride_w + 1)<br>Because the hardware requires Ho\*Wo to be a multiple of 16, when applying for the dst Tensor, the shape should be aligned upward to 16, and the actual shape size to apply for should be Cout \* round_howo.<br>round_howo = ceil(Ho \* Wo /16) \* 16. |
| featureMap | Input | Input tensor, with the Tensor TPosition being A1.<br><br>Shape of the input tensor "feature_map", in the format [C1, H, W, C0].<br>C1\*C0 is the number of input channels, with the following requirements:<br>&bull;When the data type of feature_map is half, C0=16.<br>&bull;When the data type of feature_map is int8_t, C0=32.<br>&bull; C1 value range: [1,4]; input channel range: [16, 32, 64, 128].<br><br>H is the height, with a value range of [1,40].<br>W is the width, with a value range of [1,40]. |
| weight | Input | Convolution kernel (weight) tensor, with the Tensor TPosition being B1.<br><br>Shape of the convolution kernel tensor "weight", in the format [C1, Kh, Kw, Cout, C0].<br>C1\*C0 is the number of input channels, with the following requirements for C0:<br>&bull;When the data type of feature_map is half, C0=16.<br>&bull;When the data type of feature_map is int8_t, C0=32.<br>&bull; C1 value range: [1,4].<br>&bull; The number of input channels of kernel_shape must be consistent with that of fm_shape.<br><br>Cout is the number of convolution kernels, with a value range of [16, 32, 64, 128]; Cout must be a multiple of 16.<br>Kh is the convolution kernel height, with a value range of [1,5].<br>Kw is the convolution kernel width, with a value range of [1,5]. |
| conv2dParams | Input | State parameters such as the input cube shape, of type Conv2dParams. The structure is defined as follows:<br><br><br>struct Conv2dParams {<br>    uint32_t imgShape[CONV2D_IMG_SIZE];       // [H, W]<br>    uint32_t kernelShapeIn[CONV2D_KERNEL_SIZE]; // [Kh, Kw]<br>    uint32_t stride[CONV2D_STRIDE];          // [stride_h, stride_w]<br>    uint32_t cin;                            // cin = C0 * C1;<br>    uint32_t cout;<br>    uint32_t padList[CONV2D_PAD];       // [pad_left, pad_right, pad_top, pad_bottom]<br>    uint32_t dilation[CONV2D_DILATION]; // [dilation_h, dilation_w]<br>    uint32_t initY;<br>    uint32_t partialSum;<br>};<br><br>|
| tilling | Input | Fractal control parameters, of type Conv2dTilling. The structure is defined as follows:<br><br>struct Conv2dTilling {<br>    const uint32_t blockSize = 16; // # M block size is always 16<br>    LoopMode loopMode = LoopMode::MODE_NM;<br><br>    uint32_t c0Size = 32;<br>    uint32_t dTypeSize = 1;<br><br>    uint32_t strideH = 0;<br>    uint32_t strideW = 0;<br>    uint32_t dilationH = 0;<br>    uint32_t dilationW = 0;<br>    uint32_t hi = 0;<br>    uint32_t wi = 0;<br>    uint32_t ho = 0;<br>    uint32_t wo = 0;<br><br>    uint32_t height = 0;<br>    uint32_t width = 0;<br><br>    uint32_t howo = 0;<br><br>    uint32_t mNum = 0;<br>    uint32_t nNum = 0;<br>    uint32_t kNum = 0;<br><br>    uint32_t mBlockNum = 0;<br>    uint32_t kBlockNum = 0;<br>    uint32_t nBlockNum = 0;<br><br>    uint32_t roundM = 0;<br>    uint32_t roundN = 0;<br>    uint32_t roundK = 0;<br><br>    uint32_t mTileBlock = 0;<br>    uint32_t nTileBlock = 0;<br>    uint32_t kTileBlock = 0;<br><br>    uint32_t mIterNum = 0;<br>    uint32_t nIterNum = 0;<br>    uint32_t kIterNum = 0;<br><br>    uint32_t mTileNums = 0;<br><br>    bool mHasTail = false;<br>    bool nHasTail = false;<br>    bool kHasTail = false;<br><br>    uint32_t kTailBlock = 0;<br>    uint32_t mTailBlock = 0;<br>    uint32_t nTailBlock = 0;<br><br>    uint32_t mTailNums = 0;<br>};<br><br>|

**Table 2** Parameter description of the Conv2DParams structure:

| Parameter | Type | Description |
| --- | --- | --- |
| imgShape | vector&lt;int&gt; | Shape of the input tensor "feature_map", in the format [H, W].<br>&bull; H is the height, with a value range of [1,40].<br>&bull; W is the width, with a value range of [1,40]. |
| kernelShape | vector&lt;int&gt; | Shape of the convolution kernel tensor "weight", in the format [Kh, Kw].<br>&bull; Kh is the height, with a value range of [1,5].<br>&bull; Kw is the width, with a value range of [1,5]. |
| stride | vector&lt;int&gt; | Convolution stride, in the format [stride_h, stride_w].<br>&bull; stride_h indicates the stride height, with a value range of [1,4].<br>&bull; stride_w indicates the stride width, with a value range of [1,4]. |
| cin | int | Fractal layout parameter, Cin = C1 \* C0, where Cin is the number of input channels and C1 has a value range of [1,4].<br>&bull; When the data type of feature_map is float, C0=8. Input channel range: [8, 16, 24, 32].<br>&bull; When the data type of feature_map is half, C0=16. Input channel range: [16, 32, 48, 64].<br>&bull; When the data type of feature_map is int8_t, C0=32. Input channel range: [32, 64, 96, 128]. |
| cout | int | Cout is the number of convolution kernels, with a value range of [16, 32, 64, 128]; Cout must be a multiple of 16. |
| padList | vector&lt;int&gt; | Number of padding rows/columns, in the format [pad_left, pad_right, pad_top, pad_bottom].<br>&bull; pad_left is the number of padding columns on the left side of feature_map, with a range of [0,4]. pad_right is the number of padding columns on the right side of feature_map, with a range of [0,4].<br>&bull; pad_top is the number of padding rows on the top of feature_map, with a range of [0,4].<br>&bull; pad_bottom is the number of padding rows on the bottom of feature_map, with a range of [0,4]. |
| dilation | vector&lt;int&gt; | Dilated convolution parameters, in the format [dilation_h, dilation_w].<br>&bull; dilation_h is the dilation height, with a range of [1,4].<br>&bull; dilation_w is the dilation width, with a range of [1,4].<br><br>After dilation, the convolution kernel width is dilation_w \* (Kw - 1) + 1, and the height is dilation_h \* (Kh - 1) + 1. |
| initY | uint32_t | Indicates whether dst needs to be initialized.<br>&bull; Value 0: bias is not used, the L0C Buffer needs to be initialized, the dst initial cube retains previous results, and the new computation result is accumulated onto the previous Conv2D computation result.<br>&bull; Value 1: bias is not used, the L0C Buffer does not need to be initialized, the data in the dst initial cube is meaningless, and the computation result directly overwrites the data in dst. |
| partialSum | uint32_t | When the TPosition of the dst parameter is CO2, this parameter controls whether the computation result is moved out.<br>&bull; Value 0: move out the computation result.<br>&bull; Value 1: do not move out the computation result, allowing subsequent computation. |

**Table 3** Parameter description of the Conv2dTilling structure

| Parameter | Type | Description |
| --- | --- | --- |
| blockSize | uint32_t | Fixed value, always 16, indicating the number of elements stored in one dimension. |
| loopMode | LoopMode | Traversal mode. The structure is defined as follows:<br><br>enum class LoopMode {<br>    MODE_NM = 0,<br>    MODE_MN = 1,<br>    MODE_KM = 2,<br>    MODE_KN = 3<br>};<br><br>|
| c0Size | uint32_t | Byte length of one block, with a range of [16 or 32]. |
| dtypeSize | uint32_t | Byte length of the input data type, with a range of [1, 2]. |
| strideH | uint32_t | Convolution stride height, with a range of [1,4]. |
| strideW | uint32_t | Convolution stride width, with a range of [1,4]. |
| dilationH | uint32_t | Dilated convolution parameter height, with a range of [1,4]. |
| dilationW | uint32_t | Dilated convolution parameter width, with a range of [1,4]. |
| hi | uint32_t | feature_map shape height, with a range of [1,40]. |
| wi | uint32_t | feature_map shape width, with a range of [1,40]. |
| ho | uint32_t | feature_map shape height, with a range of [1,40]. |
| wo | uint32_t | feature_map shape width, with a range of [1,40]. |
| height | uint32_t | weight shape height, with a range of [1,5]. |
| width | uint32_t | weight shape width, with a range of [1,5]. |
| howo | uint32_t | feature_map shape size, which is ho \* wo. |
| mNum | uint32_t | M-axis equivalent data length parameter value, with a range of [1,4096]. |
| nNum | uint32_t | N-axis equivalent data length parameter value, with a range of [1,4096]. |
| kNum | uint32_t | K-axis equivalent data length parameter value, with a range of [1,4096]. |
| roundM | uint32_t | M-axis equivalent data length parameter value rounded up to a multiple of blockSize, with a range of [1,4096]. |
| roundN | uint32_t | N-axis equivalent data length parameter value rounded up to a multiple of blockSize, with a range of [1,4096]. |
| roundK | uint32_t | K-axis equivalent data length parameter value rounded up to a multiple of c0Size, with a range of [1,4096]. |
| mBlockNum | uint32_t | Number of M-axis blocks, mBlockNum = mNum / blockSize, with a range of [1,4096]. |
| nBlockNum | uint32_t | Number of N-axis blocks, nBlockNum = nNum / blockSize, with a range of [1,4096]. |
| kBlockNum | uint32_t | Number of K-axis blocks, kBlockNum = kNum / blockSize, with a range of [1,4096]. |
| mIterNum | uint32_t | Number of M-axis dimensions to traverse, with a range of [1,4096]. |
| nIterNum | uint32_t | Number of N-axis dimensions to traverse, with a range of [1,4096]. |
| kIterNum | uint32_t | Number of K-axis dimensions to traverse, with a range of [1,4096]. |
| mTileBlock | uint32_t | Number of M-axis split blocks, with a range of [1,4096]. |
| nTileBlock | uint32_t | Number of N-axis split blocks, with a range of [1,4096]. |
| kTileBlock | uint32_t | Number of K-axis split blocks, with a range of [1,4096]. |
| kTailBlock | uint32_t | Number of K-axis tail blocks, with a range of [1,4096]. |
| mTailBlock | uint32_t | Number of M-axis tail blocks, with a range of [1,4096]. |
| nTailBlock | uint32_t | Number of N-axis tail blocks, with a range of [1,4096]. |
| kHasTail | bool | Whether the K axis has a tail block. |
| mHasTail | bool | Whether the M axis has a tail block. |
| nHasTail | bool | Whether the N axis has a tail block. |
| mTileNums | uint32_t | Length of the number of M-axis split blocks, with a range of [1,4096]. |
| mTailNums | uint32_t | Length of the number of M-axis tail blocks, with a range of [1,4096]. |

## Data Types

**Table 4** Data type combinations of feature_map, weight, and dst

| feature_map.dtype | weight.dtype | dst.dtype |
| --- | --- | --- |
| int8_t | int8_t | int32_t |
| half | half | float |
| half | half | half |

## Return Value

None

## Constraints

- This API currently does not support the scenario where **W** = **Kw** and **H** > **Kh**, which may produce unpredictable results.
- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Example

This API is deprecated. Use the **Mmad** API instead.
