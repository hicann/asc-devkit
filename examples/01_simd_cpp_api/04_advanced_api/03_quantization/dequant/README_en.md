# Dequant Example

## Overview

This example uses the [AscendDequant](../../../../../docs/zh/api/SIMD-API/adv_api/quantization/AscendDequant.md) high-level API to implement dequantization computation, which restores quantized low-precision data to high-precision data. The example demonstrates the PER_CHANNEL scenario (per-channel quantization), where int32_t type input data is multiplied by a scale factor and converted to float type output. On the 950 series, while the AscendDequant API is compatible, it is recommended to prefer the [Dequantize](../../../../../docs/zh/api/SIMD-API/adv_api/quantization/Dequantize.md) API, which adapts to various quantization scenarios through a unified struct configuration.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── dequant
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── dequant.asc             // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  This example performs element-wise dequantization on the input tensor, converting int32_t data type to float and other data types.

- Example specifications:

  **Table 1: Example Input/Output Specifications**

  | Category | name | shape | data type | format |
  | --- | --- | --- | --- | --- |
  | Example Type (OpType) | dequant | - | - | - |
  | Example Input | inputGm | [128, 32] | int32_t | ND |
  | Example Input | deqScaleGm | [1, 32] | float | ND |
  | Example Output | outputGm | [128, 32] | float | ND |
  | Kernel Function Name | dequant_custom | - | - | - |

- Example implementation:  
   This example implements a fixed shape with input inputGm[128, 32], scaleGm[1, 32], and output outputGm[128, 32]. It performs element-wise dequantization, converting int32_t data type to float and other data types.

  - Kernel implementation  
    The computation logic is: This example transfers the input data to on-chip storage, then uses the AscendDequant (A2/A3) or Dequantize (950 series) high-level API to complete the dequantization computation, obtain the final result, and transfer it to external storage.

  - Tiling implementation  
    The tiling implementation process for the DequantCustom example is as follows: First obtain the maximum/minimum temporary space size required by the AscendDequant or Dequantize API, use the minimum temporary space, and then determine the required tiling parameters based on the input length dataLength.

  - Invocation implementation  
    Use the kernel invocation operator <<<>>> to call the kernel function.

## Build and Run  

Run the following steps in the root directory of this example to build and run the example.
- Configure environment variables  
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.
    
- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU mode by default
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo
  ```

  To use CPU debugging or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  For example:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debugging mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Before switching the build mode, clear the cmake cache by running `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU run, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  
- Execution result  
  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
