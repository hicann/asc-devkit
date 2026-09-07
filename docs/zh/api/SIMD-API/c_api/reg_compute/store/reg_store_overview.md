# Reg数据搬出概述

Reg数据搬出接口用于将矢量数据寄存器、掩码寄存器或非对齐寄存器中的数据写入Unified Buffer（UB）。接口在Vector Function（`__simd_vf__`标记的函数）内使用，仅在AIV上生效。

## 接口概览

### 矢量数据寄存器搬出

#### 对齐搬出

对齐搬出接口的目的地址对齐要求随接口而异，并非都要求32字节对齐。使用带偏移的接口时，偏移后的实际访问地址也须满足表中的对齐要求。

**表1**  矢量数据寄存器对齐搬出接口

| 接口名称 | 模式 | 功能简述 | 目的地址对齐要求 |
| --- | --- | --- | --- |
| [asc_storealign](asc_storealign.md) | 连续对齐搬出 | 按`mask`将一个VL（256字节）矢量数据寄存器的数据连续写入UB。 | 32字节对齐。 |
| [asc_storealign](asc_storealign.md) | 立即数偏移搬出 | 按`mask`将一个VL的数据写入`dst + offset`，`offset`单位为元素。 | 32字节对齐。 |
| [asc_storealign](asc_storealign.md) | 地址寄存器偏移搬出 | 按`mask`将一个VL的数据写入由`addr_reg`指定的偏移地址。 | 32字节对齐。 |
| [asc_storealign](asc_storealign.md) | 非连续对齐搬出 | 按`block_stride`和`repeat_stride`配置8个DataBlock的写入位置。 | 32字节对齐。 |
| [asc_storealign_postupdate](asc_storealign_postupdate.md) | 立即数偏移Post Update搬出 | 按`mask`连续搬出一个VL，并按元素偏移自动更新目的地址指针。 | 32字节对齐。 |
| [asc_storealign_postupdate](asc_storealign_postupdate.md) | 非连续对齐Post Update搬出 | 按步长配置8个DataBlock的写入位置，并按32字节单位自动更新目的地址指针。 | 32字节对齐。 |
| [asc_storealign_1st](asc_storealign_1st.md) | 首元素搬出 | 将矢量数据寄存器的首个元素写入UB。 | `dtype`对齐。 |
| [asc_storealign_1st](asc_storealign_1st.md) | 首元素立即数偏移搬出 | 将首个元素写入`dst + offset`，`offset`单位为元素。 | `dtype`对齐。 |
| [asc_storealign_1st](asc_storealign_1st.md) | 首元素地址寄存器偏移搬出 | 将首个元素写入由`addr_reg`指定的偏移地址。 | `dtype`对齐。 |
| [asc_storealign_1st_postupdate](asc_storealign_1st_postupdate.md) | 首元素Post Update搬出 | 搬出首个元素后，按元素偏移自动更新目的地址指针。 | `dtype`对齐。 |
| [asc_storealign_intlv](asc_storealign_intlv.md) | 交织连续搬出 | 将两个矢量数据寄存器按元素交织后搬出，单次搬出量为`2 x VL`（512字节）。 | 32字节对齐。 |
| [asc_storealign_intlv](asc_storealign_intlv.md) | 交织立即数偏移搬出 | 将两个寄存器交织后写入`dst + offset`，`offset`单位为元素。 | 32字节对齐。 |
| [asc_storealign_intlv](asc_storealign_intlv.md) | 交织地址寄存器偏移搬出 | 将两个寄存器交织后写入由`addr_reg`指定的偏移地址。 | 32字节对齐。 |
| [asc_storealign_pack](asc_storealign_pack.md) | 压缩连续搬出 | 按`mask`将有效元素的低半部分bit压缩后写入UB。 | 32字节对齐。 |
| [asc_storealign_pack](asc_storealign_pack.md) | 压缩立即数偏移搬出 | 将压缩结果写入`dst + offset`，`offset`单位为元素。 | 32字节对齐。 |
| [asc_storealign_pack](asc_storealign_pack.md) | 压缩地址寄存器偏移搬出 | 将压缩结果写入由`addr_reg`指定的偏移地址。 | 32字节对齐。 |
| [asc_storealign_pack_postupdate](asc_storealign_pack_postupdate.md) | 压缩Post Update搬出 | 压缩搬出后，按元素偏移自动更新目的地址指针。 | 32字节对齐。 |
| [asc_storealign_pack_quarter](asc_storealign_pack_quarter.md) | Quarter Pack连续搬出 | 按`mask`将有效32位元素的低8bit压缩后写入UB。 | 32字节对齐。 |
| [asc_storealign_pack_quarter](asc_storealign_pack_quarter.md) | Quarter Pack立即数偏移搬出 | 将Quarter Pack压缩结果写入`dst + offset`，`offset`单位为元素。 | 32字节对齐。 |
| [asc_storealign_pack_quarter](asc_storealign_pack_quarter.md) | Quarter Pack地址寄存器偏移搬出 | 将Quarter Pack压缩结果写入由`addr_reg`指定的偏移地址。 | 32字节对齐。 |
| [asc_storealign_pack_quarter_postupdate](asc_storealign_pack_quarter_postupdate.md) | Quarter Pack Post Update搬出 | Quarter Pack压缩搬出后，按元素偏移自动更新目的地址指针。 | 32字节对齐。 |
| [asc_store](asc_store.md) | 连续搬出 | 将整个矢量数据寄存器搬出，单次搬出量为一个VL。32字节对齐场景建议使用`asc_storealign`，非32字节对齐场景建议使用`asc_storeunalign`。 | `dtype`对齐。 |
| [asc_store](asc_store.md) | 带搬出数量搬出 | 将矢量数据寄存器前`count`个元素搬出。32字节对齐场景建议使用`asc_storealign`，非32字节对齐场景建议使用`asc_storeunalign`。 | `dtype`对齐。 |

