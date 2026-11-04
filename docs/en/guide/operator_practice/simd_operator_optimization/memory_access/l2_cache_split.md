# L2 Cache Splitting<a name="ZH-CN_TOPIC_0000001893038949"></a>

<!-- md-trans-meta sourceCommit=63bf0ce4379c8555ca9af9ecde65b638815937ab translatedAt=2026-10-23T12:40:22.868Z pushedAt=2026-11-03T07:05:13.335Z nl=2 -->

[Priority]: High

[Description] Assume that the L2 cache of an AI processor is 192 MB, the mixed read/write bandwidth of the L2 cache is about 7 TB/s, and the bandwidth of GM is about 1.6 TB/s. There is a large gap between the two. When loading or storing the same amount of data, accessing the L2 cache to read and write data is faster than accessing GM. If the data cannot hit the L2 cache, that is, the data to be accessed is not in the L2 cache, the data must be read from or written to GM, resulting in low bandwidth utilization. Eventually, loading or storing data becomes the performance bottleneck of the entire operator running process. Splitting strategy recommendation: When the amount of input and output data exceeds the L2 cache size, enable the L2 cache splitting strategy in Tiling.

The core idea of the optimization is: **By enabling the L2 cache splitting strategy, control the data scale processed in a single round within the L2 cache range, improve the data reuse rate in the L2 cache, and thereby reduce repeated data loading and storing**. This document and the accompanying sample set the valid data amount threshold that the L2 cache can accommodate to 100 MB. This value is used only for sample demonstration.

## Scenarios

- 20 vector cores are used for parallel computation. Each element undergoes two rounds of "add 1" operations, and the final result is `data + 2`.
- In the negative example, L2 Cache splitting is not performed. In each round, the 384MB data is loaded, computed, and stored as a whole.
- In the positive example, the 384MB data is split into four L2 batches, B0 to B3, each containing 96MB of data. Each batch undergoes two rounds of "add 1" computation.
- For the complete implementation, see the [add_l2_cache_split sample](../../../../../../examples/01_simd_cpp_api/03_basic_api/10_cache_control/add_l2_cache_split).

The data processing methods of the two scenarios are shown in the following figures.

**Figure 1**  L2 Cache splitting not enabled<a name="fig_l2_cache_no_split"></a>

![](../../../figures/no_l2_cache_split.png "L2 Cache splitting not enabled")

**Figure 2**  L2 Cache splitting enabled<a name="fig_l2_cache_split"></a>

![](../../../figures/l2_cache_split.png "L2 Cache splitting enabled")

## Negative Example: No L2 Cache Splitting

In the negative example, no data splitting is performed, and the entire 384 MB of data is processed as a whole. The L2 Cache is invisible to users. From the user's perspective, the process of moving data from GM to UB is as follows: the total 384 MB of data is evenly divided among 20 cores, with each core moving approximately 19.2 MB.

**[User perspective]**

Only the GM-to-UB move process is perceived. The total 384 MB of data is evenly divided among 20 cores, with each core moving 19.2 MB.

**[Actual data path]**

The input data is first filled from GM into the L2 Cache, and then into UB.

**Figure 3**  GM-to-UB path<a name="fig_l2_cache_gm_to_ub"></a>

![](../../../figures/l2cache_to_UB.png "GM-to-UB path")

**[Load/Fill process]**

Assume that the data processed by the 20 cores is loaded into the L2 Cache segment by segment in core order, and the valid data threshold of the L2 Cache is 100 MB. In this case, the L2 Cache can hold the data volume of approximately 5 cores.

**Figure 4**  Data loading process to fill the L2 Cache<a name="fig_l2_cache_fill_process"></a>

![](../../../figures/l2cache_fill_process.png "Data loading process to fill the L2 Cache")

When subsequent new data fills the L2 Cache, it may replace the early Cache Lines of cores 0 to 4.

**[Execution sequence of the negative example without splitting]**

**Figure 5**  Negative example with L2 Cache splitting not enabled<a name="fig_l2_cache_no_split_example"></a>

![](../../../figures/l2cache_no_split_example.png "Negative example with L2 Cache splitting not enabled")

When processing the second half of the 384 MB data in round 1 (R0), the newly loaded data may replace the previously loaded data. Since R0 is the first round of access, comparing only one round of load and compute cannot stably reflect the L2 Cache hit benefit. In round 2 (R1), when the 384 MB data is accessed again, the previous data may no longer be in the L2 Cache, so it is more likely to be loaded again from GM.

## Positive Example: Splitting by L2 Cache

