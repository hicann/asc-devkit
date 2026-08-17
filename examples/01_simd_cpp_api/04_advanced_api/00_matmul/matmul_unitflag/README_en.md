# Matmul UnitFlag Direct Invocation Example

## Overview
A Matmul example that enables the UnitFlag feature to parallelize the CUBE computation pipeline and the FIXPIPE data transfer-out pipeline. After enabling the UnitFlag feature, the Matmul API internally implements fine-grained synchronization between MMAD instructions and FIXPIPE instructions, thereby parallelizing the computation pipeline and the data transfer-out pipeline to improve example performance.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure
```
├── matmul_unitflag
│   ├── scripts
│   │   ├── gen_data.py                  // Script for generating input data and ground truth data
│   │   └── verify_result.py             // Ground truth verification file
│   ├── CMakeLists.txt                   // Build project file
│   ├── data_utils.h                     // Data read/write functions
│   ├── matmul_unitflag.asc              // Ascend C example implementation & invocation example
│   └── README.md                        // Example description document
```

## Example Description
- Example Function:  
  When the Matmul example calls the Matmul API for computation, it enables the UnitFlag feature for the MDL template by setting the enUnitFlag parameter in MatmulConfig to true. The Norm and IBShare templates enable the UnitFlag feature by default, while the MDL template does not enable it by default.

- Constraints
  - The UnitFlag feature only supports the Norm, IBShare, and MDL templates.
  - When the UnitFlag feature is enabled, the example does not support having both L0C transfer-out to Global Memory and L1 transfer-out to Global Memory pipelines simultaneously.
  - When the UnitFlag feature is enabled and the L0C accumulation feature is also enabled, multiple Iterate computations with a single GetTensorC output are not supported.

- Example Specifications:  
  In this example: M = 1024, N = 4096, K = 1024.
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="5" align="center">Matmul</td></tr>
  <tr><td rowspan="4" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">matmul_unitflag_custom</td></tr>
  </table>

- Example Implementation: 
  - Key Kernel Steps
    - When creating the Matmul object, customize the MatmulConfig parameters and set the enUnitFlag parameter to true to enable the UnitFlag feature, obtaining a custom Matmul object using the MDL template.
      ```cpp
      __aicore__ inline constexpr MatmulConfig GetUnitFlagCfg()
      {
          auto mmCfg = CFG_MDL;
      #ifdef ENABLE_UNITFLAG_FEATURE
          // enable UnitFlag feature
          mmCfg.enUnitFlag = true;
      #endif
          return mmCfg;
      }
      constexpr static MatmulConfig CFG_MDL_UNITFLAG = GetUnitFlagCfg();

      using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType>;
      using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType>;
      using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
      using BIAS_TYPE =  AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
      AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_MDL_UNITFLAG> matmulObj;
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

  The following output indicates a successful accuracy comparison.

  ```bash
  test pass!
  ```
