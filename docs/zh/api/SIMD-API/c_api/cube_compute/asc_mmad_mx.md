# asc_mmad_mx

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
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

头文件路径为：`"c_api/cube_compute/cube_compute.h"`。

本接口用于完成带有量化系数的矩阵乘加计算（MmadMx），即左矩阵A和右矩阵B均有对应的量化系数矩阵，左量化系数矩阵ScaleA和右量化系数矩阵ScaleB。在MmadMx场景中，左量化系数矩阵与左矩阵相乘，右量化系数矩阵与右矩阵相乘，然后对两个乘积的结果做矩阵乘法，其计算公式如下：

$$
C_{M \times N} = \left(ScaleA_{M \times (K/32)} \otimes A_{M \times K}\right) \times \left(ScaleB_{(K/32) \times N} \otimes B_{K \times N}\right) + C_{M \times N}
$$

其中，$\otimes$表示广播乘法，矩阵A、B沿K维度每连续32个元素共享对应量化系数矩阵ScaleA、ScaleB中的一个量化系数。C矩阵可以通过配置本接口的参数，初始化为全0矩阵、L0C Buffer中的矩阵或Bias矩阵。

本接口不单独传入Scale矩阵地址，而是根据A、B矩阵的起始地址，按照固定的地址映射关系从L0A_MX Buffer和L0B_MX Buffer读取对应的Scale矩阵。各矩阵的信息说明见下表：

**表1** 矩阵信息说明

| 矩阵 | 存储位置 | 形状（行数×列数） | 数据格式 | 分形大小（行数×列数） |
| --- | --- | --- | --- | --- |
| A | L0A Buffer | M×K | Nz | 16×K0 |
| ScaleA | L0A_MX Buffer | M×(K / 32) | Zz | 16×2 |
| B | L0B Buffer | K×N | Zn | K0×16 |
| ScaleB | L0B_MX Buffer | (K / 32)×N | Nn | 2×16 |
| C | L0C Buffer | M×N | Nz | 16×16 |
| Bias（用于C矩阵初始化） | BiasTable Buffer | 1×N，使用时通过广播复制M行来初始化C矩阵 | ND | - |

当矩阵数据类型为`fp8_e5m2_t`、`fp8_e4m3fn_t`时，`K0 = 32`；当矩阵数据类型为`fp4x2_e2m1_t`、`fp4x2_e1m2_t`时，`K0 = 64`。

本接口为矩阵计算接口，仅在AIC上生效。

## 函数原型

**占位符形式：**

```c
// 不显式传入Bias起始地址，通过参数控制C矩阵初始化方式
__aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix,
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
__aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix,
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

`a_dtype`、`b_dtype`的取值组合见下表：

**表2** 支持的数据类型组合<a id="asc_mmad_mx_data_type"></a>

| a_dtype | b_dtype | ScaleA/ScaleB数据类型 | 结果矩阵C数据类型 | Bias数据类型 |
| --- | --- | --- | --- | --- |
| `fp4x2_e1m2_t` | `fp4x2_e1m2_t` | `fp8_e8m0_t` | `float` | `float` |
| `fp4x2_e1m2_t` | `fp4x2_e2m1_t` | `fp8_e8m0_t` | `float` | `float` |
| `fp4x2_e2m1_t` | `fp4x2_e1m2_t` | `fp8_e8m0_t` | `float` | `float` |
| `fp4x2_e2m1_t` | `fp4x2_e2m1_t` | `fp8_e8m0_t` | `float` | `float` |
| `fp8_e4m3fn_t` | `fp8_e4m3fn_t` | `fp8_e8m0_t` | `float` | `float` |
| `fp8_e4m3fn_t` | `fp8_e5m2_t` | `fp8_e8m0_t` | `float` | `float` |
| `fp8_e5m2_t` | `fp8_e4m3fn_t` | `fp8_e8m0_t` | `float` | `float` |
| `fp8_e5m2_t` | `fp8_e5m2_t` | `fp8_e8m0_t` | `float` | `float` |

**典型示例：**

```c
// 不显式传入Bias起始地址
__aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix,
                                    __ca__ fp4x2_e1m2_t* a_matrix,
                                    __cb__ fp4x2_e2m1_t* b_matrix,
                                    uint16_t m,
                                    uint16_t k,
                                    uint16_t n,
                                    asc_unit_flag_mode unit_flag_mode,
                                    bool disable_gemv,
                                    bool c_matrix_source,
                                    bool c_matrix_init_val)

