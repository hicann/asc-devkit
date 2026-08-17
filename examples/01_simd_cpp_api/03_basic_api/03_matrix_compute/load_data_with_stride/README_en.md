# LoadDataWithStride Data Transfer Example in Matrix Multiplication

## Overview

This example introduces the usage scenarios and methods of the LoadDataWithStride instruction in matrix multiplication. Combined with SetLoadDataRepeatWithStride, it can transfer 2D A and B matrices from L1 to L0A/L0B, where A and B represent the left and right input matrices of matrix multiplication respectively.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```
├── load_data_with_stride
│   ├── figures                     // Illustrations
│   ├── scripts
│   │   ├── gen_data.py             // Script for generating input data and ground truth data
│   │   └── verify_result.py        // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt              // Build project file
│   ├── data_utils.h                // Data read/write functions
│   ├── load_data_with_stride.asc   // Ascend C example implementation and invocation example
│   └── README.md                   // Example documentation
```

## Example Description

The transpose capabilities and supported data types of the LoadDataWithStride instruction for 2D matrices are as follows:

- Supported data types: int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/half/bfloat16_t/int32_t/uint32_t/float.
- The enTranspose parameter takes effect only when the destination address is on L0A and the data type is B8/B16/B32, determining whether to enable the transpose function.
  When the destination address is on L0B, transpose is performed automatically and the enTranspose parameter is ignored.

Since this example does not currently support the int4b_t input data type, it demonstrates the following six LoadDataWithStride usage scenarios in matrix multiplication:

### LoadDataWithStride Interface Scenario Table

| scenarioNum | Input Data Type | A Matrix Transpose | B Matrix Transpose |
| --- | --- | --- | --- |
| 1 | int8_t | No transpose | No transpose |
| 2 | int8_t | Transpose | No transpose |
| 3 | half | No transpose | No transpose |
| 4 | half | Transpose | No transpose |
| 5 | float | No transpose | No transpose |
| 6 | float | Transpose | No transpose |

### Using LoadDataWithStride to Transfer 2D Format Data for Matrix Computation

LoadDataWithStride is essentially used to perform Image to Column expansion on Feature Maps in NC1HWC0 format, and then select specified data blocks from the expanded 2D matrix and transfer them to the corresponding memory locations.
Based on the img2col process performed by the `LoadDataWithStride` instruction, it can be seen that when N=1, filterW/H=1, padding=0, strideW/H=1, and dilation=1, it can be used for 2D matrix transfer. The diagram is shown below:

<div align="center">
  <img src="figures/load_data_with_stride_2d.png" width="800"><br>
  Figure 1: half data type, L1 -> L0A, non-transpose, diagram of using LoadDataWithStride for 2D matrix transfer
</div>

Taking the left matrix as an example, as shown in the following code: l1W = CeilAlign(m, BLOCK_CUBE), corresponding to M of the left matrix on L1; channelSize = CeilAlign(k, c0Size), corresponding to K of the left matrix on L1.

```cpp
// LoadData3DParamsV2: describes L1 to L0 data transfer parameters (2D data transfer)
AscendC::LoadData3DParamsV2<T> loadDataParams;
loadDataParams.l1H = 1;                               // Source operand height, fixed to 1 for 2D data transfer
loadDataParams.l1W = CeilAlign(m, BLOCK_CUBE);        // Source operand width, corresponding to matrix M
loadDataParams.channelSize = CeilAlign(k, c0Size);    // Number of channels, corresponding to matrix K
loadDataParams.kExtension = CeilAlign(k, c0Size);     // Destination operand width transfer length (K direction)
loadDataParams.mExtension = CeilAlign(m, BLOCK_CUBE); // Destination operand height transfer length (M direction)
loadDataParams.strideW = 1;                           // Filter W direction sliding stride, fixed to 1 for 2D data transfer
loadDataParams.strideH = 1;                           // Filter H direction sliding stride, fixed to 1 for 2D data transfer
loadDataParams.filterW = 1;                           // Filter width, fixed to 1 for 2D data transfer
loadDataParams.filterH = 1;                           // Filter height, fixed to 1 for 2D data transfer
loadDataParams.dilationFilterW = 1;                   // Filter W direction dilation factor
loadDataParams.dilationFilterH = 1;                   // Filter H direction dilation factor
loadDataParams.filterSizeW = false;                   // Whether to extend filterW (+256 when true)
loadDataParams.filterSizeH = false;                   // Whether to extend filterH (+256 when true)
loadDataParams.enTranspose = false;                   // Whether to transpose, effective only on L0A path with B8/B16/B32
loadDataParams.fMatrixCtrl = false;                   // Get FeatureMap attributes from the left matrix (currently only supports false)

// When calling LoadDataWithStride, this interface must be called to configure dstStride.
AscendC::LoadDataRepeatParamWithStride repeatParams;
repeatParams.dstStride = CeilDiv(CeilAlign(m, BLOCK_CUBE), BLOCK_CUBE); // Offset in the K axis direction of the output matrix
AscendC::SetLoadDataRepeatWithStride(repeatParams);
// Use LoadDataWithStride interface to transfer data from L1 to L0A
AscendC::LoadDataWithStride(a2Local, a1Local, loadDataParams);
```

## Build and Run

Run the following steps in the root directory of this example to build and run the operator.

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` (required) | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1` (default), `2`, `3`, `4`, `5`, `6` | Scenario number |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
