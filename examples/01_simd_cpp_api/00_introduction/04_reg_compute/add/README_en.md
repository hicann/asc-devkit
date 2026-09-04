# RegBase Programming Add Example

## Overview
This example implements vector self-addition based on the RegBase programming paradigm, with the computation logic `y = x + x`. The example first moves input data from GM (Global Memory) to UB (Unified Buffer), then completes register-level Add computation by calling RegBase interfaces through VF functions, and finally writes the results from UB back to GM.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|----------------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure
```
├── add
│   ├── scripts
│   │   ├── gen_data.py                // Input data and ground truth data generation script
│   │   └── verify_result.py           // Ground truth comparison file
│   ├── CMakeLists.txt                 // Build project file
│   ├── data_utils.h                   // Data read and write functions
│   ├── add.asc                        // AscendC example implementation & call example
│   └── README.md                      // Example documentation
```

## Example Description
- Example Function:  
  This example implements vector self-addition based on the RegBase programming paradigm. Input `x` is `float` type two-dimensional data, output `y` has the same shape as input, and each output element satisfies `y[i] = x[i] + x[i]`. The example uses 4-core parallel processing with a total input length of `256 * 256` `float` elements. Each core processes `totalLength / 4` consecutive elements, demonstrating the basic flow of GM/UB data movement, VF function calls, RegBase register computation, and pipeline synchronization.
- Example Specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type(OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[256, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">y</td><td align="center">[256, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Kernel Name</td><td colspan="3" align="center">vector_add</td></tr>
  </table>

- Example Implementation:
  - Kernel Implementation
    - Obtain the block index of the current core through `GetBlockIdx` and calculate the data offset `coreOffset` for the current core.
    - Use `GlobalTensor` to bind input and output GM addresses.
    - Use `LocalMemAllocator<Hardware::UB>` to allocate UB cache for the current core.
    - Call `DataCopy` to move data from GM to UB for the current core.
    - Use `SetFlag<HardEvent::MTE2_V>` and `WaitFlag<HardEvent::MTE2_V>` to ensure Vector computation starts only after GM to UB movement completes.
    - Call `AddVF` function through `asc_vf_call` to complete the `LoadAlign -> Add -> StoreAlign` register computation flow within the VF function.
    - Use `SetFlag<HardEvent::V_MTE3>` and `WaitFlag<HardEvent::V_MTE3>` to ensure UB results are moved back to GM only after Vector computation completes.
    - Call `DataCopy` to move results from UB to GM.

  - Call Implementation  
    Use the kernel call operator `<<<>>>` to launch the `vector_add` kernel function.

## Build and Run

Execute the following steps in the root directory of this example to build and run the example.
- Configure Environment Variables  
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit package on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, it is installed to `/usr/local/Ascend` by default.

- Example Execution

  Execute the following commands in this example directory.
  ```bash
  mkdir -p build && cd build;                                               # Create and enter build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # Build project (default npu mode)
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Execute the compiled executable program to run the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify output results are correct
  ```

  For CPU debugging or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # CPU debugging mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Note:** Before switching build modes, clean the cmake cache. Execute `rm CMakeCache.txt` in the build directory and re-run cmake.

- Build Options Description

  | Option | Available Values | Description |
  |--------|------------------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debugging, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: Ascend 950PR/Ascend 950DT |

- Execution Results  
  The execution results are as follows, indicating the precision comparison succeeded.
  ```bash
  test pass!
  ```