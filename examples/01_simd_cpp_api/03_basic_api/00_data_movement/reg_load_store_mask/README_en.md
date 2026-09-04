# ld_st_reg_mask Example

## Overview
This example implements UB (Unified Buffer) load/store operations for MaskReg (mask register) based on the Reg programming interface, as well as masked store operations using mask. It supports multiple scenarios. Select a scenario through the CMake build parameter `SCENARIO_NUM`.
    <table>
	 	<tr>
        <td>SCENARIO_NUM</td>
	 		<td>Scenario Type</td>
	 	</tr>
	 	<tr>
	 		<td>1</td>
	 		<td>Basic transfer scenario: Uses LoadAlign, StoreAlign and other APIs to implement MaskReg load/store</td>
	 	</tr>
	 	<tr>
	 		<td>2</td>
	 		<td>Composite computation scenario: Uses LoadAlign to load MaskReg, then uses it as a mask for Select to complete selection computation</td>
	 	</tr>
	 </table>

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure
```
├── reg_load_store_mask
│   ├── scripts
│   │   └── gen_data.py                // Input data and ground truth generation script
│   ├── figures                        // Illustrations
│   ├── CMakeLists.txt                 // Build configuration file
│   ├── data_utils.h                   // Data read/write functions
│   ├── README.md                      // Example description
│   ├── ld_st_reg_mask_scenario_1.asc  // Scenario 1: Ascend C implementation for basic transfer scenario
│   └── ld_st_reg_mask_scenario_2.asc  // Scenario 2: Ascend C implementation for composite computation scenario
```

## Example Description

### Scenario 1: Basic Transfer Scenario
- Example functionality:
  - Data load: Takes the first 256 bits of data from the input matrix and calls the LoadAlign API to load data from UB into MaskReg.
  - Data store: Sets all bits of MaskReg to 1 and calls the StoreAlign API to store data from MaskReg to UB.
- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">uint8_t</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">y</td><td align="center">[1, 1024]</td><td align="center">uint8_t</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">ld_st_reg_mask_scenario_1</td></tr>
  </table>
- Example implementation:
<p align="center">
  <img src="figures/ld_st_reg_mask.png" width="1000">
</p>

  1. In the CopyVF function, call the LoadAlign API to load 256 bits (32*uint8_t) of data from UB into MaskReg to achieve dynamic mask setting. In this example, the 32 uint8_t values are set to 1,0,...,1, meaning the first and last values are 1 (b'00000001). Since the chip reads numbers from the low bit each time, these 32 numbers are ultimately filled into MaskReg as b'1000...1...000.
  2. Call the Duplicate API for data filling. MaskReg can indicate which elements participate in computation. From step 1, the 1st bit and the 249th bit in MaskReg are 1. Using this mask, only the 1st and 249th numbers in RegTensor are filled with the value 2.
  3. Use the StoreAlign API to save the results from RegTensor to UB.
  4. Set all bits in MaskReg to 1, and save the data from MaskReg to UB (address = save address from step 3 + 256B) through the StoreAlign API, implementing the function of storing MaskReg data on UB. Corresponding to 32 uint8_t values, each bit of each value is 1, so each value is 255 (0xFF).

### Scenario 2: Composite Computation Scenario
- Example functionality:
  Selects values from the xReg or yReg vector at corresponding positions based on the bits in maskReg. When a mask bit is 1, the corresponding element from src0 is selected; when a mask bit is 0, the corresponding element from src1 is selected.
- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="4" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td align="center">mask</td><td align="center">[1, 32]</td><td align="center">uint8_t</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">ld_st_reg_mask_scenario_2</td></tr>
  </table>
- Example implementation:
   In the SelectVF function, call the LoadAlign API to load mask data from UB into MaskReg, then pass it to the Select API for computation, writing the result back to UB.

- Invocation implementation
  Uses the kernel invocation syntax <<<>>> to call the kernel function.

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
  SCENARIO_NUM=1                                                                # Set the scenario number
  mkdir -p build && cd build;                                                   # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # Build the project (default npu mode)
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM                     # Generate test input data
  ./demo                                                                        # Run the compiled executable to execute the example
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`, `2` | Scenario number: 1=basic transfer scenario, 2=composite computation scenario |

- Execution result
  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
