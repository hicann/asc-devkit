# Type Alignment Optimization for Data Transfer Efficiency Best Practice Example

## Overview

  This example quantitatively demonstrates **the impact of struct type alignment on Global Memory access**: leveraging the 1B, 2B, 4B, 8B and 16B width memory access instructions supported by Ascend 950PR/Ascend 950DT, multiple structs of different sizes are constructed for element-wise data transfer, comparing the transfer efficiency differences brought by type alignment.

## Supported Products

  - Ascend 950PR/Ascend 950DT

## Supported CANN Software Version

  - \>= CANN 9.1.0

## Directory Structure

  ```
  ├── aligned_types
  │   ├── aligned_types.asc      // SIMT implementation & invocation example
  │   ├── CMakeLists.txt         // CMake build file
  │   └── README.md
  ```

## Example Description

This example selects 14 structs, grouped by `sizeof` into 4B, 8B, 16B, and 32B groups. Each group has aligned and non-aligned comparisons; the 4B group covers `alignof=1` and `alignof=2` narrow alignment levels, plus 2 mixed-field struct comparisons with inconsistent field types. During testing, an input array `d_idata` of length `num_elements` is copied entirely to `d_odata`, and transfer efficiency is compared.

- Basic Concepts

  **1. alignof**

  A compile-time operator that returns the alignment requirement of an object, that is, the number of bytes that the starting address of the object must be a multiple of. The default `alignof` of a struct equals the maximum `alignof` of all its fields, and can be forcibly increased using the `alignas` keyword. For example, for `struct T` and `struct T2` below, `T` has `alignof` of 4B, and `T2` has `alignof` of 8B.

  ```cpp
  struct T { uint8_t a; uint32_t b; };  // alignof = max(1, 4) = 4
  struct alignas(8) T2 { uint8_t a; uint32_t b; };  // alignof = 8 (alignas increases alignment)
  ```

  **2. alignas**

  An alignment specifier introduced in C++11 that forces increased alignment when declaring objects.

  Restrictions of `alignas`:

  - **Can only increase, not decrease**: The `N` in `alignas(N)` must be greater than or equal to the default `alignof` of the type, that is, the maximum `alignof` of all fields; otherwise the compiler will ignore it or report an error.
  - **Must be a power of 2**: The `N` in `alignas(N)` must be a power of 2.
  - **Also increases sizeof**: `sizeof` must be a multiple of `alignof`, so when increasing `alignof`, `sizeof` rounds up.

  ```cpp
  struct alignas(4) Align3 { unsigned char r, g, b; }; // alignof=4 sizeof=4 (increased with alignas)
  struct Misalign3 { unsigned char r, g, b; }; // alignof=1 sizeof=3 (default)
  ```

- Key Parameters

  | Parameter | Value | Description |
  |------|-----|------|
  | MEM_BYTES      | **512MiB** | Input/output buffer size in bytes |
  | THREAD_COUNT   | 2048   | 2048 threads per core, grid fixed at `<<<1, 2048>>>` |
  | num_elements   | MEM_BYTES / sizeof(TData) | Number of elements per case, automatically scales with struct size |

- Example Specifications

  | Name              | Fields                    | sizeof | alignof | num_elements  |
  |------------------|--------------------------|-------|--------|-----------|
  | Align4           | 4 x u8                   |  4B   |   4B   | 134217728 |
  | Misalign4        | 4 x u8                   |  4B   | **1B** | 134217728 |
  | AlignU8U16       | 2 x u8 + u16             |  4B   |   4B   | 134217728 |
  | MisalignU8U16    | 2 x u8 + u16             |  4B   | **2B** | 134217728 |
  | AlignU8PadU16    | u8 (1B padding) + u16    |  4B   |   4B   | 134217728 |
  | MisalignU8PadU16 | u8 (1B padding) + u16    |  4B   | **2B** | 134217728 |
  | Align2U16        | 2 x u16                  |  4B   |   4B   | 134217728 |
  | Misalign2U16     | 2 x u16                  |  4B   | **2B** | 134217728 |
  | Align8           | 2 x u32                  |  8B   |   8B   | 67108864  |
  | Misalign8        | 2 x u32                  |  8B   | **4B** | 67108864  |
  | Align16          | 4 x u32                  |  16B  |  16B   | 33554432  |
  | Misalign16       | 4 x u32                  |  16B  | **4B** | 33554432  |
  | Align32          | 8 x u32                  |  32B  |  32B   | 16777216  |
  | Misalign32       | 8 x u32                  |  32B  | **4B** | 16777216  |

