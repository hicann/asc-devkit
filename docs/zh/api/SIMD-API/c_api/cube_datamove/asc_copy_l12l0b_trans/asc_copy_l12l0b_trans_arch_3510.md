# asc_copy_l12l0b_trans

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

将数据从L1 Buffer搬运到L0B Buffer，搬运过程中以大小为512字节的数据分形为单位进行二维分形矩阵转置。本接口按分形索引、重复次数、源侧步长、目的侧间隔与分形间间隔等参数组织搬运，适用于普通矩阵计算所需2D格式数据在L1 Buffer与L0B Buffer之间伴随转置的搬运场景。

下面通过示例来讲解接口功能和关键参数：下文图中一个N形或者一个Z形代表一个分形。

- 对于uint8_t/int8_t数据类型，每次迭代处理32 × 32 × 1B数据，可处理2个分形（一个分形512B），每次迭代中，源操作数中2个连续的16 × 32分形将被合并为1个32 × 32的方块矩阵，基于方块矩阵做转置，转置后分裂为2个16 × 32分形，根据目的操作数分形间隔等参数可以有不同的排布。

如下图示例：

- 共处理3072B的数据，每次迭代处理32 × 32 × 1B数据，需要3次迭代可以完成，repeat_time = 3；
- src_stride = 1，表示相邻迭代间，源操作数前一个方块矩阵与后一个方块矩阵起始地址的间隔为1（单位：32 × 32 × 1B），这里的单位实际上是拼接后的方块矩阵的大小；
- dst_gap = 1，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔为1（单位：512B）；
- dst_frac_gap = 0，表示每个迭代内目的操作数前一个分形的结束地址与后一个分形起始地址的间隔为0（单位：512B）。
![](../../figures/asc_copy_l12l0_trans_b8_example1.png)

如下图示例：

- repeat_time和src_stride的解释和上图示例一致。
- dst_gap = 0，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址和下一个迭代第一个分形起始地址无间隔。
- dst_frac_gap = 2，表示每个迭代内目的操作数前一个分形的结束地址与后一个分形起始地址的间隔为2（单位：512B）。
![](../../figures/asc_copy_l12l0_trans_b8_example2.png)

- 对于half/bfloat16_t数据类型，每次迭代处理16 × 16 × 2B数据，可处理1个分形（一个分形512B），每次迭代中，源操作数中1个16 × 16分形将被转置。
  - 因为每次迭代处理16 × 16 × 2B数据，需要3次迭代可以完成，repeat_time = 3；
  - src_stride = 1，表示相邻迭代间，源操作数前一个方块矩阵与后一个方块矩阵起始地址的间隔为1 （单位：16 × 16 × 2B）；
  - dst_gap = 0，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址无间隔；
  - 该场景下，因为其分形即为方块矩阵，每个迭代处理一个分形，不存在迭代内分形的间隔，该参数设置无效。
![](../../figures/asc_copy_l12l0_trans_b16_example.png)

- 对于float/int32_t/uint32_t数据类型，每次迭代处理16 × 16 × 4B数据，可处理2个分形（一个分形512B），每次迭代中，源操作数2个连续的16 × 8分形将被合并为1个16 × 16的方块矩阵，基于方块矩阵做转置，转置后分裂为2个16 × 8分形，根据目的操作数分形间隔等参数可以有不同的排布。
如下图示例：
  - 因为每次迭代处理16 × 16 × 4B数据，需要3次迭代可以完成，repeat_time = 3；
  - src_stride = 1，表示相邻迭代间，源操作数前一个方块矩阵与后一个方块矩阵起始地址的间隔为1（单位：16 × 16 × 4B），这里的单位实际上是拼接后的方块矩阵的大小；
  - dst_gap = 1，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔为1（单位：512B）；
  - dst_frac_gap = 0，表示每个迭代内目的操作数前一个分形结束地址与后一个分形起始地址的间隔为0（单位：512B）。
![](../../figures/asc_copy_l12l0_trans_b32_example1.png)

如下图示例：
- repeat_time和src_stride的解释和上图示例一致。
- dst_gap = 0，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址和下一个迭代第一个分形起始地址无间隔。
- dst_frac_gap = 2，表示每个迭代内目的操作数前一个分形结束地址与后一个分形起始地址的间隔为2（单位：512B）。
![](../../figures/asc_copy_l12l0_trans_b32_example2.png)

- 对于int4b\_t数据类型，每次迭代处理64\*64\*0.5B数据，可处理4个分形（一个分形512B），每次迭代中，源操作数中4个连续的16\*64分形将被合并为1个64\*64的方块矩阵，基于方块矩阵做转置，转置后分裂为4个16\*64分形，根据目的操作数分形间隔等参数可以有不同的排布。

    int4b\_t数据类型需要两个数拼成一个int8\_t或uint8\_t的数，拼凑的规则如下：

    ![](../../figures/int4b_example.png)

  如下图所示：
  - 共需要处理6144B的数据，每次迭代处理64\*64\*0.5B数据，需要3次迭代可以完成，repeat_time = 3；
  - src_stride = 1，表示相邻迭代间，源操作数前一个方块矩阵与后一个方块矩阵起始地址的间隔为1（单位：64\*64\*0.5B），这里的单位实际上是拼接后的方块矩阵的大小；
  - dst_gap = 1，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔为1（单位：512B）；
  - dst_frac_gap = 0，表示每个迭代内目的操作数前一个分形的结束地址与后一个分形起始地址的间隔为0（单位：512B）。
  
  ![](../../figures/asc_copy_l12l0_trans_b4_example1.png)

  如下图示例：

  - repeat_time和src_stride的解释和上图示例一致。
  - dst_gap = 0，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址和下一个迭代第一个分形起始地址无间隔。
  - dst_frac_gap = 2，表示每个迭代内目的操作数前一个分形的结束地址与后一个分形起始地址的间隔为2（单位：512B）。

  ![](../../figures/asc_copy_l12l0_trans_b4_example2.png)

