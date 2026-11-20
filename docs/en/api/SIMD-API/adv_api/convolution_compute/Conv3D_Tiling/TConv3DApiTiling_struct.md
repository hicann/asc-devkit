# TConv3DApiTiling Structure

<!-- md-trans-meta sourceCommit=ca3b72f6b4f376702407e982cf2cc174087341f5 translatedAt=2026-11-10T06:24:38.954Z pushedAt=2026-11-18T11:07:26.585Z nl=2 -->

The TConv3DApiTiling structure contains the Conv3D operator specification information and the related parameters of the Tiling split algorithm. It is passed to the Conv3D kernel side for data splitting, data movement, and computation. For the parameter description of the TConv3DApiTiling structure, see [Table 1](#table18244199192620).

You can obtain the TConv3DApiTiling structure by calling the [GetTiling](GetTiling.md) API. For details, see [Conv3D Tiling Usage Guidelines](Conv3D_Tiling_usage.md). Currently, custom configuration of the parameters in the TConv3DApiTiling structure is not supported.

**Table 1**  TConv3DApiTiling structure description

<a name="table18244199192620"></a>

| Parameter Name | Data Type | Description |
| --- | --- | --- |
| groups | uint32_t | Reserved parameter. Currently only 1 is supported. |
| singleCoreDo | uint64_t | Size of Dout processed on a single core. |
| singleCoreCo | uint32_t | Size of Cout processed on a single core. |
| singleCoreM | uint64_t | Size of M processed on a single core. |
| orgDo | uint64_t | Original Dout size in Conv3D computation. |
| orgCo | uint32_t | Original Cout size in Conv3D computation. |
| orgHo | uint64_t | Original Hout size in Conv3D computation. |
| orgWo | uint64_t | Original Wout size in Conv3D computation. |
| orgCi | uint32_t | Original Cin size in Conv3D computation. |
| orgDi | uint64_t | Original Din size in Conv3D computation. |
| orgHi | uint64_t | Original Hin size in Conv3D computation. |
| orgWi | uint64_t | Original Win size in Conv3D computation. |
| kernelD | uint32_t | Original kernel D dimension size of the convolution kernel in Conv3D computation. |
| kernelH | uint32_t | Original kernel H dimension size of the convolution kernel in Conv3D computation. |
| kernelW | uint32_t | Original kernel W dimension size of the convolution kernel in Conv3D computation. |
| strideD | uint32_t | Stride D dimension size in Conv3D computation. |
| strideH | uint32_t | Stride H dimension size in Conv3D computation. |
| strideW | uint32_t | Stride W dimension size in Conv3D computation. |
| dilationD | uint32_t | Dilation D dimension size in Conv3D computation. |
| dilationH | uint32_t | Dilation H dimension size in Conv3D computation. |
| dilationW | uint32_t | Dilation W dimension size in Conv3D computation. |
| padHead | uint32_t | Padding D dimension size in the head direction in Conv3D computation. |
| padTail | uint32_t | Padding D dimension size in the tail direction in Conv3D computation. |
| padUp | uint32_t | Padding H dimension size in the up direction in Conv3D computation. |
| padDown | uint32_t | Padding H dimension size in the down direction in Conv3D computation. |
| padLeft | uint32_t | Padding W dimension size in the left direction in Conv3D computation. |
| padRight | uint32_t | Padding W dimension size in the right direction in Conv3D computation. |
| mL0 | uint32_t | Size of M processed at a time on L0. |
| kL0 | uint32_t | Size of K processed at a time on L0. |
| nL0 | uint32_t | Size of N processed at a time on L0. |
| kAL1 | uint32_t | Actual size of Input K on L1, equal to Cin1InL1 * KH * KW * C0, where Cin1InL1 is the Tiling split size after KD * Cin1 axis merging. |
| kBL1 | uint32_t | Actual size of Weight K on L1, equal to Cin1InL1 * KH * KW * C0, where Cin1InL1 is the Tiling split size after KD * Cin1 axis merging. |
| nBL1 | uint32_t | Actual data size of Weight loaded in the Cout dimension on L1. |
| mAL1 | uint32_t | Actual data size of Input loaded in M on L1. |
| al1FullLoad | uint8_t | Whether to fully load Input data in the L1 buffer.<br><br>0: Input data is not fully loaded in the L1 buffer.<br><br>1: Input data is fully loaded in the L1 buffer. |
| bl1FullLoad | uint8_t | Whether to fully load Weight data in the L1 buffer.<br><br>0: Weight data is not fully loaded in the L1 buffer.<br><br>1: Weight data is fully loaded in the L1 buffer. |
| iterateMNOrder | uint8_t | Output order of the M axis and N axis when outputting the result matrix.<br><br>0: Output the M direction first. Output the M direction first, then the N direction. See Figure 2.<br><br>1: Output the N direction first. Output the N direction first, then the M direction. See Figure 3.<br><br>M consists of Hout and Wout. The output order in the M direction is: output the Wout direction first, then the Hout direction. |
| biasFullLoadFlag | uint8_t | Whether to fully load Bias into the L1 buffer.<br><br>0: No. The size of Bias loaded at a time within a single core equals nL0, the size of the N direction of a single matrix multiplication.<br><br>1: Yes. Bias within a single core is fully loaded at once. |

Note: The M axis above is the vertical axis of the input after img2col expansion during the forward convolution operation, numerically equal to Hout \* Wout. K is the horizontal axis of the input after img2col expansion, numerically equal to KD\*C1\*KH\*KW\*C0. KD/KH/KW are the Depth, Height, and Width of Weight, which are abbreviations of kernelD/kernelH/kernelW. N is the Cout of Weight. See Figure 1 for details.

**Figure 1**  MKN in 3D forward convolution  
![](../../../../figures/conv3d_mkn.png "MKN in 3D forward convolution")

**Figure 2**  MFirst in 3D forward convolution  
![](../../../../figures/conv3d_mfirst.png "MFirst in 3D forward convolution")

**Figure 3**  NFirst in 3D forward convolution  
![](../../../../figures/conv3d_nfirst.png "NFirst in 3D forward convolution")
