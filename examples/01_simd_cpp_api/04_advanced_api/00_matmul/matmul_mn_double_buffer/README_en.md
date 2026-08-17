# Matmul M/N-Axis Pipeline Parallelism Example
## Overview
A Matmul example with M/N-axis pipeline parallelism. This feature applies to scenarios where the input matrix K is very small but M or N is very large, that is, singleCoreK<=baseK but singleCoreM is much larger than baseM or singleCoreN is much larger than baseN. Enabling M/N-direction pipeline parallelism may provide performance benefits.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure
```
├── matmul_mn_double_buffer
│   ├── scripts
│   │   ├── gen_data.py                     // Script for generating input data and ground truth data
│   │   └── verify_result.py                // Ground truth verification file
│   ├── CMakeLists.txt                      // Build project file
│   ├── data_utils.h                        // Data read/write functions
│   ├── matmul_mn_double_buffer.asc         // Ascend C example implementation & invocation example
│   └── README.md                           // Example description document
```
## Example Description
- Example Function:  
  This example calls the Matmul high-level API to perform matrix multiplication and bias addition on the input A and B matrices. When defining the Matmul object, pass the MatmulConfig that configures parameters such as scheduleType as a template parameter to enable M/N-axis pipeline parallelism. This example uses the MDL template with N-direction pipeline parallelism. For the Norm template or M-direction pipeline parallelism, refer to this example for implementation.

- Example Specifications:  
  In this example: M = 128, N = 7680, K = 16
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">Matmul</td></tr>
  <tr><td rowspan="4" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">matmul_mn_double_buffer_custom</td></tr>
  </table>
- Example Implementation
  - Key Kernel Steps
    - When creating the Matmul object, customize the MatmulConfig parameters, set MatmulConfigMode to CONFIG_MDL, set the scheduleType parameter to ScheduleType::OUTER_PRODUCT, and set the iterateOrder parameter to IterateOrder::ORDER_M to enable N-direction pipeline parallelism with the MDL template, obtaining a custom Matmul object using the MDL template.
      ```cpp
      constexpr static MatmulConfigMode configModeMDL = MatmulConfigMode::CONFIG_MDL;
      constexpr static MatmulFuncParams funcParamsOrderM{false, false, false, false, 0, IterateOrder::ORDER_M, ScheduleType::OUTER_PRODUCT, true, true};
      constexpr static MatmulConfig CFG_MDL_OUTER_PRODUCT_ORDER_M = GetMMConfig<configModeMDL>(funcParamsOrderM);

      using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, ATYPE, false>;
      using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType, false>;
      using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
      using BIAS_TYPE =  AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
      AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_MDL_OUTER_PRODUCT_ORDER_M> matmulObj;
      ```

  - Key Tiling Steps
    - Set custom MatmulConfig parameters to synchronize Kernel-side configured parameters such as scheduleType to the Tiling side.
      ```cpp
      matmul_tiling::MatmulConfigParams matmulConfigParams(1, false, matmul_tiling::ScheduleType::OUTER_PRODUCT,
          matmul_tiling::MatrixTraverse::FIRSTM, false);
      cubeTiling.SetMatmulConfigParams(matmulConfigParams);
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
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution Result

  The following output indicates a successful accuracy comparison:
  ```bash
  test pass!
  ```
