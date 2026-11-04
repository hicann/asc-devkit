# 4:2 Sparse Cube Multiplication<a name="ZH-CN_TOPIC_0000002532388135"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T11:58:09.936Z pushedAt=2026-11-03T07:05:13.295Z nl=2 -->

## Feature Overview<a name="zh-cn_topic_0000002298767897_section310824820358"></a>

4:2 sparse cube multiplication is also called Sparse Matmul. In this scenario, the input original left cube A and right cube B are sparse cubes. In sparse cube B, at least two of every four elements are zero elements. Before performing the Matmul computation, you need to perform 4:2 densification on cube B, that is, filter out two zero elements from every four elements in the original sparse cube B, so that cube B is densified into a dense cube. In the Sparse Matmul scenario, the Matmul API is called to complete the cube multiplication of cube A and the densified cube B. Sparse Matmul can skip the zero elements in sparse cube B and only move, store, and compute the non-zero elements, thereby reducing the memory usage and computation workload during cube multiplication and improving performance.

## Implementation Process<a name="zh-cn_topic_0000002298767897_section6579933163517"></a>

1.  Preprocess data.

    In the data preparation phase before computation, you need to densify cube B, which is originally a sparse cube. For details about the densification process, see [Densification Algorithm Description](../../../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadWithSparse.md). After densification, the 4:2 densified right cube B and the index cube are obtained. The densified right cube B and the index cube are used as the computation inputs of the Sparse Matmul scenario.

    **Figure 1**  Schematic diagram of the 4:2 densification process on the original sparse cube B<a name="zh-cn_topic_0000002298767897_fig107701022221"></a>  
    ![](../../../../figures/sparse_dense.png "Schematic diagram of the 4-2 densification process on the original sparse cube B")

    During densification, for every four elements of sparse cube B, two 2-bit indices are generated in the index cube, and each index points to the relative position of the corresponding non-zero element. For details about the rules, see [Densification Algorithm Description](../../../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadWithSparse.md). The data type of the index cube generated during densification is int2. Before the index cube is loaded into Matmul, it must be assembled into the int8 data type. The index cube is arranged in reverse order in an int8 address. For example, the index cube 1 2 0 1 0 2 1 0 is arranged in the address as 1 0 2 1 0 1 2 0, where 1 0 2 1 (corresponding to the first four bits 1 2 0 1 of the index cube) forms one int8, and 0 1 2 0 (corresponding to the last four bits 0 2 1 0 of the index cube) forms another int8.

