# asc_fill_l1

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

将一个标量填充值写入L1 Buffer的目的地址，并将填充值广播到多个32字节分形块中，可用于L1 Buffer的内存初始化。需要通过[asc_fill_value_config](../defs/union/asc_fill_value_config.md)配置的重复次数、单次分形块数、相邻重复之间的间隔。

本接口仅在AIC上执行有效。

## 函数原型

```cpp
__aicore__ inline void asc_fill_l1(__cbuf__ <dst_dtype>* dst,
                                   <value_dtype> value,
                                   const asc_fill_value_config& config)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ <dst_dtype>* dst,
                                        <value_dtype> value,
                                        const asc_fill_value_config& config)
```

### dtype支持的数据类型

dst dtype与value dtype支持以下组合：

- `dst_dtype`为`int16_t`、`uint16_t`、`half`、`int32_t`、`uint32_t`或`float`时，`value_dtype`支持`half`、`uint32_t`。
- `dst_dtype`为`bfloat16_t`时，`value_dtype`支持`half`、`bfloat16_t`、`uint32_t`。

### 函数原型典型示例

```cpp
// 示例：使用half类型的值填充half类型的L1 Buffer。
__aicore__ inline void asc_fill_l1(__cbuf__ half* dst,
                                   half value,
                                   const asc_fill_value_config& config)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 含义 |
|---|---|---|
| dst | 输出 | 目的操作数，目的矩阵的起始地址，存储位置为L1 Buffer，起始地址需要按照32字节对齐。 |
| value | 输入 | 源操作数，标量填充值。|
| config | 输入 | 填充配置的结构体[asc_fill_value_config](../defs/union/asc_fill_value_config.md)，结构体说明见[表2](#fill_value_config_fields)。 |

**表2** config字段说明 <a id="fill_value_config_fields"></a>

| 字段名 | 含义 |
|---|---|
| repeat | 重复次数，即填充值按`blk_num`个32字节块写入L1 Buffer后，再以`dst_gap`为间隔重复写入的总次数。取值范围：[1，32767]。 |
| blk_num | 单次重复写入的32字节块数，即一次重复内连续填充的32字节块个数。取值范围：[1，32767]。 |
| dst_gap | 相邻两次重复之间的间隔，单位为32字节块。相邻两次填充的起始地址偏移为$(\mathrm{blk}_{\mathrm{num}} + \mathrm{dst}_{\mathrm{gap}}) \times 32$字节。取值范围：[0，32767]。 |

如[图1](#fill_l1_layout)所示，`value = 0`表示填充值为0，`repeat = 5`表示执行5次填充，`blk_num = 3`表示每次连续填充3个32字节块，`dst_gap = 5`表示相邻两次填充之间间隔5个32字节块。

**图1** L1 Buffer中Fill示意图<a id="fill_l1_layout"></a>
![L1 Buffer中Fill的数据排布](../../../figures/Fill_L1.png "L1 Buffer中Fill示意图")

## 返回值说明

无

## 流水类型

PIPE_MTE2

## 约束说明

- 本接口非AIC调用直接返回。
- `dst`起始地址需要按照32字节对齐（L1 Buffer对齐要求），否则触发地址对齐异常。
- L1 Buffer总容量为512KB。`dst`偏移量与实际占用空间之和不可超过L1 Buffer容量，否则触发地址溢出异常。
- 如果本指令与其他指令存在目的地址重叠，需要插入同步指令（[asc_sync_notify](../sync/asc_sync_notify.md)和[asc_sync_wait](../sync/asc_sync_wait.md)），保证多个指令串行化，防止出现异常数据。调用`asc_fill_l1_sync`时，接口内部完成同步。
- `value`的数据类型需与`dst`的数据类型满足函数原型章节中的组合关系。两者数据类型不相同时，需要根据`dst`的数据类型转换`value`的值，具体请参考调用示例。

## 调用示例

将代码保存为`examples.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id9 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id9 -->

样例单核占用64KB L1 Buffer、32KB L0A Buffer、32KB L0B Buffer和64KB L0C Buffer。样例依次复用`EVENT_ID0`完成MTE2到MTE1、MTE1到M、M到FIX的同步。

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
constexpr uint32_t CUBE_BLOCK = 16;
constexpr uint32_t BLOCK_BYTES = 32;

__global__ __cube__ void AscFillL1Kernel(__gm__ float* output)
{
    asc_init();
    __cbuf__ half a_l1[M * K], b_l1[N * K];
    __ca__ half a_l0[M * K];
    __cb__ half b_l0[K * N];
    __cc__ float c_l0[ELEMENTS];

    // 使用两种value类型将L1中的A、B矩阵全部填充为1。
    asc_fill_value_config config;
    config.repeat = M;
    config.blk_num = K * sizeof(half) / BLOCK_BYTES;
    config.dst_gap = 0;
    asc_fill_l1(a_l1, static_cast<half>(1.0f), config);

    config.repeat = N;
    // 0x3C00为half类型的1.0，uint32_t中打包两个该值。
    asc_fill_l1(b_l1, 0x3C003C00U, config);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // 将填充后的矩阵搬入L0A和L0B。
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE_BLOCK, K / CUBE_BLOCK,
        M / CUBE_BLOCK, M / CUBE_BLOCK);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE_BLOCK, K / CUBE_BLOCK,
        N / CUBE_BLOCK, N / CUBE_BLOCK);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    // 计算矩阵乘；两个输入矩阵均为1，因此每个结果元素均为K。
    asc_mmad(c_l0, a_l0, b_l0, M, K, N, 0, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, N, M, N, M, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
    asc_sync_pipe(PIPE_FIX);
}

void PrintRow(const char* label, const std::vector<float>& data)
{
    std::cout << label << ':';
    for (uint32_t i = 0; i < 8; ++i) {
        std::cout << ' ' << data[i];
    }
    std::cout << " ..." << std::endl;
}
} // namespace

int main()
{
    std::vector<float> output(ELEMENTS);
    const std::vector<float> golden(ELEMENTS, static_cast<float>(K));

    // 分配Device内存、启动Kernel并校验结果。
    aclInit(nullptr);
    aclrtSetDevice(0);
    float* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    AscFillL1Kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), output_device,
        output.size() * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);

    PrintRow("Output row 0", output);
    PrintRow("Golden row 0", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_fill_l1 result is correct."
                         : "[Failed] asc_fill_l1 result mismatch.") << std::endl;

    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
