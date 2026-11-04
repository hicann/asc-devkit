# GroupedMatmul Operator Performance Tuning Case<a name="ZH-CN_TOPIC_0000002107767061"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T11:22:00.004Z pushedAt=2026-11-03T07:05:13.267Z nl=2 -->

## Case Introduction<a name="section144912211504"></a>

This case performs performance analysis and optimization on the per-token quantization scenario of the grouped Matmul operator, that is, the GroupedMatmul operator. The computation process of the GroupedMatmul operator (expressed in Python code) is as follows:

```
offset = 0
for i in range(g):
    mmOut = x[offset:offset + groupList[i]] * weight[i] + bias[i]
    y[offset:offset + groupList[i]] = Gelu(mmOut * scale[i] * pertokenScale[offset:offset + groupList[i]])
    offset += groupList[i]
```

The verification platform is the Atlas A2 products.

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

-   For scenarios where vector computation accounts for a high proportion (Vector Bound), set the core startup ratio of AIC cores to AIV cores in the AI Core to 1:2;
-   Optimize the CV parallel pipeline to reduce the idle waiting time between Cube and Vector computations;
-   Optimize the Vector computation pipeline to improve the Vector parallel computation speed.

## Obtaining Performance Data<a name="section4647105095111"></a>

The test is performed with eight cores fixed, that is, **numBlocks** is fixed to 8 in the current performance and subsequent optimization tiling.

Obtain the operator performance data using the msOpProf operator tuning tool:

-   Obtain the performance data from real environment execution (the instruction cycle proportion data **ArithmeticUtilization.csv**), which includes the proportion of each pipeline;
-   Obtain the simulation performance data (instruction pipeline diagram), which includes the occupancy interval of each pipeline, allowing you to observe the dependencies between pipelines and thereby optimize parallel efficiency.

## Analyzing the Main Bottleneck<a name="section371410542511"></a>

When testing is performed with a fixed number of eight cores, the instruction cycle proportion data obtained through the **msopprof** command is as follows:

**Figure 1** Instruction cycle proportion data ArithmeticUtilization.csv (total performance time: 218.1us)<a name="fig394318012515"></a>  
![](../../figures/instr_cycle.png "Instruction cycle proportion data ArithmeticUtilization-csv (total performance time: 218-1us)")

The instruction pipeline diagram obtained through the msopprof simulator is shown in the following figure:

**Figure 2** Instruction pipeline diagram<a name="fig1566120572287"></a>  
![](../../figures/instr_pipe.png "Instruction pipeline diagram")

Performance analysis is performed by combining the preceding two types of data (real data and simulation data):

-   Vector computation is bound. Currently, the ratio is set to 1:1 to reduce the core startup overhead.
-   During the actual optimization process, after the preceding issues are optimized and the Vector computation proportion decreases, both Cube and Vector computations have gaps, and they wait for each other.

    ![](../../figures/zh-cn_image_0000002110163600.png)

-   Double buffer is not enabled for Vector computation, so computation and data transfer are not parallelized.

    ![](../../figures/zh-cn_image_0000002145980193.png)

## Designing an Optimization Solution<a name="section7611135813517"></a>

-   Set the startup ratio of AIC cores to AIV cores in the AI Core to 1:2. For each batch of data output by the AIC, two AIVs compute the corresponding dequantization and activation functions in parallel. In the loop of the Vector-side code, AIV0 and AIV1 perform computation alternately (provided that the loop count is not 1). The code example is as follows:

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
        CrossCoreWaitFlag<2>(SYNC_AIC_TO_AIV);
        for (uint32_t offsetM = 0; offsetM < curCubeSingleM; offsetM += vecBaseM) {
             vecCount++;
            if (vecCount % taskRation != subBlockIdx) {
                continue;  // AIV0 and AIV1 perform computation alternately
            }
            if (unlikely(offsetM + vecBaseM >= curCubeSingleM)) { 
                curVecBaseM = curCubeSingleM - offsetM; 
            }
            // Use the AscendDequant API to perform per-channel dequantization
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

-   After the startup ratio of AIC to AIV is set to 1:2, gaps appear in both Cube and Vector computations, and each side waits for the other. The reason is that Vector and Cube computations share a single workspace for data transfer in some scenarios. This is optimized by using four workspaces: the host applies for the workspace at 4 times baseM \* baseN, and the Cube-side code can skip the waiting of the first four rounds before computation.

    ```
    if ASCEND_IS_AIC {
        if (cubeCount >= tiling->parallNum) {  // tiling->parallNum is set to 4
            CrossCoreWaitFlag<2>(SYNC_AIV_TO_AIC);
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

-   Enable double buffer for vector computation, and set the number of memory blocks allocated by **InitBuffer** to **2**.

    ```
    pipe->InitBuffer(scaleInQueue, 2, tiling->mmTilingData.baseN * sizeof(DTYPE_SCALE));
    pipe->InitBuffer(perTokenScaleInQueue, 2, tiling->mmTilingData.baseM * sizeof(float));
    pipe->InitBuffer(vecInQueue, 2, tiling->ubCalSize * sizeof(cT::T));
    pipe->InitBuffer(vecOutQueue, 2, tiling->ubCalSize * sizeof(DTYPE_Y));
    ```

## Verifying the Performance Gains of the Optimization Solution<a name="section8934151165215"></a>

-   After setting the AIC-to-AIV startup ratio in the AI Core to 1:2, the total execution time dropped from 218.1us to 154.2us. The instruction pipeline diagram shows that the wait between cube computations has decreased.

    ![](../../figures/zh-cn_image_0000002085555866.png)

-   As shown in the preceding figure, vector computation is no longer in the bound state, but both cube and vector computations still have gaps and are not fully utilized (at the positions indicated by the two arrows above). The reasons are analyzed as follows:

    Vector computation is waiting for the data output by cube computation, while the cube side needs to wait for vector computation to finish and release the workspace so that it can store the next round of computation results. Currently, to run cube and vector computations in a pipelined parallel manner, the workspace uses two copies of space:

    ![](../../figures/img_1_cn.png)

    Because vector and cube computations share one copy of the workspace for data transfer in some scenarios, there is a data dependency, which causes waiting intervals.

    Four copies of the workspace can be used for optimization:

    ![](../../figures/img_2_cn.png)

    After optimization, the total time dropped from 154.2us to 131.8us. The instruction pipeline diagram shows that the gaps in vector and cube computations are significantly reduced.

    ![](../../figures/zh-cn_image_0000002085558494.png)

-   Double buffer is enabled for vector computation. After optimization, the total execution time decreases from 131.8us to 128.1us.

    ![](../../figures/zh-cn_image_0000002121330009.png)

## Summary<a name="section15200958526"></a>

-   When vector computation is the main bottleneck, set the startup ratio of AIC cores to AIV cores in the AI Core to 1:2.
-   When the cube and vector computation times are close and both have gaps caused by waiting for each other, use four workspaces for optimization.
-   Observe whether data movement is masked by computation. When multiple rounds of computation have no data dependency and the buffer is large enough, enable double buffer to improve parallel efficiency.
