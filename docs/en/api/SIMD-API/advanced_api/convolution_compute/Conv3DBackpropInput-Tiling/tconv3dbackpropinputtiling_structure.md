# TConv3DBackpropInputTiling Structure

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:40:55.224Z -->

The **TConv3DBackpropInputTiling** structure contains the specification information of the Conv3DBackpropInput operator and the related parameters of the Tiling algorithm. It is passed to the Conv3DBackpropInput kernel side for data tiling, data transfer, and computation. The following table describes the parameters of the **TConv3DBackpropInputTiling** structure.

The user obtains the **TConv3DBackpropInputTiling** structure by calling the [GetTiling](GetTiling-114.md) API. For details about the process, see [Conv3DBackpropInput Tiling Usage Guidelines](conv3dbackpropinput_tiling_usage_guidelines.md). Currently, custom configuration of the parameters in the **TConv3DBackpropInputTiling** structure is not supported.

**Table 1** Description of the TConv3DBackpropInputTiling structure

| Parameter | Description |
| --- | --- |
| batch | N of the input GradOutput, which equals the N of the forward input Input of the convolution. |
| cin | Channel of the output GradInput, which equals the Channel of the forward input Input of the convolution. |
| cout | Channel of the input GradOutput. |
| cout1 | C1 of the input GradOutput, which equals cout/c0. |
| cin1 | C1 of the output GradInput, which equals the C1 of the forward input Input of the convolution and equals cin/c0. |
| cout1G | Reserved parameter. The user does not need to be aware of it. |
| cin1G | Reserved parameter. The user does not need to be aware of it. |
| c0 | Size of C0 under the current input data type. Currently, this parameter supports only the value 16. |
| c0Bits | Number of right-shift bits equivalent to dividing any number by c0. For example, if c0=8, c0Bits=3; if c0=16, c0Bits=4. |
| dout | Depth size of the input GradOutput, in elements. |
| ho | Height size of the input GradOutput, in elements. |
| wo | Width size of the input GradOutput, in elements. |
| di | Depth size of the output GradInput, which equals the Depth size of the forward input Input of the convolution, in elements. |
| hi | Height size of the output GradInput, which equals the Height size of the forward input Input of the convolution, in elements. |
| wi | Width size of the output GradInput, which equals the Width size of the forward input Input of the convolution, in elements. |
| dk | Depth size of the input Weight, in elements. |
| hk | Height size of the input Weight, in elements. |
| wk | Width size of the input Weight, in elements. |
| group | Reserved parameter. The user does not need to be aware of it. |
| strideD | Depth size of Stride in convolution backward computation, in elements. |
| strideH | StrideHeight size in convolution backward computation, in elements. |
| strideW | StrideWidth size in convolution backward computation, in elements. |
| padFront | Front direction of the Depth dimension of the GradInput Padding of the output matrix in convolution backward computation, in elements. |
| padBack | Back direction of the Depth dimension of the GradInput Padding of the output matrix in convolution backward computation, in elements. |
| padUp | Up direction of the Height dimension of the GradInput Padding of the output matrix in convolution backward computation, in elements. |
| padDown | Down direction of the Height dimension of the GradInput Padding of the output matrix in convolution backward computation, in elements. |
| padLeft | Left direction of the Width dimension of the GradInput Padding of the output matrix in convolution backward computation, in elements. |
| padRight | Right direction of the Width dimension of the GradInput Padding of the output matrix in convolution backward computation, in elements. |
| backpropPadTail | Reserved parameter. The user does not need to be aware of it. |
| backpropPadUp | Up direction of the Height dimension of the GradOutput Padding of the input matrix in convolution backward computation, in elements. |
| backpropPadDown | Down direction of the Height dimension of the GradOutput Padding of the input matrix in convolution backward computation, in elements. |
| backpropPadLeft | Left direction of the Width dimension of the GradOutput Padding of the input matrix in convolution backward computation, in elements. |
| backpropPadRight | Right direction of the Width dimension of the GradOutput Padding of the input matrix in convolution backward computation, in elements. |
| dilationD | Depth size of Dilation in convolution backward computation, in elements. |
| dilationH | Height size of Dilation in convolution backward computation, in elements. |
| dilationW | Width size of Dilation in convolution backward computation, in elements. |
| al0Pbuffer | 1 indicates that DoubleBuffer is disabled, and 2 indicates that DoubleBuffer is enabled. |
| bl0Pbuffer | 1 indicates that DoubleBuffer is disabled, and 2 indicates that DoubleBuffer is enabled. |
| cl0Pbuffer | 1 indicates that DoubleBuffer is disabled, and 2 indicates that DoubleBuffer is enabled. |
| al1Pbuffer | 1 indicates that DoubleBuffer is disabled, and 2 indicates that DoubleBuffer is enabled. |
| bl1Pbuffer | 1 indicates that DoubleBuffer is disabled, and 2 indicates that DoubleBuffer is enabled. |
| singleCoreGroup | Reserved parameter. The user does not need to be aware of it. |
| singleCoreCout | Size of the cout data amount computed in the M direction on a single core. |
| singleCoreCout1 | Size of cout1 on a single core. |
| singleCoreCin1 | Size of cin1 on a single core. |
| singleCoreDin | Size of Din on a single core. |
| singleCoreHo | Size of the ho data amount computed in the K direction on a single core. |
| baseM | Size of the M direction on L0. |
| baseK | Size of the K direction on L0. |
| baseN | Size of the N direction on L0. |
| baseD | Reserved parameter. The user does not need to be aware of it. |
| baseBatch | Reserved parameter. The user does not need to be aware of it. |
| baseGroup | Reserved parameter. The user does not need to be aware of it. |
| stepM | Multiple of baseM in the M direction of the buffer where the feature matrix is cached in L1. |
| stepN | Multiple of baseN in the N direction of the buffer where the weight matrix is cached in L1. |
| stepKa | Multiple of baseK in the K direction of the buffer where the feature matrix is cached in L1. |
| stepKb | Multiple of baseK in the K direction of the buffer where the weight matrix is cached in L1. |
| stepBatch | Reserved parameter. The user does not need to be aware of it. |
| stepGroup | Reserved parameter. The user does not need to be aware of it. |
| iterateOrder | Reserved parameter. The user does not need to be aware of it. |
| hf32Flag | Reserved parameter. The user does not need to be aware of it. |
| initOutputFlag | Reserved parameter. The user does not need to be aware of it. |
| reserved | Reserved parameter. The user does not need to be aware of it. |
| singleCoreBatch | Reserved parameter. The user does not need to be aware of it. |
| singleCoreM | Size of the data amount to be computed in the M direction on a single core. |
| singleCoreCin | Size of the cin data amount computed in the N direction on a single core. |
