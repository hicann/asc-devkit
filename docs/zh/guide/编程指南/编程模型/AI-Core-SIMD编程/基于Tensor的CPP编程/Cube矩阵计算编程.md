# Cube矩阵计算编程<a name="ZH-CN_TOPIC_0000002600000003"></a>

本文介绍如何在Ascend C中使用C++ Tensor编程API编写基于L1 Buffer、L0A Buffer等片上存储的**矩阵计算代码**。

AI Core包含Vector计算单元（部署于AIV核）和Cube计算单元（部署于AIC核）。Cube计算单元专用于执行矩阵运算，直接访问的专用缓存如下：L0A Buffer存储左矩阵，L0B Buffer存储右矩阵，L0C Buffer存储初始累加值及矩阵计算结果。

## 矩阵编程的基本步骤

Cube矩阵计算的基本步骤为：数据搬入 → 数据加载 → 计算 → 数据搬出。当我们描述一个常见的Cube矩阵计算时，需要执行以下四个步骤：

1. 通过DMA将数据从GM搬入到`Local Memory(L1 Buffer)`
2. 通过数据加载将L1 Buffer搬入到`L0A Buffer & L0B Buffer`
3. 进行矩阵计算，计算结果存储在`L0C Buffer`中
4. 通过DMA将数据从`L0C Buffer`搬出到GM

## 矩阵计算内存管理

### 矩阵计算内存申请

矩阵计算编程主要通过L0A/L0B/L0C Buffer进行矩阵计算。开发者需将输入数据从Global Memory（以下简称GM）搬入L1 Buffer，再从L1 Buffer搬入L0A/L0B Buffer，最终通过Cube计算指令完成计算。L1 Buffer与L0A/L0B/L0C Buffer均通过LocalTensor对象进行管理，开发者可通过LocalMemAllocator分配器申请矩阵计算所需的内存空间。

```cpp
__global__ __cube__ void matmul_custom(...)
{
    constexpr uint32_t m_size = 128;
    constexpr uint32_t n_size = 128;
    constexpr uint32_t k_size = 256;

    AscendC::LocalMemAllocator<AscendC::Hardware::L1> l1Allocator;
    AscendC::LocalMemAllocator<AscendC::Hardware::L0A> l0aAllocator;
    AscendC::LocalMemAllocator<AscendC::Hardware::L0B> l0bAllocator;
    AscendC::LocalMemAllocator<AscendC::Hardware::L0C> l0cAllocator;

    // Allocate L1 Buffer for matrix A and B
    AscendC::LocalTensor<half> l1a_buffer = l1Allocator.Alloc<half, m_size * k_size>();
    AscendC::LocalTensor<half> l1b_buffer = l1Allocator.Alloc<half, k_size * n_size>();
    // Allocate L0A/L0B/L0C Buffer for matrix computation
    AscendC::LocalTensor<half> l0a_buffer = l0aAllocator.Alloc<half, m_size * k_size>();
    AscendC::LocalTensor<half> l0b_buffer = l0bAllocator.Alloc<half, k_size * n_size>();
    AscendC::LocalTensor<float> l0c_buffer = l0cAllocator.Alloc<float, m_size * n_size>();

    // ... Use Buffer for matrix computation
}
```

### 矩阵计算内存布局

矩阵计算单元采用分块计算逻辑，其中16×16是矩阵计算单元分形的基本粒度（对于half数据类型；int8类型为16×32或32×16；fp32类型分形为16×8），硬件可以并行处理多个矩阵计算单元。在传统的线性存储布局（如行主序或列主序）中，数据按行或列连续存放。当需要读取一个16×16的矩形数据块时，若直接从内存中获取，则需访问不连续的多个内存地址以逐行“收集”数据，这种非连续访问会导致内存效率显著下降。
为解决上述问题，引入了矩阵分形存储格式。该格式通过软件层对数据进行重排，使每个16×16计算块在物理内存中连续存放。硬件仅需发起一次连续读取，即可将整个数据块加载至片上寄存器，避免了多地址寻址开销，从而大幅提升矩阵计算单元的数据吞吐率。对于half数据类型，16×16是矩阵计算单元的基本分形粒度，当前支持Cube计算的产品型号包括[NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md)和[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)。
昇腾AI处理器的矩阵计算单元为追求极高的数据吞吐率与计算密度，采用了专有的硬件架构设计。与通用CPU常用的连续线性存储不同，该单元要求输入输出数据必须符合特定的分形存储格式。在使用Ascend C的Mmad类接口进行算子开发时，深入理解这些数据格式对于实现正确的数据搬运、分块及性能优化至关重要。
为便于理解与记忆，文档采用“大Y小x”的直观命名法：
 - 大Y（Z/N）：表示分形矩阵之间的排列顺序（Z为行主序，N为列主序）。
 - 小x（z/n）：表示分形矩阵内部元素的排列顺序（z为行主序，n为列主序）。

### 关键分形格式详解

矩阵分形存储格式是对多维Tensor在内存中数据排布格式的描述。
为了适配矩阵计算单元的物理读取路径，不同的产品形态有不同的格式要求：
针对[NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md)，矩阵乘法C = A × B要求：左矩阵A使用Zz格式，右矩阵B使用Zn格式，结果矩阵C使用Nz格式，如下图所示：

