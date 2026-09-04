# DataCopy Quantization Activation Transfer Example

## Overview

This example implements inline quantization activation data transfer based on DataCopy, transferring matrix multiplication results from L0C Buffer to GM (Global Memory). It supports combinations of inline NZ2ND, inline quantization, and relu activation capabilities. This example provides 6 different test scenarios.
    <table>
  	  	 	<tr>
  	  	 		<td>scenarioNum</td>
          <td>Output Format</td>
  	  	 		<td>Quantization Mode</td>
          <td>Type Conversion</td>
          <td>Activation Mode</td>
  	  	 	</tr>
  	  	 	<tr>
  	  	 		<td>1</td>
          <td>ND</td>
  	  	 		<td>Scalar quantization mode</td>
          <td>s322f16</td>
          <td>relu activation</td>
  	  	 	</tr>
  	  	 	<tr>
  	  	 		<td>2</td>
          <td>NZ</td>
  	  	 		<td>Vector quantization mode</td>
          <td>s322f16</td>
          <td>None</td>
  	  	 	</tr>
        <tr>
  	  	 		<td>3</td>
          <td>NZ</td>
  	  	 		<td>Scalar quantization mode</td>
          <td>f322s8</td>
          <td>None</td>
  	  	 	</tr>
        <tr>
  	  	 		<td>4</td>
          <td>ND</td>
  	  	 		<td>Vector quantization mode</td>
          <td>f322s8</td>
          <td>relu activation</td>
  	  	 	</tr>
        <tr>
  	  	 		<td>5</td>
          <td>ND</td>
  	  	 		<td>Scalar quantization mode</td>
          <td>s322s8</td>
          <td>None</td>
  	  	 	</tr>
        <tr>
  	  	 		<td>6</td>
          <td>NZ</td>
  	  	 		<td>Vector quantization mode</td>
          <td>s322s8</td>
          <td>relu activation</td>
  	  	 	</tr>
  	  	 </table>

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.2.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.2.0 |

## Directory Structure

```
├── data_copy_l0c2gm
│   ├── scripts
│   │   ├── gen_data.py         // Input data and ground truth generation script
│   │   └── verify_result.py    // Verification script to check output data against ground truth
│   ├── CMakeLists.txt          // Build configuration file
│   ├── data_utils.h            // Data read/write functions
│   ├── data_copy_l0c2gm.asc    // Ascend C example implementation & invocation example
│   ├── README.md               // Chinese example documentation
│   └── README_en.md            // English example documentation
```

## Example Description

- Example functionality
  Transfers matrix multiplication results from L0C Buffer to GM (Global Memory), supporting the following two inline quantization modes:
  - Scalar quantization: Uses the SetFixpipePreQuantFlag API to set Scalar quantization parameters.
  - Vector quantization: Uses the SetFixPipeConfig API to set Vector quantization parameters.
  Supports combination with inline NZ2ND and relu activation capabilities. Refer to inline quantization activation transfer for API documentation.

- Example specifications

  <table>
  <caption>Scenario 1: Scalar quantization + relu activation</caption>
  <tr><td rowspan="4" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[128, 128]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[128, 256]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">z</td><td align="center">[128, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">data_copy_l0c2gm</td></tr>
  </table>

  <table>
  <caption>Scenario 2: Vector quantization</caption>
  <tr><td rowspan="4" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[128, 128]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[128, 256]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">z</td><td align="center">[128, 256]</td><td align="center">half</td><td align="center">NZ</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">data_copy_l0c2gm</td></tr>
  </table>

  <table>
  <caption>Scenario 3: Scalar quantization</caption>
  <tr><td rowspan="4" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[128, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[128, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">z</td><td align="center">[128, 256]</td><td align="center">int8_t</td><td align="center">NZ</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">data_copy_l0c2gm</td></tr>
  </table>

  <table>
  <caption>Scenario 4: Vector quantization + relu activation</caption>
  <tr><td rowspan="4" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[128, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[128, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">z</td><td align="center">[128, 256]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">data_copy_l0c2gm</td></tr>
  </table>

  <table>
  <caption>Scenario 5: Scalar quantization</caption>
  <tr><td rowspan="4" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[128, 128]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[128, 256]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">z</td><td align="center">[128, 256]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">data_copy_l0c2gm</td></tr>
  </table>

  <table>
  <caption>Scenario 6: Vector quantization + relu activation</caption>
  <tr><td rowspan="4" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[128, 128]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[128, 256]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">z</td><td align="center">[128, 256]</td><td align="center">int8_t</td><td align="center">NZ</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">data_copy_l0c2gm</td></tr>
  </table>

- Example implementation

  - Kernel implementation

    - Calls the DataCopy basic API to transfer data from GM (Global Memory) to A1 (L1 Buffer) and B1 (L1 Buffer), performing ND to NZ format conversion.

    - Calls the LoadData API to transfer data from A1 (L1 Buffer) and B1 (L1 Buffer) to A2 (L0A Buffer) and B2 (L0B Buffer).

    - Calls the Mmad API to perform matrix multiplication on input matrix A with shape [128, 128] and input matrix B with shape [128, 256], producing an output result matrix with shape [128, 256].

    - Configures DataCopyCO12DstParams for DataCopy inline quantization activation transfer, transferring the Mmad computation results from L0C Buffer to GM (Global Memory).

  - Invocation implementation
    Uses the kernel invocation syntax `<<<>>>` to call the kernel function.

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
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM ./output/output.bin ./output/golden.bin  # Verify whether the output result is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`, `2`, `3`, `4`, `5`, `6` | Scenario number: 1=Scalar quantization+relu activation+ND output, 2=Vector quantization+NZ output, 3=Scalar quantization+NZ output, 4=Vector quantization+relu activation+ND output, 5=Scalar quantization+ND output, 6=Vector quantization+relu activation+NZ output |

- Execution result
  The following execution result indicates that the precision comparison is successful:
  ```bash
  test pass!
  ```
