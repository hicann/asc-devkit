# torch.library Custom Operator Direct Invocation Example

## Overview

This example uses the Roll operator to demonstrate how to register custom operators using PyTorch's torch.library mechanism.
## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version

- \>= CANN 9.1.0

## Directory Structure

```
├── torch_library_roll
│   ├── CMakeLists.txt          // Build project file
│   ├── roll_custom.asc         // Ascend C operator implementation & torch.library registration
│   ├── roll_custom_test.py     // PyTorch invocation script
│   └── figures                 // Image resources for README
```

## Example Description

- Example Function:

  The Roll operator performs element rolling on a 2D input tensor along the specified dimension `dims` with roll steps `shifts`. Elements that exceed the boundary wrap around from the other side.

  Taking `input.shape=[4,4]`, `shifts=1`, `dims=0` as an example, the effect of rolling 1 step forward along dim0 (row direction) is as follows:

  <p align="center">
    <img src="./figures/roll.png" width="50%">
     </p>
  <p align="center">
  Figure 1: Roll operator rolling effect illustration
  </p>

  Each row of elements moves down one row, and the last row wraps back to the first row.

- Example Specifications:
  <table border="2" align="center">
  <caption>Table 1: RollCustom Example Specification Description</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">RollCustom</td></tr>
  <tr><td rowspan="4" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[8,2048]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td align="center">shifts</td><td align="center">[1]</td><td align="center">int32</td><td align="center">ND</td></tr>
  <tr><td align="center">dims</td><td align="center">[1]</td><td align="center">int32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">output</td><td align="center">[8,2048]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">roll_custom</td></tr>
  </table>

- Custom Operator Registration:

  This example implements the complete registration and invocation chain of the custom operator in `roll_custom.asc` in the following order, as shown in the figure below:

  <p align="center">
    <img src="./figures/自定义算子注册.png" width="50%">
     </p>
  <p align="center">
  Figure 2: Custom operator registration
  </p>


  (1) Implement the Device-side kernel function

  `roll_custom` is the kernel function of this example, implementing the Roll operator rolling logic on the Device side using the Ascend C SIMT programming model.

  (2) Implement the Host-side function

  `ascendc_roll` is the Host-side function, defined in the `ascendc_ops` namespace, responsible for calling the Device kernel function. This function first obtains the current stream on the NPU through `c10_npu::getCurrentNPUStream()` (used to manage asynchronous task queues for operator execution), then uses the obtained stream as an input parameter of the kernel invocation operator `<<<>>>` to call the kernel function `roll_custom` for asynchronous operator execution on the NPU.

  (3) Complete operator registration

  Use PyTorch's `TORCH_LIBRARY` and `TORCH_LIBRARY_IMPL` macros to complete the custom operator declaration and dispatch binding.

  PyTorch provides the `TORCH_LIBRARY` macro as the core interface for custom operator registration, used to create and initialize the custom operator library. After registration, the operator can be called from the Python side through `torch.ops.namespace.op_name`:

  ```cpp
  TORCH_LIBRARY(ascendc_ops, m) {
      m.def("ascendc_roll(Tensor x, int[] shifts, int[] dims) -> Tensor");
  }
  ```

  `TORCH_LIBRARY_IMPL` is used to bind the operator to a specific `DispatchKey` (PyTorch device scheduling identifier). For NPU devices, the operator implementation must be registered to the `PrivateUse1` dedicated `DispatchKey`:

  ```cpp
  TORCH_LIBRARY_IMPL(ascendc_ops, PrivateUse1, m)
  {
      m.impl("ascendc_roll", TORCH_FN(ascendc_ops::ascendc_roll));
  }
  ```

  After registration is complete, the custom operator can be called directly from the Python side through `torch.ops.ascendc_ops.ascendc_roll(...)`.

- Python Invocation

  In the `roll_custom_test.py` invocation script, load the generated custom operator library through `torch.ops.load_library`, call the registered `ascendc_roll` function, and verify the numerical correctness of the custom operator by comparing the NPU output with the CPU standard roll result.

## Build and Run

- Install PyTorch and Ascend Extension for PyTorch plugin

  Refer to the [pytorch: Ascend Extension for PyTorch](https://gitcode.com/Ascend/pytorch) open source repository or the [Ascend Extension for PyTorch Ascend Community](https://hiascend.com/document/redirect/Pytorch-index) installation instructions. Select a compatible `Python` version distribution and complete the installation of `torch` and `torch-npu`.

- Install Prerequisites

  ```bash
  pip3 install expecttest     # Dependency for torch_npu.testing.testcase
  ```

- Configure Environment Variables  

  Configure environment variables based on the [installation method](../../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example

  Run the following steps in the root directory of this example to run the example.

  ```bash
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                # Build the project
  python3 ../roll_custom_test.py   # Run the example
  ```

- Build Options Description

  | Option       | Values           | Description                                                         |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |

- Execution Result

  The following output indicates that the accuracy verification is successful.

  ```bash
  Ran 1 test in **s
  OK
  ```
