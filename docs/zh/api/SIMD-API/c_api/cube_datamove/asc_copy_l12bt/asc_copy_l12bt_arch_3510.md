# asc_copy_l12bt

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

头文件路径为：`"c_api/cube_datamove/cube_datamove.h"`。

将L1 Buffer中的偏置数据搬运到BiasTable Buffer，供矩阵计算单元（Mmad）作为bias输入读取。支持源操作数为`half`、`bfloat16_t`、`int32_t`、`float`类型搬运，其中搬运过程对于`half`、`bfloat16_t`会进行随路格式转换后写入BiasTable Buffer：`bfloat16_t`自动转换为`float`；`half`在开启转换即`conv_control`设置为1时转换为`float`，未开启转换即`conv_control`设置为0时每个`half`元素仅扩展为32 bit（高16 bit为占位数据）。

本接口提供两种参数列表完全不同的功能模式，用户按需选择对应重载：

- **高维切分搬运**：通过配置数据块个数、单个数据块长度、源/目间隔等搬运参数实现连续与非连续搬运。
- **连续数据搬运**：仅指定`size`，按照一个连续数据块搬运，源/目间隔均为0。

本接口仅在AIC上执行有效。

## 函数原型

### 高维切分搬运模式

```c
// 占位符形式
__aicore__ inline void asc_copy_l12bt(uint64_t dst,
                                      __cbuf__ <dtype>* src,
                                      uint16_t conv_control,
                                      uint16_t n_burst,
                                      uint16_t len_burst,
                                      uint16_t source_gap,
                                      uint16_t dst_gap)
```

#### dtype支持的数据类型

dtype支持的数据类型为`half`、`bfloat16_t`、`int32_t`、`float`。

#### 函数原型典型示例

```c
// 示例：bfloat16_t类型的高维切分搬运。
__aicore__ inline void asc_copy_l12bt(uint64_t dst,
                                      __cbuf__ bfloat16_t* src,
                                      uint16_t conv_control,
                                      uint16_t n_burst,
                                      uint16_t len_burst,
                                      uint16_t source_gap,
                                      uint16_t dst_gap)
```

### 连续数据搬运模式

```c
// 占位符形式
__aicore__ inline void asc_copy_l12bt(uint64_t dst,
                                      __cbuf__ <dtype>* src,
                                      uint32_t size)
__aicore__ inline void asc_copy_l12bt_sync(uint64_t dst,
                                           __cbuf__ <dtype>* src,
                                           uint32_t size)
```

#### dtype支持的数据类型

dtype支持的数据类型为`half`、`bfloat16_t`、`int32_t`、`float`。

#### 函数原型典型示例

```c
// 示例：float类型的连续数据搬运。
__aicore__ inline void asc_copy_l12bt(uint64_t dst,
                                      __cbuf__ float* src,
                                      uint32_t size)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数，存储位置为BiasTable Buffer。目的地址需64字节对齐。 |
| src | 输入 | 源操作数，存储位置为L1 Buffer。源地址需32字节对齐。 |
| conv_control | 输入 | 源操作数为`half`类型时，是否开启随路格式转换。<br>&nbsp;&nbsp;&bull; `0`：不开启转换，每个`half`元素扩展为32 bit（高16 bit为占位数据）后写入BT；<br>&nbsp;&nbsp;&bull; `1`：开启转换，每个`half`元素转换为`float`后写入BT。仅dtype为`half`时可设置，其他dtype需设置为0。 |
| n_burst | 输入 | 待搬运的连续数据块个数。取值范围 [1, 4095]。 |
| len_burst | 输入 | 每个连续数据块的长度，单位32字节。取值范围 [1, 65535]。dtype为`float`或`int32_t`时需2对齐（即取值为偶数），否则触发异常。 |
| source_gap | 输入 | 源端（L1 Buffer）相邻两次搬运之间的间隔，单位32字节，从上一次搬运结束位置到下一次搬运起始位置。取值范围 [0, 65535]。`n_burst=1`时无意义，置0即可。 |
| dst_gap | 输入 | 目的端（BiasTable Buffer）相邻两次搬运之间的间隔，单位32字节，从上一次搬运结束位置到下一次搬运起始位置。取值范围 [0, 65535]。`n_burst=1`时无意义，置0即可。dtype为`float`或`int32_t`时需2对齐（即取值为偶数），否则触发异常。 |
| size | 输入 | 搬运数据总大小，单位字节。<br>**注：size需要32字节对齐，若未对齐，搬运量会向下取整到32字节对齐。** |

## 返回值说明

无

## 流水类型

PIPE_MTE1

## 约束说明

### 通用约束

- 本接口非AIC调用直接返回。
- dst起始地址需64字节对齐（BiasTable Buffer对齐要求），src起始地址需32字节对齐（L1 Buffer对齐要求），否则触发异常。
- BiasTable Buffer容量上限4KB，dst偏移与搬运大小之和不可越界，否则触发写溢出异常。
- L1 Buffer容量上限512KB，src偏移与搬运大小之和不可越界，否则触发读异常。
- 如果本指令与其他指令存在目的地址重叠，需要插入同步指令（[asc_sync_notify](../../sync/asc_sync_notify.md)和[asc_sync_wait](../../sync/asc_sync_wait.md)），保证多个指令串行化，防止出现异常数据。
- n_burst为0时不执行搬运，本接口被视为NOP（空操作）。
- dtype为`float`或`int32_t`时，len_burst与dst_gap均必须2对齐（即取值为偶数），size/32需为偶数，否则触发搬运异常。
- conv_control开启（非0）时dtype必须为`half`，否则触发搬运异常，传入非0/1的值仅最低位生效（如2等价于0即转换关闭、3等价于1即转换开启）。

## 调用示例

将代码保存为`examples.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <cstdio>
#include "acl/acl.h"
#include "c_api/asc_simd.h"

constexpr uint32_t M = 128, N = 128, K = 128;
constexpr uint32_t BYTES = N * sizeof(half);

__global__ __cube__ void CopyL12btKernel(__gm__ half* input)
{
    asc_init();
    __cbuf__ half l1[N];
    asc_copy_gm2l1(l1, input, 1, BYTES / 32, 0, 0, 0);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12bt(0, l1, BYTES);
    asc_sync_pipe(PIPE_ALL);
}

int main()
{
    aclInit(nullptr); aclrtSetDevice(0);
    half* input = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    CopyL12btKernel<<<1, 0>>>(input);
    const aclError ret = aclrtSynchronizeDevice();
    std::printf("M=%u N=%u K=%u asc_copy_l12bt=%s\n", M, N, K,
        ret == ACL_SUCCESS ? "PASS" : "FAIL");
    aclrtFree(input); aclrtResetDevice(0); aclFinalize();
    return ret == ACL_SUCCESS ? 0 : 1;
}
```
