# Matmul Constant Tiling Example
## Overview
A Matmul example with constant Tiling. Constant Tiling converts some or all Tiling parameters from variables to constant values during compilation, and the constant Tiling parameters are used during example execution.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure
```
├── matmul_constant_tiling
│   ├── scripts
│   │   ├── gen_data.py                // Script for generating input data and ground truth data
│   │   └── verify_result.py           // Ground truth verification file
│   ├── CMakeLists.txt                 // Build project file
│   ├── data_utils.h                   // Data read/write functions
│   ├── matmul_constant_tiling.asc     // Ascend C example implementation & invocation example
│   └── README.md                      // Example description document
```
## Example Description
- Example Function:  
  The MatmulConstantCustom example performs matrix multiplication and bias addition on the input A and B matrices, while using constant Tiling to implement static compilation on the Kernel side, deriving Tiling information before example execution. By implementing constant Matmul Tiling, Scalar operations in the example are reduced, improving example performance. In the constant Tiling scenario, the SingleShape set using the SetSingleShape interface on the Kernel side is the maximum shape for single-core computation at runtime, and the actual computation shape should be less than or equal to this shape.

- Example Specifications:  
  In this example: M = 128, N = 30720, K = 64.
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">Matmul</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">matmul_constant_tiling_custom</td></tr>
  </table>
- Example Implementation

  - Key Kernel Steps
    - Specific steps:
      - Configure the constant MatmulShapeParams to obtain the custom MatmulConfig.
        ```cpp
        constexpr int32_t MAX_M = 10000; // custom matmul kernel support max value of M Dim shape
        constexpr int32_t MAX_N = 10000; // custom matmul kernel support max value of N Dim shape
        constexpr int32_t MAX_K = 10000; // custom matmul kernel support max value of K Dim shape
        constexpr int32_t BASE_M = 128;  // BASEM * BASE_K * sizeof(typeA) <=L0A size
        constexpr int32_t BASE_N = 256;  // BASEN * BASE_K * sizeof(typeB) <=L0B size
        constexpr int32_t BASE_K = 64;   // BASEM * BASE_N * sizeof(typeC) <=L0C size
        constexpr MatmulShapeParams shapeParams = { MAX_M,
                                                      MAX_N,
                                                      MAX_K,
                                                      BASE_M,
                                                      BASE_N,
                                                      BASE_K };
        constexpr MatmulConfig CUSTOM_CFG = GetMMConfig<MatmulConfigMode::CONFIG_MDL>(shapeParams);
        ```
      - Obtain the constant Tiling information through the GetMatmulApiTiling interface.
        ```cpp
        auto constantCFG = AscendC::GetMatmulApiTiling<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE>(mmCFG);
        ```
      - Create the Matmul object using the custom MatmulConfig template.
        ```cpp
        using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, aType>;
        using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, bType>;
        using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, cType>;
        using BIAS_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, biasType>;
        constexpr static auto CONSTANT_CFG = GetCustomConstantCFG<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE>();
        AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CONSTANT_CFG> matmulObj;
        ```
      - Initialize and pass in the constant Tiling information.
      - Set the left matrix A, right matrix B, and Bias.
        ```cpp
        matmulObj.SetTail(tailM, tailN, shapes.k);
        matmulObj.SetTensorA(aGlobal, false);
        matmulObj.SetTensorB(bGlobal, false);
        if (shapes.isBias) {
            matmulObj.SetBias(biasGlobal);
        }
        ```
      - Complete the matrix multiplication operation.
        ```cpp
        matmulObj.IterateAll(cGlobal);
        ```
      - End the matrix multiplication operation.
        ```cpp
        matmulObj.End();
        ```

  - Key Tiling Steps
      - Ascend C provides a set of Matmul Tiling APIs to help users obtain the Tiling parameters needed for Matmul kernel computation. Simply pass in the A/B/C matrix information and call the API interfaces to obtain the relevant parameters in the TCubeTiling structure. Since Tiling is constant, only the core-splitting operation needs to be implemented in Tiling. Users can obtain the optimal multi-core splitting strategy through the multi-core Tiling interface. Other Tiling information is derived through constant deduction on the Kernel side, and the Kernel side no longer needs runtime Tiling information.

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
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution Result

  The following output indicates a successful accuracy comparison:
  ```bash
  test pass!
  ```
