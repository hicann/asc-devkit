# Ascend C Operator Integration with GE Example

## Overview

This example uses the AddCustom operator to demonstrate how to integrate Ascend C `<<<>>>` kernel function invocations into the GE (Graph Engine) native graph execution pipeline.

> **What is GE?** <br>
> GE is the graph execution engine in the Ascend computing architecture. Unlike the approach where each operator is dispatched and executed immediately, the GE graph mode organizes operators into a computation graph first, then executes the graph as a whole. The graph mode provides a global perspective, facilitating graph optimization and unified scheduling. For more background, refer to [Graph Mode Development Guide](https://hiascend.com/document/redirect/CannCommunityGraphguide).

**Integration path**:
- Operator implementation: Implement the Ascend C kernel function in `add_custom.asc` to perform a `z = x + y` computation.
- GE integration: Declare the `AddCustom` operator prototype, implement the GE execution entry point, and construct the GE graph in `graph.cpp`.
- Graph execution: `Session::RunGraph` executes the entire graph. When GE reaches the `AddCustom` node, it calls the operator-side execution implementation.

<img src="figures/execution_flow.png" width="75%">

The figure above shows the complete integration pipeline from left to right. The left side is the graph construction side: first, `AddCustom` is declared through the operator prototype, then `Data` nodes and two `AddCustom` nodes are connected into a GE graph in `graph.cpp`, and finally `Session::RunGraph` triggers full-graph execution. The right side is the operator side: when GE reaches each `AddCustom` node, it enters the execution implementation registered in `add_custom.asc`, and the execution entry point launches the Ascend C kernel function via `<<<>>>`.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.1.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.1.0 |

## Directory Structure

```text
├── triple_chevron_notation
│   ├── CMakeLists.txt          // Build project: produces the custom operator dynamic library and GE online graph construction executable
│   ├── add_custom.asc          // Operator side: Ascend C kernel function implementation and GE operator execution implementation
│   ├── add_custom_proto.h      // Graph construction side: AddCustom operator prototype declaration
│   ├── figures                 // Diagrams
│   ├── graph.cpp               // Graph construction side: constructs the GE graph and invokes Session::RunGraph for execution
│   └── README.md               // Example documentation
```

## Example Implementation

### Operator Implementation

The core content of the operator implementation is in `add_custom.asc`. The Add computation formula is:

$$
z = x + y
$$

- x: input, shape [8, 2048], data type float;
- y: input, shape [8, 2048], data type float;
- z: output, shape [8, 2048], data type float;

### GE Integration Process

To execute an Ascend C kernel function as a node in a GE graph, three steps must be completed: operator prototype declaration, operator execution implementation, and GE online graph construction.

The first step is to declare the `AddCustom` operator prototype in `add_custom_proto.h`. The operator prototype describes the operator interface visible to the GE graph construction side, including inputs, outputs, and data types. After declaration, the GE API can use `ge::op::AddCustom` in graph construction code to create custom operator nodes. For the concept of operator prototypes, refer to [What is an Operator Prototype](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta1/programug/graphdevg/atlasag_25_0008.html).

The operator prototype declaration is as follows. The GE graph construction side generates the `ge::op::AddCustom` type based on this prototype:

```cpp
REG_OP(AddCustom)
    .INPUT(x, TensorType({DT_FLOAT}))
    .INPUT(y, TensorType({DT_FLOAT}))
    .OUTPUT(z, TensorType({DT_FLOAT}))
    .OP_END_FACTORY_REG(AddCustom);
```

The second step is to implement the `ge::AddCustom` execution class in `add_custom.asc`. This class inherits from `EagerExecuteOp` and `ShapeInferOp`: `InferShape()` and `InferDataType()` are used during the graph compilation phase to infer output tensor information, and `Execute()` is used during the graph runtime phase to launch the Ascend C kernel function. `REG_AUTO_MAPPING_OP(AddCustom)` maps the `AddCustom` node in the GE graph to this execution class.

The third step is to create `ge::op::AddCustom` nodes in `graph.cpp` based on the operator prototype, and connect the `Data` input nodes and `AddCustom` computation nodes into a GE graph. For building a graph through operator prototypes, refer to [Building a Graph Through Operator Prototypes](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta1/programug/graphdevg/atlasag_25_0009.html).

At runtime, `Session::RunGraph` triggers execution of the entire GE graph.

```cpp
std::vector<ge::Tensor> inputs{BuildInputTensor(X_SCALE), BuildInputTensor(Y_SCALE)};
std::vector<ge::Tensor> outputs;
const auto runRet = session.RunGraph(GRAPH_ID, inputs, outputs);
```

## Build and Run

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
  # Configure and build the example
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;
  # Set the custom operator package path
  export ASCEND_CUSTOM_OPP_PATH="$(pwd)/output:${ASCEND_CUSTOM_OPP_PATH}"
  # Run the example
  ./demo
  ```

- Build option description

  | Option | Values | Description |
  | -------| -------| ------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The following output is observed upon success:

  ```bash
  Output: 0 0.5 1 1.5 ...
  Golden: 0 0.5 1 1.5 ...
  AddCustom GE triple-chevron sample success.
  ```