In the positive example, the 384 MB data is split into four batches of 96 MB each based on the 100 MB threshold. In each round, 20 cores process 96 MB of data in total, and the data is reused across rounds. As a result, the data read before each computation can hit the L2 Cache, improving operator performance.
The loop hierarchy of the positive example is `L2 Batch -> Round`. All rounds of the same batch are completed consecutively before switching to the next batch:

```text
Number of L2 batches      = ceil(total data size / L2 Cache valid data threshold)
                = ceil(384MB / 100MB)
                = 4
Size of a single L2 batch = 384MB / 4 = 96MB
```

```cpp
for (uint32_t l2BatchIdx = 0; l2BatchIdx < L2_BATCH_NUM; ++l2BatchIdx) {
    for (uint32_t roundIdx = 0; roundIdx < COMPUTE_ROUND_NUM; ++roundIdx) {
        ProcessL2Batch(l2BatchIdx);
        AscendC::SyncAll();
    }
}
```

**[Execution sequence of the L2 Cache splitting positive example]**

**Figure 6**  Positive example with splitting enabled<a name="fig_l2_cache_split_example"></a>

![](../../../figures/l2cache_split_example.png "Positive example with splitting enabled")

Between the two rounds of access to the same batch, no other large data exceeding the 100 MB valid threshold is inserted, so the second round has a better chance of hitting the data left in the L2 Cache by the first round.<br>
The data size of B0 is 96 MB, which does not exceed the 100 MB threshold. No other large data is inserted between the two accesses to B0 to update the L2 Cache, so R1 has a better chance of reading the result of R0 from the L2 Cache. After B0 is fully processed, B1 to B3 are processed in sequence following the same principle.

**[Key performance observations]**

Only one round of computation is executed: R0 is the first-round access, so both the negative example and the positive example need to load data from GM for the first time, and the difference in cache misses is usually not obvious.<br>
The positive and negative examples reflect the performance optimization through the degree of data reuse in the L2 Cache: when R1 accesses the same batch of addresses again, whether the target data is still in the L2 Cache is the key to reducing cache misses.<br>

The loop hierarchy of the negative example is `Round -> Whole Data`. Each outer round processes the complete 384MB range:

```cpp
for (uint32_t roundIdx = 0; roundIdx < COMPUTE_ROUND_NUM; ++roundIdx) {
    ProcessWholeData();
    AscendC::SyncAll();
}
```

**Table 1** Sample data volume

| Data Volume | Computation Method | Result |
| ------ | ------ | -----: |
| Total data volume | Fixed sample value | 384MB |
| Total number of elements | `384MB / sizeof(half)` | 201326592 |
| L2 Cache valid data threshold | Fixed sample value | 100MB |
| Negative example single-stage full-core data | L2 Cache splitting not performed | 384MB |
| Positive example L2 Batch count | `ceil(384MB / 100MB)` | 4 |
| Positive example single L2 Batch data volume | `384MB / 4` | 96MB |
| Positive example single L2 Batch element count | `96MB / sizeof(half)` | 50331648 |

## Why Not Splitting Reduces the Reuse Probability

Taking the 100MB threshold in this document as an example, cores 0 to 4 process about 96MB in total, which is already close to the effective L2 Cache capacity. When core 5 and subsequent cores continue to load new data, the total data volume exceeds the 100MB threshold, and the data of core 0 and other cores that entered the L2 Cache earlier may be replaced. In other words, the replacement risk already arises during the first round of processing, not in the second round. If the operator reuses early addresses in subsequent steps of the same round, this replacement directly manifests as a Cache Miss within a single round.<br>
The document designs two rounds of computation to explicitly demonstrate data reuse: the second round accesses the same batch of addresses that the first round has just processed. Without splitting, when the second round accesses the address range where `data[0]` resides, the previous data may have already been replaced during the subsequent scanning in the first round, so a Cache Miss is more likely and data is fetched again from GM.<br>
The positive example of L2 Cache splitting limits the data volume that a single loop needs to process to 96MB. Within a single batch, the total data covered by 20 cores does not exceed the 100MB threshold, and each core processes only about 4.8MB. The first and second rounds of computation are first completed on `data[B0]`, and then B1 to B3 are processed in sequence. No other large data is inserted between the two rounds of access to the same batch to update the L2 Cache, so the second round has a better chance of directly hitting the L2 Cache data that the first round has just written back.<br>

## Common Processing Logic

Both implementations use the same `data` buffer and write the result back to the same GM address from which the data was loaded. The difference lies in the continuous data range passed to the common processing function: the negative example passes the complete 384 MB range, while the positive example passes a single 96 MB L2 batch range. The following code only shows the 32-byte DataBlock per-core distribution formula used by the actual sample and the difference in the ranges passed by the positive and negative examples; it does not expand on the per-core load/store and computation details.

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

