# SIMD与SIMT混合编程HashTable MTE队列优化样例

## 概述

本样例比较HashTable`insert_or_assign`的两种value写入路径：SIMT Warp直接写value，以及单kernel内使用MTE任务队列写value。样例实现一个固定容量的C++风格HashTable，仅用于展示本次优化路径，保留本次对比需要的语义和数据结构。

**优化路径**：

| Case | SCENARIO_NUM | 实现方式 | 说明 |
|:---|:---:|:---|:---|
| Case 0 | 0 | SIMT Warp直接写value | 一个Warp完成hash/probe/CAS后，由Warp内线程分片写入一条value向量，作为对照实现。 |
| Case 1 | 1 | MTE task queue搬运value | 单kernel内使用task-id ring queue，SIMT持续生产UB任务，MTE按任务批量搬运value，并与后续SIMT探测形成流水。 |

两种Case的数据处理流水如下图。

![HashTable MTE value movement pipeline](./figures/hash_table_mte_pipeline.png)

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## 目录结构介绍

```text
├── simd_simt_hash_table_mte_queue
│   ├── CMakeLists.txt                // 编译工程文件
│   ├── figures                       // README中的图片资源
│   ├── hash_table_mte_queue.asc      // Ascend C样例实现
│   ├── README.md                     // 样例说明文档
│   └── scripts
│       └── run.sh                    // 编译并使用msOpProf采集两种Case性能数据
```

## 样例描述

**样例功能**：

样例实现一个固定容量HashTable，支持与C++17`std::unordered_map::insert_or_assign`类似的插入或更新语义：key已存在时更新value，key不存在时插入key/value。Hash冲突通过线性探测处理。

需要注意的是，本样例当前面向单批次唯一key插入场景做实现与性能对比，不支持对同一批输入中的重复key做去重或顺序语义保证。若同一批次中出现重复key，最终结果不在本样例约束范围内。

HashTable由两段GM存储组成：`table[bucket_count]`保存bucket元数据，其中每个bucket只保存key和状态；`table_values[bucket_count, dim]`保存value数据，其中第`bucket_idx`行保存`table[bucket_idx]`对应key的value向量。key和value不在同一结构体内连续存放，而是通过相同的`bucket_idx`关联；每条value向量长度为`dim`，`dim`为运行时参数。

bucket的key字段同时承载占用信息：

| key字段取值 | 含义 |
|:---|:---|
| `EMPTY_KEY` | 空bucket，可以被插入流程CAS锁定。 |
| `LOCKED_KEY` | bucket正在插入或更新，其他线程需要等待key发布后重新判断。 |
| 真实key | bucket已发布，对find可见。 |

插入或更新过程中，kernel先将目标bucket的key从`EMPTY_KEY`或真实key CAS为`LOCKED_KEY`，再写入value，最后发布真实key。find只匹配真实key，因此不会观察到“key已经可见但value尚未写完”的中间结果。

HashTable插入流程如下：

$$
bucket = hash(key) \& (capacity - 1)
$$

$$
bucket = (bucket + 1) \& (capacity - 1), \quad \text{when collision occurs}
$$

**样例规格**：

<table>
<tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">HashTable insert_or_assign</td></tr>
<tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">keys</td><td align="center">[128 * 1024]</td><td align="center">int64</td><td align="center">ND</td></tr>
<tr><td align="center">values</td><td align="center">[128 * 1024, dim]</td><td align="center">float32</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">样例输出</td><td align="center">table keys</td><td align="center">[256 * 1024]</td><td align="center">int64</td><td align="center">ND</td></tr>
<tr><td align="center">table values</td><td align="center">[256 * 1024, dim]</td><td align="center">float32</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">核函数名</td><td align="center">SCENARIO_NUM=0</td><td colspan="3" align="center"><code>insert_or_assign_warp_store_kernel</code></td></tr>
<tr><td align="center">SCENARIO_NUM=1</td><td colspan="3" align="center"><code>insert_or_assign_mte_task_queue_kernel</code></td></tr>
</table>

