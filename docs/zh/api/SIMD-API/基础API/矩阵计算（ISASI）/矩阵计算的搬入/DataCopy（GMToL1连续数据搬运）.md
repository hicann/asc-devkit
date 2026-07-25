# DataCopy（GMToL1连续数据搬运）<a id="ZH-CN_TOPIC_0000002569070903"></a>

## 产品支持情况<a id="zh-cn_topic_0000002535739034_section796754519912"></a>

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
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
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

## 功能说明<a id="zh-cn_topic_0000002535739034_section106841136114319"></a>

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

该接口能够将矩阵从Global Memory连续搬运至L1 Buffer（TPosition为A1/B1），数据搬运时格式和内容保持不变。

## 函数原型<a id="zh-cn_topic_0000002535739034_section82039854412"></a>

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const uint32_t count)
```

## 参数说明<a id="zh-cn_topic_0000002535739034_section16128134420472"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
| ---------- | ---------- |
| T | 源操作数或者目的操作数的数据类型。支持的数据类型请参考[数据类型](#zh-cn_topic_0000002535739034_section4219135304818)。 |

**表2** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，类型为LocalTensor，存储位置为L1 Buffer（TPosition为A1/B1），目的地址需要32字节对齐。 |
| src | 输入 | 源操作数，类型为GlobalTensor，存储位置为Global Memory，源地址需要1字节对齐。 |
| count | 输入 | 参与搬运的元素个数。<br>**注：count * sizeof(T)需要32字节对齐，若未对齐，搬运量会向下取整到32字节对齐。** |

以half数据类型为例，源操作数的shape为1 * 128。当count = 128时，[图1](#zh-cn_topic_0000002535739034_fig79455329161)将源操作数中128个元素连续搬运至目的操作数。

**图1** 连续搬运示意图<a id="zh-cn_topic_0000002535739034_fig79455329161"></a>

![](../../../../figures/continuous_data_copy_diagram.png)

## 数据类型<a id="zh-cn_topic_0000002535739034_section4219135304818"></a>

源矩阵和目的矩阵支持的数据类型保持一致。

<!-- npu="950" id10 -->
针对Ascend 950PR/Ascend 950DT，支持数据类型为：b8、b16、b32、b64。
<!-- end id10 -->

<!-- npu="A3" id11 -->
针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持数据类型为：b8、b16、b32、b64。
<!-- end id11 -->

<!-- npu="910b" id12 -->
针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持数据类型为：b8、b16、b32、b64。
<!-- end id12 -->

<!-- npu="310p" id13 -->
针对Atlas 推理系列产品AI Core，支持数据类型为：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t、half、float、double。
<!-- end id13 -->

<!-- npu="910" id14 -->
针对Atlas 训练系列产品，支持数据类型为：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t、half、float、double。
<!-- end id14 -->

<!-- npu="x90" id15 -->
针对Kirin X90，支持数据类型为：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t、half、float、double。
<!-- end id15 -->

<!-- npu="9030" id16 -->
针对Kirin 9030，支持数据类型为：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t、half、float、double。
<!-- end id16 -->

## 返回值说明

无

## 约束说明<a id="zh-cn_topic_0000002535739034_section2045914466492"></a>

- 位于Global Memory的源地址必须1字节对齐，位于L1 Buffer的目的地址必须32字节对齐。
- 调用连续搬运接口时，count * sizeof(T)需要32字节对齐，若未对齐，则搬运量会向下取整到32字节对齐。
- 如果需要执行多个DataCopy指令，且DataCopy的目的地址存在重叠，需要通过调用[PipeBarrier\(ISASI\)](../../同步控制/核内同步/PipeBarrier(ISASI).md)来插入同步指令，保证多个DataCopy指令的串行化，防止出现异常数据。如下图左侧示意图，执行两个DataCopy指令，搬运的目的GM地址存在重叠，两条搬运指令之间需要通过调用PipeBarrier<PIPE\_MTE3\>\(\)添加MTE3搬出流水的同步；如下图右侧示意图所示，搬运的目的地址Unified Buffer存在重叠，两条搬运指令之间需要调用PipeBarrier<PIPE\_MTE2\>\(\)添加MTE2搬入流水的同步。

    ![](../../../../figures/绘图4.png)

<!-- npu="A3,910b" id17 -->
- 针对如下产品型号：

    <!-- npu="A3" id18 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品；
    <!-- end id18 -->

    <!-- npu="910b" id19 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品；
    <!-- end id19 -->

    在跨卡通信算子开发场景，DataCopy类接口支持跨卡数据搬运，仅支持HCCS物理链路，不支持其他通路；开发者开发过程中，需要关注涉及卡间通信的物理通路，可通过npu-smi info -t topo命令查询HCCS物理链路。
<!-- end id17 -->

## 调用示例<a id="zh-cn_topic_0000002535739034_section088124295117"></a>

以[图1 连续搬运示意图](#zh-cn_topic_0000002535739034_fig79455329161)所示场景为例：

```cpp
constexpr uint32_t copyCount = 128;
// 源操作数：GM上连续存放的128个half。
AscendC::GlobalTensor<half> srcGm;
srcGm.SetGlobalBuffer((__gm__ half *)src, copyCount);
// 目的操作数：L1 Buffer。
AscendC::LocalTensor<half> dstLocal(AscendC::TPosition::A1, 0, copyCount);
// count = 128，表示搬运128个half元素，实际搬运字节数256B满足32B对齐约束。
AscendC::DataCopy(dstLocal, srcGm, copyCount);
```
