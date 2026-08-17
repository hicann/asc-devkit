# BroadCastVecToMM Example

## Overview
This example implements data broadcast transfer based on BroadCastVecToMM, broadcasting data located in UB (Unified Buffer) and transferring it to L0C Buffer.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Atlas Inference Series Products AI Core | >= CANN 9.0.0 |

## Directory Structure
```
├── broadcast_ub2l0c
│   ├── scripts
│   │   ├── gen_data.py               // Input data and ground truth generation script
│   │   └── verify_result.py          // Verification script to check output data against ground truth
│   ├── CMakeLists.txt                // Build configuration file
│   ├── data_utils.h                  // Data read/write functions
│   ├── broad_cast_vec_to_mm.asc      // Ascend C example implementation & invocation example
│   └── README.md                     // Example description document
```

## Example Description
- Example functionality:

  This example broadcasts data with shape [1, 16] located in UB (Unified Buffer) to [16, 16] and transfers it to L0C Buffer. Refer to BroadCastVecToMM for API documentation.
- Example specifications:

  <table border="2">
  <tr><td rowspan="3" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">z</td><td align="center">[16, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">broad_cast_vec_to_mm_custom</td></tr>
  </table>

- Example implementation:

  - Kernel implementation
    - Calls the DataCopy basic API to transfer data from GM (Global Memory) to UB (Unified Buffer), and transfers the broadcast data out to GM (Global Memory).
    - Calls the BroadCastVecToMM basic API to broadcast data on UB (Unified Buffer) from [1, 16] to [16, 16] and transfer it to L0C Buffer.
    - Calls the DataCopy enhanced data transfer API to transfer the broadcast data from L0C Buffer to UB (Unified Buffer).

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
  mkdir -p build && cd build;                                               # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2002 ..;make -j;                      # Build the project, default npu mode
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct, confirming algorithm logic correctness
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2002 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2002 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2002` | NPU architecture: dav-2002 corresponds to Atlas Inference Series Products AI Core |

- Execution result

  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
