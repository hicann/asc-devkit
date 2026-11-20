# Basic Knowledge<a name="ZH-CN_TOPIC_0000002532228163"></a>

<!-- md-trans-meta sourceCommit=78e6b675ef9cf0d5985838e3f2b6ce251fdd50b0 translatedAt=2026-11-09T10:31:48.277Z pushedAt=2026-11-19T06:32:19.748Z nl=2 -->

> [!NOTE] Note 
>
> This section provides programming guidance for cube multiplication using advanced APIs. When using advanced APIs for actual cube programming, you need to check the supported product models through the [API Reference](../../../../api/SIMD-API/adv_api/cube_compute/cube_compute.md).

## Cube Multiplication Overview<a name="zh-cn_topic_0000001622194138_section833318865814"></a>

The formula of Matmul is: C = A \* B + bias. The following figure shows its schematic diagram.

-   A and B are source operands. A is the left cube with the shape \[M, K\], and B is the right cube with the shape \[K, N\].
-   C is the destination operand, which is the cube that stores the cube multiplication result, with the shape \[M, N\].
-   Bias is the cube multiplication bias with the shape \[1, N\]. This bias is applied to each row of the A\*B result cube.

**Figure 1**  Matmul cube multiplication schematic diagram<a name="zh-cn_topic_0000001622194138_fig3161943163113"></a>  
![](../../../figures/mm_diag.png "Matmul cube multiplication schematic diagram")

## Cube Multiplication Data Stream<a name="zh-cn_topic_0000001622194138_section295172162917"></a>

