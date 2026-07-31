# SetFmatrixBitMode<a name="ZH-CN_TOPIC_0000002517448448"></a>

## 产品支持情况<a name="section1550532418810"></a>

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

## 功能说明<a name="section618mcpsimp"></a>

用于调用[LoadData（卷积数据搬运）](../矩阵计算的搬入/LoadData_3D.md)时设置FeatureMap的属性描述。LoadData（卷积数据搬运）的模板参数isSetFMatrix设置为false时，表示LoadData（卷积数据搬运）传入的FeatureMap的属性（包括l1H、l1W、padList，参数介绍参考[表3 LoadData3DParamsV1结构体内参数说明](../矩阵计算的搬入/LoadData_3D.md#zh-cn_topic_0000002512171652_table679014222918)、[表4 LoadData3DParamsV2结构体内参数说明](../矩阵计算的搬入/LoadData_3D.md#zh-cn_topic_0000002512171652_table193501032193419)）将不生效，开发者需要通过该接口进行设置。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetFmatrix(const SetFMatrixBitModeParams& param, const FmatrixMode& fmatrixMode)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| --------- | ---------- | ------ |
| fmatrixMode | 输入 | 用于控制LoadData指令从left还是right寄存器获取信息。FmatrixMode类型，定义如下。当前只支持FMATRIX\_LEFT，左右矩阵均使用该配置。<br><pre>enum class FmatrixMode : uint8_t {<br>    FMATRIX_LEFT = 0,<br>    FMATRIX_RIGHT = 1,<br>};</pre> |
| param | 输入 | 类型为SetFMatrixBitMode，具体参考[表2](#table85031523118)。 |

<a name="table85031523118"></a>
**表2** SetFMatrixBitMode类参数说明

| 参数名称 | 含义 |
| --------- | --------- |
| config0 | uint64_t类型，与SetFMatrixBitModeConfig0位域（bit-field）结构体类型参数config0BitMode组成联合体（union），初始化为0，可以使用类对象的GetConfig0()函数获取其值。 |
| config0BitMode | SetFMatrixBitModeConfig0位域（bit-field）结构体类型，参数参考[表3](#table1162220101434)，与config0组成联合体（union）。 |

SetFMatrixBitMode类参数设计思想说明：

联合体（union）是一种特殊的数据结构，允许在相同的内存位置存储不同的数据类型。union的所有成员共享同一块内存空间，大小由最大成员决定，同一时间只能使用一个成员。

位域（bit-field）是一种特殊的类成员，允许精确控制结构体中成员变量所占用的内存位数。结构体中成员变量从上到下对应内存中从低位到高位。

SetFMatrixBitMode类使用union与bit-field方法，采用bit位表达参数类型，使用bit-field结构体自动处理入参的bit位数，并利用union的特性实现多参数融合传递，仅需传递一个入参即可包含全部所需信息，对应底层接口仅需要接收一个参数。同时，当需要修改参数中某一bit位的值时，仅通过循环和位运算即可实现，不需要重新传入参数，减少了scalar计算，实现性能提升。

SetFMatrixBitMode类可以直接使用[LoadData3DParamsV2结构体](../矩阵计算的搬入/LoadData_3D.md#zh-cn_topic_0000002512171652_table193501032193419)类型对象初始化：

```cpp
template <typename T>
__aicore__ inline SetFMatrixBitModeParams(const LoadData3DParamsV2<T> &loadData3DParams_);
```

也可以使用各参数的Set函数修改参数值，并且由于使用了联合体，还可以对config0直接进行逐bit位修改来修改参数。

<a name="table1162220101434"></a>
**表3** SetFMatrixBitModeConfig0结构体参数说明

| 参数名称 | 含义 |
| --- | --- |
| l1H | 源操作数height，取值范围：l1H∈[1, 32767]。该参数是位域结构体的最低位参数，占用16bit，可以使用SetFMatrixBitMode类对象的SetL1H()函数设置其值。 |
| l1W | 源操作数width，取值范围：l1W∈[1, 32767]。该参数是位域结构体的第二低位参数，占用16bit，可以使用SetFMatrixBitMode类对象的SetL1W()函数设置其值。 |
| padList0 | 对应[表LoadData3DParamsV2结构体内参数说明](../矩阵计算的搬入/LoadData_3D.md#zh-cn_topic_0000002512171652_table193501032193419)中padding列表中的padding_left值，取值范围：[0,255]。默认为0。该参数是位域结构体的第三低位参数，占用8bit，可以使用SetFMatrixBitMode类对象的SetPadList()函数设置其值。 |
| padList1 | 对应[表1](SetFmatrix.md#table8955841508)中padding列表中的padding_right值，取值范围：[0,255]。默认为0。该参数是位域结构体的第四低位参数，占用8bit，可以使用SetFMatrixBitMode类对象的SetPadList()函数设置其值。 |
| padList2 | 对应[表1](SetFmatrix.md#table8955841508)中padding列表中的padding_top值，取值范围：[0,255]。默认为0。该参数是位域结构体的第五低位参数，占用8bit，可以使用SetFMatrixBitMode类对象的SetPadList()函数设置其值。 |
| padList3 | 对应[表1](SetFmatrix.md#table8955841508)中padding列表中的padding_bottom值，取值范围：[0,255]。默认为0。该参数是位域结构体的最高位参数，占用8bit，可以使用SetFMatrixBitMode类对象的SetPadList()函数设置其值。 |

## 返回值说明

无

## 约束说明<a name="section633mcpsimp"></a>

- 该接口需要配合[LoadData（卷积数据搬运）](../矩阵计算的搬入/LoadData_3D.md)接口一起使用，需要在[LoadData（卷积数据搬运）](../矩阵计算的搬入/LoadData_3D.md)接口之前调用。
- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。

## 调用示例<a name="section642mcpsimp"></a>

```cpp
AscendC::TPipe pipe;

AscendC::TQue<AscendC::TPosition::A1, 1> inQueueFmA1;
AscendC::TQue<AscendC::TPosition::A2, 1> inQueueFmA2;
// weight queue
AscendC::TQue<AscendC::TPosition::B1, 1> inQueueWeB1;
AscendC::TQue<AscendC::TPosition::B2, 1> inQueueWeB2;
pipe.InitBuffer(inQueueFmA1, 1, featureMapA1Size * sizeof(fmap_T));
pipe.InitBuffer(inQueueFmA2, 1, featureMapA2Size * sizeof(fmap_T));
pipe.InitBuffer(inQueueWeB1, 1, weightA1Size * sizeof(weight_T));
pipe.InitBuffer(inQueueWeB2, 1, weightB2Size * sizeof(weight_T));
pipe.InitBuffer(outQueueCO1, 1, dstCO1Size * sizeof(dstCO1_T));

AscendC::LocalTensor<fmap_T> featureMapA1 = inQueueFmA1.DeQue<fmap_T>();
AscendC::LocalTensor<weight_T> weightB1 = inQueueWeB1.DeQue<weight_T>();
AscendC::LocalTensor<fmap_T> featureMapA2 = inQueueFmA2.AllocTensor<fmap_T>();
AscendC::LocalTensor<weight_T> weightB2 = inQueueWeB2.AllocTensor<weight_T>();
uint16_t channelSize = 32;
uint16_t H = 4, W = 4;
uint8_t Kh = 2, Kw = 2;
uint16_t Cout = 16;
uint16_t C0, C1;
uint8_t dilationH = 2, dilationW = 2;

uint8_t padList[PAD_SIZE] = {0, 0, 0, 0};
AscendC::SetFmatrix(H, W, padList, FmatrixMode::FMATRIX_LEFT);
/*  
SetFMatrixBitModeParams param;
param.SetL1H(H);
param.SetL1W(W);
param.SetPadList(padList);
AscendC::SetFmatrix(param, FmatrixMode::FMATRIX_LEFT);
*/ 
AscendC::SetLoadDataPaddingValue(0);
AscendC::SetLoadDataRepeat({0, 1, 0});
AscendC::SetLoadDataBoundary((uint32_t)0);
static constexpr AscendC::IsResetLoad3dConfig LOAD3D_CONFIG = {false,false};
AscendC::LoadData<fmap_T, LOAD3D_CONFIG>(featureMapA2, featureMapA1,
    { padList, H, W, channelSize, k, howoRound, 0, 0, 1, 1, Kw, Kh, dilationW, dilationH, false, false, 0 });
AscendC::LoadData(weightB2, weightB1, { 0, weRepeat, 1, 0, 0, false, 0 });

inQueueFmA2.EnQue<fmap_T>(featureMapA2);
inQueueWeB2.EnQue<weight_T>(weightB2);
inQueueFmA1.FreeTensor(featureMapA1);
inQueueWeB1.FreeTensor(weightB1);
```
