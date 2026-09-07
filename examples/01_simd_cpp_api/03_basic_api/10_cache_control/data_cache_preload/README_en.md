# DataCachePreload Indirect Index Preload Sample

## Overview
**This sample is for testing and validation only, not for actual operator usage.** This sample demonstrates how to use [DataCachePreload](../../../../../docs/zh/api/SIMD-API/basic_api/cache_control/DataCachePreload.md) in an indirect index (random-hopping) access scenario: when the Scalar unit reads scattered data from GM, DataCachePreload is used to proactively load upcoming Cache Lines into DCache, reducing Scalar memory access wait time and improving performance.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```text
├── data_cache_preload
│   ├── scripts
│   │   ├── gen_data.py             // Input data and ground truth generation script
│   │   └── verify_result.py        // Verification script for output vs golden data
│   ├── CMakeLists.txt              // Build project file
│   ├── data_utils.h                // Data read/write utility functions
│   ├── data_cache_preload.asc      // Ascend C sample implementation & invocation
│   └── README.md                   // Sample documentation
```

## Sample Description

This sample uses SCENARIO_NUM to control the execution branch, with different values corresponding to whether DataCachePreload is used.

| SCENARIO_NUM | Scenario | Description |
|-----------------|---------|------|
| 1 | Indirect index accumulation (no preload) | Direct Scalar reads without DataCachePreload |
| 2 | Indirect index accumulation (with preload) | Uses DataCachePreload to pre-fetch next Cache Line into DCache |

### Formula and Sample Specifications

Both scenarios share the same formula for indirect index accumulation:

$$
result = \sum_{i=0}^{255} data\_gm[indices[i]]
$$

- `data_gm` is a 64KB random `uint8_t` data buffer.
- `indices` contains 256 random offsets, each pointing to a different Cache-Line-aligned position.
- Since `indices` are randomly scattered across 64KB, the vast majority of Scalar reads trigger a DCache miss.
- `DataCachePreload` asynchronously pre-fetches the next Cache Line while reading current data, forming a pipeline between Scalar reads and GM to DCache data movement.

#### SCENARIO_NUM=1 (Indirect Index Accumulation, without DataCachePreload)

Sample Type (OpType): DataCachePreloadNoPreload

| name | size | data type | Description |
|------|------|-----------|-------------|
| data_gm | [65536] | uint8_t | 64KB random data buffer (1024 Cache Lines) |
| indices | [256] | uint32_t | Random indices, Cache-Line-aligned |
| result | [1] | uint32_t | Accumulated sum (sample output) |

Kernel Name: kernel_no_preload

#### SCENARIO_NUM=2 (Indirect Index Accumulation, with DataCachePreload)

Sample Type (OpType): DataCachePreloadWithPreload

| name | size | data type | Description |
|------|------|-----------|-------------|
| data_gm | [65536] | uint8_t | 64KB random data buffer (1024 Cache Lines) |
| indices | [256] | uint32_t | Random indices, Cache-Line-aligned |
| result | [1] | uint32_t | Accumulated sum (sample output) |

Kernel Name: kernel_with_preload

## Sample Implementation

### SCENARIO_NUM=1 (Without DataCachePreload)

`kernel_no_preload` implementation:

1. Create a `GlobalTensor<uint64_t>` bound to data_gm and copy GM data to UB via `DataCopy`:

    ```cpp
    AscendC::GlobalTensor<uint64_t> dataGlobal;
    dataGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ uint64_t*>(data_gm),
                               bufferSize / sizeof(uint64_t));
    AscendC::LocalTensor<uint64_t> xLocal = ubAllocator.Alloc<uint64_t, dataLen / 8>();
    AscendC::DataCopy(xLocal, dataGlobal, dataLen / 8);
    ```

2. Directly read `data_gm[indices[i]]` via Scalar in the loop and accumulate:

    ```cpp
    for (uint32_t i = 0; i < count; i++) {
        uint32_t idx = indices[i];
        sum += data_gm[idx];
    }
    ```

3. Since indices are randomly scattered across 64KB, most Scalar reads trigger DCache misses, making memory access latency the main bottleneck.

### SCENARIO_NUM=2 (With DataCachePreload)

`kernel_with_preload` implementation:

1. Create a `GlobalTensor<uint64_t>` bound to data_gm and copy GM data to UB via `DataCopy` (same as Scenario 1).
2. Compute Cache-Line-aligned offset: `cacheOffset = idx & ~63ULL` (align to 64B boundary).
3. Preload the first index's Cache Line before the loop via `DataCachePreload(dataGlobal, cacheOffset)`.
4. In the loop: read current data → preload the next Cache Line, forming a pipeline:

    ```cpp
    // First preload
    uint32_t idxCurr = indices[0];
    int64_t cacheOffset = (int64_t)(idxCurr & ~63ULL);
    AscendC::DataCachePreload(dataGlobal, cacheOffset);

    for (uint32_t i = 0; i < count; i++) {
        // Read current (preloaded in previous iteration, likely DCache hit)
        uint8_t val = data_gm[idxCurr];
        sum += val;
        // Preload next
        if (i + 1 < count) {
            uint32_t idxNext = indices[i + 1];
            int64_t offsetNext = (int64_t)(idxNext & ~63ULL);
            AscendC::DataCachePreload(dataGlobal, offsetNext);
            idxCurr = idxNext;
        }
    }
    ```

