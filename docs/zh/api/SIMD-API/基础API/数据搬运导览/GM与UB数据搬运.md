# GM与UB数据搬运<a name="ZH-CN_TOPIC_0000002573902833"></a>

## 总体说明<a name="ZH-CN_TOPIC_0000002543422566"></a>

Global Memory（GM）与Unified Buffer（UB）之间的数据搬运提供了多样化的接口，支持丰富的数据搬运场景（见[表1](#zh-cn_topic_0000002565937657_table12546123753110)）。为实现高效的向量（Vector）计算，系统支持将数据从GM搬运至UB，供Vector计算单元进行向量计算；计算完成后，再将结果从UB搬回GM，完成数据闭环。开发者能够通过灵活配置结构体参数，精确控制数据块数量、每个地址长度及地址间隔等关键属性，实现连续和非连续数据搬运（高维切分数据搬运），充分释放硬件的数据搬运与计算协同能力，显著提升向量算子的执行效率与整体性能。

**表 1**  GM与UB数据搬运接口概述

<a name="zh-cn_topic_0000002565937657_table12546123753110"></a>

| 数据通路 | 功能 | 描述 |
| :---: | :--- | :--- |
| GM->UB | 连续数据搬运(DataCopy) | 提供基础的数据搬运能力，数据在传输过程中保持原始格式和内容不变，支持连续数据搬运。 |
| UB->GM | 连续数据搬运(DataCopy) | 提供基础的数据搬运能力，数据在传输过程中保持原始格式和内容不变，支持连续数据搬运。 |
| GM->UB | 高维切分数据搬运(DataCopy) | 提供基础的数据搬运能力，数据在传输过程中保持原始格式和内容不变，支持连续和非连续的数据搬运。 |
| UB->GM | 高维切分数据搬运(DataCopy) | 提供基础的数据搬运能力，数据在传输过程中保持原始格式和内容不变，支持连续和非连续的数据搬运。 |
| GM->UB | 切片数据搬运(DataCopy) | 该接口为软仿接口，支持数据的切片搬运，提取多维Tensor数据的子集进行搬运。 |
| UB->GM | 切片数据搬运(DataCopy) | 该接口为软仿接口，支持数据的切片搬运，提取多维Tensor数据的子集进行搬运。 |
| GM->UB | 随路转换ND2NZ搬运(DataCopy) | 该接口为软仿接口，支持在从Global Memory到Unified Buffer的数据搬运过程中进行ND到NZ格式的转换。 |
| UB->GM | 随路转换NZ2ND搬运(DataCopy) | 该接口为软仿接口，支持在从Unified Buffer到Global Memory的数据搬运过程中进行NZ到ND格式的转换。 |
| GM->UB | 多维数据搬运NDDMA(DataCopy) | 多维数据搬运接口，相比于基础数据搬运接口，可更加自由配置搬入的维度信息以及对应的Stride。 |
| GM->UB | 非对齐数据搬运(DataCopyPad) | 支持数据的非对齐搬运，搬运到UB时可以自行填充数据。 |
| UB->GM | 非对齐数据搬运(DataCopyPad) | 支持数据的非对齐搬运。 |

## DataCopy（GM与UB连续数据搬运）<a name="ZH-CN_TOPIC_0000002382908021"></a>

支持Global Memory与Unified Buffer之间的连续数据搬运，数据在传输过程中保持原始格式和内容不变。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Global Memory -> Unified Buffer
    - GM -> VECIN
- Unified Buffer -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id1 -->
    - CO2 -> GM（仅Atlas 推理系列产品AI Core支持）
    <!-- end id1 -->

搬运的数据用于[矢量计算](../Memory矢量计算/Memory矢量计算.md)，具体的接口请参考：[DataCopy（GM与UB连续数据搬运）](../Memory矢量计算/数据搬运/DataCopy（GM与UB连续数据搬运）.md)。

src和dst分别为源操作数和目的操作数；count为连续搬运的元素个数。

- Global Memory -> Unified Buffer

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const uint32_t count)
    ```

- Unified Buffer -> Global Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
    ```

## DataCopy（GM与UB高维切分数据搬运）<a name="ZH-CN_TOPIC_00000023829080211"></a>

支持Global Memory与Unified Buffer之间的高维切分数据搬运，数据在传输过程中保持原始格式和内容不变。