![矩阵乘法场景涉及的数据格式](../../../../figures/矩阵乘法场景设计的数据格式.png)

针对[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)，矩阵乘法C = A × B要求：左矩阵A使用Nz格式，右矩阵B使用Zn格式，结果矩阵C使用Nz格式，如下图所示：

![矩阵乘法场景涉及的数据格式](../../../../figures/矩阵乘法场景涉及的数据格式.png)

#### ND格式（N-Dimension）

ND格式是通用的N维张量格式，数据在内存中连续线性存放，通常存在于GM中。在进行矩阵运算前，通常需要将其转换为Nz格式以适配Cube计算单元。

#### Nz格式

Nz格式主要用于L1 Buffer和L0C Buffer中存放数据。针对[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)，L0A Buffer也采用Nz分形格式。该格式采用大N（外部列主序）+ 小z（内部行主序）的方式排布。

![Nz数据排布格式](../../../../figures/Nz数据排布格式.png)

Nz是Ascend架构的中间格式，介于线性数据(ND)和计算专用数据(Zz/Zn)之间，便于格式转换。大N排布（列主序Block）的设计通常是为了配合多核并行或后续算子（如Vector向量计算）在处理通道（Channel）维度数据时的便利性。

> 📌 Nz格式在不同Buffer中的应用场景：
> - **L0C Buffer**: 存储矩阵乘法结果。分形形状为16×16（对于half数据类型，256元素），适合Cube计算单元高效运算。
> - **L1 Buffer**: 存储中间数据。分形形状为16×(32B/sizeof(DataType))，大小512字节，便于转换为Zz和Zn格式。
> - **L0A Buffer（仅[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)）**: 存储左矩阵。

#### Zz格式

针对[NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md)，L0A Buffer使用此格式（[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)使用Nz格式）。该格式采用大Z（外部行主序）+ 小z（内部行主序）。

![Zz格式(以half类型为例)](../../../../figures/Zz格式-以half类型为例.png)

在矩阵乘法中，A矩阵逐行读取数据。Zz格式的行主序特性使同一行元素物理地址连续，与计算访问模式匹配，便于Cube单元左侧通道通过DMA连续读取，避免跨步访问，提升数据搬运效率。

#### Zn格式

Zn格式主要用于L0B Buffer中存放数据，如果离线处理好的右矩阵数据直接为Zn格式，则在GM和L1 Buffer上存储该格式。该格式采用大Z(外部行主序) + 小n(内部列主序)。

![Zn格式(以half类型为例)](../../../../figures/Zn格式-以half类型为例.png)

矩阵乘法需读取B矩阵列向量。普通行主序存储导致列数据在内存中地址不连续。Zn格式在分形内部按列存储，使列数据在L0B中紧凑排列，便于Cube单元右侧通道线性读取完整列向量，避免跨步访问。

### 格式转换

根据上述信息，Global Memory采用ND格式，L1 Buffer采用Nz格式，因此需进行ND2NZ格式转换。

![ND2NZ格式转换](../../../../figures/ND2NZ格式转换.png)

以上图为例，图中展示了一个具体的M=40、K=56的矩阵（数据类型为half），在从GM搬运至L1 Buffer并转换为Nz格式时的内存排布变化：
- 原始数据（ND格式）：左侧GM中存储的是原始矩阵，shape为40×56。数据呈线性连续排布（图中长箭头所示），遵循行主序规则。
- 对齐与填充：由于Cube单元的分形要求，目标矩阵必须能够被16×16的Block整除（假设数据类型为half，即32/sizeof(half)=16)。M轴填充：原始M = 40不能被16整除，需向上补齐至48（16×3）。K轴填充：原始K=56不能被16整除，需向上补齐至64（16×4）。图中灰色区域即为补齐产生的无效数据，白色区域为原始有效数据。
- 重排布（Nz格式）：右侧L1 Buffer中，数据被重组为3×4个分形块。Z字流动：每个16×16的小方块内部以及方块之间，均按照特定的Z字形顺序存储（如图中折线箭头所示），完成了从“线性”到“分形”的物理映射，做好了进入Cube计算的准备。

## 基础Tensor编程范式

基础Tensor仅封装数据指针与大小信息，开发者需手动配置搬运参数、计算分形步长、管理内存布局。适用于需要精细控制硬件行为的场景。

### 矩阵数据搬入

矩阵搬入接口实现Global Memory经L1 Buffer到L0A/L0B Buffer的数据传输，通常与Mmad接口配合使用。该接口支持多种搬运模式：二维搬运、三维格式转换、压缩数据解压、矩阵转置、稀疏化数据处理等。

开发者通过配置参数控制搬运路径、起始位置、长度等信息，充分利用硬件能力，最大化算子性能。每种数据通路对应特定的存储层级和访问特性。

结合上述数据流和分形知识，C++ Tensor编程提供以下矩阵搬入能力（完整接口见API参考手册）：

