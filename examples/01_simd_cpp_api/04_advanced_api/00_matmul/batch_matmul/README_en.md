# Batch Matmul Direct Invocation Example

## Overview

An example for batch processing Matmul computations.

By transferring multiple Matmul input data in a single transfer, the number of transfers is reduced, improving performance. This is applicable when multiple Matmul computations are needed and the input shape of a single Matmul is small, where transfer overhead accounts for a large proportion of the total time.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── batch_matmul
│   ├── scripts
│   │   ├── gen_data.py              // Script for generating input data and ground truth data
│   │   └── verify_result.py         // Ground truth verification file
│   ├── CMakeLists.txt               // Build project file
│   ├── data_utils.h                 // Data read/write functions
│   ├── batch_matmul.asc             // Ascend C example implementation & invocation example
│   └── README.md                    // Example description document
```
## Example Description

- Example Function:  
  Call the Matmul high-level API to batch process 3 sets of Matmul computations, performing matrix multiplication and adding bias on A and B matrices in BSNGD format for each set.

  For details about the BSNGD format, refer to the data layout description in [IterateBatch](../../../../../docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/IterateBatch.md).

- Example Specifications:   
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="6" align="center">BatchMatmulCustom</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td><td align="center">layout</td></tr>
  <tr><td align="center">a</td><td align="center">[2, 32, 1, 3, 64]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td><td align="center">BSNGD</td></tr>
  <tr><td align="center">b</td><td align="center">[2, 256, 1, 3, 64]</td><td align="center">half</td><td align="center">ND</td><td align="center">true</td><td align="center">BSNGD</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[2, 32, 1, 3, 256]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td><td align="center">BSNGD</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="6" align="center">batch_matmul_custom</td></tr>
  </table>

- Example Implementation:  
  - Key Kernel Steps
    - Complete the multi-batch matrix multiplication operation.
      ```cpp
      matmulObj.IterateBatch(cGlobal[batchOffsetC], batchA, batchB, false);
      ```

  - Key Tiling Steps
    - Call SetALayout, SetBLayout, SetCLayout, and SetBatchNum to set the Layout axis information and maximum BatchNum for A/B/C.
      ```cpp
      cubeTiling->SetALayout(A_BNUM, A_SNUM, 1, A_GNUM, A_DNUM);
      cubeTiling->SetBLayout(B_BNUM, B_SNUM, 1, B_GNUM, B_DNUM);
      cubeTiling->SetCLayout(C_BNUM, C_SNUM, 1, C_GNUM, C_DNUM);
      cubeTiling->SetBatchNum(BATCH_NUM);
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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;             # Build the project, npu mode by default
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                        # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output is correct and confirm the algorithm logic
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

  The following output indicates a successful accuracy comparison.

  ```bash
  test pass!
  ```
