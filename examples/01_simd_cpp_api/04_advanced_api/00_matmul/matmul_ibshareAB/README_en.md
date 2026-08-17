# Matmul IBShareAB Feature Example

## Overview

An example that calls the Matmul high-level API to enable the IBShare feature, reusing the same matrix A or matrix B data on the L1 Buffer. This example covers the scenario where both matrix A and matrix B are reused simultaneously. Enabling this feature reduces data transfer overhead. Two scenarios are supported, selected through the CMake build parameter `SCENARIO_NUM`.
    <table>
 	<tr>
 		<td>scenarioNum</td>
 		<td>Scenario Type</td>
 	</tr>
 	<tr>
 		<td>1</td>
 		<td>Enable AB matrix IBShare (A and B matrices are not split)</td>
 	</tr>
 	<tr>
 		<td>2</td>
 		<td>Disable AB matrix IBShare (A and B matrices are split along the K axis)</td>
 	</tr>
 </table>

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── matmul_ibshareAB
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Ground truth verification file
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── figures                 // Illustrations
│   ├── matmul_ibshareAB.asc    // Ascend C example implementation & invocation example
│   └── README.md               // Example description document
```

## Example Description

- Example Function:  
  When calling the Matmul high-level API, enable the IBShare feature for both A and B matrices. During computation, the two AIVs corresponding to the same AIC use the same A and B matrix data on the L1 Buffer in each iteration.

  When both matrix A and matrix B have IBShare enabled, the K column is not split for computation. When neither has IBShare enabled, computation is split along the K column. By comparing the execution time of the two scenarios, the performance improvement of this feature can be observed.
  Data processing illustration for the AB matrix IBShare enabled scenario (A and B matrices are not split): ![alt text](./figures/matmul_ABshare.png)  
  Data processing illustration for the AB matrix IBShare disabled scenario (A and B matrices are split): ![alt text](./figures/matmul_noABshare.png)  


- Example Specifications:  
  In this example: M = 128, N = 256, K = 384.
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">Matmul</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">matmul_ABshare_custom</td></tr>
  </table>

- Example Implementation: 

  - Key Kernel Steps  
    - Create the Matmul object:  
        Configure the A and B matrix IBShare parameters as true or false based on the SCENARIO_NUM build option in CMakeLists.
        ```cpp
        #if SCENARIO_NUM == 1
        constexpr bool isABshare = true;
        #else
        constexpr bool isABshare = false;
        #endif

        Matmul<
            MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType, false, LayoutMode::NONE, isABshare>,
            MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType, false, LayoutMode::NONE, isABshare>,
            MatmulType<AscendC::TPosition::VECIN, CubeFormat::ND, CType>> matmulObj;
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
  SCENARIO_NUM=1                                                                # Set the scenario number
  mkdir -p build && cd build;                                                   # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # Build the project, npu mode by default
  python3 ../scripts/gen_data.py                                                # Generate test input data
  ./demo                                                                        # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin       # Verify whether the output is correct and confirm the algorithm logic
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=1 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=1 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes or scenarios. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build Option Description

  | Option | Values | Description |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU run, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`, `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1` (default), `2` | Scenario number: 1=Enable AB matrix IBShare, 2=Disable AB matrix IBShare |

- Execution Result

  The following output indicates a successful accuracy comparison:
  ```bash
  test pass!
  ```
