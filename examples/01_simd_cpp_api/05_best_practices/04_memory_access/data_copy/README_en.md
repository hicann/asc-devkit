# DataCopy Best Practice Example

## Overview

This example demonstrates data transfer practices from Global Memory to UB and from Global Memory to L1. The example does not include computation logic and focuses on observing MTE2 transfer behavior, as well as the impact of block granularity, unaligned data transfer, L2Cache reuse, and same-address access conflict avoidance on data transfer performance. It compares the performance of DataCopy/DataCopyPad across different transfer modes.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── data_copy
│   ├── scripts
│   │   └── gen_data.py        // Input data generation script
│   ├── CMakeLists.txt         // Build project file
│   ├── data_copy.asc          // Ascend C example entry and Kernel invocation
│   ├── data_copy_l1.h         // GM to L1 transfer implementation
│   ├── data_copy_ub.h         // GM to UB transfer implementation
│   ├── data_utils.h           // Data read/write functions
│   └── README.md              // Example documentation
```

## Example Description

The input for this example is a half-type 2D matrix in ND format. The aligned scenario input shape is [12288, 12288], and the unaligned scenario input shape is [12288, 12287]. The destination storage location is selected through the build option `COPY_DST`, and different transfer scenarios are selected through `SCENARIO_NUM`.

- `COPY_DST=UB`: Uses AIV cores to perform GM to UB transfer. Kernel name is `kernel_data_copy_pad_gm2ub`
- `COPY_DST=L1`: Uses AIC cores to perform GM to L1 transfer. Kernel name is `kernel_data_copy_gm2l1`

## Example Implementation and Performance Analysis

For table presentation convenience, the following text refers to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products collectively as Atlas A2/A3 Series, and Ascend 950PR/Ascend 950DT as Ascend 950 Series.

This chapter addresses the most common issues in data transfer optimization: it first explains performance metric meanings, then compares block granularity, unaligned data, L2Cache reuse, and multi-core same-address access conflicts. Each optimization point includes implementation method, comparison method, performance data, and conclusions, making it easy to correlate code behavior with performance changes.

### Core Feature Overview

<table>
<tr><th>Optimization Point</th><th>Primary Observation Target</th><th>Comparison Method</th></tr>
<tr><td>Block Granularity</td><td>Impact of single DataCopy transfer size on MTE2 efficiency</td><td>Keep matrix size constant, adjust <code>TILE_M/TILE_N</code></td></tr>
<tr><td>Unaligned Data Transfer</td><td>Impact of non-divisible shapes on transfer overhead</td><td>Keep block size constant, change <code>N=12288</code> to <code>N=12287</code></td></tr>
<tr><td>L2Cache Reuse</td><td>Cache benefit when repeatedly accessing the same GM data</td><td>Compare full-block repeated transfer vs. N-direction sliced repeated transfer</td></tr>
<tr><td>Same-Address Conflict Avoidance</td><td>Conflict impact when multiple cores access the same GM address range simultaneously</td><td>Compare all cores accessing in the same order vs. staggered access by core</td></tr>
</table>

### Performance Metric Description

The standard MTE2 performance table primarily observes transfer duration and transfer instruction ratio. Field meanings are as follows.

"Performance improvement" in the table is calculated as `baseline duration / current duration - 1`; positive "performance change" indicates performance improvement, and negative indicates performance degradation.
<table>
<tr><th>Field Name</th><th>Field Meaning</th></tr>
<tr><td>Task Duration(μs)</td><td>Total execution time of the entire task. Operator execution time is determined by this parameter.</td></tr>
<tr><td>*_total_cycles</td><td>Total cycle count for Task execution.</td></tr>
<tr><td>*_mte2_time(μs)</td><td>MTE2 type instruction (DDR -> AI Core transfer instructions) duration, in μs.</td></tr>
<tr><td>*_mte2_ratio</td><td>Ratio of MTE2 type instruction (DDR -> AI Core transfer instructions) cycle count to total cycle count.</td></tr>
</table>

The L2Cache performance table additionally shows cache hit, miss, and eviction related counts beyond MTE2 duration. Field meanings are as follows.

<table>
<tr><th>Field Name</th><th>Field Meaning</th></tr>
<tr><td>Task Duration(μs)</td><td>Total execution time of the entire task. Operator execution time is determined by this parameter.</td></tr>
<tr><td>*_total_cycles</td><td>Total cycle count for Task execution.</td></tr>
<tr><td>*_time(μs)</td><td>Theoretical execution time of the Task on the corresponding AI Core, in μs.</td></tr>
<tr><td>*_write_cache_hit</td><td>Number of write Cache hits.</td></tr>
<tr><td>*_write_cache_miss_allocate</td><td>Number of cache reallocations after write Cache misses.</td></tr>
<tr><td>*_r*_read_cache_hit</td><td>Number of read r* channel Cache hits. r0/r1 are the two hardware read/write channels. When analyzing total read hits, accumulate both channels.</td></tr>
<tr><td>*_r*_read_cache_miss_allocate</td><td>Number of reallocations after read r* channel Cache misses. r0/r1 are the two hardware read/write channels. When analyzing total read misses, accumulate both channels.</td></tr>
<tr><td>*_read_local_l2_hit</td><td>Number of read Cache hits.</td></tr>
<tr><td>*_read_local_l2_miss</td><td>Number of read Cache misses.</td></tr>
<tr><td>*_read_local_l2_victim</td><td>Number of read Cache misses that triggered data eviction from Cache.</td></tr>
</table>

### Optimization Point 1: Impact of Block Granularity on Transfer Efficiency

**Implementation**: Refer to the GM to UB `DataCopyPad` transfer in `data_copy_ub.h` and the GM to L1 `DataCopy` transfer in `data_copy_l1.h`. Different `TILE_M/TILE_N` combinations are switched through compile-time scenario parameters. The input matrix size remains unchanged; only `TILE_M/TILE_N` is modified. Smaller blocks increase the number of transfer instructions, while larger blocks reduce instruction dispatch count but require more on-chip temporary space.

First, examine the shape of a single DataCopy transfer: the UB path transfers data directly into UB following the ND layout, while the L1 path completes the ND to NZ layout conversion when transferring into L1. Both paths use different parameters, but both are determined by `TILE_M/TILE_N` for the 2D block size per transfer.

<table>
<tr><th>Path</th><th>Parameter Field</th><th>Value in This Example</th><th>Meaning</th></tr>
<tr><td rowspan="4">GM to UB</td><td><code>DataCopyParams.blockCount</code></td><td><code>tileM</code></td><td>Number of rows per transfer</td></tr>
<tr><td><code>DataCopyParams.blockLen</code></td><td><code>curCols * sizeof(half)</code></td><td>Continuous bytes transferred per row</td></tr>
<tr><td><code>DataCopyParams.srcStride</code></td><td><code>(n - curCols) * sizeof(half)</code></td><td>Bytes skipped between adjacent rows on the source side</td></tr>
<tr><td><code>DataCopyParams.dstStride</code></td><td><code>0</code></td><td>Stored continuously in UB, no extra row skipping</td></tr>
<tr><td rowspan="4">GM to L1</td><td><code>Nd2NzParams.nValue</code></td><td><code>tileM</code></td><td>Number of rows in the ND source matrix for this transfer</td></tr>
<tr><td><code>Nd2NzParams.dValue</code></td><td><code>curCols</code> or <code>tileN</code></td><td>Number of columns in the ND source matrix for this transfer</td></tr>
<tr><td><code>Nd2NzParams.srcDValue</code></td><td><code>n</code></td><td>Row width of the original matrix in GM</td></tr>
<tr><td><code>Nd2NzParams.dstNzC0Stride</code></td><td><code>AlignUp(tileM, 16)</code></td><td>C0 direction stride of the NZ layout in L1</td></tr>
</table>

In this example, the single DataCopy source data volume is calculated as `tileM * curCols * sizeof(half)`:

<table>
<tr><th>Path</th><th>Scenario</th><th>Tile</th><th>Single Transfer Volume</th></tr>
<tr><td rowspan="3">GM to UB</td><td>Scenario 1</td><td><code>[1,64]</code></td><td><code>128B</code></td></tr>
<tr><td>Scenario 2</td><td><code>[64,64]</code></td><td><code>8192B</code></td></tr>
<tr><td>Scenario 3</td><td><code>[64,1024]</code></td><td><code>131072B</code></td></tr>
<tr><td rowspan="2">GM to L1</td><td>Scenario 1</td><td><code>[64,64]</code></td><td><code>8192B</code></td></tr>
<tr><td>Scenario 2</td><td><code>[64,256]</code></td><td><code>32768B</code></td></tr>
</table>

As blocks become larger, each instruction transfers more data, and the loop count and instruction dispatch count decrease accordingly.

**Core Distribution and Data Loading Pattern**:

```text
Input matrix GM: [M, N]

