# DataCopy Data Slice Transfer Example

## Overview
This example implements data slice transfer based on DataCopy, extracting subsets of multi-dimensional Tensor data for transfer between GM (Global Memory) and UB (Unified Buffer) paths.
## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure
```
├── data_copy_gm2ub_slice
│   ├── scripts
│   │   ├── gen_data.py         // Input data and ground truth generation script
│   │   └── verify_result.py    // Verification script to check output data against ground truth
│   ├── CMakeLists.txt          // Build configuration file
│   ├── data_utils.h            // Data read/write functions
│   ├── slice.asc               // Ascend C example implementation & invocation example
│   └── README.md               // Example description document
```

## Example Description
- Example functionality:
  Implements a data slice transfer example that supports extracting subsets of a two-dimensional source operand Tensor[3, 87] (extracting 4 data segments: [0, 16:40], [0, 47:71], [2, 16:40], [2, 47:71], totaling 96 float32 values) and contiguously transferring them to a two-dimensional destination operand Tensor[2, 48]. Refer to [Slice Data Transfer](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopy（GM与UB切片数据搬运）.md) for API documentation.

- Example specifications:

  <table>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">Slice Parameters</td></tr>
  <tr><td align="center">x</td><td align="center">[3, 87]</td><td align="center">float32</td><td align="center">ND</td><td align="center">[[0, 16:40], [0, 47:71]], [[2, 16:40], [2, 47:71]]</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">y</td><td align="center">[2, 48]</td><td align="center">float32</td><td align="center">ND</td><td align="center">Contiguous write to Tensor[2, 48]</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">kernel_slice</td></tr>
  </table>

- Example implementation:
  - Kernel implementation
    The computation logic is: input data must first be transferred from GM (Global Memory) to UB (Unified Buffer) according to slice parameters, then transferred out to external GM (Global Memory).

    Refer to Ascend C API DataCopy slice data transfer for detailed API description.

  - Invocation implementation
    Uses the kernel invocation syntax <<<>>> to call the kernel function.

## Build and Run
Run the following steps in the root directory of this example to build and run it.
- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;                                               # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # Build the project, default npu mode
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct, confirming algorithm logic correctness
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
