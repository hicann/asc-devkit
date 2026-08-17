# 优化建议总览表<a name="ZH-CN_TOPIC_0000002351966029"></a>

**表1**  性能优化建议总览表

<a name="table1318114421218"></a>
<table><thead align="left"><tr id="row8182944131211"><th class="cellrowborder" valign="top" width="12.55%" id="mcps1.2.4.1.1"><p id="p918218445121"><a name="p918218445121"></a><a name="p918218445121"></a>分类</p>
</th>
<th class="cellrowborder" valign="top" width="42.89%" id="mcps1.2.4.1.2"><p id="p1165171215245"><a name="p1165171215245"></a><a name="p1165171215245"></a>分类描述</p>
</th>
<th class="cellrowborder" valign="top" width="44.56%" id="mcps1.2.4.1.3"><p id="p18182144410126"><a name="p18182144410126"></a><a name="p18182144410126"></a>优化建议</p>
</th>
</tr>
</thead>
<tbody><tr id="row467101116190"><td class="cellrowborder" valign="top" width="12.55%" headers="mcps1.2.4.1.1 "><p id="p12671161117190"><a name="p12671161117190"></a><a name="p12671161117190"></a><a href="tiling_strategy/tiling_strategy.md">Tiling策略</a></p>
</td>
<td class="cellrowborder" valign="top" width="42.89%" headers="mcps1.2.4.1.2 "><p id="p172264002014"><a name="p172264002014"></a><a name="p172264002014"></a>提供Tiling相关的优化建议，便于开发者选择合适的Tiling切分策略。</p>
</td>
<td class="cellrowborder" valign="top" width="44.56%" headers="mcps1.2.4.1.3 "><p id="p1880795418191"><a name="p1880795418191"></a><a name="p1880795418191"></a><a href="tiling_strategy/inter_core_load_balance.md">核间负载均衡</a></p>
</td>
</tr>
<tr id="row149341920192014"><td class="cellrowborder" rowspan="5" valign="top" width="12.55%" headers="mcps1.2.4.1.1 "><p id="p883213304206"><a name="p883213304206"></a><a name="p883213304206"></a><a href="overhead_optimization/overhead_optimization.md">头尾开销优化</a></p>
</td>
<td class="cellrowborder" rowspan="5" valign="top" width="42.89%" headers="mcps1.2.4.1.2 "><p id="p583343014202"><a name="p583343014202"></a><a name="p583343014202"></a>提供降低算子头尾开销（算子执行计算前后产生的时延）的优化建议。</p>
</td>
<td class="cellrowborder" valign="top" width="44.56%" headers="mcps1.2.4.1.3 "><p id="p12833530132017"><a name="p12833530132017"></a><a name="p12833530132017"></a><a href="overhead_optimization/core_count_kernel_type.md">设置合适的核数和算子Kernel类型</a></p>
</td>
</tr>
<tr id="row15726103392612"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p829011278387"><a name="p829011278387"></a><a name="p829011278387"></a><a href="overhead_optimization/limit_tiling_data_size.md">限制TilingData结构大小</a></p>
</td>
</tr>
<tr id="row37921842182612"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p20792194211264"><a name="p20792194211264"></a><a name="p20792194211264"></a><a href="overhead_optimization/avoid_tpipe_init_in_object.md">避免TPipe在对象内创建和初始化</a></p>
</td>
</tr>
<tr id="row466510256183"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p6665162514181"><a name="p6665162514181"></a><a name="p6665162514181"></a><a href="overhead_optimization/remove_workspace_redundancy.md">核函数内删除Workspace相关冗余操作</a></p>
</td>
</tr>
<tr id="row127293531686"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1873011531581"><a name="p1873011531581"></a><a name="p1873011531581"></a><a href="overhead_optimization/dci_compile_option.md">设置DCI编译选项来减少算子尾开销</a></p>
</td>
</tr>
<tr id="row1679913565276"><td class="cellrowborder" rowspan="2" valign="top" width="12.55%" headers="mcps1.2.4.1.1 "><p id="p426115882810"><a name="p426115882810"></a><a name="p426115882810"></a><a href="pipeline_scheduling/pipeline_scheduling.md">流水编排</a></p>
</td>
<td class="cellrowborder" rowspan="2" valign="top" width="42.89%" headers="mcps1.2.4.1.2 "><p id="p7261787283"><a name="p7261787283"></a><a name="p7261787283"></a><span>通过任务并行化、异步调度等方法，提升硬件资源利用率，实现更高的吞吐率。</span></p>
<p id="p8265289289"><a name="p8265289289"></a><a name="p8265289289"></a></p>
</td>
<td class="cellrowborder" valign="top" width="44.56%" headers="mcps1.2.4.1.3 "><p id="p10261383288"><a name="p10261383288"></a><a name="p10261383288"></a><a href="pipeline_scheduling/enable_double_buffer.md">开启DoubleBuffer</a></p>
</td>
</tr>
<tr id="row1136613722817"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p226138122819"><a name="p226138122819"></a><a name="p226138122819"></a><a href="pipeline_scheduling/enable_iterate_avoid_sync.md">启用Iterate或IterateAll异步接口避免AIC/AIV同步依赖</a></p>
</td>
</tr>
<tr id="row1182744171217"><td class="cellrowborder" rowspan="10" valign="top" width="12.55%" headers="mcps1.2.4.1.1 "><p id="p151821744141212"><a name="p151821744141212"></a><a name="p151821744141212"></a><a href="memory_access/memory_access.md">内存访问</a></p>
</td>
<td class="cellrowborder" rowspan="10" valign="top" width="42.89%" headers="mcps1.2.4.1.2 "><p id="p9166412142413"><a name="p9166412142413"></a><a name="p9166412142413"></a>通过控制搬运的数据块大小和GM地址等来实现搬运效率的最大化；通过Buffer的共享与复用、数据压缩精简、使用专用存储空间、访存调度优化等方法来减少内存占用，提升计算效率。</p>
</td>
<td class="cellrowborder" valign="top" width="44.56%" headers="mcps1.2.4.1.3 "><p id="p1950214170344"><a name="p1950214170344"></a><a name="p1950214170344"></a><a href="memory_access/transfer_larger_data_blocks.md">尽量一次搬运较大的数据块</a></p>
</td>
</tr>
<tr id="row952142912317"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p4521132952319"><a name="p4521132952319"></a><a name="p4521132952319"></a><a href="memory_access/gm_address_512b_alignment.md">GM地址尽量512B对齐</a></p>
</td>
</tr>
<tr id="row18583183214231"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p20583932132320"><a name="p20583932132320"></a><a name="p20583932132320"></a><a href="memory_access/efficient_transfer_api.md">高效的使用搬运API</a></p>
</td>
</tr>
<tr id="row919045611594"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p219010567596"><a name="p219010567596"></a><a name="p219010567596"></a><a href="memory_access/avoid_same_address_access.md">避免同地址访问</a></p>
</td>
</tr>
<tr id="row35892491516"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p458910491110"><a name="p458910491110"></a><a name="p458910491110"></a><a href="memory_access/l2_cache_mode_config.md">设置合理的L2 CacheMode</a></p>
</td>
</tr>
<tr id="row10663175002711"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1651216229385"><a name="p1651216229385"></a><a name="p1651216229385"></a><a href="memory_access/shared_temp_buffer.md">算子与高阶API共享临时Buffer</a></p>
</td>
</tr>
<tr id="row018210442123"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p691144823817"><a name="p691144823817"></a><a name="p691144823817"></a><a href="memory_access/vecin_vecout_reuse.md">纯搬运类算子VECIN和VECOUT建议复用</a></p>
</td>
</tr>
<tr id="row1429012717381"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1340842916384"><a name="p1340842916384"></a><a name="p1340842916384"></a><a href="memory_access/reduce_shapeinfo_optimize_stack.md">通过缩减Tensor ShapeInfo维度，优化栈空间</a></p>
</td>
</tr>
<tr id="row154071429103819"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1716658185315"><a name="p1716658185315"></a><a name="p1716658185315"></a><a href="memory_access/avoid_ub_bank_conflict/overview.md">避免UB的bank冲突</a></p>
</td>
</tr>
<tr id="row14297312203113"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p162977123317"><a name="p162977123317"></a><a name="p162977123317"></a><a href="memory_access/l2_cache_split.md">L2 Cache切分</a></p>
</td>
</tr>
<tr id="row1618284481218"><td class="cellrowborder" rowspan="4" valign="top" width="12.55%" headers="mcps1.2.4.1.1 "><p id="p1184422893011"><a name="p1184422893011"></a><a name="p1184422893011"></a><a href="vector_compute/vector_compute.md">矢量计算</a></p>
</td>
<td class="cellrowborder" rowspan="4" valign="top" width="42.89%" headers="mcps1.2.4.1.2 "><p id="p52813093010"><a name="p52813093010"></a><a name="p52813093010"></a>矢量计算相关优化建议。</p>
</td>
<td class="cellrowborder" valign="top" width="44.56%" headers="mcps1.2.4.1.3 "><p id="p141827448127"><a name="p141827448127"></a><a name="p141827448127"></a><a href="vector_compute/ub_fusion_vector_compute.md">通过Unified Buffer融合实现连续vector计算</a></p>
</td>
</tr>
<tr id="row244169133112"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p13310519891"><a name="p13310519891"></a><a name="p13310519891"></a><a href="vector_compute/vector_counter_mode.md">Vector算子灵活运用Counter模式</a></p>
</td>
</tr>
<tr id="row244169133113"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p13310519892"><a name="p13310519892"></a><a name="p13310519892"></a><a href="vector_compute/mask_reuse_optimization.md">基于全局掩码复用的计算性能优化</a></p>
</td>
</tr>
<tr id="row14158230103211"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p12515651494"><a name="p12515651494"></a><a name="p12515651494"></a><a href="vector_compute/low_latency_instruction.md">选择低延迟指令，优化归约操作性能</a></p>
</td>
</tr>
<tr id="row8338204923918"><td class="cellrowborder" rowspan="5" valign="top" width="12.55%" headers="mcps1.2.4.1.1 "><p id="p0844172803015"><a name="p0844172803015"></a><a name="p0844172803015"></a><a href="matrix_compute/matrix_compute.md">矩阵计算</a></p>
</td>
<td class="cellrowborder" rowspan="5" valign="top" width="42.89%" headers="mcps1.2.4.1.2 "><p id="p112817307307"><a name="p112817307307"></a><a name="p112817307307"></a>矩阵计算相关优化建议。</p>
</td>
<td class="cellrowborder" valign="top" width="44.56%" headers="mcps1.2.4.1.3 "><p id="p9338134917391"><a name="p9338134917391"></a><a name="p9338134917391"></a><a href="matrix_compute/bt_buffer_bias_compute.md">通过BT Buffer实现高效的bias计算</a></p>
</td>
</tr>
<tr id="row86314411522"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1663104165214"><a name="p1663104165214"></a><a name="p1663104165214"></a><a href="matrix_compute/fp_buffer_quantization.md">通过FP Buffer存放量化参数实现高效随路量化</a></p>
</td>
</tr>
<tr id="row126277105216"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p762187125210"><a name="p762187125210"></a><a name="p762187125210"></a><a href="matrix_compute/l0c_buffer_matmul_accumulate.md">通过L0C Buffer数据暂存实现高效的矩阵乘结果累加</a></p>
</td>
</tr>
<tr id="row1064711011536"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p136471014532"><a name="p136471014532"></a><a name="p136471014532"></a><a href="matrix_compute/small_matrix_l1_buffer.md">较小矩阵长驻L1 Buffer，仅分次搬运较大矩阵</a></p>
</td>
</tr>
<tr id="row316648195317"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p19778446153910"><a name="p19778446153910"></a><a name="p19778446153910"></a><a href="matrix_compute/matmul_atomic_add.md">Matmul开启AtomicAdd选项</a></p>
</td>
</tr>
</tbody>
</table>
