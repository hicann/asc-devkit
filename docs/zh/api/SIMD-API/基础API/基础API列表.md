# 基础API列表

## 基础数据结构

| 接口名 | 功能描述 |
| --- | --- |
| [LocalTensor](数据结构/LocalTensor/LocalTensor.md) | LocalTensor用于存放AI Core中Local Memory（内部存储）的数据，支持逻辑位置TPosition为VECIN、VECOUT、VECCALC、A1、A2、B1、B2、CO1、CO2。 |
| [GlobalTensor](数据结构/GlobalTensor/GlobalTensor.md) | GlobalTensor用来存放Global Memory（外部存储）的全局数据。 |
| [layout_structure](data_structure_TensorAPI/data_structure_TensorAPI.md) | Tensor API基础数据结构中Layout相关基础数据结构。 |
| [tensor_structure](data_structure_TensorAPI/data_structure_TensorAPI.md) | Tensor API基础数据结构中Tensor相关数据结构。 |
| [GetLayoutPattern](data_structure_TensorAPI/utils/GetLayoutPattern.md) | GetLayoutPattern用于从Layout类型中提取其LayoutPattern类型信息。 |
| [Coordinate](辅助数据结构/Coordinate/Coordinate.md) | Coordinate本质上是一个元组（tuple），用于表示张量在不同维度的位置信息，即坐标值。 |
| [Layout](辅助数据结构/Layout/Layout.md) | Layout数据结构是描述多维张量内存布局的基础模板类，通过编译时的形状（Shape）和步长（Stride）信息，实现逻辑坐标空间到一维内存地址空间的映射。 |
| [TensorTrait](辅助数据结构/TensorTrait/TensorTrait.md) | TensorTrait数据结构是描述Tensor相关信息的基础模板类，包含Tensor的数据类型、逻辑位置和Layout内存布局。 |
| [ShapeInfo](辅助数据结构/ShapeInfo.md) | 存放LocalTensor或GlobalTensor的shape信息。 |
| [ListTensorDesc](辅助数据结构/ListTensorDesc.md) | 解析符合指定内存排布格式的数据，在kernel侧根据索引获取存储对应数据的地址及shape信息。 |
| [TensorDesc](辅助数据结构/TensorDesc/TensorDesc.md) | 储存ListTensorDesc.GetDesc()中根据index获取对应的Tensor描述信息。 |
| [UnaryRepeatParams](辅助数据结构/UnaryRepeatParams.md) | 控制单操作数地址步长的数据结构，包含相邻迭代间和同一迭代内DataBlock的地址步长参数。 |
| [BinaryRepeatParams](辅助数据结构/BinaryRepeatParams.md) | 控制双操作数地址步长的数据结构，包含相邻迭代间和同一迭代内DataBlock的地址步长参数。 |
| [complex32/complex64](辅助数据结构/complex32-complex64.md) | 复数类型，complex32实部和虚部都是half类型位宽32位，complex64实部和虚部都是float类型位宽64位。 |
| [TPosition](辅助数据结构/TPosition.md) | 用一种抽象的逻辑位置（TPosition）来表达各级别的存储，代替片上物理存储的概念。主要类型包括：VECIN、VECOUT、VECCALC、A1、A2、B1、B2、CO1、CO2。 |

## 矩阵计算（ISASI）

### 矩阵数据搬入至L0-Buffer

| 接口名 | 功能描述 |
| --- | --- |
| [LoadData（2D矩阵搬运）](矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md) | LoadData（2D矩阵搬运）负责完成普通矩阵计算所需的2D格式数据的搬运，以512字节的数据分形为单位从L1 Buffer搬运至L0A Buffer/L0B Buffer。 |
| [LoadData（2D矩阵搬运V2）](矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D_V2.md) | LoadData（2D矩阵搬运V2）负责完成普通矩阵计算所需的2D格式数据的搬运，使用V2参数结构，支持更多数据类型。 |
| [LoadData（MX矩阵搬运）](矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D_MX.md) | LoadData（MX矩阵搬运）用于搬运MX格式矩阵数据，支持将矩阵数据从L1 Buffer搬运至L0A Buffer/L0B Buffer，并同步搬运量化系数矩阵到L0A_MX Buffer/L0B_MX Buffer。 |
| [LoadData（BitMode 2D矩阵搬运）](矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D_BitMode.md) | LoadData（BitMode 2D矩阵搬运）是LoadData（2D矩阵搬运V2）的bit模式变体，通过联合体结构传入参数，支持按位操作的数据搬运。 |
| [LoadData（卷积数据搬运）](矩阵计算（ISASI）/矩阵计算的搬入/LoadData_3D.md) | LoadData（卷积数据搬运）本质上是用于将NC1HWC0格式的Feature Map完成Image to Column展开，然后再从展开后的二维矩阵中选取指定数据块搬入对应内存位置。 |
| [LoadData（BitMode卷积数据搬运）](矩阵计算（ISASI）/矩阵计算的搬入/LoadData_3D_BitMode.md) | LoadData（BitMode卷积数据搬运）用于完成image to column操作，将多维feature map转为二维矩阵，是LoadData（卷积数据搬运）的bit模式变体。 |
| [LoadDataWithStride](矩阵计算（ISASI）/矩阵计算的搬入/LoadDataWithStride.md) | LoadDataWithStride本质上是用于将NC1HWC0格式的Feature Map完成Image to Column展开，然后再从展开后的二维矩阵中选取指定数据块搬入对应内存位置，支持配置输出矩阵K轴方向偏移量。 |
| [LoadDataWithTranspose](矩阵计算（ISASI）/矩阵计算的搬入/LoadDataWithTranspose.md) | LoadDataWithTranspose负责完成普通矩阵计算所需的2D格式的数据的搬运，搬运过程中会伴随转置操作，参考特性分形转置。 |
| [LoadDataWithSparse](矩阵计算（ISASI）/矩阵计算的搬入/LoadDataWithSparse.md) | 用于从L1 Buffer中搬运以512字节为单位存放的稠密权重矩阵到L0B Buffer里，同时搬运以128字节为单位的索引矩阵到内置的专用buffer空间（用于后续MmadWithSparse接口进行读取）。 |
| [BroadCastVecToMM(ISASI)](矩阵计算（ISASI）/矩阵计算的搬入/BroadCastVecToMM(ISASI).md) | 将矢量数据广播到矩阵中，每个数据块中的每16个元素会被连续复制16次，支持Unified Buffer到L0C Buffer的数据传输通路。 |
| [DataCopy（L1ToBiasTable-Buffer数据搬运）](矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（L1ToBiasTable-Buffer数据搬运）.md) | DataCopy数据搬运支持将矩阵计算用到的Bias参数从L1 Buffer移动到BiasTable Buffer。 |
| [DataCopy（L1ToFixpipe-Buffer数据搬运）](矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（L1ToFixpipe-Buffer数据搬运）.md) | DataCopy数据搬运支持将随路量化参数从L1 Buffer移动到Fixpipe Buffer。 |

### 矩阵数据搬入至L1-Buffer

| 接口名 | 功能描述 |
| --- | --- |
| [DataCopy（GMToL1连续数据搬运）](矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（GMToL1连续数据搬运）.md) | 该接口能够将矩阵从Global Memory连续搬运至L1 Buffer（TPosition为A1/B1），数据搬运时格式和内容保持不变。 |
| [DataCopy（GMToL1高维切分数据搬运）](矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（GMToL1高维切分数据搬运）.md) | 该接口主要实现将矩阵从Global Memory搬运至L1 Buffer（TPosition为A1/B1），数据搬运时格式和内容保持不变。 |
| [DataCopy（GMToL1随路转换-ND2NZ搬运）](矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（GMToL1随路转换-ND2NZ搬运）.md) | 该接口主要实现将矩阵从Global Memory搬运至L1 Buffer（TPosition为A1/B1），并支持在数据搬运时进行ND到NZ格式的转换。 |
| [DataCopy（GMToL1随路转换-DN2NZ搬运）](矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（GMToL1随路转换-DN2NZ搬运）.md) | 该接口主要实现将矩阵从Global Memory搬运至L1 Buffer（TPosition为A1/B1），并支持在数据搬运时进行DN到NZ格式的转换。 |
| [DataCopyPad（GMToL1非对齐数据搬运）](矩阵计算（ISASI）/矩阵计算的搬入/DataCopyPad（GMToL1非对齐数据搬运）.md) | 该接口提供从Global Memory到L1 Buffer的数据非对齐搬运功能，可以根据开发者的需要自行填充数据。 |
| [LoadData（GMToL1-2D矩阵搬运）](矩阵计算（ISASI）/矩阵计算的搬入/LoadData（GMToL1-2D矩阵搬运）.md) | 负责完成普通矩阵计算所需的2D格式数据的搬运，以大小为512字节的数据分形为单位从Global Memory搬运至L1 Buffer（TPosition为A1/B1）。 |
| [LoadData（GMToL1-2D矩阵搬运V2）](矩阵计算（ISASI）/矩阵计算的搬入/LoadData（GMToL1-2D矩阵搬运V2）.md) | 负责完成普通矩阵计算所需的2D格式数据的搬运，以大小为512字节的数据分形为单位从Global Memory搬运至L1 Buffer（TPosition为A1/B1）。 |
| [DataCopy（UBToL1连续数据搬运）](矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（UBToL1连续数据搬运）.md) | 该接口实现将矩阵从Unified Buffer（UB，TPosition为VECIN/VECCALC/VECOUT）搬运至L1 Buffer，搬运方式为连续搬运，数据搬运时格式和内容保持不变。 |
| [DataCopy（UBToL1高维切分数据搬运）](矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（UBToL1高维切分数据搬运）.md) | 该接口实现将矩阵从Unified Buffer（UB，TPosition为VECIN/VECCALC/VECOUT）搬运至L1 Buffer，支持非连续搬运和连续搬运，数据搬运时格式和内容保持不变。 |
| [DataCopyPad（UBToL1非对齐数据搬运）](矩阵计算（ISASI）/矩阵计算的搬入/DataCopyPad（UBToL1非对齐数据搬运）.md) | 该接口提供从Unified Buffer到L1 Buffer的数据非对齐搬运功能。 |
| [DataCopy（UBToL1随路转换-ND2NZ搬运）](矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（UBToL1随路转换-ND2NZ搬运）.md) | 支持在数据搬运时进行ND到NZ格式的转换。数据从Unified Buffer（UB，TPosition为VECIN/VECCALC/VECOUT）搬运至L1 Buffer，搬运过程中完成ND->NZ格式转换。 |

### 辅助配置接口

| 接口名 | 功能描述 |
| --- | --- |
| [Fill](矩阵计算（ISASI）/矩阵搬入辅助配置接口/Fill.md) | 将特定物理存储位置的LocalTensor初始化为某一具体数值。仅支持L1 Buffer/L0A Buffer/L0B Buffer上的LocalTensor初始化。 |
| [SetFmatrix](矩阵计算（ISASI）/矩阵搬入辅助配置接口/SetFmatrix.md) | 用于调用LoadData（卷积数据搬运）时设置FeatureMap的属性描述。LoadData（卷积数据搬运）的模板参数isSetFMatrix设置为false时，表示LoadData（卷积数据搬运）传入的FeatureMap的属性将不生效，开发者需要通过该接口进行设置。 |
| [SetLoadDataBoundary](矩阵计算（ISASI）/矩阵搬入辅助配置接口/SetLoadDataBoundary.md) | 设置LoadData（卷积数据搬运）接口所需的L1 Buffer（TPosition: A1/B1）边界值。 |
| [SetLoadDataRepeat](矩阵计算（ISASI）/矩阵搬入辅助配置接口/SetLoadDataRepeat.md) | 用于设置LoadData（卷积数据搬运）接口的repeat参数。设置repeat参数后，可以通过调用一次LoadData（卷积数据搬运）接口完成多个迭代的数据搬运。 |
| [SetLoadDataRepeatWithStride](矩阵计算（ISASI）/矩阵搬入辅助配置接口/SetLoadDataRepeatWithStride.md) | 用于设置LoadDataWithStride接口的repeat参数。设置repeat参数后，可以通过调用一次LoadData（卷积数据搬运）接口完成多个迭代的数据搬运，且必须配置输出矩阵K轴方向偏移量参数dstStride。 |
| [SetLoadDataPaddingValue](矩阵计算（ISASI）/矩阵搬入辅助配置接口/SetLoadDataPaddingValue.md) | 用于调用LoadData（卷积数据搬运）接口时设置Pad填充的数值。LoadData（卷积数据搬运）的模板参数isSetPadding设置为false时，用户需要通过本接口设置Pad填充的数值，设置为true时，本接口设置的填充值不生效。 |
| [LoadDataUnzip](矩阵计算（ISASI）/矩阵搬入辅助配置接口/LoadDataUnzip.md) | 将GM上的数据解压并搬运到L1 Buffer（A1/B1）或L0B Buffer（B2）上。执行该API前需要执行LoadUnzipIndex加载压缩索引表。 |
| [LoadImageToLocal](矩阵计算（ISASI）/矩阵搬入辅助配置接口/LoadImageToLocal.md) | 将图像数据从Global Memory搬运到Local Memory。搬运过程中可以完成图像预处理操作：包括图像翻转，改变图像尺寸（抠图，裁边，缩放，伸展），以及色域转换，类型转换等。图像预处理的相关参数通过SetAippFunctions进行配置。 |
| [LoadUnzipIndex](矩阵计算（ISASI）/矩阵搬入辅助配置接口/LoadUnzipIndex.md) | 加载GM上的压缩索引表到内部寄存器。 |
| [SetAippFunctions](矩阵计算（ISASI）/矩阵搬入辅助配置接口/SetAippFunctions.md) | 设置图片预处理（AIPP，AI Core pre-process）相关参数。和LoadImageToLocal接口配合使用。设置后，调用LoadImageToLocal接口可在搬运过程中完成图像预处理操作：包括数据填充、通道交换、单行读取、数据类型转换、通道填充、色域转换。调用SetAippFunctions接口时需传入源图片在Global Memory上的矩阵、源图片的图片格式。 |

