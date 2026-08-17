# Mx Matmul Scale Multi-Cache Direct Invocation Example

## Overview

An MxMatmul example that enables multi-cache for quantization scale matrix data on the L1 Buffer in MXFP4/MXFP8 data format. Enabling scale multi-cache reduces redundant MTE2 transfers, thereby improving performance.

Taking the left quantization scale matrix scaleA with K-direction multi-cache and a multiplier of scaleFactorK as an example: during the MTE2 transfer from GM to A1 (L1 Buffer), matrix A transfers stepM * stepK base blocks at once, while the scaleA matrix transfers stepM * (scaleFactorK * stepK) base blocks at once and caches them in A1. During subsequent Iterate computation, the cached scaleA data sequentially performs broadcast multiplication with different data in matrix A.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure
```
├── matmul_mx_scale_cache
│   ├── scripts
│   │   ├── gen_data.py            // Script for generating input data and ground truth data
│   │   └── verify_result.py       // Ground truth verification file
│   ├── CMakeLists.txt             // Build project file
│   ├── data_utils.h               // Data read/write functions
│   ├── matmul_mx_scale_cache.asc  // Ascend C example implementation & invocation example
│   └── README.md                  // Example description document
```

## Example Description

- Example Function:  
  The mxTypePara parameter in Matmul Tiling represents scale multi-cache. Specifically:  
  - mxTypePara[0:7] represents the ratio coefficient scaleFactorKa of the data load amount between scaleA and matrix A in the K direction.
  - mxTypePara[8:15] represents the ratio coefficient scaleFactorKb of the data load amount between scaleB and matrix B in the K direction.
  - mxTypePara[16:23] represents the ratio coefficient scaleFactorM of the data load amount between scaleA and matrix A in the M direction.
  - mxTypePara[24:31] represents the ratio coefficient scaleFactorN of the data load amount between scaleA and matrix A in the N direction.
  
  For example, if tilingData.mxTypePara = 0x01010104, then scaleFactorKa = 4, indicating that 4x cache is enabled for scaleA in the K direction.

- Constraints
  - For the scaleA matrix, M-direction multi-cache is only allowed when the K direction of scaleA is fully loaded on L1.
  - For the scaleB matrix, N-direction multi-cache is only allowed when the K direction of scaleB is fully loaded on L1.

- Example Specifications:  
  In this example: M = 32, N = 128, K = 128, scaleK = 4. scaleK is the result of K divided by 32, which is 4.
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="5" align="center">MatmulMxTypeParaCustom</td></tr>
  <tr><td rowspan="6" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">fp8_e5m2_t</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">scaleA</td><td align="center">[M, scaleK]</td><td align="center">fp8_e8m0_t</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">fp8_e5m2_t</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">scaleB</td><td align="center">[scaleK, N]</td><td align="center">fp8_e8m0_t</td><td align="center">ND</td><td align="center">true</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">matmulMxTypeParaCustom</td></tr>
  </table>
- Example Implementation: 
  - Key Kernel Steps
      - Create the Matmul object: use MatmulTypeWithScale to define the parameter type information of A, scaleA, B, and scaleB, including memory logical position, data format, data type, and transpose information.
        ```cpp
        typedef AscendC::MatmulTypeWithScale<AscendC::TPosition::GM, AscendC::TPosition::GM, CubeFormat::ND, fp8_e5m2_t, false, AscendC::TPosition::GM, CubeFormat::ND, false> aType;
        typedef AscendC::MatmulTypeWithScale<AscendC::TPosition::GM, AscendC::TPosition::GM, CubeFormat::ND, fp8_e5m2_t, false, AscendC::TPosition::GM, CubeFormat::ND, true> bType;
        typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
        typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
        // When defining the matmul object, pass MatmulWithScalePolicy to enable the MxMatmul template policy
        AscendC::Matmul<aType, bType, cType, biasType, CFG_MDL, AscendC::MatmulCallBackFunc<nullptr, nullptr, nullptr>, AscendC::Impl::Detail::MatmulWithScalePolicy> matmulObj;
        ```
      - Set the left matrix A and left quantization scale matrix scaleA, right matrix B and right quantization scale matrix scaleB, and Bias.
        ```cpp
        matmulObj.SetTensorA(aGlobal, isTransA);
        matmulObj.SetTensorB(bGlobal, isTransB);
        matmulObj.SetTensorScaleA(asGlobal, isTransScaleA);
        matmulObj.SetTensorScaleB(bsGlobal, isTransScaleB);

        if (tiling.isBias) {
            matmulObj.SetBias(biasGlobal);
        }
        ```

  - Key Tiling Steps
    - Create a Tiling object: use SetMadType to enable the Mx feature, use SetScaleAType to set ScaleA information, and use SetScaleBType to set scaleB information.
      ```cpp
      cubeTiling.SetMadType(matmul_tiling::MatrixMadType::MXMODE);
      cubeTiling.SetScaleAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, false);
      cubeTiling.SetScaleBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, true);
      if (cubeTiling.GetTiling(tilingData) == -1) {
          std::cout << "Generate tiling failed." << std::endl;
          return {};
      }
      // Bits 0-6 represent the ratio coefficient of the data load amount between scaleA and matrix A in the K direction, bits 8-14 represent the ratio coefficient between scaleB and matrix B in the K direction. 260 represents binary 0000 0001 0000 0100, using 100 in bits 0-6 to enable 4x cache.
      tilingData.mxTypePara = 16843012;
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
  ./demo                       # Run the compiled executable to execute the example
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
