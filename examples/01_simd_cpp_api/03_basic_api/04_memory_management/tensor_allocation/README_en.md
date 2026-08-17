# tensor_allocation Example

## Overview

This example introduces the allocation and usage of GlobalTensor, LocalTensor, and RegTensor.

<table>
  <tr>
    <td align="center">Tensor Type</td>
    <td align="center">Usage</td>
    <td align="center">Allocation Method</td>
  </tr>
  <tr>
    <td align="center">GlobalTensor</td>
    <td>Stores global data in Global Memory (external storage)</td>
    <td>Created using constructor</td>
  </tr>
  <tr>
    <td align="center">LocalTensor</td>
    <td>Stores data in Local Memory (internal data) within AI Core</td>
    <td>Managed by TQue queue, LocalMemAllocator allocator, or created using constructor</td>
  </tr>
  <tr>
    <td align="center">RegTensor</td>
    <td>Basic computational unit in Reg programming, used for vector computation</td>
    <td>Created using constructor</td>
  </tr>
</table>

> **Note:** RegTensor is only applicable to Ascend 950PR/Ascend 950DT products.

This example includes 4 scenarios. Scenarios 1 to 3 demonstrate three allocation methods for LocalTensor, and scenario 4 demonstrates the allocation and usage of RegTensor. The detailed descriptions are as follows:

<table>
  <tr>
    <td align="center">scenarioNum</td>
    <td align="center">Scenario Name</td>
    <td align="center">Characteristics</td>
  </tr>
  <tr>
    <td align="center">1</td>
    <td align="center">Using TQue queue to manage LocalTensor</td>
    <td>Low difficulty, no need to manually set synchronization events, simpler programming interface</td>
  </tr>
  <tr>
    <td align="center">2</td>
    <td align="center">Using LocalMemAllocator to manage LocalTensor</td>
    <td>Medium difficulty, requires manual hardware event synchronization, more flexible synchronization<br>When the size of LocalTensor is a constant, you can use template parameters to set the size during allocation for better performance</td>
  </tr>
  <tr>
    <td align="center">3</td>
    <td align="center">Creating LocalTensor using LocalTensor constructor</td>
    <td>High difficulty, developers have complete manual control over memory layout, requires precise calculation to avoid address conflicts, and requires manual hardware event synchronization</td>
  </tr>
  <tr>
    <td align="center">4</td>
    <td align="center">Using RegTensor based on LocalTensor</td>
    <td>Directly operates on vector registers of the chip through Reg programming interfaces, achieving greater flexibility and better performance<br>For the LocalTensor part in Reg programming, scenarios 1 to 3 are all applicable<br>Only applicable to Ascend 950PR/Ascend 950DT products</td>
  </tr>
</table>

**Figure 1** Data flow between tensors

<img src="figures/Tensor流转图.png">

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── tensor_allocation
│   ├── CMakeLists.txt          // Build configuration file
│   ├── tensor_allocation.asc   // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example functionality:
  This example implements a vector addition operation (z = x + y) using Ascend C. It introduces three different methods to manage LocalTensor on UB (Unified Buffer), and RegBase programming for RegTensor addition operations. You can select different scenarios by setting the `SCENARIO_NUM` environment variable.

- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">ElementWise</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">tensor_allocate_custom</td></tr>
  </table>

- Example implementation:
  - Kernel implementation

    **Scenario 1 (Process1): Using TQue queue mechanism**
    - Initialize TQue queue through `TPipe::InitBuffer`, specifying queue depth and buffer size
    - Allocate LocalTensor using `TQue::AllocTensor`, and free LocalTensor using `TQue::FreeTensor`
    - Manage data flow through the queue's EnQue/DeQue mechanism
    
    **Scenario 2 (Process2): Using LocalMemAllocator allocator**
    - Create a `LocalMemAllocator<Hardware::UB>` allocator object
    - Call the `Alloc<T>` method to directly allocate LocalTensor of specified size
    - Requires manual invocation of `SetFlag/WaitFlag` for hardware event synchronization
    - When the buffer size tileSize is a constant, it is recommended to use the tileSize template parameter for better performance
    
    **Scenario 3 (Process3): Using address offset for manual management**
    - Manually calculate the starting address offset of each tensor in UB
    - Specify `TPosition` and starting address through LocalTensor constructor
    - Complete manual control over memory layout, requires developers to precisely calculate addresses to avoid conflicts
    - Requires manual invocation of `SetFlag/WaitFlag` for hardware event synchronization

    **Scenario 4 (Process4): Implementing RegTensor addition based on Reg programming**
    - The LocalTensor part is the same as scenario 2
    - Allocate RegTensor in __simd_vf__ and use RegTensor to complete the addition calculation

  - Invocation implementation
    Use the kernel invocation syntax <<<>>> to call the kernel function, starting 1 core for computation.

## Build and Run

Run the following steps in the root directory of this example to build and run it.

- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO_NUM=1  # Set scenario number
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;    # Build the project, default npu mode
  ./demo                           # Run the example
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`, `2`, `3`, `4` | Scenario number: 1 (using TQue queue mechanism), 2 (using LocalMemAllocator allocator), 3 (using address offset for manual management), 4 (implementing RegTensor addition based on Reg programming) |

  > **Notice:** When `SCENARIO_NUM` is set to `4`, only `CMAKE_ASC_ARCHITECTURES` set to `dav-3510` is supported.

- Execution result

  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```