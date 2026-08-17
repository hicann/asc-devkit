# IsFinite Example

## Overview

This example uses the IsFinite high-level API to determine element-wise whether input floating-point numbers are neither NAN nor INF. The output result is a floating-point number or a boolean value.

> **API tip:** In addition to the `IsFinite` API used in this example, Ascend C also provides the following determination related high-level APIs. To switch, simply replace the function name:
>
> - **IsInf**: Determine whether the input is infinity.
> - **IsNaN**: Determine whether the input is not a number.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```text
├── isfinite
│   ├── scripts
│   │   └── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── isfinite.asc            // Ascend C operator implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  Determine element-wise whether input floating-point numbers are neither NAN nor INF. The output result is a floating-point number or a boolean value. For input data that is neither NAN nor INF, when the output is a floating-point type, the result at the corresponding position is 1 of that floating-point type; otherwise it is 0. When the output is a bool type, the result at the corresponding position is true; otherwise false.  
  The computation formulas are as follows:  
  $$dst_i = IsFinite(src_i)$$

  When the input is a floating-point type:  
  $$
  IsFinite(x) = 
  \begin{cases}
  0.0, & x = \pm\inf \text{ or } x = \text{nan} \\
  1.0, & x \ne \pm\inf \text{ and } x \ne \text{nan}
  \end{cases}
  $$

  When the output is a bool type:  
  $$
  IsFinite(x) =
  \begin{cases}
  false, & x = \pm\inf \text{ or } x = \text{nan} \\
  true, & x \ne \pm\inf \text{ and } x \ne \text{nan}
  \end{cases}
  $$
- Example specifications:  
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> isfinite </td></tr>

  <tr><td rowspan="3" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 1024]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">isfinite_custom</td></tr>
  </table>

- Example implementation:  
  This example implements an isfinite_custom example with fixed shapes of input x[1, 1024] and output y[1, 1024].

  - Kernel implementation  
    The computation logic is: The vector computation APIs provided by Ascend C operate on LocalTensor elements. Input data must first be transferred to on-chip storage, then the IsFinite high-level API is used to complete the isfinite computation, obtain the final result, and transfer it to external storage.

  - Invocation implementation  
    Use the kernel invocation operator <<<>>> to call the kernel function.

## Build and Run  

Run the following steps in the root directory of this example to build and run the operator.

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
