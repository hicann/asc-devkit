# Mx Matmul NZ Input Direct Invocation Example

## Overview

An MxMatmul example that uses user-defined TSCM and VECOUT input in MXFP4/MXFP8 data format.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure
```
├── matmul_mx_ub_tscm_nz
│   ├── scripts
│   │   ├── gen_data.py                       // Script for generating input data and ground truth data
│   │   └── verify_result.py                  // Ground truth verification file
│   ├── CMakeLists.txt                        // Build project file
│   ├── data_utils.h                          // Data read/write functions
│   ├── matmul_mx_ub_tscm_nz.asc              // Ascend C example implementation & invocation example
│   └── README.md                             // Example description document
```

## Example Description

- Example Function:  
  When the MatmulMxUbTscmNzCustom example calls the Matmul API for computation, the memory logical positions of matrices A and B use VECOUT, the memory logical positions of scaleA and scaleB matrices use TSCM, and all 4 input matrices are in NZ format. The left quantization scale matrix is multiplied with the left matrix, the right quantization scale matrix is multiplied with the right matrix, and matrix multiplication is performed on the two products.

- Example Specifications:  
  In this example: M = 64, N = 128, K = 128, scaleK = 4. scaleK is the result of K divided by 32, which is 4.
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="5" align="center">MatmulMxUbTscmNzCustom</td></tr>
  <tr><td rowspan="6" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">fp4x2_e1m2_t</td><td align="center">NZ</td><td align="center">false</td></tr>
  <tr><td align="center">scaleA</td><td align="center">[M, scaleK]</td><td align="center">fp8_e8m0_t</td><td align="center">NZ</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">fp4x2_e1m2_t</td><td align="center">NZ</td><td align="center">false</td></tr>
  <tr><td align="center">scaleB</td><td align="center">[scaleK, N]</td><td align="center">fp8_e8m0_t</td><td align="center">NZ</td><td align="center">true</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">matmulMxUbTscmNzCustom</td></tr>
  </table>
- Example Implementation: 
  - Key Kernel Steps
    - Create the Matmul object: use MatmulTypeWithScale to enable scaleA and scaleB, set the memory logical positions of the left and right matrices to VECOUT, set the memory logical positions of the left and right quantization scale matrices to TSCM, set the physical layout format of all input matrix data to NZ, and set the SCALE_ISTRANS parameter of scaleB to true.
      ```cpp
      using aType = AscendC::MatmulTypeWithScale<AscendC::TPosition::VECOUT, AscendC::TPosition::TSCM, CubeFormat::NZ, fp4x2_e1m2_t, false, AscendC::TPosition::GM, CubeFormat::NZ, false, AscendC::TPosition::GM>;
      using bType = AscendC::MatmulTypeWithScale<AscendC::TPosition::VECOUT, AscendC::TPosition::TSCM, CubeFormat::NZ, fp4x2_e1m2_t, false, AscendC::TPosition::GM, CubeFormat::NZ, true, AscendC::TPosition::GM>;
      using cType = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float>;
      using biasType = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float>;
      // When defining the matmul object, pass MatmulWithScalePolicy to enable the MxMatmul template policy
      AscendC::Matmul<aType, bType, cType, biasType, CFG_MDL, AscendC::MatmulCallBackFunc<nullptr, nullptr, nullptr>, AscendC::Impl::Detail::MatmulWithScalePolicy> matmulObj;
      ```
    - Set the left matrix A and left quantization scale matrix scaleA, right matrix B and right quantization scale matrix scaleB, and Bias.
      ```cpp
      // SetTensorA
      pipe->InitBuffer(leftMatrixQue, 1, tiling.singleCoreM * tiling.singleCoreK);
      bufferLeft = leftMatrixQue.AllocTensor<fp4x2_e1m2_t>();
      DataCopy(bufferLeft, aGlobal, tiling.singleCoreM * tiling.singleCoreK);
      AscendC::PipeBarrier<PIPE_ALL>();
      matmulObj.SetTensorA(bufferLeft, isTransA);
      
      // SetTensorB
      pipe->InitBuffer(rightMatrixQue, 1, tiling.singleCoreK * tiling.singleCoreN);
      bufferRight = rightMatrixQue.AllocTensor<fp4x2_e1m2_t>();
      DataCopy(bufferRight, bGlobal, tiling.singleCoreK * tiling.singleCoreN);
      AscendC::PipeBarrier<PIPE_ALL>();
      matmulObj.SetTensorB(bufferRight, isTransB);
      
      // SetTensorScaleA
      pipe->InitBuffer(qidMxA1, 1, alignSingleCoreM * alignSingleCoreK / 32);
      bufferLeftScale = qidMxA1.AllocTensor<fp8_e8m0_t>();
      DataCopy(bufferLeftScale, asGlobal, tiling.singleCoreM * tiling.singleCoreK / 32);
      AscendC::PipeBarrier<PIPE_ALL>();
      matmulObj.SetTensorScaleA(bufferLeftScale, isTransScaleA);

      // SetTensorScaleB
      pipe->InitBuffer(qidMxB1, 1, alignSingleCoreN * alignSingleCoreK / 32);
      bufferRightScale = qidMxB1.AllocTensor<fp8_e8m0_t>();
      DataCopy(bufferRightScale, bsGlobal, tiling.singleCoreK * tiling.singleCoreN / 32);
      AscendC::PipeBarrier<PIPE_ALL>();
      matmulObj.SetTensorScaleB(bufferRightScale, isTransScaleB);

      if (tiling.isBias) {
          matmulObj.SetBias(biasGlobal);
      }
      ```

  - Key Tiling Steps
    - Create a Tiling object: use SetMadType to enable the Mx feature, use SetScaleAType to set scaleA information, and use SetScaleBType to set scaleB information.
      ```cpp
      cubeTiling.SetAType(matmul_tiling::TPosition::VECOUT, matmul_tiling::CubeFormat::NZ,
          matmul_tiling::DataType::DT_FLOAT8_E5M2, isAtrans);
      cubeTiling.SetBType(matmul_tiling::TPosition::VECOUT, matmul_tiling::CubeFormat::NZ,
          matmul_tiling::DataType::DT_FLOAT8_E5M2, isBtrans);
      cubeTiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND,
          matmul_tiling::DataType::DT_FLOAT);
      cubeTiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND,
          matmul_tiling::DataType::DT_FLOAT);
      cubeTiling.SetScaleAType(matmul_tiling::TPosition::TSCM, matmul_tiling::CubeFormat::NZ, isScaleATrans);
      cubeTiling.SetScaleBType(matmul_tiling::TPosition::TSCM, matmul_tiling::CubeFormat::NZ, isScaleBTrans);
      cubeTiling.SetMadType(matmul_tiling::MatrixMadType::MXMODE);
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
  mkdir -p build && cd build;   # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;             # Build the project, npu mode by default
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                        # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output is correct and confirm the algorithm logic
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build Option Description

  | Option | Values | Description |
  | ----------------| -----------------------------| ---------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU run, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution Result

  The following output indicates a successful accuracy comparison:
  ```bash
  test pass!
  ```
