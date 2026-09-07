# L2 Cache切分<a name="ZH-CN_TOPIC_0000001893038949"></a>

【优先级】：高

【描述】假设，AI处理器的L2 Cache大小为192MB，L2 Cache读写混合带宽约为7TB/s，而GM的带宽约为1.6TB/s，两者之间存在较大差距。搬入或搬出相同数据量的情况下，访问L2 Cache读写数据比GM更快。若数据无法命中L2 Cache，即需要访问的数据不在L2 Cache内，导致需要去GM上读写，带宽利用效率较低，最终算子搬入或搬出数据变为算子整个运行过程的性能瓶颈。切分策略建议：当输入和输出数据的数据量超过L2 Cache大小时，Tiling中开启L2 Cache切分策略。

优化的核心思路是：**通过开启L2 Cache切分策略，将单轮处理的数据规模控制在L2 Cache范围内，提升数据在L2 Cache中的复用率，从而减少重复的数据搬入或搬出**。本文及配套样例将L2 Cache可容纳的有效数据量阈值设为100MB，该取值仅用于样例展示。

## 场景设定

- 使用20个Vector Core并行计算，每个元素执行两轮“加1”运算，最终结果为`data + 2`。
- 反例不做L2 Cache切分，每轮把384MB数据作为一个整体进行搬入、计算、搬出。
- 正例将384MB数据切分成B0至B3四个L2 Batch，每批包含96MB的数据量，每一批进行两轮加1计算。
- 完整实现方法请参考[add_l2_cache_split样例](../../../../../../examples/01_simd_cpp_api/03_basic_api/10_cache_control/add_l2_cache_split)。

两个场景的数据处理方式如下图所示。

**图 1**  未开启L2 Cache切分<a name="fig_l2_cache_no_split"></a>

![](../../../figures/no_l2_cache_split.png "未开启L2-Cache切分")

**图 2**  开启L2 Cache切分<a name="fig_l2_cache_split"></a>

![](../../../figures/l2_cache_split.png "开启L2-Cache切分")

## 反例：不做L2 Cache切分

反例中不做数据切分，直接处理384MB数据整体。L2 Cache对用户不可见，用户可感知数据从GM搬运到UB的过程为：384MB总数据由20个核均分，平均每个核搬运约19.2MB。

**【用户视角】**

只感知GM到UB搬运过程，384MB总数据由20个核均分搬运，每个核搬运19.2MB。

**【实际数据路径】**

输入数据会先从GM填充到L2 Cache，再到UB。

**图 3**  GM搬运到UB路径<a name="fig_l2_cache_gm_to_ub"></a>

![](../../../figures/l2cache_to_UB.png "GM搬运到UB路径")

**【搬运填充过程】**

假设20个核处理的数据按核序逐段搬入L2 Cache，L2 Cache有效数据阈值为100MB，则L2 Cache内约可容纳5个核的数据量。

**图 4**  数据搬运填充L2 Cache过程<a name="fig_l2_cache_fill_process"></a>

![](../../../figures/l2cache_fill_process.png "数据搬运填充L2 Cache过程")

后续新数据填充L2 Cache时，可能替换核0~核4早期Cache Line。

**【不切分反例的执行时序】**

**图 5**  未开启L2 Cache切分反例<a name="fig_l2_cache_no_split_example"></a>

![](../../../figures/l2cache_no_split_example.png "未开启L2 Cache切分反例")

在第1轮（R0）处理384MB后半段数据时，后续搬入的新数据可能会替换先前搬入的数据。由于R0是首轮访问，仅比较一轮搬运计算不能稳定体现L2 Cache命中收益；第2轮（R1）再次访问384MB数据时，先前的数据可能已经不在L2 Cache中，因此更可能从GM重新搬运。

## 正例：按L2 Cache切分

在正例中，根据阈值100MB， 对384MB数据进行切分，分成4个Batch，每份96MB。每轮20核共处理96MB数据，每轮数据复用，这样每次计算前读取的数据能够命中L2 Cache，提升算子性能。
正例的循环层级为`L2 Batch -> Round`。同一批连续完成全部轮次后，再切换到下一批：

```text
L2 Batch数      = ceil(总数据量 / L2 Cache有效数据阈值)
                = ceil(384MB / 100MB)
                = 4
单个L2 Batch大小 = 384MB / 4 = 96MB
```

```cpp
for (uint32_t l2BatchIdx = 0; l2BatchIdx < L2_BATCH_NUM; ++l2BatchIdx) {
    for (uint32_t roundIdx = 0; roundIdx < COMPUTE_ROUND_NUM; ++roundIdx) {
        ProcessL2Batch(l2BatchIdx);
        AscendC::SyncAll();
    }
}
```

