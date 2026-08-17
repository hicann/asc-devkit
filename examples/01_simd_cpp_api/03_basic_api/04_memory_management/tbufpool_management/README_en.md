# TBufPool Memory Management Example

## Overview

This example implements TBufPool memory resource management based on the TPipe::InitBufPool and TBufPool::InitBufPool interfaces, demonstrating TBufPool resource allocation, memory partitioning, memory reuse, and custom TBufPool usage. This example provides 3 different test scenarios.

> **Notice:** This example is only applicable to the TPipe and TQue programming model.

<table>
  <tr>
    <td align="center">scenarioNum</td>
    <td align="center">Scenario Name</td>
    <td align="center">Description</td>
  </tr>
  <tr>
    <td align="center">1</td>
    <td align="center">TBufPool Memory Reuse</td>
    <td align="center">Use TPipe::InitBufPool to initialize two TBufPools, specifying the second to reuse the start address and length of the first, achieving memory reuse</td>
  </tr>
  <tr>
    <td align="center">2</td>
    <td align="center">TBufPool Resource Subdivision</td>
    <td align="center">Use TBufPool::InitBufPool to further subdivide the entire resource block into smaller resource blocks, specifying reuse relationships between sub-resource pools</td>
  </tr>
  <tr>
    <td align="center">3</td>
    <td align="center">Custom TBufPool</td>
    <td align="center">Use the EXTERN_IMPL_BUFPOOL macro to assist users in customizing TBufPool classes, implementing non-contiguous memory block allocation and memory sharing</td>
  </tr>
</table>

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── tbufpool_management
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── tbufpool_management.asc // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function

  This example demonstrates various usage patterns of TBufPool memory resource management, including memory reuse, resource subdivision, and custom TBufPool.

- Example specifications

  **Scenario 1: TBufPool Memory Reuse**
  <table>
    <tr>
      <td align="center">Category</td>
      <td align="center">name</td>
      <td align="center">shape</td>
      <td align="center">data type</td>
      <td align="center">format</td>
    </tr>
    <tr>
      <td rowspan="2" align="center">Example Input</td>
      <td align="center">x</td>
      <td align="center">[3, 65536]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">y</td>
      <td align="center">[3, 65536]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">Example Output</td>
      <td align="center">z</td>
      <td align="center">[3, 65536]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">Kernel Function Name</td>
      <td colspan="4" align="center">tbufpool_management_custom</td>
    </tr>
  </table>

  **Scenario 2: TBufPool Resource Subdivision**
  <table>
    <tr>
      <td align="center">Category</td>
      <td align="center">name</td>
      <td align="center">shape</td>
      <td align="center">data type</td>
      <td align="center">format</td>
    </tr>
    <tr>
      <td rowspan="2" align="center">Example Input</td>
      <td align="center">x</td>
      <td align="center">[4, 32768]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">y</td>
      <td align="center">[4, 32768]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">Example Output</td>
      <td align="center">z</td>
      <td align="center">[4, 32768]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">Kernel Function Name</td>
      <td colspan="4" align="center">tbufpool_management_custom</td>
    </tr>
  </table>

  **Scenario 3: Custom TBufPool**
  <table>
    <tr>
      <td align="center">Category</td>
      <td align="center">name</td>
      <td align="center">shape</td>
      <td align="center">data type</td>
      <td align="center">format</td>
    </tr>
    <tr>
      <td rowspan="2" align="center">Example Input</td>
      <td align="center">x</td>
      <td align="center">[1, 65536]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">y</td>
      <td align="center">[1, 65536]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">Example Output</td>
      <td align="center">z</td>
      <td align="center">[1, 65536]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">Kernel Function Name</td>
      <td colspan="4" align="center">tbufpool_management_custom</td>
    </tr>
  </table>

- Example implementation

  - Kernel implementation

    **Memory Resource Management (Scenario Differences)**

    <table>
      <tr>
        <td align="center">scenarioNum</td>
        <td align="center">Memory Management Method</td>
        <td align="center">Implementation Description</td>
      </tr>
      <tr>
        <td align="center">1</td>
        <td align="center">TBufPool Memory Reuse</td>
        <td align="center">Call TPipe::InitBufPool to initialize tbufPool1 and tbufPool2, specifying tbufPool2 to reuse the start address and length of tbufPool1; call TBufPool::InitBuffer to allocate memory for TQue</td>
      </tr>
      <tr>
        <td align="center">2</td>
        <td align="center">TBufPool Resource Subdivision</td>
        <td align="center">Call TPipe::InitBufPool to initialize tbufPool0, call TBufPool::InitBufPool to subdivide tbufPool1 and tbufPool2, specifying tbufPool2 to reuse tbufPool1; call TBufPool::InitBuffer to allocate memory for TQue</td>
      </tr>
      <tr>
        <td align="center">3</td>
        <td align="center">Custom TBufPool</td>
        <td align="center">Use EXTERN_IMPL_BUFPOOL macro to implement custom TBufPool class MyBufPool; call TPipe::InitBufPool to allocate memory for MyBufPool, implement TQue and TBuf memory allocation through InitBuffer</td>
      </tr>
    </table>

    **Common Computation Flow**

    - Call the DataCopy basic API to transfer data from GM (Global Memory) to UB (Unified Buffer).
    - Call the Add interface to perform addition on two input tensors.
    - Call the DataCopy basic API to transfer computation results from UB (Unified Buffer) to GM (Global Memory).

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
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin   # Verify whether the output result is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  
  Examples:
  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`, `2`, `3` | Scenario number: 1=TBufPool memory reuse, 2=TBufPool resource subdivision, 3=Custom TBufPool |

- Execution result

  The following execution result indicates that the accuracy comparison is successful:
  ```bash
  test pass!
  ```
