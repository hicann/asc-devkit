# LoadDataWithTranspose<a id="ZH-CN_TOPIC_0000002538231154"></a>

## 产品支持情况<a id="zh-cn_topic_0000002543851571_section796754519912"></a>

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
- Kirin 9030：不支持
<!-- end id9 -->

## 功能说明<a id="zh-cn_topic_0000002543851571_section106841136114319"></a>

头文件路径为：`"basic_api/kernel_operator_mm_intf.h"`。

LoadDataWithTranspose负责完成普通矩阵计算所需的2D格式的数据的搬运，搬运过程中会伴随转置操作，参考特性[分形转置](#zh-cn_topic_0000002543851571_section10110103802915)。以512字节的数据分形为单位进行搬运，支持如下数据通路的搬运：

L1 Buffer->L0A Buffer、L1 Buffer->L0B Buffer。

<!-- npu="950" id10 -->
针对Ascend 950PR/Ascend 950DT，本接口不支持L1 Buffer->L0A Buffer通路，仅支持L1 Buffer->L0B Buffer通路。
<!-- end id10 -->

实现原理可参考伪代码：[LoadDataWithTranspose伪代码](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0/scripts/load_data_with_transpose.py)。

## 函数原型<a id="zh-cn_topic_0000002543851571_section82039854412"></a>

```cpp
template <typename T>
__aicore__ inline void LoadDataWithTranspose(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2dTransposeParams& loadDataParams)
```

<!-- npu="950" id11 -->
针对Ascend 950PR/Ascend 950DT：

```cpp
template <typename T>
__aicore__ inline void LoadDataWithTranspose(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2dTransposeParamsV2& loadDataParams)
```
<!-- end id11 -->

## 参数说明<a id="zh-cn_topic_0000002543851571_section16128134420472"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
| -------- | ------ |
| T | 源操作数和目的操作数的数据类型。 |

**表2** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ----------- | ------ |
| dst | 输出 | 目的操作数，结果矩阵，类型为LocalTensor。<br>分形约束参考[矩阵计算输入搬运约束](矩阵计算输入搬运约束.md)。<br>起始地址对齐约束参考[对齐约束](矩阵计算输入搬运约束.md)。<br>LocalTensor的起始地址需要保证512字节对齐。<br>数据类型和src的数据类型保持一致。 |
| src | 输入 | 源操作数，类型为LocalTensor。<br>分形约束参考[矩阵计算输入搬运约束](矩阵计算输入搬运约束.md)。<br>起始地址对齐约束参考[对齐约束](矩阵计算输入搬运约束.md)。<br>LocalTensor的起始地址需要保证32字节对齐。<br>数据类型和dst的数据类型保持一致。 |
| loadDataParams | 输入 | LoadDataWithTranspose相关参数。不同型号该参数的类型不同，请参考[loadDataParams参数类型说明](#loaddataparams参数类型说明)。<br>具体定义请参考`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_mm.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。 |

### loadDataParams参数类型说明

不同型号的参数loadDataParams的类型不同，开发者可参考如下：

- 类型为LoadData2dTransposeParams，参数说明请参考[表3](#zh-cn_topic_0000002543851571_table13526111319538)。
<!-- npu="950" id12 -->
- 特别针对Ascend 950PR/Ascend 950DT，类型为LoadData2dTransposeParamsV2。参数说明请参考[表4](#zh-cn_topic_0000002543851571_table64891930194618)。
<!-- end id12 -->

**表3** LoadData2dTransposeParams结构体内参数说明<a id="zh-cn_topic_0000002543851571_table13526111319538"></a>

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ----------- | ------ |
| startIndex | 输入 | 方块矩阵ID，搬运起始位置为源操作数中第几个方块矩阵（0为源操作数中第1个方块矩阵）。取值范围：startIndex∈[0, 65535]。默认为0。<br>例如，源操作数中有20个大小为16\*8\*4字节的分形（数据类型为float），startIndex=1表示搬运起始位置为第2个方块矩阵，即将第3和第4个分形从源操作数中转置到目的操作数中（第1、2个分形组成第1个方块矩阵，第3、4个分形组成第2个方块矩阵）。<br>特性细节可参考：[设置搬运起始位置](#zh-cn_topic_0000002543851571_section520575413118)。 |
| repeatTimes | 输入 | 迭代次数。<br>对于uint8_t/int8_t数据类型，每次迭代处理32\*32\*1字节数据；<br>对于half/bfloat16_t数据类型，每次迭代处理16\*16\*2字节数据；<br>对于float/int32_t/uint32_t数据类型，每次迭代处理16\*16\*4字节数据。<br>对于int4b_t数据类型，每次迭代处理16\*64\*0.5字节数据。<br>取值范围：repeatTimes∈[0, 255]。默认为0。<br>**注：repeatTimes = 0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| srcStride | 输入 | 相邻迭代间，源操作数前一个分形与后一个分形起始地址的间隔。这里的单位实际上是拼接后的方块矩阵的大小。<br>对于uint8_t/int8_t数据类型，单位是32\*32\*1字节；<br>对于half/bfloat16_t数据类型，单位是16\*16\*2字节；<br>对于float/int32_t/uint32_t数据类型，单位是16\*16\*4字节。<br>对于int4b_t数据类型，每次迭代处理16\*64\*0.5字节数据。<br>取值范围：srcStride∈[0, 65535]。默认为0。<br>**注：srcStride = 0表示在连续的重复执行周期之间，重复获取相同的分形矩阵。**<br>特性细节可参考：[非连续搬入](#zh-cn_topic_0000002543851571_section1750533101219)。 |
| dstGap | 输入 | 相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔，单位：512字节。取值范围：dstGap∈[0, 65535]。默认为0。<br>**注：dstGap = 0表示目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址无间隔，分形连续排布。**<br>特性细节可参考：[非连续搬入](#zh-cn_topic_0000002543851571_section1750533101219)。 |
| dstFracGap | 输入 | 每个迭代内目的操作数转置前一个分形结束地址与后一个分形起始地址的间隔，单位为512字节，仅在数据类型为float/int32_t/uint32_t/uint8_t/int8_t/int4b_t时有效。取值范围：dstFracGap∈[0, 65535]。默认为0。<br>**注：dstFracGap = 0表示每个迭代内目的操作数前一个分形结束地址与后一个分形起始地址无间隔，分形连续排布。**<br>特性细节可参考：[非连续搬入](#zh-cn_topic_0000002543851571_section1750533101219)。 |
| addrMode | 输入 | 控制地址更新方式，默认为false：<br>&nbsp;&nbsp;&bull; true：递减，每次迭代在前一个地址的基础上减去srcStride。<br>&nbsp;&nbsp;&bull; false：递增，每次迭代在前一个地址的基础上加上srcStride。<br>特性细节可参考：[控制地址更新方式](#zh-cn_topic_0000002543851571_section26691915141315)。<br>**注：保持默认值即可，该参数不涉及性能。** |

<!-- npu="950" id13 -->
针对Ascend 950PR/Ascend 950DT，类型为LoadData2dTransposeParamsV2，请参考下表：

**表4** LoadData2dTransposeParamsV2结构体内参数说明<a id="zh-cn_topic_0000002543851571_table64891930194618"></a>

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ----------- | ------ |
| startIndex | 输入 | 方块矩阵ID，搬运起始位置为源操作数中第几个分形。取值范围：startIndex∈[0, 65535]。默认为0。 |
| repeatTimes | 输入 | 迭代次数。<br>对于int4b_t数据类型，每次迭代处理4个分形，每个分形为16\*64\*0.5字节数据。<br>对于uint8_t/int8_t数据类型，每次迭代处理2个分形，每个分形处理16\*32\*1字节数据；<br>对于half/bfloat16_t数据类型，每次迭代处理1个分形，每个分形处理16\*16\*2字节数据；<br>对于int32_t/uint32_t/float数据类型，每次迭代处理4个分形，每个分形为16\*8\*4字节数据。<br>取值范围：repeatTimes∈[1, 255]。 |
| srcStride | 输入 | 相邻迭代间，源操作数前一个分形与后一个分形起始地址的间隔。单位为单个分形512字节。<br>取值范围：srcStride∈[0, 65535]。默认为0。 |
| dstGap | 输入 | 相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔，单位：512字节。取值范围：dstGap∈[0, 65535]。默认为0。 |
| dstFracGap | 输入 | 每个迭代内目的操作数转置前一个分形结束地址与后一个分形起始地址的间隔，单位为512字节，仅在数据类型为float/int32_t/uint32_t/uint8_t/int8_t/int4b_t时有效。 |
| srcFracGap | 输入 | 每个迭代内源操作数前一个分形结束地址与后一个分形起始地址的间隔，单位为512字节，仅在数据类型为float/int32_t/uint32_t/uint8_t/int8_t/int4b_t时有效。 |
| addrMode | 输入 | 控制地址更新方式，默认为false：<br>&nbsp;&nbsp;&bull; true：递减，每次迭代在前一个地址的基础上减去srcStride。<br>&nbsp;&nbsp;&bull; false：递增，每次迭代在前一个地址的基础上加上srcStride。 |
<!-- end id13 -->

## 数据类型<a id="zh-cn_topic_0000002543851571_section4219135304818"></a>

<!-- npu="950" id14 -->
Ascend 950PR/Ascend 950DT，支持数据类型为：int8_t、uint8_t、half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id14 -->

<!-- npu="A3" id15 -->
Atlas A3 训练系列产品/Atlas A3 推理系列产品：

- 对于载入L0A Buffer支持int8_t、uint8_t、half、bfloat16_t、int32_t、uint32_t、float。
- 对于载入L0B Buffer支持int4b_t、int8_t、uint8_t、half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id15 -->

<!-- npu="910b" id16 -->
Atlas A2 训练系列产品/Atlas A2 推理系列产品：

- 对于载入L0A Buffer支持int8_t、uint8_t、half、bfloat16_t、int32_t、uint32_t、float。
- 对于载入L0B Buffer支持int4b_t、int8_t、uint8_t、half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id16 -->

<!-- npu="310b" id17 -->
Atlas 200I/500 A2 推理产品，支持数据类型为：int4b_t、int8_t、uint8_t、half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id17 -->

<!-- npu="x90" id18 -->
Kirin X90，支持数据类型：int8_t、half。
<!-- end id18 -->

## 返回值说明<a id="zh-cn_topic_0000002543851571_section640mcpsimp"></a>

无

## 约束说明<a id="zh-cn_topic_0000002543851571_section2045914466492"></a>

- repeatTimes为0时，表示不执行搬运，该接口将被视为NOP（空操作）。
- 当目的地址位于L0A Buffer/L0B Buffer时，地址必须512字节对齐。源地址位于L1 Buffer且地址必须32字节对齐。b4数据类型仅在目的操作数为L0B Buffer的时候支持。指令执行占用的流水为PIPE_MTE1。
- 当srcStride=0时，表示连续的repeat之间读取的源操作数中的同一块数据分形。
- fp32场景下，源操作数2个连续的16\*8分形将被合并为1个16\*16的方块矩阵，然后再基于该方块矩阵做转置，因此要求两个连续分形合并为方块矩阵，要求L1 Buffer上的A矩阵满足ZZ或ZN排布。
- 开发者需要保证目的操作数转置后的分形没有重叠。
<!-- npu="950" id19 -->
- 针对Ascend 950PR/Ascend 950DT，推荐使用LoadData2dTransposeParamsV2作为参数，该参数具有更精细的搬运粒度。
<!-- end id19 -->
## 关键特性说明<a id="zh-cn_topic_0000002543851571_section1891111310132"></a>

### 功能和参数讲解

下面通过示例来讲解接口功能和关键参数：下文图中一个N形或者一个Z形代表一个分形。

- 对于uint8\_t/int8\_t数据类型，每次迭代处理32\*32\*1字节数据，可处理2个分形（一个分形512字节），每次迭代中，源操作数中2个连续的16\*32分形将被合并为1个32\*32的方块矩阵，基于方块矩阵做转置，转置后分裂为2个16\*32分形，根据目的操作数分形间隔等参数可以有不同的排布。

    如下图示例：

    - 共需要处理3072字节的数据，每次迭代处理32\*32\*1字节数据，需要3次迭代可以完成，repeatTimes = 3；
    - srcStride = 1，表示相邻迭代间，源操作数前一个方块矩阵与后一个方块矩阵起始地址的间隔为1（单位：32\*32\*1字节），这里的单位实际上是拼接后的方块矩阵的大小；
    - dstGap = 1，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔为1（单位：512字节）；
    - dstFracGap = 0，表示每个迭代内目的操作数前一个分形的结束地址与后一个分形起始地址的间隔为0（单位：512字节）。

    ![](../../../../figures/Nd2Nz转换示意图-13.png)

    如下图示例：

    - repeatTimes和srcStride的解释和上图示例一致；
    - dstGap = 0，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址和下一个迭代第一个分形起始地址无间隔；
    - dstFracGap = 2，表示每个迭代内目的操作数前一个分形的结束地址与后一个分形起始地址的间隔为2（单位：512字节）。

    ![](../../../../figures/Nd2Nz转换示意图-14.png)

- 对于half/bfloat16\_t数据类型，每次迭代处理16\*16\*2字节数据，可处理1个分形（一个分形512字节），每次迭代中，源操作数中1个16\*16分形将被转置。

    - 共需要处理1536字节的数据，每次迭代处理16\*16\*2字节数据，需要3次迭代可以完成，repeatTimes = 3；
    - srcStride = 1，表示相邻迭代间，源操作数前一个方块矩阵与后一个方块矩阵起始地址的间隔为1 （单位：16\*16\*2字节）；
    - dstGap = 0，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址无间隔；
    - 该场景下，因为其分形即为方块矩阵，每个迭代处理一个分形，不存在迭代内分形的间隔，该参数设置无效。

    ![](../../../../figures/Nd2Nz转换示意图-15.png)

- 对于float/int32\_t/uint32\_t数据类型，每次迭代处理16\*16\*4字节数据，可处理2个分形（一个分形512字节），每次迭代中，源操作数2个连续的16\*8分形将被合并为1个16\*16的方块矩阵，基于方块矩阵做转置，转置后分裂为2个16\*8分形，根据目的操作数分形间隔等参数可以有不同的排布。

    如下图示例：

    - 共需要处理3072字节的数据，每次迭代处理16\*16\*4字节数据，需要3次迭代可以完成，repeatTimes = 3；
    - srcStride = 1，表示相邻迭代间，源操作数前一个方块矩阵与后一个方块矩阵起始地址的间隔为1（单位：16\*16\*4字节），这里的单位实际上是拼接后的方块矩阵的大小；
    - dstGap = 1，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔为1（单位：512字节）；
    - dstFracGap = 0，表示每个迭代内目的操作数前一个分形结束地址与后一个分形起始地址的间隔为0（单位：512字节）。

    ![](../../../../figures/Nd2Nz转换示意图-16.png)

    如下图示例：

    - repeatTimes和srcStride的解释和上图示例一致；
    - dstGap = 0，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址和下一个迭代第一个分形起始地址无间隔；
    - dstFracGap = 2，表示每个迭代内目的操作数前一个分形结束地址与后一个分形起始地址的间隔为2（单位：512字节）。

    ![](../../../../figures/Nd2Nz转换示意图-17.png)

- 对于int4b\_t数据类型，每次迭代处理64\*64\*0.5字节数据，可处理4个分形（一个分形512字节），每次迭代中，源操作数中4个连续的16\*64分形将被合并为1个64\*64的方块矩阵，基于方块矩阵做转置，转置后分裂为4个16\*64分形，根据目的操作数分形间隔等参数可以有不同的排布。

    int4b\_t数据类型需要两个数拼成一个int8\_t或uint8\_t的数，拼凑的规则如下：

    ![](../../../../figures/zh-cn_image_0000001898040113.png)

    如下图示例：

    - 共需要处理6144字节的数据，每次迭代处理64\*64\*0.5字节数据，需要3次迭代可以完成，repeatTimes = 3；
    - srcStride = 1，表示相邻迭代间，源操作数前一个方块矩阵与后一个方块矩阵起始地址的间隔为1（单位：64\*64\*0.5字节），这里的单位实际上是拼接后的方块矩阵的大小；
    - dstGap = 1，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔为1（单位：512字节）；
    - dstFracGap = 0，表示每个迭代内目的操作数前一个分形的结束地址与后一个分形起始地址的间隔为0（单位：512字节）。

    ![](../../../../figures/Nd2Nz转换示意图-18.png)

    如下图示例：

    - repeatTimes和srcStride的解释和上图示例一致；
    - dstGap = 0，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址和下一个迭代第一个分形起始地址无间隔；
    - dstFracGap = 2，表示每个迭代内目的操作数前一个分形的结束地址与后一个分形起始地址的间隔为2（单位：512字节）。

    ![](../../../../figures/Nd2Nz转换示意图-19.png)

### 分形转置<a id="zh-cn_topic_0000002543851571_section10110103802915"></a>

对int4b_t/int8_t/half/float的矩阵转置操作，所需的不同512字节分形数量不同：

- int4b_t数据类型时，4个连续的16×64的分形拼接为一个64×64的方块矩阵，再进行转置并拆分为4个16×64的分形。
- int8_t数据类型时，2个连续的16×32的分形拼接为一个32×32的方块矩阵，再进行转置并拆分为2个16×32 的分形。
- half数据类型时，仅读取1个16×16的分形，并直接进行转置。
- float数据类型时，2个连续的16×8的分形拼接为一个16×16的方块矩阵，再进行转置并拆分为2个16×8的分形。

具体场景如下：

- 对于int4b_t数据类型，每次迭代处理64\*64\*0.5字节数据，可处理4个分形（一个分形512字节），每次迭代中，源操作数中4个连续的16\*64分形将被合并为1个64\*64的方块矩阵，基于方块矩阵做转置，转置后分裂为4个16\*64分形，根据目的操作数分形间隔等参数可以有不同的排布。

    int4b_t数据类型需要两个数拼成一个int8_t或uint8_t的数，拼凑的规则如下：

    ![](../../../../figures/loaddatawithtrans_b4_format.png)

    将两个int4b_t值6和7拼接为一个uint8_t时，先取其4位二进制：6为0110，7为0111。将7放在高位、6放在低位拼接，得01110110，对应的十进制为118。

    如下图示例：

    - 共需要处理12288字节的数据，每次迭代处理64\*64\*0.5字节数据，需要3次迭代可以完成，repeatTimes = 3；
    - srcStride = 1，表示相邻迭代间，源操作数前一个方块矩阵与后一个方块矩阵起始地址的间隔为1（单位：64\*64\*0.5字节）；
    - dstGap = 7，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔为7（单位：512字节）；
    - dstFracGap = 0，表示每个迭代内目的操作数前一个分形的结束地址与后一个分形起始地址的间隔为0（单位：512字节）；
    - 外层for循环的循环次数是2，源操作数的地址偏移量是3（单位：64\*64\*0.5字节），目的操作数的地址偏移量是1（单位：64\*64\*0.5字节）。

    ![](../../../../figures/loaddatawithtrans_l12l0a_b4_demo1.png)

    如下图示例：

    - repeatTimes和srcStride的解释和上图示例一致；
    - dstGap = 0，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址和下一个迭代第一个分形起始地址无间隔；
    - dstFracGap = 2，表示每个迭代内目的操作数前一个分形的结束地址与后一个分形起始地址的间隔为2（单位：512字节）；
    - 外层for循环的循环次数是2，源操作数的地址偏移量是3（单位：64\*64\*0.5字节），目的操作数的地址偏移量是3（单位：64\*64\*0.5字节）。

    ![](../../../../figures/loaddatawithtrans_l12l0a_b4_demo2.png)

- 对于int8_t数据类型，每次迭代处理32\*32\*1字节数据，可处理2个分形（一个分形512字节），每次迭代中，源操作数中2个连续的16\*32分形将被合并为1个32\*32的方块矩阵，基于方块矩阵做转置，转置后分裂为2个16\*32分形，根据目的操作数分形间隔等参数可以有不同的排布。

    如下图示例：

    - 共需要处理6144字节的数据，每次迭代处理32\*32\*1字节数据，需要3次迭代可以完成，repeatTimes = 3；
    - srcStride = 1，表示相邻迭代间，源操作数前一个方块矩阵与后一个方块矩阵起始地址的间隔为1（单位：32\*32\*1字节）；
    - dstGap = 3，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔为3（单位：512字节）；
    - dstFracGap = 0，表示每个迭代内目的操作数前一个分形的结束地址与后一个分形起始地址的间隔为0（单位：512字节）。
    - 外层for循环的循环次数是2，源操作数的地址偏移量是3（单位：32\*32\*1字节），目的操作数的地址偏移量是1（单位：32\*32\*1字节）。

    ![](../../../../figures/loaddatawithtrans_l12l0a_b8_demo1.png)

    如下图示例：

    - repeatTimes和srcStride的解释和上图示例一致；
    - dstGap = 0，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址和下一个迭代第一个分形起始地址无间隔；
    - dstFracGap = 2，表示每个迭代内目的操作数前一个分形的结束地址与后一个分形起始地址的间隔为2（单位：512字节）；
    - 外层for循环的循环次数是2，源操作数的地址偏移量是3（单位：32\*32\*1字节），目的操作数的地址偏移量是3（单位：32\*32\*1字节）。

    ![](../../../../figures/loaddatawithtrans_l12l0a_b8_demo2.png)

- 对于half数据类型，每次迭代处理16\*16\*2字节数据，可处理1个分形（一个分形512字节），每次迭代中，源操作数中1个16\*16分形将被转置。

    - 共需要处理3072字节的数据，每次迭代处理16\*16\*2字节数据，需要3次迭代可以完成，repeatTimes = 3；
    - srcStride = 1，表示相邻迭代间，源操作数前一个方块矩阵与后一个方块矩阵起始地址的间隔为1 （单位：16\*16\*2字节）；
    - dstGap = 0，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址无间隔；
    - 该场景下，因为其分形即为方块矩阵，每个迭代处理一个分形，不存在迭代内分形的间隔，dstFracGap参数设置无效。
    - 外层for循环的循环次数是2，源操作数的地址偏移量是3（单位：16\*16\*2字节），目的操作数的地址偏移量是3（单位：16\*16\*2字节）。

    ![](../../../../figures/loaddatawithtrans_l12l0a_b16.png)

- 对于float数据类型，每次迭代处理16\*16\*4字节数据，可处理2个分形（一个分形512字节），每次迭代中，源操作数2个连续的16\*8分形将被合并为1个16\*16的方块矩阵，基于方块矩阵做转置，转置后分裂为2个16\*8分形，根据目的操作数分形间隔等参数可以有不同的排布。

    如下图示例：

    - 共需要处理6144字节的数据，每次迭代处理16\*16\*4字节数据，需要3次迭代可以完成，repeatTimes = 3；
    - srcStride = 1，表示相邻迭代间，源操作数前一个方块矩阵与后一个方块矩阵起始地址的间隔为1（单位：16\*16\*4字节）；
    - dstGap = 3，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔为3（单位：512字节）；
    - dstFracGap = 0，表示每个迭代内目的操作数前一个分形结束地址与后一个分形起始地址的间隔为0（单位：512字节）。
    - 外层for循环的循环次数是2，源操作数的地址偏移量是3（单位：16\*16\*4字节），目的操作数的地址偏移量是1（单位：16\*16\*4字节）。

    ![](../../../../figures/loaddatawithtrans_l12l0a_b32_demo1.png)

    如下图示例：

    - repeatTimes和srcStride的解释和上图示例一致；
    - dstGap = 0，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址和下一个迭代第一个分形起始地址无间隔；
    - dstFracGap = 2，表示每个迭代内目的操作数前一个分形结束地址与后一个分形起始地址的间隔为2（单位：512字节）；
    - 外层for循环的循环次数是2，源操作数的地址偏移量是3（单位：16\*16\*4字节），目的操作数的地址偏移量是3（单位：16\*16\*4字节）。

    ![](../../../../figures/loaddatawithtrans_l12l0a_b32_demo2.png)

### 设置搬运起始位置<a id="zh-cn_topic_0000002543851571_section520575413118"></a>

从L1 Buffer搬运2个方块矩阵到L0A Buffer，通过设置startIndex为1跳过第一个方块矩阵。

- 对于int8_t数据类型，每次迭代处理32\*32\*1字节数据，可处理2个分形（一个分形512字节），每次迭代中，源操作数中2个连续的16\*32分形将被合并为1个32\*32的方块矩阵，基于方块矩阵做转置，转置后分裂为2个32\*16分形，根据目的操作数分形间隔等参数可以有不同的排布。

    如下图示例：

    - startIndex = 1：表示搬运起始位置为第2个方块矩阵，即将第3和第4个分形从源操作数中转置到目的操作数中；
    - repeatTimes = 2，表示每次迭代处理32\*32\*1字节数据，2次迭代完成；
    - srcStride = 1，表示相邻迭代间，源操作数前一个方块矩阵与后一个方块矩阵起始地址的间隔为1（单位：32\*32\*1字节）；
    - dstGap = 1，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔为1（单位：512字节）；
    - dstFracGap = 0，表示每个迭代内目的操作数前一个分形的结束地址与后一个分形起始地址的间隔为0（单位：512字节）。

    ![](../../../../figures/loaddatawithtrans_l12l0a_set_strat_pos.png)

### 非连续搬入<a id="zh-cn_topic_0000002543851571_section1750533101219"></a>

从L1 Buffer搬运2个方块矩阵到L0A Buffer，利用srcStride、dstGap和dstFracGap进行跳读跳写。

- 对于int8_t数据类型，每次迭代处理32\*32\*1字节数据，可处理2个分形（一个分形512字节），每次迭代中，源操作数中2个连续的16\*32分形将被合并为1个32\*32的方块矩阵，基于方块矩阵做转置，转置后分裂为2个32\*16分形，根据目的操作数分形间隔等参数可以有不同的排布。

    如下图示例：

    - startIndex = 0：表示搬运起始位置为第1个方块矩阵，即将第1和第2个分形从源操作数中转置到目的操作数中；
    - repeatTimes = 2，表示每次迭代处理32\*32\*1字节数据，2次迭代完成；
    - srcStride = 2，表示相邻迭代间，源操作数前一个方块矩阵与后一个方块矩阵起始地址的间隔为2（单位：32\*32\*1字节），间隔2个方块矩阵；
    - dstGap = 2，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔为2（单位：512字节），间隔2个数据分形；
    - dstFracGap = 1，表示每个迭代内目的操作数前一个分形的结束地址与后一个分形起始地址的间隔为1（单位：512字节），间隔1个数据分形。

    ![](../../../../figures/loaddatawithtrans_l12l0a_noncontinuous_copy.png)

### 控制地址更新方式<a id="zh-cn_topic_0000002543851571_section26691915141315"></a>

从L1 Buffer搬运3个方块矩阵到L0A Buffer，设置addrMode为1改变源操作数地址的更新方式。

- 对于int8_t数据类型，每次迭代处理32\*32\*1字节数据，可处理2个分形（一个分形512字节），每次迭代中，源操作数中2个连续的16\*32分形将被合并为1个32\*32的方块矩阵，基于方块矩阵做转置，转置后分裂为2个32\*16分形，根据目的操作数分形间隔等参数可以有不同的排布。

    如下图示例：

    - repeatTimes = 3，表示每次迭代处理32\*32\*1字节数据，3次迭代完成；
    - startIndex = 2：表示搬运起始位置为第3个方块矩阵，即将第5和第6个分形从源操作数中转置到目的操作数中；
    - srcStride = 1，表示相邻迭代间，源操作数前一个方块矩阵与后一个方块矩阵起始地址的间隔为1（单位：32\*32\*1字节）；
    - addrMode = 1，源操作数中每一次迭代的方块矩阵索引ID递减；
    - dstGap = 1，表示相邻迭代间，目的操作数前一个迭代第一个分形的结束地址到下一个迭代第一个分形起始地址的间隔为1（单位：512字节）；
    - dstFracGap = 0，表示每个迭代内目的操作数前一个分形的结束地址与后一个分形起始地址的间隔为0（单位：512字节）。

    ![](../../../../figures/loaddatawithtrans_l12l0a_ctrl_addr_update.png)

## 调用示例<a id="zh-cn_topic_0000002543851571_section088124295117"></a>

### b8数据类型，A矩阵需要转置的场景示例

在数据类型为b8，A矩阵转置的场景下，LoadData接口不支持转置，因此需要调用LoadDataWithTranspose接口完成数据搬运。

搬运过程的数据排布变化示意图如下：

![](../../../../figures/loaddatawithtrans_l12l0a_b8.png)

示例代码片段如下，仅展示样例中的部分代码，完整示例请参考：[load\_data\_l12l0样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0)。

```cpp
// dstoffset要根据A矩阵在L0 Buffer上，宽度方向的对齐来求解
uint32_t dstOffset = CeilDivision(k, fractalShape[1]) * fractalSize * fractalNum;
// srcoffset要根据A矩阵在L1 Buffer上，高度方向的对齐来求解
uint32_t srcOffset = CeilDivision(k, fractalShape[0] * fractalNum) * fractalSize * fractalNum;

// Nz -> Zz
AscendC::LoadData2dTransposeParams loadDataParams;
loadDataParams.startIndex = 0;
loadDataParams.repeatTimes = CeilDivision(k, fractalShape[0] * fractalNum);
loadDataParams.srcStride = 1;
loadDataParams.dstGap = 0;
// 每个迭代内目的操作数转置前一个分形结束地址与后一个分形起始地址的间隔，单位为512字节
loadDataParams.dstFracGap = CeilDivision(k, fractalShape[1]) - 1;
for (int i = 0; i < CeilDivision(m, fractalShape[1]); ++i) {
    AscendC::LoadDataWithTranspose(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}
```

### int8_t数据类型，A、B矩阵需要转置的场景示例

该示例输入a矩阵为int8_t类型，shape为[40,70]，输入b矩阵为int8_t类型，shape为[70,50]，输出c的类型为int32_t。a矩阵从L1 Buffer（A1）搬运到L0A Buffer（A2）并转置，b矩阵从L1 Buffer（B1）搬运到L0B Buffer（B2）并转置，之后进行Mmad计算和Fixpipe计算。示例代码片段如下，仅展示样例中的部分代码，完整使用样例请参见[LoadData_L12L0样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0)。

```cpp
uint16_t m = 40, k = 70, n = 50;
uint32_t fractalShape[2] = {16, 32 / sizeof(int8_t)};
uint32_t fractalNum = 2;
uint32_t fractalSize = fractalShape[0] * fractalShape[1];

// A矩阵LoadDataWithTranspose: Nz -> Zz
uint32_t dstOffset = CeilDivision(k, fractalShape[1]) * fractalSize * fractalNum;
uint32_t srcOffset = CeilDivision(k, fractalShape[0] * fractalNum) * fractalSize * fractalNum;
AscendC::LoadData2dTransposeParams loadDataParams;
loadDataParams.startIndex = 0;
loadDataParams.repeatTimes = CeilDivision(k, fractalShape[0] * fractalNum);
loadDataParams.srcStride = 1;
loadDataParams.dstGap = 0;
loadDataParams.dstFracGap = CeilDivision(k, fractalShape[1]) - 1;
for (int i = 0; i < CeilDivision(m, fractalShape[1]); ++i) {
    AscendC::LoadDataWithTranspose(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}

// B矩阵LoadDataWithTranspose: Nz -> Zn
uint32_t dstOffsetB = CeilDivision(n, fractalShape[0] * fractalNum) * fractalSize * fractalNum;
uint32_t srcOffsetB = fractalSize * fractalNum;
AscendC::LoadData2dTransposeParams loadDataParamsB;
loadDataParamsB.startIndex = 0;
loadDataParamsB.repeatTimes = CeilDivision(n, fractalShape[1]);
loadDataParamsB.srcStride = CeilDivision(k, fractalShape[0] * fractalNum);
loadDataParamsB.dstGap = 1;
loadDataParamsB.dstFracGap = 0;
for (int i = 0; i < CeilDivision(k, fractalShape[0] * fractalNum); ++i) {
    AscendC::LoadDataWithTranspose(b2Local[i * dstOffsetB], b1Local[i * srcOffsetB], loadDataParamsB);
}
```

### half数据类型，A、B矩阵需要转置的场景示例

该示例输入a矩阵为half类型，shape为[40,70]，输入b矩阵为half类型，shape为[70,50]，输出c的类型为float。a矩阵从L1 Buffer（A1）搬运到L0A Buffer（A2）并转置，b矩阵从L1 Buffer（B1）搬运到L0B Buffer（B2）并转置。示例代码片段如下，仅展示样例中的部分代码，完整使用样例请参见[LoadData_L12L0样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0)。

```cpp
uint16_t m = 40, k = 70, n = 50;
uint32_t fractalShape[2] = {16, 32 / sizeof(half)};
uint32_t fractalNum = 1;
uint32_t fractalSize = fractalShape[0] * fractalShape[1];

// A矩阵LoadDataWithTranspose: Nz -> Zz
uint32_t dstOffset = CeilDivision(k, fractalShape[1]) * fractalSize * fractalNum;
uint32_t srcOffset = CeilDivision(k, fractalShape[0] * fractalNum) * fractalSize * fractalNum;
AscendC::LoadData2dTransposeParams loadDataParams;
loadDataParams.startIndex = 0;
loadDataParams.repeatTimes = CeilDivision(k, fractalShape[0] * fractalNum);
loadDataParams.srcStride = 1;
loadDataParams.dstGap = 0;
loadDataParams.dstFracGap = CeilDivision(k, fractalShape[1]) - 1;
for (int i = 0; i < CeilDivision(m, fractalShape[1]); ++i) {
  AscendC::LoadDataWithTranspose(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}

// B矩阵LoadDataWithTranspose: Nz -> Zn
uint32_t dstOffsetB = CeilDivision(n, fractalShape[0] * fractalNum) * fractalSize * fractalNum;
uint32_t srcOffsetB = fractalSize * fractalNum;
AscendC::LoadData2dTransposeParams loadDataParamsB;
loadDataParamsB.startIndex = 0;
loadDataParamsB.repeatTimes = CeilDivision(n, fractalShape[1]);
loadDataParamsB.srcStride = CeilDivision(k, fractalShape[0] * fractalNum);
loadDataParamsB.dstGap = 0;
loadDataParamsB.dstFracGap = 0;
for (int i = 0; i < CeilDivision(k, fractalShape[0] * fractalNum); ++i) {
  AscendC::LoadDataWithTranspose(b2Local[i * dstOffsetB], b1Local[i * srcOffsetB], loadDataParamsB);
}
```

### float数据类型，A、B矩阵需要转置的场景示例

该示例输入a矩阵为float类型，shape为[40,70]，输入b矩阵为float类型，shape为[70,50]，输出c的类型为float。a矩阵从L1 Buffer（A1）搬运到L0A Buffer（A2）并转置，b矩阵从L1 Buffer（B1）搬运到L0B Buffer（B2）并转置。示例代码片段如下，仅展示样例中的部分代码，完整使用样例请参见[LoadData_L12L0样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0)。

```cpp
uint32_t m = 40, k = 70, n = 50;
uint32_t fractalShape[2] = {16, 32 / sizeof(float)};
uint32_t fractalNum = 2;
uint32_t fractalSize = fractalShape[0] * fractalShape[1];

// A矩阵LoadDataWithTranspose: Zz -> Zz
uint32_t dstOffset = CeilDivision(k, fractalShape[1] * fractalNum) * fractalSize * fractalNum;
uint32_t srcOffset = fractalSize * fractalNum;
AscendC::LoadData2dTransposeParams loadDataParams;
loadDataParams.startIndex = 0;
loadDataParams.repeatTimes = CeilDivision(k, fractalShape[1] * fractalNum);
loadDataParams.srcStride = CeilDivision(m, fractalShape[1] * fractalNum);
loadDataParams.dstGap = 1;
loadDataParams.dstFracGap = 0;
for (int i = 0; i < CeilDivision(m, fractalShape[1] * fractalNum); ++i) {
  AscendC::LoadDataWithTranspose(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}

// B矩阵LoadDataWithTranspose: Nz -> Zn
uint32_t dstOffsetB = CeilDivision(n, fractalShape[0]) * fractalSize * fractalNum;
uint32_t srcOffsetB = CeilDivision(n, fractalShape[1] * fractalNum) * fractalSize * fractalNum;
AscendC::LoadData2dTransposeParams loadDataParamsB;
loadDataParamsB.startIndex = 0;
loadDataParamsB.repeatTimes = CeilDivision(n, fractalShape[1] * fractalNum);
loadDataParamsB.srcStride = 1;
loadDataParamsB.dstGap = 0;
loadDataParamsB.dstFracGap = CeilDivision(n, fractalShape[0]) - 1;
for (int i = 0; i < CeilDivision(k, fractalShape[0]); ++i) {
    AscendC::LoadDataWithTranspose(b2Local[i * dstOffsetB], b1Local[i * srcOffsetB], loadDataParamsB);
}
```

<!-- npu="950" id20 -->
### 使用LoadData2dTransposeParamsV2结构体作为参数的场景示例

该示例使用了LoadData2dTransposeParamsV2结构体作为参数，输入a矩阵为int8_t类型，shape为[128,128]，输入数据格式为NZ，输入b矩阵为int8_t类型，shape为[128,256]，输入数据格式为NZ，输出c的类型为float。a矩阵从L1 Buffer（A1）搬运到L0A Buffer（A2）且不转置，b矩阵从L1 Buffer（B1）搬运到L0B Buffer（B2）并转置，示例仅展示接口调用过程，其余计算和搬运不作参考。完整示例请参考：[load_data_l12l0样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0)。

```cpp
uint32_t m = 256;
uint32_t n = 256;
uint32_t k = 128;
pipe = tpipe;
TQue<TPosition::B1, 1> qidB1_;
TQue<TPosition::B2, 1> qidB2_;
pipe->InitBuffer(qidB1_, 1, n * k * sizeof(int8_t));
pipe->InitBuffer(qidB2_, 1, n * k * sizeof(int8_t));
auto rightMatrix = qidB1_.template DeQue<int8_t>();
LocalTensor<int8_t> b2 = qidB2_.AllocTensor<int8_t>();
uint16_t fracNum = 2;
uint16_t kStep = CeilDiv(kLength, 16);
uint16_t nStep = CeilDiv(nLength, 32);
for (uint16_t i = 0; i < nStep; i ++) {
    LoadData2dTransposeParamsV2 loadDataParams;
    loadDataParams.startIndex = i * kStep;
    loadDataParams.repeatTimes = kStep / 2;
    loadDataParams.srcStride = 2;
    loadDataParams.dstGap = nStep*2 - 1;
    LoadDataWithTranspose(b2[1024*i], rightMatrix, loadDataParams);
}
qidB2_.EnQue(b2);
qidB1_.FreeTensor(rightMatrix);
```
<!-- end id20 -->