**【L2 Cache切分正例的执行时序】**

**图 6**  开启切分正例<a name="fig_l2_cache_split_example"></a>

![](../../../figures/l2cache_split_example.png "开启切分正例")

同一Batch的两轮访问之间，没有插入其他超过100MB有效阈值的大数据，因此第2轮更有机会命中第1轮留在L2 Cache中的数据。<br>
B0的数据量为96MB，不超过100MB阈值；B0在两次访问之间也没有其他大数据插入更新L2 Cache，因此R1更有机会从L2 Cache读取R0的结果。B0处理完毕后再依次处理B1至B3，原理相同。

**【性能观察重点】**

只执行一轮计算：R0是首轮访问，反例和正例都需要从GM首次搬入数据，Cache Miss差异通常不明显。<br>
正反例通过对L2 Cache中数据复用程度体现性能优化：R1再次访问同一批地址时，目标数据是否仍在L2 Cache中，才是减少Cache Miss的关键。<br>

反例的循环层级为`Round -> Whole Data`。外层每轮都处理完整384MB范围：

```cpp
for (uint32_t roundIdx = 0; roundIdx < COMPUTE_ROUND_NUM; ++roundIdx) {
    ProcessWholeData();
    AscendC::SyncAll();
}
```

**表 1**  样例数据量

| 数据量 | 计算方法 | 结果 |
| ------ | ------ | -----: |
| 总数据量 | 样例固定值 | 384MB |
| 总元素数 | `384MB / sizeof(half)` | 201326592 |
| L2 Cache有效数据阈值 | 样例固定值 | 100MB |
| 反例单阶段全核数据 | 不做L2 Cache切分 | 384MB |
| 正例L2 Batch数 | `ceil(384MB / 100MB)` | 4 |
| 正例单个L2 Batch数据量 | `384MB / 4` | 96MB |
| 正例单个L2 Batch元素数 | `96MB / sizeof(half)` | 50331648 |

## 为什么不切分会降低复用概率

以本文100MB阈值为例，核0至核4合计处理约96MB，已经接近L2 Cache有效容量；当核5及后续核继续搬入新数据时，总数据量超过100MB阈值，核0等较早进入L2 Cache的数据就可能被替换。也就是说，替换风险在第1轮处理过程中已经产生，不是等到第2轮才产生。如果算子在同一轮后续步骤就复用早期地址，这种替换会在单轮内直接表现为Cache Miss。<br>
文档中设计两轮计算，是为了显式体现数据复用：第2轮再次访问第1轮刚处理过的同一批地址。不切分时，第2轮重新访问`data[0]`所在地址范围，先前数据可能已经在第1轮后续扫描中被替换，因此更可能发生Cache Miss并从GM重新取数。<br>
L2 Cache切分正例将单个循环需要处理的数据量限制在96MB。单个Batch内，20个核合计覆盖的数据量不超过100MB阈值，每个核只处理约4.8MB。先在`data[B0]`上完成第1轮和第2轮计算，再依次处理B1至B3。同一批两轮访问之间没有插入其他大数据更新L2 Cache，因此第2轮更有机会直接命中第1轮刚写回的L2 Cache数据。<br>

## 公共处理逻辑

两种实现都使用同一份`data` Buffer，并将结果写回搬入时的同一GM地址。差异在于传给公共处理函数的连续数据范围不同：反例传入完整384MB范围，正例传入单个96MB L2 Batch范围。以下代码仅展示实际样例采用的32字节DataBlock分核公式和正反例传入范围差异，不展开单核内部搬运和计算细节。

```cpp
__aicore__ inline void ProcessWholeData()
{
    ProcessDataRange(0, TOTAL_DATA_ELEMENTS);
}

__aicore__ inline void ProcessL2Batch(uint32_t l2BatchIdx)
{
    ProcessDataRange(l2BatchIdx * L2_BATCH_ELEMENTS, L2_BATCH_ELEMENTS);
}

__aicore__ inline void ProcessDataRange(int64_t rangeOffsetInData, uint32_t rangeElementNum)
{
    const uint32_t coreIdx = AscendC::GetBlockIdx();
    const uint32_t rangeDataBlockNum = rangeElementNum / ELEMENTS_PER_DATA_BLOCK;
    const uint32_t baseDataBlocksPerCore = rangeDataBlockNum / VECTOR_CORE_NUM;
    const uint32_t coresWithExtraDataBlock = rangeDataBlockNum % VECTOR_CORE_NUM;
    const uint32_t extraDataBlockNumBeforeCore =
        coreIdx < coresWithExtraDataBlock ? coreIdx : coresWithExtraDataBlock;
    const uint32_t coreDataBlockNum =
        baseDataBlocksPerCore + (coreIdx < coresWithExtraDataBlock ? 1U : 0U);
    const uint32_t coreDataBlockOffset =
        coreIdx * baseDataBlocksPerCore + extraDataBlockNumBeforeCore;
    const uint32_t coreElementNum = coreDataBlockNum * ELEMENTS_PER_DATA_BLOCK;
    const int64_t coreOffsetInData =
        rangeOffsetInData + static_cast<int64_t>(coreDataBlockOffset) * ELEMENTS_PER_DATA_BLOCK;
    ProcessCoreRange(coreOffsetInData, coreElementNum);
}

// 单核内部搬运和计算流程以配套样例完整实现为准。
```

