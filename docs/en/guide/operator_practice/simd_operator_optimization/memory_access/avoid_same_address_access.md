# Avoiding Same-Address Access<a name="ZH-CN_TOPIC_0000002338550312"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T12:38:04.999Z pushedAt=2026-11-03T07:05:13.329Z nl=2 -->

[Priority] High

>[!NOTE] Note 
>This performance optimization guide applies to the following product models:
><!-- npu="950" id1 -->
>- Ascend 950PR & 950DT products
><!-- end id1 -->
><!-- npu="A3" id2 -->
>- Atlas A3 products
><!-- end id2 -->
><!-- npu="910b" id3 -->
>- Atlas A2 products
><!-- end id3 -->

[Description] When units such as MTE2, MTE3, and Scalar access Global Memory data, their address requests are aligned to a 512-byte granularity before being processed. When data in Global Memory is accessed simultaneously and the addresses fall within a continuous 512-byte range, multiple requests are processed serially due to data consistency, which in turn degrades data transfer efficiency.

The current operator execution mechanism guarantees that the addresses of user kernel function input parameters (including Workspace/Tiling) are 512-byte aligned. Therefore, developers only need to determine, based on the address offset, whether two addresses fall within a continuous 512-byte range.

As shown in the following figure, the cores in the AI Core simultaneously issue read/write requests to Global Memory data. Although addr0\~addr5 are multiple different addresses, they are treated as the same address request because they fall within a continuous 512-byte range. In this case, these data requests are processed serially, and the data access efficiency is reduced. The impact of same-address access depends on the number of cores accessing simultaneously. The more cores that perform same-address access, the more severe the performance degradation caused by serialization.

![](../../../figures/reduce_opt.png)

The main method for avoiding same-address access is **adjusting the data access order**. For the accompanying sample described below, see optimization point 4 in [DataCopy Best Practice Sample](../../../../../../examples/01_simd_cpp_api/05_best_practices/04_memory_access/data_copy).

**Adjusting the data access order**

This section uses the scenario of transferring a half-type input with a shape of [6144, 512] from GM to Unified Buffer (UB) on an Atlas A2 products or Atlas A3 products as an example to illustrate how to avoid same-address access conflicts by adjusting the data access order. In this scenario, 48 cores participate in the transfer, that is, `numBlocks=48`.

To demonstrate the impact of same-address conflicts, L2 Cache hint is disabled in the preceding scenario. Each input row is 1024 bytes (512 half values), and the tile is \([128, 64]\). The GM-to-UB copy is completed through `DataCopyPad`. Each core loads the entire input cube once, the N direction is split into 64 columns, and the data volume of a single copy is `128 * 64 * 2` bytes. In the original implementation, all cores access the same input cube in the same mBlockIdx order, which tends to cause them to access the same GM address segment at the same time. In the optimized implementation, each core rotates the access order within the block group by \((mBlockIdx + blockIdx) \% numBlocks\), reducing the probability of same-address access conflicts.

In the following figures, M0-M47 represent the 48 blocks obtained by splitting the input cube along the M direction, and T0-T47 represent the 48 copy moments on the time axis.

