# pybind11 Custom Operator Direct Invocation Example

## Overview

This example demonstrates how to register a custom operator using pybind11, based on the Add operator.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.1.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.1.0 |

## Directory Structure

```
├── pybind
│   ├── CMakeLists.txt        // Build project file
│   ├── add_custom.asc        // Ascend C operator implementation & pybind11 registration
│   ├── add_custom_test.py    // PyTorch invocation script
│   └── README.md             // Example documentation
```

## Example Description

- Example functionality:

  The Add computation formula is:

  ```
  z = x + y
  ```

- Example specifications:
  <table border="2" align="center">
  <caption>Table 1: AddCustom Example Specification Description</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">AddCustom</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[8, 2048]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[8, 2048]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[8, 2048]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">add_custom</td></tr>
  </table>

- Custom operator registration:

  This example defines a C++ namespace named `ascendc_ops` in `add_custom.asc` and implements the `ascendc_add` function within it.

  pybind11 enables the PyTorch framework to invoke the example kernel program, thereby achieving integrated deployment of Ascend C examples within the PyTorch framework.

  `add_custom.asc` uses the `pybind11` library to wrap C++ code into a Python module. In `PYBIND11_MODULE(ascendc_ops, m)`, `ascendc_ops` is the exported Python module name and `m` is the pybind11 module object; `m.def` binds the C++ function `ascendc_ops::ascendc_add` as a Python-callable `ascendc_add` function, for example:

  ```cpp
  PYBIND11_MODULE(ascendc_ops, m)
  {
      m.doc() = "add_custom pybind11 interfaces";
      m.def("ascendc_add", &ascendc_ops::ascendc_add, "");
  }
  ```

  In the `ascendc_add` function, the current NPU stream is obtained via `c10_npu::getCurrentNPUStream()`, and the custom kernel function `add_custom` is invoked through the kernel invocation notation `<<<>>>` to execute the example on the NPU.

- Python test script

  In the `add_custom_test.py` invocation script, the custom module is imported via `import ascendc_ops`, the registered `ascendc_add` function is called, and the numerical correctness of the custom example is verified by comparing the NPU output with the CPU standard addition result.

## Build and Run

- Install PyTorch and the Ascend Extension for PyTorch plugin

  Refer to the [pytorch: Ascend Extension for PyTorch](https://gitcode.com/Ascend/pytorch) open source repository or the [Ascend Extension for PyTorch Ascend Community](https://hiascend.com/document/redirect/Pytorch-index) installation guide. Select a supported `Python` version distribution and complete the installation of `torch` and `torch-npu`.

- Install prerequisites

  ```bash
  pip3 install pybind11 expecttest
  ```

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example
  Run the following steps in the root directory of this example.

  ```bash
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project
  python3 ../add_custom_test.py    # Run the example
  ```

- Build option description

  | Option | Values | Description |
  | -------| -------| ------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The following output indicates successful precision comparison.

  ```bash
  Ran 1 test in **s
  OK
  ```
