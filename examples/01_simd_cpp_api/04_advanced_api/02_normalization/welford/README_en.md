# Welford Example

## Overview

This example, based on the kernel direct invocation example project, demonstrates how to call the WelfordUpdate and WelfordFinalize high-level APIs consecutively within a single kernel function to implement the complete Welford online algorithm. Welford is an online method for computing mean and variance that can incrementally calculate mean and variance in a single data pass, reducing memory access and improving computation performance.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── welford
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── welford.asc             // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  This example calls the WelfordUpdate and WelfordFinalize high-level APIs sequentially within a single kernel function to implement the complete Welford online algorithm. Advantages of the Welford algorithm:
  - Incrementally compute mean and variance of all samples without storing all samples
  - Only requires a single pass over the data, reducing memory access and improving computation performance
  - WelfordUpdate: Online update of mean and variance, with formulas $M_t = M_{t-1} + (x_t - M_{t-1}) / n$, $S_t = S_{t-1} + (x_t - M_{t-1}) \times (x_t - M_t)$
  - WelfordFinalize: Aggregate results from multiple blocks to obtain the final mean and variance, with formulas $Mean = \frac{\sum(M_i \cdot n_i)}{\sum n_i}$, $Var = \frac{\sum(S_i + (Mean_i - Mean)^2 \cdot n_i)}{\sum n_i}$

- Example specifications:  
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> welford </td></tr>

  <tr><td rowspan="5" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">srcGm</td><td align="center">[1, 64]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">inMeanGm</td><td align="center">[1, 64]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">inVarGm</td><td align="center">[1, 64]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="5" align="center">Example Output</td></tr>
  <tr><td align="center">outMeanGm</td><td align="center">[1, 64]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">outVarGm</td><td align="center">[1, 64]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">finalMeanGm</td><td align="center">[8]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">finalVarGm</td><td align="center">[8]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">welford_coop_custom</td></tr>
  </table>

- Example implementation:  
  This example implements a fused Welford example with fixed shapes (RN=1, AB=64).

  This example calls two APIs within a single kernel function:
    1. WelfordUpdate: Compute local mean and variance for each data block
    2. WelfordFinalize: Aggregate mean and variance from all blocks to obtain global mean and variance

  The Welford algorithm formulas are described in the example function section. For detailed information, refer to the WelfordUpdate API documentation and WelfordFinalize API documentation.

  - Kernel implementation

    The computation logic is:  
    Call the WelfordUpdate and WelfordFinalize high-level APIs sequentially within a single kernel function to implement the complete Welford online algorithm, and transfer the computation results to external storage.

  - Tiling implementation

    The tiling implementation process is as follows:
    1. Call AscendC::GetWelfordUpdateMaxMinTmpSize to obtain the minimum temporary space size required by the WelfordUpdate API, and use the minimum value as the workspace size to ensure correct functionality.
    2. Use the CopyTilingData function to copy tiling data from GM to the kernel side.

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
