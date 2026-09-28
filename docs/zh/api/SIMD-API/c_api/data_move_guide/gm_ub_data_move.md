# GM与UB数据搬运

GM与UB数据搬运是Vector侧加载输入和回写结果的主要通路。GM到UB由PIPE_MTE2执行，UB到GM由PIPE_MTE3执行，相关接口仅在AIV生效。NPU架构版本3510支持连续、高维切分、非对齐以及GM到UB的NDDMA多维搬运。

## 总体说明

| 方向 | 搬运模式 | 接口 | 主要参数单位 |
| --- | --- | --- | --- |
| GM到UB | 连续 | [asc_copy_gm2ub](../vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_3510.md) | `size`：字节 |
| GM到UB | 高维切分 | [asc_copy_gm2ub](../vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_3510.md) | 块长、源/目的步长：字节 |
| GM到UB | 非对齐连续 | [asc_copy_gm2ub_align](../vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md) | `size`：字节 |
| GM到UB | 非对齐高维切分 | [asc_copy_gm2ub_align](../vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md) | 有效长度、源/目的步长：字节；左右Padding：元素 |
| GM到UB | 最多五维的Padding、Transpose、Broadcast或Slice | [asc_ndim_copy_gm2ub](../vector_datamove/asc_ndim_copy_gm2ub.md) | 循环大小、步长、Padding：元素 |
| UB到GM | 连续 | [asc_copy_ub2gm](../vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm_arch_3510.md) | `size`：字节 |
| UB到GM | 高维切分 | [asc_copy_ub2gm](../vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm_arch_3510.md) | 块长、源/目的步长：字节 |
| UB到GM | 非对齐连续 | [asc_copy_ub2gm_align](../vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md) | `size`：字节 |
| UB到GM | 非对齐高维切分 | [asc_copy_ub2gm_align](../vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md) | 有效长度、源/目的步长：字节 |

普通接口以字节表示有效长度，只需满足数据类型字节对齐，有效长度可以不是32字节的整数倍。非32字节有效长度仍会在UB一侧产生对齐补齐访问，容量计算需包含该范围。需要显式控制片上补齐布局、GM到UB左右Padding、L2 Cache策略或更大的高维切分参数范围时，选择对应的`*_align`高维切分原型；需要描述三维及以上排布或实现多维重排时，选择NDDMA。

## asc_copy_gm2ub（GM到UB连续数据搬运）

该模式从GM连续读取`size`字节，并按原排布写入UB，数据格式和内容保持不变。

```c
__aicore__ inline void asc_copy_gm2ub(__ubuf__ void* dst,
                                      __gm__ void* src,
                                      uint32_t size)
```

- `dst`需要32字节对齐，`src`需要1字节对齐。
- `size`单位为字节，取值范围为[1, $2^{21}-1$]，并需满足数据类型字节对齐要求。
- `size`不需要满足32字节对齐，不要为了凑齐32字节而扩大`size`、读取GM有效范围之外的数据。长度非32字节对齐时，硬件会在目的UB尾部写入dummy数据补齐至32字节边界，因此UB需要预留补齐后的空间。

## asc_copy_gm2ub（GM到UB高维切分数据搬运）

该模式搬运`burst_count`个数据块，每块包含`burst_len`字节。`src_stride`和`dst_stride`分别表示源、目的相邻数据块**起始地址之间**的距离，单位均为字节。

```c
__aicore__ inline void asc_copy_gm2ub(__ubuf__ void* dst,
                                      __gm__ void* src,
                                      uint16_t burst_count,
                                      uint16_t burst_len,
                                      uint16_t src_stride,
                                      uint16_t dst_stride)
```

