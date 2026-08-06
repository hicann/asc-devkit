# SIMT Programming Mode Histogram Example

## Overview

This example demonstrates how to use the Ascend C SIMT [`asc_atomic_add()`](../../../../../../docs/zh/api/SIMT-API/atomic_operations/asc_atomic_add.md) interface to efficiently count the frequency of each byte value in an input byte sequence. The function illustration is shown below:<br>
<img src="figures/introduction.png" alt="intro" style="width: 50%; height: auto;">

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version

- \>= CANN 9.1.0

## Directory Structure

```text
├── histogram
│   ├── figures                // Image resources for README
│   ├── CMakeLists.txt         // Example build script
│   ├── histogram.asc          // Histogram example function implementation
│   └── README.md
```

## Example Description

- Example Function:  
  The Histogram example counts the frequency of each byte value (0-255) in a fixed-size input byte stream. The input data shape is [294912].

- Example Specifications:
  <table>
  <tr><td align="center">Example Type (OpType)</td><td colspan="4" align="center">histogram</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[294912]</td><td align="center">uint8</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">actual_histogram</td><td align="center">[256]</td><td align="center">uint32</td><td align="center">ND</td></tr>
  </table>

- Data Partitioning:
  * Phase 1 Kernel `accumulate_block_local_histogram`:
    * ThreadBlock count: Uses the maximum physical core count of `72`.
    * Threads per ThreadBlock: Uses the default value of 1024.
    * Per-thread processing: Traverses `uint32` input in grid-stride, parsing 4 bytes each time and updating the local histogram of the corresponding Warp.
  * Phase 2 Kernel `merge_block_local_histogram`:
    * ThreadBlock count: `256`, each ThreadBlock is responsible for one bin, where a bin represents a single count unit in the histogram array.
    * Threads per ThreadBlock: To use the reduction algorithm, uses the first power of 2 greater than 72, which is `128`.
    * Per-thread processing: Each thread reads counts of the same bin from several local histograms and accumulates them, then reduces within the ThreadBlock to obtain the final count.
  * Expected Result: The 256 bin counts output by the NPU should match the results computed by the CPU based on the same input.

## Example Implementation
The overall flow is divided into two Kernels. In the first phase, the input is partitioned into ThreadBlocks, and each ThreadBlock generates a local histogram. In the second phase, each ThreadBlock merges all local histograms by bin to obtain the final 256-bin result.

### Phase 1

Overall flow: Obtain input from GM, then partition the input data and assign it to corresponding ThreadBlocks. Each ThreadBlock further partitions the input data and assigns it to corresponding Warps. Each Warp is responsible for computing a local histogram. The count for each bin in the Warp's local histogram is obtained through atomic accumulation using `asc_atomic_add` in UB. Finally, the local histograms computed by each Warp are merged to obtain the current ThreadBlock's local histogram, and the result is written back to GM. The flow diagram is as follows:

<img src="figures/local_his.png" alt="local_his" style="width: 50%; height: auto;">

Notes:
  1. Since UB has lower read/write latency, histogram count write operations are performed on UB.
  2. The maximum available UB space is 216KB. The current number of launched cores is 72, and each core uses the default configuration of 1024 threads (that is, 32 Warps). Since each Warp requires 1KB of space, 32 Warps require 32KB total, which can be placed in UB at once.
  3. If all Warps in a ThreadBlock maintain a single local histogram, it would cause severe thread conflicts and affect performance. Therefore, each Warp maintains its own local histogram. Threads within the current Warp only compete to write to the corresponding local histogram. Threads within a Warp read input in grid-stride and use `asc_atomic_add` to increment the count of the corresponding bin for each byte by 1, then merge all Warp local histograms within the ThreadBlock. The `asc_atomic_add` is used for atomic accumulation of bin counts in the local histogram of the same Warp, ensuring correct update results when multiple threads simultaneously hit the same bin.<br>

### Phase 2

Overall flow: Obtain local histogram data from GM. Each ThreadBlock is responsible for summing one bin across all local histograms. Each thread writes the corresponding bin count from the local histograms into UB, then performs reduction summation in UB, and finally writes the result to GM.
The flow diagram is as follows: The figure below uses Block0 as an example. Block0 is responsible for computing the count of the bin at index 0, that is, bin[0]. Each thread in this Block writes bin[0] from the corresponding local histogram to UB. For example, thread 0 writes bin[0] from local histogram 0 to position index 0 in UB, thread 1 writes bin[0] from local histogram 1 to position index 1 in UB, and so on. Finally, UB stores the bin[0] count from each local histogram, then the data on UB is reduced and summed to obtain the total bin[0], and the result is written back to GM.

<img src="figures/merge_his.png" alt="merge_his" style="width: 50%; height: auto;">

## Build and Run
Run the following steps in the root directory of this example to build and execute the example.
- Configure Environment Variables  
  Configure environment variables based on the [installation method](../../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit in the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example

  Run the following commands in this example directory.

  ```bash
  mkdir -p build && cd build      # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..   # Configure the project
  make -j                         # Build the example
  ./histogram                     # Run the example
  ```

  Build Options Description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |

  The following output indicates that the accuracy verification is successful.
  
  ```text
  Running histogram256 on Ascend C SIMT for fixed xxx bytes
  Validation passed
  ```
