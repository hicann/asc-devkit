# 关键特性说明<a name="ZH-CN_TOPIC_0000002555621184"></a>

确定性计算是指在相同输入条件下，无论执行次数或执行环境如何变化，总能产生完全一致输出结果的计算过程。确定性计算为系统稳定性和实验可验证性提供保障。

## 确定性计算概述<a name="ZH-CN_TOPIC_0000002555780816"></a>

为引出原子操作场景下非确定性计算的问题，我们构建如下常见的确定性计算场景：首先，通过单组浮点数数据搬运完成GM的初始化；随后，启动原子累加操作；最后，经由多次数据搬运，在GM上对多组浮点数数据进行累加。具体伪代码如下：

```text
1. 向GM搬运数据data0;    // 数据搬运，覆盖GM原有随机值，期望GM数据为data0。
2. SetAtomicAdd();         // 开启原子累加，后续从Unified Buffer（UB）/L0C Buffer/L1 Buffer到GM的搬运均执行原子累加。
3. 向GM搬运data1;    // 带随路原子操作的数据搬运，期望GM数据为data0 + data1。
4. 向GM搬运data2;    // 带随路原子操作的数据搬运，期望GM数据为data0 + data1 + data2。
5. 向GM搬运data3;    // 带随路原子操作的数据搬运，期望GM数据为data0 + data1 + data2 + data3。
```

如下图1所示，开发者的预期结果：指令发射的顺序能够严格对应实际指令执行顺序，多次执行该段代码，无论执行多少次，最终GM数据均为data0 + data1 + data2 + data3，结果完全一致，实现确定性计算。

**图1**  确定性计算场景，GM上数据变化过程<a name="zh-cn_topic_0000002552941426_fig71821376818"></a>

![](../../../figures/deterministic_computation_gm_data_change_process.png "Deterministic computation scenario, GM data change process")

但实际情况是，若开发者不做干预，程序每次运行时这些指令的执行顺序都可能发生变化，最终导致GM数据与预期结果不一致。下面列举两种可能的指令执行顺序及其对应的执行流程。

## 非确定性计算，结果1<a name="zh-cn_topic_0000002552941426_section027603463315"></a>

**图2**  非确定性计算场景1，GM上数据变化过程<a name="zh-cn_topic_0000002552941426_fig273814664819"></a>

![](../../../figures/non_deterministic_computation_1_gm_data_change_process.png "Non-deterministic computation scenario 1, GM data change process")

如图2所示，该场景中指令执行流程如下：

1. 初始状态，GM数据为：随机值；
2. 向GM搬运data0，GM数据被初始化为：data0；
3. 执行SetAtomicAdd，为后续搬运指令开启原子累加，GM数据为：data0；
4. 三次带随路原子操作的搬运指令乱序，实际执行顺序为"搬出data2→搬出data3→搬出data1"，最终GM上数据为：data0 + data2 + data3 + data1。

**非确定性计算的产生原因1：**

带随路原子操作的搬运指令乱序，由于浮点数加法不满足结合律，即\(a+b\)+c!=a+\(b+c\)，使得最终GM数据data0 + data2 + data3 + data1与预期的data0 + data1 + data2 + data3存在偏差。

带随路原子操作的搬运指令乱序，导致最终结果产生偏差的前提条件有以下三条：

- 原子操作类型为原子累加（最大值、最小值运算满足结合律）
- 原子操作数据类型为浮点数（整数加法满足结合律）
- 带随路原子操作的搬运指令达到3条及以上（浮点数加法满足交换律，但是不满足结合律）。

## 非确定性计算，结果2<a name="zh-cn_topic_0000002552941426_section1547411390818"></a>

**图3**  非确定性计算场景2，GM上数据变化过程<a name="zh-cn_topic_0000002552941426_fig1829915169912"></a>

![](../../../figures/non_deterministic_computation_2_gm_data_change_process.png "Non-deterministic computation scenario 2, GM data change process")

如图3所示，该场景中指令执行流程如下：

1. 初始状态，GM数据为：随机值；
2. 执行SetAtomicAdd，为后续搬运指令开启原子累加，GM数据为：随机值；
3. 先后执行两次带随路原子操作的搬运指令，执行顺序为“搬出data1→搬出data2”，GM数据为：随机值 + data1 + data2；
4. 向GM搬运data0，GM上累加的结果被data0覆盖，GM数据为：data0；
5. 最后执行data3的搬运，最终GM上数据为：data0 + data3。

**非确定性计算的产生原因2：**