DataCachePreload Constraints:

- Each call preloads exactly 1 Cache Line (64B).
- Consecutive calls should not exceed 4, otherwise the Reservation Station may stall.
- Preload is an async instruction that can overlap with subsequent computation/memory access.

## Performance Data and Analysis

When profiling with `msprof`, compare the execution time of the two kernels:

- `kernel_no_preload`: High DCache miss rate, large Scalar read latency.
- `kernel_with_preload`: Preloads the next round's data in advance, significantly improving DCache hit rate.

### Performance Comparison

The following performance data is the average of 5 independent runs to eliminate random fluctuations.

#### Atlas A2 Training Series Products (dav-2201 Architecture)

| Metric | kernel_no_preload | kernel_with_preload | Improvement |
|------|------------------|---------------------|---------|
| **Task Duration (us)** | 31.237 | 19.840 | **1.57x speedup** |
| **aiv_scalar_time (us)** | 29.254 | 17.748 | **Reduced by 39.3%** |

#### Ascend 950 Series Products (dav-3510 Architecture)

| Metric | kernel_no_preload | kernel_with_preload | Improvement |
|------|------------------|---------------------|---------|
| **Task Duration (us)** | 31.018 | 20.445 | **1.52x speedup** |
| **aiv_scalar_time (us)** | 28.344 | 17.910 | **Reduced by 36.8%** |

Metric descriptions:
**Task Duration**: End-to-end kernel execution time, the most direct performance indicator.
**aiv_scalar_time**: Scalar unit execution time; in indirect index scenarios, Scalar memory access is the main bottleneck.

#### Detailed Data from 5 Independent Runs (Atlas A2 Training Series)

| Run | no_preload Duration(us) | with_preload Duration(us) | Speedup |
|---------|------------------------|--------------------------|--------|
| Run 1 | 31.261 | 20.280 | 1.54x |
| Run 2 | 30.941 | 19.361 | 1.60x |
| Run 3 | 30.361 | 19.341 | 1.57x |
| Run 4 | 31.120 | 19.940 | 1.56x |
| Run 5 | 32.501 | 20.280 | 1.60x |
| **Average** | **31.237** | **19.840** | **1.57x** |

#### Detailed Data from 5 Independent Runs (Ascend 950 Series)

| Run | no_preload Duration(us) | with_preload Duration(us) | Speedup |
|---------|------------------------|--------------------------|--------|
| Run 1 | 31.130 | 20.375 | 1.53x |
| Run 2 | 30.914 | 20.344 | 1.52x |
| Run 3 | 30.526 | 20.552 | 1.49x |
| Run 4 | 31.554 | 20.399 | 1.55x |
| Run 5 | 30.968 | 20.554 | 1.51x |
| **Average** | **31.018** | **20.445** | **1.52x** |

### Conclusion

**DataCachePreload demonstrates significant optimization in indirect index (random-hopping) scenarios**. By proactively loading the next round's data into DCache, **Scalar memory access latency is effectively hidden, achieving approximately 1.5x performance improvement**.

## Compilation and Execution

Execute the following steps in this sample's root directory.

- Configure Environment Variables
  Please refer to the [installation guide](../../../../../docs/zh/quick_start.md#prepare&install) to configure environment variables based on your CANN installation.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```
  > **Note:** `${install_path}` is the CANN package installation path; defaults to `/usr/local/Ascend`.

- Sample Execution
  This sample uses the `SCENARIO_NUM` parameter to run the two kernel functions independently:
  - `SCENARIO_NUM=1`: Runs `kernel_no_preload` (without DataCachePreload)
  - `SCENARIO_NUM=2`: Runs `kernel_with_preload` (with DataCachePreload)

  Run the following commands in the sample root directory:
  ```bash
  SCENARIO_NUM=1;                                                           # Set scenario number (1=no preload, 2=with preload)
  mkdir -p build && cd build;                                               # Create and enter build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # Build project (default NPU mode)
  python3 ../scripts/gen_data.py                                            # Generate test input data
  msprof ./demo $SCENARIO_NUM
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify results
  ```

  For CPU debug or NPU simulation modes, add `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim`:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Note:** Run `rm CMakeCache.txt` in the build directory before switching build modes.

- Build Options

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU, CPU debug, or NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | dav-2201 for Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 for Ascend 950 series |
  | `SCENARIO_NUM` | `1` (default), `2` | Scenario: 1 (no preload), 2 (with preload) |

- Result
  The output below indicates that the results are correct:
  ```bash
  test pass!
  ```