| 数据通路 | 功能    | C++ Tensor编程接口  | 分形支持情况 |
|---------|---------|---------------|-------------|
| Global Memory → L1 Buffer   | 随路转换ND2NZ搬运  | `DataCopy(dst, src, nd2nzParams)` | ![ND->Nz](../../../../figures/ND-Nz.png) |
| L1 Buffer → L0A Buffer  | 将L1 Buffer数据搬运到L0A Buffer中  | `LoadData(dst, src, l12l0Params)` | ![Nz->Zz](../../../../figures/Nz-Zz.png)<br>[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)上L0A分形为Nz，搬运时为Nz-Nz |
| L1 Buffer → L0B Buffer  | 将L1 Buffer数据搬运到L0B Buffer中  | `LoadData(dst, src, l12l0Params)` | ![Nz->Zn](../../../../figures/Nz-Zn.png) |

#### GM2L1 随路转换ND2NZ搬运

该能力主要实现将矩阵从GM搬运至L1 Buffer，并支持在数据搬运时进行ND到Nz格式的转换。

该能力通过`DataCopy`接口配合`Nd2NzParams`参数实现，配置`ndNum`、`srcDValue`等参数完成GM2L1搬运和ND2NZ格式转换。

![GM到L1的ND2NZ搬运示例](../../../../figures/GM到L1的ND2NZ搬运示例.png)

以上图为例，实现A矩阵(Shape(16, 23)，数据类型为half)从GM搬运到L1的能力，针对[NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md)，调用接口如下：

```cpp
// Allocate L1 Buffer space
constexpr uint64_t m = 16;
constexpr uint64_t k = 32;
AscendC::LocalMemAllocator<AscendC::Hardware::L1> l1Allocator;
AscendC::LocalTensor<half> dst = l1Allocator.Alloc<half, m * k>();

AscendC::GlobalTensor<half> srcGlobal;
srcGlobal.SetGlobalBuffer((__gm__ half*)src);

// Set parameters for Nd2NzParams
AscendC::Nd2NzParams intriParams{/*ndNum*/1, /*nValue*/16, /*dValue*/23, /*srcNdMatrixStride*/0, /*srcDValue*/32, /*dstNzC0Stride*/16, /*dstNzNStride*/1, /*dstNzMatrixStride*/0};

// Global Memory -> Local Memory with ND2NZ conversion
AscendC::DataCopy(dst, srcGlobal, intriParams);
```

#### L12L0 数据搬运

该能力主要实现将矩阵从L1 Buffer搬运至L0A/L0B Buffer，并支持从Nz分形转换到Zz或Zn格式。针对[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)，C++ Tensor编程提供了`LoadData`接口配合`LoadData2DParamsV2`参数，以512B数据分形为基本单位完成L1 Buffer到L0A/L0B Buffer的2D搬运，并可通过`ifTranspose`配置分形转置。对于3D搬运场景，需要使用`LoadData`接口配合`LoadData3DParamsV1`或`LoadData3DParamsV2`参数；其中NPU架构版本3510不支持`LoadData3DParamsV1`，需使用`LoadData3DParamsV2`。

![L12L0A非转置搬运](../../../../figures/L12L0A非转置搬运.png)

通过如下方式调用可完成上述的L1 Buffer到L0A Buffer的搬入操作。

```cpp
AscendC::LocalMemAllocator<AscendC::Hardware::L1> l1Allocator;
AscendC::LocalMemAllocator<AscendC::Hardware::L0A> l0aAllocator;
AscendC::LocalTensor<half> l1_buffer = l1Allocator.Alloc<half, 6400>();
AscendC::LocalTensor<half> l0a_buffer = l0aAllocator.Alloc<half, 2304>();

AscendC::LoadData2DParamsV2 l12l0Params;
// mStep is 2, kStep is 3, indicating transfer of 6 data blocks
l12l0Params.mStep = 2;
l12l0Params.kStep = 3;
// mStartPosition is 2, indicates M direction transfer starting position
l12l0Params.mStartPosition = 2;
// kStartPosition is 2, indicates K direction transfer starting position
l12l0Params.kStartPosition = 2;
// srcStride is 5, indicates interval between fractal starting addresses in K direction
l12l0Params.srcStride = 5;
// dstStride is 3: indicates interval between fractal starting addresses in M direction
l12l0Params.dstStride = 3;
// Do not transpose the fractal in this example
l12l0Params.ifTranspose = false;

AscendC::LoadData(l0a_buffer, l1_buffer, l12l0Params);
```

搬运到L0B Buffer采用同样的`LoadData`接口实现，接口参数及实现功能同L12L0A，可参考[矩阵计算的搬入](../../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/cube_compute_load.md)查看更多内容。

当输入A矩阵（GM）是转置排布（K×M）时，搬入到L1 Buffer后为（K×M）排布的Nz分形。要使L0A Buffer存放（M×K）的矩阵，需要在L1 Buffer到L0A Buffer的搬运过程中进行转置。针对[NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md)，可使用`LoadDataWithTranspose`接口，调用示例如下。

![需要转置的处理场景](../../../../figures/需要转置的处理场景.png)

```cpp
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
  AscendC::LoadDataWithTranspose(l0aTensor[i * dstOffset], l1aTensor[i * srcOffset], loadDataParams);
}
```

