# NPU Architecture Version 2201<a name="ZH-CN_TOPIC_0000002477825678"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T14:25:31.829Z pushedAt=2026-11-03T07:05:13.428Z nl=2 -->

This section describes the hardware constraints and recommended solutions. The corresponding product models are:

<!-- npu="A3" id1 -->
-   Atlas A3 products
<!-- end id1 -->
<!-- npu="910b" id2 -->
-   Atlas A2 products
<!-- end id2 -->

**Table 1** Hardware constraints and recommended solutions

<a name="table5984016192019"></a>
<table><thead align="left"><tr id="row49841016132015"><th class="cellrowborder" valign="top" width="13.608639136086392%" id="mcps1.2.4.1.1"><p id="p1498411692018"><a name="p1498411692018"></a><a name="p1498411692018"></a>Category</p></th>
<th class="cellrowborder" valign="top" width="33.406659334066596%" id="mcps1.2.4.1.2"><p id="p16984816162019"><a name="p16984816162019"></a><a name="p16984816162019"></a>Hardware Constraint Description</p></th>
<th class="cellrowborder" valign="top" width="52.98470152984702%" id="mcps1.2.4.1.3"><p id="p681910154818"><a name="p681910154818"></a><a name="p681910154818"></a>Solution Suggestion</p></th>
</tr>
</thead>
<tbody><tr id="row1098410169204"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p73939428116"><a name="p73939428116"></a><a name="p73939428116"></a>Memory access (L0 Buffer/L1 Buffer/Unified Buffer (UB), etc.)</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p339313421116"><a name="p339313421116"></a><a name="p339313421116"></a>Minimum access granularity/address alignment requirements of each storage unit:</p>
<p id="p14393742910"><a name="p14393742910"></a><a name="p14393742910"></a>UB: 32-byte alignment.</p>
<p id="p1339384218119"><a name="p1339384218119"></a><a name="p1339384218119"></a>L1 Buffer: 32-byte alignment.</p>
<p id="p03939425116"><a name="p03939425116"></a><a name="p03939425116"></a>L0A Buffer/L0B Buffer: 512-byte alignment.</p>
<p id="p139316421914"><a name="p139316421914"></a><a name="p139316421914"></a>L0C Buffer: 64-byte alignment.</p>
<p id="p1739319428114"><a name="p1739319428114"></a><a name="p1739319428114"></a>BiasTable Buffer: 64-byte alignment.</p>
<p id="p13932421316"><a name="p13932421316"></a><a name="p13932421316"></a>Fixpipe Buffer: 128-byte alignment.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><a name="ul9477161417"></a><a name="ul9477161417"></a><ul id="ul9477161417"><li>When transferring data, be aware of the alignment constraints.</li><li>For the UB, in some unaligned scenarios, you can use unaligned transfer APIs or apply certain techniques (for example, including redundant data when moving in and removing redundant data when moving out). For details, see <a href="../../../../operator_practice/simd_operator_impl/vector_programming/unaligned_scenario.md">Unaligned Scenarios</a>.</li></ul></td>
</tr>
<tr id="row698415161206"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p153938429117"><a name="p153938429117"></a><a name="p153938429117"></a>Memory access (UB)</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p1393442615"><a name="p1393442615"></a><a name="p1393442615"></a>UB bank access conflict (Vector computation access/transfer access).</p>
<p id="p73934420118"><a name="p73934420118"></a><a name="p73934420118"></a></p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p123932421311"><a name="p123932421311"></a><a name="p123932421311"></a>According to chip requirements, stagger the addresses during software implementation to resolve the bank conflict. For the specific solution, see the <a href="../../../../operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/avoid_bank_conflict_npu_arch_2201.md">Avoiding UB Bank Conflicts</a> section.</p></td>
</tr>
<tr id="row798416160209"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p5393154210115"><a name="p5393154210115"></a><a name="p5393154210115"></a>Memory access (GM)</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p11393174214120"><a name="p11393174214120"></a><a name="p11393174214120"></a>When multiple cores access the same GM address in parallel, the hardware serializes the accesses.</p>
<p id="p14393442116"><a name="p14393442116"></a><a name="p14393442116"></a></p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p9393104218116"><a name="p9393104218116"></a><a name="p9393104218116"></a>Accesses to the same address are serialized by the hardware, and the performance is determined by the queueing time, decreasing by approximately 10% to 20%.</p>
<p id="p539311424118"><a name="p539311424118"></a><a name="p539311424118"></a>For multi-core access, use staggered access (adjusting the data access order and modifying the partitioning strategy, etc.) so that the first load of data into the L2 Cache is followed by improved performance for subsequent accesses. For the specific solution, see the <a href="../../../../operator_practice/simd_operator_optimization/memory_access/avoid_same_address_access.md">Avoiding Same-Address Access</a> section.</p></td>
</tr>
<tr id="row189843164202"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1339364210115"><a name="p1339364210115"></a><a name="p1339364210115"></a>Memory access (GM)</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p153934427114"><a name="p153934427114"></a><a name="p153934427114"></a>When the length of data transferred in a single transfer is 16 KB or more, the optimal bandwidth performance can be achieved.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p1839334215112"><a name="p1839334215112"></a><a name="p1839334215112"></a>Based on measured experience, when the length of data transferred in a single transfer is 16 KB or more, the optimal bandwidth performance can usually be achieved. Therefore, for a single transfer, transfer data blocks as large as possible (this varies by chip).</p>
<p id="p14280187181119"><a name="p14280187181119"></a><a name="p14280187181119"></a>For the specific solution, see the <a href="../../../../operator_practice/simd_operator_optimization/memory_access/transfer_larger_data_blocks.md">Transferring Larger Data Blocks at a Time</a> section.</p></td>
</tr>
<tr id="row10984111617206"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1139394216112"><a name="p1139394216112"></a><a name="p1139394216112"></a>Memory access (GM--&gt;L1)</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p139313427112"><a name="p139313427112"></a><a name="p139313427112"></a>In DataCopy, the interval between adjacent contiguous data blocks of the source operand (the interval between the tail of the preceding data block and the head of the following data block) must not exceed 65535, in units of DataBlock (32 bytes).</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p6393842612"><a name="p6393842612"></a><a name="p6393842612"></a>When the gap between the tail of the preceding data block and the head of the following data block exceeds 65535, split it into multiple instructions.</p></td>
</tr>
<tr id="row28355376396"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1839313422120"><a name="p1839313422120"></a><a name="p1839313422120"></a>Memory access (GM)</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p439317429119"><a name="p439317429119"></a><a name="p439317429119"></a>Data transfer is split into 128B/256B/512B segments of different lengths, and non-aligned data is rounded up.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p133931242413"><a name="p133931242413"></a><a name="p133931242413"></a>For Tiling, align the inner axis of the transfer to 128B, 256B, or 512B as much as possible.</p>
<p id="p4131102271514"><a name="p4131102271514"></a><a name="p4131102271514"></a>For the specific solution, see the <a href="../../../../operator_practice/simd_operator_optimization/memory_access/gm_address_512b_alignment.md">GM Address 512B Alignment</a> section.</p></td>
</tr>
<tr id="row7725134123913"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1639310422014"><a name="p1639310422014"></a><a name="p1639310422014"></a>Cube</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p339311421712"><a name="p339311421712"></a><a name="p339311421712"></a>The depth of the MTE1 and MMAD instruction queues is 32.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p113937420111"><a name="p113937420111"></a><a name="p113937420111"></a>The corresponding instruction queues tend to fill up, which blocks the dispatch of other instructions and causes pipeline stalls.</p>
<p id="p18393742614"><a name="p18393742614"></a><a name="p18393742614"></a>LoadData (2D cube transfer) moves data from the L1 Buffer to the L0 Buffer and requires 32 instructions to be issued, whereas LoadData (convolution data transfer) requires only one instruction. It is recommended to use LoadData (convolution data transfer).</p></td>
</tr>
<tr id="row10203343193114"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1139319422115"><a name="p1139319422115"></a><a name="p1139319422115"></a>ICache</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p139319421513"><a name="p139319421513"></a><a name="p139319421513"></a>The ICache hardware specification limits the size to 32KB.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p23931842016"><a name="p23931842016"></a><a name="p23931842016"></a>Split Tiling_key or use template functions to reduce the code segment. For details, see <a href="../../../advanced_programming/aclnn_operator_development/design_and_implementation/multi_branch_strategy.md">Multi-Branch Strategy</a>.</p></td>
</tr>
<tr id="row194173017534"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1839310421316"><a name="p1839310421316"></a><a name="p1839310421316"></a>ICache</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p1139313428113"><a name="p1139313428113"></a><a name="p1139313428113"></a>When multiple cores access the ICache at the same address in parallel, the access is serialized by the hardware.</p>
<p id="p8393542217"><a name="p8393542217"></a><a name="p8393542217"></a></p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p639415421613"><a name="p639415421613"></a><a name="p639415421613"></a>In small-shape scenarios, minimize the number of launched cores to reduce same-address access issues across multiple cores.</p></td>
</tr>
<tr id="row724416349531"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p4394342218"><a name="p4394342218"></a><a name="p4394342218"></a>DCache</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p11394134215110"><a name="p11394134215110"></a><a name="p11394134215110"></a>The DCache hardware specification limits the size to 32 KB.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p939416422113"><a name="p939416422113"></a><a name="p939416422113"></a>None</p></td>
</tr>
<tr id="row6730204410538"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p153942421414"><a name="p153942421414"></a><a name="p153942421414"></a>Scalar</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p73942421015"><a name="p73942421015"></a><a name="p73942421015"></a>When the scalar writes to GM, the data is cached in the DCache. The hardware does not guarantee consistency between the DCache and GM, so the user must ensure it.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p1139411421118"><a name="p1139411421118"></a><a name="p1139411421118"></a>Use DataCacheCleanAndInvalid to ensure consistency.</p></td>
</tr>
<tr id="row56187293595"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p113943421110"><a name="p113943421110"></a><a name="p113943421110"></a>Cube</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p439413421616"><a name="p439413421616"></a><a name="p439413421616"></a>The L0C buffer capacity is 128 KB.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p439454212111"><a name="p439454212111"></a><a name="p439454212111"></a>None</p></td>
</tr>
<tr id="row16184293594"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p939494215115"><a name="p939494215115"></a><a name="p939494215115"></a>Cube</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p12394194219112"><a name="p12394194219112"></a><a name="p12394194219112"></a>The BiasTable buffer is 1 KB.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p1639410425117"><a name="p1639410425117"></a><a name="p1639410425117"></a>None</p></td>
</tr>
<tr id="row1361852935918"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p339410421114"><a name="p339410421114"></a><a name="p339410421114"></a>Cube</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p14651181022320"><a name="p14651181022320"></a><a name="p14651181022320"></a>In Cube computing scenarios, the float computing power is 1/4 of the half computing power.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p73946421415"><a name="p73946421415"></a><a name="p73946421415"></a>None</p></td>
</tr>
<tr id="row196181129135911"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p193948425111"><a name="p193948425111"></a><a name="p193948425111"></a>Cube</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p163941542119"><a name="p163941542119"></a><a name="p163941542119"></a>In Cube output on-the-fly quantization scenarios, quantization from int32_t to bfloat16_t is not supported.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><a name="ul7869116221"></a><a name="ul7869116221"></a><ul id="ul7869116221"><li>The AIV performs int32_t-&gt;float and float&gt;bfloat16_t conversions.</li><li>The AIC supports float-&gt;bfloat16_t on-the-fly quantization.</li></ul></td>
</tr>
<tr id="row661982915597"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p0394442216"><a name="p0394442216"></a><a name="p0394442216"></a>Vector</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p339412428113"><a name="p339412428113"></a><a name="p339412428113"></a>The Reduce API performs worse with half than with float.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p173946428117"><a name="p173946428117"></a><a name="p173946428117"></a>When half is written back to the UB, non-32-byte alignment occurs, which degrades performance. It is recommended to convert half to float for computation. In this scenario, use the float data type.</p></td>
</tr>
<tr id="row461911299592"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p14394142119"><a name="p14394142119"></a><a name="p14394142119"></a>Vector</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p93942421517"><a name="p93942421517"></a><a name="p93942421517"></a>The Exp/Ln APIs take the same amount of time to process the same number of half and float values.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p1139412421411"><a name="p1139412421411"></a><a name="p1139412421411"></a>Float is optimized internally, so the performance of the two is comparable. Developers can select an appropriate precision type based on the actual situation.</p></td>
</tr>
<tr id="row13619162955918"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p039544211115"><a name="p039544211115"></a><a name="p039544211115"></a>Pipeline synchronization (intra-core)</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p439594219111"><a name="p439594219111"></a><a name="p439594219111"></a>If set/wait synchronization does not match, the state remains, affecting subsequent operators.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p10395542819"><a name="p10395542819"></a><a name="p10395542819"></a>Use the twin debugging/mssanitizer tool to identify such issues in advance.</p></td>
</tr>
<tr id="row96190298591"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1395242514"><a name="p1395242514"></a><a name="p1395242514"></a>Pipeline synchronization (inter-core)</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p12395184212116"><a name="p12395184212116"></a><a name="p12395184212116"></a>The CrossCoreSetFlag counter has a limit. If it exceeds 15 times, a reverse synchronization is required; otherwise, the system may hang.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p11395342314"><a name="p11395342314"></a><a name="p11395342314"></a>Use the twin debugging/mssanitizer tool to report errors in advance for scenarios that exceed the limit.</p></td>
</tr>
<tr id="row13620172910597"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p139512427119"><a name="p139512427119"></a><a name="p139512427119"></a>General API constraints</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p1396164216116"><a name="p1396164216116"></a><a name="p1396164216116"></a>When using Ascend C APIs, general constraints on address overlap between source operands and destination operands</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p143966421117"><a name="p143966421117"></a><a name="p143966421117"></a>When using the Tensor high-dimensional split computation API of the basic APIs, to save address space, developers can define a single Tensor for both the source operand and the destination operand (that is, address overlap). Note the following constraints when using it:</p>
<a name="ul24282818269"></a><a name="ul24282818269"></a><ul id="ul24282818269"><li>Within a single iteration: the source operand and the destination operand must overlap 100% completely. Partial overlap is not supported.</li><li>Across multiple iterations: overlap between the destination operand of a preceding iteration and the source operand of a subsequent iteration is not supported. For example, the destination operand of the Nth iteration is the source operand of the (N+1)th iteration. In this case, the Nth iteration may overwrite the value of the source operand, resulting in unexpected results. In particular, for some binary computation APIs (Add, Sub, Mul, Max, Min, AddRelu, SubRelu), when the data type is half, int32_t, or float, overlap between the destination operand of a preceding iteration and the source operand of a subsequent iteration is supported: only for the case where the destination operand overlaps with the second source operand, and src1RepStride or dstRepStride must be 0.</li></ul></td>
</tr>
</tbody>
</table>
