# Copy API Example

## Overview

This example implements UB (Unified Buffer) internal data transfer functionality based on the Copy API, applicable to scenarios that require data transfer between different TPositions such as VECIN, VECCALC, and VECOUT. The example supports switching between different scenarios through build parameters, helping developers understand the usage of the Copy API.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── copy_ub2ub
│   ├── scripts
│   │   ├── gen_data.py             // Input data and ground truth generation script
│   │   └── verify_result.py        // Verification script to check output data against ground truth
│   ├── CMakeLists.txt              // Build configuration file
│   ├── data_utils.h                // Data read/write functions
│   ├── copy.asc                    // Ascend C example implementation & invocation example
│   └── README.md                   // Example description document
```

## Scenario Description

This example selects different scenarios through the build parameter `SCENARIO_NUM`. All scenarios use ND data format, and the kernel function name is `copy_custom`.

<table border="2">
<caption>Table 1: Scenario Configuration Comparison</caption>
<tr><th>scenarioNum</th><th>Input Shape</th><th>Output Shape</th><th>Computation Mode</th><th>Description</th></tr>
<tr><td>1</td><td>[1, 512]</td><td>[1, 512]</td><td>Tensor high-dimensional split computation</td><td>Source and destination operand spaces are the same size</td></tr>
<tr><td>2</td><td>[18, 64]</td><td>[18, 8]</td><td>Tensor high-dimensional split computation</td><td>Source and destination operand spaces are different sizes</td></tr>
<tr><td>3</td><td>[18, 64]</td><td>[18, 8]</td><td>Counter mode</td><td>Source and destination operand spaces are different sizes</td></tr>
</table>

### Scenario Parameter Description

**Tensor high-dimensional split computation**: Controls the number of elements participating in computation per iteration through the mask parameter. Each DataBlock is 32B, containing 8 elements (for int32 type). Controls the number of iterations through the repeatTime parameter, and the address stride of source and destination operands through the stride parameter.

**Counter mode**: The mask parameter indicates the number of elements processed per Repeat. The number of elements participating in computation is repeatTime * mask. Set the computation mode through SetMaskCount, and set the mask through SetVectorMask.

**stride parameter**: {dstStride, srcStride, dstRepeatSize, srcRepeatSize} controls the address stride of source and destination operands within the same iteration and between adjacent iterations.

- **Scenario 1**: Tensor high-dimensional split computation, mask=64, repeatTime=8, stride={1, 1, 8, 8}. Source and destination operand spaces are the same size, processing 64 elements per iteration, 8 iterations, transferring 512 elements in total.

- **Scenario 2**: Tensor high-dimensional split computation, mask=8, repeatTime=18, stride={1, 1, 1, 8}. Transfers [18, 8] from [18, 64]. srcRepeatSize=8 means the source operand skips 64 elements per Repeat (jumping to the next row), dstRepeatSize=1 means the destination operand is packed tightly, transferring 144 elements in total.

- **Scenario 3**: Counter mode, mask=144, repeatTime=1, stride={1, 8, 8, 8}. Transfers [18, 8] from [18, 64]. srcStride=8 means the source operand DataBlock address stride is 8 (taking the first 8 elements of each row), transferring 144 elements in total.

## Example Description

- Example specifications
  <table border="2">
  <caption>Table 2: Example Specifications</caption>
  <tr>
    <td align="center">Category</td>
    <td align="center">name</td>
    <td align="center">shape</td>
    <td align="center">data type</td>
    <td align="center">format</td>
  </tr>
  <tr>
    <td align="center">Example Input</td>
    <td align="center">x</td>
    <td align="center">[1, 512]/[18, 64]</td>
    <td align="center">int32</td>
    <td align="center">ND</td>
  </tr>
  <tr>
    <td align="center">Example Output</td>
    <td align="center">z</td>
    <td align="center">[1, 512]/[18, 8]</td>
    <td align="center">int32</td>
    <td align="center">ND</td>
  </tr>
  <tr>
    <td align="center">Kernel Function Name</td>
    <td colspan="4" align="center">copy_custom</td>
  </tr>
  </table>

- Example implementation
  - Kernel implementation
    - Calls the DataCopy basic API to transfer data from GM (Global Memory) to UB (Unified Buffer)
    - Calls the Copy API to transfer data from UB (Unified Buffer) to UB (Unified Buffer), supporting both tensor high-dimensional split computation and Counter mode
    - Calls the DataCopy basic API to transfer data from UB (Unified Buffer) to GM (Global Memory)

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
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin  # Verify whether the output result is correct
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
  | `SCENARIO_NUM` | `1` (default), `2`, `3` | Scenario number |

- Execution result

  The following execution result indicates that the precision comparison is successful.

  ```bash
  test pass!
  ```
