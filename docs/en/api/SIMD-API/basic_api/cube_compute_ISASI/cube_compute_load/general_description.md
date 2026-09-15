# Overview<a name="ZH-CN_TOPIC_0000002569070899"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:54:33.905Z -->

Cube loading for cube computation is a core category of APIs used for data movement in the Ascend C programming framework, primarily implementing efficient data transfer between Global Memory and L0A Buffer/L0B Buffer. This category of APIs provides multiple data movement modes to meet the movement requirements of data such as the left and right matrices in cube computation, and is typically used in conjunction with the Mmad API.

The cube loading APIs for cube computation support multiple data movement scenarios, including but not limited to **two-dimensional format data movement, three-dimensional format conversion, compressed data decompression, cube transpose movement, and sparse data processing**. By flexibly configuring different parameter structures, developers can precisely control key information such as the data movement path, start position, and movement length, fully leveraging the hardware's data movement capability to maximize the overall execution performance of the operator.

The cube loading APIs for cube computation support multiple data paths, each corresponding to a specific storage level and access characteristic.

**Table 1** **Data paths and storage hierarchy**

<a name="zh-cn_topic_0000002543771563_table3523123518108"></a>

| Source Location | Source Address Alignment Requirement | Destination Location | Destination Address Alignment Requirement | Typical Application Scenarios |
| --- | --- | --- | --- | --- |
| Global Memory | 1 byte | L1 Buffer(A1) | 32 bytes | Load left cube data from global memory to L1 Buffer. |
| Global Memory | 1 byte | L1 Buffer(B1) | 32 bytes | Load right cube data from global memory to L1 Buffer. |
| Global Memory | 1 byte | L0A Buffer(A2) | 512 bytes | Load left cube data from global memory to L0A Buffer. |
| Global Memory | 1 byte | L0B Buffer(B2) | 512 bytes | Load right cube data from global memory to L0B Buffer. |
| L1 Buffer(A1) | 32 bytes | L0A Buffer(A2) | 512 bytes | Load left cube data from L1 Buffer to L0A Buffer. |
| L1 Buffer(B1) | 32 bytes | L0B Buffer(B2) | 512 bytes | Load right cube data from L1 Buffer to L0B Buffer. |
| L1 Buffer | 32 bytes | BiasTable Buffer | 64 bytes | Load the Bias data required for cube computation from L1 Buffer to BiasTable Buffer. |
| L1 Buffer | 32 bytes | Fixpipe Buffer | 128 bytes | Load data such as quantization parameters required during Fixpipe movement from L1 Buffer to Fixpipe Buffer. |

<!-- npu="950" id1 -->
> [!NOTE] Note
> Due to hardware changes, Ascend 950PR/Ascend 950DT remove the data paths from GM to L0A Buffer and L0B Buffer. As a result, the original data movement from GM to L0A Buffer and L0B Buffer must be split into two steps: data movement from GM to L1 Buffer, and data movement from L1 Buffer to L0A Buffer and L0B Buffer.
<!-- end id1 -->

## API Classification and Usage Scenarios for GM->L1 Buffer<a name="zh-cn_topic_0000002543771563_section_gm2l1"></a>

**Table 2** **GM->L1 Buffer APIs**

<a name="zh-cn_topic_0000002543771563_table18501247192710"></a>

| API Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| [GM->L1 Buffer Continuous Data Movement (DataCopy)](DataCopy_GMToL1_continuous.md) | Moves a cube from Global Memory to the L1 Buffer continuously, with the format and content unchanged. | GM->L1 Buffer | Cube computation data is loaded from off-chip into the L1 Buffer. |
| [GM->L1 Buffer High-Dimensional Split Data Movement (DataCopy)](DataCopy_GMToL1_highdim_split.md) | Moves data from GM to the L1 Buffer, supporting non-continuous movement by configuring the number of data blocks, length, and address interval. | GM->L1 Buffer | Non-continuous movement of cube tiles and multi-channel data. |
| [GM->L1 Buffer On-the-fly Conversion - ND2NZ Movement (DataCopy)](DataCopy_GMToL1_ND2NZ.md) | Moves data from GM to the L1 Buffer while simultaneously completing the conversion from ND to NZ fractal format. | GM->L1 Buffer | Converts ND-format feature maps to NZ fractal format before cube computation. |
| [GM->L1 Buffer Load2D Instruction Movement](gmtol1_load2d_instruction_movement.md) | Moves 2D-format cube data from GM to the L1 Buffer in units of 512B data fractals. | GM->L1 Buffer | 2D fractal data required for general cube computation is loaded into the L1 Buffer. |

