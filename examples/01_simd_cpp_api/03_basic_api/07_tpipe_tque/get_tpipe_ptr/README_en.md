# GetTPipePtr Example

## Overview

This example obtains the global TPipe pointer based on the GetTPipePtr interface and performs TPipe-related operations through this pointer.

> **Notice:** This example is only applicable to the TPipe and TQue programming model.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── get_tpipe_ptr
│   ├── CMakeLists.txt          // Build project file
│   ├── get_tpipe_ptr.asc       // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function

  When creating a TPipe object, the object initialization sets a globally unique TPipe pointer. This example calls the GetTPipePtr interface to obtain this pointer, so the kernel function can perform TPipe-related operations without explicitly passing the TPipe pointer. Below are code snippet examples showing invocation with and without the GetTPipePtr interface.

  **Calling the GetTPipePtr Interface**

  ```cpp
  template <uint32_t totalLength, uint32_t tileNum>
  __aicore__ inline void Process(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z)
  {
      // No TPipe variable
      // ...
      // Call GetTPipePtr to obtain the TPipe pointer and use it
      GetTPipePtr()->InitBuffer(inQueueX, BUFFER_NUM, tileLength * sizeof(float));
  }

  template <uint32_t totalLength, uint32_t tileNum>
  __global__ __vector__ void get_tpipe_ptr_custom(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z)
  {
      AscendC::TPipe pipe;
      // No need to explicitly pass the TPipe pointer
      Process<totalLength, tileNum>(x, y, z);
  }
  ```

  **Not Calling the GetTPipePtr Interface (Kernel Function Explicitly Passes TPipe Pointer)**

  ```cpp
  template <uint32_t totalLength, uint32_t tileNum>
  __aicore__ inline void Process(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z, AscendC::TPipe* pipe)
  {
      // TPipe pointer parameter required
      // ...
      // Use the passed TPipe pointer
      pipe->InitBuffer(inQueueX, BUFFER_NUM, tileLength * sizeof(float));
  }

  template <uint32_t totalLength, uint32_t tileNum>
  __global__ __vector__ void get_tpipe_ptr_custom(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z)
  {
      AscendC::TPipe pipe;
      // Need to explicitly pass the TPipe pointer
      Process<totalLength, tileNum>(x, y, z, &pipe);
  }
  ```

- Example specifications

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
      <td align="center">[8, 2048]</td>
      <td align="center">float</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">y</td>
      <td align="center">[8, 2048]</td>
      <td align="center">float</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">Example Output</td>
      <td align="center">z</td>
      <td align="center">[8, 2048]</td>
      <td align="center">float</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">Kernel Function Name</td>
      <td colspan="4" align="center">get_tpipe_ptr_custom</td>
    </tr>
  </table>

- Example implementation

  - Kernel implementation

    - Call the GetTPipePtr interface to obtain the global TPipe pointer.

    - Call the TPipe::InitBuffer interface to allocate memory space for TQue.

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
