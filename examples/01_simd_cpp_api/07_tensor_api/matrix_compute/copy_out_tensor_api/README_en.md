# Tensor API copy-out Example


## Overview

This example implements dynamic Shape matrix multiplication with Bias based on the Tensor API programming approach, demonstrating the usage of copy-out APIs (L0C to GM, L0C to UB) and mmad (matrix multiply-add) Tensor APIs. Select different write-back modes through the build-time parameter `SCENARIO_NUM`. Shape parameters are dynamically passed at runtime through the `MatmulTiling` structure.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | > CANN 9.1.0 |

> **Note:** This example depends on CANN features that have not been officially released. Use the latest CANN master package.

## Directory Structure

```text
├── copy_out_tensor_api
│   ├── scripts
│   │   ├── gen_data.py                    // Input data and ground truth generation script
│   │   └── verify_result.py               // Ground truth comparison file
│   ├── CMakeLists.txt                     // Build configuration file
│   ├── data_utils.h                       // Data read/write functions
│   ├── copy_out_tensor_api.asc            // Ascend C example implementation & invocation example
│   └── README.md                          // Example description document
```

## Example Description

- Example functionality:
  This example implements dynamic Shape multi-core Matmul with Bias, performing L0C-to-GM or L0C-to-UB data transfer through the copy API, and matrix multiply-add computation through the mmad API.

  1. Dynamic Matmul Functionality

  This example implements multi-core Matmul functionality. Shape parameters (M, N, K, singleCoreM, singleCoreN, singleCoreK) are dynamically passed to the kernel at runtime through the `MatmulTiling` structure. `BASE_M`, `BASE_N`, `BASE_K` and `STEP_M`, `STEP_N`, `STEP_K` serve as compile-time template parameters, determining the allocation size of L0/L1 level buffers.

  2. Bias Functionality

  This example adds the Bias vector to the matrix multiplication result through the mmad API in the first iteration of Matmul computation. Bias is a float-type one-dimensional vector with length N.

  3. Multi-scenario Support

  Select different write-back modes through the build-time macro parameter `SCENARIO_NUM`:

  | SCENARIO_NUM | Write-back Mode | Write-back Path | Description |
  | :--- | :--- | :--- | :--- |
  | 0 | Normal mode | L0C -> GM | Uses `copy_l0c_to_gm` to write computation results directly back to GM |
  | 1 | unitFlag mode | L0C -> GM | Explicitly enables `unitFlag` in `copy_l0c_to_gm` |
  | 2 | UB transfer mode M split | L0C -> UB -> GM | `copy_l0c_to_ub` uses `DUAL_DST_SPLIT_M`, AIV writes back split by M dimension |
  | 3 | UB transfer mode N split | L0C -> UB -> GM | `copy_l0c_to_ub` uses `DUAL_DST_SPLIT_N`, AIV writes back split by N dimension |

  - Normal mode (SCENARIO_NUM=0/1): L0C results are transferred directly to GM. Scenario 1 additionally enables `unitFlag`
  - UB transfer mode (SCENARIO_NUM=2/3): L0C results are first transferred to UB, then written back to GM by the AIV side

  Note: A/B matrix data types are half in all scenarios. C matrix and Bias data types are float in all scenarios.

  4. MatmulTiling Structure Definition

  ```cpp
  struct MatmulTiling {
      int32_t m;            // Number of rows of matrix A
      int32_t n;            // Number of columns of matrix B
      int32_t k;            // Number of columns of matrix A / number of rows of matrix B
      int32_t singleCoreM;  // M dimension size processed per core
      int32_t singleCoreN;  // N dimension size processed per core
      int32_t singleCoreK;  // K dimension size processed per core
  };
  ```

