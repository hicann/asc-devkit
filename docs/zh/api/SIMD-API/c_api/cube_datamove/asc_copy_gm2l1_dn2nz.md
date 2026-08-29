# asc_copy_gm2l1_dn2nz

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

将数据从Global Memory搬运到L1 Buffer，并在搬运过程中进行DN到Nz格式的转换，使其满足后续矩阵计算对分形列连续性的要求。

进行DN2Nz格式搬运前，需先通过[asc_set_gm2l1_nz_para](asc_set_gm2l1_nz_para.md)接口配置DN2Nz搬运的目的Nz矩阵步长与DN矩阵搬运个数。

本接口仅在AIC上执行有效。

如[图1](#dn2nz_equivalent_conversion)所示，DN到Nz的格式转换也等价于ND到Zn的格式转换，如果要进行ND到Zn的格式转换可采用此指令。

**图1** DN2Nz与ND2Zn示意图<a id="dn2nz_equivalent_conversion"></a>

![](../../../figures/gm2l1_dn2nz_concept.png)

## 函数原型

### 模板原型（占位符形式）

```cpp
__aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ <dtype>* dst,
                                            __gm__ <dtype>* src,
                                            uint64_t loop1_src_stride,
                                            uint8_t l2_cache_ctl,
                                            uint16_t n_value,
                                            uint32_t d_value,
                                            uint64_t loop4_src_stride,
                                            bool smallc0_en)
__aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ <dtype>* dst,
                                                 __gm__ <dtype>* src,
                                                 uint64_t loop1_src_stride,
                                                 uint8_t l2_cache_ctl,
                                                 uint16_t n_value,
                                                 uint32_t d_value,
                                                 uint64_t loop4_src_stride,
                                                 bool smallc0_en)
```

### dtype支持的数据类型

dtype支持的数据类型为`int8_t`、`uint8_t`、`hifloat8_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

### 典型示例

```cpp
// 示例：half类型的DN到Nz格式搬运。
__aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ half* dst,
                                            __gm__ half* src,
                                            uint64_t loop1_src_stride,
                                            uint8_t l2_cache_ctl,
                                            uint16_t n_value,
                                            uint32_t d_value,
                                            uint64_t loop4_src_stride,
                                            bool smallc0_en)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数，存储位置为L1 Buffer。目的地址需32字节对齐。 |
