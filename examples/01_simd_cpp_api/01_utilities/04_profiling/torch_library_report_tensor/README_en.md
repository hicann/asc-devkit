# torch.library Invocation Integrated Profiling for Recording Shape Information

## Overview

This example demonstrates how to display operator input/output Shape, data type, and Format information in `msopprof` results when using the `torch.library` custom operator invocation method.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|----------------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── torch_library_report_tensor
│   ├── CMakeLists.txt                   // Build project file
│   ├── torch_library_report_tensor.py   // PyTorch invocation script
│   ├── torch_library_report_tensor.asc  // torch.library operator registration, Profiling reporting, and Add Kernel implementation
│   └── README.md                        // Example description document
```

## Example Description

- Example features

  The example registers a PyTorch custom operator named `ascendc_ops::ascendc_add`. The Python script inputs two NPU Tensors with shape `[8, 2048]`, data type `float16`, and format `ND`, calls the custom operator to perform Add computation, and verifies accuracy against CPU results. The computation formula is:

  ```
  z = x + y
  ```

- Example specifications

  <table border="2">
  <caption>Table 1: AddCustom Example Specification Description</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">AddCustom</td></tr>
  <tr><td align="center"></td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">Example Input</td><td align="center">x</td><td align="center">[8, 2048]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">Example Input</td><td align="center">y</td><td align="center">[8, 2048]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">Example Output</td><td align="center">z</td><td align="center">[8, 2048]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">add_custom</td></tr>
  </table>

- Example implementation

  The implementation of this example consists of three parts:

  1. `torch.library` operator registration

     `torch_library_report_tensor.asc` defines the `ascendc_ops` namespace and registers `ascendc_add` through `TORCH_LIBRARY` and `TORCH_LIBRARY_IMPL`. The Python side can invoke it as follows:

     ```python
     torch.ops.ascendc_ops.ascendc_add(x.npu(), y.npu())
     ```

  2. Profiling Shape Information Recording

     The `ascendc_add` function constructs Profiling metadata before dispatching the Kernel and passes it to `msopprof` through `aclprofRangePushEx`. After the Kernel dispatch completes, `aclprofRangePop` is called to end the current Profiling range.

     The purpose is to enable `msopprof` to display input/output Tensor information in the current custom operator record when generating `PipeUtilization.csv`, for example:

     ```text
     Input Shapes: "8,2048;8,2048"
     Input Data Types: FLOAT16;FLOAT16
     Input Formats: ND;ND
     Output Shapes: "8,2048"
     Output Data Types: FLOAT16
     Output Formats: ND
     ```

     The reported information consists of the following structures:

     - `aclprofTensor`: Describes a single Tensor, including input/output type, Format, data type, Shape dimensions, and Shape values.
     - `aclprofTensorInfo`: Describes an operator invocation, including operator name, operator type, block count, stream, and input/output Tensor arrays.
     - `aclprofEventAttributes`: The outer message structure used by `aclprofRangePushEx` to carry `aclprofTensorInfo`.

     The `INPUT(x)` and `OUTPUT(z)` macros in the code extract Format, data type, and Shape information from PyTorch `at::Tensor`; the `INIT_ACL_PROF_TENSOR_INFO` macro assembles operator information and Tensor information into `aclprofTensorInfo`.

  3. Add Kernel Implementation

     The device-side `add_custom` Kernel uses static Tensor programming to implement minimal Add computation. The Kernel template parameters only include a fixed Shape: `<8, 2048>`. When the Kernel launches, the block count is passed through `<<<blockNum, 0, stream>>>`, and the Kernel internally calculates the data length processed by each block through `AscendC::GetBlockNum()`.

     This example Kernel only supports `[8, 2048]`, `float16` input. If the input Shape does not meet the requirements, the host side reports an error through `TORCH_CHECK`.

- Python test script

  In the `torch_library_report_tensor.py` invocation script, load the generated custom dynamic library through `torch.ops.load_library`, call the registered `ascendc_add` function, and compare the NPU computation result with the CPU `torch.add` result.

## Build and Run

- Install PyTorch and Ascend Extension for PyTorch

  Refer to the [pytorch: Ascend Extension for PyTorch](https://gitcode.com/Ascend/pytorch) open source repository or [Ascend Extension for PyTorch Community](https://hiascend.com/document/redirect/Pytorch-index) installation instructions. Select a compatible `Python` version distribution and complete the installation of `torch` and `torch-npu`.
  
  > The torch_npu version is 26.0.0.

- Install prerequisites

  ```bash
  pip3 install expecttest
  ```

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following steps in the root directory of this example to run the example.

  ```bash
  mkdir -p build; cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j
  msopprof --application="python3 ../torch_library_report_tensor.py" --output="../result"
  ```

- Build options

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result
  The execution result is as follows, indicating successful Profiling data collection and parsing, where DIR_NAME and PATH are the output file name and data storage directory respectively.

  ```bash
  [INFO] Query all data in ${DIR_NAME} done.
  [INFO] Profiling finished.
  [INFO] Process profiling data complete. Data is saved in ${PATH}
  ```

- Shape information display

  Open `../result/OPPROF_*/PipeUtilization.csv` to view Shape information. The Shape information in this example is written to the following fields.

  <table>
    <tr>
      <td align="center">...</td>
      <td align="center">Op Name</td>
      <td align="center">Op Type</td>
      <td align="center">Input Shapes</td>
      <td align="center">Input Data Types</td>
      <td align="center">Input Formats</td>
      <td align="center">Output Shapes</td>
      <td align="center">Output Data Types</td>
      <td align="center">Output Formats</td>
      <td align="center">...</td>
    </tr>
    <tr>
      <td align="center">...</td>
      <td align="center">xxx</td>
      <td align="center">xxx</td>
      <td align="center">"8,2048;8,2048"</td>
      <td align="center">FLOAT16;FLOAT16</td>
      <td align="center">ND;ND</td>
      <td align="center">"8,2048"</td>
      <td align="center">FLOAT16</td>
      <td align="center">ND</td>
      <td align="center">...</td>
    </tr>
  </table>