> **说明：** `keys`输入默认要求同一批次内互不重复。样例支持不同key落入同一初始hash bucket的冲突场景，但不支持同一批次重复key的去重。

样例参数如下：

| 参数 | 值 |
|:---|---:|
| `HASH_TABLE_CAPACITY` | 256 * 1024 |
| `KEY_NUM` | 128 * 1024 |
| 负载因子 | 0.5 |
| `TEST_DIMS` | 16, 32, 64, 128, 256 |
| `BLOCK_DIM` | 64 |
| `THREAD_COUNT` | 512 |
| `WARP_SIZE` | 32 |

## 样例实现

性能对比以msOpProf输出的`Task Duration(μs)`为准。

### Case 0：SIMT Warp直接写value

**实现方式**：参考`insert_or_assign_warp_store_vf()`函数实现。

该实现中，每个Warp处理一条key。lane 0执行hash、线性探测和CAS锁定bucket，Warp内线程通过`asc_shfl`获取目标bucket index，并按`WARP_SIZE`分片写入value向量。对于探测到的临时`LOCKED_KEY`槽位，Case 0的lane 0会在当前槽位上持续重试，而不是直接跳过到下一个槽位，以避免同一个key在并发插入时跳过瞬时锁定槽位并产生重复可见项。value搬运由SIMT线程直接访问GM完成。

写入顺序为：CAS锁定目标bucket，Warp内线程写value，执行`asc_threadfence()`，最后由lane 0发布真实key。

**关键代码**：

```cpp
__simt_vf__ inline void insert_or_assign_warp_store_vf(__gm__ Bucket* table,
    __gm__ float* table_values, __gm__ const int64_t* keys, __gm__ const float* values, uint32_t key_num,
    uint32_t capacity, uint32_t dim, uint32_t block_index, uint32_t num_blocks)
{
    const uint32_t lane_id = static_cast<uint32_t>(threadIdx.x) % WARP_SIZE;
    ...
    if (lane_id == 0) {
        if (try_lock_bucket_for_key_case0(key_addr, key)) {
            success = 1;
            break;
        }
        ...
    }
    ...
    for (uint32_t j = lane_id; j < dim; j += WARP_SIZE) {
        table_values[dst_base + j] = values[src_base + j];
    }
    asc_threadfence();
    if (lane_id == 0) {
        *reinterpret_cast<__gm__ volatile int64_t*>(&table[bucket_idx].key) = key;
    }
}
```

**性能数据**：

| Case | 实现方式 | dim | 核数 | Task Duration(μs) |
|:---|:---|---:|:---:|---:|
| 0 | SIMT Warp直接写value | 16 | 64 | 217.916 |
| 0 | SIMT Warp直接写value | 32 | 64 | 218.686 |
| 0 | SIMT Warp直接写value | 64 | 64 | 271.254 |
| 0 | SIMT Warp直接写value | 128 | 64 | 376.517 |
| 0 | SIMT Warp直接写value | 256 | 64 | 585.892 |

**性能数据分析**：

- 随着`dim`增大，SIMT线程逐元素写value的耗时明显上升。`dim=256`时，value搬运已经成为该路径的主要开销。

**原理说明**：

HashTable插入包含hash、线性探测、CAS和冲突处理等离散控制流，适合用SIMT实现。value向量是连续内存，SIMT线程逐元素访问GM时，通常不如MTE连续搬运高效。

**优化方向**：

Case 0中，控制流和值写入均由同一组SIMT线程完成。Hash/probe/CAS属于离散控制流，适合SIMT线程处理；但value向量按GM连续地址存放，随着`dim`增大，Warp内线程需要执行更多逐元素GM写入，SIMT线程会被value搬运占用，后续key的探测也要等待当前value写完后才能继续推进。

