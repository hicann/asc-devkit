# LoadData（BitMode卷积数据搬运）<a id="ZH-CN_TOPIC_0000002517284408"></a>

## 产品支持情况<a id="section1550532418810"></a>

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

## 功能说明<a id="section618mcpsimp"></a>

LoadData（BitMode卷积数据搬运）用于完成image to column操作，将多维feature map转为二维矩阵。支持的数据通路为：L1 Buffer->L0A Buffer、L1 Buffer->L0B Buffer。

本接口与LoadData（卷积数据搬运）接口的差异在于参数的传入方式不同，本接口传入的是一个联合体结构Load3DBitModeParam。

## 函数原型<a id="section620mcpsimp"></a>

```cpp
template <TPosition Dst, TPosition Src, typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const Load3DBitModeParam& loadDataParams)
```

## 参数说明<a id="section622mcpsimp"></a>

**表1** 模板参数说明<a name="table07381635103112"></a>

| 参数名称 | 含义 |
| ---------- | ---------- |
| T | 源操作数和目的操作数的数据类型。 |
| Src | 源操作数存储的逻辑位置（TPosition），仅本接口使用。 |
| Dst | 目的操作数存储的逻辑位置（TPosition），仅本接口使用。 |

**表2** 通用参数说明<a name="table18368155193919"></a>

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，类型为LocalTensor。<br>数据连续排列顺序由目的操作数所在TPosition决定，具体约束如下：<br>&nbsp;&nbsp;&bull; A2：ZZ格式/NZ格式；<br>&nbsp;&nbsp;&bull; B2：ZN格式；<br>&nbsp;&nbsp;&bull; A1/B1：无格式要求，一般情况下为NZ格式。 |
| src | 输入 | 源操作数，类型为LocalTensor。<br>数据类型需要与dst保持一致。 |
| loadDataParams | 输入 | LoadData参数结构体，类型为：<br>&nbsp;&nbsp;&bull; Load3DBitModeParam，具体参考[表3](#table106611666584)。<br>上述结构体参数定义请参考`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_mm.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。 |

**表3** Load3DBitModeParam类参数说明<a name="table106611666584"></a>

| 参数名称 | 含义 |
| ---------- | ---------- |
| config0 | uint64_t类型，与Load3DBitModeConfig0位域（bit-field）结构体类型参数config0BitMode组成联合体（union），初始化为0，可以使用类对象的GetConfig0()函数获取其值。 |
| config0BitMode | Load3DBitModeConfig0位域（bit-field）结构体类型，参数参考[表4](#table184321224173)，与config0组成联合体（union）。 |
| config1 | uint64_t类型，与Load3DBitModeConfig1位域（bit-field）结构体类型参数config1BitMode组成联合体（union），初始化为0，可以使用类对象的GetConfig1()函数获取其值。 |
| config1BitMode | Load3DBitModeConfig1位域（bit-field）结构体类型，参数参考[表5](#table87491086815)，与config1组成联合体（union）。 |

Load3DBitModeParam类参数设计思想说明：

联合体（union）是一种特殊的数据结构，允许在相同的内存位置存储不同的数据类型。union的所有成员共享同一块内存空间，大小由最大成员决定，同一时间只能使用一个成员。

位域（bit-field）是一种特殊的类成员，允许精确控制结构体中成员变量所占用的内存位数。结构体中成员变量从上到下对应内存中从低位到高位。

Load3DBitModeParam类使用union与bit-field方法，采用bit位表达参数类型，使用bit-field结构体自动处理入参的bit位数，并利用union的特性实现多参数融合传递，仅需传递一个入参即可包含全部所需信息，对应底层接口仅需要接收一个参数。同时，当需要修改参数中某一bit位的值时，仅需要通过循环和位运算即可实现，不需要重新传入参数。

与使用LoadData3DParamsV2结构体的LoadData（卷积数据搬运）接口相比，减少了scalar计算，实现性能提升。

Load3DBitModeParam类可以直接使用LoadData3DParamsV2结构体类型对象初始化：

```cpp
template <typename T>
__aicore__ inline Load3DBitModeParam(const LoadData3DParamsV2<T> &loadData3DParams_);
```

也可以使用各参数的Set函数修改参数值，并且由于使用了联合体，还可以对config0和config1直接进行逐bit位修改来修改参数。

**表4** Load3DBitModeConfig0结构体参数说明<a name="table184321224173"></a>

| 参数名称 | 含义 |
| ---------- | ---------- |
| kStep | 该指令在目的操作数width维度的传输长度，如果不覆盖最右侧的分形，对于half类型，应为16的倍数，对于int8_t/uint8_t应为32的倍数；覆盖的情况则无倍数要求。取值范围：kStep∈[1, 65535]。<br>（与[LoadData（卷积数据搬运）](LoadData_3D.md)中的kExtension含义相同）。<br>该参数是位域结构体的最低位参数，占用16bit，可以使用Load3DBitModeParam类对象的SetKExtension()函数设置其值，使用GetKExtension()函数获取其值。 |
| mStep | 该指令在目的操作数height维度的传输长度，如果不覆盖最下侧的分形，对于half/int8_t/uint8_t，应为16的倍数；覆盖的情况则无倍数要求。取值范围：mStep∈[1, 65535]。<br>（与[LoadData（卷积数据搬运）](LoadData_3D.md)中的mExtension含义相同）。<br>该参数是位域结构体的第二低位参数，占用16bit，可以使用Load3DBitModeParam类对象的SetMExtension()函数设置其值，使用GetMExtension()函数获取其值。 |
| kPos | 该指令在目的操作数width维度的起点，对于half类型，应为16的倍数，对于int8_t/uint8_t应为32的倍数。取值范围[0, 65535]。默认为0。<br>（与[LoadData（卷积数据搬运）](LoadData_3D.md)中的kStartPt含义相同）。<br>该参数是位域结构体的第三低位参数，占用16bit，可以使用Load3DBitModeParam类对象的SetKStartPt()函数设置其值，使用GetKStartPt()函数获取其值。 |
| mPos | 该指令在目的操作数height维度的起点，取值范围[0, 32767]。默认为0。<br>（与[LoadData（卷积数据搬运）](LoadData_3D.md)中的mStartPt含义相同）。<br>该参数是位域结构体的最高位参数，占用16bit，可以使用Load3DBitModeParam类对象的SetMStartPt()函数设置其值，使用GetMStartPt()函数获取其值。 |

**表5** Load3DBitModeConfig1结构体参数说明<a name="table87491086815"></a>

| 参数名称 | 含义 |
| ---------- | ---------- |
| strideW | 卷积核在源操作数width维度滑动的步长，取值范围：strideW∈[1, 63]。<br>（与[LoadData（卷积数据搬运）](LoadData_3D.md)中的strideW含义相同）。<br>该参数是位域结构体的最低位参数，占用6bit，可以使用Load3DBitModeParam类对象的SetStrideW()函数设置其值，使用GetStrideW()函数获取其值。 |
| strideH | 卷积核在源操作数height维度滑动的步长，取值范围：strideH∈[1, 63]。<br>（与[LoadData（卷积数据搬运）](LoadData_3D.md)中的strideH含义相同）<br>该参数是位域结构体的第二低位参数，占用6bit，可以使用Load3DBitModeParam类对象的SetStrideH()函数设置其值，使用GetStrideH()函数获取其值。 |
| Wk | 卷积核width，取值范围：Wk∈[1, 255]。<br>（与[LoadData（卷积数据搬运）](LoadData_3D.md)中的filterW含义相同）。<br>该参数是位域结构体的第三低位参数，占用8bit，可以使用Load3DBitModeParam类对象的SetFilterW()函数设置其值，使用GetFilterW()函数获取其值。 |
| Hk | 卷积核height，取值范围：Hk∈[1, 255]。<br>（与[LoadData（卷积数据搬运）](LoadData_3D.md)中的filterH含义相同）。<br>该参数是位域结构体的第四低位参数，占用8bit，可以使用Load3DBitModeParam类对象的SetFilterH()函数设置其值，使用GetFilterH()函数获取其值。 |
| dilationW | 卷积核width膨胀系数，取值范围：dilationW∈[1, 255]。<br>（与[LoadData（卷积数据搬运）](LoadData_3D.md)中的dilationFilterW含义相同）<br>该参数是位域结构体的第五低位参数，占用8bit，可以使用Load3DBitModeParam类对象的SetDilationFilterW()函数设置其值，使用GetDilationFilterW()函数获取其值。 |
| dilationH | 卷积核height膨胀系数，取值范围：dilationH∈[1, 255]。<br>（与[LoadData（卷积数据搬运）](LoadData_3D.md)中的dilationFilterH含义相同）。<br>该参数是位域结构体的第六低位参数，占用8bit，可以使用Load3DBitModeParam类对象的SetDilationFilterH()函数设置其值，使用GetDilationFilterH()函数获取其值。 |
| filterW | 是否在filterW的基础上将卷积核width增加256个元素。true，增加；false，不增加。<br>（与[LoadData（卷积数据搬运）](LoadData_3D.md)中的filterSizeW含义相同）。<br>该参数是位域结构体的第七低位参数，占用1bit，可以使用Load3DBitModeParam类对象的SetFilterSizeW()函数设置其值，使用GetFilterSizeW()函数获取其值。 |
| filterH | 是否在filterH的基础上将卷积核height增加256个元素。true，增加；false，不增加。<br>（与[LoadData（卷积数据搬运）](LoadData_3D.md)中的filterSizeH含义相同）。<br>该参数是位域结构体的第八低位参数，占用1bit，可以使用Load3DBitModeParam类对象的SetFilterSizeH()函数设置其值，使用GetFilterSizeH()函数获取其值。 |
| transpose | 是否启用转置功能，对整个目标矩阵进行转置，支持数据类型为bool，仅在目的TPosition为A2，且源操作数为half类型时有效。默认为false。<br>&nbsp;&nbsp;&bull; true：启用<br>&nbsp;&nbsp;&bull; false：不启用<br>（与[LoadData（卷积数据搬运）](LoadData_3D.md)中的enTranspose含义相同）。<br>该参数是位域结构体的第九低位参数，占用1bit，可以使用Load3DBitModeParam类对象的SetTranspose()函数设置其值，使用GetTranspose()函数获取其值。 |
| fmatrixCtrl | 表示LoadData3DV2指令从左矩阵还是右矩阵获取FeatureMap的属性描述，与SetFmatrix配合使用，当前只支持设置为false，默认值为false。<br>&nbsp;&nbsp;&bull; true：从右矩阵中获取FeatureMap的属性描述；<br>&nbsp;&nbsp;&bull; false：从左矩阵中获取FeatureMap的属性描述。<br>（与[LoadData（卷积数据搬运）](LoadData_3D.md)中的fMatrixCtrl含义相同）。<br>该参数是位域结构体的第十低位参数，占用1bit，可以使用Load3DBitModeParam类对象的SetFmatrixCtrl()函数设置其值，使用GetFmatrixCtrl()函数获取其值。 |
| sizeChannel | 源操作数的通道数，取值范围：sizeChannel∈[1, 63]。<br>sizeChannel的取值要求为：对于uint32_t/int32_t/float，sizeChannel可取值为4，N\*8，N\*8+4；对于half/bfloat16，sizeChannel可取值为4，8，N\*16，N\*16+4，N\*16+8；对于int8_t/uint8_t，sizeChannel可取值为4，8，16，32\*N，N\*32+4，N\*32+8，N\*32+16；对于int4b_t，sizeChannel可取值为8，16，32，N \* 64，N\*64+8，N\*64+16，N\*64+32。N为正整数。<br>（与[LoadData（卷积数据搬运）](LoadData_3D.md)中的channelSize含义相同）。<br>该参数是位域结构体的最高位参数，占用16bit，可以使用Load3DBitModeParam类对象的SetChannelSize()函数设置其值，使用GetChannelSize()函数获取其值。 |

## 数据类型

支持数据类型为：int8_t、uint8_t、hifloat8_t、fp8_e5m2_t、fp8_e4m3fn_t、half、bfloat16_t、int32_t、uint32_t、float。

## 返回值说明<a id="section640mcpsimp"></a>

无

## 约束说明<a id="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。

## 调用示例<a id="section6461234123118"></a>

示例代码片段如下：

```cpp
// featureMapA1为half类型、位于L1 Buffer，featureMapA2为half类型、位于L0A Buffer
uint16_t H = 4, W = 4;
uint8_t Kh = 2, Kw = 2;
uint16_t C0 = 16;
uint8_t dilationH = 2, dilationW = 2;
uint8_t padTop = 1, padBottom = 1, padLeft = 1, padRight = 1;
uint8_t strideH = 1, strideW = 1;
uint8_t padList[4] = {padLeft, padRight, padTop, padBottom};

// 使用LoadData3DParamsV2结构体对象初始化Load3DBitModeParam
// 构造参数顺序：padList, l1H, l1W, sizeChannel, kExtension, mExtension, kStartPt, mStartPt,
//             strideW, strideH, filterW, filterH, dilationFilterW, dilationFilterH,
//             enTranspose, enSmallK, padValue, filterSizeW, filterSizeH, fMatrixCtrl
AscendC::LoadData3DParamsV2<half> param = {
    padList, H, W, C0, C0, static_cast<uint16_t>(H * W), 0, 0,
    strideW, strideH, Kw, Kh, dilationW, dilationH,
    false, false, (half)0, false, false, false};
AscendC::Load3DBitModeParam paramBitMode(param);
AscendC::LoadData<AscendC::TPosition::A2, AscendC::TPosition::A1, half>(featureMapA2, featureMapA1, paramBitMode);
```
