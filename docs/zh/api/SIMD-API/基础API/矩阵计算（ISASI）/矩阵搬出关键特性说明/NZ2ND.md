# NZ2ND<a name="ZH-CN_TOPIC_0000002568950981"></a>

## 特性说明

矩阵搬出时，NZ2ND功能即为实现NZ格式到ND格式的矩阵转换。

NZ2ND转化过程可以参考以下伪代码：

```cpp
for (i = 0; i < ndNum; i++) {
    // fractal_size = 16 * sizeof(L0C_element)
    src_temp_nd_addr = src + srcNdStride * fractal_size * i;
    dst_temp_nd_addr = dst + dstNdStride * sizeof(dst_element) * i;
    for (j = 0; j < mSize; j++) {
        src_temp_n_addr = src_temp_nd_addr + j * 16 * sizeof(L0C_element);
        dst_temp_n_addr = dst_temp_nd_addr + j * dstStride * sizeof(dst_element);
        for (k = 0; k < ceil(nSize / 16); k++) {
            src_block_addr = src_temp_n_addr + k * srcStride * 16 * sizeof(L0C_element);
            dst_block_addr = dst_temp_n_addr + k * 16 * sizeof(dst_element);
        }
    }
}
```

伪代码使用的参数具体说明如下：

- src表示源操作数搬运的起始地址。
- dst表示目的操作数搬运的起始地址。
- 源操作数每个NZ矩阵的起始地址为src\_temp\_nd\_addr。
- 目的操作数每个ND矩阵的起始地址为dst\_temp\_nd\_addr。
- 源操作数中每个NZ矩阵中每一行的起始地址为src\_temp\_n\_addr。
- 目的操作数每个ND矩阵中每一行的起始地址为dst\_temp\_n\_addr。
- 在每一行中，以16个数据为一个数据块，源操作数每块数据块的起始地址为src\_block\_addr，目的操作数中每一块数据块的起始地址为dst\_block\_addr。

以下参数为NZ2ND用户可配置参数，含义及取值范围参见[Fixpipe搬运参数](../矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)与[参数说明](../矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)：

- mSize与nSize分别表示矩阵m方向和n方向上的维度。
- srcStride表示NZ矩阵中不同Z分形列的间隔，单位为C0\_Size。
- dstStride表示目标ND矩阵中不同行的间隔，单位为元素。
- ndNum表示需要做NZ2ND转化的矩阵个数。
- srcNdStride表示不同NZ矩阵之间的间隔，单位为16\*C0\_Size（C0\_Size=16\*sizeof\(T\)，T为操作数的数据类型）字节。
- dstNdStride表示不同目标ND矩阵之间的间隔，单位为元素。

## 特性约束

- 当开启Channel Split/Channel Merge功能（NZ2NZ）时，此功能不可用。
- 目标操作数地址之间不能重叠。

## 使用示例

如下图所示，在L0C Buffer中有两个NZ格式的矩阵，每个矩阵N方向上维度为32，M方向上维度为48，经过NZ2ND转换，将数据格式变为右部分所示排布。

**图1** NZ2ND示意图<a name="zh-cn_topic_0000002547300777_fig862819576334"></a>  

![](../../../../figures/Fixpipe_NZ2ND.png)

对于该场景，可以通过配置intriParams结构体参数实现上图的效果：

- ndNum = 2，表示需要做NZ2ND转化的矩阵个数为2。
- nSize = 32，表示源NZ矩阵在N方向上的大小为32个元素。
- mSize = 48，表示源NZ矩阵在M方向上的大小为48个元素。
- srcStride = 64，表示源NZ矩阵中相邻Z排布的起始地址之间的间隔为64 \* C0\_Size\(C0\_Size = 16\*sizeof\(T\)，T为操作数的数据类型\)字节。
- dstStride = 64，表示目的ND矩阵每一行中的元素个数为64。
- srcNdStride = 256，表示不同NZ矩阵起始地址之间的间隔为256 \* C0\_Size字节。
- dstNdStride = 4096，表示目的相邻ND矩阵起始地址之间的偏移为4096个元素。