针对[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)，`LoadDataWithTranspose`仅支持L1 Buffer到L0B Buffer通路，推荐使用`LoadData2dTransposeParamsV2`作为搬运参数，不支持本节所述的L1 Buffer到L0A Buffer转置场景。该场景需要改用`LoadData`接口配合`LoadData2DParamsV2`参数，并将`ifTranspose`设置为`true`。

以`half`数据类型为例，A矩阵在L1 Buffer上为（K×M）的Nz分形，在L0A Buffer上需要转换为（M×K）的Nz分形，可按如下方式配置：

```cpp
constexpr uint32_t fractalShape[2] = {16, 32 / sizeof(half)};
uint32_t kAlignL1 = CeilAlign(k, fractalShape[0]);
uint32_t mAlignL1 = CeilAlign(m, fractalShape[1]);
uint32_t mAlignL0 = CeilAlign(m, fractalShape[0]);

AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStartPosition = 0;
loadDataParams.kStartPosition = 0;
loadDataParams.mStep = CeilDivision(kAlignL1, fractalShape[0]);
loadDataParams.kStep = CeilDivision(mAlignL1, fractalShape[1]);
loadDataParams.srcStride = CeilDivision(kAlignL1, fractalShape[0]);
loadDataParams.dstStride = CeilDivision(mAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = true;
loadDataParams.sid = 0;
AscendC::LoadData(l0aTensor, l1aTensor, loadDataParams);
```

### 矩阵数据搬出

矩阵搬出接口实现L0C Buffer到Global Memory或L1 Buffer的数据传输，支持随路量化、ReLU、NZ2ND格式转换、通道拆分合并等操作。开发者通过配置参数控制搬运路径、位置和长度等，通常在`Mmad`接口后使用。针对[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)，还支持L0C Buffer到UB的搬运以及UB双目标模式。L0C Buffer到Global Memory通路的NZ2ND格式转换同时支持NPU架构版本2201和3510。矩阵搬出能力如下图所示：

[NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md)下的矩阵搬出流程为：

![矩阵搬出流程](../../../../figures/矩阵搬出流程-AtlasA2A3.png)

[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)下的矩阵搬出流程为：

![矩阵搬出流程](../../../../figures/矩阵搬出流程-Ascend950PR.png)

矩阵搬出接口支持多种随路能力的灵活组合，以L0C Buffer到Global Memory的搬运场景为例：矩阵计算结果暂存于L0C Buffer后需搬运至GM，在此过程中可协同执行随路量化、ReLU激活、格式转换及通道拆分合并等操作。下图分别展示了不同NPU架构版本下这些随路能力的有效组合、中间数据类型及完整数据路径。例如，L0C Buffer中的F32数据可通过QF322B8_PRE随路量化得到B8数据，可选使能ReLU后经NZ2NZ/NZ2ND格式转换输出至GM。图中F32->F16和F32->BF16为数据类型转换，其余路径为随路Scalar/Vector量化模式。

[NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md)下的矩阵搬出能力为：

![L0C2GM搬运](../../../../figures/L0C2GM搬运-AtlasA2A3.png)

[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)下的矩阵搬出能力为：

![L0C2GM搬运](../../../../figures/L0C2GM搬运-Ascend950PR.png)

C++ Tensor编程提供了`DataCopy`接口配合`DataCopyCO12DstParams`参数来使能发挥芯片的各种随路能力，通过直接传入配置参数可完成随路搬运的操作。
示例：Mmad含有矩阵乘偏置，左矩阵和右矩阵的数据类型为int8_t，结果矩阵的数据类型为int32_t。量化模式为DEQF16，Scalar量化参数为2.0，将Mmad计算出的结果由int32_t量化成half并搬出。

```cpp
// Scalar quantization, quantization parameter is 2.0
float quantScalar = 2.0f;
uint64_t deqScalar = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&quantScalar));
// Write scalar quantization parameter to register for subsequent DataCopy instructions
AscendC::SetFixpipePreQuantFlag(deqScalar);
// Create parameters for DataCopy
AscendC::DataCopyCO12DstParams intriParams;
intriParams.nSize = n;
intriParams.mSize = m;
intriParams.srcStride = CeilAlign(m, CUBE_BLOCK);
intriParams.dstStride = n;
intriParams.quantPre = QuantMode_t::DEQF16;
intriParams.reluPre = 1; // Enable ReLU
intriParams.nz2ndEn = true; // Enable NZ2ND format conversion
// Set NZ2ND parameters before enabling NZ2ND in DataCopy
AscendC::SetFixpipeNz2ndFlag(/*ndNum*/1, /*srcNdStride*/0, /*dstNdStride*/1);
// Execute data transfer based on configurations in intriParams
AscendC::DataCopy(cGM, cLocal, intriParams);
```

另外，开发者可通过`SetFixPipeConfig`设置Tensor量化参数或清除unitFlag，通过`SetFixpipeNz2ndFlag`设置NZ2ND格式转换信息。`SetFixPipeConfig`的`reluPreTensor`和`quantPreTensor`需要为Fixpipe Buffer上数据类型为`uint64_t`的`LocalTensor`，当前`reluPreTensor`传入空`LocalTensor`即可。对应的接口和处理能力如下：

