# cast Example

## Overview
This example implements the Cast operation based on the Reg programming interface, primarily calling the Cast API for data type conversion.
This example supports two data type conversion scenarios. Select a scenario through the CMake build parameter `SCENARIO_NUM`.
  <table>
    <tr>
      <td>SCENARIO_NUM</td>
        <td>Data Type Conversion</td>
    </tr>
    <tr>
      <td>1</td>
      <td>Narrow to wide data type, using half to int32_t as an example</td>
    </tr>
    <tr>
      <td>2</td>
      <td>Wide to narrow data type, using float to int16_t as an example</td>
    </tr>
    </table>

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure
```
├── cast
│   ├── scripts
│   │   ├── gen_data.py                // Input data and ground truth generation script
│   ├── CMakeLists.txt                 // Build configuration file
│   ├── data_utils.h                   // Data read/write functions
│   ├── figures                        // Illustrations
│   ├── cast.asc                       // Ascend C example implementation & invocation example
│   └── README.md                      // Example description
```

## Example Description
This example performs data type conversion on the input vector. When the input and output data types have different bit widths, the Cast API reads or writes in an interleaved manner. Therefore, this example uses corresponding compression or decompression modes during data transfer. The usage of LoadAlign/StoreAlign APIs is for reference only. The specific description is as follows:

**Scenario 1: Narrow to Wide Data Type Conversion**
- Example functionality: Converts half-type data to int32_t-type data.
- Parameter description:
  - layoutMode = RegLayout::ZERO: The Cast API reads data from xReg at index 2\*N+0, used with the LoadAlign API to load input data sequentially into the 2\*N positions of xReg
  - satMode = SatMode::NO_SAT: This scenario demonstrates the non-saturation mode for floating-point to integer conversion. When the input data exceeds the output data type range, the result is truncated to the output data type width. For example, if the input half value is 4294967297.0, corresponding to integer 4294967297 (0x100000001), the lower 32 bits are taken, and the output int32_t value is 1
  - roundMode = RoundMode::CAST_FLOOR: This scenario demonstrates the floor (round toward negative infinity) rounding mode. For example, if the input half value is 2.5, the output int32 value is 2
  - mask: In the Cast API, the mask filters based on the wider data type between input and output, so this scenario generates MaskReg based on the int32_t data type
- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">half</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">y</td><td align="center">[1, 256]</td><td align="center">int32_t</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">cast</td></tr>
  </table>
- Example implementation:
  The Cast API from half to int32_t processes 64 data elements each time. The specific flow and diagram are as follows:
  - Load: Call the LoadAlign API using decompression mode to load data into the 2\*N positions, with 2\*N+1 positions set to 0
  - Compute: Call the Cast API. The input-to-output bit width ratio is 1:2, so data is read from the 2\*N positions of xReg, type-converted, and sequentially written to yReg
  - Store: Call the StoreAlign API for standard store
  - Invocation implementation: Uses the kernel invocation syntax <<<>>> to call the kernel function.
  <img src="figures/reg_cast_1.png">

**Scenario 2: Wide to Narrow Data Type Conversion**
- Example functionality: Converts float-type data to int16_t-type data.
- Parameter description:
  - layoutMode = RegLayout::ZERO: The Cast API writes data to yReg at index 2\*N+0, used with the StoreAlign API to store data from the 2\*N positions of yReg
  - satMode = SatMode::SAT: This scenario demonstrates the saturation mode for floating-point to integer conversion. When the input data exceeds the output data type range, the result is the corresponding extreme value of the output type. For example, if the input float value is 32768.0, corresponding to integer 32768, the maximum value of int16_t is taken, and the output int16_t value is 32767
  - roundMode = RoundMode::CAST_ROUND: This scenario demonstrates the round (round to nearest integer) rounding mode. For example, if the input half value is 2.5, the output int32 value is 3
  - mask: In the Cast API, the mask filters based on the wider data type between input and output, so this scenario generates MaskReg based on the float data type
- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">y</td><td align="center">[1, 256]</td><td align="center">int16_t</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">cast</td></tr>
  </table>
- Example implementation:
  The Cast API from float to int16_t processes 64 data elements each time. The specific flow and diagram are as follows:
  - Load: Call the LoadAlign API for standard load
  - Compute: Call the Cast API. The input-to-output bit width ratio is 2:1, so data is read sequentially from xReg, type-converted, and sequentially written to the 2\*N positions of yReg, with 2\*N+1 positions set to 0
  - Store: Call the StoreAlign API using compression mode to store only data at the 2\*N positions
  - Invocation implementation: Uses the kernel invocation syntax <<<>>> to call the kernel function.
  <img src="figures/reg_cast_2.png">

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
  SCENARIO_NUM=1                                                                 # Execute scenario 1
  mkdir -p build && cd build;                                                    # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;  # Build the project (default npu mode)
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM                      # Generate test input data
  ./demo                                                                         # Run the compiled executable to execute the example
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # CPU debug mode
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `SCENARIO_NUM` | 1, 2 | Example execution scenario: Scenario 1: narrow to wide data type, Scenario 2: wide to narrow data type |
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result
  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
