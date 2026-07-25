# GM与L1或L0数据搬运

## 总体说明<a name="ZH-CN_TOPIC_0000002543422572"></a>

Global Memory与L1 Buffer/L0 Buffer之间的数据搬运提供了灵活、分层的接口支持，能够充分适配多样化的计算场景（见[表1](#zh-cn_topic_0000002565857683_table12546123753110)）。其中，Global Memory与L1 Buffer、Global Memory-\>L0A Buffer、Global Memory-\>L0B Buffer的数据搬运用于矩阵计算（参考[矩阵计算的搬入](../矩阵计算（ISASI）/矩阵计算的搬入/矩阵计算的搬入.md)），L0C Buffer-\>Global Memory用于将矩阵计算结果搬出，完成矩阵计算的输出流程（参考[矩阵计算的搬出](../矩阵计算（ISASI）/矩阵计算的搬出/矩阵计算的搬出.md)）。开发者能够通过灵活配置结构体参数，精确控制数据块数量、数据块长度及地址间隔等关键属性，实现高维切分数据搬运，充分释放硬件的数据搬运与计算协同能力，显著提升算子的执行效率与整体性能。

**表 1**  Global Memory与L1 Buffer/L0 Buffer数据搬运接口概述<a name="zh-cn_topic_0000002565857683_table12546123753110"></a>

| 数据通路 | 功能 | 描述 |
|----------|------|------|
| Global Memory与L1 Buffer | 连续数据搬运(DataCopy) | 提供基础的数据搬运能力，数据在传输过程中保持原始格式和内容不变，支持连续数据搬运。 |
| Global Memory与L1 Buffer | 高维切分数据搬运(DataCopy) | 提供基础的数据搬运能力，数据在传输过程中保持原始格式和内容不变，支持连续和非连续的数据搬运。 |
| Global Memory->L1 Buffer | 随路转换-[ND2NZ](<../矩阵计算（ISASI）/矩阵计算分形介绍/关键分形格式详解.md#ZH-CN_TOPIC_0000002568950893>)搬运(DataCopy) | 支持在数据搬运时进行[ND](<../矩阵计算（ISASI）/矩阵计算分形介绍/关键分形格式详解.md#zh-cn_topic_0000002545089965_section958745018719>)到[NZ](<../矩阵计算（ISASI）/矩阵计算分形介绍/关键分形格式详解.md#zh-cn_topic_0000002545089965_section363412741113>)格式的转换。 |
| Global Memory->L1 Buffer | 随路转换-[DN2NZ](<../矩阵计算（ISASI）/矩阵计算分形介绍/关键分形格式详解.md#ZH-CN_TOPIC_0000002568950893>)搬运(DataCopy) | 支持在数据搬运时进行[DN](<../矩阵计算（ISASI）/矩阵计算分形介绍/关键分形格式详解.md#zh-cn_topic_0000002545089965_section958745018719>)到[NZ](<../矩阵计算（ISASI）/矩阵计算分形介绍/关键分形格式详解.md#zh-cn_topic_0000002545089965_section363412741113>)格式的转换。 |
| Global Memory->L1 Buffer | 非对齐数据搬运(DataCopyPad) | 支持数据非对齐搬运，可以根据开发者的需要自行填充数据。 |
| Global Memory->L1 Buffer | [NZ](<../矩阵计算（ISASI）/矩阵计算分形介绍/关键分形格式详解.md#zh-cn_topic_0000002545089965_section363412741113>)数据搬运(LoadData（2D矩阵搬运）) | 负责完成[NZ](<../矩阵计算（ISASI）/矩阵计算分形介绍/关键分形格式详解.md#zh-cn_topic_0000002545089965_section363412741113>)矩阵的数据搬运。 |
| Global Memory->L1 Buffer | [NZ](<../矩阵计算（ISASI）/矩阵计算分形介绍/关键分形格式详解.md#zh-cn_topic_0000002545089965_section363412741113>)数据搬运(LoadData（2D矩阵搬运V2）) | 负责完成[NZ](<../矩阵计算（ISASI）/矩阵计算分形介绍/关键分形格式详解.md#zh-cn_topic_0000002545089965_section363412741113>)矩阵的数据搬运。 |
| Global Memory->L0A Buffer | 2D格式分形矩阵搬运(LoadData（2D矩阵搬运）) | 负责完成普通矩阵计算所需的2D格式数据的搬运。 |
| Global Memory->L0B Buffer | 2D格式分形矩阵搬运(LoadData（2D矩阵搬运）) | 负责完成普通矩阵计算所需的2D格式数据的搬运。 |
| L0C Buffer->Global Memory | 随路量化激活搬运(DataCopy) | 支持多种随路能力的组合，需要设置不同的寄存器，配合DataCopy接口使能不同的数据搬运能力。 |
| L0C Buffer->Global Memory | 随路量化激活搬运(FixPipe) | 支持多种随路能力的组合，FixPipe接口内包含了设置寄存器与数据搬运能力。 |

## DataCopy（GM与L1连续数据搬运）<a name="ZH-CN_TOPIC_0000002574022813"></a>

DataCopy能够实现Global Memory和L1 Buffer之间的连续数据搬运，数据搬运时格式和内容保持不变。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Global Memory -> L1 Buffer
    - GM -> A1/B1
- L1 Buffer -> Global Memory
    - A1/B1 -> GM

搬运的数据用于[矩阵计算](../矩阵计算（ISASI）/矩阵计算（ISASI）.md)，接口具体介绍请参考：[DataCopy（GMToL1连续数据搬运）](../矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（GMToL1连续数据搬运）.md)。

src和dst分别为源操作数和目的操作数；count为连续搬运的元素个数。

- Global Memory -> L1 Buffer

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const uint32_t count)
    ```

- L1 Buffer -> Global Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
    ```

## DataCopy（GM与L1高维切分数据搬运）<a name="ZH-CN_TOPIC_0000002543262916"></a>

DataCopy能够实现Global Memory和L1 Buffer之间的连续数据搬运和非连续数据搬运，数据搬运时格式和内容保持不变。

高维切分是指能够通过配置数据块个数、单个数据块长度、地址偏移等搬运参数实现非连续搬运。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Global Memory -> L1 Buffer
    - GM -> A1/B1
- L1 Buffer -> Global Memory
    - A1/B1 -> GM

搬运的数据用于[矩阵计算](../矩阵计算（ISASI）/矩阵计算（ISASI）.md)，接口具体介绍请参考：[DataCopy（GMToL1高维切分数据搬运）](../矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（GMToL1高维切分数据搬运）.md)。

src和dst分别为源操作数和目的操作数；repeatParams为搬运参数。

- Global Memory -> L1 Buffer

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& repeatParams)
    ```

- L1 Buffer -> Global Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const  LocalTensor<T>& src, const DataCopyParams& repeatParams)
    ```

## DataCopy（GMToL1随路转换-ND2NZ搬运）<a name="ZH-CN_TOPIC_0000002573902841"></a>

该接口主要实现将数据从Global Memory搬运至L1 Buffer，并支持在数据搬运时进行[ND](../矩阵计算（ISASI）/矩阵计算分形介绍/关键分形格式详解.md)到[NZ](../矩阵计算（ISASI）/矩阵计算分形介绍/关键分形格式详解.md)格式的转换。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Global Memory -> L1 Buffer
    - GM -> A1/B1

搬运的数据用于[矩阵计算](../矩阵计算（ISASI）/矩阵计算（ISASI）.md)，接口具体介绍请参考：[DataCopy（GMToL1随路转换-ND2NZ搬运）](../矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（GMToL1随路转换-ND2NZ搬运）.md)。

src和dst分别为源操作数和目的操作数；intriParams为ND2NZ搬运配置参数。

- 不支持enableSmallC0模式

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
    ```

<!-- npu="950" id1 -->
- 支持enableSmallC0模式（仅Ascend 950PR/Ascend 950DT支持）

    ```cpp
    template <typename T, bool enableSmallC0 = false>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
    ```
<!-- end id1 -->

<!-- npu="950" id2 -->
## DataCopy（GMToL1随路转换-DN2NZ搬运）<a name="ZH-CN_TOPIC_0000002573902899"></a>

该接口主要实现将数据从Global Memory搬运至L1 Buffer，并支持在数据搬运时进行[DN](../矩阵计算（ISASI）/矩阵计算分形介绍/关键分形格式详解.md)到[NZ](../矩阵计算（ISASI）/矩阵计算分形介绍/关键分形格式详解.md)格式的转换。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Global Memory -> L1 Buffer
    - GM -> A1/B1

搬运的数据用于[矩阵计算](../矩阵计算（ISASI）/矩阵计算（ISASI）.md)，接口具体介绍请参考：[DataCopy（GMToL1随路转换-DN2NZ搬运）](../矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（GMToL1随路转换-DN2NZ搬运）.md)。

src和dst分别为源操作数和目的操作数；intriParams为DN2NZ搬运配置参数。

- 支持enableSmallC0模式（仅Ascend 950PR/Ascend 950DT支持）

    ```cpp
    template <typename T, bool enableSmallC0 = false>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Dn2NzParams& intriParams);
    ```
<!-- end id2 -->

<!-- npu="950" id3 -->
## DataCopyPad（GMToL1非对齐数据搬运）<a name="ZH-CN_TOPIC_0000002543262891"></a>

该接口主要实现将数据从Global Memory搬运至L1 Buffer，并支持在数据搬运时提供非对齐数据搬运功能，可以根据开发者的需要自行填充数据。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Global Memory -> L1 Buffer
    - GM -> A1/B1

搬运的数据用于[矩阵计算](../矩阵计算（ISASI）/矩阵计算（ISASI）.md)，接口具体介绍请参考：[DataCopyPad（GMToL1非对齐数据搬运）](../矩阵计算（ISASI）/矩阵计算的搬入/DataCopyPad（GMToL1非对齐数据搬运）.md)。

src和dst分别为源操作数和目的操作数；dataCopyParams和padParams为搬运参数。

- 仅Ascend 950PR/Ascend 950DT支持

    ```cpp
    template <typename T, PaddingMode mode = PaddingMode::Normal>
    __aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams, const DataCopyPadExtParams<T>& padParams)
    ```
<!-- end id3 -->

## GMToL1 NZ数据搬运(LoadData（2D矩阵搬运）)<a name="ZH-CN_TOPIC_0000002543422574"></a>

该接口主要实现将数据从Global Memory搬运至L1 Buffer，负责完成普通矩阵计算所需的2D格式数据的搬运，以大小为512Byte的数据分形为单位进行搬运。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Global Memory -> L1 Buffer
    - GM -> A1/B1

搬运的数据用于[矩阵计算](../矩阵计算（ISASI）/矩阵计算（ISASI）.md)，接口具体介绍请参考：[LoadData（GMToL1-2D矩阵搬运）](../矩阵计算（ISASI）/矩阵计算的搬入/LoadData（GMToL1-2D矩阵搬运）.md)。

src和dst分别为源操作数和目的操作数；loadDataParams为搬运参数。

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParams& loadDataParams)
```

<!-- npu="950" id4 -->
## GMToL1 NZ数据搬运(LoadData（2D矩阵搬运V2）)

该接口主要实现将数据从Global Memory搬运至L1 Buffer，负责完成普通矩阵计算所需的2D格式数据的搬运，以大小为512Byte的数据分形为单位进行搬运。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Global Memory -> L1 Buffer
    - GM -> A1/B1

搬运的数据用于[矩阵计算](../矩阵计算（ISASI）/矩阵计算（ISASI）.md)，接口具体介绍请参考：[LoadData（GMToL1-2D矩阵搬运V2）](../矩阵计算（ISASI）/矩阵计算的搬入/LoadData（GMToL1-2D矩阵搬运V2）.md)。

src和dst分别为源操作数和目的操作数；loadDataParams为搬运参数。

- 仅Ascend 950PR/Ascend 950DT支持

    ```cpp
    template <typename T>
    __aicore__ inline void LoadData(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParamsV2& loadDataParams)
    ```
<!-- end id4 -->

## GMToL0A 2D格式分形矩阵搬运(LoadData（2D矩阵搬运）)<a name="ZH-CN_TOPIC_0000002574022815"></a>

该接口主要实现将数据从Global Memory搬运至L0A Buffer，负责完成普通矩阵计算所需的2D格式数据的搬运，以大小为512Byte的数据分形为单位进行搬运。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Global Memory -> L0A Buffer
    - GM -> A2

搬运的数据用于[矩阵计算](../矩阵计算（ISASI）/矩阵计算（ISASI）.md)，接口具体介绍请参考：[LoadData（2D矩阵搬运）](../矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md)。

src和dst分别为源操作数和目的操作数；loadDataParams为搬运参数。

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParams& loadDataParams)
```

## GMToL0B 2D格式分形矩阵搬运(LoadData（2D矩阵搬运）)<a name="ZH-CN_TOPIC_0000002543262918"></a>

该接口主要实现将数据从Global Memory搬运至L0B Buffer，负责完成普通矩阵计算所需的2D格式数据的搬运，以大小为512Byte的数据分形为单位进行搬运。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- Global Memory -> L0B Buffer
    - GM -> B2

搬运的数据用于[矩阵计算](../矩阵计算（ISASI）/矩阵计算（ISASI）.md)，接口具体介绍请参考：[LoadData（2D矩阵搬运）](../矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md)。

src和dst分别为源操作数和目的操作数；loadDataParams为搬运参数。

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParams& loadDataParams)
```

