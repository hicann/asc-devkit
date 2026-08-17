# Ascend C Operator Integration with PyTorch Graph Mode via TorchAir

## Overview

This example uses the AddCustom operator to demonstrate how to integrate an Ascend C operator into PyTorch graph mode through TorchAir. After the user calls `torch.ops.ascendc_ops.ascendc_add` in a model, `torch.compile` captures the call, TorchAir converts it into a GE `AddCustom` node, and the GE execution entry point launches the Ascend C kernel.

> **What is TorchAir graph mode integration?** <br>
> TorchAir handles the conversion from PyTorch graphs to GE graphs. For details on how PyTorch-side interfaces enter the TorchAir/GE graph execution pipeline, refer to [TorchAir Graph Mode Integration Overview](https://www.hiascend.com/document/detail/zh/Pytorch/2600/modthirdparty/torchairuseguide/docs/zh/custom_op_graph/overview.md).

**Core pipeline**:

1. Implement the Ascend C Add kernel in `add_custom_kernel.asc`.
2. Implement the GE `AddCustom` deliverables in `add_custom_ge.asc`, including the operator prototype, shape/dtype inference, and the `Execute` entry point.
3. `add_custom_test.py` loads the compiled `libascendc_ops.so` via `torch.ops.load_library`, making the PyTorch-side interfaces and GE `AddCustom` deliverables effective.
4. Add PyTorch-side schema/Meta registration and TorchAir converter in `add_custom_pytorch.asc` and `add_custom_test.py`, so that the `ascendc_add` node in the model maps to the GE `AddCustom` node.
5. Execute `torch.compile` graph mode testing through `add_custom_test.py` using the TorchAir backend, and verify the GE graph execution results.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.1.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.1.0 |

## Directory Structure

```text
├── ge_torchair
│   ├── CMakeLists.txt          // Build project file
│   ├── add_custom_kernel.asc   // Ascend C kernel implementation
│   ├── add_custom_pytorch.asc  // PyTorch-side schema and Meta registration
│   ├── add_custom_ge.asc       // GE AddCustom prototype registration and execution implementation
│   ├── add_custom_test.py      // TorchAir graph mode verification script
│   └── README.md               // Example documentation
```

## Example Implementation

### Ascend C Computation Implementation

`add_custom_kernel.asc` implements the Ascend C kernel for the Add computation. The Add formula is:

$$
z = x + y
$$

- x: input, shape `[8, 2048]`, data type float, format ND;
- y: input, shape `[8, 2048]`, data type float, format ND;
- z: output, shape `[8, 2048]`, data type float, format ND.

### GE Deliverables Implementation

`add_custom_ge.asc` provides the GE-side `AddCustom` prototype, shape/dtype inference, and execution entry point. When GE reaches the `AddCustom` node, it enters the execution entry point and launches the Ascend C kernel in `add_custom_kernel.asc`.

For basic concepts of building GE nodes through operator prototypes, refer to [What is an Operator Prototype](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta1/programug/graphdevg/atlasag_25_0008.html).

### Requirements for TorchAir Graph Integration

`add_custom_pytorch.asc` provides the PyTorch-side visible `ascendc_ops::ascendc_add` interface. The graph capture phase primarily requires two types of information: the interface signature and the Meta implementation.

The interface signature enables Python-side invocation via `torch.ops.ascendc_ops.ascendc_add(x, y)`:

```cpp
TORCH_LIBRARY_FRAGMENT(ascendc_ops, m)
{
    m.def("ascendc_add(Tensor x, Tensor y) -> Tensor");
}
```

When `torch.compile` captures the graph, it does not execute actual NPU computation, but it needs to infer output tensor information. Therefore, a Meta implementation must also be registered:

```cpp
TORCH_LIBRARY_IMPL(ascendc_ops, Meta, m)
{
    m.impl("ascendc_add", TORCH_FN(AscendcAddMeta));
}
```

### TorchAir to GE Node Conversion

`add_custom_test.py` registers a TorchAir converter that converts `torch.ops.ascendc_ops.ascendc_add` to a GE `AddCustom` node:

```python
@torchair.register_fx_node_ge_converter(torch.ops.ascendc_ops.ascendc_add.default)
def convert_ascendc_add(x, y, z=None, meta_outputs=None):
    return torchair.ge.custom_op(
        "AddCustom",
        inputs={"x": x, "y": y},
        outputs=["z"],
    )
```

### Loading and Graph Mode Execution

After the implementation and registration are compiled into `libascendc_ops.so`, `add_custom_test.py` loads the dynamic library first, making the PyTorch-side interfaces and GE `AddCustom` deliverables effective:

```python
torch.ops.load_library(LIB_PATH)
```

The interface is called in the model via `torch.ops.ascendc_ops.ascendc_add`, then passed to `torch.compile` for capture and execution with the TorchAir backend:

```python
class AddCustomModel(torch.nn.Module):
    def forward(self, x, y):
        return torch.ops.ascendc_ops.ascendc_add(x, y)

opt_model = torch.compile(model, fullgraph=True, backend=npu_backend, dynamic=False)
```

The compiled model runs on the NPU side. When GE reaches the `AddCustom` node, it enters the execution entry point in `add_custom_ge.asc` and launches the Ascend C kernel in `add_custom_kernel.asc`.

## Build and Run

- Install PyTorch, the Ascend Extension for PyTorch plugin, and TorchAir

  Refer to the [pytorch: Ascend Extension for PyTorch](https://gitcode.com/Ascend/pytorch) open source repository, the [Ascend Extension for PyTorch Ascend Community](https://hiascend.com/document/redirect/Pytorch-index), and the TorchAir installation guide. Select a supported `Python` version distribution and complete the installation of `torch`, `torch-npu`, and `torchair`.

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following steps in the root directory of this example.

  ```bash
  # Create and enter the build directory
  mkdir -p build && cd build;

  # Build the single operator and generate libascendc_ops.so
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;

  # Load libascendc_ops.so and run the TorchAir graph mode test
  python3 ../add_custom_test.py
  ```

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: `dav-2201` corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; `dav-3510` corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  `add_custom_test.py` generates input tensors and standard addition results on the CPU side, then copies the inputs to the NPU side to execute the compiled graph mode model. After execution, the script copies the NPU output back to the CPU and compares the graph mode output with the CPU standard result using `torch.testing.assert_close`.

  The following output indicates that the TorchAir graph mode execution result matches the CPU standard addition result.

  ```bash
  Ran 1 test in **s
  OK
  ```
