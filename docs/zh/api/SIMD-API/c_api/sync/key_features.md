# 关键特性说明<a id="inter_core_sync_key_features"></a>

如图1所示，本章节将配合时序图介绍四对核间同步接口支持的四种同步控制模式各自实现的原理。

- 模式0：AI Core核间的同步控制。对于AIC全核场景，同步所有的AIC核，直到所有的AIC核都执行到`asc_sync_inter_arrive`时，`asc_sync_inter_wait`后续的全部流水或者由`pipe`参数指定的流水（与NPU架构有关）中的指令才会执行；对于AIV全核场景，同步所有的AIV核，直到所有的AIV核都执行到`asc_sync_inter_arrive`时，`asc_sync_inter_wait`后续的全部流水或者由`pipe`参数指定的流水（与NPU架构有关）中的指令才会执行。
- 模式1：AI Core内部，AIV核之间的同步控制。如果两个AIV核都运行了`asc_sync_subblock_arrive`，`asc_sync_subblock_wait`后续的全部流水或者由`pipe`参数指定的流水（与NPU架构有关）中的指令才会执行。
- 模式2：AI Core内部，AIC与AIV之间的同步控制。在AIC核执行`asc_sync_block_arrive`之后，两个AIV上`asc_sync_block_wait`后续的全部流水或者由`pipe`参数指定的流水（与NPU架构有关）中的指令才会继续执行；两个AIV都执行`asc_sync_block_arrive`后，AIC上`asc_sync_block_wait`后续的全部流水或者由`pipe`参数指定的流水（与NPU架构有关）中的指令才能执行。
- 模式4：AI Core内部，AIC与单个AIV之间的同步控制。在AIC核执行`asc_sync_intra_arrive`之后，单个AIV上`asc_sync_intra_wait`后续的由`pipe`参数指定的流水中的指令才会继续执行；单个AIV执行`asc_sync_intra_arrive`后，AIC上`asc_sync_intra_wait`后续的由`pipe`参数指定的流水中的指令才能执行。该模式仅在[NPU架构版本3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)上支持。

> [!NOTE]说明
> `asc_sync_inter_wait`、`asc_sync_subblock_wait`、`asc_sync_block_wait`接口阻塞的流水类型因NPU架构而异：
> - 在[NPU架构版本2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)上，上述接口传入的`pipe`参数无效，无论取何值都阻塞全部流水的后续指令。
> - 在[NPU架构版本3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)上，上述接口传入的`pipe`参数生效，阻塞由`pipe`参数指定的流水的后续指令。

**图1**  同步控制模式示意图<a id="sync_control_mode_diagram"></a>    
![](../../../figures/3510_sync_control_mode_diagram.png "同步控制模式示意图")

下述同步特性均以如下场景配置为例：核函数使用`__mix__(1, 2)`修饰，即每个AI Core包含1个AIC和2个AIV，并设置逻辑核数`numBlocks=2`，即共启动2个AI Core，因此存在2个AIC和4个AIV。为便于描述，将2个AIC分别编号为AIC0、AIC1；AI Core0中的2个AIV分别编号为AIV0-0、AIV0-1，AI Core1中的2个AIV分别编号为AIV1-0、AIV1-1。**各核中与`flag_id`或`sync_id`对应的计数器初始值均为0。**

模式0、模式1和模式2在NPU架构版本2201和3510上均支持。以下各节的同步流程对两种架构均适用，架构差异仅体现在`wait`接口传入的`pipe`参数是否生效以及阻塞的流水范围上，具体以各节的说明为准。

<!-- npu="950" id1 -->

> [!NOTE]说明
> 以下针对模式0的描述，仅针对[NPU架构版本3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)有效。调用相关接口时必须显式传入`pipe`参数，`arrive`和`wait`接口传入的`pipe`参数均生效，`wait`接口会阻塞由`pipe`参数指定的流水的后续指令。

## 多AI Core中AIC或者AIV全核同步（模式0）<a id="multi_ai_core_aic_aiv_full_sync"></a>

多AI Core中AIC或者AIV全核同步需要配套使用模式0的[asc_sync_inter_arrive](asc_sync_inter_arrive.md)和[asc_sync_inter_wait](asc_sync_inter_wait.md)接口。该场景可细分为两类：

- 2个AI Core中的2个AIC进行全核同步。

    当2个AIC都执行完`asc_sync_inter_arrive`时，每个AIC对应`flag_id`的计数器增加1。若AIC该`flag_id`的计数器为非0，被`asc_sync_inter_wait`所阻塞的指定流水中的指令才会执行下去，该`flag_id`的计数器减去1。

    对于每个AIC，`asc_sync_inter_arrive`和`asc_sync_inter_wait`都必须配对使用。

