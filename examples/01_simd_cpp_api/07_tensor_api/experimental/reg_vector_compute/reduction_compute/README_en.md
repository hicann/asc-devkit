# reduce_sum/reduce_max/reduce_min Interface Sample

## Overview

This sample combines the experimental Tensor API register load, element-scope reduction, and store operations to demonstrate `asc::te::experimental::reduce_sum`, `reduce_max`, and `reduce_min`. Input `src` is a one-dimensional `float` Tensor with shape `[64]`. The three reduction results are written to three 64-element regions in the output Tensor.

The computation is as follows:

```text
dst[0]   = sum(src[0:64])
dst[64]  = max(src[0:64])
dst[128] = min(src[0:64])
```

Only the first element of each result region is valid. The remaining elements are not verified.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Directory Structure

```text
├── CMakeLists.txt          // Build configuration
├── README.md               // Chinese sample description
├── README_en.md            // English sample description
├── data_utils.h            // Host-side binary file helpers
├── reduction_compute.asc   // Operator kernel, host invocation, and result verification
└── scripts
    └── gen_data.py         // Input and golden data generation script
```

## Sample Description

- Sample function:

  The sample reads the input from `input/input.bin`, moves the input Tensor from GM to UB, loads it into a register with `load`, and calls the three experimental Reg Tensor reduction APIs. It then uses `store` to write the results back to UB and GM. The host saves the complete output to `output/output.bin` and verifies the valid results against the three values in `output/golden.bin`.

- Sample specifications:

  | File | Content | Size |
  | --- | --- | --- |
  | `input/input.bin` | 64 `float` input elements | 256 B |
  | `output/output.bin` | Three 64-element `float` result regions | 768 B |
  | `output/golden.bin` | Three `float` values in sum, max, min order | 12 B |

- Sample implementation:

  - Key kernel steps

    1. Use `asc::te::make_tensor`, `asc::te::make_mem_ptr`, and layouts to construct the GM and UB Tensors.
    2. Wrap the MTE2 pipeline with `asc_lock/asc_unlock`, and use `make_copy(copy_gm_to_ub{})` and `copy` to move the input from GM to UB.
    3. Use `all_mask<float>` and `load` to load 64 `float` values into a Reg Tensor.
    4. Call `reduce_sum<reduce_scope::element, float>`, `reduce_max`, and `reduce_min` to perform element-scope reductions.
    5. Use `store` to write the three UB result regions, then use `make_copy(copy_ub_to_gm{})` and `copy` to write them back to GM.

  The core computation is as follows:

  ```cpp
  const auto coord = asc::te::make_coord(0);
  auto mask = asc::te::experimental::all_mask<float>();
  auto src = asc::te::experimental::load(src_tensor, coord).with_mask(mask);

  auto sum = asc::te::experimental::reduce_sum<asc::te::experimental::reduce_scope::element, float>(src);
  auto max = asc::te::experimental::reduce_max(src);
  auto min = asc::te::experimental::reduce_min(src);

  asc::te::experimental::store(sum_tensor, coord, sum);
  asc::te::experimental::store(max_tensor, coord, max);
  asc::te::experimental::store(min_tensor, coord, min);
  ```

  `reduce_sum` also supports `reduce_scope::datablock` and `reduce_scope::pair`; specify both the reduction scope and destination type as `<scope, DstType>`. `reduce_max` and `reduce_min` also support `reduce_scope::datablock`; the output type is deduced from the input `reg_tensor`, and only `<scope>` is needed when selecting a non-default reduction scope.

## Build and Run

This sample has been verified with CANN 9.2.0, `dav-3510`, and NPU simulation mode. Run the following steps from the sample root directory.

- Configure environment variables

  ```bash
  source ${install_path}/set_env.sh
  ```

  `${install_path}` is the CANN 9.2.0 installation directory, for example `/usr/local/Ascend/cann-9.2.0`.

- Build and run

  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..
  make -j
  python3 ../scripts/gen_data.py
  ./demo
  ```

  On a supported NPU system, omit `-DCMAKE_ASC_RUN_MODE=sim` to use the default `npu` mode. Clear the CMake cache in the `build` directory before switching the run mode or chip model.

- Build options

  | Option | Description |
  | --- | --- |
  | `CMAKE_ASC_RUN_MODE` | Operator execution mode: `npu` or `sim`. The default is `npu`. |
  | `CMAKE_ASC_ARCHITECTURES` | NPU architecture. This sample supports only `dav-3510`. |
  | `CANN_ASC_USE_EXPERIMENTAL` | Experimental ASC API switch. This sample requires `ON`. |

- Expected result

  Successful verification prints:

  ```text
  test pass!
  ```
