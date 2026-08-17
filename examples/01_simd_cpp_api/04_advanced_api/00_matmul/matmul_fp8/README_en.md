# Matmul FP8 Direct Invocation Example

## Overview

A Matmul example with A and B matrices as hifloat8, fp8_e4m3fn, or fp8_e5m2 data type input.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```
├── matmul_fp8
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Ground truth verification file
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── matmul_fp8.asc          // Ascend C example implementation & invocation example
│   └── README.md               // Example description document
```

## Example Description

- Example Function:  
  Performs matrix multiplication and bias addition on the input A and B matrices, implementing a Matmul example with hifloat8, fp8_e4m3fn, and fp8_e5m2 data type input. When the input data type is hifloat8, the data types of A and B must be the same.

- Example Specifications:  
  In this example: M = 428, N = 479, K = 158.
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="5" align="center">Matmul</td></tr>
  <tr><td rowspan="4" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">hifloat8, fp8_e4m3fn, fp8_e5m2</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">hifloat8, fp8_e4m3fn, fp8_e5m2</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">matmul_fp8_custom</td></tr>
  </table>
- Example Implementation: 
  - Key Kernel Steps
    - Create the Matmul object, setting the data type to hifloat8 or various fp8 data types based on SCENARIO_NUM.
      ```cpp
      #if SCENARIO_NUM == 1
      MatmulFp8Kernel<fp8_e4m3fn_t, fp8_e4m3fn_t, float, float> MatmulFp8Kernel;
      #elif SCENARIO_NUM == 2
      MatmulFp8Kernel<fp8_e5m2_t, fp8_e5m2_t, float, float> MatmulFp8Kernel;
      #elif SCENARIO_NUM == 3
      MatmulFp8Kernel<fp8_e4m3fn_t, fp8_e5m2_t, float, float> MatmulFp8Kernel;
      #elif SCENARIO_NUM == 4
      MatmulFp8Kernel<fp8_e5m2_t, fp8_e4m3fn_t, float, float> MatmulFp8Kernel;
      #else
      MatmulFp8Kernel<hifloat8_t, hifloat8_t, float, float> MatmulFp8Kernel;
      #endif

      ...

      AscendC::Matmul<AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, aType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, bType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, cType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, biasType>, mmCfg> matmulObj;
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

  - SCENARIO_NUM: Set the data type combination for A and B
    - 0 : Corresponds to A hifloat8   B hifloat8 scenario
    - 1 : Corresponds to A fp8_e4m3fn B fp8_e4m3fn scenario
    - 2 : Corresponds to A fp8_e5m2   B fp8_e5m2 scenario
    - 3 : Corresponds to A fp8_e4m3fn B fp8_e5m2 scenario
    - 4 : Corresponds to A fp8_e5m2   B fp8_e4m3fn scenario

  ```bash
  mkdir -p build && cd build;        # Create and enter the build directory
  cmake -DSCENARIO_NUM=0 -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;      # Build the project, SCENARIO_NUM can be 0/1/2/3/4, for example cmake .. -DSCENARIO_NUM=1;make -j;
  python3 ../scripts/gen_data.py -scenarioNum=0  # Generate test input data, scenarioNum can be 0/1/2/3/4, for example python3 ../scripts/gen_data.py -scenarioNum=1
  ./demo                             # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output is correct and confirm the algorithm logic
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DSCENARIO_NUM=0 -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # CPU debug mode
  cmake -DSCENARIO_NUM=0 -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build Option Description

  | Option | Values | Description |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `SCENARIO_NUM` | `0`, `1`, `2`, `3`, `4` | Scenario number, see example description for details |
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU run, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution Result

  The following output indicates a successful accuracy comparison:
  ```bash
  test pass!
  ```
