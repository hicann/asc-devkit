# Basic Knowledge<a name="ZH-CN_TOPIC_0000002532228163"></a>

<!-- md-trans-meta sourceCommit=78e6b675ef9cf0d5985838e3f2b6ce251fdd50b0 translatedAt=2026-10-23T11:54:42.087Z pushedAt=2026-11-03T07:05:13.293Z nl=2 -->

>[!NOTE] Note 
>This section provides programming guidance for cube multiplication using advanced APIs. When using advanced APIs for actual cube programming, you need to check the supported product models through the [API Reference](../../../../api/SIMD-API/adv_api/cube_compute/cube_compute.md).

## Cube Multiplication Overview<a name="zh-cn_topic_0000001622194138_section833318865814"></a>

The formula for Matmul is: C = A \* B + bias. The following figure shows its schematic diagram.

-   A and B are source operands. A is the left cube with a shape of \[M, K\]; B is the right cube with a shape of \[K, N\].
-   C is the destination operand, which stores the cube multiplication result and has a shape of \[M, N\].
-   bias is the cube multiplication bias with a shape of \[1, N\]. This bias is applied to each row of the A\*B result cube.

**Figure 1**  Matmul cube multiplication diagram<a name="zh-cn_topic_0000001622194138_fig3161943163113"></a>  
![](../../../figures/mm_diag.png "Matmul cube multiplication diagram")

## Cube Multiplication Data Flow<a name="zh-cn_topic_0000001622194138_section295172162917"></a>

