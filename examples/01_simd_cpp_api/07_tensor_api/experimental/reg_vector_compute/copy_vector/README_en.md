# Copy Vector Tensor API Sample

## Overview

This sample demonstrates Tensor API data transfers on the AIV, including `GM->UB`, `UB->GM`, and `UB->UB`
transfers across multiple layout patterns, as well as constant left and right padding on the `GM->UB` path.
Scenarios 1 through 3 use `int8_t` tensors with a shape of `[32, 32]`. Scenario 4 uses a one-dimensional
tensor with a shape of `[1024]`. Scenario 5 pads an input of shape `[32, 24]` to produce an output of shape
`[32, 32]`. Each scenario validates the entire output.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Sample Scenarios

| SCENARIO_NUM | Key path | Layout | Dimensions | Main API usage |
| --- | --- | --- | --- | --- |
| 1 | GM->UB | ND | Four-dimensional | `copy(dst, src)` automatically infers the transfer path |
| 2 | UB->GM | DN | Four-dimensional | Calls `atom.call(dst, src)` after `make_copy(..., trait)` |
| 3 | UB->UB | NZ | Four-dimensional | Uses an explicit atom and the coordinate/shape region transfer overload |
| 4 | GM->UB | ND | One-dimensional | Uses `make_shape(1024)` and `make_stride(1)` to select the one-dimensional copy route |
| 5 | GM->UB | ND | Four-dimensional | Pads three elements on the left and five on the right, and uses `asc_set_copy_pad_val` to set the padding value to `-9` |

The kernels for all five scenarios are declared with `__global__ __vector__`.

## Directory Structure

```text
├── CMakeLists.txt
├── README.md
├── README_en.md
├── copy_vector.asc
├── data_utils.h
└── scripts
    └── gen_data.py
```

## Build and Run

Run the following steps in the sample root directory to build and execute the operator.

- Configure environment variables
  Configure environment variables based on the [installation method](https://gitcode.com/cann/asc-devkit/blob/master/docs/en/quick_start.md) of the CANN development kit on the current environment. **Currently only [CANN master](https://gitcode.com/cann/asc-devkit/blob/master/docs/en/quick_start.md) is supported.**

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. If no installation directory is
  > specified, the package is installed under `/usr/local/Ascend` by default.

  ```bash
  source ${install_path}/set_env.sh
  ```

  Replace `${install_path}` with the actual CANN software package installation directory.

- Run the sample

  The following example uses scenario 1:

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build
  cmake -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..
  make -j
  python3 ../scripts/gen_data.py --scenario_num=${SCENARIO_NUM}
  ./demo
  ```

  The `scenario_num` passed when generating data must match the `SCENARIO_NUM` used at build time. To run a
  different scenario, change both values to the corresponding number from 1 through 6.

  To use NPU simulation, add the following build option:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DSCENARIO_NUM=${SCENARIO_NUM} -DCANN_ASC_USE_EXPERIMENTAL=ON ..
  ```

  When changing the run mode, chip model, or scenario, clear the CMake cache in the `build` directory before
  configuring the project again.

- Build options

  | Option | Description |
  | --- | --- |
  | `CMAKE_ASC_RUN_MODE` | Operator execution mode. Available values are `npu` and `sim`. The default value is `npu`. |
  | `CMAKE_ASC_ARCHITECTURES` | NPU chip model. The default value is `dav-3510`. |
  | `CANN_ASC_USE_EXPERIMENTAL` | Enables experimental ASC interfaces. This sample requires `ON`. The default value is `OFF`. |
  | `SCENARIO_NUM` | Type conversion scenario number. This option is required and accepts values from 1 through 6. |

- Expected result

  A successful execution prints:

  ```text
  test pass!
  ```
