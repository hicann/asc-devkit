# DataCopyPad（GMToL1非对齐数据搬运）

## 产品支持情况<a name="section1550532418810"></a>

<a name="table38301303189"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

该接口提供从Global Memory到L1 Buffer的数据非对齐搬运功能，可以根据开发者的需要自行填充数据。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline void DataCopyPad(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams, const DataCopyPadExtParams<T>& padParams)
```

## 参数说明<a name="section622mcpsimp"></a>

**表 1** 模板参数说明

<a name="table4835205712588"></a>

| 参数名 | 描述 |
| --- | --- |
| T | 操作数以及paddingValue（待填充数据值）的数据类型。 |
| mode | 配置数据搬运模式。PaddingMode类型，定义如下：<br><pre>enum class PaddingMode : uint8_t {<br>    Normal = 0,  // 默认模式，与原有数据搬运格式保持一致，每次数据搬运都会补齐至32字节对齐<br>    Compact,     // 紧凑模式，允许单次搬运不对齐，统一在整块数据末尾补齐至32字节对齐<br>};</pre> |

**表 2** 接口参数说明

<a name="zh-cn_topic_0235751031_table33761356"></a>

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数，类型为LocalTensor。<br>LocalTensor的起始地址需要保证32字节对齐。 |
| src | 输入 | 源操作数，类型为GlobalTensor。<br>GlobalTensor的起始地址需要保证1字节对齐。 |
| dataCopyParams | 输入 | 搬运参数。<br>&nbsp;&nbsp;&bull; DataCopyExtParams类型，具体参数说明请参考[表4](#table10572141063919)。 |
| padParams | 输入 | 从Global Memory搬运数据至Local Memory时，可以根据开发者需要，在搬运数据左边或右边填充数据。padParams是用于控制数据填充过程的参数。<br>&nbsp;&nbsp;&bull; DataCopyPadExtParams类型，具体参数请参考[表6](#table844881954715)。 |

**表 3** DataCopyExtParams结构体参数定义

<a name="table10572141063919"></a>

| 参数名称 | 含义 |
| --- | --- |
| blockCount | 指定该指令包含的连续传输数据块个数，数据类型为uint16_t，取值范围：blockCount∈[1, 4095]。 |
| blockLen | 指定该指令每个连续传输数据块长度，**该指令支持非对齐搬运**，**每个连续传输数据块长度单位为字节**。数据类型为uint32_t，取值范围：blockLen∈[1, 2097151]，**blockLen必须是sizeof(T)的整数倍**。 |
| srcStride | 源操作数，相邻连续数据块的间隔（前面一个数据块的尾与后面数据块的头的间隔），单位为字节，取值范围为[-blockLen, 2^40-1]。 |
| dstStride | 目的操作数，相邻连续数据块间的间隔（前面一个数据块的尾与后面数据块的头的间隔），单位为dataBlock(32字节)，取值范围为[0, 65535]。 |
| rsv | 保留字段。 |

**表 4** DataCopyPadExtParams<T\>结构体参数定义

<a name="table844881954715"></a>

| 参数名称 | 含义 |
| --- | --- |
| isPad | 是否需要填充用户自定义的数据，取值范围：true，false。<br>&nbsp;&nbsp;&bull; true：填充padding value。<br>&nbsp;&nbsp;&bull; false：表示用户不需要指定填充值，会默认填充随机值。 |
| leftPadding | 连续搬运数据块左侧需要补充的数据范围，单位为元素个数。<br>**leftPadding、rightPadding所占的字节数均不能超过32字节。**<br>**Compact模式下leftPadding、rightPadding均不生效，仅在整块数据末尾统一补齐至32字节对齐。** |
| rightPadding | 连续搬运数据块右侧需要补充的数据范围，单位为元素个数。<br>**leftPadding、rightPadding所占的字节数均不能超过32字节。**<br>**Compact模式下leftPadding、rightPadding均不生效，仅在整块数据末尾统一补齐至32字节对齐。** |
| paddingValue | 左右两侧需要填充的数据值，需要保证在数据占用字节范围内。<br>数据类型和源操作数保持一致，T数据类型。<br>**当数据类型长度为64位时，该参数只能设置为0。** |

下面给出如下场景的配置示例：

- <a name="li1975762118172"></a>搬运模式的配置示例
    - Normal模式

        blockLen为48，每个连续传输数据块包含48字节；srcStride为0，因为源操作数的逻辑位置为GM，srcStride的单位为字节，即源操作数相邻数据块之间紧密排列；dstStride为0，因为目的操作数的逻辑位置为L1 Buffer，dstStride的单位为DataBlock（32字节），目的操作数相邻数据块之间无间隔，注意数据块包含leftPadding/rightPadding数据。

        blockLen + leftPadding + rightPadding满足32字节对齐，isPad为false，左右两侧填充的数据值会默认为随机值，否则为paddingValue。此处示例中，leftPadding为0，rightPadding为16，每个连续传输数据块都会在右侧填充16字节。目的操作数的总长度为192字节。

    - Compact模式

        blockLen为48，每个连续传输数据块包含48字节；srcStride为0，因为源操作数的逻辑位置为GM，srcStride的单位为字节，即源操作数相邻数据块之间紧密排列；dstStride为0，因为目的操作数的逻辑位置为L1 Buffer，dstStride的单位为DataBlock（32字节），目的操作数相邻数据块之间紧密排列，不会填充数据。

        **Compact模式下leftPadding、rightPadding均不生效**，有效数据紧密排列后，统一在整块数据末尾补齐至32字节对齐。此处示例中，blockLen \* blockCount = 48 \* 3 = 144字节，在整块数据末尾补齐16字节满足32字节对齐，目的操作数的总长度为160字节。

**图 1**  Normal模式与Compact模式搬运配置示意图<a id="fig_paddingmode_demo"></a>

![](../../../../figures/paddingMode.png "Normal模式与Compact模式搬运配置示意图")

## 数据类型

支持的数据类型为：bool、int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、complex32、int64_t、uint64_t、double、complex64。

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- leftPadding、rightPadding的字节数均不能超过32字节。

## 调用示例<a name="section177231425115410"></a>

示例场景如[图 1](#fig_paddingmode_demo)所示，该接口仅支持Ascend 950PR/Ascend 950DT。

Normal模式调用示例如下：

```cpp
constexpr uint32_t srcElemCount = 144;
constexpr uint32_t dstElemCount = 192;

