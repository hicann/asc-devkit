# Matmul特性介绍<a name="ZH-CN_TOPIC_0000002500468238"></a>

除了前述[基础知识](../basics.md)和[算子实现](../operator_impl.md)中介绍的基本计算能力外，Matmul矩阵编程还提供了适用于不同场景的处理能力及多种功能，具体场景和功能列于下表中，详细内容请见后续章节。

**表1**  Matmul功能特性表

<a name="zh-cn_topic_0000002298078557_table1220612410208"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002298078557_row19206141132015"><th class="cellrowborder" valign="top" width="23.64%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002298078557_p52061441122017"><a name="zh-cn_topic_0000002298078557_p52061441122017"></a><a name="zh-cn_topic_0000002298078557_p52061441122017"></a>特性描述</p>
</th>
<th class="cellrowborder" valign="top" width="76.36%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002298078557_p1206124122015"><a name="zh-cn_topic_0000002298078557_p1206124122015"></a><a name="zh-cn_topic_0000002298078557_p1206124122015"></a>功能简介</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002298078557_row8206941192015"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p108541234142315"><a name="zh-cn_topic_0000002298078557_p108541234142315"></a><a name="zh-cn_topic_0000002298078557_p108541234142315"></a><a href="multi_core_aligned_split.md">多核对齐切分</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p8702111310525"><a name="zh-cn_topic_0000002298078557_p8702111310525"></a><a name="zh-cn_topic_0000002298078557_p8702111310525"></a>在多核场景中，支持将矩阵数据沿M、N、K轴切分，满足M能被singleCoreM整除、N能被singleCoreN整除、K能被singleCoreK整除的对齐场景时的处理方式，从而实现多核并行计算矩阵乘。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row12206184142016"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p954132121113"><a name="zh-cn_topic_0000002298078557_p954132121113"></a><a name="zh-cn_topic_0000002298078557_p954132121113"></a><a href="multi_core_unaligned_split.md">多核非对齐切分</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p17648104611560"><a name="zh-cn_topic_0000002298078557_p17648104611560"></a><a name="zh-cn_topic_0000002298078557_p17648104611560"></a>在多核场景中，支持将矩阵数据沿M、N、K轴切分。当出现M不能被singleCoreM整除、或N不能被singleCoreN整除、或K不能被singleCoreK整除的非对齐场景（即尾块场景）时的处理方式。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row12207144114201"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p5853634122310"><a name="zh-cn_topic_0000002298078557_p5853634122310"></a><a name="zh-cn_topic_0000002298078557_p5853634122310"></a><a href="async_scenario.md">异步场景处理</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p182077411209"><a name="zh-cn_topic_0000002298078557_p182077411209"></a><a name="zh-cn_topic_0000002298078557_p182077411209"></a>MIX场景（包含矩阵计算和矢量计算）下不需要等待矩阵乘计算完成，先执行其它计算。</p>
</td>
</tr>
<tr id="row5682043577"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p2779724105810"><a name="zh-cn_topic_0000002298078557_p2779724105810"></a><a name="zh-cn_topic_0000002298078557_p2779724105810"></a><a href="matmul_output_quant_dequant.md">矩阵乘输出的量化/反量化</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p8779024175813"><a name="zh-cn_topic_0000002298078557_p8779024175813"></a><a name="zh-cn_topic_0000002298078557_p8779024175813"></a>将矩阵乘的计算结果从L0C Buffer（CO1）搬出到Global Memory时，对矩阵元素执行数据量化或反量化操作。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row1320716413201"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p1854719415227"><a name="zh-cn_topic_0000002298078557_p1854719415227"></a><a name="zh-cn_topic_0000002298078557_p1854719415227"></a><a href="../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulCallBackFunc.md">自定义数据搬入搬出</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p1820704113201"><a name="zh-cn_topic_0000002298078557_p1820704113201"></a><a name="zh-cn_topic_0000002298078557_p1820704113201"></a>自定义矩阵乘计算前后的数据搬运函数。本功能支持用户实现左矩阵A、右矩阵B分别从Global Memory自定义搬入到L1 Buffer（A1）、L1 Buffer（B1）的过程，输出C矩阵从L0C Buffer（CO1）自定义搬出到Global Memory的过程。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row142071416207"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p94242715239"><a name="zh-cn_topic_0000002298078557_p94242715239"></a><a name="zh-cn_topic_0000002298078557_p94242715239"></a><a href="matmul_output_channel_split.md">矩阵乘输出的Channel拆分</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p320711418202"><a name="zh-cn_topic_0000002298078557_p320711418202"></a><a name="zh-cn_topic_0000002298078557_p320711418202"></a>矩阵乘输出的Channel拆分，又称ChannelSplit。指float数据类型、NZ数据格式的输出C矩阵按照16*8的分形大小存储。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row20207741152014"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p5546154142210"><a name="zh-cn_topic_0000002298078557_p5546154142210"></a><a name="zh-cn_topic_0000002298078557_p5546154142210"></a><a href="matrix_vector_multiply.md">矩阵向量乘</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p120764172019"><a name="zh-cn_topic_0000002298078557_p120764172019"></a><a name="zh-cn_topic_0000002298078557_p120764172019"></a>矩阵向量乘即GEMV，指矩阵乘计算中M=1，K&gt;1的场景，即对形状为(1, K)的左矩阵A实现矩阵乘计算。</p>
</td>
</tr>
<tr id="row1892083116812"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p87791524145813"><a name="zh-cn_topic_0000002298078557_p87791524145813"></a><a name="zh-cn_topic_0000002298078557_p87791524145813"></a><a href="4_2_sparse_matmul.md">4:2稀疏矩阵乘</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p97799246588"><a name="zh-cn_topic_0000002298078557_p97799246588"></a><a name="zh-cn_topic_0000002298078557_p97799246588"></a>4:2稀疏矩阵乘，又称Sparse Matmul。指对稀疏左矩阵A和4:2稠密化的右矩阵B实现矩阵乘计算。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row16577182820235"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p13858194593417"><a name="zh-cn_topic_0000002298078557_p13858194593417"></a><a name="zh-cn_topic_0000002298078557_p13858194593417"></a><a href="../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulPolicy.md">上/下三角矩阵乘</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p457718285237"><a name="zh-cn_topic_0000002298078557_p457718285237"></a><a name="zh-cn_topic_0000002298078557_p457718285237"></a>忽略位于矩阵中下三角或上三角位置的元素的计算，实现矩阵中上三角或下三角位置的元素的矩阵乘计算。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row2748530162319"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p141909471896"><a name="zh-cn_topic_0000002298078557_p141909471896"></a><a name="zh-cn_topic_0000002298078557_p141909471896"></a><a href="tscm_input_matmul.md">TSCM输入的矩阵乘</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p1748143002320"><a name="zh-cn_topic_0000002298078557_p1748143002320"></a><a name="zh-cn_topic_0000002298078557_p1748143002320"></a>对内存逻辑位置为<a href="../../../../technical_appendix/concepts_and_terms/glossary.md#p1392229512">TSCM</a>的左矩阵A或右矩阵B实现矩阵乘计算。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row1737123113238"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p1218910471691"><a name="zh-cn_topic_0000002298078557_p1218910471691"></a><a name="zh-cn_topic_0000002298078557_p1218910471691"></a><a href="matmul_output_n_alignment.md">矩阵乘输出的N方向对齐</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p18372153102310"><a name="zh-cn_topic_0000002298078557_p18372153102310"></a><a name="zh-cn_topic_0000002298078557_p18372153102310"></a>矩阵乘输出的N方向对齐，又称ND_ALIGN格式输出。指对数据格式为ND_ALIGN的输出C矩阵实现N方向32字节对齐的自动补齐及输出。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row191001232122314"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p81883473913"><a name="zh-cn_topic_0000002298078557_p81883473913"></a><a name="zh-cn_topic_0000002298078557_p81883473913"></a><a href="single_matmul_local_output.md">单次矩阵乘局部输出</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p610015321237"><a name="zh-cn_topic_0000002298078557_p610015321237"></a><a name="zh-cn_topic_0000002298078557_p610015321237"></a>单次矩阵乘局部输出，又称Partial Output，指矩阵乘计算时不对单核K方向的计算结果做累加，直接输出计算结果。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row1661618291916"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p17187847894"><a name="zh-cn_topic_0000002298078557_p17187847894"></a><a name="zh-cn_topic_0000002298078557_p17187847894"></a><a href="aic_aiv_independent_execution.md">AIC和AIV独立运行机制</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p116161329690"><a name="zh-cn_topic_0000002298078557_p116161329690"></a><a name="zh-cn_topic_0000002298078557_p116161329690"></a>AIC和AIV独立运行机制，又称双主模式。MIX场景（包含矩阵计算和矢量计算）下AIC核和AIV核独立运行代码，不依赖消息驱动。</p>
</td>
</tr>
<tr id="row185123919917"><td class="cellrowborder" valign="top" width="23.64%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p14741614191313"><a name="zh-cn_topic_0000002298078557_p14741614191313"></a><a name="zh-cn_topic_0000002298078557_p14741614191313"></a><a href="mx_matmul_scenario.md">MxMatmul场景</a></p>
</td>
<td class="cellrowborder" valign="top" width="76.36%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p107411814141310"><a name="zh-cn_topic_0000002298078557_p107411814141310"></a><a name="zh-cn_topic_0000002298078557_p107411814141310"></a>带有量化系数的矩阵乘法，即左矩阵和右矩阵均有对应的量化系数矩阵，对左矩阵和右矩阵分别量化后再做矩阵乘计算。</p>
</td>
</tr>
</tbody>
</table>

