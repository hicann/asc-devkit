# Matmul and LeakyRelu Fusion Example

## Overview

This example uses high-level APIs to implement fused computation of Matmul and LeakyRelu activation function, achieving fusion computation of matrix operation units and vector operation units.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|----------------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── matmul_leakyrelu_advanced_api
│   ├── CMakeLists.txt                    // Build project file
│   ├── data_utils.h                      // Data read and write functions
│   ├── matmul_leakyrelu_advanced_api.asc // Ascend C example implementation & invocation example
│   ├── scripts
│   │   ├── gen_data.py                   // Input data and golden data generation script
│   │   └── verify_result.py              // Golden data comparison file
│   └── README.md                         // Example documentation
```

## Example Description

- Example Function:
  This example uses the Ascend C Matmul high-level API to implement fused computation of matrix multiplication, Bias addition, and LeakyRelu activation function. The Matmul high-level API encapsulates details such as data movement during matrix computation, Cube computation scheduling, and basic pipeline synchronization. Developers primarily need to complete matrix shape configuration, tiling generation, input Tensor setup, Vector-side fusion computation, and result write-back.

  The MatmulLeakyRelu computation formula is as follows:
  ```
  C = A * B + Bias
  C = C > 0 ? C : C * 0.001
  ```
  Where matrix A has shape `[M, K]`, matrix B has shape `[K, N]`, Bias has shape `[N]`, and output matrix C has shape `[M, N]`. The matrix multiplication result first adds Bias, then performs LeakyRelu activation on each output element.

- Example Specification:
  This example has parameters `M = 512, N = 128, K = 128`. In the code, the K axis of matrix A uses `Ka`, and the K axis of matrix B uses `Kb`. In this example, `Ka = Kb = K = 128`. Inputs A and B are of type `half`, Bias and output C are of type `float`, all in `ND` format. The input and output specifications are shown in the following table:
  <table>
  <tr><td rowspan="1" align="center">Example Type(OpType)</td><td colspan="4" align="center">MatmulLeakyRelu</td></tr>
  <tr><td rowspan="4" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">A</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">B</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">Bias</td><td align="center">[N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">C</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">matmul_leakyrelu_custom</td></tr>
  </table>

  The example uses `__mix__(1, 2)` to declare the kernel function. The Host side launches 1 AI Core combination with `numBlocks = 1`. The current tiling only splits cores along the N axis, while the M axis and K axis do not split cores.

- Example Implementation:
  - Tiling Generation Process
    - Create a `matmul_tiling::MultiCoreMatmulTiling` object `tilingApi` for generating tiling parameters required for multi-core Matmul.
    - `SetDim(2)` indicates that the tiling side configures 2 AIV logical cores to participate in Matmul computation initiation.
    - `SetAType`, `SetBType`, `SetCType`, `SetBiasType` respectively set the data source location, data format, and data type for A, B, C, and Bias. The C type is set to `VECIN` to facilitate subsequent LeakyRelu activation.
    - `SetOrgShape(M, N, K)` sets the original complete matrix shape, and `SetShape(M, N, K)` sets the actual size in the M, N, and K directions participating in Matmul computation (in elements).
    - `EnableBias(true)` indicates that Matmul computation includes Bias input.
    - `SetFixSplit(128, 64, 128)` fixes the Matmul basic block size.
    - `SetBufferSpace(-1, -1, ubSize - usedUb)` sets the L1/L0C/UB space sizes available for Matmul. Passing `-1` for L1 and L0C means using the default size for the corresponding buffer of the current AI processor. `usedUb = baseM * baseN * sizeof(float)` is used to reserve a UB buffer needed for LeakyRelu.
    - `GetTiling(tilingData)` generates the final tiling result. If `-1` is returned, it indicates tiling computation failure, and the tiling result cannot be used further.

  - Kernel-side Overall Approach
    - `matmul_leakyrelu_custom` is a `__global__ __mix__(1, 2)` kernel function that contains both Cube-side and Vector-side computation logic at runtime.
    - The A, B, Bias, and C in the Kernel parameters all use `__gm__ uint8_t*` to represent byte addresses on GM. Inside the Kernel, A/B are fixed as `half`, C/Bias are fixed as `float`, and converted to corresponding `GlobalTensor` for use. This example calls `matmul_leakyrelu_custom` without passing template parameters.
    - The workspace parameter uses `__kfc_workspace__` modifier, indicating the system workspace allocated and passed by the Host side. When registering the Matmul object on the Kernel side, this workspace is passed to `REGIST_MATMUL_OBJ` via `GetSysWorkSpacePtr()`.
    - Create `GlobalTensor` objects `aGlobal`, `bGlobal`, `cGlobal`, and `biasGlobal` to represent A, B, C, and Bias data in GM. `GlobalTensor` only describes the address and number of elements on GM. The actual data movement and L1/L0 partitioning are completed by the Matmul high-level API combined with tiling.
    - On the Kernel side, use `GetBlockIdx()` to obtain the current AIV-side logical task number and calculate the slice number of the current AIV logical core in the N direction.
    - Matrix A reuses the starting address on each logical core, while matrix B, Bias, and matrix C need to be offset in the N direction according to the AIV logical core number.
    - Create `TQue<AscendC::TPosition::VECIN, 1>` to manage the intermediate result cache from Matmul output to Vector side, and create `TQue<AscendC::TPosition::VECOUT, 1>` to manage the Vector-side LeakyRelu output cache. Use `pipe.InitBuffer` to allocate on-chip cache for each `baseM * baseN` output block.
    - Create Matmul high-level API object `matmulObj`. The `MatmulType` template parameter describes the location, format, and data type of A/B/C/Bias on the Kernel side.
    - Register the Matmul object via `REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), matmulObj, &tiling)`. During registration, `TPipe`, system workspace, Matmul object, and corresponding `TCubeTiling` are passed to the Matmul high-level API for internal use.
    - Call `SetTensorA`, `SetTensorB`, and `SetBias` to bind the A, B, and Bias inputs that the current logical block needs to read.

  - Matmul and LeakyRelu Fusion Process
    - `while (matmulObj.template Iterate<true>())` executes the Matmul output block that the current logical core is responsible for. This example keeps `baseM = 128` and `baseN = 64` unchanged. Each AIV logical core needs 4 `Iterate<true>()` calls to complete its own `512 * 64` output region.
    - `mmOutQueue.AllocTensor<float>()` allocates a LocalTensor cache from the Vector input queue to hold the Matmul output block.
    - `matmulObj.template GetTensorC<true>(mmOutLocal, false, true)` obtains the C intermediate result of the current block from the Matmul object. Since C type is set to `VECIN`, this intermediate result can serve as Vector-side LeakyRelu input.
    - `AscendC::LeakyRelu(reluOutLocal, mmOutLocal, static_cast<float>(0.001), tiling.baseM * tiling.baseN)` performs element-wise LeakyRelu activation on the current block and writes the result to `reluOutLocal`.
    - `mmOutQueue.EnQue` and `mmOutQueue.DeQue` establish data dependency between Matmul result write and subsequent LeakyRelu read. `reluOutQueue.EnQue` and `reluOutQueue.DeQue` establish data dependency between Vector computation and data movement out.
    - `AscendC::DataCopy` moves the activated `baseM * baseN` block to the corresponding position in the GM output matrix C.
    - `matmulObj.End()` notifies the Matmul object that the current core computation has ended and releases the internal state of the Matmul high-level API.

  - Core Splitting and Address Offset Description
    - The 0th AIV logical core processes columns 0 to 63 of matrix C, and the 1st AIV logical core processes columns 64 to 127.
    - Matrix A has shape `[512, 128]`. When splitting cores in the N direction, each logical block needs complete M axis and K axis inputs, so matrix A does not have core splitting offset.
    - Matrix B has shape `[128, 128]`, corresponding to `[Kb, N]` in the code. Different logical blocks are responsible for different columns of matrix C. The starting offset of matrix B is `blockIdx * tiling.singleCoreN`, pointing to the starting column of the current N-direction slice.
    - Bias has shape `[128]`, corresponding to 128 columns in the N direction. The Bias starting offset is also `blockIdx * tiling.singleCoreN`.
    - Matrix C has shape `[512, 128]`, with each row length being `tiling.N = 128`. The starting offset of matrix C logical block is `blockIdx * tiling.singleCoreN`.
    - Each logical block internally contains `singleCoreM / baseM * singleCoreN / baseN = 4 * 1 = 4` `baseM * baseN` output blocks. Since the current `baseN` equals `singleCoreN`, within a single core, only M-direction base blocks are traversed. The activated results are written back after mapping to the M-direction base block coordinates within the single core according to the current `Iterate` round.
    - Since both the M axis and N axis are divisible by corresponding blocks, the code has no tail block handling logic.

  - Invocation Implementation
    Use the kernel invocation operator `<<<>>>` to call the kernel function. Runtime parameters are passed in order: Device-side matrix A address, matrix B address, Bias address, matrix C address, workspace address, and Host-side generated tiling data.

- SetDim and Kernel Launch Core Count Configuration in `__mix__(1, 2)` Scenario:

  In MIX scenarios (including matrix computation and vector computation), in separation mode (the basic architecture of products supported by this example is separation mode), the Matmul API is initiated from the AIV side. When AIV calls `Iterate`, it notifies AIC to perform matrix computation, and after AIC completes, it notifies AIV. Therefore, the tiling side splits tasks from the AIV perspective.

  In this example, `SetDim(2)` indicates that the tiling side generates Matmul core splitting parameters for 2 AIV logical cores. `__mix__(1, 2)` indicates that each AI Core combination contains 1 AIC and 2 AIVs.
  
  This example is a Kernel direct call example. The Host-side kernel launch `numBlocks = 1` means launching 1 AI Core combination, corresponding to 2 AIV-side logical tasks, which exactly matches the 2 AIV logical cores configured by `SetDim(2)`.

- Interface Parameter Description:

  The following structures pass parameters using curly braces `{}`. The meaning of each field is as follows:

  **`AscendC::DataCopyParams`** — Used by the `DataCopy` interface to describe data movement parameters from Local Memory to Global Memory:
  ```cpp
  struct DataCopyParams {
      uint16_t blockCount;  // Number of consecutive data blocks to be moved, [1, 4095]
      uint16_t blockLen;    // Length of each consecutive data block, in DataBlock (32 bytes), [1, 65535]
      uint16_t srcGap;      // Gap between adjacent consecutive data blocks of source operand, in DataBlock (32 bytes)
      uint16_t dstGap;      // Gap between adjacent consecutive data blocks of destination operand, in DataBlock (32 bytes)
  };
  ```
  This example uses `{tiling.baseM, tiling.baseN * sizeof(float) / AscendC::DEFAULT_C0_SIZE, 0, (tiling.N - tiling.baseN) * sizeof(float) / AscendC::DEFAULT_C0_SIZE}` when moving out matrix C blocks. Here, `blockCount` indicates moving `baseM` rows, `blockLen` indicates the amount of data corresponding to `baseN` matrix C elements per row. `dstGap` indicates the gap (in DataBlock) between adjacent rows on the destination side, used to write `baseM * baseN` blocks back to the corresponding column range of the complete `M * N` matrix.

## Build and Run

Execute the following steps in the root directory of this example to build and run the example.
- Configure Environment Variables
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit package on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. If no installation directory is specified, it defaults to `/usr/local/Ascend`.

- Example Execution

  Execute the following commands in this example directory.
  ```bash
  mkdir -p build && cd build;                                               # Create and enter build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # Build project (default npu mode)
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Execute the compiled executable to run the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify output correctness, confirm algorithm logic is correct
  ```

  When using NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Note:** Before switching build modes, clean the cmake cache. Execute `rm CMakeCache.txt` in the build directory and then run cmake again.

- Build Option Description

  | Option | Available Values | Description |
  |--------|------------------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU run, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

  > **Note:** When `CMAKE_ASC_ARCHITECTURES=dav-3510`, the build project automatically adds `-DENABLE_CV_COMM_VIA_SSBUF=true` for ASC compilation to enable SSBUF communication. The `dav-2201` scenario does not add this compilation macro.

- Execution Result
  The execution result is as follows, indicating successful precision comparison.
  ```bash
  test pass!
  ```
