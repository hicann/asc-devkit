# SoftmaxFlashV3 Example

## Overview

This example uses the SoftmaxFlashV3 high-level API in a large model training attention optimization scenario to implement the softmaxflashv3 single operator. This API is an enhanced version of SoftmaxFlash, corresponding to the Softmax PASA algorithm. It adds mean computation on top of V2 and performs numerical stability optimization through the alpha parameter. It is applicable to softmax computation scenarios that require higher numerical precision. This example uses half (input/output) and float (statistics) data types, with an input tensor shape of [8, 2048], to complete SoftmaxFlashV3 attention computation.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── softmaxflashv3
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── softmaxflashv3.asc      // Ascend C operator implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:
  Treat the input tensor [m0, m1, ..., mt, n] (t greater than or equal to 0) with the product of non-tail axis lengths m0, m1, ..., mt as m, so the input tensor shape is treated as [m, n]. Split the tail axis of the input tensor x into splitMeanCnt blocks, with the split tensor being x_cnti. Perform computation according to the following formulas, where x, inmax, insum, and inmean are inputs, and M, S, E, and A are all outputs.
  When update is false:

  $$
  A_1 = \text{rowmean}(x_{cnt})_i, i \in [0, \text{splitMeanCnt}]\\
  A_2 = \text{rowmean}(x_i), i \in [0, n]\\
  x_i = x_i - (A_2 - A_1) * (\alpha / (1 - \alpha))\\
  A = A_2\\
  M_1 = \text{rowmax}(x_i), i \in [0, n]\\
  M = M_1\\
  M_2 = M\\
  \text{SoftmaxFlashV3}(z_i) = \exp(x_i - M_2), i \in [0, n]\\
  S = \sum_{i}^{n} \exp(x_i - M_2)\\
  $$

  When update is true:

  $$
  A_1 = \text{rowmean}(x_{cnt})_i, i \in [0, \text{splitMeanCnt}]\\
  A_2 = \text{rowmean}(x_i), i \in [0, n]\\
  x_i = x_i - (A_2 - A_1) * (\alpha / (1 - \alpha))\\
  A = (A_2 + \text{inmean} * (\text{loopCnt} - 1)) / \text{loopCnt}\\
  M_1 = \text{rowmax}(x_i), i \in [0, n]\\
  C = (A_2 - A) * (\alpha / (1 - \alpha))\\
  P = (\text{inmean} - A) * (\alpha / (1 - \alpha))\\
  M = \max(C + M_1, P + \text{inmax})\\
  M_2 = M - C\\
  \text{SoftmaxFlashV3}(z_i) = \exp(x_i - M_2), i \in [0, n]\\
  E = \exp(\text{inmax}_i - M_2 + P)\\
  S = \sum_{i}^{n} \exp(x_i - M_2) + E * \text{insum}\\
  $$

- Example specifications:

<div align="left">
<table>
<caption>Table 1: Example Specifications</caption>
<tr><td align="center" rowspan="1">Example Type (OpType)</td><td align="center" colspan="4"> softmaxflashv3 </td></tr>

<tr><td align="center" rowspan="6">Example Input</td></tr>
<tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">src</td><td align="center"> [8, 2048] </td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td align="center">inMax</td><td align="center"> [8, 8] </td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">inSum</td><td align="center"> [8, 8] </td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">inMean</td><td align="center"> [8, 8] </td><td align="center">float</td><td align="center">ND</td></tr>

<tr><td align="center" rowspan="2">Example Output</td></tr>
<tr><td align="center">dst</td><td align="center"> [8, 2048] </td><td align="center">half</td><td align="center">ND</td></tr>

<tr><td align="center" rowspan="1">Kernel Function Name</td><td align="center" colspan="4">softmaxflashv3_custom</td></tr>
</table>
</div>

- Example implementation:  
  This example implements a softmaxflashv3 example with fixed shapes of input src[8, 2048], inMax[8, 8], inSum[8, 8], inMean[8, 8], and output dst[8, 2048].

  - Kernel implementation  
    Core computation steps: After transferring the input data in, call `AscendC::SoftmaxFlashV3` to complete the SoftmaxFlashV3 computation, then transfer the result out.

  - Tiling implementation  
    The tiling implementation process for the softmaxflashv3 example is as follows: First split the shape by row count for multi-core processing, using average allocation aligned upward by the number of cores to determine the computation row count for the main core, then determine the tail core computation row count. The kernel side computes the SoftMaxTiling parameters independently based on baseM.

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
