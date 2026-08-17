# Acosh Example

## Overview

This example uses the Acosh high-level API to compute the inverse hyperbolic cosine function.

> **Related examples:** In addition to the `Acosh` API used in this example, Ascend C also provides the following trigonometric function related high-level APIs. Except for sincos, the implementation approach is essentially the same. To switch, simply replace the function name:
>
> - **acos**: Inverse cosine function.
> - **asin**: Inverse sine function.
> - **asinh**: Inverse hyperbolic sine function.
> - **atanh**: Inverse hyperbolic tangent function.
> - **cos**: Cosine function.
> - **cosh**: Hyperbolic cosine function.
> - **sinh**: Hyperbolic sine function.
> - **tan**: Tangent function.
> - **sincos**: Sine and cosine function, computing sine and cosine separately. Requires two output tensors when calling.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```text
├── acosh
│   ├── CMakeLists.txt     // Build project file
│   ├── acosh.asc          // Ascend C example implementation & invocation example
│   └── README.md          // Example documentation
```

## Example Description

- Example function:
  Perform element-wise inverse hyperbolic cosine function computation. The computation formulas are as follows:
  $$dstTensor_i = Acosh(srcTensor_i)$$
  $$Acosh(x)=\begin{cases}Nan, & x < 1 \\ \ln(x+\sqrt{x^{2}-1}), & x > 1\end{cases}$$
- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> acosh </td></tr>

  <tr><td rowspan="3" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">dst</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">acosh_custom</td></tr>
  </table>

- Example implementation:  
  This example implements an acosh_custom example with fixed shapes of input src[1, 16] and output dst[1, 16].

  - Kernel implementation

    Use the Acosh high-level API to complete the inverse hyperbolic cosine computation.

  - Tiling implementation

    The host side obtains the maximum and minimum temporary space required by the Acosh API through GetAcoshMaxMinTmpSize.

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project, NPU mode by default
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

- Execution result

  The following execution result indicates that the precision comparison is successful.

  ```bash
  test pass!
  ```