M direction split by core:
┌────────────── M ──────────────┐
│ core0: singleCoreM rows       │
│ core1: singleCoreM rows       │
│ ...                           │  NUM_BLOCKS cores in parallel
│ last core: singleCoreM rows   │
└───────────────────────────────┘

Single core transfers by tile:
singleCoreM rows
┌──── tileN ────┬──── tileN ────┬──── tileN ────┐
│ tileM rows    │ tileM rows    │ tileM rows    │
├───────────────┼───────────────┼───────────────┤
│ tileM rows    │ tileM rows    │ tileM rows    │
└───────────────┴───────────────┴───────────────┘

GM -> UB: DataCopyPad, executed by AIV cores
GM -> L1: DataCopy(ND2NZ), executed by AIC cores
```

**MTE2 Bandwidth Theoretical Analysis**:

The baseline scenarios in this group only perform a single GM read, without considering L2Cache reuse benefits. The input matrix is `M=12288, N=12288`, data type is half, and the total read data volume is:

$$Total Read Data = M \times N \times sizeof(half) = 12288 \times 12288 \times 2B = 301989888B \approx 301.99MB$$

Rough estimation of MTE2 theoretical duration based on GM peak bandwidth:

$$MTE2 Theoretical Duration = \frac{301.99MB}{GM Peak Bandwidth}$$

For Atlas A2/A3 Series, with GM bandwidth approximately 1.8TB/s, the theoretical duration is approximately 167.77μs. In the large block scenario, GM to UB `aiv_mte2_time` is 202.657μs, and GM to L1 `aic_mte2_time` is 214.519μs, which are approximately 20.8% and 27.9% higher than theoretical values respectively.

For Ascend 950 Series, with GM bandwidth approximately 1.6TB/s, the theoretical duration is approximately 188.74μs. In the large block scenario, GM to UB `aiv_mte2_time` is 185.66μs, and GM to L1 `aic_mte2_time` is 187.19μs, which are close to theoretical estimates. This estimation is only used to judge transfer efficiency magnitude. Actual performance is affected by instruction dispatch, address continuity, DataCopyPad/ND2NZ processing, and other factors.

**GM to UB Performance Data**:

<table>
<tr><th>Architecture</th><th>Scenario</th><th>Configuration</th><th>Task Duration(μs)</th><th>aiv_total_cycles</th><th>aiv_mte2_time(μs)</th><th>aiv_mte2_ratio</th><th>MTE2 Performance Improvement vs Baseline</th><th>Description</th></tr>
<tr><td rowspan="3">Atlas A2/A3 Series</td><td>1</td><td>Tensor=[12288,12288]<br>Tile=[1,64]<br>DataCopyParams={blockCount=1, blockLen=128B, srcStride=24448B}<br>Block Num=48</td><td>564.84</td><td>49222116</td><td>548.161</td><td>0.989</td><td>Baseline</td><td>Small block transfer</td></tr>
<tr><td>2</td><td>Tensor=[12288,12288]<br>Tile=[64,64]<br>DataCopyParams={blockCount=64, blockLen=128B, srcStride=24448B}<br>Block Num=48</td><td>233.54</td><td>20167322</td><td>220.772</td><td>0.972</td><td>+148.3%</td><td>Medium block transfer</td></tr>
<tr><td>3</td><td>Tensor=[12288,12288]<br>Tile=[64,1024]<br>DataCopyParams={blockCount=64, blockLen=2048B, srcStride=22528B}<br>Block Num=48</td><td>215.82</td><td>18563430</td><td>202.657</td><td>0.969</td><td>+170.5%</td><td>Large block transfer</td></tr>
<tr><td rowspan="3">Ascend 950 Series</td><td>1</td><td>Tensor=[12288,12288]<br>Tile=[1,64]<br>DataCopyParams={blockCount=1, blockLen=128B, srcStride=24448B}<br>Block Num=64</td><td>884.06</td><td>91585425</td><td>881.13</td><td>1</td><td>Baseline</td><td>Small block transfer</td></tr>
<tr><td>2</td><td>Tensor=[12288,12288]<br>Tile=[64,64]<br>DataCopyParams={blockCount=64, blockLen=128B, srcStride=24448B}<br>Block Num=64</td><td>208.64</td><td>31489587</td><td>205.99</td><td>0.99</td><td>+327.8%</td><td>Medium block transfer</td></tr>
<tr><td>3</td><td>Tensor=[12288,12288]<br>Tile=[64,1024]<br>DataCopyParams={blockCount=64, blockLen=2048B, srcStride=22528B}<br>Block Num=64</td><td>188.52</td><td>19710997</td><td>185.66</td><td>0.99</td><td>+374.6%</td><td>Large block transfer</td></tr>
</table>

**GM to L1 Performance Data**:

<table>
<tr><th>Architecture</th><th>Scenario</th><th>Configuration</th><th>Task Duration(μs)</th><th>aic_total_cycles</th><th>aic_mte2_time(μs)</th><th>aic_mte2_ratio</th><th>MTE2 Performance Improvement vs Baseline</th><th>Description</th></tr>
<tr><td rowspan="2">Atlas A2/A3 Series</td><td>1</td><td>Tensor=[12288,12288]<br>Tile=[64,64]<br>Nd2NzParams={nValue=64, dValue=64, srcDValue=12288}<br>Block Num=24</td><td>300.7</td><td>12870910</td><td>283.508</td><td>0.978</td><td>Baseline</td><td>Medium block transfer</td></tr>
<tr><td>2</td><td>Tensor=[12288,12288]<br>Tile=[64,256]<br>Nd2NzParams={nValue=64, dValue=256, srcDValue=12288}<br>Block Num=24</td><td>230.24</td><td>9795512</td><td>214.519</td><td>0.972</td><td>+32.2%</td><td>Large block transfer</td></tr>
<tr><td rowspan="2">Ascend 950 Series</td><td>1</td><td>Tensor=[12288,12288]<br>Tile=[64,64]<br>Nd2NzParams={nValue=64, dValue=64, srcDValue=12288}<br>Block Num=32</td><td>245.1</td><td>12480180</td><td>241.56</td><td>0.99</td><td>Baseline</td><td>Medium block transfer</td></tr>
<tr><td>2</td><td>Tensor=[12288,12288]<br>Tile=[64,256]<br>Nd2NzParams={nValue=64, dValue=256, srcDValue=12288}<br>Block Num=32</td><td>190.52</td><td>9853588</td><td>187.19</td><td>0.99</td><td>+29.0%</td><td>Large block transfer</td></tr>
</table>

**Optimization Effect Analysis**:
- Increasing block size significantly reduces transfer instruction dispatch overhead. In GM to UB scenarios, large blocks vs. small blocks improve end-to-end performance by approximately 161.7% on Atlas A2/A3 Series and approximately 369.0% on Ascend 950 Series.
- In GM to L1 scenarios, after increasing `TILE_N`, end-to-end performance improves by approximately 29% or more on both Atlas A2/A3 Series and Ascend 950 Series, indicating that MTE2 transfer efficiency is higher after reducing overhead from excessively small transfer granularity.
- When configuring in practice, it is recommended to prioritize increasing the single transfer size within the limits of on-chip space, while keeping blocks within UB/L1 available cache. The larger the single transfer byte count and the fewer `mLoopCount/nLoopCount`, the lower the DataCopy instruction count and loop control overhead.

### Optimization Point 2: Impact of Unaligned Data Transfer

**Implementation**: The unaligned data scenario reuses the same transfer flow as the aligned scenario, only changing the matrix `N` dimension so that the last transfer block cannot cover the full column width.

When unaligned, the last N-direction tile has `curCols` less than `TILE_N`. In the UB path, `DataCopyParams.blockLen` changes from the full `TILE_N * sizeof(half)` to `curCols * sizeof(half)`; in the L1 path, `Nd2NzParams.dValue` changes from the full `TILE_N` to `curCols`.

In the unaligned scenario of this example, the full tile transfer volume for GM to UB is `64 * 1024 * 2B = 131072B`, and the tail block is `64 * 1023 * 2B = 130944B`; the full tile transfer volume for GM to L1 is `64 * 256 * 2B = 32768B`, and the tail block is `64 * 255 * 2B = 32640B`. The unaligned scenario does not significantly increase total data volume; rather, the tail block requires additional boundary processing, causing transfer efficiency degradation.

**Comparison Method**: Keep block size constant, change `N` from `12288` to `12287`, and observe the impact of unaligned data processing on end-to-end duration and MTE2 duration.

**GM to UB Performance Data**:

<table>
<tr><th>Architecture</th><th>Scenario</th><th>Configuration</th><th>Task Duration(μs)</th><th>aiv_total_cycles</th><th>aiv_mte2_time(μs)</th><th>aiv_mte2_ratio</th><th>Performance Change vs Aligned Scenario</th><th>Description</th></tr>
<tr><td rowspan="2">Atlas A2/A3 Series</td><td>3</td><td>Tensor=[12288,12288]<br>Tile=[64,1024]<br>blockLen=2048B<br>srcStride=22528B<br>Block Num=48</td><td>215.82</td><td>18563430</td><td>202.657</td><td>0.969</td><td>Baseline</td><td>Aligned large block</td></tr>
<tr><td>4</td><td>Tensor=[12288,12287]<br>Tile=[64,1024]<br>full/tail curCols=1024,1023<br>blockLen=2048B,2046B<br>srcStride=22526B,22528B<br>Block Num=48</td><td>275.3</td><td>23648813</td><td>259.047</td><td>0.973</td><td>-21.6%</td><td>Unaligned data large block</td></tr>
<tr><td rowspan="2">Ascend 950 Series</td><td>3</td><td>Tensor=[12288,12288]<br>Tile=[64,1024]<br>blockLen=2048B<br>srcStride=22528B<br>Block Num=64</td><td>188.52</td><td>19710997</td><td>185.66</td><td>0.99</td><td>Baseline</td><td>Aligned large block</td></tr>
<tr><td>4</td><td>Tensor=[12288,12287]<br>Tile=[64,1024]<br>full/tail curCols=1024,1023<br>blockLen=2048B,2046B<br>srcStride=22526B,22528B<br>Block Num=64</td><td>192.07</td><td>19739629</td><td>189.16</td><td>0.99</td><td>-1.8%</td><td>Unaligned data large block</td></tr>
</table>

**GM to L1 Performance Data**:

<table>
<tr><th>Architecture</th><th>Scenario</th><th>Configuration</th><th>Task Duration(μs)</th><th>aic_total_cycles</th><th>aic_mte2_time(μs)</th><th>aic_mte2_ratio</th><th>Performance Change vs Aligned Scenario</th><th>Description</th></tr>
<tr><td rowspan="2">Atlas A2/A3 Series</td><td>2</td><td>Tensor=[12288,12288]<br>Tile=[64,256]<br>Nd2NzParams={nValue=64, dValue=256, srcDValue=12288}<br>Block Num=24</td><td>230.24</td><td>9795512</td><td>214.519</td><td>0.972</td><td>Baseline</td><td>Aligned large block</td></tr>
<tr><td>3</td><td>Tensor=[12288,12287]<br>Tile=[64,256]<br>Nd2NzParams={nValue=64, dValue=256(full),255(tail), srcDValue=12287}<br>Block Num=24</td><td>438.76</td><td>19020328</td><td>422.515</td><td>0.986</td><td>-47.5%</td><td>Unaligned data large block</td></tr>
<tr><td rowspan="2">Ascend 950 Series</td><td>2</td><td>Tensor=[12288,12288]<br>Tile=[64,256]<br>Nd2NzParams={nValue=64, dValue=256, srcDValue=12288}<br>Block Num=32</td><td>190.52</td><td>9853588</td><td>187.19</td><td>0.99</td><td>Baseline</td><td>Aligned large block</td></tr>
<tr><td>3</td><td>Tensor=[12288,12287]<br>Tile=[64,256]<br>Nd2NzParams={nValue=64, dValue=256(full),255(tail), srcDValue=12287}<br>Block Num=32</td><td>206.61</td><td>10589826</td><td>202.97</td><td>0.99</td><td>-7.8%</td><td>Unaligned data large block</td></tr>
</table>

**Optimization Effect Analysis**:
- Unaligned data introduces additional boundary processing, with more significant impact on Atlas A2/A3 Series, especially in the GM to L1 scenario where end-to-end performance degrades by approximately 47.5%.
- On Ascend 950 Series, the impact of unaligned data is relatively smaller but still introduces additional overhead. When selecting blocks, prioritize alignment of the primary transfer dimension.
- It is recommended to use aligned matrices when designing matrix shapes and splitting strategies. Based on the half data type in this example, Atlas A2/A3 Series recommend that the continuous byte count corresponding to the primary transfer dimension satisfies 512B alignment; Ascend 950 Series recommend 128B alignment.

### Optimization Point 3: Repeated Transfer and L2Cache Reuse

**Implementation**: The repeated transfer scenario performs multiple rounds of GM reads on the same data scale, using `msopprof --ai-core=on --aic-metrics=L2Cache` to collect L2Cache read hit and miss allocate data.

For full-block repeated transfer, `RepeatCopy(0, n, 4)` means `nStart=0`, `nCount=N`, `repeatTimes=4`, with each round reading the same data block along the full N direction. A2/A3 has an L2Cache size of 192MB, and 950PR has an L2Cache size of 128MB. In this example, a single `M * N` matrix is approximately 301.99MB. During full-block repeated transfer, the single-round working set exceeds L2Cache capacity, making write eviction likely and resulting in low L2Cache hit rates.

For sliced repeated transfer, first set `quarterN=N/4`, then for each `splitIdx` call `RepeatCopy(splitIdx * quarterN, quarterN, 4)`, meaning each time only 4 consecutive repetitions occur within 1/4 of the N-direction slice. Both approaches transfer the same total data volume, but after slicing, each slice is approximately 75.50MB with a smaller single-round working set, making it more likely to remain in L2Cache during consecutive repeated accesses.

**Comparison Method**: Compare transferring the full matrix 4 times consecutively along the same path vs. slicing the N direction into 4 parts and transferring each part 4 times consecutively. The latter repeats access within each slice consecutively, making it easier to observe L2Cache reuse benefits.

Scenario 5/6 (GM to UB) and Scenario 4/5 (GM to L1) both use `Tile=[64,1024]`. The source data volume for a single `DataCopyPad` or `DataCopy (Nd2NzParams)` transfer is:

$$64 \times 1024 \times sizeof(half) = 64 \times 1024 \times 2B = 131072B = 0.131072MB$$

During full-block repeated transfer, the per-round working set is `301.99MB`, with total read of `1207.96MB` over 4 consecutive rounds; after slicing the N direction into 4 parts, each slice working set is `75.50MB`, with total read data volume still `1207.96MB`, but each slice is more easily retained and reused by L2Cache.

**L2Cache Reuse Pattern**:

```text
Scenario 5(UB) / Scenario 4(L1): Full matrix consecutive repeated transfer

