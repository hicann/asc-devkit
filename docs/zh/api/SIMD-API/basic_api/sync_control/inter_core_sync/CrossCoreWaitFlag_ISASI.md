# CrossCoreWaitFlag\(ISASI\)<a id="ZH-CN_TOPIC_0000001787470374"></a>

## 产品支持情况<a id="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI_res.md#id1 -->

## 功能说明<a id="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_block_sync_intf.h"`。

在核间同步场景中，CrossCoreSetFlag接口和CrossCoreWaitFlag接口配对工作，具体功能请参考[CrossCoreSetFlag功能说明](CrossCoreSetFlag_ISASI.md#section618mcpsimp)。

## 函数原型<a id="section620mcpsimp"></a>

```cpp
template <uint8_t modeId = 0, pipe_t pipe = PIPE_S>
__aicore__ inline void CrossCoreWaitFlag(uint16_t flagId)
```

## 参数说明<a id="section622mcpsimp"></a>

### 模板参数及输入参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| modeId | 核间同步的模式，默认值为0。不同产品对该参数的生效情况及支持的流水类型请参见[模板参数默认值及生效情况](#template-parameter-defaults)。<br>不同产品对同步模式的支持情况请参见[modeId支持的取值说明](#modeId支持的取值说明)。<br>各个模式支持的对应核函数（Kernel）类型请参照[表3](#table3)。 |
| pipe | 设置该接口阻塞的流水类型，默认值为PIPE_S。不同产品对该参数的生效情况及支持的流水类型请参见[模板参数默认值及生效情况](#template-parameter-defaults)。 |

### 模板参数默认值及生效情况<a id="template-parameter-defaults"></a>

<!-- npu="950" id8 -->
- 针对Ascend 950PR/Ascend 950DT，两个模板参数的使用描述如下：

    - modeId
        - `modeId`不直接作为硬件指令的输入，CrossCoreWaitFlag会根据该参数选择对应的硬件指令。
        - 模式0、模式1和模式2使用相同的硬件指令。省略`modeId`时使用默认值`0`，不影响接口功能；但**建议显式传入与CrossCoreSetFlag相同的`modeId`**，便于确认两个接口的同步模式匹配。
        - 模式4使用的硬件指令与模式0、模式1和模式2不同，**必须显式传入`modeId=4`**，否则接口会按照默认的模式0执行，导致同步行为不符合预期。
    - pipe
        - `pipe`作为硬件指令的输入，参数取值**生效**，用于指定CrossCoreWaitFlag阻塞的流水。
        - 省略`pipe`时使用默认值`PIPE_S`。
        - 模式0、模式1、模式2和模式4支持`PIPE_S`、`PIPE_V`、`PIPE_M`、`PIPE_MTE1`、`PIPE_MTE2`、`PIPE_MTE3`、`PIPE_FIX`，不支持`PIPE_ALL`。
<!-- end id8 -->
<!-- npu="A3,910b" id9 -->
- 针对如下产品，**参数取值不影响硬件指令功能**，两个模板参数的使用描述如下：

    <!-- npu="A3" id10 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id10 -->
    <!-- npu="910b" id11 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id11 -->

    `modeId`和`pipe`均不作为硬件指令的输入，建议按如下方式配置：

    - modeId：省略该参数不影响接口功能；但**建议显式传入与CrossCoreSetFlag相同的`modeId`**，便于确认两个接口的同步模式匹配。
    - pipe：**建议省略该参数并使用默认值`PIPE_S`**。无论`pipe`取值为何，CrossCoreWaitFlag都会阻塞**全部流水**的后续指令。
<!-- end id9 -->

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| flagId | 输入 | 核间同步的标记。不同产品对flagId的取值范围说明请参见[flagId取值范围说明](#flagId取值范围说明)。 |

### modeId支持的取值说明<a id="modeId支持的取值说明"></a>

不同产品对同步模式的支持情况如下：
<!-- npu="950" id12 -->
- 针对Ascend 950PR/Ascend 950DT，支持的同步模式为：模式0、模式1、模式2、模式4。
<!-- end id12 -->
<!-- npu="A3,910b" id13 -->
- 针对如下产品，支持的同步模式为：模式0、模式1、模式2。
    <!-- npu="A3" id14 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id14 -->
    <!-- npu="910b" id15 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id15 -->
<!-- end id13 -->

### flagId取值范围说明<a id="flagId取值范围说明"></a>

- 核间同步的模式为模式0、1、2时，支持的取值范围为0-15。
<!-- npu="950" id16 -->
- 针对Ascend 950PR/Ascend 950DT，核间同步的模式为模式4时，支持的取值范围情况如下：
    - AIV0发起的flagId 0-15的CrossCoreSetFlag操作对应AIC CrossCoreWaitFlag中flagId 0-15的操作。
    - AIV1发起的flagId 0-15的CrossCoreSetFlag操作对应AIC CrossCoreWaitFlag中flagId 16-31的操作。
    - AIC发起的flagId 0-15的CrossCoreSetFlag操作对应AIV0 CrossCoreWaitFlag中flagId 0-15的操作。
    - AIC发起的flagId 16-31的CrossCoreSetFlag操作对应AIV1 CrossCoreWaitFlag中flagId 0-15的操作。
<!-- end id16 -->

此外，当与Matmul高阶API或SyncAll接口同时使用时，开发者自行使用的flagId还需注意避免与这些接口内部已占用的flagId冲突，详见[约束说明](#flagId冲突说明)中的flagId占用情况。

## 返回值说明<a id="section640mcpsimp"></a>

无

## 约束说明<a id="section633mcpsimp"></a>

- 由于当核函数（Kernel）类型为KERNEL_TYPE_AIC_ONLY或KERNEL_TYPE_AIV_ONLY时，硬件不会开启调度模块，也就无法正常进行核间同步，因此不同的同步模式配置[核函数（Kernel）类型](../../Kernel-Tiling/set_Kernel_type.md)或[函数修饰符](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#section1074418132518)的情况如下：
    - 在纯Vector/Cube场景下（模式0或模式1），建议设置核函数（Kernel）类型为KERNEL\_TYPE\_MIX\_AIV\_1\_0或KERNEL\_TYPE\_MIX\_AIC\_1\_0，其它支持的核函数（Kernel）类型请参考表3。
    - 对于Vector和Cube混合场景（模式2和模式4），需根据AI Core中AIC和AIV的比例灵活配置核函数（Kernel）类型，不同模式支持的函数修饰符和核函数（Kernel）类型请参照表3。

        **表3**  模式与支持的核函数（Kernel）类型配置<a id="table3"></a>

        | 模式 | 支持的函数修饰符 | 支持的核函数（Kernel）类型配置 |
        | --- | --- | --- |
        | 0 | \_\_mix\_\_(0, 1)、\_\_mix\_\_(1, 0)、\_\_mix\_\_(1, 1)、\_\_mix\_\_(1, 2) | KERNEL\_TYPE\_MIX\_AIV\_1\_0、KERNEL\_TYPE\_MIX\_AIC\_1\_0、KERNEL\_TYPE\_MIX\_AIC\_1\_1、KERNEL\_TYPE\_MIX\_AIC\_1\_2 |
        | 1 | \_\_mix\_\_(1, 1)、\_\_mix\_\_(1, 2) | KERNEL\_TYPE\_MIX\_AIC\_1\_1、KERNEL\_TYPE\_MIX\_AIC\_1\_2 |
        | 2 | \_\_mix\_\_(1, 1)、\_\_mix\_\_(1, 2) | KERNEL\_TYPE\_MIX\_AIC\_1\_1、KERNEL\_TYPE\_MIX\_AIC\_1\_2 |
        | 4 | \_\_mix\_\_(1, 2) | KERNEL\_TYPE\_MIX\_AIC\_1\_2 |

- CrossCoreWaitFlag必须与[CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md)接口配合使用，避免计算核一直处于阻塞阶段。建议显式配置与配对CrossCoreSetFlag相同的modeId，以便确认Set与Wait使用相同的同步模式。

<a id="flagId冲突说明"></a>

- 以下接口内部实现中使用了CrossCoreSetFlag和CrossCoreWaitFlag会占用一部分flagId，开发者同时使用CrossCoreSetFlag/CrossCoreWaitFlag与以下接口时，需注意避免flagId使用冲突：

    - Matmul高阶API占用的flagId范围与定义的Matmul对象数目相关，假设定义了N个Matmul对象，Matmul高阶API内部占用的flagId范围为\[0, 2 \* N - 1\]。Matmul最多支持定义4个对象，此时flagId占用范围为\[0,7\]。
    - SyncAll硬件同步接口占用的flagId情况请参考[flagId占用情况](SyncAll.md#syncall_flagId冲突说明)。

- flagId相关的约束：
    - 对于模式0、1、2，每个AIC和每个AIV都各自有16个flagId，支持的取值范围为0-15。如果flagId的值超出该范围，则截取低4bit（例如，flagId=17时，截取后为1）。
    <!-- npu="950" id17 -->
    - 针对Ascend 950PR/Ascend 950DT，核间同步的模式为模式4时，AIC有32个flagId，支持的取值范围为0-31，如果flagId的值超出该范围，则截取低5bit（例如，flagId=35时，截取后为3）；AIV有16个flagId，支持的取值范围为0-15，如果flagId的值超出该范围，则截取低4bit（例如，flagId=17时，截取后为1）。
    <!-- end id17 -->
    - 每个flagId都对应一个计数器，当调用[CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md)时，若计数器值为0则会阻塞后续指令下发，已下发指令可正常执行；当调度模块感知到核间同步（CrossCoreSetFlag）全部完成后，会将对应CrossCoreWaitFlag的计数器的值增加1。此时，计数器值为非0，阻塞解除，并且将对应计数器的值减去1进行还原。具体执行逻辑与细节可以参考[关键特性说明](key_features.md#ZH-CN_TOPIC_0000002586300741)。flagId对应的计数器计数范围为0-15。如果计数器的值超出该范围，则会异常报错，中断流程。
- 模式0、1、2下，同一个flagId用于不同核间同步模式的约束：
    - 同一核上，若同一个flagId需用于不同核间同步模式，须在模式切换前完成前一个模式的所有同步操作——即确保该flagId关联的所有CrossCoreSetFlag与配套CrossCoreWaitFlag调用均已执行完毕。
    - 对于不同的核，可以直接将同一flagId用于不同的核间同步模式，具体包括以下2种场景：
        - 多个AI Core之间，使用flagId=0同步所有的AIC（模式0）；单个AI Core内，使用flagId=0同步所有AIV（模式1）。
        - 单个AI Core内，使用flagId=0同步所有AIV（模式1）；另一个AI Core内，使用flagId=0同步AIC与所有AIV（模式2）。
- 使用该接口模式0时，建议开启batchmode模式，使算子独占全部所需核资源，否则可能因满足以下条件导致死锁：
    - 多流并发场景（≥2条执行流）。
    - ≥2个算子并发执行。
    - 所有并发算子的核数总和超过物理核数。
    - ≥2个并发算子使用了核间同步功能。

    具体而言，在多流场景下，某条流的核间同步算子虽分配到n个物理核，但可能仅有n-m个核先被调度执行，而其余m个核因被其他流的核间同步算子抢占而尚未启动。先启动的n-m个核执行到核间同步时等待剩余m核完成，而剩余m核因被其他流的核间同步算子占用而无法释放，形成死锁。

    核函数（Kernel）直调场景下通过[\_\_schedmode\_\_\(mode\)](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)限定符来设置batchmode模式；工程化算子开发场景下，通过TilingContext的SetScheduleMode接口来设置batchmode模式，具体请参考《[基础数据结构和接口](https://gitcode.com/cann/metadef/blob/master/docs/zh/api/README.md)》。

## 调用示例<a id="section837496171220"></a>

**表4**  样例描述

| SCENARIO_NUM取值 | 业务场景 | 使用的同步模式 |
| --- | --- | --- |
| 0 | 纯Vector计算场景（16个AIV） | mode0（AIV全核同步） |
| 1 | 纯Vector计算场景（2个AIV） | mode1 |
| 2 | Cube与Vector融合计算场景 | mode2（AIC等AIV）、mode2（AIV等AIC）、mode0（AIC全核同步） |

如上表所示，当SCENARIO_NUM取不同值时，会分别演示纯Vector计算场景和Cube与Vector融合计算场景下三种同步模式的具体使用方法，下面展示了纯Vector计算场景的部分调用代码：

```cpp
    AscendC::Muls(xLocal, xLocal, float(AscendC::GetBlockIdx()), this->blockLength);
    AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);

    // Unified Buffer（UB）到GM搬运启用原子累加。
    AscendC::SetAtomicAdd<float>();
    // DataCopy属于PIPE_MTE3流水操作。
    AscendC::DataCopy(atomicResultGm, xLocal, this->blockLength);
    // 当本AIV完成前置PIPE_MTE3(DataCopy)流水操作后，通知其他AIV核，本AIV已经完成。
    AscendC::CrossCoreSetFlag<0, PIPE_MTE3>(0);
    // 阻塞本AIV继续往下执行指令，直到其他AIV全部都完成PIPE_MTE3流水操作，才解除阻塞往下执行。
    AscendC::CrossCoreWaitFlag<0>(0);
    // 关闭原子累加。
    AscendC::DisableDmaAtomic();

    if (AscendC::GetBlockIdx() == 0) {
        AscendC::DataCopy(yLocal, atomicResultGm, this->blockLength);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
        AscendC::DataCopy(atomicResultGm, yLocal, this->blockLength);
        return;
    }
```

完整样例请参考[CrossCoreSetFlag和CrossCoreWaitFlag核间同步样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/cross_core_set_wait_flag)。
