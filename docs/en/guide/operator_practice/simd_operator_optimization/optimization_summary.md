# Optimization Suggestions Overview<a name="ZH-CN_TOPIC_0000002351966029"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:32:03.904Z pushedAt=2026-11-03T07:05:13.323Z nl=2 -->

**Table 1** Performance optimization suggestions overview

<a name="table1318114421218"></a>
<table><thead align="left"><tr id="row8182944131211"><th class="cellrowborder" valign="top" width="12.55%" id="mcps1.2.4.1.1"><p id="p918218445121"><a name="p918218445121"></a><a name="p918218445121"></a>Category</p></th>
<th class="cellrowborder" valign="top" width="42.89%" id="mcps1.2.4.1.2"><p id="p1165171215245"><a name="p1165171215245"></a><a name="p1165171215245"></a>Description</p></th>
<th class="cellrowborder" valign="top" width="44.56%" id="mcps1.2.4.1.3"><p id="p18182144410126"><a name="p18182144410126"></a><a name="p18182144410126"></a>Optimization Suggestion</p></th>
</tr>
</thead>
<tbody><tr id="row467101116190"><td class="cellrowborder" valign="top" width="12.55%" headers="mcps1.2.4.1.1 "><p id="p12671161117190"><a name="p12671161117190"></a><a name="p12671161117190"></a><a href="tiling_strategy/tiling_strategy.md">Tiling Strategy</a></p></td>
<td class="cellrowborder" valign="top" width="42.89%" headers="mcps1.2.4.1.2 "><p id="p172264002014"><a name="p172264002014"></a><a name="p172264002014"></a>Provides Tiling-related optimization suggestions to help developers select an appropriate Tiling strategy.</p></td>
<td class="cellrowborder" valign="top" width="44.56%" headers="mcps1.2.4.1.3 "><p id="p1880795418191"><a name="p1880795418191"></a><a name="p1880795418191"></a><a href="tiling_strategy/inter_core_load_balance.md">Inter-core Load Balance</a></p></td>
</tr>
<tr id="row149341920192014"><td class="cellrowborder" rowspan="5" valign="top" width="12.55%" headers="mcps1.2.4.1.1 "><p id="p883213304206"><a name="p883213304206"></a><a name="p883213304206"></a><a href="overhead_optimization/overhead_optimization.md">Head and Tail Overhead Optimization</a></p></td>
<td class="cellrowborder" rowspan="5" valign="top" width="42.89%" headers="mcps1.2.4.1.2 "><p id="p583343014202"><a name="p583343014202"></a><a name="p583343014202"></a>Provides optimization suggestions for reducing the operator head and tail overhead (the latency generated before and after operator computation).</p></td>
<td class="cellrowborder" valign="top" width="44.56%" headers="mcps1.2.4.1.3 "><p id="p12833530132017"><a name="p12833530132017"></a><a name="p12833530132017"></a><a href="overhead_optimization/core_count_kernel_type.md">Set an appropriate number of cores and operator kernel type</a></p></td>
</tr>
<tr id="row15726103392612"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p829011278387"><a name="p829011278387"></a><a name="p829011278387"></a><a href="overhead_optimization/limit_tiling_data_size.md">Limit the TilingData structure size</a></p></td>
</tr>
<tr id="row37921842182612"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p20792194211264"><a name="p20792194211264"></a><a name="p20792194211264"></a><a href="overhead_optimization/avoid_tpipe_init_in_object.md">Avoid creating and initializing TPipe within an object</a></p></td>
</tr>
<tr id="row466510256183"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p6665162514181"><a name="p6665162514181"></a><a name="p6665162514181"></a><a href="overhead_optimization/remove_workspace_redundancy.md">Remove redundant Workspace-related operations in the kernel</a></p></td>
</tr>
<tr id="row127293531686"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1873011531581"><a name="p1873011531581"></a><a name="p1873011531581"></a><a href="overhead_optimization/dci_compile_option.md">Set DCI compilation options to reduce the operator tail overhead</a></p></td>
</tr>
<tr id="row1679913565276"><td class="cellrowborder" rowspan="2" valign="top" width="12.55%" headers="mcps1.2.4.1.1 "><p id="p426115882810"><a name="p426115882810"></a><a name="p426115882810"></a><a href="pipeline_scheduling/pipeline_scheduling.md">Pipeline scheduling</a></p></td>
<td class="cellrowborder" rowspan="2" valign="top" width="42.89%" headers="mcps1.2.4.1.2 "><p id="p7261787283"><a name="p7261787283"></a><a name="p7261787283"></a><span>Improves hardware resource utilization and achieves higher throughput through methods such as task parallelization and asynchronous scheduling.</span></p>
<p id="p8265289289"><a name="p8265289289"></a><a name="p8265289289"></a></p></td>
<td class="cellrowborder" valign="top" width="44.56%" headers="mcps1.2.4.1.3 "><p id="p10261383288"><a name="p10261383288"></a><a name="p10261383288"></a><a href="pipeline_scheduling/enable_double_buffer.md">Enable DoubleBuffer</a></p></td>
</tr>
<tr id="row1136613722817"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p226138122819"><a name="p226138122819"></a><a name="p226138122819"></a><a href="pipeline_scheduling/enable_iterate_avoid_sync.md">Enable the Iterate or IterateAll asynchronous API to avoid AIC/AIV synchronization dependencies</a></p></td>
</tr>
<tr id="row1182744171217"><td class="cellrowborder" rowspan="10" valign="top" width="12.55%" headers="mcps1.2.4.1.1 "><p id="p151821744141212"><a name="p151821744141212"></a><a name="p151821744141212"></a><a href="memory_access/memory_access.md">Memory Access</a></p></td>
<td class="cellrowborder" rowspan="10" valign="top" width="42.89%" headers="mcps1.2.4.1.2 "><p id="p9166412142413"><a name="p9166412142413"></a><a name="p9166412142413"></a>Maximize data transfer efficiency by controlling the size of transferred data blocks and GM addresses. Reduce memory usage and improve computing efficiency through buffer sharing and reuse, data compression, use of dedicated storage space, and memory access scheduling optimization.</p></td>
<td class="cellrowborder" valign="top" width="44.56%" headers="mcps1.2.4.1.3 "><p id="p1950214170344"><a name="p1950214170344"></a><a name="p1950214170344"></a><a href="memory_access/transfer_larger_data_blocks.md">Transfer Larger Data Blocks at a Time</a></p></td>
</tr>
<tr id="row952142912317"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p4521132952319"><a name="p4521132952319"></a><a name="p4521132952319"></a><a href="memory_access/gm_address_512b_alignment.md">Align GM Addresses to 512B</a></p></td>
</tr>
<tr id="row18583183214231"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p20583932132320"><a name="p20583932132320"></a><a name="p20583932132320"></a><a href="memory_access/efficient_transfer_api.md">Use Transfer APIs Efficiently</a></p></td>
</tr>
<tr id="row919045611594"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p219010567596"><a name="p219010567596"></a><a name="p219010567596"></a><a href="memory_access/avoid_same_address_access.md">Avoid Accessing the Same Address</a></p></td>
</tr>
<tr id="row35892491516"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p458910491110"><a name="p458910491110"></a><a name="p458910491110"></a><a href="memory_access/l2_cache_mode_config.md">Set a Reasonable L2 CacheMode</a></p></td>
</tr>
<tr id="row10663175002711"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1651216229385"><a name="p1651216229385"></a><a name="p1651216229385"></a><a href="memory_access/shared_temp_buffer.md">Share Temporary Buffers Between Operators and Advanced APIs</a></p></td>
</tr>
<tr id="row018210442123"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p691144823817"><a name="p691144823817"></a><a name="p691144823817"></a><a href="memory_access/vecin_vecout_reuse.md">Reuse VECIN and VECOUT for Pure Data Transfer Operators</a></p></td>
</tr>
<tr id="row1429012717381"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1340842916384"><a name="p1340842916384"></a><a name="p1340842916384"></a><a href="memory_access/reduce_shapeinfo_optimize_stack.md">Optimize Stack Space by Reducing Tensor ShapeInfo Dimensions</a></p></td>
</tr>
<tr id="row154071429103819"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1716658185315"><a name="p1716658185315"></a><a name="p1716658185315"></a><a href="memory_access/avoid_ub_bank_conflict/overview.md">Avoiding Unified Buffer (UB) bank conflicts</a></p></td>
</tr>
<tr id="row14297312203113"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p162977123317"><a name="p162977123317"></a><a name="p162977123317"></a><a href="memory_access/l2_cache_split.md">L2 Cache splitting</a></p></td>
</tr>
<tr id="row1618284481218"><td class="cellrowborder" rowspan="4" valign="top" width="12.55%" headers="mcps1.2.4.1.1 "><p id="p1184422893011"><a name="p1184422893011"></a><a name="p1184422893011"></a><a href="vector_compute/vector_compute.md">Vector computation</a></p></td>
<td class="cellrowborder" rowspan="4" valign="top" width="42.89%" headers="mcps1.2.4.1.2 "><p id="p52813093010"><a name="p52813093010"></a><a name="p52813093010"></a>Optimization suggestions related to vector computation.</p></td>
<td class="cellrowborder" valign="top" width="44.56%" headers="mcps1.2.4.1.3 "><p id="p141827448127"><a name="p141827448127"></a><a name="p141827448127"></a><a href="vector_compute/ub_fusion_vector_compute.md">Implementing continuous vector computation through UB fusion</a></p></td>
</tr>
<tr id="row244169133112"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p13310519891"><a name="p13310519891"></a><a name="p13310519891"></a><a href="vector_compute/vector_counter_mode.md">Flexibly using the Counter mode for Vector operators</a></p></td>
</tr>
<tr id="row244169133113"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p13310519892"><a name="p13310519892"></a><a name="p13310519892"></a><a href="vector_compute/mask_reuse_optimization.md">Optimizing computation performance based on global mask reuse</a></p></td>
</tr>
<tr id="row14158230103211"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p12515651494"><a name="p12515651494"></a><a name="p12515651494"></a><a href="vector_compute/low_latency_instruction.md">Selecting low-latency instructions to optimize reduction operation performance</a></p></td>
</tr>
<tr id="row8338204923918"><td class="cellrowborder" rowspan="5" valign="top" width="12.55%" headers="mcps1.2.4.1.1 "><p id="p0844172803015"><a name="p0844172803015"></a><a name="p0844172803015"></a><a href="matrix_compute/matrix_compute.md">Cube computation</a></p></td>
<td class="cellrowborder" rowspan="5" valign="top" width="42.89%" headers="mcps1.2.4.1.2 "><p id="p112817307307"><a name="p112817307307"></a><a name="p112817307307"></a>Optimization suggestions related to cube computation.</p></td>
<td class="cellrowborder" valign="top" width="44.56%" headers="mcps1.2.4.1.3 "><p id="p9338134917391"><a name="p9338134917391"></a><a name="p9338134917391"></a><a href="matrix_compute/bt_buffer_bias_compute.md">Efficient bias computation through BT Buffer</a></p></td>
</tr>
<tr id="row86314411522"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1663104165214"><a name="p1663104165214"></a><a name="p1663104165214"></a><a href="matrix_compute/fp_buffer_quantization.md">Efficient in-band quantization by storing quantization parameters in FP Buffer</a></p></td>
</tr>
<tr id="row126277105216"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p762187125210"><a name="p762187125210"></a><a name="p762187125210"></a><a href="matrix_compute/l0c_buffer_matmul_accumulate.md">Efficient cube multiplication result accumulation through L0C Buffer data staging</a></p></td>
</tr>
<tr id="row1064711011536"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p136471014532"><a name="p136471014532"></a><a name="p136471014532"></a><a href="matrix_compute/small_matrix_l1_buffer.md">Keep smaller cubes resident in L1 Buffer and move larger cubes in batches</a></p></td>
</tr>
<tr id="row316648195317"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p19778446153910"><a name="p19778446153910"></a><a name="p19778446153910"></a><a href="matrix_compute/matmul_atomic_add.md">Enable the AtomicAdd option for Matmul</a></p></td>
</tr>
</tbody>
</table>
