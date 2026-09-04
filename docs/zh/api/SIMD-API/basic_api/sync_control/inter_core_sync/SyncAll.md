# SyncAll<a id="ZH-CN_TOPIC_0000001538296817"></a>

## 产品支持情况<a id="section1550532418810"></a>

### 软同步原型

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
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/SyncAll_res.md#id1 -->

### 硬同步（不支持config指定流水）原型

<!-- npu="950" id10 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas 训练系列产品：不支持
<!-- end id16 -->
<!-- npu="x90" id17 -->
- Kirin X90：支持
<!-- end id17 -->
<!-- npu="9030" id18 -->
- Kirin 9030：支持
<!-- end id18 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/SyncAll_res.md#id2 -->

### 硬同步（支持config指定流水）原型

<!-- npu="950" id19 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id19 -->
<!-- npu="A3" id20 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id20 -->
<!-- npu="910b" id21 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id21 -->
<!-- npu="310b" id22 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id22 -->
<!-- npu="310p" id23 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id23 -->
<!-- npu="310p" id24 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id24 -->
<!-- npu="910" id25 -->
- Atlas 训练系列产品：不支持
<!-- end id25 -->
<!-- npu="x90" id26 -->
- Kirin X90：不支持
<!-- end id26 -->
<!-- npu="9030" id27 -->
- Kirin 9030：不支持
<!-- end id27 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/SyncAll_res.md#id3 -->

## 功能说明<a id="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_block_sync_intf.h"`。

SyncAll是核间同步控制接口，根据不同的函数原型介绍其功能：