| src | 输入 | 源操作数，存储位置为GM。起始地址需要按照1字节对齐。 |
| loop1_src_stride | 输入 | 源操作数同一DN矩阵的相邻行起始地址间的偏移，单位字节。取值范围：[0, $2^{40}-1$]。 |
| l2_cache_ctl | 输入 | 配置输入的GM数据在L2 Cache中的管理策略。取值需为已定义的策略（0/1/2/4之一），其他值触发L2 Cache策略异常。取值说明见[表2](#l2_cache_ctl_values)。 |
| n_value | 输入 | 源矩阵中DN矩阵的列数，单位为元素。取值范围：[1, 65535]。 |
| d_value | 输入 | 源矩阵中DN矩阵的行数，单位为元素。取值范围：[1, $2^{21}-1$]。 |
| loop4_src_stride | 输入 | 源矩阵相邻DN矩阵起始地址间的偏移，单位为字节。取值范围：[0, $2^{40}-1$]。<br>&nbsp;&nbsp;&bull; 源矩阵DN矩阵个数为1时，此参数无意义，设置为0即可。<br>&nbsp;&nbsp;&bull; 源矩阵DN矩阵个数大于1时，当`loop4_src_stride = 0`时，表示重复搬出源矩阵的第一个DN矩阵。 |
| smallc0_en | 输入 | SmallC0模式开关，仅当$d\_value \le 4$时可开启，否则导致搬运异常。<br>&nbsp;&nbsp;&bull; `false`：不开启SmallC0模式，按标准Nz格式的C0（32字节）排布搬运；<br>&nbsp;&nbsp;&bull; `true`：开启SmallC0模式，搬运过程中搬运的数据量不满足$4 \times \mathrm{sizeof}(\mathrm{dtype})$字节对齐时，硬件在目的矩阵中自动补0至$4 \times \mathrm{sizeof}(\mathrm{dtype})$字节对齐。 |

**表2** `l2_cache_ctl`取值说明 <a id="l2_cache_ctl_values"></a>

| 取值 | 模式 | 含义 |
|------|------|------|
| 0 | Normal模式 | 启用L2 Cache，并将分配的Cache Line标记为高替换优先级。 |
| 1 | Last模式 | &bull; 启用L2 Cache，并将分配的Cache Line标记为低替换优先级。<br>&bull; **Last模式暂不支持。**|
| 2 | Persistent模式 | &bull; 启用L2 Cache。已存入L2 Cache中的数据可能被替换，若需确保特定GM的数据始终保留在L2 Cache中，可采用驻留模式。<br>&bull; 被标记为驻留模式的Cache Line只能被其他同样标记为驻留模式的Cache Line替换。<br>&bull; **Persistent模式暂不支持。**|
| 4 | Disable模式 | 不启用L2 Cache，每次都直接从GM中读取，并保持已有Cache Line的状态不变。 |

## 返回值说明

无

## 流水类型

PIPE_MTE2

## 约束说明

### 通用约束

- 本接口非AIC调用直接返回。
- `dst`的起始地址要求32字节对齐（L1 Buffer对齐要求），否则会导致搬运异常。
- `src`的起始地址要求1字节对齐（Global Memory对齐要求），否则会导致搬运异常。
- 如果本指令与其他指令存在目的地址重叠，需要插入同步指令（[asc_sync_notify](../sync/asc_sync_notify.md)和[asc_sync_wait](../sync/asc_sync_wait.md)），保证多个指令串行化，防止出现异常数据。
- L1 Buffer容量上限：L1 Buffer总容量512KB，`dst`偏移量与搬运大小之和不可越界，否则触发异常。

### DN2Nz约束

- 调用本接口前，需要先调用[asc_set_gm2l1_nz_para](asc_set_gm2l1_nz_para.md)接口配置DN2Nz搬运的目的Nz矩阵步长与DN矩阵搬运个数。
- `loop1_src_stride`、`n_value`、`d_value`、`loop4_src_stride`取值需确保在取值范围内，超出取值范围的值会被截断，导致搬运结果不符合预期。
- `smallc0_en`设置为`true`时，$d\_value \le 4$，否则导致搬运异常。

## 调用示例

将代码保存为`examples.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

样例单核占用64KB L1 Buffer、32KB L0A Buffer、32KB L0B Buffer和64KB L0C Buffer。样例依次复用`EVENT_ID0`完成MTE2到MTE1、MTE1到M、M到FIX的同步。基础API同类场景可参考[data_copy_gm2l1样例](../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_gm2l1)中的场景3。

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
constexpr uint32_t CUBE_K = 16;
constexpr uint16_t HALF_ONE = 0x3c00;
constexpr uint16_t HALF_NEG_ONE = 0xbc00;

__global__ __cube__ void AscCopyGm2l1Dn2nzKernel(
    __gm__ uint16_t* a, __gm__ uint16_t* b, __gm__ float* output)
{
    asc_init();
    __cbuf__ half a_l1[M * K], b_l1[N * K];
    __ca__ half a_l0[M * K];
    __cb__ half b_l0[K * N];
    __cc__ float c_l0[ELEMENTS];

    // 输入A、B分别按[K, M]和[N, K]的DN格式存放，搬入后转换为Nz格式。
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_dn2nz(reinterpret_cast<__cbuf__ half*>(a_l1), reinterpret_cast<__gm__ half*>(a),
        M * sizeof(half), 0, M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_dn2nz(reinterpret_cast<__cbuf__ half*>(b_l1), reinterpret_cast<__gm__ half*>(b),
        K * sizeof(half), 0, K, N, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // 将Nz矩阵分别搬入L0A Buffer和L0B Buffer，供矩阵乘计算使用。
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE_M, K / CUBE_K,
        M / CUBE_M, M / CUBE_M);
    asc_copy_l12l0b_transpose(b_l0, b_l1, 0, 0, K / CUBE_M, N / CUBE_K,
        K / CUBE_M, N / CUBE_M);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    // 计算128 x 128矩阵乘，并将结果从L0C Buffer搬回Global Memory。
    asc_mmad(c_l0, a_l0, b_l0, M, K, N, 0, false, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, N, M, N, M, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
    asc_sync_pipe(PIPE_FIX);
}

uint16_t ToHalfBits(int32_t value)
{
    return value > 0 ? HALF_ONE : (value < 0 ? HALF_NEG_ONE : 0);
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
    // 在GM中按DN格式存放A和B，即分别存放A、B的转置矩阵。
    std::vector<uint16_t> a(K * M), b(N * K);
    std::vector<float> output(ELEMENTS), golden(ELEMENTS);
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t k = 0; k < K; ++k) {
            const int32_t value = static_cast<int32_t>((row + 2 * k) % 3) - 1;
            a[k * M + row] = ToHalfBits(value);
        }
    }
    for (uint32_t k = 0; k < K; ++k) {
        for (uint32_t col = 0; col < N; ++col) {
            const int32_t value = static_cast<int32_t>((2 * k + col) % 3) - 1;
            b[col * K + k] = ToHalfBits(value);
        }
    }
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t col = 0; col < N; ++col) {
            int32_t sum = 0;
            for (uint32_t k = 0; k < K; ++k) {
                const int32_t a_value = static_cast<int32_t>((row + 2 * k) % 3) - 1;
                const int32_t b_value = static_cast<int32_t>((2 * k + col) % 3) - 1;
                sum += a_value * b_value;
            }
            golden[row * N + col] = static_cast<float>(sum);
        }
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t *a_device = nullptr, *b_device = nullptr;
    float* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size() * sizeof(uint16_t), a.data(), a.size() * sizeof(uint16_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size() * sizeof(uint16_t), b.data(), b.size() * sizeof(uint16_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    AscCopyGm2l1Dn2nzKernel<<<1, 0>>>(a_device, b_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), output_device,
        output.size() * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);

    PrintRow("Output row 0", output);
    PrintRow("Golden row 0", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_copy_gm2l1_dn2nz preserved the MMAD operands/result."
                         : "[Failed] asc_copy_gm2l1_dn2nz result mismatch.") << std::endl;

    aclrtFree(a_device);
    aclrtFree(b_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
