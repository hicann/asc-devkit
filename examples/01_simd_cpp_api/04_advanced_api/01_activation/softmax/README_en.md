# Softmax Example

## Overview

This example uses the SoftMax high-level API in an activation function scenario to perform row-wise SoftMax computation on the input tensor, and uses AdjustSoftMaxRes for post-processing of the computation result. SoftMax performs reducemax, sub, exp, reducesum, and div steps row-wise on the input tensor [m, n] to obtain a normalized probability distribution, commonly used in attention mechanisms and the output layer of classification tasks.

> **API tip:** In addition to the `SoftMax` API used in this example, Ascend C also provides the `SimpleSoftMax` API, which performs SoftMax computation on the input tensor using pre-computed sum and max data, with no internal reduce process. Replace `AscendC::SoftMax` with `AscendC::SimpleSoftMax` in `softmax.asc` to switch.

AdjustSoftMaxRes is used for post-processing of SoftMax computation results. When specified values exist in the input max, it adjusts the corresponding softmaxres results row-wise to custom values. This example uses float data type with an input tensor shape of [32, 32] to complete SoftMax computation and AdjustSoftMaxRes post-processing.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── softmax
│   ├── scripts
│   │   └── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── README.md               // Example documentation
│   └── softmax.asc             // Ascend C example implementation & invocation example
```

## Example Description

- Example function:

  This example performs row-wise SoftMax computation on the input tensor and uses AdjustSoftMaxRes for post-processing of the computation result. When specified values (0xFF7FFFFF, that is, the maximum finite value of float type) exist in the input max, it adjusts the data at the corresponding positions in the output to custom values (0.0, that is, floating-point zero). This mechanism is commonly used in attention mask scenarios to set the softmax output of invalid positions to zero.

- Example specifications:

<div align="left">
<table>
<caption>Table 1: Example Specifications</caption>
<tr><td align="center" rowspan="1">Example Type (OpType)</td><td align="center" colspan="4">softmax</td></tr>

<tr><td align="center" rowspan="3">Example Input</td></tr>
<tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[32, 32]</td><td align="center">float</td><td align="center">ND</td></tr>

<tr><td align="center" rowspan="4">Example Output</td></tr>
<tr><td align="center">y</td><td align="center">[32, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">max</td><td align="center">[32, 8]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">sum</td><td align="center">[32, 8]</td><td align="center">float</td><td align="center">ND</td></tr>

<tr><td align="center" rowspan="1">Kernel Function Name</td><td align="center" colspan="4">softmax_custom</td></tr>
</table>
</div>

- Example implementation:  
  This example implements a softmax example with fixed shapes of input x[32, 32] and output y[32, 32], max[32, 8], sum[32, 8].

  - Kernel implementation  
    Core computation steps: After transferring the input data in, call `AscendC::SoftMax` to complete the SoftMax computation, then call `AscendC::AdjustSoftMaxRes` for post-processing of the result, and finally transfer the result out.

  - Tiling implementation  
    The tiling implementation process for the softmax example is as follows: First split the shape by row count for multi-core processing, using average allocation aligned upward by the number of cores to determine the computation row count for the main core, then determine the tail core computation row count. For the main core computation shape, call GetSoftMaxMinTmpSize to obtain the temporary buffer size required by the API, then call the tiling function of the SoftMax high-level API to obtain the tiling parameters required by the API. The tiling required by the high-level API for tail core computation is calculated by the kernel side independently.

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
