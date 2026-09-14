# Matmul Performance Optimization Strategy Overview<a name="ZH-CN_TOPIC_0000002360993885"></a>

<!-- md-trans-meta sourceCommit=198cbfdf29f72f78c8de735d88a5244dc258f2e7 translatedAt=2026-08-26T13:26:39.513Z pushedAt=2026-09-11T09:52:02.661Z -->

This section provides a series of performance tuning cases for operators that involve Matmul computation. Based on the actual application scenarios, you can refer to the optimization methods and ideas in the related cases and apply them to your practice. The cases are classified into the following five categories. For a brief introduction to each category, see the following table. For details, see the subsequent sections.

-   Tiling optimization

    **Table 1**  Tiling optimization strategy overview

    <a name="table11377439144919"></a>
    <table><thead align="left"><tr id="row7377639104916"><th class="cellrowborder" valign="top" width="30.086991300869915%" id="mcps1.2.4.1.1"><p id="p4377123917495"><a name="p4377123917495"></a><a name="p4377123917495"></a>Classification</p>
    </th>
    <th class="cellrowborder" valign="top" width="40.82591740825918%" id="mcps1.2.4.1.2"><p id="p2377193920493"><a name="p2377193920493"></a><a name="p2377193920493"></a>Applicable Scenarios</p>
    </th>
    <th class="cellrowborder" valign="top" width="29.087091290870916%" id="mcps1.2.4.1.3"><p id="p1377173904916"><a name="p1377173904916"></a><a name="p1377173904916"></a>Related Cases</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row1737712393496"><td class="cellrowborder" valign="top" width="30.086991300869915%" headers="mcps1.2.4.1.1 "><p id="p13781393494"><a name="p13781393494"></a><a name="p13781393494"></a>Tiling optimization: optimizes the Tiling core allocation and basic block splitting strategy.</p>
    </td>
    <td class="cellrowborder" valign="top" width="40.82591740825918%" headers="mcps1.2.4.1.2 "><p id="p337893924919"><a name="p337893924919"></a><a name="p337893924919"></a>Large Shape scenarios with sufficient data volume.</p>
    </td>
    <td class="cellrowborder" valign="top" width="29.087091290870916%" headers="mcps1.2.4.1.3 "><p id="p123781939164916"><a name="p123781939164916"></a><a name="p123781939164916"></a><a href="matmul_operator_tiling_strategy_optimization.md">Matmul Operator Tiling Optimization Strategy</a></p>
    </td>
    </tr>
    </tbody>
    </table>

