# Fill<a name="ZH-CN_TOPIC_0000001834660669"></a>

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

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_mm_intf.h"`。

将特定物理存储位置的LocalTensor初始化为某一具体数值。仅支持L1 Buffer/L0A Buffer/L0B Buffer上的LocalTensor初始化。

如下图所示，将数据空间内最后三行初始化为0，配置InitConstValueParams参数：initValue=0表示初始化值为0，repeatTimes=5表示按照行方向迭代5次，blockNum=3表示每次repeat对列方向的3块32Byte数据块清0，dstGap=5表示相邻repeat之间间隔5个数据块。

**图1** L1 Buffer中Fill示意图<a name="zh-cn_topic_0000002512331632_fig197511640134513"></a>  
![](../../../../figures/Fill_L1.png "L1 Buffer中Fill示意图")

如下图所示，将内存空间内的值全部初始化为0，需要配置InitConstValueParams参数：initValue=0表示初始化值为0，repeatTimes=5表示按照行方向迭代5次，blockNum=2表示每次repeat对列方向的2块512Byte数据块清0，dstGap=3表示相邻repeat之间间隔3个数据块。

**图2** L0A Buffer中Fill示意图<a name="zh-cn_topic_0000002512331632_fig11499125816478"></a>  
![](../../../../figures/Fill_L0A.png)

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T, typename U = PrimT<T>, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void Fill(const LocalTensor<T>& dst, const InitConstValueParams<U>& initConstValueParams)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1** 模板参数说明

| 参数名称 | 描述 |
| ------- | ------ |
| T | dst的数据类型。 |
| U | 初始化值的数据类型。<br>&nbsp;&nbsp;&bull;当dst使用基础数据类型时，U和dst的数据类型T需保持一致，否则编译失败。<br>&nbsp;&nbsp;&bull;当dst使用[TensorTrait](../../辅助数据结构/TensorTrait/TensorTrait.md)类型时，U和dst的数据类型T的LiteType需保持一致，否则编译失败。<br>&nbsp;&nbsp;最后一个模板参数仅用于上述数据类型检查，用户无需关注。 |

**表2** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| --------- | ---------- | ------ |
| dst | 输出 | 目的操作数，结果矩阵，类型为LocalTensor。 |
| InitConstValueParams | 输入 | 初始化相关参数，类型为InitConstValueParams。<br>具体定义请参考`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_mm.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。<br>参数说明请参考[表3](#table3)。 |

<a name="table3"></a>
**表3** InitConstValueParams结构体参数说明

| 参数名称 | 含义 |
| --------- | ------ |
| repeatTimes | 迭代次数。默认值为0。取值范围：repeatTimes∈[0, 32767]。**注：repeatTimes = 0表示不执行，该指令将被视为NOP（空操作）。** |
| blockNum | 每次迭代初始化的数据块个数，取值范围：blockNum∈[0, 32767]。默认值为0。**注：blockNum = 0表示不执行，该指令将被视为NOP（空操作）。**<br>&nbsp;&nbsp;&bull; dst的物理存储位置为L1 Buffer（TPosition: A1/B1）时，每一个block（数据块）大小是32B；<br>&nbsp;&nbsp;&bull; dst的物理存储位置为L0A Buffer（TPosition: A2）/L0B Buffer（TPosition: B2）时，每一个block（数据块）大小是512B。 |
| dstGap | 目的操作数前一个迭代结束地址到后一个迭代起始地址之间的距离。<br>&nbsp;&nbsp;&bull; dst的物理存储位置为L1 Buffer（TPosition: A1/B1）时，单位是32B；<br>&nbsp;&nbsp;&bull; dst的物理存储位置为L0A Buffer（TPosition: A2）/L0B Buffer（TPosition: B2）时，单位是512B。<br>&nbsp;&nbsp;取值范围：dstGap∈[0, 32767]。默认值为0。 |
| initValue | 初始化的value值，支持的数据类型与dst保持一致。 |

## 数据类型

<!-- npu="910" id10 -->Atlas 训练系列产品，支持的数据类型为：half。<!-- end id10 --><br>
<!-- npu="310p" id11 -->Atlas 推理系列产品AI Core，支持的数据类型为：int16_t、uint16_t、half。<!-- end id11 --><br>
<!-- npu="910b" id12 -->Atlas A2训练系列产品/Atlas A2推理系列产品，支持的数据类型为：int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。<!-- end id12 --><br>
<!-- npu="A3" id13 -->Atlas A3训练系列产品/Atlas A3推理系列产品，支持的数据类型为：int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。<!-- end id13 --><br>
<!-- npu="310b" id14 -->Atlas 200I/500 A2 推理产品，支持的数据类型为：int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。<!-- end id14 --><br>
<!-- npu="950" id15 -->Ascend 950PR/Ascend 950DT，支持的数据类型为：int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。<!-- end id15 --><br>
<!-- npu="x90" id16 -->Kirin X90，支持的数据类型为：int8_t、half。<!-- end id16 --><br>
<!-- npu="9030" id17 -->Kirin 9030，支持的数据类型为：half。<!-- end id17 -->

## 返回值说明

无

## 约束说明<a name="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。
- 当目的操作数位于L0A Buffer/L0B Buffer时，指令执行占用的流水为PIPE\_MTE1，目的操作数地址需要512Byte对齐。当目的操作数位于L1 Buffer时，指令执行占用的流水为PIPE\_MTE2，目的地址需要32Byte对齐。
- 当repeatTimes为0或者blockNum为0时，指令不会进行任何操作。
- 不同的型号在设置InitConstValueParams参数时，支持配置参数的不同。
    - 仅支持配置迭代次数和初始化值场景下，其他参数配置无效。每次迭代处理固定数据量（512字节），迭代间无间隔。
    - 支持配置所有参数场景下，支持配置迭代次数、初始化值、每个迭代处理的数据块个数和迭代间间隔。
    - 特别地，针对如下型号，仅支持配置迭代次数和初始化值。
        
        <!-- npu="910" id18 -->
        - Atlas 训练系列产品。
        <!-- end id18 -->
        <!-- npu="310p" id19 -->
        - Atlas 推理系列产品AI Core。
        <!-- end id19 -->

- 支持的物理存储位置为：L1 Buffer（TPosition: A1/B1）、L0A Buffer（TPosition: A2）、L0B Buffer（TPosition: B2）。
    <!-- npu="950" id20 -->
    - 特别针对Ascend 950PR/Ascend 950DT，支持物理存储位置为：L1 Buffer（TPosition: A1/B1）。
    <!-- end id20 -->
    - 如果物理存储位置为L1 Buffer（TPosition: A1/B1），起始地址需要满足32B对齐；
    - 如果物理存储位置为L0A Buffer（TPosition: A2）、L0B Buffer（TPosition: B2），起始地址需要满足512B对齐。

## 调用示例<a name="section642mcpsimp"></a>

```cpp
uint32_t mLength = 16;
uint32_t kLength = 16;
Fill(leftMatrix, {1, static_cast<uint16_t>(mLength * kLength * sizeof(float) / 32), 0, 1}); // 给leftMatrix填充mLength * kLength长度的数据为1，按32B的颗粒度进行填充
```

若接口用于对L1 Buffer/L0A Buffer/L0B Buffer上的LocalTensor清0使用，示例请参考：[Fill调用样例](../../../../../../../examples/01_simd_cpp_api/06_compatibility_guide/fill)。
