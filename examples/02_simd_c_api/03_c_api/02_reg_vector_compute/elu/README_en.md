# ELU Example

## Overview

This example implements the ELU (Exponential Linear Unit) activation operator using the C API, mainly calling the `asc_exp`, `asc_mul_scalar` and `asc_select` APIs.

## Supported Products and CANN Versions

| Product | CANN Version |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.0.0 |

## Directory Structure

```text
├── elu
│   ├── scripts
│   │   ├── gen_data.py                // Input data and ground truth generation script
│   ├── CMakeLists.txt                 // Build configuration file
│   ├── data_utils.h                   // Data read/write functions
│   ├── elu.asc                        // AscendC example implementation & invocation example
│   ├── README.md                      // Example description
│   └── README_en.md                   // Example description (English)
```

## Example Description

- Example functionality:  
  Computes the element-wise ELU activation of the input. The formula is:
  $$
  ELU(x)=
  \begin{cases}
  scale \cdot x, & x > 0 \\
  \alpha \cdot scale \cdot (e^{x \cdot inputScale} - 1), & x \le 0
  \end{cases}
  $$

- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Sample Type (OpType)</td><td colspan="3" align="center">AIV Sample</td></tr>
  <tr><td rowspan="2" align="center">Sample Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">dst</td><td align="center">same as src</td><td align="center">float / float16</td></tr>
  <tr><td rowspan="5" align="center">Sample Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">src</td><td align="center">any valid shape</td><td align="center">float / float16</td></tr>
  <tr><td align="center">alpha</td><td align="center">scalar</td><td align="center">float</td></tr>
  <tr><td align="center">scale</td><td align="center">scalar</td><td align="center">float</td></tr>
  <tr><td align="center">input_scale</td><td align="center">scalar</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function</td><td colspan="3" align="center">elu_custom (fp32) / elu_custom_half (fp16)</td></tr>
  </table>

- Example implementation:
  It uses segment-based dynamic tiling to copy the input into UB in segments, computes the ELU in the register domain (`vector_float`), then copies it out to GM; fp16 is computed in the fp32 domain and written back as half.
  - Invocation
    The kernel is invoked with the `<<<>>>` kernel invocation operator.

## Build and Run

Execute the following steps in the example root directory.

- Configure the environment variables  
  Configure the environment variables according to the [installation](../../../../../docs/en/quick_start.md#prepare&install) of the CANN toolkit on your environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN installation directory; if not specified, it defaults to `/usr/local/Ascend`.

- Build and run

  Run the following commands in the example directory.

  ```bash
  mkdir -p build && cd build;                                                    # create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                           # build the project (npu mode by default)
  python3 ../scripts/gen_data.py                                                 # generate test input data
  ./demo                                                                         # run the compiled executable
  ```

  To use NPU simulation mode, add `-DCMAKE_ASC_RUN_MODE=sim`:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Note:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory, then re-run cmake.

- Sample execution output  
  The output below indicates that the precision comparison passed.

  ```bash
  [Success] File mode verification passed.
  ```

  Built-in comprehensive self-test (validates correctness across various shapes):

  ```bash
  ./demo --selftest
  ```

  It outputs `[Success] All cases verification passed.` and returns exit code 0.