-   Parallelism optimization

    **Table 2**  Parallelism optimization strategy overview

    <a name="table225712267501"></a>
    <table><thead align="left"><tr id="row18257182614507"><th class="cellrowborder" valign="top" width="30.086991300869915%" id="mcps1.2.4.1.1"><p id="p1925717267508"><a name="p1925717267508"></a><a name="p1925717267508"></a>Classification</p>
    </th>
    <th class="cellrowborder" valign="top" width="40.82591740825918%" id="mcps1.2.4.1.2"><p id="p182571626155016"><a name="p182571626155016"></a><a name="p182571626155016"></a>Applicable Scenarios</p>
    </th>
    <th class="cellrowborder" valign="top" width="29.087091290870916%" id="mcps1.2.4.1.3"><p id="p325710262505"><a name="p325710262505"></a><a name="p325710262505"></a>Related Cases</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row9258226145013"><td class="cellrowborder" valign="top" width="30.086991300869915%" headers="mcps1.2.4.1.1 "><p id="p1425862615018"><a name="p1425862615018"></a><a name="p1425862615018"></a>Multi-core task parallelism: reasonably distribute data to different cores to execute tasks.</p>
    </td>
    <td class="cellrowborder" valign="top" width="40.82591740825918%" headers="mcps1.2.4.1.2 "><p id="p5258326185015"><a name="p5258326185015"></a><a name="p5258326185015"></a>Scenarios where the K axis of the matrix is large, and the M and N axes are smaller than the K axis.</p>
    </td>
    <td class="cellrowborder" valign="top" width="29.087091290870916%" headers="mcps1.2.4.1.3 "><p id="p1625832617508"><a name="p1625832617508"></a><a name="p1625832617508"></a><a href="enable_multi_core_split_k_for_matmul_advanced_api.md">Matmul High-Level API Enables Multi-Core K-Axis Splitting</a></p>
    </td>
    </tr>
    <tr id="row1225815262506"><td class="cellrowborder" valign="top" width="30.086991300869915%" headers="mcps1.2.4.1.1 "><p id="p8258152625011"><a name="p8258152625011"></a><a name="p8258152625011"></a>Parallel data access across multiple cores: optimizes the parallel data access mechanism across multiple cores, for example, by optimizing address access conflicts for the same memory data in multi-core scenarios, to improve multi-core data access efficiency.</p>
    </td>
    <td class="cellrowborder" valign="top" width="40.82591740825918%" headers="mcps1.2.4.1.2 "><p id="p162587262503"><a name="p162587262503"></a><a name="p162587262503"></a>Scenarios where Matmul is executed across multiple cores, the K axis of the input matrix is large, and the K axis is not fully loaded.</p>
    </td>
    <td class="cellrowborder" valign="top" width="29.087091290870916%" headers="mcps1.2.4.1.3 "><p id="p192580261505"><a name="p192580261505"></a><a name="p192580261505"></a><a href="matmul_advanced_api_enable_multi_core_k_axis_staggered_memory_access.md">Matmul High-Level API Enables Staggered Multi-Core K-Axis Memory Access</a></p>
    </td>
    </tr>
    <tr id="row8258126165014"><td class="cellrowborder" valign="top" width="30.086991300869915%" headers="mcps1.2.4.1.1 "><p id="p8258192614509"><a name="p8258192614509"></a><a name="p8258192614509"></a>Intra-core pipeline parallelism: optimizes the intra-core pipeline parallelism by leveraging the mutual independence and parallel execution capability of different instruction queues.</p>
    <p id="p1025832613508"><a name="p1025832613508"></a><a name="p1025832613508"></a></p>
    </td>
    <td class="cellrowborder" valign="top" width="40.82591740825918%" headers="mcps1.2.4.1.2 "><a name="ol68291208611"></a><a name="ol68291208611"></a><ol id="ol68291208611"><li>The MMAD pipeline and FIXPIPE pipeline of the operator are executed serially, and the synchronization wait time accounts for a high proportion of the total operator execution time.</li><li>The operator is MTE2-bound, and the MTE2 pipeline is executed serially with other pipelines.</li></ol>
    </td>
    <td class="cellrowborder" valign="top" width="29.087091290870916%" headers="mcps1.2.4.1.3 "><a name="ol224351311615"></a><a name="ol224351311615"></a><ol id="ol224351311615"><li><a href="matmul_advanced_api_enable_unitflag.md">Matmul High-Level API Enables UnitFlag</a></li><li><a href="matmul_advanced_api_enable_nbuffer33_template.md">Matmul High-Level API Enables the NBuffer33 Template</a></li></ol>
    </td>
    </tr>
    </tbody>
    </table>