### Mmad计算

| 接口名 | 功能描述 |
| --- | --- |
| [Mmad](矩阵计算（ISASI）/Mmad计算/Mmad.md) | Mmad接口是Ascend C面向昇腾AI芯片的矩阵乘加核心计算接口，专为高性能算子开发设计，封装了昇腾NPU硬件的矩阵乘加计算能力，广泛用于神经网络层（如全连接层、卷积层）、数值计算类算子的开发。 |
| [MmadMx](矩阵计算（ISASI）/Mmad计算/MmadMx.md) | MmadMx（全称Microscaling Mmad）为带有量化系数的矩阵乘法，即左矩阵和右矩阵均有对应的量化系数矩阵，左量化系数矩阵scaleA和右量化系数矩阵scaleB。MmadMx场景中，左量化系数矩阵与左矩阵乘积，右量化系数矩阵与右矩阵乘积，对两个乘积的结果做矩阵乘法。 |
| [MmadBitMode](矩阵计算（ISASI）/Mmad计算/MmadBitMode.md) | MmadBitMode对于MmadParams结构体构造进行了优化，本接口适用于scalar流水成为性能优化瓶颈的场景，支持基础Mmad/MmadMx计算功能。本接口与Mmad/MmadMx接口的差异在于参数传入的方式不同，本接口传入的是联合结构体MmadBitModeParams。 |
| [MmadWithSparse](矩阵计算（ISASI）/Mmad计算/MmadWithSparse.md) | MmadWithSparse接口负责完成特殊稀疏矩阵乘加操作。稀疏矩阵是一种特殊类型的矩阵，即矩阵中包含较多的零元素。4：2结构化稀疏要求一个连续的4个权重或激活值的组（通常是张量中的一行或一列）中，最多只有2个值为非零，其余2个强制为零。 |

### Mmad寄存器配置说明

| 接口名 | 功能描述 |
| --- | --- |
| [SetHF32Mode](矩阵计算（ISASI）/Mmad计算辅助配置接口/SetHF32Mode.md) | 用于设置Mmad计算是否开启HF32模式，开启该模式后L0A Buffer/L0B Buffer中的FP32数据将在参与Mmad计算之前被舍入为HF32。 |
| [SetHF32TransMode](矩阵计算（ISASI）/Mmad计算辅助配置接口/SetHF32TransMode.md) | 设置HF32模式取整的具体方式，需要先使用SetHF32Mode开启HF32取整模式。 |
| [SetMMColumnMajor/SetMMRowMajor](矩阵计算（ISASI）/Mmad计算辅助配置接口/SetMMColumnMajor-SetMMRowMajor.md) | 控制Mmad/MmadWithSparse优先通过M/N的哪个方向。 |

### 矩阵计算的搬出

| 接口名 | 功能描述 |
| --- | --- |
| [DataCopy（L0C到GM数据搬运）](矩阵计算（ISASI）/矩阵计算的搬出/DataCopy（L0C到GM数据搬运）.md) | 矩阵计算的结果存放在L0C Buffer，DataCopy接口用于将结果搬运至Global Memory（GM）中，并且在搬运过程中支持随路格式转换等操作。 |
| [Fixpipe（L0C到GM数据搬运）](矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md) | 矩阵计算的结果存放在L0C Buffer，Fixpipe接口用于将结果搬运至Global Memory（GM）中，并且在搬运过程中支持随路格式转换等操作。 |
| [DataCopy（L0C到L1数据搬运）](矩阵计算（ISASI）/矩阵计算的搬出/DataCopy（L0C到L1数据搬运）.md) | 矩阵计算的结果存放在L0C Buffer，DataCopy接口用于将结果搬运至L1 Buffer中，并且在搬运过程中支持随路格式转换等操作。 |
| [Fixpipe（L0C到L1数据搬运）](矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到L1数据搬运）.md) | 矩阵计算的结果存放在L0C Buffer，Fixpipe接口用于将结果搬运至L1 Buffer中，并且在搬运过程中支持随路格式转换等操作。 |
| [Fixpipe（L0C Buffer到UB数据搬运）](矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到UB数据搬运）.md) | 矩阵计算的结果存放在L0C Buffer，Fixpipe接口用于将结果搬运至Unified Buffer（UB）中，并且在搬运过程中支持随路格式转换等操作。 |

### L1到GM数据搬运

| 接口名 | 功能描述 |
| --- | --- |
| [DataCopy（L1ToGM连续数据搬运）](矩阵计算（ISASI）/矩阵计算的搬出/DataCopy（L1ToGM连续数据搬运）.md) | 该接口能够将矩阵从L1 Buffer连续搬运至Global Memory，数据搬运时格式和内容保持不变。 |
| [DataCopy（L1ToGM高维切分数据搬运）](矩阵计算（ISASI）/矩阵计算的搬出/DataCopy（L1ToGM高维切分数据搬运）.md) | 该接口主要实现将矩阵从L1 Buffer搬运至Global Memory，数据搬运时格式和内容保持不变。 |

### 搬出寄存器配置说明

| 接口名 | 功能描述 |
| --- | --- |
| [SetFixPipeConfig](矩阵计算（ISASI）/矩阵搬出辅助配置接口/SetFixPipeConfig.md) | DataCopy数据搬运（L0C-\>GM、L0C-\>L1）过程中进行随路量化时，通过调用该接口设置量化的参数。 |
| [SetFixpipePreQuantFlag](矩阵计算（ISASI）/矩阵搬出辅助配置接口/SetFixpipePreQuantFlag.md) | DataCopy数据搬运（L0C-\>GM、L0C-\>L1）过程中进行随路量化时，通过调用该接口设置scalar量化参数。 |
| [SetFixpipeNz2ndFlag](矩阵计算（ISASI）/矩阵搬出辅助配置接口/SetFixpipeNz2ndFlag.md) | DataCopy数据搬运（L0C-\>GM）过程中进行随路格式转换（NZ格式转换为ND格式）时，通过调用该接口设置格式转换的相关配置。 |
| [SetFixPipeClipRelu](矩阵计算（ISASI）/矩阵搬出辅助配置接口/SetFixPipeClipRelu.md) | DataCopy数据搬运（L0C-\>GM）过程中进行随路量化后，通过调用该接口设置ClipReLU操作的最大值。 |
| [SetFixPipeAddr](矩阵计算（ISASI）/矩阵搬出辅助配置接口/SetFixPipeAddr.md) | DataCopy数据搬运（L0C-\>GM）过程中进行随路量化后，通过调用该接口设置Elementwise操作时LocalTensor的地址。 |

## 矩阵计算（TensorAPI）

### 矩阵数据搬入至L1

| 接口名 | 功能描述 |
| --- | --- |
| [Copy（Global Memory到L1 Buffer数据搬运）](矩阵计算（TensorAPI）/矩阵计算的搬入/Copy（GM到L1数据搬运）.md) | Tensor API通过`Copy`接口统一执行不同通路数据搬运。该接口用于将Global Memory中的数据搬运到L1 Buffer。`Copy`接口根据源张量和目的张量的存储位置、数据类型和Layout选择具体搬运实现。搬运块数、搬运长度、源/目的侧步长以及格式转换相关信息由Tensor Layout推导，用户不需要在`Copy`调用中额外传入搬运参数。 |

### 矩阵数据搬入至L0

| 接口名 | 功能描述 |
| --- | --- |
| [Copy（L1 Buffer到L0A Buffer数据搬运）](矩阵计算（TensorAPI）/矩阵计算的搬入/Copy（L1到L0A数据搬运）.md) | Tensor API通过`Copy`接口统一执行不同通路数据搬运。该接口用于将L1 Buffer中的左矩阵数据搬运到L0A Buffer。 |
| [Copy（L1 Buffer到L0B Buffer数据搬运）](矩阵计算（TensorAPI）/矩阵计算的搬入/Copy（L1到L0B数据搬运）.md) | Tensor API通过`Copy`接口统一执行不同通路数据搬运。该接口用于将L1 Buffer中的右矩阵数据搬运到L0B Buffer。 |
| [Copy（L1 Buffer到L0ScaleA Buffer数据搬运）](矩阵计算（TensorAPI）/矩阵计算的搬入/Copy（L1到L0ScaleA数据搬运）.md) | Tensor API通过`Copy`接口统一执行不同通路数据搬运。该接口用于将L1 Buffer中的左矩阵缩放数据搬运到L0ScaleA Buffer。左矩阵缩放数据在L0ScaleA Buffer上的首地址由左矩阵在L0A Buffer的首地址的1/16推导出来。 |
| [Copy（L1 Buffer到L0ScaleB Buffer数据搬运）](矩阵计算（TensorAPI）/矩阵计算的搬入/Copy（L1到L0ScaleB数据搬运）.md) | Tensor API通过`Copy`接口统一执行不同通路数据搬运。该接口用于将L1 Buffer中的右矩阵缩放数据搬运到L0ScaleB Buffer。右矩阵缩放数据在L0ScaleB Buffer上的首地址由右矩阵在L0B Buffer的首地址的1/16推导出来。 |
| [Copy（L1 Buffer到BiasTable Buffer数据搬运）](矩阵计算（TensorAPI）/矩阵计算的搬入/Copy（L1到BiasTable数据搬运）.md) | Tensor API通过`Copy`接口统一执行不同通路数据搬运。该接口用于将L1 Buffer中的bias数据搬运到BiasTable Buffer，作为Mmad计算中的bias输入。 |
| [Copy（L1 Buffer到Fixpipe Buffer数据搬运）](矩阵计算（TensorAPI）/矩阵计算的搬入/Copy（L1到Fixpipe数据搬运）.md) | Tensor API通过`Copy`接口统一执行不同通路数据搬运。该接口用于将L1 Buffer中的量化数据搬运到Fixpipe Buffer。量化数据可用于L0C输出到GM/UB时做随路量化。 |

### Mmad计算

| 接口名 | 功能描述 |
| --- | --- |
| [Mmad](矩阵计算（TensorAPI）/Mmad计算/Mmad.md) | `Mmad`接口用于完成L0A Buffer上左矩阵A和L0B Buffer上右矩阵B的矩阵乘加，结果写入L0C Buffer上结果矩阵C。默认模式为普通矩阵计算。 |

### 矩阵计算的搬出

| 接口名 | 功能描述 |
| --- | --- |
| [Copy（L0C Buffer到Global Memory数据搬运）](矩阵计算（TensorAPI）/矩阵计算的搬出/Copy（L0C到GM数据搬运）.md) | Tensor API通过`Copy`接口统一执行不同通路数据搬运。该接口用于将L0C Buffer中的矩阵计算结果搬运到Global Memory。L0C Buffer中的数据通常为`Mmad`的输出，数据格式为`NZ`。搬运到Global Memory时，接口会根据目的张量布局自动选择`NZ`到`ND`、`NZ`到`DN`或`NZ`到`NZ`的随路格式转换。 |
| [Copy（L0C Buffer到Unified Buffer数据搬运）](矩阵计算（TensorAPI）/矩阵计算的搬出/Copy（L0C到UB数据搬运）.md) | Tensor API通过`Copy`接口统一执行不同通路数据搬运。该接口用于将L0C Buffer中的矩阵计算结果搬运到Unified Buffer。L0C Buffer中的数据通常为`Mmad`的输出，数据格式为`NZ`。搬运到Unified Buffer时，接口会根据目的张量格式自动选择`NZ`到`ND`、`NZ`到`DN`或`NZ`到`NZ`的随路格式转换。 |

