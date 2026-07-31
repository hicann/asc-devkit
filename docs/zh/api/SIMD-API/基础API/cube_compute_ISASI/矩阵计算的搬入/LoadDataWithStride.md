# LoadDataWithStride<a id="ZH-CN_TOPIC_0000002659754907"></a>

## 产品支持情况<a id="section796754519912"></a>

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

## 功能说明<a id="section129811351452"></a>

头文件路径为：`"basic_api/kernel_operator_mm_intf.h"`。

本接口本质上实现功能和[LoadData（卷积数据搬运）](./LoadData_3D.md)接口一致，用于将NC1HWC0格式的Feature Map完成Image to Column展开，然后再从展开后的二维矩阵中选取指定数据块搬入对应内存位置。与LoadData（卷积数据搬运）接口的差异在于，本接口仅支持Ascend 950PR/Ascend 950DT产品，支持配置输出矩阵K轴方向偏移量dstStride的能力且必须配置该参数，调用之前必须使用辅助配置接口[SetLoadDataRepeatWithStride](../矩阵搬入辅助配置接口/SetLoadDataRepeatWithStride.md)配置dstStride参数。此外，相比于LoadData（卷积数据搬运），本接口内部不包含针对其他芯片版本的兼容性实现，减少了兼容造成的额外开销，性能表现有所优化。

LoadDataWithStride支持的数据通路为：L1 Buffer->L0A Buffer、L1 Buffer->L0B Buffer。

## 函数原型<a id="section82039854412"></a>

```cpp
template <typename T, const IsResetLoad3dConfig& defaultConfig = IS_RESER_LOAD3D_DEFAULT_CONFIG, typename U = PrimT<T>,typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void LoadDataWithStride(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2<U>& loadDataParams)
```

## 参数说明<a id="section19594184564415"></a>

**表1** 模板参数说明