-   Memory optimization

    **Table 3**  Memory Optimization Strategy Overview

    <a name="table136011854115018"></a>
    <table><thead align="left"><tr id="row1060112547508"><th class="cellrowborder" valign="top" width="30.086991300869915%" id="mcps1.2.4.1.1"><p id="p860113549502"><a name="p860113549502"></a><a name="p860113549502"></a>Classification</p>
    </th>
    <th class="cellrowborder" valign="top" width="40.82591740825918%" id="mcps1.2.4.1.2"><p id="p460185475011"><a name="p460185475011"></a><a name="p460185475011"></a>Applicable Scenarios</p>
    </th>
    <th class="cellrowborder" valign="top" width="29.087091290870916%" id="mcps1.2.4.1.3"><p id="p4601115413505"><a name="p4601115413505"></a><a name="p4601115413505"></a>Related Cases</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row1760225445018"><td class="cellrowborder" valign="top" width="30.086991300869915%" headers="mcps1.2.4.1.1 "><p id="p1960215413501"><a name="p1960215413501"></a><a name="p1960215413501"></a>Memory sharing and reuse: Reduce the overhead caused by repeated data movement through Buffer sharing and cache reuse.</p>
    </td>
    <td class="cellrowborder" valign="top" width="40.82591740825918%" headers="mcps1.2.4.1.2 "><p id="p196026548501"><a name="p196026548501"></a><a name="p196026548501"></a>In MIX scenarios, the GM addresses of the A matrices or B matrices of multiple AIVs are the same, and the A matrices or B matrices reused by multiple AIVs are fully loaded on the L1 Buffer.</p>
    </td>
    <td class="cellrowborder" valign="top" width="29.087091290870916%" headers="mcps1.2.4.1.3 "><p id="p1160255465017"><a name="p1160255465017"></a><a name="p1160255465017"></a><a href="matmul_advanced_api_enable_ibshare_template_to_share_a_and_b_matrix_data.md">Matmul High-Level API Enables IBShare Template to Share A and B Matrix Data</a></p>
    <p id="p8602185495016"><a name="p8602185495016"></a><a name="p8602185495016"></a><a href="matmul_advanced_api_enable_ibshare_template_to_share_b_matrix_data.md">Matmul High-Level API Enables IBShare Template to Share B Matrix Data</a></p>
    </td>
    </tr>
    <tr id="row19602135415012"><td class="cellrowborder" valign="top" width="30.086991300869915%" headers="mcps1.2.4.1.1 "><p id="p1460235415508"><a name="p1460235415508"></a><a name="p1460235415508"></a>Memory alignment: Ensure that the processed data meets specific alignment requirements, and use different movement strategies for non-aligned data to improve data movement efficiency.</p>
    </td>
    <td class="cellrowborder" valign="top" width="40.82591740825918%" headers="mcps1.2.4.1.2 "><p id="p4602554135017"><a name="p4602554135017"></a><a name="p4602554135017"></a>Scenarios where the inner axis of the input matrix is not 256-byte aligned and the data volume is large.</p>
    </td>
    <td class="cellrowborder" valign="top" width="29.087091290870916%" headers="mcps1.2.4.1.3 "><p id="p16602754195019"><a name="p16602754195019"></a><a name="p16602754195019"></a><a href="nd2nz_format_conversion_on_aiv_core.md">ND2NZ Format Conversion on the AIV Core</a></p>
    </td>
    </tr>
    </tbody>
    </table>

-   Scalar optimization

    **Table 4**  Scalar Optimization Strategy Overview

    <a name="table214614239517"></a>
    <table><thead align="left"><tr id="row514716230519"><th class="cellrowborder" valign="top" width="30.086991300869915%" id="mcps1.2.4.1.1"><p id="p16147112355116"><a name="p16147112355116"></a><a name="p16147112355116"></a>Classification</p>
    </th>
    <th class="cellrowborder" valign="top" width="40.82591740825918%" id="mcps1.2.4.1.2"><p id="p12147112318519"><a name="p12147112318519"></a><a name="p12147112318519"></a>Applicable Scenarios</p>
    </th>
    <th class="cellrowborder" valign="top" width="29.087091290870916%" id="mcps1.2.4.1.3"><p id="p7147112365112"><a name="p7147112365112"></a><a name="p7147112365112"></a>Related Cases</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row1414892319513"><td class="cellrowborder" valign="top" width="30.086991300869915%" headers="mcps1.2.4.1.1 "><p id="p5148162310512"><a name="p5148162310512"></a><a name="p5148162310512"></a>Tiling constantization: completes the Matmul Tiling computation during Kernel compilation, converts variables into constants that propagate through the system, and reduces Scalar to improve performance.</p>
    </td>
    <td class="cellrowborder" valign="top" width="40.82591740825918%" headers="mcps1.2.4.1.2 "><p id="p147291104560"><a name="p147291104560"></a><a name="p147291104560"></a>There are many Scalar computations during Matmul initialization, which affect the instruction header overhead.</p>
    <p id="p1563512345614"><a name="p1563512345614"></a><a name="p1563512345614"></a>There are many Scalar computations between Matmul iterations, which block the MTE2 pipeline.</p>
    </td>
    <td class="cellrowborder" valign="top" width="29.087091290870916%" headers="mcps1.2.4.1.3 "><p id="p12148182365118"><a name="p12148182365118"></a><a name="p12148182365118"></a><a href="matmul_high_level_api_enabling_tiling_full_constant.md">Enabling Full Tiling Constantization Through the Matmul High-Level API</a></p>
    </td>
    </tr>
    <tr id="row16148523155115"><td class="cellrowborder" valign="top" width="30.086991300869915%" headers="mcps1.2.4.1.1 "><p id="p514812319512"><a name="p514812319512"></a><a name="p514812319512"></a>Pure Cube mode: reduces the additional Scalar overhead introduced by the message processing mechanism.</p>
    </td>
    <td class="cellrowborder" valign="top" width="40.82591740825918%" headers="mcps1.2.4.1.2 "><p id="p1914812237516"><a name="p1914812237516"></a><a name="p1914812237516"></a>Compared with the MIX mode, this mode has no vector computation and only matrix computation.</p>
    </td>
    <td class="cellrowborder" valign="top" width="29.087091290870916%" headers="mcps1.2.4.1.3 "><p id="p17148723205118"><a name="p17148723205118"></a><a name="p17148723205118"></a><a href="matmul_advanced_api_enable_cube_only_mode.md">Enabling Pure Cube Mode Through the Matmul High-Level API</a></p>
    </td>
    </tr>
    </tbody>
    </table>

