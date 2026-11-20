# Overview<a name="ZH-CN_TOPIC_0000002569070899"></a>

<!-- md-trans-meta sourceCommit=dacba287b3c94417af1e532d9cbd29c398889dea translatedAt=2026-11-02T03:38:49.602Z pushedAt=2026-11-18T01:25:06.724Z nl=2 -->

Matrix computation move-in is a core category of APIs in the Ascend C programming framework for data movement, primarily implementing efficient data transfer between Global Memory and L0A Buffer/L0B Buffer. These APIs provide multiple data movement modes to meet the movement requirements of left and right matrices in matrix computation, and are typically used in conjunction with the Mmad API.

The matrix computation move-in APIs support multiple data movement scenarios, including but not limited to **2D format data movement, 3D format conversion, compressed data decompression, matrix transpose movement, and sparse data processing**. By flexibly configuring different parameter structures, you can precisely control key information such as the data movement path, start position, and movement length, fully leveraging the hardware's data movement capability to maximize the overall execution performance of the operator.

The matrix computation move-in APIs support multiple data paths, each corresponding to a specific storage level and access characteristics.

**Table 1** **Data paths and storage levels**

<a name="zh-cn_topic_0000002543771563_table3523123518108"></a>

| Source Location | Source Address Alignment Requirement | Destination Location | Destination Address Alignment Requirement | Typical Application Scenario |
| --- | --- | --- | --- | --- |
| Global Memory | 1 byte | L1 Buffer (A1) | 32 bytes | Load left matrix data from global memory to L1 Buffer. |
| Global Memory | 1 byte | L1 Buffer (B1) | 32 bytes | Load right matrix data from global memory to L1 Buffer. |
| Global Memory | 1 byte | L0A Buffer (A2) | 512 bytes | Load left matrix data from global memory to L0A Buffer. |
| Global Memory | 1 byte | L0B Buffer (B2) | 512 bytes | Load right matrix data from global memory to L0B Buffer. |
| L1 Buffer (A1) | 32 bytes | L0A Buffer (A2) | 512 bytes | Load left matrix data from L1 Buffer to L0A Buffer. |
| L1 Buffer (B1) | 32 bytes | L0B Buffer (B2) | 512 bytes | Load right matrix data from L1 Buffer to L0B Buffer. |
| L1 Buffer | 32 bytes | BiasTable Buffer | 64 bytes | Load the Bias data required for matrix computation from L1 Buffer to BiasTable Buffer. |
| L1 Buffer | 32 bytes | Fixpipe Buffer | 128 bytes | Load data such as quantization parameters required during Fixpipe movement from L1 Buffer to Fixpipe Buffer. |

<!-- npu="950" id1 -->
> [!NOTE]
> Due to hardware changes, the Ascend 950PR&950DT products removes the data paths from GM to L0A Buffer and L0B Buffer. As a result, the original data movement from GM to L0A Buffer and L0B Buffer must be split into two steps: data movement from GM to L1 Buffer, and data movement from L1 Buffer to L0A Buffer and L0B Buffer.
<!-- end id1 -->

## GM->L1 Buffer Interface Classification and Usage Scenarios<a name="zh-cn_topic_0000002543771563_section_gm2l1"></a>

**Table 2** **GM->L1 Buffer Interfaces**

<a name="zh-cn_topic_0000002543771563_table18501247192710"></a>

| Interface Category | Main Function | Supported Path | Typical Application Scenario |
| --- | --- | --- | --- |
| [DataCopy (GMToL1 Continuous Data Transfer)](DataCopy_GMToL1_continuous.md) | Moves the matrix from Global Memory to the L1 Buffer continuously, with the format and content unchanged. | GM->L1 Buffer | Matrix computation data is moved from off-chip into the L1 Buffer. |
| [DataCopy (GMToL1 High-Dimensional Split Data Transfer)](DataCopy_GMToL1_highdim_split.md) | Moves data from GM to the L1 Buffer, supporting non-continuous transfer by configuring the number, length, and address interval of data blocks. | GM->L1 Buffer | Non-continuous transfer of matrix slices and multi-channel data. |
| [DataCopy (GMToL1 On-the-Fly Conversion - ND2NZ Transfer)](DataCopy_GMToL1_ND2NZ.md) | Moves data from GM to the L1 Buffer, completing the conversion from ND to NZ fractal format during the transfer. | GM->L1 Buffer | Converts ND-format feature maps to NZ fractal format before matrix computation. |
| [LoadData (GMToL1-2D Matrix Transfer)](LoadData_GMToL1_2D.md) | Moves 2D-format matrix data from GM to the L1 Buffer in units of 512B data fractals. | GM->L1 Buffer | Moves the 2D fractal data required for general matrix computation into the L1 Buffer. |

