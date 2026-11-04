# Data Path<a name="ZH-CN_TOPIC_0000002535040662"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T13:57:49.218Z pushedAt=2026-10-26T07:41:44.255Z nl=2 -->

## AI Core Hardware Architecture and Storage Units

<!-- npu="910b,A3" id1 -->
The AI Core is the core compute unit of the Ascend processor. Taking [NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md) as an example, its internal hierarchical storage architecture is shown in Figure 1, which mainly consists of the following components:

- **Compute unit (highlighted in green in the figure):** Includes the Cube (matrix) compute unit, the Vector compute unit, and the Scalar compute unit, which are responsible for matrix multiplication and accumulation, vector operations, and scalar control flow, respectively.
- **Storage unit:** From outside to inside by hierarchy, these are Global Memory (GM, located outside the AI Core), L1 Buffer, L0A/L0B/L0C Buffer, Unified Buffer (UB), BiasTable Buffer, Fixpipe Buffer, and so on. The capacity and alignment requirements of each storage unit differ. For details, see [Memory Hierarchy Architecture](./overall_constraints.md#address-alignment-constraints).
- **Transfer unit (highlighted in yellow in the figure):** Includes MTE1 (Memory Transfer Engine 1), MTE2, MTE3, and FixPipe, which are responsible for DMA transfers of data between different storage units. Among them, MTE2 is responsible for data movement in the GM -> Local Memory direction, MTE3 is responsible for data movement in the Local Memory -> GM direction, MTE1 is responsible for data movement in the L1 Buffer -> L0 Buffer direction, and FixPipe is responsible for moving the computation result out of the L0C Buffer through [on-the-fly quantization](../../cube_compute_ISASI/cube_store_key_features/accompanying_quantization.md) and [on-the-fly activation](../../cube_compute_ISASI/cube_store_key_features/accompanying_relu.md).

**Figure 1** Schematic diagram of the AI Core hardware architecture under NPU architecture version 2201  

![](../../../../figures/atlas_a2_a3_architecture.png)
<!-- end id1 -->

<!-- npu="950" id2 -->
Taking [NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md) as an example, its internal hierarchical storage architecture is shown in Figure 2. This architecture has the following changes in the data path:

- A unidirectional data path from the L0C Buffer to the UB is added.
- A data path from the UB to the L1 Buffer is added.
- Delete the data path from GM to L0A Buffer and L0B Buffer.
- Delete the data path from L1 Buffer to GM.

**Figure 2**  AI Core hardware architecture diagram under NPU architecture version 3510  

![](../../../../figures/npu_3510_hw_arch_sync.png)
<!-- end id2 -->

  > [!NOTE]Note
  > - The same physical memory may correspond to multiple different [TPosition](../../../basic_api/aux_data_structures/TPosition.md) values. For example, UB is simultaneously mapped to the three logical positions VECIN, VECCALC, and VECOUT, which represent the input, intermediate computation, and output stages of vector computation, respectively. L1 Buffer is simultaneously mapped to A1, B1, C1, and TSCM, whose specific meanings depend on the computation flow currently being served (left/right matrix staging for matrix multiplication or shared communication). This design allows the same physical memory to be assigned different logical semantics at different computation stages, and developers can select an appropriate TPosition based on the stage requirements of the programming model.
  > - The same logical position may correspond to different physical memory in different product models. For example, the logical position CO2 is mapped to Global Memory in Atlas A2 products, but to UB in Atlas training and inference products. For details, see [Mapping Between Logical Positions and Physical Storage](../../../general_description_and_constraints.md#section1359919519819).

## Data Path and Transfer Pipeline

Based on the storage units and transfer units described above, the data paths within the AI Core and their corresponding [hardware pipelines](../../sync_control/intra_core_sync/intra_core_sync_overview.md#zh-cn_topic_0000002542725361_section1272612276459) are shown in the following table. Each data path specifies the source (SRC) and destination (DST) storage units as well as the hardware pipeline that performs the movement. Understanding these paths is the basis for selecting the correct data movement API parameters.

  > [!NOTE]
  > The following table summarizes the data paths of all product models. For whether each data path is supported on a specific product, refer to the product support table in the corresponding API reference document.

**Table 1**  Data movement function overview<a name="table1151112542363"></a>

| Source (SRC) | Destination (DST) | Pipeline | Function | Supported APIs |
|----------|----------|----------|----------|----------|
| Global Memory | L1 Buffer | MTE2 | Continuously moves matrix data from Global Memory to L1 Buffer for staging. | DataCopy |
| Global Memory | L1 Buffer | MTE2 | Moves matrix data from Global Memory to L1 Buffer for staging using high-dimensional splitting. | DataCopy |
| Global Memory | L1 Buffer | MTE2 | Moves matrix data from Global Memory to L1 Buffer while performing on-the-fly conversion from ND to NZ format. | DataCopy |
| Global Memory | L1 Buffer | MTE2 | Moves matrix data from Global Memory to L1 Buffer while performing on-the-fly conversion from DN to NZ format. | DataCopy |
| Global Memory | L1 Buffer | MTE2 | Moves unaligned data from Global Memory to L1 Buffer and pads the invalid region. | DataCopyPad |
| Global Memory | L1 Buffer | MTE2 | Moves NZ-format data from Global Memory to L1 Buffer. | LoadData (2D matrix movement) |
| Global Memory | L1 Buffer | MTE2 | Moves NZ-format data from Global Memory to L1 Buffer. | LoadData (2D matrix movement V2) |
| Global Memory | L0A Buffer | MTE2 | Moves a 2D format fractal matrix from Global Memory to L0A Buffer as the left matrix input for matrix computation. | LoadData (2D matrix movement) |
| Global Memory | L0B Buffer | MTE2 | Moves a 2D format fractal matrix from Global Memory to L0B Buffer as the right matrix input for matrix computation. | LoadData (2D matrix movement) |
| Global Memory | UB | MTE2 | Continuously moves matrix data from Global Memory to UB. | DataCopy |
| Global Memory | UB | MTE2 | Moves matrix data from Global Memory to UB using high-dimensional splitting. | DataCopy |
| Global Memory | UB | MTE2 | Moves data from Global Memory to UB using the slicing method. | DataCopy |
| Global Memory | UB | MTE2 | Moves data from Global Memory to UB while performing on-the-fly conversion from ND to NZ format. | DataCopy |
| Global Memory | UB | MTE2 | Moves data from Global Memory to UB in a multi-dimensional manner using NDDMA. | DataCopy |
| Global Memory | UB | MTE2 | Moves unaligned data from Global Memory to UB and pads the invalid region. | DataCopyPad |
| UB | Global Memory | MTE3 | Continuously moves data from UB to Global Memory. | DataCopy |
| UB | Global Memory | MTE3 | Moves data from UB to Global Memory using high-dimensional splitting. | DataCopy |
| UB | Global Memory | MTE3 | Moves data from UB to Global Memory using the slicing method. | DataCopy |
| UB | Global Memory | MTE3 | Moves data from UB to Global Memory while performing on-the-fly conversion from NZ to ND format. | DataCopy |
| UB | Global Memory | MTE3 | Moves unaligned data from UB to Global Memory, using dummy data for padding when reading UB and discarding the dummy data when writing to Global Memory. | DataCopyPad |
| UB | L1 Buffer | MTE3 | Continuously moves data from UB to L1 Buffer. | DataCopy |
| UB | L1 Buffer | MTE3 | Moves data from UB to L1 Buffer using high-dimensional splitting. | DataCopy |
| UB | L1 Buffer | MTE3 | Moves data from UB to L1 Buffer while performing on-the-fly conversion from ND to NZ format. | DataCopy |
| UB | L1 Buffer | MTE3 | Moves unaligned data from UB to L1 Buffer and pads the invalid region. | DataCopyPad |
| L1 Buffer | UB | MTE1 | Continuously moves data from L1 Buffer to UB. | DataCopyL1ToUB |
| L1 Buffer | UB | MTE1 | Moves data from L1 Buffer to UB using high-dimensional splitting. | DataCopyL1ToUB |
| L1 Buffer | Global Memory | MTE3 | Continuously moves data from L1 Buffer to Global Memory. | DataCopy |
| L1 Buffer | Global Memory | MTE3 | Moves data from L1 Buffer to Global Memory using high-dimensional splitting. | DataCopy |
| L1 Buffer | L0A Buffer | MTE1 | Moves a 2D format fractal matrix from L1 Buffer to L0A Buffer as the left matrix input for Cube matrix multiplication. | LoadData (2D matrix movement) |
| L1 Buffer | L0A Buffer | MTE1 | Moves a 2D format fractal matrix from L1 Buffer to L0A Buffer as the left matrix input for Cube matrix multiplication. | LoadData (2D matrix movement V2) |
| L1 Buffer | L0A Buffer | MTE1 | Moves a 2D format fractal matrix from L1 Buffer to L0A Buffer as the left matrix input for Cube matrix multiplication. | LoadData (MX matrix movement) |
| L1 Buffer | L0A Buffer | MTE1 | Moves a 2D format fractal matrix from L1 Buffer to L0A Buffer while performing transposition. | LoadDataWithTranspose |
| L1 Buffer | L0A Buffer | MTE1 | Moves a 3D format fractal matrix from L1 Buffer to L0A Buffer, supporting 3D-Tile rule movement. | LoadData (convolution data movement) |
| L1 Buffer | L0A Buffer | MTE1 | Moves a 3D format fractal matrix from L1 Buffer to L0A Buffer, supporting 3D-Tile rule movement and configuring the K-axis offset of the output matrix. | LoadDataWithStride |
| L1 Buffer | L0B Buffer | MTE1 | Moves a 2D format fractal matrix from L1 Buffer to L0B Buffer as the right matrix input for Cube matrix multiplication. | LoadData (2D matrix movement) |
| L1 Buffer | L0B Buffer | MTE1 | Moves a 2D format fractal matrix from L1 Buffer to L0B Buffer as the right matrix input for Cube matrix multiplication. | LoadData (2D matrix movement V2) |
| L1 Buffer | L0B Buffer | MTE1 | Moves a 2D format fractal matrix from L1 Buffer to L0B Buffer as the right matrix input for Cube matrix multiplication. | LoadData (MX matrix movement) |
| L1 Buffer | L0B Buffer | MTE1 | Moves a 2D format fractal matrix from L1 Buffer to L0B Buffer while performing transposition. | LoadDataWithTranspose |
| L1 Buffer | L0B Buffer | MTE1 | Moves a 3D format fractal matrix from L1 Buffer to L0B Buffer, supporting 3D-Tile rule movement. | LoadData (convolution data movement) |
| L1 Buffer | L0B Buffer | MTE1 | Moves a 3D format fractal matrix from L1 Buffer to L0B Buffer, supporting 3D-Tile rule movement and configuring the K-axis offset of the output matrix. | LoadDataWithStride |
| L1 Buffer | L0B Buffer | MTE1 | Moves a dense weight matrix from L1 Buffer to L0B Buffer while supporting sparse decompression. | LoadDataWithSparse |
| L1 Buffer | BiasTable Buffer | MTE1 | Continuously moves bias data from L1 Buffer to BiasTable Buffer for matrix computation. | DataCopy |
| L1 Buffer | BiasTable Buffer | MTE1 | Moves bias data from L1 Buffer to BiasTable Buffer using high-dimensional splitting. | DataCopy |
| L1 Buffer | Fixpipe Buffer | FixPipe | Continuously moves the on-the-fly quantization and on-the-fly ReLU parameters required for moving matrix data out from L1 Buffer to Fixpipe Buffer. | DataCopy |
| L1 Buffer | Fixpipe Buffer | FixPipe | Moves the on-the-fly quantization and on-the-fly ReLU parameters required for moving matrix data out from L1 Buffer to Fixpipe Buffer using high-dimensional splitting. | DataCopy |
| L0C Buffer | Global Memory | FixPipe | Moves the Cube computation result from L0C Buffer to Global Memory, supporting on-the-fly quantization and activation post-processing. | DataCopy |
| L0C Buffer | Global Memory | FixPipe | Moves the Cube computation result from L0C Buffer to Global Memory, completing quantization, activation, and format conversion through the FixPipe pipeline. | FixPipe |
| L0C Buffer | L1 Buffer | FixPipe | Moves the Cube computation result from L0C Buffer to L1 Buffer for staging, supporting on-the-fly quantization and activation. | DataCopy |
| L0C Buffer | L1 Buffer | FixPipe | Moves the Cube computation result from L0C Buffer to L1 Buffer for staging, completing quantization, activation, and format conversion through FixPipe. | FixPipe |
| L0C Buffer | UB | MTE3 | Moves the Cube computation result from L0C Buffer to UB, supporting on-the-fly quantization and activation post-processing. | DataCopy |
| L0C Buffer | UB | PIPE_FIX | Moves the Cube computation result from L0C Buffer to UB, completing quantization, activation, and format conversion through FixPipe. | FixPipe |
| UB | UB | PIPE_V | Continuous data movement within UB. | DataCopy |
| UB | UB | PIPE_V | Moves data within UB using high-dimensional splitting. | DataCopy |
| UB | UB | PIPE_V | Continuous data movement within UB. | Copy |
| UB | UB | PIPE_V | Masked high-dimensional data movement within UB. | Copy |