// 显式传入Bias起始地址
__aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix,
                                    __ca__ fp4x2_e1m2_t* a_matrix,
                                    __cb__ fp4x2_e2m1_t* b_matrix,
                                    uint64_t bias,
                                    uint16_t m,
                                    uint16_t k,
                                    uint16_t n,
                                    asc_unit_flag_mode unit_flag_mode,
                                    bool disable_gemv)
```

## 参数说明

**表3** 参数说明<a id="asc_mmad_mx_param_table"></a>

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| c_matrix | 输出 | 目的操作数，结果矩阵C在L0C Buffer中的起始地址，数据类型为`float`，需按照1024字节对齐。 |
| a_matrix | 输入 | 源操作数，左矩阵A在L0A Buffer中的起始地址。数据类型由接口重载决定，具体请参见[表2](#asc_mmad_mx_data_type)。<br>矩阵数据类型为`fp4x2_e2m1_t`、`fp4x2_e1m2_t`时需按照512字节对齐，矩阵数据类型为`fp8_e5m2_t`、`fp8_e4m3fn_t`时需按照1024字节对齐。<br>ScaleA矩阵在L0A_MX Buffer中的起始地址为`a_matrix`起始地址除以16。 |
| b_matrix | 输入 | 源操作数，右矩阵B在L0B Buffer中的起始地址。数据类型由接口重载决定，具体请参见[表2](#asc_mmad_mx_data_type)。<br>矩阵数据类型为`fp4x2_e2m1_t`、`fp4x2_e1m2_t`时需按照512字节对齐，矩阵数据类型为`fp8_e5m2_t`、`fp8_e4m3fn_t`时需按照1024字节对齐。<br>ScaleB矩阵在L0B_MX Buffer中的起始地址为`b_matrix`起始地址除以16。 |
| bias | 输入 | Bias矩阵在BiasTable Buffer中的起始地址，需按照64字节对齐，可以为非零地址，仅显式传入Bias起始地址的接口包含该参数。<br>Bias支持的数据类型为`float`，调用本接口前，需将Bias数据通过[asc_copy_l12bt](../cube_datamove/asc_copy_l12bt/asc_copy_l12bt_arch_3510.md)接口搬运到该地址，Bias占用长度为`n * sizeof(float)`向上补齐到64字节。 |
| m | 输入 | 左矩阵A和结果矩阵C的M维大小，单位为元素，取值范围为[0, 4095]。 |
| k | 输入 | 左矩阵A和右矩阵B的K维大小，单位为元素，取值范围为[0, 4095]，并且需为64的倍数。 |
| n | 输入 | 右矩阵B和结果矩阵C的N维大小，单位为元素，取值范围为[0, 4095]。 |
| unit_flag_mode | 输入 | 用于控制矩阵乘加指令与矩阵搬出指令的细粒度并行，开启UnitFlag后，硬件每计算完一个分形，计算结果就会被搬出。取值说明如下：<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::DISABLE`：不开启UnitFlag。<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::ENABLE_KEEP`：开启UnitFlag，硬件执行完指令后不改变单元标志位。<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::ENABLE_UPDATE`：开启UnitFlag，硬件执行完指令后改变单元标志位。<br>矩阵乘加指令与对应的矩阵搬出指令必须都开启或都不开启UnitFlag，开启后指令之间无需再插入同步指令。 |
| disable_gemv | 输入 | M为1时，配置是否关闭GEMV模式。<br>&nbsp;&nbsp;&bull; false：开启GEMV模式。<br>&nbsp;&nbsp;&bull; true：关闭GEMV模式。<br>M不为1时，该参数不生效。 |
| c_matrix_source | 输入 | 仅不显式传入Bias起始地址的重载包含该参数。当参数`c_matrix_init_val`为false时，配置矩阵C的初始值来源。<br>&nbsp;&nbsp;&bull; false：矩阵C的初始值来源于L0C Buffer。<br>&nbsp;&nbsp;&bull; true：矩阵C的初始值来源于BiasTable Buffer，并固定从0地址开始读取。 |
| c_matrix_init_val | 输入 | 仅不显式传入Bias起始地址的重载包含该参数。配置是否将矩阵C的初始值设置为0。<br>&nbsp;&nbsp;&bull; true：将矩阵C的初始值设置为0，参数`c_matrix_source`不生效。<br>&nbsp;&nbsp;&bull; false：不执行清零操作，矩阵C的初始值由参数`c_matrix_source`配置。 |

