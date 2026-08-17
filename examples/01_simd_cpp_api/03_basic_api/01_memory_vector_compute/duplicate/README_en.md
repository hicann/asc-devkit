# Duplicate Example

## Overview

This example implements data padding functionality based on the Duplicate API, copying a scalar value or immediate number multiple times and filling it into a vector. The Duplicate API supports copying a single scalar value or immediate number a specified number of times to fill all elements of the destination Tensor, commonly used for Tensor initialization, constant padding, mask generation, and other scenarios.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── duplicate
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── duplicate.asc           // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  This example demonstrates using the Duplicate API to implement data padding functionality, copying a scalar value or immediate number multiple times and filling it into a vector. The Duplicate API is applicable to scenarios where constant values need to be filled into a Tensor, such as Tensor initialization, constant padding, and mask generation. The value parameter specifies the scalar value to fill, and the count parameter specifies the number of elements to fill. For detailed API documentation, refer to [Duplicate API Documentation](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta1/API/ascendcopapi/atlasascendc_api_07_0088.html).

- Example specifications:  
  <table border="2" align="center">
  <caption>Table 1: Example Input/Output Specifications</caption>
  <tr><td rowspan="1" align="center">Example Type</td><td colspan="4" align="center">Duplicate</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[1,256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">y</td><td align="center">[1,256]</td><td align="center">half</td><td align="center">ND</td></tr>
  
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">duplicate_custom</td></tr>
  </table>

- Example implementation:  
  This example implements a Duplicate data padding example with fixed input shape x[1,256] and output shape y[1,256].
  
  Duplicate API parameter description:
  - dst: destination operand for storing the padding result
  - value: scalar value or immediate number to fill, in this example a half-type constant value of 18.0
  - count: number of elements to fill, in this example 256

  - Kernel implementation  
    - Call the DataCopy basic API to transfer data from GM (Global Memory) to UB (Unified Buffer)
    - Call the Duplicate interface to fill the scalar value into all elements of the output Tensor
    - Call the DataCopy basic API to transfer results from UB (Unified Buffer) to GM (Global Memory)

  - Invocation implementation  
    Use the kernel invocation operator <<<>>> to call the kernel function.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables  
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.
    
- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic is correct
  ```
  The following execution result indicates that the accuracy comparison is successful.
  ```bash
  test pass!
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  
  Examples:
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # CPU debug mode
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # NPU simulation mode
  ```
  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
