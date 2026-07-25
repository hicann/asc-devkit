# DataCopyL1ToUB（L1到UB数据搬运）

## 产品支持情况

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
<!-- npu="x90" id8 -->
- Kirin X90：不支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：不支持
<!-- end id9 -->

## 功能说明

提供从L1 Buffer搬运数据到Unified Buffer的基础能力，数据在传输过程中保持原始格式和内容不变，支持连续和非连续的数据搬运。

## 函数原型

```cpp
// 连续搬运
template <typename T, uint8_t subBlockId = 0>
__aicore__ inline void DataCopyL1ToUB(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)

// 同时支持非连续搬运和连续搬运
template <typename T, uint8_t subBlockId = 0>
__aicore__ inline void DataCopyL1ToUB(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| T | 操作数的数据类型。 |
| subBlockId | AI Core上的Vector核ID。根据输入的Vector核ID，将数据搬运至对应核上的UB地址。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输出 | 目的操作数，类型为LocalTensor。LocalTensor的起始地址要求32字节对齐。 |
| src | 输入 | 源操作数，类型为LocalTensor。LocalTensor的起始地址要求32字节对齐。 |
| repeatParams | 输入 | 搬运参数，[DataCopyParams](#参数说明)类型。通过该参数可配置搬运的数据块大小、个数、间隔等信息，同时支持非连续和连续搬运。具体定义请参考`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_data_copy.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。 |
| count | 输入 | 参与搬运的元素个数。<br>**注：count * sizeof(T)需要32字节对齐，若未对齐，搬运量会向下取整到32字节对齐。** |

**表3** DataCopyParams结构体参数定义

| 参数名 | 描述 |
| :--- | :--- |
| blockCount | 待搬运的连续传输数据块个数。uint16_t类型，取值范围：blockCount∈[1, 4095]。 |
| blockLen | 待搬运的每个连续传输数据块的长度，单位为DataBlock（32字节）。uint16_t类型，取值范围：blockLen∈[1, 65535]。 |
| srcGap | 源操作数相邻连续数据块的间隔（前面一个数据块的尾与后面一个数据块的头的间隔），单位为DataBlock（32字节）。uint16_t类型，srcGap不要超出该数据类型的取值范围。 |
| dstGap | 目的操作数相邻连续数据块的间隔（前面一个数据块的尾与后面一个数据块的头的间隔），单位为DataBlock（32字节）。uint16_t类型，dstGap不要超出该数据类型的取值范围。 |

## 数据类型

支持的数据类型为b8、b16、b32、b64。

## 返回值说明

无

## 约束说明

- 该接口仅适用于Mix算子场景，且仅支持Cube核数与Vector核数比例为**1:2**的配置。在该场景下，subBlockId仅支持取值**0**或**1**。

- 如果需要执行多个DataCopyL1ToUB指令，且DataCopyL1ToUB的目的地址存在重叠，需要通过调用[PipeBarrier\(ISASI\)](../../同步控制/核内同步/PipeBarrier(ISASI).md)来插入同步指令，保证多个DataCopyL1ToUB指令的串行化，防止出现异常数据。

## 调用示例

```cpp
// srcLocal、dstLocal为half类型的LocalTensor
// 使用传入count参数的搬运接口，完成连续搬运
AscendC::DataCopyL1ToUB(dstLocal, srcLocal, 512);
// 使用传入DataCopyParams参数的搬运接口，支持连续和非连续搬运
DataCopyParams intriParams;
intriParams.blockCount = 1; // 连续数据块个数为1
intriParams.blockLen = 512 * sizeof(half) / 32; // 连续数据块长度，单位为DataBlock，此处长度为512个half元素
intriParams.srcGap = 0; // 源操作数做连续搬运
intriParams.dstGap = 0; // 目的操作数连续排布
AscendC::DataCopyL1ToUB(dstLocal, srcLocal, intriParams);
```
