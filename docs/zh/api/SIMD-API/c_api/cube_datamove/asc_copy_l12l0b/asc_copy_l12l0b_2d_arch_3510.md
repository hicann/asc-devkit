# asc_copy_l12l0b_arch_3510（2D矩阵搬运）

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

负责完成矩阵计算所需的2D格式数据的搬运，搬运粒度为大小为512字节的数据分形，支持非转置与转置矩阵从L1 Buffer到L0B buffer的数据搬运。

对于不同的数据类型，对应的分形矩阵如下：

- 对于b4数据类型，每个数据分形在L0B Buffer中为一个64x16的矩阵。
- 对于b8数据类型，每个数据分形在L0B Buffer中为一个32×16的矩阵。
- 对于b16数据类型，每个数据分形为一个16×16的矩阵。
- 对于b32数据类型，每个数据分形在L0B Buffer中为一个8×16的矩阵。

本接口仅在AIC上执行有效。

## 函数原型

### 2D搬运模式

```cpp
__aicore__ inline void asc_copy_l12l0b(__cb__ <dtype>* dst,
                                       __cbuf__ <dtype>* src,
                                       uint16_t m_start_position,
                                       uint16_t k_start_position,
                                       uint8_t m_step,
                                       uint8_t k_step,
                                       int16_t src_stride,
                                       uint16_t dst_stride)
__aicore__ inline void asc_copy_l12l0b_sync(__cb__ <dtype>* dst,
                                            __cbuf__ <dtype>* src,
                                            uint16_t m_start_position,
                                            uint16_t k_start_position,
                                            uint8_t m_step,
                                            uint8_t k_step,
                                            int16_t src_stride,
                                            uint16_t dst_stride)
```

### dtype支持的数据类型

dtype支持的数据类型为`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

### 函数原型典型示例

```cpp
// 示例：half类型的2D搬运。
__aicore__ inline void asc_copy_l12l0b(__cb__ half* dst,
                                       __cbuf__ half* src,
                                       uint16_t m_start_position,
                                       uint16_t k_start_position,
                                       uint8_t m_step,
                                       uint8_t k_step,
                                       int16_t src_stride,
                                       uint16_t dst_stride)
```

### 2D转置搬运模式

```cpp
__aicore__ inline void asc_copy_l12l0b_transpose(__cb__ <dtype>* dst,
                                                 __cbuf__ <dtype>* src,
                                                 uint16_t m_start_position,
                                                 uint16_t k_start_position,
                                                 uint8_t m_step,
                                                 uint8_t k_step,
                                                 int16_t src_stride,
                                                 uint16_t dst_stride)
__aicore__ inline void asc_copy_l12l0b_transpose_sync(__cb__ <dtype>* dst,
                                                      __cbuf__ <dtype>* src,
                                                      uint16_t m_start_position,
                                                      uint16_t k_start_position,
                                                      uint8_t m_step,
                                                      uint8_t k_step,
                                                      int16_t src_stride,
                                                      uint16_t dst_stride)
```

### dtype支持的数据类型

dtype支持的数据类型为`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

### 函数原型典型示例

