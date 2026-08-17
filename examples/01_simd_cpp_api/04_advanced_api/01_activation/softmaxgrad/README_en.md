# SoftmaxGrad Example

## Overview

This example uses the `SoftmaxGrad` or `SoftmaxGradFront` high-level APIs in a neural network backpropagation scenario to implement softmax gradient computation.

- **SoftmaxGrad**: Perform complete softmax gradient backward computation on the input tensor `[m, n]` row by row, with the computation formula `(grad - sum(grad * src)) * src`.
- **SoftmaxGradFront**: Compute only the first half of the softmax gradient, with the computation formula `sum(grad * src)`, outputting the sum result for each row. This API is commonly used in scenarios such as FlashAttention that require intermediate gradient results.

Relationship between the two APIs: `SoftmaxGradFront` is a subset of `SoftmaxGrad`. When `SoftmaxGrad(isFront=true)`, it is recommended to use `SoftmaxGradFront`. This example controls the two modes through the compilation macro `USE_FRONT_MODE`, uses float data type, and both input x and y have shapes of `[960, 960]`.

> **Notice:** The last axis length of the input tensor must satisfy 32-byte alignment (a multiple of 8 for float type). In `SoftmaxGradFront` mode, the last axis of the output tensor is fixed at 1 datablock (8 elements for float type), and all element values are identical.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── softmaxgrad
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data (supports both modes)
│   ├── CMakeLists.txt          // Build project file (supports -DUSE_FRONT_MODE)
│   ├── data_utils.h            // Data read/write functions
│   ├── softmaxgrad.asc         // Ascend C example implementation & invocation example (two modes merged)
│   └── README.md               // Example documentation
```

## Example Specifications

<div align="left">
<table>
<caption>Table 1: SoftmaxGrad Mode Example Specifications</caption>
<tr><td align="center">Name</td><td align="center">Shape</td><td align="center">Data Type</td><td align="center">Format</td></tr>
<tr><td align="center">Input x</td><td align="center">[960, 960]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">Input y</td><td align="center">[960, 960]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">Output z</td><td align="center">[960, 960]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">Kernel Function Name</td><td align="center" colspan="3">softmaxgrad_custom</td></tr>
</table>
</div>

- Example implementation:  
This example implements a fixed-shape example with input x `[960, 960]` and y `[960, 960]`, computed across multiple cores.

  - Tiling implementation  
    Split by row count, using average allocation aligned upward by the number of cores to determine the computation row count for the main core and tail core. Determine the number of rows processed per loop iteration by querying SLICE_TABLE based on the reduce axis length. Call `GetSoftMaxGradMinTmpSize` and `SoftMaxGradTilingFunc` to obtain the tiling parameters required by the API.

  - Kernel implementation  
    Core computation steps: After transferring the input data in, call the corresponding SoftmaxGrad/SoftmaxGradFront API to complete the gradient computation, then transfer the result out.

    SoftmaxGrad mode invocation:
    ```cpp
    AscendC::SoftmaxGrad<float, true>(yLocal, xLocal, yLocal, tmpBuffer, softmaxTiling, false, srcShape);
    ```

    SoftmaxGradFront mode invocation:
    ```cpp
    AscendC::SoftmaxGradFront<float>(zLocal, xLocal, yLocal, tmpBuffer, softmaxTiling, srcShape);
    ```

- Invocation implementation  
  Use the kernel invocation operator `<<<>>>` to call the kernel function.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

   **SoftmaxGrad mode (default)**:
   ```bash
   mkdir -p build && cd build;
   cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j;
   python3 ../scripts/gen_data.py
   ./demo
   ```

   **SoftmaxGradFront mode**:
   ```bash
   mkdir -p build && cd build;
   cmake -DUSE_FRONT_MODE=ON -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j;
   python3 ../scripts/gen_data.py --front-mode
   ./demo
   ```

  To use CPU debugging or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  For example:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j; # CPU debugging mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j; # NPU simulation mode
  ```

  > **Notice:** Before switching the build mode, clear the cmake cache by running `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

   <div align="left">
   <table>
   <caption>Table 3: Build Option Description</caption>
   <tr><td align="center">Option</td><td align="center">Values</td><td align="center">Description</td></tr>
   <tr><td align="center">CMAKE_ASC_RUN_MODE</td><td align="center">npu (default), cpu, sim</td><td align="center">Run mode: NPU run, CPU debug, NPU simulation</td></tr>
   <tr><td align="center">CMAKE_ASC_ARCHITECTURES</td><td align="center">dav-2201 (default), dav-3510</td><td align="center">NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; dav-3510 corresponds to Ascend 950PR/Ascend 950DT</td></tr>
   <tr><td align="center">USE_FRONT_MODE</td><td align="center">OFF (default), ON</td><td align="center">Example mode: OFF for SoftmaxGrad, ON for SoftmaxGradFront</td></tr>
   </table>
   </div>

  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
