# Transpose Class Example


## Overview

This example implements data transpose functionality based on the Transpose and TransDataTo5HD interfaces, detailing the usage in three scenarios: normal transpose, enhanced transpose, and 5HD format conversion. It covers three types of data conversion requirements: (1) transposing 16*16 2D matrix data blocks, (2) converting between [N,C,H,W] and [N,H,W,C] 4D matrix formats, and (3) converting NCHW format to NC1HWC0 format.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── transpose
│   ├── scripts
│   │   ├── gen_data.py         // Input data and ground truth data generation
│   │   └── verify_result.py    // Verify whether output data matches ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── transpose.asc           // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:
  The transpose class example implements matrix transpose functionality, supporting three scenarios: normal transpose, enhanced transpose, and 5HD format conversion:

  1. Normal transpose: supports transposing 16*16 2D matrix data blocks

  2. Enhanced transpose: specifies the transpose type through transposeParams, supports 16*16 2D matrix data block transpose, and supports conversion between [N,C,H,W] and [N,H,W,C] 4D matrices

  3. 5HD format conversion: supports 16*16 2D matrix data block transpose, supports converting [N,C,H,W] 4D format to [N,C1,H,W,C0] 5D format

- Scenario description:

  The example can switch between different scenarios through the compilation parameter `SCENARIO_NUM`. See the table below for parameter details:

  | Scenario Number | Scenario Name | Input Shape | Output Shape | Data Type | Input Format | Output Format | Transpose Type |
  |------|--------|------|------|--------|------|------|------|
  | 1 | Normal Transpose | [16,16] | [16,16] | half | ND | ND | / |
  | 2 | Enhanced Transpose | [3,3,2,8] | [3,2,8,3] | half | NCHW | NHWC | TRANSPOSE_NCHW2NHWC |
  | 3 | 5HD Format Conversion | [2,32,16,16] | [2,2,16,16,16] | half | NCHW | NC1HWC0 | / |

- Data format description:  
  Feature maps in convolutional neural networks are typically stored as 4D arrays. The 4D format is explained as follows:
    - N: Batch count.
    - H: Height, feature map height.
    - W: Width, feature map width.
    - C: Channels, feature map channels.

  Since data can only be stored linearly, these four dimensions have a corresponding order. Different deep learning frameworks store feature map data in different orders. For example, in TensorFlow, the order is [Batch, Height, Width, Channels], that is, NHWC.

  The 5HD format (NC1HWC0) is a data layout format specific to Ascend NPU, where:
    - N: Batch count.
    - C1: C1 = ceil(C / C0). If the result is not evenly divisible, round down.
    - H: Height, feature map height.
    - W: Width, feature map width.
    - C0: Equal to the size of the matrix computation unit in the AI Core. If the data type bit width is 32-bit or 16-bit, C0=16; if the data type bit width is 8-bit, C0=32. In this example, it is 16.

    The NHWC/NCHW -> NC1HWC0 conversion process: split the data along the C dimension into C1 portions of NHWC0/NC0HW, then arrange the C1 portions of NHWC0/NC0HW contiguously in memory as NC1HWC0.

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
  SCENARIO_NUM=1      # Set the scenario number (1=Normal Transpose, 2=Enhanced Transpose, 3=5HD Format Conversion)
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # Build the project
  python3 ../scripts/gen_data.py -scenario_num=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin  # Verify whether the output result is correct
  ```
  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  
  Examples:
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # CPU debug mode
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1` (default), `2`, `3` | Scenario number: 1 (Normal Transpose), 2 (Enhanced Transpose), 3 (5HD Format Conversion) |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.
  ```bash
  test pass!
  ```