## Memory矢量计算

### 数据搬运
| 接口名 | 功能描述 |
| --- | --- |
| [DataCopy（GM与UB连续数据搬运）](Memory矢量计算/数据搬运/DataCopy（GM与UB连续数据搬运）.md) | 支持Global Memory与Unified Buffer之间的连续数据搬运，数据在传输过程中保持原始格式和内容不变。 |
| [DataCopy（GM与UB高维切分数据搬运）](Memory矢量计算/数据搬运/DataCopy（GM与UB高维切分数据搬运）.md) | 支持Global Memory与Unified Buffer之间的高维切分数据搬运，数据在传输过程中保持原始格式和内容不变。 |
| [DataCopy（GM与UB切片数据搬运）](Memory矢量计算/数据搬运/DataCopy（GM与UB切片数据搬运）.md) | 该接口为软仿接口，从易用性角度出发进行设计，支持数据的切片搬运，提取多维Tensor数据的子集进行搬运。 |
| [DataCopy（GMToUB随路转换ND2NZ搬运）](Memory矢量计算/数据搬运/DataCopy（GMToUB随路转换ND2NZ搬运）.md) | 该接口为软仿接口，从易用性角度出发进行设计，支持在从Global Memory到Unified Buffer的数据搬运过程中进行ND到NZ格式的转换。 |
| [DataCopy（UBToGM随路转换NZ2ND搬运）](Memory矢量计算/数据搬运/DataCopy（UBToGM随路转换NZ2ND搬运）.md) | 该接口为软仿接口，从易用性角度出发进行设计，支持在从Unified Buffer到Global Memory的数据搬运过程中进行NZ到ND格式的转换。 |
| [DataCopy（GMToUB多维数据搬运NDDMA）](Memory矢量计算/数据搬运/DataCopy（GMToUB多维数据搬运NDDMA）.md) | 多维数据搬运接口，相比于基础数据搬运接口，可更加自由配置搬入的维度信息以及对应的Stride。 |
| [DataCopyPad（GMToUB非对齐数据搬运）](Memory矢量计算/数据搬运/DataCopyPad（GMToUB非对齐数据搬运）.md) | 该接口提供将数据从Global Memory非对齐搬运至Unified Buffer的功能，可以根据开发者的需要自行填充数据。 |
| [DataCopyPad（UBToGM非对齐数据搬运）](Memory矢量计算/数据搬运/DataCopyPad（UBToGM非对齐数据搬运）.md) | 该接口提供将数据从Unified Buffer非对齐搬运至Global Memory的功能。 |
| [SetPadValue(ISASI)](Memory矢量计算/数据搬运辅助配置接口/SetPadValue(ISASI).md) | 从Global Memory将数据非对齐搬运至Unified Buffer时，可根据开发者的需要自行填充数据。SetPadValue用于设置DataCopyPad需要填充的数值。 |
| [SetLoopModePara](Memory矢量计算/数据搬运辅助配置接口/SetLoopModePara.md) | DataCopy、DataCopyPad过程中通过该接口使能loop mode并且设置loop mode的参数，在数据搬运结束后通过ResetLoopModePara重置loop mode的参数。 |
| [ResetLoopModePara](Memory矢量计算/数据搬运辅助配置接口/ResetLoopModePara.md) | 重置loop mode的参数。与SetLoopModePara搭配使用，在使能loop mode并且设置loop mode的参数的数据搬运场景下，数据搬运结束后需要调用该函数来重置loop mode参数。 |
| [DataCopy（UBToUB连续数据搬运）](Memory矢量计算/数据搬运/DataCopy（UBToUB连续数据搬运）.md) | 支持Unified Buffer与Unified Buffer之间的连续数据搬运，数据在传输过程中保持原始格式和内容不变。 |
| [DataCopy（UBToUB高维切分数据搬运）](Memory矢量计算/数据搬运/DataCopy（UBToUB高维切分数据搬运）.md) | 支持Unified Buffer与Unified Buffer之间的高维切分数据搬运，数据在传输过程中保持原始格式和内容不变。 |
| [Copy（UBToUB连续数据搬运）](Memory矢量计算/数据搬运/Copy（UBToUB连续数据搬运）.md) | 支持Unified Buffer和Unified Buffer之间的连续数据搬运，数据搬运时格式和内容保持不变。 |
| [Copy（UBToUB掩码式高维数据搬运）](Memory矢量计算/数据搬运/Copy（UBToUB掩码式高维数据搬运）.md) | 支持Unified Buffer和Unified Buffer之间的数据搬运，数据搬运时格式和内容保持不变，支持mask操作和DataBlock间隔操作。 |

### 基础算术
| 接口名 | 功能描述 |
| --- | --- |
| [Exp](Memory矢量计算/基础算术/Exp.md) | Exp属于单目矢量类计算接口，负责将输入的tensor按元素取自然指数。 |
| [Ln](Memory矢量计算/基础算术/Ln.md) | Ln属于单目矢量类计算接口，负责将输入的tensor按元素取自然对数。 |
| [Abs](Memory矢量计算/基础算术/Abs.md) | Abs属于单目矢量类计算接口，负责将输入的tensor按元素取绝对值。 |
| [Reciprocal](Memory矢量计算/基础算术/Reciprocal.md) | Reciprocal属于单目矢量类计算接口，负责将输入的tensor按元素取倒数。 |
| [Sqrt](Memory矢量计算/基础算术/Sqrt.md) | Sqrt属于单目矢量类计算接口，负责将输入的tensor按元素做开方。 |
| [Rsqrt](Memory矢量计算/基础算术/Rsqrt.md) | Rsqrt属于单目矢量类计算接口，负责将输入的tensor按元素进行开方后取倒数运算。 |
| [Relu](Memory矢量计算/基础算术/Relu.md) | Relu属于单目矢量类计算接口，负责将输入的tensor按元素做ReLU（Rectified Linear Unit）计算。 |
| [Neg(ISASI)](Memory矢量计算/基础算术/Neg(ISASI).md) | 按元素进行取相反数操作。 |
| [Add](Memory矢量计算/基础算术/Add.md) | Add属于双目矢量类计算接口，负责将输入的两个tensor按元素求和。 |
| [Sub](Memory矢量计算/基础算术/Sub.md) | Sub属于双目矢量类计算接口，负责将输入的两个tensor按元素求差。 |
| [Mul](Memory矢量计算/基础算术/Mul.md) | Mul属于双目矢量类计算接口，负责将输入的两个tensor按元素求积。 |
| [Div](Memory矢量计算/基础算术/Div.md) | Div属于双目矢量类计算接口，负责将输入的两个tensor按元素求商。 |
| [Max](Memory矢量计算/基础算术/Max.md) | Max属于双目矢量类计算接口，负责将输入的两个tensor按元素求最大值。 |
| [Min](Memory矢量计算/基础算术/Min.md) | Min属于双目矢量类计算接口，负责将输入的两个tensor按元素求最小值。 |
| [BilinearInterpolation(ISASI)](Memory矢量计算/基础算术/BilinearInterpolation(ISASI).md) | BilinearInterpolation属于双目矢量类计算接口，核心通过水平迭代+垂直迭代的双层循环完成乘累加运算，适用于非规则数据访问+权重乘加的向量运算场景（数据Gather+Multiply+Reduce）。 |
| [Prelu(ISASI)](Memory矢量计算/基础算术/Prelu(ISASI).md) | 源操作数src0大于0的情况下直接将src0写入目的操作数dst，否则将源操作数src0 * src1的结果写入dst。 |
| [Mull(ISASI)](Memory矢量计算/基础算术/Mull(ISASI).md) | 对前count个输入数据src0、src1按元素相乘操作，将结果写入dst0Local，溢出部分写入dst1Local。 |
| [Adds](Memory矢量计算/基础算术/Adds.md) | Adds属于双目标量类计算接口，输入为一个矢量tensor和一个标量scalar，Adds负责将矢量tensor内的每个元素与标量进行求和。 |
| [Adds（灵活标量位置）](Memory矢量计算/基础算术/Adds（灵活标量位置）.md) | 提供灵活标量位置的接口，支持标量在前和标量在后两种场景。其中标量输入支持配置LocalTensor单点元素。 |
| [Muls](Memory矢量计算/基础算术/Muls.md) | Muls属于双目标量类计算接口，输入为一个矢量tensor和一个标量scalar，Muls负责将矢量tensor内的每个元素与标量进行求积。 |
| [Muls（灵活标量位置）](Memory矢量计算/基础算术/Muls（灵活标量位置）.md) | 提供灵活标量位置的接口，支持标量在前和标量在后两种场景。其中标量输入支持配置LocalTensor单点元素。 |
| [Maxs](Memory矢量计算/基础算术/Maxs.md) | Maxs属于双目标量类计算接口，输入为一个矢量tensor和一个标量scalar，Maxs负责将矢量tensor内的每个元素与标量进行求最大值。 |
| [Maxs（灵活标量位置）](Memory矢量计算/基础算术/Maxs（灵活标量位置）.md) | 提供灵活标量位置的接口，支持标量在前和标量在后两种场景。其中标量输入支持配置LocalTensor单点元素。 |
| [Mins](Memory矢量计算/基础算术/Mins.md) | Mins属于双目标量类计算接口，输入为一个矢量tensor和一个标量scalar，Mins负责将矢量tensor内的每个元素与标量进行求最小值。 |
| [Mins（灵活标量位置）](Memory矢量计算/基础算术/Mins（灵活标量位置）.md) | 提供灵活标量位置的接口，支持标量在前和标量在后两种场景。其中标量输入支持配置LocalTensor单点元素。 |
| [Subs](Memory矢量计算/基础算术/Subs（灵活标量位置）.md) | 矢量内每个元素和标量间做减法，支持标量在前和标量在后两种场景，其中标量输入支持配置LocalTensor单点元素。 |
| [Divs](Memory矢量计算/基础算术/Divs（灵活标量位置）.md) | 矢量内每个元素和标量间做除法，支持标量在前和标量在后两种场景，其中标量输入支持配置LocalTensor单点元素。 |
| [LeakyRelu](Memory矢量计算/基础算术/LeakyRelu.md) | LeakyRelu属于双目标量类计算接口，输入为一个矢量tensor和一个标量scalar，LeakyRelu负责将矢量tensor内的每个元素执行Leaky ReLu（Leaky Rectified Linear Unit）操作。 |

### 逻辑计算
| 接口名 | 功能描述 |
| --- | --- |
| [Not](Memory矢量计算/逻辑计算/Not.md) | 对元素做按位取反。 |
| [And](Memory矢量计算/逻辑计算/And.md) | 对元素做按位与运算。 |
| [Or](Memory矢量计算/逻辑计算/Or.md) | 每对元素做按位或运算。 |
| [Ands](Memory矢量计算/逻辑计算/Ands.md) | 矢量内每个元素和标量间做与操作，支持标量在前和标量在后两种场景，其中标量输入支持配置LocalTensor单点元素。 |
| [Ors](Memory矢量计算/逻辑计算/Ors.md) | 矢量内每个元素和标量间做或操作，支持标量在前和标量在后两种场景，其中标量输入支持配置LocalTensor单点元素。 |
| [ShiftLeft](Memory矢量计算/逻辑计算/ShiftLeft.md) | 对源操作数中的每个元素进行左移操作，左移的位数由标量scalarValue决定。 |
| [ShiftLeft（左移位数为Tensor）](Memory矢量计算/逻辑计算/ShiftLeft（左移位数为Tensor）.md) | 对源操作数中的每个元素进行左移操作。 |
| [ShiftRight](Memory矢量计算/逻辑计算/ShiftRight.md) | 对源操作数中的每个元素进行右移操作，右移的位数由标量scalarValue决定。 |
| [ShiftRight（右移位数为Tensor）](Memory矢量计算/逻辑计算/ShiftRight（右移位数为Tensor）.md) | 源操作数内每个元素做右移。 |

