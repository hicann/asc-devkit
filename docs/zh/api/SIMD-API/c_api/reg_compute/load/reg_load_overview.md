# Reg数据搬入概述

Reg数据搬入接口用于将Unified Buffer（UB）中的数据搬入矢量数据寄存器、掩码寄存器或非对齐寄存器。接口均仅在AIV上生效，且需在Vector Function（`__simd_vf__`标记的函数）内调用。

## 接口分类与选择

根据数据访问方式、搬入后的数据排布以及是否需要自动更新地址，选择相应接口。

### 矢量数据寄存器搬入

#### 对齐搬入

**表1**  矢量数据寄存器对齐搬入接口

| 接口名称 | 模式 | 功能简述 | 地址对齐要求 |
| --- | --- | --- | --- |
| [asc_loadalign](asc_loadalign.md) | 连续对齐搬入 | 连续搬入一个VL长度的数据，由用户更新源地址。 | 32字节 |
| [asc_loadalign](asc_loadalign.md) | 立即数偏移搬入模式 | 从源起始地址偏移指定距离后搬入一个VL长度的数据。 | 32字节 |
| [asc_loadalign](asc_loadalign.md) | 地址寄存器偏移搬入模式 | 按地址寄存器指定的偏移搬入一个VL长度的数据。 | 32字节 |
| [asc_loadalign](asc_loadalign.md) | 非连续对齐搬入模式 | 按配置的DataBlock步长非连续搬入8个DataBlock。 | 32字节 |
| [asc_loadalign_postupdate](asc_loadalign_postupdate.md) | 立即数偏移搬入模式 | 搬入一个VL长度的数据，并按立即数偏移更新源地址。 | 32字节 |
| [asc_loadalign_postupdate](asc_loadalign_postupdate.md) | 非连续对齐搬入模式 | 非连续搬入8个DataBlock，并自动更新源地址。 | 32字节 |
| [asc_loadalign_datablock_strided](asc_loadalign_datablock_strided.md) | 非连续对齐搬入模式 | 按配置的DataBlock步长非连续搬入8个DataBlock，通过返回值返回结果。 | 32字节 |
| [asc_loadalign_brc_datablock](asc_loadalign_brc_datablock.md) | 对齐搬入模式 | 将一个DataBlock广播到整个矢量数据寄存器。 | 32字节 |
| [asc_loadalign_brc_datablock](asc_loadalign_brc_datablock.md) | 立即数偏移搬入模式 | 从立即数偏移位置读取一个DataBlock并广播。 | 32字节 |
| [asc_loadalign_brc_datablock](asc_loadalign_brc_datablock.md) | 地址寄存器偏移搬入模式 | 从地址寄存器偏移位置读取一个DataBlock并广播。 | 32字节 |
| [asc_loadalign_brc_datablock_postupdate](asc_loadalign_brc_datablock_postupdate.md) | Post Update搬入模式 | 将一个DataBlock广播到整个矢量数据寄存器，并自动更新源地址。 | 32字节 |
| [asc_loadalign_brc_elem](asc_loadalign_brc_elem.md) | 对齐搬入模式 | 将一个元素广播到整个矢量数据寄存器。 | 按dtype对齐 |
| [asc_loadalign_brc_elem](asc_loadalign_brc_elem.md) | 立即数偏移搬入模式 | 从立即数偏移位置读取一个元素并广播。 | 按dtype对齐 |
| [asc_loadalign_brc_elem](asc_loadalign_brc_elem.md) | 地址寄存器偏移搬入模式 | 从地址寄存器偏移位置读取一个元素并广播。 | 按dtype对齐 |
| [asc_loadalign_brc_elem_postupdate](asc_loadalign_brc_elem_postupdate.md) | Post Update搬入模式 | 将一个元素广播到整个矢量数据寄存器，并自动更新源地址。 | 按dtype对齐 |
| [asc_loadalign_brc_elem2datablock](asc_loadalign_brc_elem2datablock.md) | 对齐搬入模式 | 连续读取8个元素，并将每个元素广播到一个DataBlock。 | b16为16字节，b32为32字节 |
| [asc_loadalign_brc_elem2datablock](asc_loadalign_brc_elem2datablock.md) | 立即数偏移搬入模式 | 从立即数偏移位置读取8个元素，并分别广播到一个DataBlock。 | b16为16字节，b32为32字节 |
| [asc_loadalign_brc_elem2datablock](asc_loadalign_brc_elem2datablock.md) | 地址寄存器偏移搬入模式 | 从地址寄存器偏移位置读取8个元素，并分别广播到一个DataBlock。 | b16为16字节，b32为32字节 |
| [asc_loadalign_brc_elem2datablock_postupdate](asc_loadalign_brc_elem2datablock_postupdate.md) | Post Update搬入模式 | 连续读取并广播8个元素，并自动更新源地址。 | b16为16字节，b32为32字节 |
| [asc_loadalign_deintlv](asc_loadalign_deintlv.md) | 对齐搬入模式 | 读取2×VL长度的数据，解交织后写入两个矢量数据寄存器。 | 32字节 |
| [asc_loadalign_deintlv](asc_loadalign_deintlv.md) | 立即数偏移搬入模式 | 从立即数偏移位置读取数据并解交织。 | 32字节 |
| [asc_loadalign_deintlv](asc_loadalign_deintlv.md) | 地址寄存器偏移搬入模式 | 从地址寄存器偏移位置读取数据并解交织。 | 32字节 |
| [asc_loadalign_deintlv_postupdate](asc_loadalign_deintlv_postupdate.md) | Post Update搬入模式 | 读取数据并解交织到两个矢量数据寄存器，同时自动更新源地址。 | 32字节 |
| [asc_loadalign_downsample](asc_loadalign_downsample.md) | 对齐搬入模式 | 读取2×VL长度的数据，保留偶数下标元素。 | 32字节 |
| [asc_loadalign_downsample](asc_loadalign_downsample.md) | 立即数偏移搬入模式 | 从立即数偏移位置读取数据并进行2倍下采样。 | 32字节 |
| [asc_loadalign_downsample](asc_loadalign_downsample.md) | 地址寄存器偏移搬入模式 | 从地址寄存器偏移位置读取数据并进行2倍下采样。 | 32字节 |
| [asc_loadalign_downsample_postupdate](asc_loadalign_downsample_postupdate.md) | Post Update搬入模式 | 读取数据并进行2倍下采样，同时自动更新源地址。 | 32字节 |
| [asc_loadalign_upsample](asc_loadalign_upsample.md) | 对齐搬入模式 | 读取VL/2长度的数据，将每个元素重复两次。 | 32字节 |
| [asc_loadalign_upsample](asc_loadalign_upsample.md) | 立即数偏移搬入模式 | 从立即数偏移位置读取数据并进行2倍上采样。 | 32字节 |
| [asc_loadalign_upsample](asc_loadalign_upsample.md) | 地址寄存器偏移搬入模式 | 从地址寄存器偏移位置读取数据并进行2倍上采样。 | 32字节 |
| [asc_loadalign_upsample_postupdate](asc_loadalign_upsample_postupdate.md) | Post Update搬入模式 | 读取数据并进行2倍上采样，同时自动更新源地址。 | 32字节 |
| [asc_loadalign_unpack](asc_loadalign_unpack.md) | 对齐搬入模式 | 读取VL/2长度的数据，在每个源元素后补1个0。 | 32字节 |
| [asc_loadalign_unpack](asc_loadalign_unpack.md) | 立即数偏移搬入模式 | 从立即数偏移位置读取数据，在每个源元素后补1个0。 | 32字节 |
| [asc_loadalign_unpack](asc_loadalign_unpack.md) | 地址寄存器偏移搬入模式 | 从地址寄存器偏移位置读取数据，在每个源元素后补1个0。 | 32字节 |
| [asc_loadalign_unpack_postupdate](asc_loadalign_unpack_postupdate.md) | Post Update搬入模式 | 读取VL/2长度的数据，在每个源元素后补1个0，并自动更新源地址。 | 32字节 |
| [asc_loadalign_unpack4](asc_loadalign_unpack4.md) | 对齐搬入模式 | 读取VL/4长度的数据，在每个源元素后补3个0。 | 32字节 |
| [asc_loadalign_unpack4](asc_loadalign_unpack4.md) | 立即数偏移搬入模式 | 从立即数偏移位置读取数据，在每个源元素后补3个0。 | 32字节 |
| [asc_loadalign_unpack4](asc_loadalign_unpack4.md) | 地址寄存器偏移搬入模式 | 从地址寄存器偏移位置读取数据，在每个源元素后补3个0。 | 32字节 |
| [asc_loadalign_unpack4_postupdate](asc_loadalign_unpack4_postupdate.md) | Post Update搬入模式 | 读取VL/4长度的数据，在每个源元素后补3个0，并自动更新源地址。 | 32字节 |