```cpp
// Set vector quantization parameters for on-the-fly quantization during data transfer
AscendC::SetFixPipeConfig(reluPreTensor, quantPreTensor, isUnitFlag);

// Configure parameters for NZ to ND format conversion during data transfer
// Number of ND matrices
uint16_t ndNum = 1;
// Source stride in fractal unit, offset between adjacent source NZ matrix headers
uint16_t srcNdStride = 0;
// Destination stride, offset between adjacent destination ND matrix headers
uint16_t dstNdStride = 1;
AscendC::SetFixpipeNz2ndFlag(ndNum, srcNdStride, dstNdStride);

// Set scalar quantization parameter for on-the-fly quantization in DataCopy (L0C->GM, L0C->L1)
float tmp = 0.5f;
// Convert float value tmp to uint64_t deqScalar
uint64_t deqScalar = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&tmp));
AscendC::SetFixpipePreQuantFlag(deqScalar);
```

详细说明可参考接口说明文档[Cube接口说明](../../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_ISASI.md)。

### 矩阵计算

[Mmad](../../../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/Mmad.md)是Ascend C封装NPU硬件计算能力的矩阵乘加核心接口，用于全连接层、卷积层等算子开发，实现C = A × B + C的矩阵乘加计算（输入为L0A Buffer和L0B Buffer，输出为L0C Buffer），其中针对[NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md)，L0A Buffer分形为Zz、L0B Buffer分形为Zn、L0C Buffer分形为Nz，而[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)产品L0A Buffer分形为Nz。

![NPU架构版本3510 `Mmad`计算](../../../../figures/Ascend950PR-Ascend950DT-asc_mmad计算.png)

**表 矩阵乘计算A、B、C矩阵说明（[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)）**

| 矩阵 | 存储位置 | 维度 | 数据格式 | 数据类型 |
|---|---|---|---|---|
| A | L0A Buffer | M × K | Nz | half |
| B | L0B Buffer | K × N | Zn | half |
| C | L0C Buffer | M × N（支持Bias初始化，维度1×N） | Nz | float |

C++ Tensor编程提供了一系列`Mmad`重载接口，支持不同数据类型的矩阵计算。以下代码实现上图中的矩阵乘计算。

```cpp
// Define L0A/L0B matrix sizes
constexpr uint32_t mSize = 64;
constexpr uint32_t kSize = 64;
constexpr uint32_t nSize = 64;

AscendC::LocalMemAllocator<AscendC::Hardware::L0A> l0aAllocator;
AscendC::LocalMemAllocator<AscendC::Hardware::L0B> l0bAllocator;
AscendC::LocalMemAllocator<AscendC::Hardware::L0C> l0cAllocator;

AscendC::LocalTensor<half> a_matrix = l0aAllocator.Alloc<half, mSize * kSize>();
AscendC::LocalTensor<half> b_matrix = l0bAllocator.Alloc<half, kSize * nSize>();
AscendC::LocalTensor<float> c_matrix = l0cAllocator.Alloc<float, mSize * nSize>();

AscendC::MmadParams mmadParams;
// Configure Mmad parameters
// m direction: 3 fractals, actual value = 3 × 16 = 48
mmadParams.m = 48;
// n direction: 3 fractals, actual value = 3 × 16 = 48
mmadParams.n = 48;
// k direction: 4 fractals, actual value = 4 × 16 = 64
mmadParams.k = 64;

// Feature parameters
// unitFlag: Controls fine-grained parallelism of Mmad and Fixpipe instructions
mmadParams.unitFlag = 0;
// disableGemv: Configures whether to enable GEMV mode when M=1
mmadParams.disableGemv = false;
// cmatrixInitVal: Initializes C matrix to zero before computation
mmadParams.cmatrixInitVal = true;
// cmatrixSource: Does not initialize C matrix from BiasTable Buffer
mmadParams.cmatrixSource = false;

AscendC::Mmad(c_matrix, a_matrix, b_matrix, mmadParams);
```

### 同步机制

AI Core内部执行单元（如MTE2搬运单元、Vector计算单元等）以异步并行方式运行，读写同一存储资源时可能存在数据依赖关系。因此，Cube计算需通过流水同步接口确保正确执行。Cube矩阵计算主要包含四步：数据搬入（Global Memory → L1 Buffer）、数据加载（L1 Buffer → L0A/B Buffer）、矩阵计算（L0A/B Buffer → L0C Buffer）、数据搬出（L0C Buffer → Global Memory）。四个步骤分别对应PIPE_MTE2、PIPE_MTE1、PIPE_M、PIPE_FIX流水线，需通过核内同步接口协调执行顺序，确保各步骤按序完成。

