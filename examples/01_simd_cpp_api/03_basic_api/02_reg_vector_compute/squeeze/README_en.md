# squeeze Example

## Overview
This example implements the Squeeze operation based on the Reg programming interface, primarily calling the Squeeze API and the StoreUnAlign/StoreUnAlignPost APIs.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure
```
├── squeeze
│   ├── scripts
│   │   ├── gen_data.py                // Input data and ground truth generation script
│   ├── CMakeLists.txt                 // Build configuration file
│   ├── data_utils.h                   // Data read/write functions
│   ├── squeeze.asc                    // Ascend C example implementation & invocation example
│   └── README.md                      // Example description
```

## Example Description
- Example functionality:
  This example sets MaskReg to MaskPattern::M4. In each iteration, elements from the input vector xReg whose indices are multiples of 4 are selected and sequentially copied into the output vector yReg in a contiguous layout, with the remaining positions in the output vector set to 0.
  - When the Squeeze API template parameter store is configured as STORE_REG, it can record the number of valid elements and store it in the AR special register, for use with the StoreUnAlign API
  - The StoreUnAlign API can use the value recorded in the AR special register as the number of elements to transfer, enabling contiguous unaligned transfer to move the Squeeze results out consecutively
- Constraint description:
  - When the Squeeze API template parameter store is configured as STORE_REG, the Squeeze API and StoreUnAlign API must be used alternately to ensure proper enabling of the AR special register
  - Before computation, call the ClearSpr API to clear the AR register; otherwise, residual data may cause precision issues
- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">y</td><td align="center">[1, 64]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">squeeze</td></tr>
  </table>
- Example implementation:
  - The input vector shape is [1, 256] with data type float. Four iterations are performed, with each iteration processing 64 elements
  - Squeeze: Each call selects elements from xReg whose indices are multiples of 4, that is, xReg[i * 4], and writes them consecutively into yReg[i]. The result y[0:16] contains 16 valid elements, the remaining y[16:64] is set to 0, and the AR special register value is set to 16
  - StoreUnAlign: Uses the value in the AR register as the number of elements to transfer, moving the first 16 elements from yReg to the unaligned register ureg or the output UB address yAddr
  - StoreUnAlignPost: After all 4 iterations complete, moves the remaining data from ureg
  - Invocation implementation: Uses the kernel invocation syntax <<<>>> to call the kernel function.

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # Build the project (default npu mode)
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Run the compiled executable to execute the example
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