GM matrix: [M, N]
┌─────────────────────────────── N ───────────────────────────────┐
│                    All columns transferred at once                │
└──────────────────────────────────────────────────────────────────┘

Start all cores, transfer the full matrix along the same path:
Round 1: All cores read the full matrix from GM -> UB or L1
Round 2: All cores read the full matrix again -> UB or L1
Round 3: All cores read the full matrix again -> UB or L1
Round 4: All cores read the full matrix again -> UB or L1
Note: Each round working set is the full matrix, making it difficult for L2Cache to fully retain the previous round data

Scenario 6(UB) / Scenario 5(L1): N direction sliced into 4 parts, each part transferred consecutively

GM matrix: [M, N]
┌──────── N/4 ────────┬──────── N/4 ────────┬──────── N/4 ────────┬──────── N/4 ────────┐
│      Slice 0        │      Slice 1        │      Slice 2        │      Slice 3         │
└─────────────────────┴─────────────────────┴─────────────────────┴─────────────────────┘

Start all cores, transfer Slice 0 data 4 consecutive rounds, then process the next slice:
Slice 0: Round 1 reads from GM, Rounds 2-4 preferentially read from L2Cache
Slice 1: Round 1 reads from GM, Rounds 2-4 preferentially read from L2Cache
Slice 2: Round 1 reads from GM, Rounds 2-4 preferentially read from L2Cache
Slice 3: Round 1 reads from GM, Rounds 2-4 preferentially read from L2Cache
Note: Single slice working set is smaller, making it easier to retain in L2Cache during consecutive repeated access
```

**L2Cache Theoretical Performance Analysis**:

The input matrix for this group of scenarios is `M=12288, N=12288`, data type is half, and the single full read data volume is:

$$Single Read Data = M \times N \times sizeof(half) = 12288 \times 12288 \times 2B = 301989888B \approx 301.99MB$$

When the full matrix is transferred 4 times consecutively, the total read data volume is:

$$Full Block Repeated Read Data = 301989888B \times 4 = 1207959552B \approx 1207.96MB$$

This access pattern has a single working set of 301.99MB, which is difficult to fully retain in L2Cache, so it can be approximately estimated as primarily reading from GM:

$$Full Block Repeated Theoretical Duration = \frac{1207.96MB}{GM Bandwidth}$$

After slicing the N direction into 4 parts, the data volume per slice is:

$$Slice Data Volume = 301989888B \div 4 = 75497472B \approx 75.50MB$$

When each slice is transferred 4 times consecutively, ideally the first time reads from GM and the subsequent 3 times read from L2Cache:

$$GM Read Data Volume = 75497472B \times 4 = 301989888B \approx 301.99MB$$

$$L2Cache Read Data Volume = 75497472B \times 3 \times 4 = 905969664B \approx 905.97MB$$

$$Slice Repeated Theoretical Duration = \frac{301.99MB}{GM Bandwidth} + \frac{905.97MB}{L2Cache Bandwidth}$$

Atlas A2/A3 Series estimates use GM bandwidth approximately 1.8TB/s and L2Cache peak bandwidth approximately 5.2TB/s; Ascend 950 Series estimates use GM bandwidth approximately 1.6TB/s and L2Cache peak bandwidth approximately 5.2TB/s.

$$Atlas A2/A3 Series Full Block Repeated Theoretical Duration = \frac{1207.96MB}{1.8TB/s} = 671.09\mu s$$

$$Ascend 950 Series Full Block Repeated Theoretical Duration = \frac{1207.96MB}{1.6TB/s} = 754.97\mu s$$

$$Atlas A2/A3 Series Slice Repeated Theoretical Duration = \frac{301.99MB}{1.8TB/s} + \frac{905.97MB}{5.2TB/s} = 342.00\mu s$$

$$Ascend 950 Series Slice Repeated Theoretical Duration = \frac{301.99MB}{1.6TB/s} + \frac{905.97MB}{5.2TB/s} = 362.97\mu s$$


From both theoretical models and measured results, the duration of N-direction sliced repeated transfer is closer to the ideal model of "first GM + subsequent L2Cache"; full-block repeated transfer has insufficient L2Cache reuse due to larger working sets, with duration closer to multiple GM reads. The GM to L1 scenario includes ND2NZ transfer, and actual duration is also affected by format conversion and L1 write layout, so it is typically higher than pure GM to UB transfer.

**GM to UB L2Cache Performance Data**:

Atlas A2/A3 Series and Ascend 950 Series have different profiler output fields and different hit rate calculation methods: Atlas A2/A3 Series uses `l2cache_hit_ratio = (r0_hit + r1_hit) / (r0_hit + r1_hit + r0_miss_allocate + r1_miss_allocate)`; Ascend 950 Series uses `l2cache_hit_ratio = hit / (hit + miss + victim)`.

<table>
<tr><th>Architecture</th><th>Scenario</th><th>Configuration</th><th>Task Duration(μs)</th><th>aiv_total_cycles</th><th>aiv_time(μs)</th><th>aiv_r0_read_cache_hit</th><th>aiv_r0_read_cache_miss_allocate</th><th>aiv_r1_read_cache_hit</th><th>aiv_r1_read_cache_miss_allocate</th><th>Description</th><th>L2Cache Hit Rate</th></tr>
<tr><td rowspan="2">Atlas A2/A3 Series</td><td>5</td><td>Tensor=[12288,12288]<br>Tile=[64,1024]<br>Block Num=48</td><td>828.06</td><td>72465129</td><td>816.05</td><td>212</td><td>4718595</td><td>217</td><td>4718592</td><td>Full matrix transferred 4 times consecutively along the same path</td><td>0.005%</td></tr>
<tr><td>6</td><td>Tensor=[12288,12288]<br>Tile=[64,1024]<br>Block Num=48</td><td>365.74</td><td>31484525</td><td>354.56</td><td>3539159</td><td>1179644</td><td>3539158</td><td>1179655</td><td>N direction sliced into 4 parts, each part transferred 4 times consecutively</td><td>75.00%</td></tr>
</table>

<table>
<tr><th>Architecture</th><th>Scenario</th><th>Configuration</th><th>Task Duration(μs)</th><th>aiv_total_cycles</th><th>aiv_time(μs)</th><th>aiv_read_local_l2_hit</th><th>aiv_read_local_l2_miss</th><th>aiv_read_local_l2_victim</th><th>Description</th><th>L2Cache Hit Rate</th></tr>
<tr><td rowspan="2">Ascend 950 Series</td><td>5</td><td>Tensor=[12288,12288]<br>Tile=[64,1024]<br>Block Num=64</td><td>741.58</td><td>77700635</td><td>740.75</td><td>31346</td><td>529720</td><td>8358412</td><td>Full matrix transferred 4 times consecutively along the same path</td><td>0.35%</td></tr>
<tr><td>6</td><td>Tensor=[12288,12288]<br>Tile=[64,1024]<br>Block Num=64</td><td>354.95</td><td>36964347</td><td>354.19</td><td>5943026</td><td>528797</td><td>2446732</td><td>N direction sliced into 4 parts, each part transferred 4 times consecutively</td><td>66.64%</td></tr>
</table>

**GM to L1 L2Cache Performance Data**:

<table>
<tr><th>Architecture</th><th>Scenario</th><th>Configuration</th><th>Task Duration(μs)</th><th>aic_total_cycles</th><th>aicore_time(μs)</th><th>aic_r0_read_cache_hit</th><th>aic_r0_read_cache_miss_allocate</th><th>aic_r1_read_cache_hit</th><th>aic_r1_read_cache_miss_allocate</th><th>Description</th><th>L2Cache Hit Rate</th></tr>
<tr><td rowspan="2">Atlas A2/A3 Series</td><td>4</td><td>Tensor=[12288,12288]<br>Tile=[64,1024]<br>Block Num=24</td><td>899.34</td><td>38725327</td><td>872.19</td><td>305</td><td>4718591</td><td>287</td><td>4718601</td><td>Full matrix ND2NZ transferred 4 times consecutively along the same path</td><td>0.006%</td></tr>
<tr><td>5</td><td>Tensor=[12288,12288]<br>Tile=[64,1024]<br>Block Num=24</td><td>414.96</td><td>17662155</td><td>397.8</td><td>3539061</td><td>1179684</td><td>3539133</td><td>1179616</td><td>N direction sliced into 4 parts, each part ND2NZ transferred 4 times consecutively</td><td>75.00%</td></tr>
</table>

<table>
<tr><th>Architecture</th><th>Scenario</th><th>Configuration</th><th>Task Duration(μs)</th><th>aic_total_cycles</th><th>aicore_time(μs)</th><th>aic_read_local_l2_hit</th><th>aic_read_local_l2_miss</th><th>aic_read_local_l2_victim</th><th>Description</th><th>L2Cache Hit Rate</th></tr>
<tr><td rowspan="2">Ascend 950 Series</td><td>4</td><td>Tensor=[12288,12288]<br>Tile=[64,1024]<br>Block Num=32</td><td>732.809</td><td>37950511</td><td>732.11</td><td>269472</td><td>2557819</td><td>4060060</td><td>Full matrix ND2NZ transferred 4 times consecutively along the same path</td><td>3.91%</td></tr>
<tr><td>5</td><td>Tensor=[12288,12288]<br>Tile=[64,1024]<br>Block Num=32</td><td>390.347</td><td>19858177</td><td>389.64</td><td>5948502</td><td>1148258</td><td>1220548</td><td>N direction sliced into 4 parts, each part ND2NZ transferred 4 times consecutively</td><td>71.52%</td></tr>
</table>

**Optimization Effect Analysis**:
- After N-direction slicing, repeated access within slices provides more sufficient L2Cache reuse, with significant end-to-end performance improvement on both Atlas A2/A3 Series and Ascend 950 Series for both GM to UB and GM to L1.
- Atlas A2/A3 Series requires accumulating observations across both r0+r1 hardware read channels, with total read Cache hits significantly increasing and miss allocate significantly decreasing; Ascend 950 Series shows corresponding behavior with `*_read_local_l2_hit` increasing and miss/victim decreasing.
- When the same batch of GM data needs to be read multiple times, prioritize sliced transfer and complete multiple accesses consecutively within each slice, keeping the single working set within the L2Cache reusable range.

### Optimization Point 4: Multi-Core Same-Address Access Conflict Avoidance

**Implementation**: In the same addr scenario, all cores access the input matrix in the same `mBlockIdx` order; in the offset addr scenario, each core staggers the access order by `(mBlockIdx + blockIdx) % numBlocks`.

This optimization point does not change the single DataCopy transfer shape, only changes the order in which different cores access GM slices. Each core fully loads the input matrix once, resulting in the same matrix being read `numBlocks` times overall. In the same addr pattern, all cores synchronously access the same address range on the same `mBlockIdx`; in the offset addr pattern, `curMBlockIdx` rotates by `blockIdx` within each group of `numBlocks` M blocks, reducing the probability of multiple cores accessing the same GM address range at the same time.

**Key Code**:

```cpp
constexpr uint32_t fullMBlockCount = m / singleCoreM;
constexpr uint32_t mTileCount = singleCoreM / tileM;

