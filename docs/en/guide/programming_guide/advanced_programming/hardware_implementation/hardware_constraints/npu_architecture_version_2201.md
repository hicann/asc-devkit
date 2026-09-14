# NPU Architecture Version 2201<a name="ZH-CN_TOPIC_0000002477825678"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-08-26T11:45:44.961Z pushedAt=2026-09-02T08:45:30.005Z -->

This section describes the hardware constraints and recommended solutions. The corresponding product models are:

-   Atlas A3 training products/Atlas A3 inference products
-   Atlas A2 training products/Atlas A2 inference products

**Table 1** Hardware constraints and recommended solutions

<a name="table5984016192019"></a>
<table><thead align="left"><tr id="row49841016132015"><th class="cellrowborder" valign="top" width="13.608639136086392%" id="mcps1.2.4.1.1"><p id="p1498411692018"><a name="p1498411692018"></a><a name="p1498411692018"></a>Category</p></th>
<th class="cellrowborder" valign="top" width="33.406659334066596%" id="mcps1.2.4.1.2"><p id="p16984816162019"><a name="p16984816162019"></a><a name="p16984816162019"></a>Hardware Constraint Description</p></th>
<th class="cellrowborder" valign="top" width="52.98470152984702%" id="mcps1.2.4.1.3"><p id="p681910154818"><a name="p681910154818"></a><a name="p681910154818"></a>Recommended Solution</p></th>
</tr>
</thead>
<tbody><tr id="row1098410169204"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p73939428116"><a name="p73939428116"></a><a name="p73939428116"></a>Memory access (L0 Buffer/L1 Buffer/UB, etc.)</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p339313421116"><a name="p339313421116"></a><a name="p339313421116"></a>Minimum access granularity/address alignment requirements of each storage unit:</p>
<p id="p14393742910"><a name="p14393742910"></a><a name="p14393742910"></a>Unified Buffer: 32-byte alignment.</p>
<p id="p1339384218119"><a name="p1339384218119"></a><a name="p1339384218119"></a>L1 Buffer: 32-byte alignment.</p>
<p id="p03939425116"><a name="p03939425116"></a><a name="p03939425116"></a>L0A Buffer/L0B Buffer: 512-byte alignment.</p>
<p id="p139316421914"><a name="p139316421914"></a><a name="p139316421914"></a>L0C Buffer: 64-byte alignment.</p>
<p id="p1739319428114"><a name="p1739319428114"></a><a name="p1739319428114"></a>BiasTable Buffer: 64-byte alignment.</p>
<p id="p13932421316"><a name="p13932421316"></a><a name="p13932421316"></a>Fixpipe Buffer: 128-byte alignment.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><a name="ul9477161417"></a><a name="ul9477161417"></a><ul id="ul9477161417"><li>When transferring data, be aware of the alignment constraints.</li><li>For UB, in some non-aligned scenarios, you can use the non-aligned transfer API or apply certain techniques (for example, including redundant data when moving in and removing redundant data when moving out) to resolve the issue. For details, see <a href="../../../../operator_practice/simd_operator_impl/vector_programming/non_aligned_scenario.md">Non-aligned Scenarios</a>.</li></ul></td>
</tr>
<tr id="row698415161206"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p153938429117"><a name="p153938429117"></a><a name="p153938429117"></a>Memory access (UB)</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p1393442615"><a name="p1393442615"></a><a name="p1393442615"></a>UB bank access conflict (Vector computation access/transfer access).</p>
<p id="p73934420118"><a name="p73934420118"></a><a name="p73934420118"></a></p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p123932421311"><a name="p123932421311"></a><a name="p123932421311"></a>Addresses must be staggered in software implementation as required by the chip to resolve the bank conflict. For the specific solution, see <a href="../../../../operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/avoid_bank_conflict_npu_arch_2201.md">Avoiding UB Bank Conflicts</a>.</p></td>
</tr>
<tr id="row798416160209"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p5393154210115"><a name="p5393154210115"></a><a name="p5393154210115"></a>Memory access (GM)</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p11393174214120"><a name="p11393174214120"></a><a name="p11393174214120"></a>Multi-core parallel same-address access to GM is serialized by hardware.</p>
<p id="p14393442116"><a name="p14393442116"></a><a name="p14393442116"></a></p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p9393104218116"><a name="p9393104218116"></a><a name="p9393104218116"></a>Access to the same address is serialized by hardware, and the performance is the queueing time, which decreases by about 10% to 20%;</p>
<p id="p539311424118"><a name="p539311424118"></a><a name="p539311424118"></a>For multi-core access, staggered access (adjusting the data access order and modifying the splitting strategy, etc.) is used so that the first load of data into the L2 cache is followed by improved performance for subsequent accesses. For the specific solution, see <a href="../../../../operator_practice/simd_operator_optimization/memory_access/avoid_same_address_access.md">Avoiding Same-Address Access</a>.</p></td>
</tr>
<tr id="row189843164202"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1339364210115"><a name="p1339364210115"></a><a name="p1339364210115"></a>Memory access (GM)</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p153934427114"><a name="p153934427114"></a><a name="p153934427114"></a>When the length of a single transfer of data is 16 KB or more, the optimal bandwidth performance can be achieved.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p1839334215112"><a name="p1839334215112"></a><a name="p1839334215112"></a>Based on measured experience, when the length of a single transfer of data is 16 KB or more, the optimal bandwidth performance can usually be achieved. Therefore, for a single transfer, transferring as large a data block as possible should be considered (this varies by chip).</p>
<p id="p14280187181119"><a name="p14280187181119"></a><a name="p14280187181119"></a>For the specific solution, see <a href="../../../../operator_practice/simd_operator_optimization/memory_access/transfer_larger_data_blocks_at_once.md">Transferring Larger Data Blocks at Once</a>.</p></td>
</tr>
<tr id="row10984111617206"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1139394216112"><a name="p1139394216112"></a><a name="p1139394216112"></a>Memory access (GM--&gt;L1)</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p139313427112"><a name="p139313427112"></a><a name="p139313427112"></a>In DataCopy, the interval between adjacent consecutive data blocks of the source operand (the interval between the tail of the preceding data block and the head of the following data block) must not exceed 65535, in the unit of DataBlock (32 bytes).</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p6393842612"><a name="p6393842612"></a><a name="p6393842612"></a>When the interval between the tail of the preceding data block and the head of the following data block exceeds 65535, split the operation into multiple instructions.</p></td>
</tr>
<tr id="row28355376396"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1839313422120"><a name="p1839313422120"></a><a name="p1839313422120"></a>Memory access (GM)</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p439317429119"><a name="p439317429119"></a><a name="p439317429119"></a>Data transfer is split into 128B/256B/512B segments of different lengths, and non-aligned transfers are rounded up.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p133931242413"><a name="p133931242413"></a><a name="p133931242413"></a>In Tiling, align the inner axis of the transfer to 128B, 256B, or 512B as much as possible.</p>
<p id="p4131102271514"><a name="p4131102271514"></a><a name="p4131102271514"></a>For the specific solution, see the <a href="../../../../operator_practice/simd_operator_optimization/memory_access/gm_address_512b_alignment.md">GM Address 512B Alignment</a> section.</p></td>
</tr>
<tr id="row7725134123913"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1639310422014"><a name="p1639310422014"></a><a name="p1639310422014"></a>Cube</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p339311421712"><a name="p339311421712"></a><a name="p339311421712"></a>The depth of the MTE1 and MMAD instruction queues is 32.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p113937420111"><a name="p113937420111"></a><a name="p113937420111"></a>The corresponding instruction queues tend to fill up, which blocks the dispatch of other instructions and causes pipeline stalls.</p>
<p id="p18393742614"><a name="p18393742614"></a><a name="p18393742614"></a>Load2D transfers data from the L1 Buffer to the L0 Buffer and requires 32 instructions to be issued, whereas Load3D requires only one instruction. It is recommended to use Load3D.</p></td>
</tr>
<tr id="row10203343193114"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1139319422115"><a name="p1139319422115"></a><a name="p1139319422115"></a>ICache</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p139319421513"><a name="p139319421513"></a><a name="p139319421513"></a>The ICache hardware specification is limited to 32KB.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p23931842016"><a name="p23931842016"></a><a name="p23931842016"></a>Split Tiling_key or use template functions to reduce the code segment. For details, see <a href="../../../advanced_programming/aclnn_operator_development/host_tiling_implementation/tiling_template_programming.md">Tiling Template Programming</a>.</p></td>
</tr>
<tr id="row194173017534"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1839310421316"><a name="p1839310421316"></a><a name="p1839310421316"></a>ICache</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p1139313428113"><a name="p1139313428113"></a><a name="p1139313428113"></a>When multiple cores access the ICache at the same address in parallel, the access is serialized by hardware.</p>
<p id="p8393542217"><a name="p8393542217"></a><a name="p8393542217"></a></p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p639415421613"><a name="p639415421613"></a><a name="p639415421613"></a>In small-shape scenarios, minimize the number of started cores to reduce multi-core same-address access issues.</p></td>
</tr>
<tr id="row724416349531"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p4394342218"><a name="p4394342218"></a><a name="p4394342218"></a>DCache</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p11394134215110"><a name="p11394134215110"></a><a name="p11394134215110"></a>The DCache hardware specification limits the size to 32KB.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p939416422113"><a name="p939416422113"></a><a name="p939416422113"></a>None</p></td>
</tr>
<tr id="row6730204410538"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p153942421414"><a name="p153942421414"></a><a name="p153942421414"></a>Scalar</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p73942421015"><a name="p73942421015"></a><a name="p73942421015"></a>When a scalar is written to the GM, the data is cached in the DCache. The hardware does not guarantee consistency between the DCache and the GM, which must be ensured by the user.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p1139411421118"><a name="p1139411421118"></a><a name="p1139411421118"></a>Use DataCacheCleanAndInvalid to ensure consistency.</p></td>
</tr>
<tr id="row56187293595"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p113943421110"><a name="p113943421110"></a><a name="p113943421110"></a>Cube</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p439413421616"><a name="p439413421616"></a><a name="p439413421616"></a>The L0C buffer capacity is 128KB.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p439454212111"><a name="p439454212111"></a><a name="p439454212111"></a>None</p></td>
</tr>
<tr id="row16184293594"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p939494215115"><a name="p939494215115"></a><a name="p939494215115"></a>Cube</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p12394194219112"><a name="p12394194219112"></a><a name="p12394194219112"></a>The BiasTable buffer is 1KB.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p1639410425117"><a name="p1639410425117"></a><a name="p1639410425117"></a>None</p></td>
</tr>
<tr id="row1361852935918"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p339410421114"><a name="p339410421114"></a><a name="p339410421114"></a>Cube</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p14651181022320"><a name="p14651181022320"></a><a name="p14651181022320"></a>In Cube compute scenarios, the float computing power is 1/4 of the half computing power.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p73946421415"><a name="p73946421415"></a><a name="p73946421415"></a>None</p></td>
</tr>
<tr id="row196181129135911"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p193948425111"><a name="p193948425111"></a><a name="p193948425111"></a>Cube</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p163941542119"><a name="p163941542119"></a><a name="p163941542119"></a>In the Cube output in-pass quantization scenario, quantization from int32_t to bfloat16_t is not supported.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><a name="ul7869116221"></a><a name="ul7869116221"></a><ul id="ul7869116221"><li>On the AIV, int32_t-&gt;float and float&gt;bfloat16_t conversions are available.</li><li>The AIC supports float-&gt;bfloat16_t in-pass quantization.</li></ul></td>
</tr>
<tr id="row661982915597"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p0394442216"><a name="p0394442216"></a><a name="p0394442216"></a>Vector</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p339412428113"><a name="p339412428113"></a><a name="p339412428113"></a>The Reduce API performs worse with half than with float.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p173946428117"><a name="p173946428117"></a><a name="p173946428117"></a>When half is written back to the UB, non-32-byte alignment causes performance degradation. It is recommended to convert half to float for computation, and use the float data type in this scenario.</p></td>
</tr>
<tr id="row461911299592"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p14394142119"><a name="p14394142119"></a><a name="p14394142119"></a>Vector</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p93942421517"><a name="p93942421517"></a><a name="p93942421517"></a>The Exp/Ln APIs take the same amount of time to process the same number of half and float values.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p1139412421411"><a name="p1139412421411"></a><a name="p1139412421411"></a>Because float is optimized internally, the two offer comparable performance. Developers can select an appropriate precision type based on actual conditions.</p></td>
</tr>
<tr id="row13619162955918"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p039544211115"><a name="p039544211115"></a><a name="p039544211115"></a>Pipeline synchronization (intra-core)</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p439594219111"><a name="p439594219111"></a><a name="p439594219111"></a>If set/wait synchronization does not match, the state remains and affects subsequent operators.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p10395542819"><a name="p10395542819"></a><a name="p10395542819"></a>Use the twin debugging/mssanitizer tool to identify such issues in advance.</p></td>
</tr>
<tr id="row96190298591"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1395242514"><a name="p1395242514"></a><a name="p1395242514"></a>Pipeline synchronization (inter-core)</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p12395184212116"><a name="p12395184212116"></a><a name="p12395184212116"></a>The CrossCoreSetFlag counter has a limit. If it exceeds 15 times, a reverse synchronization is required; otherwise, the system hangs.</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p11395342314"><a name="p11395342314"></a><a name="p11395342314"></a>Use the twin debugging/mssanitizer tool to report errors in advance for scenarios that exceed the limit.</p></td>
</tr>
<tr id="row13620172910597"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p139512427119"><a name="p139512427119"></a><a name="p139512427119"></a>General API constraints</p></td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p1396164216116"><a name="p1396164216116"></a><a name="p1396164216116"></a>When using Ascend C APIs, the general constraint on address overlap between the source operand and the destination operand</p></td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p143966421117"><a name="p143966421117"></a><a name="p143966421117"></a>When using the high-dimensional tensor slicing computation API of the basic API, to save address space, developers can define a single Tensor for both the source operand and the destination operand (that is, address overlap). Note the following constraints during use:</p>
<a name="ul24282818269"></a><a name="ul24282818269"></a><ul id="ul24282818269"><li>Within a single iteration: the source operand and the destination operand must overlap 100% completely. Partial overlap is not supported.</li><li>Across multiple iterations: overlap between the destination operand of a preceding iteration and the source operand of a subsequent iteration is not supported. For example, the destination operand of the Nth iteration is the source operand of the (N+1)th iteration. In this case, the Nth iteration may overwrite the value of the source operand, resulting in unexpected results. In particular, for some binary computation APIs (Add, Sub, Mul, Max, Min, AddRelu, SubRelu), when the data type is half, int32_t, or float, overlap between the destination operand of a preceding iteration and the source operand of a subsequent iteration is supported: only for the case where the destination operand overlaps the second source operand, and src1RepStride or dstRepStride must be 0.</li></ul></td>
</tr>
</tbody>
</table>
