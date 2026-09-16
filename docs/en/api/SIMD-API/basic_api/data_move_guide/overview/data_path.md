# Data Path<a name="ZH-CN_TOPIC_0000002535040662"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:06:11.169Z -->

## AI Core Hardware Architecture and Storage Units

<!-- npu="910b,A3" id1 -->
The AI Core is the core compute unit of the Ascend processor. Taking [NPU architecture version 2201](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md) as an example, its internal hierarchical storage architecture is shown in Figure 1, which mainly consists of the following components:

- **Compute units (highlighted in yellow in the figure):** These include the Cube (cube) compute unit, the Vector compute unit, and the Scalar compute unit, which are responsible for cube multiply-accumulate operations, vector operations, and scalar control flow, respectively.
- **Storage units:** From outside to inside by hierarchy, these are Global Memory (GM, located outside the AI Core), L1 Buffer, L0A/L0B/L0C Buffer, Unified Buffer (UB), BiasTable Buffer, Fixpipe Buffer, and so on. The capacity and alignment requirements of each storage unit vary. For details, see [Memory Hierarchy Architecture](./general_constraint_description.md#address-alignment-constraints).
- **Movement units (highlighted in yellow in the figure):** These include MTE1 (Memory Transfer Engine 1), MTE2, MTE3, and FixPipe, which are responsible for DMA transfers of data between different storage units. Among them, MTE2 is responsible for moving data in the GM  ->  Local Memory direction, MTE3 is responsible for moving data in the Local Memory -> GM direction, MTE1 is responsible for moving data from L1 Buffer to L0 Buffer, and FixPipe is responsible for moving the computation result out of L0C Buffer via [on-the-fly quantization](../../cube_compute_ISASI/cube_store_key_features/on_the_fly_quantization.md) and [on-the-fly activation](../../cube_compute_ISASI/cube_store_key_features/on_the_fly_relu.md).

**Figure 1**  Schematic diagram of the AI Core hardware architecture under NPU architecture version 2201  

![](../../../../figures/atlas_a2_a3_architecture.png)
<!-- end id1 -->

<!-- npu="950" id2 -->
Taking [NPU architecture version 3510](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md) as an example, its internal hierarchical storage architecture is shown in Figure 2. This architecture has the following changes in data paths:

- A unidirectional data path from L0C Buffer to UB is added.
- A data path from UB to L1 Buffer is added.
- Delete the data path from GM to L0A Buffer and L0B Buffer.
- Delete the data path from L1 Buffer to GM.

**Figure 2**  AI Core hardware architecture diagram under NPU architecture version 3510  

![](../../../../figures/ascend_950pr_950dt_architecture.png)
<!-- end id2 -->

  > [!NOTE]
  > - The same physical memory may correspond to multiple different [TPosition](../../aux_data_structures/TPosition.md) values. For example, the Unified Buffer (UB) is mapped to three logical positions, VECIN, VECCALC, and VECOUT, which represent the input, intermediate computation, and output stages of vector computation, respectively. The L1 Buffer is mapped to A1, B1, C1, and TSCM, whose specific meanings depend on the computation flow currently being served (left/right cube staging for cube multiplication or shared communication). This design allows the same physical memory to be assigned different logical semantics at different computation stages, and developers can select an appropriate TPosition based on the stage requirements of the programming model.
  > - The same logical position may correspond to different physical memory in different product models. For example, the logical position CO2 is mapped to Global Memory in Atlas A2 training products/Atlas A2 inference products, but is mapped to Unified Buffer in Atlas training/inference products. For details, see [Mapping Between Logical Positions and Physical Storage](../../../general_description_and_constraints.md#section1359919519819).

## Data Paths and Movement Pipelines

Based on the storage units and movement units described above, the data paths within the AI Core and their corresponding [hardware pipelines](../../sync_control/intra_core_sync/intra_core_synchronization_capability_overview.md#zh-cn_topic_0000002542725361_section1272612276459) are listed in the following table. Each data path specifies the source (SRC) and destination (DST) storage units, as well as the hardware pipeline that performs the movement. Understanding these paths is the basis for selecting the correct data movement API parameters.

  > [!NOTE] Description
  > The following table summarizes the data paths of all product models. To check whether a specific data path is supported on a particular product, refer to the product support table in the corresponding API reference document.

**Table 1**  Data movement function overview<a name="table1151112542363"></a>

| Source (SRC) | Destination (DST) | Pipeline | Function | Supported APIs |
|----------|----------|----------|----------|----------|
| Global Memory | L1 Buffer | MTE2 | Continuously moves cube data from Global Memory to L1 Buffer for staging. | DataCopy |
| Global Memory | L1 Buffer | MTE2 | Moves cube data from Global Memory to L1 Buffer for staging via high-dimensional splitting. | DataCopy |
| Global Memory | L1 Buffer | MTE2 | Moves cube data from Global Memory to L1 Buffer while performing on-the-fly ND-to-NZ format conversion. | DataCopy |
| Global Memory | L1 Buffer | MTE2 | Moves cube data from Global Memory to L1 Buffer while performing on-the-fly DN-to-NZ format conversion. | DataCopy |
| Global Memory | L1 Buffer | MTE2 | Moves unaligned data from Global Memory to L1 Buffer and pads the invalid boundary regions. | DataCopyPad |
| Global Memory | L1 Buffer | MTE2 | Moves NZ-format data from Global Memory to L1 Buffer. | Load2D |
| Global Memory | L1 Buffer | MTE2 | Moves NZ-format data from Global Memory to L1 Buffer. | Load2DV2 |
| Global Memory | L0A Buffer | MTE2 | Moves 2D-format fractal matrices from Global Memory to L0A Buffer as the left cube input for cube computation. | Load2D |
| Global Memory | L0B Buffer | MTE2 | Moves 2D-format fractal matrices from Global Memory to L0B Buffer as the right cube input for cube computation. | Load2D |
| Global Memory | Unified Buffer | MTE2 | Continuously moves cube data from Global Memory to Unified Buffer. | DataCopy |
| Global Memory | Unified Buffer | MTE2 | Moves cube data from Global Memory to Unified Buffer via high-dimensional splitting. | DataCopy |
| Global Memory | Unified Buffer | MTE2 | Moves data from Global Memory to Unified Buffer in a sliced manner. | DataCopy |
| Global Memory | Unified Buffer | MTE2 | Moves data from Global Memory to Unified Buffer while performing on-the-fly ND-to-NZ format conversion. | DataCopy |
| Global Memory | Unified Buffer | MTE2 | Moves data from Global Memory to Unified Buffer in a multi-dimensional manner using NDDMA. | DataCopy |
| Global Memory | Unified Buffer | MTE2 | Moves unaligned data from Global Memory to Unified Buffer and pads the invalid boundary regions. | DataCopyPad |
| Unified Buffer | Global Memory | MTE3 | Moves data from Unified Buffer to Global Memory while performing on-the-fly NZ-to-ND format conversion. | DataCopy |
| Unified Buffer | Global Memory | MTE3 | Moves unaligned data from Unified Buffer to Global Memory and pads the invalid boundary regions. | DataCopyPad |
| Unified Buffer | L1 Buffer | MTE3 | Continuously moves data from Unified Buffer to L1 Buffer. | DataCopy |
| Unified Buffer | L1 Buffer | MTE3 | Moves data from Unified Buffer to L1 Buffer via high-dimensional splitting. | DataCopy |
| Unified Buffer | L1 Buffer | MTE3 | Moves data from Unified Buffer to L1 Buffer while performing on-the-fly ND-to-NZ format conversion. | DataCopy |
| Unified Buffer | L1 Buffer | MTE3 | Moves unaligned data from Unified Buffer to L1 Buffer and pads the invalid boundary regions. | DataCopyPad |
| L1 Buffer | Unified Buffer | MTE3 | Continuously moves data from L1 Buffer to Unified Buffer. | DataCopyL1ToUB |
| L1 Buffer | Unified Buffer | MTE3 | Moves data from L1 Buffer to Unified Buffer via high-dimensional splitting. | DataCopyL1ToUB |
| L1 Buffer | L0A Buffer | MTE1 | Moves 2D-format fractal matrices from L1 Buffer to L0A Buffer as the left cube input for Cube cube multiplication. | Load2D |
| L1 Buffer | L0A Buffer | MTE1 | Moves 2D-format fractal matrices from L1 Buffer to L0A Buffer as the left cube input for Cube cube multiplication. | Load2DV2 |
| L1 Buffer | L0A Buffer | MTE1 | Moves 2D-format fractal matrices from L1 Buffer to L0A Buffer as the left cube input for Cube cube multiplication. | Load2DMX |
| L1 Buffer | L0A Buffer | MTE1 | Moves 2D-format fractal matrices from L1 Buffer to L0A Buffer while performing transposition. | LoadDataWithTranspose |
| L1 Buffer | L0A Buffer | MTE1 | Moves 3D-format fractal matrices from L1 Buffer to L0A Buffer, supporting 3D-Tile rule movement. | Load3D |
| L1 Buffer | L0B Buffer | MTE1 | Moves 2D-format fractal matrices from L1 Buffer to L0B Buffer as the right cube input for Cube cube multiplication. | Load2D |
| L1 Buffer | L0B Buffer | MTE1 | Moves 2D-format fractal matrices from L1 Buffer to L0B Buffer as the right cube input for Cube cube multiplication. | Load2DV2 |
| L1 Buffer | L0B Buffer | MTE1 | Moves 2D-format fractal matrices from L1 Buffer to L0B Buffer as the right cube input for Cube cube multiplication. | Load2DMX |
| L1 Buffer | L0B Buffer | MTE1 | Moves 2D-format fractal matrices from L1 Buffer to L0B Buffer while performing transposition. | LoadDataWithTranspose |
| L1 Buffer | L0B Buffer | MTE1 | Moves 3D-format fractal matrices from L1 Buffer to L0B Buffer, supporting 3D-Tile rule movement. | Load3D |
| L1 Buffer | L0B Buffer | MTE1 | Moves dense weight matrices from L1 Buffer to L0B Buffer, supporting sparse decompression. | LoadDataWithSparse |
| L1 Buffer | BiasTable Buffer | MTE1 | Continuously moves bias data from L1 Buffer to BiasTable Buffer for cube computation. | DataCopy |
| L1 Buffer | BiasTable Buffer | MTE1 | Moves bias data from L1 Buffer to BiasTable Buffer via high-dimensional splitting. | DataCopy |
| L1 Buffer | Fixpipe Buffer | FixPipe | Continuously moves the on-the-fly quantization and on-the-fly ReLU parameters required for cube data output from L1 Buffer to Fixpipe Buffer. | DataCopy |
| L1 Buffer | Fixpipe Buffer | FixPipe | Moves the on-the-fly quantization and on-the-fly ReLU parameters required for cube data output from L1 Buffer to Fixpipe Buffer via high-dimensional splitting. | DataCopy |
| L0C Buffer | Global Memory | FixPipe | Moves Cube computation results from L0C Buffer to Global Memory, supporting on-the-fly quantization and activation post-processing. | DataCopy |
| L0C Buffer | Global Memory | FixPipe | Moves Cube computation results from L0C Buffer to Global Memory, completing quantization, activation, and format conversion via the FixPipe pipeline. | FixPipe |
| L0C Buffer | L1 Buffer | FixPipe | Moves Cube computation results from L0C Buffer to L1 Buffer for staging, supporting on-the-fly quantization and activation. | DataCopy |
| L0C Buffer | L1 Buffer | FixPipe | Moves Cube computation results from L0C Buffer to L1 Buffer for staging, completing quantization, activation, and format conversion via FixPipe. | FixPipe |
| L0C Buffer | Unified Buffer | MTE3 | Moves Cube computation results from L0C Buffer to Unified Buffer, supporting on-the-fly quantization and activation post-processing. | DataCopy |
| L0C Buffer | Unified Buffer | MTE3 | Moves Cube computation results from L0C Buffer to Unified Buffer, completing quantization, activation, and format conversion via FixPipe. | FixPipe |
| Unified Buffer | Unified Buffer | PIPE_V | Continuously moves data within Unified Buffer. | DataCopy |
| Unified Buffer | Unified Buffer | PIPE_V | Moves data within Unified Buffer via high-dimensional splitting. | DataCopy |
| Unified Buffer | Unified Buffer | PIPE_V | Continuously moves data within Unified Buffer. | Copy |
| Unified Buffer | Unified Buffer | PIPE_V | Moves data within Unified Buffer in a masked high-dimensional manner. | Copy |
