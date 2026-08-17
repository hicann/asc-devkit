# Sparse Matmul Direct Invocation Example

## Overview
A Matmul example for 4:2 sparse matrix multiplication (Sparse Matmul), which reduces memory usage and computation during matrix multiplication. Sparse Matmul skips zero elements in the sparse matrix B and only performs data transfer, storage, and computation on non-zero elements.  
In this scenario, the input left matrix A and the right matrix are sparse matrices. In the sparse matrix B, at least 2 out of every 4 elements are zero. Before performing Matmul computation, the user must densify matrix B with a 4:2 ratio, that is, filter out 2 zero elements from every 4 elements based on the original sparse matrix B, densifying B into a dense matrix. The Sparse Matmul scenario calls the Matmul API to complete the matrix multiplication of matrix A and the 4:2 densified matrix B.  
> **Note:** 4:2 sparse matrix multiplication (Sparse Matmul) currently only supports transposed matrix B.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure
```
├── matmul_sparse
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Ground truth verification file
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── matmul_sparse.asc       // Ascend C example implementation & invocation example
│   └── README.md               // Example description document
```
## Example Description
- Example Function:  
  The Matmul example calls the Matmul high-level API to perform matrix multiplication and add bias on the input left matrix A and the 4:2 densified right matrix B.   
  During the data preparation phase before computation, the densification of matrix B is completed through the ground truth data generation script, and the index matrix is obtained. When implementing Matmul computation, call the Matmul high-level API and pass in the 4:2 densified matrix B and the index matrix to complete the matrix multiplication for the Sparse Matmul scenario.

- Example Specifications:  
  In this example: M = 128, N = 7680, K = 64.
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="5" align="center">Matmul</td></tr>
  <tr><td rowspan="4" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">int8_t</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">int8_t</td><td align="center">ND</td><td align="center">true</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">int32_t</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">int32_t</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">matmul_sparse_custom</td></tr>
  </table>

- Example Implementation: 
  - Key Kernel Steps
    - When creating the Matmul object, define the parameter type information of matrix B through SparseMatmulType.
      ```cpp
      using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, ATYPE, false>;
      // Use SparseMatmulType to define the parameter type information of matrix B
      using B_TYPE = AscendC::SparseMatmulType<AscendC::TPosition::GM, AscendC::TPosition::GM, CubeFormat::ND, BType, true>;
      using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
      using BIAS_TYPE =  AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
      AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_MDL> matmulObj;
      ```
    - Set the index matrix.
      ```cpp
      matmulObj.SetSparseIndex(gm_index); // Set the index matrix gm_index generated during the densification of matrix B
      ```

  - Key Tiling Steps
    - Enable the Sparse Matmul sparse matrix computation scenario.
      ```cpp
      auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
      matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
      tiling.SetSparse(true); // Enable the Sparse Matmul sparse matrix computation scenario
      ```

  - Invocation Implementation  
    Use the kernel invocation operator <<<>>> to call the kernel function.

## Build and Run
Run the following steps in the root directory of this example to build and run the example.
- Configure Environment Variables  
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;    # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project, npu mode by default
  python3 ../scripts/gen_data.py    # Generate test input data
  ./demo                        # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin    # Verify whether the output is correct and confirm the algorithm logic
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build Option Description

  | Option | Values | Description |
  | ----------------| -----------------------------| ---------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU run, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default) | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products |

- Execution Result

  The following output indicates a successful accuracy comparison.

  ```bash
  test pass!
  ```
