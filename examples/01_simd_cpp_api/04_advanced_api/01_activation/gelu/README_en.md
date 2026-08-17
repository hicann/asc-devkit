# Gelu Example

## Overview

This example uses the Gelu high-level API in an activation function scenario to perform element-wise GELU (Gaussian Error Linear Unit) computation on the input tensor. This example uses float data type with an input tensor of 32 elements to complete Gelu example computation.

> **Advanced API tip:** In addition to the `Gelu` API used in this example, Ascend C also provides the following advanced GELU APIs with the same invocation method as `Gelu`. Simply replace the function name to switch:
> - **FasterGelu**: An accelerated version of GELU, applicable to scenarios with higher performance requirements. Replace `AscendC::Gelu` with `AscendC::FasterGelu` in `gelu.asc`.
> - **FasterGeluV2**: A further optimized version of GELU that reduces computing power requirements. Replace `AscendC::Gelu` with `AscendC::FasterGeluV2` in `gelu.asc`.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── gelu
│   ├── scripts
│   │   └── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── README.md               // Example documentation
│   └── gelu.asc                // Ascend C operator implementation & invocation example
```

## Example Description

- Example function:  
  This example performs element-wise GELU activation computation on the input tensor and writes the computation result to the output tensor.

  The computation formulas are as follows:
  $$dstLocal_i = GELU(srcLocal_i)$$
  $$GELU(x)=0.5 * x * (1 + tanh(\sqrt{\frac{2}{\pi}} * (x + 0.044715 * x^3)))$$
  $$GELU(x)=\frac{x}{1 + e^{-1.59576912 * (x + 0.044715 * x^3)}}$$

- Example specifications:

<div align="left">
<table>
<tr><td align="center" rowspan="1">Example Type (OpType)</td><td align="center" colspan="4"> gelu </td></tr>

<tr><td align="center" rowspan="3">Example Input</td></tr>
<tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">src</td><td align="center">[1, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center" rowspan="2">Example Output</td></tr>
<tr><td align="center">dst</td><td align="center">[1, 32]</td><td align="center">float</td><td align="center">ND</td></tr>

<tr><td align="center" rowspan="1">Kernel Function Name</td><td align="center" colspan="4">gelu_custom</td></tr>
</table>
</div>

- Example implementation:  
  This example implements a gelu_custom example with fixed shapes of input src[1, 32] and output dst[1, 32].

  - Kernel implementation  
    Core computation steps: After transferring the input data in, call `AscendC::Gelu` to complete the GELU computation, then transfer the result out.

  - Tiling implementation  
    This example is a single-core element-wise computation scenario with no complex multi-core logic. The host side obtains the temporary buffer size required by the API through `AscendC::GetGeluMinTmpSize` and passes it directly to the kernel.

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
