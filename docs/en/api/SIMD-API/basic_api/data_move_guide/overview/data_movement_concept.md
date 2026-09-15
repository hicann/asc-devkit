# Data Movement Concepts<a name="ZH-CN_TOPIC_0000002534880720"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-09-01T07:23:07.852Z -->

## Overview of Movement Function Categories

The data movement APIs provide comprehensive data movement capabilities, covering everything from the most basic continuous movement to complex inline conversion and quantization activation. In ascending order of functional complexity, the movement capabilities can be divided into the following categories: basic data movement, high-dimensional splitting movement, slice movement, inline format conversion movement, inline quantization activation movement, unaligned movement, UB internal movement (Copy), cube fractal movement, and multi-dimensional data movement NDDMA. Each category has corresponding detailed API descriptions in the subsequent sections.

## Basic Data Movement (DataCopy Continuous)

The simplest movement mode. It moves a segment of continuous data in memory at the source address to the destination address without any modification. The format and content of the data remain unchanged during transmission, and no conversion or computation is performed. In the function prototype, the uint32\_t count parameter specifies the number of elements to move (with sizeof\(T\) as the unit), and the API automatically converts the element count into the corresponding number of bytes.

**Applicable scenarios**: fast movement when the data is known to be stored continuously at both the source and destination addresses and the addresses meet the alignment requirements. It is typically used for batch data loading before vector computation (GM->UB) and result write-back after computation (UB->GM). See [DataCopy (GM and UB - Continuous Data Movement)](../gm_and_ub_data_movement.md#ZH-CN_TOPIC_0000002382908021), [DataCopy (GM and L1 - Continuous Data Movement)](../gm_and_l1_or_l0_data_movement.md#ZH-CN_TOPIC_0000002574022813), and other sections.

**Figure 1**  Basic continuous movement schematic diagram  

![](../../../../figures/continuous_data_copy_diagram.png "Basic continuous movement schematic diagram")

## High-Dimensional Splitting Data Movement (DataCopy High-Dimensional Splitting)

When the source or destination operand is distributed non-contiguously at fixed intervals in memory, the high-dimensional splitting mode is required. This mode introduces the DataCopyParams (or DataCopyExtParams) parameter structure, which describes the "block-repeat" pattern of the movement through the following core parameters:

- **blockLen**: The length of each data block, in units of DataBlock (32 bytes). DataBlock is the minimum addressing granularity of Ascend C data movement. For example, to move 128 half-type elements (2 bytes each, 256 bytes in total = 8 DataBlocks), blockLen = 8.
- **blockCount**: The number of data blocks to be moved. Each round of movement sequentially fetches blockCount contiguous data blocks.
- **srcStride/dstStride (srcGap/dstGap)**: The address interval between adjacent data blocks (in units of DataBlock). When stride = blockLen, the data is actually contiguous in memory; when stride\>blockLen, it indicates a jump between each data block, implementing non-contiguous (strided) movement.
- **repeat**: The number of repeated movement rounds. The entire movement process is executed in repeat rounds, each round moving blockCount data blocks, and the address offset between rounds is controlled by srcRepeatStride/dstRepeatStride.

**Applicable scenarios**: Common block (Tiling) data movement in operators such as Conv and Matmul, where data is distributed non-contiguously at fixed strides on the source or destination side. Refer to [DataCopy (GM and UB - High-Dimensional Splitting Data Movement)](../gm_and_ub_data_movement.md#ZH-CN_TOPIC_00000023829080211), [DataCopy (GM and L1 - High-Dimensional Splitting Data Movement)](../gm_and_l1_or_l0_data_movement.md#ZH-CN_TOPIC_0000002543262916) and other sections.

**Figure 2**  High-Dimensional Splitting Continuous Movement Schematic Diagram  

![](../../../../figures/continuous_data_copy_diagram.png "High-Dimensional Splitting Continuous Movement Schematic Diagram")

**Figure 3**  High-Dimensional Splitting Non-Continuous Movement Schematic Diagram  

![](../../../../figures/discontinuous_data_copy_diagram.png "Schematic diagram of high-dimensional splitting non-continuous data movement")

## Slice Data Movement (DataCopy Slice)

Slice movement supports extracting a rectangular sub-region (Slice) from a multi-dimensional Tensor for movement. By describing the slice parameters (burstLen, beginOffset, endOffset, etc.) on each dimension of the source and destination through a SliceInfo structure array, flexible slicing of 1 to 5 dimensions can be achieved. Slice movement is essentially a special type of high-dimensional non-continuous movement, but the API provides a more intuitive multi-dimensional configuration approach.

**Applicable scenarios**: extracting an ROI (Region of Interest) sub-region from a large feature map, extracting valid data regions in Pad mode, and so on. Refer to the [DataCopy (GM and UB - Slice Data Movement)](../gm_and_ub_data_movement.md#ZH-CN_TOPIC_0000002568770166) section.

## Inline Format Conversion Movement (ND2NZ/NZ2ND/DN2NZ)

In the Ascend cube computation process, data usually needs to be converted from ND (Standard Normal Data, standard data arrangement, that is, conventional formats such as NHWC/NCHW) to NZ (Normal Normal Data, fractal cube arrangement, that is, row-column arrangement organized by small cube blocks) format, and then converted back from NZ to ND after computation is complete. Ascend C provides the "inline conversion" capability, which completes format conversion at the same time as data movement without requiring additional conversion instructions, thereby achieving "zero-overhead format conversion during movement".

- **ND2NZ movement**: While data is moved from Global Memory to Unified Buffer/L1 Buffer, the ND format is converted to NZ format. The fractal dimension information of the source cube is configured through the Nd2NzParams parameter. See [DataCopy (GM -> UB - inline conversion ND2NZ movement)](../gm_and_ub_data_movement.md#ZH-CN_TOPIC_0000002349187356), [DataCopy (GM -> L1 - inline conversion ND2NZ movement)](../gm_and_l1_or_l0_data_movement.md#ZH-CN_TOPIC_0000002573902841), and other sections.
- **NZ2ND movement**: While data is moved from Unified Buffer to Global Memory, the NZ format is converted back to ND format. The dimension information of the destination cube is configured through the Nz2NdParamsFull parameter. See [DataCopy (UBToGM - inline conversion NZ2ND movement)](../gm_and_ub_data_movement.md#ZH-CN_TOPIC_0000002391805265) and other sections.
- **DN2NZ movement**: (Supported by ISASI products) Conversion movement from the DN format of depthwise convolution to the NZ format. See [DataCopy (GM -> L1 - inline conversion - DN2NZ movement)](../gm_and_l1_or_l0_data_movement.md#datacopy-gm---l1---inline-conversion---dn2nz-movement) section.

**Figure 4**  ND2NZ format conversion schematic diagram  

![](../../../../figures/nd2nz_conversion_half.png "ND2NZ format conversion schematic diagram")

## Inline Quantization Activation Movement (FixPipe/DataCopy Quantization Activation)

The output results of cube computation (Cube unit) are stored in the L0C Buffer, and usually require post-processing such as quantization, dequantization, and ReLU activation before they can be used for subsequent computation. The inline quantization activation movement API (DataCopy or FixPipe) of Ascend C can perform quantization type conversion and activation operations at the same time as moving L0C Buffer data to Global Memory or L1 Buffer, achieving the fusion of "movement + computation" and greatly reducing the overhead of additional computation instructions.

**Applicable scenarios**: Conversion and movement of cube multiply-accumulate results (float/int32_t) to half-precision/int8_t output in Matmul/Conv operators. See [DataCopy (L0C -> GM - Inline Quantization Activation Movement)](../gm_and_l1_or_l0_data_movement.md#datacopy-l0c-gm-quantization-activation), [Fixpipe (L0C -> GM - Inline Quantization Activation Movement)](../gm_and_l1_or_l0_data_movement.md#fixpipe-l0c-gm-quantization-activation), and other sections.

## Unaligned Data Movement (DataCopyPad)

The standard DataCopy API requires that the start addresses of the source and destination operands satisfy alignment constraints (for example, UB requires 32-byte alignment). When the actual data length does not satisfy the alignment requirement (for example, moving 17 half elements, totaling 34 bytes, which is not a multiple of 32), the DataCopyPad API is required. This API allows the last data block moved to be of unaligned length, and the padding value and padding mode can be configured through the padParams parameter, so that the destination address after movement remains aligned.

**Applicable scenarios**: Movement when the total data length does not satisfy the 32-byte alignment requirement, and scenarios where the tail data needs to be padded during movement. See [DataCopyPad (GM -> UB - Unaligned Data Movement)](../gm_and_ub_data_movement.md#ZH-CN_TOPIC_0000001894460401), [DataCopyPad (GM -> L1 - Unaligned Data Movement)](../gm_and_l1_or_l0_data_movement.md#datacopypad-gm---l1-unaligned-data-movement), and other sections.

## UB Internal Movement (Copy)

Copy is a data movement instruction dedicated to the Unified Buffer, supporting data movement among VECIN/VECCALC/VECOUT. Unlike DataCopy, the Copy API supports mask operations (which can control, bit by bit, which elements participate in the movement) and DataBlock interval control, and is suitable for data rearrangement and intermediate result staging within vector computation. See [UBToUB continuous data movement (Copy)](../ub_and_ub_data_movement.md#ZH-CN_TOPIC_0000002575088175), [UBToUB masked high-dimensional data movement (Copy)](../ub_and_ub_data_movement.md#ZH-CN_TOPIC_0000002575088175), and other sections.

## Cube Fractal Movement (Load2D/Load3D)

Load2D and Load3D are 2D/3D format fractal movement APIs dedicated to the cube computation path. They are responsible for moving cube data in L1 to the L0A Buffer/L0B Buffer according to the special fractal format (such as NZ arrangement) required by the Cube computation unit. These APIs can be accompanied by a transpose operation (LoadDataWithTranspose) and support loading sparse weight matrices (LoadDataWithSparse). See [LoadData (L1ToL0A-2D format fractal cube movement)](../l1_and_l0_data_movement.md#loaddata-l1---l0a-2d-format-fractal-cube-movement), [LoadData (L1ToL0A-3D format fractal cube movement)](../l1_and_l0_data_movement.md#loaddata-l1---l0a-3d-format-fractal-cube-movement), and other sections.

## Multi-dimensional Data Movement NDDMA (DataCopy)

NDDMA (N-Dimensional DMA) is a more flexible data movement method configured by dimension, supporting multi-dimensional movement between Global Memory and Unified Buffer. Unlike high-dimensional splitting movement (which uses the fixed mode of blockLen/blockCount/repeat), NDDMA allows developers to freely configure movement parameters dimension by dimension, with the movement dimension dim supporting 1 to 5 dimensions. Each dimension (loop) configures the following core parameters through the NdDmaLoopInfo structure:

- **loopSize**: the number of elements to be moved within the dimension.
- **loopSrcStride/loopDstStride**: the address interval between adjacent elements of the source/destination operand within the dimension (in element count as the unit). By configuring stride, effects such as strided movement, Transpose, and BroadCast can be achieved.
- **loopLpSize/loopRpSize**: the number of elements to be padded on the left/right side within the dimension. Through NdDmaConfig, the Padding value filling method can be further selected: constant filling (specifying a fixed value constantValue) or nearest-value filling (automatically selecting the boundary value for filling).

NDDMA independently uses a 32KB NDDMA Cache. Before using the DataCopy API, the cache must be refreshed through NdDmaDci\(\) to ensure data consistency in multi-core scenarios.

**Applicable scenarios**: scenarios that require free control of multi-dimensional movement modes, such as Padding filling of multi-dimensional Tensor, Transpose movement, BroadCast movement, and Slice sub-cube extraction. Refer to the [GMToUB Multi-dimensional Data Movement NDDMA (DataCopy)](../gm_and_ub_data_movement.md#ZH-CN_TOPIC_0000002544407954) section.