<!-- npu="950" id2 -->
For Ascend 950PR&950DT products, the following GM->L1 Buffer interfaces are added. See Table 3.

**Table 3** **GM->L1 Buffer Interfaces**

| Interface Category | Main Function | Supported Path | Typical Application Scenario |
| --- | --- | --- | --- |
| [DataCopy (GMToL1 On-the-Fly Conversion - DN2NZ Transfer)](DataCopy_GMToL1_DN2NZ.md) | Moves data from GM to the L1 Buffer, completing the conversion from DN to NZ format during the transfer. | GM->L1 Buffer | Moves DN-format weight data into the L1 Buffer and converts it to NZ format. |
| [DataCopyPad (GMToL1 Unaligned Data Transfer)](DataCopyPad_GMToL1.md) | Moves data from GM to the L1 Buffer in an unaligned manner, supporting padding on the left/right side of the data. | GM->L1 Buffer | Pads unaligned data in GM when moving it into the L1 Buffer. |
| [LoadData (GMToL1-2D Matrix Transfer V2)](LoadData_GMToL1_2DV2.md) | Uses the 2DV2 instruction to move 2D-format data from GM to the L1 Buffer in units of 512B fractals. | GM->L1 Buffer | Moves 2D fractal data for matrix computation from GM into the L1 Buffer. |

<!-- end id2 -->

## UB->L1 Buffer API Classification and Usage Scenarios<a name="zh-cn_topic_0000002543771563_section_ub2l1"></a>

**Table 4** **UB->L1 Buffer APIs**

| API Category | Main Function | Supported Path | Typical Application Scenario |
| --- | --- | --- | --- |
| [DataCopy (UBToL1 continuous data transfer)](DataCopy_UBToL1_continuous.md) | Moves data from UB to L1 Buffer continuously, with format and content unchanged. | UB->L1 Buffer | Vector computation results are transferred from UB to L1 Buffer for matrix computation. |
| [DataCopy (UBToL1 high-dimensional split data transfer)](DataCopy_UBToL1_highdim_split.md) | Moves data from UB to L1 Buffer, supporting non-continuous transfer through block parameter configuration. | UB->L1 Buffer | Non-contiguously arranged data in UB is moved into L1 Buffer. |
| [DataCopy (UBToL1 inline conversion - ND2NZ transfer)](DataCopy_UBToL1_ND2NZ.md) | Moves data from UB to L1 Buffer while completing ND-to-NZ fractal format conversion. | UB->L1 Buffer | ND-format data in UB is moved into L1 Buffer and converted to NZ format. |
| [DataCopyPad (UBToL1 unaligned data transfer)](DataCopyPad_UBToL1.md) | Moves data from UB to L1 Buffer in an unaligned manner, supporting padding on the left/right side of the data. | UB->L1 Buffer | Movement and padding of data at non-32B-aligned boundaries in UB. |

## API Classification and Usage Scenarios for L1 Buffer -> L0A/B Buffer<a name="zh-cn_topic_0000002543771563_section_l12l0"></a>

**Table 5** **L1 Buffer -> L0A/B Buffer APIs**

| API Category | Main Function | Supported Path | Typical Application Scenario |
| --- | --- | --- | --- |
| [LoadData (2D matrix transfer)](LoadData_2D.md) | Moves 2D format matrix data in 512B fractal units, supporting the L1 Buffer -> L0A Buffer/L0B Buffer paths and fractal transpose. | L1 Buffer -> L0A Buffer, L1 Buffer -> L0B Buffer | Matrix computation data is moved from L1 Buffer into the L0A Buffer/L0B Buffer computation buffers. |
| [LoadData (convolution data movement)](LoadData_3D.md) | After expanding the NC1HWC0 format Feature Map through Image to Column, moves it into L0A Buffer/L0B Buffer in 512B fractal units. | L1 Buffer -> L0A Buffer, L1 Buffer -> L0B Buffer | Convolution im2col expansion and matrix computation data move-in. |
| [LoadDataWithTranspose](LoadDataWithTranspose.md) | Moves 2D format data with a transpose operation, concatenating multiple fractals into a square matrix, transposing it, and then splitting it to move into L0A Buffer/L0B Buffer. | L1 Buffer -> L0A Buffer, L1 Buffer -> L0B Buffer | A/B matrices that require transposition in matrix computation are moved into L0 Buffer. |
| [DataCopy (L1ToBiasTable-Buffer data transfer)](DataCopy_L1ToBiasTable.md) | Moves the bias parameters of matrix computation from L1 Buffer to BiasTable Buffer. | L1 Buffer -> BiasTable Buffer | Bias add parameters after matrix multiplication are moved in. |
| [DataCopy (L1ToFixpipe-Buffer data transfer)](DataCopy_L1ToFixpipe.md) | Moves the on-the-fly quantization and on-the-fly ReLU parameters from L1 Buffer to Fixpipe Buffer. | L1 Buffer -> Fixpipe Buffer | On-the-fly quantization and activation parameters of matrix computation results are moved in. |