## DataCopy（L0CToGM随路量化激活搬运）<a name="ZH-CN_TOPIC_0000002573902843"></a>

该接口主要实现将数据从L0C Buffer搬运至Global Memory，并支持多种随路能力的组合，需要设置不同的寄存器。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- L0C Buffer -> Global Memory
    - CO1 -> GM

搬运的数据为[矩阵计算](../矩阵计算（ISASI）/矩阵计算（ISASI）.md)的结果，接口具体介绍请参考：[DataCopy（L0C到GM数据搬运）](../矩阵计算（ISASI）/矩阵计算的搬出/DataCopy（L0C到GM数据搬运）.md)。

src和dst分别为源操作数和目的操作数；intriParams为搬运参数。

```cpp
template <typename T, typename U>
__aicore__ inline void DataCopy(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyCO12DstParams& intriParams)
```

## Fixpipe（L0CToGM随路量化激活搬运）<a name="ZH-CN_TOPIC_0000002543422576"></a>

该接口主要实现将数据从L0C Buffer搬运至Global Memory，并支持多种随路能力的组合，接口内包含了设置寄存器与数据搬运能力。

具体支持的数据通路为（以[逻辑位置TPosition](../../基础API/辅助数据结构/TPosition.md)表示）：

- L0C Buffer -> Global Memory
    - CO1 -> GM

搬运的数据为[矩阵计算](../矩阵计算（ISASI）/矩阵计算（ISASI）.md)的结果，以Ascend 950PR/Ascend 950DT为例，接口示例如下：

注意，不同产品型号的接口原型可能不同，具体介绍请参考：[Fixpipe（L0C到GM数据搬运）](../矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)。

src和dst分别为源操作数和目的操作数；intriParams为搬运参数，cbufWorkspace为开启tensor量化时所需的量化参数。

- 不开启随路[tensor量化](../矩阵计算（ISASI）/矩阵搬出关键特性说明/随路量化.md)功能：

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

- 开启随路[tensor量化](../矩阵计算（ISASI）/矩阵搬出关键特性说明/随路量化.md)功能：

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint64_t>& cbufWorkspace, const FixpipeParamsArch3510<config.format>& intriParams)
    ```
