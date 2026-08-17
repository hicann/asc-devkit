# GmByPassDCache Class Example

## Overview

This example combines calls to the `ReadGmByPassDCache` and `WriteGmByPassDCache` interfaces to read and write data from GM addresses without going through DCache. This example adds 100 to the read values and writes them to the output address.

When multiple cores operate on GM addresses, if the data cannot be aligned to Cache Line boundaries, the DCache path reads and writes in Cache Line sizes, which causes random data overwriting between cores. In this case, the approach of reading and writing GM addresses directly without going through DCache can be adopted to avoid the random overwriting issue.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── gm_by_pass_dcache
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── gm_by_pass_dcache.asc   // Ascend C operator implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  Combine calls to `ReadGmByPassDCache` and `WriteGmByPassDCache` to implement:
  1. Use `ReadGmByPassDCache` to read int32_t elements one by one from GM addresses
  2. Use `WriteGmByPassDCache` to write the read values plus 100 to the output GM addresses

- Example specifications:
  <table>
  <caption>Table 1: Example Specifications</caption>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[1, 1024]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">kernel_gm_by_pass_dcache</td></tr>  
  </table>

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
  mkdir -p build && cd build;                                        # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;               # Build the project, default npu mode
  python3 ../scripts/gen_data.py                                     # Generate test input data
  ./demo                                                             # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin  # Verify whether the output result is correct
  ```

  To use CPU debug mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;  # CPU debug mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu` | Run mode: NPU execution, CPU debug |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products/Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  
- Execution result

  The following execution result indicates that the accuracy comparison is successful:

  ```bash
  test pass!
  ```