for (uint32_t mBlockIdx = 0; mBlockIdx < fullMBlockCount; mBlockIdx++) {
    uint32_t blockGroupStart = (mBlockIdx / numBlocks) * numBlocks;
    uint32_t curMBlockIdx = offsetAddr ? blockGroupStart + (mBlockIdx + blockIdx) % numBlocks : mBlockIdx;
    uint32_t mStart = curMBlockIdx * singleCoreM;

    for (uint32_t mTileIdx = 0; mTileIdx < mTileCount; mTileIdx++) {
        uint32_t mIdx = mStart + mTileIdx * tileM;
    }
}
```

In this example, each DataCopy transfers in the N direction at `tileN` granularity; when `offsetAddr=true`, the order of access slices per core is staggered so that different cores access different GM address ranges at the same time.

**Comparison Method**: All cores fully load the same input matrix, comparing same-order access vs. core-staggered access order. Staggered access adjusts the parallel slice access order to reduce the probability of multiple cores accessing the same address at the same time.

**GM to UB Performance Data**:

<table>
<tr><th>Architecture</th><th>Scenario</th><th>Configuration</th><th>Task Duration(μs)</th><th>aiv_total_cycles</th><th>aiv_mte2_time(μs)</th><th>aiv_mte2_ratio</th><th>Description</th></tr>
<tr><td rowspan="2">Atlas A2/A3 Series</td><td>7</td><td>Tensor=[6144,512]<br>Tile=[128,64]<br>Block Num=48</td><td>539.42</td><td>42823246</td><td>474.717</td><td>0.984</td><td>All cores load fully in the same order</td></tr>
<tr><td>8</td><td>Tensor=[6144,512]<br>Tile=[128,64]<br>Block Num=48</td><td>328.88</td><td>28054500</td><td>307.38</td><td>0.973</td><td>All cores load fully with staggered slice order</td></tr>
<tr><td rowspan="2">Ascend 950 Series</td><td>7</td><td>Tensor=[8192,512]<br>Tile=[128,64]<br>Block Num=64</td><td>342.29</td><td>33624098</td><td>339.38</td><td>0.99</td><td>All cores load fully in the same order</td></tr>
<tr><td>8</td><td>Tensor=[8192,512]<br>Tile=[128,64]<br>Block Num=64</td><td>335.64</td><td>35298771</td><td>333.08</td><td>0.99</td><td>All cores load fully with staggered slice order</td></tr>
</table>

**GM to L1 Performance Data**:

<table>
<tr><th>Architecture</th><th>Scenario</th><th>Configuration</th><th>Task Duration(μs)</th><th>aic_total_cycles</th><th>aic_mte2_time(μs)</th><th>aic_mte2_ratio</th><th>Description</th></tr>
<tr><td rowspan="2">Atlas A2/A3 Series</td><td>6</td><td>Tensor=[6144,512]<br>Tile=[256,64]<br>Block Num=24</td><td>278.56</td><td>10880150</td><td>240.198</td><td>0.98</td><td>All cores load fully in the same order</td></tr>
<tr><td>7</td><td>Tensor=[6144,512]<br>Tile=[256,64]<br>Block Num=24</td><td>221.34</td><td>9532153</td><td>209.8</td><td>0.977</td><td>All cores load fully with staggered slice order</td></tr>
<tr><td rowspan="2">Ascend 950 Series</td><td>6</td><td>Tensor=[8192,512]<br>Tile=[256,64]<br>Block Num=32</td><td>369.99</td><td>13312673</td><td>366.24</td><td>0.99</td><td>All cores load fully in the same order</td></tr>
<tr><td>7</td><td>Tensor=[8192,512]<br>Tile=[256,64]<br>Block Num=32</td><td>187.35</td><td>9713703</td><td>185.03</td><td>0.99</td><td>All cores load fully with staggered slice order</td></tr>
</table>

**Optimization Effect Analysis**:
- Offset addr reduces the probability of multiple cores accessing the same GM address range at the same time by staggering the multi-core access order. Atlas A2/A3 Series UB/L1 scenarios and Ascend 950 Series L1 scenarios show more obvious benefits.
- From an end-to-end performance perspective, Atlas A2/A3 Series UB scenario improves by approximately 64.0%, Ascend 950 Series L1 scenario improves by approximately 97.5%; Ascend 950 Series UB scenario shows smaller benefits.

### Optimization Summary

| Optimization Method | Core Principle | Usage Recommendation |
|:---|:---|:---|
| Increase transfer block size | Reduce DataCopy instruction count and loop control overhead, improve MTE2 effective transfer efficiency | When on-chip space allows, prioritize larger `TILE_M/TILE_N` |
| Maintain primary dimension alignment | Avoid boundary processing and incomplete transfer overhead from unaligned data | When designing shapes or splitting strategies, try to make the primary transfer dimension divisible by `TILE_N`, and ensure continuous transfer byte count satisfies Atlas A2/A3 Series 512B, Ascend 950 Series 128B alignment |
| Sliced repeated access | Restrict repeated access to a smaller data range to improve L2Cache hit probability | When the same batch of GM data needs to be read multiple times, prioritize slicing first then repeating within each slice |
| Stagger multi-core access order | Reduce the probability of multiple cores accessing the same GM address range at the same time | When multiple cores read the same large data block, rotate access slice order by `blockIdx` |

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.


  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in this example directory.
  ```bash
  SCENARIO_NUM=1 ASC_ARCH=dav-2201
  COPY_DST=UB
  mkdir -p build && cd build
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCOPY_DST=$COPY_DST -DCMAKE_ASC_ARCHITECTURES=$ASC_ARCH ..;make -j; 
  python3 ../scripts/gen_data.py -scenarioNum $SCENARIO_NUM -copyDst $COPY_DST -arch $ASC_ARCH
  ./demo
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:

  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCOPY_DST=$COPY_DST -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..
  make -j
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Parameter | Description | Values | Default |
  |------|------|--------|--------|
  | `SCENARIO_NUM` | Scenario number | `COPY_DST=UB`: 1-8; `COPY_DST=L1`: 1-7 | 1 |
  | `COPY_DST` | Transfer destination | `UB`, `L1` | `UB` |
  | `CMAKE_ASC_RUN_MODE` | Run mode | `npu`, `sim` | `npu` |
  | `CMAKE_ASC_ARCHITECTURES` | NPU hardware architecture | `dav-2201`, `dav-3510` | `dav-2201` |