#### 非对齐搬出

`asc_storeunalign*`接口通过非对齐寄存器`vector_store_unalign`暂存尾块，通过后处理接口搬出暂存的尾块。连续调用时必须复用同一个非对齐寄存器；后处理接口也必须与前序主搬出接口复用该寄存器。

**表2**  矢量数据寄存器非对齐搬出接口

| 接口名称 | 模式 | 功能简述 | 目的地址对齐要求 | 配套使用的后处理接口 |
| --- | --- | --- | --- | --- |
| [asc_storeunalign](asc_storeunalign.md) | 带搬出数量搬出 | 将前`count`个元素搬出；主块写入UB，尾块暂存至非对齐寄存器。目的地址由用户手动更新。 | `dtype`对齐，无需32字节对齐。 | [asc_storeunalign_post](asc_storeunalign_post.md)的立即数偏移搬出模式。 |
| [asc_storeunalign_postupdate](asc_storeunalign_postupdate.md) | 立即数偏移Post Update搬出 | 将前`count`个元素搬出，并自动将目的地址指针更新`count`个元素；尾块暂存至非对齐寄存器。 | `dtype`对齐，无需32字节对齐。 | [asc_storeunalign_post_postupdate](asc_storeunalign_post_postupdate.md)。 |
| [asc_storeunalign_postupdate](asc_storeunalign_postupdate.md) | 地址寄存器偏移Post Update搬出 | 固定搬出一个VL，使用并自动更新`addr_reg& count`表示的目的地址偏移；尾块暂存至非对齐寄存器。 | `dtype`对齐，无需32字节对齐。 | [asc_storeunalign_post](asc_storeunalign_post.md)的地址寄存器偏移搬出模式。 |

### 掩码寄存器搬出

#### 对齐搬出

掩码寄存器对齐搬出不支持配置额外掩码。直接搬出时写出32字节掩码数据；压缩搬出时每间隔1bit丢弃1bit，并将保留bit连续写入UB。

**表3**  掩码寄存器对齐搬出接口

| 接口名称 | 模式 | 功能简述 | 目的地址对齐要求 |
| --- | --- | --- | --- |
| [asc_storealign](asc_storealign.md) | 连续对齐搬出 | 直接将32字节掩码寄存器数据写入UB。 | 32字节对齐。 |
| [asc_storealign](asc_storealign.md) | 立即数偏移搬出 | 将掩码数据写入`dst + offset`，`offset`单位为字节。 | 32字节对齐。 |
| [asc_storealign](asc_storealign.md) | 地址寄存器偏移搬出 | 将掩码数据写入由`addr_reg`指定的偏移地址。 | 32字节对齐。 |
| [asc_storealign_postupdate](asc_storealign_postupdate.md) | 立即数偏移Post Update搬出 | 直接搬出掩码数据后，按字节偏移自动更新目的地址指针。 | 32字节对齐。 |
| [asc_storealign_pack](asc_storealign_pack.md) | 压缩连续搬出 | 每间隔1bit丢弃1bit，将保留bit压缩后写入UB。 | 16字节对齐。 |
| [asc_storealign_pack](asc_storealign_pack.md) | 压缩立即数偏移搬出 | 将压缩结果写入`dst + offset`，`offset`单位为字节。 | 16字节对齐。 |
| [asc_storealign_pack](asc_storealign_pack.md) | 压缩地址寄存器偏移搬出 | 将压缩结果写入由`addr_reg`指定的偏移地址。 | 16字节对齐。 |
| [asc_storealign_pack_postupdate](asc_storealign_pack_postupdate.md) | 压缩Post Update搬出 | 压缩搬出后，按字节偏移自动更新目的地址指针。 | 16字节对齐。 |

#### 非对齐搬出

掩码寄存器非对齐搬出也需要`vector_store_unalign`辅助保存尾块，通过后处理接口搬出暂存的尾块，并在后处理时复用该寄存器。

**表4**  掩码寄存器非对齐搬出接口

