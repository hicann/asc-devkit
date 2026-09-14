# Avoiding Same-Address Access<a name="ZH-CN_TOPIC_0000002338550312"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-26T14:17:29.956Z -->

[Priority] High

> [!NOTE] Note
> This performance optimization guide applies to the following products:
> - Ascend 950PR/Ascend 950DT
> - Atlas A3 training products/Atlas A3 inference  products
> - Atlas A2 training products/Atlas A2 inference products

[Description] When units such as MTE2, MTE3, and Scalar access Global Memory data, their address requests are processed after being aligned to a 512-byte granularity. When data in Global Memory is accessed simultaneously and the addresses fall within a continuous 512-byte range, multiple requests are processed serially due to data consistency, which degrades data transfer efficiency.

The current operator execution mechanism ensures that the addresses of user kernel input parameters (including Workspace/Tiling) are 512-byte aligned. Therefore, developers only need to determine whether two addresses fall within a continuous 512-byte range based on the address offset.

As shown in the following figure, the cores in an AI Core simultaneously issue read/write requests for Global Memory data. Although addr0 to addr5 are different addresses, they are treated as the same address request because they fall within a continuous 512-byte range. As a result, these data requests are processed serially, and the data access efficiency is reduced. The impact of same-address access depends on the number of cores accessing simultaneously. The more cores that access the same address, the more severe the performance degradation caused by serialization.

![](../../../figures/best_practice_optimization_reduce_usage_section.png)

The main method to avoid same-address access is to **adjust the data access order**. For the sample described below, see optimization point 4 in [DataCopy Best Practices Sample](../../../../../examples/01_simd_cpp_api/05_best_practices/04_memory_access/data_copy).

**Adjusting the Data Access Order**

This section uses the scenario of transferring a half-type input with the shape [6144, 512] from GM to UB on an Atlas A2 training series product/Atlas A2 inference series product or an Atlas A3 training series product/Atlas A3 inference series product as an example to illustrate how to avoid same-address access conflicts by adjusting the data access order. In this scenario, 48 cores participate in the transfer, that is, `numBlocks=48`.

To demonstrate the impact of same-address conflicts, L2Cache hint is disabled in the preceding scenario. Each row of the input data is 1024 bytes (512 half values), the Tile is \([128, 64]\), and `DataCopyPad` is used to move data from GM to UB. Each core loads the entire input matrix once, the N dimension is split into 64 columns, and the amount of data moved in a single operation is `128 * 64 * 2` bytes. In the original implementation, all cores access the same input matrix in the same mBlockIdx order, which makes them likely to access the same GM address segment at the same time. In the optimized implementation, each core rotates the access order within the block group according to \((mBlockIdx + blockIdx) \% numBlocks\), reducing the probability of same-address access conflicts.

In the following diagrams, M0-M47 represent the 48 blocks obtained by splitting the input matrix along the M dimension, and T0-T47 represent the 48 move moments on the time axis.

