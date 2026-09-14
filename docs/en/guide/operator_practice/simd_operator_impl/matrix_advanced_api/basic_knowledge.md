# Basic Knowledge<a name="ZH-CN_TOPIC_0000002532228163"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T13:30:06.988Z -->

>[!NOTE]
>This section provides programming guidance for matrix multiplication using high-order APIs. When using high-order APIs for actual matrix programming, you need to check the supported product models through [API Reference](../../../../api/SIMD-API/advanced_api/matrix_compute/matrix_compute_84.md).

## Matrix Multiplication Overview<a name="zh-cn_topic_0000001622194138_section833318865814"></a>

The calculation formula of Matmul is: C = A \* B + bias. The following figure shows its schematic diagram.

-   A and B are source operands. A is the left matrix with the shape of \[M, K\]; B is the right matrix with the shape of \[K, N\].
-   C is the destination operand, which is the matrix storing the matrix multiplication result and has the shape of \[M, N\].
-   bias is the matrix multiplication bias with the shape of \[1, N\]. This bias is applied to each row of the A\*B result matrix.

**Figure 1**  Matmul matrix multiplication schematic diagram<a name="zh-cn_topic_0000001622194138_fig3161943163113"></a>  
![](../../../figures/matmul_matrix_multiplication_diagram.png "Matmul matrix multiplication schematic diagram")

## Matrix Multiplication Data Stream<a name="zh-cn_topic_0000001622194138_section295172162917"></a>

Before learning about the matrix multiplication data stream, review the following important storage **logical positions**:

-   Storage location of loaded data: A1, which stores the entire A matrix and can be compared to the L2 cache in a CPU multi-level cache.
-   Storage location of loaded data: B1, which stores the entire B matrix and can be compared to the L2 cache in a CPU multi-level cache.
-   Storage location of loaded data: C1, which stores the entire Bias matrix of the matrix multiplication and can be compared to the L2 cache in a CPU multi-level cache.
-   Storage location of loaded data: A2, which stores the split small A matrix and can be compared to the L1 cache in a CPU multi-level cache.
-   Storage location of loaded data: B2, which stores the split small B matrix and can be compared to the L1 cache in a CPU multi-level cache.
-   Storage location of loaded data: C2, which stores the split small Bias matrix of the matrix multiplication and can be compared to the L1 cache in a CPU multi-level cache.
-   Storage location of result data: CO1, which stores the small result C matrix and can be understood as Cube Out.
-   Storage location of result data: CO2, which stores the entire result C matrix and can be understood as Cube Out.
-   Storage location for loaded data: VECCALC, which is generally used when temporary variables are required for computation.

**Matrix multiplication data stream** refers to the flow of matrix multiplication inputs and outputs among storage locations. The data stream of logical positions is shown in the following figure (bias is omitted for simplicity):

-   The data stream of matrix A from the input position to A2 is as follows (the input position can be GM or VECOUT): GM-\>A2, GM-\>A1-\>A2; VECOUT-\>A1-\>A2.

    Since A1 has more space than A2, data from GM or VECOUT can be loaded into A1 for caching first. Before the data is used for Cube computation, it is directly loaded from A1 to A2. This reduces the waiting time before computation when a large amount of data is loaded, improving performance. The GM-\>A2 data stream is used only in scenarios where a small amount of data is loaded.

-   The data stream of matrix B from the input position to B2 is as follows (the input position can be GM or VECOUT): GM-\>B2, GM-\>B1-\>B2; VECOUT-\>B1-\>B2.

    Since B1 has more space than B2, data from GM or VECOUT can be loaded into B1 for caching first. Before the data is used for Cube computation, it is directly loaded from B1 to B2. This reduces the waiting time before computation when a large amount of data is loaded, improving performance. The GM-\>B2 data stream is used only in scenarios where a small amount of data is loaded.

-   Complete the A2\*B2=CO1 computation.
-   Aggregate CO1 data to CO2: CO1-\>CO2.
-   Move data from CO2 to the output position (the output position can be GM or VECIN): CO2-\>GM/CO2-\>VECIN.

![](../../../figures/matrix_programming_logical_position_diagram_40.png)

## Data Format<a name="zh-cn_topic_0000001622194138_section1453415011"></a>

