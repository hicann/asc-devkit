# GroupedMatmul Operator Performance Tuning Case<a name="ZH-CN_TOPIC_0000002107767061"></a>

<!-- md-trans-meta sourceCommit=e4a5905334db2bef02e668063a0b99d64a2e7709 translatedAt=2026-08-26T13:04:53.984Z -->

## Case Introduction<a name="section144912211504"></a>

This case performs performance analysis and optimization on the per-token quantization scenario of grouped MatMul, that is, the GroupedMatmul operator. The computation process of the GroupedMatmul operator (expressed in Python code) is as follows:

```
offset = 0
for i in range(g):
    mmOut = x[offset:offset + groupList[i]] * weight[i] + bias[i]
    y[offset:offset + groupList[i]] = Gelu(mmOut * scale[i] * pertokenScale[offset:offset + groupList[i]])
    offset += groupList[i]
```

The verification platform is Atlas A2 training products/Atlas A2 inference products.

The optimization analysis uses the following operator specifications as an example:

**Table 1** Operator specifications

<a name="table162628535334"></a>
<table><thead align="left"><tr id="row17263115333315"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="p528831925514"><a name="p528831925514"></a><a name="p528831925514"></a>Input</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="p1726365314337"><a name="p1726365314337"></a><a name="p1726365314337"></a>Shape</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="p4263353163314"><a name="p4263353163314"></a><a name="p4263353163314"></a>Data Type</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p526345316338"><a name="p526345316338"></a><a name="p526345316338"></a>Format</p></th>
</tr>
</thead>
<tbody><tr id="row162631353183318"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p20263155316336"><a name="p20263155316336"></a><a name="p20263155316336"></a>x</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p162631531335"><a name="p162631531335"></a><a name="p162631531335"></a>(1024,1024)</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p42639536338"><a name="p42639536338"></a><a name="p42639536338"></a>int8</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p826310536339"><a name="p826310536339"></a><a name="p826310536339"></a>ND</p></td>
</tr>
<tr id="row826325343314"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p192636538334"><a name="p192636538334"></a><a name="p192636538334"></a>weight</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p12263353193311"><a name="p12263353193311"></a><a name="p12263353193311"></a>(8,1024,8192)</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p12263155343315"><a name="p12263155343315"></a><a name="p12263155343315"></a>int8</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p1126355314334"><a name="p1126355314334"></a><a name="p1126355314334"></a>NZ</p></td>
</tr>
<tr id="row202631053103315"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p1926311535334"><a name="p1926311535334"></a><a name="p1926311535334"></a>bias</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p1226325333311"><a name="p1226325333311"></a><a name="p1226325333311"></a>(8,8192)</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p526355319335"><a name="p526355319335"></a><a name="p526355319335"></a>int32</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p4264253143313"><a name="p4264253143313"></a><a name="p4264253143313"></a>ND</p></td>
</tr>
<tr id="row1226495323315"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p22641353183318"><a name="p22641353183318"></a><a name="p22641353183318"></a>groupList</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p92641753193315"><a name="p92641753193315"></a><a name="p92641753193315"></a>8</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p426435383317"><a name="p426435383317"></a><a name="p426435383317"></a>int64</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p122641553143317"><a name="p122641553143317"></a><a name="p122641553143317"></a>ND</p></td>
</tr>
<tr id="row426485373312"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p126415383311"><a name="p126415383311"></a><a name="p126415383311"></a>scale</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p0264115343312"><a name="p0264115343312"></a><a name="p0264115343312"></a>(8,8192)</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1626425318338"><a name="p1626425318338"></a><a name="p1626425318338"></a>float</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p17264205311334"><a name="p17264205311334"></a><a name="p17264205311334"></a>ND</p></td>
</tr>
<tr id="row1226435333318"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p162649534337"><a name="p162649534337"></a><a name="p162649534337"></a>pertokenScale</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p9264175373314"><a name="p9264175373314"></a><a name="p9264175373314"></a>1024</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p4264135312338"><a name="p4264135312338"></a><a name="p4264135312338"></a>float</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p92641153133318"><a name="p92641153133318"></a><a name="p92641153133318"></a>ND</p></td>
</tr>
<tr id="row796676153518"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p69667643515"><a name="p69667643515"></a><a name="p69667643515"></a>y</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p096686143510"><a name="p096686143510"></a><a name="p096686143510"></a>(1024,8192)</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p189660612351"><a name="p189660612351"></a><a name="p189660612351"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p109662683513"><a name="p109662683513"></a><a name="p109662683513"></a>ND</p></td>
</tr>
</tbody>
</table>

