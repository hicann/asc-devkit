# Cast Example

## Overview

This example implements data type and precision conversion between source and destination operand Tensors based on Cast. This example implements conversion in two scenarios: half to int32_t and half to int4b_t.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```text
├── cast
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── cast.asc                // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  The CastCustom example performs precision conversion based on the data types of the source and destination operand Tensors.
- Example specifications:  
  <table>
  <caption>Table 1: Example Specifications</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> cast </td></tr>

  <tr><td rowspan="3" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 512]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 512]</td><td align="center">int32_t/int4b_t</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">cast_custom</td></tr>
  </table>

- Scenario description:  
  <table>
  <caption>Table 2: SCENARIO_NUM Parameter Description</caption>
  <tr><td align="center">SCENARIO_NUM</td><td align="center">Input Type</td><td align="center">Output Type</td><td align="center">Description</td></tr>
  <tr><td align="center">0</td><td align="center">half</td><td align="center">int4b_t</td><td align="center">half to int4b_t</td></tr>
  <tr><td align="center">1</td><td align="center">half</td><td align="center">int32_t</td><td align="center">half to int32_t</td></tr>
  </table>

- Example implementation:  
  This example implements a CastCustom example with fixed input shape x[1, 512] and output shape y[1, 512], supporting two conversion scenarios: half to int32_t and half to int4b_t.

  - Kernel implementation  
    - Call the DataCopy basic API to transfer data from GM (Global Memory) to UB (Unified Buffer)
    - Call the Cast interface to perform data type conversion (half to int32_t or half to int4b_t)
    - Call the DataCopy basic API to transfer results from UB (Unified Buffer) to GM (Global Memory)

  - Invocation implementation  
    Use the kernel invocation operator <<<>>> to call the kernel function.

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
  SCENARIO_NUM=0
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM  # Generate test input data
  ./demo                        # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  
  Examples:

  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `0` (default), `1` | Scenario: 0 corresponds to half to int4b_t, 1 corresponds to half to int32_t |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