对于不切分反例的384MB整体范围，前12个核各处理629146个DataBlock，其余8个核各处理629145个DataBlock，单核数据量约19.2MB，20个核合计覆盖完整`data[0, 384MB)`范围且互不重叠。对于正例的单个96MB L2 Batch，前8个核各处理157287个DataBlock，其余12个核各处理157286个DataBlock，单核数据量约4.8MB，20个核合计覆盖完整L2 Batch且互不重叠。每个DataBlock包含16个`half`元素，因此所有`DataCopy`搬运的数据长度均满足32字节对齐要求。完整核内流水同步实现以配套样例为准；阶段之间的核间同步由配套样例的`KernelAdd::SyncAllCores`函数直接调用API`AscendC::SyncAll()`接口完成，不是额外封装的自定义同步函数。

## 正反例对比总结

**表 2**  正反例对比

| 维度 | 反例 | 正例 |
| ------ | ------ | ------ |
| L2 Cache切分 | 不切分 | 切分为4个96MB L2 Batch |
| 循环层级 | `Round -> Whole Data` | `L2 Batch -> Round` |
| 单阶段全核数据量 | 384MB | 96MB |
| 单核平均处理量 | 约19.2MB | 约4.8MB |
| 执行时间线 | `R0/data[0,384MB) -> R1/data[0,384MB)` | `B0/R0 -> B0/R1 -> ... -> B3/R0 -> B3/R1` |
| 同一批两次访问之间 | 一轮内20核已合计扫描完整384MB范围 | 不插入其他超过阈值的大数据范围 |
| L2 Cache数据替换风险 | 较高 | 较低 |
| 第2轮L2 Cache命中率 | 预期较低，以实测为准 | 预期较高，以实测为准 |

## 适用场景

- 算子总数据量超过可用的L2 Cache有效阈值，且同一批数据需要被多次读取。
- 只读取一次的数据不存在跨Round复用，调整L2 Cache切分顺序通常收益有限。

## 测试用例设计

测试正反例使用相同的384MB的GM数据Buffer、核数、Round数和计算逻辑。唯一核心差异是反例不做L2 Cache切分，正例中输入数据按100MB有效数据阈值切分为4个96MB L2 Batch。

### 正反例用例

**表 3**  正反例用例

| 用例 | 循环层级 | 数据元素数 | 核数 | Round数 | 验证目的 |
| ------ | ------ | ------ | -------: | ------ | -----: |
| 反例no-split | `Round -> Whole Data` | 201326592 | 20 | 2 | 验证20核合计384MB数据量超过L2 Cache有效阈值时的替换风险 |
| 正例L2-split | `L2 Batch -> Round` | 201326592 | 20 | 2 | 验证单个96MB Batch连续复用带来的L2 Cache局部性改善 |

正例每个L2 Batch包含50331648个`half`元素，占96MB。以32字节DataBlock为最小单位分配给20个Vector Core，前8个核各处理2516592个元素，其余12个核各处理2516576个元素。反例每轮直接处理完整384MB范围，20个核共同覆盖`data[0,384MB)`。所有`DataCopy`长度均保持32字节对齐。

### 验证检查点

**表 4**  验证检查点

| 检查项 | 验证方法 | 预期结果 |
| ------ | ------ | ------ |
| L2 Cache命中率 | 独立进程采集L2 Cache读命中指标 | 正例中位数通常高于反例，具体字段按架构和工具版本解释 |
| 搬运效率 | 对比MTE2耗时和有效带宽 | 正例MTE2耗时中位数通常降低，有效带宽通常提升 |
| 端到端耗时 | 正反例各采集多组独立Profile并比较`Task Duration(us)`中位数 | 搬运受限场景下，正例耗时通常低于反例 |

> **说明**：`aiv_gm_to_ub_bw(GB/s)`表示有效带宽，不表示GM读取量。优化后有效带宽可能升高，不能以“带宽更低”作为预期。性能采集应交替安排正反例的先后顺序，并使用多组结果的中位数降低系统波动影响。
