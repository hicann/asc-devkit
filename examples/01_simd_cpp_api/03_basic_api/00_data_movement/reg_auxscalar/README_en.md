# auxscalar_reg Example

## Overview
This example implements reading multiple scalar data from UB (Unified Buffer) via AuxScalar based on the Reg programming interface, combined with Adds for vector-scalar addition computation.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure
```
├── reg_auxscalar
│   ├── scripts
│   │   ├── gen_data.py                // Input data and ground truth generation script
│   ├── CMakeLists.txt                 // Build configuration file
│   ├── data_utils.h                   // Data read/write functions
│   ├── auxscalar_reg.asc              // Ascend C example implementation & invocation example
│   └── README.md                      // Example description
```

## Example Description
- Example functionality:
  Reads 4 scalar data values from UB and performs Adds computation with vector x. Vector x has shape [1, 512]. A for loop controls each scalar to compute with 128 consecutive vector elements.
  - Scalars read via AuxScalar can be used directly within VF functions. When used in MainScalar (outside VF functions), synchronization instructions are required

  **AuxScalar + Adds Mode**
  - Uses the AuxScalar method (`__ubuf__` pointer subscript access, such as `scalarAddr[0]`) to read scalars from UB, combined with Adds for vector-scalar addition
  - Example specifications:
    <table>
    <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
    <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">x</td><td align="center">[1, 512]</td><td align="center">half</td></tr>
    <tr><td align="center">scalar</td><td align="center">[1, 4] (input_y.bin actually contains 16 half values for 32B alignment, the last 12 are padding)</td><td align="center">half</td></tr>
    <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[1, 512]</td><td align="center">half</td></tr>
    <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">auxscalar_reg</td></tr>
    </table>
  - Example implementation:
    In the AuxScalarAddsVF function, reads the i-th scalar value from UB via `scalarAddr[i]` and calls `Adds` for vector-scalar addition computation.
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
