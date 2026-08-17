# reg_sync Example

## Overview
This example implements a synchronization mechanism for UB (Unified Buffer) read or write operations based on the Reg programming interface, supporting multiple scenarios. Select the scenario through the CMake build parameter `SCENARIO_NUM`.
    <table>
   	 	<tr>
        <td>SCENARIO_NUM</td>
 	 		<td>Synchronization Scenario</td>
  	 	</tr>
  	 	<tr>
  	 		<td>1</td>
  	 		<td>Register ordering</td>
  	 	</tr>
  	 	<tr>
  	 		<td>2</td>
  	 		<td>LocalMemBar (write-read dependency)</td>
  	 	</tr>
  	 </table>


## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```
├── reg_sync
│   ├── scripts
│   │   ├── gen_data.py                // Script for generating input data and ground truth data
│   ├── CMakeLists.txt                 // Build project file
│   ├── data_utils.h                   // Data read/write functions
│   ├── reg_sync.asc                   // Ascend C operator implementation & invocation example
│   └── README.md                      // Example documentation
```

## Example Description

### Scenario 1: Register Ordering

**Example function**: Perform in-place exp computation on the input vector x, writing the result back to the same address.

**Example specifications**:
<table>
<tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
<tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
<tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">float</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[1, 1024]</td><td align="center">float</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="3" align="center">reg_sync</td></tr>
</table>

- Example implementation  
1. Call the LoadAlign interface to read data from the UB address dataAddr and write it to reg0
2. Perform Exp computation on reg0, writing the result back to reg0
3. Call the StoreAlign interface to write the computed reg0 back to the same dataAddr

- Synchronization description
1. In steps 1 and 3 above, LoadAlign and StoreAlign operate on the same address, requiring the timing of reading UB first and then writing UB
2. Because reading UB and writing UB use the same register reg0, hardware register ordering is triggered, and instructions execute in code order without inserting synchronization instructions

- Invocation implementation  
  Use the kernel invocation operator `<<<>>>` to call the kernel function, launching 1 core.

### Scenario 2: LocalMemBar (Write-Read Dependency)

**Example function**: Perform softmax computation on the input vector x

**Example specifications**:
<table>
<tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
<tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
<tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">float</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[1, 1024]</td><td align="center">float</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="3" align="center">reg_sync</td></tr>
</table>

- Example implementation 
 
1. Find the maximum value of all input data x, and fill maxReg with the maximum value
2. Compute the difference between input data and the maximum value sequentially, then take the natural exponent of the difference, that is, expReg = exp(srcReg - maxReg)
3. Call the StoreAlign interface to transfer the exp result to the temporary buffer (tmpLocal) in UB
4. Sum all exp results and fill sumReg with the sum
5. Call the LoadAlign interface to read the exp result from the UB temporary buffer (tmpLocal)
6. Compute exp divided by sumReg sequentially and transfer the result out

- Synchronization description

1. In steps 3 and 5 above, StoreAlign and LoadAlign operate on the same address, with a write-after-read dependency (RAW), requiring the timing of writing UB first and then reading UB
2. Manually call the LocalMemBar interface to insert synchronization between VEC_STORE and VEC_LOAD to ensure instructions execute in code order
3. Without LocalMemBar, step 5 may execute first, reading old data from tmpLocal that has not been updated by step 3

- Invocation implementation  
  Use the kernel invocation operator `<<<>>>` to call the kernel function, launching 1 core.

## Build and Run
Run the following steps in the root directory of this example to build and run the operator.
- Configure environment variables  
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.
    
- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO_NUM=1
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

  > **Notice:** Clear the cmake cache before switching build modes or scenarios. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`, `2` | Scenario number: 1=Register ordering, 2=LocalMemBar |

- Execution result  
  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
