# Overview<a name="ZH-CN_TOPIC_0000002569070899"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T12:11:58.635Z pushedAt=2026-10-26T07:41:44.103Z nl=2 -->

Data load for matrix computation is a core category of APIs used for data movement in the Ascend C programming framework, primarily implementing efficient data transfer between Global Memory and L0A Buffer/L0B Buffer. This category of APIs provides multiple data movement modes to meet the movement requirements of data such as the left and right matrices in matrix computation, and is typically used in conjunction with the Mmad API.

The data load APIs for matrix computation support multiple data movement scenarios, including but not limited to **two-dimensional format data movement, three-dimensional format conversion, compressed data decompression, matrix transpose movement, and sparse data processing**. By flexibly configuring different parameter structures, developers can precisely control key information such as the data movement path, start position, and movement length, fully leveraging the hardware's data movement capability to maximize the overall execution performance of the operator.

The data load APIs for matrix computation support multiple data paths, each corresponding to a specific storage level and access characteristic.

**Table 1** **Data Paths and Storage Levels**

<a name="zh-cn_topic_0000002543771563_table3523123518108"></a>

| Source Location | Source Address Alignment Requirement | Destination Location | Destination Address Alignment Requirement | Typical Application Scenarios |
| --- | --- | --- | --- | --- |
| Global Memory | 1 byte | L1 Buffer(A1) | 32 bytes | Load left matrix data from global memory to L1 Buffer. |
| Global Memory | 1 byte | L1 Buffer(B1) | 32 bytes | Load right matrix data from global memory to L1 Buffer. |
| Global Memory | 1 byte | L0A Buffer(A2) | 512 bytes | Load left matrix data from global memory to L0A Buffer. |
| Global Memory | 1 byte | L0B Buffer(B2) | 512 bytes | Load right matrix data from global memory to L0B Buffer. |
| L1 Buffer(A1) | 32 bytes | L0A Buffer(A2) | 512 bytes | Load left matrix data from L1 Buffer to L0A Buffer. |
| L1 Buffer(B1) | 32 bytes | L0B Buffer(B2) | 512 bytes | Load right matrix data from L1 Buffer to L0B Buffer. |
| L1 Buffer | 32 bytes | BiasTable Buffer | 64 bytes | Load the Bias data required for matrix computation from L1 Buffer to BiasTable Buffer. |
| L1 Buffer | 32 bytes | Fixpipe Buffer | 128 bytes | Load data such as quantization parameters required during Fixpipe movement from L1 Buffer to Fixpipe Buffer. |

<!-- npu="950" id1 -->
> [!NOTE] Note
> Due to hardware changes, the Ascend 950PR&950DT products removes the data paths from GM to L0A Buffer and L0B Buffer. As a result, the original data movement from GM to L0A Buffer and L0B Buffer must be split into two steps: data movement from GM to L1 Buffer, and data movement from L1 Buffer to L0A Buffer and L0B Buffer.
<!-- end id1 -->

## API Classification and Usage Scenarios of GM->L1 Buffer<a name="zh-cn_topic_0000002543771563_section_gm2l1"></a>

**Table 2** **GM->L1 Buffer APIs**

<a name="zh-cn_topic_0000002543771563_table18501247192710"></a>

| API Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| [DataCopy (GMToL1 Continuous Data Movement)](DataCopy_GMToL1_continuous.md) | Moves a matrix from Global Memory to the L1 Buffer continuously, with the format and content unchanged. | GM->L1 Buffer | Matrix computation data is moved from off-chip into the L1 Buffer. |
| [DataCopy (GMToL1 High-Dimensional Split Data Movement)](DataCopy_GMToL1_highdim_split.md) | Moves data from GM to the L1 Buffer, supporting non-continuous movement by configuring the number of data blocks, length, and address interval. | GM->L1 Buffer | Non-continuous movement of matrix tiles and multi-channel data. |
| [DataCopy (GMToL1 On-the-fly Conversion - ND2NZ Movement)](DataCopy_GMToL1_ND2NZ.md) | Moves data from GM to the L1 Buffer, completing the conversion from ND to NZ fractal format during the movement. | GM->L1 Buffer | Converts ND-format feature maps to NZ fractal format before matrix computation. |
| [LoadData (GMToL1-2D Matrix Movement)](LoadData_GMToL1_2D.md) | Moves 2D-format matrix data from GM to the L1 Buffer in units of 512B data fractals. | GM->L1 Buffer | Moves the 2D fractal data required for ordinary matrix computation into the L1 Buffer. |

<!-- npu="950" id2 -->
For the Ascend 950PR&950DT products, the following GM->L1 Buffer APIs are added. Developers should refer to Table 3.

