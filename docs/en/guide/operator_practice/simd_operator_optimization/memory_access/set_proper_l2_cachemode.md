# Setting a Proper L2 CacheMode<a name="ZH-CN_TOPIC_0000002356424662"></a>

<!-- md-trans-meta sourceCommit=e4a5905334db2bef02e668063a0b99d64a2e7709 translatedAt=2026-08-26T14:21:28.012Z -->

[Priority] High

> [!NOTE] Note
> This performance optimization guide applies to the following product models:
> - Ascend 950PR/Ascend 950DT
> - Atlas A3 training products/A3 inference products
> - Atlas A2 training products/A2 inference products

[Description] The L2 Cache is commonly used to cache frequently accessed data. Its physical location is shown in the following figure:

![](../../../figures/best_practice_optimization_reduce_usage_section_61.png)

The bandwidth of the L2 Cache is several times higher than that of the GM. Therefore, when data hits the L2 Cache, the data transfer time is optimized by several times. Generally, the higher the L2 Cache hit rate, the better the operator performance. In actual access, a proper L2 CacheMode must be set to ensure that repeatedly read data is cached in the L2 Cache as much as possible.

## L2 Cache Access Principles and CacheMode<a name="section75591806575"></a>

When data is moved in through the MTE2 unit, the typical L2 Cache access process is as follows:

![](../../../figures/best_practice_optimization_reduce_usage_section_62.png)

When data is moved out through the MTE3 or Fixpipe unit, the typical L2 Cache access process is as follows:

![](../../../figures/best_practice_optimization_reduce_usage_section_63.png)

As shown in the preceding processes, when the total amount of accessed data exceeds the L2 Cache capacity, the AI Core replaces data in the L2 Cache. Due to the cache coherence requirement, the old data must be written back to the GM first during replacement (which occupies GM bandwidth). Only after the old data is written back can new data enter the L2 Cache.

Developers can set the CacheMode for the data to be accessed. For Global Memory data that is accessed only once, set its access state to not enter the L2 Cache. In this way, the L2 Cache can be used more efficiently to cache data that needs to be read repeatedly, preventing one-time accessed data from replacing valid data.

## Setting the L2 CacheMode<a name="section058817270210"></a>

Ascend C provides the SetL2CacheHint API based on GlobalTensor, allowing users to specify the CacheMode as needed.

Consider the following scenario: construct an addition operation of two tensors, where the shapes of inputs x and y and output z are all \(8192, 8192\). For the complete sample, see [Setting a Reasonable L2 CacheMode Sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/05_best_practices/00_vector_compute/add_high_performance).

<a name="table12921549195512"></a>
<table><thead align="left"><tr id="row1229364945511"><th class="cellrowborder" valign="top" width="6.813978389954251%" id="mcps1.1.4.1.1"><p id="p2081249145715"><a name="p2081249145715"></a><a name="p2081249145715"></a>Implementation</p></th>
<th class="cellrowborder" valign="top" width="42.67497323079919%" id="mcps1.1.4.1.2"><p id="p2029374985519"><a name="p2029374985519"></a><a name="p2029374985519"></a>Original implementation</p></th>
<th class="cellrowborder" valign="top" width="50.51104837924658%" id="mcps1.1.4.1.3"><p id="p152931149115516"><a name="p152931149115516"></a><a name="p152931149115516"></a>Optimized implementation</p></th>
</tr>
</thead>
<tbody><tr id="row1629374995517"><td class="cellrowborder" valign="top" width="6.813978389954251%" headers="mcps1.1.4.1.1 "><p id="p108124995710"><a name="p108124995710"></a><a name="p108124995710"></a>Implementation method</p></td>
<td class="cellrowborder" valign="top" width="42.67497323079919%" headers="mcps1.1.4.1.2 "><p id="p1764591574211"><a name="p1764591574211"></a><a name="p1764591574211"></a>The total data volume is 384 MB, where x: 128 MB; y: 128 MB; z: 128 MB.</p>
<p id="p13628115612406"><a name="p13628115612406"></a><a name="p13628115612406"></a>48 cores are used for computation, with the data split into 6 parts along the row direction and 8 parts along the column direction.</p>
<p id="p5900191641514"><a name="p5900191641514"></a><a name="p5900191641514"></a>The CacheMode of the GlobalTensors corresponding to x, y, and z is set to CACHE_MODE_NORMAL, which requires passing through the L2 Cache. The total data volume that needs to enter the L2 Cache is 384 MB.</p></td>
<td class="cellrowborder" valign="top" width="50.51104837924658%" headers="mcps1.1.4.1.3 "><p id="p2198131313619"><a name="p2198131313619"></a><a name="p2198131313619"></a>The total data volume is 384 MB, where x: 128 MB; y: 128 MB; z: 128 MB.</p>
<p id="p3198111373618"><a name="p3198111373618"></a><a name="p3198111373618"></a>48 cores are used for computation, with the data split into 6 parts along the row direction and 8 parts along the column direction.</p>
<p id="p1545392118509"><a name="p1545392118509"></a><a name="p1545392118509"></a>The CacheMode of the GlobalTensor corresponding to z is set to CACHE_MODE_NORMAL; the CacheMode of the GlobalTensors corresponding to x and y is set to CACHE_MODE_DISABLE. Only z is set to pass through the L2 Cache. The total data volume that needs to enter the L2 Cache is 128 MB.</p></td>
</tr>
<tr id="row3293124918559"><td class="cellrowborder" valign="top" width="6.813978389954251%" headers="mcps1.1.4.1.1 "><p id="p12812993573"><a name="p12812993573"></a><a name="p12812993573"></a>Example code</p></td>
<td class="cellrowborder" valign="top" width="42.67497323079919%" headers="mcps1.1.4.1.2 "><a name="screen924835613570"></a><a name="screen924835613570"></a><pre class="screen" codetype="Cpp" id="screen924835613570">xGm.SetGlobalBuffer((__gm__ half *)x + startM * totalN + startN, bufferSize);
yGm.SetGlobalBuffer((__gm__ half *)y + startM * totalN + startN, bufferSize);
zGm.SetGlobalBuffer((__gm__ half *)z + startM * totalN + startN, bufferSize);</pre></td>
<td class="cellrowborder" valign="top" width="50.51104837924658%" headers="mcps1.1.4.1.3 "><a name="screen271414925813"></a><a name="screen271414925813"></a><pre class="screen" codetype="Cpp" id="screen271414925813">xGm.SetGlobalBuffer((__gm__ half *)x + startM * totalN + startN, bufferSize);
yGm.SetGlobalBuffer((__gm__ half *)y + startM * totalN + startN, bufferSize);
zGm.SetGlobalBuffer((__gm__ half *)z + startM * totalN + startN, bufferSize);
// disable the L2 cache mode of x and y
xGm.SetL2CacheHint(AscendC::CacheMode::CACHE_MODE_DISABLE);
yGm.SetL2CacheHint(AscendC::CacheMode::CACHE_MODE_DISABLE);</pre></td>
</tr>
</tbody>
</table>

>[!NOTE] Note
>You can run the following command to obtain the performance data of the preceding example through the msOpProf tool for comparison.
>```
>msopprof --launch-count=2 --output=./prof ./execute_add_op
>```
>Focus on the write bandwidth rates of aiv\_gm\_to\_ub\_bw\(GB/s\) and aiv\_main\_mem\_write\_bw\(GB/s\) in Memory.csv.