```cpp
// 示例：half类型的2D搬运。
__aicore__ inline void asc_copy_l12l0b_transpose(__cb__ half* dst,
                                       __cbuf__ half* src,
                                       uint16_t m_start_position,
                                       uint16_t k_start_position,
                                       uint8_t m_step,
                                       uint8_t k_step,
                                       int16_t src_stride,
                                       uint16_t dst_stride)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 含义 |
| --- | --- | --- |
| dst | 输出 | 目的矩阵，存储位置为L0B Buffer。起始地址需要按照512字节对齐。 |
| src | 输入 | 源矩阵，存储位置为L1 Buffer。起始地址需要按照32字节对齐。 |
| m_start_position | 输入 | 源矩阵行方向起始分形位置。单位为16个元素，对应分形行方向的固定宽度。取值范围：[0, $2^{16}-1$]。 |
| k_start_position | 输入 | 源矩阵列方向起始分形位置。单位为32字节，对应分形列方向的基本寻址粒度。取值范围：[0, $2^{16}-1$]。 |
| m_step | 输入 | 源矩阵行方向搬运的分形行数。单位为16个元素，对应分形行方向的固定宽度。取值范围：[0, 255]。<br>转置场景下（调用`asc_copy_l12l0b_transpose`），m_step除需满足取值范围外，还需满足以下额外约束：<br>&nbsp;&nbsp;&bull;当数据类型为b4时，m_step必须是4的倍数；<br>&nbsp;&nbsp;&bull;当数据类型为b8时，m_step必须是2的倍数；<br>&nbsp;&nbsp;&bull;当数据类型为b16时，m_step必须是1的倍数；<br>&nbsp;&nbsp;&bull;当数据类型为b32时，m_step无额外约束。<br>**注：m_step=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| k_step | 输入 | 源矩阵列方向搬运的分形列数。单位为32字节，对应分形列方向的基本寻址粒度。取值范围：[0, 255]。<br>转置场景下（调用`asc_copy_l12l0b_transpose`），k_step除需满足取值范围外，还需满足以下额外约束：<br>&nbsp;&nbsp;&bull;当数据类型为b4、b8或b16时，k_step没有额外约束；<br>&nbsp;&nbsp;&bull;当数据类型为b32时，k_step必须是2的倍数。<br>**注：k_step=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| src_stride | 输入 | 源矩阵列方向相邻分形起始地址的间隔。单位为512字节，即一个数据分形的大小。取值范围：[$-2^{15}$, $2^{15}-1$]。 |
| dst_stride | 输入 | 目的矩阵列方向相邻分形起始地址的间隔。单位为512字节，即一个数据分形的大小。取值范围：[1, $2^{16}-1$]。 |

## 返回值说明

无

## 流水类型

PIPE_MTE1

## 约束说明

### 通用约束

- 本接口非AIC调用直接返回。
- src位于L1 Buffer，dst位于L0B Buffer，二者位于不同的物理存储单元，不存在地址重叠。
- 如果本指令与其他指令存在目的地址重叠，需要插入同步指令（[asc_sync_notify](../../sync/asc_sync_notify.md)和[asc_sync_wait](../../sync/asc_sync_wait.md)），保证多个指令串行化，防止出现异常数据。
- L0B Buffer容量上限：L0B Buffer总容量64KB，dst偏移量与搬运大小之和不可越界，否则触发写溢出异常。
- L1 Buffer容量上限：L1 Buffer总容量512KB，src偏移量与源矩阵占用大小之和不可越界，否则触发异常。

### 2D分形搬运约束

- m_step或k_step为0时不执行搬运，本接口被视为NOP（空操作）。应避免无意传入0，否则不会产生有效数据且会增加指令调度开销。
- 最小搬运粒度为1个512字节的数据分形。矩阵边缘不足一个分形时，源矩阵需要补齐到完整分形；搬运会包含补齐区域中的无效数据。
- 对于矩阵转置场景，不同的数据类型需要满足不同的m_step和k_step约束：b4数据类型时，m_step必须是4的倍数；b8数据类型时，m_step必须是2的倍数；b16数据类型时，m_step必须是1的倍数；b32数据类型时，k_step必须是2的倍数。
- 对于矩阵转置场景，由于存在m_step和k_step约束，需要确保L1 Buffer预留足够的空间，避免发生读越界，导致搬运出现未定义行为；
- 对于b8转置场景搬运，即调用`asc_copy_l12l0b_transpose`接口，假设C矩阵大小为（M，N），若$N > 32$且$N \bmod 32 \le 16$：
  - 此场景下若只调用一次`asc_copy_l12l0b_transpose`搬运接口，由于b8转置场景最小搬运单位为两个分形，会导致N方向多搬1个无效分形，后续Mmad计算时需要让此分型参与计算，否则会导致计算结果错误，并且最后在矩阵搬出时，只搬出有效区域。在此调用方式下，计算的数据大小和搬出的数据大小不一致，导致无法在矩阵搬出时开启UnitFlag功能。
  - 若多次调用for循环调用`asc_copy_l12l0b_transpose`搬运接口，需合理设置`dst_stride`参数，写入L0B Buffer时跳过转置多读的N方向无效分型，则Mmad计算时无需让此分型参与计算，计算和矩阵搬出的数据大小保持一致，此场景下可以在矩阵搬出时开启UnitFlag功能。但需要注意采用for循环多次搬运，会容易导致MTE1指令队列满，极致流水场景下可能会造成流水阻塞，需合理选择是否使用for循环多次搬运。

## 调用示例

将代码保存为`examples.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