本接口仅在AIC上执行有效。

## 函数原型

```c
// 占位符形式
__aicore__ inline void asc_copy_l12l0b_trans(__cb__ <dtype>* dst,
                                             __cbuf__ <dtype>* src,
                                             uint16_t index_id,
                                             uint8_t repeat,
                                             uint16_t src_stride,
                                             uint16_t dst_gap,
                                             uint16_t dst_frac_gap,
                                             uint16_t src_frac_gap)
__aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ <dtype>* dst,
                                                  __cbuf__ <dtype>* src,
                                                  uint16_t index_id,
                                                  uint8_t repeat,
                                                  uint16_t src_stride,
                                                  uint16_t dst_gap,
                                                  uint16_t dst_frac_gap,
                                                  uint16_t src_frac_gap)
```

### dtype支持的数据类型

dtype支持的数据类型为`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

### 函数原型典型示例

```c
// 示例：half类型的转置搬运。
__aicore__ inline void asc_copy_l12l0b_trans(__cb__ half* dst,
                                             __cbuf__ half* src,
                                             uint16_t index_id,
                                             uint8_t repeat,
                                             uint16_t src_stride,
                                             uint16_t dst_gap,
                                             uint16_t dst_frac_gap,
                                             uint16_t src_frac_gap)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数，存储位置为L0B Buffer。目的地址需512字节对齐。 |
| src | 输入 | 源操作数，存储位置为L1 Buffer。源地址需32字节对齐。 |
| index_id | 输入 | 源侧起始分形索引，即源矩阵中起始分形矩阵的序号（从0计起）。取值范围：[0, $2^{16}-1$]。 |
| repeat | 输入 | 迭代搬运次数。取值范围 [1, 255]。<br> 对于`b4`数据类型，每次迭代处理4个分形，每个分形为16\*64\*0.5B数据；<br> 对于`b8`数据类型，每次迭代处理2个分形，每个分形为16\*32\*1B数据；<br>对于`b16`数据类型，每次迭代处理1个分形，每个分形为16\*16\*2B数据；对于`b32`数据类型，每次迭代处理4个分形，每个分形为16\*8\*4B数据。 |
| src_stride | 输入 | 相邻repeat之间，源操作数前一个分形与后一个分形起始地址的间隔，单位512字节。取值范围：[0, $2^{16}-1$]。 |
| dst_gap | 输入 | 相邻repeat之间，目的操作数前一个分形结束地址与后一个分形起始地址的间隔，单位512字节。指令实际生效步长为传入值加1（即传入0表1个分形步长）。取值范围：[0, $2^{16}-1$]。 |
| dst_frac_gap | 输入 | 同一repeat内目的操作数前一个分形结束地址与后一个分形起始地址的间隔，单位512字节。指令实际生效步长为传入值加1，并且仅b4/b8/b32下参数生效。取值范围：[0, $2^{16}-1$]。 |
| src_frac_gap | 输入 | 同一repeat内源操作数前一个分形结束地址与后一个分形起始地址的间隔，单位512字节。指令实际生效步长为传入值加1，并且仅b4/b8/b32下参数生效。取值范围：[0, $2^{16}-1$]。 |

## 返回值说明

无

## 流水类型

PIPE_MTE1

## 约束说明

- 本接口非AIC调用直接返回。
- src起始地址需32字节对齐（L1 Buffer对齐要求），否则触发地址对齐异常。
- dst起始地址需512字节对齐（L0B Buffer对齐要求），否则触发地址对齐异常。
- 如果本指令与其他指令存在目的地址重叠，需要插入同步指令（[asc_sync_notify](../../sync/asc_sync_notify.md)和[asc_sync_wait](../../sync/asc_sync_wait.md)），保证多个指令串行化，防止出现异常数据。
- L1 Buffer容量上限：L1 Buffer总容量512KB，src偏移与搬运大小之和不可越界，否则触发地址溢出异常。
- L0B Buffer容量上限：L0B Buffer总容量64KB，dst偏移与搬运大小之和不可越界，否则触发地址溢出异常。
- `repeat`为0时表示不执行搬运，本接口被视为NOP（空操作）。
- 用户需要保证目的操作数转置后的分形没有重叠，否则会导致搬运结果不符合预期。

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
constexpr uint32_t BYTES = K * N * sizeof(half);

__global__ __cube__ void CopyL12l0bTransKernel(__gm__ half* input)
{
    asc_init();
    __cbuf__ half l1[K * N];
    __cb__ half l0b[K * N];
    asc_copy_gm2l1(l1, input, 1, BYTES / 32, 0, 0, 0);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12l0b_trans(l0b, l1, static_cast<uint16_t>(0), static_cast<uint8_t>(N / 16),
        static_cast<uint16_t>(K / 16), static_cast<uint16_t>(0), static_cast<uint16_t>(0),
        static_cast<uint16_t>(0));
    asc_sync_pipe(PIPE_MTE1);
}

int main()
{
    aclInit(nullptr); aclrtSetDevice(0);
    half* input = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    CopyL12l0bTransKernel<<<1, 0>>>(input);
    const aclError ret = aclrtSynchronizeDevice();
    std::printf("M=%u N=%u K=%u asc_copy_l12l0b_trans=%s\n", M, N, K,
        ret == ACL_SUCCESS ? "PASS" : "FAIL");
    aclrtFree(input); aclrtResetDevice(0); aclFinalize();
    return ret == ACL_SUCCESS ? 0 : 1;
}
```
