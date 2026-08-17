# Matmul Dequantization Direct Invocation Example

## Overview
A Matmul example with output dequantization, supporting both uniform coefficient dequantization mode and vector dequantization mode.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure
```
├── matmul_quant
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Ground truth verification file
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── matmul_quant.asc        // Ascend C example implementation & invocation example
│   └── README.md               // Example description document
```

## Example Description
- Example Function:  
  When the Matmul example calls the Matmul API for computation with int8_t type input, the computation result is output as half type with dequantization, supporting both uniform coefficient dequantization mode and vector dequantization mode. In this scenario, when the C matrix data is transferred from CO1 to Global Memory, a dequantization operation is performed, applying a uniform coefficient or vector to all values of the output matrix for dequantization.

- Example Specifications:  
  In this example: M = 1024, N = 1024, K = 1024.
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="5" align="center">Matmul</td></tr>
  <tr><td rowspan="4" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">int8_t</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">int8_t</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">int32_t</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">half</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">matmul_quant_custom</td></tr>
  </table>

- Example Implementation: 
  - Key Kernel Steps
    - Set dequantization parameters.  
      When the build option QUANT_MODE is 1, set the build macro CUSTOM_QUANT_VECTOR and compile to execute the vector dequantization mode.    
      Based on whether the macro CUSTOM_QUANT_VECTOR is defined, set the corresponding dequantization parameters.
      ```cpp
      #if defined(CUSTOM_QUANT_VECTOR)
          matmulObj.SetQuantVector(quantGlobal);
      #else
          float quantFloat = 0.1f;
          uint64_t quantValue = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&quantFloat));
          matmulObj.SetQuantScalar(quantValue);
      #endif
      ```

  - Key Tiling Steps
    - Set the Matmul dequantization mode.
      ``` cpp
      #if defined(CUSTOM_QUANT_VECTOR)
          tilingApi.SetDequantType(matmul_tiling::DequantType::TENSOR); // set TENSOR quant mode
      #else
          tilingApi.SetDequantType(matmul_tiling::DequantType::SCALAR); // set SCALAR quant mode
      #endif
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
  # -DQUANT_MODE=0: Enable uniform coefficient dequantization mode;
  # -DQUANT_MODE=1: Enable vector dequantization mode;
  # -m=0: Enable uniform coefficient dequantization mode;
  # -m=1: Enable vector dequantization mode;
  mkdir -p build && cd build;    # Create and enter the build directory
  cmake -DQUANT_MODE=0 -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project, using uniform coefficient dequantization mode as an example, npu mode by default
  python3 ../scripts/gen_data.py -m=0   # Generate test input data, using uniform coefficient dequantization mode as an example
  ./demo                        # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin    # Verify whether the output is correct and confirm the algorithm logic
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DQUANT_MODE=0 -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DQUANT_MODE=0 -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
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