- Example Implementation

  - Kernel Implementation  

    This example has a fixed total data size of 512MiB, using 2048 threads on a single core to perform data transfer. The kernel function is implemented as a template, supporting different types and alignment structs, performing element-wise transfer operations. The specific implementation is as follows:

    ```
    constexpr uint32_t THREAD_COUNT = 2048;                  // threads per block
    constexpr size_t MEM_BYTES = 512ULL * 1024ULL * 1024ULL; // total data size 512 MiB
    uint32_t num_elements = static_cast<uint32_t>(MEM_BYTES / sizeof(TData));

    template <class TData>
    __global__ __launch_bounds__(THREAD_COUNT) void aligned_type_kernel(
        TData* d_odata, TData* d_idata, uint32_t num_elements)
    {
        const uint32_t tid = blockIdx.x * blockDim.x + threadIdx.x;
        const uint32_t num_threads = blockDim.x * gridDim.x;
        for (uint32_t pos = tid; pos < num_elements; pos += num_threads) {
            d_odata[pos] = d_idata[pos];
        }
    }
    ```

  - Invocation Implementation  

    Use the kernel invocation operator `<<<>>>` to call the kernel function.

## Performance Analysis

- Performance Metrics Description

  |             Field Name          | Field Description                                             |
  |:---------------------------:|:-------------------------------------------------|
  |      Task Duration(us)      | Total task duration, including scheduling time to the accelerator, execution time on the accelerator, and response completion time.          |
  |      aiv_total_cycles       | Total CPU cycles consumed by the task executing on the Vector Core.          |

  Except for Task Duration, all other metrics in this example show the average values across all blocks.

- Performance Data

  |       TData       | Task Duration(μs) | aiv_total_cycles |
  | :---------------: | :---------------: | :--------------: |
  |      Align4       |     27889.33      |   46005213.32    |
  |     Misalign4     |     81223.02      |   134028903.60   |
  |     Align2U16     |     27833.56      |   45918496.16    |
  |   Misalign2U16    |     46656.61      |   76990974.19    |
  |    AlignU8U16     |     27854.56      |   45939796.81    |
  |   MisalignU8U16   |     46666.11      |   76970946.68    |
  |   AlignU8PadU16   |     27809.72      |   45872788.00    |
  | MisalignU8PadU16  |     46664.42      |   76969440.94    |
  |      Align8       |     17417.23      |   28735088.65    |
  |     Misalign8     |     21287.91      |   35112582.91    |
  |      Align16      |     27931.29      |   46090787.94    |
  |    Misalign16     |     38353.60      |   63284236.16    |
  |      Align32      |     52357.78      |   86396567.61    |
  |    Misalign32     |     77790.93      |   128352303.30   |