Before learning about the cube multiplication data flow, review the concepts of several important storage **logical positions**. For the mapping between logical positions and physical storage, see [Mapping Between Logical Positions and Physical Storage](../../../../api/SIMD-API/general_description_and_constraints.md#section1359919519819):

-   Storage location for loaded data: A1, used to store the entire Cube A, analogous to the secondary cache in a CPU multi-level cache;
-   Storage location for loaded data: B1, used to store the entire cube B, analogous to the secondary cache in a CPU multi-level cache;
-   Storage location for loaded data: C1, used to store the entire Bias cube for cube multiplication, analogous to the secondary cache in a CPU multi-level cache;
-   Storage location for loaded data: A2, used to store the small Cube A blocks after blocking, analogous to the primary cache in a CPU multi-level cache;
-   Storage location for loaded data: B2, used to store the small cube B blocks after blocking, analogous to the primary cache in a CPU multi-level cache;
-   Storage location for loaded data: C2, used to store the small Bias cube blocks for cube multiplication after blocking, analogous to the primary cache in a CPU multi-level cache;
-   Storage location for result data: CO1, used to store the small result cube C blocks, which can be understood as Cube Out;
-   Storage location for result data: CO2, used to store the entire result cube C, which can be understood as Cube Out;
-   Storage location of temporary variables: VECCALC, which is generally used when temporary variables are required for computation.

**Cube multiplication data flow** refers to the flow of cube multiplication inputs and outputs among the storage locations. The data flow of logical positions is shown in the following figure (bias is omitted for simplicity):

-   The data flow of cube A from the input position to A2 is as follows (the input position can be GM or VECOUT): GM-\>A2, GM-\>A1-\>A2; VECOUT-\>A1-\>A2.

    Since A1 has a larger space than A2, data can be loaded from GM or VECOUT into A1 for caching first. Before the data is used for Cube computation, it is directly loaded from A1 into A2. In this way, the waiting time before computation can be reduced when a large amount of data is loaded, thereby improving performance. The data flow GM-\>A2 is used only in scenarios where a small amount of data is loaded.

-   The data flow of cube B from the input position to B2 is as follows (the input position can be GM or VECOUT): GM-\>B2, GM-\>B1-\>B2; VECOUT-\>B1-\>B2.

    Since B1 has a larger space than B2, data can be loaded from GM or VECOUT into B1 for caching first. Before the data is used for Cube computation, it is directly loaded from B1 into B2. In this way, the waiting time before computation can be reduced when a large amount of data is loaded, thereby improving performance. The data flow GM-\>B2 is used only in scenarios where a small amount of data is loaded.

-   Complete the A2\*B2=CO1 computation.
-   CO1 data is aggregated into CO2: CO1-\>CO2.
-   From CO2 to the output position (the output position can be GM or VECIN): CO2-\>GM/CO2-\>VECIN.

![](../../../figures/mat_pos_40.png)

## Data Format<a name="zh-cn_topic_0000001622194138_section1453415011"></a>

When performing Matmul cube multiplication, two fractal formats are mainly involved: ND and [NZ](../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md#li19960204116136). For other data formats, see [Data Layout Format](../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md).

-   ND: a common format, an N-dimensional tensor.
-   NZ: a special format introduced to meet the high-performance computing requirements of the Cube computing unit in the AI Core.


    The transformation process from ND to NZ is as follows:

    ```
    (..., N, H, W )->pad->(..., N, H1*H0, W1*W0)->reshape->(..., N, H1, H0, W1, W0)->transpose->(..., N, W1, H1, H0, W0)
    ```

    As shown in the following figure, a cube of size (W, H) is divided into (H1\*W1) fractals, which are arranged in column-major order, forming an N-like shape. Each fractal contains (H0\*W0) elements, which are arranged in row-major order, forming a z-like shape. Therefore, this data format is called the NZ (large N, small Z) format.

    ![](../../../figures/nd2nz_trans_1.png)

    The following uses a specific example to further illustrate the difference in data layout between the ND and NZ formats. Assume that the fractal format is 2\*2. For the 4\*4 cube shown in the following figure, when stored in the ND (1, 4, 4) and NZ (1, 2, 2, 2, 2) formats, the data layouts in memory are as follows:

    ND: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15

    NZ: 0, 1, 4, 5, 8, 9, 12, 13, 2, 3, 6, 7, 10, 11, 14, 15

    ![](../../../figures/nd2nz_2.png)

## Data Tiling<a name="zh-cn_topic_0000001622194138_section68451031218"></a>

-   **Multi-core Splitting**

    To implement multi-core parallelism, the cube data needs to be split and distributed to different cores for processing. The splitting strategy is shown in the following figure:

    -   For cube A, split it along the M axis into multiple singleCoreM blocks. Each core processes data of size SingleCoreM \* K.
    -   For cube B, split it along the N axis into multiple singleCoreN blocks. Each core processes data of size K \* SingleCoreN.
    -   For cube C, multiplying cube A of size SingleCoreM \* K by cube B of size K \* SingleCoreN yields cube C of size SingleCoreM \* SingleCoreN, which is the size of cube C output on a single core.

    For example, in the following figure, eight cores participate in the computation. Cube A is split into four blocks along the M axis, and cube B is split into two blocks along the N axis. Each core processes only one block (for example, the green part in the figure is the data involved in the computation on core3): multiplying the cube A block of size SingleCoreM \* K by the cube B block of size SingleCoreN\* K yields the cube C block of size SingleCoreM \* SingleCoreN.

    ![](../../../figures/nd2nz_new.png)

    In addition, the length of the K axis processed on a single core is SingleCoreK. For scenarios with a large K axis, the K axis can be split into multiple singleCoreK blocks. For detailed cases, see [Enabling Multi-core K Splitting for Matmul Advanced APIs](../../best_practices/matmul_tuning_cases/matmul_multi_core_split_k.md).

-   **Intra-core Splitting**

    In most cases, the storage of Local Memory cannot fully accommodate the input and output of an operator. Therefore, a portion of the input needs to be loaded for computation and then moved out, followed by loading the next portion of the input for computation, until the complete final result is obtained. That is, intra-core input splitting is required. The splitting strategy is as follows:

    -   For cube A, split along the M axis to divide singleCoreM into multiple baseM blocks. The number of blocks corresponds to mIter in the figure. Split along the K axis into multiple baseK blocks.
    -   For cube B, split along the N axis to divide singleCoreN into multiple baseN blocks. The number of blocks corresponds to nIter in the figure. Split along the K axis into multiple baseK blocks.
    -   For cube C, the block of size baseM\*baseK in cube A and the block of size baseK\*baseN in cube B are multiplied and accumulated to obtain the block of size baseM\*baseN at the corresponding position in cube C. For example, the green cube block 5 in the result cube in the figure is obtained through the following accumulation process: a\*a+b\*b+c\*c+d\*d+e\*e+f\*f.

    ![](../../../figures/nd2nz_43.png)

    In addition to the basic block shapes baseM, baseN, and baseK, there are some commonly used tiling parameters, whose meanings are as follows:

    -   <a name="zh-cn_topic_0000001622194138_li123441825184114"></a>**iterateOrder**: One Iterate computes a cube C tile of size \[baseM, baseN\]. After the Iterate is complete, Matmul automatically offsets the position of the cube C output by the next Iterate. iterateOrder indicates the order of the automatic offset.

        -   **0** indicates offsetting along the M axis first and then along the N axis.
        -   **1** indicates offsetting along the N axis first and then along the M axis.

        In the example shown in the preceding figure, the value of **iterateOrder** is **0**.

    -   depthA1, depthB1: number of cube tiles stored in L1 Buffer (A1) and L1 Buffer (B1) that are fully loaded into L0A Buffer (A2) and L0B Buffer (B2), respectively. The storage sizes of L0A Buffer (A2) and L0B Buffer (B2) are baseM \* baseK and baseN \* baseK, respectively. That is, depthA1 is the number of baseM \* baseK blocks contained in the A1 cube tile, and depthB1 is the number of baseN \* baseK blocks contained in the B1 cube tile.
    -   stepM, stepN: stepM is the multiple of baseM in the buffer M direction of the left cube cached in L1 Buffer (A1), and stepN is the multiple of baseN in the buffer N direction of the right cube cached in L1 Buffer (B1).
    -   stepKa, stepKb: stepKa is the multiple of baseK in the buffer K direction of the left cube cached in L1 Buffer (A1), and stepKb is the multiple of baseK in the buffer K direction of the right cube cached in L1 Buffer (B1).