- 2个AI Core中的4个AIV进行全核同步。

    当4个AIV都执行完`asc_sync_inter_arrive`时，每个AIV核对应`flag_id`的计数器增加1。若AIV该`flag_id`的计数器为非0，被`asc_sync_inter_wait`所阻塞的指定流水中的指令才会执行下去，该`flag_id`的计数器减去1。

    对于每个AIV，`asc_sync_inter_arrive`和`asc_sync_inter_wait`都必须配对使用。

以图2为例，演示2个AI Core中的2个AIC（AIC0、AIC1）进行全核同步，代码片段如下：

```cpp
// 进行核间同步时，即使只有AIC参与同步也不能用__cube__修饰核函数，具体原因请参考asc_sync_inter_arrive的约束说明。
if ASC_IS_AIC {
    // 每个核都应该有类似如下的成对调用asc_sync_inter_arrive和asc_sync_inter_wait。
    // 两个接口的flag_id必须一致；两个接口都必须显式传入pipe，asc_sync_inter_wait会阻塞pipe参数指定的流水。
    asc_sync_inter_arrive(PIPE_M, 0);  // 待前置PIPE_M流水任务完成后，通知调度模块。
    asc_sync_inter_wait(PIPE_FIX, 0);  // 直到所有AIC都完成前置PIPE_M流水，调度模块更新flag_id=0的计数器后，解除PIPE_FIX流水后续指令的阻塞。
}
```

AIC0中在执行`asc_sync_inter_wait`后，此时AIC0 `flag_id=0`的计数器为0，后续PIPE_FIX流水中的指令被阻塞，需要等到2个AIC核均执行完`asc_sync_inter_arrive`。

AIC1的`asc_sync_inter_arrive`执行完后，此时调度模块感知到2个AIC均已执行完`asc_sync_inter_arrive`，因此所有AIC各自的`flag_id=0`的计数器值增加为1。AIC0和AIC1检测到各自对应的`flag_id=0`的计数器变为1，都解除PIPE_FIX流水后续指令的阻塞，继续执行后续指令，并且将计数器值减去1。

**图2**  模式0：多AI Core中AIC全核同步<a id="multi_ai_core_aic_full_sync"></a>    
![](../figures/inter_core_aic_all_sync.png "多AI_Core中AIC全核同步")
<!-- end id1 -->

<!-- npu="A3,910b" id2 -->

> [!NOTE]说明
> 以下针对模式1的描述，仅针对[NPU架构版本2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)有效。调用相关接口时必须显式传入`pipe`参数。其中，`arrive`接口传入的`pipe`参数生效，`wait`接口传入的`pipe`参数不生效，`wait`接口会阻塞全部流水的后续指令。

## 单个AI Core中AIV全核同步（模式1）<a id="single_ai_core_aiv_full_sync"></a>

该场景需要配套使用模式1的[asc_sync_subblock_arrive](asc_sync_subblock_arrive.md)和[asc_sync_subblock_wait](asc_sync_subblock_wait.md)接口。

1个AI Core中的2个AIV进行全核同步。当2个AIV全部都执行完`asc_sync_subblock_arrive`时，每个AIV对应`flag_id`的计数器增加1。若AIV该`flag_id`的计数器为非0，被`asc_sync_subblock_wait`所阻塞的所有流水中的指令才会执行下去，该`flag_id`的计数器减去1。对于每个AIV，`asc_sync_subblock_arrive`和`asc_sync_subblock_wait`都必须配对使用。

> [!CAUTION]注意
> 该模式中，不同AI Core的AIV核之间不会互相影响同步。

以图3为例，演示第0个AI Core中的2个AIV（AIV0-0、AIV0-1）进行全核同步，代码片段如下：

```cpp
// 进行核间同步时，即使只有AIV参与同步也不能用__vector__修饰核函数，具体原因请参考asc_sync_subblock_arrive的约束说明。
if ASC_IS_AIV {
    if (block_idx == 0) {
        // 参与同步的2个AIV属于第0个AI Core。
        // 每个AIV都应该有类似如下的成对调用asc_sync_subblock_arrive和asc_sync_subblock_wait。
        // 两个接口的flag_id必须一致；两个接口都必须显式传入pipe，asc_sync_subblock_wait的pipe参数在2201架构下不生效。
        asc_sync_subblock_arrive(PIPE_MTE3, 0);  // 待前置PIPE_MTE3流水任务完成后，通知调度模块。
        asc_sync_subblock_wait(PIPE_MTE2, 0);    // 直到该AI Core中的所有AIV都完成前置PIPE_MTE3流水，调度模块更新flag_id=0的计数器后，解除所有流水后续指令的阻塞。
    }
}
```

AIV0-0中在执行`asc_sync_subblock_wait`后，此时AIV0-0 `flag_id=0`的计数器为0，后续所有流水中的指令全部被阻塞，需要等到2个AIV均执行完`asc_sync_subblock_arrive`。

