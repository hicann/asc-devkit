# Matmul TSCM Input from VECOUT Direct Invocation Example

## Overview
A Matmul example that uses user-defined TSCM input with data sourced from VECOUT, allowing developers to manage the L1 Buffer independently for efficient hardware resource utilization. TSCM stands for Temp Swap Cache Memory, used for temporarily swapping data to additional space. In this scenario, the developer manages the L1 Buffer and uses the L1 Buffer address corresponding to the input matrix data as the Matmul input.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure
```
├── matmul_tscm_src_vecout
│   ├── scripts
│   │   ├── gen_data.py                         // Script for generating input data and ground truth data
│   │   └── verify_result.py                    // Ground truth verification file
│   ├── CMakeLists.txt                          // Build project file
│   ├── data_utils.h                            // Data read/write functions
│   ├── matmul_tscm_src_vecout.asc              // Ascend C example implementation & invocation example
│   └── README.md                               // Example description document
```
## Example Description
- Example Function:  
  The Matmul example customizes the data transfer of matrix A from VECOUT to L1, keeping all data of matrix A resident in L1. When calling the Matmul API for computation, matrix A is set as TSCM input and matrix B is set as GM input, performing matrix multiplication and bias addition on the input A and B matrices.

- Constraints
  - The TSCM input matrix must be fully loaded on the L1 Buffer.

- Example Specifications:  
  In this example: M = 32, N = 256, K = 32.
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="5" align="center">Matmul</td></tr>
  <tr><td rowspan="4" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">matmul_tscm_src_vecout_custom</td></tr>
  </table>

- Example Implementation: 
  - Key Kernel Steps
    - Create the Matmul object. In the MatmulType of the left matrix A, POSITION is TSCM and SRC_POSITION is VECOUT.
      ```cpp
      AscendC::MatmulType<AscendC::TPosition::TSCM, CubeFormat::NZ, AType, 
                          IS_TRANS_A, LayoutMode::NONE, false, AscendC::TPosition::VECOUT>
      ```
    - Customize the transfer of the left matrix A from VECOUT to TSCM, set the left matrix A, right matrix B, and Bias, where the left matrix A is TSCM input.
      ```cpp
      // Copy aMatrix from vecout to tscm
      AscendC::TSCM<AscendC::TPosition::VECOUT, 1> scm;
      pipe->InitBuffer(scm, 1, tiling.M * tiling.Ka * sizeof(AType));
      auto scmTensor = scm.AllocTensor<AType>();
      DataCopy(scmTensor, vecoutLocal, tiling.M * tiling.Ka);
      scm.EnQue(scmTensor);
      AscendC::LocalTensor<AType> scmLocal = scm.DeQue<AType>();

      matmulObj.SetTensorA(scmLocal, isTransA); // Set aMatrix tscm input
      matmulObj.SetTensorB(bGlobal, isTransB);
      if (tiling.isBias) {
          matmulObj.SetBias(biasGlobal);
      }
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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # Build the project, npu mode by default
  python3 ../scripts/gen_data.py    # Generate test input data
  ./demo                        # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin    # Verify whether the output is correct and confirm the algorithm logic
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build Option Description

  | Option | Values | Description |
  | ----------------| -----------------------------| ---------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU run, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution Result

  The following output indicates a successful accuracy comparison.

  ```bash
  test pass!
  ```
