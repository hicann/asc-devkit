# GroupedMatmul算子性能调优案例<a name="ZH-CN_TOPIC_0000002107767061"></a>

## 案例介绍<a name="section144912211504"></a>

本案例对分组Matmul即GroupedMatmul算子的per-token量化场景进行性能分析和优化，GroupedMatmul算子计算过程（通过python代码表达）为：

```
offset = 0
for i in range(g):
    mmOut = x[offset:offset + groupList[i]] * weight[i] + bias[i]
    y[offset:offset + groupList[i]] = Gelu(mmOut * scale[i] * pertokenScale[offset:offset + groupList[i]])
    offset += groupList[i]
```

验证平台为Atlas A2 训练系列产品/Atlas A2 推理系列产品。

优化分析以如下算子规格为例：

**表1**  算子规格

<a name="table162628535334"></a>
<table><thead align="left"><tr id="row17263115333315"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="p528831925514"><a name="p528831925514"></a><a name="p528831925514"></a>input</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="p1726365314337"><a name="p1726365314337"></a><a name="p1726365314337"></a>shape</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="p4263353163314"><a name="p4263353163314"></a><a name="p4263353163314"></a>data type</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p526345316338"><a name="p526345316338"></a><a name="p526345316338"></a>format</p>
</th>
</tr>
</thead>
<tbody><tr id="row162631353183318"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p20263155316336"><a name="p20263155316336"></a><a name="p20263155316336"></a>x</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p162631531335"><a name="p162631531335"></a><a name="p162631531335"></a>(1024,1024)</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p42639536338"><a name="p42639536338"></a><a name="p42639536338"></a>int8</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p826310536339"><a name="p826310536339"></a><a name="p826310536339"></a>ND</p>
</td>
</tr>
<tr id="row826325343314"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p192636538334"><a name="p192636538334"></a><a name="p192636538334"></a>weight</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p12263353193311"><a name="p12263353193311"></a><a name="p12263353193311"></a>(8,1024,8192)</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p12263155343315"><a name="p12263155343315"></a><a name="p12263155343315"></a>int8</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p1126355314334"><a name="p1126355314334"></a><a name="p1126355314334"></a>NZ</p>
</td>
</tr>
<tr id="row202631053103315"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p1926311535334"><a name="p1926311535334"></a><a name="p1926311535334"></a>bias</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p1226325333311"><a name="p1226325333311"></a><a name="p1226325333311"></a>(8,8192)</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p526355319335"><a name="p526355319335"></a><a name="p526355319335"></a>int32</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p4264253143313"><a name="p4264253143313"></a><a name="p4264253143313"></a>ND</p>
</td>
</tr>
<tr id="row1226495323315"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p22641353183318"><a name="p22641353183318"></a><a name="p22641353183318"></a>groupList</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p92641753193315"><a name="p92641753193315"></a><a name="p92641753193315"></a>8</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p426435383317"><a name="p426435383317"></a><a name="p426435383317"></a>int64</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p122641553143317"><a name="p122641553143317"></a><a name="p122641553143317"></a>ND</p>
</td>
</tr>
<tr id="row426485373312"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p126415383311"><a name="p126415383311"></a><a name="p126415383311"></a>scale</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p0264115343312"><a name="p0264115343312"></a><a name="p0264115343312"></a>(8,8192)</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1626425318338"><a name="p1626425318338"></a><a name="p1626425318338"></a>float</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p17264205311334"><a name="p17264205311334"></a><a name="p17264205311334"></a>ND</p>
</td>
</tr>
<tr id="row1226435333318"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p162649534337"><a name="p162649534337"></a><a name="p162649534337"></a>pertokenScale</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p9264175373314"><a name="p9264175373314"></a><a name="p9264175373314"></a>1024</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p4264135312338"><a name="p4264135312338"></a><a name="p4264135312338"></a>float</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p92641153133318"><a name="p92641153133318"></a><a name="p92641153133318"></a>ND</p>
</td>
</tr>
<tr id="row796676153518"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p69667643515"><a name="p69667643515"></a><a name="p69667643515"></a>y</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p096686143510"><a name="p096686143510"></a><a name="p096686143510"></a>(1024,8192)</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p189660612351"><a name="p189660612351"></a><a name="p189660612351"></a>float16</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p109662683513"><a name="p109662683513"></a><a name="p109662683513"></a>ND</p>
</td>
</tr>
</tbody>
</table>

主要介绍以下优化方法：

-   对于Vector计算占比较高（Vector Bound）的场景，将AI Core中的AIC核和AIV核启动比例设置为1:2；
-   优化CV并行流水，减少Cube和Vector计算间的空闲等待时间；
-   优化Vector计算流水，提高Vector并行计算速度。

## 获取性能数据<a name="section4647105095111"></a>

固定8核测试，即当前性能和后续优化tiling中numBlocks固定设置为8。

