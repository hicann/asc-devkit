# Matmul Sample

## Overview

This sample uses the Matmul high-level API to implement matrix multiplication.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|----------------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── matmul_advanced_api
│   ├── scripts
│   │   ├── gen_data.py         // Input data and golden data generation script
│   │   └── verify_result.py    // Golden value comparison file
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read and write functions
│   ├── matmul_advanced_api.asc // Ascend C sample implementation & invocation sample
│   └── README.md               // Sample documentation
```

## Sample Description

- Sample Function:
  This sample uses the Ascend C Matmul high-level API to implement matrix multiplication. The Matmul high-level API encapsulates details such as data movement during matrix computation, Cube computation scheduling, and basic pipeline synchronization. Developers primarily need to complete matrix specification configuration, tiling generation, input/output Tensor setup, and result write-back.

  The matrix multiplication formula is:
  $$
  C = A * B
  $$
  where matrix A has shape `[M, K]`, matrix B has shape `[K, N]`, and output matrix C has shape `[M, N]`. Each element `C[m, n]` in matrix C is the accumulated result of element-wise multiplication along the K axis between row `m` of matrix A and column `n` of matrix B.

- Sample Specifications:
  This sample uses parameters `M = 512, N = 512, K = 128`. In the code, the K axis of matrix A uses `Ka`, and the K axis of matrix B uses `Kb`. In this sample, `Ka = Kb = K = 128`. Input matrices A and B are of `half` type and `ND` format. Output matrix C is of `float` type and `ND` format. The input and output specifications are shown in the following table:
  <table>
  <tr><td rowspan="1" align="center">Sample Type(OpType)</td><td colspan="4" align="center">Matmul</td></tr>
  <tr><td rowspan="3" align="center">Sample Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">A</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">B</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Sample Output</td><td align="center">C</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">matmul_custom</td></tr>
  </table>

  This sample is a pure Cube matrix computation scenario and generates tiling with a fixed 2 Cube cores. Under this sample's specifications, the tiling result divides `M = 512` equally among 2 cores, with each core processing `singleCoreM = 256`, `singleCoreN = 512`, and `singleCoreK = 128`. The original K dimension corresponds to `tiling.Ka = tiling.Kb = 128`.

- Sample Implementation:
  - Tiling Generation Process
    - Create a `matmul_tiling::MultiCoreMatmulTiling` object `tilingApi` to generate tiling parameters required for multi-core Matmul.
    - `SetDim(2)` indicates that up to 2 Cube cores can be used during multi-core Matmul tiling calculation. In pure Cube matrix computation scenarios, `GetTiling` generates the actual number of cores used `usedCoreNum` within this core count constraint.
    - `SetAType`, `SetBType`, and `SetCType` set the data source location, data format, and data type for matrices A, B, and C respectively, which must match the `MatmulType` template parameters on the Kernel side.
    - `SetOrgShape(M, N, K)` sets the original complete matrix shape, and `SetShape(M, N, K)` sets the actual `M, N, K direction sizes (in elements)` participating in Matmul calculation.
    - `EnableBias(false)` indicates that this sample does not use bias.
    - `SetBufferSpace(-1, -1, -1)` sets the L1/L0C/UB space sizes available for Matmul. Passing `-1` means using the default size of the corresponding buffer on the current AI processor, and the tiling interface selects the base block and transfer strategy accordingly.
    - `GetTiling(tilingData)` generates the final tiling result. A return value of `-1` indicates tiling calculation failure, and the tiling result cannot be used further. After generating `TCubeTiling` on the Host side, it is passed directly as a Kernel parameter.

  - Kernel Side Overall Approach
    - `ASCENDC_CUBE_ONLY` indicates the current mode is pure cube (matrix computation only).
    - `matmul_custom` is a `__global__ __cube__` kernel function that runs on the Cube computation unit.
    - The `tiling` parameter type in Kernel inputs is `AscendC::tiling::TCubeTiling`, which is generated on the Host side and passed directly as a Kernel parameter. The Kernel side uses this parameter to control core distribution, base block size, and Matmul internal buffer usage.
    - Create `GlobalTensor` objects `aGlobal`, `bGlobal`, and `cGlobal` to represent matrices A, B, and C in GM respectively. `GlobalTensor` only describes the address and element count in GM; actual data movement and L1/L0 partitioning are completed by the Matmul high-level API combined with tiling.
    - Create a high-level API object `mm`. The `MatmulType` template parameters describe the location, format, and data type of matrices A/B/C on the Kernel side. In this sample, all are in GM and ND format, with A/B as `half` and C as `float`.
    - The Host side obtains the system workspace size through `GetLibApiWorkSpaceSize()`, allocates `workspaceDevice`, and passes it as the Kernel parameter `workspace`.
    - Register the Matmul object through `REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling)`.
    - The Host side starts the Kernel with `numBlocks = tiling.usedCoreNum`, and all started Blocks participate in actual computation.
    - Call `mm.SetOrgShape(tiling.M, tiling.N, tiling.Ka, tiling.Kb)` to set the original complete matrix shape, in elements. This interface must be called before `SetTensorA` and `SetTensorB`.
    - Call `mm.SetTensorA(aGlobal[GetBlockIdx() * tiling.Ka * tiling.singleCoreM], false)` to set the starting address of matrix A for the current core to read. The 0th core starts reading from row 0 of matrix A, and the 1st core starts reading from row 256 of matrix A. The second parameter `false` indicates no transpose.
    - Call `mm.SetTensorB(bGlobal[0], false)` to set the starting address of matrix B. Both cores need the complete matrix B for computation, so both matrix B addresses start from the base address. The second parameter `false` indicates no transpose.
    - Call `mm.IterateAll(cGlobal[GetBlockIdx() * tiling.singleCoreM * tiling.N])` to execute all Matmul computations for the current core and write the results back to the corresponding offset position in matrix C.
    - Call `mm.End()` to end the use of the current Matmul object and release internal computation resources. If there are other Matmul objects later, calling `End` avoids resource conflicts between multiple Matmul objects.

  - Core Distribution and Address Offset Description
    - `GetBlockIdx()` indicates the current core number. The 0th core processes rows 0 to 255 of matrix C, and the 1st core processes rows 256 to 511 of matrix C.
    - Matrix A has shape `[512, 128]`, with each row length being `tiling.Ka = 128`, so the matrix A offset is `GetBlockIdx() * tiling.Ka * tiling.singleCoreM`.
    - Matrix B has shape `[128, 512]`, corresponding to `[Kb, N]` in the code. The two cores compute different rows of matrix C respectively, but each output element requires complete K axis accumulation, so both cores read the complete matrix B from `bGlobal[0]`.
    - Matrix C has shape `[512, 512]`, with each row length being `tiling.N = 512`, so the matrix C write-back offset is `GetBlockIdx() * tiling.singleCoreM * tiling.N`.

  - Invocation Implementation
    Use the kernel call operator `<<<>>>` to invoke the kernel function. In pure Cube scenarios, `numBlocks` comes from `tiling.usedCoreNum` during invocation. Runtime parameters are passed in order: Device-side matrix A address, matrix B address, matrix C address, workspace address, and Host-side generated tiling data.

## Build and Run

Execute the following steps in the root directory of this sample to build and run the sample.
- Configure Environment Variables
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development toolkit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. If no installation directory is specified, it defaults to `/usr/local/Ascend`.

- Sample Execution

  Execute the following commands in this sample directory.
  ```bash
  mkdir -p build && cd build;                                               # Create and enter build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # Build project (default npu mode)
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Execute the compiled program to run the sample
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify output correctness and confirm algorithm logic
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Note:** Before switching build modes, clean the cmake cache by executing `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build Option Description

  | Option | Available Values | Description |
  |--------|------------------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution Result
  The following execution result indicates successful precision comparison.
  ```bash
  test pass!
  ```
