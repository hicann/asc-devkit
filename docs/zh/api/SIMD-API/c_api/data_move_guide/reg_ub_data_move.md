# Reg与UB数据搬运

<!-- npu="950" id1 -->

Reg数据搬入接口用于将Unified Buffer（UB）中的数据搬入矢量数据寄存器、掩码寄存器或非对齐寄存器，Reg数据搬出接口用于将寄存器中的数据写回UB。两类接口均仅在AIV上生效，且需在Vector Function（`__simd_vf__`标记的函数）内调用。

NPU架构版本3510的矢量数据寄存器长度（VL）为256字节，掩码寄存器长度为32字节。单个矢量数据寄存器可容纳的元素个数由数据位宽决定，例如b16为128个元素，b32为64个元素。接口的完整模式、地址对齐要求、原理图和示例分别参见[Reg数据搬入概述](../reg_compute/load/reg_load_overview.md)和[Reg数据搬出概述](../reg_compute/store/reg_store_overview.md)。本节按使用场景提炼接口选择方法。

## 接口选择

### UB搬入Reg

| 接口模式 | 代表接口 | 选择说明 |
| --- | --- | --- |
| 连续/立即数/地址寄存器偏移搬入 | [asc_loadalign](../reg_compute/load/asc_loadalign.md) | 搬入一个VL，可由调用方计算地址，也可使用立即数偏移或地址寄存器偏移。 |
| 立即数偏移/非连续对齐Post Update搬入 | [asc_loadalign_postupdate](../reg_compute/load/asc_loadalign_postupdate.md) | 搬入后自动更新源地址，适合连续迭代；不提供地址寄存器偏移原型。 |
| 非连续对齐搬入 | [asc_loadalign](../reg_compute/load/asc_loadalign.md)、[asc_loadalign_datablock_strided](../reg_compute/load/asc_loadalign_datablock_strided.md) | 按配置的DataBlock步长非连续搬入8个DataBlock。 |
| 广播搬入 | [asc_loadalign_brc_datablock](../reg_compute/load/asc_loadalign_brc_datablock.md)、[asc_loadalign_brc_elem](../reg_compute/load/asc_loadalign_brc_elem.md)、[asc_loadalign_brc_elem2datablock](../reg_compute/load/asc_loadalign_brc_elem2datablock.md) | 分别支持DataBlock广播、单元素广播以及将8个元素分别广播到8个DataBlock。 |
| 解交织/采样/解包搬入 | [asc_loadalign_deintlv](../reg_compute/load/asc_loadalign_deintlv.md)、[asc_loadalign_downsample](../reg_compute/load/asc_loadalign_downsample.md)、[asc_loadalign_upsample](../reg_compute/load/asc_loadalign_upsample.md)、[asc_loadalign_unpack](../reg_compute/load/asc_loadalign_unpack.md)、[asc_loadalign_unpack4](../reg_compute/load/asc_loadalign_unpack4.md) | 支持解交织、2倍下采样、2倍上采样、二分之一解包和四分之一解包。 |
| 连续非对齐搬入（易用形式） | [asc_load](../reg_compute/load/asc_load.md) | 源地址按`dtype`对齐即可，接口负责完成一个VL的连续搬入。 |
| 连续非对齐搬入（高性能形式） | [asc_loadunalign_pre](../reg_compute/load/asc_loadunalign_pre.md)、[asc_loadunalign](../reg_compute/load/asc_loadunalign.md)、[asc_loadunalign_postupdate](../reg_compute/load/asc_loadunalign_postupdate.md) | 使用`vector_load_unalign`保存跨32字节边界的数据，适合需要显式复用非对齐状态的场景。 |
| 掩码寄存器连续对齐搬入 | [asc_loadalign](../reg_compute/load/asc_loadalign.md)、[asc_loadalign_postupdate](../reg_compute/load/asc_loadalign_postupdate.md)、[asc_loadalign_mask](../reg_compute/load/asc_loadalign_mask.md) | 支持直接搬入、Post Update搬入或通过返回值搬入。 |
| 掩码寄存器采样搬入 | [asc_loadalign_downsample](../reg_compute/load/asc_loadalign_downsample.md)、[asc_loadalign_downsample_postupdate](../reg_compute/load/asc_loadalign_downsample_postupdate.md)、[asc_loadalign_mask_downsample](../reg_compute/load/asc_loadalign_mask_downsample.md)、[asc_loadalign_upsample](../reg_compute/load/asc_loadalign_upsample.md)、[asc_loadalign_upsample_postupdate](../reg_compute/load/asc_loadalign_upsample_postupdate.md)、[asc_loadalign_mask_upsample](../reg_compute/load/asc_loadalign_mask_upsample.md) | 支持2倍下采样和2倍上采样，可选择Post Update或返回值形式。 |

### Reg搬出UB