## 返回值说明

无

## 流水类型

PIPE_M

## 约束说明

- 本接口仅在AIC上生效，在AIV上调用将直接返回。
- `m`、`k`、`n`中的任意一个值为0时，接口将被视为NOP（空操作）。
- 调用本接口前，需通过[asc_copy_l12l0a_mx](../cube_datamove/asc_copy_l12l0a_mx.md)和[asc_copy_l12l0b_mx](../cube_datamove/asc_copy_l12l0b_mx.md)等接口，将ScaleA和ScaleB矩阵分别搬入L0A_MX Buffer和L0B_MX Buffer。ScaleA和ScaleB矩阵的数据类型必须为`fp8_e8m0_t`，K方向上每个Scale元素对应32个输入矩阵元素。

- 内存使用约束说明：

  - L0C Buffer大小为256KB，L0A Buffer和L0B Buffer大小均为64KB，L0A_MX Buffer和L0B_MX Buffer大小均为4KB，BiasTable Buffer大小为4KB。矩阵的起始地址和占用空间不能超出对应Buffer的范围。
  - 各矩阵的起始地址需满足[表3](#asc_mmad_mx_param_table)中的对齐要求。操作数的其他地址约束请参考[存储单元说明](../general_description_and_constraints.md#存储单元说明)。
  - ScaleA矩阵在L0A_MX Buffer中的起始地址为`a_matrix`起始地址除以16，ScaleB矩阵在L0B_MX Buffer中的起始地址为`b_matrix`起始地址除以16。搬入Scale矩阵时需使用与矩阵A和矩阵B匹配的目的地址。
  - `k`需为64的倍数。原始K不是64的倍数时，需将K向上补齐到64的倍数，并将补齐区域的矩阵A和矩阵B数据设置为0；传入接口的`k`、矩阵存储空间以及Scale矩阵的K维大小均需按照补齐后的K计算。
  - 矩阵存储空间的M、N维需分别向上补齐到16的倍数，`m`和`n`仍传入矩阵的有效M、N值。补齐部分为无效数据，不参与结果矩阵有效区域的计算。
  - Scale矩阵需覆盖本次矩阵乘加所需的全部数据。ScaleA的M维需向上补齐到16的倍数，K维大小为K / 32；ScaleB的K维大小为K / 32，N维需向上补齐到16的倍数。对应的起始地址与占用空间不能超出MX Buffer的范围。
  - 当M为1且`disable_gemv`为false时，默认开启GEMV模式。此时从L0A Buffer读取矩阵A时按照ND格式读取，矩阵A需按照ND格式排布，起始地址仍需满足[表3](#asc_mmad_mx_param_table)中的对齐要求。

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

  注意，应避免nan输入，否则可能会产生执行报错。

  边界值计算结果请见[MmadMx接口边界值汇总](../../../appendix/mmad_mx_interface_boundary_value_summary.md)。

<!-- npu="950" id8 -->
## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

```cpp
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t M = 40;
constexpr uint32_t K = 70;
constexpr uint32_t N = 50;
constexpr uint32_t SCALE_K = 4;
constexpr uint32_t M_ALIGN = 48;
constexpr uint32_t K_ALIGN = 128;
constexpr uint32_t N_ALIGN = 64;
constexpr uint32_t CUBE_M = 16;
constexpr uint32_t CUBE_K = 32;
constexpr uint32_t GM2L1_K_ALIGN = 96;
constexpr uint8_t FP8_E4M3_ONE = 0x38;
constexpr uint8_t FP8_E4M3_NEG_ONE = 0xb8;
constexpr uint8_t FP8_E5M2_ONE = 0x3c;
constexpr uint8_t FP8_E5M2_NEG_ONE = 0xbc;

__global__ __cube__ void asc_mmad_mx_kernel(__gm__ uint8_t* a, __gm__ uint8_t* scale_a,
    __gm__ uint8_t* b, __gm__ uint8_t* scale_b, __gm__ float* output)
{
    asc_init();
    __cbuf__ fp8_e4m3fn_t a_l1[M_ALIGN * K_ALIGN];
    __cbuf__ fp8_e5m2_t b_l1[N_ALIGN * K_ALIGN];
    __cbuf__ fp8_e8m0_t scale_a_l1[M_ALIGN * SCALE_K], scale_b_l1[N_ALIGN * SCALE_K];
    __ca__ fp8_e4m3fn_t a_l0[M_ALIGN * K_ALIGN];
    __cb__ fp8_e5m2_t b_l0[N_ALIGN * K_ALIGN];
    __cc__ float c_l0[M_ALIGN * N_ALIGN];

    asc_set_gm2l1_nz_para(1, 1, M_ALIGN, 0);
    asc_copy_gm2l1_nd2nz(a_l1, reinterpret_cast<__gm__ fp8_e4m3fn_t*>(a), K,
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, SCALE_K / 2, 0);
    asc_copy_gm2l1_dn2nz(reinterpret_cast<__cbuf__ half*>(scale_a_l1),
        reinterpret_cast<__gm__ half*>(scale_a), SCALE_K, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM,
        SCALE_K / 2, M, 0, false);
    asc_set_gm2l1_nz_para(1, 1, N_ALIGN, 0);
    asc_copy_gm2l1_nd2nz(b_l1, reinterpret_cast<__gm__ fp8_e5m2_t*>(b), K,
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, N, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, SCALE_K / 2, 0);
    asc_copy_gm2l1_dn2nz(reinterpret_cast<__cbuf__ half*>(scale_b_l1),
        reinterpret_cast<__gm__ half*>(scale_b), SCALE_K, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM,
        SCALE_K / 2, N, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // ND2NZ搬运已将K方向补齐到96，还需将[96, 128)区域清零。
    asc_fill_value_config fill_config;
    fill_config.repeat = M_ALIGN;
    fill_config.blk_num = 1;
    fill_config.dst_gap = 0;
    asc_fill_l1(reinterpret_cast<__cbuf__ uint16_t*>(a_l1) + M_ALIGN * GM2L1_K_ALIGN / 2, 0U, fill_config);
    fill_config.repeat = N_ALIGN;
    asc_fill_l1(reinterpret_cast<__cbuf__ uint16_t*>(b_l1) + N_ALIGN * GM2L1_K_ALIGN / 2, 0U, fill_config);
    asc_sync_pipe(PIPE_MTE1);

    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M_ALIGN / CUBE_M, K_ALIGN / CUBE_K,
        M_ALIGN / CUBE_M, M_ALIGN / CUBE_M);
    asc_copy_l12l0a_mx(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(a_l0)) / 16,
        scale_a_l1, 0, 0, M_ALIGN / CUBE_M, SCALE_K / 2, SCALE_K / 2, SCALE_K / 2);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, N_ALIGN / CUBE_M, K_ALIGN / CUBE_K,
        N_ALIGN / CUBE_M, N_ALIGN / CUBE_M);
    asc_copy_l12l0b_mx(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(b_l0)) / 16,
        scale_b_l1, 0, 0, N_ALIGN / CUBE_M, SCALE_K / 2, SCALE_K / 2, SCALE_K / 2);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_mmad_mx(c_l0, a_l0, b_l0, M, K_ALIGN, N, asc_unit_flag_mode::DISABLE, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, N, M, N, M_ALIGN, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM,
        asc_unit_flag_mode::DISABLE, QuantMode_t::NoQuant, asc_relu_pre_mode::NONE, false, true, false, false);
    asc_sync_pipe(PIPE_ALL);
}

void print_row(const char* label, const std::vector<float>& data)
{
    std::cout << label << ':';
    for (uint32_t i = 0; i < 8; ++i) std::cout << ' ' << data[i];
    std::cout << " ..." << std::endl;
}
} // namespace

int main()
{
    std::vector<uint8_t> a(M * K), b(N * K), scale_a(M * SCALE_K, 127), scale_b(N * SCALE_K, 127);
    std::vector<float> output(M * N), golden(M * N);
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t k = 0; k < K; ++k) {
            a[row * K + k] = (row + 2 * k) % 5 < 2 ? FP8_E4M3_ONE : FP8_E4M3_NEG_ONE;
        }
    }
    for (uint32_t col = 0; col < N; ++col) {
        for (uint32_t k = 0; k < K; ++k) {
            b[col * K + k] = (3 * col + k) % 7 < 3 ? FP8_E5M2_ONE : FP8_E5M2_NEG_ONE;
        }
        for (uint32_t block = 0; block < SCALE_K; ++block) {
            scale_b[col * SCALE_K + block] = static_cast<uint8_t>(126 + (col + block) % 3);
        }
    }
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t col = 0; col < N; ++col) {
            for (uint32_t k = 0; k < K; ++k) {
                const float a_value = a[row * K + k] == FP8_E4M3_ONE ? 1.0f : -1.0f;
                const float b_value = b[col * K + k] == FP8_E5M2_ONE ? 1.0f : -1.0f;
                const float b_scale = std::ldexp(1.0f,
                    static_cast<int32_t>(scale_b[col * SCALE_K + k / CUBE_K]) - 127);
                golden[row * N + col] += a_value * b_value * b_scale;
            }
        }
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *a_device = nullptr, *b_device = nullptr, *scale_a_device = nullptr, *scale_b_device = nullptr;
    float* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&scale_a_device), scale_a.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&scale_b_device), scale_b.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size(), a.data(), a.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size(), b.data(), b.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(scale_a_device, scale_a.size(), scale_a.data(), scale_a.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(scale_b_device, scale_b.size(), scale_b.data(), scale_b.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    asc_mmad_mx_kernel<<<1, 0>>>(a_device, scale_a_device, b_device, scale_b_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), output_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_row("MX output row 0", output);
    print_row("Golden row 0", golden);
    std::cout << "Scale exponent bytes: " << +scale_a[0] << ' ' << +scale_b[0] << std::endl;
    bool passed = true;
    for (uint32_t i = 0; i < output.size(); ++i) {
        if (std::fabs(output[i] - golden[i]) >= 0.01f) {
            passed = false;
            break;
        }
    }
    std::cout << (passed ? "[Success] asc_mmad_mx applied MX scales."
                         : "[Failed] asc_mmad_mx result mismatch.") << std::endl;
    aclrtFree(a_device); aclrtFree(b_device); aclrtFree(scale_a_device); aclrtFree(scale_b_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```

更多场景使用样例请参考[asc_mmad_mx样例](../../../../../../examples/02_simd_c_api/03_c_api/03_matrix_compute/mmad_mx)。
<!-- end id8 -->
