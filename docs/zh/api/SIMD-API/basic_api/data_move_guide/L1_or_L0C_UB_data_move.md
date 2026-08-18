# L1或L0C与UB数据搬运

## 总体说明<a name="ZH-CN_TOPIC_0000002574022823"></a>

L1 Buffer/L0C Buffer与Unified Buffer（UB）之间的数据搬运提供了灵活的接口支持，能够充分适配多样化的计算场景（见[表1](#zh-cn_topic_0000002534897870_table1417935217588)）。其中，UB-\>L1 Buffer的数据搬运用于矩阵计算（参考[矩阵计算的搬入](../cube_compute_ISASI/cube_compute_load/cube_compute_load.md)），L0C Buffer-\>UB、L1 Buffer-\>UB用于将矩阵计算结果搬出，完成矩阵计算的输出流程（参考[矩阵计算的搬出](../cube_compute_ISASI/cube_compute_store/cube_compute_store.md)）。

**表 1**  L1 Buffer/L0C Buffer与UB数据搬运接口概述<a name="zh-cn_topic_0000002534897870_table1417935217588"></a>

| 数据通路 | 功能 | 描述 |
|----------|------|------|
| UB->L1 Buffer | 连续数据搬运(DataCopy) | 提供基础的数据搬运能力，数据在传输过程中保持原始格式和内容不变，支持连续数据搬运。 |
| UB->L1 Buffer | 高维切分数据搬运(DataCopy) | 提供基础的数据搬运能力，数据在传输过程中保持原始格式和内容不变，支持连续和非连续的数据搬运。 |
| UB->L1 Buffer | 随路转换-[ND2NZ](<../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md#ZH-CN_TOPIC_0000002568950893>)搬运(DataCopy) | 支持在数据搬运时进行[ND](<../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md#zh-cn_topic_0000002545089965_section958745018719>)到[NZ](<../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md#zh-cn_topic_0000002545089965_section363412741113>)格式的转换。 |
| UB->L1 Buffer | 非对齐数据搬运(DataCopyPad) | 支持数据非对齐搬运，可以根据开发者的需要自行填充数据。 |
| L0C Buffer->UB | 随路转换与量化(DataCopy) | 支持多种随路能力的组合，DataCopy接口内包含了设置寄存器与数据搬运能力。 |
| L0C Buffer->UB | 随路量化激活搬运(FixPipe) | 支持多种随路能力的组合，FixPipe接口内包含了设置寄存器与数据搬运能力。 |
| L1 Buffer->UB | 连续数据搬运(DataCopyL1ToUB) | 提供基础的数据搬运能力，数据在传输过程中保持原始格式和内容不变，支持连续数据搬运。 |
| L1 Buffer->UB | 高维切分数据搬运(DataCopyL1ToUB) | 提供基础的数据搬运能力，数据在传输过程中保持原始格式和内容不变，支持连续和非连续的数据搬运。 |

## DataCopy（UBToL1连续数据搬运）

DataCopy能够实现UB到L1 Buffer的连续数据搬运，数据搬运时格式和内容保持不变。

具体支持的数据通路为（以[逻辑位置TPosition](../../basic_api/aux_data_structures/TPosition.md)表示）：

- UB -> L1 Buffer
    - UB -> C1

搬运的数据用于[矩阵计算](../cube_compute_ISASI/cube_compute_ISASI.md)，接口具体介绍请参考：[DataCopy（UBToL1连续数据搬运）](../cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_continuous.md)。

src和dst分别为源操作数和目的操作数；count为连续搬运的元素个数。

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## DataCopy（UBToL1高维切分数据搬运）

DataCopy能够实现UB到L1 Buffer的连续数据搬运和非连续数据搬运，数据搬运时格式和内容保持不变。

高维切分是指能够通过配置数据块个数、单个数据块长度、地址偏移等搬运参数实现非连续搬运。

具体支持的数据通路为（以[逻辑位置TPosition](../../basic_api/aux_data_structures/TPosition.md)表示）：

- UB -> L1 Buffer
    - UB -> C1

搬运的数据用于[矩阵计算](../cube_compute_ISASI/cube_compute_ISASI.md)，接口具体介绍请参考：[DataCopy（UBToL1高维切分数据搬运）](../cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_highdim_split.md)。

src和dst分别为源操作数和目的操作数；repeatParams为搬运参数。

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```

## DataCopy（UBToL1随路转换-ND2NZ搬运）

该接口主要实现将数据从UB搬运至L1 Buffer，并支持在数据搬运时进行[ND](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md)到[NZ](../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md)格式的转换。

具体支持的数据通路为（以[逻辑位置TPosition](../../basic_api/aux_data_structures/TPosition.md)表示）：

- UB -> L1 Buffer
    - UB -> C1

搬运的数据用于[矩阵计算](../cube_compute_ISASI/cube_compute_ISASI.md)，接口具体介绍请参考：[DataCopy（UBToL1随路转换-ND2NZ搬运）](../cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_ND2NZ.md)。

src和dst分别为源操作数和目的操作数；intriParams为ND2NZ搬运配置参数。

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const Nd2NzParams& intriParams)
```

## DataCopyPad（UBToL1非对齐数据搬运）

该接口主要实现将数据从UB搬运至L1 Buffer，并支持在数据搬运时提供非对齐数据搬运功能，可以根据开发者的需要自行填充数据。

具体支持的数据通路为（以[逻辑位置TPosition](../../basic_api/aux_data_structures/TPosition.md)表示）：

- UB -> L1 Buffer
    - UB -> C1

搬运的数据用于[矩阵计算](../cube_compute_ISASI/cube_compute_ISASI.md)，接口具体介绍请参考：[DataCopyPad（UBToL1非对齐数据搬运）](../cube_compute_ISASI/cube_compute_load/DataCopyPad_UBToL1.md)。

src和dst分别为源操作数和目的操作数；dataCopyParams和nd2nzParams为搬运参数。

```cpp
template <typename T>
__aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams, const Nd2NzParams& nd2nzParams)
```

<!-- npu="310p" id1 -->
## DataCopy（L0CToUB随路量化激活搬运）

该接口主要实现将数据从L0C Buffer搬运至UB，并支持多种随路能力的组合，接口内包含了设置寄存器与数据搬运能力。

具体支持的数据通路为（以[逻辑位置TPosition](../../basic_api/aux_data_structures/TPosition.md)表示）：

- L0C Buffer -> UB
    - CO1 -> UB

搬运的数据为[矩阵计算](../cube_compute_ISASI/cube_compute_ISASI.md)的结果，接口具体介绍请参考：[DataCopy（L0C到UB数据搬运）](../cube_compute_ISASI/cube_compute_store/DataCopy_L0CToUB.md)。

该接口仅Atlas 推理系列产品AI Core支持。

src和dst分别为源操作数和目的操作数；intriParams和enhancedParams为搬运参数。

- 源操作数和目的操作数类型一致

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
    ```

- 源操作数和目的操作数类型不一致

    ```cpp
    template <typename T, typename U>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
    ```
<!-- end id1 -->

## Fixpipe（L0CToUB随路量化激活搬运）

该接口主要实现将数据从L0C Buffer搬运至UB，并支持多种随路能力的组合，接口内包含了设置寄存器与数据搬运能力。

具体支持的数据通路为（以[逻辑位置TPosition](../../basic_api/aux_data_structures/TPosition.md)表示）：

- L0C Buffer -> UB
    - CO1 -> UB

搬运的数据为[矩阵计算](../cube_compute_ISASI/cube_compute_ISASI.md)的结果，以Ascend 950PR/Ascend 950DT为例，接口示例如下：

注意，不同产品型号的接口原型可能不同，具体介绍请参考：[Fixpipe（L0C到UB数据搬运）](../cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToUB.md)。

src和dst分别为源操作数和目的操作数；intriParams为搬运参数，cbufWorkspace为开启tensor量化时所需的量化参数。

- 不开启随路[tensor量化](../cube_compute_ISASI/cube_store_key_features/accompanying_quantization.md)功能：

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

- 开启随路[tensor量化](../cube_compute_ISASI/cube_store_key_features/accompanying_quantization.md)功能：

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint64_t>& cbufWorkspace, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

<!-- npu="950" id2 -->
## DataCopyL1ToUB（L1ToUB连续数据搬运）

DataCopyL1ToUB能够实现L1 Buffer到UB的连续数据搬运，数据搬运时格式和内容保持不变。

具体支持的数据通路为（以[逻辑位置TPosition](../../basic_api/aux_data_structures/TPosition.md)表示）：

- L1 Buffer -> UB
    - C1 -> UB

搬运的数据用于[矩阵计算](../cube_compute_ISASI/cube_compute_ISASI.md)，接口具体介绍请参考：[DataCopyL1ToUB（L1到UB数据搬运）](../cube_compute_ISASI/cube_compute_store/DataCopyL1ToUB.md)。

该接口仅Ascend 950PR/Ascend 950DT支持。

src和dst分别为源操作数和目的操作数；count为连续搬运的元素个数。

```cpp
template <typename T, uint8_t subBlockId = 0>
__aicore__ inline void DataCopyL1ToUB(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```
<!-- end id2 -->

<!-- npu="950" id3 -->
## DataCopyL1ToUB（L1ToUB高维切分数据搬运）

DataCopyL1ToUB能够实现L1 Buffer到UB的连续数据搬运和非连续数据搬运，数据搬运时格式和内容保持不变。

高维切分是指能够通过配置数据块个数、单个数据块长度、地址偏移等搬运参数实现非连续搬运。

具体支持的数据通路为（以[逻辑位置TPosition](../../basic_api/aux_data_structures/TPosition.md)表示）：

- L1 Buffer -> UB
    - C1 -> UB

搬运的数据用于[矩阵计算](../cube_compute_ISASI/cube_compute_ISASI.md)，接口具体介绍请参考：[DataCopyL1ToUB（L1到UB数据搬运）](../cube_compute_ISASI/cube_compute_store/DataCopyL1ToUB.md)。

该接口仅Ascend 950PR/Ascend 950DT支持。

src和dst分别为源操作数和目的操作数；repeatParams为搬运参数。

```cpp
template <typename T, uint8_t subBlockId = 0>
__aicore__ inline void DataCopyL1ToUB(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```
<!-- end id3 -->
