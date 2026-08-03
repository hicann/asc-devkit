# 关键特性说明<a name="ZH-CN_TOPIC_0000002586300741"></a>

如图1所示，本章节将配合时序图介绍CrossCoreSetFlag和CrossCoreWaitFlag配合使用时支持的四种同步控制模式各自实现的原理。

- 模式0：AI Core核间的同步控制。对于AIC全核场景，同步所有的AIC核，直到所有的AIC核都执行到CrossCoreSetFlag时，CrossCoreWaitFlag后续的指令才会执行；对于AIV全核场景，同步所有的AIV核，直到所有的AIV核都执行到CrossCoreSetFlag时，CrossCoreWaitFlag后续的指令才会执行。
- 模式1：AI Core内部，AIV核之间的同步控制。如果两个AIV核都运行了CrossCoreSetFlag，CrossCoreWaitFlag后续的指令才会执行。
- 模式2：AI Core内部，AIC与AIV之间的同步控制。在AIC核执行CrossCoreSetFlag之后，两个AIV上CrossCoreWaitFlag后续的指令才会继续执行；两个AIV都执行CrossCoreSetFlag后，AIC上CrossCoreWaitFlag后续的指令才能执行。
- 模式4：AI Core内部，AIC与单个AIV之间的同步控制。AIV0与AIV1可单独触发AIC等待。

**图1**  同步控制模式示意图<a name="fig37581010773"></a>  
![](../../../../figures/3510_sync_control_mode_diagram.png "同步控制模式示意图")

下述同步特性均以该场景配置为例：每个AI Core包含1个AIC和M个AIV，共启动N个AI Core，即存在N个AIC和M \* N个AIV。

## 多AI Core中AIC或者AIV全核同步<a name="ZH-CN_TOPIC_0000002586180795"></a>

多AI Core中AIC或者AIV全核同步需要配套使用模式0的[CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md)和[CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md)接口。该场景可细分为两类：

- N个AI Core中的N个AIC进行全核同步。

    当N个AIC都执行完CrossCoreSetFlag时，每个AIC对应flagId的计数器增加1。若AIC该flagId的计数器为非0，被CrossCoreWaitFlag所阻塞的所有指令才会执行下去，该flagId的计数器减去1。

    对于每个AIC，CrossCoreSetFlag和CrossCoreWaitFlag都必须配对使用。

- N个AI Core中的M \* N个AIV进行全核同步。

    当M \* N个AIV都执行完CrossCoreSetFlag时，每个AIV核对应flagId的计数器增加1。若AIV该flagId的计数器为非0，被CrossCoreWaitFlag所阻塞的所有指令才会执行下去，该flagId的计数器减去1。

    对于每个AIV，CrossCoreSetFlag和CrossCoreWaitFlag都必须配对使用。

    ```cpp
    // 每个核都应该有类似如下的成对调用CrossCoreSetFlag和CrossCoreWaitFlag
    // modeId必须配置为0, pipe要一致，flagId要一致
    AscendC::CrossCoreSetFlag<0, PIPE_M>(0);       // 待前置PIPE_M流水任务完成后，通知调度模块
    AscendC::CrossCoreWaitFlag(0);                 // 直到所有AIC都完成前置PIPE_M流水，调度模块更新flagId=0的计数器后，解除阻塞。
    ```

以图1为例，演示N=2时，2个AI Core中的2个AIC进行全核同步。

AIC 1中在执行CrossCoreWaitFlag后，此时AIC 1 flagId=0的计数器为0，后续所有指令全部被阻塞，需要等到2个AIC核均执行完CrossCoreSetFlag。

AIC 2的CrossCoreSetFlag执行完后，此时调度模块感知到2个AIC均已执行完CrossCoreSetFlag，因此所有AIC各自的flagId=0的计数器值增加为1。AIC 1和AIC 2检测到各自对应的flagId=0的计数器变为1，都解除阻塞，继续执行后续指令，并且将计数器值减去1。

**图1**  模式0：多AI Core中AIC全核同步<a name="zh-cn_topic_0000002517901268_fig288716464319"></a>  
![](../../../../figures/inter_core_aic_all_sync.png "多AI_Core中AIC全核同步")

## 单AI Core中AIV全核同步<a name="ZH-CN_TOPIC_0000002555621176"></a>

该场景需要配套使用模式1的[CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md)和[CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md)接口。

1个AI Core中的M个AIV进行全核同步。当M个AIV全部都执行完CrossCoreSetFlag时，每个AIV对应flagId的计数器增加1。若AIV该flagId的计数器为非0，被CrossCoreWaitFlag所阻塞的所有指令才会执行下去，该flagId的计数器减去1。对于每个AIV，CrossCoreSetFlag和CrossCoreWaitFlag都必须配对使用。

