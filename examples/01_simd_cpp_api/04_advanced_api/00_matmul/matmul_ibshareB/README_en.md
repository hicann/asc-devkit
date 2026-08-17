# Matmul IBShareB Direct Invocation Example

## Overview

An example that enables the IBShare feature to reuse the same matrix A or matrix B data on the L1 Buffer. This example covers the scenario where only matrix B is reused. Enabling this feature reduces data transfer overhead. Multiple scenarios are supported, selected through environment variables.
    <table>
	 	<tr>
	 		<td>scenarioNum</td>
	 		<td>Scenario Type</td>
	 	</tr>
	 	<tr>
	 		<td>1</td>
	 		<td>Default implementation, enable IBShare template</td>
	 	</tr>
	 	<tr>
	 		<td>2</td>
	 		<td>Enable pure Cube mode + IBShareB</td>
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
├── matmul_ibshareB
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Ground truth verification file
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── matmul_ibshareB.asc     // Ascend C example implementation & invocation example
│   └── README.md               // Example description document
```

## Example Description

- Example Function:  
  When calling the Matmul high-level API, enable the IBShare feature for matrix B. During computation, the two AIVs corresponding to the same AIC use the same B matrix data on the L1 Buffer in each iteration.

- Constraints:
  - When only matrix A or matrix B has IBShare enabled, the reused matrix must be fully loaded on the L1 Buffer.

- Example Specifications:  
  In this example: M = 64, N = 256, K = 384.
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="5" align="center">MatmulIBShareBCustom</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">matmul_ibshareb_custom</td></tr>
  </table>

- Example Implementation: 
  - Key Kernel Steps
    - Create the Matmul object and set the IBShare parameter of matrix B to true.
      - Method 1: Default implementation, use the default IBShare template CFG_IBSHARE_NORM to create the Matmul object.
        ```cpp
        #include "lib/matmul_intf.h"
    
        using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType>;
        using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType, false, LayoutMode::NONE, true>;
        using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
        using BIAS_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
        AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_IBSHARE_NORM> matmulObj;
        ```
      - Method 2: Pure Cube mode implementation. In the code that defines the Matmul object, set the ASCENDC_CUBE_ONLY macro, which must be set before #include "lib/matmul_intf.h".
        ```cpp
        #define ASCENDC_CUBE_ONLY // Set the ASCENDC_CUBE_ONLY macro
        #include "lib/matmul_intf.h"

        using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType>;
        using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType, false, LayoutMode::NONE, true>;
        using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
        using BIAS_TYPE =  AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
        AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_NORM> matmulObj;
        ```

  - Kernel Function  
    When using pure Cube mode, specify __cube__ at the entry point of the kernel function implementation.

  - Invocation Implementation  
    Use the kernel invocation operator <<<>>> to call the kernel function.
    ```cpp
    matmul_ibshareb_custom<<<tilingData.usedCoreNum / MIX_RATIO, 0, stream>>>(x1, x2, bias, y, workspaceDevice,
                          tilingDevice);       // Method 1: Non-pure Cube mode, SetDim sets the core count for AIV:AIC combination
    matmul_ibshareb_custom<<<tilingData.usedCoreNum, 0, stream>>>(x1, x2, bias, y, workspaceDevice,
                          tilingDevice);       // Method 2: Pure Cube mode, SetDim sets the AIC core count                          
    ```

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
  SCENARIO_NUM=1                                                                # Default implementation. Enable IBShare template
  mkdir -p build && cd build;                                                   # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # Build the project, npu mode by default
  python3 ../scripts/gen_data.py                                                # Generate test input data
  ./demo                                                                        # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin       # Verify whether the output is correct and confirm the algorithm logic
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  SCENARIO_NUM=1
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build Option Description

  | Option | Values | Description |
  | ----------------| -----------------------------| ---------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU run, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`, `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1` (default), `2` | Scenario number: 1=Default implementation, enable IBShare template, 2=Enable pure Cube mode + IBShareB |

- Execution Result

  The following output indicates a successful accuracy comparison:
  ```bash
  test pass!
  ```