<a name="table69111151111819"></a>
<table><thead align="left"><tr id="row199098514189"><th class="cellrowborder" valign="top" width="8.303319380901392%" id="mcps1.1.4.1.1"><p id="p7909145111187"><a name="p7909145111187"></a><a name="p7909145111187"></a>Implementation</p></th>
<th class="cellrowborder" valign="top" width="45.28375352866739%" id="mcps1.1.4.1.2"><p id="p18909175121819"><a name="p18909175121819"></a><a name="p18909175121819"></a>Original Implementation</p></th>
<th class="cellrowborder" valign="top" width="46.41292709043123%" id="mcps1.1.4.1.3"><p id="p109095516185"><a name="p109095516185"></a><a name="p109095516185"></a>Optimized Implementation</p></th>
</tr>
</thead>
<tbody><tr id="row16910155141810"><td class="cellrowborder" valign="top" width="8.303319380901392%" headers="mcps1.1.4.1.1 "><p id="p19101051101810"><a name="p19101051101810"></a><a name="p19101051101810"></a>Implementation Method</p></td>
<td class="cellrowborder" valign="top" width="45.28375352866739%" headers="mcps1.1.4.1.2 "><p id="p8910165111182"><a name="p8910165111182"></a><a name="p8910165111182"></a><code>offsetAddr=false</code>: all cores access the complete input cube in the same order, starting the copy from the same mBlockIdx, which tends to cause same-address access conflicts.</p></td>
<td class="cellrowborder" valign="top" width="46.41292709043123%" headers="mcps1.1.4.1.3 "><p id="p13534425386"><a name="p13534425386"></a><a name="p13534425386"></a><code>offsetAddr=true</code>: all cores access the complete input cube in a rotating order within the block group, so that different cores access different GM address segments at the same time as much as possible.</p></td>
</tr>
<tr id="row6910155119182"><td class="cellrowborder" valign="top" width="8.303319380901392%" headers="mcps1.1.4.1.1 "><p id="p691005117182"><a name="p691005117182"></a><a name="p691005117182"></a>Figure</p></td>
<td class="cellrowborder" valign="top" width="45.28375352866739%" headers="mcps1.1.4.1.2 "><p id="p469662116417"><a name="p469662116417"></a><a name="p469662116417"></a><a name="image1757423545813"></a><a name="image1757423545813"></a><span><img class="eddx" id="image1757423545813" src="../../../figures/same_addr_before.png" width="422.94" height="376.36672500000003"></span></p></td>
<td class="cellrowborder" valign="top" width="46.41292709043123%" headers="mcps1.1.4.1.3 "><p id="p2017574513412"><a name="p2017574513412"></a><a name="p2017574513412"></a><a name="image5603194513417"></a><a name="image5603194513417"></a><span><img class="eddx" id="image5603194513417" src="../../../figures/same_addr_after.png" width="422.94" height="376.36672500000003"></span></p></td>
</tr>
<tr id="row591018519184"><td class="cellrowborder" valign="top" width="8.303319380901392%" headers="mcps1.1.4.1.1 "><p id="p09108512180"><a name="p09108512180"></a><a name="p09108512180"></a>Example code</p></td>
<td class="cellrowborder" valign="top" width="45.28375352866739%" headers="mcps1.1.4.1.2 "><a name="screen179105514187"></a><a name="screen179105514187"></a><pre class="screen" codetype="Cpp" id="screen179105514187">uint32_t blockIdx = AscendC::GetBlockIdx();
for (uint32_t mBlockIdx = 0;
     mBlockIdx &lt; fullMBlockCount;
     mBlockIdx++) {
    // Original implementation: all cores use the same mBlockIdx.
    uint32_t curMBlockIdx = mBlockIdx;
    uint32_t mStart = curMBlockIdx * singleCoreM;
    DataCopyPad(...) // The copy logic is the same.

}</pre></td>
<td class="cellrowborder" valign="top" width="46.41292709043123%" headers="mcps1.1.4.1.3 "><a name="screen5910135111816"></a><a name="screen5910135111816"></a><pre class="screen" codetype="Cpp" id="screen5910135111816">uint32_t blockIdx = AscendC::GetBlockIdx();
for (uint32_t mBlockIdx = 0;
     mBlockIdx &lt; fullMBlockCount;
     mBlockIdx++) {
    // Optimized implementation: within each group of numBlocks M blocks,
    // rotate the access order by the blockIdx of the current core.
    uint32_t blockGroupStart =
        (mBlockIdx / numBlocks) * numBlocks;
    uint32_t curMBlockIdx = blockGroupStart +
        (mBlockIdx + blockIdx) % numBlocks;
    uint32_t mStart = curMBlockIdx * singleCoreM;
    DataCopyPad(...) // The copy logic is the same.
}</pre></td>
</tr>
<tr id="row591018519185"><td class="cellrowborder" valign="top" width="8.303319380901392%" headers="mcps1.1.4.1.1 "><p id="p09108512181"><a name="p09108512181"></a><a name="p09108512181"></a>Access order example</p></td>
<td class="cellrowborder" valign="top" width="45.28375352866739%" headers="mcps1.1.4.1.2 "><p id="p09108512182"><a name="p09108512182"></a><a name="p09108512182"></a>In this example, <code>numBlocks=48</code>. In the original implementation, the 48 cores access the same <code>curMBlockIdx</code> at the same time: at T0, all cores access M0; at T1, all cores access M1; at T2, all cores access M2; and so on.</p></td>
<td class="cellrowborder" valign="top" width="46.41292709043123%" headers="mcps1.1.4.1.3 "><p id="p09108512183"><a name="p09108512183"></a><a name="p09108512183"></a>In this example, <code>numBlocks=48</code>. In the optimized implementation, different cores access different <code>curMBlockIdx</code> values at the same time: at T0, <code>block0</code> accesses M0, <code>block1</code> accesses M1, <code>block46</code> accesses M46, and <code>block47</code> accesses M47; at T1, they access M1, M2, M47, and M0, respectively.</p></td>
</tr>
</tbody>
</table>

>[!NOTE] Note 
>You can run the following command to obtain the performance data of the preceding example through the msOpProf tool and compare the results.
>```
>msopprof ./demo
>```
>Focus on the MTE2 copy-related metrics in op\_summary\_\*.csv.
