# Matmul Performance Optimization Strategy Overview<a name="ZH-CN_TOPIC_0000002360993885"></a>

<!-- md-trans-meta sourceCommit=7c6b4a9f1bae21994540d7d2fa2801ed0b239247 translatedAt=2026-11-09T10:26:23.895Z pushedAt=2026-11-19T06:32:19.711Z nl=2 -->

This section provides a series of performance tuning cases for operators that involve Matmul computation. Based on the actual application scenarios, you can refer to the optimization methods and ideas in the related cases and apply them to your practice. The cases are classified into the following five categories. For a brief introduction to each category, see the following table. For details, see the subsequent sections.

-   Tiling optimization

    **Table 1**  Tiling optimization strategy overview<a name="table11377439144919"></a>

    <table><thead align="left"><tr id="row7377639104916"><th class="cellrowborder" valign="top" width="30.086991300869915%" id="mcps1.2.4.1.1"><p id="p4377123917495"><a name="p4377123917495"></a><a name="p4377123917495"></a>Classification</p>
    </th>
    <th class="cellrowborder" valign="top" width="40.82591740825918%" id="mcps1.2.4.1.2"><p id="p2377193920493"><a name="p2377193920493"></a><a name="p2377193920493"></a>Applicable Scenario</p>
    </th>
    <th class="cellrowborder" valign="top" width="29.087091290870916%" id="mcps1.2.4.1.3"><p id="p1377173904916"><a name="p1377173904916"></a><a name="p1377173904916"></a>Related Case</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row1737712393496"><td class="cellrowborder" valign="top" width="30.086991300869915%" headers="mcps1.2.4.1.1 "><p id="p13781393494"><a name="p13781393494"></a><a name="p13781393494"></a>Tiling optimization: Optimize the tiling core allocation and base block splitting strategy.</p>
    </td>
    <td class="cellrowborder" valign="top" width="40.82591740825918%" headers="mcps1.2.4.1.2 "><p id="p337893924919"><a name="p337893924919"></a><a name="p337893924919"></a>Large shape scenarios with sufficient data volume.</p>
    </td>
    <td class="cellrowborder" valign="top" width="29.087091290870916%" headers="mcps1.2.4.1.3 "><p id="p123781939164916"><a name="p123781939164916"></a><a name="p123781939164916"></a><a href="matmul_tiling_strategy.md">Matmul operator tiling strategy optimization</a></p>
    </td>
    </tr>
    </tbody>
    </table>

