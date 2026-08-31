# Cast Tensor API Sample

## Overview

This sample combines the experimental Tensor API register load/store operations with current C API conversion interfaces to perform element-wise data type conversion. Both input and output are one-dimensional tensors with shape `[256]`.

The sample covers six conversion scenarios between floating-point types and between integer and floating-point types. It also demonstrates the `unpack` and `unpack4` memory access modes together with `deinterleave` data reordering used for conversions between types of different widths. The computation is as follows:

```text
y[i] = convert<To>(x[i])
```

## Supported Products

- Ascend 950PR/Ascend 950DT

## Directory Structure

```text
├── CMakeLists.txt          // Build configuration
├── README.md               // Sample description
├── cast.asc                   // Operator kernel, host invocation, and result verification
└── scripts
    └── gen_data.py         // Script for generating input and golden data
```

## Sample Description

- Sample Function:

  `SCENARIO_NUM` selects the data type conversion scenario to compile and run. The input type, output type, layout, rounding mode, and saturation mode for each scenario are listed below.

  | SCENARIO_NUM | Input Type | Output Type | Layout | Round | Sat | Description |
  | --- | --- | --- | --- | --- | --- | --- |
  | 1 | `half` | `int32_t` | `zero` | `floor` | `not_sat` | Rounds down and converts the result to a 32-bit signed integer. |
  | 2 | `float` | `int16_t` | `zero` | `rint` | `sat` | Rounds to the nearest integer and saturates the result to the `int16_t` range. |
  | 3 | `int8_t` | `int32_t` | `zero` | `rint` | `not_sat` | Extends an 8-bit signed integer to a 32-bit signed integer. |
  | 4 | `int32_t` | `uint8_t` | `zero` | `rint` | `sat` | Saturates the value to `[0, 255]` and converts it to an 8-bit unsigned integer. |
  | 5 | `bfloat16_t` | `float` | `zero` | `rint` | `not_sat` | Extends `bfloat16_t` to `float`. |
  | 6 | `float` | `bfloat16_t` | `zero` | `rint` | `not_sat` | Rounds `float` to `bfloat16_t`. |

- Sample Specifications:

  <table>
    <tr>
      <td align="center">Sample Type (OpType)</td>
      <td colspan="4" align="center">cast</td>
    </tr>
    <tr>
      <td rowspan="2" align="center">Input</td>
      <td align="center">name</td>
      <td align="center">shape</td>
      <td align="center">data type</td>
      <td align="center">layout</td>
    </tr>
    <tr>
      <td align="center">x</td>
      <td align="center">[256]</td>
      <td align="center">half/float/int8_t/int32_t/bfloat16_t</td>
      <td align="center">One-dimensional</td>
    </tr>
    <tr>
      <td rowspan="2" align="center">Output</td>
      <td align="center">name</td>
      <td align="center">shape</td>
      <td align="center">data type</td>
      <td align="center">layout</td>
    </tr>
    <tr>
      <td align="center">y</td>
      <td align="center">[256]</td>
      <td align="center">int32_t/int16_t/uint8_t/float/bfloat16_t</td>
      <td align="center">One-dimensional</td>
    </tr>
    <tr>
      <td align="center">Kernel Name</td>
      <td colspan="4" align="center">cast</td>
    </tr>
  </table>

  | SCENARIO_NUM | `input/input_x.bin` | Input Size | `output/output.bin` / `output/golden.bin` | Output Size |
  | --- | --- | --- | --- | --- |
  | 1 | 256 `half` elements | 512 B | 256 `int32_t` elements | 1024 B |
  | 2 | 256 `float` elements | 1024 B | 256 `int16_t` elements | 512 B |
  | 3 | 256 `int8_t` elements | 256 B | 256 `int32_t` elements | 1024 B |
  | 4 | 256 `int32_t` elements | 1024 B | 256 `uint8_t` elements | 256 B |
  | 5 | 256 `bfloat16_t` | 512 B | 256 `float` elements | 1024 B |
  | 6 | 256 `float` elements | 1024 B | 256 `bfloat16_t` | 512 B |

- Sample Implementation:

  - Key Kernel Steps

    1. Use `asc::te::make_tensor`, `asc::te::make_mem_ptr`, and a one-dimensional layout with `make_shape(256)` and `make_stride(1)` to construct the GM and UB tensors.
    2. Wrap the MTE2 pipeline with `asc_lock/asc_unlock`, and use `asc::te::copy` to move the input from GM to UB.
    3. Calculate the number of elements processed by each register operation based on the larger width of the source and destination types, and use `asc::te::experimental::update_mask` to set the valid-element mask.
    4. Use normal `load/store` for equal-width conversions, `unpack` or `unpack4` for widening loads, and use `deinterleave` to compact narrowing results before a normal `store`.
    5. Call the current C API conversion overloads with `ASC_POSITION_EVEN` or `ASC_DISPERSE_FIRST_QUARTER`.
    6. Lock the Vector and MTE3 pipelines in sequence with the same mutex, and then move the result from UB to GM.

    The core conversion code is as follows:

    ```cpp
    auto src_reg = asc::te::experimental::load(src, coord);
    src_reg = src_reg.with_mask(asc::te::experimental::update_mask<WiderT>(remain));

    auto dst_reg = asc::te::experimental::cast<int32_t>(src_reg);
    asc::te::experimental::store(dst, coord, dst_reg);
    ```

    A narrowing conversion uses layout zero for the register result. A 2x narrowing conversion applies one `deinterleave` operation to extract even positions, while a 4x narrowing conversion applies two to extract every fourth element before writing contiguous destination data with a normal `store`.

## Build and Run

Run the following steps in the sample root directory to build and execute the operator.

- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit on the current environment. **Currently only [CANN master](../../../../../../docs/en/quick_start.md#cann-install) is supported.**

  ```bash
  source ${install_path}/set_env.sh
  ```

  Replace `${install_path}` with the actual CANN software package installation directory.

- Run the Sample

  The following example runs scenario 1:

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build
  cmake -DSCENARIO_NUM=1 -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..
  make -j
  python3 ../scripts/gen_data.py --scenario_num=1
  ./demo
  ```

  The `scenario_num` passed to the data generation script must match the `SCENARIO_NUM` used during compilation. To run another scenario, set both values to the corresponding number from 1 to 6.

  To use NPU simulation, add the following build option:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DSCENARIO_NUM=1 -DCANN_ASC_USE_EXPERIMENTAL=ON ..
  ```

  When switching the run mode, chip model, or scenario, clear the CMake cache in the `build` directory before reconfiguring the project.

- Build Options

  | Option | Description |
  | --- | --- |
  | `CMAKE_ASC_RUN_MODE` | Operator execution mode. Available values are `npu` and `sim`. The default is `npu`. |
  | `CMAKE_ASC_ARCHITECTURES` | NPU chip model. The default is `dav-3510`. |
  | `CANN_ASC_USE_EXPERIMENTAL` | Experimental ASC API switch. This sample requires `ON`; the default is `OFF`. |
  | `SCENARIO_NUM` | Required data type conversion scenario number. The valid range is 1 to 6. |

- Expected Result

  The following output indicates successful execution:

  ```text
  test pass!
  ```
