# DataCopy（L1ToFixpipe-Buffer数据搬运）<a id="ZH-CN_TOPIC_0000002569070951"></a>

## 产品支持情况<a id="zh-cn_topic_0000002538001510_section18204144912492"></a>

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

## 功能说明<a id="zh-cn_topic_0000002538001510_section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

DataCopy数据搬运支持将随路量化参数从L1 Buffer移动到Fixpipe Buffer。Fixpipe Buffer包含两种参数：pre_stage的量化参数，这些参数各自独立存储，并拥有独立的地址空间。量化参数和ReLU参数通过dst地址的高16位（dst[31:16]）进行区分，具体如下：

**图1** Fixpipe Buffer图示

![](../../../../figures/fixpipe_buffer_02.png)

## 函数原型<a id="section_function_prototype"></a>

```cpp
// 连续搬运场景
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)

// 连续或非连续搬运场景（高维切分）
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```

## 参数说明<a id="section_param_desc"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
| ---------- | ---------- |
| T | 操作数的数据类型。对于pre_stage的量化参数的搬运需要设置为uint64_t。支持的数据类型请参考[数据类型](#section_data_type)。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，类型为LocalTensor。存储位置为Fixpipe Buffer，目的地址需要128字节对齐。 |
| src | 输入 | 源操作数，类型为LocalTensor，存储位置为L1 Buffer，源地址需要32字节对齐。 |
| repeatParams | 输入 | 搬运参数，DataCopyParams类型。通过该参数可配置搬运的数据块大小、个数、间隔等信息，同时支持非连续和连续搬运。<br>上述结构体参数定义请参考`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_data_copy.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。 |
| count | 输入 | 参与搬运的元素个数。<br>**注：count * sizeof(T)需要32字节对齐，若未对齐，搬运量会向下取整到32字节对齐。** |

**表3** DataCopyParams结构体参数定义

| 参数名 | 含义 |
| ---------- | ---------- |
| blockCount | 待搬运的连续传输数据块个数。uint16_t类型，取值范围：blockCount∈[1, 4095]。<br>**注：blockCount = 0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| blockLen | 待搬运的每个连续传输数据块长度，dst位于Fixpipe Buffer时，单位为128字节。uint16_t类型，取值范围：blockLen∈[1, 65535]。<br>**注：blockLen = 0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| srcGap | 在L1 Buffer -> Fixpipe Buffer场景中，srcGap特指源操作数相邻连续数据块的间隔（前面一个数据块的头与后面数据块的头的间隔），单位为DataBlock（32字节）。uint16_t类型，srcGap∈[0, 2^16-1]。<br>&nbsp;&nbsp;&bull; blockCount = 1时，srcGap无意义，设置为0即可。<br>&nbsp;&nbsp;&bull; blockCount ≠ 1时，当srcGap=0时，表示重复搬出源操作数的第一个数据块。 |
| dstGap | 在L1 Buffer -> Fixpipe Buffer场景中，dstGap特指目的操作数相邻连续数据块的间隔（前面一个数据块的头与后面数据块的头的间隔），单位为DataBlock（128字节）。uint16_t类型，dstGap不要超出该数据类型的取值范围，dstGap∈[1, 2^16-1]。<br>&nbsp;&nbsp;&bull; blockCount = 1时，dstGap无意义，设置为0即可。<br>&nbsp;&nbsp;&bull; blockCount ≠ 1时，当dstGap=0时，表示目的矩阵中搬入的每个数据块都会覆盖第一个数据块。 |

## 数据类型<a id="section_data_type"></a>

src数据类型支持uint64_t，dst数据类型支持uint64_t。

## 返回值说明

无

## 约束说明<a id="section_constraint"></a>

<!-- npu="950" id11 -->
- 针对Ascend 950PR/Ascend 950DT，blockLen参数单位为64B。
<!-- end id11 -->
- dst物理内存为Fixpipe Buffer起始地址要求128字节对齐，src物理内存为L1 Buffer，起始地址要求32字节对齐。
- blockCount取值范围：blockCount∈[1, 4095]，设置为0表示不执行，此指令将被视为NOP并报告警告。
- srcGap∈[0, 2^16-1]，dstGap∈[1, 2^16-1]。
- 如果读写地址大于对应的mem_block，地址将被绕行并引发警告。
- 如果Fixpipe Buffer中的目标dst地址溢出，将引发异常。

## 调用示例<a id="section_call_example"></a>

L1 Buffer(C1)->Fixpipe Buffer(C2PIPE2GM)，搬运Vector量化参数。

```cpp
// c1Addr = 0、fbAddr = 0、N = 256、n = 256；burstLen = CeilAlign(256 * sizeof(uint64_t), 128) / 128 = 16，dataCopyParams = {1, 16, 0, 0}。
AscendC::LocalTensor<uint64_t> quantAlphaTensor(AscendC::TPosition::C1, c1Addr, N);

if constexpr (scenarioNum == 2 || scenarioNum == 4 || scenarioNum == 6) {
    CopyQuantAlphaGmToL1(quantAlphaTensor);
}

// vector quant mode
if constexpr (scenarioNum == 2) {
    intriParams.quantPre = QuantMode_t::VDEQF16;
} else if constexpr (scenarioNum == 4) {
    intriParams.quantPre = QuantMode_t::VQF322B8_PRE;
} else {
    intriParams.quantPre = QuantMode_t::VREQ8;
}
AscendC::LocalTensor<uint64_t> fbTensor(AscendC::TPosition::C2PIPE2GM, fbAddr, N);
uint16_t burstLen = CeilAlign(n * sizeof(uint64_t), 128) / 128;
AscendC::DataCopyParams dataCopyParams(1, burstLen, 0, 0);
AscendC::DataCopy(fbTensor, quantAlphaTensor, dataCopyParams);
AscendC::SetFixPipeConfig(fbTensor);
```

完整示例请参考[DataCopy（L1->Fixpipe Buffer数据搬运）](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_l0c2gm)中场景2、4、6。
