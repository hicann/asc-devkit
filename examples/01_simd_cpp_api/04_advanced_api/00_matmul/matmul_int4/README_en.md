# Matmul int4 Data Type Input Direct Invocation Example

## Overview

A Matmul example with A and B matrices as int4b_t data type input.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── matmul_int4
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Ground truth verification file
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── matmul_int4.asc         // Ascend C example implementation & invocation example
│   └── README.md               // Example description document
```

## Example Description
- Example Function:  
  When the Matmul example calls the Matmul API for computation, it sets the data type parameter of the left matrix A and right matrix B to int4b_t on the Kernel side, and calls the SetAType and SetBType interfaces to set the data type of the left matrix A and right matrix B to DT_INT4 on the Host side, implementing an example with int4 data type input.

- Constraints
  - The data types of the left matrix A and right matrix B must both be int4, and the corresponding output C matrix data type only supports int32 and half.
  - When the left matrix A is of int4 data type, transpose is not supported.
  - Matmul objects with int4 data type input only support Norm, MDL, and IBShare templates.

- Example Specifications:  
  In this example: M = 256, N = 7680, K = 128.
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="5" align="center">Matmul</td></tr>
  <tr><td rowspan="4" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">int4b_t</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">int4b_t</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">int32_t</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">int32_t</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">matmul_int4_custom</td></tr>
  </table>
- Example Implementation: 
  - Key Kernel Steps
    - Create the Matmul object with data type int4b_t.
      ```cpp
      using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AscendC::int4b_t>;
      using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AscendC::int4b_t>;
      using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, int32_t>;
      using BIAS_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, int32_t>;
      AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_MDL> matmulObj;
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
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default) | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products |

- Execution Result

  The following output indicates a successful accuracy comparison:
  ```bash
  test pass!
  ```