- `dst`需要32字节对齐，`src`需要1字节对齐。
- `burst_len`需满足数据类型字节对齐；只搬运一个数据块时，两个步长可设为0。
- `dst_stride`等于`burst_len`时为Compact排布，所有有效数据块紧密排列，仅在整体末尾补齐至32字节边界。
- `dst_stride`不等于`burst_len`时为Normal排布，`dst_stride`需要32字节对齐，每个数据块分别补齐到32字节边界。
- 源GM跨度按最后一个数据块的起始偏移加`burst_len`计算；目的UB跨度还要计入Compact整体补齐或Normal逐块补齐。

## asc_copy_gm2ub_align（GM到UB非对齐数据搬运）

该接口在搬入UB时把非32字节对齐的数据补齐到32字节边界，支持8位、16位和32位数据类型。连续模式仅传入`size`；高维切分模式还可配置左右Padding、Padding值来源、L2 Cache策略和源/目的步长。

### 连续模式

```c
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ <dtype>* dst,
                                            __gm__ <dtype>* src,
                                            uint32_t size)
```

`size`不是32字节的整数倍时，硬件将目的数据补齐到32字节边界，补齐值固定取数据块的首元素。该连续原型不提供常量填充选择；如果需要常量填充，应使用高维切分原型，将`burst_count`设为1、`enable_constant_pad`设为`true`，并先通过[asc_set_copy_pad_val](../vector_datamove/asc_set_copy_pad_val.md)配置填充值。目的UB必须为补齐后的范围预留空间。

### 高维切分模式

高维切分模式支持以下两种目的排布：

- **Compact模式：** `dst_stride`等于`burst_len`，左右Padding均为0。多个有效数据块在UB中紧密排列，全部数据仅在整体末尾补齐到32字节边界。
- **Normal模式：** `dst_stride`不等于`burst_len`且为32字节的整数倍。每个数据块分别补齐，并可通过`left_padding_num`和`right_padding_num`在两侧填充元素。

左右Padding非0时，`enable_constant_pad`不生效，必须先调用`asc_set_copy_pad_val`配置填充值。左右Padding均为0时，`enable_constant_pad`为`false`表示用数据块首元素补齐，为`true`表示使用`asc_set_copy_pad_val`配置的常量，此时也必须先调用该配置接口。需要重复执行相同搬运模式时，可通过[asc_set_gm2ub_loop_size](../vector_datamove/asc_set_gm2ub_loop_size.md)、[asc_set_gm2ub_loop1_stride](../vector_datamove/asc_set_gm2ub_loop1_stride.md)和[asc_set_gm2ub_loop2_stride](../vector_datamove/asc_set_gm2ub_loop2_stride.md)配置两层循环。

## asc_ndim_copy_gm2ub（GM到UB多维数据搬运）

NDDMA通过五层循环描述GM到UB的数据访问，每一维均可配置元素个数、源步长、目的步长和左右Padding。未使用的高维仍需按接口要求填写合法循环大小。

```c
__aicore__ inline void asc_ndim_copy_gm2ub(__ubuf__ <dtype>* dst,
                                           __gm__ <dtype>* src,
                                           uint32_t loop0_size,
                                           uint32_t loop1_size,
                                           uint32_t loop2_size,
                                           uint32_t loop3_size,
                                           uint32_t loop4_size,
                                           uint8_t loop0_lp_count,
                                           uint8_t loop0_rp_count,
                                           bool padding_mode,
                                           asc_load_l2_cache_mode l2_cache_mode)
```

- 调用主接口前，必须通过[asc_set_ndim_loop_stride](../vector_datamove/asc_set_ndim_loop_stride.md)完成各维源、目的步长配置。
- 通过[asc_set_ndim_pad_count](../vector_datamove/asc_set_ndim_pad_count.md)设置1至4维的左右Padding数量，0维Padding数量由主接口传入。
- 常数Padding通过[asc_set_ndim_pad_value](../vector_datamove/asc_set_ndim_pad_value.md)设置；`padding_mode`为`false`时使用边界最近值填充。
- 各维目的步长必须避免循环之间发生写覆盖；一条指令可访问的总地址范围不能超过40位。
- 多核可能读取同一块GM且数据可能被其他核更新时，搬运前调用[asc_ndim_copy_dci](../vector_datamove/asc_ndim_copy_dci.md)刷新NDDMA DataCache。

