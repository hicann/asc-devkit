# 关键特性说明<a id="inter_core_sync_key_features"></a>

如图1所示，本章节将配合时序图介绍CrossCoreSetFlag和CrossCoreWaitFlag配合使用时支持的四种同步控制模式各自实现的原理。

- 模式0：AI Core核间的同步控制。对于AIC全核场景，同步所有的AIC核，直到所有的AIC核都执行到CrossCoreSetFlag时，CrossCoreWaitFlag后续的指令才会执行；对于AIV全核场景，同步所有的AIV核，直到所有的AIV核都执行到CrossCoreSetFlag时，CrossCoreWaitFlag后续的指令才会执行。
- 模式1：AI Core内部，AIV核之间的同步控制。如果两个AIV核都运行了CrossCoreSetFlag，CrossCoreWaitFlag后续的指令才会执行。
- 模式2：AI Core内部，AIC与AIV之间的同步控制。在AIC核执行CrossCoreSetFlag之后，两个AIV上CrossCoreWaitFlag后续的指令才会继续执行；两个AIV都执行CrossCoreSetFlag后，AIC上CrossCoreWaitFlag后续的指令才能执行。
- 模式4：AI Core内部，AIC与单个AIV之间的同步控制。AIV0与AIV1可单独触发AIC等待。

**图1**  同步控制模式示意图<a id="sync_control_mode_diagram"></a>    
![](../../../../figures/3510_sync_control_mode_diagram.png "同步控制模式示意图")

下述同步特性均以如下场景配置为例：核函数使用`__mix__(1, 2)`修饰，即每个AI Core包含1个AIC和2个AIV，并设置逻辑核数`numBlocks=2`，即共启动2个AI Core，因此存在2个AIC和4个AIV。为便于描述，将2个AIC分别编号为AIC0、AIC1；AI Core0中的2个AIV分别编号为AIV0-0、AIV0-1，AI Core1中的2个AIV分别编号为AIV1-0、AIV1-1。

<!-- npu="A3,910b" id1 -->

> [!NOTE]说明
> 以下针对模式0、1、2的描述，仅针对[NPU架构版本2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)有效，此时`CrossCoreWaitFlag`传入的`pipe`参数无效，因此采用默认值，默认阻塞全部流水的后续指令。

## 多AI Core中AIC或者AIV全核同步（模式0）<a id="multi_ai_core_aic_aiv_full_sync"></a>

多AI Core中AIC或者AIV全核同步需要配套使用模式0的[CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md)和[CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md)接口。该场景可细分为两类：

- 2个AI Core中的2个AIC进行全核同步。

    当2个AIC都执行完CrossCoreSetFlag时，每个AIC对应flagId的计数器增加1。若AIC该flagId的计数器为非0，被CrossCoreWaitFlag所阻塞的所有指令才会执行下去，该flagId的计数器减去1。

    对于每个AIC，CrossCoreSetFlag和CrossCoreWaitFlag都必须配对使用。

- 2个AI Core中的4个AIV进行全核同步。

    当4个AIV都执行完CrossCoreSetFlag时，每个AIV核对应flagId的计数器增加1。若AIV该flagId的计数器为非0，被CrossCoreWaitFlag所阻塞的所有指令才会执行下去，该flagId的计数器减去1。

    对于每个AIV，CrossCoreSetFlag和CrossCoreWaitFlag都必须配对使用。

以图2为例，演示2个AI Core中的2个AIC（AIC0、AIC1）进行全核同步，代码片段如下：

```cpp
// 进行核间同步时，即使只有AIC参与同步也不能用__cube__修饰核函数，具体原因请参考CrossCoreSetFlag的约束说明。
if (ASCEND_IS_AIC) {
    // 每个核都应该有类似如下的成对调用CrossCoreSetFlag和CrossCoreWaitFlag。
    // modeId必须配置为0，flagId要一致；CrossCoreWaitFlag的pipe使用默认值。
    AscendC::CrossCoreSetFlag<0, PIPE_M>(0);       // 待前置PIPE_M流水任务完成后，通知调度模块。
    AscendC::CrossCoreWaitFlag<0>(0);              // 直到所有AIC都完成前置PIPE_M流水，调度模块更新flagId=0的计数器后，解除阻塞。
}
```

