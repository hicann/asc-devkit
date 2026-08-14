# 设置合理的L2 CacheMode<a name="ZH-CN_TOPIC_0000002356424662"></a>

【优先级】高

>[!NOTE]说明 
>该性能优化指导适用于如下产品型号：
><!-- npu="950" id1 -->
>- Ascend 950PR/Ascend 950DT
><!-- end id1 -->
><!-- npu="A3" id2 -->
>- Atlas A3 训练系列产品/Atlas A3 推理系列产品
><!-- end id2 -->
><!-- npu="910b" id3 -->
>- Atlas A2 训练系列产品/Atlas A2 推理系列产品
><!-- end id3 -->

【描述】L2 Cache常用于缓存频繁访问的数据，其物理位置如下图所示：

![](../../../figures/reduce_61.png)

L2 Cache的带宽相比GM的带宽有数倍的提升，因此当数据命中L2 Cache时，数据的搬运耗时会优化数倍。通常情况下，L2 Cache命中率越高，算子的性能越好，在实际访问中需要通过设置合理的L2 CacheMode来保证重复读取的数据尽量缓存在L2 Cache上。

## L2 Cache访问的原理及CacheMode介绍<a name="section75591806575"></a>

数据通过MTE2搬运单元搬入时，L2 Cache访问的典型流程如下：

![](../../../figures/reduce_62.png)

数据通过MTE3或者Fixpipe搬运单元搬出时，L2 Cache访问的典型流程如下：

![](../../../figures/reduce_opt_63.png)

从上面的流程可以看出，当数据访问总量超出L2 Cache容量时，AI Core会对L2 Cache进行数据替换。由于Cache一致性的要求，替换过程中旧数据需要先写回GM（此过程中会占用GM带宽），旧数据写回后，新的数据才能进入L2 Cache。

开发者可以针对访问的数据设置其CacheMode，对于只访问一次的Global Memory数据设置其访问状态为不进入L2 Cache，这样可以更加高效的利用L2 Cache缓存需要重复读取的数据，避免一次性访问的数据替换有效数据。

## 设置L2 CacheMode的方法<a name="section058817270210"></a>

Ascend C基于GlobalTensor提供了SetL2CacheHint接口，用户可以根据需要指定CacheMode。

考虑如下场景，构造两个Tensor的相加计算，输入x、y和输出z的Shape均为\(8192, 8192\)。下文完整样例请参考[设置合理L2 CacheMode样例](../../../../../../examples/01_simd_cpp_api/05_best_practices/00_vector_compute/add_high_performance)。

<a name="table12921549195512"></a>
<table><thead align="left"><tr id="row1229364945511"><th class="cellrowborder" valign="top" width="6.813978389954251%" id="mcps1.1.4.1.1"><p id="p2081249145715"><a name="p2081249145715"></a><a name="p2081249145715"></a>实现方案</p>
</th>
<th class="cellrowborder" valign="top" width="42.67497323079919%" id="mcps1.1.4.1.2"><p id="p2029374985519"><a name="p2029374985519"></a><a name="p2029374985519"></a>原始实现</p>
</th>
<th class="cellrowborder" valign="top" width="50.51104837924658%" id="mcps1.1.4.1.3"><p id="p152931149115516"><a name="p152931149115516"></a><a name="p152931149115516"></a>优化实现</p>
</th>
</tr>
</thead>
<tbody><tr id="row1629374995517"><td class="cellrowborder" valign="top" width="6.813978389954251%" headers="mcps1.1.4.1.1 "><p id="p108124995710"><a name="p108124995710"></a><a name="p108124995710"></a>实现方法</p>
</td>
<td class="cellrowborder" valign="top" width="42.67497323079919%" headers="mcps1.1.4.1.2 "><p id="p1764591574211"><a name="p1764591574211"></a><a name="p1764591574211"></a>总数据量384MB，其中：x：128MB；y：128MB；z：128MB。</p>
<p id="p13628115612406"><a name="p13628115612406"></a><a name="p13628115612406"></a>使用48个核参与计算，按行方向切分6份，列方向切分8份。</p>
<p id="p5900191641514"><a name="p5900191641514"></a><a name="p5900191641514"></a>x、y、z对应GlobalTensor的CacheMode均设置为CACHE_MODE_NORMAL，需要经过L2 Cache，需要进入L2 Cache的总数据量为384MB。</p>
</td>
<td class="cellrowborder" valign="top" width="50.51104837924658%" headers="mcps1.1.4.1.3 "><p id="p2198131313619"><a name="p2198131313619"></a><a name="p2198131313619"></a>总数据量384MB，其中：x：128MB；y：128MB；z：128MB。</p>
<p id="p3198111373618"><a name="p3198111373618"></a><a name="p3198111373618"></a>使用48个核参与计算，按行方向切分6份，列方向切分8份。</p>
<p id="p1545392118509"><a name="p1545392118509"></a><a name="p1545392118509"></a>z对应的GlobalTensor的CacheMode设置为CACHE_MODE_NORMAL；x和y对应的GlobalTensor的CacheMode设置为CACHE_MODE_DISABLE。只将z设置为需要经过L2 Cache。需要进入L2 Cache的总数据量为128MB。</p>
</td>
</tr>
<tr id="row3293124918559"><td class="cellrowborder" valign="top" width="6.813978389954251%" headers="mcps1.1.4.1.1 "><p id="p12812993573"><a name="p12812993573"></a><a name="p12812993573"></a>示例代码</p>
</td>
<td class="cellrowborder" valign="top" width="42.67497323079919%" headers="mcps1.1.4.1.2 "><a name="screen924835613570"></a><a name="screen924835613570"></a><pre class="screen" codetype="Cpp" id="screen924835613570">xGm.SetGlobalBuffer((__gm__ half *)x + startM * totalN + startN, bufferSize);
yGm.SetGlobalBuffer((__gm__ half *)y + startM * totalN + startN, bufferSize);
zGm.SetGlobalBuffer((__gm__ half *)z + startM * totalN + startN, bufferSize);</pre>
</td>
<td class="cellrowborder" valign="top" width="50.51104837924658%" headers="mcps1.1.4.1.3 "><a name="screen271414925813"></a><a name="screen271414925813"></a><pre class="screen" codetype="Cpp" id="screen271414925813">xGm.SetGlobalBuffer((__gm__ half *)x + startM * totalN + startN, bufferSize);
yGm.SetGlobalBuffer((__gm__ half *)y + startM * totalN + startN, bufferSize);
zGm.SetGlobalBuffer((__gm__ half *)z + startM * totalN + startN, bufferSize);
// disable the L2 cache mode of x and y
xGm.SetL2CacheHint(AscendC::CacheMode::CACHE_MODE_DISABLE);
yGm.SetL2CacheHint(AscendC::CacheMode::CACHE_MODE_DISABLE);</pre>
</td>
</tr>
</tbody>
</table>

>[!NOTE]说明 
>你可以通过执行如下命令行，通过msOpProf工具获取上述示例的性能数据并进行对比。
>```
>msopprof --launch-count=2 --output=./prof ./execute_add_op
>```
>重点关注Memory.csv中的aiv\_gm\_to\_ub\_bw\(GB/s\)和aiv\_main\_mem\_write\_bw\(GB/s\)写带宽的速率。