开启原子累加前的普通搬运指令与开启原子操作的搬运指令之间发生乱序，会导致GM上已完成原子操作的数据被data0错误覆盖，进而产生非确定性计算结果。

此类乱序导致结果偏差无需满足任何前提条件，开发者无需再区分原子操作类型、原子操作数据类型，也无需考虑带随路原子操作的搬运指令数量是否达到3条及以上。

## 确定性计算实现方案<a name="ZH-CN_TOPIC_0000002586300751"></a>

根据导致非确定性计算的两个根因，下面也从解决这两个方面描述确定性计算实现的方案。核心思想是在指令之间插入适当的同步，使每次程序运行时相关指令都按照预期确定的顺序执行，最终保证每次执行程序输出的结果都相同。具体来说包含以下两个方面：

- 开启原子累加前的搬运指令与开启原子操作的指令之间插入同步

    如下伪代码所示，在指令1与2之间插入同步，能够确保开始原子操作前GM的初始值符合预期。

- 开启原子累加操作后，多条搬运指令之间的同步

    指令3与4、4与5之间插入同步，能够确保浮点数累加的顺序符合预期。

    >[!CAUTION]注意
    >开启原子操作的指令与后续搬运指令之间不需要开发者插入同步。

```text
// 整个原子累加在同一个核内执行，控制5个指令的执行顺序为"1→2→3→4→5"。
1. 向GM搬运数据data0;    // 数据搬运，覆盖GM原有随机值，期望GM数据为data0。
核内同步
2. SetAtomicAdd();         // 开启原子累加，后续从UB/L0C Buffer/L1 Buffer到GM的搬运均执行原子累加。
// 指令2与3间无需插入同步。
3. 向GM搬运data1;    // 开启原子累加后的数据搬运，期望GM数据为data0 + data1。
核内同步
4. 向GM搬运data2;    // 开启原子累加后的数据搬运，期望GM数据为data0 + data1 + data2。
核内同步
5. 向GM搬运data3;    // 开启原子累加后的数据搬运，期望GM数据为data0 + data1 + data2 + data3。
```

如下伪代码所示，当上述指令都在不同核中执行时，需要将上述的核内同步替换为核间同步。

```text
// 整个原子累加在4个不同核中执行，控制4个核执行的顺序为“核0→核1→核2→核3”。
if (GetBlockIdx() == 0) {
   向GM搬运数据data0;
   核间同步
} else if (GetBlockIdx() == 1) {
   核间同步
   SetAtomicAdd();
   向GM搬运data1;
   核间同步
} else if (GetBlockIdx() == 2) {
   核间同步
   SetAtomicAdd();
   向GM搬运data2;
   核间同步
} else if (GetBlockIdx() == 3) {
   核间同步
   SetAtomicAdd();
   向GM搬运data3;
}
```

下面介绍如何基于硬件同步指令实现核内同步，以及如何基于软件同步方案实现核间同步。

## 核内同步<a name="zh-cn_topic_0000002583421469_section6896184932119"></a>