| 接口名称 | 模式 | 功能简述 | 目的地址对齐要求 | 配套使用的后处理接口 |
| --- | --- | --- | --- | --- |
| [asc_storeunalign_postupdate](asc_storeunalign_postupdate.md) | 掩码寄存器Post Update搬出 | `uint16_t`目的地址时，每2bit提取LSB并打包为16字节；`uint32_t`目的地址时，每4bit提取LSB并打包为8字节。搬出后自动更新目的地址指针，尾块暂存至非对齐寄存器。 | `uint16_t`目的地址16字节对齐；`uint32_t`目的地址8字节对齐。 | [asc_storeunalign_post](asc_storeunalign_post.md)的立即数偏移搬出模式。 |

## 关键特性

### 对齐连续搬出方式对比

以下提供四种地址偏移方式，每种方式均以两个长度为1024、数据类型为`half`的输入进行逐元素相加，并将结果连续写入UB的计算过程为例。单次迭代处理128个元素，共迭代8次，最终结果均为`dst[i] = src0[i] + src1[i]`。

**表5**  对齐连续搬出方式对比

| 场景 | 搬出方式 | 目的地址维护方式 | 适用场景 |
| --- | --- | --- | --- |
| 场景1 | 用户手动偏移地址搬出 | 调用方通过指针运算计算每次搬出的目的地址。 | 目的地址偏移规则简单，且需要由用户显式控制地址。 |
| 场景2 | 通过接口立即数偏移搬出 | 将相对目的基地址的固定元素偏移作为`int32_t`参数传入。 | 偏移量在编译期确定，且无需修改目的地址指针。 |
| 场景3 | 地址寄存器偏移搬出 | 通过[asc_update_addr_reg](../reg_addr_reg/asc_update_addr_reg.md)生成`addr_reg`，由硬件在迭代中更新偏移。 | Hardware Loop内偏移随迭代变化。 |
| 场景4 | Post Update搬出 | `asc_storealign_postupdate`在每次搬出后自动更新目的地址指针。 | 连续搬出且无需用户手动维护目的地址。 |

#### 用户手动偏移地址搬出

调用`asc_storealign`的连续对齐搬出模式。目的地址通过`dst + i * one_repeat_size`计算，`one_repeat_size`为128个`half`元素。

```cpp
vector_half src0_reg;
vector_half src1_reg;
vector_half dst_reg;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
for (uint16_t i = 0; i < repeat_times; ++i) {
    asc_loadalign(src0_reg, src0 + i * one_repeat_size);
    asc_loadalign(src1_reg, src1 + i * one_repeat_size);
    asc_add(dst_reg, src0_reg, src1_reg, mask);
    asc_storealign(dst + i * one_repeat_size, dst_reg, mask);
}
```

#### 通过接口立即数偏移搬出

调用`asc_storealign`的立即数偏移搬出模式。`offset`相对`dst`基地址，单位为元素；本样例通过`i * 128`计算8次搬出的偏移，覆盖1024个`half`元素。

```cpp
vector_half src0_reg;
vector_half src1_reg;
vector_half dst_reg;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
constexpr uint16_t REPEAT_TIMES = 8;
for (uint16_t i = 0; i < REPEAT_TIMES; ++i) {
    asc_loadalign(src0_reg, src0 + i * one_repeat_size);
    asc_loadalign(src1_reg, src1 + i * one_repeat_size);
    asc_add(dst_reg, src0_reg, src1_reg, mask);
    asc_storealign(dst, dst_reg, i * 128, mask);
}
```

#### 地址寄存器偏移搬出

调用`asc_storealign`的地址寄存器偏移搬出模式。`addr_reg`须在最内层循环中赋值并使用，偏移单位为元素。

```cpp
vector_half src0_reg;
vector_half src1_reg;
vector_half dst_reg;
vector_bool mask;
addr_reg offset;
for (uint16_t i = 0; i < repeat_times; ++i) {
    offset = asc_update_addr_reg_b16(one_repeat_size);
    mask = asc_update_mask_b16(output_length);
    asc_loadalign(src0_reg, src0, offset);
    asc_loadalign(src1_reg, src1, offset);
    asc_add(dst_reg, src0_reg, src1_reg, mask);
    asc_storealign(dst, dst_reg, offset, mask);
}
```

#### Post Update搬出

调用`asc_storealign_postupdate`的立即数偏移搬出模式。目的地址以引用方式传入，接口在每次搬出后将其向后更新`one_repeat_size`个元素。

```cpp
vector_half src0_reg;
vector_half src1_reg;
vector_half dst_reg;
vector_bool mask;
for (uint16_t i = 0; i < repeat_times; ++i) {
    mask = asc_update_mask_b16(output_length);
    asc_loadalign_postupdate(src0_reg, src0, one_repeat_size);
    asc_loadalign_postupdate(src1_reg, src1, one_repeat_size);
    asc_add(dst_reg, src0_reg, src1_reg, mask);
    asc_storealign_postupdate(dst, dst_reg, one_repeat_size, mask);
}
```
