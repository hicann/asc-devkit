# Truncate Reg API Sample

## Overview

This sample demonstrates how to use the experimental register Tensor API `asc::te::trunc` to round floating-point input elements toward zero. Both the input and output are two-dimensional ND tensors with shape `[16, 16]`, containing 256 elements. The input and output data types are the same.

The computation is as follows:

```text
y[i] = trunc(x[i])
```

For example, `3.75` produces `3.0`, and `-3.75` produces `-3.0`.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Directory Structure

```text
├── CMakeLists.txt              // Build configuration
├── README.md                   // Sample description
├── truncate_reg_api.asc        // Operator kernel, host invocation, and result verification
└── scripts
    └── gen_data.py             // Script for generating input and golden data
```

## Sample Description

- Sample Function:

  `SCENARIO_NUM` selects the data type scenario to compile and run.

  | SCENARIO_NUM | Input Type | Output Type | Description |
  | --- | --- | --- | --- |
  | 1 | `half` | `half` | Rounds each FP16 element toward zero. |
  | 2 | `bfloat16_t` | `bfloat16_t` | Rounds each BF16 element toward zero. |
  | 3 | `float` | `float` | Rounds each FP32 element toward zero. |

- Sample Specifications:

  <table>
    <tr>
      <td align="center">Sample Type (OpType)</td>
      <td colspan="4" align="center">truncate_reg_api</td>
    </tr>
    <tr>
      <td rowspan="2" align="center">Input</td>
      <td align="center">name</td>
      <td align="center">shape</td>
      <td align="center">data type</td>
      <td align="center">layout</td>
    </tr>
    <tr>
      <td align="center">src</td>
      <td align="center">[16, 16]</td>
      <td align="center">half/bfloat16_t/float</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td rowspan="2" align="center">Output</td>
      <td align="center">name</td>
      <td align="center">shape</td>
      <td align="center">data type</td>
      <td align="center">layout</td>
    </tr>
    <tr>
      <td align="center">dst</td>
      <td align="center">[16, 16]</td>
      <td align="center">Same as the input type</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">Kernel Name</td>
      <td colspan="4" align="center">truncate_reg_api</td>
    </tr>
  </table>

  | SCENARIO_NUM | `input/input_x.bin` | Input Size | `output/output.bin` / `output/golden.bin` | Output Size |
  | --- | --- | --- | --- | --- |
  | 1 | 256 `half` elements | 512 B | 256 `half` elements | 512 B |
  | 2 | 256 BF16 bit patterns stored as `uint16_t` | 512 B | 256 BF16 bit patterns stored as `uint16_t` | 512 B |
  | 3 | 256 `float` elements | 1024 B | 256 `float` elements | 1024 B |

- Sample Implementation:

  - Key Kernel Steps

    1. Use `asc::te::make_tensor`, `asc::te::make_mem_ptr`, and `asc::te::make_frame_layout` to construct two-dimensional tensors in GM and UB.
    2. Wrap the MTE2 pipeline with `asc_lock/asc_unlock`, and use `asc::te::copy` to move the input from GM to UB.
    3. Calculate the number of elements processed by each register operation from the register size and element type size, and use `asc::te::all_mask<T>()` to generate an all-valid mask.
    4. Use the tensor `load` interface to load UB data into `asc::te::reg_tensor<T>`, call `asc::te::trunc` to round the data toward zero, and use the tensor `store` interface to write the result back to UB.
    5. Lock the Vector and MTE3 pipelines in sequence with the same mutex, and then move the result from UB to GM.

    The core computation code is as follows:

    ```cpp
    const auto coord = asc::te::make_coord(offset / columnCount, offset % columnCount);
    auto srcReg = src.load(coord);
    srcReg.with_mask(asc::te::all_mask<T>());

    auto dstReg = asc::te::trunc(srcReg);
    dst.store(coord, dstReg);
    ```

  - Invocation

    The host reads `input/input_x.bin`, allocates host and device memory, and launches one Vector Block using `<<<1, 0, stream>>>`. After the kernel finishes, the host writes the result to `output/output.bin` and compares it byte by byte with `output/golden.bin`.

## Build and Run

Run the following steps in the sample root directory to build and execute the operator.

- Configure Environment Variables

  Configure the CANN software package environment variables. For details, see [Environment Variable Configuration](../../../../../docs/quick_start.md#prepare&install).

  ```bash
  source ${install_path}/set_env.sh
  ```

  Replace `${install_path}` with the actual CANN software package installation directory.

- Run the Sample

  The following example runs scenario 1:

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build
  cmake -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..
  make -j
  python3 ../scripts/gen_data.py -scenarioNum=${SCENARIO_NUM}
  ./demo
  ```

  The `scenarioNum` passed to the data generation script must match the `SCENARIO_NUM` used during compilation. To run another scenario, set both values to the corresponding number from 1 to 3.

  To use NPU simulation, add the corresponding build option:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DSCENARIO_NUM=${SCENARIO_NUM} ..
  ```

  When switching the run mode, chip model, or scenario, clear the CMake cache in the `build` directory before reconfiguring the project.

- Build Options

  | Option | Description |
  | --- | --- |
  | `CMAKE_ASC_RUN_MODE` | Operator execution mode. Available values are `npu` and `sim`. The default is `npu`. |
  | `CMAKE_ASC_ARCHITECTURES` | NPU chip model. The default is `dav-3510`. |
  | `SCENARIO_NUM` | Required data type scenario number. The valid range is 1 to 3. |

- Expected Result

  The following output indicates successful execution:

  ```text
  test pass!
  ```
