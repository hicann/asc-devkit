# DataCopy（L1ToBiasTable Buffer数据搬运）<a id="ZH-CN_TOPIC_0000002538071240"></a>

## 产品支持情况<a id="zh-cn_topic_0000002566886667_section796754519912"></a>

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

## 功能说明<a id="zh-cn_topic_0000002566886667_section106841136114319"></a>

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

DataCopy数据搬运支持将矩阵计算用到的Bias参数从L1 Buffer移动到BiasTable Buffer。

<!-- npu="A3,910b" id10 -->
针对Atlas A2 训练系列产品/Atlas A2 推理系列产品和Atlas A3 训练系列产品/Atlas A3 推理系列产品，BiasTable Buffer总大小为1KB。
<!-- end id10 -->
<!-- npu="950" id11 -->
针对Ascend 950PR/Ascend 950DT，BiasTable Buffer总大小为4KB。
<!-- end id11 -->

## 函数原型<a id="zh-cn_topic_0000002566886667_section82039854412"></a>

- 连续搬运场景。

  ```cpp
  template <typename T>
  __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
  ```
- 连续或非连续搬运场景（高维切分）。

  ```cpp
  template <typename T>
  __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
  ```
- 连续或非连续搬运场景，支持源操作数和目的操作数数据类型不一致。

  ```cpp
  template <typename T, typename U>
  __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& repeatParams)
  ```

## 参数说明<a id="zh-cn_topic_0000002566886667_section16128134420472"></a>

**表1** 模板参数说明


