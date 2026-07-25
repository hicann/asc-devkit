# NZ2DN<a name="ZH-CN_TOPIC_0000002550560514"></a>

## 特性说明

矩阵计算的搬出过程中NZ2DN的核心功能体现为，将NZ格式矩阵转化为DN格式矩阵。

NZ2DN转化过程可以参考以下伪代码：

```cpp
for (int h = 0; h < dnNum; h++) {
    src_tmp3_addr = src + h ∗ 16 ∗ sizeof(src_element) ∗ srcNzMatrixStride;
    dst_tmp3_addr = dst + h ∗ sizeof(dst_element) ∗ dstDnMatrixStride;
    for (int i = 0; i < ceil(nSize / 16); i++) {
        src_tmp2_addr = src_tmp3_addr + i ∗ sizeof(src_element) ∗ 16 ∗ srcStride;
        dst_tmp2_addr = dst_tmp3_addr + i ∗ sizeof(dst_element) ∗ 16 ∗ dstStride;
        for (int j = 0; j < 16; j++) {
            if (i ∗ 16 + j < nSize) {
                src_block_addr = src_tmp2_addr + j ∗ sizeof(src_element);
                dst_block_addr = dst_tmp2_addr + j ∗ sizeof(dst_element) ∗ dstStride;
                for (int k = 0; k < mSize; k++) {
                    src_ele_addr = src_block_addr + k ∗ sizeof(src_element) ∗ 16 ∗ srcNzC0Stride;
                    dst_ele_addr = dst_block_addr + k ∗ sizeof(dst_element);
                }
            }
        }
    }
}
```

伪代码使用的参数具体说明如下：

- src表示源操作数搬运的起始地址。
- dst表示目的操作数搬运的起始地址。
- 源操作数每个NZ矩阵的起始地址为src\_tmp3\_addr。
- 目的操作数每个DN矩阵的起始地址为dst\_tmp3\_addr。
- 源操作数NZ矩阵以每16列为一组数据，每一组数据的起始地址为src\_tmp2\_addr。
- 目的操作数DN矩阵以每16行为一组数据，每一组数据的起始地址为dst\_tmp2\_addr。
- 在上述源自NZ矩阵的每一组数据中，再以每列元素为一组数据，每一组数据的起始地址为src\_block\_addr。
- 在上述源自DN矩阵的每一组数据中，再以每行元素为一组数据，每一组数据的起始地址为dst\_block\_addr。
- src\_ele\_addr为NZ矩阵一列的每个元素的地址，dst\_ele\_addr为DN矩阵一行的每个元素的地址，二者为一一对应关系，对应完成NZ2DN格式转换。

以下参数为NZ2DN用户可配置参数，含义取值范围参见[Fixpipe搬运参数（FixpipeParamsArch3510）](../矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md#参数说明)：

- dnNum表示需要做NZ2DN转化的矩阵个数。
- mSize与nSize分别表示矩阵m方向和n方向上的维度。
- srcStride表示源NZ矩阵中相邻Z排布的起始地址之间的间隔，单位为C0\_Size。
- dstStride表示目的DN矩阵每一行中的元素个数，单位为element。
- srcNzMatrixStride表示不同源NZ矩阵的起始地址的间隔，单位为C0\_Size。
- dstDnMatrixStride表示相邻目的DN矩阵起始地址间的间隔，单位为element。
- srcNzC0Stride表示源矩阵NZ分形相邻行的地址偏移为，单位为C0\_Size。

## 特性约束

- 当开启Channel Split/Channel Merge功能（NZ2NZ）时，此功能不可用。
- 当L0C Buffer搬运到UB开启双目标模式时，此功能不可用。
- 目标操作数地址之间不能重叠。

## 使用示例

如下图所示，在L0C Buffer中有两个NZ格式的矩阵，每个矩阵N方向上维度为32，M方向上维度为48，经过NZ2DN转换，将数据格式变为右部分所示排布。

**图1** NZ2DN示意图<a name="fig3981319172211"></a>  

![](../../../../figures/fixpipe_nz2dn.png)

对于该场景，可以通过配置intriParams结构体参数实现上图的效果：

- dnNum = 2，表示需要做NZ2DN转化的矩阵个数为2。
- nSize = 32，表示源NZ矩阵在N方向上的大小为32个元素。
- mSize = 48，表示源NZ矩阵在M方向上的大小为48个元素。
- srcStride = 80，表示源NZ矩阵中相邻Z排布的起始地址之间的间隔为80 \* C0\_Size\(C0\_Size = 16\*sizeof\(T\)，T为操作数的数据类型\)字节。
- dstStride = 80，表示目的DN矩阵每一行中的元素个数为80。
- srcNzMatrixStride = 240，表示不同源NZ矩阵的起始地址的间隔为240 \* C0\_Size。
- dstDnMatrixStride = 48 \* 80，表示相邻目的DN矩阵起始地址间的间隔为48 \* 80 =3840个元素，40为间隔行数，80为dstStride （每行的元素个数）。
- srcNzC0Stride = 1：表示源矩阵NZ分形相邻行的地址偏移为1 \* C0\_Size。