高维切分数据搬运可通过配置数据块个数、单个数据块长度、地址偏移等搬运参数，同时支持非连续和连续的数据搬运。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Global Memory -> Unified Buffer
    - GM -> VECIN
- Unified Buffer -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id2 -->
    - CO2 -> GM（仅Atlas 推理系列产品AI Core支持）
    <!-- end id2 -->

搬运的数据用于[矢量计算](../Memory矢量计算/Memory矢量计算.md)，具体的接口请参考：[DataCopy（GM与UB高维切分数据搬运）](../Memory矢量计算/数据搬运/DataCopy（GM与UB高维切分数据搬运）.md)。

src和dst分别为源操作数和目的操作数；repeatParams为DataCopyParams类型的搬运参数，通过该参数可配置搬运的数据块大小、个数、间隔等信息，同时支持非连续和连续搬运。

- Global Memory -> Unified Buffer

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& repeatParams)
    ```

- Unified Buffer -> Global Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
    ```

## DataCopy（GM与UB切片数据搬运）<a name="ZH-CN_TOPIC_0000002568770166"></a>

该接口为软仿接口，从易用性角度出发进行设计，支持数据的切片搬运，提取多维Tensor数据的子集进行搬运。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Global Memory -> Unified Buffer
    - GM -> VECIN
- Unified Buffer -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id3 -->
    - CO2 -> GM（仅Atlas 推理系列产品AI Core支持）
    <!-- end id3 -->

搬运的数据用于[矢量计算](../Memory矢量计算/Memory矢量计算.md)，具体的接口请参考：[DataCopy（GM与UB切片数据搬运）](../Memory矢量计算/数据搬运/DataCopy（GM与UB切片数据搬运）.md)。

src和dst分别为源操作数和目的操作数；dstSliceInfo和srcSliceInfo分别为目的和源操作数的切片信息，类型为SliceInfo；dimValue为操作数维度信息。

- Global Memory -> Unified Buffer

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const SliceInfo dstSliceInfo[], const SliceInfo srcSliceInfo[], const uint32_t dimValue = 1)
    ```

- Unified Buffer -> Global Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const GlobalTensor<T> &dst, const LocalTensor<T> &src, const SliceInfo dstSliceInfo[], const SliceInfo srcSliceInfo[], const uint32_t dimValue = 1)
    ```

## DataCopy（GM -> UB随路转换ND2NZ搬运）<a name="ZH-CN_TOPIC_0000002349187356"></a>

该接口为软仿接口，从易用性角度出发进行设计，支持在从Global Memory到Unified Buffer的数据搬运过程中进行ND到NZ格式的转换。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Global Memory -> Unified Buffer
    - GM -> VECIN

搬运的数据用于[矢量计算](../Memory矢量计算/Memory矢量计算.md)，具体的接口请参考：[DataCopy（GM -> UB随路转换ND2NZ搬运）](../Memory矢量计算/数据搬运/DataCopy（GMToUB随路转换ND2NZ搬运）.md)。

src和dst分别为源操作数和目的操作数；intriParams为Nd2NzParams类型的搬运参数，配置ND到NZ格式转换信息。

