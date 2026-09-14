# Optimization Suggestion Overview<a name="ZH-CN_TOPIC_0000002351966029"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:11:13.779Z -->

**Table 1** Overview of performance optimization suggestions

<a name="table1318114421218"></a>
<table><thead align="left"><tr id="row8182944131211"><th class="cellrowborder" valign="top" width="12.55%" id="mcps1.2.4.1.1"><p id="p918218445121"><a name="p918218445121"></a><a name="p918218445121"></a>Category</p></th>
<th class="cellrowborder" valign="top" width="42.89%" id="mcps1.2.4.1.2"><p id="p1165171215245"><a name="p1165171215245"></a><a name="p1165171215245"></a>Description</p></th>
<th class="cellrowborder" valign="top" width="44.56%" id="mcps1.2.4.1.3"><p id="p18182144410126"><a name="p18182144410126"></a><a name="p18182144410126"></a>Optimization Suggestion</p></th>
</tr>
</thead>
<tbody><tr id="row467101116190"><td class="cellrowborder" valign="top" width="12.55%" headers="mcps1.2.4.1.1 "><p id="p12671161117190"><a name="p12671161117190"></a><a name="p12671161117190"></a><a href="tiling_strategy/tiling_strategy.md">Tiling Strategy</a></p></td>
<td class="cellrowborder" valign="top" width="42.89%" headers="mcps1.2.4.1.2 "><p id="p172264002014"><a name="p172264002014"></a><a name="p172264002014"></a>Provides Tiling-related optimization suggestions to help developers select an appropriate Tiling strategy.</p></td>
<td class="cellrowborder" valign="top" width="44.56%" headers="mcps1.2.4.1.3 "><p id="p1880795418191"><a name="p1880795418191"></a><a name="p1880795418191"></a><a href="tiling_strategy/inter_core_load_balancing.md">Inter-core Load Balancing</a></p></td>
</tr>
<tr id="row149341920192014"><td class="cellrowborder" rowspan="5" valign="top" width="12.55%" headers="mcps1.2.4.1.1 "><p id="p883213304206"><a name="p883213304206"></a><a name="p883213304206"></a><a href="overhead_optimization/overhead_optimization.md">Head and Tail Overhead Optimization</a></p></td>
<td class="cellrowborder" rowspan="5" valign="top" width="42.89%" headers="mcps1.2.4.1.2 "><p id="p583343014202"><a name="p583343014202"></a><a name="p583343014202"></a>Provides optimization suggestions for reducing the head and tail overhead of operators (the latency incurred before and after operator computation).</p></td>
<td class="cellrowborder" valign="top" width="44.56%" headers="mcps1.2.4.1.3 "><p id="p12833530132017"><a name="p12833530132017"></a><a name="p12833530132017"></a><a href="overhead_optimization/set_appropriate_number_of_cores_and_operator_kernel_type.md">Setting an Appropriate Number of Cores and Operator Kernel Type</a></p></td>
</tr>
<tr id="row15726103392612"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p829011278387"><a name="p829011278387"></a><a name="p829011278387"></a><a href="overhead_optimization/limit_tilingdata_structure_size.md">Limiting the TilingData Structure Size</a></p></td>
</tr>
<tr id="row37921842182612"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p20792194211264"><a name="p20792194211264"></a><a name="p20792194211264"></a><a href="overhead_optimization/avoid_creating_and_initializing_tpipe_inside_object.md">Avoiding Creating and Initializing TPipe Inside an Object</a></p></td>
</tr>
<tr id="row466510256183"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p6665162514181"><a name="p6665162514181"></a><a name="p6665162514181"></a><a href="overhead_optimization/remove_redundant_workspace_related_operations_in_kernel_function.md">Removing Redundant Workspace-Related Operations in the Kernel Function</a></p></td>
</tr>
<tr id="row127293531686"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1873011531581"><a name="p1873011531581"></a><a name="p1873011531581"></a><a href="overhead_optimization/set_dci_compilation_options_to_reduce_operator_tail_overhead.md">Setting DCI Compilation Options to Reduce Operator Tail Overhead</a></p></td>
</tr>
<tr id="row1679913565276"><td class="cellrowborder" rowspan="2" valign="top" width="12.55%" headers="mcps1.2.4.1.1 "><p id="p426115882810"><a name="p426115882810"></a><a name="p426115882810"></a><a href="pipeline_scheduling/pipeline_scheduling.md">Pipeline Orchestration</a></p></td>
<td class="cellrowborder" rowspan="2" valign="top" width="42.89%" headers="mcps1.2.4.1.2 "><p id="p7261787283"><a name="p7261787283"></a><a name="p7261787283"></a><span>Improves hardware resource utilization and achieves higher throughput through methods such as task parallelization and asynchronous scheduling.</span></p>
<p id="p8265289289"><a name="p8265289289"></a><a name="p8265289289"></a></p></td>
<td class="cellrowborder" valign="top" width="44.56%" headers="mcps1.2.4.1.3 "><p id="p10261383288"><a name="p10261383288"></a><a name="p10261383288"></a><a href="pipeline_scheduling/enable_doublebuffer.md">Enabling DoubleBuffer</a></p></td>
</tr>
<tr id="row1136613722817"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p226138122819"><a name="p226138122819"></a><a name="p226138122819"></a><a href="pipeline_scheduling/enable_iterate_or_iterateall_to_avoid_aic_aiv_synchronization_dependency.md">Enabling Iterate or IterateAll Asynchronous APIs to Avoid AIC/AIV Synchronization Dependency</a></p></td>
</tr>
<tr id="row1182744171217"><td class="cellrowborder" rowspan="10" valign="top" width="12.55%" headers="mcps1.2.4.1.1 "><p id="p151821744141212"><a name="p151821744141212"></a><a name="p151821744141212"></a><a href="memory_access/memory_access.md">Memory Access</a></p></td>
<td class="cellrowborder" rowspan="10" valign="top" width="42.89%" headers="mcps1.2.4.1.2 "><p id="p9166412142413"><a name="p9166412142413"></a><a name="p9166412142413"></a>Maximize the move efficiency by controlling the size of the data blocks to be moved and the GM address. Reduce memory usage and improve computation efficiency through Buffer sharing and reuse, data compression and simplification, use of dedicated storage space, and memory access scheduling optimization.</p></td>
<td class="cellrowborder" valign="top" width="44.56%" headers="mcps1.2.4.1.3 "><p id="p1950214170344"><a name="p1950214170344"></a><a name="p1950214170344"></a><a href="memory_access/transfer_larger_data_blocks_at_once.md">Move Larger Data Blocks at a Time</a></p></td>
</tr>
<tr id="row952142912317"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p4521132952319"><a name="p4521132952319"></a><a name="p4521132952319"></a><a href="memory_access/gm_address_512b_alignment.md">Align GM Address to 512B</a></p></td>
</tr>
<tr id="row18583183214231"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p20583932132320"><a name="p20583932132320"></a><a name="p20583932132320"></a><a href="memory_access/use_move_api_efficiently.md">Use Move APIs Efficiently</a></p></td>
</tr>
<tr id="row919045611594"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p219010567596"><a name="p219010567596"></a><a name="p219010567596"></a><a href="memory_access/avoid_same_address_access.md">Avoid Same-Address Access</a></p></td>
</tr>
<tr id="row35892491516"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p458910491110"><a name="p458910491110"></a><a name="p458910491110"></a><a href="memory_access/set_proper_l2_cachemode.md">Set a Reasonable L2 CacheMode</a></p></td>
</tr>
<tr id="row10663175002711"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1651216229385"><a name="p1651216229385"></a><a name="p1651216229385"></a><a href="memory_access/share_temporary_buffer_between_operators_and_advanced_apis.md">Share Temporary Buffers Between Operators and High-Level APIs</a></p></td>
</tr>
<tr id="row018210442123"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p691144823817"><a name="p691144823817"></a><a name="p691144823817"></a><a href="memory_access/reuse_movement_operators_vecin_and_vecout.md">Reuse VECIN and VECOUT for Pure Move Operators</a></p></td>
</tr>
<tr id="row1429012717381"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1340842916384"><a name="p1340842916384"></a><a name="p1340842916384"></a><a href="memory_access/reduce_tensor_shapeinfo_dimensions_to_optimize_stack_space.md">Reduce Tensor ShapeInfo Dimensions to Optimize Stack Space</a></p></td>
</tr>
<tr id="row154071429103819"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1716658185315"><a name="p1716658185315"></a><a name="p1716658185315"></a><a href="memory_access/avoid_ub_bank_conflict/overview.md">Avoiding UB Bank Conflicts</a></p></td>
</tr>
<tr id="row14297312203113"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p162977123317"><a name="p162977123317"></a><a name="p162977123317"></a><a href="memory_access/l2_cache_tiling.md">L2 Cache Splitting</a></p></td>
</tr>
<tr id="row1618284481218"><td class="cellrowborder" rowspan="3" valign="top" width="12.55%" headers="mcps1.2.4.1.1 "><p id="p1184422893011"><a name="p1184422893011"></a><a name="p1184422893011"></a><a href="vector_compute/vector_compute.md">Vector Computation</a></p></td>
<td class="cellrowborder" rowspan="3" valign="top" width="42.89%" headers="mcps1.2.4.1.2 "><p id="p52813093010"><a name="p52813093010"></a><a name="p52813093010"></a>Optimization suggestions related to vector computation.</p></td>
<td class="cellrowborder" valign="top" width="44.56%" headers="mcps1.2.4.1.3 "><p id="p141827448127"><a name="p141827448127"></a><a name="p141827448127"></a><a href="vector_compute/implement_continuous_vector_computation_through_unified_buffer_fusion.md">Implementing Continuous Vector Computation Through Unified Buffer Fusion</a></p></td>
</tr>
<tr id="row244169133112"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p13310519891"><a name="p13310519891"></a><a name="p13310519891"></a><a href="vector_compute/flexibly_use_counter_mode_for_vector_operators.md">Flexibly Using Counter Mode for Vector Operators</a></p></td>
</tr>
<tr id="row14158230103211"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p12515651494"><a name="p12515651494"></a><a name="p12515651494"></a><a href="vector_compute/select_low_latency_instructions_to_optimize_reduction_operation_performance.md">Selecting Low-Latency Instructions to Optimize Reduction Operation Performance</a></p></td>
</tr>
<tr id="row8338204923918"><td class="cellrowborder" rowspan="5" valign="top" width="12.55%" headers="mcps1.2.4.1.1 "><p id="p0844172803015"><a name="p0844172803015"></a><a name="p0844172803015"></a><a href="matrix_compute/matrix_compute.md">Cube Computation</a></p></td>
<td class="cellrowborder" rowspan="5" valign="top" width="42.89%" headers="mcps1.2.4.1.2 "><p id="p112817307307"><a name="p112817307307"></a><a name="p112817307307"></a>Optimization suggestions related to Cube computation.</p></td>
<td class="cellrowborder" valign="top" width="44.56%" headers="mcps1.2.4.1.3 "><p id="p9338134917391"><a name="p9338134917391"></a><a name="p9338134917391"></a><a href="matrix_compute/implement_efficient_bias_computation_through_bt_buffer.md">Implementing Efficient Bias Computation Through BT Buffer</a></p></td>
</tr>
<tr id="row86314411522"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1663104165214"><a name="p1663104165214"></a><a name="p1663104165214"></a><a href="matrix_compute/store_quantization_parameters_in_fp_buffer_for_efficient_on_the_fly_quantization.md">Store quantization parameters in FP Buffer for efficient on-the-fly quantization</a></p></td>
</tr>
<tr id="row126277105216"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p762187125210"><a name="p762187125210"></a><a name="p762187125210"></a><a href="matrix_compute/store_data_temporarily_in_l0c_buffer_for_efficient_matrix_multiplication_result_accumulation.md">Store data temporarily in L0C Buffer for efficient matrix multiplication result accumulation</a></p></td>
</tr>
<tr id="row1064711011536"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p136471014532"><a name="p136471014532"></a><a name="p136471014532"></a><a href="matrix_compute/keep_smaller_matrix_resident_in_l1_buffer_and_move_larger_matrix_in_batches.md">Keep the smaller matrix resident in L1 Buffer and move the larger matrix in batches</a></p></td>
</tr>
<tr id="row316648195317"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p19778446153910"><a name="p19778446153910"></a><a name="p19778446153910"></a><a href="matrix_compute/enable_atomicadd_for_matmul.md">Enable the AtomicAdd option for Matmul</a></p></td>
</tr>
</tbody>
</table>