#### 非对齐搬入

**表2**  矢量数据寄存器非对齐搬入接口

| 接口名称 | 模式 | 功能简述 | 地址对齐要求 |
| --- | --- | --- | --- |
| [asc_load](asc_load.md) | 连续非对齐搬入模式 | 搬入一个VL长度的数据，由用户更新源地址。该接口为易用性接口。 | 按dtype对齐 |
| [asc_loadunalign_pre](asc_loadunalign_pre.md) | 源地址预处理模式 | 根据源地址初始化非对齐寄存器。 | 按dtype对齐 |
| [asc_loadunalign_pre](asc_loadunalign_pre.md) | 地址寄存器偏移预处理模式 | 根据源地址和地址寄存器偏移初始化非对齐寄存器。 | 按dtype对齐 |
| [asc_loadunalign](asc_loadunalign.md) | 连续非对齐搬入模式 | 使用预处理结果搬入一个VL长度的数据，由用户更新源地址。 | 按dtype对齐 |
| [asc_loadunalign_postupdate](asc_loadunalign_postupdate.md) | 连续非对齐搬入模式 | 使用预处理结果搬入指定数量的元素，并自动更新源地址和非对齐寄存器。 | 按dtype对齐 |
| [asc_loadunalign_postupdate](asc_loadunalign_postupdate.md) | 地址寄存器偏移搬入模式 | 从地址寄存器偏移位置搬入数据，并自动更新偏移和非对齐寄存器。 | 按dtype对齐 |