| 接口模式 | 代表接口 | 选择说明 |
| --- | --- | --- |
| 连续/立即数/地址寄存器偏移/非连续对齐搬出 | [asc_storealign](../reg_compute/store/asc_storealign.md) | 按`mask`连续写入一个VL，或按DataBlock步长写入。 |
| 立即数偏移/非连续对齐Post Update搬出 | [asc_storealign_postupdate](../reg_compute/store/asc_storealign_postupdate.md) | 搬出后自动更新目的地址，适合连续迭代；不提供地址寄存器偏移原型。 |
| 搬出首元素 | [asc_storealign_1st](../reg_compute/store/asc_storealign_1st.md)、[asc_storealign_1st_postupdate](../reg_compute/store/asc_storealign_1st_postupdate.md) | 仅将矢量数据寄存器的首个元素写入UB，可选择是否自动更新目的地址。 |
| 交织搬出 | [asc_storealign_intlv](../reg_compute/store/asc_storealign_intlv.md) | 将两个矢量数据寄存器按元素交织后写入UB。 |
| 压缩搬出 | [asc_storealign_pack](../reg_compute/store/asc_storealign_pack.md)、[asc_storealign_pack_quarter](../reg_compute/store/asc_storealign_pack_quarter.md) | 分别将有效元素的低半部分bit或32位元素的低8bit压缩后写入UB。 |
| 连续搬出（易用形式） | [asc_store](../reg_compute/store/asc_store.md) | 可搬出整个矢量数据寄存器或前`count`个元素；具体对齐场景按接口文档选择`asc_storealign`或非对齐接口。 |
| 非对齐搬出 | [asc_storeunalign](../reg_compute/store/asc_storeunalign.md)、[asc_storeunalign_postupdate](../reg_compute/store/asc_storeunalign_postupdate.md) | 使用`vector_store_unalign`暂存不能立即写出的尾块，并在连续搬出结束后调用配套后处理接口。 |
| 掩码寄存器连续对齐搬出 | [asc_storealign](../reg_compute/store/asc_storealign.md)、[asc_storealign_postupdate](../reg_compute/store/asc_storealign_postupdate.md) | 支持直接搬出和Post Update搬出。 |
| 掩码寄存器压缩搬出 | [asc_storealign_pack](../reg_compute/store/asc_storealign_pack.md)、[asc_storealign_pack_postupdate](../reg_compute/store/asc_storealign_pack_postupdate.md) | 每间隔1bit丢弃1bit，将保留bit压缩后写入UB。 |
| 掩码寄存器非对齐搬出 | [asc_storeunalign_postupdate](../reg_compute/store/asc_storeunalign_postupdate.md) | 提取掩码寄存器中的有效bit并打包写入UB，结束后调用配套后处理接口。 |

## 地址维护方式

对齐连续搬入和搬出可按具体接口选择以下地址维护方式，选择原则与两篇现有概述中的对比表一致；并非每个接口都同时支持所有方式：

- 调用方通过指针运算计算每次访问的地址，适合偏移规则简单且需要显式控制地址的场景。
- 将相对基地址的立即数偏移传入接口，适合需要保留基地址且偏移可由普通整数表示的场景。
- 通过[asc_update_addr_reg](../reg_compute/reg_addr_reg/asc_update_addr_reg.md)生成`addr_reg`，适合Hardware Loop内偏移随迭代变化的场景。
- 使用Post Update接口，在完成搬入或搬出后自动更新地址。

`asc_loadalign_postupdate`和`asc_storealign_postupdate`仅支持通过立即数参数指定地址更新量，以及各自的非连续对齐原型，不提供`addr_reg`参数。地址寄存器偏移应使用`asc_loadalign`或`asc_storealign`等非Post Update接口；非对齐接口的支持情况以具体原型为准。

偏移单位随接口和寄存器类型变化。矢量数据偏移通常以元素为单位，掩码地址偏移可能以字节为单位，非连续模式还可能以DataBlock为单位，具体以接口参数说明为准。

## 非对齐搬运

非对齐搬运用于有效起始地址满足`dtype`对齐、但不满足32字节对齐的连续数据访问场景。详细原理和配合关系参见[Reg数据搬入概述中的非对齐搬运特性](../reg_compute/load/reg_load_overview.md#非对齐搬运特性)。

### 非对齐搬入

`asc_load`是连续非对齐搬入的易用接口。需要显式复用非对齐状态时，先调用`asc_loadunalign_pre`，再调用`asc_loadunalign`或`asc_loadunalign_postupdate`。普通`asc_loadunalign`每次搬入前均需重新预处理；连续调用立即数偏移形式的`asc_loadunalign_postupdate`时，可复用接口更新后的非对齐寄存器，源地址被另行修改后必须重新预处理。

非对齐搬入会访问有效起始地址向低地址方向按32字节对齐后的范围，因此该范围也必须位于已分配的UB空间内。

### 非对齐搬出

非对齐主搬出接口将完整块写入UB，并把末尾不足对齐块的数据暂存在`vector_store_unalign`中。连续调用时必须复用同一个非对齐寄存器，结束后按下表调用配套后处理接口写出最后一个尾块。

| 主搬出方式 | 配套后处理接口 | 地址维护方式 |
| --- | --- | --- |
| [asc_storeunalign](../reg_compute/store/asc_storeunalign.md) | [asc_storeunalign_post](../reg_compute/store/asc_storeunalign_post.md) | 调用方更新目的地址。 |
| [asc_storeunalign_postupdate](../reg_compute/store/asc_storeunalign_postupdate.md)立即数偏移模式 | [asc_storeunalign_post_postupdate](../reg_compute/store/asc_storeunalign_post_postupdate.md) | 主接口自动更新目的地址。 |
| [asc_storeunalign_postupdate](../reg_compute/store/asc_storeunalign_postupdate.md)地址寄存器偏移模式 | [asc_storeunalign_post](../reg_compute/store/asc_storeunalign_post.md) | 主接口自动更新地址寄存器，主搬出和后处理接口复用该寄存器。 |

## 使用约束

- 各模式的实际访问地址必须满足对应接口的对齐要求，实际访问范围必须位于可用UB空间内且不能越界。
- 多条Reg指令访问重叠UB区域并存在读写冲突时，使用[asc_mem_bar](../reg_compute/reg_sync/asc_mem_bar.md)保证执行顺序。
- b64数据类型仅由部分接口支持，且地址寄存器偏移和掩码构造方式不同，具体参见[Reg数据搬入概述中的b64数据类型搬运](../reg_compute/load/reg_load_overview.md#b64数据类型的搬运)。
- 寄存器类型及声明方式参见[寄存器类型](../defs/type/reg_data_types.md)。

<!-- end id1 -->