AIV0-1的`asc_sync_subblock_arrive`执行完后，此时调度模块感知到2个AIV均已执行完`asc_sync_subblock_arrive`，因此将AIV0-0和AIV0-1各自的`flag_id=0`的计数器值增加为1。AIV0-0和AIV0-1检测到各自对应的`flag_id=0`的计数器变为1，都解除所有流水后续指令的阻塞，继续执行后续指令，并且将计数器值减去1。

**图3**  模式1：单个AI Core中AIV全核同步<a id="single_ai_core_aiv_full_sync_diagram"></a>    
![](../figures/single_core_aiv_all_sync.png "单AI_Core中AIV全核同步")
<!-- end id2 -->

<!-- npu="950" id3 -->

> [!NOTE]说明
> 以下针对模式2的描述，仅针对[NPU架构版本3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)有效。调用相关接口时必须显式传入`pipe`参数，`arrive`和`wait`接口传入的`pipe`参数均生效，`wait`接口会阻塞由`pipe`参数指定的流水的后续指令。

## 单个AI Core中AIC与AIV全核同步（模式2）<a id="single_ai_core_aic_aiv_full_sync"></a>

单个AI Core中AIC与AIV全核同步需要配套使用模式2的[asc_sync_block_arrive](asc_sync_block_arrive.md)和[asc_sync_block_wait](asc_sync_block_wait.md)接口。该场景可细分为两类：

- 1个AI Core中的AIC执行`asc_sync_block_arrive`，对应的2个AIV都执行`asc_sync_block_wait`。

    当AIC执行完`asc_sync_block_arrive`时，2个AIV对应`flag_id`的计数器增加1。若AIV该`flag_id`的计数器为非0，则解除AIV由`pipe`参数指定的流水后续指令的阻塞，`asc_sync_block_wait`后的指令继续发射，该`flag_id`的计数器减去1。

    总计AIC 1次调用`asc_sync_block_arrive`，对应2个AIV各1次`asc_sync_block_wait`才算配对使用。

- 1个AI Core中的2个AIV都执行`asc_sync_block_arrive`，对应的AIC执行`asc_sync_block_wait`。

    当2个AIV全都执行完`asc_sync_block_arrive`时，AIC对应`flag_id`的计数器增加1。若AIC该`flag_id`的计数器为非0，则解除AIC由`pipe`参数指定的流水后续指令的阻塞，`asc_sync_block_wait`后的指令继续发射，该`flag_id`的计数器减去1。

    总计2个AIV各1次`asc_sync_block_arrive`，对应AIC 1次调用`asc_sync_block_wait`才算配对使用。

以图4为例，演示第0个AI Core中AIC0与AIV0-0、AIV0-1进行同步（AIV发起`asc_sync_block_arrive`），代码片段如下：

```cpp
// AIC和AIV均参与核间同步，需分别使用对应宏包裹代码段，具体原因请参考asc_sync_block_arrive的约束说明。
if ASC_IS_AIV {
    if (block_idx == 0) {
        // 参与同步的AIV属于第0个AI Core。
        // 每个AIV都应该有类似如下的asc_sync_block_arrive调用，且flag_id必须一致。
        asc_sync_block_arrive(PIPE_MTE3, 0);  // 待前置PIPE_MTE3流水任务完成后，通知调度模块。必须由所有AIV执行。
    }
}
if ASC_IS_AIC {
    if (block_idx == 0) {
        // 参与同步的AIC属于第0个AI Core。
        // asc_sync_block_wait必须显式传入pipe，并阻塞pipe参数指定的流水。
        asc_sync_block_wait(PIPE_MTE2, 0);  // 直到该AI Core中的所有AIV都完成前置PIPE_MTE3流水，调度模块更新flag_id=0的计数器后，AIC解除PIPE_MTE2流水后续指令的阻塞。
    }
}
```

AIC0中在执行`asc_sync_block_wait`后，此时AIC0 `flag_id=0`的计数器为0，后续PIPE_MTE2流水中的指令被阻塞，需要等到2个AIV均执行完`asc_sync_block_arrive`。

当AIV0-1的`asc_sync_block_arrive`在Vector指令1执行完后，前置PIPE\_V的指令全部完成，`asc_sync_block_arrive`执行完成，但是此时AIV0-0未执行`asc_sync_block_arrive`。因此AIC0 `flag_id=0`的计数器还是为0，AIC的PIPE_MTE2流水中的指令依然被阻塞。

当AIV0-0的Vector指令1、2全部执行完毕后，前置PIPE\_V的指令全部完成，`asc_sync_block_arrive`生效。此时调度模块感知到2个AIV均已执行完`asc_sync_block_arrive`，因此将AIC0 `flag_id=0`的计数器值增加为1。AIC0检测到对应的`flag_id=0`的计数器变为1，则AIC0核解除PIPE_MTE2流水后续指令的阻塞，继续执行后续指令Matrix指令2，并且将计数器值减去1。

