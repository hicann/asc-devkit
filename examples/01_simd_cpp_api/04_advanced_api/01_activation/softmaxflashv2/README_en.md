# SoftmaxFlashV2 Example

## Overview

This example uses the SoftmaxFlashV2 high-level API in a large language model attention mechanism scenario to implement the softmaxflashv2 single operator. This API is an enhanced version of SoftmaxFlash, corresponding to the FlashAttention-2 algorithm. It supports update mode (incremental computation) and is commonly used in attention mechanism computation and long sequence chunked processing scenarios in large language models. This example uses float data type with an input tensor shape of [960, 960] to complete SoftmaxFlashV2 attention computation.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── softmaxflashv2
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── softmaxflashv2.asc      // Ascend C operator implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:
  softmaxflashv2 single operator: Treat the product of non-tail axis lengths of the input tensor [m0, m1, ...mt, n] (t greater than or equal to 0) as m, so the input tensor shape is treated as [m, n]. Perform the following row-wise computation on the input tensor [m, n]. Different update values correspond to different computation formulas, where x, inmax, and insum are inputs, and M, S, and E are all outputs.
  When update is false:

  $$M = rowmax(x_i)$$

  $$SoftmaxFlashV2(z_i) = exp(x_i - M)$$

  $$S=\sum_{i}^{n} \exp(x_i - M)$$

  When update is true:

  $$M = max(rowmax(x_i), inmax)$$

  $$SoftmaxFlashV2(z_i) = exp(x_i - M)$$

  $$E = exp(inmax_i - M)$$

  $$S = sum_{i}^{n} exp(x_i - M) + E \cdot insum$$

- Example specifications:

<div align="left">
<table>
<caption>Table 1: Example Specifications</caption>
<tr><td align="center" rowspan="1">Example Type (OpType)</td><td align="center" colspan="4">Softmaxflash</td></tr>

<tr><td align="center" rowspan="3">Example Input</td></tr>
<tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center"> [960, 960] </td><td align="center">float</td><td align="center">ND</td></tr>

<tr><td align="center" rowspan="3">Example Output</td></tr>
<tr><td align="center">max</td><td align="center"> [960, 8] </td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">sum</td><td align="center"> [960, 8] </td><td align="center">float</td><td align="center">ND</td></tr>

<tr><td align="center" rowspan="1">Kernel Function Name</td><td align="center" colspan="4">softmaxflashv2_custom</td></tr>
</table>
</div>

- Example implementation:  
  This example implements a softmaxflashv2 example with fixed shapes of input x [960, 960] and output max[960, 8], sum[960, 8].

  - Kernel implementation  
    Core computation steps: After transferring the input data in, call `AscendC::SoftmaxFlashV2(xLocal, sumLocal, maxLocal, xLocal, expmaxLocal, sharedTmpBuffer, softmaxTiling)` to complete the SoftmaxFlashV2 computation, then transfer the result out.

  - Tiling implementation  
    The tiling implementation process for the softmaxflashv2 example is as follows: First split the shape by row count for multi-core processing, using average allocation aligned upward by the number of cores to determine the computation row count for the main core, then determine the tail core computation row count. For the main core computation shape, call the tiling function of the SoftmaxFlashV2 high-level API to obtain the tiling parameters required by the API. The tiling required by the high-level API for tail core computation is calculated by the kernel side independently.

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
