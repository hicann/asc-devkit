# ReduceMax Example

## Overview

This example uses the ReduceMax high-level API in a reduction scenario to compute the maximum value of each column in a matrix.

> **API tip:** In addition to the `ReduceMax` API used in this example, Ascend C also provides the following reduction APIs with the same invocation method as `ReduceMax`. Simply replace the function name to switch:
> - **ReduceMin**: Compute the minimum value. Replace `AscendC::ReduceMax` with `AscendC::ReduceMin` and `GetReduceMaxMaxMinTmpSize` with `GetReduceMinMaxMinTmpSize` in `reducemax.asc`.
> - **ReduceMean**: Compute the mean value. Replace `AscendC::ReduceMax` with `AscendC::ReduceMean` and `GetReduceMaxMaxMinTmpSize` with `GetReduceMeanMaxMinTmpSize` in `reducemax.asc`.
> - **ReduceSum**: Compute the sum. Replace `AscendC::ReduceMax` with `AscendC::ReduceSum` and `GetReduceMaxMaxMinTmpSize` with `GetReduceSumMaxMinTmpSize` in `reducemax.asc`.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── reducemax
│   ├── scripts
│   │   └── gen_data.py             // Script for generating input data and ground truth data
│   ├── CMakeLists.txt              // Build project file
│   ├── data_utils.h                // Data read/write functions
│   ├── reducemax.asc               // Ascend C example implementation & invocation example
│   └── README.md                   // Example documentation
```

## Example Description  

- Example function:  
  This example computes the maximum value of each column in the input matrix.

- Example specifications:  
<table>
<tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> reducemax </td></tr>

<tr><td rowspan="3" align="center">Example Input</td></tr>
<tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[32, 136]</td><td align="center">float</td><td align="center">ND</td></tr>


<tr><td rowspan="2" align="center">Example Output</td></tr>
<tr><td align="center">y</td><td align="center">[1, 136]</td><td align="center">float</td><td align="center">ND</td></tr>


<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">reduce_custom</td></tr>
</table>

- Example implementation:  
  This example computes the maximum value of each column in the input [32, 136] matrix, producing output [1, 136].

  - Kernel implementation:  
    The computation logic is: Input data must first be transferred to on-chip storage, then the ReduceMax high-level API is used to complete the computation, and the result is transferred out.

  - Tiling implementation:  
    The tiling implementation process is as follows: Use the GetReduceMaxMaxMinTmpSize API to calculate the required maximum/minimum temporary space size, use the minimum temporary space, and then determine the required tiling parameters based on the input length.

  - Invocation implementation:  
    Use the kernel invocation operator <<<>>> to call the kernel function.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.
- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # NPU mode by default
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo
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