<!-- npu="950" id2 -->
For Ascend 950PR/Ascend 950DT, the following GM->L1 Buffer APIs are newly added. Developers are advised to refer to Table 3.

**Table 3** **GM->L1 Buffer APIs**

| API Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| [GM->L1 Buffer On-the-fly Conversion DN2NZ Movement (DataCopy)](DataCopy_GMToL1_DN2NZ.md) | Moves data from GM to the L1 Buffer while simultaneously completing the conversion from DN to NZ format. | GM->L1 Buffer | DN-format weight data is loaded into the L1 Buffer and converted to NZ format. |
| [GM->L1 Buffer Non-Aligned Data Movement (DataCopyPad)](DataCopyPad_GMToL1.md) | Moves data from GM to the L1 Buffer in a non-aligned manner, supporting self-padding on the left/right sides of the data. | GM->L1 Buffer | Padding is performed when non-aligned data in GM is loaded into the L1 Buffer. |
| [GM->L1 Buffer Load2DV2 Instruction Movement](gmtol1_load2dv2_instruction_movement.md) | Uses the 2DV2 instruction to move 2D-format data from GM to the L1 Buffer in units of 512B fractals. | GM->L1 Buffer | 2D fractal data for cube computation is moved from GM into the L1 Buffer. |
<!-- end id2 -->

## API Classification and Usage Scenarios for UB->L1 Buffer<a name="zh-cn_topic_0000002543771563_section_ub2l1"></a>

**Table 4** **UB->L1 Buffer APIs**

| API Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| [Continuous Data Movement from UB to L1 Buffer (DataCopy)](DataCopy_UBToL1_continuous.md) | Moves data from the Unified Buffer to the L1 Buffer continuously, with the format and content unchanged. | UB->L1 Buffer | Vector computation results are transferred from the UB to the L1 Buffer for cube computation. |
| [High-dimensional Split Data Movement from UB to L1 Buffer (DataCopy)](DataCopy_UBToL1_highdim_split.md) | Moves data from the UB to the L1 Buffer, supporting non-continuous movement through block parameter configuration. | UB->L1 Buffer | Non-contiguously arranged data in the UB is loaded into the L1 Buffer. |
| [On-the-fly ND2NZ Conversion Data Movement from UB to L1 Buffer (DataCopy)](DataCopy_UBToL1_ND2NZ.md) | Moves data from the UB to the L1 Buffer, completing ND-to-NZ fractal format conversion during the movement. | UB->L1 Buffer | ND-format data in the UB is loaded into the L1 Buffer and converted to NZ format. |
| [Unaligned Data Movement from UB to L1 Buffer (DataCopyPad)](DataCopyPad_UBToL1.md) | Moves data from the UB to the L1 Buffer in an unaligned manner, supporting self-padding on the left/right sides of the data. | UB->L1 Buffer | Movement and padding of data at non-32B-aligned boundaries in the UB. |

## API Classification and Usage Scenarios for L1 Buffer->L0A/B Buffer<a name="zh-cn_topic_0000002543771563_section_l12l0"></a>

**Table 5** **L1 Buffer->L0A/B Buffer APIs**

| API Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| [LoadData (2D cube movement)](Load2D.md) | Moves 2D format cube data in units of 512B fractals, supporting the L1 Buffer->L0A Buffer/L0B Buffer paths and fractal transpose. | L1 Buffer->L0A Buffer, L1 Buffer->L0B Buffer | Cube computation data is loaded from L1 Buffer into the L0A Buffer/L0B Buffer computation buffers. |
| [LoadData (convolution data movement)](Load3D.md) | After completing Image to Column expansion of NC1HWC0 format Feature Map, loads the data into L0A Buffer/L0B Buffer in units of 512B fractals. | L1 Buffer->L0A Buffer, L1 Buffer->L0B Buffer | im2col expansion of convolution and cube computation data loading. |
| [LoadDataWithTranspose](LoadDataWithTranspose.md) | Moves 2D format data with a transpose operation, concatenating multiple fractals into a square cube, transposing it, and then splitting and loading it into L0A Buffer/L0B Buffer. | L1 Buffer->L0A Buffer, L1 Buffer->L0B Buffer | Loading of A/B matrices that require transposition in cube computation into L0 Buffer. |
| [L1 Buffer->BiasTable Buffer data movement](DataCopy_L1ToBiasTable.md) | Moves the bias parameters of cube computation from L1 Buffer to BiasTable Buffer. | L1 Buffer->BiasTable Buffer | Loading of bias add parameters after cube multiplication. |
| [L1 Buffer->Fixpipe Buffer data movement](DataCopy_L1ToFixpipe.md) | Moves the on-the-fly quantization and on-the-fly ReLU parameters from L1 Buffer to Fixpipe Buffer. | L1 Buffer->Fixpipe Buffer | Loading of on-the-fly quantization and activation parameters for cube computation results. |

