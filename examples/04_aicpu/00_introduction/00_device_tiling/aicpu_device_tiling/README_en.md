# AI CPU Operator Tiling Offload Sample Introduction

## Overview

This sample demonstrates tiling offload computation using AI CPU operators.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|----------------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series/Atlas A3 Inference Series | >= CANN 9.0.0 |
| Atlas A2 Training Series/Atlas A2 Inference Series | >= CANN 9.0.0 |

## Directory Structure

```
├── aicpu_device_tiling
│   ├── CMakeLists.txt                     // Build project file
│   ├── aicore_kernel.asc                  // AI Core operator implementation
│   ├── kernel_args.h                      // Tiling struct header file
│   ├── main.asc                           // AI CPU and AI Core operator calls
│   ├── aicpu_tiling.aicpu                 // AI CPU operator implementation
│   └── README.md                          // Sample documentation
```

## Sample Description
- In main.asc, both AI CPU operators and AI Core operators are invoked using the kernel launch operator <<<...>>>. The AI CPU operator passes the tiling computation results to the AI Core operator.
- The AI CPU operator and AI Core operator are launched on different streams, namely aicpu_stream and aicore_stream in the sample. Events record tasks dispatched on streams. Use aclrtRecordEvent to record an event in the specified stream, and use aclrtStreamWaitEvent to block the specified stream until the specified event completes.

## Compilation and Execution
Follow these steps in the sample root directory.

- Configure environment variables  
  Configure environment variables according to the CANN development kit package [installation method](../../../../../docs/en/quick_start.md#prepare&install).
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. If not specified, it defaults to `/usr/local/Ascend`.

- Sample execution

  Execute the following commands in the sample directory.
  ```bash
  mkdir -p build && cd build;      # Create and enter build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Compile project
  ./demo                           # Execute the generated executable program
  ```

- Compilation options

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU Architecture: dav-2201 corresponds to Atlas A2 Training Series/Atlas A2 Inference Series and Atlas A3 Training Series/Atlas A3 Inference Series, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The following result indicates successful execution:
  Note that `__mix__(1, 2)` launches 1 Cube execution unit and 2 Vector execution units, so the `Hello World` log prints 3 times.

  ```bash
  MyAicpuKernel inited
  MyAicpuKernel inited type 1 mode 2 len 4 end!
  Hello World: int mode 2 len 4 m 10.
  Hello World: int mode 2 len 4 m 10.
  Hello World: int mode 2 len 4 m 10.
  ```