-   Parallelism optimization

    **Table 2**  Parallelism optimization strategy overview<a name="table225712267501"></a>

    <table><thead align="left"><tr id="row18257182614507"><th class="cellrowborder" valign="top" width="30.086991300869915%" id="mcps1.2.4.1.1"><p id="p1925717267508"><a name="p1925717267508"></a><a name="p1925717267508"></a>Classification</p>
    </th>
    <th class="cellrowborder" valign="top" width="40.82591740825918%" id="mcps1.2.4.1.2"><p id="p182571626155016"><a name="p182571626155016"></a><a name="p182571626155016"></a>Applicable Scenario</p>
    </th>
    <th class="cellrowborder" valign="top" width="29.087091290870916%" id="mcps1.2.4.1.3"><p id="p325710262505"><a name="p325710262505"></a><a name="p325710262505"></a>Related Case</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row9258226145013"><td class="cellrowborder" valign="top" width="30.086991300869915%" headers="mcps1.2.4.1.1 "><p id="p1425862615018"><a name="p1425862615018"></a><a name="p1425862615018"></a>Task parallelism across multiple cores: reasonably distribute data to different cores to execute tasks.</p>
    </td>
    <td class="cellrowborder" valign="top" width="40.82591740825918%" headers="mcps1.2.4.1.2 "><p id="p5258326185015"><a name="p5258326185015"></a><a name="p5258326185015"></a>Scenarios where the K axis of the cube is large, and the M and N axes are smaller than the K axis.</p>
    </td>
    <td class="cellrowborder" valign="top" width="29.087091290870916%" headers="mcps1.2.4.1.3 "><p id="p1625832617508"><a name="p1625832617508"></a><a name="p1625832617508"></a><a href="matmul_multi_core_split_k.md">Enabling multi-core K-axis splitting for the Matmul advanced API</a></p>
    </td>
    </tr>
    <tr id="row1225815262506"><td class="cellrowborder" valign="top" width="30.086991300869915%" headers="mcps1.2.4.1.1 "><p id="p8258152625011"><a name="p8258152625011"></a><a name="p8258152625011"></a>Data access parallelism across multiple cores: optimize the multi-core parallel data access mechanism, such as optimizing address access conflicts for the same memory data in multi-core scenarios, to improve multi-core data access efficiency.</p>
    </td>
    <td class="cellrowborder" valign="top" width="40.82591740825918%" headers="mcps1.2.4.1.2 "><p id="p162587262503"><a name="p162587262503"></a><a name="p162587262503"></a>Scenarios where Matmul is executed across multiple cores, the K axis of the input cube is large, and the K axis is not fully loaded.</p>
    </td>
    <td class="cellrowborder" valign="top" width="29.087091290870916%" headers="mcps1.2.4.1.3 "><p id="p192580261505"><a name="p192580261505"></a><a name="p192580261505"></a><a href="matmul_multi_core_k_stagger.md">Enabling staggered multi-core K-axis memory access for the Matmul advanced API</a></p>
    </td>
    </tr>
    <tr id="row8258126165014"><td class="cellrowborder" valign="top" width="30.086991300869915%" headers="mcps1.2.4.1.1 "><p id="p8258192614509"><a name="p8258192614509"></a><a name="p8258192614509"></a>Pipeline parallelism within a single core: leverage the mutual independence and parallel execution characteristics of different instruction queues to optimize intra-core pipeline parallelism.</p>
    <p id="p1025832613508"><a name="p1025832613508"></a><a name="p1025832613508"></a></p>
    </td>
    <td class="cellrowborder" valign="top" width="40.82591740825918%" headers="mcps1.2.4.1.2 "><a name="ol68291208611"></a><a name="ol68291208611"></a><ol id="ol68291208611"><li>The MMAD pipeline and FIXPIPE pipeline of the operator are executed serially, and the synchronization wait time accounts for a high proportion of the total operator execution time.</li><li>MTE2 Bound, and the MTE2 pipeline and other pipelines are executed serially.</li></ol>
    </td>
    <td class="cellrowborder" valign="top" width="29.087091290870916%" headers="mcps1.2.4.1.3 "><a name="ol224351311615"></a><a name="ol224351311615"></a><ol id="ol224351311615"><li><a href="matmul_unit_flag.md">Enabling UnitFlag for the Matmul advanced API</a></li><li><a href="matmul_nbuffer33.md">Enabling the NBuffer33 template for the Matmul advanced API</a></li></ol>
    </td>
    </tr>
    </tbody>
    </table>