`asc_load`为易用性接口。对性能有要求的连续非对齐搬入场景，可使用`asc_loadunalign`或`asc_loadunalign_postupdate`。

### 掩码寄存器搬入

**表3**  掩码寄存器搬入接口

| 接口名称 | 模式 | 功能简述 | 地址对齐要求 |
| --- | --- | --- | --- |
| [asc_loadalign](asc_loadalign.md) | 连续对齐搬入 | 连续搬入VL/8长度的数据，由用户更新源地址。 | 32字节 |
| [asc_loadalign](asc_loadalign.md) | 立即数偏移搬入模式 | 从源起始地址偏移指定距离后搬入VL/8长度的数据。 | 32字节 |
| [asc_loadalign](asc_loadalign.md) | 地址寄存器偏移搬入模式 | 按地址寄存器指定的偏移搬入VL/8长度的数据。 | 32字节 |
| [asc_loadalign_postupdate](asc_loadalign_postupdate.md) | 立即数偏移搬入模式 | 搬入VL/8长度的数据，并按立即数偏移更新源地址。 | 32字节 |
| [asc_loadalign_mask](asc_loadalign_mask.md) | 连续对齐搬入 | 连续搬入VL/8长度的数据，通过返回值返回结果。 | 32字节 |
| [asc_loadalign_downsample](asc_loadalign_downsample.md) | 对齐搬入模式 | 读取VL/4长度的数据，保留偶数下标bit。 | 32字节 |
| [asc_loadalign_downsample](asc_loadalign_downsample.md) | 立即数偏移搬入模式 | 从立即数偏移位置读取数据并进行2倍下采样。 | 32字节 |
| [asc_loadalign_downsample](asc_loadalign_downsample.md) | 地址寄存器偏移搬入模式 | 从地址寄存器偏移位置读取数据并进行2倍下采样。 | 32字节 |
| [asc_loadalign_downsample_postupdate](asc_loadalign_downsample_postupdate.md) | Post Update搬入模式 | 读取数据并进行2倍下采样，同时自动更新源地址。 | 32字节 |
| [asc_loadalign_mask_downsample](asc_loadalign_mask_downsample.md) | 对齐搬入模式 | 读取VL/4长度的数据，保留偶数下标bit，通过返回值返回结果。 | 32字节 |
| [asc_loadalign_upsample](asc_loadalign_upsample.md) | 对齐搬入模式 | 读取VL/16长度的数据，将每个bit重复两次。 | 16字节 |
| [asc_loadalign_upsample](asc_loadalign_upsample.md) | 立即数偏移搬入模式 | 从立即数偏移位置读取数据并进行2倍上采样。 | 16字节 |
| [asc_loadalign_upsample](asc_loadalign_upsample.md) | 地址寄存器偏移搬入模式 | 从地址寄存器偏移位置读取数据并进行2倍上采样。 | 16字节 |
| [asc_loadalign_upsample_postupdate](asc_loadalign_upsample_postupdate.md) | Post Update搬入模式 | 读取数据并进行2倍上采样，同时自动更新源地址。 | 16字节 |
| [asc_loadalign_mask_upsample](asc_loadalign_mask_upsample.md) | 对齐搬入模式 | 读取VL/16长度的数据，将每个bit重复两次，通过返回值返回结果。 | 16字节 |

