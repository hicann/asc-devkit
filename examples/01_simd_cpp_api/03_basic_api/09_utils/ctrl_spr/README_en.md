# CtrlSpr Example

## Overview

This example implements setting, reading, and resetting specific bits of the CTRL register (control register) based on the `SetCtrlSpr`, `GetCtrlSpr`, and `ResetCtrlSpr` interfaces, and verifies the non-saturation mode through floating-point computation.

This example demonstrates setting, reading, and resetting CTRL[48] and CTRL[60] bits, verifying whether INF retains its original value in non-saturation mode, and verifying by reading the value of CTRL[48] after setting and after reset through GetCtrlSpr:
- CTRL[60] controls the global effectiveness of saturation mode. When set to 1, global saturation is enabled.
- CTRL[48] controls the saturation mode during floating-point computation and floating-point precision conversion, and only takes effect when CTRL[60] is enabled.
  - When set to 0 (saturation mode): INF output is saturated to +/-MAX (65504 for half type), NaN output is saturated to 0.
  - When set to 1 (non-saturation mode): INF/NaN retain their original output.
- After using the register, use the ResetCtrlSpr interface to reset the register to default values to prevent subsequent computations from being affected.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```
├── ctrl_spr
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── ctrl_spr.asc            // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  Verify the complete flow and functional effects of CTRL register non-saturation mode operations:
  1. SetCtrlSpr: Set `CTRL[60]=1` to enable global effectiveness, set `CTRL[48]=1` to select non-saturation mode (INF/NaN retain original output)
  2. GetCtrlSpr: Read the value of `CTRL[48]` after setting, store in the first 4 elements of `ctrlLocal`
  3. Adds: Perform half-type floating-point addition computation with input containing INF values, store in `dstLocal`, used to verify whether INF retains its original value in non-saturation mode
  4. ResetCtrlSpr: Reset `CTRL[48]` and `CTRL[60]` to default values
  5. GetCtrlSpr: Read the value of `CTRL[48]` after reset, store in the last 4 elements of `ctrlLocal`

- Example specifications:
  <table>
  <caption>Table 1: Example Specifications</caption>
  <tr><td rowspan="1" align="center">Number of Cores</td><td colspan="4" align="center">1</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src</td><td align="center">[1, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td><td align="center">output_ctrl</td><td align="center">[1, 8]</td><td align="center">int64_t</td><td align="center">ND</td></tr>
  <tr><td align="center">output_sat</td><td align="center">[1, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">kernel_ctrl_spr</td></tr>  
  </table>

- Output description:
  - output_ctrl.bin: Register value verification output, 8 int64_t values (first 4 are `CTRL[48]` value after setting = 1, last 4 are `CTRL[48]` value after reset = 0)
  - output_sat.bin: Non-saturation mode function verification output, first 128 are normal values + 1, last 128 are INF (INF retained in non-saturation mode)

- Verification logic:
  - Input data: first 128 are normal values (0~127), last 128 are INF
  - Set `CTRL[48]=1` (non-saturation mode): INF + 1 = INF (INF retained)
  - `CTRL[48]` value after setting = 1, value after reset = 0


## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables  
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;                                          # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                 # Build the project, default npu mode
  python3 ../scripts/gen_data.py                                       # Generate test input data and golden data
  ./demo                                                               # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py                                  # Verify whether the output result is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  
  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;   # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;   # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` (default) | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

  > **Notice:** This example is only supported on Ascend 950PR/Ascend 950DT.

- Execution result

  The following execution result indicates that the accuracy comparison is successful:
  ```bash
  test pass!
  ```
