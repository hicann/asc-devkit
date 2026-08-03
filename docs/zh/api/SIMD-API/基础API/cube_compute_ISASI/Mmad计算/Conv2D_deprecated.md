# Conv2D（废弃）

## 产品支持情况

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

## 功能说明

**该接口废弃，并将在后续版本移除，请不要使用该接口。**

计算给定输入张量和权重张量的2-D卷积，输出结果张量。Conv2D卷积层多用于图像识别，使用过滤器提取图像中的特征。

## 函数原型

```cpp
template <typename T, typename U>
__aicore__ inline void Conv2D(const LocalTensor<T>& dst, const LocalTensor<U>& featureMap, const LocalTensor<U>& weight, Conv2dParams& conv2dParams, Conv2dTilling& tilling)
```

入参中的tiling结构需要通过如下切分方案计算接口来获取：

```cpp
template <typename T>
__aicore__ inline Conv2dTilling GetConv2dTiling(Conv2dParams& conv2dParams)
```

## 参数说明

**表1** 接口参数说明

| 参数名称 | 类型 | 说明 |
| --- | --- | --- |
| dst | 输出 | 目的操作数。<br><br><!-- npu="910" id8 -->Atlas 训练系列产品，支持的TPosition为：CO1，CO2<!-- end id8 --><br><!-- npu="310p" id9 -->Atlas 推理系列产品AI Core，支持的TPosition为：CO1，CO2<!-- end id9 --><br><br>结果中有效张量格式为[Cout/16, Ho, Wo, 16]，大小为Cout \* Ho \* Wo，Ho与Wo可以根据其他数据计算得出。<br>Ho = floor((H + pad_top + pad_bottom - dilation_h \* (Kh - 1) - 1) / stride_h + 1)<br>Wo = floor((W + pad_left + pad_right - dilation_w \* (Kw - 1) - 1) / stride_w + 1)<br>由于硬件要求Ho\*Wo需为16倍数，在申请dst Tensor时，shape应向上16对齐，实际申请shape大小应为Cout \* round_howo。<br>round_howo = ceil(Ho \* Wo /16) \* 16。 |
| featureMap | 输入 | 输入张量，Tensor的TPosition为A1。<br><br>输入张量"feature_map"的形状，格式是[C1, H, W, C0]。<br>C1\*C0为输入的channel数，要求如下：<br>&bull;当feature_map的数据类型为half时，C0=16。<br>&bull;当feature_map的数据类型为int8_t时，C0=32。<br>&bull; C1取值范围：[1,4]，输入的channel的范围：[16，32，64，128]。<br><br>H为高，取值范围：[1,40]。<br>W为宽，取值范围：[1,40]。 |
| weight | 输入 | 卷积核（权重）张量，Tensor的TPosition为B1。<br><br>卷积核张量"weight"的形状，格式是[C1, Kh, Kw, Cout, C0]。<br>C1\*C0为输入的channel数，对于C0要求如下：<br>&bull;当feature_map的数据类型为half时，C0=16。<br>&bull;当feature_map的数据类型为int8_t时，C0=32。<br>&bull; C1取值范围：[1,4]。<br>&bull; kernel_shape输入的channel数需与fm_shape输入的channel数保持一致。<br><br>Cout为卷积核数目，取值范围：[16，32，64，128]，Cout必须为16的倍数。<br>Kh为卷积核高；值的范围：[1,5]。<br>Kw表示卷积核宽；值的范围：[1,5]。 |
| conv2dParams | 输入 | 输入矩阵形状等状态参数，类型为Conv2dParams。结构体具体定义为：<br><br><pre><br>struct Conv2dParams {<br>    uint32_t imgShape[CONV2D_IMG_SIZE];       // [H, W]<br>    uint32_t kernelShapeIn[CONV2D_KERNEL_SIZE]; // [Kh, Kw]<br>    uint32_t stride[CONV2D_STRIDE];          // [stride_h, stride_w]<br>    uint32_t cin;                            // cin = C0 * C1;<br>    uint32_t cout;<br>    uint32_t padList[CONV2D_PAD];       // [pad_left, pad_right, pad_top, pad_bottom]<br>    uint32_t dilation[CONV2D_DILATION]; // [dilation_h, dilation_w]<br>    uint32_t initY;<br>    uint32_t partialSum;<br>};<br></pre> |
| tilling | 输入 | 分形控制参数，类型为Conv2dTilling。结构体具体定义为：<br><br><pre>struct Conv2dTilling {<br>    const uint32_t blockSize = 16; // # M block size is always 16<br>    LoopMode loopMode = LoopMode::MODE_NM;<br><br>    uint32_t c0Size = 32;<br>    uint32_t dTypeSize = 1;<br><br>    uint32_t strideH = 0;<br>    uint32_t strideW = 0;<br>    uint32_t dilationH = 0;<br>    uint32_t dilationW = 0;<br>    uint32_t hi = 0;<br>    uint32_t wi = 0;<br>    uint32_t ho = 0;<br>    uint32_t wo = 0;<br><br>    uint32_t height = 0;<br>    uint32_t width = 0;<br><br>    uint32_t howo = 0;<br><br>    uint32_t mNum = 0;<br>    uint32_t nNum = 0;<br>    uint32_t kNum = 0;<br><br>    uint32_t mBlockNum = 0;<br>    uint32_t kBlockNum = 0;<br>    uint32_t nBlockNum = 0;<br><br>    uint32_t roundM = 0;<br>    uint32_t roundN = 0;<br>    uint32_t roundK = 0;<br><br>    uint32_t mTileBlock = 0;<br>    uint32_t nTileBlock = 0;<br>    uint32_t kTileBlock = 0;<br><br>    uint32_t mIterNum = 0;<br>    uint32_t nIterNum = 0;<br>    uint32_t kIterNum = 0;<br><br>    uint32_t mTileNums = 0;<br><br>    bool mHasTail = false;<br>    bool nHasTail = false;<br>    bool kHasTail = false;<br><br>    uint32_t kTailBlock = 0;<br>    uint32_t mTailBlock = 0;<br>    uint32_t nTailBlock = 0;<br><br>    uint32_t mTailNums = 0;<br>};<br></pre> |

**表2** Conv2DParams结构体内参数说明：

| 参数名称 | 类型 | 说明 |
| --- | --- | --- |
| imgShape | vector&lt;int&gt; | 输入张量"feature_map"的形状，格式是[H, W]。<br>&bull; H为高，取值范围：[1,40]。<br>&bull; W为宽，取值范围：[1,40]。 |
| kernelShape | vector&lt;int&gt; | 卷积核张量"weight"的形状，格式是[Kh, Kw]。<br>&bull; Kh为高，取值范围：[1,5]。<br>&bull; Kw为宽，取值范围：[1,5]。 |
| stride | vector&lt;int&gt; | 卷积步长，格式是[stride_h, stride_w]。<br>&bull; stride_h表示步长高，值的范围：[1,4]。<br>&bull; stride_w表示步长宽，值的范围：[1,4]。 |
| cin | int | 分形排布参数，Cin = C1 \* C0，Cin为输入的channel数，C1取值范围：[1,4]。<br>&bull;当feature_map的数据类型为float时，C0=8。输入的channel的范围：[8，16，24，32]。<br>&bull;当feature_map的数据类型为half时，C0=16。输入的channel的范围：[16，32，48，64]。<br>&bull;当feature_map的数据类型为int8_t时，C0=32。输入的channel的范围：[32，64，96，128]。 |
| cout | int | Cout为卷积核数目，取值范围：[16，32，64，128]，Cout必须为16的倍数。 |
| padList | vector&lt;int&gt; | padding行数/列数，格式是[pad_left, pad_right, pad_top, pad_bottom]。<br>&bull; pad_left为feature_map左侧pad列数，范围[0,4]。pad_right为feature_map右侧pad列数，范围[0,4]。<br>&bull; pad_top为feature_map顶部pad行数，范围[0,4]。<br>&bull; pad_bottom为feature_map底部pad行数，范围[0,4]。 |
| dilation | vector&lt;int&gt; | 空洞卷积参数，格式[dilation_h, dilation_w]。<br>&bull; dilation_h为空洞高，范围：[1,4]。<br>&bull; dilation_w为空洞宽，范围：[1,4]。<br><br>膨胀后卷积核宽为dilation_w \* (Kw - 1) + 1，高为dilation_h \* (Kh - 1) + 1。 |
| initY | uint32_t | 表示dst是否需要初始化。<br>&bull;取值0：不使用bias，L0C Buffer需要初始化，dst初始矩阵保存有之前结果，新计算结果会累加前一次Conv2D计算结果。<br>&bull;取值1：不使用bias，L0C Buffer不需要初始化，dst初始矩阵中数据无意义，计算结果直接覆盖dst中的数据。 |
| partialSum | uint32_t | 当dst参数所在的TPosition为CO2时，通过该参数控制计算结果是否搬出。<br>&bull;取值0：搬出计算结果<br>&bull;取值1：不搬出计算结果，可以进行后续计算 |

**表3** Conv2dTilling结构体内参数说明

| 参数名称 | 类型 | 说明 |
| --- | --- | --- |
| blockSize | uint32_t | 固定值，恒为16，一个维度内存放的元素个数。 |
| loopMode | LoopMode | 遍历模式，结构体具体定义为：<br><br><pre>enum class LoopMode {<br>    MODE_NM = 0,<br>    MODE_MN = 1,<br>    MODE_KM = 2,<br>    MODE_KN = 3<br>};<br></pre> |
| c0Size | uint32_t | 一个block的字节长度，范围[16或者32]。 |
| dtypeSize | uint32_t | 传入的数据类型的字节长度，范围[1, 2]。 |
| strideH | uint32_t | 卷积步长-高，范围:[1,4]。 |
| strideW | uint32_t | 卷积步长-宽，范围:[1,4]。 |
| dilationH | uint32_t | 空洞卷积参数-高，范围：[1,4]。 |
| dilationW | uint32_t | 空洞卷积参数-宽，范围：[1,4]。 |
| hi | uint32_t | feature_map形状-高，范围：[1,40]。 |
| wi | uint32_t | feature_map形状-宽，范围：[1,40]。 |
| ho | uint32_t | feature_map形状-高，范围：[1,40]。 |
| wo | uint32_t | feature_map形状-宽，范围：[1,40]。 |
| height | uint32_t | weight形状-高，[1,5]。 |
| width | uint32_t | weight形状-宽，[1,5]。 |
| howo | uint32_t | feature_map形状大小，为ho \* wo。 |
| mNum | uint32_t | M轴等效数据长度参数值，范围：[1,4096]。 |
| nNum | uint32_t | N轴等效数据长度参数值，范围：[1,4096]。 |
| kNum | uint32_t | K轴等效数据长度参数值，范围：[1,4096]。 |
| roundM | uint32_t | M轴等效数据长度参数值且以blockSize为倍数向上取整，范围：[1,4096]。 |
| roundN | uint32_t | N轴等效数据长度参数值且以blockSize为倍数向上取整，范围：[1,4096]。 |
| roundK | uint32_t | K轴等效数据长度参数值且以c0Size为倍数向上取整，范围：[1,4096]。 |
| mBlockNum | uint32_t | M轴Block个数，mBlockNum = mNum / blockSize，范围：[1,4096]。 |
| nBlockNum | uint32_t | N轴Block个数，nBlockNum = nNum / blockSize，范围：[1,4096]。 |
| kBlockNum | uint32_t | K轴Block个数，kBlockNum = kNum / blockSize，范围：[1,4096]。 |
| mIterNum | uint32_t | 遍历M轴维度数量，范围：[1,4096]。 |
| nIterNum | uint32_t | 遍历N轴维度数量，范围：[1,4096]。 |
| kIterNum | uint32_t | 遍历K轴维度数量，范围：[1,4096]。 |
| mTileBlock | uint32_t | M轴切分块个数，范围：[1,4096]。 |
| nTileBlock | uint32_t | N轴切分块个数，范围：[1,4096]。 |
| kTileBlock | uint32_t | K轴切分块个数，范围：[1,4096]。 |
| kTailBlock | uint32_t | K轴尾块个数，范围：[1,4096]。 |
| mTailBlock | uint32_t | M轴尾块个数，范围：[1,4096]。 |
| nTailBlock | uint32_t | N轴尾块个数，范围：[1,4096]。 |
| kHasTail | bool | K轴是否存在尾块。 |
| mHasTail | bool | M轴是否存在尾块。 |
| nHasTail | bool | N轴是否存在尾块。 |
| mTileNums | uint32_t | M轴切分块个数的长度，范围：[1,4096]。 |
| mTailNums | uint32_t | M轴尾块个数的长度，范围：[1,4096]。 |

## 数据类型

**表4** feature_map、weight和dst的数据类型组合

| feature_map.dtype | weight.dtype | dst.dtype |
| --- | --- | --- |
| int8_t | int8_t | int32_t |
| half | half | float |
| half | half | half |

## 返回值说明

无

## 约束说明

- 该接口当前不支持W=Kw并且H\>Kh的场景，其将产生不可预期的结果。
- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。

## 调用示例

该接口已废弃，请使用Mmad接口替代。