// For the per-core load/store and computation process, refer to the complete implementation in the accompanying sample.
```

For the complete 384 MB range of the negative example without splitting, the first 12 cores each process 629146 DataBlocks, and the remaining 8 cores each process 629145 DataBlocks, with a per-core data volume of about 19.2 MB. The 20 cores together cover the complete `data[0, 384MB)` range without overlap. For a single 96 MB L2 batch of the positive example, the first 8 cores each process 157287 DataBlocks, and the remaining 12 cores each process 157286 DataBlocks, with a per-core data volume of about 4.8 MB. The 20 cores together cover the complete L2 batch without overlap. Each DataBlock contains 16 `half` elements, so the data length moved by all `DataCopy` operations meets the 32-byte alignment requirement. For the complete intra-core pipeline synchronization implementation, refer to the accompanying sample. The inter-core synchronization between stages is completed by the `KernelAdd::SyncAllCores` function in the accompanying sample, which directly calls the `AscendC::SyncAll()` API, rather than an additionally wrapped custom synchronization function.

## Comparison Summary of Positive and Negative Examples

**Table 2** Comparison of positive and negative examples

| Dimension | Negative Example | Positive Example |
| ------ | ------ | ------ |
| L2 Cache splitting | Not split | Split into four 96MB L2 batches |
| Loop hierarchy | `Round -> Whole Data` | `L2 Batch -> Round` |
| Total data volume across all cores in a single stage | 384MB | 96MB |
| Average processing volume per core | About 19.2MB | About 4.8MB |
| Execution timeline | `R0/data[0,384MB) -> R1/data[0,384MB)` | `B0/R0 -> B0/R1 -> ... -> B3/R0 -> B3/R1` |
| Between two accesses to the same batch | Within one round, 20 cores have collectively scanned the complete 384MB range | No other large data range exceeding the threshold is inserted |
| Risk of L2 Cache data replacement | Higher | Lower |
| L2 Cache hit rate in round 2 | Expected to be lower, subject to actual measurement | Expected to be higher, subject to actual measurement |

## Application Scenario

- The total data volume of the operator exceeds the available valid L2 Cache threshold, and the same batch of data needs to be read multiple times.
- Data that is read only once has no cross-round reuse, so adjusting the L2 Cache splitting order usually yields limited benefit.

## Test Case Design

The positive and negative example test cases use the same 384 MB GM data buffer, number of cores, number of rounds, and computation logic. The only core difference is that the negative example does not perform L2 Cache splitting, while in the positive example the input data is split into four 96 MB L2 batches based on the 100 MB valid data threshold.

### Positive and Negative Example Test Cases

**Table 3** Positive and negative example test cases

| Test Case | Loop Level | Number of Data Elements | Number of Cores | Number of Rounds | Verification Purpose |
| ------ | ------ | ------ | -------: | ------ | -----: |
| Negative example no-split | `Round -> Whole Data` | 201326592 | 20 | 2 | Verifies the replacement risk when the total 384 MB data across 20 cores exceeds the L2 Cache valid threshold |
| Positive example L2-split | `L2 Batch -> Round` | 201326592 | 20 | 2 | Verifies the L2 Cache locality improvement brought by continuous reuse of a single 96 MB batch |

Each L2 batch in the positive example contains 50331648 `half` elements, occupying 96 MB. Data is allocated to 20 Vector Cores in units of 32-byte DataBlocks, with the first 8 cores each processing 2516592 elements and the remaining 12 cores each processing 2516576 elements. In the negative example, each round directly processes the complete 384 MB range, with 20 cores jointly covering `data[0,384MB)`. All `DataCopy` lengths remain 32-byte aligned.

### Verification Checkpoints

**Table 4**  Verification checkpoints

| Check Item | Verification Method | Expected Result |
| ------ | ------ | ------ |
| L2 Cache hit rate | Collect the L2 Cache read hit metric in an independent process | The median of the positive example is usually higher than that of the negative example. Interpret specific fields according to the architecture and tool version. |
| Copy efficiency | Compare the MTE2 duration and effective bandwidth | The median MTE2 duration of the positive example usually decreases, and the effective bandwidth usually increases. |
| End-to-end duration | Collect multiple independent profiles for the positive and negative examples and compare the median of `Task Duration(us)` | In copy-bound scenarios, the duration of the positive example is usually lower than that of the negative example. |

> **Note**: `aiv_gm_to_ub_bw(GB/s)` indicates the effective bandwidth, not the GM read volume. After optimization, the effective bandwidth may increase, so "lower bandwidth" should not be used as an expectation. For performance collection, alternate the order of the positive and negative examples, and use the median of multiple sets of results to reduce the impact of system fluctuations.
