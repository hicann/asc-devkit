# High-Level API Basic Matmul Example
## Overview
A Matmul example that calls the Matmul API to implement matrix multiplication. It performs matrix multiplication and bias addition on the input A and B matrices, with the computation formula: C = A * B + Bias.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure
```
├── matmul
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Ground truth verification file
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── matmul.asc              // Ascend C example implementation & invocation example
│   └── README.md               // Example description
```
## Example Description
- Example Function:  
  The Matmul example calls the Matmul API to perform matrix multiplication and bias addition on the input A and B matrices, with the computation formula: C = A * B + Bias.

- Example Specifications:  
  In this example: M = 128, N = 2048, K = 1024.
<table>
<tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="5" align="center">MatmulCustom</td></tr>
<tr><td rowspan="4" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
<tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
<tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
<tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">matmul_custom</td></tr>
</table>

- Example Implementation: 
  - Key Kernel Steps
    - The specific steps are as follows:
      - Create the Matmul object.
      - Initialize.
      - Set the left matrix A, right matrix B, and bias matrix Bias.
      - Complete the matrix multiplication operation.
      - End the matrix multiplication operation.

  - Key Tiling Steps
      - Ascend C provides a set of Matmul Tiling APIs to help users obtain the Tiling parameters needed for Matmul kernel computation. Simply pass in the A/B/C matrix information and call the API interfaces to obtain the relevant parameters in the TCubeTiling structure.
      - The process for obtaining Tiling parameters is as follows:
        - Create a Tiling object.
        - Set the parameter type information for A, B, C, and Bias; M, N, Ka, Kb shape information, and so on.
        - Call the GetTiling interface to obtain Tiling information.

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

  The following output indicates a successful accuracy comparison.

  ```bash
  test pass!
  ```
