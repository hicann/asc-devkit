# scatter_st_reg Example

## Overview
This example implements discrete data store functionality (scattering elements to UB (Unified Buffer)) based on the Reg programming interface, using the Scatter API.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure
```
├── reg_store_scatter
│   ├── scripts
│   │   ├── gen_data.py                // Input data and ground truth generation script
│   ├── CMakeLists.txt                 // Build configuration file
│   ├── data_utils.h                   // Data read/write functions
│   ├── scatter_st_reg.asc             // Ascend C example implementation & invocation example
│   └── README.md                      // Example description
```

## Example Description
- Example functionality:
  Disperses elements from the source data to destination addresses based on indices.

  **Scatter Mode**
  - The source operand data volume is 128 elements, the index data volume is 128 elements, and the output data volume is 128 elements. The generated index is a reversed sequence of 128 elements, so the stored data volume is also 128.
  - Example specifications:
    <table>
    <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
    <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">x</td><td align="center">[1, 128]</td><td align="center">half</td></tr>
    <tr><td></td><td align="center">index</td><td align="center">[1, 128]</td><td align="center">uint16_t</td></tr>
    <tr><td rowspan="1" align="center">Example Output</td><td align="center">y</td><td align="center">[1, 128]</td><td align="center">half</td></tr>
    <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">scatter_st_reg</td></tr>
    </table>
  - Example implementation:
    The ScatterVF function calls the Scatter API to scatter data to UB by element index.
    - Invocation implementation
      Uses the kernel invocation syntax `<<<>>>` to call the kernel function, launching 1 core.

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
  mkdir -p build && cd build;                                                         # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                                # Build the project (default npu mode)
  python3 ../scripts/gen_data.py;                                                     # Generate test input data
  ./demo                                                                              # Run the compiled executable to execute the example
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
