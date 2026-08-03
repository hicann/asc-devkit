# LoadData（卷积数据搬运）<a id="ZH-CN_TOPIC_0000002538071224"></a>

## 产品支持情况<a id="zh-cn_topic_0000002512171652_section796754519912"></a>

### LoadData（卷积数据搬运） v1接口

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：不支持
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
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：不支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：不支持
<!-- end id9 -->

### LoadData（卷积数据搬运） v2接口和LoadData（卷积数据搬运） v2 Pro接口

<!-- npu="950" id10 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas 训练系列产品：不支持
<!-- end id16 -->
<!-- npu="x90" id17 -->
- Kirin X90：支持
<!-- end id17 -->
<!-- npu="9030" id18 -->
- Kirin 9030：不支持
<!-- end id18 -->

## 功能说明<a id="zh-cn_topic_0000002512171652_section106841136114319"></a>

头文件路径为：`"basic_api/kernel_operator_mm_intf.h"`。

LoadData（卷积数据搬运）本质上是用于将NC1HWC0格式的Feature Map完成Image to Column展开，然后再从展开后的二维矩阵中选取指定数据块搬入对应内存位置。见图1。

由于LoadData搬运过程中，是以512字节的数据分形为单位进行搬运的，因此，LoadData也可以用于普通矩阵计算所需的2D格式数据的搬运。如何使用LoadData进行2D格式数据的搬运可以参考[关键特性说明](#zh-cn_topic_0000002512171652_section102629572045)。

LoadData（卷积数据搬运）仅支持如下数据通路：L1 Buffer->L0A Buffer、L1 Buffer->L0B Buffer。

实现原理可参考伪代码：[LoadData（卷积数据搬运）伪代码](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0/scripts/load3d.py)。

**图1** LoadData to L0A Buffer功能示意图<a id="zh-cn_topic_0000002512171652_fig54450833715"></a>  

![](../../../../figures/load3d_l1tol0a_demo.png "LoadData-to-L0A Buffer功能示意图")

## 函数原型<a id="zh-cn_topic_0000002512171652_section82039854412"></a>

**LoadData（卷积数据搬运） v1接口：**

```cpp
template <typename T, const IsResetLoad3dConfig &defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG, typename U = PrimT<T>, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV1<U>& loadDataParams)
```

**LoadData（卷积数据搬运） v2接口：**

```cpp
template <typename T, const IsResetLoad3dConfig& defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG, typename U = PrimT<T>, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2<U>& loadDataParams)
```

**LoadData（卷积数据搬运） v2 Pro接口：**

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2Pro& loadDataParams)
```

## 参数说明<a id="zh-cn_topic_0000002512171652_section16128134420472"></a>

**表1** 模板参数说明

| 参数名称 | 含义 |
| ---------- | ------ |
| T | 源操作数和目的操作数的数据类型。支持的数据类型请参考[数据类型](#zh-cn_topic_0000002512171652_section4219135304818)。 |
| defaultConfig | 控制是否在LoadData（卷积数据搬运） v1/LoadData（卷积数据搬运） v2接口内部设置相关属性。IsResetLoad3dConfig类型。IsResetLoad3dConfig结构定义如下：<pre>struct IsResetLoad3dConfig {<br>   bool isSetFMatrix = true;<br>   bool isSetPadding = true;<br>};</pre>isSetFMatrix配置为true，表示在接口内部设置FeatureMap的属性描述（包括l1H、l1W、padList，参数介绍参考[表3](#zh-cn_topic_0000002512171652_table679014222918)、[表4](#zh-cn_topic_0000002512171652_table193501032193419)）；设置为false，表示该接口传入的FeatureMap的属性描述不生效，开发者需要通过SetFmatrix进行设置。<br>isSetPadding配置为true，表示在接口内部设置Pad属性描述（即padValue参数，参数介绍参考[表3](#zh-cn_topic_0000002512171652_table679014222918)、[表4](#zh-cn_topic_0000002512171652_table193501032193419)）；设置为false，表示该接口传入的Pad属性不生效，开发者需要通过SetLoadDataPaddingValue进行设置。可参考样例SetFmatrix调用示例。<br>该参数的默认值如下：<pre>constexpr IsResetLoad3dConfig IS_RESER_LOAD3D_DEFAULT_CONFIG = {true, true};</pre>特性细节可参考：[Feature Map、Pad属性描述寄存器设置](#zh-cn_topic_0000002512171652_section1881795134015)。 |
| U | LoadData3DParamsV1/LoadData3DParamsV2中padValue的数据类型。<br>&nbsp;&nbsp;&bull;当dst、src使用基础数据类型时，U和dst、src的数据类型T需保持一致，否则编译失败。<br>&nbsp;&nbsp;&bull;当dst、src使用TensorTrait类型时，U和dst、src的数据类型T的LiteType需保持一致，否则编译失败。<br>最后一个模板参数仅用于上述数据类型检查，用户无需关注。 |

**表2** 通用参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ----------- | ------ |
| dst | 输出 | 目的操作数，类型为LocalTensor。<br>数据连续排列顺序由目的操作数所在物理存储位置决定，不同产品型号的具体格式约束请参考[约束说明](#zh-cn_topic_0000002512171652_dst_layout)。 |
| src | 输入 | 源操作数，类型为LocalTensor。<br>数据连续排列顺序由目的操作数所在物理存储位置决定：<br>&nbsp;&nbsp;&bull; L1 Buffer（TPosition: A1/B1）：NC1HWC0格式。 |
| loadDataParams | 输入 | LoadData参数结构体，类型为：<br>&nbsp;&nbsp;&bull; LoadData3DParamsV1，具体参考[表3](#zh-cn_topic_0000002512171652_table679014222918)。<br>&nbsp;&nbsp;&bull; LoadData3DParamsV2，具体参考[表4](#zh-cn_topic_0000002512171652_table193501032193419)。<br>&nbsp;&nbsp;&bull; LoadData3DParamsV2Pro，具体参考[表5](#zh-cn_topic_0000002512171652_table118027314415)。<br>上述结构体参数定义请参考`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_mm.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。 |


