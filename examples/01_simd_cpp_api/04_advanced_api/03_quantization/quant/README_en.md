# Quant Example

## Overview

This example uses the [AscendQuant](../../../../../docs/zh/api/SIMD-API/adv_api/quantization/AscendQuant.md) high-level API to implement quantization computation, which converts high-precision data to low-precision data to reduce storage and computation overhead. The example demonstrates the process of quantizing float type input data to int8_t type output through scale and offset. On the 950 series, while the AscendQuant API is compatible, it is recommended to prefer the [Quantize](../../../../../docs/zh/api/SIMD-API/adv_api/quantization/Quantize.md) API, which adapts to various quantization scenarios through a unified struct configuration.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── quant
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── quant.asc               // Ascend C operator implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  QuantCustom single example, performing element-wise quantization on the input tensor, converting half/float data types to int8_t data type.

- Example specifications:
  <table border="2" align="left">
  <caption>Table 1: Example Input/Output Specifications</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> quant </td></tr>

  <tr><td rowspan="3" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">inputGm</td><td align="center">[1, 1024]</td><td align="center">float</td><td align="center">ND</td></tr>


  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">outputGm</td><td align="center">[1, 1024]</td><td align="center">int8_t</td><td align="center">ND</td></tr>


  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">quant_custom</td></tr>
  </table>
  <br clear="left" />
<br />

- Example implementation:  
  This example implements a fixed shape input inputGm[1, 1024] with quantization parameters scale=2.0 and offset=0.9. This example is for the PER_TENSOR scenario (per-tensor quantization), converting float data type to int8_t data type.

  - Kernel implementation  
    The computation logic is: The vector computation APIs provided by Ascend C operate on LocalTensor elements. Input data must first be transferred to on-chip storage, then the AscendQuant (A2/A3) or Quantize (950 series) high-level API is used to complete the quantization computation, obtain the final result, and transfer it to external storage.

  - Tiling implementation  
    The tiling implementation process for the QuantCustom example is as follows: First obtain the maximum/minimum temporary space size required by the AscendQuant or Quantize API, use the minimum temporary space, and then determine the required tiling parameters based on the input length dataLength.

  - Invocation implementation  
    Use the kernel invocation operator <<<>>> to call the kernel function.

## Build and Run  

Run the following steps in the root directory of this example to build and run the operator.
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