| 参数名称 | 含义 |
| -------- | ---- |
| T | 源操作数和目的操作数的数据类型。支持的数据类型请参考[数据类型](#section4219135304818)。 |
| defaultConfig | 控制是否在接口内部设置相关属性。 IsResetLoad3dConfig类型。IsResetLoad3dConfig结构定义如下：<br><pre>struct IsResetLoad3dConfig {<br>   bool isSetFMatrix = true;<br>   bool isSetPadding = true;<br>};</pre>isSetFMatrix配置为true，表示在接口内部设置FeatureMap的属性描述（包括l1H、l1W、padList，参数介绍参考[表3](#table193501032193419)）；设置为false，表示该接口传入的FeatureMap的属性描述不生效，开发者需要通过[SetFmatrix](../矩阵搬入辅助配置接口/SetFmatrix.md)进行设置。<br>isSetPadding配置为true，表示在接口内部设置Pad属性描述（即padValue参数，参数介绍参考[表3](#table193501032193419)）；设置为false，表示该接口传入的Pad属性不生效，开发者需要通过[SetLoadDataPaddingValue](../矩阵搬入辅助配置接口/SetLoadDataPaddingValue.md)进行设置。<br>该参数的默认值如下：<br><pre>constexpr IsResetLoad3dConfig IS_RESER_LOAD3D_DEFAULT_CONFIG = {true, true};</pre>特性细节可参考：[Feature Map、Pad属性描述寄存器设置](./LoadData_3D.md#zh-cn_topic_0000002512171652_section1881795134015)。 |
| U | LoadData3DParamsV2中padValue的数据类型。<br>当dst、src使用基础数据类型时，U和dst、src的数据类型T需保持一致，否则编译失败。<br>最后一个模板参数仅用于上述数据类型检查，用户无需关注。 |

**表2** 通用参数说明

| 参数名称 | 输入/输出 | 含义 |
| -------- | --------- | ---- |
| dst | 输出 | 目的操作数，类型为LocalTensor。数据连续排列顺序由目的操作数所在物理存储位置决定，具体约束如下：<br>&nbsp;&nbsp;&bull; L0A Buffer(TPosition: A2)：NZ格式。<br>&nbsp;&nbsp;&bull; L0B Buffer(TPosition: B2)：ZN格式。 |
| src | 输入 | 源操作数，类型为LocalTensor。数据连续排列顺序由目的操作数所在物理存储位置决定：L1 Buffer(TPosition: A1/B1)：NC1HWC0格式。 |
| loadDataParams | 输入 | LoadData参数结构体，类型为LoadData3DParamsV2，具体参考[表3](#table193501032193419)。 |

**表3** LoadData3DParamsV2结构体内参数说明<a id="table193501032193419"></a>  

| 参数名称 | 含义 |
| ---------- | ------ |
| padList | padding列表 [padding_left, padding_right, padding_top, padding_bottom]，每个元素取值范围：[0, 255]。默认为{0, 0, 0, 0}。 |
| l1H | 源操作数height，取值范围：l1H∈[0, 32767]。默认为0。<br>**l1H = 0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| l1W | 源操作数width，取值范围：l1W∈[0, 32767]。默认为0。<br>**l1W = 0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| channelSize | 源操作数的通道数，取值范围：channelSize∈[0, 65535]。默认为0。<br>对于uint32_t/int32_t/float，channelSize可取值为4，N\*8，N\*8+4；对于half/bfloat16，channelSize可取值为4，8，N\*16，N\*16 + 4，N\*16+8；对于int8_t/uint8_t，channelSize可取值为4，8，16，32\*N，N\*32+4，N\*32+8，N\*32+16；对于int4b_t，channelSize可取值为8，16，32，N\*64，N\*64+8，N\*64+16，N\*64+32。N为正整数。<br>**注：channelSize = 0表示不执行搬运，该接口将被视为NOP（空操作）。** |
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
| enTranspose | 是否启用转置功能，对整个目标矩阵进行转置，支持数据类型为bool，默认为false。<br>&nbsp;&nbsp;&bull; true：启用<br>&nbsp;&nbsp;&bull; false：不启用<br>**注：该参数在目的操作数的物理存储位置为L0A Buffer（TPosition: A2），且源操作数为b8/b16/b32类型时有效。在目的操作数的物理存储位置为L0B Buffer（TPosition: B2）时本参数无效。** |
| enSmallK | 是否开启small k特性，每个分形矩阵大小为16*4，支持数据类型为bool，默认为false。当前产品形态，该特性已不再支持。 |
| padValue | Pad填充值的数值，数据类型需要与src保持一致。若不想开启padding，可将padList设为全0。默认为0。 |
| filterSizeW | 是否在filterW的基础上将卷积核width增加256个元素。true，增加；false，不增加。默认为false。 |
| filterSizeH | 是否在filterH的基础上将卷积核height增加256个元素。true，增加；false，不增加。默认为false。 |
| fMatrixCtrl | 表示LoadData（卷积数据搬运） v2指令从左矩阵还是右矩阵获取FeatureMap的属性描述，与SetFmatrix配合使用，默认值为false。当前只支持设置为false。<br>&nbsp;&nbsp;&bull; true：从右矩阵中获取FeatureMap的属性描述；<br>&nbsp;&nbsp;&bull; false：从左矩阵中获取FeatureMap的属性描述。 |

## 数据类型<a id="section4219135304818"></a>

- 数据通路为L1 Buffer(TPosition::A1)->L0A Buffer(TPosition::A2)时，支持数据类型为：int8_t、uint8_t、hifloat8_t、fp8_e5m2_t、fp8_e4m3fn_t、half、bfloat16_t、int32_t、uint32_t、float。

- 数据通路为L1 Buffer(TPosition::B1)->L0B Buffer(TPosition::B2)时，支持数据类型为：half、bfloat16_t、int32_t、uint32_t、float。

## 返回值说明<a id="section4219135304919"></a>

无

## 约束说明<a id="section2045914466492"></a>

- 非转置场景下，L1 Buffer-\>L0B Buffer通路不支持。L1 Buffer-\>L0B Buffer通路下会自动进行转置，不需要配置enTranspose，此时enTranspose参数无效。
- 当目的地址位于L0A Buffer/L0B Buffer时，地址必须512字节对齐。当源地址位于L1 Buffer时，地址必须32字节对齐。
- 必须使用辅助配置接口[SetLoadDataRepeatWithStride](../矩阵搬入辅助配置接口/SetLoadDataRepeatWithStride.md)配置dstStride参数。

## 调用示例<a id="section14867162784812"></a>

本接需与SetLoadDataRepeatWithStride接口配合使用，展示代码示例片段：

```cpp
// LoadDataWithStride指令完成img2col的过程，可知 img2col后A矩阵高度为ho * wo,根据ho和wo的计算公式，代入卷积核宽度、卷积核滑动步长、卷积核膨胀系数等参数可知：A矩阵的高度为CeilAlign(k, fractalShape[0])；img2col后A矩阵宽度为ci * kh * kw，代入kh=1，kw=1，可知A矩阵的宽度为CeilAlign(m, fractalShape[1])。最后，配置loadDataParams.enTranspose = true，将整个A 矩阵转置并且将其中每一个分形转置。
AscendC::LoadData3DParamsV2<T> loadDataParams;

// 设置loadDataParams相关参数
...

// 使用SetLoadDataRepeatWithStride接口，设置LoadDataWithStride接口的repeat参数
AscendC::LoadDataRepeatParamWithStride repeatParams;
repeatParams.repeatTime = 1;  // height/width方向上的迭代次数
repeatParams.repeatStride = 1;  // height/width方向上前后迭代起始位置的距离
repeatParams.repeatMode = 0;  // 迭代方向：0：迭代沿height; 1：迭代沿width
repeatParams.dstStride = CeilDivision(m, fractalShape[0]);  // 输出矩阵K轴方向偏移
AscendC::SetLoadDataRepeatWithStride(repeatParams);

AscendC::LoadDataWithStride(a2Local, a1Local, loadDataParams);
```