-   Memory optimization

    **Table 3** Memory optimization strategy overview<a name="table136011854115018"></a>

    <table><thead align="left"><tr id="row1060112547508"><th class="cellrowborder" valign="top" width="30.086991300869915%" id="mcps1.2.4.1.1"><p id="p860113549502"><a name="p860113549502"></a><a name="p860113549502"></a>Classification</p>
    </th>
    <th class="cellrowborder" valign="top" width="40.82591740825918%" id="mcps1.2.4.1.2"><p id="p460185475011"><a name="p460185475011"></a><a name="p460185475011"></a>Applicable Scenario</p>
    </th>
    <th class="cellrowborder" valign="top" width="29.087091290870916%" id="mcps1.2.4.1.3"><p id="p4601115413505"><a name="p4601115413505"></a><a name="p4601115413505"></a>Related Case</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row1760225445018"><td class="cellrowborder" valign="top" width="30.086991300869915%" headers="mcps1.2.4.1.1 "><p id="p1960215413501"><a name="p1960215413501"></a><a name="p1960215413501"></a>Memory sharing and reuse: Reduce the overhead caused by repeated data movement through buffer sharing and cache reuse.</p>
    </td>
    <td class="cellrowborder" valign="top" width="40.82591740825918%" headers="mcps1.2.4.1.2 "><p id="p196026548501"><a name="p196026548501"></a><a name="p196026548501"></a>In MIX scenarios, the GM addresses of the A or B cubes of multiple AIVs are the same, and the A or B cubes reused by multiple AIVs are fully loaded on the L1 buffer.</p>
    </td>
    <td class="cellrowborder" valign="top" width="29.087091290870916%" headers="mcps1.2.4.1.3 "><p id="p1160255465017"><a name="p1160255465017"></a><a name="p1160255465017"></a><a href="matmul_ibshare_ab.md">Enabling the IBShare template for the Matmul advanced API to share A and B cube data</a></p>
    <p id="p8602185495016"><a name="p8602185495016"></a><a name="p8602185495016"></a><a href="matmul_ibshare_b.md">Enabling the IBShare template for the Matmul advanced API to share B cube data</a></p>
    </td>
    </tr>
    <tr id="row19602135415012"><td class="cellrowborder" valign="top" width="30.086991300869915%" headers="mcps1.2.4.1.1 "><p id="p1460235415508"><a name="p1460235415508"></a><a name="p1460235415508"></a>Memory alignment: Ensure that the processed data meets specific alignment requirements, and use different movement strategies for non-aligned data to improve data movement efficiency.</p>
    </td>
    <td class="cellrowborder" valign="top" width="40.82591740825918%" headers="mcps1.2.4.1.2 "><p id="p4602554135017"><a name="p4602554135017"></a><a name="p4602554135017"></a>Scenarios where the inner axis of the input cube is not 256-byte aligned and the data volume is large.</p>
    </td>
    <td class="cellrowborder" valign="top" width="29.087091290870916%" headers="mcps1.2.4.1.3 "><p id="p16602754195019"><a name="p16602754195019"></a><a name="p16602754195019"></a><a href="aiv_nd2nz_conversion.md">ND2NZ format conversion on the AIV core</a></p>
    </td>
    </tr>
    </tbody>
    </table>

-   Scalar optimization

    **Table 4** Scalar optimization strategy overview<a name="table214614239517"></a>

    <table><thead align="left"><tr id="row514716230519"><th class="cellrowborder" valign="top" width="30.086991300869915%" id="mcps1.2.4.1.1"><p id="p16147112355116"><a name="p16147112355116"></a><a name="p16147112355116"></a>Classification</p>
    </th>
    <th class="cellrowborder" valign="top" width="40.82591740825918%" id="mcps1.2.4.1.2"><p id="p12147112318519"><a name="p12147112318519"></a><a name="p12147112318519"></a>Applicable Scenario</p>
    </th>
    <th class="cellrowborder" valign="top" width="29.087091290870916%" id="mcps1.2.4.1.3"><p id="p7147112365112"><a name="p7147112365112"></a><a name="p7147112365112"></a>Related Case</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row1414892319513"><td class="cellrowborder" valign="top" width="30.086991300869915%" headers="mcps1.2.4.1.1 "><p id="p5148162310512"><a name="p5148162310512"></a><a name="p5148162310512"></a>Tiling constantization: completes the Matmul tiling computation during kernel compilation, converting variables into constants that are propagated throughout the system, thereby reducing scalar operations and improving performance.</p>
    </td>
    <td class="cellrowborder" valign="top" width="40.82591740825918%" headers="mcps1.2.4.1.2 "><p id="p147291104560"><a name="p147291104560"></a><a name="p147291104560"></a>Matmul initialization involves many scalar computations, which affect the instruction header overhead.</p>
    <p id="p1563512345614"><a name="p1563512345614"></a><a name="p1563512345614"></a>Many scalar computations occur between Matmul iterations, blocking the MTE2 pipeline.</p>
    </td>
    <td class="cellrowborder" valign="top" width="29.087091290870916%" headers="mcps1.2.4.1.3 "><p id="p12148182365118"><a name="p12148182365118"></a><a name="p12148182365118"></a><a href="matmul_tiling_constant.md">Enabling full tiling constantization for the Matmul advanced API</a></p>
    </td>
    </tr>
    <tr id="row16148523155115"><td class="cellrowborder" valign="top" width="30.086991300869915%" headers="mcps1.2.4.1.1 "><p id="p514812319512"><a name="p514812319512"></a><a name="p514812319512"></a>Cube-only mode: reduces the additional scalar overhead introduced by the message processing mechanism.</p>
    </td>
    <td class="cellrowborder" valign="top" width="40.82591740825918%" headers="mcps1.2.4.1.2 "><p id="p1914812237516"><a name="p1914812237516"></a><a name="p1914812237516"></a>Compared with the MIX mode, this applies to scenarios with only cube computation and no vector computation.</p>
    </td>
    <td class="cellrowborder" valign="top" width="29.087091290870916%" headers="mcps1.2.4.1.3 "><p id="p17148723205118"><a name="p17148723205118"></a><a name="p17148723205118"></a><a href="matmul_pure_cube.md">Enabling cube-only mode for the Matmul advanced API</a></p>
    </td>
    </tr>
    </tbody>
    </table>