>[!CAUTION]注意
>该模式中，不同AI Core的AIV核之间不会互相影响同步。

```cpp
// 每个AIV都应该有类似如下的成对调用CrossCoreSetFlag和CrossCoreWaitFlag
// modeId必须配置为1, pipe要一致，flagId要一致
AscendC::CrossCoreSetFlag<1, PIPE_MTE3>(0);       // 待前置PIPE_MTE3流水任务完成后，通知调度模块
AscendC::CrossCoreWaitFlag(0);                 // 直到该AI Core中的所有AIV都完成前置PIPE_MTE3流水，调度模块更新flagId=0的计数器后，解除阻塞。
```

以图2为例，演示M=2时，1个AI Core中的2个AIV进行全核同步。

AIV 1-1中在执行CrossCoreWaitFlag后，此时AIV 1-1 flagId=0的计数器为0，后续所有指令全部被阻塞，需要等到2个AIV均执行完CrossCoreSetFlag。

AIV 1-2的CrossCoreSetFlag执行完后，此时调度模块感知到2个AIV均已执行完CrossCoreSetFlag，因此将AIV 1-1和AIV 1-2各自的flagId=0的计数器值增加为1。AIV 1-1和AIV 1-2检测到各自对应的flagId=0的计数器变为1，都解除阻塞，继续执行后续指令，并且将计数器值减去1。

**图2**  模式1：单AI Core中AIV全核同步<a name="zh-cn_topic_0000002518061186_fig15383112914156"></a>  
![](../../../../figures/single_core_aiv_all_sync.png "单AI_Core中AIV全核同步")

## 单AI Core中AIC与AIV全核同步<a name="ZH-CN_TOPIC_0000002555780808"></a>

单AI Core中AIC与AIV全核同步需要配套使用模式2的[CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md)和[CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md)接口。该场景可细分为两类：

- 1个AI Core中的AIC执行CrossCoreSetFlag，对应的M个AIV都执行CrossCoreWaitFlag。

    当AIC执行完CrossCoreSetFlag时，M个AIV对应flagId的计数器增加1。若AIV该flagId的计数器为非0，则解除AIV的阻塞，CrossCoreWaitFlag后的指令继续发射，该flagId的计数器减去1。

    总计AIC 1次调用CrossCoreSetFlag，对应M个AIV各1次CrossCoreWaitFlag才算配对使用。

- 1个AI Core中的M个AIV都执行CrossCoreSetFlag，对应的AIC执行CrossCoreWaitFlag。

    当M个AIV全都执行完CrossCoreSetFlag时，AIC对应flagId的计数器增加1。若AIC该flagId的计数器为非0，则解除AIC的阻塞，CrossCoreWaitFlag后的指令继续发射，该flagId的计数器减去1。

    总计M个AIV各1次CrossCoreSetFlag，对应AIC 1次调用CrossCoreWaitFlag才算配对使用。

```cpp
// AIC和AIV都应该有类似如下的成对调用CrossCoreSetFlag和CrossCoreWaitFlag
// modeId必须配置为2, pipe要一致，flagId要一致
if ASCEND_IS_AIV {
    AscendC::CrossCoreSetFlag<2, PIPE_MTE3>(0);       // 待前置PIPE_MTE3流水任务完成后，通知调度模块。必须要所有AIV都执行。
}
if ASCEND_IS_AIC {
    AscendC::CrossCoreWaitFlag(0);                    // 直到该AI Core中的所有AIV都完成前置PIPE_MTE3流水，调度模块更新flagId=0的计数器后，AIC解除阻塞。
}
```

以图3为例，演示M=2时，1个AI Core中AIV与AIC进行同步（AIV发起CrossCoreSetFlag）。

AIC 1中在执行CrossCoreWaitFlag后，此时AIC 1 flagId=0的计数器为0，后续所有指令全部被阻塞，需要等到2个AIV均执行完CrossCoreSetFlag。

当AIV 1-2的CrossCoreSetFlag在Vector指令1执行完后，前置PIPE\_V的指令全部完成，CrossCoreSetFlag执行完成，但是此时AIV 1-1未执行CrossCoreSetFlag。因此AIC 1 flagId=0的计数器还是为0，AIC的所有指令依然被阻塞。

