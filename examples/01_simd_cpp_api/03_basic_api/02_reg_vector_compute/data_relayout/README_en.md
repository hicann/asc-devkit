# data_relayout Example

## Overview
This example implements data relayout functionality based on the Reg programming interface, supporting multiple scenarios. Select a scenario through the CMake build parameter `SCENARIO_NUM`.
    <table>
  	  	 	<tr>
        <td>SCENARIO_NUM</td>
  	  	 		<td>Relayout Scenario</td>
  	  	 	</tr>
  	  	 	<tr>
  	  	 		<td>1</td>
  	  	 		<td>Interleave (interleave two uint16_t vectors)</td>
  	  	 	</tr>
  	  	 	<tr>
  	  	 		<td>2</td>
  	  	 		<td>Pack (extract lower 16 bits from a uint32_t vector to a uint16_t vector)</td>
  	  	 	</tr>
  	  	 </table>

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure
```
├── data_relayout
│   ├── scripts
│   │   ├── gen_data.py                // Input data and ground truth generation script
│   ├── CMakeLists.txt                 // Build configuration file
│   ├── data_utils.h                   // Data read/write functions
│   ├── data_relayout.asc              // Ascend C example implementation & invocation example
│   └── README.md                      // Example description
```

## Example Description
- Example functionality:
  Demonstrates the usage of data relayout APIs (Interleave/Pack), supporting both Interleave and Pack scenarios.

  **Scenario 1: Interleave Mode**
  - Two uint16_t vectors (128 elements each) are interleaved, outputting two uint16_t vectors
  - Interleave divides VL (128) elements into upper and lower halves (64 each) and interleaves them:
    - dst0 = [src0[0], src1[0], src0[1], src1[1], ..., src0[63], src1[63]]
    - dst1 = [src0[64], src1[64], src0[65], src1[65], ..., src0[127], src1[127]]
  - Example specifications:
    <table>
    <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
    <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">x</td><td align="center">[1, 128]</td><td align="center">uint16_t</td></tr>
    <tr><td align="center">y</td><td align="center">[1, 128]</td><td align="center">uint16_t</td></tr>
    <tr><td rowspan="2" align="center">Example Output</td><td align="center">dst0</td><td align="center">[1, 128]</td><td align="center">uint16_t</td></tr>
    <tr><td align="center">dst1</td><td align="center">[1, 128]</td><td align="center">uint16_t</td></tr>
    <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">data_relayout</td></tr>
    </table>
  - Example implementation:
    The InterleaveVF function calls the Interleave API for data interleaving.
    - Invocation implementation
      Uses the kernel invocation syntax `<<<>>>` to call the kernel function, launching 1 core.

  **Scenario 2: Pack Mode**
  - Extracts the lower 16 bits from 1 uint32_t vector (128 elements) into 1 uint16_t vector
  - Pack<uint16_t, uint32_t, LOWEST>: extracts the lower 16 bits of each uint32_t
  - Example specifications:
    <table>
    <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
    <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">x</td><td align="center">[1, 128]</td><td align="center">uint32_t</td></tr>
    <tr><td rowspan="1" align="center">Example Output</td><td align="center">dst</td><td align="center">[1, 128]</td><td align="center">uint16_t</td></tr>
    <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">data_relayout</td></tr>
    </table>
  - Example implementation:
    The PackVF function calls the Pack API to extract the lower 16 bits.
    - Invocation implementation
      Uses the kernel invocation syntax `<<<>>>` to call the kernel function, launching 1 core.

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
  SCENARIO_NUM=1                                                                 # Select the execution scenario (1=Interleave, 2=Pack)
  mkdir -p build && cd build;                                                    # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;  # Build the project (default npu mode)
  python3 ../scripts/gen_data.py -scenarioNum $SCENARIO_NUM                      # Generate test input data
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
  | `SCENARIO_NUM` | 1, 2 | Example execution scenario: Scenario 1=Interleave, Scenario 2=Pack |
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
