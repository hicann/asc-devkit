# compare Example

## Overview
This example implements data comparison functionality across multiple scenarios using the Compare and Compares APIs based on the Reg programming interface.
This example supports two comparison scenarios. Select a scenario through the CMake build parameter `SCENARIO_NUM`.
  <table>
    <tr>
      <td>SCENARIO_NUM</td>
        <td>Comparison Scenario</td>
    </tr>
    <tr>
      <td>1</td>
      <td>Compare: element-wise comparison between two vectors</td>
    </tr>
    <tr>
      <td>2</td>
      <td>Compares: element-wise comparison between a vector and a scalar</td>
    </tr>
    </table>

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure
```
├── compare
│   ├── scripts
│   │   ├── gen_data.py                // Input data and ground truth generation script
│   ├── CMakeLists.txt                 // Build configuration file
│   ├── data_utils.h                   // Data read/write functions
│   ├── figures                        // Illustrations
│   ├── compare.asc                    // Ascend C example implementation & invocation example
│   └── README.md                      // Example description
```

## Example Description
The Compare API is typically used together with the Select API. This example only demonstrates the combined usage of Compare and Select.
This example switches between different scenarios through the build parameter `SCENARIO_NUM`:
**Scenario 1: Compare**
- Example functionality:
  Takes the element-wise maximum of two vectors xReg and yReg of the same size.
- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">compare</td></tr>
  </table>
- Example implementation:
  - Calls the Compare API in GT (greater than) mode to compare the two vectors. For float data type, the MaskReg format stores one mask per 4 bits, so Compare reads data sequentially from xReg and yReg, and writes comparison results sequentially to the 4 * N bit positions of MaskReg: if xReg is greater than yReg, the corresponding maskReg bit is set to 1, otherwise 0
  - Calls the Select API to select data from xReg or yReg based on the 4 * N bits of MaskReg: if the maskReg bit is 1, the element from xReg is selected for that position; otherwise, the element from yReg is selected
  - Invocation implementation: Uses the kernel invocation syntax <<<>>> to call the kernel function.
  <img src="figures/compare.png">

**Scenario 2: Compares**
- Example functionality:
  Compares each element of vector xReg with scalar 0. If xReg[i] is greater than 0, zReg[i] takes xReg[i]; otherwise, it takes yReg[i].
- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">compare</td></tr>
  </table>
- Example implementation:
  - Calls the Compares API in GT (greater than) mode to compare vector xReg with scalar 0. For float data type, the MaskReg format stores one mask per 4 bits, so Compares reads data from xReg and compares with scalar 0, then writes comparison results sequentially to the 4 * N bit positions of MaskReg: if xReg is greater than 0, the corresponding maskReg bit is set to 1, otherwise 0
  - Calls the Select API to select data from xReg or yReg based on the 4 * N bits of MaskReg: if the maskReg bit is 1, the element from xReg is selected for that position; otherwise, the element from yReg is selected
  - Invocation implementation: Uses the kernel invocation syntax <<<>>> to call the kernel function.

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
  SCENARIO_NUM=1                                                                # Execute scenario 1
  mkdir -p build && cd build;                                                   # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # Build the project (default npu mode)
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM                     # Generate test input data
  ./demo                                                                        # Run the compiled executable to execute the example
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
  | `SCENARIO_NUM` | 1, 2 | Example execution scenario: Scenario 1: Compare, Scenario 2: Compares |
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
