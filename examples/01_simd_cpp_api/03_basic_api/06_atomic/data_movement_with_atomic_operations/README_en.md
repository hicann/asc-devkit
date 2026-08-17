# DataCopy with Atomic Operations Example

## Overview

This example introduces the implementation flow of atomic accumulation and atomic maximum comparison when transferring UB local tensors to GM, based on the `SetAtomicAdd` and `SetAtomicMax` atomic operation APIs. Note that after calling atomic operation APIs, you must call `DisableDmaAtomic()` to disable atomic mode to prevent subsequent computations from being affected.

> **API tip:** In addition to the `SetAtomicAdd` and `SetAtomicMax` APIs used in this example, Ascend C also provides the `SetAtomicMin` API for configuring VECOUT-to-GM transfer rules. The calling method of `SetAtomicMin` is the same as `SetAtomicMax`; simply replace the function name to switch.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── data_movement_with_atomic_operations
│   ├── scripts
│   │   ├── gen_data.py                           // Input data and ground truth generation script
│   │   └── verify_result.py                      // Verification script to check output data against ground truth
│   ├── CMakeLists.txt                            // Build configuration file
│   ├── data_utils.h                              // Data read/write functions
│   ├── data_movement_with_atomic_operations.asc  // Ascend C example implementation & invocation example
│   └── README.md                                 // Example description document
```

## Detailed Scenario Description

This example switches between different scenarios through the build parameter `SCENARIO_NUM`:

<table border="2">
<caption>Table 1: Scenario Configuration Comparison</caption>
<tr><th>scenarioNum</th><th>Atomic Operation API</th><th>Input Shape</th><th>Output Shape</th><th>Data Type</th><th>Description</th></tr>
<tr><td>1</td><td>SetAtomicAdd</td><td>[1, 256] (three cores read the same)</td><td>[1, 256]</td><td>half</td><td>Three cores read the same input, atomic accumulation operation</td></tr>
<tr><td>2</td><td>SetAtomicMax</td><td>[1, 256]x3 (three cores read different)</td><td>[1, 256]</td><td>half</td><td>Three cores read different inputs, atomic maximum comparison operation</td></tr>
</table>

**Scenario 1: SetAtomicAdd Atomic Accumulation (Three Cores Read the Same Input)**
- Input shape: src=[1, 256] (three cores simultaneously read input_x.bin), dst=[1, 256] (input_y.bin represents existing data on dst)
- Output shape: dst=[1, 256]
- Data type: half
- Description: Three cores simultaneously read the same input data (input_x.bin). Through `SetAtomicAdd`, atomic accumulation mode is enabled, accumulating each core's data into the shared output buffer. The result is input_y + input_x*3


**Scenario 2: SetAtomicMax Atomic Maximum Comparison (Three Cores Read Different Inputs)**
- Input shape: src0=[1, 256], src1=[1, 256], src2=[1, 256] (three cores read input_x0.bin, input_x1.bin, input_x2.bin respectively)
- Output shape: dst=[1, 256]
- Data type: half
- Description: Three cores obtain their core IDs via `GetBlockIdx()` and read different input data (input_x0.bin, input_x1.bin, input_x2.bin) respectively. input_y.bin is first copied to the output buffer as the existing dst initial value on GM. After enabling atomic maximum comparison mode through `SetAtomicMax`, each output position is the maximum among the three inputs and the dst initial value.

## Example Description

- Example specifications:
  <table>
  <caption>Table 2: Example Specification Description</caption>
  <tr><td rowspan="5" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td rowspan="1" align="center">input_x / input_x0</td><td align="center">[1, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">input_x1</td><td align="center">[1, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">input_x2</td><td align="center">[1, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">input_y</td><td align="center">[1, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">dst</td><td align="center">[1, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">data_movement_with_atomic_operations_custom</td></tr>
  <tr><td rowspan="1" align="center">Parallel Block Count</td><td colspan="4" align="center">3</td></tr>
  </table>

## Build and Run

Run the following steps in the root directory of this example to build and run it.

- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO_NUM=1  # Set the scenario number (values: 1, 2)
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # Build the project
  python3 ../scripts/gen_data.py -scenarioNum $SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin  # Verify whether the output result is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # CPU debug mode
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1` (default), `2` | Scenario number: 1 (SetAtomicAdd atomic accumulation), 2 (SetAtomicMax atomic maximum comparison) |

- Execution result

  The following execution result indicates that the precision comparison is successful:

  ```bash
  test pass!
  ```
