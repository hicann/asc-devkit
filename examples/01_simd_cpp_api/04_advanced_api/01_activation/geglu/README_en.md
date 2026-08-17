# GeGLU Example


## Overview

This example uses the GeGLU high-level API in a GLU (Gated Linear Unit) gated activation function scenario to implement GLU variant activation computation. GeGLU uses GELU as the activation function, with the computation pattern `dst = src0 * GELU(src1)`. This example uses float data type with an input tensor of 1024 elements to complete Geglu example computation.

> **API tip:** In addition to the `GeGLU` API used in this example, Ascend C also provides the following GLU variant APIs with the same invocation method as `GeGLU`. Simply replace the function name to switch:
> - **ReGlu**: A GLU variant that uses Relu as the activation function. Replace `AscendC::GeGLU` with `AscendC::ReGlu` in `geglu.asc`.


## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── geglu
│   ├── scripts
│   │   └── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── README.md               // Example documentation
│   └── geglu.asc               // Ascend C example implementation & invocation example
```

## Example Description

- Example function:
  This example performs GeGLU gated activation computation on two input tensors and writes the computation result to the output tensor.

  GeGLU is a GLU variant that uses GELU as the activation function.

  The computation formulas are as follows:
  $$dstTensor_i = srcTensor0_i \otimes GELU(srcTensor1_i)$$
  Where the GELU activation function formula is:
  $$GELU(x)=0.5 * x * (1 + tanh(\sqrt{\frac{2}{\pi}} * (x + 0.044715 * x^3)))$$
  After simplification:
  $$GELU(x)=\frac{x}{1 + e^{-1.59576912 * (x + 0.044715 * x^3)}}$$

- Example specifications:

<div align="left">
<table>
<tr><td align="center" rowspan="1">Example Type (OpType)</td><td align="center" colspan="4"> geglu </td></tr>

<tr><td align="center" rowspan="4">Example Input</td></tr>
<tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">src0</td><td align="center">[1,1024]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">src1</td><td align="center">[1,1024]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center" rowspan="2">Example Output</td></tr>
<tr><td align="center">dst</td><td align="center">[1,1024]</td><td align="center">float</td><td align="center">ND</td></tr>

<tr><td align="center" rowspan="1">Kernel Function Name</td><td align="center" colspan="4">geglu_custom</td></tr>
</table>
</div>

- Example implementation:  
  This example implements a geglu_custom example with fixed shapes of input src[1,1024], src1[1,1024], and output dst[1,1024].

  - Kernel implementation  
    Core computation steps: After transferring the input data in, call `AscendC::GeGLU` to complete the GeGLU gated activation computation, then transfer the result out.

  - Tiling implementation  
    This example is a single-core element-wise computation scenario with no complex multi-core logic. The host side obtains the temporary buffer size required by the API through `AscendC::GetGeGLUMaxMinTmpSize` and passes it directly to the kernel.

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
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU mode by default
  python3 ../scripts/gen_data.py
  ./demo
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

- Execution result

  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
