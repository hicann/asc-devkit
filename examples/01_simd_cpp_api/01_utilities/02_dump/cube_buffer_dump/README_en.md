# Cube Buffer DumpTensor API Description

## Overview

This sample uses the Cube-programmed `QuantMatmulHifp8` computation as context. It uses `DumpTensor` to print
the HiFloat8 matrix A, bias, and scale from L1 Buffer, Bias Table Buffer, and Fixpipe Buffer, demonstrating
Cube Buffer data printing on dav-3510.

## Supported Products and CANN Software Versions

| Product | CANN Software Version | HDK Version |
|---------|-----------------------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 | >= 25.7.0 |

## Directory Structure

```text
├── cube_buffer_dump
│   ├── scripts
│   │   ├── gen_data.py          // Generates input and golden data
│   │   └── verify_result.py     // Verifies computation output and dump records
│   ├── CMakeLists.txt           // Build project file
│   ├── data_utils.h             // Binary file read and write helpers
│   ├── cube_buffer_dump.asc     // Quantized MatMul, DumpTensor calls, and host launch
│   ├── README.md                // Chinese sample document
│   └── README_en.md             // English sample document
```

## Sample Description

- Function:

  The sample uses HiFloat8 quantized matrix multiplication with a bias and a per-channel scale as context.
  Each output element `C[m, n]` is calculated as follows:

  ```text
  C[m, n] = (sum(A[m, k] * B[k, n]) + bias[n]) * scale[n]
  ```

  MMAD uses the float bias in Bias Table Buffer for HiFloat8 matrix multiplication and accumulation. Fixpipe
  uses the per-channel scale in Fixpipe Buffer to convert the float accumulation result to bfloat16.

- Specifications:

  The sample uses fixed dimensions `M=16, K=32, N=16`. The tensor specifications are listed below:

  <table>
  <tr><td rowspan="1" align="center">OpType</td><td colspan="4" align="center">QuantMatmulHifp8</td></tr>
  <tr><td rowspan="5" align="center">Inputs</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">hifloat8</td><td align="center">ND</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">hifloat8</td><td align="center">ND</td></tr>
  <tr><td align="center">bias</td><td align="center">[N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">scale</td><td align="center">[N]</td><td align="center">uint64_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Output</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">bfloat16</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel function</td><td colspan="4" align="center">QuantMatmulHifp8</td></tr>
  </table>

- Implementation:

  One AI Core executes the computation as follows:

  1. Move HiFloat8 matrices A and B from Global Memory to L1, then to L0A and L0B.
  2. Move the float bias from Global Memory to L1, then to Bias Table Buffer. MMAD uses Bias Table Buffer as
     the initial C matrix and computes `A * B + bias`.
  3. Move the per-channel `uint64_t` scale from Global Memory to L1, then to Fixpipe Buffer.
  4. Fixpipe reads the scale, converts the float L0C result to bfloat16, and writes it to Global Memory.

  A and B contain the HiFloat8 value `1.0`, the bias contains `0` through `15`, even columns use scale `1.0`,
  and odd columns use scale `0.5`. The expected value in column `n` of every output row is therefore
  `(32 + bias[n]) * scale[n]`.

- DumpTensor positions:

  The sample prints the following data:

  | desc | Position | Purpose |
  |------|----------|----------------|
  | 100 | L1 Buffer | First 32 elements of HiFloat8 matrix A |
  | 101 | L1 Buffer | Float bias before it is moved to Bias Table Buffer |
  | 102 | L1 Buffer | Per-channel `uint64_t` scale before it is moved to Fixpipe Buffer |
  | 201 | Bias Table Buffer | Float bias used by MMAD to initialize the C matrix |
  | 301 | Fixpipe Buffer | `uint64_t` scale read by Fixpipe during output quantization |

  `scripts/verify_result.py` verifies the computation output and all five `DumpTensor` records.

## Fixpipe Buffer Dump Characteristics

This sample reads scale from address `0` in the Fixpipe Buffer quantization parameter area. On dav-3510, data before and after the L1-to-Fixpipe Buffer transfer has the following relationship:

```text
fixpipe_value = l1_value & 0x00007fe0ffffe0ff
```

The mask retains bits `[7:0]`, `[31:13]`, and `[46:37]`, and clears bits `[12:8]`, `[36:32]`, and `[63:47]`.
The `desc=301` result must equal the `desc=102` value after applying this mask.

For the relevant rules, see
[L1 and Fixpipe Buffer Data Transfer](../../../../../docs/zh/api/SIMD-API/基础API/数据搬运导览/L1与Fixpipe-Buffer数据搬运.md) and
[On-the-fly Quantization](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵搬出关键特性说明/随路量化.md).
The 3510 quantization parameter table in the latter marks bits `[12:0]` as invalid. In this sample's dav-3510
dump output, bits `[7:0]` retain their input values and only bits `[12:8]` are cleared. This difference only
affects the visible values of invalid bits and does not change their quantization semantics.

## Build and Run

Run the following commands in the sample root directory:

```bash
source ${install_path}/cann/set_env.sh
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 .. && make -j
python3 ../scripts/gen_data.py
./demo 2>&1 | tee run.log
python3 ../scripts/verify_result.py output/output.bin output/golden.bin run.log \
    input/a_gm.bin input/bias_gm.bin input/scale_gm.bin
```

`${install_path}` is the CANN installation directory. The default installation root is `/usr/local/Ascend`.

A successful verification prints:

```text
Output result check: PASS
HiFloat8 A L1 data check: PASS
Bias L1 raw input check: PASS
Bias Table raw data check: PASS
Fixpipe L1 raw input check: PASS
Fixpipe quant visible-mask check: PASS
test pass!
```