AIC0中在执行CrossCoreWaitFlag后，此时AIC0 flagId=0的计数器为0，后续所有指令全部被阻塞，需要等到2个AIC核均执行完CrossCoreSetFlag。

AIC1的CrossCoreSetFlag执行完后，此时调度模块感知到2个AIC均已执行完CrossCoreSetFlag，因此所有AIC各自的flagId=0的计数器值增加为1。AIC0和AIC1检测到各自对应的flagId=0的计数器变为1，都解除阻塞，继续执行后续指令，并且将计数器值减去1。

**图2**  模式0：多AI Core中AIC全核同步<a id="multi_ai_core_aic_full_sync"></a>    
![](../../../../figures/inter_core_aic_all_sync.png "多AI_Core中AIC全核同步")

## 单个AI Core中AIV全核同步（模式1）<a id="single_ai_core_aiv_full_sync"></a>

该场景需要配套使用模式1的[CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md)和[CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md)接口。

1个AI Core中的2个AIV进行全核同步。当2个AIV全部都执行完CrossCoreSetFlag时，每个AIV对应flagId的计数器增加1。若AIV该flagId的计数器为非0，被CrossCoreWaitFlag所阻塞的所有指令才会执行下去，该flagId的计数器减去1。对于每个AIV，CrossCoreSetFlag和CrossCoreWaitFlag都必须配对使用。

> [!CAUTION]注意
> 该模式中，不同AI Core的AIV核之间不会互相影响同步。

以图3为例，演示第0个AI Core中的2个AIV（AIV0-0、AIV0-1）进行全核同步，代码片段如下：

```cpp
// 进行核间同步时，即使只有AIV参与同步也不能用__vector__修饰核函数，具体原因请参考CrossCoreSetFlag的约束说明。
if (ASCEND_IS_AIV) {
    if (AscendC:: GetBlockIdx() <= 1) {
        // 参与同步的2个AIV属于第0个AI Core。
        // 每个AIV都应该有类似如下的成对调用CrossCoreSetFlag和CrossCoreWaitFlag。
        // modeId必须配置为1，flagId要一致；CrossCoreWaitFlag的pipe使用默认值。
        AscendC::CrossCoreSetFlag<1, PIPE_MTE3>(0);       // 待前置PIPE_MTE3流水任务完成后，通知调度模块。
        AscendC::CrossCoreWaitFlag<1>(0);                  // 直到该AI Core中的所有AIV都完成前置PIPE_MTE3流水，调度模块更新flagId=0的计数器后，解除阻塞。
    }
}
```

AIV0-0中在执行CrossCoreWaitFlag后，此时AIV0-0 flagId=0的计数器为0，后续所有指令全部被阻塞，需要等到2个AIV均执行完CrossCoreSetFlag。

AIV0-1的CrossCoreSetFlag执行完后，此时调度模块感知到2个AIV均已执行完CrossCoreSetFlag，因此将AIV0-0和AIV0-1各自的flagId=0的计数器值增加为1。AIV0-0和AIV0-1检测到各自对应的flagId=0的计数器变为1，都解除阻塞，继续执行后续指令，并且将计数器值减去1。

**图3**  模式1：单个AI Core中AIV全核同步<a id="single_ai_core_aiv_full_sync_diagram"></a>    
![](../../../../figures/single_core_aiv_all_sync.png "单AI_Core中AIV全核同步")

## 单个AI Core中AIC与AIV全核同步（模式2）<a id="single_ai_core_aic_aiv_full_sync"></a>