**图4**  模式2：单个AI Core中AIC与AIV全核同步（AIV进行asc_sync_block_arrive）<a id="single_ai_core_aic_aiv_full_sync_diagram"></a>    
![](../figures/single_core_aic_aiv_sync_aiv_setflag.png "单AI_Core中AIC与AIV全核同步（AIV进行asc_sync_block_arrive）")
<!-- end id3 -->

<!-- npu="950" id4 -->
> [!NOTE]说明
> 模式4仅在[NPU架构版本3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)上支持。`asc_sync_intra_wait`传入的`pipe`参数生效，阻塞由`pipe`参数指定的流水的后续指令。

## 单个AI Core中AIC与单个AIV同步（模式4）<a id="single_ai_core_aic_single_aiv_sync"></a>

单个AI Core中AIC与单个AIV同步需要配套使用模式4的[asc_sync_intra_arrive](asc_sync_intra_arrive.md)和[asc_sync_intra_wait](asc_sync_intra_wait.md)接口。该场景可细分为两类：

- 1个AI Core中的AIC执行`asc_sync_intra_arrive`，对应的AIV执行`asc_sync_intra_wait`。

    当AIC执行完`asc_sync_intra_arrive`时，AIV对应`sync_id`的计数器增加1。若AIV该`sync_id`的计数器为非0，则解除AIV由`pipe`参数指定的流水的阻塞，`asc_sync_intra_wait`后的指令继续发射，该`sync_id`的计数器减去1。

    总计AIC 1次调用`asc_sync_intra_arrive`，单个AIV完成一次`asc_sync_intra_wait`才算配对使用。

- 1个AI Core中的1个AIV执行`asc_sync_intra_arrive`，对应的AIC执行`asc_sync_intra_wait`。

    当AIV执行完`asc_sync_intra_arrive`时，AIC对应`sync_id`的计数器增加1。若AIC该`sync_id`的计数器为非0，则解除AIC由`pipe`参数指定的流水的阻塞，`asc_sync_intra_wait`后的指令继续发射，该`sync_id`的计数器减去1。

    总计单个AIV 1次`asc_sync_intra_arrive`，对应AIC 1次调用`asc_sync_intra_wait`才算配对使用。

以图5为例，演示第0个AI Core中AIV0-1与AIC0进行同步（AIV0-1发起`asc_sync_intra_arrive`），代码片段如下：

```cpp
// AIC和AIV均参与核间同步，需分别使用对应宏包裹代码段，具体原因请参考asc_sync_intra_arrive的约束说明。
if ASC_IS_AIV {
    if ((block_idx == 0) && (asc_get_sub_block_id() == 1)) {
        // AIV0-1上的sync_id 0~15对应AIC0上的sync_id 16~31。
        asc_sync_intra_arrive(PIPE_MTE3, 0);  // 待AIV0-1上前置PIPE_MTE3流水任务完成后，通知调度模块。
    }
}
if ASC_IS_AIC {
    if (block_idx == 0) {
        // asc_sync_intra_wait的pipe参数生效，此处阻塞PIPE_FIX流水。
        asc_sync_intra_wait(PIPE_FIX, 16);  // 直到该AI Core中的AIV0-1完成前置PIPE_MTE3流水，调度模块更新sync_id=16的计数器后，AIC0解除阻塞。
    }
}
```

AIC0中在执行`asc_sync_intra_wait`后，此时AIC0 `sync_id=16`的计数器为0，后续由`pipe`参数指定的流水（此处为PIPE_FIX）的指令全部被阻塞，需要等到1个AIV执行完`asc_sync_intra_arrive`。

- AIV0-0不需要执行`asc_sync_intra_arrive`，也不需要参与同步。
- AIV0-1的PIPE_MTE3指令全部执行完毕后，`asc_sync_intra_arrive`生效。此时调度模块感知1个AIV已执行完`asc_sync_intra_arrive`，因此将AIC0 `sync_id=16`的计数器值增加为1。AIC0检测到对应的`sync_id=16`的计数器变为1，则AIC0核解除由`pipe`参数指定的流水后续指令的阻塞，继续执行后续PIPE_FIX的指令，并且将计数器值减去1。

**图5**  模式4：单个AI Core中AIC与单个AIV同步（AIV进行asc_sync_intra_arrive）<a id="single_ai_core_aic_single_aiv_sync_diagram"></a>    
![](../figures/single_ai_core_aic_single_aiv_sync.png "单AI_Core中AIC与单个AIV同步（AIV进行asc_sync_intra_arrive）")
<!-- end id4 -->
