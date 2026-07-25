# NZ2NZ<a name="ZH-CN_TOPIC_0000002538231204"></a>

## 特性说明

矩阵计算的搬出过程中NZ2NZ的核心功能体现为，矩阵计算结果搬出前后保持NZ数据排布不变。

<!-- npu="A3,910b" id1 -->
以如下产品型号为例：
<!-- npu="A3" id2 -->
Atlas A3 训练系列产品/Atlas A3 推理系列产品
<!-- end id2 -->
<!-- npu="910b" id3 -->
Atlas A2 训练系列产品/Atlas A2 推理系列产品
<!-- end id3 -->
NZ2NZ转化过程可以参考以下伪代码：

```cpp
for (j = 0; j < mSize; j++) {
    src_temp_n_addr = src + j * sizeof(L0C_element) * 16;
    dst_temp_n_addr = dst + j * 16 * sizeof(DST_element);
    for (k = 0; k < ceil(nSize / 16); k++) {
        src_block_addr = src_temp_n_addr + k * srcStride * sizeof(L0C_element) * 16;
        dst_block_addr = dst_temp_n_addr + k * dstStride * 32;
    }
}
```

伪代码使用的参数具体说明如下：

- src表示源操作数搬运的起始地址。
- dst表示目的操作数搬运的起始地址。
- 源操作数中NZ矩阵每一行的起始地址为src\_temp\_n\_addr。
- 目的操作数中NZ矩阵每一行的起始地址为dst\_temp\_n\_addr。
- 在每一行中，以16个数据为粒度进行数据块切分，源操作数每一块数据块的起始地址为src\_block\_addr，目的操作数中每一数据块起始地址为dst\_block\_addr。

以下参数为NZ2NZ用户可配置参数，含义及取值范围参见[Fixpipe搬运参数](../矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)：

- mSize与nSize分别表示矩阵M方向和N方向上的维度。
- srcStride表示源NZ矩阵中不同Z分形列的间隔，单位为C0\_Size（C0\_Size=16\*sizeof\(T\)，T为操作数的数据类型）。
- dstStride表示目标NZ矩阵中不同Z分形列的间隔，单位为32字节。
<!-- end id1 -->

<!-- npu="950" id4 -->
以Ascend 950PR/Ascend 950DT为例，NZ2NZ转化过程可以参考以下伪代码：

```cpp
for (j = 0; j < mSize; j++) {
    src_temp_n_addr = src + j * sizeof(L0C_element) * 16;
    dst_temp_n_addr = dst + j * 16 * sizeof(DST_element);
    for (k = 0; k < ceil(nSize / 16); k++) {
        src_block_addr = src_temp_n_addr + k * srcStride * sizeof(L0C_element) * 16;
        dst_block_addr = dst_temp_n_addr + k * dstStride * sizeof(DST_element);
    }
}
```

伪代码使用的参数具体说明如下：

- src表示源操作数搬运的起始地址。
- dst表示目的操作数搬运的起始地址。
- 源操作数中NZ矩阵每一行的起始地址为src\_temp\_n\_addr。
- 目的操作数中NZ矩阵每一行的起始地址为dst\_temp\_n\_addr。
- 在每一行中，以16个数据为粒度进行数据块切分，源操作数每一块数据块的起始地址为src\_block\_addr，目的操作数中每一数据块起始地址为dst\_block\_addr。

以下参数为NZ2NZ用户可配置参数，含义及取值范围参见[Fixpipe搬运参数](../矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)：

- mSize与nSize分别表示矩阵M方向和N方向上的维度。
- srcStride表示源NZ矩阵中不同Z分形列的间隔，单位为C0\_Size（C0\_Size=16\*sizeof\(T\)，T为操作数的数据类型）。
- dstStride表示目标NZ矩阵中不同Z分形列的间隔，单位为element。
<!-- end id4 -->

## 特性约束

- 当NZ2NZ与F32 Channel Split特性组合开启时，搬出到GM的NZ分形中的小分形维度为16\*8，确保小z分形的大小为512B。
- 对于B8数据类型NZ输出时，Channel Merge默认开启，搬出GM/L1 Buffer小分形维度为16\*32，确保小z分形的大小为512B。
- 对于B4数据类型NZ输出时，Channel Merge默认开启，搬出GM/L1 Buffer小分形维度为16\*64，确保小z分形的大小为512B。
- 目标操作数地址之间不能重叠。

## 使用示例

如下图所示，在L0C Buffer中有一个NZ格式的矩阵，该矩阵N方向上维度为48，M方向上维度为24，经过NZ2NZ转换，将数据格式变为右部分所示排布。

<!-- npu="A3,910b" id6 -->
**图1** NZ2NZ示意图（[NPU架构版本2201](../../../../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）<a name="zh-cn_topic_0000002515820868_fig199191646174210"></a>  

![](../../../../figures/Fixpipe_NZ2NZ.png)

示意图中参数设置示例（通过Fixpipe接口搬运并去除dummy数据）和解释说明如下：

- 当M方向上的数据元素个数不是16的倍数时，搬入时会额外读取dummy数据，并在写入目标位置后丢弃这些dummy数据。矩阵块被定义为连续的16\*16的数据块，数据块的个数为M/16向上取整，矩阵块的长度为M\*16\*sizeof\(T\)，T是数据类型。
- nSize=48，表示源NZ矩阵中待搬运矩阵在N方向上的大小为48个元素。
- mSize=24，表示源NZ矩阵中待搬运矩阵在M方向上的大小为24个元素。
- srcStride=64，表示源NZ矩阵中待搬运矩阵相邻Z排布的起始地址偏移，即下图中第一个Z排布的起始地址与第二个Z排布的起始地址之间的间隔为64 \* C0\_Size。
- dstStride = 40，表示目的NZ矩阵中相邻Z排布的起始地址偏移，即下图中第一个Z排布的起始地址与第二个Z排布的起始地址之间的间隔为40\*32字节。
<!-- end id6 -->

<!-- npu="950" id5 -->
**图2** NZ2NZ示意图（[NPU架构版本3510](../../../../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）<a name="zh-cn_topic_0000002515820868_fig199191646174233"></a>  

![](../../../../figures/Fixpipe_NZ2NZ_950.png)

示意图中参数设置示例（通过Fixpipe接口搬运并去除dummy数据）和解释说明如下：

- 当M方向上的数据元素个数不是16的倍数时，搬入时会额外读取dummy数据，并在写入目标位置后丢弃这些dummy数据。矩阵块被定义为连续的16\*16的数据块，数据块的个数为M/16向上取整，矩阵块的长度为M\*16\*sizeof\(T\)，T是数据类型。
- nSize=48，表示源NZ矩阵中待搬运矩阵在N方向上的大小为48个元素。
- mSize=24，表示源NZ矩阵中待搬运矩阵在M方向上的大小为24个元素。
- srcStride=64，表示源NZ矩阵中待搬运矩阵相邻Z排布的起始地址偏移，即下图中第一个Z排布的起始地址与第二个Z排布的起始地址之间的间隔为64 \* C0\_Size。
- dstStride = 40 * 16，表示目的NZ矩阵中相邻Z排布的起始地址偏移，即下图中第一个Z排布的起始地址与第二个Z排布的起始地址之间的间隔为40\*16个元素。
<!-- end id5 -->