### 复合计算
| 接口名 | 功能描述 |
| --- | --- |
| [Axpy](Memory矢量计算/复合计算/Axpy.md) | 向量和标量的乘积，并将乘积结果逐元素加到的输出向量上。 |
| [CastDequant](Memory矢量计算/复合计算/CastDequant.md) | 对输入做量化并进行精度转换，对输出结果的溢出采用饱和处理。本接口需要与SetDeqScale接口配合使用。 |
| [AddRelu](Memory矢量计算/复合计算/AddRelu.md) | 按元素求和，再进行Relu计算（结果和0对比取较大值）。 |
| [AddReluCast](Memory矢量计算/复合计算/AddReluCast.md) | 按元素求和，结果和0对比取较大值，并根据源操作数和目的操作数Tensor的数据类型进行精度转换。 |
| [AddDeqRelu](Memory矢量计算/复合计算/AddDeqRelu.md) | 依次计算按元素求和、结果进行deq量化后再进行relu计算（结果和0对比取较大值）。 |
| [SubRelu](Memory矢量计算/复合计算/SubRelu.md) | 按元素求差，再进行Relu计算（结果和0对比取较大值）。 |
| [SubReluCast](Memory矢量计算/复合计算/SubReluCast.md) | 按元素求差，再进行Relu计算（结果和0对比取较大值），并根据源操作数和目的操作数Tensor的数据类型进行精度转换。 |
| [MulAddDst](Memory矢量计算/复合计算/MulAddDst.md) | 按元素将src0和src1相乘并和dst相加，将最终结果存放进dst中。 |
| [MulCast](Memory矢量计算/复合计算/MulCast.md) | 按元素求积，并根据源操作数和目的操作数Tensor的数据类型进行精度转换。 |
| [FusedMulAdd](Memory矢量计算/复合计算/FusedMulAdd.md) | 按元素将src0和dst相乘并加上src1，最终结果存放入dst。 |
| [MulAddRelu](Memory矢量计算/复合计算/MulAddRelu.md) | 按元素将src0和dst相乘并加上src1，再进行Relu计算（结果和0对比取较大值），最终结果存放进dst中。 |
| [AbsSub(ISASI)](Memory矢量计算/复合计算/AbsSub(ISASI).md) | 将src0Local与src1相减再求绝对值，并将计算结果写入dst。 |
| [ExpSub(ISASI)](Memory矢量计算/复合计算/ExpSub(ISASI).md) | src0与src1相减，将差值作为指数计算自然常数e的幂次，并将计算结果写入dst。 |
| [MulsCast(ISASI)](Memory矢量计算/复合计算/MulsCast(ISASI).md) | 将矢量源操作数前count个数据与标量相乘再按照CAST_ROUND模式转换成half类型，并将计算结果写入dst，此接口支持标量在前和标量在后两种场景。 |

### 比较与选择
| 接口名 | 功能描述 |
| --- | --- |
| [Compare](Memory矢量计算/比较与选择/Compare.md) | 逐元素比较两个tensor大小，如果比较后的结果为真，则输出结果的对应比特位为1，否则为0。 |
| [Compare（结果存入寄存器）](Memory矢量计算/比较与选择/Compare（结果存入寄存器）.md) | 逐元素比较两个tensor大小，如果比较后的结果为真，则输出结果的对应比特位为1，否则为0。计算结果存入CmpMask 128bit寄存器中，可用于后续Select计算，也可使用GetCmpMask接口获取寄存器保存的数据。 |
| [Compares](Memory矢量计算/比较与选择/Compares.md) | 逐元素比较一个tensor中的元素和另一个Scalar的大小，如果比较后的结果为真，则输出结果的对应比特位为1，否则为0。 |
| [Compares（灵活标量位置）](Memory矢量计算/比较与选择/Compares（灵活标量位置）.md) | 提供灵活标量位置的接口，支持标量在前和标量在后两种场景。其中标量输入支持配置LocalTensor单点元素。 |
| [GetCmpMask(ISASI)](Memory矢量计算/比较与选择/GetCmpMask(ISASI).md) | 此接口用于获取Compare（结果存入寄存器）指令的比较结果。 |
| [SetCmpMask(ISASI)](Memory矢量计算/比较与选择/SetCmpMask(ISASI).md) | 设置比较寄存器的值，配合不传入mask参数的Select接口使用，根据不同的selMode传入不同的数据。 |
| [Select](Memory矢量计算/比较与选择/Select.md) | 给定两个源操作数src0和src1，根据selMask（用于选择的Mask掩码）的比特位值选取元素，得到目的操作数dst。选择规则为：当selMask的比特位是1时，从src0中选取；比特位是0时从src1选取。 |
| [Select（灵活标量位置）](Memory矢量计算/比较与选择/Select（灵活标量位置）.md) | 给定两个源操作数src0和src1，根据selMask（用于选择的Mask掩码）的比特位值选取元素，得到目的操作数dst。选择的规则为：当selMask的比特位是1时，从src0中选取，比特位是0时从src1选取。 |
| [GatherMask](Memory矢量计算/比较与选择/GatherMask.md) | 以**内置固定模式**或者**用户自定义输入的Tensor**数值对应的二进制为gather mask（数据收集的掩码），从源操作数中选取元素写入目的操作数中。1为选取，0为不选取。 |

### 类型转换
| 接口名 | 功能描述 |
| --- | --- |
| [SetDeqScale](Memory矢量计算/类型转换辅助配置接口/SetDeqScale.md) | 本接口用于设置DEQSCALE寄存器的值，DEQSCALE寄存器位宽为64bit，用于Vector计算单元上的量化计算，寄存器中存放的参数在不同场景下的含义不同。 |
| [Cast](Memory矢量计算/类型转换/Cast.md) | 根据源操作数和目的操作数tensor的数据类型进行精度转换。 |
| [Truncate(ISASI)](Memory矢量计算/类型转换/Truncate(ISASI).md) | 将源操作数的浮点数元素截断到整数位，同时源操作数的数据类型保持不变。 |

### 归约计算
| 接口名 | 功能描述 |
| --- | --- |
| [ReduceDataBlock](Memory矢量计算/归约计算/ReduceDataBlock.md) | `ReduceDataBlock`接口对输入数据以DataBlock为单位进行归约操作，根据模板参数`reduceType`，对每个DataBlock内的数据求和/求最大值/求最小值。 |
| [ReduceRepeat](Memory矢量计算/归约计算/ReduceRepeat.md) | `ReduceRepeat`接口用于对每个repeat内所有数据进行归约操作，根据模板参数reduceType进行求和/求最大值/求最小值操作，结果按顺序写入目标地址。 |
| [ReducePairElem](Memory矢量计算/归约计算/ReducePairElem.md) | `ReducePairElem`接口根据模板参数`reduceType`，对相邻两个（奇偶）元素进行归约操作，结果按顺序写入目标地址。**当前仅支持求和操作。** |
| [ReduceMax](Memory矢量计算/归约计算/ReduceMax.md) | `ReduceMax`接口用于从所有输入数据中找出最大值和最大值索引。 |
| [ReduceMin](Memory矢量计算/归约计算/ReduceMin.md) | `ReduceMin`接口用于从所有输入数据中找出最小值和最小值索引。 |
| [ReduceSum](Memory矢量计算/归约计算/ReduceSum.md) | `ReduceSum`接口对所有输入数据进行求和。 |
| [GetReduceRepeatSumSpr(ISASI)](Memory矢量计算/归约计算辅助配置接口/GetReduceRepeatSumSpr(ISASI).md) | 获取ReduceSum接口的计算结果，仅支持tensor前n个数据连续计算接口使用。计算结果以全局变量形式存储，可以随时调用获取。 |
| [GetReduceRepeatMaxMinSpr(ISASI)](Memory矢量计算/归约计算辅助配置接口/GetReduceRepeatMaxMinSpr(ISASI).md) | 本接口用于获取调用ReduceRepeat时所有repeat内的最值及其索引，或获取调用ReduceMax/ReduceMin得到的最值。计算结果以全局变量形式存储，可以随时调用获取。 |

### 数据排布转换
| 接口名 | 功能描述 |
| --- | --- |
| [Transpose](Memory矢量计算/数据排布转换/Transpose.md) | Transpose接口用于实现16*16的二维矩阵数据块转置或者[N,C,H,W]与[N,H,W,C]数据格式互相转换。 |
| [TransDataTo5HD](Memory矢量计算/数据排布转换/TransDataTo5HD.md) | TransDataTo5HD接口数据格式转换，一般用于将NCHW格式转换成NC1HWC0格式。特别的，也可以用于二维矩阵数据块的转置。完成转置功能时，相比于Transpose接口，Transpose仅支持16\*16大小的矩阵转置；本接口单次repeat内可处理512Byte的数据（16个DataBlock），根据数据类型不同，支持不同shape的矩阵转置，同时还可以支持多次repeat操作。 |

### 数据填充
| 接口名 | 功能描述 |
| --- | --- |
| [Duplicate](Memory矢量计算/数据填充/Duplicate.md) | Duplicate接口将一个变量或立即数复制多次并填充到向量中。 |
| [Brcb](Memory矢量计算/数据填充/Brcb.md) | Brcb接口对输入数据，每一次取输入张量中的8个数进行填充操作，将每个数填充到结果张量的一个DataBlock（32字节）中。 |
| [CreateVecIndex](Memory矢量计算/数据填充/CreateVecIndex.md) | CreateVecIndex接口创建指定起始值的向量索引。 |
| [VectorPadding(ISASI)](Memory矢量计算/数据填充/VectorPadding(ISASI).md) | 根据padMode（pad模式）与padSide（pad方向）对源操作数按照datablock进行填充操作。 |

### 排序组合（ISASI）
| 接口名 | 功能描述 |
| --- | --- |
| [ProposalConcat](Memory矢量计算/排序组合（ISASI）/ProposalConcat.md) | 将连续元素合入Region Proposal内对应位置，每次迭代会将16个连续元素合入到16个Region Proposals的对应位置里。 |
| [ProposalExtract](Memory矢量计算/排序组合（ISASI）/ProposalExtract.md) | 与ProposalConcat功能相反，从Region Proposals内将相应位置的单个元素抽取后重排，每次迭代处理16个Region Proposals，抽取16个元素后连续排列。 |
| [RpSort16](Memory矢量计算/排序组合（ISASI）/RpSort16.md) | 根据Region Proposals中的score域对其进行排序（score大的排前面），每次排16个Region Proposals。 |
| [MrgSort4](Memory矢量计算/排序组合（ISASI）/MrgSort4.md) | 将已经排好序的最多4条Region Proposals队列，排列并合并成1条队列，结果按照score域由大到小排序。 |
| [Sort32](Memory矢量计算/排序组合（ISASI）/Sort32.md) | Sort32接口实现一次迭代内对32个数的降序排列操作。 |
| [MrgSort](Memory矢量计算/排序组合（ISASI）/MrgSort.md) | 将已经排好序的最多4条队列，合并排列成1条队列，结果按照score域由大到小排序。 |
| [GetMrgSortResult](Memory矢量计算/排序组合（ISASI）/GetMrgSortResult.md) | GetMrgSortResult接口需与MrgSort接口配合使用。当MrgSort接口中的ifExhaustedSuspension参数设置为true时，若某个输入队列中的数据耗尽，MrgSort会停止执行。此时，可调用GetMrgSortResult接口来获取4个队列里已经处理过的数据个数。 |

### 离散与聚合
| 接口名 | 功能描述 |
| --- | --- |
| [Gather](Memory矢量计算/离散与聚合/Gather.md) | Gather接口接受输入张量（src）、地址偏移张量（srcOffset）和基地址（srcBaseAddr），根据基地址和地址偏移量确定输入张量的索引，将输入张量中对应元素收集到结果张量（dst）中。 |
| [Gatherb(ISASI)](Memory矢量计算/离散与聚合/Gatherb(ISASI).md) | Gatherb接口接受输入张量（src）、DataBlock偏移张量（offset），根据索引位置将输入张量按DataBlock（32字节）收集到结果张量（dst）中。 |
| [Scatter(ISASI)](Memory矢量计算/离散与聚合/Scatter(ISASI).md) | 给定一个连续的输入张量和一个目的地址偏移张量，Scatter指令根据偏移地址生成新的结果张量后将输入张量分散到结果张量中。 |

### 掩码操作
| 接口名 | 功能描述 |
| --- | --- |
| [SetMaskCount](Memory矢量计算/掩码操作/SetMaskCount.md) | 设置Mask模式为Counter模式。该模式下，不需要开发者去感知迭代次数、处理非对齐的尾块等操作，可直接传入计算数据量，实际迭代次数由Vector计算单元自动推断。本接口推荐配合API中isSetMask模板参数使用，当isSetMask为false时，支持用户调用本接口手动管理Counter模式，并通过SetVectorMask设置Counter模式下参与计算的元素个数。 |
| [SetMaskNorm](Memory矢量计算/掩码操作/SetMaskNorm.md) | 设置Mask模式为Normal模式。该模式为系统默认模式，支持开发者配置迭代次数。本接口推荐配合API中isSetMask模板参数使用，当isSetMask为false时，支持用户调用本接口手动管理Normal模式，并通过SetVectorMask设置Normal模式下的掩码。 |
| [SetVectorMask](Memory矢量计算/掩码操作/SetVectorMask.md) | 本接口用于在矢量计算时设置mask。 |
| [ResetMask](Memory矢量计算/掩码操作/ResetMask.md) | 恢复mask的值为默认值（全1），表示矢量计算中每次迭代内的所有元素都将参与运算。 |

