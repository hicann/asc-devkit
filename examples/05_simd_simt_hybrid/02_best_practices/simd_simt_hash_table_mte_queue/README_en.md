# HashTable MTE Queue Optimization Sample for SIMD and SIMT Hybrid Programming

## Overview

This sample compares two value write paths for HashTable`insert_or_assign`: direct value writes by a SIMT Warp, and value writes by an MTE task queue in a single kernel. The sample implements a fixed-capacity C++-style HashTable. It shows this optimization path and keeps only the semantics and data structures required for the comparison.

**Optimization paths**:

| Case | SCENARIO_NUM | Implementation | Description |
|:---|:---:|:---|:---|
| Case 0 | 0 | Direct value writes by a SIMT Warp | After a Warp completes hash, probe, and CAS operations, the threads in the Warp write one value vector in shards. This case is the baseline implementation. |
| Case 1 | 1 | Value movement by an MTE task queue | A task-id ring queue is used in a single kernel. SIMT continuously produces UB tasks, and MTE moves values in batches by task. The MTE movement forms a pipeline with subsequent SIMT probing. |

The following figure shows the data processing pipelines of the two cases.

![HashTable MTE value movement pipeline](./figures/hash_table_mte_pipeline.png)

## Supported Products and CANN Software Version

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```text
├── simd_simt_hash_table_mte_queue
│   ├── CMakeLists.txt                // Compilation project file
│   ├── figures                       // Image resources in the README
│   ├── hash_table_mte_queue.asc      // Ascend C sample implementation
│   ├── README.md                     // Sample description document
│   └── scripts
│       └── run.sh                    // Compiles and uses msOpProf to collect performance data of the two cases
```

## Sample Description

**Sample function**:

This sample implements a fixed-capacity HashTable. It supports insert-or-update semantics similar to C++17`std::unordered_map::insert_or_assign`: when a key exists, the sample updates the value; when a key does not exist, the sample inserts the key and value. The sample handles hash collisions by linear probing.

Note that this sample currently targets single-batch insertion with unique keys for implementation and performance comparison. It does not deduplicate duplicate keys within the same input batch or guarantee ordering semantics for them. If the same key appears multiple times in one batch, the final result is outside the scope of this sample.

The HashTable consists of two GM storage segments. `table[bucket_count]` stores bucket metadata. Each bucket stores only a key and a state. `table_values[bucket_count, dim]` stores value data. Row `bucket_idx` stores the value vector of the key that corresponds to `table[bucket_idx]`. The key and value are not stored contiguously in the same structure. They are associated by the same `bucket_idx`. Each value vector has a length of `dim`, and `dim` is a runtime parameter.

The key field of a bucket also stores occupation information:

| Key field value | Meaning |
|:---|:---|
| `EMPTY_KEY` | The bucket is empty and can be locked by CAS in the insertion flow. |
| `LOCKED_KEY` | The bucket is being inserted or updated. Other threads must wait until the key is published and then check the key again. |
| Real key | The bucket is published and visible to find operations. |

During insertion or update, the kernel first changes the key of the target bucket from `EMPTY_KEY` or a real key to `LOCKED_KEY` by CAS. Then it writes the value and publishes the real key. The find operation matches only real keys, so it cannot observe an intermediate state in which the key is visible but the value is not completely written.

The HashTable insertion flow is as follows:

$$
bucket = hash(key) \& (capacity - 1)
$$

$$
bucket = (bucket + 1) \& (capacity - 1), \quad \text{when collision occurs}
$$

**Sample specifications**:

<table>
<tr><td rowspan="1" align="center">Sample Type (OpType)</td><td colspan="4" align="center">HashTable insert_or_assign</td></tr>
<tr><td rowspan="3" align="center">Sample Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">keys</td><td align="center">[128 * 1024]</td><td align="center">int64</td><td align="center">ND</td></tr>
<tr><td align="center">values</td><td align="center">[128 * 1024, dim]</td><td align="center">float32</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">Sample Output</td><td align="center">table keys</td><td align="center">[256 * 1024]</td><td align="center">int64</td><td align="center">ND</td></tr>
<tr><td align="center">table values</td><td align="center">[256 * 1024, dim]</td><td align="center">float32</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">Kernel Name</td><td align="center">SCENARIO_NUM=0</td><td colspan="3" align="center"><code>insert_or_assign_warp_store_kernel</code></td></tr>
<tr><td align="center">SCENARIO_NUM=1</td><td colspan="3" align="center"><code>insert_or_assign_mte_task_queue_kernel</code></td></tr>
</table>