Case 1保留SIMT处理hash/probe/CAS的职责，只把连续value搬运拆给MTE执行。SIMT线程锁定bucket后将value源地址、目的地址和待发布key写入UB任务队列，然后继续生产后续任务；MTE按任务队列批量执行GM->UB->GM搬运。这样，后续key的SIMT探测可以与前序value搬运在单kernel内并行推进，从而缓解Case 0中SIMT线程被连续value写入占用的问题。

### Case 1：MTE task queue搬运value

**实现方式**：参考`mte_task_queue`和`insert_or_assign_mte_task_queue_kernel()`函数实现。

该实现仍由SIMT线程执行hash/probe/CAS，但不再把完整任务列表写入GM。每个Vector Core维护一个UB task-id ring queue，SIMT producer把成功锁定的任务写入ring，任务中记录输入value源地址、HashTable value目的地址、待发布key和key地址；MTE consumer按ring中的任务批量搬运value，value写完后由SIMT drain路径发布真实key。`global_id`、`simt_assign_id`、`mte_finish_id`、`key_assign_id`和`key_finish_id`分别跟踪任务分配、MTE可见范围、value搬运完成进度、key发布认领范围和最终发布进度。bucket key在value写完前保持`LOCKED_KEY`，真实key只在完整value向量写回GM后发布。

下图给出Case 1中单个任务的可见性时序。`global_id`、`simt_assign_id`、`key_assign_id`和`key_finish_id`由SIMT路径推进，`mte_finish_id`由MTE路径推进。

![Case 1 visibility protocol](./figures/hash_table_visibility_protocol.png)

**关键代码**：

```cpp
class mte_task_queue {
public:
    __simt_callee__ inline void assign_warp_task(
        uint64_t src_addr, uint64_t dst_addr, int64_t key, uint64_t key_addr, bool need_assign)
    {
        uint32_t write_mask = asc_ballot(need_assign ? 1 : 0);
        uint32_t warp_write_count = __popc(write_mask);
        uint32_t task_start_id = (laneid() == 0) ? apply_id(warp_write_count) : 0;
        task_start_id = asc_shfl(task_start_id, 0);
        drain(task_start_id + warp_write_count);
        if (need_assign) {
            uint32_t lane_offset = __popc(write_mask & lanemask_lt());
            fill_task_info(task_start_id + lane_offset, src_addr, dst_addr, key, key_addr);
        }
        asc_threadfence();
        if (laneid() == 0) {
            assign_task(task_start_id, task_start_id + warp_write_count);
        }
    }

    __aicore__ inline void run_task() const
    {
        while (true) {
            uint32_t simt_assign_id = *reinterpret_cast<__ubuf__ volatile uint32_t*>(simt_assign_id_);
            uint32_t proc_count = min(max_proc_batch, simt_assign_id - mte_finish_id);
            ...
            asc_copy_gm2ub_align(value_local_, src_addr, 1, value_bytes, 0, 0, true, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
            asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
            asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
            asc_copy_ub2gm_align(dst_addr, value_local_, 1, value_bytes, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
            asc_sync_notify(PIPE_MTE3, PIPE_S, EVENT_ID0);
            asc_sync_wait(PIPE_MTE3, PIPE_S, EVENT_ID0);
            *reinterpret_cast<__ubuf__ volatile uint32_t*>(mte_finish_id_) = mte_finish_id + proc_count;
        }
    }
};
```

**性能数据**：

| Case | 实现方式 | dim | 核数 | Task Duration(μs) |
|:---|:---|---:|:---:|---:|
| 1 | MTE task queue搬运value | 16 | 64 | 127.638 |
| 1 | MTE task queue搬运value | 32 | 64 | 110.601 |
| 1 | MTE task queue搬运value | 64 | 64 | 113.900 |
| 1 | MTE task queue搬运value | 128 | 64 | 131.696 |
| 1 | MTE task queue搬运value | 256 | 64 | 204.030 |

