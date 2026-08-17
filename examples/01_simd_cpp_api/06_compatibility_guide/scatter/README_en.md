# Scatter Compatibility Example

## Overview

This example demonstrates the data scatter function, which can scatter the input tensor into the result tensor based on the input tensor and destination address offset tensor.

Different hardware implementations are isolated through compile-time macros:
- Atlas A2/A3: Does not support the Scatter instruction. Uses the scalar transfer method (GetValue/SetValue loop) for implementation.
- Ascend 950PR/Ascend 950DT: Directly calls the Scatter instruction for implementation.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── scatter
│   ├── scripts
│   │   ├── gen_data.py         // Input data and ground truth generation script
│   │   └── verify_result.py    // Verify whether output data matches ground truth
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── scatter_custom.asc      // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Specifications

<table>
<caption>Example Specification Table</caption>
<tr><td rowspan="1" align="center">Category</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td rowspan="2" align="center">Example Input</td>
<td align="center">src</td><td align="center">[1, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td align="center">dst_offset</td><td align="center">[1, 128]</td><td align="center">uint32_t</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">dst</td><td align="center">[1, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">scatter_custom</td></tr>
</table>

## Example Implementation

- Atlas A2/A3: Does not support the Scatter instruction. Uses scalar GetValue/SetValue loops to read offset addresses and source data element by element, writing source data to the target position to implement data scatter.
- Ascend 950PR/Ascend 950DT: Calls the Scatter instruction to scatter source data to the target tensor based on offset addresses.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in this example directory.
  ```bash
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution results

  The following execution result indicates that the accuracy comparison succeeded:

  ```bash
  test pass!
  ```