**表3** LoadData3DParamsV1结构体内参数说明<a id="zh-cn_topic_0000002512171652_table679014222918"></a>

| 参数名称 | 含义 |
| ---------- | ------ |
| padList | padding列表 [padding_left, padding_right, padding_top, padding_bottom]，每个元素取值范围：[0, 255]。默认为{0, 0, 0, 0}。 |
| l1H | 源操作数height，取值范围：l1H∈[1, 32767]。 |
| l1W | 源操作数width，取值范围：l1W∈[1, 32767]。 |
| c1Index | 该指令在源tensor C1维度的起点，取值范围：c1Index∈[0, 4095]。默认为0。 |
| fetchFilterW | 该指令在卷积核上w维度的起始位置，取值范围：fetchFilterW∈[0, 254]。默认为0。 |
| fetchFilterH | 该指令在filter上h维度的起始位置，取值范围：fetchFilterH∈[0, 254]。默认为0。 |
| leftTopW | 该指令在源操作数上w维度的起点，取值范围：leftTopW∈[-255, 32767]。默认为0。如果padding_left = a，leftTopW配置为-a。 |
| leftTopH | 该指令在源操作数上h维度的起点，取值范围：leftTopH∈[-255, 32767]。默认为0。如果padding_top = a，leftTopH配置为-a。 |
| strideW | 卷积核在源操作数w维度滑动的步长，取值范围：strideW∈[1, 63]。 |
| strideH | 卷积核在源操作数h维度滑动的步长，取值范围：strideH∈[1, 63]。 |
| filterW | 卷积核width，取值范围：filterW∈[1, 255]。 |
| filterH | 卷积核height，取值范围：filterH∈[1, 255]。 |
| dilationFilterW | 卷积核width膨胀系数，取值范围：dilationFilterW∈[1, 255]。 |
| dilationFilterH | 卷积核height膨胀系数，取值范围：dilationFilterH∈[1, 255]。 |
| jumpStride | 迭代之间，目的操作数首地址步长，取值范围：jumpStride∈[1, 127]。 |
| repeatMode | 迭代模式。<br>&nbsp;&nbsp;&bull;模式0：每次迭代，增加卷积核窗口中的点，对应在目的矩阵上往w维度方向增长。<br>&nbsp;&nbsp;&bull;模式1：每次迭代，增加滑动窗口左上坐标，对应在目的矩阵上往h维度方向增长。<br>取值范围：repeatMode∈[0, 1]。默认为0。 |
| repeatTime | 迭代次数，每一次源操作数和目的操作数的地址都会改变。取值范围：repeatTime∈[1, 255]。 |
| cSize | 配置是否开启cSize = 4（b16） / cSize = 8（b8）优化，取值范围：cSize∈[0, 1]。默认为0。 |
| padValue | Pad填充值的数值，数据类型需要与src保持一致。默认为0。若不想开启padding，可将padList设为全0。 |


**表4** LoadData3DParamsV2结构体内参数说明<a id="zh-cn_topic_0000002512171652_table193501032193419"></a>

