# LoadData（MX矩阵搬运）<a id="ZH-CN_TOPIC_0000002517258774"></a>

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

头文件路径为：`"basic_api/kernel_operator_mm_intf.h"`。

负责完成矩阵计算所需的左右矩阵数据和对应的左右量化系数矩阵数据的搬运。其中左右矩阵数据以大小为512字节的数据分形为单位进行搬运，左右量化系数矩阵以32字节的数据分形为单位进行搬运。支持的数据通路如下：

- 左右矩阵：L1 Buffer->L0A Buffer、L1 Buffer->L0B Buffer。
- 量化系数矩阵：L1 Buffer->L0A_MX Buffer、L1 Buffer->L0B_MX Buffer。

其中，L0A_MX/L0B_MX Buffer的大小为4KB，和L0A/L0B Buffer地址的映射关系如下：

$$L0A\_MX\ Buffer\ Address = L0A\ Buffer\ Address / 16$$

$$L0B\_MX\ Buffer\ Address = L0B\ Buffer\ Address / 16$$

- 对于b4数据类型，左右矩阵数据分形在L0A Buffer中为一个16×64的矩阵，在L0B Buffer中为一个64×16的矩阵。
- 对于b8数据类型，左右矩阵数据分形在L0A Buffer中为一个16×32的矩阵，在L0B Buffer中为一个32×16的矩阵。
- 量化系数矩阵分形是一个固定数据类型为fp8_e8m0_t、分形大小为16×2的矩阵。

## 函数原型<a id="section620mcpsimp"></a>

```cpp
template <typename T, typename U>
__aicore__ inline void LoadData(const LocalTensor<U>& dst, const LocalTensor<T>& src, const LocalTensor<fp8_e8m0_t>& srcMx, const LoadData2DParamsV2& loadDataParams, const LoadData2DMxParams& loadMxDataParams)
```


## 参数说明<a id="section622mcpsimp"></a>

**表 1** 模板参数说明<a name="table07381635103112"></a>

| 参数名称 | 含义 |
| ---------- | ---------- |
| T | T用来表示src的数据类型。<br>支持数据类型为：fp4x2_e2m1_t/fp4x2_e1m2_t/fp8_e4m3fn_t/fp8_e5m2_t。 |
| U | U用来表示dst的数据类型。<br>支持数据类型为：fp4x2_e2m1_t/fp4x2_e1m2_t/fp8_e4m3fn_t/fp8_e5m2_t。 |

**表 2** 通用参数说明<a name="table18368155193919"></a>

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，类型为LocalTensor。<br>数据连续排列顺序由目的操作数所在TPosition决定，具体约束如下：<br>&nbsp;&nbsp;&bull; A2：ZZ格式/NZ格式；对应的分形大小为16 \* (32字节 / sizeof(T))。<br>&nbsp;&nbsp;&bull; B2：ZN格式；对应的分形大小为 (32字节 / sizeof(T))  \* 16。 <br>支持的物理存储位置为L0A Buffer（TPosition: A2）/L0B Buffer（TPosition: B2）。<br>**注：L0A_MX Buffer和L0B_MX Buffer的地址和L0A/L0B的地址是固定比例关系，接口中会自行按照L0A/L0B的地址进行推导，用户无需配置。** |
| src | 输入 | 源操作数，类型为LocalTensor。<br>数据类型需要与dst保持一致。  <br>支持的物理存储位置为L1 Buffer（TPosition: A1/B1）。 |
| srcMx | 输入 | 源操作数，类型为LocalTensor，仅支持fp8_e8m0_t类型。 |
| loadDataParams | 输入 | LoadData参数结构体，类型为LoadData2DParamsV2。具体参考表[LoadData（2D矩阵搬运V2）](LoadData_2D_V2.md)中的LoadData2DParamsV2结构体内参数说明。 <br>**本结构体用于控制左右矩阵数据的搬运。**  <br>|
| loadMxDataParams | 输入 | LoadData参数结构体，类型为LoadData2DMxParams。具体参考[表3](#table15901153712305)。<br>**本结构体用于控制左右量化系数矩阵的搬运。**  <br>上述结构体参数定义请参考`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_mm.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。 |

**表 3** LoadData2DMxParams结构体参数说明<a name="table15901153712305"></a>

| 参数名称 | 含义 |
| ---------- | ---------- |
| xStartPosition | 源矩阵X轴方向的起始位置，即M维度方向，单位为1个分形（1个单位代表一个32字节的分形）。 |
| yStartPosition | 源矩阵Y轴方向的起始位置，即K维度方向，单位为32字节。 |
| xStep | 源矩阵X轴方向搬运长度，即M维度方向，单位为1个分形（1个单位代表一个32字节的分形）。取值范围：xStep∈[0, 255]。<br>**注：xStep=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| yStep | 源矩阵Y轴方向搬运长度，即K维度方向，单位为32字节。取值范围：yStep∈[0, 255]。<br>**注：yStep=0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| srcStride | 源矩阵X方向前一个分形起始地址与后一个分形起始地址的间隔，单位为32字节。 |
| dstStride | 目标矩阵X方向前一个分形起始地址与后一个分形起始地址的间隔，单位为32字节。 |

下面通过一个具体的示例来解释LoadData2DMxParams结构体参数。假设A矩阵shape为（M，K），则ScaleA矩阵shape为（M，K/32），ScaleA数据类型为fp8_e8m0_t，ScaleA矩阵分形排布见[图1](#fig138710913432)。

**图 1** ScaleA在L0A Buffer的分形排布<a id="fig138710913432"></a>

![](../../../../figures/ScaleA在L0A的分形排布.png "ScaleA在L0A Buffer的分形排布")

下图为ScaleA从L1 Buffer搬运至L0A Buffer过程中的配置参数示意。每一行为32字节，对应着[图1](#fig138710913432)中的一个分形。xStep为M维度分形的个数，如图中的xStep = M / 16 = 3，yStep为K维度32字节的个数，如图中的yStep = K / 32 / 2 = 21，srcStride和dstStride同理，表示在K维度上32字节的个数。

**图 2**  ScaleA从L1 Buffer搬运至L0A Buffer的配置参数示意

![](../../../../figures/Nd2Nz转换示意图.png "ScaleA从L1 Buffer搬运至L0A Buffer的配置参数示意")

## 数据类型

支持数据类型为：fp4x2_e2m1_t、fp4x2_e1m2_t、fp8_e5m2_t、fp8_e4m3fn_t。

## 返回值说明<a id="section640mcpsimp"></a>

无

## 约束说明<a id="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。

## 关键特性说明

左右矩阵搬运的相关关键特性与LoadData（2D矩阵搬运V2）一致，可参考[LoadData（2D矩阵搬运V2）的关键特性说明](LoadData_2D_V2.md)。

量化系数矩阵的搬运特性说明如下，起始地址计算公式为：

$$startAddr = srcAddr + (xStartPosition \times \lvert srcStride \rvert + yStartPosition) \times 32B$$

量化系数矩阵分形是一个固定数据类型为fp8_e8m0_t、分形大小为16×2的矩阵，搬运示意图如下：

**图3** 量化系数矩阵搬运示意图

![](../../../../figures/load2dmx_scale_transfer_demo.png "量化系数矩阵搬运示意图")

## 调用示例

完整调用示例请参考[LoadData（MX矩阵搬运）样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_2dmx_l12l0)。