2.  Set the sparse Matmul scenario.

    On the host side, before [obtaining Tiling](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/GetTiling.md), you need to set the sparse Matmul scenario via the [SetSparse](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetSparse.md) API.

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    matmul_tiling::MatmulApiTiling tiling(ascendcPlatform); 
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_INT8); 
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_INT8);  
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_INT32);  
    // Set the Sparse Matmul scenario
    tiling.SetSparse(true);
    ... // Other implementation content
    optiling::TCubeTiling tilingData;   
    int ret = tiling.GetTiling(tilingData);
    ```

3.  Create a Matmul object.

    When creating a Matmul object on the kernel side, use [MatmulType](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Matmul_usage.md) to define the parameter type information of A, C, and Bias, including the memory logic position, data format, and data type. Use the [SparseMatmulType](#zh-cn_topic_0000002298767897_table3658657131020) type to define the parameter type of cube B, including the memory logic position of cube B, the memory logic position of the index cube, data format, data type, and so on.

    ```
    #include "lib/matmul_intf.h"
    
    using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, ATYPE, false>;
    // Use SparseMatmulType to define the parameter type information of cube B
    using B_TYPE = AscendC::SparseMatmulType<AscendC::TPosition::GM, AscendC::TPosition::GM, CubeFormat::ND, BType, true>;
    using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
    using BIAS_TYPE =  AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
    AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_MDL> mm;
    ```

4.  <a name="zh-cn_topic_0000002298767897_li52321325141410"></a>Set the index cube.

    Pass the index cube generated during densification via the [SetSparseIndex](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetSparseIndex.md) API.

    ```
    mm.SetTensorA(gm_a);    // Set the left cube A
    mm.SetTensorB(gm_b);    // Set the right cube B
    mm.SetSparseIndex(gm_index); // Pass the index cube generated during densification
    mm.SetBias(gm_bias);    // Set the bias
    ```

5.  Complete the cube multiplication.

    On the kernel side, complete the cube multiplication based on the index cube loaded in [Step 4](#zh-cn_topic_0000002298767897_li52321325141410). The Matmul API internally densifies cube A, that is, it selects 2 elements at the corresponding positions from every 4 elements of cube A based on the index cube to participate in the computation.

    ```
    // Call the Iterate and GetTensorC or IterateAll APIs to complete the cube multiplication computation.
    while (mm.Iterate()) {   
        mm.GetTensorC(gm_c); 
    }
    // mm.IterateAll(gm_c);
    mm.End();
    ```

## Parameters<a name="zh-cn_topic_0000002298767897_section139621338103518"></a>

**Table 1**  SparseMatmulType parameters

<a name="zh-cn_topic_0000002298767897_table3658657131020"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002298767897_row8658257191010"><th class="cellrowborder" valign="top" width="21.21%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002298767897_p191082218111"><a name="zh-cn_topic_0000002298767897_p191082218111"></a><a name="zh-cn_topic_0000002298767897_p191082218111"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="78.79%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002298767897_p1554812516113"><a name="zh-cn_topic_0000002298767897_p1554812516113"></a><a name="zh-cn_topic_0000002298767897_p1554812516113"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002298767897_row9658257131018"><td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298767897_p1510818219111"><a name="zh-cn_topic_0000002298767897_p1510818219111"></a><a name="zh-cn_topic_0000002298767897_p1510818219111"></a>POSITION</p></td>
<td class="cellrowborder" valign="top" width="78.79%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298767897_p1054865111113"><a name="zh-cn_topic_0000002298767897_p1054865111113"></a><a name="zh-cn_topic_0000002298767897_p1054865111113"></a>Memory logic position.</p>
<p id="zh-cn_topic_0000002298767897_p115481458112"><a name="zh-cn_topic_0000002298767897_p115481458112"></a><a name="zh-cn_topic_0000002298767897_p115481458112"></a>The cube B can only be set to TPosition::GM.</p></td>
</tr>
<tr id="zh-cn_topic_0000002298767897_row1465855711017"><td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298767897_p161083211112"><a name="zh-cn_topic_0000002298767897_p161083211112"></a><a name="zh-cn_topic_0000002298767897_p161083211112"></a>INDEX_POSITION</p></td>
<td class="cellrowborder" valign="top" width="78.79%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298767897_p1454811531116"><a name="zh-cn_topic_0000002298767897_p1454811531116"></a><a name="zh-cn_topic_0000002298767897_p1454811531116"></a>Memory logic position of the index cube.</p>
<p id="zh-cn_topic_0000002298767897_p75481751111"><a name="zh-cn_topic_0000002298767897_p75481751111"></a><a name="zh-cn_topic_0000002298767897_p75481751111"></a>Only TPosition::GM is supported.</p></td>
</tr>
<tr id="zh-cn_topic_0000002298767897_row96581574100"><td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298767897_p91081627115"><a name="zh-cn_topic_0000002298767897_p91081627115"></a><a name="zh-cn_topic_0000002298767897_p91081627115"></a>CubeFormat</p></td>
<td class="cellrowborder" valign="top" width="78.79%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298767897_p1154817581115"><a name="zh-cn_topic_0000002298767897_p1154817581115"></a><a name="zh-cn_topic_0000002298767897_p1154817581115"></a>Physical layout format of the data. For details, see <a href="../basics.md#zh-cn_topic_0000001622194138_section1453415011">Data Format</a>.</p>
<p id="zh-cn_topic_0000002298767897_p17548195181111"><a name="zh-cn_topic_0000002298767897_p17548195181111"></a><a name="zh-cn_topic_0000002298767897_p17548195181111"></a>Cube B supports CubeFormat::ND and CubeFormat::NZ.</p></td>
</tr>
<tr id="zh-cn_topic_0000002298767897_row2658125791017"><td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298767897_p18108622115"><a name="zh-cn_topic_0000002298767897_p18108622115"></a><a name="zh-cn_topic_0000002298767897_p18108622115"></a>TYPE</p></td>
<td class="cellrowborder" valign="top" width="78.79%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298767897_p1054818541113"><a name="zh-cn_topic_0000002298767897_p1054818541113"></a><a name="zh-cn_topic_0000002298767897_p1054818541113"></a>Cube B supports only the int8_t data type.</p></td>
</tr>
<tr id="zh-cn_topic_0000002298767897_row6658125715106"><td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298767897_p1910817211117"><a name="zh-cn_topic_0000002298767897_p1910817211117"></a><a name="zh-cn_topic_0000002298767897_p1910817211117"></a>ISTRANS</p></td>
<td class="cellrowborder" valign="top" width="78.79%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298767897_p12549359114"><a name="zh-cn_topic_0000002298767897_p12549359114"></a><a name="zh-cn_topic_0000002298767897_p12549359114"></a>Whether to enable cube transposition.</p>
<p id="zh-cn_topic_0000002298767897_p854917510119"><a name="zh-cn_topic_0000002298767897_p854917510119"></a><a name="zh-cn_topic_0000002298767897_p854917510119"></a>Currently, only the value true is supported, which enables cube transposition.</p></td>
</tr>
<tr id="zh-cn_topic_0000002298767897_row8659057201019"><td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298767897_p201082216112"><a name="zh-cn_topic_0000002298767897_p201082216112"></a><a name="zh-cn_topic_0000002298767897_p201082216112"></a>LAYOUT</p></td>
<td class="cellrowborder" valign="top" width="78.79%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298767897_p195491654114"><a name="zh-cn_topic_0000002298767897_p195491654114"></a><a name="zh-cn_topic_0000002298767897_p195491654114"></a>Indicates the data layout. In the Sparse Matmul scenario, only LAYOUT::NONE is supported.</p>
<p id="zh-cn_topic_0000002298767897_p95491154113"><a name="zh-cn_topic_0000002298767897_p95491154113"></a><a name="zh-cn_topic_0000002298767897_p95491154113"></a>NONE: default value, indicating that BatchMatmul is not used.</p></td>
</tr>
<tr id="zh-cn_topic_0000002298767897_row1965945716102"><td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298767897_p1210818291112"><a name="zh-cn_topic_0000002298767897_p1210818291112"></a><a name="zh-cn_topic_0000002298767897_p1210818291112"></a>IBSHARE</p></td>
<td class="cellrowborder" valign="top" width="78.79%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298767897_p1892915173233"><a name="zh-cn_topic_0000002298767897_p1892915173233"></a><a name="zh-cn_topic_0000002298767897_p1892915173233"></a>Whether to enable IBShare (IntraBlock Share). IBShare reuses the same cube A or cube B data in the L1 buffer. When IBShare is enabled for both cube A and cube B, the cube A and cube B data in the L1 buffer are reused simultaneously.</p>
<p id="zh-cn_topic_0000002298767897_p1654965121115"><a name="zh-cn_topic_0000002298767897_p1654965121115"></a><a name="zh-cn_topic_0000002298767897_p1654965121115"></a>In the Sparse Matmul scenario, currently only the value false is supported, which disables IBShare.</p></td>
</tr>
</tbody>
</table>

## Scenarios<a name="zh-cn_topic_0000002298767897_section118051016163613"></a>

Matmul computation scenario where the left cube A is a sparse cube and the right cube B is a cube after 4:2 densification.

## Constraints<a name="zh-cn_topic_0000002298767897_section11339101810594"></a>

-   This scenario supports only the cube-only mode (cube computation only) under the [MDL template](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulConfig.md).
-   The index cube passed via the **SetSparseIndex** API supports only the int8 data type and the [NZ data layout](../../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md#li19960204116136).
-   In the original sparse cube B, at most two of every four elements should be non-zero (that is, at least two zero elements). If three or more non-zero elements exist, only the first two non-zero elements are used.
-   None of M, K, and N can be 0.

## Example<a name="zh-cn_topic_0000002298767897_section15486294368"></a>

For a complete sample of the sparse Matmul scenario, see [Operator Sample for the Sparse Matmul Scenario](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_sparse).