- 硬件同步接口：
    用于多个核之间的同步控制，支持以下同步场景：
    - 纯Vector算子的全核同步，可参考[CrossCoreSetFlag的模式0](key_features.md#ZH-CN_TOPIC_0000002586300741)。
    - 融合算子的全核同步，一次性实现三个核间同步能力（参考[CrossCoreSetFlag的关键特性说明](key_features.md#ZH-CN_TOPIC_0000002586300741)）：首先完成单个AI Core内，AIC与所有AIV同步（AIC等AIV）；接着完成多个AI Core间，所有AIC的同步；最后完成单个AI Core内，所有AIV与AIC同步（AIV等AIC）。

- 软件同步接口：
    仅支持纯Vector算子的多核（参与同步的核数可通过入参指定）同步。每一个AIV都向全局内存对应位置写入信号值1，接着每一个AIV都持续轮询全局内存中所有AIV对应的信号值，直到所有AIV核对应的信号值都变为1。

在纯Vector算子场景中，若所有AIV核默认参与同步，推荐采用性能更优的硬件同步接口；若需指定部分AIV核参与同步，则应使用软件同步接口，并通过入参usedCores完成配置。

## 函数原型<a id="section620mcpsimp"></a>

- 软同步

    ```cpp
    template <bool isAIVOnly = true>
    __aicore__ inline void SyncAll(const GlobalTensor<int32_t>& gmWorkspace, const LocalTensor<int32_t>& ubWorkspace, const int32_t usedCores = 0)
    ```

- 硬同步
    - 不支持config指定流水

        ```cpp
        template <bool isAIVOnly = true>
        __aicore__ inline void SyncAll()
        ```

    - 支持config指定流水

        ```cpp
        template <bool isAIVOnly = true, const SyncAllConfig& config = DEFAULT_SYNC_ALL_CONFIG>
        __aicore__ inline void SyncAll()
        ```

## 参数说明<a id="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| isAIVOnly | 控制SyncAll作用于纯Vector算子或Mix（包含Cube和Vector计算）算子。可选值：<br>&bull; **true**（默认值）：纯Vector算子的全核同步，仅执行Vector核的全核同步。<br>&bull; **false**：Mix（包含Cube和Vector计算）算子的全核同步，先分别完成Vector核和Cube核的全核同步，再执行两者之间的同步（软同步接口不支持此功能）。 |
| config | **该配置仅当isAIVOnly=True时有效。**<br>struct SyncAllConfig {&#x000A;    pipe_t triggerPipe;&#x000A;    pipe_t waitPipe;&#x000A;};&#x000A;&#x000A;// 默认使用全部流水来进行触发和等待行为。<br>constexpr SyncAllConfig DEFAULT_SYNC_ALL_CONFIG = {PIPE_ALL, PIPE_ALL};<br>控制SyncAll函数的行为，在多个AI Core之间进行流水同步时，指定哪些流水（pipe）用于触发和等待。<br>&bull; **triggerPipe**：指定哪个流水用于“发送触发信号”。仅支持MTE2、MTE3、PIPE_ALL。<br>&bull; **waitPipe**：指定哪个流水用于“接收等待信号”。仅支持MTE2、MTE3、PIPE_ALL。<br>// 多个AIV进行计算&#x000A;<br>Compute();&#x000A;<br>// 多个AIV将计算结果搬出到GM&#x000A;<br>CopyToGM();&#x000A;<br>// 等待所有数据拷贝到GM后，进行累加&#x000A;<br>constexpr AscendC::SyncAllConfig CustomConfig = {PIPE_MTE3, PIPE_ALL};&#x000A;<br>AscendC::SyncAll&lt;true, CustomConfig&gt;();&#x000A;<br>for (int i = 0; i &lt; blockNum; i++) {&#x000A;<br>    if (i != blockIdx) {&#x000A;<br>        CopyFromGm();&#x000A;<br>        Accumulate();&#x000A;<br>    }&#x000A;<br>}&#x000A;<br>// 累加后的数据写回&#x000A;<br>CopyToGm(); |

**表2**  参数说明

| 参数名称 | 输入/输出 | 含义 |
| --- | --- | --- |
| gmWorkspace | 输入 | gmWorkspace为用户定义的全局空间，作为所有核共用的缓存，用于保存每个核的状态标记，类型为GlobalTensor，支持的数据类型为int32_t。GlobalTensor数据结构的定义请参考[GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor.md)。<br>所需空间大小和使用注意项参见[约束说明](#section633mcpsimp)。<br>硬同步接口不支持该参数。 |
| ubWorkspace | 输入 | ubWorkspace为用户定义的局部空间，每个核单独自用，用于标记当前核的状态。<br>类型为[LocalTensor](../../data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN、VECCALC、VECOUT，支持的数据类型为int32_t。<br>所需空间大小参见[约束说明](#section633mcpsimp)。<br>硬同步接口不支持该参数。 |
| usedCores | 输入 | 指定多少个核之间的同步，传入数值不能超过算子调用时指定的逻辑numBlocks。此参数为默认参数，不传此参数表示全核软同步。<br>仅在软同步接口中支持，硬同步接口不支持该参数。 |

## 返回值说明<a id="section91032023123812"></a>

无

## 约束说明<a id="section633mcpsimp"></a>

SyncAll硬件同步和软件同步接口的内部实现不同，约束条件也有所区别。

- 软件同步接口约束：
    - gmWorkspace缓存申请的空间大小要求大于等于GetBlockNum()*32Bytes，并且缓存的值需要初始化为0。目前常见的有两种初始化方式：
        - 通过在host侧进行初始化操作，确保传入该接口时，gmWorkspace缓存已经初始化为0；
        - 在kernel侧初始化的时候对gmWorkspace缓存初始化，需要注意的是，每个核上都需要初始化全部的gmWorkspace缓存空间。
    - ubWorkspace申请的空间大小要求大于等于GetBlockNum()*32Bytes。
    - 在纯Vector算子场景中，若所有AIV核默认参与同步，推荐采用性能更优的硬件同步接口；若需指定部分AIV核参与同步，则应使用软件同步接口，并通过入参usedCores完成配置。
    - usedCores传入数值不能超过算子调用时指定的逻辑AI Core中AIV的数量：GetBlockNum()*GetTaskRatio()，不传此参数表示全核软同步。

- 硬件同步接口约束：
    - 使用硬同步接口时，需根据场景使用合适的核函数（Kernel）修饰符：
        - 在纯Vector算子，需使用\_\_mix\_\_\(0, 1\)修饰核函数（Kernel），而不是使用\_\_vector\_\_，具体原因请参考[CrossCoreSetFlag约束说明](CrossCoreSetFlag_ISASI.md#约束说明)中的核函数（Kernel）类型配置说明。
        - 对于Mix（包含Cube和Vector计算）算子，需使用\_\_mix\_\_\(1, 1\)或\_\_mix\_\_\(1, 2\)修饰核函数（Kernel）。
    - 对于纯Vector算子，模板参数isAIVOnly必须设置为true，否则会导致程序卡死。
    - 对于Mix（包含Cube和Vector计算）算子，模板参数isAIVOnly设置为true，会导致接口实际只能完成Vector全核同步与预期同步行为不符。
    - 使用该接口时，建议开启batchmode模式，使算子独占全部所需核资源，否则可能因满足以下条件导致死锁：
        - 多流并发场景（≥2条执行流）。
        - ≥2个算子并发执行。
        - 所有并发算子的核数总和超过物理核数。
        - ≥2个并发算子使用了核间同步功能。

        具体而言，在多流场景下，某条流的核间同步算子虽分配到n个物理核，但可能仅有n-m个核先被调度执行，而其余m个核因被其他流的核间同步算子抢占而尚未启动。先启动的n-m个核执行到核间同步时等待剩余m核完成，而剩余m核因被其他流的核间同步算子占用而无法释放，形成死锁。
        核函数（Kernel）直调场景下通过[\_\_schedmode\_\_\(mode\)](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)限定符来设置batchmode模式；工程化算子开发场景下，通过TilingContext的SetScheduleMode接口来设置batchmode模式，具体请参考[《基础数据结构和接口》](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/API/basicdataapi/atlasopapi_07_00001.html)。
<a id="syncall_flagId冲突说明"></a>
    - SyncAll硬件同步接口内部实现中使用了[CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md)和[CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md)进行核间同步控制，因此开发者同时使用CrossCoreSetFlag（或CrossCoreWaitFlag）和SyncAll硬件同步接口时，需注意避免以下[flagId](CrossCoreSetFlag_ISASI.md#flagId取值范围说明)冲突：
        <!-- npu="950" id28 -->
        - 针对[NPU架构版本3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)，SyncAll硬件同步接口的flagId占用情况如表3所示：

            **表3**  SyncAll硬件同步接口flagId占用情况

            | SyncAll 使用场景 | 占用flagId汇总 | 各计算核flagId占用情况 |
            | :--- | :--- | :--- |
            | isAIVOnly=true | 14 | AIV：14 |
            | isAIVOnly=false（核函数（Kernel）使用`__mix__(1, 1)`修饰时） | 11、12、13 | AIC：11、12、13；AIV：12、13 |
            | isAIVOnly=false（核函数（Kernel）使用`__mix__(1, 2)`修饰时） | 11、12、13、28、29 | AIC：11、12、13、28、29；AIV：12、13 |

        <!-- end id28 -->
        <!-- npu="A3,910b" id29 -->
        - 针对[NPU架构版本2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)，SyncAll硬件同步接口的flagId占用情况如表4所示：

            **表4**  SyncAll硬件同步接口flagId占用情况

            | SyncAll 使用场景 | 占用flagId汇总 | 各计算核flagId占用情况 |
            | :--- | :--- | :--- |
            | isAIVOnly=true | 14 | AIV：14 |
            | isAIVOnly=false | 11、12、13 | AIC：11、12、13；AIV：12、13 |

        <!-- end id29 -->

- 硬件同步接口和软件同步接口公共约束：使用该接口进行多核控制时，算子调用时指定的逻辑AI Core核数numBlocks必须保证不大于实际运行该算子的AI处理器核数，否则框架进行多轮调度时会插入异常同步，导致核函数（Kernel）“卡死”现象。

## 调用示例<a id="section642mcpsimp"></a>

本示例实现功能为使用8个核进行数据处理，每个核均是处理32个float类型数据，对该数据乘2后再与其他核上进行同样乘2的数据进行相加，中间结果保存到workGm，因此多个核之间需要进行数据同步。此样例中，使用软同步，入口函数传入的syncGm里的值都已经在host侧初始化为0。若以下用例改成使用硬同步，则不需要传入syncGm，并且不需要使用workQueue。

```cpp
// syncGlobal为用户定义的全局空间，作为所有核共用的缓存，类型为GlobalTensor；workLocal为用户定义的局部空间，每个核单独自用，类型为LocalTensor。
int srcDataSize = 256; // 参与计算的元素个数。
int32_t blockNum = AscendC::GetBlockNum(); // 获取核总数。
int32_t blockIdx = AscendC::GetBlockIdx(); // 获取当前工作的核ID。
uint32_t perBlockSize = srcDataSize / blockNum; // 每个核平分处理相同个数。
// 当前工作核计算后的数据先保存到外部工作空间，workGlobal为GlobalTensor，dstLocal为LocalTensor。
AscendC::DataCopy(workGlobal[blockIdx * perBlockSize], dstLocal, perBlockSize);
// 等待所有核都完成计算。
AscendC::SyncAll(syncGlobal, workLocal);
```

完整样例请参考[SyncAll样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/sync_all)。