- Example specifications:
  In the kernel function direct invocation example, the default supported shape is: M = 1024, N = 1024, K = 256.

  **Scenario 0: Normal mode, L0C -> GM**

  <table border="2">
  <caption>Table 1: Scenario 0 Specifications</caption>
  <tr><td rowspan="1" align="left">Example Type (OpType)</td><td colspan="4" align="left">Matmul with Bias</td></tr>
  <tr><td rowspan="5" align="left">Example Input</td><td align="left">name</td><td align="left">shape</td><td align="left">data type</td><td align="left">format</td></tr>
  <tr><td align="left">x (Matrix A)</td><td align="left">[M, K]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td align="left">y (Matrix B)</td><td align="left">[K, N]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td align="left">bias</td><td align="left">[N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td align="left">tiling</td><td align="left">MatmulTiling structure</td><td align="left">int32_t</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">Example Output</td><td align="left">z (Matrix C)</td><td align="left">[M, N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">Kernel Function Name</td><td colspan="4" align="left">copy_out_tensor_api</td></tr>
  </table>

  **Scenario 1: unitFlag mode, L0C -> GM**

  <table border="2">
  <caption>Table 2: Scenario 1 Specifications</caption>
  <tr><td rowspan="1" align="left">Example Type (OpType)</td><td colspan="4" align="left">Matmul with Bias</td></tr>
  <tr><td rowspan="5" align="left">Example Input</td><td align="left">name</td><td align="left">shape</td><td align="left">data type</td><td align="left">format</td></tr>
  <tr><td align="left">x (Matrix A)</td><td align="left">[M, K]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td align="left">y (Matrix B)</td><td align="left">[K, N]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td align="left">bias</td><td align="left">[N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td align="left">tiling</td><td align="left">MatmulTiling structure</td><td align="left">int32_t</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">Example Output</td><td align="left">z (Matrix C)</td><td align="left">[M, N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">Kernel Function Name</td><td colspan="4" align="left">copy_out_tensor_api</td></tr>
  </table>

  **Scenario 2: UB transfer mode M split, L0C -> UB -> GM**

  <table border="2">
  <caption>Table 3: Scenario 2 Specifications</caption>
  <tr><td rowspan="1" align="left">Example Type (OpType)</td><td colspan="4" align="left">Matmul with Bias</td></tr>
  <tr><td rowspan="5" align="left">Example Input</td><td align="left">name</td><td align="left">shape</td><td align="left">data type</td><td align="left">format</td></tr>
  <tr><td align="left">x (Matrix A)</td><td align="left">[M, K]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td align="left">y (Matrix B)</td><td align="left">[K, N]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td align="left">bias</td><td align="left">[N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td align="left">tiling</td><td align="left">MatmulTiling structure</td><td align="left">int32_t</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">Example Output</td><td align="left">z (Matrix C)</td><td align="left">[M, N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">Kernel Function Name</td><td colspan="4" align="left">copy_out_tensor_api</td></tr>
  </table>

  **Scenario 3: UB transfer mode N split, L0C -> UB -> GM**

  <table border="2">
  <caption>Table 4: Scenario 3 Specifications</caption>
  <tr><td rowspan="1" align="left">Example Type (OpType)</td><td colspan="4" align="left">Matmul with Bias</td></tr>
  <tr><td rowspan="5" align="left">Example Input</td><td align="left">name</td><td align="left">shape</td><td align="left">data type</td><td align="left">format</td></tr>
  <tr><td align="left">x (Matrix A)</td><td align="left">[M, K]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td align="left">y (Matrix B)</td><td align="left">[K, N]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td align="left">bias</td><td align="left">[N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td align="left">tiling</td><td align="left">MatmulTiling structure</td><td align="left">int32_t</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">Example Output</td><td align="left">z (Matrix C)</td><td align="left">[M, N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">Kernel Function Name</td><td colspan="4" align="left">copy_out_tensor_api</td></tr>
  </table>

  Compile-time template parameter default values:

  | Parameter | Default | Description |
  | :------ | :----- | :----------------------- |
  | BASE_M  | 128    | M dimension base block size |
  | BASE_N  | 128    | N dimension base block size |
  | BASE_K  | 64     | K dimension base block size |
  | STEP_M  | 1      | M dimension L1 cache step |
  | STEP_N  | 1      | N dimension L1 cache step |
  | STEP_K  | 4      | K dimension L1 cache step |

  Runtime dynamic parameter default values:

  | Parameter | Default | Description |
  | :----------- | :----- | :--------------------- |
  | m            | 1024   | Number of rows of matrix A |
  | n            | 1024   | Number of columns of matrix B |
  | k            | 256    | Number of columns of matrix A / number of rows of matrix B |
  | singleCoreM  | 256    | Per-core M dimension size |
  | singleCoreN  | 128    | Per-core N dimension size |
  | singleCoreK  | 256    | Per-core K dimension size |

## Build and Run

Run the following steps in the root directory of this example to build and run it.

- Configure environment variables
  Configure environment variables based on the [installation method](https://gitcode.com/cann/asc-devkit/blob/master/docs/en/quick_start.md) of the CANN development kit on the current environment. **Currently only [CANN master](https://gitcode.com/cann/asc-devkit/blob/master/docs/en/quick_start.md) is supported.**
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example
  Taking scenario 0 (normal mode, L0C -> GM) as an example:

  ```bash
  SCENARIO_NUM=0
  mkdir -p build && cd build;                                                    # Create and enter the build directory
  cmake -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # Build the project, default NPU mode
  python3 ../scripts/gen_data.py;                                                # Generate test input data
  ./demo;                                                                       # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin        # Verify whether the output result is correct, confirming algorithm logic correctness
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter. Example:

  ```bash
  cmake -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and re-run cmake.

  When building other scenarios, modify the `SCENARIO_NUM` variable. For example, to build scenario 2 (UB transfer mode M split), set `SCENARIO_NUM=2`.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The following execution result indicates that the precision comparison is successful.

  ```bash
  test pass!
  ```
