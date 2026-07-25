# SetFixPipeConfig<a name="ZH-CN_TOPIC_0000002538231208"></a>

## 产品支持情况<a name="zh-cn_topic_0000002547729135_section18204144912492"></a>

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
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

## 功能说明<a name="zh-cn_topic_0000002547729135_section618mcpsimp"></a>

DataCopy数据搬运（[L0C Buffer->GM](../矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)、[L0C Buffer->L1 Buffer](../矩阵计算的搬出/Fixpipe（L0C到L1数据搬运）.md)）过程中进行随路量化时，通过调用该接口设置量化的参数。

## 函数原型<a name="zh-cn_topic_0000002547729135_section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void SetFixPipeConfig(const LocalTensor<T>& reluPre, const LocalTensor<T>& quantPre, bool isUnitFlag = false)

template <typename T, bool setRelu = false>
__aicore__ inline void SetFixPipeConfig(const LocalTensor<T>& preData, bool isUnitFlag = false)
```

## 参数说明<a name="zh-cn_topic_0000002547729135_section622mcpsimp"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
| ---------- | ---------- |
| T | 操作数的数据类型。 |
| setRelu | 针对设置一个tensor的情况，setRelu当前仅支持设置为false，即只支持设置随路quantPre系数。 |

**表2** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| reluPre | 输入 | 源操作数，ReLU操作时参与计算的tensor，类型为LocalTensor，支持的物理内存为Fixpipe Buffer（TPosition为C2PIPE2GM），为后续的功能扩展做保留，传入一个空LocalTensor即可。 |
| quantPre | 输入 | 源操作数，quant tensor，量化操作时参与计算的tensor，类型为LocalTensor，支持的物理内存为Fixpipe Buffer（TPosition为C2PIPE2GM）。 |
| isUnitFlag | 输入 | unitFlag清空标志位，类型为bool。如果设为true，L0C Buffer中所有内存块的单元标志位都会设为0。旨在消除之前操作中unitFlag产生的多余影响。 |
| preData | 输入 | 支持设置一个tensor，通过开关控制是ReLU tensor还是Quant tensor，支持的物理内存为Fixpipe Buffer（TPosition为C2PIPE2GM）。当前仅支持传入quant tensor。对于单独设置unitFlag清空标志位isUnitFlag的场景，支持的物理地址为L0C Buffer（TPosition设置为CO1）。 |

## 数据类型

支持的数据类型为：uint64_t。

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="zh-cn_topic_0000002547729135_section633mcpsimp"></a>

quantPre和reluPre必须是Fixpipe Buffer上的tensor。

## 调用示例<a name="section642mcpsimp"></a>

完整示例可参考[data_copy_l0c2gm](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_l0c2gm)。

```cpp
__aicore__ inline void SetFPC(const LocalTensor <int32_t>& reluPreTensor, const LocalTensor <int32_t>& quantPreTensor)
{
 
    AscendC::LocalTensor<uint64_t> workA1 = inQueueDeqA1.AllocTensor<uint64_t>();
    uint16_t deqSize = 128; // deq tensor的size
    AscendC::DataCopy(workA1, deqGlobal, deqSize); // deqGlobal为量化系数的gm地址
    AscendC::LocalTensor<uint64_t> deqFB = inQueueDeqFB.AllocTensor<uint64_t>(); // deq tensor在Fix上的地址
    uint16_t fbufBurstLen = deqSize / 128;  // l1->fix, burst_len unit is 128Bytes
    AscendC::DataCopyParams dataCopyParams(1, fbufBurstLen, 0, 0);
    AscendC::DataCopy(deqFB, workA1, dataCopyParams); // 通过DataCopy搬入C2PIPE2GM。
    AscendC::SetFixPipeConfig(deqFB); // 设置量化tensor
    AscendC::PipeBarrier<PIPE_FIX>();
}
```
