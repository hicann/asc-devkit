# TConv3DBpFilterTiling Structure

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:29:24.485Z -->

The **TConv3DBpFilterTiling** structure contains the specification information of the **Conv3dBackpropFilter** operator and the related parameters of the tiling partition algorithm. It is passed to the **Conv3dBackpropFilter** Kernel side for data partitioning, data transfer, and computation. For the parameter description of the **TConv3DBpFilterTiling** structure, see [Table 1](#table1563162142915).

Users obtain the **TConv3DBpFilterTiling** structure by calling the [GetTiling](GetTiling-128.md) API. For details, see [Usage Guidelines](conv3dbackpropfilter_tiling_usage_guidelines.md). Currently, customizing the parameters in the **TConv3DBpFilterTiling** structure is not supported.

**Table 1**  TConv3DBpFilterTiling Structure Description

<a name="table1563162142915"></a>
| Parameter | Description |
| --- | --- |
| batch | Batch of the input GradOutput, in elements. |
| cin | Channel of the input Input, in elements. |
| cout | Channel of the input GradOutput, in elements. |
| cin1G | Reserved parameter. Users do not need to be aware of it. |
| cout1G | Reserved parameter. Users do not need to be aware of it. |
| dout | Depth of the input GradOutput, in elements. |
| ho | Height of the input GradOutput, in elements. |
| wo | Width of the input GradOutput, in elements. |
| di | Depth of the input Input, in elements. |
| hi | Height of the input Input, in elements. |
| wi | Width of the input Input, in elements. |
| dk | Depth of the output Weight, in elements. |
| hk | Height of the output Weight, in elements. |
| wk | Width of the output Weight, in elements. |
| group | Reserved parameter. Users do not need to be aware of it. |
| strideD | Depth of the Stride in convolution backward computation, in elements. |
| strideH | Height of the Stride in convolution backward computation, in elements. |
| strideW | Width of the Stride in convolution backward computation, in elements. |
| padFront | Front direction of the Depth dimension of Padding in convolution backward computation, in elements. |
| padBack | Back direction of the Depth dimension of Padding in convolution backward computation, in elements. |
| padUp | Up direction of the Height dimension of Padding in convolution backward computation, in elements. |
| padDown | Down direction of the Height dimension of Padding in convolution backward computation, in elements. |
| padLeft | Left direction of the Width dimension of Padding in convolution backward computation, in elements. |
| padRight | Right direction of the Width dimension of Padding in convolution backward computation, in elements. |
| dilationD | Depth of Dilation in convolution backward computation, in elements. |
| dilationH | Height of Dilation in convolution backward computation, in elements. |
| dilationW | Width of Dilation in convolution backward computation, in elements. |
| channelSize | Size of C0 under the current input data type. This parameter currently supports only the value 16. |
| al0Pbuffer | 1 indicates that DoubleBuffer is disabled, and 2 indicates that DoubleBuffer is enabled. |
| bl0Pbuffer | 1 indicates that DoubleBuffer is disabled, and 2 indicates that DoubleBuffer is enabled. |
| cl0Pbuffer | 1 indicates that DoubleBuffer is disabled, and 2 indicates that DoubleBuffer is enabled. |
| al1Pbuffer | 1 indicates that DoubleBuffer is disabled, and 2 indicates that DoubleBuffer is enabled. |
| bl1Pbuffer | 1 indicates that DoubleBuffer is disabled, and 2 indicates that DoubleBuffer is enabled. |
| baseM | M direction size on L0, in elements. |
| baseK | K direction size on L0, in elements. |
| baseN | N direction size on L0, in elements. |
| m0 | Minimum partition M direction size on L0. |
| k0 | Minimum partition K direction size on L0. |
| n0 | Minimum partition N direction size on L0. |
| stepM | Multiple of baseM in the M direction of the buffer cached in L1 for the matrix. |
| stepN | Multiple of baseN in the N direction of the buffer cached in L1 for the matrix. |
| stepKa | Multiple of baseK in the K direction of the buffer cached in L1 for the matrix. |
| stepKb | Multiple of baseK in the K direction of the buffer cached in L1 for the matrix. |
| iterateOrder | Reserved parameter. Users do not need to be aware of it. |
| bl1Bound | Maximum amount of data of the GradOutput matrix loaded into L1. |
| hf32Flag | Reserved parameter. Users do not need to be aware of it. |
| singleCoreDK | Reserved parameter. Users do not need to be aware of it. |
| singleCoreGroup | Reserved parameter. Users do not need to be aware of it. |
| singleCoreCout | Size of the amount of cout data calculated in the M direction on a single core, in elements. |
| singleCoreHo | Size of the amount of ho data calculated in the K direction on a single core, in elements. |
| singleCoreBatch | Size of batch on a single core, in elements. |
| singleCoreCin | Size of the amount of cin data calculated in the N direction on a single core, in elements. |
| totalL1Size | L1 size, in elements. |
| singleCoreM | Size of M on a single core, in elements. |
| singleCoreN | Size of N on a single core, in elements. |
| singleCoreK | Size of K on a single core, in elements. |
