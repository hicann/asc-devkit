# Sum Example

## Overview

This example demonstrates the Sum high-level API, which computes the sum of elements along the last dimension of the input data. If the input is a 1D vector, it sums all elements in the vector. If the input is a matrix, it sums the elements in each row along the last dimension.

> **API tip:** In addition to the `Sum` API used in this example, Ascend C also provides the `Mean` API for computing the mean value. The invocation method is the same as `Sum`. Simply replace the function name to switch:
> - **Mean**: Compute the mean. Replace `AscendC::Sum` with `AscendC::Mean` and `GetSumMaxMinTmpSize` with `GetMeanMaxMinTmpSize` in `sum.asc`.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── sum
│   ├── scripts
│   │   └── gen_data.py               // Script for generating input data and ground truth data
│   ├── CMakeLists.txt                // Build project file
│   ├── data_utils.h                  // Data read/write utility functions
│   ├── sum.asc                       // Ascend C example implementation & main program
│   └── README.md                     // Example documentation
```

## Example Specifications

- Example function:  
  This example computes the sum of elements in each row of the matrix.

<table>
<caption>Table 1: Example Specifications</caption>
<tr><td rowspan="1" align="left">Example Type (OpType)</td><td colspan="4" align="center"> sum </td></tr>

<tr><td rowspan="3" align="left">Example Input</td></tr>
<tr><td align="left">name</td><td align="left">shape</td><td align="left">data type</td><td align="left">format</td></tr>
<tr><td align="left">src</td><td align="left">[3, 32]</td><td align="left">float</td><td align="left">ND</td></tr>

<tr><td rowspan="2" align="left">Example Output</td></tr>
<tr><td align="left">dst</td><td align="left">[8, 1]</td><td align="left">float</td><td align="left">ND</td></tr>
<tr><td rowspan="1" align="left">Kernel Function Name</td><td colspan="4" align="center">sum_custom</td></tr>
</table>

- Example implementation:  
  This example implements a fixed shape with input x[3, 32] and output y[8, 1], where the number of valid values in y is 3. After summing the elements of each row of input x, the valid data of output y occupies the first 3 positions, and the last 5 positions are padding data.
  - Kernel implementation:  
    The computation logic is: This example transfers the input data to on-chip storage, then uses the Sum high-level API to complete the summation, and transfers the result out.

  - Tiling implementation:  
    The tiling implementation process is as follows: Use the GetSumMaxMinTmpSize API to calculate the required maximum/minimum temporary space size, use the minimum temporary space, and then determine the required tiling parameters based on the input length.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j;    # Build the project
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  ```

  To use CPU debugging or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  For example:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debugging mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Before switching the build mode, clear the cmake cache by running `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU run, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