### 数据重排（ISASI）
| 接口名 | 功能描述 |
| --- | --- |
| [Interleave](Memory矢量计算/数据重排（ISASI）/Interleave.md) | 给定源操作数src0和src1，将src0和src1中的元素交织存入结果操作数dst0和dst1中。 |
| [DeInterleave](Memory矢量计算/数据重排（ISASI）/DeInterleave.md) | 给定源操作数src0和src1，将src0和src1中的元素解交织存入结果操作数dst0和dst1中。 |

## Reg矢量计算

### 寄存器数据类型
| 接口名 | 功能描述 |
| --- | --- |
| [RegTensor](Reg矢量计算/寄存器数据类型/RegTensor.md) | Reg矢量计算基本单元，RegTensor位宽为VL（Vector Length），具体值可能因不同AI处理器型号而异。 |
| [MaskReg](Reg矢量计算/寄存器数据类型/MaskReg.md) | MaskReg用于指示在计算过程中哪些元素参与计算，宽度为RegTensor的八分之一（VL/8）。 |
| [UnalignRegForLoad & UnalignRegForStore](Reg矢量计算/寄存器数据类型/UnalignRegForLoad-UnalignRegForStore.md) | UnalignRegForLoad、UnalignRegForStore用作缓冲区来优化UB和RegTensor之间连续不对齐地址访问的开销。在读不对齐地址前，UnalignRegForLoad、UnalignRegForStore应该通过LoadUnAlignPre API初始化，然后使用LoadUnAlign API。在写不对齐地址时，先使用StoreUnAlign API， |
| [AddrReg](Reg矢量计算/寄存器数据类型/AddrReg.md) | AddrReg即为Address Register（地址寄存器），是用于存储地址偏移量的寄存器。AddrReg应该通过CreateAddrReg API初始化，然后在循环中使用AddrReg存储地址偏移量。AddrReg在每层循环中根据所设置的stride进行自增。 |

### Reg数据搬运
| 接口名 | 功能描述 |
| --- | --- |
| [连续对齐搬入](Reg矢量计算/Reg数据搬入/连续对齐搬入（LoadAlign）.md) | Reg矢量计算数据搬运接口，适用于从UB连续对齐搬入RegTensor。单搬入模式下，可以将数据从UB搬运到一个目的寄存器，双搬入模式下，可以将数据从UB搬运到两个目的寄存器。 |
| [连续对齐搬出](Reg矢量计算/Reg数据搬出/连续对齐搬出（StoreAlign）.md) | Reg矢量计算数据搬运接口，适用于从RegTensor连续对齐搬出到UB。 |
| [非连续对齐搬入](Reg矢量计算/Reg数据搬入/非连续对齐搬入（LoadAlign）.md) | Reg矢量计算数据搬运接口，适用于从UB非连续对齐搬入RegTensor（以DataBlock为单位）。 |
| [非连续对齐搬出](Reg矢量计算/Reg数据搬出/非连续对齐搬出（StoreAlign）.md) | Reg矢量计算数据搬运接口，适用于从RegTensor非连续对齐搬出到UB（以DataBlock为单位）。 |
| [连续非对齐搬入](Reg矢量计算/Reg数据搬入/连续非对齐搬入（LoadUnAlign）.md) | Reg矢量计算数据搬运接口，适用于从UB非32B对齐地址起始连续搬入RegTensor。 |
| [连续非对齐搬出](Reg矢量计算/Reg数据搬出/连续非对齐搬出（StoreUnAlign）.md) | Reg矢量计算数据搬运接口，适用于从RegTensor连续非对齐搬出到UB。 |
| [MaskReg搬入](Reg矢量计算/Reg数据搬入/MaskReg搬入（LoadAlign）.md) | Reg矢量计算数据搬运接口，适用于从UB或RegTensor搬入MaskReg。 |
| [MaskReg搬出](Reg矢量计算/Reg数据搬出/MaskReg搬出（StoreAlign）.md) | Reg矢量计算数据搬运接口，适用于从MaskReg搬出到UB。 |
| [Move](Reg矢量计算/Reg数据搬入/Move.md) | 对srcReg中的有效元素逐个复制写入dstReg中对应位置处。 |

### MaskReg计算
| 接口名 | 功能描述 |
| --- | --- |
| [Move](Reg矢量计算/MaskReg计算/Move.md) | 将src中的元素复制到dst中的对应位置。如果有输入mask，则仅复制被mask选定的有效元素，无效元素填0。 |
| [Interleave](Reg矢量计算/MaskReg计算/Interleave.md) | 将源操作数src0和src1中的元素交织存入目的操作数dst0和dst1中。 |
| [DeInterleave](Reg矢量计算/MaskReg计算/DeInterleave.md) | 将源操作数src0和src1中的元素解交织存入目的操作数dst0和dst1中。 |
| [Select](Reg矢量计算/MaskReg计算/Select.md) | 给定两个源操作数src0和src1，根据mask的比特位值选取元素，得到目的操作数dst。选择的规则为：当mask的比特位是1时，从src0中选取对应位置的数，比特位是0时从src1选取对应位置的数。 |
| [Pack](Reg矢量计算/MaskReg计算/Pack.md) | 根据所选的低位模式或高位模式，将源操作数src的偶数位bit，提取到目的操作数dst的低半部分或高半部分。 |
| [UnPack](Reg矢量计算/MaskReg计算/UnPack.md) | 根据所选的低位模式或高位模式，将源操作数src的低半部分或高半部分，展开到目的操作数dst。展开方式为：将每bit展开为2bit，高位置零。 |
| [MoveMask](Reg矢量计算/MaskReg计算/MoveMask.md) | 从SetVectorMask设置的掩码寄存器{MASK1, MASK0}中读取Mask值，并按模板参数T对应的数据格式转换后写入返回值MaskReg。 |

### 基础算术
| 接口名 | 功能描述 |
| --- | --- |
| [Abs](Reg矢量计算/基础算术/Abs.md) | 对srcReg中的有效元素逐个取绝对值，并将结果写入dstReg对应位置。 |
| [Relu](Reg矢量计算/基础算术/Relu.md) | 该接口根据mask对输入数据srcReg进行Relu（线性整流）操作，将结果写入dstReg。 |
| [Exp](Reg矢量计算/基础算术/Exp.md) | 该接口根据mask，对源操作数srcReg进行按元素指数操作，将结果写入目的操作数dstReg。 |
| [Sqrt](Reg矢量计算/基础算术/Sqrt.md) | 该接口根据mask，对源操作数srcReg逐元素做平方根运算，将结果写入目的操作数dstReg。 |
| [Ln](Reg矢量计算/基础算术/Ln.md) | 该接口根据mask逐元素对源操作数求自然对数，将结果写入目的操作数。 |
| [Log](Reg矢量计算/基础算术/Log.md) | 该接口根据mask逐元素对源操作数求自然对数，将结果写入目的操作数。 |
| [Log2](Reg矢量计算/基础算术/Log2.md) | 该接口用于根据mask对输入数据srcReg进行求以2为底的对数操作，将结果写入dstReg。 |
| [Log10](Reg矢量计算/基础算术/Log10.md) | 该接口根据mask逐元素对源操作数求以10为底的对数，将结果写入目的操作数。 |
| [Neg](Reg矢量计算/基础算术/Neg.md) | 该接口用于根据mask对输入数据srcReg进行取相反数操作，将结果写入dstReg。 |
| [Add](Reg矢量计算/基础算术/Add.md) | 该接口根据mask，对源操作数srcReg0、srcReg1进行按元素求和操作，将结果写入目的操作数dstReg。 |
| [Sub](Reg矢量计算/基础算术/Sub.md) | 该接口根据mask，对源操作数srcReg0、srcReg1进行按元素求差操作，将结果写入目的操作数dstReg。 |
| [Mul](Reg矢量计算/基础算术/Mul.md) | 该接口根据mask对输入数据srcReg0、srcReg1按元素相乘操作，将结果写入dstReg。 |
| [Div](Reg矢量计算/基础算术/Div.md) | 该接口根据mask，对源操作数srcReg0、srcReg1进行按元素相除操作，将结果写入目的操作数dstReg。 |
| [Max](Reg矢量计算/基础算术/Max.md) | 该接口根据mask，对源操作数srcReg0、srcReg1进行按元素求最大值操作，将结果写入目的操作数dstReg。 |
| [Min](Reg矢量计算/基础算术/Min.md) | 该接口根据mask，对源操作数srcReg0、srcReg1进行按元素求最小值操作，将结果写入目的操作数dstReg。 |
| [Mull](Reg矢量计算/基础算术/Mull.md) | 该接口根据mask对输入数据srcReg0、srcReg1按元素相乘操作，将乘法结果的低位部分写入dstReg0，溢出（高位）部分写入dstReg1。 |
| [AddC](Reg矢量计算/基础算术/AddC.md) | 该接口根据mask，对源操作数srcReg0、srcReg1及输入进位carrySrc进行按元素求和操作，将结果写入目的操作数dstReg，同时将每个元素的进位结果写入carry。 |
| [SubC](Reg矢量计算/基础算术/SubC.md) | 该接口根据mask，对源操作数srcReg0、srcReg1及输入进位carrySrc进行按元素求差操作，将结果写入目的操作数dstReg，同时将每个元素的进位结果写入carry。 |
| [Prelu](Reg矢量计算/基础算术/Prelu.md) | 该接口用于实现PReLU（Parametric ReLU）激活函数：当源操作数srcReg0中对应元素大于0时，直接将该元素写入目的操作数dstReg；否则将srcReg0与srcReg1对应元素相乘的结果写入dstReg。 |
| [Adds](Reg矢量计算/基础算术/Adds.md) | 该接口根据mask，对源操作数srcReg按元素与标量scalarValue进行求和操作，将结果写入目的操作数dstReg。 |
| [Muls](Reg矢量计算/基础算术/Muls.md) | 矢量内每个元素与标量求积。 |
| [Maxs](Reg矢量计算/基础算术/Maxs.md) | 该接口根据mask，对源操作数srcReg按元素与标量相比，如果比标量大，则取源操作数值；如果比标量小，则取标量值，将结果写入目的操作数dstReg。 |
| [Mins](Reg矢量计算/基础算术/Mins.md) | 该接口根据mask，对源操作数srcReg按元素与标量相比，如果比标量大，则取标量值，比标量小，则取源操作数值，将结果写入目的操作数dstReg。 |
| [LeakyRelu](Reg矢量计算/基础算术/LeakyRelu.md) | 该接口用于按元素执行Leaky ReLU(Leaky Rectified Linear Unit)操作：当源操作数中某元素大于0时，直接将该元素写入目的操作数；否则将该元素乘以标量值scalarValue后写入目的操作数。 |

### 逻辑计算
| 接口名 | 功能描述 |
| --- | --- |
| [Not](Reg矢量计算/逻辑计算/Not.md) | 本节介绍两种接口，分别用于对RegTensor和MaskReg进行有效bit进行取反运算得到结果并保存。 |
| [And](Reg矢量计算/逻辑计算/And.md) | 本节介绍两种接口，分别用于对RegTensor和MaskReg进行有效bit进行逻辑与运算得到结果并保存。 |
| [Or](Reg矢量计算/逻辑计算/Or.md) | 本节介绍两种接口，分别用于对RegTensor和MaskReg进行有效bit进行逻辑或运算得到结果并保存。 |
| [Xor](Reg矢量计算/逻辑计算/Xor.md) | 本节介绍两种接口，分别用于对RegTensor和MaskReg进行有效bit进行按位异或运算得到结果并保存。 |
| [ShiftLeft](Reg矢量计算/逻辑计算/ShiftLeft.md) | 根据mask，对输入数据srcReg0，按照srcReg1对应元素进行左移操作，并将结果写入dstReg。 |
| [ShiftRight](Reg矢量计算/逻辑计算/ShiftRight.md) | 根据mask，对输入数据srcReg0按照srcReg1对应元素进行右移操作，将结果写入dstReg。 |
| [ShiftLefts](Reg矢量计算/逻辑计算/ShiftLefts.md) | 源操作数内每个元素做逻辑左移，逻辑左移的位数由输入参数scalarValue决定。 |
| [ShiftRights](Reg矢量计算/逻辑计算/ShiftRights.md) | 源操作数内每个元素做右移，右移的位数由输入参数scalarValue决定。 |