**表2**  BatchMatmu功能l特性表

<a name="zh-cn_topic_0000002298078557_table1558122415575"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002298078557_row1255852419575"><th class="cellrowborder" valign="top" width="24.04%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002298078557_p4558162445718"><a name="zh-cn_topic_0000002298078557_p4558162445718"></a><a name="zh-cn_topic_0000002298078557_p4558162445718"></a>特性描述</p>
</th>
<th class="cellrowborder" valign="top" width="75.96000000000001%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002298078557_p105581124185715"><a name="zh-cn_topic_0000002298078557_p105581124185715"></a><a name="zh-cn_topic_0000002298078557_p105581124185715"></a>功能简介</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002298078557_row1655816246576"><td class="cellrowborder" valign="top" width="24.04%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p755882465718"><a name="zh-cn_topic_0000002298078557_p755882465718"></a><a name="zh-cn_topic_0000002298078557_p755882465718"></a><a href="batch_matmul_basics.md">Batch Matmul基础功能</a></p>
</td>
<td class="cellrowborder" valign="top" width="75.96000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p175581124155711"><a name="zh-cn_topic_0000002298078557_p175581124155711"></a><a name="zh-cn_topic_0000002298078557_p175581124155711"></a>Batch Matmul基础功能，支持批量处理Matmul，调用一次IterateBatch接口，计算出多个singleCoreM * singleCoreN大小的C矩阵。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002298078557_row1955852414573"><td class="cellrowborder" valign="top" width="24.04%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002298078557_p6559172455716"><a name="zh-cn_topic_0000002298078557_p6559172455716"></a><a name="zh-cn_topic_0000002298078557_p6559172455716"></a><a href="batch_matmul_reuse_bias.md">Batch Matmul复用Bias矩阵</a></p>
</td>
<td class="cellrowborder" valign="top" width="75.96000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002298078557_p16559162418579"><a name="zh-cn_topic_0000002298078557_p16559162418579"></a><a name="zh-cn_topic_0000002298078557_p16559162418579"></a>每个Batch的Matmul计算复用同一个不带Batch轴的Bias矩阵。</p>
</td>
</tr>
</tbody>
</table>
