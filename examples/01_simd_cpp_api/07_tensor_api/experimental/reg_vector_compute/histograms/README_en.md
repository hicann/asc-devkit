# histograms Interface Sample

## Overview

This sample demonstrates how to use the experimental Tensor API `asc::te::experimental::histograms` to compute the cumulative histogram for the high-half range `[128, 255]` from 256 `uint8_t` elements and output 128 `uint16_t` elements.

## Supported Products

- Ascend 950PR&950DT products

## Directory Structure

```text
├── CMakeLists.txt                 // Build project
├── README.md                      // Sample description
└── histograms.asc                 // histograms interface sample implementation
```

## Sample Description

- Sample function:

  The sample moves the input Tensor from GM to UB, loads it into registers using `load`, calls `histograms` in high-half cumulative mode, and writes the result back to UB and GM. The host generates the input and verifies the output directly.

- Sample scenarios:

  | SCENARIO_NUM | Input and Output Tensor Layouts | Description |
  | --- | --- | --- |
  | 1 | One-dimensional, shapes `[256]` and `[128]` | Processes the complete input and output with one-dimensional coordinates. |
  | 2 | Two-dimensional, shapes `[1, 256]` and `[1, 128]` | Processes the complete input and output with two-dimensional coordinates. |

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