### 复合计算
| 接口名 | 功能描述 |
| --- | --- |
| [Axpy](Reg矢量计算/复合计算/Axpy.md) | 根据mask对输入数据dstReg、srcReg、scalarValue按元素做乘加操作，将结果写入dstReg。 |
| [AbsSub](Reg矢量计算/复合计算/AbsSub.md) | srcReg0与srcReg1相减再求绝对值，根据mask将计算结果写入dstReg。 |
| [ExpSub](Reg矢量计算/复合计算/ExpSub.md) | srcReg0与srcReg1相减，差值作为e的指数计算，根据mask将计算结果写入dstReg。 |
| [MulDstAdd](Reg矢量计算/复合计算/MulDstAdd.md) | dstReg与srcReg0相乘后与srcReg1相加，根据mask将计算结果写入dstReg。 |
| [MulAddDst](Reg矢量计算/复合计算/MulAddDst.md) | srcReg0与srcReg1相乘再加上dstReg的值，根据mask将计算结果写入dstReg。 |
| [MulsCast](Reg矢量计算/复合计算/MulsCast.md) | src与scalar相乘的结果再按照CAST_ROUND模式转换成half类型，根据mask将计算结果写入dst。 |

### 比较与选择
| 接口名 | 功能描述 |
| --- | --- |
| [Compare](Reg矢量计算/比较与选择/Compare.md) | 逐元素比较两个RegTensor大小，如果比较后的结果为真，则输出结果的对应比特位为1，否则为0。 |
| [Compares](Reg矢量计算/比较与选择/Compares.md) | 逐元素比较一个Tensor中的元素和另一个scalar的大小，如果比较后的结果为真，则输出结果的对应比特位为1，否则为0。 |
| [Select](Reg矢量计算/比较与选择/Select.md) | 给定两个源操作数srcReg0和srcReg1，根据mask的比特位值选取元素，得到目的操作数dstReg。选择的规则为：当mask的比特位是1时，从srcReg0中选取对应位置的数，比特位是0时从srcReg1选取对应位置的数。 |
| [Squeeze](Reg矢量计算/比较与选择/Squeeze.md) | 将传入的srcReg中被mask选择的有效元素依次复制到dstReg中，有效元素在dstReg中从低到高连续排列。dstReg中剩余位置元素置为0。 |

### 类型转换
| 接口名 | 功能描述 |
| --- | --- |
| [Cast](Reg矢量计算/类型转换/Cast.md) | Cast用于数据类型精度转换，将源操作数数据类型转换成目的操作数数据类型，能够实现浮点转整数、浮点转浮点、整数转浮点、整数转整数的数据类型转换。 |
| [Truncate](Reg矢量计算/类型转换/Truncate.md) | 将源操作数中的浮点数元素截断为整数值（保留原数据类型），并存入目的操作数。 |

### 归约计算
| 接口名 | 功能描述 |
| --- | --- |
| [Reduce](Reg矢量计算/归约计算/Reduce.md) | 归约指令根据ReduceType，将数据集合简化为单一值或者更小的集合。 |
| [ReduceDataBlock](Reg矢量计算/归约计算/ReduceDataBlock.md) | 归约指令根据ReduceType，将数据集合在每个DataBlock（32B）内简化为单一值。 |
| [PairReduceElem](Reg矢量计算/归约计算/PairReduceElem.md) | 将传入的srcReg中相邻两个数值相加，并将产生的结果保存在dstReg中的低位位置。 |

### 数据填充
| 接口名 | 功能描述 |
| --- | --- |
| [Duplicate](Reg矢量计算/数据填充/Duplicate.md) | 支持Scalar和Tensor两种模式。 |

### 离散操作
| 接口名 | 功能描述 |
| --- | --- |
| [Gather](Reg矢量计算/离散操作/Gather.md) | 该指令会根据索引值indexReg将源操作数srcReg按元素收集到目的操作数dstReg中。 |

### 数据重排
| 接口名 | 功能描述 |
| --- | --- |
| [Interleave](Reg矢量计算/数据重排/Interleave.md) | 给定源操作数寄存器srcReg0和srcReg1，将srcReg0和srcReg1中的元素交织存入结果操作数dstReg0和dstReg1中。 |
| [DeInterleave](Reg矢量计算/数据重排/DeInterleave.md) | 给定源操作数寄存器srcReg0和srcReg1，将srcReg0和srcReg1中的元素解交织存入结果操作数dstReg0和dstReg1中。 |

### 数据压缩
| 接口名 | 功能描述 |
| --- | --- |
| [Unsqueeze](Reg矢量计算/数据压缩/Unsqueeze.md) | 将dstReg中数据根据mask进行解压缩。解压缩方式：dstReg中第0个元素置为0，dstReg中的第i个元素等于mask中从第0个到第\(i-1\)个元素中1的数量。mask最高位被忽略不参与统计。 |
| [Pack](Reg矢量计算/数据压缩/Pack.md) | 将源操作数srcReg中的元素选取低8位（对于b16类型）、低16位（对于b32类型）、低32位（对于b64类型）写入dstReg的低半部分或高半部分。 |
| [UnPack](Reg矢量计算/数据压缩/UnPack.md) | 对于无符号整型，将源操作数srcReg中低半部分或高半部分的元素以高位填0扩充位宽的方式写入dstReg。对于有符号整型，将源操作数srcReg中低半部分或高半部分的元素以保持符号位扩充位宽的方式写入dstReg。 |

### 直方图计算
| 接口名 | 功能描述 |
| --- | --- |
| [Histograms](Reg矢量计算/直方图计算/Histograms.md) | 对直方图数据进行统计，在目的操作数dstReg的基础数据上加上源操作数srcReg数据的统计结果，包括数据的频率统计和累计统计。 |

### 索引操作
| 接口名 | 功能描述 |
| --- | --- |
| [Arange](Reg矢量计算/索引操作/Arange.md) | 该函数以传入的scalar的值为起始值，生成递增/递减的索引，并将索引保存在dstReg中。 |

### 同步控制
| 接口名 | 功能描述 |
| --- | --- |
| [LocalMemBar](Reg矢量计算/同步控制/LocalMemBar.md) | Reg矢量计算宏函数内不同流水线之间的同步指令。该同步指令指定src源流水线和dst目的流水线，目的流水线将等待源流水线上所有指令完成才进行执行。读写场景下，当读指令使用的寄存器和写指令使用的寄存器相同时，可以触发寄存器保序，指令将会按照代码顺序执行，不需要插入同步指令，而当使用的寄存器不同时，如果要确保读写指令顺序执行，则需要插入同步指令，写写场景同理。 |
| [AllocMutexID (ISASI)](同步控制/核内同步/AllocMutexID-(ISASI).md) | 从框架获取并占用一个MutexID，与ReleaseMutexID配合使用，管理MutexID的获取和释放。获取的MutexID可以传入Mutex接口使用，此时Mutex可以与TQue等其他接口配合使用。 |
| [DataSyncBarrier(ISASI)](同步控制/核内同步/DataSyncBarrier(ISASI).md) | 阻塞后续的指令执行，直到所有之前的内存访问指令（需要等待的内存位置可通过参数控制）执行结束，用于解决Scalar的内存依赖。 |
| [PipeBarrier(ISASI)](同步控制/核内同步/PipeBarrier(ISASI).md) | 完成同一流水线内的同步控制，用于在同一流水线内部约束执行顺序。其作用是，保证前序指令中所有数据的读写工作全部完成，后序指令才能执行。 |
| [ReleaseMutexID (ISASI)](同步控制/核内同步/ReleaseMutexID-(ISASI).md) | 从框架释放一个MutexID，与AllocMutexID配合使用。 |
| [SetFlag/WaitFlag(ISASI)](同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) | SetFlag/WaitFlag接口用于核内多流水间的同步。 |
| [CrossCoreSetFlag(ISASI)](同步控制/核间同步/CrossCoreSetFlag(ISASI).md) | 在核间同步场景中，CrossCoreSetFlag接口和CrossCoreWaitFlag接口配对工作，使用时需传入核间同步的标记ID（flagId），每个ID对应一个用于控制同步的计数器。CrossCoreSetFlag接口用于通知调度模块“本核的pipe流水任务已经完成”，CrossCoreWaitFlag接口用于阻塞后续指令下发，直到所有相关核均完成同步后方可解除阻塞。 |
| [CrossCoreWaitFlag(ISASI)](同步控制/核间同步/CrossCoreWaitFlag(ISASI).md) | 在核间同步场景中，CrossCoreSetFlag接口和CrossCoreWaitFlag接口配对工作，具体功能请参考[CrossCoreSetFlag](同步控制/核间同步/CrossCoreSetFlag(ISASI).md)。 |

### 系统变量访问
| 接口名 | 功能描述 |
| --- | --- |
| [ClearSpr](Reg矢量计算/系统变量访问/ClearSpr.md) | 对指定的特殊寄存器进行清零。 |

### 辅助数据类型
| 接口名 | 功能描述 |
| --- | --- |
| [PostLiteral](Reg矢量计算/辅助数据类型/PostLiteral.md) | enum class PostLiteral  |
| [RegLayout](Reg矢量计算/辅助数据类型/RegLayout.md) | enum class RegLayout  |
| [SatMode](Reg矢量计算/辅助数据类型/SatMode.md) | 设置Cast类型转换饱和与不饱和模式。 |
| [MaskMergeMode](Reg矢量计算/辅助数据类型/MaskMergeMode.md) | 决定mask未选择元素对于dst元素是否置0。 |
| [RoundMode](Reg矢量计算/辅助数据类型/RoundMode.md) | 控制舍入模式。 |

## 标量计算
| 接口名 | 功能描述 |
| --- | --- |
| [GetBitCount](标量计算/GetBitCount.md) | 统计uint64_t类型数值的二进制表示中指定比特值（0或1）的出现次数。 |
| [CountLeadingZero](标量计算/CountLeadingZero.md) | 统计uint64_t类型数值的二进制表示中前导0的个数（二进制表示中最高有效位到第一个'1'的'0'的数量，输入为0时返回64）。 |
| [CountBitsCntSameAsSignBit](标量计算/CountBitsCntSameAsSignBit.md) | 统计int64_t类型数值的二进制表示中，从最高数值位开始与符号位相同的连续比特位的个数。 |
| [GetSFFValue](标量计算/GetSFFValue.md) | 查找uint64_t类型数值的二进制表示中从最低有效位开始的首个指定比特值（0或1）的位置，如果没找到则返回-1。 |
| [CeilDivision](标量计算/CeilDivision.md) | 计算两个整数num1和num2相除后向上取整结果。 |
| [Cast（float转half/int32_t）](标量计算/Cast（float转half-int32_t）.md) | 对标量的数据类型进行转换。 |
| [Cast（float转bfloat16_t）](标量计算/Cast（float转bfloat16_t）.md) | float类型标量数据转换成bfloat16_t类型标量数据。 |
| [Cast（多类型转float）](标量计算/Cast（多类型转float）.md) | 该接口将输入数据转换为float类型。 |
| [Nop](标量计算/Nop.md) | 用户输入延迟的个数，实现延迟若干个cycle。 |
| [WriteGmByPassDCache(ISASI)](标量计算/WriteGmByPassDCache(ISASI).md) | 不经过DCache向GM地址上写数据。 |
| [ReadGmByPassDCache(ISASI)](标量计算/ReadGmByPassDCache(ISASI).md) | 不经过DCache从GM地址上读数据。 |