// 源操作数：GM上连续存放3个数据块，每块48B，共144个int8_t。
AscendC::GlobalTensor<int8_t> srcGm;
srcGm.SetGlobalBuffer((__gm__ int8_t *)src, srcElemCount);
// 目的操作数：L1 Buffer。Normal模式下每块48B有效数据右侧补16B，3块共192B。
AscendC::LocalTensor<int8_t> dstLocal(AscendC::TPosition::A1, 0, dstElemCount);

AscendC::DataCopyExtParams copyParams;
// 搬运3个连续传输数据块。
copyParams.blockCount = 3;
// 每个连续传输数据块包含48B有效数据。
copyParams.blockLen = 48;
// 源操作数在GM，单位为字节；相邻数据块紧密排列，间隔为0B。
copyParams.srcStride = 0;
// 目的端A1相邻数据块无额外间隔，dstStride的单位为DataBlock（32字节）。
copyParams.dstStride = 0;
// 保留字段，固定填0。
copyParams.rsv = 0;

AscendC::DataCopyPadExtParams<int8_t> padParams;
// padding区域使用默认随机值，不使用paddingValue。
padParams.isPad = false;
// 左侧不填充，GM数据直接从每个目的数据块起始位置写入。
padParams.leftPadding = 0;
// 每块右侧填充16个int8_t，即16B；48B + 16B = 64B，满足32B对齐。
padParams.rightPadding = 16;
// isPad = false 时该值不生效，填0保持字段完整。
padParams.paddingValue = static_cast<int8_t>(0);

AscendC::DataCopyPad<int8_t, AscendC::PaddingMode::Normal>(dstLocal, srcGm, copyParams, padParams);
```

Compact模式调用示例如下：

```cpp
constexpr uint32_t srcElemCount = 144;
constexpr uint32_t dstElemCount = 160;

// 源操作数：GM上连续存放3个数据块，每块48B，共144个int8_t。
AscendC::GlobalTensor<int8_t> srcGm;
srcGm.SetGlobalBuffer((__gm__ int8_t *)src, srcElemCount);

// 目的操作数：L1 Buffer的A1位置。Compact模式下3块有效数据共144B，只在最后右侧补16B。
AscendC::LocalTensor<int8_t> dstLocal(AscendC::TPosition::A1, 0, dstElemCount);

AscendC::DataCopyExtParams copyParams;
// 搬运3个连续传输数据块。
copyParams.blockCount = 3;
// 每个连续传输数据块包含48B有效数据。
copyParams.blockLen = 48;
// 源操作数在GM，单位为字节；相邻数据块紧密排列，间隔为0B。
copyParams.srcStride = 0;
// Compact模式下目的端有效数据紧密排列，不在每块之间插入padding间隔。
copyParams.dstStride = 0;
// 保留字段，固定填0。
copyParams.rsv = 0;

AscendC::DataCopyPadExtParams<int8_t> padParams;
// padding区域使用默认随机值，不使用paddingValue。
padParams.isPad = false;
// isPad = false时该值不生效，填0保持字段完整。
padParams.paddingValue = static_cast<int8_t>(0);

AscendC::DataCopyPad<int8_t, AscendC::PaddingMode::Compact>(dstLocal, srcGm, copyParams, padParams);
```