> **Note:** The `keys` input is expected to be unique within one batch. The sample supports hash-collision scenarios where different keys map to the same initial bucket, but it does not support deduplication of duplicate keys within the same batch.

The sample parameters are as follows:

| Parameter | Value |
|:---|---:|
| `HASH_TABLE_CAPACITY` | 256 * 1024 |
| `KEY_NUM` | 128 * 1024 |
| Load factor | 0.5 |
| `TEST_DIMS` | 16, 32, 64, 128, 256 |
| `BLOCK_DIM` | 64 |
| `THREAD_COUNT` | 512 |
| `WARP_SIZE` | 32 |

## Sample Implementation

The performance comparison uses `Task Duration(μs)` in the msOpProf output.

### Case 0: Direct Value Writes by a SIMT Warp

**Implementation**: For the implementation, refer to the `insert_or_assign_warp_store_vf()` function.

In this implementation, each Warp processes one key. Lane 0 performs hash, linear probing, and CAS bucket locking. Threads in the Warp obtain the target bucket index by `asc_shfl` and write the value vector in shards by `WARP_SIZE`. For a transient `LOCKED_KEY` slot encountered during probing, lane 0 in Case 0 keeps retrying on the current slot instead of skipping to the next slot. This prevents the same key from skipping a temporarily locked slot during concurrent insertion and creating duplicate visible entries. SIMT threads directly access GM to complete value movement.

The write sequence is as follows: lock the target bucket by CAS, write the value by threads in the Warp, run `asc_threadfence()`, and publish the real key from lane 0.

**Key code**:

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

**Performance data**:

| Case | Implementation | dim | Number of Cores | Task Duration(μs) |
|:---|:---|---:|:---:|---:|
| 0 | Direct value writes by a SIMT Warp | 16 | 64 | 217.916 |
| 0 | Direct value writes by a SIMT Warp | 32 | 64 | 218.686 |
| 0 | Direct value writes by a SIMT Warp | 64 | 64 | 271.254 |
| 0 | Direct value writes by a SIMT Warp | 128 | 64 | 376.517 |
| 0 | Direct value writes by a SIMT Warp | 256 | 64 | 585.892 |

**Performance data analysis**:

- As `dim` increases, the time consumed by element-by-element value writes from SIMT threads increases significantly. When `dim=256`, value movement becomes the main overhead of this path.

**Principle**:

HashTable insertion includes discrete control flow such as hash, linear probing, CAS, and collision handling. SIMT is suitable for this control flow. A value vector is stored in contiguous memory. When SIMT threads access GM element by element, they are usually less efficient than MTE for contiguous movement.

**Optimization direction**:

In Case 0, the same group of SIMT threads handles both control flow and value writes. Hash, probe, and CAS operations are discrete control flow and are suitable for SIMT threads. However, value vectors are stored at contiguous GM addresses. As `dim` increases, threads in a Warp must perform more element-by-element GM writes. The SIMT threads are occupied by value movement, and probing for subsequent keys must wait until the current value is written.

Case 1 keeps SIMT responsible for hash, probe, and CAS operations, and moves only contiguous value movement to MTE. After SIMT threads lock a bucket, they write the source address, destination address, and key to publish to a UB task queue. Then they continue to produce subsequent tasks. MTE performs GM->UB->GM movement in batches by task queue. In this way, SIMT probing for subsequent keys can run in parallel with previous value movement in a single kernel. This design reduces the impact of contiguous value writes occupying SIMT threads in Case 0.

### Case 1: Value Movement by an MTE Task Queue

**Implementation**: For the implementation, refer to the `mte_task_queue` class and the `insert_or_assign_mte_task_queue_kernel()` function.

In this implementation, SIMT threads still perform hash, probe, and CAS operations, but they no longer write the complete task list to GM. Each Vector Core maintains a UB task-id ring queue. The SIMT producer writes successfully locked tasks to the ring. Each task records the input value source address, the HashTable value destination address, the key to publish, and the key address. The MTE consumer moves values in batches by tasks in the ring. After the value writes are complete, the SIMT drain path publishes the real key. `global_id`, `simt_assign_id`, `mte_finish_id`, `key_assign_id`, and `key_finish_id` track task allocation, the MTE-visible range, value movement progress, key publishing claim range, and final publishing progress respectively. The bucket key remains `LOCKED_KEY` before the value is written. The real key is published only after the complete value vector is written back to GM.

The following figure shows the visibility timing of a single task in Case 1. The SIMT path advances `global_id`, `simt_assign_id`, `key_assign_id`, and `key_finish_id`. The MTE path advances `mte_finish_id`.

![Case 1 visibility protocol](./figures/hash_table_visibility_protocol.png)

**Key code**:

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