> [!NOTE]Note
> For Ascend 950PR/Ascend 950DT, LoadDataWithTranspose supports only the L1 Buffer->L0B Buffer path, and does not support the L1 Buffer->L0A Buffer path.

<!-- npu="910b,A3" id3 -->
For Atlas A2 training products/Atlas A2 inference products and Atlas A3 training products/Atlas A3 inference products, the following L1 Buffer->L0B Buffer APIs are supported. Developers are advised to refer to Table 6.

**Table 6** **L1 Buffer->L0B Buffer APIs**

| API Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| [LoadDataWithSparse](LoadDataWithSparse.md) | Moves the densified weight cube and index cube required for 4-out-of-2 structured sparse cube computation. | L1 Buffer->L0B Buffer | Sparse cube multiplication computation data loading. |
<!-- end id3 -->

<!-- npu="950" id4 -->
For Ascend 950PR/Ascend 950DT, the following L1 Buffer->L0A/B Buffer APIs are newly added. Developers are advised to refer to Table 7.

**Table 7** **L1 Buffer->L0A/B Buffer APIs**

| API Category | Main Functions | Supported Path | Typical Application Scenarios |
| --- | --- | --- | --- |
| [LoadData (2D cube movement V2)](Load2DV2.md) | Moves 2D format cube data in units of 512B fractals, using the V2 parameter structure and supporting more data types. | L1 Buffer->L0A Buffer, L1 Buffer->L0B Buffer | Cube computation data loading into L0A Buffer/L0B Buffer, supporting the b4 data type. |
| [LoadData (BitMode 2D cube movement)](Load2DBitMode.md) | A bit-mode variant of Load2DV2, supporting bitwise data movement. | L1 Buffer->L0A Buffer, L1 Buffer->L0B Buffer | Used for ordinary cube data movement pursuing ultimate Scalar performance. |
| [LoadData (MX cube movement)](Load2DMX.md) | An MX format extension of Load2DV2, supporting MX format cube data movement. | L1 Buffer->L0A Buffer, L1 Buffer->L0B Buffer | Cube data loading in MX floating-point format. |
| [LoadData (BitMode convolution data movement)](Load3DBitMode.md) | A bit-mode variant of Load3D, supporting bitwise 3D data movement. | L1 Buffer->L0A Buffer, L1 Buffer->L0B Buffer | Used for convolution im2col expansion and loading pursuing ultimate Scalar performance. |
<!-- end id4 -->

## Auxiliary Configuration API Description<a name="zh-cn_topic_0000002543771563_section_aux"></a>

**Table 8** **Auxiliary Configuration APIs**

<a name="zh-cn_topic_0000002543771563_table107953337145"></a>

| API Name | Function Description | APIs Used in Conjunction |
| --- | --- | --- |
| [Fill](../cube_load_aux_config/Fill.md) | Initializes and sets the values of L1 Buffer/L0A Buffer/L0B Buffer. | - |
| [SetFcube](../cube_load_aux_config/SetFmatrix.md) | Sets the FeatureMap attribute description. | Load3D |
| [SetLoadDataBoundary](../cube_load_aux_config/SetLoadDataBoundary.md) | Sets the L1 Buffer boundary value. | Load3D |
| [SetLoadDataRepeat](../cube_load_aux_config/SetLoadDataRepeat.md) | Sets the Repeat parameter to implement multi-iteration movement. | Load3D |
| [SetLoadDataPaddingValue](../cube_load_aux_config/SetLoadDataPaddingValue.md) | Sets the Padding fill value. | Load3D |
| [LoadDataUnzip](../cube_load_aux_config/LoadDataUnzip.md) | Decompresses data on GM and moves it to A1/B1/B2. | LoadUnzipIndex |
| [LoadImageToLocal](../cube_load_aux_config/LoadImageToLocal.md) | Moves image data from Global Memory to Local Memory. | SetAippFunctions |
| [LoadUnzipIndex](../cube_load_aux_config/LoadUnzipIndex.md) | Loads the compressed index table on GM into internal registers. | - |
| [SetAippFunctions](../cube_load_aux_config/SetAippFunctions.md) | Sets parameters related to image pre-processing (AIPP, AI Core pre-process). | LoadImageToLocal |
