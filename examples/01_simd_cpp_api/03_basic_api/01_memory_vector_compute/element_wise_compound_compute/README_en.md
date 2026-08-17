# ElementWiseCompoundCompute Example

## Overview

This example demonstrates the usage of compound computation interfaces. Compound computation interfaces fuse multiple computation operations into a single instruction, which effectively reduces the number of instructions, lowers intermediate storage overhead, and improves computation efficiency compared to calling multiple basic interfaces separately. Refer to AddRelu/Axpy for interface documentation.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── element_wise_compound_compute
│   ├── scripts
│   │   ├── gen_data.py                      // Script for generating input data and ground truth data
│   │   └── verify_result.py                 // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt                       // Build project file
│   ├── data_utils.h                         // Data read/write functions
│   ├── element_wise_compound_compute.asc    // Ascend C example implementation & invocation example
│   └── README.md                            // Example documentation
```

## Example Description

- Example specifications:
  <table border="2">
  <caption>Table 1: Example Specifications</caption>
  <tr>
    <th align="left">Scenario Number (SCENARIO_NUM)</th>
    <th align="left">Interface Name</th>
    <th align="left">Function Description</th>
    <th align="left">Computation Formula</th>
    <th align="left">Input Type</th>
    <th align="left">Output Type</th>
  </tr>
  <tr>
    <td align="left">1</td>
    <td align="left">AddRelu</td>
    <td align="left">Vector addition fused with ReLU activation</td>
    <td align="left">dst = max(src0 + src1, 0)</td>
    <td align="left">half</td>
    <td align="left">half</td>
  </tr>
  <tr>
    <td align="left">2</td>
    <td align="left">Axpy</td>
    <td align="left">Scalar multiplication fused with vector addition</td>
    <td align="left">dst = dst + src * scalar</td>
    <td align="left">half</td>
    <td align="left">half</td>
  </tr>
  </table>

  Input and output shapes are both [1, 512], format is ND, and the kernel function name is `element_wise_compound_compute_custom`.

- Example implementation:
  - Kernel implementation
    - Call the DataCopy basic API to transfer data from GM (Global Memory) to UB (Unified Buffer)
    - Call different compound computation interfaces based on the scenario: Scenario 1 calls AddRelu to fuse vector addition with ReLU activation; Scenario 2 calls Axpy to fuse scalar multiplication with vector addition
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
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM  # Generate test input data
  ./demo                        # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin  # Verify whether the output result is correct
  ```
  The following execution result indicates that the accuracy comparison is successful.
  ```bash
  test pass!
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
  | `SCENARIO_NUM` | `1` (default), `2` | Scenario number: 1 (AddRelu), 2 (Axpy) |
