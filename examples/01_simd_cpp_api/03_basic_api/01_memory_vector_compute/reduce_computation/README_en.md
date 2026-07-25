# ReduceComputation Example

## Overview

This example implements reduction computation based on ReduceMax/ReduceMin/ReduceSum interfaces, supporting the following 6 scenarios:
    <table>
  	  	 	<tr>
  	  	 		<td>scenarioNum</td>
  	  	 		<td>Scenario</td>
          <td>Description</td>
  	  	 	</tr>
  	  	 	<tr>
  	  	 		<td>1</td>
  	  	 		<td>ReduceMax first-n computation</td>
          <td>Find the maximum value and its index position from the first n data elements of the input tensor</td>
  	  	 	</tr>
  	  	 	<tr>
  	  	 		<td>2</td>
  	  	 		<td>ReduceMax high-dimensional split computation</td>
          <td>Find the maximum value and its index position from all input data, using mask to control elements participating in each iteration</td>
  	  	 	</tr>
        <tr>
  	  	 		<td>3</td>
  	  	 		<td>ReduceMin first-n computation</td>
          <td>Find the minimum value and its index position from the first n data elements of the input tensor</td>
  	  	 	</tr>
        <tr>
  	  	 		<td>4</td>
  	  	 		<td>ReduceMin high-dimensional split computation</td>
          <td>Find the minimum value and its index position from all input data, using mask to control elements participating in each iteration</td>
  	  	 	</tr>
        <tr>
  	  	 		<td>5</td>
  	  	 		<td>ReduceSum first-n computation</td>
          <td>Sum the first n data elements of the input tensor</td>
  	  	 	</tr>
        <tr>
  	  	 		<td>6</td>
  	  	 		<td>ReduceSum high-dimensional split computation</td>
          <td>Sum all input data, using mask to control elements participating in each iteration</td>
  	  	 	</tr>
  	  	 </table>

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── reduce_computation
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── reduce_computation.asc  // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  This example implements reduction computation based on ReduceMax/ReduceMin/ReduceSum interfaces, including first-n data computation and tensor high-dimensional split computation interfaces. Refer to [ReduceMax](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/归约计算/ReduceMax.md)/[ReduceMin](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/归约计算/ReduceMin.md)/[ReduceSum](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/归约计算/ReduceSum.md) for interface documentation.

- Example specifications:  
  The input/output specifications for different scenarios are shown in the table below:

  <table border="2" align="center">
  <tr>
    <th align="center">scenarioNum</th>
    <th align="center">Example Scenario</th>
    <th align="center">Input name</th>
    <th align="center">Input shape</th>
    <th align="center">Input data type</th>
    <th align="center">Output name</th>
    <th align="center">Output shape</th>
    <th align="center">Output data type</th>
  </tr>
  <tr>
    <td align="center">1</td>
    <td align="center">ReduceMax first-n computation</td>
    <td align="center">x</td>
    <td align="center">[1, 288]</td>
    <td align="center">half</td>
    <td align="center">y</td>
    <td align="center">[1, 16]</td>
    <td align="center">half</td>
  </tr>
  <tr>
    <td align="center">2</td>
    <td align="center">ReduceMax high-dimensional split</td>
    <td align="center">x</td>
    <td align="center">[1, 512]</td>
    <td align="center">half</td>
    <td align="center">y</td>
    <td align="center">[1, 16]</td>
    <td align="center">half</td>
  </tr>
  <tr>
    <td align="center">3</td>
    <td align="center">ReduceMin first-n computation</td>
    <td align="center">x</td>
    <td align="center">[1, 288]</td>
    <td align="center">half</td>
    <td align="center">y</td>
    <td align="center">[1, 16]</td>
    <td align="center">half</td>
  </tr>
  <tr>
    <td align="center">4</td>
    <td align="center">ReduceMin high-dimensional split</td>
    <td align="center">x</td>
    <td align="center">[1, 512]</td>
    <td align="center">half</td>
    <td align="center">y</td>
    <td align="center">[1, 16]</td>
    <td align="center">half</td>
  </tr>
  <tr>
    <td align="center">5</td>
    <td align="center">ReduceSum first-n computation</td>
    <td align="center">x</td>
    <td align="center">[1, 288]</td>
    <td align="center">half</td>
    <td align="center">y</td>
    <td align="center">[1, 16]</td>
    <td align="center">half</td>
  </tr>
  <tr>
    <td align="center">6</td>
    <td align="center">ReduceSum high-dimensional split</td>
    <td align="center">x</td>
    <td align="center">[8320]</td>
    <td align="center">half</td>
    <td align="center">y</td>
    <td align="center">[1, 16]</td>
    <td align="center">half</td>
  </tr>
  </table>

- Example implementation:

  - Kernel implementation
    - Call the DataCopy basic API to transfer data from GM (Global Memory) to UB (Unified Buffer), and transfer data after reduction computation out to GM (Global Memory).
    - Call ReduceMax/ReduceMin/ReduceSum interfaces to complete reduction computation.
    - In the ReduceSum first-n data computation scenario, call [GetReduceRepeatSumSpr](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/归约计算辅助配置接口/GetReduceRepeatSumSpr(ISASI).md) to obtain computation results.

- Invocation implementation  
  Use the kernel invocation operator `<<<>>>` to call the kernel function.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.
- Configure environment variables  
  Configure environment variables according to the [installation method](../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;                                               # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM                     # Generate test input data
  ./demo                                                                    # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic is correct
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
  | `SCENARIO_NUM` | `1`, `2`, `3`, `4`, `5`, `6` | Scenario number: 1=ReduceMax first-n, 2=ReduceMax high-dimensional split, 3=ReduceMin first-n, 4=ReduceMin high-dimensional split, 5=ReduceSum first-n, 6=ReduceSum high-dimensional split |

- Execution result

  The following execution result indicates that the accuracy comparison is successful:
  ```bash
  test pass!
  ```