Before learning about the cube multiplication data flow, review the concepts of several important storage **logical positions**. For the mapping between logical positions and physical storage, see [Mapping Between Logical Positions and Physical Storage](../../../../api/SIMD-API/general_description_and_constraints.md#section1359919519819):

-   Storage location for data loading: A1, used to store the entire A cube, comparable to the L2 cache in a CPU multi-level cache;
-   Storage location for data loading: B1, used to store the entire B cube, comparable to the L2 cache in a CPU multi-level cache;
-   Storage location for data loading: C1, used to store the entire cube multiplication Bias cube, comparable to the L2 cache in a CPU multi-level cache;
-   Storage location for data loading: A2, used to store the small A cube after blocking, comparable to the L1 cache in a CPU multi-level cache;
-   Storage location for data loading: B2, used to store the small B cube after blocking, comparable to the L1 cache in a CPU multi-level cache;
-   Storage location for data loading: C2, used to store the small cube multiplication Bias cube after blocking, comparable to the L1 cache in a CPU multi-level cache;
-   Storage location for result data: CO1, used to store the small result C cube, which can be understood as Cube Out;
-   Storage location for result data: CO2, used to store the entire result C cube, which can be understood as Cube Out;
-   Storage location for temporary variables: VECCALC, which is generally used when temporary variables are required for computation.

**Cube multiplication data flow** refers to the flow of cube multiplication inputs and outputs between storage locations. The data flow between logical positions is shown in the following figure (bias is omitted for simplicity):

-   The data flow of cube A from the input location to A2 is as follows (the input location can be GM or VECOUT): GM-\>A2, GM-\>A1-\>A2; VECOUT-\>A1-\>A2.

    Because A1 has more space than A2, data can be loaded from GM or VECOUT into A1 for caching first. Before the data is used for cube computation, it is directly loaded from A1 into A2. In this way, when a large amount of data is loaded, the waiting time before computation can be reduced and performance improved. Only in scenarios where a small amount of data is loaded may the GM-\>A2 data flow be used.

-   The data flow of cube B from the input location to B2 is as follows (the input location can be GM or VECOUT): GM-\>B2, GM-\>B1-\>B2; VECOUT-\>B1-\>B2.

    Because B1 has more space than B2, data can be loaded from GM or VECOUT into B1 for caching first. Before the data is used for cube computation, it is directly loaded from B1 into B2. In this way, when a large amount of data is loaded, the waiting time before computation can be reduced and performance improved. Only in scenarios where a small amount of data is loaded may the GM-\>B2 data flow be used.

-   Complete the A2\*B2=CO1 computation.
-   CO1 data is aggregated into CO2: CO1-\>CO2.
-   From CO2 to the output location (the output location can be GM or VECIN): CO2-\>GM/CO2-\>VECIN.

![](../../../figures/mat_pos_40.png)

## Data Format<a name="zh-cn_topic_0000001622194138_section1453415011"></a>

When performing Matmul cube multiplication, two fractal formats are mainly involved: ND and [NZ](../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md#li19960204116136). For other data formats, see [Data Layout Format](../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md).

-   ND: a common format, an N-dimensional tensor.
-   NZ: a special format introduced to meet the high-performance computing requirements of the cube computing unit in the AI Core.


    The transformation from ND –\> NZ is as follows:

    ```
    (..., N, H, W )->pad->(..., N, H1*H0, W1*W0)->reshape->(..., N, H1, H0, W1, W0)->transpose->(..., N, W1, H1, H0, W0)
    ```

    As shown in the following figure, a cube of size (W, H) is divided into (H1\*W1) fractals, which are arranged in column-major order, forming an N-like shape. Each fractal contains (H0\*W0) elements, which are arranged in row-major order, forming a Z-like shape. Therefore, this data format is called the NZ (large N, small Z) format.

    ![](../../../figures/nd2nz_trans_1.png)

    The following uses a specific example to further understand the difference in data layout between the ND and NZ formats. Assume that the fractal format is 2\*2. For the cube 4\*4 shown in the following figure, when stored in the ND (1, 4, 4) and NZ (1, 2, 2, 2, 2) formats, the data layouts in memory are as follows:

    ND: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15

    NZ: 0, 1, 4, 5, 8, 9, 12, 13, 2, 3, 6, 7, 10, 11, 14, 15

    ![](../../../figures/nd2nz_2.png)

## Data Tiling<a name="zh-cn_topic_0000001622194138_section68451031218"></a>

-   **Multi-core splitting**

    To implement multi-core parallelism, the cube data needs to be split and assigned to different cores for processing. The splitting strategy is shown in the following figure:

    -   For cube A, split it along the M axis into multiple singleCoreM blocks. Each core processes data of size SingleCoreM \* K.
    -   For cube B, split it along the N axis into multiple singleCoreN blocks. Each core processes data of size K \* SingleCoreN.
    -   For cube C, the cube A of size SingleCoreM \* K is multiplied by the cube B of size K \* SingleCoreN to obtain the cube C of size SingleCoreM \* SingleCoreN, which is the size of the cube C output on a single core.

    For example, in the following figure, eight cores participate in the computation. Cube A is divided into four blocks along the M axis, and cube B is split into two blocks along the N axis. Each core processes only one block (for example, the green part in the figure is the data involved in the computation on core3): the cube A block of size SingleCoreM \* K is multiplied by the cube B block of size SingleCoreN\* K to obtain the cube C block of size SingleCoreM \* SingleCoreN.

    ![](../../../figures/nd2nz_new.png)

    In addition, the length of the K axis processed on a single core is SingleCoreK. For scenarios with a large K axis, the K axis can be split into multiple singleCoreK segments. For detailed cases, see [Matmul Advanced API Multi-Core K Splitting](../../best_practices/matmul_tuning_cases/matmul_multi_core_split_k.md).

-   **Intra-core splitting**

    In most cases, the Local Memory storage cannot fully accommodate the input and output of an operator. It is necessary to load a portion of the input for computation, move the result out, and then load the next portion of the input for computation, until the complete final result is obtained. That is, intra-core input splitting is required. The splitting strategy is as follows:

    -   For cube A, split along the M axis to split singleCoreM into multiple baseM segments. The number of segments corresponds to **mIter** in the figure. Split along the K axis to split it into multiple baseK segments.
    -   For cube B, split along the N axis to split singleCoreN into multiple baseN segments. The number of segments corresponds to **nIter** in the figure. Split along the K axis to split it into multiple baseK segments.
    -   For cube C, the baseM\*baseK-sized block in cube A and the baseK\*baseN-sized block in cube B are multiplied and accumulated to obtain the baseM\*baseN-sized block at the corresponding position in cube C. For example, the green cube block 5 in the result cube in the figure is obtained through the following accumulation process: a\*a+b\*b+c\*c+d\*d+e\*e+f\*f.

    ![](../../../figures/nd2nz_43.png)

    In addition to the base block shapes baseM, baseN, and baseK, there are some commonly used tiling parameters, whose meanings are as follows:

    -   <a name="zh-cn_topic_0000001622194138_li123441825184114"></a>**iterateOrder**: One **Iterate** call computes a C cube tile of size \[baseM, baseN\]. After the **Iterate** is complete, Matmul automatically offsets the position of the C cube output by the next **Iterate**. **iterateOrder** indicates the order of the automatic offset.

        -   **0** indicates offsetting in the M axis direction first and then in the N axis direction.
        -   **1** indicates offsetting in the N axis direction first and then in the M axis direction.

        In the example in the preceding figure, the value of **iterateOrder** is **0**.

    -   **depthA1**, **depthB1**: the number of times the cube tiles stored in L1 Buffer (A1) and L1 Buffer (B1) are fully loaded into L0A Buffer (A2) and L0B Buffer (B2), respectively. The storage sizes of L0A Buffer (A2) and L0B Buffer (B2) are baseM \* baseK and baseN \* baseK, respectively. That is, depthA1 is the number of baseM \* baseK blocks contained in the A1 cube tile, and depthB1 is the number of baseN \* baseK blocks contained in the B1 cube tile.
    -   **stepM**, **stepN**: **stepM** is the multiple of baseM in the buffer M direction of the left cube cached in L1 Buffer (A1), and **stepN** is the multiple of baseN in the buffer N direction of the right cube cached in L1 Buffer (B1).
    -   **stepKa**, **stepKb**: **stepKa** is the multiple of baseK in the buffer K direction of the left cube cached in L1 Buffer (A1), and **stepKb** is the multiple of baseK in the buffer K direction of the right cube cached in L1 Buffer (B1).