**性能数据分析**：

- Case 1把长度为`dim`的value向量搬运从SIMT线程中拆出，在单kernel内由SIMT producer和MTE consumer并行推进。
- 相比Case 0，Case 1在五组dim上均取得加速；`dim=256`时加速比最高，约2.87x。

## 性能对比总结

### Ascend 950PR性能对比

各dim配置下的性能数据如下：

| dim | Case 0 SIMT Warp直接写value(μs) | Case 1 MTE task queue(μs) | Case 1加速比 |
|---:|---:|---:|---:|
| 16 | 217.916 | 127.638 | 1.71x |
| 32 | 218.686 | 110.601 | 1.98x |
| 64 | 271.254 | 113.900 | 2.38x |
| 128 | 376.517 | 131.696 | 2.86x |
| 256 | 585.892 | 204.030 | 2.87x |

### 优化要点总结

| 优化手段 | 核心原理 | 样例体现 |
|:---|:---|:---|
| 用MTE替代SIMT逐元素写value | 把长度为`dim`的value向量写入从SIMT线程中拆出，改为MTE执行GM->UB->GM批量搬运，减少SIMT线程直接访问GM的开销 | Case 0由Warp线程分片写value，Case 1改为MTE消费任务并完成value搬运 |
| 单kernel任务队列流水并行 | 在单kernel内用UB task-id ring queue连接SIMT探测和MTE搬运，让后续key的探测与前序value的搬运并行推进 | Case 1使用`mte_task_queue`组织producer/consumer流水，在同一kernel内重叠SIMT探测与MTE搬运 |

---

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 切换Case

  在cmake编译时通过`-DSCENARIO_NUM=N`指定要编译的Case，各Case说明：
  - `0`: SIMT Warp直接写value
  - `1`: MTE task queue搬运value

  示例：

  ```bash
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=1
  ```

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 样例执行

  在样例目录下执行如下命令。

  ```bash
  SCENARIO_NUM=1                                                       # 选择执行场景
  mkdir -p build
  cd build
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..
  make -j
  ./hash_table_mte_queue 128
  ```

  也可以直接运行对比脚本，编译两种Case并采集`16/32/64/128/256`五组dim的性能数据。

  ```bash
  ./scripts/run.sh
  ```

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |:---|:---|:---|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构，对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `0`、`1` | Case编号：0=SIMT Warp直接写value，1=MTE task queue搬运value |

- 执行结果

  执行结果如下，表示HashTable插入结果和值校验通过。

  ```bash
  Verification PASSED
  ```

### 性能分析

使用msOpProf获取性能数据。对比脚本会先执行一次样例并检查`Verification PASSED`，确认结果正确后再通过`msopprof`命令采集目标kernel性能数据。

Case 0测试方式如下：

```bash
cmake -DSCENARIO_NUM=0 -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..
make -j
./hash_table_mte_queue 128
msopprof ./hash_table_mte_queue 128
```

Case 1测试方式如下：

```bash
cmake -DSCENARIO_NUM=1 -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..
make -j
./hash_table_mte_queue 128
msopprof ./hash_table_mte_queue 128
```

当前目录下会生成`OPPROF_`前缀的文件夹，保存当前kernel的性能分析数据。

```text
OPPROF_xxxx_XXXXXXXX
├── ArithmeticUtilization.csv
├── L2Cache.csv
├── Memory.csv
├── MemoryL0.csv
├── MemoryUB.csv
├── OpBasicInfo.csv
├── PipeUtilization.csv
└── ResourceConflictRatio.csv
```

查看具体的性能分析结果：

```bash
# 查看Task Duration等基础信息
cat ./OPPROF_*/OpBasicInfo.csv

# 查看Vector/MTE等流水耗时和占比
cat ./OPPROF_*/PipeUtilization.csv
```