通过msOpProf算子调优工具获取算子性能数据：

-   获取真实环境执行的性能数据（指令的cycle占比数据ArithmeticUtilization.csv），包含各个流水的占比情况；
-   获取仿真性能数据（指令流水图），包含各个流水的占用区间，可观察流水间依赖情况，从而优化并行效率。

## 分析主要瓶颈点<a name="section371410542511"></a>

固定8核进行测试的情况下，通过msopprof命令获取指令的cycle占比数据如下：

**图1**  指令的cycle占比数据ArithmeticUtilization.csv（性能总耗时为218.1us）<a name="fig394318012515"></a>  
![](../../figures/instr_cycle.png "指令的cycle占比数据ArithmeticUtilization-csv（性能总耗时为218-1us）")

通过msopprof simulator获取到的指令流水图如下图所示：

**图2**  指令流水图<a name="fig1566120572287"></a>  
![](../../figures/instr_pipe.png "指令流水图")

结合上述两种数据（真实数据和仿真数据）进行性能分析：

-   Vector计算bound，当前为减少核启动开销设置为1:1；
-   实际优化过程中，对上述问题进行优化、Vector计算占比下降后，Cube和Vector计算各自都有间隙，相互之间都有等待耗时；

    ![](../../figures/zh-cn_image_0000002110163600.png)

-   Vector计算没有开启double buffer，计算和数据搬运部分没有并行。

    ![](../../figures/zh-cn_image_0000002145980193.png)

## 设计优化方案<a name="section7611135813517"></a>

-   将AI Core中的AIC核和AIV核启动比例设置为1:2。每次AIC输出的数据，由两个AIV并行计算对应的反量化和激活函数；在Vector侧代码的循环里，AIV0和AIV1交替进行计算（前提条件，循环次数不为1）。代码示例如下：

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
                continue;  // AIV0和AIV1交替进行计算
            }
            if (unlikely(offsetM + vecBaseM >= curCubeSingleM)) { 
                curVecBaseM = curCubeSingleM - offsetM; 
            }
            // 使用AscendDequant接口做perchannel反量化
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

-   AIC和AIV启动比例设置为1:2后，出现Cube和Vector计算各自都有间隙、相互之间都有等待耗时的情况。分析原因是因为Vector和Cube计算存在使用一份workspace进行数据传递的场景，通过4份workspace的方案进行优化：host按4倍baseM \* baseN申请workspace，Cube侧代码在计算前可以跳过前4轮的等待。

    ```
    if ASCEND_IS_AIC {
        if (cubeCount >= tiling->parallNum) {  // tiling->parallNum设置为4
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

-   Vector计算开启double buffer，InitBuffer指定分配内存块个数为2。

    ```
    pipe->InitBuffer(scaleInQueue, 2, tiling->mmTilingData.baseN * sizeof(DTYPE_SCALE));
    pipe->InitBuffer(perTokenScaleInQueue, 2, tiling->mmTilingData.baseM * sizeof(float));
    pipe->InitBuffer(vecInQueue, 2, tiling->ubCalSize * sizeof(cT::T));
    pipe->InitBuffer(vecOutQueue, 2, tiling->ubCalSize * sizeof(DTYPE_Y));
    ```

## 验证优化方案性能收益<a name="section8934151165215"></a>

-   将AI Core中的AIC和AIV启动比例设置为1:2后，执行总耗时从218.1us下降为154.2us。指令流水图显示Cube计算间等待变小。

    ![](../../figures/zh-cn_image_0000002085555866.png)

-   如上图所示，Vector计算已经不处于bound状态，但Cube和Vector计算都有间隙，未被充分利用（上述两个箭头的位置）。分析原因如下：

    Vector计算在等Cube计算输出的数据，Cube侧需要等Vector计算完释放workspace以存放下一轮的计算结果，当前为了让Cube、Vector计算流水并行，workspace使用了两份空间：

    ![](../../figures/img_1_cn.png)

    因为Vector和Cube计算存在使用一份workspace进行数据传递的场景，存在数据依赖，所以会有等待的间隔。

    可以采用4份workspace进行优化：

    ![](../../figures/img_2_cn.png)

    优化后，总耗时由154.2us下降为131.8us。指令流水图显示Vector、Cube计算各自间隙明显减小。

    ![](../../figures/zh-cn_image_0000002085558494.png)

-   Vector计算开启double buffer，优化后执行总耗时从131.8us下降为128.1us。

    ![](../../figures/zh-cn_image_0000002121330009.png)

## 总结<a name="section15200958526"></a>

-   在Vector计算为主要瓶颈点时，将AI Core中的AIC核和AIV核启动比例设置为1:2；
-   Cube、Vector计算时间接近，且两者都有因相互等待导致的间隙时，采用4份workspace优化；
-   观察数据搬运是否与计算相互掩盖，多轮计算没有数据依赖，且buffer够大时，开启double buffer，增加并行效率。