NDDMA可以通过步长组合实现多维Padding、Transpose、Broadcast和Slice，但只支持GM到UB方向。

## asc_copy_ub2gm（UB到GM连续数据搬运）

该模式从UB连续读取`size`字节并写入GM，数据格式和内容保持不变。

```c
__aicore__ inline void asc_copy_ub2gm(__gm__ void* dst,
                                      __ubuf__ void* src,
                                      uint32_t size)
```

- `src`需要32字节对齐，`dst`需要1字节对齐。
- `size`单位为字节，取值范围为[1, $2^{21}-1$]，并需满足数据类型字节对齐要求。
- `size`不需要满足32字节对齐，接口只向GM写入`size`指定的有效字节，不会把UB中对齐范围内的其他数据写入GM。但硬件会从UB读取补齐到32字节边界的范围，源UB需要为该范围预留合法空间。

## asc_copy_ub2gm（UB到GM高维切分数据搬运）

该模式通过`burst_count`和`burst_len`描述多个有效数据块，`src_stride`和`dst_stride`分别表示源、目的相邻数据块起始地址之间的字节距离。

```c
__aicore__ inline void asc_copy_ub2gm(__gm__ void* dst,
                                      __ubuf__ void* src,
                                      uint16_t burst_count,
                                      uint16_t burst_len,
                                      uint16_t dst_stride,
                                      uint16_t src_stride)
```

`src_stride`等于`burst_len`时为Compact排布，硬件仅在所有有效数据的整体末尾补齐读取；`src_stride`不等于`burst_len`时为Normal排布，`src_stride`需要32字节对齐，硬件对每个数据块分别补齐读取。补齐部分不写入GM，但必须位于合法UB范围内。不建议把`src_stride`设为0实现Broadcast；需要把同一源数据块广播到多个GM位置时，应通过[asc_set_ub2gm_loop_size](../vector_datamove/asc_set_ub2gm_loop_size.md)、[asc_set_ub2gm_loop1_stride](../vector_datamove/asc_set_ub2gm_loop1_stride.md)和[asc_set_ub2gm_loop2_stride](../vector_datamove/asc_set_ub2gm_loop2_stride.md)按接口文档配置循环。

## asc_copy_ub2gm_align（UB到GM非对齐数据搬运）

该接口允许有效数据长度不是32字节的整数倍。硬件从UB读取时补充dummy数据至32字节对齐，写入GM时丢弃补充部分，因此不会把dummy数据写到GM，也不支持配置填充值。

高维切分模式支持两种源排布：

- **Compact模式：** `src_stride`等于`burst_len`，源数据块在UB中紧密排列，仅在所有有效数据末尾读取补齐数据。
- **Normal模式：** `src_stride`不等于`burst_len`且为32字节的整数倍，每个数据块分别读取至32字节边界。

即使补齐数据不会写入GM，源UB仍需为硬件读取的对齐范围预留合法空间。

## 同步与边界检查

- PIPE_MTE2写入UB后由PIPE_V读取时，建立PIPE_MTE2到PIPE_V的事件依赖；PIPE_V生成结果后由PIPE_MTE3读取时，建立PIPE_V到PIPE_MTE3的事件依赖。
- 多条PIPE_MTE2或PIPE_MTE3搬运的目的地址重叠时，使用[asc_sync_pipe](../sync/intra_core_sync/asc_sync_pipe.md)保证同一流水内的执行顺序。
- GM访问范围按有效源步长计算，UB访问范围还需包含对齐补齐、Padding和循环步长产生的最大偏移。
- 普通、非对齐和NDDMA接口的步长语义不同，不能复用同一组未换算参数。
