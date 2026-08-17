# LayerNorm Example

## Overview

This example describes how to call the LayerNorm, LayerNormGrad, and LayerNormGradBeta high-level APIs consecutively within a single kernel function to implement the complete forward and backward propagation of LayerNorm.

> **Similar API tip:** Ascend C also provides `BatchNorm` and `GroupNorm` with the same invocation method as `LayerNorm`.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── layernorm
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── layernorm.asc           // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  LayerNorm example: Call the LayerNorm, LayerNormGrad, and LayerNormGradBeta high-level APIs consecutively within a single kernel function to complete the full forward and backward propagation computation of LayerNorm. In training scenarios, this example can compute the forward output, input gradient, and parameter gradient in a single pass, improving computation efficiency.

- Example specifications:  
  <table border="2" align="center">
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> layernorm </td></tr>
  <tr><td rowspan="6" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">inputXGm</td><td align="center">[2, 32, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">gammaGm</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">betaGm</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">dyGm</td><td align="center">[2, 32, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="7" align="center">Example Output</td></tr>
  <tr><td align="center">outputYGm</td><td align="center">[2, 32, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">outputMeanGm</td><td align="center">[2, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">outputVarGm</td><td align="center">[2, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">outputPdXGm</td><td align="center">[2, 32, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">outputPdGammaGm</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">outputPdBetaGm</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">layernorm_custom</td></tr>
  </table>

- Example implementation:  
  This example implements a fused LayerNorm example with fixed shapes (inputX[2, 32, 16], gamma[1, 16], beta[1, 16], dy[2, 32, 16]).

  - Kernel implementation

    This example implements the complete data flow computation within a single kernel function:
    1. LayerNorm forward computation: Input inputX, gamma, beta; compute output y, mean, and variance var.
    2. LayerNormGrad backward computation: Input dy, inputX, var, mean, gamma; compute input gradient pd_x and intermediate result resForGamma.
    3. LayerNormGradBeta parameter gradient computation: Input dy, resForGamma; compute gradients pd_gamma and pd_beta for gamma and beta.

  - Tiling implementation

    The tiling implementation process for the layernorm example is as follows:
    1. Obtain and set the minimum temporary space size required by the LayerNorm, LayerNormGrad, and LayerNormGradBeta APIs respectively.
    2. Obtain the tiling parameters required by the kernel-side APIs of the three APIs based on the input shape, remaining available computation space, and other information.

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