-   Data movement optimization

    **Table 5**  Data movement optimization strategy overview

    <a name="table2401471518"></a>
    <table><thead align="left"><tr id="row44094713510"><th class="cellrowborder" valign="top" width="30.086991300869915%" id="mcps1.2.4.1.1"><p id="p114013470517"><a name="p114013470517"></a><a name="p114013470517"></a>Classification</p>
    </th>
    <th class="cellrowborder" valign="top" width="40.82591740825918%" id="mcps1.2.4.1.2"><p id="p164015478513"><a name="p164015478513"></a><a name="p164015478513"></a>Applicable Scenarios</p>
    </th>
    <th class="cellrowborder" valign="top" width="29.087091290870916%" id="mcps1.2.4.1.3"><p id="p44084775116"><a name="p44084775116"></a><a name="p44084775116"></a>Related Cases</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row442164713513"><td class="cellrowborder" valign="top" width="30.086991300869915%" headers="mcps1.2.4.1.1 "><p id="p64224795112"><a name="p64224795112"></a><a name="p64224795112"></a>Data movement throughput optimization: improves bandwidth utilization and data movement efficiency by properly controlling the size of data blocks to be moved.</p>
    </td>
    <td class="cellrowborder" valign="top" width="40.82591740825918%" headers="mcps1.2.4.1.2 "><a name="ol9152163610615"></a><a name="ol9152163610615"></a><ol id="ol9152163610615"><li>Large-shape scenarios where MTE2 performs many loop-based data movements.</li><li>Scenarios where the input and output data volume exceeds the L2 cache size.</li></ol>
    </td>
    <td class="cellrowborder" valign="top" width="29.087091290870916%" headers="mcps1.2.4.1.3 "><a name="ol0907154317615"></a><a name="ol0907154317615"></a><ol id="ol0907154317615"><li><a href="matmul_advanced_api_enable_mdl_template.md">Enabling the MDL template through the Matmul high-level API</a></li><li><a href="matmul_advanced_api_enable_l2_cache_tiling.md">Enabling L2 cache splitting through the Matmul high-level API</a></li></ol>
    </td>
    </tr>
    <tr id="row1542174715511"><td class="cellrowborder" valign="top" width="30.086991300869915%" headers="mcps1.2.4.1.1 "><p id="p10421347185119"><a name="p10421347185119"></a><a name="p10421347185119"></a>Prefetch data movement: prefetches the data blocks to be moved to reduce the gaps between pipelines.</p>
    </td>
    <td class="cellrowborder" valign="top" width="40.82591740825918%" headers="mcps1.2.4.1.2 "><p id="p14284716513"><a name="p14284716513"></a><a name="p14284716513"></a>Scenarios where the MTE2 pipeline has large gaps and the M or N value is large.</p>
    </td>
    <td class="cellrowborder" valign="top" width="29.087091290870916%" headers="mcps1.2.4.1.3 "><p id="p042947135117"><a name="p042947135117"></a><a name="p042947135117"></a><a href="matmul_advanced_api_enable_mte2_preload.md">Matmul high-level API enables MTE2 Preload</a></p>
    </td>
    </tr>
    </tbody>
    </table>
