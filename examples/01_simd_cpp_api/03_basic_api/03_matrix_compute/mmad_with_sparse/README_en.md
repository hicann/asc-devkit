# mmad_with_sparse Example

## Overview

This example introduces the basic API MmadWithSparse invocation example.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── mmad_with_sparse
│   ├── scripts
│   │   ├── gen_data.py             // Script for generating input data and ground truth data
│   │   └── verify_result.py        // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt              // Build project file
│   ├── data_utils.h                // Data read/write functions
│   ├── mmad_with_sparse.asc        // Ascend C example implementation & invocation example
│   └── README.md                   // Example documentation
```

## Example Description

- Example function:
  This example implements a sparse matrix multiplication with [m, n, k] fixed at [128, 128, 64], using the Ascend C basic API MmadWithSparse interface. 2:4 sparseMatmul is a special type of matrix multiplication that requires at most 2 values to be non-zero in each consecutive group of 4 weights or activation values, with the remaining 2 forced to zero. The mathematical expression is:

  ```
  C = A * B
  ```
  where the original B matrix shape is [N, K], with at least 2 zero elements in every 4 elements, compressed to [N, K/2] through a 2:4 densification strategy before input. The B matrix must be stored in transposed form, that is, constructed and compressed as [N, K] input. The A and B matrix data types only support int8_t, and the index matrix data type is uint8_t.
- Example specifications:

<a name="table1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 1: Example Specifications</span></caption>
  <tr><td rowspan="4" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">a</td><td align="center">[128, 64]</td><td align="center">int8</td><td align="center">NZ</td></tr>
  <tr><td align="center">b</td><td align="center">[128, 32]</td><td align="center">int8</td><td align="center">ZN</td></tr>
  <tr><td align="center">idx</td><td align="center">[128, 8]</td><td align="center">uint8</td><td align="center">ZN</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[128, 128]</td><td align="center">int32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">mmad_with_sparse_custom</td></tr>
</table>

- Example implementation:
  The example implementation is divided into the following stages:
  - **CopyIn**: Transfer input data from Global Memory to Local Memory L1, and load the index matrix into L1. The Index fractal and B matrix fractal must be Zn fractals, meaning the B matrix on GM must be transposed, and the Index must be generated offline in Zn layout
  - **SplitB**: Use LoadDataWithSparse to transfer the B matrix and index matrix from L1 to L0B and the built-in index buffer
  - **SplitA**: Transfer the A matrix from L1 to L0A
  - **Compute**: Use MmadWithSparse to complete the sparse matrix multiplication operation, with computation results stored in Local Memory L0C
  - **CopyOut**: Transfer output data from L0C to the output GM in Global Memory

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
  cmake ..;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic is correct
  ```
  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim ..;make -j; # NPU simulation mode
  ```
  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default) | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products/Atlas A3 Training Series Products/Atlas A3 Inference Series Products |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```

## Data Generation and Verification Description

### gen_data.py Script Functions

1. **Construct sparse matrix B**: Generate a sparse matrix of the specified shape, with at least 2 zero elements in every 4-element block of each row
2. **Densification**: Densify the sparse matrix B through the 2:4 strategy to generate the dense matrix dense_B
3. **Index matrix generation**:
   - Generate index_matrix: record the relative positions of selected elements in each block (for NPU computation)
   - Generate index_mask_matrix: record the absolute indices of selected elements (for golden computation)
4. **Generate golden data**: Use the densified matrix and index matrix to compute the ground truth for sparse matrix multiplication
5. **Data format conversion**:
   - Convert the index matrix from uint8 to uint2 format
   - Perform ND to NZ block transposition on input matrices
