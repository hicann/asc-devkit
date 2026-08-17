# Clamp Example

## Overview

This example uses the Clamp high-level API to clamp input values (excluding NaN) to the interval [min, max].
When min is greater than max, all non-NaN values are replaced with max. Both min and max can be scalars or tensors.  

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```text
├── clamp
│   ├── scripts
│   │   └── gen_data.py   // Script for generating input data and ground truth data
│   ├── CMakeLists.txt    // Build project file
│   ├── data_utils.h      // Data read/write functions
│   ├── clamp.asc         // Ascend C example implementation & invocation example
│   └── README.md         // Example documentation
```

## Example Description

- Example function:  
  Replace non-NaN values in the input that are greater than max with max, and replace non-NaN values that are less than min with min. Values between min and max (inclusive) remain unchanged as output. When min is greater than max, all non-NaN values are replaced with max. Both min and max can be scalars or tensors.

  The computation formulas are as follows:  

  $$
  dst_i = Clamp(src_i, min_i, max_i)
  $$

  $$
  Clamp(src_i, min_i, max_i) =
  \begin{cases}
  min_i, & src_i < min_i \\
  src_i, & min_i \le src_i \le max_i \\
  max_i, & src_i > max_i \\
  \end{cases}
  $$

- Example specifications:  
  <table>
  <caption>Table 1: Example Specifications</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> clamp </td></tr>

  <tr><td rowspan="5" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src</td><td align="center">[1, 128]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">src_min</td><td align="center">[1, 128]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">src_max</td><td align="center">[1, 128]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">dst</td><td align="center">[1, 128]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">clamp_custom</td></tr>
  </table>

- Scenario description:
  <table>
  <caption>Table 2: scalarType Parameter Description</caption>
  <tr><td align="center">scalarType</td><td align="center">min Type</td><td align="center">max Type</td><td align="center">Description</td></tr>
  <tr><td align="center">1</td><td align="center">Tensor</td><td align="center">Tensor</td><td align="center">Both min and max are tensors</td></tr>
  <tr><td align="center">2</td><td align="center">Tensor</td><td align="center">Scalar</td><td align="center">min is a tensor, max is a scalar</td></tr>
  <tr><td align="center">3</td><td align="center">Scalar</td><td align="center">Tensor</td><td align="center">min is a scalar, max is a tensor</td></tr>
  <tr><td align="center">4</td><td align="center">Scalar</td><td align="center">Scalar</td><td align="center">Both min and max are scalars</td></tr>
  </table>

- Example implementation:

  This example implements a clamp_custom example with shapes of input src[128], src_min[128], src_max[128], and output dst[128], supporting four scenario combinations where min and max are tensors or scalars.

  - Kernel implementation

    Use the Clamp high-level API to complete the Clamp computation, obtain the final result, and transfer it to external storage.

  - Invocation implementation

    Use the kernel invocation operator <<<>>> to call the kernel function.

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # Build the project, NPU mode by default
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  ```

  To use CPU debugging or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  
  For example:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # CPU debugging mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Before switching the build mode, clear the cmake cache by running `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU run, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` (default) | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The following execution result indicates that the precision comparison is successful.

  ```bash
  test pass!
  ```
