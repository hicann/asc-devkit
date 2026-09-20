# operator<< Interface Sample

## Overview

This sample demonstrates how to use the experimental Tensor API `asc::te::experimental::operator<<` to shift `uint32_t` elements left by the corresponding `int32_t` shift values.

```text
dst[i] = src[i] << shift[i]
```

## Supported Products

- Ascend 950PR/Ascend 950DT

## Directory Structure

```text
├── CMakeLists.txt                 // Build project
├── README.md                      // Sample description
└── shift_left.asc                 // operator<< interface sample implementation
```

## Sample Description

- Sample function:

  The sample moves the source data and shift values from GM to UB, loads them into registers using `load`, performs element-wise left shifts using `operator<<`, and writes the result back to UB and GM. The host generates the inputs and verifies the output directly.

- Sample scenarios:

  | SCENARIO_NUM | Tensor Layout | Description |
  | --- | --- | --- |
  | 1 | One-dimensional, shape `[128]` | Loads and stores register blocks by one-dimensional offsets. |
  | 2 | Two-dimensional, shape `[2, 64]` | Loads and stores each row by two-dimensional coordinates. |

- Sample implementation:

  The core computation is as follows:

  ```cpp
  const auto coord = asc::te::make_coord(0);
  auto mask = asc::te::experimental::all_mask<uint32_t>();
  auto src = asc::te::experimental::load(src_tensor, coord).with_mask(mask);
  auto shift = asc::te::experimental::load(shift_tensor, coord).with_mask(mask);
  auto dst = src << shift;
  asc::te::experimental::store(dst_tensor, coord, dst);
  ```

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

  For NPU simulation mode, add `-DCMAKE_ASC_RUN_MODE=sim` to the CMake command. When changing the run mode, chip model, or scenario, clear the CMake cache in the `build` directory before configuring the project again.

- Result

  Successful verification prints:

  ```text
  test pass!
  ```