当AIV 1-1的Vector指令1、2全部执行完毕后，前置PIPE\_V的指令全部完成，CrossCoreSetFlag生效。此时调度模块感知到2个AIV均已执行完CrossCoreSetFlag，因此将AIC 1 flagId=0的计数器值增加为1。AIC 1检测到对应的flagId=0的计数器变为1，则AIC 1核解除阻塞，继续执行后续指令Matrix指令2，并且将计数器值减去1。

**图3**  模式2：单AI Core中AIC与AIV全核同步（AIV进行CrossCoreSetFlag）<a name="zh-cn_topic_0000002549581037_fig164171141103813"></a>  
![](../../../../figures/single_core_aic_aiv_sync_aiv_setflag.png "单AI_Core中AIC与AIV全核同步（AIV进行CrossCoreSetFlag）")

## 单AI Core中AIC与单个AIV同步<a name="ZH-CN_TOPIC_0000002586300742"></a>

单AI Core中AIC与单个AIV同步需要配套使用模式4的[CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md)和[CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md)接口。该场景可细分为两类：

- 1个AI Core中的AIC执行CrossCoreSetFlag，对应的AIV执行CrossCoreWaitFlag。

    当AIC执行完CrossCoreSetFlag时，AIV对应flagId的计数器增加1。若AIV该flagId的计数器为非0，则解除AIV的阻塞，CrossCoreWaitFlag后的指令继续发射，该flagId的计数器减去1。

    总计AIC 1次调用CrossCoreSetFlag，单个AIV完成一次CrossCoreWaitFlag才算配对使用。

- 1个AI Core中的1个AIV执行CrossCoreSetFlag，对应的AIC执行CrossCoreWaitFlag。

    当AIV执行完CrossCoreSetFlag时，AIC对应flagId的计数器增加1。若AIC该flagId的计数器为非0，则解除AIC的阻塞，CrossCoreWaitFlag后的指令继续发射，该flagId的计数器减去1。

    总计单个AIV 1次CrossCoreSetFlag，对应AIC 1次调用CrossCoreWaitFlag才算配对使用。

```cpp
// AIC和AIV都应该有类似如下的成对调用CrossCoreSetFlag和CrossCoreWaitFlag 
// modeId必须配置为4，flagId要一致 
if ASCEND_IS_AIV { 
    if (blockIdx % 2 == 0) { 
        AscendC::CrossCoreSetFlag<4, PIPE_MTE3>(0);   // 待AIV0上前置PIPE_MTE3流水任务完成后，通知调度器。对于flagId，AIV0上的0-15对应AIC上的0-15。 
    } 
    if (blockIdx % 2 == 1) { 
        AscendC::CrossCoreSetFlag<4, PIPE_MTE3>(0);   // 待AIV1上前置PIPE_MTE3流水任务完成后，通知调度器。对于flagId，AIV1上的0-15对应AIC上的16-31。 
    } 
} 
if ASCEND_IS_AIC { 
    AscendC::CrossCoreWaitFlag<4, PIPE_FIX>(0);        // 阻塞后续PIPE_FIX流水指令，直到该AI Core中的AIV0都完成前置PIPE_MTE3流水，调度器更新flagId=0的计数器后，AIC解除阻塞 
    ... 
    AscendC::CrossCoreWaitFlag<4, PIPE_FIX>(16);       // 阻塞后续PIPE_FIX流水指令，直到该AI Core中的AIV1都完成前置PIPE_MTE3流水，调度器更新flagId=16的计数器后，AIC解除阻塞 
    ... 
}
```

以图4为例，演示M=2时，1个AI Core中AIV1与AIC进行同步（AIV1-2发起CrossCoreSetFlag）。

AIC 1中在执行CrossCoreWaitFlag后，此时AIC 1 flagId=0的计数器为0，后续所有指令全部被阻塞，需要等到1个AIV执行完CrossCoreSetFlag。

- AIV 1-1不需要执行CrossCoreSetFlag。
- AIV 1-2的PIPE_MTE3指令全部执行完毕后，CrossCoreSetFlag生效。此时调度模块感知1个AIV已执行完CrossCoreSetFlag，因此将AIC 1 flagId=0的计数器值增加为1。AIC 1检测到对应的flagId=0的计数器变为1，则AIC 1核解除阻塞，继续执行后续PIPE_FIX的指令，并且将计数器值减去1。

**图4**  模式4：单AI Core中AIC与单个AIV同步（AIV进行CrossCoreSetFlag）<a name="zh-cn_topic_0000002549581037_fig164171141103814"></a>  
![](../../../../figures/single_ai_core_aic_single_aiv_sync.png "单AI_Core中AIC与单个AIV同步（AIV进行CrossCoreSetFlag）")