The following optimization methods are mainly introduced:

-   For scenarios where Vector computation accounts for a high proportion (Vector Bound), set the kernel launch ratio of AIC cores to AIV cores in the AI Core to 1:2;
-   Optimize the CV parallel pipeline to reduce idle waiting time between Cube and Vector computation.
-   Optimize the Vector computation pipeline to improve the Vector parallel computation speed.

## Obtaining Performance Data<a name="section4647105095111"></a>

The test is performed with a fixed number of 8 cores, that is, numBlocks is fixed to 8 in both the current performance test and the subsequent optimized tiling.

Obtain the operator performance data using the msOpProf operator tuning tool:

-   Obtain the performance data from the real environment (the instruction cycle proportion data ArithmeticUtilization.csv), which includes the proportion of each pipeline;
-   Obtain the simulation performance data (the instruction pipeline diagram), which includes the occupancy interval of each pipeline and allows you to observe the dependencies between pipelines, thereby optimizing the parallel efficiency.

## Analyzing the Main Bottleneck<a name="section371410542511"></a>

With a fixed 8-core test configuration, the instruction cycle proportion data obtained through the msopprof command is as follows:

**Figure 1** Instruction cycle proportion data ArithmeticUtilization.csv (total performance time is 218.1us)<a name="fig394318012515"></a>  
![](../../figures/instruction_cycle_proportion_data_arithmeticutilization_csv_(total_performance_consumption_is_218_1us).png "Instruction cycle proportion data ArithmeticUtilization.csv (total performance time is 218-1us)")

The instruction pipeline diagram obtained through the msopprof simulator is shown below:

**Figure 2** Instruction pipeline diagram<a name="fig1566120572287"></a>  
![](../../figures/instruction_pipeline_diagram.png "Instruction pipeline diagram")

Performance analysis is performed by combining the two types of data above (real data and simulation data):

-   Vector computation is bound. The kernel launch ratio is currently set to 1:1 to reduce kernel launch overhead;
-   During the actual optimization process, after the above issues are optimized and the Vector computation proportion decreases, both Cube and Vector computations have their own gaps and wait for each other;

    ![](../../figures/zh-cn_image_0000002110163600.png)

-   Double buffer is not enabled for Vector computation, so computation and data transfer are not parallelized.

    ![](../../figures/zh-cn_image_0000002145980193.png)

## Designing an Optimization Solution<a name="section7611135813517"></a>

-   Set the kernel launch ratio of AIC cores to AIV cores in the AI Core to 1:2. For each batch of data output by AIC, two AIVs compute the corresponding dequantization and activation function in parallel. In the loop of the Vector-side code, AIV0 and AIV1 compute alternately (provided that the loop count is not 1). The code example is as follows:

    ```
    uint32_t vecCount = 0;
    uint32_t taskRation = GetTaskRatio();
    for (uint32_t offsetN = 0; offsetN < curCubeSingleN; offsetN += mnConfig.baseN) {
        if (unlikely(offsetN + mnConfig.baseN >= curCubeSingleN)) {
            curVecBaseN = curCubeSingleN - offsetN;
        }
        uint32_t alignBaseN = Ceil(curVecBaseN, uint32_t(8)) * 8;
        DataCopyScale(curVecBaseN, alignBaseN, scaleOffset + offsetN);
        uint32_t curVecBaseM = vecBaseM;
        uint64_t mmOutOffset = mnConfig.workSpaceOffset + offsetN * mnConfig.baseM;
        CrossCoreWaitFlag(SYNC_AIC_TO_AIV);
        for (uint32_t offsetM = 0; offsetM < curCubeSingleM; offsetM += vecBaseM) {
             vecCount++;
            if (vecCount % taskRation != subBlockIdx) {
                continue;  // AIV0 and AIV1 compute alternately.
            }
            if (unlikely(offsetM + vecBaseM >= curCubeSingleM)) { 
                curVecBaseM = curCubeSingleM - offsetM; 
            }
            // Use the AscendDequant API to perform per-channel dequantization.
            LocalTensor<cT::T> mmOutLocal = vecInQueue.AllocTensor<cT::T>();
            DataCopyPad2D(mmOutLocal, mmOutGm[mmOutOffset + offsetM * curVecBaseN],
                          curVecBaseM, curVecBaseN, curVecBaseN);
            vecInQueue.EnQue(mmOutLocal);
            ComputeDequantAndActivate(mnConfig, curVecBaseM, alignBaseN, curVecBaseN, offsetM);
            LocalTensor<DTYPE_Y> yLocal = vecOutQueue.DeQue<DTYPE_Y>();
            DataCopyPad2D(yGm[outOffset + offsetM * tiling->n + offsetN], yLocal,
                          curVecBaseM, curVecBaseN, alignBaseN, tiling->n);
            vecOutQueue.FreeTensor(yLocal);
        }
        ...
    }
    ```