| 参数名称 | 含义 |
| ---------- | ------ |
| padList | padding列表 [padding_left, padding_right, padding_top, padding_bottom]，每个元素取值范围：[0, 255]。默认为{0, 0, 0, 0}。 |
| l1H | 源操作数height，取值范围：l1H∈[0, 32767]。默认为0。<br>**l1H = 0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| l1W | 源操作数width，取值范围：l1W∈[0, 32767]。默认为0。<br>**l1W = 0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| channelSize | 源操作数的通道数，取值范围：channelSize∈[0, 65535]。默认为0。不同产品型号支持的channelSize取值不同，具体请参考[约束说明](#zh-cn_topic_0000002512171652_channelsize_constraint)。<br>**注：channelSize = 0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| kExtension | 该指令在目的操作数width维度的传输长度，如果不覆盖最右侧的分形，对于b32类型，应为8的倍数，对于b16类型，应为16的倍数，对于b8类型，应为32的倍数，对于b4类型，应为64的倍数；覆盖的情况则任何数据类型都无倍数要求。取值范围：kExtension∈[0, 65535]。默认为0。<br>**注：kExtension = 0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| mExtension | 该指令在目的操作数height维度的传输长度，如果不覆盖最下侧的分形，对于b4、b8、b16类型，应为16的倍数，b32类型无要求；覆盖的情况则任何数据类型都无倍数要求。取值范围：mExtension∈[0, 65535]。默认为0。<br>**注：mExtension = 0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| kStartPt | 该指令在目的操作数width维度的起点，对于b32类型，应为8的倍数，对于b16类型，应为16的倍数，对于b8类型，应为32的倍数，对于b4类型，应为64的倍数；取值范围[0, 65535]。默认为0。 |
| mStartPt | 该指令在目的操作数height维度的起点，取值范围[0, 32767]。默认为0。 |
| strideW | 卷积核在源操作数width维度滑动的步长，取值范围：strideW∈[0, 63]。默认为1。 |
| strideH | 卷积核在源操作数height维度滑动的步长，取值范围：strideH∈[0, 63]。默认为1。 |
| filterW | 卷积核width，取值范围：filterW∈[0, 255]。默认为1。<br>**注：filterW=0且filterSizeW=false时表示不执行搬运，该接口将被视为NOP（空操作）。** |
| filterH | 卷积核height，取值范围：filterH∈[0, 255]。默认为1。<br>**注：filterH=0且filterSizeH=false时表示不执行搬运，该接口将被视为NOP（空操作）。** |
| dilationFilterW | 卷积核width膨胀系数，取值范围：dilationFilterW∈[0, 255]。默认为1。 |
| dilationFilterH | 卷积核height膨胀系数，取值范围：dilationFilterH∈[0, 255]。默认为1。 |
| enTranspose | 是否启用转置功能，对整个目标矩阵进行转置，支持数据类型为bool，默认为false。<br>&nbsp;&nbsp;&bull; true：启用<br>&nbsp;&nbsp;&bull; false：不启用<br>不同产品型号下enTranspose的有效条件不同，具体请参考[约束说明](#zh-cn_topic_0000002512171652_entranspose_constraint)。<br>**注：在目的操作数的物理存储位置为L0B Buffer（TPosition: B2）时本参数无效。** |
| enSmallK | 是否开启small k特性，每个分形矩阵大小为16*4，支持数据类型为bool，默认为false。当前产品形态，该特性已不再支持。 |
| padValue | Pad填充值的数值，数据类型需要与src保持一致。若不想开启padding，可将padList设为全0。默认为0。 |
| filterSizeW | 是否在filterW的基础上将卷积核width增加256个元素。true，增加；false，不增加。默认为false。 |
| filterSizeH | 是否在filterH的基础上将卷积核height增加256个元素。true，增加；false，不增加。默认为false。 |
| fMatrixCtrl | 表示LoadData（卷积数据搬运） v2指令从左矩阵还是右矩阵获取FeatureMap的属性描述，与SetFmatrix配合使用，默认值为false。当前只支持设置为false。<br>&nbsp;&nbsp;&bull; true：从右矩阵中获取FeatureMap的属性描述；<br>&nbsp;&nbsp;&bull; false：从左矩阵中获取FeatureMap的属性描述。 |

**表5** LoadData3DParamsV2Pro结构体内参数说明<a id="zh-cn_topic_0000002512171652_table118027314415"></a>

| 参数名称 | 含义 |
| ---------- | ------ |
| channelSize | 源操作数的通道数，取值范围：channelSize∈[0, 65535]。<br>对于half，channelSize除16的余数应当为0，4或8。<br>对于int8_t和uint8_t，channelSize除32的余数应当为0，4，8或16。<br>对于int4b_t，channelSize为8，16，32，N\*64，N\*64+8，N\*64+16，N\*64+32，N为正整数。 |
| enTranspose | 是否启用转置功能，对整个目标矩阵进行转置，支持数据类型为bool，仅在目的TPosition为A2，且源操作数为half类型时有效。默认为false。<br>&nbsp;&nbsp;&bull; true：启用；<br>&nbsp;&nbsp;&bull; false：不启用。 |
| enSmallK | 是否开启small k特性，每个分形矩阵大小为16\*4，支持数据类型为bool，默认为false。<br>**注：当前产品形态，该特性已不再支持。** |
| filterSizeW | 是否在filterW的基础上将卷积核width增加256个元素。true，增加；false，不增加。 |
| filterSizeH | 是否在filterH的基础上将卷积核height增加256个元素。true，增加；false，不增加。 |
| fMatrixCtrl | 表示LoadData（卷积数据搬运） v2指令从左矩阵还是右矩阵获取FeatureMap的属性描述，与SetFmatrix配合使用，当前只支持设置为false，默认值为false。<br>&nbsp;&nbsp;&bull; true：从右矩阵中获取FeatureMap的属性描述；<br>&nbsp;&nbsp;&bull; false：从左矩阵中获取FeatureMap的属性描述。 |
| extConfig | 组合参数（uint64_t类型），默认值为0；<br><pre>extConfig = ((uint64_t)mStartPt << 48)<br>    &#124; ((uint64_t)kStartPt << 32)<br>    &#124; ((uint64_t)mExtension << 16)<br>    &#124; (uint64_t)kExtension;</pre> |
| filterConfig | 组合参数（uint64_t类型），默认值为0X10101010101；<br><pre>filterConfig = ((uint64_t)dilationFilterH << 40)<br>    &#124; ((uint64_t)dilationFilterW << 32)<br>    &#124; ((uint64_t)filterH << 24)<br>    &#124; ((uint64_t)filterW << 16)<br>    &#124; ((uint64_t)strideH << 8)<br>    &#124; (uint64_t)strideW;</pre> |

## 数据类型<a id="zh-cn_topic_0000002512171652_section4219135304818"></a>

<!-- npu="310p,910" id19 -->
**LoadData（卷积数据搬运） v1接口：**

<!-- npu="310p" id20 -->
- Atlas 推理系列产品AI Core，支持的数据类型为：int8_t、uint8_t、half。
<!-- end id20 -->

<!-- npu="910" id21 -->
- Atlas 训练系列产品，支持的数据类型为：int8_t、uint8_t、half。
<!-- end id21 -->
<!-- end id19 -->

<!-- npu="950,A3,910b,310b,310p,x90" id22 -->
**LoadData（卷积数据搬运） v2接口、LoadData（卷积数据搬运） v2 Pro接口：**

<!-- npu="950" id23 -->
- Ascend 950PR/Ascend 950DT，支持数据类型为：int8_t、uint8_t、hifloat8_t、fp8_e5m2_t、fp8_e4m3fn_t、half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id23 -->

<!-- npu="A3" id24 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：
    - TPosition为A1/A2时，支持数据类型为：int4b_t、int8_t、uint8_t、half、bfloat16_t、int32_t、uint32_t、float。
    - TPosition为B1/B2时，支持数据类型为：half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id24 -->

<!-- npu="910b" id25 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：
    - TPosition为A1/A2时，支持数据类型为：int4b_t、int8_t、uint8_t、half、bfloat16_t、int32_t、uint32_t、float。
    - TPosition为B1/B2时，支持数据类型为：half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id25 -->

<!-- npu="310b" id26 -->
- Atlas 200I/500 A2 推理产品：
    - TPosition为A1/A2时，支持数据类型为：int4b_t、int8_t、uint8_t、half、bfloat16_t、int32_t、uint32_t、float。
    - TPosition为B1/B2时，支持数据类型为：half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id26 -->

<!-- npu="310p" id27 -->
- Atlas 推理系列产品AI Core，支持的数据类型为：int4b_t、int8_t、uint8_t、half。
<!-- end id27 -->

<!-- npu="x90" id28 -->
- Kirin X90产品：
    - TPosition为A1/A2时，支持数据类型为：int8_t、half。
    - TPosition为B1/B2时，支持数据类型为：int8_t、half。
<!-- end id28 -->
<!-- end id22 -->

## 返回值说明<a id="zh-cn_topic_0000002512171652_section640mcpsimp"></a>

无

## 约束说明<a id="zh-cn_topic_0000002512171652_section2045914466492"></a>

- 非转置场景下，L1 Buffer->L0B Buffer通路不支持。L1 Buffer->L0B Buffer通路下会自动进行转置，不需要配置enTranspose，此时enTranspose参数无效。

<!-- npu="950" id29 -->
- 针对Ascend 950PR/Ascend 950DT：必须使用辅助配置接口SetLoadDataRepeat配置dstStride参数，若不开启repeat模式，接口内repeat相关参数使用默认构造值。
<!-- end id29 -->

<!-- npu="A3,910b" id30 -->
- 针对如下产品型号：

    <!-- npu="A3" id31 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品；
    <!-- end id31 -->
    <!-- npu="910b" id32 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品；
    <!-- end id32 -->

    L1 Buffer-\>L0B Buffer通路不支持b4/b8类型数据；转置场景下，L1 Buffer-\>L0A Buffer通路不支持b8数据类型。
<!-- end id30 -->

- 当l1H/l1W/channelSize/kExtension/mExtension中的任意一个值为0时，该指令不会被执行。
- 当filterW为0且filterSizeW为false或者当filterH为0且filterSizeH为false时，该指令不会被执行。
- 当目的地址位于L0A Buffer/L0B Buffer时，地址必须512字节对齐。当源地址位于L1 Buffer时，地址必须32字节对齐。指令执行占用的流水为PIPE_MTE1。
<!-- npu="910,310p" id33 -->
- LoadData3DParamsV1 cSize特性的开启，需要保证L1 Buffer（A1/B1）中的feature map为4 channel对齐。
<!-- end id33 -->

- 数据连续排列顺序由目的操作数所在物理存储位置决定，具体约束如下：<a id="zh-cn_topic_0000002512171652_dst_layout"></a>

    <!-- npu="A3,910b" id34 -->
    - 针对Atlas A3 训练系列产品/Atlas A3 推理系列产品、Atlas A2 训练系列产品/Atlas A2 推理系列产品：
        - L0A Buffer（TPosition: A2）：ZZ格式；
        - L0B Buffer（TPosition: B2）：ZN格式。
    <!-- end id34 -->

    <!-- npu="950" id35 -->
    - 针对Ascend 950PR/Ascend 950DT：
        - L0A Buffer（TPosition: A2）：NZ格式；
        - L0B Buffer（TPosition: B2）：ZN格式。
    <!-- end id35 -->

- LoadData3DParamsV2结构体中channelSize的取值约束如下：<a id="zh-cn_topic_0000002512171652_channelsize_constraint"></a>

    <!-- npu="310p" id36 -->
    - 针对Atlas 推理系列产品AI Core：对于half，channelSize可取值为4，8，16，N\*16+4，N\*16+8；对于int8_t/uint8_t，channelSize可取值为4，8，16，32，N\*32+4，N\*32+8，N\*32+16；对于int4b_t，channelSize可取值为8，16，32，N\*64，N\*64+8，N\*64+16，N\*64+32。N为正整数。
    <!-- end id36 -->

    <!-- npu="950,A3,910b,310b,x90" id37 -->
    - 针对Ascend 950PR/Ascend 950DT、Atlas A3 训练系列产品/Atlas A3 推理系列产品、Atlas A2 训练系列产品/Atlas A2 推理系列产品、Atlas 200I/500 A2 推理产品、Kirin X90：对于uint32_t/int32_t/float，channelSize可取值为4，N\*8，N\*8+4；对于half/bfloat16，channelSize可取值为4，8，N\*16，N\*16 + 4，N\*16+8；对于int8_t/uint8_t，channelSize可取值为4，8，16，32\*N，N\*32+4，N\*32+8，N\*32+16；对于int4b_t，channelSize可取值为8，16，32，N\*64，N\*64+8，N\*64+16，N\*64+32。N为正整数。
    <!-- end id37 -->

- LoadData3DParamsV2结构体中enTranspose的有效条件如下：<a id="zh-cn_topic_0000002512171652_entranspose_constraint"></a>

    <!-- npu="A3,910b" id38 -->
    - 针对Atlas A3 训练系列产品/Atlas A3 推理系列产品、Atlas A2 训练系列产品/Atlas A2 推理系列产品：在目的操作数的物理存储位置为L0A Buffer（TPosition: A2），且源操作数为b16/b32类型时有效。
    <!-- end id38 -->

    <!-- npu="950" id39 -->
    - 针对Ascend 950PR/Ascend 950DT：在目的操作数的物理存储位置为L0A Buffer（TPosition: A2），且源操作数为b8/b16/b32类型时有效。
    <!-- end id39 -->

## 关键特性说明<a id="zh-cn_topic_0000002512171652_section102629572045"></a>

### 使用LoadData（卷积数据搬运）完成矩阵计算所需2D格式数据的搬运<a id="zh-cn_topic_0000002512171652_section86392366416"></a>

对于NC1HWC0格式的数据，当N为1，卷积核width和height为1，padding为0，卷积核在源操作数width维度和height维度滑动的步长为1，卷积核width和height膨胀系数为1，此时经过image to column展开后的数据排布可以看成矩阵计算过程中L1 Buffer上的NZ分形排布，因此该场景下可以使用LoadData（卷积数据搬运）接口进行矩阵计算所需2D格式数据的搬运，示意图如下所示：

![](../../../../figures/load3d_3d22d_demo.png)

#### 非转置场景

首先L1 Buffer->L0B Buffer通路不支持非转置场景；而对于L1 Buffer->L0A Buffer通路，根据不同的产品形态有不同的分形排布转换：

<!-- npu="A3,910b" id40 -->
- 针对Atlas A3 训练系列产品/Atlas A3 推理系列产品、Atlas A2 训练系列产品/Atlas A2 推理系列产品：

    LoadData（卷积数据搬运）会自动完成分形的变换，从L1 Buffer上的NZ分形排布转换成L0A Buffer中所需的ZZ分形排布，如下b16类型场景的示意图所示：

    ![](../../../../figures/load3d_l1tol0a_b16_nontrans.png)
<!-- end id40 -->

<!-- npu="950" id41 -->
- 针对Ascend 950PR/Ascend 950DT：

    L1 Buffer上的分形排布就是L0A Buffer中所需的NZ分形排布，LoadData（卷积数据搬运）不会进行分形排布转换，如下b16类型场景的示意图所示：

    ![](../../../../figures/load3d_l1tol0a_b16_nontrans_950.png)
<!-- end id41 -->

#### 转置场景

<!-- npu="A3,910b" id42 -->
- 针对Atlas A3 训练系列产品/Atlas A3 推理系列产品、Atlas A2 训练系列产品/Atlas A2 推理系列产品：

    LoadData（卷积数据搬运）仅在b16/b32数据类型下支持转置（enTranspose=True），按照类型详细说明：

    - 对于b16场景：每个分形大小是16×16。

        L1 Buffer->L0A Buffer通路下，转置场景下，除了每个分形矩阵自身会被转置，每个分形矩阵在整个二维特征图中的位置也会被转置，转置示意图如下：

        ![](../../../../figures/load3d_l1tol0a_b16_trans.png)

        L1 Buffer->L0B Buffer通路下，LoadData（卷积数据搬运）接口会自动完成转置，不需要配置enTranspose参数，enTranspose参数此时无效，转置示意图如下：

        ![](../../../../figures/load3d_l1tol0b_b16_trans.png)

    - 对于b32场景：每个分形大小是16×8。

        L1 Buffer->L0A Buffer通路下，需要配置enTranspose参数来开启转置功能，转置示意图如下：

        ![](../../../../figures/load3d_l1tol0a_b32_trans.png)

        L1 Buffer->L0B Buffer通路下，LoadData（卷积数据搬运）接口会自动完成转置，不需要配置enTranspose参数，enTranspose参数此时无效，转置示意图如下：

        ![](../../../../figures/load3d_l1tol0b_b32_trans.png)
<!-- end id42 -->

<!-- npu="950" id43 -->
- 针对Ascend 950PR/Ascend 950DT：

    LoadData（卷积数据搬运）在b8/b16/b32数据类型下均支持转置，其中，L1 Buffer->L0A Buffer通路通过开启enTranspose参数开启转置，L1 Buffer->L0B Buffer通路下会自动完成转置，不需要配置enTranspose参数，enTranspose参数此时无效。下面按照类型详细说明。

    - 对于b8场景：每个分形大小是16×32。

        L1 Buffer->L0A Buffer通路下，转置场景下，2个连续的16×32的分形拼接为一个32×32的方块矩阵，再进行转置并拆分为2个16×32的分形，转置示意图如下：

        ![](../../../../figures/load3d_l1tol0a_b8_trans_950.png)

        L1 Buffer->L0B Buffer通路下，LoadData（卷积数据搬运）接口会自动完成转置，不需要配置enTranspose参数，enTranspose参数此时无效，转置示意图如下：

        ![](../../../../figures/load3d_l1tol0b_b8_trans_950.png)

    - 对于b16场景：每个分形大小是16×16。

        L1 Buffer->L0A Buffer通路下，转置场景下，除了每个分形矩阵自身会被转置，每个分形矩阵在整个二维特征图中的位置也会被转置，转置示意图如下：

        ![](../../../../figures/load3d_l1tol0a_b16_trans_950.png)

        L1 Buffer->L0B Buffer通路下，LoadData（卷积数据搬运）接口会自动完成转置，不需要配置enTranspose参数，enTranspose参数此时无效，转置示意图如下：

        ![](../../../../figures/load3d_l1tol0b_b16_trans_950.png)

    - 对于b32场景：每个分形大小是16×8。

        L1 Buffer->L0A Buffer通路下，转置场景示意图如下：

        ![](../../../../figures/load3d_l1tol0a_b32_trans_950.png)

        L1 Buffer->L0B Buffer通路下，LoadData（卷积数据搬运）接口会自动完成转置，不需要配置enTranspose参数，enTranspose参数此时无效，转置示意图如下：

        ![](../../../../figures/load3d_l1tol0b_b32_trans_950.png)
<!-- end id43 -->

### Repeat Mode<a id="zh-cn_topic_0000002512171652_section131671145123912"></a>

LoadData（卷积数据搬运）接口可以通过配置mExtension和kExtension来完成多个方向的数据块搬运，我们把这里的多块数据的搬运看成一次迭代，支持通过配置repeat mode，能实现在M方向（height）或者K方向（width）进行repeat搬运，也就是说，调用一次LoadData（卷积数据搬运）接口可以完成多次迭代的数据搬运，如下图所示，其中repeatStride和repeatTime还有沿哪个方向repeat，我们通过[SetLoadDataRepeat](../矩阵搬入辅助配置接口/SetLoadDataRepeat.md)配置。

<!-- npu="A3,910b" id44 -->
针对如下产品型号：

<!-- npu="A3" id45 -->
Atlas A3 训练系列产品/Atlas A3 推理系列产品；
<!-- end id45 -->

<!-- npu="910b" id46 -->
Atlas A2 训练系列产品/Atlas A2 推理系列产品；
<!-- end id46 -->

M方向repeat示意图：

![](../../../../figures/load3d_repeatmode.png)

需要注意的是，当我们调用LoadData（卷积数据搬运）接口在M方向进行repeat搬运时，如果我们开启转置，则重复过程中的所有分块矩阵将被视为一个整体大矩阵，并按照我们前文所述的转置场景进行处理。以b32场景为例，下图给出了在M方向进行repeat搬运时的转置示意图，其中repeatMode=0，repeatStride=2，repeatTime=3。

![](../../../../figures/load3d_repeatmode_m.png)

K方向repeat示意图：

![](../../../../figures/load3d_repeatmode_k.png)

当我们调用LoadData（卷积数据搬运）接口在K方向进行repeat搬运时，如果我们开启转置，则重复过程中的所有分块矩阵也将被视为一个整体大矩阵，并按照我们前文所述的转置场景进行处理。以b32场景为例，下图给出了在K方向进行repeat搬运时的转置示意图，其中repeatMode=1，repeatStride=2，repeatTime=3。

![](../../../../figures/load3d_repeatmode_k_trans.png)
<!-- end id44 -->

<!-- npu="950" id47 -->
针对Ascend 950PR/Ascend 950DT：

**M方向repeat示意图：**

![](../../../../figures/load3d_repeatmode_m_950.png)

当我们调用LoadData（卷积数据搬运）接口在M方向进行repeat搬运时，如果我们开启转置，则重复过程中的所有分块矩阵将被视为一个整体大矩阵，并按照我们前文所述的转置场景进行处理。以b32场景为例，下图给出了在M方向进行repeat搬运时的转置示意图，其中repeatMode=0，repeatStride=2，repeatTime=3，dstStride=3。

![](../../../../figures/load3d_repeatmode_m_trans_950.png)

**K方向repeat示意图：**

![](../../../../figures/load3d_repeatmode_k_950.png)

当我们调用LoadData（卷积数据搬运）接口在K方向进行repeat搬运时，如果我们开启转置，则重复过程中的所有分块矩阵也将被视为一个整体大矩阵，并按照我们前文所述的转置场景进行处理。以b32场景为例，下图给出了在K方向进行repeat搬运时的转置示意图，其中repeatMode=1，repeatStride=2，repeatTime=3，dstStride=3。

![](../../../../figures/load3d_repeatmode_k_trans_950.png)
<!-- end id47 -->

### Feature Map、Pad属性描述寄存器设置<a id="zh-cn_topic_0000002512171652_section1881795134015"></a>

LoadData（卷积数据搬运）接口具备两个特殊寄存器：Feature Map属性描述和Pad属性描述寄存器，作为Image to Column展开时的参数。当我们进行矩阵计算时，出于高性能搬运考虑，往往会从GM搬运大块数据到L1 Buffer，即在L1 Buffer中缓存一块较大的数据，然后通过for循环进行L1 Buffer->L0 Buffer的搬入并进行Mmad计算。当通过LoadData（卷积数据搬运）接口进行L1 Buffer->L0 Buffer的搬入时，对于Feature Map属性和Pad属性我们通常只需要设置一次，不需要反复进行设置，因此我们提供了IsResetLoad3dConfig这个模板参数用于手动管理这两个属性，减少反复设置带来的指令性能开销。

IsResetLoad3dConfig结构定义如下：

```cpp
struct IsResetLoad3dConfig {
   bool isSetFMatrix = true;
   bool isSetPadding = true;
}; 
```

isSetFMatrix配置为true，表示在LoadData（卷积数据搬运）接口内部设置FeatureMap的属性描述（包括l1H、l1W、padList，参数介绍参考[表4](#zh-cn_topic_0000002512171652_table193501032193419)）；设置为false，表示该接口传入的FeatureMap的属性描述不生效，开发者需要通过[SetFmatrix](../矩阵搬入辅助配置接口/SetFmatrix.md)进行手动管理设置。

isSetPadding配置为true，表示在接口内部设置Pad属性描述（即padValue参数，参数介绍参考[表4](#zh-cn_topic_0000002512171652_table193501032193419)）；设置为false，表示该接口传入的Pad属性不生效，开发者需要通过[SetLoadDataPaddingValue](../矩阵搬入辅助配置接口/SetLoadDataPaddingValue.md)进行设置。

### LoadData（卷积数据搬运）接口数据格式说明<a id="zh-cn_topic_0000002512171652_section726316123184"></a>

要求输入的feature map和filter的格式是[NC1HWC0](../矩阵计算分形介绍/关键分形格式详解.md#zh-cn_topic_0000002545089965_section217615301084)，其中C0是最低维度而且C0是固定值为16（对于u8/s8类型为32），C1=C/C0。

为了简化场景，以下场景假设输入的feature map的channel为4，即Ci=4。输入feature maps在L1 Buffer（A1）中的形状为（Hi,Wi,Ci），经过LoadData处理后在L0A Buffer（A2）的数据形状为（Wo\*Ho, Hk\*Wk\*Ci）。其中Wo和Ho是卷积后输出的shape，Hk和Wk是filter的shape。

直观地来看，img2col的过程就是filter在feature map上扫过，将对应feature map的数据展开成输出数据的每一行的过程。filter首先在W方向上滑动Wo步，然后在H方向上走一步然后重复以上过程，最终输出Wo\*Ho行数据。下图中红色和黄色的数据分别代表第一行和第二行。数字表示原始输入数据，filter和输出数据三者之间的关联关系。可以看到，LoadData首先在输入数据的Ci维度搬运对应于00的4个数，然后搬运对应于01的四个数，最终这一行的大小为Hk\*Wk\*Ci即3\*3\*4=36个数。

对应的feature map格式如下图：

**图2** feature map格式<a id="zh-cn_topic_0000002512171652_fig8137131714319"></a>  

![](../../../../figures/feature-map格式.png "feature-map格式")

对应的filter的格式如下图：

其中n为filter的个数，可以看出维度排布为（Hk,Wk,Ci,n），但是需要注意的是下图的格式还需要根据Mmad中B矩阵的格式转换。

**图3** filter的格式<a id="zh-cn_topic_0000002512171652_fig1395194710496"></a>  

![](../../../../figures/filter的格式.png "filter的格式")

实际操作中，由于存储空间或者计算能力限制，我们通常会将整个卷积计算分块，一次只搬运并计算一小块数据。

**图4** 卷积计算分块<a id="zh-cn_topic_0000002512171652_fig5278110624"></a>  

![](../../../../figures/卷积计算分块.png "卷积计算分块")

对于L0A Buffer（A2）中的feature map来说有两种方案，水平分块和垂直分块。分别对应参数中repeatMode的0和1。

注：下图中的分形矩阵大小为4x4，实际应该为16x16（对于u8/s8类型为16x32）

repeatMode =0时，每次repeat会改变在filter窗口中读取数据点的位置，然后跳到下一个C0的位置。

**图5** repeatMode = 0时的filter窗口<a id="zh-cn_topic_0000002512171652_fig11710143294513"></a>  

![](../../../../figures/repeatMode-0-时的filter窗口.png "repeatMode-0-时的filter窗口")

repeatMode =1的时候filter窗口中读取数据的位置保持不变，每个repeat在feature map中前进C0个元素。

**图6** repeatMode = 1时的filter窗口<a id="zh-cn_topic_0000002512171652_fig1648315981717"></a>  

![](../../../../figures/repeatMode-1-时的filter窗口.png "repeatMode-1-时的filter窗口")

## 调用示例<a id="zh-cn_topic_0000002512171652_section088124295117"></a>

如下示例中：A矩阵转置，shape为[k,m]，b32数据类型下，GM->L1 Buffer->L0A Buffer数据排布分别是ND、NZ和ZZ。但是L1 Buffer->L0A Buffer的时候，无法调用LoadDataWithTranspose指令对其进行转置，因为在K轴方向两个连续的分形并不能合并为一个16\*16的方块，示意图如下：

![](../../../../figures/load3d_l1tol0a_b32demo.png)

此时可以调用LoadData（卷积数据搬运）接口实现A矩阵转置。调用LoadData（卷积数据搬运） v2接口时，在写入L0A Buffer之前会先分别将A矩阵高度和宽度轴向16、8对齐，接着该指令会将整个A矩阵进行转置并且每一个分形也转置，最终写入到L0A Buffer的A矩阵是ZZ排布

示例代码片段如下，仅展示样例中的部分代码，完整示例请参考：[load_data_l12l0样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0)。

```cpp
// LoadData（卷积数据搬运） v2接口完成img2col的过程，可知img2col后A矩阵高度为ho * wo，根据ho和wo的计算公式，代入卷积核宽度、卷积核滑动步长、卷积核膨胀系数等参数可知：A矩阵的高度为CeilAlign(k, fractalShape[0])；img2col后A矩阵宽度为ci * kh * kw，代入kh=1，kw=1，可知A矩阵的宽度为CeilAlign(m, fractalShape[1])。最后，配置loadDataParams.enTranspose = true，将整个A矩阵转置并且将其中每一个分形转置
// 使用LoadData（卷积数据搬运）接口，实现NZ2ZZ
AscendC::LoadData3DParamsV2<T> loadDataParams;
// 源操作数height
loadDataParams.l1H = 1;
// 源操作数width
loadDataParams.l1W = CeilAlign(k, fractalShape[0]);
// 源操作数的通道数，
// img2col的结果矩阵高度为ho * wo，根据ho和wo的计算公式，代入卷积核宽度、卷积核滑动步长、卷积核膨胀系数等参数可知：ho * wo = loadDataParams.l1H * loadDataParams.l1W
// img2col的结果矩阵宽度为ci * kh * kw，代入kh=1，kw=1，可知结果矩阵的宽度为ci=loadDataParams.channelSize = m
loadDataParams.channelSize = CeilAlign(m, fractalShape[1]);
// 该指令在目的操作数width维度的传输长度，如果不覆盖最右侧的分形，对于half类型，应为16的倍数，对于int8_t/uint8_t应为32的倍数；覆盖的情况则无倍数要求。
loadDataParams.kExtension = CeilAlign(m, fractalShape[1]);
// 该指令在目的操作数height维度的传输长度，如果不覆盖最下侧的分形，对于half/int8_t/uint8_t，应为16的倍数；覆盖的情况则无倍数要求。
loadDataParams.mExtension = CeilAlign(k, fractalShape[1] * fractalNum);
// 卷积核在源操作数width维度滑动的步长
loadDataParams.strideW = 1;
// 卷积核在源操作数height维度滑动的步长
loadDataParams.strideH = 1;
// 卷积核width
loadDataParams.filterW = 1;
// 卷积核height
loadDataParams.filterH = 1;
// 卷积核width膨胀系数
loadDataParams.dilationFilterW = 1;
// 卷积核height膨胀系数
loadDataParams.dilationFilterH = 1;
loadDataParams.filterSizeW = false;
loadDataParams.filterSizeH = false;
loadDataParams.enTranspose = true;
loadDataParams.fMatrixCtrl = false;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```

关于如何使用LoadData（卷积数据搬运）进行2D数据搬运，请参考：[load_data_l12l0样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0)中的对应场景。