**Performance data**:

| Case | Implementation | dim | Number of Cores | Task Duration(μs) |
|:---|:---|---:|:---:|---:|
| 1 | Value movement by an MTE task queue | 16 | 64 | 127.638 |
| 1 | Value movement by an MTE task queue | 32 | 64 | 110.601 |
| 1 | Value movement by an MTE task queue | 64 | 64 | 113.900 |
| 1 | Value movement by an MTE task queue | 128 | 64 | 131.696 |
| 1 | Value movement by an MTE task queue | 256 | 64 | 204.030 |

**Performance data analysis**:

- Case 1 moves the value vector with a length of `dim` out of SIMT threads. SIMT producer and MTE consumer run in parallel in a single kernel.
- Compared with Case 0, Case 1 accelerates all five `dim` configurations. The highest speedup is about 2.87x when `dim=256`.

## Performance Comparison Summary

### Performance Comparison on Ascend 950PR

The performance data of each `dim` configuration is as follows:

| dim | Case 0 Direct value writes by a SIMT Warp (μs) | Case 1 MTE task queue (μs) | Case 1 Speedup |
|---:|---:|---:|---:|
| 16 | 217.916 | 127.638 | 1.71x |
| 32 | 218.686 | 110.601 | 1.98x |
| 64 | 271.254 | 113.900 | 2.38x |
| 128 | 376.517 | 131.696 | 2.86x |
| 256 | 585.892 | 204.030 | 2.87x |

### Optimization Summary

| Optimization | Core Principle | Sample Implementation |
|:---|:---|:---|
| Replace SIMT element-by-element value writes with MTE | Move value vector writes with a length of `dim` out of SIMT threads. MTE performs GM->UB->GM batch movement to reduce the overhead of direct GM access by SIMT threads. | In Case 0, Warp threads write values in shards. In Case 1, MTE consumes tasks and completes value movement. |
| Single-kernel task queue pipeline parallelism | Use a UB task-id ring queue in a single kernel to connect SIMT probing and MTE movement. This lets probing for subsequent keys run in parallel with previous value movement. | Case 1 uses `mte_task_queue` to organize a producer and consumer pipeline. SIMT probing and MTE movement overlap in the same kernel. |

---

## Compilation and Running

In the sample root directory, perform the following steps to compile and run the sample.

- Switch the case.

  Specify the case to compile by `-DSCENARIO_NUM=N` during CMake compilation. The cases are as follows:
  - `0`: Direct value writes by a SIMT Warp
  - `1`: Value movement by an MTE task queue

  Command:

  ```bash
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=1
  ```

- Configure environment variables.

  Configure environment variables based on the [installation method](../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit package in the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Description:** `${install_path}` is the installation directory of the CANN package. If no installation directory is specified, the default installation directory is `/usr/local/Ascend`.

- Run the sample.

  Run the following commands in the sample directory.

  ```bash
  SCENARIO_NUM=1                                                       # Select the execution scenario.
  mkdir -p build
  cd build
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..
  make -j
  ./hash_table_mte_queue 128
  ```

  You can also directly run the comparison script. The script compiles the two cases and collects performance data for five `dim` configurations: `16/32/64/128/256`.

  ```bash
  ./scripts/run.sh
  ```

- Compilation options

  | Option | Value | Description |
  |:---|:---|:---|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture, which corresponds to Ascend 950PR/Ascend 950DT. |
  | `SCENARIO_NUM` | `0`, `1` | Case number: 0 = direct value writes by a SIMT Warp; 1 = value movement by an MTE task queue. |

- Execution result

  The execution result is as follows, which indicates that the HashTable insertion result and value verification passed.

  ```bash
  Verification PASSED
  ```

### Performance Analysis

Use msOpProf to obtain performance data. The comparison script first runs the sample and checks `Verification PASSED`. After confirming that the result is correct, the script collects target kernel performance data by the `msopprof` command.

The Case 0 test method is as follows:

```bash
cmake -DSCENARIO_NUM=0 -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..
make -j
./hash_table_mte_queue 128
msopprof ./hash_table_mte_queue 128
```

The Case 1 test method is as follows:

```bash
cmake -DSCENARIO_NUM=1 -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..
make -j
./hash_table_mte_queue 128
msopprof ./hash_table_mte_queue 128
```

A directory with the `OPPROF_` prefix is generated in the current directory to store performance analysis data of the current kernel.

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

View detailed performance analysis results:

```bash
# View basic information such as Task Duration.
cat ./OPPROF_*/OpBasicInfo.csv

# View Vector/MTE pipeline time and ratio.
cat ./OPPROF_*/PipeUtilization.csv
```