-   After the kernel launch ratio of AIC to AIV is set to 1:2, gaps appear in both Cube and Vector computations, and each side waits for the other. Analysis shows that this is because Vector and Cube computations share a single workspace for data transfer. This is optimized by using four workspaces: the host applies for a workspace of 4 times baseM \* baseN, and the Cube-side code can skip the waiting of the first four rounds before computation.

    ```
    if ASCEND_IS_AIC {
        if (cubeCount >= tiling->parallNum) {  // Set tiling->parallNum to 4.
            CrossCoreWaitFlag(SYNC_AIV_TO_AIC);
        }
        mm.SetOrgShape(mnConfig.m, tiling->n, tiling->k);
        mm.SetSingleShape(curSingleM, curSingleN, tiling->k);
        mm.SetTensorA(xGm[xOffset]);
        auto weightSlice = weightGm[weightOffset];
        if (mnConfig.numBlocksM == 1) {
            weightSlice.SetL2CacheHint(CacheMode::CACHE_MODE_DISABLE);
        }
        mm.SetTensorB(weightSlice);
        uint64_t workspaceOffset = mnConfig.workSpaceOffset;
        while (mm.Iterate()) {
            mm.GetTensorC(mmOutGm[workspaceOffset], 0, true);
            CrossCoreSetFlag<2, PIPE_FIX>(SYNC_AIC_TO_AIV);
            workspaceOffset += (mnConfig.baseM * mnConfig.baseN);
        }
    }
    cubeCount++;
    ```

-   Enable double buffer for Vector computation, and set the number of allocated memory blocks to 2 in InitBuffer.

    ```
    pipe->InitBuffer(scaleInQueue, 2, tiling->mmTilingData.baseN * sizeof(DTYPE_SCALE));
    pipe->InitBuffer(perTokenScaleInQueue, 2, tiling->mmTilingData.baseM * sizeof(float));
    pipe->InitBuffer(vecInQueue, 2, tiling->ubCalSize * sizeof(cT::T));
    pipe->InitBuffer(vecOutQueue, 2, tiling->ubCalSize * sizeof(DTYPE_Y));
    ```

## Verifying the Performance Gains of the Optimization Solution<a name="section8934151165215"></a>

-   After the AIC-to-AIV kernel launch ratio in the AI Core is set to 1:2, the total execution time decreases from 218.1 us to 154.2 us. The instruction pipeline diagram shows that the wait between Cube computations is reduced.

    ![](../../figures/zh-cn_image_0000002085555866.png)

-   As shown in the preceding figure, Vector computation is no longer in the bound state, but both Cube and Vector computations have gaps and are not fully utilized (at the positions of the two arrows above). The reasons are analyzed as follows:

    Vector computation waits for the data output by Cube computation, while the Cube side needs to wait for Vector computation to finish and release the workspace to store the computation results of the next round. Currently, to run Cube and Vector computations in a parallel pipeline, two copies of workspace are used:

    ![](../../figures/1_zh-cn_image_0000002085556446.png)

    Because Vector and Cube computations share one copy of workspace for data transfer in some scenarios, data dependency exists, resulting in wait intervals.

    Four copies of workspace can be used for optimization:

    ![](../../figures/2_zh-cn_image_0000002121156181.png)

    After optimization, the total execution time decreases from 154.2 us to 131.8 us. The instruction pipeline diagram shows that the gaps in Vector and Cube computations are significantly reduced.

    ![](../../figures/zh-cn_image_0000002085558494.png)

-   After double buffer is enabled for Vector computation, the total execution time decreases from 131.8 us to 128.1 us.

    ![](../../figures/zh-cn_image_0000002121330009.png)

## Summary<a name="section15200958526"></a>

-   When Vector computation is the main bottleneck, set the launch ratio of AIC cores to AIV cores in the AI Core to 1:2;
-   When the Cube and Vector computation times are close and both have gaps caused by waiting for each other, use four workspaces for optimization;
-   Observe whether data transfer overlaps with computation. When multiple rounds of computation have no data dependency and the buffer is large enough, enable double buffer to improve parallel efficiency.
