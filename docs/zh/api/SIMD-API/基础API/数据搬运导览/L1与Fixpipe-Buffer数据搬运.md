# L1与Fixpipe Buffer数据搬运

## DataCopy（L1ToFixpipe Buffer连续数据搬运）<a name="ZH-CN_TOPIC_0000002543262934"></a>

DataCopy能够实现L1 Buffer至Fixpipe Buffer的连续数据搬运，数据搬运时格式和内容保持不变。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- L1 Buffer -> Fixpipe Buffer
    - C1 -> C2PIPE2GM

搬运的数据为[随路量化](../矩阵计算（ISASI）/矩阵搬出关键特性说明/随路量化.md)和[随路relu](../矩阵计算（ISASI）/矩阵搬出关键特性说明/随路ReLU.md)参数，接口具体介绍请参考：[DataCopy（L1ToFixpipe-Buffer数据搬运）](../矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（L1ToFixpipe-Buffer数据搬运）.md)。

src和dst分别为源操作数和目的操作数；count为连续搬运的元素个数。

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count);
```

## DataCopy（L1ToFixpipe Buffer高维切分数据搬运）<a name="ZH-CN_TOPIC_0000002573902863"></a>

DataCopy能够实现L1 Buffer至Fixpipe Buffer的连续数据搬运和非连续数据搬运，数据搬运时格式和内容保持不变。

高维切分是指能够通过配置数据块个数、单个数据块长度、地址偏移等搬运参数实现非连续搬运。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- L1 Buffer -> Fixpipe Buffer
    - C1 -> C2PIPE2GM

搬运的数据为[随路量化](../矩阵计算（ISASI）/矩阵搬出关键特性说明/随路量化.md)和[随路relu](../矩阵计算（ISASI）/矩阵搬出关键特性说明/随路ReLU.md)参数，接口具体介绍请参考：[DataCopy（L1ToFixpipe-Buffer数据搬运）](../矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（L1ToFixpipe-Buffer数据搬运）.md)。

src和dst分别为源操作数和目的操作数；repeatParams为搬运参数。

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```
