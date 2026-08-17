# msProf Profiling Example Based on Add

## Overview

This example uses Add computation as the carrier to demonstrate how to collect on-board performance data through `msopprof`. The example constructs input data and golden data directly on the host side, executes the Ascend C kernel function, and performs result verification within the process, without relying on additional data generation or verification scripts.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|----------------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── msprof
│   ├── CMakeLists.txt       // Build project file
│   ├── msprof.asc           // Ascend C example implementation and host-side invocation example
│   └── README.md            // Example description
```

## Example Description

- Add computation:

  This example uses `float` type by default to perform element-wise addition of two tensors:

  $$
  z = x + y
  $$

The shapes of inputs `x`, `y` and output `z` are all `[8, 2048]`. The host side constructs input data and golden data directly, copies the input data to the device side, launches the `add_custom` Kernel, which splits data by `blockNum = 8` and performs Add computation, and finally copies the result back to the host side for accuracy comparison.

## msOpProf Tool Introduction

The msOpProf tool is a single-operator performance analysis tool that includes two usage modes: `msopprof` and `msopprof simulator`. This tool collects performance data such as operator execution time, pipeline utilization, and memory access to help analyze operator performance bottlenecks.

- On-board performance collection

  Through on-board performance collection, you can directly measure the runtime of operators on the Ascend AI processor. Run the following command based on the executable `demo`:

  ```bash
  msopprof ./demo
  ```

  After the command completes, a folder named `OPPROF_{timestamp}_XXX` is generated in the default directory. The performance data folder structure is as follows:

  ```bash
  ├── dump                       # Raw performance data
  ├── ArithmeticUtilization.csv  # Cube/vector instruction cycle ratio
  ├── L2Cache.csv                # L2 Cache hit rate, affecting MTE2; plan data transfer logic to increase hit rate
  ├── Memory.csv                 # UB, L1, and main memory read/write bandwidth
  ├── MemoryL0.csv               # L0A, L0B, and L0C read/write bandwidth
  ├── MemoryUB.csv               # Vector and Scalar to UB read/write bandwidth
  ├── OpBasicInfo.csv            # Operator basic information
  ├── PipeUtilization.csv        # Compute unit and transfer unit duration and ratio
  ├── ResourceConflictRatio.csv  # UB bank group, bank conflict, and resource conflict ratio
  └── visualize_data.bin         # MindStudio Insight visualization file
  ```

For more msOpProf tool usage, refer to [MindStudio Tools](https://www.hiascend.com/document/redirect/CannCommercialToolOpDev) Operator Tuning (msOpProf).

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit on the current environment.

  > To use the msProf tool, install CANN commercial/community edition. For details, refer to [msOpProf Tool Installation Guide](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/install_guide/msopprof_install_guide.md).


  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;           # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;   # Build the project
  msopprof ./demo                      # Run performance profiling through msopprof based on the demo executable
  ```

- Build options

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: `dav-2201` corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; `dav-3510` corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  When accuracy comparison succeeds, the output is as follows:

  ```bash
  test pass!
  ```

  After `msopprof` completes, a performance data directory named `OPPROF_{timestamp}_XXX` is generated in the current directory, containing performance data files such as execution time, pipeline utilization, and memory access.
