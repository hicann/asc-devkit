# LoadData（BitMode 2D矩阵搬运）<a id="ZH-CN_TOPIC_0000002517418694"></a>

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
- Atlas 推理系列产品 AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品 Vector Core：不支持
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

负责完成普通矩阵计算所需的2D格式数据的搬运，以大小为512字节的数据分形为单位进行搬运，支持的数据通路为：L1 Buffer->L0A Buffer、L1 Buffer->L0B Buffer。

本接口与LoadData（2D矩阵搬运V2）接口的差异在于参数的传入方式不同，本接口传入的是一个联合体结构Load2DBitModeParam。

## 函数原型<a id="section620mcpsimp"></a>

```cpp
template <TPosition Dst, TPosition Src, typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src,const Load2DBitModeParam& loadDataParams)
```

## 参数说明<a id="section622mcpsimp"></a>

**表 1** 模板参数说明

| 参数名称 | 含义 |
| ---------- | ---------- |
| T | 源操作数和目的操作数的数据类型。 |
| Src | 源操作数存储的逻辑位置（TPosition），支持的取值为A1和B1，仅本接口使用。 |
| Dst | 目的操作数存储的逻辑位置（TPosition），支持的取值为A2和B2，仅本接口使用。 |

**表 2** 通用参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，类型为LocalTensor。<br>数据连续排列顺序由目的操作数所在TPosition决定，具体约束如下：<br>&nbsp;&nbsp;&bull; A2：ZZ格式/NZ格式；对应的分形大小为16 \* (32字节 / sizeof(T))。<br>&nbsp;&nbsp;&bull; B2：ZN格式；对应的分形大小为 (32字节 / sizeof(T))  \* 16。<br>&nbsp;&nbsp;&bull; A1/B1：无格式要求，一般情况下为NZ格式。NZ格式下，对应的分形大小为16 * (32字节 / sizeof(T))。 |
| src | 输入 | 源操作数，类型为LocalTensor。<br>数据类型需要与dst保持一致。 |
| loadDataParams | 输入 | LoadData参数结构体，类型为Load2DBitModeParam，具体参考[表3](#table10539223195311)。<br>上述结构体参数定义请参考`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_mm.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。 |

**表 3** Load2DBitModeParam类参数说明<a id="table10539223195311"></a>

| 参数名称 | 含义 |
| ---------- | ---------- |
| config0 | uint64_t类型，与Load2DBitModeConfig0位域（bit-field）结构体类型参数config0BitMode组成联合体（union），初始化为0，可以使用类对象的GetConfig0()函数获取其值。 |
| config0BitMode | Load2DBitModeConfig0位域（bit-field）结构体类型，参数参考[表4](#table4109172132317)，与config0组成联合体（union）。 |
| config1 | uint64_t类型，与Load2DBitModeConfig1位域（bit-field）结构体类型参数config1BitMode组成联合体（union），初始化为0，可以使用类对象的GetConfig1()函数获取其值。 |
| config1BitMode | Load2DBitModeConfig1位域（bit-field）结构体类型，参数参考[表5](#table122891852142311)，与config1组成联合体（union）。 |
| ifTranspose | 是否启用转置功能，对每个分形矩阵进行转置，默认为false。含义与LoadData2DParamsV2结构体中的同名参数含义相同，具体参考[LoadData（2D矩阵搬运）](LoadData_2D.md)。<br>&nbsp;&nbsp;&bull; true：启用<br>&nbsp;&nbsp;&bull; false：不启用<br>开启转置功能时，支持的数据类型约束如下：<br>源操作数、目的操作数支持b4、b8、b16、b32数据类型。 |

Load2DBitModeParam类参数设计思想说明：

联合体（union）是一种特殊的数据结构，允许在相同的内存位置存储不同的数据类型。union的所有成员共享同一块内存空间，大小由最大成员决定，同一时间只能使用一个成员。

位域（bit-field）是一种特殊的类成员，允许精确控制结构体中成员变量所占用的内存位数。结构体中成员变量从上到下对应内存中从低位到高位。

Load2DBitModeParam类使用union与bit-field方法，采用bit位表达参数类型，使用bit-field结构体自动处理入参的bit位数，并利用union的特性实现多参数融合传递，仅需传递一个入参即可包含全部所需信息，对应底层接口仅需要接收一个参数。同时，当需要修改参数中某一bit位的值时，仅需要通过循环和位运算即可实现，不需要重新传入参数。

与使用LoadData2DParamsV2结构体的LoadData接口相比，减少了scalar计算，实现性能提升。

Load2DBitModeParam类可以直接使用LoadData2DParamsV2结构体类型对象初始化：

```cpp
LoadData2DParamsV2 loadDataParams;
loadDataParams.mStartPosition = 0;
loadDataParams.kStartPosition = 0;
loadDataParams.mStep = 2;
loadDataParams.kStep = 2;
loadDataParams.srcStride = 2;
loadDataParams.dstStride = 2;
loadDataParams.sid = 0;
loadDataParams.ifTranspose = false;
Load2DBitModeParam params(loadDataParams);  // 直接使用LoadData2DParamsV2结构体类型对象初始化
```

也可以使用各参数的Set函数修改参数值，并且由于使用了联合体，还可以对config0和config1直接进行逐bit位修改来修改参数。

**表 4** Load2DBitModeConfig0结构体参数说明<a name="table4109172132317"></a>

| 参数名称 | 含义 |
| ---------- | ---------- |
| mStartPosition | 以M*K矩阵为例，源矩阵M轴方向的起始位置，单位为16个元素。<br>该参数是位域结构体的最低位参数，占用16bit，可以使用Load2DBitModeParam类对象的SetMStartPosition()函数设置其值，使用GetMStartPosition()函数获取其值，具体参考[表6](#table1123714295457)。 |
| kStartPosition | 以M*K矩阵为例，源矩阵K轴方向的起始位置，单位为32字节。<br>该参数是位域结构体的第二低位参数，占用16bit，可以使用Load2DBitModeParam类对象的SetKStartPosition()函数设置其值，使用GetKStartPosition()函数获取其值，具体参考[表6](#table1123714295457)。 |
| mStep | 以M*K矩阵为例，源矩阵M轴方向搬运长度，单位为16个元素。取值范围：mStep∈[0, 255]。<br>通过ifTranspose参数启用转置功能时，mStep除需满足 [0, 255]的取值范围外，还需满足以下额外约束：<br>&nbsp;&nbsp;&bull; 当数据类型为b4时，mStep必须是4的倍数；<br>&nbsp;&nbsp;&bull; 当数据类型为b8时，mStep必须是2的倍数；<br>&nbsp;&nbsp;&bull; 当数据类型为b16时，mStep必须是1的倍数；<br>&nbsp;&nbsp;&bull; 当数据类型为b32时，mStep无额外约束。<br>该参数是位域结构体的第三低位参数，占用8bit，可以使用Load2DBitModeParam类对象的SetMStep()函数设置其值，使用GetMStep()函数获取其值，具体参考[表6](#table1123714295457)。 |
| kStep | 以M*K矩阵为例，源矩阵K轴方向搬运长度，单位为32字节。取值范围：kStep∈[0, 255]。<br>通过ifTranspose参数启用转置功能时，kStep除需满足[0, 255]的取值范围外，还需满足以下额外约束：<br>&nbsp;&nbsp;&bull; 当数据类型为b4、b8或b16时，kStep没有额外约束；<br>&nbsp;&nbsp;&bull; 当数据类型为b32时，kStep必须是2的倍数。<br>该参数是位域结构体的最高位参数，占用8bit，可以使用Load2DBitModeParam类对象的SetKStep()函数设置其值，使用GetKStep()函数获取其值，具体参考[表6](#table1123714295457)。 |

Load2DBitModeConfig0结构体参数的含义与LoadData2DParamsV2结构体中的同名参数含义相同，具体参考[LoadData（2D矩阵搬运）](LoadData_2D.md)。

**表 5** Load2DBitModeConfig1结构体参数说明<a name="table122891852142311"></a>

| 参数名称 | 含义 |
| ---------- | ---------- |
| srcStride | 以M*K矩阵为例，源矩阵K方向前一个分形起始地址与后一个分形起始地址的间隔，单位：512字节。<br>该参数是位域结构体的最低位参数，占用16bit，可以使用Load2DBitModeParam类对象的SetSrcStride()函数设置其值，使用GetSrcStride()函数获取其值，具体参考[表6](#table1123714295457)。 |
| dstStride | 以M*K矩阵为例，目标矩阵K方向前一个分形起始地址与后一个分形起始地址的间隔，单位：512字节。<br>该参数是位域结构体的最高位参数，占用16bit，可以使用Load2DBitModeParam类对象的SetDstStride()函数设置其值，使用GetDstStride()函数获取其值，具体参考[表6](#table1123714295457)。 |

Load2DBitModeConfig1结构体参数的含义与LoadData2DParamsV2结构体中的同名参数含义相同，具体参考[LoadData（2D矩阵搬运）](LoadData_2D.md)。

**表 6** Load2DBitModeParam类成员函数说明<a name="table1123714295457"></a>

| 函数名称 | 功能 |
| ---------- | ---------- |
| void SetMStartPosition(uint32_t mStartPosition_) | 将Load2DBitModeConfig0结构体参数mStartPosition的值设置为mStartPosition_。 |
| void SetKStartPosition(uint32_t kStartPosition_) | 将Load2DBitModeConfig0结构体参数kStartPosition的值设置为kStartPosition_。 |
| void SetMStep(uint16_t mStep_) | 将Load2DBitModeConfig0结构体参数mStep的值设置为mStep_。 |
| void SetKStep(uint16_t kStep_) | 将Load2DBitModeConfig0结构体参数kStep的值设置为kStep_。 |
| void SetSrcStride(int32_t srcStride_) | 将Load2DBitModeConfig1结构体参数srcStride的值设置为srcStride_。 |
| void SetDstStride(uint16_t dstStride_) | 将Load2DBitModeConfig1结构体参数dstStride的值设置为dstStride_。 |
| uint32_t GetMStartPosition() const | 获取Load2DBitModeConfig0结构体参数mStartPosition的值。 |
| uint32_t GetKStartPosition() const | 获取Load2DBitModeConfig0结构体参数kStartPosition的值。 |
| uint16_t GetMStep() const | 获取Load2DBitModeConfig0结构体参数mStep的值。 |
| uint16_t GetKStep() const | 获取Load2DBitModeConfig0结构体参数kStep的值。 |
| int32_t GetSrcStride() const | 获取Load2DBitModeConfig1结构体参数srcStride的值。 |
| uint16_t GetDstStride() const | 获取Load2DBitModeConfig1结构体参数dstStride的值。 |

## 数据类型

支持数据类型为：int8_t、uint8_t、fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e5m2_t、fp8_e4m3fn_t、half、bfloat16_t、int32_t、uint32_t、float。

## 返回值说明<a id="section640mcpsimp"></a>

无

## 约束说明<a id="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md)。
<!-- npu="950" id10 -->
- 针对Ascend 950PR/Ascend 950DT，仅支持L1 Buffer->L0A Buffer、L1 Buffer->L0B Buffer数据通路。
<!-- end id10 -->

## 调用示例<a id="section6461234123118"></a>

完整搬运流程请参考[LoadData（2D矩阵搬运V2）样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_2dv2_l12l0)。LoadData（BitMode 2D矩阵搬运）的使用可以参考下面的调用示例，L1上为NZ数据排布、shape为[M,K]的A矩阵，调用LoadData指令完成L1 Buffer->L0A Buffer的Nz2Nz搬运。

- 示例一：使用LoadData2DParamsV2构造Load2DBitModeParam

```cpp
constexpr uint32_t fractalElemCount = 256;
constexpr uint32_t tensorElemCount = 4 * fractalElemCount;

// 源操作数：L1 Buffer，存放32x32的half矩阵。
AscendC::LocalTensor<half> srcLocal(AscendC::TPosition::A1, a1Addr, tensorElemCount);

// 目的操作数：L0A Buffer，预留4个512B分形接收完整32x32 half矩阵。
AscendC::LocalTensor<half> dstLocal(AscendC::TPosition::A2, a2Addr, tensorElemCount);

AscendC::LoadData2DParamsV2 loadDataParams;
// 从源矩阵M轴第0个16元素开始搬运。
loadDataParams.mStartPosition = 0;
// 从源矩阵K轴第0个32B开始搬运。
loadDataParams.kStartPosition = 0;
// M方向搬运2 * 16个元素。
loadDataParams.mStep = 2;
// K方向搬运32个half，即2 * 32B。
loadDataParams.kStep = 2;
// 源L1的M方向有2个分形，K方向相邻源分形起始地址间隔为2 * 512B。
loadDataParams.srcStride = 2;
// 目的L0A按2个M方向分形连续排布，K方向相邻目的分形起始地址间隔为2 * 512B。
loadDataParams.dstStride = 2;
// 不对每个分形做转置，保持L1中的分形方向搬入L0A。
loadDataParams.ifTranspose = false;
// 预留参数，固定配置为0。
loadDataParams.sid = 0;

// 使用LoadData2DParamsV2构造bit mode参数，底层以bit位形式传递上述搬运配置。
AscendC::Load2DBitModeParam bitModeParams(loadDataParams);
AscendC::LoadData<AscendC::TPosition::A2, AscendC::TPosition::A1, half>(dstLocal, srcLocal, bitModeParams);
```

- 示例二：使用Set函数修改Load2DBitModeParam

```cpp
AscendC::Load2DBitModeParam bitModeParams;
// 从源矩阵M轴第0个16元素开始搬运。
bitModeParams.SetMStartPosition(0);
// 从源矩阵K轴第0个32B开始搬运。
bitModeParams.SetKStartPosition(0);
// M方向搬运2 * 16个元素。
bitModeParams.SetMStep(2);
// K方向搬运32个half，即2 * 32B。
bitModeParams.SetKStep(2);
// 源L1的M方向有2个分形，K方向相邻源分形起始地址间隔为2 * 512B。
bitModeParams.SetSrcStride(2);
// 目的L0A按2个M方向分形连续排布，K方向相邻目的分形起始地址间隔为2 * 512B。
bitModeParams.SetDstStride(2);
// 不对每个分形做转置，保持L1中的分形方向搬入L0A。
bitModeParams.SetIfTranspose(false);

AscendC::LoadData<AscendC::TPosition::A2, AscendC::TPosition::A1, half>(dstLocal, srcLocal, bitModeParams);
```