**Table 3** **GM->L1 Buffer APIs**

| API Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| [DataCopy (GMToL1 On-the-fly Conversion - DN2NZ Movement)](DataCopy_GMToL1_DN2NZ.md) | Moves data from GM to the L1 Buffer, completing the conversion from DN to NZ format during the movement. | GM->L1 Buffer | Moves DN-format weight data into the L1 Buffer and converts it to NZ format. |
| [DataCopyPad (GMToL1 Non-aligned Data Movement)](DataCopyPad_GMToL1.md) | Moves data from GM to the L1 Buffer in a non-aligned manner, supporting self-padding on the left/right side of the data. | GM->L1 Buffer | Pads non-aligned data in GM when moving it into the L1 Buffer. |
| [LoadData (GMToL1-2D Matrix Movement V2)](LoadData_GMToL1_2DV2.md) | Uses the 2DV2 instruction to move 2D-format data from GM to the L1 Buffer in units of 512B fractals. | GM->L1 Buffer | Moves 2D fractal data for matrix computation from GM into the L1 Buffer. |

<!-- end id2 -->

## API Classification and Usage Scenarios for UB->L1 Buffer<a name="zh-cn_topic_0000002543771563_section_ub2l1"></a>

**Table 4** **UB->L1 Buffer APIs**

| API Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| [DataCopy (UBToL1 Continuous Movement)](DataCopy_UBToL1_continuous.md) | Moves data from UB continuously to the L1 Buffer, with the format and content unchanged. | UB->L1 Buffer | Vector computation results are moved from UB into the L1 Buffer for matrix computation. |
| [DataCopy (UBToL1 High-Dimensional Split Data Movement)](DataCopy_UBToL1_highdim_split.md) | Moves data from UB to the L1 Buffer, supporting non-continuous movement through block parameter configuration. | UB->L1 Buffer | Non-contiguously arranged data in UB is moved into the L1 Buffer. |
| [DataCopy (UBToL1 On-the-fly Conversion - ND2NZ Movement)](DataCopy_UBToL1_ND2NZ.md) | Moves data from UB to the L1 Buffer, completing ND-to-NZ fractal format conversion during the movement. | UB->L1 Buffer | ND-format data in UB is moved into the L1 Buffer and converted to NZ format. |
| [DataCopyPad (UBToL1 Non-aligned Data Movement)](DataCopyPad_UBToL1.md) | Moves data from UB to the L1 Buffer in a non-aligned manner, supporting self-padding on the left/right sides of the data. | UB->L1 Buffer | Movement and padding of non-32B-aligned boundary data in UB. |

## API Classification and Usage Scenarios for L1 Buffer->L0A/B Buffer<a name="zh-cn_topic_0000002543771563_section_l12l0"></a>

**Table 5** **L1 Buffer->L0A/B Buffer APIs**

| API Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| [LoadData (2D Matrix Movement)](LoadData_2D.md) | Moves 2D format matrix data in 512B fractal units, supporting the L1 Buffer->L0A Buffer/L0B Buffer paths and fractal transpose. | L1 Buffer->L0A Buffer, L1 Buffer->L0B Buffer | Matrix computation data is moved from L1 Buffer into the L0A Buffer/L0B Buffer compute buffers. |
| [LoadData (Convolution Data Movement)](LoadData_3D.md) | After performing Image to Column expansion on the NC1HWC0 format Feature Map, moves the data into L0A Buffer/L0B Buffer in 512B fractal units. | L1 Buffer->L0A Buffer, L1 Buffer->L0B Buffer | im2col expansion for convolution and movement of matrix computation data into L0 Buffer. |
| [LoadDataWithTranspose](LoadDataWithTranspose.md) | Moves 2D format data with an on-the-fly transpose operation, splicing multiple fractals into a square matrix, transposing it, and then splitting it to move into L0A Buffer/L0B Buffer. | L1 Buffer->L0A Buffer, L1 Buffer->L0B Buffer | A/B matrices that require transposition in matrix computation are moved into L0 Buffer. |
| [DataCopy (L1ToBiasTable-Buffer Data Movement)](DataCopy_L1ToBiasTable.md) | Moves the bias parameters for matrix computation from L1 Buffer to BiasTable Buffer. | L1 Buffer->BiasTable Buffer | Movement of bias add parameters after matrix multiplication. |
| [DataCopy (L1ToFixpipe-Buffer Data Movement)](DataCopy_L1ToFixpipe.md) | Moves the on-the-fly quantization and on-the-fly ReLU parameters from L1 Buffer to Fixpipe Buffer. | L1 Buffer->Fixpipe Buffer | Movement of on-the-fly quantization and activation parameters for matrix computation results. |