单个AI Core中AIC与AIV全核同步需要配套使用模式2的[CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md)和[CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md)接口。该场景可细分为两类：

- 1个AI Core中的AIC执行CrossCoreSetFlag，对应的2个AIV都执行CrossCoreWaitFlag。

    当AIC执行完CrossCoreSetFlag时，2个AIV对应flagId的计数器增加1。若AIV该flagId的计数器为非0，则解除AIV的阻塞，CrossCoreWaitFlag后的指令继续发射，该flagId的计数器减去1。

    总计AIC 1次调用CrossCoreSetFlag，对应2个AIV各1次CrossCoreWaitFlag才算配对使用。

- 1个AI Core中的2个AIV都执行CrossCoreSetFlag，对应的AIC执行CrossCoreWaitFlag。

    当2个AIV全都执行完CrossCoreSetFlag时，AIC对应flagId的计数器增加1。若AIC该flagId的计数器为非0，则解除AIC的阻塞，CrossCoreWaitFlag后的指令继续发射，该flagId的计数器减去1。

    总计2个AIV各1次CrossCoreSetFlag，对应AIC 1次调用CrossCoreWaitFlag才算配对使用。

以图4为例，演示第0个AI Core中AIC0与AIV0-0、AIV0-1进行同步（AIV发起CrossCoreSetFlag），代码片段如下：

```cpp
// AIC和AIV均参与核间同步，需分别使用对应宏包裹代码段，具体原因请参考CrossCoreSetFlag的约束说明。
if (ASCEND_IS_AIV) {
    if (AscendC:: GetBlockIdx() <= 1) {
        // 参与同步的AIV属于第0个AI Core。
        // 每个AIV都应该有类似如下的CrossCoreSetFlag调用。
        // modeId必须配置为2，flagId要一致。
        AscendC::CrossCoreSetFlag<2, PIPE_MTE3>(0);       // 待前置PIPE_MTE3流水任务完成后，通知调度模块。必须由所有AIV执行。
    }
}
if (ASCEND_IS_AIC) {
    if (AscendC:: GetBlockIdx() == 0) {
        // 参与同步的AIC属于第0个AI Core。
        // AIC需要有类似如下的CrossCoreWaitFlag调用。
        // CrossCoreWaitFlag的pipe使用默认值。
        AscendC::CrossCoreWaitFlag<2>(0);                 // 直到该AI Core中的所有AIV都完成前置PIPE_MTE3流水，调度模块更新flagId=0的计数器后，AIC解除阻塞。
    }
}
```

AIC0中在执行CrossCoreWaitFlag后，此时AIC0 flagId=0的计数器为0，后续所有指令全部被阻塞，需要等到2个AIV均执行完CrossCoreSetFlag。

当AIV0-1的CrossCoreSetFlag在Vector指令1执行完后，前置PIPE\_V的指令全部完成，CrossCoreSetFlag执行完成，但是此时AIV0-0未执行CrossCoreSetFlag。因此AIC0 flagId=0的计数器还是为0，AIC的所有指令依然被阻塞。

当AIV0-0的Vector指令1、2全部执行完毕后，前置PIPE\_V的指令全部完成，CrossCoreSetFlag生效。此时调度模块感知到2个AIV均已执行完CrossCoreSetFlag，因此将AIC0 flagId=0的计数器值增加为1。AIC0检测到对应的flagId=0的计数器变为1，则AIC0核解除阻塞，继续执行后续指令Matrix指令2，并且将计数器值减去1。

**图4**  模式2：单个AI Core中AIC与AIV全核同步（AIV进行CrossCoreSetFlag）<a id="single_ai_core_aic_aiv_full_sync_diagram"></a>    
![](../../../../figures/single_core_aic_aiv_sync_aiv_setflag.png "单AI_Core中AIC与AIV全核同步（AIV进行CrossCoreSetFlag）")
<!-- end id1 -->