## 通用约束

- 本接口仅在AIV上生效，非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用。
- 各功能模式下的实际读取地址必须满足[接口分类与选择](#接口分类与选择)中的地址对齐要求，且实际读取范围必须在UB地址空间内且不越界，否则会报错。
- UB容量上限为256KB，用户可用容量随编译选项与编程场景变化。UB地址偏移后不可超过实际可用容量，否则会报错。详细说明请参见[存储单元说明](../../general_description_and_constraints.md#存储单元说明)。
- 如果本指令与其他指令存在UB地址重叠，需要插入同步指令[asc_mem_bar](../reg_sync/asc_mem_bar.md)，保证多个指令串行化，防止出现异常数据。

## 关键特性说明

### 对齐连续搬入方式对比

对齐连续搬入提供四种源地址维护方式。以下示例均将长度为1024、数据类型为`half`的源数据连续搬入矢量数据寄存器，再写入UB。单次迭代处理128个元素，共迭代8次，最终结果均为`dst[i] = src[i]`。

**表4**  对齐连续搬入方式对比

| 场景 | 搬入方式 | 源地址维护方式 | 适用场景 |
| --- | --- | --- | --- |
| 场景1 | 连续对齐搬入 | 调用方通过指针运算计算每次搬入的源地址。 | 源地址偏移规则简单，且需要由用户显式控制地址。 |
| 场景2 | 立即数偏移搬入 | 将相对源基地址的元素偏移作为`int32_t`参数传入，接口不修改源地址指针。 | 需要保留源基地址，并通过普通整数指定偏移。 |
| 场景3 | 地址寄存器偏移搬入 | 通过[asc_update_addr_reg](../reg_addr_reg/asc_update_addr_reg.md)生成`addr_reg`，由硬件在迭代中更新偏移。 | Hardware Loop内偏移随迭代变化。 |
| 场景4 | Post Update搬入 | `asc_loadalign_postupdate`在每次搬入后自动更新源地址指针。 | 连续搬入且无需用户手动维护源地址。 |

#### 连续对齐搬入

调用`asc_loadalign`的连续对齐搬入形式。源地址通过`src + i * one_repeat_size`计算，`one_repeat_size`为128个`half`元素。

```cpp
__simd_vf__ inline void load_align_continuous_vf(__ubuf__ half* dst, __ubuf__ half* src,
                                                 uint16_t one_repeat_size, uint16_t repeat_times)
{
    vector_half data_reg;
    vector_bool mask = asc_create_mask_b16(PAT_ALL);
    for (uint16_t i = 0; i < repeat_times; ++i) {
        asc_loadalign(data_reg, src + i * one_repeat_size);
        asc_storealign(dst + i * one_repeat_size, data_reg, mask);
    }
}
```

#### 立即数偏移搬入

调用`asc_loadalign`的立即数偏移搬入模式。`offset`相对`src`基地址，单位为元素；本示例通过`i * one_repeat_size`计算每次搬入的偏移。

```cpp
__simd_vf__ inline void load_align_immediate_offset_vf(__ubuf__ half* dst, __ubuf__ half* src,
                                                       uint16_t one_repeat_size, uint16_t repeat_times)
{
    vector_half data_reg;
    vector_bool mask = asc_create_mask_b16(PAT_ALL);
    for (uint16_t i = 0; i < repeat_times; ++i) {
        asc_loadalign(data_reg, src, i * one_repeat_size);
        asc_storealign(dst + i * one_repeat_size, data_reg, mask);
    }
}
```

#### 地址寄存器偏移搬入

调用`asc_loadalign`的地址寄存器偏移搬入模式。`addr_reg`须在最内层循环中赋值并使用，b16模式下偏移单位为元素。

```cpp
__simd_vf__ inline void load_align_addr_reg_offset_vf(__ubuf__ half* dst, __ubuf__ half* src,
                                                      uint16_t one_repeat_size, uint16_t repeat_times)
{
    vector_half data_reg;
    vector_bool mask = asc_create_mask_b16(PAT_ALL);
    addr_reg offset;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        offset = asc_update_addr_reg_b16(one_repeat_size);
        asc_loadalign(data_reg, src, offset);
        asc_storealign(dst + i * one_repeat_size, data_reg, mask);
    }
}
```

#### Post Update搬入

调用`asc_loadalign_postupdate`的立即数偏移搬入模式。源地址以引用方式传入，接口在每次搬入后将其向后更新`one_repeat_size`个元素。

```cpp
__simd_vf__ inline void load_align_post_update_vf(__ubuf__ half* dst, __ubuf__ half* src,
                                                  uint16_t one_repeat_size, uint16_t repeat_times)
{
    vector_half data_reg;
    vector_bool mask = asc_create_mask_b16(PAT_ALL);
    for (uint16_t i = 0; i < repeat_times; ++i) {
        asc_loadalign_postupdate(data_reg, src, one_repeat_size);
        asc_storealign(dst + i * one_repeat_size, data_reg, mask);
    }
}
```

### 非对齐搬运特性

非对齐搬运用于有效起始地址满足dtype对齐、但不满足32字节对齐的连续数据搬入或搬出场景。搬入和搬出均通过[非对齐寄存器](../../defs/type/data_type_definition.md#非对齐寄存器)保存跨32字节边界的数据，以保证连续数据的完整性。

#### 非对齐搬入原理

非对齐搬入通过非对齐寄存器缓存起始地址所在32字节块的前置数据，再与后续读取的数据拼接，得到从有效起始地址开始的VL长度数据。

**图1**  非对齐搬入示意图<a id="fig-loadunalign"></a>

![非对齐搬入示意图](../../figures/capi_loadunalign.png "非对齐搬入示意图")

设有效起始字节地址为`src_start_addr`，将其向低地址方向按32字节对齐，得到`aligned_src_start_addr`。处理过程如下：

1. 调用[asc_loadunalign_pre](asc_loadunalign_pre.md)，将`[aligned_src_start_addr, aligned_src_start_addr + 32)`范围内的数据缓存至非对齐寄存器。
2. 调用[asc_loadunalign](asc_loadunalign.md)或[asc_loadunalign_postupdate](asc_loadunalign_postupdate.md)，将前置缓存与后续读取数据拼接，得到`[src_start_addr, src_start_addr + VL)`范围的数据并写入矢量数据寄存器。`asc_loadunalign_postupdate`还会更新非对齐寄存器，以缓存下一次连续搬入所需的前置数据。

实际访问会覆盖向低地址方向对齐后的32字节范围，因此除有效起始地址满足dtype对齐外，对齐后的读取范围也必须位于UB地址空间内。

**表5**  非对齐搬入接口的配合关系

| 主搬入方式 | 配套接口 | 说明 |
| --- | --- | --- |
| [asc_loadunalign](asc_loadunalign.md) | [asc_loadunalign_pre](asc_loadunalign_pre.md) | 用户手动更新源地址。每次搬入前均需调用预处理接口，且两个接口必须使用相同的源地址。 |
| [asc_loadunalign_postupdate](asc_loadunalign_postupdate.md)的立即数偏移模式 | [asc_loadunalign_pre](asc_loadunalign_pre.md) | 接口自动更新源地址。首次搬入前调用预处理接口；后续连续搬入可复用更新后的非对齐寄存器，源地址被另行修改后必须重新预处理。 |
| [asc_loadunalign_postupdate](asc_loadunalign_postupdate.md)的地址寄存器偏移模式 | [asc_update_addr_reg](../reg_addr_reg/asc_update_addr_reg.md)、[asc_loadunalign_pre](asc_loadunalign_pre.md) | 接口自动更新地址寄存器偏移。每次循环迭代先生成起始偏移，再使用相同的源地址和偏移进行预处理；迭代内连续搬入可复用更新后的非对齐寄存器，源地址或偏移被另行修改后必须重新预处理。 |

#### 非对齐搬出原理

非对齐搬出将矢量数据寄存器中的连续数据写入非32字节对齐的UB地址。每次主搬出会将可以直接写入的主块写入UB，并将末尾不能构成32字节对齐块的数据保存在非对齐寄存器。后续连续搬出时，接口将该尾块与本次主块的起始数据拼接后写入UB，并更新本次尾块。

首次搬出前，非对齐寄存器无需初始化。连续调用时，必须复用同一个非对齐寄存器；若上一次尾块与本次主块不连续，则需要在下一次主搬出前调用后处理接口完成前一次尾块的写入。

**图2**  非对齐搬出示意图（首次搬出）<a id="fig-storeunalign-empty"></a>

![非对齐搬出示意图（首次搬出）](../../figures/capi_storeunalign_1.png "非对齐搬出示意图（首次搬出）")

首次搬出时，非对齐寄存器中没有前序尾块数据。主搬出接口将矢量数据寄存器中可直接写入的数据写入UB，并将末尾数据保存至非对齐寄存器。调用后处理接口后，尾块数据被写入UB。

**图3**  非对齐搬出示意图（连续搬出）<a id="fig-storeunalign-cached"></a>

![非对齐搬出示意图（连续搬出）](../../figures/capi_storeunalign_2.png "非对齐搬出示意图（连续搬出）")

后续连续搬出时，主搬出接口将非对齐寄存器中的前序尾块与本次矢量数据寄存器的主块数据拼接后写入UB，再将本次尾块更新至非对齐寄存器。连续搬出结束后，调用后处理接口写入最后一次缓存的尾块。

**表6**  非对齐搬出接口的配合关系

| 主搬出方式 | 配套接口 | 说明 |
| --- | --- | --- |
| [asc_storeunalign](../store/asc_storeunalign.md) | [asc_storeunalign_post](../store/asc_storeunalign_post.md) | 用户手动更新目的地址。 |
| [asc_storeunalign_postupdate](../store/asc_storeunalign_postupdate.md)的立即数偏移模式 | [asc_storeunalign_post_postupdate](../store/asc_storeunalign_post_postupdate.md) | 接口自动更新目的地址。 |
| [asc_storeunalign_postupdate](../store/asc_storeunalign_postupdate.md)的地址寄存器偏移模式 | [asc_storeunalign_post](../store/asc_storeunalign_post.md) | 主搬出和后处理接口需复用同一个非对齐寄存器及地址寄存器。 |

#### 非对齐搬入搬出示例

下图以`uint32_t`数据为例，展示连续非对齐搬入后再连续非对齐搬出的过程。搬入前置初始化和搬出后处理均位于循环外：连续搬入时，`asc_loadunalign_postupdate`更新的非对齐寄存器可供下一次搬入使用；连续搬出时，`asc_storeunalign_postupdate`将尾块保存在非对齐寄存器，最后一次循环结束后再统一写回。

**图4**  连续非对齐搬入搬出示例<a id="fig-loadstoreunalign"></a>

![连续非对齐搬入搬出示例](../../figures/capi_loadunalign_storeunalign.png "连续非对齐搬入搬出示例")

```c
__simd_vf__ inline void load_store_unalign_vf(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src,
                                              uint32_t count, uint16_t repeat_times)
{
    vector_load_unalign ureg0;
    vector_store_unalign ureg1;
    vector_uint32_t data_reg;
    // 非对齐搬入初始化，只需在循环开始前调用一次。
    asc_loadunalign_pre(ureg0, src);
    for (uint16_t i = 0; i < repeat_times; ++i) {
        asc_loadunalign_postupdate(data_reg, ureg0, src, count);
        asc_storeunalign_postupdate(dst, ureg1, data_reg, count);
    }
    // 非对齐搬出后处理，只需在循环结束后调用一次。
    asc_storeunalign_post_postupdate(dst, ureg1, 0);
}
```

搬运步骤如下：

1. 图中①：循环开始前，`asc_loadunalign_pre`将源地址所在32字节块的数据缓存到`ureg0`，为首次非对齐搬入准备前置数据。
2. 图中②：首次循环调用`asc_loadunalign_postupdate`，将`ureg0`中的前置数据与后续读取数据拼接为一个VL长度的`data_reg`，并更新`ureg0`和`src`。
3. 图中③：首次调用`asc_storeunalign_postupdate`，将`data_reg`中可直接写入的主块数据写入UB，将尾块缓存到`ureg1`，并更新`dst`。
4. 图中④：后续循环再次调用`asc_loadunalign_postupdate`，复用更新后的`ureg0`和`src`完成下一次搬入，并再次更新二者。
5. 图中⑤：后续调用`asc_storeunalign_postupdate`，将`ureg1`中的前序尾块与本次`data_reg`中的主块数据拼接后写入UB，再将本次尾块更新到`ureg1`并更新`dst`。
6. 图中⑥：循环结束后，`asc_storeunalign_post_postupdate`将`ureg1`中最后缓存的尾块写入UB，完成连续非对齐搬出。

示例中`count`为每次搬运的元素个数。`src`和`dst`分别由搬入、搬出接口自动更新，循环内需要复用各自的非对齐寄存器。`count`不超过单个矢量数据寄存器可容纳的元素个数，且搬入、搬出地址范围必须位于实际可用UB空间内。

### b64数据类型的搬运

以下接口支持b64数据类型。

**表7**  支持b64数据类型的接口

| 支持的数据类型 | 接口 |
| --- | --- |
| `int64_t`、`uint64_t` | [asc_loadalign](asc_loadalign.md)<br>[asc_loadalign_postupdate](asc_loadalign_postupdate.md) |
| `int64_t` | [asc_load](asc_load.md)<br>[asc_loadunalign_pre](asc_loadunalign_pre.md)<br>[asc_loadunalign](asc_loadunalign.md)<br>[asc_loadunalign_postupdate](asc_loadunalign_postupdate.md) |

当`asc_loadalign`、`asc_loadunalign_pre`或`asc_loadunalign_postupdate`通过地址寄存器访问b64数据时，地址寄存器更新接口不支持b64模式，应使用[asc_update_addr_reg_b32](../reg_addr_reg/asc_update_addr_reg.md)生成地址寄存器。一个b64元素占用两个b32寻址单元，因此各维偏移量以及`asc_loadunalign_postupdate`的`inc`应设置为期望b64元素偏移量的2倍。

`asc_loadalign`和`asc_loadalign_postupdate`的非连续对齐搬入模式中，处理`int64_t`数据时，掩码创建接口不支持b64模式。应先使用[asc_create_mask_b32](../reg_mask/asc_create_mask.md)创建b32掩码，再使用[asc_unpack_lower](../reg_permute_sel/asc_unpack.md)将其展开为b64数据对应的掩码。展开后的掩码以连续8个bit为一组，仅每组最低位的bit有效，用于控制一个b64元素。

以下示例以`asc_loadalign`为例，展示b64掩码的构造方式以及地址寄存器偏移量的换算方法。

```cpp
vector_int64_t src_reg;
vector_bool mask_b32 = asc_create_mask_b32(PAT_ALL);
vector_bool mask_b64;
asc_unpack_lower(mask_b64, mask_b32);
addr_reg load_offset;
addr_reg store_offset;
for (uint16_t i = 0; i < repeatTimes; ++i) {
    load_offset = asc_update_addr_reg_b32(elements_vl * 2);
    // 搬出到UB后数据类型为B32，偏移不需要*2
    store_offset = asc_update_addr_reg_b32(elements_vl);
    asc_loadalign(src_reg, input, load_offset);
    asc_storealign_pack(output, src_reg, store_offset, mask_b64);
}
```
