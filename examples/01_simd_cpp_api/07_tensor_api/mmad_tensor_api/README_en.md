# mmad Inline Quantization Example Based on Tensor API

## Overview

This example implements dynamic Shape matrix multiplication and inline quantization computation based on the Tensor API programming paradigm. Matrix A is not transposed, matrix B is stored transposed, A/B matrix data type is int8, mmad accumulation type is int32, and output matrix C data type is half. The quantization coefficient quant is input in uint64_t format, scaling the int32 accumulation results in L0C during the Fixpipe phase and writing back to GM.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | > CANN 9.1.0 |

> **Note:** This example depends on CANN features that have not been officially released. Use the latest CANN master package.

## Directory Structure

```text
├── mmad_tensor_api
│   ├── scripts
│   │   ├── gen_data.py                 // Script for generating input data and ground truth data
│   │   └── verify_result.py            // Ground truth comparison file
│   ├── CMakeLists.txt                  // Build project file
│   ├── data_utils.h                    // Data read/write functions
│   ├── mmad_tensor_api.asc             // Ascend C example implementation and invocation example
│   └── README.md                       // Example documentation
```

## Example Description

- Example function:

  This example implements multi-core dynamic Shape Matmul and inline quantization functionality.

  1. Dynamic Matmul functionality

     The example constructs Tensor objects on GM, L1, L0A, L0B, L0C through Tensor API in the Kernel, uses the copy interface to complete block transfers from GM to L1 and L1 to L0, and performs matrix multiplication through the mmad interface. Matrix A is read in ND layout, matrix B is read in DN layout (stored as transposed [N, K] in input files). Shape parameters (M, N, K, singleCoreM, singleCoreN, singleCoreK) are passed to the Kernel at runtime, while baseM, baseN, baseK and stepM, stepN, stepK, ENABLE_UNITFLAG serve as compile-time template parameters determining L1/L0 Buffer allocation sizes and block stepping.

  2. Fixpipe inline quantization/dequantization functionality

     For specific input/output data types, Fixpipe supports performing data quantization operations on output C matrix elements when transferring computation results from L0C to GM, by configuring Fixpipe quantization mode and quantization parameters.
     Fixpipe provides two different granularities of inline quantization: per_tensor and per_channel.
      1. per_tensor: Quantize/dequantize the entire Tensor, with the Tensor having a unique scaling factor. This approach can reduce model storage and computation costs but decreases model accuracy.
      2. per_channel: Quantize/dequantize each channel of the Tensor individually, with the same scaling factor shared within a channel and different scaling factors between channels. This approach better preserves model accuracy but increases model storage and computation costs.

  3. ENABLE_UNITFLAG

     The example controls the unitFlag feature switch through the `ENABLE_UNITFLAG` template parameter, with default value true.
     For detailed unitFlag description, see [mmad unitFlag Feature Example](../../03_basic_api/03_matrix_compute/mmad_unitflag/README_en.md)

- Example specifications:

  In the kernel function direct invocation example, the default supported shape is: M = 1024, N = 1024, K = 256.

  <a name="table1"></a>
  <table border="2" align="center">
  <caption style="font-weight: normal;">
      <span style="font-weight: bold; font-size: 1.2em;">Table 1: Example Specifications</span></caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">Matmul with Quant</td></tr>
  <tr><td rowspan="4" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x (Matrix A)</td><td align="center">[M, K]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td align="center">y (Matrix B, transposed storage)</td><td align="center">[N, K]</td><td align="center">int8_t</td><td align="center">DN</td></tr>
  <tr><td align="center">quant</td><td align="center">[N]</td><td align="center">uint64_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z (Matrix C)</td><td align="center">[M, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">mmad_quant_custom</td></tr>
  </table>

  Compile-time template parameter defaults:

  | Parameter    | Default | Description                    |
  | :------ | :----- | :---------------------- |
  | BASE_M  | 256    | M dimension base block size       |
  | BASE_N  | 256    | N dimension base block size       |
  | BASE_K  | 64     | K dimension base block size       |
  | STEP_M  | 1      | M dimension L1 cache step         |
  | STEP_N  | 1      | N dimension L1 cache step         |
  | STEP_K  | 4      | K dimension L1 cache step         |
  | ENABLE_UNITFLAG  | true      | unitFlag feature switch         |

  Runtime dynamic parameter defaults:

  | Parameter         | Default | Description                       |
  | :----------- | :----- | :------------------------- |
  | m            | 1024   | Number of rows in Matrix A                |
  | n            | 1024   | Number of columns in Matrix B                |
  | k            | 256    | Number of columns in Matrix A / Number of rows in Matrix B    |
  | singleCoreM  | 256    | Single-core M dimension size              |
  | singleCoreN  | 128    | Single-core N dimension size              |
  | singleCoreK  | 256    | Single-core K dimension size              |
  | NUM_BLOCKS   | 32     | Number of Blocks launched by Kernel      |

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables
  Configure environment variables according to the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment. **Currently only [CANN master](../../../../docs/en/quick_start.md#cann-install) is supported**.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;   # Create and enter the build directory
  cmake ..;make -j;             # Build the project
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                        # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic is correct
  ```

  Example:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  | :--- | :----- | :--- |
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` (default) | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result
  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