- Comprehensive Analysis

  The total data transferred in all test cases is uniformly 512MiB; given the same struct size, type alignment makes memory access instructions wider, resulting in less total time and higher transfer efficiency. The latency differences between aligned and non-aligned groups are summarized as follows:

  | Struct Size                  | Aligned Task Duration(us) | Non-aligned Task Duration(us)  | Non-aligned / Aligned Task Duration |
  | :------------------------: | :-------------------: | :---------------------: | :---------: |
  | 4B (4xu8)                  | 27889.33 (alignof=4)  | 81223.02 (alignof=1)    | **2.91x**   |
  | 4B (2xu16)                 | 27833.56 (alignof=4)  | 46656.61 (alignof=2)    | **1.68x**   |
  | 4B (2xu8 + u16)            | 27854.56 (alignof=4)  | 46666.11 (alignof=2)    | **1.68x**   |
  | 4B (u8 + 1B padding + u16) | 27809.72 (alignof=4)  | 46664.42 (alignof=2)    | **1.68x**   |
  | 8B                         | 17417.23 (alignof=8)  | 21287.91 (alignof=4)    | **1.22x**   |
  | 16B                        | 27931.29 (alignof=16) | 38353.60 (alignof=4)    | **1.37x**   |
  | 32B                        | 52357.78 (alignof=32) | 77790.93 (alignof=4)    | **1.49x**   |

  From the table above:

  (1) With the same total data size, structs with the same `sizeof` show significantly higher transfer efficiency when type-aligned compared to non-aligned. The 4B group shows the most significant contrast. As `alignof` increases from `1B to 2B to 4B`, the `Non-aligned Task Duration / Aligned Task Duration` ratio decreases, meaning transfer latency decreases progressively. This is because `alignof` affects the bit width of memory access instructions; smaller widths result in longer transfer times.
  
  > Note that **doubling the instruction width does not equal halving the latency**: the kernel function uses a for loop to load/store data element by element, and load/store instructions between different elements can execute in pipeline parallel. Taking elements `x1` and `x2` as an example, after `load x1` completes, `store x1` and `load x2` can enter the memory pipeline simultaneously, and the execution time of multiple memory accesses is partially hidden by pipelining. Therefore, the measured speedup ratio for the 4B group is less than the theoretical upper bound calculated from instruction counts.

  (2) The three 4B struct groups (2xu16, 2xu8 + u16, u8 + 1B padding + u16) show no difference in `Task Duration` when compared vertically, indicating that whether fields are the same type or mixed types, and whether padding exists, does not affect transfer efficiency. The compiler only cares about the overall `sizeof` and `alignof` of the struct.

  In summary, with the same total data size, type alignment improves transfer efficiency for structs of the same size, and higher `alignof` yields higher efficiency, with an upper limit of 16B.

- Tuning Recommendations

  Ascend 950PR/Ascend 950DT supports five memory access instruction widths: 1B, 2B, 4B, 8B and 16B. Therefore, it is recommended to select the highest possible width that does not exceed the struct size.

## Build and Run

- Configure Environment Variables  
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example

  Run the following commands in this example directory.

  ```bash
  mkdir -p build && cd build           # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 .. && make -j   # Build the project
  ./demo                                # Run the example
  ```

- Build Options Description

  | Option                      | Values     | Description                                                         |
  | ------------------------- | ---------- | ----------------------------------------------------------- |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT)  |

- Execution Result

  The following output indicates that the example ran successfully.

  ```bash
  [Success] Case accuracy is verification passed.
  ```

## Performance Data Collection

  Use the `msOpProf` tool to collect performance data on a single component:

  ```bash
  msopprof ./demo   # Analyze example performance
  ```

  After the command completes, a folder named "OPPROF_{timestamp}_XXX" is generated in the default directory. The performance data folder structure example is as follows:

  ```text
  ├──dump                       # Raw performance data, no user attention needed
  ├──ArithmeticUtilization.csv  # cube/vector instruction cycle ratio
  ├──L2Cache.csv                # L2 Cache hit rate
  ├──Memory.csv                 # UB, L1, and main memory read/write bandwidth
  ├──MemoryL0.csv               # L0A, L0B, and L0C read/write bandwidth
  ├──MemoryUB.csv               # Vector and Scalar to UB read/write bandwidth
  ├──OpBasicInfo.csv            # Operator basic information
  ├──PipeUtilization.csv        # Compute unit and transfer unit duration and ratio
  ├──ResourceConflictRatio.csv  # UB bank group, bank conflict, and resource conflict ratio across all instructions
  └──visualize_data.bin         # MindStudio Insight presentation file
  ```

  View specific performance analysis results:

  ```
  # For example, view Task Duration related data
  cat ./OPPROF_*/OpBasicInfo.csv
  ```
