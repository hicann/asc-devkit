# Matmul with VECOUT Input Direct Invocation Example

## Overview
A Matmul example that uses user-defined VECOUT input, allowing developers to manage the Unified Buffer independently for efficient hardware resource utilization.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure
```
├── matmul_vecout
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Ground truth verification file
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── matmul_vecout.asc       // Ascend C example implementation & invocation example
│   └── README.md               // Example description document
```

## Example Description
- Example Function:  
  The Matmul example calls the Matmul API to perform matrix multiplication and bias addition on the input A and B matrices. The input position of matrix A is VECOUT.

- Example Specifications:  
  In this example: M = 31, N = 31, K = 31.
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">Matmul</td></tr>
  <tr><td rowspan="4" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">matmul_vecout_custom</td></tr>
  </table>

- Example Implementation: 
  - Key Kernel Steps  
    - Create the Matmul object. In the MatmulType of the left matrix A, POSITION is VECOUT.
      ```cpp
      AscendC::MatmulType<AscendC::TPosition::VECOUT, CubeFormat::ND, AType>
      ```
    - Customize the transfer of the left matrix A from GM to VECOUT, and set the left matrix A as VECOUT input.
      ```cpp
      AscendC::LocalTensor<AType> vecinTensor = vecin.AllocTensor<AType>();
      // Parameters for transferring matrix A
      DataCopyPad(vecinTensor, aGlobal, {blockCount, blockLen, srcStride, dstStride, 0}, {false, 0, 0, 0});
      vecin.EnQue(vecinTensor);
      AscendC::LocalTensor<AType> vecinLocal = vecin.DeQue<AType>();

      AscendC::LocalTensor<AType> vecoutTensor = vecout.AllocTensor<AType>();
      DataCopy(vecoutTensor, vecinLocal, singleSize); // Copy the entire size for convenience
      vecout.EnQue(vecoutTensor);
      AscendC::LocalTensor<AType> vecoutLocal = vecout.DeQue<AType>();
      vecin.FreeTensor(vecinLocal);

      matmulObj.SetTensorA(vecoutLocal, isTransA);
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
  mkdir -p build && cd build;   # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;             # Build the project, npu mode by default
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                        # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output is correct and confirm the algorithm logic
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
