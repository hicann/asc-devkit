# 关键特性说明<a id="inter_core_sync_key_features"></a>

本文档主要介绍核间同步的两个关键事项：

- 核间同步flagId的占用情况，说明各接口内部占用的flagId取值范围，帮助开发者在使用CrossCoreSetFlag/CrossCoreWaitFlag时避免flagId冲突。
- 各核间同步控制模式的实现原理，配合时序图介绍模式0、1、2、4各自的工作机制。

## 核间同步flagId占用说明<a id="inter_core_sync_flagid_usage"></a>

### 概述

开发者使用[CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md)和[CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md)接口进行核间同步时，需要传入核间同步标记**flagId**，每个flagId对应一个用于控制同步的计数器。

部分接口的内部实现也会占用一部分flagId。当开发者同时使用这些接口与CrossCoreSetFlag/CrossCoreWaitFlag时，若flagId发生冲突（即相互独立的同步操作复用了同一个flagId），其对应的计数器会被错误地共享和修改，导致核间同步行为异常：可能出现阻塞无法解除而使程序卡死，或同步提前完成、阻塞提前解除而导致计算结果错误。

因此，当开发者同时调用CrossCoreSetFlag/CrossCoreWaitFlag与这些占用flagId的接口时，应**避开各接口内部已占用的flagId**，选用未被占用的flagId。