<!-- npu="950" id5 -->
> [!NOTE]Note
> For Ascend 950PR&950DT products, LoadDataWithTranspose supports only the L1 Buffer -> L0B Buffer path, and does not support the L1 Buffer -> L0A Buffer path.
<!-- end id5 -->

<!-- npu="910b,A3" id3 -->
For Atlas A2 products and Atlas A3 products, the following L1 Buffer -> L0B Buffer APIs are supported. See Table 6.

**Table 6** **L1 Buffer -> L0B Buffer APIs**

| API Category | Main Function | Supported Path | Typical Application Scenario |
| --- | --- | --- | --- |
| [LoadDataWithSparse](LoadDataWithSparse.md) | Moves the densified weight matrix and index matrix required for 4-select-2 structured sparse matrix computation. | L1 Buffer -> L0B Buffer | Sparse matrix multiplication computation data move-in. |

<!-- end id3 -->

<!-- npu="950" id4 -->
For Ascend 950PR&950DT products, the following L1 Buffer -> L0A/B Buffer APIs are added. See Table 7.

**Table 7** **L1 Buffer -> L0A/B Buffer APIs**

| API Category | Main Function | Supported Path | Typical Application Scenario |
| --- | --- | --- | --- |
| [LoadData (2D matrix transfer V2)](LoadData_2D_V2.md) | Moves 2D format matrix data in 512B fractal units, using the V2 parameter structure and supporting more data types. | L1 Buffer -> L0A Buffer, L1 Buffer -> L0B Buffer | Matrix computation data is moved into L0A Buffer/L0B Buffer, supporting the b4 data type. |
| [LoadData (BitMode 2D matrix transfer)](LoadData_2D_BitMode.md) | A bit-mode variant of LoadData (2D matrix transfer V2) that supports bitwise data movement. | L1 Buffer -> L0A Buffer, L1 Buffer -> L0B Buffer | Used for ordinary matrix data movement that pursues extreme Scalar performance. |
| [LoadData (MX matrix transfer)](LoadData_2D_MX.md) | An MX format extension of LoadData (2D matrix transfer V2) that supports MX format matrix data movement. | L1 Buffer -> L0A Buffer, L1 Buffer -> L0B Buffer | Matrix data move-in in MX floating-point format. |
| [LoadData (BitMode convolution data movement)](LoadData_3D_BitMode.md) | A bit-mode variant of LoadData (convolution data movement) that supports bitwise 3D data movement. | L1 Buffer -> L0A Buffer, L1 Buffer -> L0B Buffer | Used for convolution im2col expansion and move-in that pursue extreme Scalar performance. |

<!-- end id4 -->

## Auxiliary Configuration APIs<a name="zh-cn_topic_0000002543771563_section_aux"></a>

**Table 8** **Auxiliary Configuration APIs**

<a name="zh-cn_topic_0000002543771563_table107953337145"></a>

| API name | Description | API Used in Conjunction |
| --- | --- | --- |
| [Fill](../cube_load_aux_config/Fill.md) | Initializes the values of L1 Buffer/L0A Buffer/L0B Buffer. | - |
| [SetFmatrix](../cube_load_aux_config/SetFmatrix.md) | Sets the FeatureMap attribute description. | LoadData (convolution data movement) |
| [SetLoadDataBoundary](../cube_load_aux_config/SetLoadDataBoundary.md) | Sets the L1 Buffer boundary value. | LoadData (convolution data movement) |
| [SetLoadDataRepeat](../cube_load_aux_config/SetLoadDataRepeat.md) | Sets the Repeat parameter to implement multi-iteration movement. | LoadData (convolution data movement) |
| [SetLoadDataPaddingValue](../cube_load_aux_config/SetLoadDataPaddingValue.md) | Sets the padding value. | LoadData (convolution data movement) |
| [LoadDataUnzip](../cube_load_aux_config/LoadDataUnzip.md) | Decompresses data on GM and moves it to L1 Buffer (A1/B1) or L0B Buffer (B2). | LoadUnzipIndex |
| [LoadImageToLocal](../cube_load_aux_config/LoadImageToLocal.md) | Moves image data from Global Memory to Local Memory. | SetAippFunctions |
| [LoadUnzipIndex](../cube_load_aux_config/LoadUnzipIndex.md) | Loads the compression index table on GM into internal registers. | - |
| [SetAippFunctions](../cube_load_aux_config/SetAippFunctions.md) | Sets parameters related to image pre-processing (AIPP, AI Core pre-process). | LoadImageToLocal |