-   Data movement optimization

    **Table 5**  Data movement optimization strategy overview<a name="table2401471518"></a>

    <table><thead align="left"><tr id="row44094713510"><th class="cellrowborder" valign="top" width="30.086991300869915%" id="mcps1.2.4.1.1"><p id="p114013470517"><a name="p114013470517"></a><a name="p114013470517"></a>Classification</p>
    </th>
    <th class="cellrowborder" valign="top" width="40.82591740825918%" id="mcps1.2.4.1.2"><p id="p164015478513"><a name="p164015478513"></a><a name="p164015478513"></a>Applicable Scenario</p>
    </th>
    <th class="cellrowborder" valign="top" width="29.087091290870916%" id="mcps1.2.4.1.3"><p id="p44084775116"><a name="p44084775116"></a><a name="p44084775116"></a>Related Case</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row442164713513"><td class="cellrowborder" valign="top" width="30.086991300869915%" headers="mcps1.2.4.1.1 "><p id="p64224795112"><a name="p64224795112"></a><a name="p64224795112"></a>Data movement throughput optimization: improves bandwidth utilization and data movement efficiency by properly controlling the size of the data blocks to be moved.</p>
    </td>
    <td class="cellrowborder" valign="top" width="40.82591740825918%" headers="mcps1.2.4.1.2 "><a name="ol9152163610615"></a><a name="ol9152163610615"></a><ol id="ol9152163610615"><li>Large-shape scenarios where MTE2 performs many loop-based data movements.</li><li>Scenarios where the input and output data volume exceeds the L2 cache size.</li></ol>
    </td>
    <td class="cellrowborder" valign="top" width="29.087091290870916%" headers="mcps1.2.4.1.3 "><a name="ol0907154317615"></a><a name="ol0907154317615"></a><ol id="ol0907154317615"><li><a href="matmul_mdl_template.md">Enabling the MDL template for the Matmul advanced API</a></li><li><a href="matmul_l2_cache_split.md">Enabling L2 cache splitting for the Matmul advanced API</a></li></ol>
    </td>
    </tr>
    <tr id="row1542174715511"><td class="cellrowborder" valign="top" width="30.086991300869915%" headers="mcps1.2.4.1.1 "><p id="p10421347185119"><a name="p10421347185119"></a><a name="p10421347185119"></a>Prefetch data movement: prefetches the data blocks to be moved to reduce the gaps between pipelines.</p>
    </td>
    <td class="cellrowborder" valign="top" width="40.82591740825918%" headers="mcps1.2.4.1.2 "><p id="p14284716513"><a name="p14284716513"></a><a name="p14284716513"></a>Scenarios where the MTE2 pipeline gap is large and M or N is large.</p>
    </td>
    <td class="cellrowborder" valign="top" width="29.087091290870916%" headers="mcps1.2.4.1.3 "><p id="p042947135117"><a name="p042947135117"></a><a name="p042947135117"></a><a href="matmul_mte2_preload.md">Enabling MTE2 preload for the Matmul advanced API</a></p>
    </td>
    </tr>
    </tbody>
    </table>