- Performance collection

  Use the `msOpProf` tool to obtain detailed performance data:

  ```bash
  msopprof ./demo
  msopprof --ai-core=on --aic-metrics=L2Cache ./demo    # Use for L2Cache related scenarios
  ```

    - Performance data description  
      After the command completes, a folder named "OPPROF_{timestamp}_XXX" will be generated in the default directory. The performance data folder structure is as follows:

      ```bash
      ├──dump                       # Raw performance data, no user attention needed
      ├──ArithmeticUtilization.csv  # Cube/Vector instruction cycle ratio
      ├──L2Cache.csv                # L2 Cache hit rate, affects MTE2, suggests reasonable data transfer logic to increase hit rate
      ├──Memory.csv                 # UB, L1 and main memory read/write bandwidth rate
      ├──MemoryL0.csv               # L0A, L0B, and L0C read/write bandwidth rate
      ├──MemoryUB.csv               # Vector and Scalar to UB read/write bandwidth rate
      ├──OpBasicInfo.csv            # Operator basic information
      ├──PipeUtilization.csv        # Computation unit and transfer unit time and ratio
      ├──ResourceConflictRatio.csv  # Bank group, bank conflict and resource conflict ratio on UB in all instructions
      └──visualize_data.bin         # MindStudio Insight presentation file
      ```

  View the specific performance analysis results:

  ```bash
  # View Task Duration and various data
  cat ./OPPROF_*/PipeUtilization.csv
  ```
