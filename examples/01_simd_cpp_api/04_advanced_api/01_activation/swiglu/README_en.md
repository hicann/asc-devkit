# SwiGLU Example

## Overview

This example uses the SwiGLU high-level API in large language model and Mixture of Experts (MoE) scenarios to perform element-wise SwiGLU computation on two input tensors. SwiGLU is a GLU variant that uses Swish as the activation function, with the computation formula dst_i = src0_i * Swish(src1_i), where Swish(x) = x/(1+e^(-beta*x)). This API is commonly used in gated feed-forward networks (FFN) in LLMs and supports float/half/bfloat16_t and other data types. This example uses float data type with an input tensor of 32 elements, a beta value of 1, to complete SwiGLU activation computation.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── swiglu
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file (supports -DCMAKE_ASC_RUN_MODE, -DCMAKE_ASC_ARCHITECTURES)
│   ├── data_utils.h            // Data read/write functions
│   ├── swiglu.asc              // Ascend C example implementation & invocation (with tiling mechanism)
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  SwiGLU is a GLU variant that uses Swish as the activation function.

  The computation formulas are as follows:
  $$dstTensor_i=(srcTensor0_i)\bigotimes Swish(srcTensor1_i)$$
  Where the Swish activation function formula is (beta is a constant):
  $$Swish(x)=x/(1 + e^{(-\beta x)})$$

- Example specifications:  
  <table border="2" align="left">
  <caption>Table 1: Example Specifications</caption>
  <tr><td align="center" rowspan="1">Example Type</td><td align="center" colspan="4"> swiglu </td></tr>

  <tr><td align="center" rowspan="4">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src0</td><td align="center">[1, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">src1</td><td align="center">[1, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center" rowspan="2">Example Output</td></tr>
  <tr><td align="center">dst</td><td align="center">[1, 32]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td align="center" rowspan="1">Kernel Function Name</td><td align="center" colspan="4">swiglu_custom</td></tr>
  </table>

- Example implementation:  
  This example implements a fixed-shape example with 32 input elements. The Tiling mechanism passes computation parameters (dataLength, sharedTmpBufferSize) from the host side to the device side, supporting flexible configuration of computation scale.

  - Kernel implementation  
    Core computation steps: After transferring input data from GM to UB, call `AscendC::SwiGLU` to complete the SwiGLU computation, then transfer the result back to Global Memory.

  - Tiling implementation  
    Temporary space handling: Obtain the required temporary space size through `AscendC::GetSwiGLUMaxMinTmpSize`, passed from Tiling to the kernel side. When the temporary space is greater than 0, use the buffer provided by the developer; otherwise, the framework automatically allocates it.

  - Invocation implementation  
    Use the kernel invocation operator `<<<>>>` to call the kernel function, passing src0, src1, dst, workspace, and tiling parameters.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  **Default mode (dav-2201 architecture)**:
  ```bash
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j;
  python3 ../scripts/gen_data.py
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
  <caption>Table 2: Build Option Description</caption>
  <tr><td align="center">Option</td><td align="center">Values</td><td align="center">Description</td></tr>
  <tr><td align="center">CMAKE_ASC_RUN_MODE</td><td align="center">npu (default), cpu, sim</td><td align="center">Run mode: NPU run, CPU debug, NPU simulation</td></tr>
  <tr><td align="center">CMAKE_ASC_ARCHITECTURES</td><td align="center">dav-2201 (default), dav-3510</td><td align="center">NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; dav-3510 corresponds to Ascend 950PR/Ascend 950DT</td></tr>
  </table>
  </div>

- Execution result

  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