搬运指令和开启原子操作的指令流水类型如下表所示，当上述指令在同一个核内执行时，开发者按需插入[单流水同步](../sync_control/intra_core_sync/PipeBarrier_ISASI.md)或者[多流水同步](../sync_control/intra_core_sync/SetFlag_WaitFlag_ISASI.md)，可参考[样例中scenarioNum=1的分支](#zh-cn_topic_0000002583421469_example1)。

**表1**  原子操作确定性计算相关指令的流水类型

| 指令名称 | 流水类型 |
| --- | --- |
| DataCopy | PIPE_MTE3 |
| Fixpipe | PIPE_FIX |
| SetAtomicAdd/SetAtomicMax/SetAtomicMin | PIPE_S |

## 核间同步<a name="zh-cn_topic_0000002583421469_section331316523218"></a>

由于当前未提供用于控制不同核之间执行顺序的硬件同步接口，因此确定性计算场景下的核间同步需通过软件仿真实现。针对纯Vector算子、纯Cube算子及Mix（同时包含Vector与Cube计算）算子三种场景，所采用的软件同步方案存在差异，具体如下表所示。

**表2**  核间同步软件实现方案

| 算子类型 | 软件同步方案 | 说明 |
| --- | --- | --- |
| 纯Vector算子 | 方案1：通过多对[IBSet](../sync_control/inter_core_sync/IBSet.md)和[IBWait](../sync_control/inter_core_sync/IBWait.md)的接口组合，可实现多个AIV间的同步，可参考[样例中scenarioNum=2的分支](#zh-cn_topic_0000002583421469_example2)。| 方案1支持指定部分AIV参与同步，并可控制各AIV的执行顺序。|
| 纯Vector算子 | 方案2：通过[InitDetermineComputeWorkspace](../sync_control/inter_core_sync/InitDetermineComputeWorkspace.md)、[NotifyNextBlock](../sync_control/inter_core_sync/NotifyNextBlock.md)和[WaitPreBlock](../sync_control/inter_core_sync/WaitPreBlock.md)三个接口配合使用，确保所有AIV核按照blockIdx升序执行。| 方案2要求所有AIV必须参与同步，且执行顺序固定为blockIdx升序。|
| 纯Cube算子 | 通过GM中的信号量实现核间同步，先建立一对AIC核之间的同步，进而扩展至多个AIC核之间的同步，可参考[样例中scenarioNum=3的分支](#zh-cn_topic_0000002583421469_example3)。| 通过Scalar单元访问GM时，需考虑多核间数据一致性问题。|
| Mix算子 | 通过GM中的信号量实现核间同步，先建立一对核之间的同步，进而扩展至多个核之间的同步。| 通过Scalar单元访问GM时，需考虑多核间数据一致性问题。此处的“核”可以是AIV或者AIC。|

下图展示了两个核之间如何通过GM中的信号量进行核间同步：

- 上一个核完成数据搬运或开启原子操作后，会通过Scalar单元向核间共享的GM中的信号量写入值1，表示自己的任务已完成。上一个核中也需要插入核内同步：
    - 当上一个核内有多条搬运指令时，它们之间需要插入核内同步1。
    - Scalar单元向GM写数据之前必须保证前序所有搬运指令都已经执行完成，因此它们之间也需要插入核内同步2。

- 当前核在执行搬运任务前，会通过Scalar单元不断读取该信号量的值。如果信号量不等于1，当前核会进入阻塞等待状态；当检测到信号量等于1时，当前核会解除阻塞，开始执行自己的数据搬运或原子操作。为确保信号量等于1之前，当前核不会执行搬运指令，需要在搬运指令之前插入核内同步3。

**图4**  一对核之间软件同步方案流程图<a name="zh-cn_topic_0000002583421469_fig1491417244811"></a>

![](../../../figures/software_synchronization_between_two_cores_flowchart.png "Software synchronization flowchart between two cores")

Scalar单元访问GM上的信号量，存在两种访问方式：

- 经过DCache访问

    使用GlobalTensor的成员函数GetValue和SetValue进行操作。这种情况下，开发者需要手动调用[DataCacheCleanAndInvalid](../cache_control/DataCacheCleanAndInvalid.md)接口，以确保多核间数据的一致性。

- 不通过DCache访问

    使用[WriteGmBypassDCache](../scalar_compute/WriteGmBypassDCache_ISASI.md)和[ReadGmBypassDCache](../scalar_compute/ReadGmBypassDCache_ISASI.md)。这种方式无需额外操作即可保证多核间数据的一致性。

两种方案在性能上的差异：不经过DCache，性能会较差，但是如果读写GM数据较少，可以考虑使用不经过DCache的方法。

如图4所示，核间同步方案中也需要与核内同步配合使用，现将三处核内同步作用说明如下：

- 核内同步1（可选）：核内存在多条数据搬运指令时，通过该同步保证各搬运操作严格按顺序执行。
- 核内同步2（必备）：等待前一个核全部任务执行完毕后，才允许Scalar单元向全局内存信号量写入1。
- 核内同步3（必备）：等待Scalar单元检测到信号量更新为1后，当前核再启动后续任务执行。

## 调用示例<a name="zh-cn_topic_0000002583421469_section027603463315"></a>

- **<a name="zh-cn_topic_0000002583421469_example1"></a>核内同步确定性计算（对应scenarioNum=1）：所有指令在同一个AIV内执行**

    ```cpp
    // 在原子累加之前，以src0Local初始化GM。
    AscendC::DataCopy(dstGlobal, src0Local, DATA_LENGTH);
    // 确保在原子累加之前，gm的初始化完成。
    AscendC::PipeBarrier<PIPE_MTE3>();

    // 开启原子累加。
    // 预期累加执行的顺序是：src2Local + src3Local + src1Local。
    AscendC::SetAtomicAdd<float>();
    AscendC::DataCopy(dstGlobal, src2Local, DATA_LENGTH);
    // 两条搬运指令之间需要调用PipeBarrier<PIPE_MTE3>()，确保src2Local和src3Local的搬运顺序正确。
    AscendC::PipeBarrier<PIPE_MTE3>();
    AscendC::DataCopy(dstGlobal, src3Local, DATA_LENGTH);
    AscendC::PipeBarrier<PIPE_MTE3>();
    AscendC::DataCopy(dstGlobal, src1Local, DATA_LENGTH);

    AscendC::DisableDmaAtomic();
    // GM结果：data0 + data2 + data3 + data1。
    ```

- **<a name="zh-cn_topic_0000002583421469_example2"></a>纯Vector算子核间同步（对应scenarioNum=2）：四次搬运分别在四个不同的AIV内执行**

    ```cpp
    // 预期的4个核执行的顺序是核0-->核2-->核3-->核1。
    // 核0负责在原子累加之前，以src0Local初始化GM；核2、3、1负责按顺序做原子累加。

    if (blockIdx == 0) {
        AscendC::LocalTensor<float> src0Local = inQueueSrc0.AllocTensor<float>();
        AscendC::DataCopy(src0Local, src0Global, DATA_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        // 在原子累加之前，以src0Local初始化GM。
        AscendC::DataCopy(dstGlobal, src0Local, DATA_LENGTH);

        // 核0通知核2，核0的src0Local初始化已完成。
        auto sync_buf = vecIn.AllocTensor<int32_t>();
        AscendC::IBSet(src4Global, sync_buf, 0, 0); // 最后两个参数：int32_t blockIdx, int32_t eventID。
        inQueueSrc0.FreeTensor(src0Local);
        vecIn.FreeTensor(sync_buf);
        AscendC::printf("核的下标%d\n", blockIdx);
    } else if (blockIdx == 1) {
        AscendC::LocalTensor<float> src1Local = inQueueSrc1.AllocTensor<float>();
        // 核1等待核3，直到核3数据已完成累加。
        auto sync_buf = vecIn.AllocTensor<int32_t>();
        AscendC::IBWait(src4Global, sync_buf, 3, 0); // 最后两个参数：int32_t blockIdx, int32_t eventID。

        AscendC::DataCopy(src1Local, src1Global, DATA_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        AscendC::DisableDmaAtomic();
        AscendC::SetAtomicAdd<float>();
        AscendC::DataCopy(dstGlobal, src1Local, DATA_LENGTH);
        AscendC::DisableDmaAtomic();
        vecIn.FreeTensor(sync_buf);
        inQueueSrc1.FreeTensor(src1Local);
        AscendC::printf("核的下标%d\n", blockIdx);
    } else if (blockIdx == 2) {
        AscendC::LocalTensor<float> src2Local = inQueueSrc2.AllocTensor<float>();
        // 核2等待核0，直到核0已完成GM初始化。
        auto sync_buf = vecIn.AllocTensor<int32_t>();
        AscendC::IBWait(src4Global, sync_buf, 0, 0); // 最后两个参数：int32_t blockIdx, int32_t eventID。

        AscendC::DataCopy(src2Local, src2Global, DATA_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        AscendC::DisableDmaAtomic();
        AscendC::SetAtomicAdd<float>();
        AscendC::DataCopy(dstGlobal, src2Local, DATA_LENGTH);
        AscendC::DisableDmaAtomic();

        // 核2通知核3，核2数据已经累加完成。
        AscendC::IBSet(src4Global, sync_buf, 2, 0);
        vecIn.FreeTensor(sync_buf);
        inQueueSrc2.FreeTensor(src2Local);
        AscendC::printf("核的下标%d\n", blockIdx);
    } else if (blockIdx == 3) {
        // 核3等待核2，直到核2数据已完成累加。
        auto sync_buf = vecIn.AllocTensor<int32_t>();
        AscendC::IBWait(src4Global, sync_buf, 2, 0); // 最后两个参数：int32_t blockIdx, int32_t eventID。

        AscendC::LocalTensor<float> src3Local = inQueueSrc3.AllocTensor<float>();
        AscendC::DataCopy(src3Local, src3Global, DATA_LENGTH);
        //  AscendC::DumpTensor(src3Local,0,8);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        AscendC::DisableDmaAtomic();
        AscendC::SetAtomicAdd<float>();
        AscendC::DataCopy(dstGlobal, src3Local, DATA_LENGTH);
        AscendC::DisableDmaAtomic();

        // 核3通知核1，核3数据已经累加完成。
        AscendC::IBSet(src4Global, sync_buf, 3, 0);
        vecIn.FreeTensor(sync_buf);
        inQueueSrc3.FreeTensor(src3Local);
        AscendC::printf("核的下标%d\n", blockIdx);
    }
    // GM结果：data0 + data2 + data3 + data1。
    ```

- **<a name="zh-cn_topic_0000002583421469_example3"></a>纯Cube核间同步（对应scenarioNum=3）：四次搬运分别在四个不同的AIC内执行**

    ```cpp
    // 预期的4个核执行的顺序是核0-->核2-->核3-->核1。
    // 核0负责在原子累加之前，以src0Local初始化GM；核2、3、1负责按顺序做原子累加。
    if (blockIdx == 0) {
        AscendC::DataCopy(src0Local, src0Global, DATA_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        // 在原子累加之前，以src0Local初始化GM。
        AscendC::DataCopy(dstGlobal, src0Local, DATA_LENGTH);

        // 核0通知核2，核0的src0Local初始化已完成。
        // 在通知下一个核之前，等待当前核的任务完成。
        AscendC::SetFlag<AscendC::HardEvent::MTE3_S>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE3_S>(0);
        AscendC::WriteGmBypassDCache<int32_t>(addr + blockIdx * 32, 1);
    } else if (blockIdx == 1) {
        int32_t preblockIdx = 3;

        // 核1等待核3，直到核3数据已完成累加。
        while (true) {
            int32_t value = AscendC::ReadGmBypassDCache<int32_t>(addr + preblockIdx * 32);
            if (value == 1) {
                AscendC::WriteGmBypassDCache<int32_t>(addr + preblockIdx * 32, 0);
                break;
            }
        }
        // 开始当前核的任务之前，等待上一个核的通知。
        AscendC::SetFlag<AscendC::HardEvent::S_MTE2>(0);
        AscendC::WaitFlag<AscendC::HardEvent::S_MTE2>(0);

        AscendC::DataCopy(src1Local, src1Global, DATA_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        AscendC::DisableDmaAtomic();
        AscendC::SetAtomicAdd<float>();
        AscendC::DataCopy(dstGlobal, src1Local, DATA_LENGTH);
        AscendC::DisableDmaAtomic();
    } else if (blockIdx == 2) {
        int32_t preblockIdx = 0;

        // 核2等待核0，直到核0已完成GM初始化。
        while (true) {
            int32_t value = AscendC::ReadGmBypassDCache<int32_t>(addr + preblockIdx * 32);
            if (value == 1) {
                AscendC::WriteGmBypassDCache<int32_t>(addr + preblockIdx * 32, 0);
                break;
            }
        }
        // 开始当前核的任务之前，等待上一个核的通知。
        AscendC::SetFlag<AscendC::HardEvent::S_MTE2>(0);
        AscendC::WaitFlag<AscendC::HardEvent::S_MTE2>(0);

        AscendC::DataCopy(src2Local, src2Global, DATA_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        AscendC::DisableDmaAtomic();
        AscendC::SetAtomicAdd<float>();
        AscendC::DataCopy(dstGlobal, src2Local, DATA_LENGTH);
        AscendC::DisableDmaAtomic();

        // 在通知下一个核之前，等待当前核的任务完成。
        AscendC::SetFlag<AscendC::HardEvent::MTE3_S>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE3_S>(0);
        AscendC::WriteGmBypassDCache<int32_t>(addr + blockIdx * 32, 1);
    } else if (blockIdx == 3) {
        int32_t preblockIdx = 2;

        // 核3等待核2，直到核2数据已完成累加。
        while (true) {
            int32_t value = AscendC::ReadGmBypassDCache<int32_t>(addr + preblockIdx * 32);
            if (value == 1) {
                AscendC::WriteGmBypassDCache<int32_t>(addr + preblockIdx * 32, 0);
                break;
            }
        }

        // 开始当前核的任务之前，等待上一个核的通知。
        AscendC::SetFlag<AscendC::HardEvent::S_MTE2>(0);
        AscendC::WaitFlag<AscendC::HardEvent::S_MTE2>(0);

        AscendC::DataCopy(src3Local, src3Global, DATA_LENGTH);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(0);

        AscendC::DisableDmaAtomic();
        AscendC::SetAtomicAdd<float>();
        AscendC::DataCopy(dstGlobal, src3Local, DATA_LENGTH);
        AscendC::DisableDmaAtomic();

        // 在通知下一个核之前，等待当前核的任务完成。
        AscendC::SetFlag<AscendC::HardEvent::MTE3_S>(0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE3_S>(0);
        AscendC::WriteGmBypassDCache<int32_t>(addr + blockIdx * 32, 1);
    }
    // GM结果：data0 + data2 + data3 + data1。
    ```

完整样例请参考：[数据搬运随路原子操作做确定性计算的样例](../../../../../../examples/01_simd_cpp_api/03_basic_api/06_atomic/set_atomic_deterministic_computation)。