| 参数名称 | 输入/输出 | 含义                                                                                                 |
| -------- | --------- | ---------------------------------------------------------------------------------------------------- |
| T、U     | 输入      | 操作数的数据类型。具体支持情况请参考[数据类型](#zh-cn_topic_0000002566886667_section4219135304818)。 |

**表2** 参数说明


| 参数名       | 输入/输出 | 含义                                                                                                                                                                                                                                        |
| ------------ | --------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| dst          | 输出      | 目的操作数，类型为LocalTensor。存储位置为BiasTable Buffer（TPosition为C2），目的地址需要64字节对齐。                                                                                                                                        |
| src          | 输入      | 源操作数，类型为LocalTensor，存储位置为L1 Buffer（TPosition为C1），源地址需要32字节对齐。                                                                                                                                                   |
| count        | 输入      | 参与搬运的元素个数。<br>**注：count * sizeof(T)需要32字节对齐，若未对齐，搬运量会向下取整到32字节对齐。**                                                                                                                                           |
| repeatParams | 输入      | 搬运参数，DataCopyParams类型。通过该参数可配置搬运的数据块大小、个数、间隔等信息，同时支持非连续和连续搬运。<br>上述结构体参数定义请参考`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_data_copy.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。参数说明见[表3](#zh-cn_topic_0000002566886667_table9182515919)。 |

**表3** DataCopyParams结构体参数定义<a id="zh-cn_topic_0000002566886667_table9182515919"></a>


| 参数名称   | 含义                                                                                                                                                                                                                                                                                                     |
| ---------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| blockCount | 待搬运的连续传输数据块个数。uint16_t类型，取值范围：blockCount∈[0, 4095]。<br>**注：blockCount = 0表示不执行搬运，该接口将被视为NOP（空操作）**。                                                                                                                                                       |
| blockLen   | 待搬运的每个连续传输数据块长度，uint16_t类型，取值范围：blockLen∈[0, 65535]。<br>不同产品型号的blockLen单位不同，具体请参考[约束说明](#zh-cn_topic_0000002566886667_section2045914466492)。<br>**注：blockLen = 0表示不执行搬运，该接口将被视为NOP（空操作）**。                                                                                                                                                      |
| srcGap     | 源操作数相邻连续数据块的间隔（前面一个数据块的尾与后面数据块的头的间隔），单位为DataBlock（32字节）。uint16_t类型，srcGap∈[0, 2^16-1]。<br>&nbsp;&nbsp;&bull; blockCount = 1时，srcGap无意义，设置为0即可。<br>&nbsp;&nbsp;&bull; blockCount ≠ 1时，当srcGap=0时，表示重复搬出源操作数的第一个数据块。 |
| dstGap     | 目的操作数相邻连续数据块间的间隔（前面一个数据块的尾与后面数据块的头的间隔），uint16_t类型，dstGap∈[0, 2^16-1]。<br>不同产品型号的dstGap单位不同，具体请参考[约束说明](#zh-cn_topic_0000002566886667_section2045914466492)。<br>&nbsp;&nbsp;&bull; blockCount = 1时，dstGap无意义，设置为0即可。<br>&nbsp;&nbsp;&bull; blockCount ≠ 1时，当dstGap=0时，表示目的矩阵中搬入的每个数据块都会覆盖第一个数据块。 |

## 数据类型<a id="zh-cn_topic_0000002566886667_section4219135304818"></a>

源操作数和目的操作数支持数据类型不一样，具体请参考如下：

<!-- npu="950" id19 -->
针对Ascend 950PR/Ascend 950DT，src支持的数据为：half、bfloat16_t、int32_t、float；dst支持的数据为：int32_t、float。
<!-- end id19 -->
<!-- npu="A3" id20 -->
针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，src支持的数据为：half、int32_t、float；dst支持的数据为：int32_t、float。
<!-- end id20 -->
<!-- npu="910b" id12 -->
针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，src支持的数据为：half、int32_t、float；dst支持的数据为：int32_t、float。
<!-- end id12 -->

## 返回值说明

无

## 约束说明<a id="zh-cn_topic_0000002566886667_section2045914466492"></a>

- dst物理内存为BiasTable Buffer，起始地址要求64字节对齐；src物理内存为L1 Buffer，起始地址要求32字节对齐。
- blockCount取值范围：blockCount∈[0, 4095]，blockCount = 0表示指令不执行，该接口将被视为NOP（空操作）。
- srcGap∈[0, 2^16-1]，dstGap∈[0, 2^16-1]。
- 如果BiasTable Buffer中的目标dst地址溢出，将引发异常。
- blockLen对不同型号单位不一样，具体请参考如下：
  <!-- npu="950" id13 -->
  - 针对Ascend 950PR/Ascend 950DT，单位为32字节，blockLen必须为偶数。
  <!-- end id13 -->
  <!-- npu="A3" id14 -->
  - 针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，单位为64字节。
  <!-- end id14 -->
  <!-- npu="910b" id15 -->
  - 针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，单位为64字节。
  <!-- end id15 -->
- dstGap对不同型号单位不一样，具体请参考如下：
  <!-- npu="950" id16 -->
  - 针对Ascend 950PR/Ascend 950DT，单位为32字节。
  <!-- end id16 -->
  <!-- npu="A3" id17 -->
  - 针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，单位为64字节。
  <!-- end id17 -->
  <!-- npu="910b" id18 -->
  - 针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，单位为64字节。
  <!-- end id18 -->

## 调用示例<a id="zh-cn_topic_0000002566886667_section088124295117"></a>

L1 Buffer->BiasTable Buffer，搬运Bias参数。

```cpp
// c1Addr = 128 * 1024、c2Addr = 0；__NPU_ARCH__ == 3510时c12c2Params = {1, 32, 0, 0}，否则为 {1, 16, 0, 0}。
AscendC::LocalTensor<outputType> bias1Local(AscendC::TPosition::C1, c1Addr, biasSizeAlign);
AscendC::LocalTensor<outputType> bias2Local(AscendC::TPosition::C2, c2Addr, biasSizeAlign);

if constexpr (SCENARIO_NUM == 4) {
    SplitBias(bias1Local, bias2Local);
}

__aicore__ inline void SplitBias(AscendC::LocalTensor<outputType> bias1Local,
                                AscendC::LocalTensor<outputType> bias2Local)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    AscendC::DataCopyParams c12c2Params =
        {1, static_cast<uint16_t>(biasSizeAlign * sizeof(outputType) / 32), 0, 0};
#else
    AscendC::DataCopyParams c12c2Params =
        {1, static_cast<uint16_t>(biasSizeAlign * sizeof(outputType) / 64), 0, 0};
#endif
    AscendC::DataCopy(bias2Local, bias1Local, c12c2Params);
}
```

完整示例请参考[data_copy_gm2l1样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_gm2l1)中场景4。