<!-- npu="950" id2 -->
> [!NOTE]说明
> 模式4仅在[NPU架构版本3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)上支持，此时`CrossCoreWaitFlag`传入的`pipe`参数生效，阻塞由**pipe指定的流水**的后续指令。

## 单个AI Core中AIC与单个AIV同步（模式4）<a id="single_ai_core_aic_single_aiv_sync"></a>

单个AI Core中AIC与单个AIV同步需要配套使用模式4的[CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md)和[CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md)接口。该场景可细分为两类：

- 1个AI Core中的AIC执行CrossCoreSetFlag，对应的AIV执行CrossCoreWaitFlag。

    当AIC执行完CrossCoreSetFlag时，AIV对应flagId的计数器增加1。若AIV该flagId的计数器为非0，则解除AIV的阻塞，CrossCoreWaitFlag后的指令继续发射，该flagId的计数器减去1。

    总计AIC 1次调用CrossCoreSetFlag，单个AIV完成一次CrossCoreWaitFlag才算配对使用。

- 1个AI Core中的1个AIV执行CrossCoreSetFlag，对应的AIC执行CrossCoreWaitFlag。

    当AIV执行完CrossCoreSetFlag时，AIC对应flagId的计数器增加1。若AIC该flagId的计数器为非0，则解除AIC的阻塞，CrossCoreWaitFlag后的指令继续发射，该flagId的计数器减去1。

    总计单个AIV 1次CrossCoreSetFlag，对应AIC 1次调用CrossCoreWaitFlag才算配对使用。

以图5为例，演示第0个AI Core中AIV0-1与AIC0进行同步（AIV0-1发起CrossCoreSetFlag），代码片段如下：

```cpp
// AIC和AIV均参与核间同步，需分别使用对应宏包裹代码段，具体原因请参考CrossCoreSetFlag的约束说明。
if (ASCEND_IS_AIV) {
    if ( AscendC:: GetBlockIdx() == 1) {
        // modeId必须配置为4。对于flagId，AIV0-1上的0-15对应AIC0上的16-31。
        AscendC::CrossCoreSetFlag<4, PIPE_MTE3>(0);   // 待AIV0-1上前置PIPE_MTE3流水任务完成后，通知调度模块。
    }
}
if (ASCEND_IS_AIC) {
    if ( AscendC:: GetBlockIdx() == 0) {
        // modeId必须配置为4，flagId要一致；CrossCoreWaitFlag的pipe参数生效，此处阻塞PIPE_FIX流水。
        AscendC::CrossCoreWaitFlag<4, PIPE_FIX>(16);       // 阻塞后续PIPE_FIX流水指令，直到该AI Core中的AIV0-1都完成前置PIPE_MTE3流水，调度模块更新flagId=16的计数器后，AIC0解除阻塞。
    }
}
```

AIC0中在执行CrossCoreWaitFlag后，此时AIC0 flagId=16的计数器为0，后续所有指令全部被阻塞，需要等到1个AIV执行完CrossCoreSetFlag。

- AIV0-0不需要执行CrossCoreSetFlag，也不需要参与同步。
- AIV0-1的PIPE_MTE3指令全部执行完毕后，CrossCoreSetFlag生效。此时调度模块感知1个AIV已执行完CrossCoreSetFlag，因此将AIC0 flagId=16的计数器值增加为1。AIC0检测到对应的flagId=16的计数器变为1，则AIC0核解除阻塞，继续执行后续PIPE_FIX的指令，并且将计数器值减去1。

**图5**  模式4：单个AI Core中AIC与单个AIV同步（AIV进行CrossCoreSetFlag）<a id="single_ai_core_aic_single_aiv_sync_diagram"></a>    
![](../../../../figures/single_ai_core_aic_single_aiv_sync.png "单AI_Core中AIC与单个AIV同步（AIV进行CrossCoreSetFlag）")
<!-- end id2 -->