## 资源管理
| 接口名 | 功能描述 |
| --- | --- |
| [TPipe](资源管理/TPipe/TPipe.md) | TPipe是用来管理全局内存等资源的框架。通过TPipe类提供的接口可以完成内存等资源的分配管理操作。 |
| [GetTPipePtr](资源管理/GetTPipePtr.md) | 创建TPipe对象时，对象初始化会设置全局唯一的TPipe指针。本接口用于获取该指针，获取该指针后，可进行TPipe相关的操作。 |
| [TBufPool](资源管理/TBufPool/TBufPool.md) | TPipe可以管理全局内存资源，而TBufPool可以手动管理或复用Unified Buffer/L1 Buffer物理内存，主要用于多个stage计算中Unified Buffer/L1 Buffer物理内存不足的场景。 |
| [TQue](资源管理/TQue/TQue.md) | 提供入队出队等接口，通过队列（Queue）完成任务间同步。 |
| [TQueBind](资源管理/TQueBind/TQueBind.md) | TQueBind绑定源逻辑位置和目的逻辑位置，根据源位置和目的位置，来确定内存分配的位置 、插入对应的同步事件，帮助开发者解决内存分配和管理、同步等问题。 |
| [TBuf](资源管理/TBuf/TBuf.md) | 使用Ascend C编程的过程中，可能会用到一些临时变量。这些临时变量占用的内存可以使用TBuf数据结构来管理。 |
| [InitSpmBuffer](资源管理/TPipe/InitSpmBuffer.md) | 初始化SPM Buffer。 |
| [WriteSpmBuffer](资源管理/TPipe/WriteSpmBuffer.md) | 将需要溢出暂存的数据拷贝到SPM Buffer中。 |
| [ReadSpmBuffer](资源管理/TPipe/ReadSpmBuffer.md) | 从SPM Buffer读回到local数据中。 |
| [GetUserWorkspace](资源管理/workspace/GetUserWorkspace.md) | 获取用户使用的workspace指针。 |
| [SetSysWorkSpace](资源管理/workspace/SetSysWorkSpace.md) | 在进行融合算子编程时，由于框架通信机制需要使用到workspace，也就是系统workspace，所以在该场景下，开发者要调用该接口，设置系统workspace的指针。 |
| [GetSysWorkSpacePtr](资源管理/workspace/GetSysWorkSpacePtr.md) | 获取系统workspace指针。 |
| [PopStackBuffer](资源管理/PopStackBuffer.md) | 在指定position（逻辑位置）申请临时空间，空间大小为指定position的全部剩余空间。 |

## 同步控制
| 接口名 | 功能描述 |
| --- | --- |
| [TQueSync](同步控制/核内同步/TQueSync模板参数.md) | TQueSync类提供同步控制接口，开发者可以使用这类API来自行完成同步控制。 |
| [IBSet](同步控制/核间同步/IBSet.md) | 调用IBSet设置某一个核的标志位，与IBWait成对使用，表示核之间的同步等待指令：被等待核中调用IBSet，通过将值1写入全局内存中某块地址来通知等待核；而等待核中调用IBWait，持续读取全局内存中相应地址块的值，直到其值变为1，从而实现核间同步。 |
| [IBWait](同步控制/核间同步/IBWait.md) | 调用IBSet设置某一个核的标志位，与IBWait成对使用，表示核之间的同步等待指令：被等待核中调用IBSet，通过将值1写入全局内存中某块地址来通知等待核；而等待核中调用IBWait，持续读取全局内存中相应地址块的值，直到其值变为1，从而实现核间同步。 |
| [SyncAll](同步控制/核间同步/SyncAll.md) | SyncAll是核间同步控制接口。 |
| [InitDetermineComputeWorkspace](同步控制/核间同步/InitDetermineComputeWorkspace.md) | InitDetermineComputeWorkspace是基于核间顺序执行的确定性计算的初始化配置接口，能够初始化GM共享内存的值，完成初始化后才可以调用WaitPreBlock和NotifyNextBlock。以上三个接口共同完成基于核间顺序执行的确定性计算，确定性计算的具体含义请参考确定性计算。 |
| [NotifyNextBlock](同步控制/核间同步/NotifyNextBlock.md) | WaitPreBlock和NotifyNextBlock是核间同步控制接口。NotifyNextBlock通过写全局内存来通知其他核当前核已执行完成，其他核可以继续往下执行；WaitPreBlock通过读取全局内存，判断当前核是否可以继续往下执行。 |
| [WaitPreBlock](同步控制/核间同步/WaitPreBlock.md) | WaitPreBlock和NotifyNextBlock是核间同步控制接口。NotifyNextBlock通过写全局内存来通知其他核当前核已执行完成，其他核可以继续往下执行；WaitPreBlock通过读取全局内存，判断当前核是否可以继续往下执行。 |
| [SetNextTaskStart](同步控制/任务间同步/SetNextTaskStart.md) | 在SuperKernel的子Kernel中调用，调用后的指令可以和后续其他的子Kernel实现并行，提升整体性能。SuperKernel按序调用子Kernel，为保证子Kernel之间数据互不干扰，会在子Kernel间插入算子间同步进行保序，子KernelN-1调用该接口后，之后的指令会和后续子KernelN实现并行。 |
| [WaitPreTaskEnd](同步控制/任务间同步/WaitPreTaskEnd.md) | 在SuperKernel的子Kernel中调用，调用前的指令可以和前序其他的子Kernel实现并行，提升整体性能。SuperKernel按序调用子Kernel，为保证子Kernel之间数据互不干扰，会在子Kernel间插入算子间同步进行保序，子KernelN+1调用该接口之前的指令会和前序子KernelN实现并行。 |

## 缓存控制
| 接口名 | 功能描述 |
| --- | --- |
| [DataCachePreload](缓存控制/DataCachePreload.md) | 从源地址所在的特定GM地址预加载数据到DCache中，每次调用只能预加载一个Cache Line大小的数据。 |
| [DataCacheCleanAndInvalid](缓存控制/DataCacheCleanAndInvalid.md) | 在AI Core内部，Scalar单元和DMA单元都可能对GM进行访问。 |
| [ICachePreLoad(ISASI)](缓存控制/ICachePreLoad(ISASI).md) | 开发者手动调用ICachePreLoad接口，能够从指令所在GM地址预加载指令到ICache中。 |
| [GetICachePreloadStatus(ISASI)](缓存控制/GetICachePreloadStatus(ISASI).md) | GetICachePreloadStatus为调试接口，在ICachePreLoad后调用，用于获取ICache的PreLoad的状态：当返回值为0时，说明ICache的PreLoad已完成；当返回值为1时，说明ICache的PreLoad未完成。 |

## 原子操作
| 接口名 | 功能描述 |
| --- | --- |
| [SetAtomicAdd](原子操作/SetAtomicAdd.md) | 对后续目的地址为GM的数据搬运开启原子累加。原子累加过程：将待拷贝的内容和GM已有内容进行求和，然后将求和结果写入GM。SetAtomicAdd接口可通过模板参数设定不同的累加数据类型。 |
| [SetAtomicType](原子操作/SetAtomicType.md) | 通过设置模板参数来设定原子操作不同的数据类型。 |
| [DisableDmaAtomic](原子操作/DisableDmaAtomic.md) | 关闭数据搬运随路原子操作功能，后续执行数据搬运时，GM中原始数据将被新搬运数据完全覆盖。 |
| [SetAtomicMax(ISASI)](原子操作/SetAtomicMax(ISASI).md) | 设置后续搬运到GM的数据是否执行原子比较：将待拷贝的内容和GM已有内容进行比较，然后将最大值写入GM。SetAtomicMax接口可通过设置模板参数来设定不同的数据类型。 |
| [SetAtomicMin(ISASI)](原子操作/SetAtomicMin(ISASI).md) | 设置后续搬运到GM的数据是否执行原子比较：将待拷贝的内容和GM已有内容进行比较，然后将最小值写入GM。SetAtomicMin接口可通过设置模板参数来设定不同的数据类型。 |
| [SetStoreAtomicConfig(ISASI)](原子操作/SetStoreAtomicConfig(ISASI).md) | 设置原子操作开启位与原子操作类型。 |
| [GetStoreAtomicConfig(ISASI)](原子操作/GetStoreAtomicConfig(ISASI).md) | 获取原子操作开启位与原子操作类型的值。 |
| [AtomicAdd](原子操作/AtomicAdd.md) | 调用该接口后，可在指定GM地址上进行原子加操作。 |
| [AtomicMin](原子操作/AtomicMin.md) | 调用该接口后，可在指定GM地址上进行原子比较取小操作。 |
| [AtomicMax](原子操作/AtomicMax.md) | 调用该接口后，可在指定GM地址上进行原子取大操作。 |
| [AtomicCas](原子操作/AtomicCas.md) | 调用该接口后，可在指定GM地址上进行原子比较，如果和value1相等，则把value2的值赋值到GM上；如果和value1不相等，则GM上的值不变。 |
| [AtomicExch](原子操作/AtomicExch.md) | 在GM内存中执行原子交换操作。具体来说，它读取指定GM地址上的数据，并将新的值存储回同一地址。函数返回旧值。 |

## 调试接口
| 接口名 | 功能描述 |
| --- | --- |
| [DumpTensor](调试接口/上板打印/DumpTensor.md) | 该接口可以打印Tensor的内容，同时支持打印自定义的标签（仅支持uint32_t数据类型的信息），比如打印当前行号等。 |
| [printf](调试接口/上板打印/printf.md) | 该接口提供CPU域/NPU域调试场景下的格式化输出功能。 |
| [DumpAccChkPoint](调试接口/上板打印/DumpAccChkPoint.md) | 该接口Dump指定Tensor的内容。同时支持打印自定义的标签（仅支持uint32_t数据类型的信息），比如打印当前行号等。区别于DumpTensor，使用该接口可以支持指定偏移位置的Tensor打印。 |
| [PrintTimeStamp](调试接口/上板打印/PrintTimeStamp.md) | 提供时间戳打点功能，用于在算子Kernel代码中标记关键执行点。 |
| [ascendc_assert](调试接口/异常检测/ascendc_assert.md) | ascendc_assert提供了一种在CPU/NPU域实现断言功能的接口。当断言条件不满足时，系统会输出断言信息并格式化打印在屏幕上。 |
| [assert](调试接口/异常检测/assert.md) | 该接口实现CPU/NPU域assert断言功能。算子执行中，如果assert内部条件判断不为真，则输出assert条件并将输入的信息格式化打印在屏幕上。 |
| [Trap](调试接口/异常检测/Trap.md) | 在Kernel侧调用，NPU模式下会中断AI Core的运行，CPU模式下等同于assert。可用于Kernel侧异常场景的调试。 |
| [CheckLocalMemoryIA(ISASI)](调试接口/异常检测/CheckLocalMemoryIA(ISASI).md) | check设定范围内的UB读写行为，如果有设定范围的读写行为则会出现EXCEPTION报错，无设定范围的读写行为则不会报错。 |
| [GmAlloc](调试接口/CPU孪生调试/GmAlloc.md) | 进行核函数的CPU侧运行验证时，用于创建共享内存：在/tmp目录下创建一个共享文件，并返回该文件的映射指针。 |
| [ICPU_RUN_KF](调试接口/CPU孪生调试/ICPU_RUN_KF.md) | 进行核函数的CPU侧运行验证时，CPU调测总入口，完成CPU侧的算子程序调用。 |
| [ICPU_SET_TILING_KEY](调试接口/CPU孪生调试/ICPU_SET_TILING_KEY.md) | 用于指定本次CPU调测使用的tilingKey。调测执行时，将只执行算子核函数中该tilingKey对应的分支。 |
| [GmFree](调试接口/CPU孪生调试/GmFree.md) | 进行核函数的CPU侧运行验证时，用于释放通过GmAlloc申请的共享内存。 |
| [SetKernelMode](调试接口/CPU孪生调试/SetKernelMode.md) | 针对分离模式，CPU调测时，设置内核模式为单AIV模式，单AIC模式或者MIX模式，以分别支持单AIV矢量算子，单AIC矩阵算子，MIX混合算子的CPU调试。不调用该接口的情况下，默认为MIX模式。为保证算子代码在多个硬件平台兼容，耦合模式下也可以调用，该场景下接口不会生效，不影响正常调试。 |
| [MetricsProfStart](调试接口/性能统计/MetricsProfStart.md) | 用于设置性能数据采集信号启动，和MetricsProfStop配合使用。使用msOpProf工具进行算子上板调优时，可在kernel侧代码段前后分别调用MetricsProfStart和MetricsProfStop来指定需要调优的代码段范围。 |
| [MetricsProfStop](调试接口/性能统计/MetricsProfStop.md) | 设置性能数据采集信号停止，和MetricsProfStart配合使用。使用msOpProf工具进行算子上板调优时，可在kernel侧代码段前后分别调用MetricsProfStart和MetricsProfStop来指定需要调优的代码段范围。 |
| [MarkStamp](调试接口/性能统计/MarkStamp.md) | 用户通过调用接口，用于在算子执行过程中标记特定位置，便于后期通过流水图分析代码执行路径与性能热点。 |

