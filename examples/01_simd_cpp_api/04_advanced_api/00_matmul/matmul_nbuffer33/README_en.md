# Matmul NBuffer33 Template Policy Direct Invocation Example

## Overview
A Matmul example using the NBuffer33 algorithm, achieving balanced load-in and load-out bandwidth to improve efficiency.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.2.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.2.0 |

## Directory Structure
```
├── matmul_nbuffer33
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Ground truth verification file
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── matmul_nbuffer33.asc    // Ascend C example implementation & invocation example
│   └── README.md               // Example description document
```
## Example Description
- Example Function:  
  The Matmul example uses the template parameter MatmulPolicy configured as NBuffer33MatmulPolicy. The A matrix for single-core computation is split into 3x3 basic blocks. These 3x3 A matrix basic blocks are fully loaded and maintained in the L1 Buffer, and each time they perform matrix multiplication with 3x1 B matrix basic blocks, while DoubleBuffer transfers the next 3x1 B matrix basic blocks needed for computation in parallel, until the matrix multiplication in the singleCoreN direction is complete.

- Constraints
  - Only the MDL template is supported.
  - Only pure Cube mode (matrix computation only) is supported. MIX mode (including both matrix and vector computation) is not supported.
  - Only the IterateAll interface is supported for obtaining the Matmul computation result C matrix.
  - stepM, stepKa, and stepKb must be less than or equal to 3.
  - The user must ensure that the sum of the fully loaded basic block size of matrix A and the loaded basic block size of matrix B does not exceed the L1 Buffer size.

- Example Specifications:  
  In this example: M = 256, N = 512, K = 192.
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="5" align="center">Matmul</td></tr>
  <tr><td rowspan="4" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">matmul_nbuffer33_custom</td></tr>
  </table>
- Example Implementation: 
  - Key Kernel Steps
    - Specific steps:
      - Create the Matmul object, configure the MDL template and NBuffer33MatmulPolicy.
        ```cpp
        AscendC::Matmul<AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType, IS_TRANS_A>,
                        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType, IS_TRANS_B>,
                        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>,
                        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>, CFG_MDL,
                        AscendC::MatmulCallBackFunc<nullptr, nullptr, nullptr>,
                        AscendC::Impl::Detail::NBuffer33MatmulPolicy>
            matmulObj;
        ```

  - Key Tiling Steps
    - Set the parameter type information for A, B, C, and Bias; M, N, Ka, Kb shape information, and so on. Enable NBuffer33 mode.
      ```cpp
      matmul_tiling::MatmulConfigParams matmulConfigParams(1, false, matmul_tiling::ScheduleType::N_BUFFER_33,
                                                            matmul_tiling::MatrixTraverse::NOSET, false);
      tilingApi.SetMatmulConfigParams(matmulConfigParams);
      ```

  - Invocation Implementation  
    Use the kernel invocation operator <<<>>> to call the kernel function.

## Build and Run
Run the following steps in the root directory of this example to build and run the example.
- Configure Environment Variables  
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;    # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project, npu mode by default
  python3 ../scripts/gen_data.py    # Generate test input data
  ./demo                        # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin    # Verify whether the output is correct and confirm the algorithm logic
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build Option Description

  | Option | Values | Description |
  | ----------------| -----------------------------| ---------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU run, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution Result

  The following output indicates a successful accuracy comparison:
  ```bash
  test pass!
  ```
