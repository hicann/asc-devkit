# CPU Debug Direct Invocation Example Description

## Overview

This example uses Add computation as the carrier to demonstrate the Ascend C CPU Debug workflow, including CPU domain compilation, executable program execution, entering debug mode through GDB, and setting breakpoints, single-stepping, and viewing variables in kernel functions. This example helps developers debug Ascend C kernel function logic and locate issues locally without relying on NPU execution.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|----------------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── 06_cpu_debug
│   ├── CMakeLists.txt          // Build project file
│   ├── cpu_debug.asc           // Ascend C example implementation & invocation example
│   └── README.md               // Example description document
```

## Example Description

- CPU Debug introduction:
  The CPU Debug feature supports debugging the runtime state during CPU execution, primarily through the GDB tool. GDB debugging supports common debugging operations such as setting breakpoints, viewing registers and memory state, single-stepping, and viewing the call stack.

- Example introduction:
  The Add implementation in this example is consistent with the [Add basic example](../../00_introduction/01_add/add/README_en.md), using `float` type by default. Each block processes `2048` data elements. The workflow is: `DataCopy` input to UB, execute `Add`, then `DataCopy` the result back to GM.


## Build and Run

Run the following steps in the root directory of this example to build and run the example.
- Configure environment variables
  Configure environment variables based on the [installation method](../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;
  ./cpu_debug
  ```
  Select the corresponding `CMAKE_ASC_ARCHITECTURES` parameter based on the NPU hardware architecture being tested.

- Build options

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `cpu` | Set to `cpu` to enable CPU domain compilation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`, `dav-3510` | Specify the NPU architecture version. CMake configures the corresponding CPU debug dependency libraries based on this value. `dav-2201` corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; `dav-3510` corresponds to Ascend 950PR/Ascend 950DT |

- Execution result
  The execution result is as follows, indicating successful execution.
  ```bash
  [Success] Case accuracy is verification passed.
  ```
- Enter GDB debug mode
  > **Note:** GDB version 12.1 or later is required.
  
  The compiled CPU domain executable supports debugging through GDB. GDB supports common debugging operations such as setting breakpoints, viewing registers and memory state, single-stepping, and viewing the call stack. Add "gdb --args" before "./cpu_debug" in the above command and run the command again to enter GDB mode.
  ```bash
  gdb --args ./cpu_debug
  ```
  CPU Debug simulates NPU execution logic by launching a separate child process for each kernel function. Therefore, when debugging with GDB, set `follow-fork-mode` to let GDB track the child process in order to debug at breakpoints inside kernel functions. After entering GDB, first set the child process tracking mode:

  ```text
  (gdb) set follow-fork-mode child
  ```

  Then debug as needed. Common operations:

  ```text
  # Set a breakpoint at the kernel function entry point
  (gdb) break add_custom

  # Run the program
  (gdb) run

  # Single-step execution
  (gdb) next

  # Continue execution to the next breakpoint
  (gdb) continue
  ```