当开发者进行一个矩阵乘法计算时（[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)产品），可通过`Mutex::Lock`&`Mutex::Unlock`的方式完成对这四个步骤的同步。[NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md)产品可使用`SetFlag`&`WaitFlag`实现相同同步效果，详见[同步机制](../基于Tensor的CPP编程/CPPTensor编程概述.md#同步机制)。

以下示例中，Kernel入参`x`、`y`、`z`的指针类型与对应`GlobalTensor`的模板类型一致，因此调用`SetGlobalBuffer`时可省略显式类型转换。若实参指针类型与`GlobalTensor`的模板类型不一致，需要先将其显式转换为对应的`__gm__ PrimType*`类型。

```cpp
__global__ __cube__ void matmul_kernel(__gm__ half* x, __gm__ half* y, __gm__ float* z)
{
    uint8_t mutex_id = 1;
    
    AscendC::LocalMemAllocator<AscendC::Hardware::L1> l1Allocator;
    AscendC::LocalMemAllocator<AscendC::Hardware::L0A> l0aAllocator;
    AscendC::LocalMemAllocator<AscendC::Hardware::L0B> l0bAllocator;
    AscendC::LocalMemAllocator<AscendC::Hardware::L0C> l0cAllocator;
    
    AscendC::LocalTensor<half> al1ATensor = l1Allocator.Alloc<half, BLOCK_M * BLOCK_K>();
    AscendC::LocalTensor<half> bl1BTensor = l1Allocator.Alloc<half, BLOCK_K * BLOCK_N>();
    AscendC::LocalTensor<half> l0aTensor = l0aAllocator.Alloc<half, BLOCK_M * BLOCK_K>();
    AscendC::LocalTensor<half> l0bTensor = l0bAllocator.Alloc<half, BLOCK_K * BLOCK_N>();
    AscendC::LocalTensor<float> l0cTensor = l0cAllocator.Alloc<float, BLOCK_M * BLOCK_N>();
    
    AscendC::GlobalTensor<half> aGm, bGm;
    AscendC::GlobalTensor<float> cGm;
    aGm.SetGlobalBuffer(x);
    bGm.SetGlobalBuffer(y);
    cGm.SetGlobalBuffer(z);
    
    // Step 1: Data transfer in (Global Memory → L1 Buffer), pipeline is PIPE_MTE2
    AscendC::Mutex::Lock<PIPE_MTE2>(mutex_id);
    AscendC::Nd2NzParams nd2nzParamsA{
        1, BLOCK_M, BLOCK_K, 0, BLOCK_K, (BLOCK_M + 15) / 16 * 16, 1, 0};
    AscendC::DataCopy(al1ATensor, aGm, nd2nzParamsA);
    AscendC::Nd2NzParams nd2nzParamsB{
        1, BLOCK_K, BLOCK_N, 0, BLOCK_N, (BLOCK_K + 15) / 16 * 16, 1, 0};
    AscendC::DataCopy(bl1BTensor, bGm, nd2nzParamsB);
    AscendC::Mutex::Unlock<PIPE_MTE2>(mutex_id);
    
    // Step 2: Data transfer in (L1 Buffer → L0A/B Buffer), pipeline is PIPE_MTE1
    AscendC::Mutex::Lock<PIPE_MTE1>(mutex_id);
    ...
    AscendC::LoadData(l0aTensor, al1ATensor, loadParams);
    AscendC::LoadData(l0bTensor, bl1BTensor, loadParams);
    AscendC::Mutex::Unlock<PIPE_MTE1>(mutex_id);
    
    // Step 3: Matrix computation (L0A/B Buffer → L0C Buffer), pipeline is PIPE_M
    AscendC::Mutex::Lock<PIPE_M>(mutex_id);
    AscendC::MmadParams mmadParams;
    mmadParams.m = BLOCK_M;
    mmadParams.n = BLOCK_N;
    mmadParams.k = BLOCK_K;
    mmadParams.unitFlag = 0;
    mmadParams.cmatrixSource = false;
    mmadParams.cmatrixInitVal = true;
    AscendC::Mmad(l0cTensor, l0aTensor, l0bTensor, mmadParams);
    AscendC::Mutex::Unlock<PIPE_M>(mutex_id);
    
    // Step 4: Data transfer out (L0C Buffer → Global Memory), pipeline is PIPE_FIX
    AscendC::Mutex::Lock<PIPE_FIX>(mutex_id);
    ...
    AscendC::DataCopy(cGm, l0cTensor, l0c2gmParams);
    AscendC::Mutex::Unlock<PIPE_FIX>(mutex_id);
}
```

### 基于基础Tensor的矩阵计算编程示例

结合上述矩阵计算的搬运与计算能力，开发者可基于C++ Tensor编程实现矩阵类算子。具体开发实践可参考[matmul实现样例](../../../../../../../examples/01_simd_cpp_api/00_introduction/02_matrix/matmul_basic_api/README.md)。

## 扩展Tensor编程范式

[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)在Tensor基础上引入Layout布局概念，将Shape和Stride移到编译期确定，实现类型安全和分形布局自动生成。开发者只需定义Tensor的Layout，搬运和计算接口自动推断底层参数，大幅降低矩阵算子开发难度。

> 📌 扩展Tensor（带有Layout）能力从[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)开始支持。

> [!NOTE]说明
> 本节扩展Tensor示例以CANN9.1.0接口为准。对于本节使用的Nz布局，`MakeFrameLayout`的类型模板参数可按需省略；示例显式传入`half`，用于确保Layout的类型和C0推导与Tensor数据类型一致，并非接口新增了强制参数。若代码将原始指针直接传入`MakeTensor`，或使用`Copy(dst, src)`形式调用搬运接口，需要分别使用`MakeMemPtr`封装内存指针，并通过`MakeCopy`构造与搬运通路匹配的`CopyAtom`，再调用`Copy(atomCopy, dst, src)`。

### Layout的层次化表述

我们使用Shape和Stride来表达Layout排布格式，Shape用于表达Layout形状，Stride则用于区分不同的排布。比如下图中的行优先和列优先排布：

- 行优先:Shape (2, 4) Stride (4, 1)
- 列优先:Shape (2, 4) Stride (1, 2)

图中每个方格中的数字表示该位置元素在内存中按顺序排列时的下标。对于相同的矩阵位置，排布方式不同时，其在内存中的顺序可能是不同的，例如，矩阵坐标 (1, 0) 在行优先和列优先的情况下，对应元素在内存中的顺序分别是4和1。

![行优先排布](../../../../figures/行优先排布.png)

![列优先排布](../../../../figures/列优先排布.png)

通常Shape或者Stride中的元素是一个单独的正整数（如上文的示例），但是遇到一些复杂的内存排布情况可能无法表达，所以采用层次化表述法，该方式中，元素也可以是一个元组。元组的第一个元素用于描述不同层次中矩阵的行数，元组的第二个元素用于描述不同层次中矩阵的列数。
例如在下图中，Layout的Shape和Stride分别为：

- Shape((2, 3), (2, 4))
- Stride((1, 4), (2, 12))

![层次化Layout表达](../../../../figures/层次化Layout表达.png)

图中展示了两层矩阵：内层矩阵为内部用灰色线包裹的矩阵，外层矩阵为将内层矩阵视为一个元素时黑色线包裹的矩阵。Shape的第一个元素描述行方向的形状，(2, 3)表示内层矩阵和外层矩阵的行数分别为2和3；Shape的第二个元素描述列方向的形状，(2, 4)表示内层矩阵和外层矩阵的列数分别为2和4。Stride中的每个元素与Shape中的元素对应，表示该对应维度下，相邻元素首地址在内存地址上的间隔，图片中用箭头表示了每个维度相邻元素的首地址间隔。

### 矩阵计算中的分形表达

在基于Ascend C进行矩阵编程场景下，会用到以下几种常用的Layout，这些格式都采用如上文介绍的层次化表述法来表达，要求有内外层两层矩阵。其中行排布可以看作Z字形排布，列排布可以看作N字形排布，Layout的分形名称从前到后依次描述从外层到内层的排布顺序，比如NZLayout格式表示外层为N字形排布，内层为Z字形排布。四维Layout的具体表达方式如下：

```cpp
Layout = ((Shape): (Stride))
Shape = ((ShapeRow0, ShapeRow1), (ShapeColumn0, ShapeColumn1))
Stride = ((StrideRow0, StrideRow1), (StrideColumn0, StrideColumn1))
```

以Nz分形为例，Nz分形格式的内层分形Shape中，ShapeRow0固定为16行，ShapeColumn0固定为32Byte / sizeof(T)，内层分形的Stride中，StrideRow0固定为32Byte / sizeof(T)，StrideColumn0也固定为1，即内层分形按z字形组织，外层分形按N字形组织。ShapeRow1需满足row方向16对齐，ShapeColumn1需满足column方向32B对齐。

![Nz数据排布格式](../../../../figures/Nz数据排布格式.png)

下面是一个连续的Nz Layout示例，其中C0_ELEMENT = Std::Int<32>{} / sizeof(T)。

```cpp
Layout = ((Shape) : (Stride))
Shape = ((Std::Int<16>{}, ceil_div(row, Std::Int<16>{})), (C0_ELEMENT, ceil_div(column, C0_ELEMENT)))
Stride = ((C0_ELEMENT, C0_ELEMENT * Std::Int<16>{}), (Std::Int<1>{}, C0_ELEMENT * ceil_align(row, Std::Int<16>{})))
```

### Layout的构造

结合如上分形表达的描述，基于Layout的能力，C++ Tensor编程提供了原始的Layout构造接口。例如，需要创建一个形状为（128，256，128）的多维内存排布时，可通过`MakeLayout`接口直接构造。

```cpp
auto threeDimLayout = AscendC::Te::MakeLayout(AscendC::Te::MakeShape(128, 256, 128));
```

对于一个Nz分形的排布，也可以通过`MakeLayout`构造层次化的Layout表达。例如，对于外层大小为（128，256），数据类型为`half`的Nz数据，可以通过`MakeLayout`构造四维Layout排布：

```cpp
// Define Nz fractal with shape (128, 256), its 4D representation is: ((16, 128 / 16), (16, 256 / 16))
auto NzLayout = AscendC::Te::MakeLayout(AscendC::Te::MakeShape(
    AscendC::Te::MakeShape(16, 8), AscendC::Te::MakeShape(16, 16)));
```

Cube矩阵计算中对于不同的内部存储都有着自己独特的分形结构，为了简化用户的编写，C++ Tensor编程提供了`MakeFrameLayout`来简化用户编写生成如上的四维Layout定义：

```cpp
// Use MakeFrameLayout to simplify Nz fractal definition, user only needs to perceive the fractal structure of corresponding storage, no need to directly express 4D structure
auto NzLayout = AscendC::Te::MakeFrameLayout<AscendC::Te::NZLayoutPtn, half>(128, 256);
```

### 基于Layout的Tensor表达

扩展的Tensor是将上述Layout信息内置到Tensor结构中，由数据指针和数据布局组成。一个Tensor对象同时描述“数据放在哪里”和“数据长什么样”。
C++ Tensor提供了`MakeTensor`函数，用于构造带有Layout的扩展Tensor结构。例如，定义一个形状为（128，256）、数据类型为`half`的Global Memory数据块，以及一个同等大小的L1 Buffer数据块（Nz分形布局），可表达为：

```cpp
// Construct Layout for Global Memory & L1 Buffer 
auto globalLayout = AscendC::Te::MakeLayout(AscendC::Te::MakeShape(128, 256));
auto l1Layout = AscendC::Te::MakeFrameLayout<AscendC::Te::NZLayoutPtn, half>(128, 256);

// Construct GlobalTensor & L1 LocalTensor through memory pointer + Layout
auto globalTensor = AscendC::Te::MakeTensor(AscendC::Te::MakeMemPtr(gmPtr), globalLayout);
auto l1Tensor = AscendC::Te::MakeTensor(AscendC::Te::MakeMemPtr(l1Ptr), l1Layout);
```

扩展Tensor提供了`operator[]`元素访问、`operator()`子Tensor提取和`Slice()`切片提取等布局调整接口，大幅简化多维数据的操作流程。相较于基础Tensor需要手动计算偏移地址、步长参数和搬运长度，扩展Tensor的接口可自动推导这些底层参数，开发者只需关注数据逻辑即可。这种抽象封装方式不仅降低了编程复杂度，也减少了参数配置错误的风险，让开发者能将精力集中于算子性能优化与算法实现，显著提升开发效率。

```cpp
// Construct tensor for Global Memory
auto globalLayout = AscendC::Te::MakeLayout(AscendC::Te::MakeShape(128, 256));
auto globalTensor = AscendC::Te::MakeTensor(AscendC::Te::MakeMemPtr(gmPtr), globalLayout);

auto subTensor = globalTensor.Slice(AscendC::Te::MakeCoord(32, 64), AscendC::Te::MakeShape(32, 32));
```

### 矩阵搬运和计算

基础搬运接口需手动传入`count`或`DataCopyParams`参数指定搬运长度和搬运模式。扩展Tensor携带Shape、Stride和存储位置信息，编译器自动推断搬运参数和物理通路，简化接口调用。同时提供**Atom定制接口**，开发者可基于`CopyTraits`自定义搬运行为（如自定义搬运模式、随路处理等），满足特殊场景定制需求。

- **GM2L1搬运**：`Copy(atomCopy, dst, src)`搬运矩阵数据到L1 Buffer，支持随路格式转换（ND2NZ、DN2NZ、NZ2NZ等）。开发者定义源Tensor和目的Tensor的Layout，并使用`MakeCopy`构造GM到L1 Buffer的搬运原子对象，接口自动推导底层搬运参数和格式转换方式。

```cpp
// Construct GM-to-L1 CopyAtom with the default trait
auto atomCopy = AscendC::Te::MakeCopy(
    AscendC::Te::CopyGM2L1{}, AscendC::Te::CopyGM2L1TraitDefault{});

// Copy equal-size tensors, and infer copy parameters from Layout
AscendC::Te::Copy(atomCopy, l1Tensor, globalTensor);

// Specify Global Tensor offset to copy a slice from Global Tensor
AscendC::Te::Copy(atomCopy, l1BiasTensor,
    globalBias.Slice(AscendC::Te::MakeCoord(0, j * baseN), AscendC::Te::MakeShape(1, curN)));
```

- **L0C2GM搬出**：基于`CopyTraits`定制随路处理（如Fixpipe量化），将L0C Buffer计算结果搬出至GM。

```cpp
// Fixpipe inline quantization example
float quantValue = 0.5f;
uint64_t quantScalar = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&quantValue));
auto atomCopy = AscendC::Te::MakeCopy(
    AscendC::Te::CopyL0C2GM{}, AscendC::Te::CopyL0C2GMTraitDefault{}).with(AscendC::Te::FixpipeParams{});
AscendC::Te::Copy(atomCopy, globalC, l0CTensor, quantScalar);
```

同样，L1 Buffer到L0A/L0B Buffer的搬运需要构造对应通路的`CopyAtom`，矩阵计算需要构造`MmadAtom`。开发者传入搬运或计算原子对象以及操作Tensor后，接口可根据Layout推导底层参数。

### 基于扩展Tensor的矩阵计算示例

结合上述矩阵计算的搬运与计算能力，开发者可基于C++ Tensor编程实现矩阵类算子。具体开发实践可参考[matmul实现样例](../../../../../../../examples/01_simd_cpp_api/00_introduction/02_matrix/matmul_tensor_api/README.md)。
