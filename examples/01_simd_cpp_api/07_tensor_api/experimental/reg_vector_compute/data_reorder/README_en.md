# data_reorder Example

## Overview

This example implements data reordering based on the static Tensor API programming model. It primarily calls the experimental `asc::te::experimental::deinterleave` interface.

## Supported Products and CANN Versions

| Product | CANN Version |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```text
data_reorder
├── scripts/gen_data.py
├── CMakeLists.txt
├── data_reorder.asc
├── data_utils.h
└── README_en.md
```

## Example Description

- Function:
  - Deinterleaves two float vectors, each containing 256 elements, and outputs two float vectors.
- Specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="3" align="center">Inputs</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">src0</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td align="center">src1</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="3" align="center">Outputs</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">dst0</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td align="center">dst1</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function</td><td colspan="3" align="center">data_reorder</td></tr>
  </table>
- Implementation:  
  The `data_reorder_vf` function calls the `deinterleave` interface to reorder data:
  - Use `load` to read the inputs from UB into the `src0_reg` and `src1_reg` registers.
  - Use `deinterleave` to deinterleave the source data into the destination `reg_tensor` objects.
  - Use `store` to write the results back to UB.
  - Invoke the kernel function using the `<<<>>>` kernel launch syntax.

## Build and Run

Perform the following steps in the example root directory to build and run the example.

- Configure environment variables  
  Configure the environment variables according to the CANN development kit [installation instructions](https://gitcode.com/cann/asc-devkit/blob/master/docs/en/quick_start.md). **Currently, only [CANN master](https://gitcode.com/cann/asc-devkit/blob/master/docs/en/quick_start.md) is supported.**

  > **Note:** `${install_path}` is the CANN package installation directory. If no installation directory is specified, the default directory is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory:

  ```bash
  mkdir -p build && cd build;                                               # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..;make -j; # Build the project (NPU mode by default)
  python3 ../scripts/gen_data.py                                            # Generate test ground truth data
  ./demo                                                                    # Run the generated executable
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` option.

  Example:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..;make -j; # NPU simulation mode
  ```

  > **Note:** Clear the CMake cache before switching build modes. Run `rm CMakeCache.txt` in the `build` directory, and then run CMake again.

- Build options

| Option | Values | Description |
| --- | --- | --- |
| `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution or NPU simulation |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: `dav-3510` corresponds to Ascend 950PR/Ascend 950DT |
| `CANN_ASC_USE_EXPERIMENTAL` | `ON` (required for this example), `OFF` (default) | Enables experimental ASC interfaces |

- Execution result

  The following output indicates that the accuracy comparison is successful:

  ```bash
  test pass!
  ```
