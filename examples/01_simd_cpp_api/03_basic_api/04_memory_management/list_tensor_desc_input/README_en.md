# list_tensor_desc_input Example

## Overview

This example implements an AddN example based on the static Tensor programming model, using the ListTensorDesc structure to handle dynamic input parameters, combining static memory allocation with event synchronization mechanisms to achieve coordinated scheduling of data transfer and computation tasks.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── list_tensor_desc_input
│   ├── CMakeLists.txt              // Build project file
│   ├── list_tensor_desc_input.asc  // Ascend C example implementation & invocation example
│   └── README.md                   // Example documentation
```

## Example Description

- Example function

  This example demonstrates the usage of ListTensorDesc dynamic input parameters in the static Tensor programming model using Add computation, applicable to the following scenarios:
  1. Dynamic processing of multiple input parameters: supports dynamic combination operations of multiple input tensors in models (such as multi-branch network structures).
  2. Memory pipeline optimization: achieves pipelined parallelism of data transfer and computation through static double buffering and event synchronization mechanisms, reducing memory access latency.
  3. Multi-core parallel computation: adapts to the multi-core architecture of AI processors, supporting efficient distribution of large-scale tensor operations.

- Example specifications

  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">AddN</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x (dynamic input parameter srcList[0])</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y (dynamic input parameter srcList[1])</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">list_tensor_desc_input_custom</td></tr>
  </table>

- Example implementation

  - Kernel implementation

    The dynamic input feature means that the kernel function parameters use the ListTensorDesc structure to store input data information. Construct the ListTensorDesc data structure as shown below.
    ```cpp
    constexpr uint32_t SHAPE_DIM = 2;
    struct TensorDesc {
      uint32_t dim{SHAPE_DIM};
      uint32_t index;
      uint64_t shape[SHAPE_DIM] = {8, 2048};
    };

    constexpr uint32_t TENSOR_DESC_NUM = 2;
    struct ListTensorDesc {
      uint64_t ptrOffset;
      TensorDesc tensorDesc[TENSOR_DESC_NUM];
      uintptr_t dataPtr[TENSOR_DESC_NUM];
    } inputDesc;
    ```
    Combine the allocated Tensor parameters into a ListTensorDesc data structure as shown below.
    ```cpp
    inputDesc = {(1 + (1 + SHAPE_DIM) * TENSOR_DESC_NUM) * sizeof(uint64_t),
                {xDesc, yDesc},
                {(uintptr_t)xDevice, (uintptr_t)yDevice}};
    ``` 
    Parse the corresponding parameters according to the passed data format as shown below.

    ```cpp
    AscendC::ListTensorDesc keyListTensorDescInit((__gm__ void*)srcList);
    __gm__ uint8_t* x = (__gm__ uint8_t*)keyListTensorDescInit.GetDataPtr<__gm__ uint8_t>(0);
    __gm__ uint8_t* y = (__gm__ uint8_t*)keyListTensorDescInit.GetDataPtr<__gm__ uint8_t>(1);
    ```

  - Invocation implementation

    Use the kernel invocation operator <<<>>> to call the kernel function.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables

  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project, default npu mode
  ./demo                           # Run the compiled executable to execute the example
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  
  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The following execution result indicates that the accuracy comparison is successful:
  ```bash
  [Success] Case accuracy verification passed.
  ```
