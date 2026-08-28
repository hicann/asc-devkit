# Compare Tensor API Sample

## Overview

This sample demonstrates how to use the Tensor API `operator>` together with `asc::te::experimental::select` to perform element-wise comparisons in multiple scenarios.

Use the CMake option `SCENARIO_NUM` to select one of the following scenarios:

| SCENARIO_NUM | Comparison Scenario |
| --- | --- |
| 1 | Compares each element of one `reg_tensor` with the corresponding element of another `reg_tensor`. |
| 2 | Compares each element of a `reg_tensor` with a scalar immediate value. |

## Supported Products

- Ascend 950PR/Ascend 950DT

## Directory Structure

```text
compare_reg_api
├── scripts
│   └── gen_data.py         // Generates input and golden data.
├── CMakeLists.txt          // Build configuration.
├── data_utils.h            // Data input and output utilities.
├── README.md               // Chinese sample description.
├── README_En.md            // English sample description.
└── compare_reg_api.asc     // Ascend C operator implementation and invocation sample.
```

## Sample Description

Comparison operators such as `operator>` are commonly used with `select`. This sample demonstrates how to combine `operator>` and `select`. The `SCENARIO_NUM` build option selects the scenario to compile.

### Scenario 1: Register Tensor as the Right Operand

- Sample Function

  The sample compares two register tensors of the same size element by element and writes the larger value to the output:

  ```text
  z[i] = x[i] > y[i] ? x[i] : y[i]
  ```

- Sample Specification

  <table>
    <tr><td align="center">Sample Type (OpType)</td><td colspan="3" align="center">AIV Sample</td></tr>
    <tr><td rowspan="3" align="center">Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
    <tr><td align="center">y</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
    <tr><td align="center">Output</td><td align="center">z</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
    <tr><td align="center">Kernel Function Name</td><td colspan="3" align="center">compare</td></tr>
  </table>

- Sample Implementation

  1. Load `x` and `y` into the `xReg` and `yReg` register tensors.
  2. Evaluate `xReg > yReg` element by element. The comparison returns the boolean register tensor `maskReg`; the corresponding mask bit is 1 when an element of `xReg` is greater than the matching element of `yReg`, and 0 otherwise.
  3. Call `select(maskReg, xReg, yReg)`. An enabled mask element selects the value from `xReg`; otherwise, the value from `yReg` is selected.
  4. Invoke the kernel function with the `<<<>>>` kernel launch syntax.

### Scenario 2: Scalar as the Right Operand

- Sample Function

  The sample compares each element of `xReg` with the scalar value 0. Positive elements are selected from `xReg`; otherwise, the corresponding elements are selected from `yReg`:

  ```text
  z[i] = x[i] > 0 ? x[i] : y[i]
  ```

- Sample Specification

  <table>
    <tr><td align="center">Sample Type (OpType)</td><td colspan="3" align="center">AIV Sample</td></tr>
    <tr><td rowspan="3" align="center">Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
    <tr><td align="center">y</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
    <tr><td align="center">Output</td><td align="center">z</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
    <tr><td align="center">Kernel Function Name</td><td colspan="3" align="center">compare</td></tr>
  </table>

- Sample Implementation

  1. Load `x` and `y` into the `xReg` and `yReg` register tensors.
  2. Evaluate `xReg > scalar` element by element, where `scalar` is 0. The comparison returns `maskReg`; the corresponding mask bit is 1 when an element of `xReg` is greater than 0, and 0 otherwise.
  3. Call `select(maskReg, xReg, yReg)`. An enabled mask element selects the value from `xReg`; otherwise, the corresponding value from `yReg` is selected.
  4. Invoke the kernel function with the `<<<>>>` kernel launch syntax.

For `float` data, one valid-element mask is stored every four bits in the mask register. The comparison writes the result for element `N` to bit position `4 * N`, and `select` reads the same position to choose between `xReg` and `yReg`.

## Build and Run

Run the following steps in the sample root directory to build and execute the operator.

- Configure Environment Variables

  Configure the environment variables according to the installation method of the CANN development toolkit in the current environment. For details, see the [Quick Start Guide](../../../../../docs/zh/quick_start.md).

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

- Run the Sample

  The following commands use scenario 1 as an example:

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build
  cmake -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..
  make -j
  python3 ../scripts/gen_data.py -scenarioNum=${SCENARIO_NUM}
  ./demo
  ```

  The `scenarioNum` passed to the data generation script must match the `SCENARIO_NUM` used during compilation.

  To run the sample in NPU simulation mode, add `-DCMAKE_ASC_RUN_MODE=sim`:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..
  make -j
  ```

  When switching the run mode or scenario, clear the CMake cache in the `build` directory before reconfiguring the project.

- Build Options

  | Option | Available Values | Description |
  | --- | --- | --- |
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Operator execution mode: NPU execution or NPU simulation. |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture. `dav-3510` corresponds to Ascend 950PR/Ascend 950DT. |
  | `CANN_ASC_USE_EXPERIMENTAL` | `ON` (required), `OFF` (default) | Enables experimental ASC APIs. |
  | `SCENARIO_NUM` | `1`, `2` | Selects the comparison scenario. |

- Expected Result

  The following output indicates that the result matches the golden data:

  ```text
  test pass!
  ```