## 工具接口
| 接口名 | 功能描述 |
| --- | --- |
| [Async](工具接口/执行模式/Async.md) | Async通过模板函数的方式对这种隔离模式进行了封装，提供了一个统一的接口，用于在不同执行单元（AIC或AIV）下执行特定函数，从而避免在代码中使用硬件条件分支。 |
| [InitSocState](工具接口/系统初始化/InitSocState.md) | 本接口对AI Core的全局状态进行初始化，包括AIC（Cube Core）与AIV（Vector Core）的公共状态及各自特有状态。由于不同产品的实现存在差异，实际执行的初始化项也有所不同。 |
| [Max](工具接口/NumericLimits工具类/Max.md) | 返回指定数据类型的最大有限值。 |
| [Lowest](工具接口/NumericLimits工具类/Lowest.md) | 返回指定数据类型的最低有限值，即满足无其他有限值y符合y < x的有限值x。 |
| [Min](工具接口/NumericLimits工具类/Min.md) | 返回指定数据类型的最小有限值。 |
| [Infinity](工具接口/NumericLimits工具类/Infinity.md) | 返回指定数据类型的正无穷大值。 |
| [NegativeInfinity](工具接口/NumericLimits工具类/NegativeInfinity.md) | 返回指定数据类型的负无穷大值。 |
| [QuietNaN](工具接口/NumericLimits工具类/QuietNaN.md) | 返回指定数据类型的安静NaN值（浮点尾数最高位为1）。 |
| [SignalingNaN](工具接口/NumericLimits工具类/SignalingNaN.md) | 返回指定数据类型的发信NaN值（浮点尾数最高位为0）。 |
| [DeNormMin](工具接口/NumericLimits工具类/DeNormMin.md) | 返回指定数据类型的最小正的非正规值。 |
| [GetTaskRatio](工具接口/系统资源与变量/GetTaskRatio.md) | 分离模式下，获取任务启动的Cube Core（AIC）或者Vector Core（AIV）的数量与逻辑AI Core数量的比例。 |
| [GetUBSizeInBytes](工具接口/系统资源与变量/GetUBSizeInBytes.md) | 获取UB空间的大小，单位为Byte。开发者根据UB的大小来计算循环次数等参数值。 |
| [GetRuntimeUBSize](工具接口/系统资源与变量/GetRuntimeUBSize.md) | 获取运行时UB空间的大小，单位为Byte。开发者根据UB的大小来计算循环次数等参数值。 |
| [GetVecLen](工具接口/系统资源与变量/GetVecLen.md) | 获取RegTensor位宽VL（Vector Length）的大小。 |
| [GetSsbufBaseAddr](工具接口/系统资源与变量/GetSsbufBaseAddr.md) | 该接口用于获取SSBuffer的基地址。 |
| [GetSystemCycle(ISASI)](工具接口/系统资源与变量/GetSystemCycle(ISASI).md) | 获取系统计数器当前的cycle值。通过计算代码段前后cycle值的差值，可得到代码段消耗的系统cycle数，再通过换算即可获得代码段的执行时间。 |
| [GetBlockNum](工具接口/系统资源与变量/GetBlockNum.md) | 获取当前任务配置的逻辑AI Core的数量，用于代码内部的多核逻辑控制等。 |
| [GetBlockIdx](工具接口/系统资源与变量/GetBlockIdx.md) | 获取当前逻辑AI Core的索引，用于代码内部的多核逻辑控制及多核偏移量计算等。 |
| [GetDataBlockSizeInBytes](工具接口/系统资源与变量/GetDataBlockSizeInBytes.md) | 获取当前芯片版本一个DataBlock的大小，单位为Byte。 |
| [GetArchVersion](工具接口/系统资源与变量/GetArchVersion.md) | 获取当前AI处理器架构版本号。 |
| [GetProgramCounter(ISASI)](工具接口/系统资源与变量/GetProgramCounter(ISASI).md) | 获取程序计数器的值，以追踪当前程序执行的位置。 |
| [GetSubBlockNum(ISASI)](工具接口/系统资源与变量/GetSubBlockNum(ISASI).md) | 在分离模式架构下，获取当前配置中，一个逻辑AI Core上Cube Core（AIC）或者Vector Core（AIV）的数量。 |
| [GetSubBlockIdx(ISASI)](工具接口/系统资源与变量/GetSubBlockIdx(ISASI).md) | 在分离模式架构下，获取逻辑AI Core上Cube Core（AIC）或者Vector Core（AIV）的逻辑索引。 |

## Cube分组管理（ISASI）
| 接口名 | 功能描述 |
| --- | --- |
| [CreateCubeResGroup](Cube分组管理（ISASI）/CubeResGroupHandle/CreateCubeResGroup.md) | 快速创建CubeResGroupHandle对象，内部完成消息队列空间和同步事件分配。推荐使用该接口，避免使用CubeResGroupHandle的构造函数创建对象，出现不同对象的消息队列空间冲突、同步事件错误等情况。 |
| [AssignQueue](Cube分组管理（ISASI）/CubeResGroupHandle/AssignQueue.md) | 用于AIV绑定CubeResGroupHandle中某一个消息队列的序号。 |
| [AllocMessage](Cube分组管理（ISASI）/CubeResGroupHandle/AllocMessage.md) | AIV从消息队列里申请消息空间，用于存放消息结构体，返回当前申请的消息空间的地址。消息队列的深度固定为4，申请消息空间的顺序为自上而下，然后循环。当消息队列指针指向的消息空间为FREE状态时，AllocMessage返回空间的地址，否则循环等待，直到当前空间的状态为FREE。 |
| [PostMessage](Cube分组管理（ISASI）/CubeResGroupHandle/PostMessage.md) | 通过AllocMessage接口获取到消息空间地址msg后，构造消息结构体CubeMsgType，发送该消息。 |
| [PostFakeMsg](Cube分组管理（ISASI）/CubeResGroupHandle/PostFakeMsg.md) | 通过AllocMessage接口获取到消息空间地址后，AIV发送假消息，刷新消息状态msgState为FAKE。 |
| [SetQuit](Cube分组管理（ISASI）/CubeResGroupHandle/SetQuit.md) | 通过AllocMessage接口获取到消息空间地址后，发送退出消息，告知该消息队列对应的AIC无需处理该队列的消息。Queue5对应的AIV发了退出消息后，Block1将不再处理Queue5的任何消息。 |
| [Wait](Cube分组管理（ISASI）/CubeResGroupHandle/Wait.md) | 在调用PostMessage或PostFakeMessage后，查询该消息是否已被AIC处理完。 |
| [FreeMessage](Cube分组管理（ISASI）/CubeResGroupHandle/FreeMessage.md) | 在自定义的回调函数逻辑中，完成消息处理后，调用该接口，刷新消息状态为FREE；或者待消息状态为指定状态waitState时，刷新消息状态为FREE。 |
| [SetSkipMsg](Cube分组管理（ISASI）/CubeResGroupHandle/SetSkipMsg.md) | AIC跳过指定个数假消息的处理，仅在回调函数中调用。下图中Block0通过调用SetSkipMsg跳过三个假消息。 |
| [Arrive](Cube分组管理（ISASI）/GroupBarrier/Arrive.md) | 通知其他等待的AIV，本AIV已经完成其依赖的任务。 |
| [GetWorkspaceLen](Cube分组管理（ISASI）/GroupBarrier/GetWorkspaceLen.md) | 返回当前GroupBarrier所占用的Global Memory消息空间大小。 |
| [UpdateKfcWorkspace](Cube分组管理（ISASI）/KfcWorkspace/UpdateKfcWorkspace.md) | 更新用于CubeResGroupHandle消息通信区的内存地址。用户使用CubeResGroupHandle接口时，需要用此接口自主管理空间地址。 |
| [GetKfcWorkspace](Cube分组管理（ISASI）/KfcWorkspace/GetKfcWorkspace.md) | 获取用于CubeResGroupHandle消息通信区的内存地址。用户使用CubeResGroupHandle接口时，需要用此接口自主管理空间地址。 |

## Kernel-Tiling
| 接口名 | 功能描述 |
| --- | --- |
| [GET_TILING_DATA](Kernel-Tiling/GET_TILING_DATA.md) | 用于获取算子kernel入口函数传入的Tiling信息，并填入注册的TilingData结构体中，此函数会以宏展开的方式进行编译。对应的算子host实现中需要定义TilingData结构体，实现并注册计算TilingData的Tiling函数。如果用户通过TilingData结构注册注册了多个TilingData结构体，使用该接口返回默认注册的结构体。 |
| [GET_TILING_DATA_WITH_STRUCT](Kernel-Tiling/GET_TILING_DATA_WITH_STRUCT.md) | 使用该接口指定结构体名称，可获取指定的tiling信息，并填入对应的Tiling结构体中，此函数会以宏展开的方式进行编译。与GET\_TILING\_DATA的区别是：GET\_TILING\_DATA只能获取默认注册的结构体，该接口可以根据指定的结构体名称获取对应的结构体，常用于针对不同的TilingKey注册了不同结构体的情况下。 |
| [GET_TILING_DATA_MEMBER](Kernel-Tiling/GET_TILING_DATA_MEMBER.md) | 用于获取tiling结构体的成员变量。 |
| [GET_TILING_DATA_PTR_WITH_STRUCT](Kernel-Tiling/GET_TILING_DATA_PTR_WITH_STRUCT.md) | 在使用该宏时，开发者可以通过指定结构体名称来获取相应的Tiling信息，并将其填入对应的Tiling结构体中。完成填充后，该宏将返回一个指向该Tiling结构体的指针，并使用\_\_tiling\_data\_ptr\_\_修饰符对该指针进行修饰。这种修饰方式能够确保在动静态Shape场景下代码的统一性和兼容性。 |
| [COPY_TILING_WITH_STRUCT](Kernel-Tiling/COPY_TILING_WITH_STRUCT.md) | 拷贝Tiling结构体，并返回指向拷贝后的Tiling结构体的指针。该宏适用于嵌套结构体场景，可拷贝结构体的子结构体成员变量。该宏将指定结构体拷贝至栈上，适用于频繁访问Tiling数据的场景，能够加快数据访问速度。 |
| [COPY_TILING_WITH_ARRAY](Kernel-Tiling/COPY_TILING_WITH_ARRAY.md) | 拷贝指定大小的数组内容到目标数组中，并返回指向拷贝后数组的指针。适用于拷贝一个结构体的数组成员变量的场景。该宏将指定数组拷贝至栈上，适用于频繁访问Tiling数据的场景，能够加快数据访问速度。 |
| [TILING_KEY_IS](Kernel-Tiling/TILING_KEY_IS.md) | 在核函数中判断本次执行时的tiling_key是否等于host侧运行时设置的某个key，从而标识tiling_key==key的一条kernel分支。 |
| [TILING_KEY_LIST](Kernel-Tiling/TILING_KEY_LIST.md) | TILING_KEY_LIST函数用于在核函数中判断当前执行的TilingKey是否与Host侧配置的指定TilingKey匹配，从而标识满足TilingKey == key1或TilingKey == key2条件的分支逻辑。 |
| [REGISTER_TILING_DEFAULT](Kernel-Tiling/REGISTER_TILING_DEFAULT.md) | 用于在kernel侧注册用户使用标准C++语法自定义的默认TilingData结构体。 |
| [REGISTER_TILING_FOR_TILINGKEY](Kernel-Tiling/REGISTER_TILING_FOR_TILINGKEY.md) | 用于在kernel侧注册与TilingKey相匹配的TilingData自定义结构体；该接口需提供一个逻辑表达式，逻辑表达式以字符串“TILING_KEY_VAR”代指实际TilingKey，表达TilingKey所满足的范围。 |
| [REGISTER_NONE_TILING](Kernel-Tiling/REGISTER_NONE_TILING.md) | 在Kernel侧使用标准C++语法自定义的TilingData结构体时，若用户不确定需要注册哪些结构体，可使用该接口告知框架侧需使用未注册的标准C++语法来定义TilingData，并配套GET\_TILING\_DATA\_WITH\_STRUCT，GET\_TILING\_DATA\_MEMBER， |
| [设置Kernel类型](Kernel-Tiling/设置Kernel类型.md) | 用于用户自定义设置kernel类型，控制算子执行时只启动该类型的核，避免启动不需要工作的核，缩短核启动开销。 |

## 特殊寄存器访问
| 接口名 | 功能描述 |
| --- | --- |
| [SetCtrlSpr(ISASI)](特殊寄存器访问/SetCtrlSpr(ISASI).md) | 对CTRL寄存器（控制寄存器）的特定比特位进行设置。 |
| [GetCtrlSpr(ISASI)](特殊寄存器访问/GetCtrlSpr(ISASI).md) | 读取CTRL寄存器（控制寄存器）特定比特位上的值。 |
| [ResetCtrlSpr(ISASI)](特殊寄存器访问/ResetCtrlSpr(ISASI).md) | 重置CTRL寄存器（控制寄存器）的特定比特位。 |
| [GetSaturationFlag(ISASI)](特殊寄存器访问/GetSaturationFlag(ISASI).md) | 获取计算/精度转换时饱和模式的开启状态。 |
| [SetSaturationFlag(ISASI)](特殊寄存器访问/SetSaturationFlag(ISASI).md) | 设置饱和开关的标志位，以控制计算/精度转换时饱和模式的开启与关闭。 |