样例单核占用32KB L1 Buffer、16KB L0A Buffer、16KB L0B Buffer和64KB L0C Buffer。样例依次复用`EVENT_ID0`完成MTE2到MTE1、MTE1到M、M到FIX的同步。

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t M = 128;
constexpr uint32_t K = 128;
constexpr uint32_t N = 128;
constexpr uint32_t ELEMENTS = M * N;
constexpr uint32_t CUBE_M = 16;
constexpr uint32_t CUBE_K = 32;

__global__ __cube__ void AscCopyL12l0bKernel(__gm__ int8_t* a, __gm__ int8_t* b,
    __gm__ int32_t* output)
{
    asc_init();
    __cbuf__ int8_t a_l1[M * K], b_l1[N * K];
    __ca__ int8_t a_l0[M * K];
    __cb__ int8_t b_l0[K * N];
    __cc__ int32_t c_l0[ELEMENTS];

    // 将A矩阵和B矩阵从GM搬入L1 Buffer，并转换为Nz格式。
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(a_l1, a, K, 0, M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(b_l1, b, K, 0, N, K, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // 将A矩阵和B矩阵从L1 Buffer分别搬入L0A Buffer和L0B Buffer。
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE_M, K / CUBE_K, M / CUBE_M, M / CUBE_M);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE_M, K / CUBE_K, N / CUBE_M, N / CUBE_M);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    // 执行M=N=K=128的矩阵乘法。
    asc_mmad(c_l0, a_l0, b_l0, M, K, N, 0, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    // 将L0C Buffer中的Nz结果转换为ND格式并搬出到GM。
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, N, M, N, M, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
    asc_sync_pipe(PIPE_FIX);
}

template <typename T>
void PrintRow(const char* label, const std::vector<T>& data)
{
    std::cout << label << ':';
    for (uint32_t i = 0; i < 8; ++i) {
        std::cout << ' ' << +data[i];
    }
    std::cout << " ..." << std::endl;
}
} // namespace

int main()
{
    // 构造输入矩阵，并在Host侧计算Golden结果。
    std::vector<int8_t> a(M * K), b(N * K);
    std::vector<int32_t> output(ELEMENTS), golden(ELEMENTS);
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t k = 0; k < K; ++k) {
            a[row * K + k] = static_cast<int8_t>(static_cast<int32_t>((row + 2 * k) % 5) - 2);
        }
    }
    for (uint32_t col = 0; col < N; ++col) {
        for (uint32_t k = 0; k < K; ++k) {
            b[col * K + k] = static_cast<int8_t>(static_cast<int32_t>((3 * col + k) % 5) - 2);
        }
    }
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t col = 0; col < N; ++col) {
            for (uint32_t k = 0; k < K; ++k) {
                golden[row * N + col] += a[row * K + k] * b[col * K + k];
            }
        }
    }

    // 分配Device内存、执行Kernel并取回结果。
    aclInit(nullptr);
    aclrtSetDevice(0);
    int8_t *a_device = nullptr, *b_device = nullptr;
    int32_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(int32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size(), a.data(), a.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size(), b.data(), b.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    AscCopyL12l0bKernel<<<1, 0>>>(a_device, b_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int32_t), output_device,
        output.size() * sizeof(int32_t), ACL_MEMCPY_DEVICE_TO_HOST);

    PrintRow("Output row 0", output);
    PrintRow("Golden row 0", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_copy_l12l0b result is correct."
                         : "[Failed] asc_copy_l12l0b result mismatch.") << std::endl;

    aclrtFree(a_device);
    aclrtFree(b_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
