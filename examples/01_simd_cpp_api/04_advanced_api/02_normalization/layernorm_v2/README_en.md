# LayerNormV2 Example

## Overview

This example, based on the kernel direct invocation example project, demonstrates how to call the LayerNorm and Normalize high-level APIs consecutively within a single kernel function to perform row-wise normalization on the input tensor and compare the output results of the two APIs.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure
 
```
├── layernorm_v2
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── layernorm_v2.asc        // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  This example calls the LayerNorm and Normalize high-level APIs sequentially within a single kernel function. The two APIs work together to implement complete normalization computation. LayerNorm computes the mean and the reciprocal of the standard deviation (rstd), then computes the variance var based on rstd using the formula:
  $$
  var = 1/(rstd*rstd) - \epsilon
  $$
  Finally, Normalize uses the original input inputX from LayerNorm, the mean computed by LayerNorm, and the variance computed from rstd as inputs for normalization computation. The LayerNorm formula is:
  $$
  y_i = \gamma_i \cdot \frac{x_i - \mu}{\sqrt{var + \epsilon}} + \beta_i
  $$

- Example specifications:  
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> layernorm_v2 </td></tr>

  <tr><td rowspan="5" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">inputXGm</td><td align="center">[32, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">gammaGm</td><td align="center">[32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">betaGm</td><td align="center">[32]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="6" align="center">Example Output</td></tr>
  <tr><td align="center">outputGm</td><td align="center">[32, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">outputMeanGm</td><td align="center">[32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">outputRstdGm</td><td align="center">[32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">normalizeOutputGm</td><td align="center">[32, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">normalizeRstdGm</td><td align="center">[32]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">layernormv2_custom</td></tr>
  </table>

- Example implementation:  
  This example uses the **AR format** (shape is [A, R]) and calls the LayerNorm and Normalize high-level APIs sequentially within a single kernel function for normalization computation. A is the batch processing axis and R is the normalization axis. For detailed information, refer to the LayerNorm API documentation and Normalize API documentation.

  - Kernel implementation

    This example implements the complete data flow computation within a single kernel function:
    1. LayerNorm forward computation: Input inputX, gamma, beta; compute output y, mean, and reciprocal of the standard deviation rstd.
    2. Normalize computation: Use the same input to compute the normalization result and rstd.
    3. Result comparison: Compare the outputs of LayerNorm and Normalize to verify their mathematical equivalence.

  - Tiling implementation

    The tiling implementation process is as follows:
    1. Obtain and set the minimum temporary space size required by the LayerNorm and Normalize APIs respectively.
    2. Obtain the tiling parameters required by the kernel-side APIs of the two APIs based on the input shape, remaining available computation space, and other information.

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
  mkdir -p build && cd build;                              # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;     # Build the project (NPU mode by default)
  python3 ../scripts/gen_data.py                           # Generate test input data
  ./demo                                                   # Run the compiled executable to execute the example
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
