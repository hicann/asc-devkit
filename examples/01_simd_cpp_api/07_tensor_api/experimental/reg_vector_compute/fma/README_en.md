# fma Interface Sample

## Overview

This sample demonstrates how to use the experimental Tensor API `asc::te::experimental::fma` to perform element-wise fused multiply-add on three `float` Tensors.

```text
dst[i] = src0[i] * src1[i] + src2[i]
```

## Supported Products

- Ascend 950PR&950DT series

## Directory Structure

```text
├── CMakeLists.txt                 // Build project
├── README.md                      // Chinese sample description
├── README_en.md                   // English sample description
└── fma.asc                        // fma interface sample implementation
```

## Sample Description

- Sample function:

  The sample moves three input Tensors from GM to UB, loads them into registers using `load`, performs element-wise fused multiply-add using `fma`, and writes the result back to UB and GM using `store`. The host generates the inputs and verifies the output.

- Sample scenarios:

  | SCENARIO_NUM | Tensor Layout | Description |
  | --- | --- | --- |
  | 1 | One-dimensional, shape `[128]` | Processes two register blocks by one-dimensional offsets. |
  | 2 | Two-dimensional, shape `[2, 64]` | Processes each row by two-dimensional coordinates. |

- Sample implementation:

  - Key kernel steps

    1. Construct one-dimensional or two-dimensional GM and UB Tensors according to `SCENARIO_NUM`.
    2. Move data between GM and UB using `copy(dst, src)`.
    3. Use `update_mask`, `load`, `fma`, and `store` to perform element-wise fused multiply-add.

## Build and Run

Perform the following steps in the sample root directory to build and run the sample.

- Configure environment variables

  Configure the CANN package environment variables by following the [environment configuration guide](../../../../../../docs/en/quick_start.md#prepare&install).

  ```bash
  source ${install_path}/set_env.sh
  ```

  Replace `${install_path}` with the CANN package installation directory.

- Run the sample

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build
  cmake -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..
  make -j
  ./demo
  ```

  For NPU simulation mode, add `-DCMAKE_ASC_RUN_MODE=sim` to the CMake command. When changing the run mode, chip model, or scenario, clear the CMake cache in the build directory before configuring the project again.

- Result

  Successful verification prints:

  ```text
  test pass!
  ```
