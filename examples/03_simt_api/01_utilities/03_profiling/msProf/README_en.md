# Profiling Example Based on Gather Operator

## Overview

This example is based on the Gather operator and demonstrates how to collect on-board performance data using msOpProf. Users can quickly locate software and hardware performance bottlenecks of operators based on the output performance data, improving the efficiency of operator performance analysis.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version
- \>= CANN 9.0.0

## Directory Structure

```text
├── msProf
│   ├── CMakeLists.txt         // CMake build file
│   ├── gather.asc             // SIMT gather invocation example
│   └── README.md
```

## Operator Description
  The gather operator implements the function of obtaining 12288 rows of data at specified indices from a 2D vector with shape [100000,128]. For detailed function description, refer to the [Gather Operator Details](../../../00_introduction/01_gather/basic_gather/gather_2d/README_en.md) section.

## msOpProf Tool Introduction
The msOpProf tool is a single-operator performance analysis tool. It includes two usage modes: msopprof and msopprof simulator. This tool helps users identify anomalies in operator memory, operator code, and operator instructions, enabling comprehensive operator tuning. It currently supports performance data collection and automatic parsing based on different run modes (on-board or simulation) and different file formats (executable files or operator binary .o files).

- On-board Performance Collection

    Through on-board performance collection, the operator running time on the Ascend AI Processor can be directly measured. This method is suitable for quickly locating operator performance issues in an on-board environment.

    Perform operator tuning through msopprof based on the executable file demo:
    ```
    msopprof ./demo
    ```

    - Performance Data Description


      After the command completes, a folder named "OPPROF_{timestamp}_XXX" is generated in the default directory. The performance data folder structure example is as follows:

      ```bash
      ├──dump                       # Raw performance data, no user attention needed
      ├──ArithmeticUtilization.csv  # cube/vector instruction cycle ratio
      ├──L2Cache.csv                # L2 Cache hit rate
      ├──Memory.csv                 # UB, L1, and main memory read/write bandwidth
      ├──MemoryL0.csv               # L0A, L0B, and L0C read/write bandwidth
      ├──MemoryUB.csv               # Vector and Scalar to UB read/write bandwidth
      ├──OpBasicInfo.csv            # Operator basic information
      ├──PipeUtilization.csv        # Compute unit and transfer unit duration and ratio
      ├──ResourceConflictRatio.csv  # UB bank group, bank conflict, and resource conflict ratio across all instructions
      └──visualize_data.bin         # MindStudio Insight presentation file
      ```

Users can open the `visualize_data.bin` file with MindStudio Insight to visually view operator information, including operator basic information, inter-core load analysis, computation workload analysis, memory load analysis, and more. For more msOpProf tool usage, refer to the "Operator Tuning msOpProf" content in [Operator Development Tools](https://www.hiascend.com/document/redirect/CannCommercialToolOpDev).


## Build and Run

Run the following steps in the root directory of this example to build and execute the operator.
- Configure Environment Variables  

  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example

  Run the following commands in this example directory.

  ```bash
  mkdir -p build && cd build;           # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;   # Build the project
  msopprof ./demo                      # Perform operator tuning through msopprof based on the executable file demo
  ```

  Build Options Description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |

  The following output indicates that the accuracy verification is successful.
  ```
  [Success] Case accuracy is verification passed.
  ```