<!-- npu="950" id_l12l0_950note -->
> [!NOTE] Note
> For the Ascend 950PR&950DT products, LoadDataWithTranspose supports only the L1 Buffer->L0B Buffer path and does not support the L1 Buffer->L0A Buffer path.
<!-- end id_l12l0_950note -->

<!-- npu="910b,A3" id3 -->
For Atlas A2 training products/Atlas A2 inference products and Atlas A3 training products/Atlas A3 inference products, the following L1 Buffer->L0B Buffer APIs are supported. Developers should refer to Table 6.

**Table 6** **L1 Buffer->L0B Buffer APIs**

| API Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| [LoadDataWithSparse](LoadDataWithSparse.md) | Moves the densified weight matrix and index matrix required for 4-out-of-2 structured sparse matrix computation. | L1 Buffer->L0B Buffer | Sparse matrix multiplication computation data movement in. |

<!-- end id3 -->

<!-- npu="950" id4 -->
For the Ascend 950PR&950DT products, the following L1 Buffer->L0A/B Buffer APIs are newly added. Developers should refer to Table 7.

**Table 7** **L1 Buffer->L0A/B Buffer APIs**

| API Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| [LoadData (2D Matrix Movement V2)](LoadData_2D_V2.md) | Moves 2D format matrix data in units of 512B fractal, using the V2 parameter structure and supporting more data types. | L1 Buffer->L0A Buffer, L1 Buffer->L0B Buffer | Matrix computation data is moved into L0A Buffer/L0B Buffer, supporting the b4 data type. |
| [LoadData (BitMode 2D Matrix Movement)](LoadData_2D_BitMode.md) | A bit-mode variant of LoadData (2D Matrix Movement V2) that supports bitwise data movement. | L1 Buffer->L0A Buffer, L1 Buffer->L0B Buffer | Used for ordinary matrix data movement that pursues ultimate Scalar performance. |
| [LoadData (MX Matrix Movement)](LoadData_2D_MX.md) | An MX format extension of LoadData (2D Matrix Movement V2) that supports MX format matrix data movement. | L1 Buffer->L0A Buffer, L1 Buffer->L0B Buffer | Matrix data movement in the MX floating-point format. |
| [LoadData (BitMode Convolution Data Movement)](LoadData_3D_BitMode.md) | A bit-mode variant of LoadData (Convolution Data Movement) that supports bitwise 3D data movement. | L1 Buffer->L0A Buffer, L1 Buffer->L0B Buffer | Used for convolution im2col expansion and moving data in, pursuing ultimate Scalar performance. |

<!-- end id4 -->

## Auxiliary Configuration API Description<a name="zh-cn_topic_0000002543771563_section_aux"></a>

**Table 8** **Auxiliary Configuration APIs**

<a name="zh-cn_topic_0000002543771563_table107953337145"></a>

| API Name | Description | APIs Used in Conjunction |
| --- | --- | --- |
| [Fill](../cube_load_aux_config/Fill.md) | Initializes and sets the values of L1 Buffer/L0A Buffer/L0B Buffer. | - |
| [SetFmatrix](../cube_load_aux_config/SetFmatrix.md) | Sets the FeatureMap attribute description. | LoadData (convolution data movement) |
| [SetLoadDataBoundary](../cube_load_aux_config/SetLoadDataBoundary.md) | Sets the L1 Buffer boundary value. | LoadData (convolution data movement) |
| [SetLoadDataRepeat](../cube_load_aux_config/SetLoadDataRepeat.md) | Sets the Repeat parameter to implement multi-iteration movement. | LoadData (convolution data movement) |
| [SetLoadDataPaddingValue](../cube_load_aux_config/SetLoadDataPaddingValue.md) | Sets the Padding fill value. | LoadData (convolution data movement) |
| [LoadDataUnzip](../cube_load_aux_config/LoadDataUnzip.md) | Decompresses data on GM and moves it to L1 Buffer (A1/B1) or L0B Buffer (B2). | LoadUnzipIndex |
| [LoadImageToLocal](../cube_load_aux_config/LoadImageToLocal.md) | Moves image data from Global Memory to Local Memory. | SetAippFunctions |
| [LoadUnzipIndex](../cube_load_aux_config/LoadUnzipIndex.md) | Loads the compressed index table on GM into internal registers. | - |
| [SetAippFunctions](../cube_load_aux_config/SetAippFunctions.md) | Sets parameters related to image pre-processing (AIPP, AI Core pre-process). | LoadImageToLocal |
