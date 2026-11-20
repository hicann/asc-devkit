# TConv3DBackpropInputTiling Struct

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-10T06:29:39.268Z pushedAt=2026-11-18T11:07:26.738Z nl=2 -->

The TConv3DBackpropInputTiling struct contains the specification information of the Conv3DBackpropInput operator and the related parameters of the Tiling algorithm. It is passed to the Conv3DBackpropInput kernel side for data splitting, data transfer, and computation. The following table describes the parameters of the TConv3DBackpropInputTiling struct.

You can obtain the TConv3DBackpropInputTiling struct by calling the [GetTiling](GetTiling.md) API. For details, see [Conv3DBackpropInput Tiling Usage Guidelines](Conv3DBackpropInput_Tiling_usage.md). Currently, custom configuration of the parameters in the TConv3DBackpropInputTiling struct is not supported.

**Table 1**  TConv3DBackpropInputTiling struct description

| Parameter Name | Description |
| --- | --- |
| batch | N of the input GradOutput, which equals the N of the forward input Input of the convolution. |
| cin | Channel of the output GradInput, which equals the Channel of the forward input Input of the convolution. |
| cout | Channel of the input GradOutput. |
| cout1 | C1 of the input GradOutput, equal to cout/c0. |
| cin1 | C1 of the output GradInput, equal to the C1 of the forward input Input of the convolution, equal to cin/c0. |
| cout1G | Reserved parameter. You do not need to be aware of it. |
| cin1G | Reserved parameter. You do not need to be aware of it. |
| c0 | Size of C0 under the current input data type. Currently, this parameter supports only the value 16. |
| c0Bits | Number of right-shift bits equivalent to dividing any number by c0. For example, if c0=8, c0Bits=3; if c0=16, c0Bits=4. |
| dout | Depth size of the input GradOutput, in unit elements. |
| ho | Height size of the input GradOutput, in unit elements. |
| wo | Width size of the input GradOutput, in unit elements. |
| di | Depth size of the output GradInput, equal to the Depth size of the forward input Input of the convolution, in unit elements. |
| hi | Height size of the output GradInput, equal to the Height size of the forward input Input of the convolution, in unit elements. |
| wi | Width size of the output GradInput, equal to the Width size of the forward input Input of the convolution, in unit elements. |
| dk | Depth size of the input Weight, in unit elements. |
| hk | Height size of the input Weight, in unit elements. |
| wk | Width size of the input Weight, in unit elements. |
| group | Reserved parameter. You do not need to be aware of it. |
| strideD | Depth size of Stride in the convolution backward computation, in unit elements. |
| strideH | StrideHeight size in the convolution backward computation, in unit elements. |
| strideW | StrideWidth size in the convolution backward computation, in unit elements. |
| padFront | Front direction of the Depth dimension of the output matrix GradInput Padding in the convolution backward computation, in unit elements. |
| padBack | Back direction of the Depth dimension of the output matrix GradInput Padding in the convolution backward computation, in unit elements. |
| padUp | Up direction of the Height dimension of the output matrix GradInput Padding in the convolution backward computation, in unit elements. |
| padDown | Down direction of the Height dimension of the output matrix GradInput Padding in the convolution backward computation, in unit elements. |
| padLeft | Left direction of the Width dimension of the output matrix GradInput Padding in the convolution backward computation, in unit elements. |
| padRight | Right direction of the Width dimension of the output matrix GradInput Padding in the convolution backward computation, in unit elements. |
| backpropPadTail | Reserved parameter. You do not need to be aware of it. |
| backpropPadUp | Up direction of the Height dimension of the input matrix GradOutput Padding in the convolution backward computation, in unit elements. |
| backpropPadDown | Down direction of the Height dimension of the input matrix GradOutput Padding in the convolution backward computation, in unit elements. |
| backpropPadLeft | Left direction of the Width dimension of the input matrix GradOutput Padding in the convolution backward computation, in unit elements. |
| backpropPadRight | Right direction of the Width dimension of the input matrix GradOutput Padding in the convolution backward computation, in unit elements. |
| dilationD | Depth size of Dilation in the convolution backward computation, in unit elements. |
| dilationH | Height size of Dilation in the convolution backward computation, in unit elements. |
| dilationW | Width size of Dilation in the convolution backward computation, in unit elements. |
| al0Pbuffer | 1 indicates that DoubleBuffer is not enabled, and 2 indicates that DoubleBuffer is enabled. |
| bl0Pbuffer | 1 indicates that DoubleBuffer is not enabled, and 2 indicates that DoubleBuffer is enabled. |
| cl0Pbuffer | 1 indicates that DoubleBuffer is not enabled, and 2 indicates that DoubleBuffer is enabled. |
| al1Pbuffer | 1 indicates that DoubleBuffer is not enabled, and 2 indicates that DoubleBuffer is enabled. |
| bl1Pbuffer | 1 indicates that DoubleBuffer is not enabled, and 2 indicates that DoubleBuffer is enabled. |
| singleCoreGroup | Reserved parameter. You do not need to be aware of it. |
| singleCoreCout | Size of the cout data amount computed in the M direction on a single core. |
| singleCoreCout1 | Size of cout1 on a single core. |
| singleCoreCin1 | Size of cin1 on a single core. |
| singleCoreDin | Size of Din on a single core. |
| singleCoreHo | Size of the ho data amount computed in the K direction on a single core. |
| baseM | M direction size on L0. |
| baseK | K direction size on L0. |
| baseN | N direction size on L0. |
| baseD | Reserved parameter. You do not need to be aware of it. |
| baseBatch | Reserved parameter. You do not need to be aware of it. |
| baseGroup | Reserved parameter. You do not need to be aware of it. |
| stepM | Multiple of baseM in the M direction of the buffer where the feature matrix is cached in L1. |
| stepN | Multiple of baseN in the N direction of the buffer where the weight matrix is cached in L1. |
| stepKa | Multiple of baseK in the K direction of the buffer where the feature matrix is cached in L1. |
| stepKb | Multiple of baseK in the K direction of the buffer where the weight matrix is cached in L1. |
| stepBatch | Reserved parameter. You do not need to be aware of it. |
| stepGroup | Reserved parameter. You do not need to be aware of it. |
| iterateOrder | Reserved parameter. You do not need to be aware of it. |
| hf32Flag | Reserved parameter. You do not need to be aware of it. |
| initOutputFlag | Reserved parameter. You do not need to be aware of it. |
| reserved | Reserved parameter. You do not need to be aware of it. |
| singleCoreBatch | Reserved parameter. You do not need to be aware of it. |
| singleCoreM | Size of the data amount to be computed in the M direction on a single core. |
| singleCoreCin | Size of the cin data amount computed in the N direction on a single core. |
