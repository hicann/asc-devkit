# ld_st_reg_unalign Example

## Overview
This example implements unaligned data transfer operations between UB (Unified Buffer) and RegTensor (the basic unit of Reg vector computation) based on the Reg programming interface. It supports multiple scenarios. Select a scenario through the CMake build parameter `SCENARIO_NUM`.
    <table>
	 	<tr>
        <td>SCENARIO_NUM</td>
	 		<td>Scenario Type</td>
	 	</tr>
	 	<tr>
	 		<td>1</td>
	 		<td>Single-core unaligned data transfer: Uses LoadUnAlign and StoreUnAlign for data transfer at unaligned addresses</td>
	 	</tr>
	 	<tr>
	 		<td>2</td>
	 		<td>Multi-core unaligned scenario computation and transfer: Uses LoadUnAlign to handle unaligned input (uint16_t), ReduceDataBlock(MAX) for block-wise maximum</td>
	 	</tr>
	 </table>


## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure
```
├── reg_load_store_unalign
│   ├── scripts
│   │   ├── gen_data.py                // Input data and ground truth generation script
│   ├── CMakeLists.txt                 // Build configuration file
│   ├── data_utils.h                   // Data read/write functions
│   ├── ld_st_reg_unalign.asc          // Ascend C example implementation & invocation example
│   └── README.md                      // Example description
```

## Example Description

### Scenario 1: Single-core Address-unaligned Data Transfer
- Example functionality:
  Input shape is a [1, 1024] float matrix. Transfers 128 data elements when both the source and destination operand addresses are not 32B aligned.
- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">y</td><td align="center">[1, 128]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">ld_st_reg_unalign_kernel</td></tr>
  <tr><td rowspan="1" align="center">Core Count</td><td colspan="4" align="center">1</td></tr>
  </table>
- Example implementation:
   Offsets the source and destination operand addresses on UB by one element (4 bytes), making the start address not 32B aligned. Calls CopyVF to transfer 128 float values from that address, writing back to UB (Unified Buffer).

### Scenario 2: Multi-core Address-unaligned and Data Volume Unaligned Scenario Transfer
- Example functionality:
  Input shape is a [14, 255] uint16_t matrix, demonstrating a natural use case for LoadUnAlign handling unaligned input. Uses ReduceDataBlock to find the maximum of each DataBlock (16 uint16_t elements), with 4 cores processing in parallel, and StoreUnAlign for contiguous result storage.
- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[14, 255]</td><td align="center">uint16_t</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">y</td><td align="center">[14, 16]</td><td align="center">uint16_t</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">ld_st_reg_unalign_kernel</td></tr>
  <tr><td rowspan="1" align="center">Core Count</td><td colspan="4" align="center">4</td></tr>
  </table>
- Example implementation:
  1. **Data load**:
     - Cores 0, 1, 2 transfer 4x255=1020 elements, core 3 transfers 2x255=510 elements, loading contiguous blocks into UB
     - UB is packed tightly, with 255 elements per row
     - Rows 1-3 start addresses are not 32B aligned (510 bytes, 1020 bytes, 1530 bytes)

  2. **LoadUnAlign natural usage**:
     - All rows uniformly use LoadUnAlignPre + LoadUnAlign to handle both aligned addresses (row 0) and unaligned addresses (rows 1-3)
     - Aligned addresses can also use the StoreAlign API. This is only to maintain uniform code format and serves as an example demonstration, not as a reference for optimal performance

  3. **ReduceDataBlock computation**:
     - Each row of 255 elements is processed in 2 VL blocks (128 + 127)
     - Each VL block produces 8 results (maximum per DataBlock)

  4. **StoreUnAlign contiguous storage**:
     - Uses the StoreUnAlign version with postUpdateStride
     - Each store offsets by 8 elements (resultsPerRepeat=8)
     - After 2 loop iterations, automatically offsets to dstAddr+16
     - StoreUnAlignPost handles the final boundary data

  5. **Output layout**:
     - 16 results per row, data arranged contiguously
     - Cores 0, 1, 2 output 64 values (GM[0-63], GM[64-127], GM[128-191]), core 3 outputs 32 values (GM[192-223])

- Invocation implementation
  Uses the kernel invocation syntax <<<>>> to call the kernel function, launching 4 cores.

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
  SCENARIO_NUM=1
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes or scenarios. Run `rm CMakeCache.txt` in the build directory and re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`, `2` | Scenario number: 1=single-core address-unaligned data transfer, 2=multi-core address-unaligned and data volume unaligned data transfer |

- Execution result
  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
