# asc_mmad

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

本接口是Ascend C面向昇腾AI芯片的矩阵乘加（Mmad）核心计算接口，专为高性能算子开发设计，封装了昇腾NPU硬件的矩阵乘加计算能力，广泛用于神经网络层（如全连接层、卷积层）、数值计算类算子的开发，其计算公式如下：

$$
C_{M \times N} = A_{M \times K} \times B_{K \times N} + C_{M \times N}
$$

其中，A、B、C分别为左、右、结果矩阵，C矩阵可以通过配置本接口的参数，初始化为全0矩阵、L0C Buffer中的矩阵或Bias矩阵，各矩阵的信息说明见下表：

<!-- npu="950" id8 -->
**表** 矩阵信息说明（[NPU架构版本3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）

| 矩阵 | 存储位置 | 形状（行数×列数） | 数据格式 | 分形大小（行数×列数） |
| --- | --- | --- | --- | --- |
| A | L0A Buffer | M×K | Nz | 16×K0 |
| B | L0B Buffer | K×N | Zn | K0×16 |
| C | L0C Buffer | M×N | Nz | 16×16 |
| Bias（用于C矩阵初始化） | BiasTable Buffer | 1×N，使用时通过广播复制M行来初始化C矩阵 | ND | - |

表格中K0的取值为`32B / sizeof(dtype)`，`dtype`为矩阵的数据类型。
<!-- end id8 -->

<!-- npu="A3,910b" id9 -->
**表** 矩阵信息说明（[NPU架构版本2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）

| 矩阵 | 存储位置 | 形状 | 数据格式 | 分形大小 |
| --- | --- | --- | --- | --- |
| A | L0A Buffer | M×K | Zz | 16×K0 |
| B | L0B Buffer | K×N | Zn | K0×16 |
| C | L0C Buffer | M×N | Nz | 16×16 |
| Bias（用于C矩阵初始化） | BiasTable Buffer | 1×N，使用时通过广播复制M行来初始化C矩阵 | ND | - |

表格中K0的取值为`32B / sizeof(dtype)`，`dtype`为矩阵的数据类型。特别地，当数据类型为`int4b_t`时，`K0 = 64`。
<!-- end id9 -->

本接口为矩阵计算接口，仅在AIC上生效。

## 函数原型

<!-- npu="950" id12 -->
### 函数原型（[NPU架构版本3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）

**占位符形式：**

```c
// 不显式传入Bias起始地址，通过参数控制C矩阵初始化方式
__aicore__ inline void asc_mmad(__cc__ <c_dtype>* c_matrix,
                                __ca__ <a_dtype>* a_matrix,
                                __cb__ <b_dtype>* b_matrix,
                                uint16_t m,
                                uint16_t k,
                                uint16_t n,
                                asc_unit_flag_mode unit_flag_mode,
                                bool disable_gemv,
                                bool c_matrix_source,
                                bool c_matrix_init_val)

// 显式传入Bias起始地址，C矩阵使用Bias矩阵初始化
__aicore__ inline void asc_mmad(__cc__ <c_dtype>* c_matrix,
                                __ca__ <a_dtype>* a_matrix,
                                __cb__ <b_dtype>* b_matrix,
                                uint64_t bias,
                                uint16_t m,
                                uint16_t k,
                                uint16_t n,
                                asc_unit_flag_mode unit_flag_mode,
                                bool disable_gemv)
```

**dtype支持的数据类型：**

`c_dtype`、`a_dtype`、`b_dtype`的取值组合见下表：

**表** 支持的数据类型组合（[NPU架构版本3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）<a id="asc_mmad_data_type"></a>

| a_dtype | b_dtype | c_dtype | Bias数据类型 |
| --- | --- | --- | --- |
| `int8_t` | `int8_t` | `int32_t` | `int32_t` |
| `hifloat8_t` | `hifloat8_t` | `float` | `float` |
| `fp8_e5m2_t` | `fp8_e5m2_t` | `float` | `float` |
| `fp8_e5m2_t` | `fp8_e4m3fn_t` | `float` | `float` |
| `fp8_e4m3fn_t` | `fp8_e5m2_t` | `float` | `float` |
| `fp8_e4m3fn_t` | `fp8_e4m3fn_t` | `float` | `float` |
| `half` | `half` | `float` | `float` |
| `bfloat16_t` | `bfloat16_t` | `float` | `float` |
| `float` | `float` | `float` | `float` |

**典型示例：**

```c
// 不显式传入Bias起始地址
__aicore__ inline void asc_mmad(__cc__ float* c_matrix,
                                __ca__ bfloat16_t* a_matrix,
                                __cb__ bfloat16_t* b_matrix,
                                uint16_t m,
                                uint16_t k,
                                uint16_t n,
                                asc_unit_flag_mode unit_flag_mode,
                                bool disable_gemv,
                                bool c_matrix_source,
                                bool c_matrix_init_val)

// 显式传入Bias起始地址
__aicore__ inline void asc_mmad(__cc__ float* c_matrix,
                                __ca__ bfloat16_t* a_matrix,
                                __cb__ bfloat16_t* b_matrix,
                                uint64_t bias,
                                uint16_t m,
                                uint16_t k,
                                uint16_t n,
                                asc_unit_flag_mode unit_flag_mode,
                                bool disable_gemv)
```
<!-- end id12 -->

<!-- npu="A3,910b" id13 -->
### 函数原型（[NPU架构版本2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）

**占位符形式：**

- asc_mmad接口：

  ```c
  __aicore__ inline void asc_mmad(__cc__ <c_dtype>* c_matrix, __ca__ <a_dtype>* a_matrix, __cb__ <b_dtype>* b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val)
  ```

  `c_dtype`、`a_dtype`、`b_dtype`的取值组合见下表：

  **表** 支持的数据类型组合（[NPU架构版本2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）

  | a_dtype | b_dtype | c_dtype | Bias数据类型 |
  | --- | --- | --- | --- |
  | `int8_t` | `int8_t` | `int32_t` | `int32_t` |
  | `half` | `half` | `float` | `float` |
  | `bfloat16_t` | `bfloat16_t` | `float` | `float` |
  | `float` | `float` | `float` | `float` |

- asc_mmad_s4接口，当输入矩阵数据类型为`int4b_t`时调用此类接口：

  ```c
  __aicore__ inline void asc_mmad_s4(__cc__ int32_t* c_matrix, __ca__ int4b_t* a_matrix, __cb__ int4b_t* b_matrix, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val)
  ```
<!-- end id13 -->

## 参数说明

<!-- npu="950" id14 -->
**表** 参数说明（[NPU架构版本3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）<a id="asc_mmad_param_table"></a>

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| c_matrix | 输出 | 目的操作数，结果矩阵C在L0C Buffer中的起始地址，需按照1024字节对齐。数据类型由接口重载决定，具体请参见[支持的数据类型组合](#asc_mmad_data_type)。 |
| a_matrix | 输入 | 源操作数，左矩阵A在L0A Buffer中的起始地址，需按照512字节对齐。数据类型由接口重载决定，具体请参见[支持的数据类型组合](#asc_mmad_data_type)。 |
| b_matrix | 输入 | 源操作数，右矩阵B在L0B Buffer中的起始地址，需按照512字节对齐。数据类型由接口重载决定，具体请参见[支持的数据类型组合](#asc_mmad_data_type)。 |
| bias | 输入 | Bias矩阵在BiasTable Buffer中的起始地址，需按照64字节对齐，可以为非零地址，仅显式传入Bias起始地址的接口包含该参数。<br>Bias的数据类型需与C矩阵的数据类型保持一致，调用本接口前，需将Bias数据通过[asc_copy_l12bt](../cube_datamove/asc_copy_l12bt/asc_copy_l12bt_arch_3510.md)接口搬运到该地址，Bias占用长度为`n * sizeof(c_dtype)`向上补齐到64字节。 |
| m | 输入 | 左矩阵A和结果矩阵C的M维大小，单位为元素，取值范围为[0, 4095]。 |
| k | 输入 | 左矩阵A和右矩阵B的K维大小，单位为元素，取值范围为[0, 4095]。 |
| n | 输入 | 右矩阵B和结果矩阵C的N维大小，单位为元素，取值范围为[0, 4095]。 |
| unit_flag_mode | 输入 | 用于控制矩阵乘加指令与矩阵搬出指令的细粒度并行，开启UnitFlag后，硬件每计算完一个分形，计算结果就会被搬出。取值说明如下：<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::DISABLE`：不开启UnitFlag。<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::ENABLE_KEEP`：开启UnitFlag，硬件执行完指令后不改变单元标志位。<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::ENABLE_UPDATE`：开启UnitFlag，硬件执行完指令后改变单元标志位。<br>矩阵乘加指令与对应的矩阵搬出指令必须都开启或都不开启UnitFlag，开启后指令之间无需再插入同步指令。 |
| disable_gemv | 输入 | M为1时，配置是否关闭GEMV模式。<br>&nbsp;&nbsp;&bull; false：开启GEMV模式。<br>&nbsp;&nbsp;&bull; true：关闭GEMV模式。<br>M不为1时，该参数不生效。 |
| c_matrix_source | 输入 | 仅不显式传入Bias起始地址的重载包含该参数。当参数`c_matrix_init_val`为false时，配置矩阵C的初始值来源。<br>&nbsp;&nbsp;&bull; false：矩阵C的初始值来源于L0C Buffer。<br>&nbsp;&nbsp;&bull; true：矩阵C的初始值来源于BiasTable Buffer，并固定从0地址开始读取。 |
| c_matrix_init_val | 输入 | 仅不显式传入Bias起始地址的重载包含该参数。配置是否将矩阵C的初始值设置为0。<br>&nbsp;&nbsp;&bull; true：将矩阵C的初始值设置为0，参数`c_matrix_source`不生效。<br>&nbsp;&nbsp;&bull; false：不执行清零操作，矩阵C的初始值由参数`c_matrix_source`配置。 |
<!-- end id14 -->

<!-- npu="A3,910b" id15 -->
**表** 参数说明（[NPU架构版本2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| c_matrix | 输出 | 目的操作数，结果矩阵C在L0C Buffer中的起始地址，需按照1024字节对齐。 |
| a_matrix | 输入 | 源操作数，左矩阵A在L0A Buffer中的起始地址，需按照512字节对齐。 |
| b_matrix | 输入 | 源操作数，右矩阵B在L0B Buffer中的起始地址，需按照512字节对齐。 |
| m | 输入 | 左矩阵A和结果矩阵C的M维大小，单位为元素，取值范围为[0, 4095]。 |
| k | 输入 | 左矩阵A和右矩阵B的K维大小，单位为元素，取值范围为[0, 4095]。 |
| n | 输入 | 右矩阵B和结果矩阵C的N维大小，单位为元素，取值范围为[0, 4095]。 |
| unit_flag_mode | 输入 | 用于控制矩阵乘加指令与矩阵搬出指令的细粒度并行，开启UnitFlag后，硬件每计算完一个分形，计算结果就会被搬出。取值说明如下：<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::DISABLE`：不开启UnitFlag。<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::ENABLE_KEEP`：开启UnitFlag，硬件执行完指令后不改变单元标志位。<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::ENABLE_UPDATE`：开启UnitFlag，硬件执行完指令后改变单元标志位。<br>矩阵乘加指令与对应的矩阵搬出指令必须都开启或都不开启UnitFlag，开启后指令之间无需再插入同步指令。 |
| k_direction_align | 输入 | K方向对齐的核心功能是通过`k_direction_align`参数控制在使用float数据类型时，L0A Buffer和L0B Buffer矩阵在K方向上的对齐方式。<br>取值说明如下：<br>&nbsp;&nbsp;&bull; false：K方向对齐到`ceil(k / 8) * 8`。<br>&nbsp;&nbsp;&bull; true：K方向对齐到`ceil(k / 16) * 16`。 |
| c_matrix_source | 输入 | 当参数`c_matrix_init_val`为false时，配置矩阵C的初始值来源。<br>&nbsp;&nbsp;&bull; false：矩阵C的初始值来源于L0C Buffer。<br>&nbsp;&nbsp;&bull; true：矩阵C的初始值来源于BiasTable Buffer。 |
| c_matrix_init_val | 输入 | 配置是否将矩阵C的初始值设置为0。<br>&nbsp;&nbsp;&bull; true：将矩阵C的初始值设置为0，参数`c_matrix_source`不生效。<br>&nbsp;&nbsp;&bull; false：不执行清零操作，矩阵C的初始值由参数`c_matrix_source`配置。 |
<!-- end id15 -->

## 返回值说明

无

## 流水类型

PIPE_M

## 约束说明

- 本接口仅在AIC上生效，在AIV上调用将直接返回。
- `m`、`k`、`n`中的任意一个值为0时，接口将被视为NOP（空操作）。

- 内存使用约束说明：
  <!-- npu="950" id10 -->
  - 针对Ascend 950PR/Ascend 950DT:

      - L0C Buffer大小为256KB，L0A Buffer和L0B Buffer大小均为64KB。BiasTable Buffer大小为4KB。矩阵的起始地址和占用空间不能超出对应Buffer的范围。
      - 各矩阵的起始地址需满足[参数说明](#asc_mmad_param_table)中的对齐要求。操作数的其他地址约束请参考[存储单元说明](../general_description_and_constraints.md#存储单元说明)。
      - 申请矩阵存储空间时，需使用按照分形大小补齐后的数值进行申请：M、N分别向上补齐到16的倍数，K向上补齐到K0的倍数，K0的取值为`32B / sizeof(dtype)`，`dtype`为矩阵的数据类型。`m`、`k`和`n`仍传入矩阵的有效M、K、N值，补齐部分为无效数据，不参与结果矩阵有效区域的计算。
      - 当M为1且`disable_gemv`为false时，将开启GEMV模式。此时从L0A Buffer读取矩阵A时按照ND格式读取，矩阵A需按照ND格式排布，起始地址仍需按照512字节对齐。
  <!-- end id10 -->

  <!-- npu="A3,910b" id11 -->
  - 针对如下产品型号：

 	  <!-- npu="A3" id16 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品
 	  <!-- end id16 -->
 	  <!-- npu="910b" id17 -->
 	  Atlas A2 训练系列产品/Atlas A2 推理系列产品
 	  <!-- end id17 -->
 	  L0C Buffer大小为128KB，L0A Buffer和L0B Buffer大小均为64KB。BiasTable Buffer大小为1KB。矩阵的起始地址和占用空间不能超出对应Buffer的范围。
 <!-- end id11 -->

- 同步约束说明：

  针对输入矩阵沿K轴分块计算，并将结果累加到同一块L0C Buffer的场景，当`(m / 16) * (n / 16) < 10`时，需在相邻两次矩阵乘加指令之间调用[asc_sync_pipe](../sync/asc_sync_pipe.md)，并将入参`pipe`设置为`PIPE_M`。

- UnitFlag约束说明：

  - 开启UnitFlag时，矩阵乘加指令与对应矩阵搬出指令需同时开启UnitFlag。当希望同一块L0C Buffer内存空间能持续只被多条矩阵乘加指令或多条矩阵搬出指令操作时，除最后一条外的指令需将`unit_flag_mode`设置为`asc_unit_flag_mode::ENABLE_KEEP`，维持被操作内存空间的持续占用状态，最后一条指令设置为`asc_unit_flag_mode::ENABLE_UPDATE`，解除被占用状态。
  - 开启UnitFlag时，矩阵计算方向需与矩阵搬出读取顺序保持一致。矩阵搬出指令开启Nz2ND随路格式转换，或未进行随路格式转换但开启B8/B4量化并触发Channel Merge功能时，调用[asc_set_mmad_direction_n](asc_set_mmad_direction_n.md)；其他场景调用[asc_set_mmad_direction_m](asc_set_mmad_direction_m.md)。
  - 开启UnitFlag时，建议矩阵乘加的计算数据量与矩阵搬出的数据量保持一致。两者不一致可能导致执行异常。需要清除UnitFlag产生的残留状态时，可调用[asc_set_l0c2gm_config](../cube_datamove/asc_set_l0c2gm_config.md)，并将`enable_unit_flag`设置为true，将L0C Buffer中所有内存块的单元标志位设置为0并关闭UnitFlag。

- 特殊值/边界值约束说明：

  浮点类型的输入或输出包含inf/nan时，可通过[asc_set_ctrl](../spr/asc_set_ctrl.md)接口配置CTRL寄存器的CTRL\[48\]比特位控制计算时的模式：

  - 设置为0时使用饱和模式，inf输出饱和为±MAX、nan输出饱和为0；
  - 设置为1时使用非饱和模式，inf/nan保持原输出。

  注意，应避免nan输入，否则可能会产生执行报错；整数类型仅支持饱和模式。

<!-- npu="950" id18 -->
## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

以下调用示例代码仅Ascend 950PR/Ascend 950DT产品支持。

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t M = 48;
constexpr uint32_t K = 96;
constexpr uint32_t N = 80;
constexpr uint32_t CUBE_M = 16;
constexpr uint32_t CUBE_K = 32;

__global__ __cube__ void asc_mmad_kernel(__gm__ int8_t* a, __gm__ int8_t* b, __gm__ int32_t* output)
{
    asc_init();
    __cbuf__ int8_t a_l1[M * K];
    __cbuf__ int8_t b_l1[N * K];
    __ca__ int8_t a_l0[M * K];
    __cb__ int8_t b_l0[N * K];
    __cc__ int32_t c_l0[M * N];

    asc_set_gm2l1_nz_para(1, 1, M, 0);
    asc_copy_gm2l1_nd2nz(
        a_l1, a, K, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, N, 0);
    asc_copy_gm2l1_nd2nz(
        b_l1, b, K, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, N, K, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE_M, K / CUBE_K, M / CUBE_M, M / CUBE_M);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE_M, K / CUBE_K, N / CUBE_M, N / CUBE_M);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_mmad(c_l0, a_l0, b_l0, M, K, N, asc_unit_flag_mode::DISABLE, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, N, M, N, M, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM,
        asc_unit_flag_mode::DISABLE, QuantMode_t::NoQuant, asc_relu_pre_mode::NONE, false, true, false, false);
    asc_sync_pipe(PIPE_ALL);
}

template <typename T>
void print_row(const char* label, const std::vector<T>& data)
{
    std::cout << label << ':';
    for (uint32_t i = 0; i < 8; ++i) std::cout << ' ' << +data[i];
    std::cout << " ..." << std::endl;
}
} // namespace

int main()
{
    // B在GM中按N x K存放，搬入L0B后形成MMAD所需的K x N矩阵。
    std::vector<int8_t> a(M * K), b(N * K);
    std::vector<int32_t> output(M * N), golden(M * N);
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t k = 0; k < K; ++k) {
            a[row * K + k] = static_cast<int8_t>(static_cast<int32_t>((row + 2 * k) % 5) - 2);
        }
    }
    for (uint32_t col = 0; col < N; ++col) {
        for (uint32_t k = 0; k < K; ++k) {
            b[col * K + k] = static_cast<int8_t>(static_cast<int32_t>((3 * col + k) % 7) - 3);
        }
    }
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t col = 0; col < N; ++col) {
            for (uint32_t k = 0; k < K; ++k) {
                golden[row * N + col] += a[row * K + k] * b[col * K + k];
            }
        }
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    int8_t *a_device = nullptr, *b_device = nullptr;
    int32_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(int32_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size(), a.data(), a.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size(), b.data(), b.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    asc_mmad_kernel<<<1, 0>>>(a_device, b_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int32_t), output_device, output.size() * sizeof(int32_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_row("Input A row 0", a);
    print_row("Input B row 0", b);
    print_row("Output row 0", output);
    print_row("Golden row 0", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_mmad passed." : "[Failed] asc_mmad failed.") << std::endl;
    aclrtFree(a_device);
    aclrtFree(b_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```

更多场景使用样例请参考[asc_mmad样例](../../../../../../examples/02_simd_c_api/03_c_api/03_matrix_compute/mmad)。
<!-- end id18 -->