核间同步分为**模式0、1、2**与**模式4**，两者的底层硬件指令不同，flagId相互独立，因此各接口flagId的占用情况按**所属模式**和**NPU架构版本**分别参见[模式0、1、2的flagId占用](#inter_core_sync_flagid_mode012)和[模式4的flagId占用](#inter_core_sync_flagid_mode4)。

### 模式0、1、2的flagId占用<a id="inter_core_sync_flagid_mode012"></a>

**表1**  模式0、1、2flagId占用情况

| 接口 | 限定条件 | 占用flagId | 各计算核占用情况 | NPU架构版本 |
| :--- | :--- | :--- | :--- | :--- |
| [SyncAll](SyncAll.md) | `isAIVOnly=true` | 14 | AIV：14 | 2201、3510 |
| [SyncAll](SyncAll.md) | `isAIVOnly=false` | 11、12、13 | AIC：11、12、13；AIV：12、13 | 2201、3510 |
| [SetNextTaskStart](../inter_task_sync/SetNextTaskStart.md) | SuperKernel子核函数（Kernel）中调用 | 11、14 | AIC：11；AIV：14 | 2201、3510 |
| [WaitPreTaskEnd](../inter_task_sync/WaitPreTaskEnd.md) | SuperKernel子核函数（Kernel）中调用 | 11、12、13、14 | AIC：11、12、13；AIV：12、13、14 | 2201、3510 |
| [Matmul](../../../adv_api/cube_compute/Matmul_Kernel/Matmul_Kernel.md) | 定义N个Matmul对象 | `[0, 2N-1]` | AIV/AIC按对象与subBlock分配 | 2201 |
| [Matmul](../../../adv_api/cube_compute/Matmul_Kernel/Matmul_Kernel.md) | SuperKernel融合场景（[GetTaskRatio](../../tool_interface/system_resources_and_variables/GetTaskRatio.md)返回2，即1:2配比） | 15 | AIV：15 | 2201 |

### 模式4的flagId占用<a id="inter_core_sync_flagid_mode4"></a>

**表2**  模式4flagId占用情况

| 接口 | 限定条件 | 占用flagId | 各计算核占用情况 | NPU架构版本 |
| :--- | :--- | :--- | :--- | :--- |
| [SyncAll](SyncAll.md) | `isAIVOnly=false`（核函数（Kernel）使用`__mix__(1, 2)`修饰时） | 12、13、28、29 | AIC：12、13、28、29；AIV：12、13 | 3510 |
| [WaitPreTaskEnd](../inter_task_sync/WaitPreTaskEnd.md) | SuperKernel子核函数（Kernel）中调用 | 12、13、28、29 | AIC：12、13、28、29；AIV：12、13 | 3510 |
| [Matmul](../../../adv_api/cube_compute/Matmul_Kernel/Matmul_Kernel.md) | 定义N个[Matmul对象](../../../adv_api/cube_compute/Matmul_Kernel/REGIST_MATMUL_OBJ.md) | `[0, N+17]` | AIC/AIV按对象、eventType、subBlock分配 | 3510 |
| [Matmul](../../../adv_api/cube_compute/Matmul_Kernel/Matmul_Kernel.md) | A矩阵和B矩阵同时开启[IBSHARE](../../../adv_api/cube_compute/Matmul_Kernel/Matmul_usage.md#p84551411817)的场景 | 15 | AIV：15 | 3510 |
| [TSCM](../../resource_management/TSCM/TSCM.md) | 从UB发起的L1 Buffer（TSCM）队列 | 0~11（从11向下逐个分配） | AIV | 3510 |
| [DataCopy（GM→L1）](../../cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_continuous.md) | 无 | 1、17 | AIC：1、17；AIV：1 | 3510 |

### 各接口限定条件说明

#### SyncAll

- **isAIVOnly=true**：纯Vector算子的全核同步，占用模式0、1、2的flagId为14（AIV）。
- **isAIVOnly=false**：Mix（包含Cube和Vector计算）算子的全核同步，占用情况随NPU架构与核函数（Kernel）类型不同：
    - **2201架构**：占用模式0、1、2的flagId为11、12、13。
    - **3510架构**：
        - 核函数（Kernel）使用`__mix__(1, 1)`修饰时，占用模式0、1、2的flagId为11、12、13。
        - 核函数（Kernel）使用`__mix__(1, 2)`修饰时，占用模式0、1、2的flagId为11，以及模式4的flagId为12、13、28、29。

    以上`isAIVOnly`模板参数的说明参见[SyncAll](SyncAll.md)；`__mix__(1, 1)`/`__mix__(1, 2)`修饰符的说明参见[函数执行空间限定符](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#函数执行空间限定符)。

#### SetNextTaskStart / WaitPreTaskEnd

- 二者为**SuperKernel**场景专用接口，仅在算子以[SuperKernel](../../../../../guide/programming_guide/advanced_programming/super_kernel/principles.md)方式融合、且子核函数（Kernel）中调用时才生效。
- **SetNextTaskStart**：占用模式0、1、2的flagId为11（AIC）、14（AIV）。参见[SetNextTaskStart](../inter_task_sync/SetNextTaskStart.md)。
- **WaitPreTaskEnd**：参见[WaitPreTaskEnd](../inter_task_sync/WaitPreTaskEnd.md)。
    - **2201架构**：占用模式0、1、2的flagId为11、12、13、14。
    - **3510架构**：占用模式0、1、2的flagId为11、12、13、14，以及模式4的flagId为12、13、28、29。

#### Matmul

- 占用flagId的范围与定义的**Matmul对象数目N**相关（N最大为4），并且随NPU架构不同而不同，参见[REGIST_MATMUL_OBJ](../../../adv_api/cube_compute/Matmul_Kernel/REGIST_MATMUL_OBJ.md#约束说明)。
- **2201架构**：占用模式0、1、2的flagId为[0, 2N-1]（N=4时占用`[0,7]`）；SuperKernel融合（1:2配比，即[GetTaskRatio](../../tool_interface/system_resources_and_variables/GetTaskRatio.md)返回2）场景下还占用flagId为15。
- **3510架构**：占用模式4的flagId为[0, N+17]（N=4时占用`[0,21]`）；A矩阵和B矩阵同时开启[IBSHARE](../../../adv_api/cube_compute/Matmul_Kernel/Matmul_usage.md#p84551411817)的场景下还占用flagId为15。

#### TSCM

- 仅当使用从UB发起的L1 Buffer（TSCM）队列时才触发，参见[TSCM](../../resource_management/TSCM/TSCM.md)。
- 占用模式4的flagId，范围为**0~11**：第1个TSCM队列占用11，第2个占用10，依次向下递减分配，最多可分配到0。

#### DataCopy（GM→L1）

- 在**3510架构**下占用模式4的flagId为1、17（AIC：1、17；AIV：1）。参见[DataCopy（GMToL1连续数据搬运）](../../cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_continuous.md)。

## 各个核间同步控制实现原理<a id="sync_control_mode"></a>
如图1所示，本章节将配合时序图介绍CrossCoreSetFlag和CrossCoreWaitFlag配合使用时支持的四种同步控制模式各自实现的原理。

- 模式0：AI Core核间的同步控制。对于AIC全核场景，同步所有的AIC核，直到所有的AIC核都执行到CrossCoreSetFlag时，CrossCoreWaitFlag后续的全部流水或者由模板参数pipe指定的流水（与NPU架构有关）中的指令才会执行；对于AIV全核场景，同步所有的AIV核，直到所有的AIV核都执行到CrossCoreSetFlag时，CrossCoreWaitFlag后续的全部流水或者由模板参数pipe指定的流水（与NPU架构有关）中的指令才会执行。
- 模式1：AI Core内部，AIV核之间的同步控制。如果两个AIV核都运行了CrossCoreSetFlag，CrossCoreWaitFlag后续的全部流水或者由模板参数pipe指定的流水（与NPU架构有关）中的指令才会执行。
- 模式2：AI Core内部，AIC与AIV之间的同步控制。在AIC核执行CrossCoreSetFlag之后，两个AIV上CrossCoreWaitFlag后续的全部流水或者由模板参数pipe指定的流水（与NPU架构有关）中的指令才会继续执行；两个AIV都执行CrossCoreSetFlag后，AIC上CrossCoreWaitFlag后续的全部流水或者由模板参数pipe指定的流水（与NPU架构有关）中的指令才能执行。
- 模式4：AI Core内部，AIC与单个AIV之间的同步控制。在AIC核执行CrossCoreSetFlag之后，单个AIV上CrossCoreWaitFlag后续的由模板参数pipe指定的流水中的指令才会继续执行；单个AIV执行CrossCoreSetFlag后，AIC上CrossCoreWaitFlag后续的由模板参数pipe指定的流水中的指令才能执行。该模式仅在[NPU架构版本3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)上支持。

> [!NOTE]说明
> 模式0、1、2下`CrossCoreWaitFlag`阻塞的流水类型因NPU架构而异：
> - 在[NPU架构版本2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)上，`CrossCoreWaitFlag`传入的`pipe`参数无效，无论取值为何都默认阻塞全部流水的后续指令，建议省略`pipe`参数并使用默认值。
> - 在[NPU架构版本3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)上，`CrossCoreWaitFlag`传入的`pipe`参数生效，阻塞由模板参数`pipe`指定的流水的后续指令。

**图1**  同步控制模式示意图<a id="sync_control_mode_diagram"></a>    
![](../../../../figures/3510_sync_control_mode_diagram.png "同步控制模式示意图")

下述同步特性均以如下场景配置为例：核函数（Kernel）使用`__mix__(1, 2)`修饰，即每个AI Core包含1个AIC和2个AIV，并设置逻辑核数`numBlocks=2`，即共启动2个AI Core，因此存在2个AIC和4个AIV。为便于描述，将2个AIC分别编号为AIC0、AIC1；AI Core0中的2个AIV分别编号为AIV0-0、AIV0-1，AI Core1中的2个AIV分别编号为AIV1-0、AIV1-1。**各核中与`flagId`对应的计数器初始值均为0。**

<!-- npu="A3,910b" id1 -->

> [!NOTE]说明
> 以下针对模式0、1、2的描述，仅针对[NPU架构版本2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)有效，此时`CrossCoreWaitFlag`传入的`pipe`参数无效，因此采用默认值，默认阻塞全部流水的后续指令。

## 多AI Core中AIC或者AIV全核同步（模式0）<a id="multi_ai_core_aic_aiv_full_sync"></a>

多AI Core中AIC或者AIV全核同步需要配套使用模式0的[CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md)和[CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md)接口。该场景可细分为两类：

- 2个AI Core中的2个AIC进行全核同步。

    当2个AIC都执行完CrossCoreSetFlag时，每个AIC对应flagId的计数器增加1。若AIC该flagId的计数器为非0，被CrossCoreWaitFlag所阻塞的所有流水中的指令才会执行下去，该flagId的计数器减去1。

    对于每个AIC，CrossCoreSetFlag和CrossCoreWaitFlag都必须配对使用。

- 2个AI Core中的4个AIV进行全核同步。

    当4个AIV都执行完CrossCoreSetFlag时，每个AIV核对应flagId的计数器增加1。若AIV该flagId的计数器为非0，被CrossCoreWaitFlag所阻塞的所有流水中的指令才会执行下去，该flagId的计数器减去1。

    对于每个AIV，CrossCoreSetFlag和CrossCoreWaitFlag都必须配对使用。

以图2为例，演示2个AI Core中的2个AIC（AIC0、AIC1）进行全核同步，代码片段如下：

```cpp
// 进行核间同步时，即使只有AIC参与同步也不能用__cube__修饰核函数（Kernel），具体原因请参考CrossCoreSetFlag的约束说明。
if (ASCEND_IS_AIC) {
    // 每个核都应该有类似如下的成对调用CrossCoreSetFlag和CrossCoreWaitFlag。
    // modeId必须配置为0，flagId要一致；CrossCoreWaitFlag的pipe使用默认值。
    AscendC::CrossCoreSetFlag<0, PIPE_M>(0);       // 待前置PIPE_M流水任务完成后，通知调度模块。
    AscendC::CrossCoreWaitFlag<0>(0);              // 直到所有AIC都完成前置PIPE_M流水，调度模块更新flagId=0的计数器后，解除所有流水后续指令的阻塞。
}
```

AIC0中在执行CrossCoreWaitFlag后，此时AIC0 flagId=0的计数器为0，后续所有流水中的指令全部被阻塞，需要等到2个AIC核均执行完CrossCoreSetFlag。

AIC1的CrossCoreSetFlag执行完后，此时调度模块感知到2个AIC均已执行完CrossCoreSetFlag，因此所有AIC各自的flagId=0的计数器值增加为1。AIC0和AIC1检测到各自对应的flagId=0的计数器变为1，都解除所有流水后续指令的阻塞，继续执行后续指令，并且将计数器值减去1。

**图2**  模式0：多AI Core中AIC全核同步<a id="multi_ai_core_aic_full_sync"></a>    
![](../../../../figures/inter_core_aic_all_sync.png "多AI_Core中AIC全核同步")

## 单个AI Core中AIV全核同步（模式1）<a id="single_ai_core_aiv_full_sync"></a>

该场景需要配套使用模式1的[CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md)和[CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md)接口。

1个AI Core中的2个AIV进行全核同步。当2个AIV全部都执行完CrossCoreSetFlag时，每个AIV对应flagId的计数器增加1。若AIV该flagId的计数器为非0，被CrossCoreWaitFlag所阻塞的所有流水中的指令才会执行下去，该flagId的计数器减去1。对于每个AIV，CrossCoreSetFlag和CrossCoreWaitFlag都必须配对使用。

> [!CAUTION]注意
> 该模式中，不同AI Core的AIV核之间不会互相影响同步。

以图3为例，演示第0个AI Core中的2个AIV（AIV0-0、AIV0-1）进行全核同步，代码片段如下：

```cpp
// 进行核间同步时，即使只有AIV参与同步也不能用__vector__修饰核函数（Kernel），具体原因请参考CrossCoreSetFlag的约束说明。
if (ASCEND_IS_AIV) {
    if (AscendC:: GetBlockIdx() <= 1) {
        // 参与同步的2个AIV属于第0个AI Core。
        // 每个AIV都应该有类似如下的成对调用CrossCoreSetFlag和CrossCoreWaitFlag。
        // modeId必须配置为1，flagId要一致；CrossCoreWaitFlag的pipe使用默认值。
        AscendC::CrossCoreSetFlag<1, PIPE_MTE3>(0);       // 待前置PIPE_MTE3流水任务完成后，通知调度模块。
        AscendC::CrossCoreWaitFlag<1>(0);                  // 直到该AI Core中的所有AIV都完成前置PIPE_MTE3流水，调度模块更新flagId=0的计数器后，解除所有流水后续指令的阻塞。
    }
}
```

AIV0-0中在执行CrossCoreWaitFlag后，此时AIV0-0 flagId=0的计数器为0，后续所有流水中的指令全部被阻塞，需要等到2个AIV均执行完CrossCoreSetFlag。

AIV0-1的CrossCoreSetFlag执行完后，此时调度模块感知到2个AIV均已执行完CrossCoreSetFlag，因此将AIV0-0和AIV0-1各自的flagId=0的计数器值增加为1。AIV0-0和AIV0-1检测到各自对应的flagId=0的计数器变为1，都解除所有流水后续指令的阻塞，继续执行后续指令，并且将计数器值减去1。

**图3**  模式1：单个AI Core中AIV全核同步<a id="single_ai_core_aiv_full_sync_diagram"></a>    
![](../../../../figures/single_core_aiv_all_sync.png "单AI_Core中AIV全核同步")

## 单个AI Core中AIC与AIV全核同步（模式2）<a id="single_ai_core_aic_aiv_full_sync"></a>

单个AI Core中AIC与AIV全核同步需要配套使用模式2的[CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md)和[CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md)接口。该场景可细分为两类：

- 1个AI Core中的AIC执行CrossCoreSetFlag，对应的2个AIV都执行CrossCoreWaitFlag。

    当AIC执行完CrossCoreSetFlag时，2个AIV对应flagId的计数器增加1。若AIV该flagId的计数器为非0，则解除AIV所有流水后续指令的阻塞，CrossCoreWaitFlag后的指令继续发射，该flagId的计数器减去1。

    总计AIC 1次调用CrossCoreSetFlag，对应2个AIV各1次CrossCoreWaitFlag才算配对使用。

- 1个AI Core中的2个AIV都执行CrossCoreSetFlag，对应的AIC执行CrossCoreWaitFlag。

    当2个AIV全都执行完CrossCoreSetFlag时，AIC对应flagId的计数器增加1。若AIC该flagId的计数器为非0，则解除AIC所有流水后续指令的阻塞，CrossCoreWaitFlag后的指令继续发射，该flagId的计数器减去1。

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
        AscendC::CrossCoreWaitFlag<2>(0);                 // 直到该AI Core中的所有AIV都完成前置PIPE_MTE3流水，调度模块更新flagId=0的计数器后，AIC解除所有流水后续指令的阻塞。
    }
}
```

AIC0中在执行CrossCoreWaitFlag后，此时AIC0 flagId=0的计数器为0，后续所有流水中的指令全部被阻塞，需要等到2个AIV均执行完CrossCoreSetFlag。

当AIV0-1的CrossCoreSetFlag在Vector指令1执行完后，前置PIPE\_V的指令全部完成，CrossCoreSetFlag执行完成，但是此时AIV0-0未执行CrossCoreSetFlag。因此AIC0 flagId=0的计数器还是为0，AIC的所有流水中的指令依然被阻塞。

当AIV0-0的Vector指令1、2全部执行完毕后，前置PIPE\_V的指令全部完成，CrossCoreSetFlag生效。此时调度模块感知到2个AIV均已执行完CrossCoreSetFlag，因此将AIC0 flagId=0的计数器值增加为1。AIC0检测到对应的flagId=0的计数器变为1，则AIC0核解除所有流水后续指令的阻塞，继续执行后续指令Matrix指令2，并且将计数器值减去1。

**图4**  模式2：单个AI Core中AIC与AIV全核同步（AIV进行CrossCoreSetFlag）<a id="single_ai_core_aic_aiv_full_sync_diagram"></a>    
![](../../../../figures/single_core_aic_aiv_sync_aiv_setflag.png "单AI_Core中AIC与AIV全核同步（AIV进行CrossCoreSetFlag）")
<!-- end id1 -->

<!-- npu="950" id2 -->
> [!NOTE]说明
> 模式4仅在[NPU架构版本3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)上支持，此时`CrossCoreWaitFlag`传入的`pipe`参数生效，阻塞由**模板参数pipe配置的指定流水**的后续指令。

## 单个AI Core中AIC与单个AIV同步（模式4）<a id="single_ai_core_aic_single_aiv_sync"></a>

单个AI Core中AIC与单个AIV同步需要配套使用模式4的[CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md)和[CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md)接口。该场景可细分为两类：

- 1个AI Core中的AIC执行CrossCoreSetFlag，对应的AIV执行CrossCoreWaitFlag。

    当AIC执行完CrossCoreSetFlag时，AIV对应flagId的计数器增加1。若AIV该flagId的计数器为非0，则解除AIV由模板参数pipe指定的流水的阻塞，CrossCoreWaitFlag后的指令继续发射，该flagId的计数器减去1。

    总计AIC 1次调用CrossCoreSetFlag，单个AIV完成一次CrossCoreWaitFlag才算配对使用。

- 1个AI Core中的1个AIV执行CrossCoreSetFlag，对应的AIC执行CrossCoreWaitFlag。

    当AIV执行完CrossCoreSetFlag时，AIC对应flagId的计数器增加1。若AIC该flagId的计数器为非0，则解除AIC由模板参数pipe指定的流水的阻塞，CrossCoreWaitFlag后的指令继续发射，该flagId的计数器减去1。

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
        // modeId必须配置为4，flagId要一致；CrossCoreWaitFlag的pipe参数生效，此处阻塞由模板参数pipe配置的指定流水（PIPE_FIX）。
        AscendC::CrossCoreWaitFlag<4, PIPE_FIX>(16);       // 阻塞由模板参数pipe配置的指定流水（此处为PIPE_FIX）的后续指令，直到该AI Core中的AIV0-1都完成前置PIPE_MTE3流水，调度模块更新flagId=16的计数器后，AIC0解除阻塞。
    }
}
```

AIC0中在执行CrossCoreWaitFlag后，此时AIC0 flagId=16的计数器为0，后续由模板参数pipe配置的指定流水（此处为PIPE_FIX）的指令全部被阻塞，需要等到1个AIV执行完CrossCoreSetFlag。

- AIV0-0不需要执行CrossCoreSetFlag，也不需要参与同步。
- AIV0-1的PIPE_MTE3指令全部执行完毕后，CrossCoreSetFlag生效。此时调度模块感知1个AIV已执行完CrossCoreSetFlag，因此将AIC0 flagId=16的计数器值增加为1。AIC0检测到对应的flagId=16的计数器变为1，则AIC0核解除由模板参数pipe配置的指定流水后续指令的阻塞，继续执行后续PIPE_FIX的指令，并且将计数器值减去1。

**图5**  模式4：单个AI Core中AIC与单个AIV同步（AIV进行CrossCoreSetFlag）<a id="single_ai_core_aic_single_aiv_sync_diagram"></a>    
![](../../../../figures/single_ai_core_aic_single_aiv_sync.png "单AI_Core中AIC与单个AIV同步（AIV进行CrossCoreSetFlag）")
<!-- end id2 -->
