# Matmul Basic API Computation Based on Static Tensor Programming

## Overview

This sample implements multi-core matrix multiplication computation based on the static Tensor programming paradigm.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|-----------------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── matmul_basic_api
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and golden data
│   │   └── verify_result.py    // Golden value comparison file
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read and write functions
│   ├── matmul_basic_api.asc    // Ascend C sample implementation & invocation sample
│   └── README.md               // Sample documentation
```

## Sample Description

- Sample Functionality:  
  This sample uses Ascend C basic API to implement a basic matrix multiplication (Matmul) [kernel function](../../../../../docs/zh/guide/编程指南/编程模型/AI-Core-SIMD编程/核函数.md). The matrix multiplication formula is as follows:
  $$
  C = A * B
  $$
  Where matrix A has shape `[M, K]`, matrix B has shape `[K, N]`, and output matrix C has shape `[M, N]`. For each element `C[m, n]` in the output matrix C, the product of row `m` of matrix A and column `n` of matrix B along the K axis accumulates. In matrix multiplication, **M direction** refers to the row direction of matrix C, **N direction** refers to the column direction of matrix C, and **K direction** refers to the inner dimension (accumulation dimension) of matrix C multiplication.

- Sample Specifications:  
  This sample uses parameters `M = 256, N = 256, K = 64`, with both input and output in `half` type and [`ND`](../../../../../docs/zh/guide/技术附录/概念原理和术语/神经网络和算子/数据排布格式.md) format. The sample launches 2 cores to complete the computation, with each core responsible for 128 rows in the M axis direction and all 256 columns in the N axis direction of the output matrix C:
  - Core 0 computes rows `0~127` of matrix C.
  - Core 1 computes rows `128~255` of matrix C.

  The input and output specifications are shown in the following table:
  <table>
  <tr><td rowspan="1" align="center">Sample Type(OpType)</td><td colspan="4" align="center">Matmul</td></tr>
  <tr><td rowspan="3" align="center">Sample Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">A</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">B</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Sample Output</td><td align="center">C</td><td align="center">[M, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">mmad_custom</td></tr>
  </table>

- Sample Implementation:
  - Kernel-side Overall Approach
    - `mmad_custom` is a [`__global__`](../../../../../docs/zh/guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md) [`__cube__`](../../../../../docs/zh/guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md) kernel function, which indicates that this function runs on the [Cube](../../../../../docs/zh/guide/技术附录/概念原理和术语/术语表.md) computation unit of [AI Core](../../../../../docs/zh/guide/技术附录/概念原理和术语/术语表.md), primarily used for matrix computation.
    - The sample uses the [static Tensor programming method](../../../../../docs/zh/guide/编程指南/编程模型/AI-Core-SIMD编程/基于Tensor的CPP编程/静态Tensor编程.md) and creates [`LocalTensor`](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md) through [`LocalMemAllocator`](../../../../../docs/zh/api/SIMD-API/基础API/资源管理/LocalMemAllocator/LocalMemAllocator简介.md).
    - `CUBE_BLOCK = 16` indicates that the half data type fractal is `16 x 16`, and the code performs [`LoadData`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md) transfers in units of `16 x 16` fractals.

  - Kernel-side Detailed Process
    - Create [`GlobalTensor`](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/GlobalTensor/GlobalTensor简介.md)`<half>` objects `aGM`, `bGM`, `cGM`, representing matrices A, B, C in [GM (Global Memory)](../../../../../docs/zh/guide/编程指南/高级编程/硬件实现/基本架构.md).
    - Obtain the current core ID through [AscendC::GetBlockIdx()](../../../../../docs/zh/api/SIMD-API/基础API/工具接口/系统资源与变量/GetBlockIdx.md) and calculate `mIterIdx`. This sample only splits tasks along the M axis, so each core only needs to process its own M-axis slice of matrix A and matrix C.
    - Set GM address offsets:
      - `aGM` offset by `mIterIdx * singleCoreM * K`, enabling the current core to read its assigned row block of matrix A.
      - `bGM` has no offset, as each core needs to read the complete matrix B.
      - `cGM` offset by `mIterIdx * singleCoreM * N`, enabling the current core to write results back to its assigned row block of matrix C.
    - Create static `LocalTensor` through `LocalMemAllocator`:
      - `a1Local`: Temporary storage of matrix A in L1.
      - `a2Local`: Temporary storage of matrix A in L0A, for [`Mmad`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/Mmad.md) to read.
      - `b1Local`: Temporary storage of matrix B in L1. `a1Local` and `b1Local` use the same L1 allocator and are allocated in order, avoiding manual L1 address offset maintenance.
      - `b2Local`: Temporary storage of matrix B in L0B, for `Mmad` to read.
      - `cLocal`: Temporary storage of matrix multiplication result in L0C.
    - Call [`DataCopy`](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopy（GMToUB随路转换ND2NZ搬运）.md) to transfer matrices A and B from GM to L1. Use [`Nd2NzParams`](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopy（GMToUB随路转换ND2NZ搬运）.md) parameters to convert the input [ND](../../../../../docs/zh/guide/技术附录/概念原理和术语/神经网络和算子/数据排布格式.md) format data to [Nz](../../../../../docs/zh/guide/技术附录/概念原理和术语/神经网络和算子/数据排布格式.md) format required by Cube computation during the transfer.
    - Call [`SetFlag<HardEvent::MTE2_MTE1>`](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) and [`WaitFlag<HardEvent::MTE2_MTE1>`](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) for synchronization. `DataCopy` belongs to the MTE2 pipeline, and the subsequent `LoadData` belongs to the [MTE1](../../../../../docs/zh/guide/技术附录/概念原理和术语/术语表.md) pipeline. [MTE1](../../../../../docs/zh/guide/技术附录/概念原理和术语/术语表.md) must wait for [MTE2](../../../../../docs/zh/guide/技术附录/概念原理和术语/术语表.md) to complete, to avoid reading L1 data that has not finished transferring.
    - Call `LoadData` to transfer matrix A from L1 to L0A, and matrix B from L1 to L0B. L0A and L0B are input buffers that the Cube matrix computation unit reads directly.
    - Call [`SetFlag<HardEvent::MTE1_M>`](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) and [`WaitFlag<HardEvent::MTE1_M>`](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) for synchronization. `LoadData` belongs to the MTE1 pipeline, and the subsequent `Mmad` belongs to the M pipeline. The M pipeline must wait for MTE1 to complete, to avoid reading L0A/L0B data that has not finished transferring.
    - Call [`Mmad`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/Mmad.md)`(cLocal, a2Local, b2Local, {baseM, baseN, baseK, 0, false, true})` to execute matrix multiplication. Here `baseM = 128`, `baseN = 256`, `baseK = 64`, corresponding to the matrix block size computed by a single core at one time.
    - Call [`SetFlag<HardEvent::M_FIX>`](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) and [`WaitFlag<HardEvent::M_FIX>`](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) for synchronization. [`Mmad`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/Mmad.md) belongs to the M pipeline, and the subsequent [`Fixpipe`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md) belongs to the FIX pipeline. The FIX pipeline must wait for the M pipeline to complete, to avoid reading L0C results that have not finished computing.
    - Call [`Fixpipe`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md) to convert the `float` accumulation result in L0C to `half` and transfer it back to the matrix C output location in GM.
    - Finally, call [`PipeBarrier`](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/PipeBarrier(ISASI).md)`<PIPE_ALL>()` to ensure that related pipeline tasks within the current core complete.

  - Invocation Implementation  
    Use the [kernel invocation operator](../../../../../docs/zh/guide/编程指南/编程模型/AI-Core-SIMD编程/核函数.md)`<<<>>>` to invoke the [kernel function](../../../../../docs/zh/guide/编程指南/编程模型/AI-Core-SIMD编程/核函数.md). When invoking, pass matrix specifications, single-core computation amount, and basic tile size as template parameters, and pass Device-side A, B, C matrix addresses as runtime parameters.

- API Parameter Description:

  The following structures all use curly braces `{}` for parameter passing. The meaning of each field is as follows (field order is consistent with API documentation; actual struct declarations may have different field orders):

  **[`AscendC::Nd2NzParams`](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopy（GMToUB随路转换ND2NZ搬运）.md)** — Used by [`DataCopy`](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopy（GMToUB随路转换ND2NZ搬运）.md) interface, describes ND→Nz format conversion parameters:
  ```cpp
  struct Nd2NzParams {
      int32_t  ndNum;              // Number of ND matrices to transfer, [0, 4095]
      uint16_t nValue;             // Number of rows in ND matrix, [0, 16384]
      int32_t  dValue;             // Number of columns in ND matrix, [0, 65535]
      int32_t  srcNdMatrixStride;  // Offset between starting addresses of adjacent ND matrices, unit: element, [0, 65535]
      int32_t  srcDValue;          // Offset between adjacent rows of the same ND matrix, unit: element, [1, 65535]
      uint16_t dstNzC0Stride;      // Adjacent row offset after conversion of multiple rows from the same source row in destination Nz, unit: C0_SIZE(32B), [1, 16384]
      uint16_t dstNzNStride;       // Adjacent row offset of Z-shaped matrices in destination Nz, unit: C0_SIZE(32B), [1, 16384]
      int32_t  dstNzMatrixStride;  // Starting address offset between adjacent Nz matrices in destination Nz, unit: element, [1, 65535]
  };
  ```
  For example, when transferring matrix A, `{1, baseM, baseK, 0, K, baseM, 1, 0}` converts baseM×baseK ND data to Nz format.

  **[`AscendC::LoadData2DParams`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md)** — Used by [`LoadData`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md) interface, describes data transfer parameters for matrix A L1 to L0A and matrix B L1 to L0B in Atlas A2 Training Series Products/Atlas A2 Inference Series Products, Atlas A3 Training Series Products/Atlas A3 Inference Series Products:

  ```cpp
  struct LoadData2DParams {
      int32_t startIndex;   // Fractal matrix ID (0 is the first), unit: 512B, [0, 65535]
      int32_t repeatTimes;  // Number of iterations, each iteration processes 512B, [1, 255]
      int32_t srcStride;    // Source fractal starting address interval between adjacent iterations, unit: 512B, [0, 65535]
      int32_t sid;          // Reserved, set to 0
      int32_t dstGap;       // Destination fractal interval between adjacent iterations, unit: 512B, [0, 65535]
      bool    ifTranspose;  // Whether to transpose each fractal, default false
      bool    addrMode;     // Address update mode, false=increment, true=decrement, default false
  };
  ```
  For example: In Atlas A2 Training Series Products/Atlas A2 Inference Series Products, Atlas A3 Training Series Products/Atlas A3 Inference Series Products, the layout format on L0A is Zz. When transferring matrix A, use `{0, baseK / CUBE_BLOCK, baseM / CUBE_BLOCK, 0, 0, false, 0}`; <br>
  When transferring matrix B, set `ifTranspose=true` to complete Nz to Zn transpose transfer.

  **[`AscendC::LoadData2DParamsV2`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md)** — Used by [`LoadData`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md) interface, describes data transfer parameters for matrix A L1 to L0A and matrix B L1 to L0B in Ascend 950PR/Ascend 950DT products:
  ```cpp
  struct LoadData2DParamsV2 {
      uint32_t mStartPosition;  // M direction start position, unit: 512B
      uint32_t kStartPosition;  // K direction start position, unit: 512B
      uint16_t mStep;           // Number of fractals transferred in M direction
      uint16_t kStep;           // Number of fractals transferred in K direction
      int32_t  srcStride;       // Source fractal interval in K direction, unit: 512B
      uint16_t dstStride;       // Destination fractal interval in K direction, unit: 512B
      bool     ifTranspose;     // Whether to transpose each fractal, default false
      uint8_t  sid;             // Reserved, set to 0
  };
  ```
  In Ascend 950PR/Ascend 950DT products, the layout format on L0A is Nz. When transferring matrix A, use `{0, 0, baseM / CUBE_BLOCK, baseK / CUBE_BLOCK, baseM / CUBE_BLOCK, baseM / CUBE_BLOCK, false, 0}` to complete A matrix Nz to Nz transfer in one operation; when transferring matrix B, use `{0, 0, baseK / CUBE_BLOCK, baseN / CUBE_BLOCK, baseK / CUBE_BLOCK, baseN / CUBE_BLOCK, true, 0}` to complete B matrix Nz to Zn transfer in one operation.

  **[`AscendC::MmadParams`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/Mmad.md)** — Used by [`Mmad`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/Mmad.md) interface, describes matrix multiplication parameters:

  ```cpp
  struct MmadParams {
      uint16_t m;               // Left matrix Height (M dimension), [0, 4095]
      uint16_t n;               // Right matrix Width (N dimension), [0, 4095]
      uint16_t k;               // Left matrix Width/Right matrix Height (K dimension), [0, 4095]
      uint16_t unitFlag;        // Mmad and Fixpipe fine-grained parallelism control, default 0
      bool     cmatrixSource;   // C matrix initial value source, false=CO1, true=C2, default false
      bool     cmatrixInitVal;  // Whether C matrix initial value is 0, default true
  };
  ```
  For example, `{baseM, baseN, baseK, 0, false, true}` computes a baseM×baseN output block and accumulates baseK length in the K direction.

  **[`AscendC::FixpipeParamsV220`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)** — Used by [`Fixpipe`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md) interface, describes L0C to GM data transfer and precision conversion parameters:
  ```cpp
  struct FixpipeParamsV220 {
      int32_t     nSize;        // Source Nz matrix N direction size, [1, 4095]
      uint16_t    mSize;        // Source Nz matrix M direction size (for Nz2ND, [1, 8192])
      uint16_t    srcStride;    // Source Nz adjacent Z layout starting offset, unit: C0_SIZE, [0, 65535]
      int32_t     dstStride;    // Number of elements per row in destination ND matrix for Nz2ND, unit: element
      bool        reluEn;       // Whether to enable ReLU
      QuantMode_t quantPre;     // Quantization mode, F322F16 means float→half
      uint64_t    deqScalar;    // Scalar quantization parameter, single scale value
      int32_t     ndNum;        // Number of source Nz matrices, [1, 65535]
      int32_t     srcNdStride;  // Starting address interval between different Nz matrices, unit: 16×C0_SIZE, [1, 512]
      int32_t     dstNdStride;  // Destination adjacent ND matrix offset, unit: element, [1, 65535]
      int32_t     unitFlag;     // Mmad and Fixpipe parallelism control
  };
  ```
  For example, `{baseN, baseM, baseM, N, false, F322F16, 0, 1, 0, 0, 0}` converts the baseM×baseN float32 result in L0C to half and writes it back to GM.

## Compilation and Execution

Execute the following steps in the root directory of this sample to compile and run the sample.
- Configure Environment Variables  
  Configure environment variables according to the [installation method](../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit package on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. If no installation directory is specified, it is installed to `/usr/local/Ascend` by default.
- Sample Execution

  Execute the following commands in this sample directory.
  ```bash
  mkdir -p build && cd build;                                               # Create and enter build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # Build project (default npu mode)
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Execute the compiled executable program to run the sample
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify output result correctness, confirm algorithm logic is correct
  ```

  When using CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Note:** Before switching compilation modes, clear the cmake cache. Execute `rm CMakeCache.txt` in the build directory and run cmake again.

- Compilation Options Description

  | Option | Available Values | Description |
  |--------|------------------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution Result  
  The execution result is as follows, indicating successful precision comparison.
  ```bash
  test pass!
  ```

## Functional Debugging

### printf

This interface provides formatted output functionality for CPU domain or NPU domain debugging scenarios.

Call the [printf](../../../../../docs/zh/api/SIMD-API/基础API/调试接口/上板打印/printf.md) interface in the operator kernel-side implementation code where log information needs to be output to print relevant content.

Example:

```cpp
AscendC::printf("matmul blockIdx=%d\n", AscendC::GetBlockIdx());
```

> [!CAUTION]
> The printf (PRINTF) interface printing functionality has a certain impact on the actual running performance of the operator and is typically used during the debugging phase. Developers can disable the printing functionality by setting ASCENDC_DUMP=0 as needed.

### DumpTensor

For operators developed based on operator projects, you can use the [DumpTensor](../../../../../docs/zh/api/SIMD-API/基础API/调试接口/上板打印/DumpTensor.md) interface to dump the content of a specified Tensor. It also supports printing custom additional information (only uint32_t data type information is supported), such as printing the current line number.

Call the DumpTensor interface in the operator kernel-side implementation code where Tensor data needs to be printed. Example:

```cpp
AscendC::DumpTensor(cLocal, baseM * baseN);
```

> [!CAUTION]
> The DumpTensor interface printing functionality has a certain impact on the actual running performance of the operator and is typically used during the debugging phase. Developers can disable the printing functionality by setting ASCENDC_DUMP=0 as needed.

## Performance Debugging

### Introduction to the msOpProf Tool

msOpProf is a single-operator performance analysis tool with two usage modes: `msopprof` and `msopprof simulator`. It helps users identify anomalies in operator memory, code, and instructions for comprehensive operator tuning. It currently supports performance data collection and automatic parsing for different run modes (on-device or simulation) and file types (executables or operator binary `.o` files).

- On-device performance collection

    On-device performance collection directly measures the operator's execution time on the Ascend AI Processor. This method is suitable for quickly locating operator performance issues in an on-device environment.

    Run msopprof on the `demo` executable for operator tuning:
    ```
    msopprof ./demo
    ```

    - Performance data description  
      After the command completes, a folder named "OPPROF_{timestamp}_XXX" will be generated in the default directory. The performance data folder structure is as follows:

      ```bash
      ├──dump                       # Raw performance data; users do not need to inspect it
      ├──ArithmeticUtilization.csv  # Cube/Vector instruction cycle proportions
      ├──L2Cache.csv                # L2 Cache hit rate; affects MTE2. Plan data transfer logic properly to increase the hit rate
      ├──Memory.csv                 # Read/write bandwidth rates of UB, L1, and main memory
      ├──MemoryL0.csv               # Read/write bandwidth rates of L0A, L0B, and L0C
      ├──MemoryUB.csv               # Read/write bandwidth rates from Vector and Scalar to UB
      ├──OpBasicInfo.csv            # Basic operator information
      ├──PipeUtilization.csv        # Durations and proportions of computation and data transfer units
      ├──ResourceConflictRatio.csv  # Proportions of UB bank groups, bank conflicts, and resource conflicts among all instructions
      └──visualize_data.bin         # MindStudio Insight presentation file
      ```
