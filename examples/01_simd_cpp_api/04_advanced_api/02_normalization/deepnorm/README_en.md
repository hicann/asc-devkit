# DeepNorm Example

## Overview

This example uses the DeepNorm high-level API provided by Ascend C in a deep neural network training scenario to implement DeepNorm normalization. This API performs LayerNorm normalization while improving training stability of deep Transformer networks by scaling the residual connection (alpha coefficient).

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── deepnorm
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── deepnorm.asc            // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  This example implements DeepNorm normalization for input data with shape [B, S, H]. The computation formula is:
  $$
  DeepNorm(x) = LayerNorm(\alpha * X + SubLayer(X))
  $$

- Example specifications:  
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> deepnorm </td></tr>

  <tr><td rowspan="6" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">inputX</td><td align="center">[4, 16, 64]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">inputGx</td><td align="center">[4, 16, 64]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">beta</td><td align="center">[1, 64]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">gamma</td><td align="center">[1, 64]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="4" align="center">Example Output</td></tr>
  <tr><td align="center">output</td><td align="center">[4, 16, 64]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">outputMean</td><td align="center">[4, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">outputVariance</td><td align="center">[4, 16]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">deepnorm_custom</td></tr>
  </table>

- Example implementation:  

  - Kernel implementation

    The computation logic is:  
    Use the DeepNorm high-level API to complete the deepnorm computation, obtain the final result, and transfer it to external storage. For detailed API information, refer to DeepNorm.
    
  - Tiling implementation

    The tiling implementation process is as follows:
    1. Call AscendC::GetDeepNormMaxMinTmpSize to obtain and use the minimum temporary space size required by the DeepNorm API.
    2. Call AscendC::GetDeepNormTilingInfo to obtain the tiling parameters required by the kernel-side API based on the input shape and workspace size.

  - Invocation implementation  
    Use the kernel invocation operator <<<>>> to call the kernel function.

## Build and Run  

Run the following steps in the root directory of this example to build and run the example.
- Configure environment variables  
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.
    
- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;   # Create and enter the build directory
  cmake ..;make -j;             # Build the project (NPU mode by default)
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                        # Run the compiled executable to execute the example
  ```

  To use CPU debugging or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  For example:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debugging mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Before switching the build mode, clear the cmake cache by running `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU run, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
