# Data Movement Concepts<a name="ZH-CN_TOPIC_0000002534880720"></a>

<!-- md-trans-meta sourceCommit=2f9021e91c029d6fd00e108fa5e17d2275791fa1 translatedAt=2026-11-02T05:02:57.598Z pushedAt=2026-11-18T01:25:06.866Z nl=2 -->

## Overview of Movement Function Classification

The data movement APIs provide comprehensive data movement capabilities, covering everything from the most basic continuous movement to complex inline conversion and quantization activation. In ascending order of functional complexity, the movement capabilities can be classified into the following categories: basic data movement, high-dimensional splitting movement, slice movement, inline format conversion movement, inline quantization activation movement, unaligned movement, Unified Buffer (UB) internal movement (Copy), matrix fractal movement, and multi-dimensional data movement NDDMA. Each category has corresponding detailed API descriptions in the subsequent sections.

## Basic Data Movement (DataCopy Continuous)

The simplest movement mode. It moves a segment of continuous data in memory at the source address to the destination address without any modification. The format and content of the data remain unchanged during transmission, and no conversion or computation is performed. In the function prototype, the uint32_t count parameter specifies the number of elements to be moved (in units of sizeof(T)), and the API automatically converts the number of elements into the corresponding number of bytes.

**Applicable scenarios**: Fast movement when the data is known to be stored continuously at both the source and destination addresses and the addresses meet the alignment requirements. It is typically used for batch data loading before vector computation (GM->UB) and result write-back after computation (UB->GM). See [DataCopy (continuous data movement between GM and UB)](../GM_UB_data_move.md#ZH-CN_TOPIC_0000002382908021), [DataCopy (continuous data movement between GM and L1)](../GM_L1_or_L0_data_move.md#ZH-CN_TOPIC_0000002574022813), and other sections.

**Figure 1**  Basic continuous movement schematic diagram  

![](../../../../figures/continuous_data_copy_diagram.png "Basic continuous movement schematic diagram")

## High-Dimensional Splitting Data Movement (DataCopy High-Dimensional Splitting)

When the source or destination operand is distributed non-contiguously in memory at fixed intervals, the high-dimensional splitting mode is required. This mode introduces the **DataCopyParams** (or **DataCopyExtParams**) parameter structure and describes the "block-repeat" pattern of the movement through the following core parameters:

- **blockLen**: Length of each data block, in units of **DataBlock** (32 bytes). **DataBlock** is the minimum addressing granularity of Ascend C data movement. For example, to move 128 elements of the half type (2 bytes each, 256 bytes in total = 8 DataBlocks), **blockLen** = 8.
- **blockCount**: Number of data blocks to be moved. Each round of movement takes **blockCount** contiguous data blocks in sequence.
- **srcStride/dstStride (srcGap/dstGap)**: Address interval between adjacent data blocks (in units of **DataBlock**). When stride = blockLen, the data is actually continuous in memory; when stride\>blockLen, it indicates that there is a jump between each data block, implementing non-contiguous (strided) movement.

**Applicable scenarios**: Common block (Tiling) data movement in operators such as Conv and Matmul, where data is distributed non-contiguously at the source or destination end at a fixed stride. See [DataCopy (GM and UB high-dimensional splitting data movement)](../GM_UB_data_move.md#ZH-CN_TOPIC_00000023829080211), [DataCopy (GM and L1 high-dimensional splitting data movement)](../GM_L1_or_L0_data_move.md#ZH-CN_TOPIC_0000002543262916), and other sections.

**Figure 2**  Schematic diagram of continuous high-dimensional splitting data movement  

![](../../../../figures/continuous_data_copy_diagram.png "Schematic diagram of high-dimensional splitting continuous movement")

**Figure 3**  Schematic diagram of high-dimensional splitting non-contiguous copy  

![](../../../../figures/discontinuous_data_copy_diagram.png "Schematic diagram of high-dimensional splitting non-continuous movement")

## Slice Data Movement (DataCopy Slice)

Slice movement supports extracting a rectangular sub-region (Slice) from a multi-dimensional Tensor for transfer. By describing the slice parameters (**startIndex**, **endIndex**, **stride**, **burstLen**, **shapeValue**) on each dimension of the source and destination through the **SliceInfo** structure array, flexible slicing of 1 to 5 dimensions can be implemented. Slice movement is essentially a special form of high-dimensional non-continuous transfer, but the API provides a more intuitive multi-dimensional configuration method.

**Applicable scenarios**: Extracting an ROI (Region of Interest) sub-region from a large feature map, extracting the valid data region in Pad mode, and so on. See [DataCopy (GM and UB slice data movement)](../GM_UB_data_move.md#ZH-CN_TOPIC_0000002568770166).

## Inline Format Conversion Data Movement (ND2NZ/NZ2ND/DN2NZ)

In the Ascend matrix computation flow, data usually needs to be converted from the ND format (standard data arrangement, that is, conventional formats such as NHWC/NCHW) to the NZ format (fractal matrix arrangement, that is, row-column arrangement organized by small matrix blocks), and then converted back from NZ to ND after computation is complete. Ascend C provides the "inline conversion" capability, which completes format conversion at the same time as data movement without requiring additional conversion instructions, thereby achieving "zero-overhead format conversion during movement".

- **ND2NZ copy**: While data is moved from Global Memory to the UB/L1 Buffer, the ND format is converted to the NZ format. The fractal dimension information of the source matrix is configured through the **Nd2NzParams** parameter. See [DataCopy (GMToUB inline conversion ND2NZ copy)](../GM_UB_data_move.md#ZH-CN_TOPIC_0000002349187356), [DataCopy (GMToL1 inline conversion - ND2NZ copy)](../GM_L1_or_L0_data_move.md#ZH-CN_TOPIC_0000002573902841), and other sections.
- **NZ2ND movement**: While data is moved from UB to Global Memory, the NZ format is converted back to the ND format. The dimension information of the destination matrix is configured through the **Nz2NdParamsFull** parameter. See [DataCopy (UBToGM inline conversion NZ2ND movement)](../GM_UB_data_move.md#ZH-CN_TOPIC_0000002391805265) and other sections.
- **DN2NZ conversion**: (Supported by ISASI products) Conversion from the DN format of depthwise convolution to the NZ format. See [DataCopy (GMToL1 inline conversion - DN2NZ conversion)](../GM_L1_or_L0_data_move.md#ZH-CN_TOPIC_0000002573902899).

**Figure 4** ND2NZ format conversion schematic diagram

![](../../../../figures/nd2nz_conversion_half.png "ND2NZ format conversion schematic diagram")

## Inline Quantization Activation Movement (FixPipe/DataCopy Inline Quantization and Activation)

The output results of matrix computation (Cube unit) are stored in the **L0C Buffer**, and usually require post-processing such as quantization (Quantize), dequantization (Dequantize), and ReLU activation before they can be used by subsequent computations. The Ascend C inline quantization activation movement APIs (**DataCopy** or **FixPipe**) can perform quantization type conversion and activation operations while moving **L0C Buffer** data to **Global Memory** or the **L1 Buffer**, achieving the fusion of "movement + computation" and greatly reducing the overhead of additional computation instructions.

**Applicable scenarios**: Conversion and movement of matrix multiply-accumulate results (float/int32_t) to half-precision/int8_t output in Matmul/Conv operators. See [DataCopy (L0CToGM inline quantization activation movement)](../GM_L1_or_L0_data_move.md#ZH-CN_TOPIC_0000002573902843), [Fixpipe (L0CToGM inline quantization activation movement)](../GM_L1_or_L0_data_move.md#ZH-CN_TOPIC_0000002543422576), and other sections.

## Unaligned Data Movement (DataCopyPad)

The standard **DataCopy** API requires that the start addresses of the source and destination operands satisfy alignment constraints (for example, UB requires 32-byte alignment). When the actual data length does not satisfy the alignment requirement (for example, moving 17 half-type data elements, totaling 34 bytes, which is not an integer multiple of 32), the **DataCopyPad** API is required. This API allows the last data block to be moved with an unaligned length, and the padding value and padding mode can be configured through the **padParams** parameter, so that the destination address remains aligned after the movement.

**Applicable scenarios**: Movement when the total data length does not satisfy the 32-byte alignment requirement, and scenarios where tail data needs to be padded during movement. See [DataCopyPad (GMToUB unaligned data movement)](../GM_UB_data_move.md#ZH-CN_TOPIC_0000001894460401), [DataCopyPad (GMToL1 unaligned data movement)](../GM_L1_or_L0_data_move.md#ZH-CN_TOPIC_0000002543262891), and other sections.

## UB Internal Data Movement (Copy)

**Copy** is a data movement instruction dedicated to the UB, supporting data movement among **VECIN**/**VECCALC**/**VECOUT**. Unlike **DataCopy**, the **Copy** API supports mask operations (which can control, bit by bit, which elements participate in the movement) and **DataBlock** interval control, and is suitable for data rearrangement and temporary storage of intermediate results within vector computation. See [Copy (UBToUB continuous data movement)](../UB_UB_data_move.md#ZH-CN_TOPIC_0000002575088175), [Copy (UBToUB mask-based high-dimensional data movement)](../UB_UB_data_move.md#ZH-CN_TOPIC_0000002575088676), and other sections.

## Matrix Fractal Movement (LoadData (2D Matrix Transfer)/LoadData (Convolution Data Transfer))

LoadData (2D matrix movement) and LoadData (convolution data movement) are 2D/3D format fractal movement APIs dedicated to the matrix computation path. They move matrix data in L1 to the L0A Buffer/L0B Buffer in the special fractal format (such as NZ arrangement) required by the Cube computation unit. These APIs can be accompanied by a transpose operation (LoadDataWithTranspose) and support loading sparse weight matrices (LoadDataWithSparse). See [L1ToL0A 2D Format Fractal Matrix Movement (LoadData (2D Matrix Movement))](../L1_L0_data_move.md#ZH-CN_TOPIC_0000002573902845), [L1ToL0A 3D Format Fractal Matrix Movement (LoadData (Convolution Data Movement))](../L1_L0_data_move.md#ZH-CN_TOPIC_0000002574022819), and other sections.

## N-Dimensional Data Movement NDDMA (DataCopy)

NDDMA (N-Dimensional DMA) is a more flexible data movement method configured by dimension, supporting multi-dimensional movement between Global Memory and UB. Unlike high-dimensional splitting movement (which uses the fixed blockLen/blockCount/repeat mode), NDDMA allows developers to freely configure movement parameters dimension by dimension, and the movement dimension dim supports 1\~5 dimensions. Each dimension (loop) configures the following core parameters through the **NdDmaLoopInfo** structure:

- **loopSize**: The number of elements to be moved within the dimension.
- **loopSrcStride/loopDstStride**: The address interval between adjacent elements of the source/destination operand within the dimension (in units of element count). By configuring stride, effects such as strided movement, Transpose, and BroadCast can be achieved.
- **loopLpSize/loopRpSize**: The number of elements to be padded on the left/right side within the dimension. Through **NdDmaConfig**, the padding value filling method can be further selected: constant padding (specifying a fixed value constantValue) or nearest-value padding (automatically selecting the boundary value for padding).

NDDMA independently uses a 32KB NDDMA Cache. Before using the DataCopy API, the cache must be flushed through NdDmaDci\(\) to ensure data consistency in multi-core scenarios.

**Applicable scenarios**: Scenarios that require free control of multi-dimensional movement modes, such as padding of multi-dimensional Tensors, Transpose movement, BroadCast movement, and Slice sub-matrix extraction. See the [DataCopy (GMToUB N-Dimensional Data Movement NDDMA)](../GM_UB_data_move.md#ZH-CN_TOPIC_0000002544407954) section.
