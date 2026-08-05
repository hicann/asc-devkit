# AntiQuant Example

## Overview

This example uses the [AscendAntiQuant](../../../../../docs/zh/api/SIMD-API/adv_api/quantization/AscendAntiQuant.md) high-level API in a model quantization inference scenario to implement dequantization computation, which restores quantized low-precision data to high-precision data. The example demonstrates the PER_CHANNEL scenario (per-channel quantization), where int8_t type input data is added with an offset, multiplied by a scale factor, and converted to half type output. On the 950 series, while the AscendAntiQuant API is compatible, it is recommended to prefer the [AntiQuantize](../../../../../docs/zh/api/SIMD-API/adv_api/quantization/AntiQuantize.md) API, which adapts to various quantization scenarios through a unified struct configuration.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── antiquant
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── antiquant.asc           // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  
  Perform element-wise dequantization, for example converting int8_t data type to half data type.

- Example specifications:
  <table border="2" align="left">
  <caption>Table 1: Example Input/Output Specifications</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> antiquant </td></tr>

  <tr><td rowspan="5" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src</td><td align="center">[8, 128]</td><td align="center">int8_t</td><td align="center">ND</td></tr>
  <tr><td align="center">offset</td><td align="center">[1, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">scale</td><td align="center">[1, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">dst</td><td align="center">[8, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">antiquant_custom</td></tr>
  </table>
  <br clear="left" />
<br />

- Example implementation:  
  This example implements an antiquant_custom example with fixed shapes of input src[8, 128], offset[1, 128], scale[1, 128], and output dst[8, 128]. It performs element-wise dequantization, converting int8_t type data to half type data.
  - Kernel implementation  
    The computation logic is: This example transfers the input data to on-chip storage, then uses the AscendAntiQuant (A2/A3) or AntiQuantize (950 series) high-level API to complete the dequantization computation, obtain the final result, and transfer it to external storage.

  - Invocation implementation  
    Use the kernel invocation operator <<<>>> to call the kernel function.

## Build and Run  

Run the following steps in the root directory of this example to build and run the example.
- Configure environment variables  
  Configure environment variables based on the [installation method](../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU mode by default
  python3 ../scripts/gen_data.py -DCMAKE_ASC_ARCHITECTURES=dav-2201   # Generate test input data
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
