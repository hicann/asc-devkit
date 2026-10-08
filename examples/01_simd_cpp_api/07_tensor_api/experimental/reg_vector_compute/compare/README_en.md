# Compare Sample

## Overview

This sample uses the Tensor API `operator>` interface to compare data in multiple scenarios.

The sample supports the following two comparison scenarios, selected by the CMake build option `SCENARIO_NUM`.

| SCENARIO_NUM | Comparison Scenario |
| --- | --- |
| 1 | The right operand is a `reg_tensor`: compares one vector with another vector element by element. |
| 2 | The right operand is an immediate: compares a vector with a scalar element by element. |

## Supported Products

- Ascend 950PR&950DT products

## Directory Structure

```text
compare
├── scripts
│   └── gen_data.py         // Script for generating input and golden data
├── CMakeLists.txt          // Build configuration
├── data_utils.h            // Data input and output utilities
└── compare.asc             // Ascend C operator implementation and invocation sample
```

## Sample Description

Comparison interfaces such as `operator>` are generally used together with the `Select` interface. This sample demonstrates the combined use of `operator>` and `Select`.

Use the build option `SCENARIO_NUM` to select one of the following scenarios.

**Scenario 1: The right operand is a `reg_tensor`**

- Sample Function:

  Obtains the greater value at each position of two equal-sized vector data registers, `src0_reg` and `src1_reg`.

- Sample Specifications:

  <table>
  <tr><td rowspan="1" align="center">Sample Type (OpType)</td><td colspan="3" align="center">AIV Sample</td></tr>
  <tr><td rowspan="3" align="center">Inputs</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Output</td><td align="center">z</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function</td><td colspan="4" align="center">compare</td></tr>
  </table>

- Sample Implementation:

  - Call `operator>` to compare the two vector data registers. The result is returned in the Boolean vector register `condition`. If an element of `src0_reg` is greater than the corresponding element of `src1_reg`, the corresponding bit is set to 1; otherwise, it is set to 0.
  - Call the `Select` interface with the comparison result `mask_reg`. If the corresponding bit in `mask_reg` is 1, the element from `src0_reg` is selected; otherwise, the element from `src1_reg` is selected.
  - For the `float` data type, each mask occupies four bits. `Compare` sequentially reads data from `src0_reg` and `src1_reg`, compares the elements, and writes each result to bit position `4 * N` in `mask_reg`. `Select` uses bit position `4 * N` in `mask_reg` to select data from either `src0_reg` or `src1_reg`.
  - Invoke the kernel function using the `<<<>>>` kernel launch syntax.

**Scenario 2: The right operand is an immediate**

- Sample Function:

  Compares each element of the vector `src0_reg` with the scalar `0`. If `src0_reg[i]` is greater than `0`, `dst_reg[i]` is assigned `src0_reg[i]`; otherwise, it is assigned `src1_reg[i]`.

- Sample Specifications:

  <table>
  <tr><td rowspan="1" align="center">Sample Type (OpType)</td><td colspan="3" align="center">AIV Sample</td></tr>
  <tr><td rowspan="3" align="center">Inputs</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Output</td><td align="center">z</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function</td><td colspan="4" align="center">compare</td></tr>
  </table>

- Sample Implementation:

  - Call the `Compares` interface in GT (greater than) mode to compare the `src0_reg` vector with the scalar `0` and write the result to `mask_reg`. If an element of `src0_reg` is greater than `0`, the corresponding bit in `mask_reg` is set to 1; otherwise, it is set to 0.
  - Call the `Select` interface with the comparison result `mask_reg`. If the corresponding bit in `mask_reg` is 1, the element from `src0_reg` is selected; otherwise, the element from `src1_reg` is selected.
  - For the `float` data type, each mask in `mask_reg` occupies four bits. `Compare` sequentially reads data from `src0_reg` and `src1_reg`, compares the elements, and writes each result to bit position `4 * N` in `mask_reg`. `Select` uses bit position `4 * N` in `mask_reg` to select data from either `src0_reg` or `src1_reg`.
  - Invoke the kernel function using the `<<<>>>` kernel launch syntax.

## Build and Run

Perform the following steps in the sample root directory to build and run the sample.

- Configure Environment Variables

  Configure the environment variables according to the CANN development kit [installation instructions](../../../../../../docs/en/quick_start.md#prepare&install). **Currently, only [CANN master](../../../../../../docs/en/quick_start.md#cann-install) is supported.**

  > **Note:** `${install_path}` is the CANN package installation directory. If no installation directory is specified, the default directory is `/usr/local/Ascend`.

- Run the Sample

  Run the following commands in the sample directory. Scenario 1 is used as an example:

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build
  cmake -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_ARCHITECTURES=dav-3510  -DCANN_ASC_USE_EXPERIMENTAL=ON..
  make -j
  python3 ../scripts/gen_data.py -scenarioNum=${SCENARIO_NUM}
  ./demo
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` option.

  Example:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..;make -j; # NPU simulation mode
  ```

  > **Note:** Clear the CMake cache before switching build modes. Run `rm CMakeCache.txt` in the `build` directory, and then run CMake again.

- Build Options

  | Option | Description |
  | --- | --- |
  | `CMAKE_ASC_RUN_MODE` | Operator execution mode. Available values are `npu` and `sim`. The default is `npu`. |
  | `CMAKE_ASC_ARCHITECTURES` | NPU chip model. The default is `dav-3510`. |
  | `CANN_ASC_USE_EXPERIMENTAL` | Experimental ASC API switch. This sample requires `ON`; the default is `OFF`. |
  | `SCENARIO_NUM` | Required comparison scenario number. The valid range is 1 to 2. |

- Execution Result

  The following output indicates that the accuracy comparison is successful:

  ```text
  test pass!
  ```