- 不支持enableSmallC0模式

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
    ```

<!-- npu="950" id4 -->
- 支持enableSmallC0模式（仅Ascend 950PR/Ascend 950DT支持）

    ```cpp
    template <typename T, bool enableSmallC0 = false>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
    ```
<!-- end id4 -->

## DataCopy（UB -> GM随路转换NZ2ND搬运）<a name="ZH-CN_TOPIC_0000002391805265"></a>

该接口为软仿接口，从易用性角度出发进行设计，支持在从Unified Buffer到Global Memory的数据搬运过程中进行NZ到ND格式的转换。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Unified Buffer -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id5 -->
    - CO2 -> GM（仅Atlas 推理系列产品AI Core支持）
    <!-- end id5 -->

搬运的数据用于[矢量计算](../Memory矢量计算/Memory矢量计算.md)，具体的接口请参考：[DataCopy（UB->GM随路转换NZ2ND搬运）](../Memory矢量计算/数据搬运/DataCopy（UBToGM随路转换NZ2ND搬运）.md)。

src和dst分别为源操作数和目的操作数；intriParams为Nz2NdParamsFull类型的搬运参数，配置NZ到ND格式转换信息。

```cpp
template <typename T>
__aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const Nz2NdParamsFull& intriParams)
```

## DataCopy（GM -> UB多维数据搬运NDDMA）<a name="ZH-CN_TOPIC_0000002544407954"></a>

多维数据搬运接口，相比于基础数据搬运接口，可更加自由配置搬入的维度信息以及对应的Stride。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Global Memory -> Unified Buffer
    - GM -> VECIN

搬运的数据用于[矢量计算](../Memory矢量计算/Memory矢量计算.md)，具体的接口请参考：[DataCopy（GM -> UB多维数据搬运NDDMA）](../Memory矢量计算/数据搬运/DataCopy（GMToUB多维数据搬运NDDMA）.md)。

src和dst分别为源操作数和目的操作数；params为NdDmaParams类型的搬运参数，配置多维度搬运信息。

- Global Memory -> Unified Buffer，支持多维度搬运

    ```cpp
    template <typename T, uint8_t dim, const NdDmaConfig& config = kDefaultNdDmaConfig>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const NdDmaParams<T, dim>& params)
    ```

- NDDMA Cache刷新

    ```cpp
    __aicore__ inline void NdDmaDci()
    ```

## DataCopyPad（GM -> UB非对齐数据搬运）<a name="ZH-CN_TOPIC_0000001894460401"></a>

该接口提供将数据从Global Memory非对齐搬运至Unified Buffer的功能，可以根据开发者的需要自行填充数据。当每个搬运的数据块长度（blockLen）非32字节对齐时，每一个数据块都需要填充数据至32字节对齐。

<!-- npu="950" id6 -->
特别地，针对Ascend 950PR/Ascend 950DT，支持Compact模式，该模式支持单个数据块非32字节对齐，将所有数据块合并成一个连续数据块，在该数据块右侧填充数据至32字节对齐。
<!-- end id6 -->

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Global Memory -> Unified Buffer
    - GM -> VECIN
    - GM -> VECOUT
    <!-- npu="950" id7 -->
    - GM -> VECCALC（仅Ascend 950PR/Ascend 950DT支持）
    <!-- end id7 -->

搬运的数据用于[矢量计算](../Memory矢量计算/Memory矢量计算.md)，具体的接口请参考：[DataCopyPad（GM -> UB非对齐数据搬运）](../Memory矢量计算/数据搬运/DataCopyPad（GMToUB非对齐数据搬运）.md)。

src和dst分别为源操作数和目的操作数；dataCopyParams为DataCopyExtParams类型的搬运参数；padParams为DataCopyPadExtParams类型的填充控制参数。

- 不支持配置设置数据搬运模式mode

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams, const DataCopyPadExtParams<T>& padParams)
    ```

<!-- npu="950" id8 -->
- 支持配置设置数据搬运模式mode（仅Ascend 950PR/Ascend 950DT支持）

    ```cpp
    template <typename T, PaddingMode mode = PaddingMode::Normal>
    __aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams, const DataCopyPadExtParams<T>& padParams)
    ```
<!-- end id8 -->

## DataCopyPad（UB -> GM非对齐数据搬运）<a name="ZH-CN_TOPIC_0000001894460502"></a>

该接口提供将数据从Unified Buffer非对齐搬运至Global Memory的功能。

对于非32字节对齐的场景，在读取Unified Buffer数据时会填入dummy假数据对齐到32B，搬入Global Memory时会将dummy空数据丢弃，从而实现Unified Buffer到Global Memory的非对齐搬运。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Unified Buffer -> Global Memory
    - VECIN -> GM
    - VECOUT -> GM

搬运的数据用于[矢量计算](../Memory矢量计算/Memory矢量计算.md)，具体的接口请参考：[DataCopyPad（UB -> GM非对齐数据搬运）](../Memory矢量计算/数据搬运/DataCopyPad（UBToGM非对齐数据搬运）.md)。

src和dst分别为源操作数和目的操作数；dataCopyParams为DataCopyExtParams类型的搬运参数。

- 不支持配置设置数据搬运模式mode

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopyPad(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams)
    ```

<!-- npu="950" id9 -->
- 支持配置设置数据搬运模式mode（仅Ascend 950PR/Ascend 950DT支持）

    ```cpp
    template <typename T, PaddingMode mode = PaddingMode::Normal>
    __aicore__ inline void DataCopyPad(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams)
    ```
<!-- end id9 -->
