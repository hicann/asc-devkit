# 术语表<a name="ZH-CN_TOPIC_0000002191896252"></a>

**表1**  术语表<a name="table276611752817"></a>

| 术语/缩略语 | 含义 |
|---|---|
| A1 | AscendC::TPosition::A1代表设备上用于矩阵计算的逻辑内存，用于存放左矩阵，物理存储对应AI Core的L1 Buffer。 |
| A2 | AscendC::TPosition::A2代表设备上用于矩阵计算的逻辑内存，用于存放小块左矩阵（如经过分割、适配L0A Buffer容量的分块），物理存储对应AI Core的L0A Buffer。 |
| AddrReg | Address Register（地址寄存器），是用于存储地址偏移量的寄存器。 |
| AI Core | AI处理器的计算核，负责执行矩阵、矢量计算密集的任务。 |
| AIC | 在AI Core分离模式下，一组Cube Core和Vector Core组合中的Cube Core。 |
| AIV | 在AI Core分离模式下，一组Cube Core和Vector Core组合中的Vector Core。 |
| Ascend IR | Ascend Intermediate Representation，AI处理器专用的、用于表达计算流程的抽象数据结构。在本文档中，若无特殊说明，IR默认指代Ascend IR。 |
| B1 | AscendC::TPosition::B1代表设备上用于矩阵计算的逻辑内存，用于存放右矩阵，物理存储对应AI Core的L1 Buffer。 |
| B2 | AscendC::TPosition::B2代表设备上用于矩阵计算的逻辑内存，用于存放小块右矩阵（如经过分割、适配L0B Buffer容量的分块），物理存储对应AI Core的L0B Buffer。 |
| Block | Block在不同场景下具有多种含义，通常情况下指AI Core的逻辑核。典型场景有：<br>- AI Core逻辑核：一个Block表示一个AI Core的逻辑核，其BlockID是以0为起始的逻辑编号。<br>- DataBlock：一个DataBlock表示一条NPU矢量计算指令处理的数据单元，大小通常为32字节，一条指令可同时处理多个DataBlock。<br>- 基本块：表示一次计算需要的典型数据块大小。 |
| BlockID | 以0为起始的AI Core逻辑编号，可以比实际硬件核数大。 |
| numBlocks | 参与计算的逻辑AI Core核数，在调用核函数（Kernel）时由开发者指定，其值一般等于或大于实际物理核数。 |
| BiasTable Buffer | 偏置存储，AI Core内部物理存储单元，通常用于存储矩阵计算所需的Bias（偏置）数据。 |
| Broadcast | 广播，一种张量操作机制。通过广播，较小的张量可以自动扩展以匹配较大的张量的形状。 |
| C1 | AscendC::TPosition::C1代表设备上用于矩阵计算的逻辑内存，用于存放Bias（偏置）数据，物理存储对应AI Core的L1 Buffer或Unified Buffer（UB）。 |
| C2 | AscendC::TPosition::C2代表设备上用于矩阵计算的逻辑内存，用于存放小块Bias（偏置）数据（如经过分割、适配BT Buffer容量的分块），物理存储对应AI Core的BT Buffer或L0C Buffer。 |
| C2PIPE2GM | AscendC::TPosition::C2PIPE2GM代表设备上用于矩阵计算的逻辑内存，用于存放量化参数，物理存储对应AI Core的Fixpipe Buffer。 |
| Cache Line | 缓存（DCache、ICache、L2 Cache）中的最小数据单位。 |
| Core | 拥有独立Scalar计算单元的计算核，Scalar计算单元承担了核内的指令发射等功能，也称之为核内的调度单元。 |
| CO1 | AscendC::TPosition::CO1代表设备上用于矩阵计算的逻辑内存，用于存放小块矩阵计算结果（如经过分割的矩阵计算结果分块），物理存储对应AI Core的L0C Buffer。 |
| CO2 | AscendC::TPosition::CO2代表设备上用于矩阵计算的逻辑内存，用于存放矩阵计算结果（如原始矩阵的最终计算结果），物理存储对应Global Memory或AI Core的UB。 |
| Compute | Ascend C算子编程范式中典型的三个阶段之一，负责完成计算任务。 |
| CopyIn | Ascend C算子编程范式中典型的三个阶段之一，负责将待计算数据从Global Memory搬运到Local Memory。 |
| CopyOut | Ascend C算子编程范式中典型的三个阶段之一，负责将计算结果从Local Memory搬运到Global Memory。 |
| Core ID | AI Core核的物理编号，与实际硬件核数一一对应。 |
| <a name="p1038918214519"></a>Cube | AI Core上的Cube计算单元，负责执行矩阵运算。以float16数据类型为例，Cube每次执行可完成两个float16类型的16x16矩阵的乘法操作。 |
| Cube Core | 矩阵计算核，专注于矩阵计算。由Scalar调度单元、矩阵计算单元、搬运单元等组成，不包括矢量计算单元。 |
| DataBlock | 矢量计算指令处理的数据单元，大小通常为32字节，矢量计算指令执行一次，可同时处理多个DataBlock。 |
| DataBlock Stride | 矢量计算指令单次Repeat内DataBlock的间隔大小，即下次处理的起始数据地址与本次处理的起始数据地址之间的DataBlock个数。 |
| DCache | Data Cache，数据缓存。<br>用于缓存Scalar计算单元近期可能被重复访问的数据段，以提升访问效率。 |
| Device | Device指安装了昇腾AI处理器的硬件设备，利用PCIe接口与主机Host侧连接，为Host提供神经网络计算能力。若存在多个Device，多个Device之间的内存资源不能共享。 |
| Dim3 | Dim3是用于定义单个线程块中三维线程结构的数据类型。使用AscendC::Simt::Dim3在启动SIMT VF时指定3个维度的取值，3个维度的乘积必须小于等于2048。 |
| DMA | Direct Memory Access，直接内存访问单元。<br>负责数据搬运，包括Global Memory和Local Memory之间的数据搬运以及不同层级Local Memory之间的数据搬运，包含搬运单元MTE2、MTE3等。 |
| DoubleBuffer/DB | 双缓冲，并行领域常用的优化方式，通过创建多个持有数据的缓冲区（Buffer）提高数据处理的并行性。 |
| Elementwise | 元素级操作是对张量的每个元素独立进行的操作。每个元素的结果仅依赖于对应的输入元素。 |
| Fixpipe | AI Core中负责将矩阵计算结果从L0C Buffer搬运到Global Memory或L1 Buffer的单元，搬运过程中随路完成量化、激活等操作。 |
| Fixpipe Buffer | AI Core内部物理存储单元，通常用于存储Fixpipe搬运过程中所需的量化参数等数据。 |
| <a name="p1439018225112"></a>Global Memory/GM | 设备端的主内存，AI Core的外部存储，用于存储大规模数据，但需要优化访问模式以提升性能。 |
| GlobalTensor | 存放Global Memory全局数据的Tensor。 |
| Host | 指与设备端Device相连接的X86服务器、ARM服务器，会利用Device提供的NN（Neural-Network ）计算能力，完成业务。 |
| ICache | Instruction Cache，指令缓存。<br>用于缓存最近或频繁使用的指令。极致性能优化时，需要关注如何降低ICache Miss（指令缓存未命中）。 |
| InferShape | 算子shape推导，仅在GE图模式时才使用。实际的网络模型生成过程中，会先进行Tensor shape以及datatype的推导。这样可以在图执行之前，就知道各Tensor的数据类型和形状，提前校验其正确性；同时提前推理出算子的输出张量描述，包括张量的形状、数据类型及数据排布格式等信息，算子构图准备阶段就可以为所有的张量静态分配内存，避免动态内存分配带来的开销。 |
| ISASI | Instruction Set Architecture Special Interface（硬件体系结构相关接口）。该类接口不能保证跨硬件版本兼容。 |
| 核函数（Kernel） | 核函数（Kernel），是Device设备上执行的并行函数。核函数（Kernel）通过__global__修饰，多个核并行执行相同的核函数（Kernel），其主要区别是不同核函数（Kernel）运行时具有不同的BlockID。 |
| Kernel Launch | 将核函数（Kernel）程序提交至硬件进行启动执行的过程。 |
| L0A Buffer | AI Core内部物理存储单元，通常用于存储矩阵计算的左矩阵。 |
| L0B Buffer | AI Core内部物理存储单元，通常用于存储矩阵计算的右矩阵。 |
| L0C Buffer | AI Core内部物理存储单元，通常用于存储矩阵计算的结果。 |
| L1 Buffer | AI Core内部物理存储单元，空间相对较大，通常用于缓存矩阵计算的输入数据。矩阵计算的输入一般需要从GM搬运到L1 Buffer，然后分别搬运到L0A Buffer和L0B Buffer。 |
| L2 Cache | 二级缓存，专门用于存储频繁访问的数据，以便减少对Global Memory的读写。 |
| Lane | Warp中的每个线程称为Lane。 |
| LCM | Local Cache Memory，AscendC::TPosition::LCM代表临时共享的UB空间，与VECCALC实现同样的功能。 |
| <a name="p193901420518"></a>Local Memory | AI Core的内部存储，包括L1 Buffer、L0A Buffer、L0B Buffer、L0C Buffer、UB等存储单元。 |
| LocalTensor | 存放AI Core中Local Memory本地数据的Tensor。 |
| Mask | 用于控制矢量计算指令每次Repeat内参与计算的元素，可通过连续模式和逐比特模式两种方式进行设置。 |
| MaskReg | 掩码寄存器，是一个256位的寄存器，用于指示在计算过程中RegTensor的哪些元素参与计算。 |
| Membase | Membase是指基于内存的架构。Membase架构中所有操作均基于内存进行，这意味着每次计算都需要从Local Memory加载数据，计算完成后将结果搬回Local Memory，中间计算结果都需要暂存在Local Memory上。 |
| <a name="p339019220518"></a>MTE1 | Memory Transfer Engine 1，AI Core的数据传递引擎，负责将数据从L1 Buffer搬运到L0A Buffer或L0B Buffer等。注意：不同硬件能力可能有差异。 |
| <a name="p53901325511"></a>MTE2 | Memory Transfer Engine 2，AI Core的数据传递引擎，负责将数据从GM搬运到L1 Buffer、L0A Buffer、L0B Buffer、UB等。注意：不同硬件能力可能有差异。 |
| <a name="p183901322511"></a>MTE3 | Memory Transfer Engine 3，AI Core的数据传递引擎，负责将数据从UB搬运到Global Memory、L1 Buffer以及从L1 Buffer搬运到Global Memory等等。注意：不同硬件能力可能有差异。 |
| NC1HWC0 | 一种五维数据格式，其中C0与硬件架构强相关，采用该格式可提升矩阵乘法的计算效率。 |
| NCHW | 按照[Batch, Channels, Height, Width]的排列顺序存储特征图数据。 |
| ND | 普通格式，N维张量。 |
| NHWC | 按照[Batch, Height, Width, Channels]的排列顺序存储特征图数据。 |
| NPU | Neural-Network Processing Unit，神经网络处理器单元。采用“数据驱动并行计算”的架构，专门用于处理人工智能应用中的大量计算任务。 |
| OP | 算子（Operator，简称OP），是深度学习算法中执行特定数学运算或操作的基础单元，例如激活函数（如ReLU）、卷积（Conv）、池化（Pooling）以及归一化（如Softmax）。通过组合这些算子，可以构建神经网络模型。 |
| OpType | 算子类型，一类算子的统称。例如，在网络中可能会出现多个Add算子，名称分别为Add1、Add2，但这类算子的OpType均为Add。 |
| Pipe | Ascend C编程范式核心概念之一，用于统一管理Device端内存等资源，一个核函数（Kernel）必须且只能初始化一个Pipe对象。 |
| Preload | 在计算任务开始前，预先将必要的指令或数据加载到缓存中，用于减少指令或数据访问的延迟，提高计算效率。 |
| Reduce | 减维操作，用于减少多维张量的维度。常见的减维操作包括求和、求平均、求最大值、求最小值等。 |
| RegTensor | 矢量数据寄存器，Reg矢量计算基本单元。 |
| Regbase | Regbase是指基于寄存器的架构。Regbase架构下，中间结果可暂存在寄存器中，无需数据搬出到Local Memory的开销。 |
| Repeat | 矢量计算指令执行一次，读取8个DataBlock数据进行计算，称之为一个迭代（Repeat）。通常情况下，需要循环执行多次才能完成所有数据的读取与计算。 |
| Repeat Stride | 矢量计算指令循环执行时，下一次Repeat起始数据地址与当前Repeat起始数据地址之间的DataBlock个数。 |
| Repeat Times | 矢量计算指令循环执行的次数。 |
| <a name="p1739112219516"></a>Scalar | AI Core上的标量计算单元，主要负责标量数据运算和对其他单元（如MTE数据搬运单元、Vector矢量计算单元、Cube矩阵计算单元）的指令发射。 |
| SIMD | SIMD：Single Instruction, Multiple Data，一条指令同时对多个数据进行相同的操作。 |
| SIMT | SIMT：Single Instruction, Multiple Threads，一条指令被多个线程并行执行，每个线程处理不同的数据。 |
| SPMD | Single-Program Multiple-Data，一种并行程序设计模型，其主要思想是使用同一个程序在多个核上并行执行，但每个核处理不同数据。 |
| SSBuffer | AI Core内部物理存储单元，分离模式下，Cube Core和Vector Core 1:2通信可选择使用SSBuffer在Cube Core和Vector Core的Scalar单元之间通信。 |
| SuperKernel | SuperKernel是一种算子的二进制融合技术，与源码融合不同，它聚焦于核函数（Kernel）的二进制的调度方案，展开深度优化，于已编译的二进制代码基础上融合创建一个超级核函数（Kernel）（SuperKernel），以调用子函数的方式调用多个其他核函数（Kernel），也就是子核函数（Kernel）。相对于单算子下发，SuperKernel技术可以减少任务调度等待时间和调度开销，同时利用Task间隙资源进一步优化算子头开销。 |
| Tensor | Tensor张量是算子计算数据的容器，是N维数据结构，最常见的是标量、矢量或矩阵。张量的元素可以包含整数值、浮点值或字符串值。 |
| Thread Block | 线程块，一个线程块最大支持2048个线程（Thread）。SIMT VF一次在AI Core上运行一个线程块（Block）。 |
| Tiling | Tiling指数据的切分和分块。计算数据量较大时，需要将数据进行多核切分、每个核也需要分块多次计算。 |
| TilingData | TilingData指数据切分和分块的相关参数（如每次搬运的块大小、循环次数）。鉴于设备端Scalar计算能力限制，一般Tiling参数在Host侧计算完成，然后传输到设备侧供核函数（Kernel）使用。 |
| TilingFunc | 算子工程提供的在Host侧计算Tiling的默认函数。 |
| TilingKey | 用来区分核函数（Kernel）不同版本的特例实现，不同的TilingKey会编译生成不同二进制。 |
| TPosition | Ascend C管理不同层级的物理内存时，用一种抽象的逻辑位置（TPosition）来表达各级别的存储，代替了片上物理存储的概念，达到隐藏硬件架构的目的。TPosition类型包括：VECIN、VECOUT、VECCALC、A1、A2、B1、B2、CO1、CO2等，其中VECIN、VECCALC、VECOUT主要用于矢量编程，A1、A2、B1、B2、CO1、CO2用于矩阵编程。 |
| <a name="p1392229512"></a>TSCM | AscendC::TPosition::TSCM表示L1 Buffer空间对应的逻辑内存，需开发者自行管理以高效利用硬件资源，主要用于Matmul计算。比如，开发者可缓存一份TSCM数据，在不同使用场景中灵活配置为Matmul操作的A矩阵、B矩阵或Bias偏置矩阵，实现内存复用与计算效率优化。 |
| UnalignRegForLoad | 非对齐寄存器，用作缓冲区来优化UB和RegTensor之间连续不对齐地址访问的开销，适用于连续非对齐搬入场景。 |
| UnalignRegForStore | 非对齐寄存器，用作缓冲区来优化UB和RegTensor之间连续不对齐地址访问的开销，适用于连续非对齐搬出场景。 |
| <a name="p17392122195118"></a>Unified Buffer/UB | AI Core内部物理存储单元，主要用于矢量计算，也可用于存储矢量计算过程中的输入、输出和临时数据。 |
| VECCALC | Vector Calculation，AscendC::TPosition::VECCALC代表设备上用于矢量计算的逻辑内存，用于存放临时变量，物理存储对应AI Core的UB。 |
| VECIN | Vector Input，AscendC::TPosition::VECIN代表设备上用于矢量计算的逻辑内存，用于存放矢量计算的输入数据，物理存储对应AI Core的UB。 |
| VECOUT | Vector Output，AscendC::TPosition::VECOUT代表设备上用于矢量计算的逻辑内存，用于存放矢量计算的输出数据，物理存储对应AI Core的UB。 |
| <a name="p203923211517"></a>Vector | AI Core上的Vector计算单元，负责执行矢量运算。其算力低于Cube，但灵活度高于Cube（如支持数学中的求倒数，求平方根等）。 |
| Vector Core | 矢量计算核，专注于矢量计算，由Scalar调度单元、矢量计算单元、搬运单元等组成，不包括矩阵计算单元。 |
| VF | Vector Function（向量函数）是由标量计算单元调用的一组连续的向量指令，用__simt_vf__或__simd_vf__标记。 |
| VL | Vector Length，RegTensor的位宽，通常取值为256Byte。 |
| Warp | 每个线程块被切分成多个Warp，Warp是执行相同指令的线程集合，每个Warp包含32个线程（thread），一个Block的多个Warp被依次调度到AI Core中的同一个AIV核执行。 |
| Workspace | 通常情况下指一个预分配的、临时使用的Global Memory内存，用于存储中间结果或临时数据。 |
| CPU域调试 | Ascend C提供的一种孪生调试方法，在CPU上模拟设备侧核函数（Kernel）的执行和调试，仅调试算子功能和精度。 |
| 基本块 | 一次计算需要的典型数据块大小。 |
| 静态Tensor编程 | 静态Tensor编程方式，相比基于Pipe的编程方式，这种方式避免了TPipe内存管理初始化过程（约数百纳秒），从而减少了运行时开销，更有助于开发者实现极致性能。通过直接构造指定地址和存储位置的LocalTensor，并将其传递给计算、搬运等API进行编程，提供了更高的灵活性。 |
| 核函数（Kernel）直调 | 一种简单直接的核函数（Kernel）调用方式。<br>完成核函数（Kernel）侧算子实现和Host侧Tiling实现后，即可通过运行时接口，完成算子核函数（Kernel）直调。该方式下Tiling开发不受CANN框架的限制，简单直接，多用于算子功能的快速验证。 |
| NPU域调试 | Ascend C提供的一种孪生调试方法，指基于NPU仿真软件或NPU硬件调试。 |
| Tiling下沉 | Tiling下沉是指将Tiling计算下沉至Device侧的AI CPU上执行，从而实现计算全程在Device侧高效完成。 |
| 分离模式 | AI Core的一种工作模式，其矩阵计算单元和矢量计算单元分别由独立的Scalar调度单元进行调度，并分离部署在Cube Core和Vector Core上。将Cube Core和Vector Core按照一定比例（1：N）进行组合，这样的组合视为一个AI Core，AI Core的核数以Cube Core为准。 |
| 孪生调试 | Ascend C提供的算子调试方法，支持在CPU域调试精度和NPU域调试精度/性能。 |
| 流水任务 | Ascend C编程范式是一种流水线式的编程范式，把算子核内的处理程序，分成多个流水任务。流水任务是指单核处理程序中主程序调度的并行任务。在核函数（Kernel）内部，可以通过流水任务实现数据的并行处理，进一步提升性能。 |
| 连续模式 | 使用Mask控制矢量计算每次Repeat内参与计算的元素时，可选择的模式之一，表示前面连续的多少个元素参与计算。 |
| 耦合模式 | AI Core的一种工作模式，采用同一个Scalar调度单元同时调度矩阵计算单元、矢量计算单元，所有的单元部署在一个AI Core上。 |
| 融合算子 | 融合算子由多个独立的小算子融合而成，其功能与多个小算子的功能等价，性能方面通常优于独立的小算子。用户可以根据实际业务场景诉求，按照具体算法自由融合矢量（Vector）、矩阵（Cube）算子以达到性能上的收益。 |
| 算子入图 | 算子入图指通过GE图模式运行算子，在图模式下首先将所有算子构造成一张图，然后通过GE将图下发到AI处理器执行。 |
| 算子原型 | 算子原型是算子的抽象描述，定义了算子的输入、输出、属性等信息。 |
| 通算融合算子 | 通算融合算子是融合集合通信任务和计算任务的算子，在算子执行过程中，计算和通信任务可以实现部分流水并行，从而提升性能。 |
| Reg矢量计算 | Reg矢量计算API是面向RegBase架构开发的API，用户可以通过该类API直接对芯片中涉及Vector计算的寄存器进行操作，实现更大的灵活性和更好的性能。 |
| 逐比特模式 | 使用Mask控制矢量计算每次Repeat内参与计算的元素时，可选择的模式之一，可以按位控制哪些元素参与计算，bit位的值为1表示参与计算，0表示不参与。 |
| 自定义算子工程 | Ascend C提供的、基于CMake组织编译的算子工程，包含算子原型定义、Tiling实现、核函数（Kernel）实现和CMake编译配置。 |