<a name="table69111151111819"></a>
<table><thead align="left"><tr id="row199098514189"><th class="cellrowborder" valign="top" width="8.303319380901392%" id="mcps1.1.4.1.1"><p id="p7909145111187"><a name="p7909145111187"></a><a name="p7909145111187"></a>Implementation</p></th>
<th class="cellrowborder" valign="top" width="45.28375352866739%" id="mcps1.1.4.1.2"><p id="p18909175121819"><a name="p18909175121819"></a><a name="p18909175121819"></a>Original implementation</p></th>
<th class="cellrowborder" valign="top" width="46.41292709043123%" id="mcps1.1.4.1.3"><p id="p109095516185"><a name="p109095516185"></a><a name="p109095516185"></a>Optimized implementation</p></th>
</tr>
</thead>
<tbody><tr id="row16910155141810"><td class="cellrowborder" valign="top" width="8.303319380901392%" headers="mcps1.1.4.1.1 "><p id="p19101051101810"><a name="p19101051101810"></a><a name="p19101051101810"></a>Implementation method</p></td>
<td class="cellrowborder" valign="top" width="45.28375352866739%" headers="mcps1.1.4.1.2 "><p id="p8910165111182"><a name="p8910165111182"></a><a name="p8910165111182"></a><code>offsetAddr=false</code>: all cores access the complete input matrix in the same order, starting the move from the same mBlockIdx, which is prone to same-address access conflicts.</p></td>
<td class="cellrowborder" valign="top" width="46.41292709043123%" headers="mcps1.1.4.1.3 "><p id="p13534425386"><a name="p13534425386"></a><a name="p13534425386"></a><code>offsetAddr=true</code>: all cores access the complete input matrix in a rotated order within the block group, so that different cores access different GM address segments at the same time as much as possible.</p></td>
</tr>
<tr id="row6910155119182"><td class="cellrowborder" valign="top" width="8.303319380901392%" headers="mcps1.1.4.1.1 "><p id="p691005117182"><a name="p691005117182"></a><a name="p691005117182"></a>Diagram</p></td>
<td class="cellrowborder" valign="top" width="45.28375352866739%" headers="mcps1.1.4.1.2 "><p id="p469662116417"><a name="p469662116417"></a><a name="p469662116417"></a><a name="image1757423545813"></a><a name="image1757423545813"></a><span><img class="eddx" id="image1757423545813" src="../../../figures/avoid_same_address_access_before_optimization.png" width="422.94" height="376.36672500000003"></span></p></td>
<td class="cellrowborder" valign="top" width="46.41292709043123%" headers="mcps1.1.4.1.3 "><p id="p2017574513412"><a name="p2017574513412"></a><a name="p2017574513412"></a><a name="image5603194513417"></a><a name="image5603194513417"></a><span><img class="eddx" id="image5603194513417" src="../../../figures/avoid_same_address_access_after_optimization.png" width="422.94" height="376.36672500000003"></span></p></td>
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
    // rotate the access order based on the blockIdx of the current core.
    uint32_t blockGroupStart =
        (mBlockIdx / numBlocks) * numBlocks;
    uint32_t curMBlockIdx = blockGroupStart +
        (mBlockIdx + blockIdx) % numBlocks;
    uint32_t mStart = curMBlockIdx * singleCoreM;
    DataCopyPad(...) // The copy logic is the same.
}</pre></td>
</tr>
<tr id="row591018519185"><td class="cellrowborder" valign="top" width="8.303319380901392%" headers="mcps1.1.4.1.1 "><p id="p09108512181"><a name="p09108512181"></a><a name="p09108512181"></a>Access order example</p></td>
<td class="cellrowborder" valign="top" width="45.28375352866739%" headers="mcps1.1.4.1.2 "><p id="p09108512182"><a name="p09108512182"></a><a name="p09108512182"></a>In this example, <code>numBlocks=48</code>. In the original implementation, the 48 cores access the same <code>curMBlockIdx</code> at the same time: at T0 they all access M0, at T1 they all access M1, at T2 they all access M2, and so on.</p></td>
<td class="cellrowborder" valign="top" width="46.41292709043123%" headers="mcps1.1.4.1.3 "><p id="p09108512183"><a name="p09108512183"></a><a name="p09108512183"></a>In this example, <code>numBlocks=48</code>. In the optimized implementation, different cores access different <code>curMBlockIdx</code> values at the same time: at T0, <code>block0</code> accesses M0, <code>block1</code> accesses M1, <code>block46</code> accesses M46, and <code>block47</code> accesses M47; at T1, they access M1, M2, M47, and M0, respectively.</p></td>
</tr>
</tbody>
</table>

>[!NOTE] Note
>You can run the following command to obtain the performance data of the preceding example using the msOpProf tool and compare the results.
>```
>msopprof ./demo
>```
>Focus on the MTE2 copy-related metrics in PipeUtilization*.csv.
