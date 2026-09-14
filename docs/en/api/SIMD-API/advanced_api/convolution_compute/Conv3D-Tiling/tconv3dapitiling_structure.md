# TConv3DApiTiling Structure

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:18:52.137Z -->

The **TConv3DApiTiling** structure contains the specification information of the Conv3D operator and the related parameters of the Tiling algorithm. It is passed to the Conv3D kernel side for data splitting, data transfer, and computation. For details about the parameters of the **TConv3DApiTiling** structure, see [Table 1](#table18244199192620).

You can obtain the **TConv3DApiTiling** structure by calling [GetTiling](GetTiling-105.md). For the detailed process, see [Conv3D Tiling Usage Guidelines](conv3d_tiling_usage_guidelines.md). Currently, user-defined configuration of the parameters in the **TConv3DApiTiling** structure is not supported.

**Table 1**  Description of the TConv3DApiTiling structure

<a name="table18244199192620"></a>
| Parameter Name | Data Type | Description |
| --- | --- | --- |
| groups | uint32_t | Reserved parameter. Currently, only 1 is supported. |
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
| padHead | uint32_t | Head direction size of the Padding D dimension in Conv3D computation. |
| padTail | uint32_t | Tail direction size of the Padding D dimension in Conv3D computation. |
| padUp | uint32_t | Up direction size of the Padding H dimension in Conv3D computation. |
| padDown | uint32_t | Down direction size of the Padding H dimension in Conv3D computation. |
| padLeft | uint32_t | Left direction size of the Padding W dimension in Conv3D computation. |
| padRight | uint32_t | Right direction size of the Padding W dimension in Conv3D computation. |
| mL0 | uint32_t | Size of M processed at a time on L0. |
| kL0 | uint32_t | Size of K processed at a time on L0. |
| nL0 | uint32_t | Size of N processed at a time on L0. |
| kAL1 | uint32_t | Actual size of Input K on L1, equal to Cin1InL1 * KH * KW * C0, where Cin1InL1 is the size after Tiling splitting of the combined KD * Cin1 axis. |
| kBL1 | uint32_t | Actual size of Weight K on L1, equal to Cin1InL1 * KH * KW * C0, where Cin1InL1 is the size after Tiling splitting of the combined KD * Cin1 axis. |
| nBL1 | uint32_t | Actual data size of the Cout dimension loaded for Weight on L1. |
| mAL1 | uint32_t | Actual data size of M loaded for Input on L1. |
| al1FullLoad | uint8_t | Whether to fully load the Input data into the L1 buffer.<br><br>0: The Input data is not fully loaded into the L1 buffer.<br><br>1: The Input data is fully loaded into the L1 buffer. |
| bl1FullLoad | uint8_t | Whether to fully load the Weight data into the L1 buffer.<br><br>0: The Weight data is not fully loaded into the L1 buffer.<br><br>1: The Weight data is fully loaded into the L1 buffer. |
| iterateMNOrder | uint8_t | Output order of the M axis and N axis when outputting the result matrix Output.<br><br>0: Output the M direction first. The M direction is output first, followed by the N direction. See Figure 2.<br><br>1: Output the N direction first. The N direction is output first, followed by the M direction. See Figure 3.<br><br>M consists of Hout and Wout. The output order of the M direction is to output the Wout direction first, followed by the Hout direction. |
| biasFullLoadFlag | uint8_t | Whether to fully load the Bias into the L1 buffer.<br><br>0: No. The size of the Bias loaded at a time within a single core is equal to the size nL0 of the N direction of a single matrix multiplication.<br><br>1: Yes. The Bias within a single core is fully loaded at a time. |

Note: The M axis mentioned above is the vertical axis of the input Input after img2col expansion during the forward convolution operation, and its value is equal to Hout \* Wout. K is the horizontal axis of the input Input after img2col expansion, and its value is equal to KD\*C1\*KH\*KW\*C0. KD/KH/KW are the Depth, Height, and Width of Weight, which are abbreviations of kernelD/kernelH/kernelW. N is the Cout of Weight. For details, see Figure 1.

**Figure 1**  MKN in Conv3D forward computation  
![](../../../../figures/mkn_in_conv3d_forward_computation.png "MKN in Conv3D forward computation")

**Figure 2**  MFirst in Conv3D forward convolution  
![](../../../../figures/mfirst_in_conv3d_forward_computation.png "MFirst in 3D forward convolution")

**Figure 3**  NFirst in Conv3D forward convolution  
![](../../../../figures/nfirst_in_conv3d_forward_computation.png "NFirst in 3D forward convolution")
