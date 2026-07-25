# DataCopy（GM与UB高维切分数据搬运）<a name="ZH-CN_TOPIC_00000023829080211"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

## 功能说明<a name="section474617392321"></a>

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

支持Global Memory与Unified Buffer之间的高维切分数据搬运，数据在传输过程中保持原始格式和内容不变。

高维切分数据搬运可通过配置数据块个数、单个数据块长度、地址偏移等搬运参数，同时支持非连续和连续的数据搬运。

具体支持的数据通路为（以[逻辑位置TPosition](../../辅助数据结构/TPosition.md)表示）：

- Global Memory -> Unified Buffer
    - GM -> VECIN
- Unified Buffer -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id10 -->
    - CO2 -> GM（仅Atlas 推理系列产品AI Core支持）
    <!-- end id10 -->

## 函数原型<a name="section1954364615315"></a>

接口同时支持非连续搬运和连续搬运，对于连续搬运场景，推荐使用[DataCopy\(GM与UB连续数据搬运\)](DataCopy（GM与UB连续数据搬运）.md)。

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

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| T | 操作数的数据类型。支持的数据类型请参考[数据类型](#section4219135304818)。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| dst | 输出 | 目的操作数。<br>&bull;类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor简介.md)时，存储位置为Unified Buffer，目的地址需要32字节对齐。<br>&bull;类型为[GlobalTensor](../../数据结构/GlobalTensor/GlobalTensor简介.md)时，存储位置为Global Memory，目的地址需要按照对应数据类型所占字节数对齐。 |
| src | 输入 | 源操作数。<br>&bull;类型为GlobalTensor时，存储位置为Global Memory，源地址需要按照对应数据类型所占字节数对齐。<br>&bull;类型为LocalTensor时，存储位置为Unified Buffer，源地址需要32字节对齐。 |
| repeatParams | 输入 | 搬运参数，DataCopyParams类型，参数说明请参考[表3](#table_highdim_3)。通过该参数可配置搬运的数据块大小、个数、间隔等信息，同时支持非连续和连续搬运。<br>具体定义请参考`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_data_copy.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。 |

**表3**  DataCopyParams结构体参数定义<a name="table_highdim_3"></a>

| 参数名 | 描述 |
| --- | --- |
| blockCount | 搬运的数据块个数。数据类型为uint16_t，取值范围：blockCount∈[0, 4095]，默认值为1。 |
| blockLen | 搬运的每个数据块长度。数据类型为uint16_t，取值范围：blockLen∈[0, 65535]，单位：DataBlock（32B）。 |
| srcGap | 源操作数相邻数据块之间的间隔（即前一个数据块**结束地址**与后一个数据块**起始地址**的差值），数据类型为uint16_t，取值范围：srcGap∈[0, 65535]，单位：DataBlock（32B）。<br>&bull; blockCount = 1时，srcGap无意义，设置为0即可。 |
| dstGap | 目的操作数相邻数据块之间的间隔（即前一个数据块**结束地址**与后一个数据块**起始地址**的差值），数据类型为uint16_t，取值范围：dstGap∈[0, 65535]，单位：DataBlock（32B）。<br>&bull; blockCount = 1时，dstGap无意义，设置为0即可。 |

下面的样例通过两个示例展示连续搬运和非连续搬运的使用方式。

针对连续搬运场景，推荐使用[DataCopy\(GM与UB连续数据搬运\)](DataCopy（GM与UB连续数据搬运）.md)，以下示例仅展示高维切分数据搬运接口具有连续搬运能力。

- 示例1：连续搬运

    以half数据类型为例，源操作数的shape为1 \* 128。

    以[图1](#zh-cn_topic_0000002535088926_fig4621152141711)为例，将源操作数中128个元素**连续**搬运至目的操作数。DataCopyParams结构体参数配置如下：

    - blockCount = 1，搬运1个数据块，表示连续搬运。
    - blockLen = 8，一个数据块长度为8个DataBlock。
    - srcGap = 0，源操作数相邻数据块之间的间隔为0。
    - dstGap = 0，目的操作数相邻数据块之间的间隔为0。

    **图1**  连续搬运示意图<a name="zh-cn_topic_0000002535088926_fig4621152141711"></a>  
    ![](../../../../figures/continuous_data_copy_diagram.png "连续搬运示意图")

- 示例2：非连续搬运

    以half数据类型为例，源操作数的shape为1 \* 112。

    以[图2](#zh-cn_topic_0000002535088926_fig422819202304)为例，将源操作数中数据**非连续**搬运至目的操作数。DataCopyParams结构体参数配置如下：

    - blockCount = 2，搬运2个数据块。
    - blockLen = 3，一个数据块的长度为3个DataBlock。
    - srcGap = 1，源操作数相邻数据块之间的间隔为1。
    - dstGap = 2，目的操作数相邻数据块之间的间隔为2。

    **图2**  非连续搬运示意图<a name="zh-cn_topic_0000002535088926_fig422819202304"></a>  
    ![](../../../../figures/discontinuous_data_copy_diagram.png "非连续搬运示意图")

## 数据类型<a name="section4219135304818"></a>

源操作数和目的操作数支持的数据类型保持一致，Global Memory -> Unified Buffer和Unified Buffer -> Global Memory两个数据通路对同一产品支持的数据类型相同，具体如下：

<!-- npu="950" id11 -->

- Ascend 950PR/Ascend 950DT，支持的数据类型为：b8、b16、b32、b64。

<!-- end id11 -->

<!-- npu="A3" id12 -->

- Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t、double。

<!-- end id12 -->

<!-- npu="910b" id13 -->

- Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t、double。

<!-- end id13 -->

<!-- npu="310b" id14 -->

- Atlas 200I/500 A2 推理产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t、double。

<!-- end id14 -->

<!-- npu="310p" id15 -->

- Atlas 推理系列产品AI Core，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float、int64_t、uint64_t、double。

<!-- end id15 -->

<!-- npu="310p" id16 -->

- Atlas 推理系列产品Vector Core，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float、int64_t、uint64_t、double。

<!-- end id16 -->

<!-- npu="910" id17 -->

- Atlas 训练系列产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float、int64_t、uint64_t、double。

<!-- end id17 -->

<!-- npu="x90" id18 -->

- Kirin X90，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float、int64_t、uint64_t、double。

<!-- end id18 -->

<!-- npu="9030" id19 -->

- Kirin 9030，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float、int64_t、uint64_t、double。

<!-- end id19 -->

## 返回值说明<a name="section44801012174220"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 位于Global Memory的地址必须按照对应数据类型所占字节数对齐，位于Unified Buffer的地址必须32字节对齐。
<!-- npu="A3,910b,950" id23 -->
- 当DataCopyParams结构体参数blockCount、blockLen任意一个值为0时，该接口将被视为NOP（空操作）。该说明针对如下型号生效：
  <!-- npu="A3" id20 -->
  - Atlas A3 训练系列产品/Atlas A3 推理系列产品
  <!-- end id20 -->
  <!-- npu="910b" id21 -->
  - Atlas A2 训练系列产品/Atlas A2 推理系列产品
  <!-- end id21 -->
  <!-- npu="950" id22 -->
  - Ascend 950PR/Ascend 950DT
  <!-- end id22 -->
<!-- end id23 -->
- DataCopyParams结构体参数的值需在取值范围内：

    **表4**  DataCopyParams结构体参数取值范围

    | 参数名 | 取值范围 |
    | --- | --- |
    | blockCount | [0, 4095] |
    | blockLen | [0, 65535] |
    | srcGap | [0, 65535] |
    | dstGap | [0, 65535] |

- 如果需要执行多个DataCopy指令，且DataCopy的目的地址存在重叠，需要通过调用[PipeBarrier(ISASI)](../../同步控制/核内同步/PipeBarrier(ISASI).md)来插入同步指令，保证多个DataCopy指令的串行化，防止出现异常数据。如下图左侧示意图，执行两个DataCopy指令，搬运的目的Global Memory地址存在重叠，两条搬运指令之间需要通过调用`PipeBarrier<PIPE_MTE3>()`添加MTE3搬出流水的同步；如下图右侧示意图所示，搬运的目的地址Unified Buffer存在重叠，两条搬运指令之间需要调用`PipeBarrier<PIPE_MTE2>()`添加MTE2搬入流水的同步。

    ![](../../../../figures/datacopy_address_overlap_sync_diagram.png)

<!-- npu="910b,A3" id24 -->

- 针对如下产品型号：
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品

    在跨卡通信算子开发场景，DataCopy类接口支持跨卡数据搬运，仅支持HCCS物理链路，不支持其他通路；开发者开发过程中，需要关注涉及卡间通信的物理通路，可通过`npu-smi info -t topo`命令查询HCCS物理链路。

<!-- end id24 -->

## 调用示例<a name="section122101199486"></a>

- Global Memory -> Unified Buffer

    ```cpp
    // srcLocal为half类型的LocalTensor，srcGlobal为half类型的GlobalTensor。
    // 使用传入DataCopyParams参数的搬运接口，支持连续和非连续搬运。
    DataCopyParams intriParams;
    intriParams.blockCount = 1; // 连续数据块个数为1。
    intriParams.blockLen = 512 * sizeof(half) / 32; // 连续数据块长度，单位为DataBlock，此处长度为512个half元素。
    intriParams.srcGap = 0; // 源操作数做连续搬运。
    intriParams.dstGap = 0; // 目的操作数连续排布。
    AscendC::DataCopy(srcLocal, srcGlobal, intriParams);
    ```

    结果示例：

    ```text
    输入数据srcGlobal：[1 2 3 ... 512]
    输出数据srcLocal：[1 2 3 ... 512]
    ```

- Unified Buffer -> Global Memory

    ```cpp
    // dstLocal为half类型的LocalTensor，dstGlobal为half类型的GlobalTensor。
    // 使用传入DataCopyParams参数的搬运接口，支持连续和非连续搬运。
    DataCopyParams intriParams;
    intriParams.blockCount = 1; // 连续数据块个数为1。
    intriParams.blockLen = 512 * sizeof(half) / 32; // 连续数据块长度，单位为DataBlock，此处长度为512个half元素。
    intriParams.srcGap = 0; // 源操作数做连续搬运。
    intriParams.dstGap = 0; // 目的操作数连续排布。
    AscendC::DataCopy(dstGlobal, dstLocal, intriParams);
    ```

    结果示例：

    ```text
    输入数据dstLocal：[1 2 3 ... 512]
    输出数据dstGlobal：[1 2 3 ... 512]
    ```