When performing Matmul matrix multiplication, two fractal formats are mainly involved: ND and [NZ](../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout_format.md#li19960204116136). For other data formats, see [Data Layout](../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout_format.md).

-   ND: a common format, an N-dimensional tensor.
-   NZ: a special format introduced to meet the high-performance computing requirements of the Cube computing unit in the AI Core.


    The transformation from ND to NZ is as follows:

    ```
    (..., N, H, W )->pad->(..., N, H1*H0, W1*W0)->reshape->(..., N, H1, H0, W1, W0)->transpose->(..., N, W1, H1, H0, W0)
    ```

    As shown in the following figure, a matrix of size (W, H) is divided into (H1\*W1) fractals, which are arranged in column-major order, forming an N shape. Each fractal contains (H0\*W0) elements, which are arranged in row-major order, forming a Z shape. Therefore, this data format is called the NZ (large N, small Z) format.

    ![](../../../figures/nd2nz_tranformer_1.png)

    The following uses a specific example to further illustrate the difference in data layout between the ND and NZ formats. Assume that the fractal format is 2\*2. For the 4\*4 matrix shown in the following figure, when stored in the ND (1, 4, 4) and NZ (1, 2, 2, 2, 2) formats, the data layouts in memory are as follows:

    ND: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15

    NZ: 0, 1, 4, 5, 8, 9, 12, 13, 2, 3, 6, 7, 10, 11, 14, 15

    ![](../../../figures/nd2nz_2.png)

## Data Tiling<a name="zh-cn_topic_0000001622194138_section68451031218"></a>

-   **Multi-core tiling**

    To implement multi-core parallelism, the matrix data needs to be split and allocated to different cores for processing. The splitting strategy is shown in the following figure:

    -   For matrix A, split it along the M axis into multiple singleCoreM blocks. Each core processes data of the SingleCoreM \* K size.
    -   For matrix B, split it along the N axis into multiple singleCoreN blocks. Each core processes data of the K \* SingleCoreN size.
    -   For matrix C, the A matrix of the SingleCoreM \* K size is multiplied by the B matrix of the K \* SingleCoreN size to obtain the C matrix of the SingleCoreM \* SingleCoreN size, which is the size of the C matrix output on a single core.

    For example, in the following figure, eight cores participate in the computation. Matrix A is split into four blocks along the M axis, and matrix B is split into two blocks along the N axis. Each core processes only one block (for example, the green part in the figure is the data involved in the computation on core3): the A matrix block of the SingleCoreM \* K size is multiplied by the B matrix block of the SingleCoreN \* K size to obtain the C matrix block of the SingleCoreM \* SingleCoreN size.

    ![](../../../figures/nd2nz_conversion_diagram_new.png)

    In addition, the length of the K axis processed on a single core is SingleCoreK. For scenarios with a large K axis, the data can be split along the K axis into multiple singleCoreK blocks. For detailed examples, see [Enabling Multi-core K Splitting for the Matmul High-Order API](../../best_practices/matmul_tuning_cases/enable_multi_core_split_k_for_matmul_advanced_api.md).

-   **Intra-core tiling**

    In most cases, the Local Memory storage cannot fully accommodate the input and output of an operator. Therefore, a portion of the input needs to be loaded for computation and then moved out, followed by loading the next portion of the input for computation, until the complete final result is obtained. That is, input splitting within the core is required. The splitting strategy is as follows:

    -   For matrix A, split along the M axis to divide singleCoreM into multiple baseM blocks. The number of blocks corresponds to mIter in the figure. Split along the K axis into multiple baseK blocks.
    -   For matrix B, split along the N axis to divide singleCoreN into multiple baseN blocks. The number of blocks corresponds to nIter in the figure. Split along the K axis into multiple baseK blocks.
    -   For matrix C, the baseM\*baseK block in matrix A and the baseK\*baseN block in matrix B are multiplied and accumulated to obtain the baseM\*baseN block at the corresponding position in matrix C. For example, the green matrix block 5 in the result matrix in the figure is obtained through the following accumulation process: a\*a+b\*b+c\*c+d\*d+e\*e+f\*f.

    ![](../../../figures/nd2nz_conversion_diagram_43.png)

    In addition to the basic block shapes baseM, baseN, and baseK, there are some commonly used tiling parameters, whose meanings are as follows:

    -   iterateOrder: One Iterate computes a C matrix slice of size \[baseM, baseN\]. After the Iterate is complete, Matmul automatically offsets the position of the C matrix output by the next Iterate. iterateOrder indicates the order of the automatic offset.

        -   0 indicates offsetting along the M axis first and then along the N axis.
        -   1 indicates offsetting along the N axis first and then along the M axis.

        In the example in the preceding figure, iterateOrder is set to 0.

    -   depthA1, depthB1: number of times the matrix slices stored on A1 and B1 are fully loaded into A2 and B2. The storage sizes of A2 and B2 are baseM \* baseK and baseN \* baseK, respectively. That is, depthA1 is the number of baseM \* baseK blocks contained in the A1 matrix slice, and depthB1 is the number of baseN \* baseK blocks contained in the B1 matrix slice.
    -   stepM, stepN: stepM is the multiple of baseM in the buffer M direction of the left matrix cached in A1, and stepN is the multiple of baseN in the buffer N direction of the right matrix cached in B1.
    -   stepKa, stepKb: stepKa is the multiple of baseK in the buffer K direction of the left matrix cached in A1, and stepKb is the multiple of baseK in the buffer K direction of the right matrix cached in B1.
