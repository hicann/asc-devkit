# 通用说明和约束<a id="simd_api_general_description_constraints"></a>

本文面向SIMD API开发者，介绍[C API](c_api/c_api.md)、[Tensor API](tensor_api/tensor_api.md)、[基础API](basic_api/basic_api.md)和[高阶API](adv_api/adv_api.md)四类API在使用过程中需要关注的通用说明和约束。

**表1** 各类API的基本编程方式和参考资料

| API类型 | 编程方式 | 参考文档 |
| --- | --- | --- |
| C API | 基于C语言指针编程，由开发者管理本地内存和同步。 | [C语言编程概述](../../guide/programming_guide/programming_model/ai_core_simd_programming/c_pointer_programming/c_programming_overview.md) |
| Tensor API | 基于携带Layout信息的扩展Tensor编程。 | [C++ Tensor编程概述](../../guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/cpp_tensor_programming_overview.md) |
| 基础API | 基于C++基础Tensor编程，可由开发者管理本地内存和同步，也可结合TPipe和TQue管理资源与流水同步。 | [C++ Tensor编程概述](../../guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/cpp_tensor_programming_overview.md)<br>[TPipe和TQue编程范式](../../guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_paradigm.md) |
| 高阶API | 基于C++ Tensor编程，通常结合TPipe和TQue编程范式使用。 | [TPipe和TQue编程范式](../../guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_paradigm.md) |

## 通用约束

### 通用地址对齐约束<a id="alignment_requirements"></a>

AI Core上的存储单元用于存储矢量计算、矩阵计算的源操作数和目的操作数，各类存储单元的对齐要求如[表2](#alignment_requirements_table)所示，因此Ascend C API操作数的起始地址对齐要求应与这些存储单元的对齐要求保持一致。**需要注意的是，如果接口中已明确说明操作数起始地址对齐要求，则以具体API中的说明为准。**

**表2** 不同存储单元的常见对齐要求

<a id="alignment_requirements_table"></a>

| 存储位置 | 常见对齐要求 |
| --- | --- |
| Global Memory | 无对齐要求 |
| UB | 32Byte对齐 |
| L1 Buffer | 32Byte对齐 |
| L0A Buffer/L0B Buffer | 512Byte对齐 |
| L0C Buffer | 64Byte对齐 |
| BiasTable Buffer | 64Byte对齐 |
| Fixpipe Buffer | 64Byte对齐 |

> [!NOTE]说明
> 上表所述地址对齐要求适用于一般情况，具体接口可能有不同要求。下面列举部分特殊情况（涉及的接口以C API为例）：
> - Global Memory：[asc_ndim_copy_gm2ub](c_api/vector_datamove/asc_ndim_copy_gm2ub.md)的数据类型为b16、b32时，GM源地址分别要求2Byte、4Byte对齐。
> - UB：[asc_ndim_copy_gm2ub](c_api/vector_datamove/asc_ndim_copy_gm2ub.md)的UB目的地址仅要求1Byte对齐。
> - L0C Buffer：NPU架构版本3510的[asc_mmad](c_api/cube_compute/mmad_compute/asc_mmad.md)要求L0C结果地址1024Byte对齐。
> - Fixpipe Buffer：NPU架构版本3510的[asc_copy_l12fb](c_api/cube_datamove/cube_compute_load/asc_copy_l12fb_arch_3510.md)要求随路Vector量化参数地址128Byte对齐。

### 通用地址重叠约束<a id="address_overlap_constraints"></a>

使用高维切分计算接口时，为了节省地址空间，可以使源操作数和目的操作数使用同一段存储空间。使用地址重叠时，需要满足以下约束：

- 单次迭代内，源操作数与目的操作数必须完全重叠，不支持部分重叠。
- 多次迭代间，不支持前序迭代的目的操作数与后序迭代的源操作数重叠。例如，第N次迭代的目的操作数是第N+1次迭代的源操作数时，第N次迭代可能改写后续迭代仍需读取的源数据，导致计算结果不符合预期。
- 对于部分双目计算类接口，当数据类型为half、int32_t或float时，可在满足接口特定条件的情况下支持前序迭代的目的操作数与后序迭代的源操作数重叠。该重叠仅限于目的操作数与第二个源操作数重叠，且`src1RepStride`或`dstRepStride`必须为0；具体支持的接口以对应API文档为准。
- 多次迭代间，不支持前序迭代的目的操作数与后序迭代的目的操作数重叠。前序迭代写入的数据可能被后续迭代覆盖，导致计算结果不符合预期。

**图1** 地址重叠示例（不支持）<a id="address_overlap_example"></a>    
![](../figures/address_overlap_unsupported.png "地址重叠示例（不支持）")

> [!NOTE]说明
> 本节所述地址重叠通用约束适用于C API和基础API的一般情况，API参考中如有额外特殊说明的，则以具体API中的说明为准。
> API中没有描述地址重叠约束的，视为不支持Tensor高维切分计算的地址重叠，地址重叠时计算结果可能不满足预期。

### 原子操作

原子操作用于多个执行单元或多个核并发访问同一Global Memory位置的场景。普通写入可能发生读改写覆盖：多个执行单元先后读取相同旧值，再分别写回结果，后写入的结果会覆盖先写入的结果。原子操作把指定位置的读-修改-写过程作为一个不可分割的操作，避免并发更新相互覆盖。

原子操作包括以下两种类型：

- 数据搬运随路原子操作：作用于后续写入Global Memory的数据搬运。
- Scalar原子操作：直接对指定Global Memory地址执行加、减、最大值、最小值、交换或比较交换。

四类API在涉及并发更新Global Memory数据时，可使用的原子操作接口及其描述如下表3所示。

**表3** 原子操作接口和使用方式

| API类别 | 原子操作接口和使用方式 |
| --- | --- |
| C API | <br>&bull; 数据搬运随路原子操作：<br>[asc_set_atomic_add](c_api/atomic/datamove_atomic/asc_set_atomic_add.md)、[asc_set_atomic_max](c_api/atomic/datamove_atomic/asc_set_atomic_max.md)、[asc_set_atomic_min](c_api/atomic/datamove_atomic/asc_set_atomic_min.md)、[asc_disable_dma_atomic](c_api/atomic/datamove_atomic/asc_disable_dma_atomic.md)<br>&bull; Scalar原子操作：<br>[asc_atomic_add](c_api/atomic/scalar_atomic/asc_atomic_add.md)、[asc_atomic_min](c_api/atomic/scalar_atomic/asc_atomic_min.md)、[asc_atomic_max](c_api/atomic/scalar_atomic/asc_atomic_max.md)、[asc_atomic_exch](c_api/atomic/scalar_atomic/asc_atomic_exch.md)、[asc_atomic_cas](c_api/atomic/scalar_atomic/asc_atomic_cas.md) |
| Tensor API | Tensor API的同步控制、原子操作等底层控制能力由C API提供。需要原子更新时，使用对应的C API原子接口，并同时满足Tensor API的Layout、访问范围和同步要求。 |
| 基础API | <br>&bull; 数据搬运随路原子操作：<br>[SetAtomicAdd](basic_api/atomic_operations/SetAtomicAdd.md)、[SetAtomicMax](basic_api/atomic_operations/SetAtomicMax_ISASI.md)、[SetAtomicMin](basic_api/atomic_operations/SetAtomicMin_ISASI.md)、[DisableDmaAtomic](basic_api/atomic_operations/DisableDmaAtomic.md)<br>&bull; Scalar原子操作：<br>[AtomicAdd](basic_api/atomic_operations/AtomicAdd.md)、[AtomicMin](basic_api/atomic_operations/AtomicMin.md)、[AtomicMax](basic_api/atomic_operations/AtomicMax.md)、[AtomicExch](basic_api/atomic_operations/AtomicExch.md)、[AtomicCas](basic_api/atomic_operations/AtomicCas.md) |
| 高阶API | Matmul接口可通过[GetTensorC](adv_api/cube_compute/Matmul_Kernel/GetTensorC.md)的`enAtomic`参数开启AtomicAdd、AtomicMax或AtomicMin操作。其他高阶API是否支持原子操作以具体接口说明为准；接口未提供相关能力时，可使用基础API的原子操作接口。 |

使用原子累加时，初始值写入和后续原子更新必须建立明确的执行顺序，避免普通写入覆盖尚未完成的原子更新。原子操作保证单次更新的不可分割性，不保证多个原子更新按源码顺序完成；浮点累加还可能因执行顺序不同而产生数值差异。需要确定执行顺序时，仍需使用适用的核内或核间同步接口。

确定性计算的具体要求请参考基础API的[原子操作特性说明](basic_api/atomic_operations/key_features.md)和C API的[原子操作特性说明](c_api/atomic/key_features.md)。

### 数据依赖与同步

AI Core中的数据搬运、矢量计算和矩阵计算由不同的硬件流水线执行，同一核内的各流水线以及不同核之间的指令都可能异步执行。例如，从GM搬入UB的数据通常由MTE2流水线写入，后续矢量计算由Vector流水线读取；不同核之间也可能通过Global Memory或片上存储交换数据。即使相关接口在源码中按先后顺序调用，后续操作也可能在前序数据写入完成前开始执行。因此，存在两类同步：

- 同一AI Core内不同流水线之间存在数据依赖时，需要通过核内同步建立执行先后关系；
- 不同核之间存在数据依赖时，需要通过核间同步建立执行先后关系。

四类API在核内流水依赖场景下可使用的同步接口如下：

**表4** 核内同步接口

| API类别 | 不同流水线之间的同步 | 同一流水线内的同步 |
| --- | --- | --- |
| C API | 流水事件同步：<br>&bull; [asc_sync_notify](c_api/sync/intra_core_sync/asc_sync_notify.md)/[asc_sync_wait](c_api/sync/intra_core_sync/asc_sync_wait.md)<br>&bull; Ascend 950PR&950DT系列产品还可使用[asc_lock](c_api/sync/intra_core_sync/asc_lock.md)/[asc_unlock](c_api/sync/intra_core_sync/asc_unlock.md) | 使用[asc_sync_pipe](c_api/sync/intra_core_sync/asc_sync_pipe.md)等待指定流水线的前序指令完成。 |
| Tensor API | 配合C API处理流水间依赖：<br>&bull; [asc_sync_notify](c_api/sync/intra_core_sync/asc_sync_notify.md)/[asc_sync_wait](c_api/sync/intra_core_sync/asc_sync_wait.md)<br>&bull; [asc_lock](c_api/sync/intra_core_sync/asc_lock.md)/[asc_unlock](c_api/sync/intra_core_sync/asc_unlock.md) | 配合C API的[asc_sync_pipe](c_api/sync/intra_core_sync/asc_sync_pipe.md)处理单流水依赖。 |
| 基础API | 流水事件同步：<br>&bull; [SetFlag/WaitFlag](basic_api/sync_control/intra_core_sync/SetFlag_WaitFlag_ISASI.md)<br>&bull; [TQueSync](basic_api/sync_control/intra_core_sync/TQueSync.md)类的同名成员函数<br>&bull; Ascend 950PR&950DT系列产品还可使用[Mutex](basic_api/sync_control/intra_core_sync/Mutex_ISASI.md)的[Lock](basic_api/sync_control/intra_core_sync/Lock.md)/[Unlock](basic_api/sync_control/intra_core_sync/Unlock.md) | 使用[PipeBarrier](basic_api/sync_control/intra_core_sync/PipeBarrier_ISASI.md)等待指定流水线的前序指令完成。 |
| 高阶API | 高阶API的部分接口会提供明确的同步模式或配套等待接口：<br>&bull; Matmul接口通过`sync`参数区分同步、异步模式，异步模式需要调用配套的等待接口。<br>&bull; [Fill](adv_api/tensor_transform/Fill.md)文档明确要求调用者在特定流水依赖场景下插入同步。 | 使用[PipeBarrier](basic_api/sync_control/intra_core_sync/PipeBarrier_ISASI.md)等待指定流水线的前序指令完成。 |

不同AI Core之间存在执行顺序要求时，需要使用核间同步接口。四类API的核间同步接口如下：

**表5** 核间同步接口

| API类别 | 核间同步接口 |
| --- | --- |
| C API | 使用四类核间同步接口：<br>&bull; [asc_sync_block_arrive](c_api/sync/inter_core_sync/asc_sync_block_arrive.md)/[asc_sync_block_wait](c_api/sync/inter_core_sync/asc_sync_block_wait.md)<br>&bull; [asc_sync_inter_arrive](c_api/sync/inter_core_sync/asc_sync_inter_arrive.md)/[asc_sync_inter_wait](c_api/sync/inter_core_sync/asc_sync_inter_wait.md)<br>&bull; [asc_sync_intra_arrive](c_api/sync/inter_core_sync/asc_sync_intra_arrive.md)/[asc_sync_intra_wait](c_api/sync/inter_core_sync/asc_sync_intra_wait.md)<br>&bull; [asc_sync_subblock_arrive](c_api/sync/inter_core_sync/asc_sync_subblock_arrive.md)/[asc_sync_subblock_wait](c_api/sync/inter_core_sync/asc_sync_subblock_wait.md) |
| Tensor API | Tensor API的核间同步能力由C API提供。 |
| 基础API | 硬件同步：<br>&bull; [CrossCoreSetFlag](basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md)/[CrossCoreWaitFlag](basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md)<br>&bull; [SyncAll](basic_api/sync_control/inter_core_sync/SyncAll.md)（硬件同步场景）<br>软件同步：<br>&bull; [IBSet](basic_api/sync_control/inter_core_sync/IBSet.md)/[IBWait](basic_api/sync_control/inter_core_sync/IBWait.md)<br>&bull; [SyncAll](basic_api/sync_control/inter_core_sync/SyncAll.md)（软件同步场景）<br>&bull; [InitDetermineComputeWorkspace](basic_api/sync_control/inter_core_sync/InitDetermineComputeWorkspace.md)、[NotifyNextBlock](basic_api/sync_control/inter_core_sync/NotifyNextBlock.md)、[WaitPreBlock](basic_api/sync_control/inter_core_sync/WaitPreBlock.md)（三个接口需组合使用） |
| 高阶API | 高阶API的核间同步由具体接口场景决定：<br>&bull; Matmul内部可能使用[CrossCoreSetFlag](basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md)/[CrossCoreWaitFlag](basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md)，使用Matmul对象时需避免flagId冲突。<br>&bull; HCCL接口的跨核依赖按其接口说明使用[SyncAll](basic_api/sync_control/inter_core_sync/SyncAll.md)或其他核间同步接口。 |

## 通用说明

### 所需头文件和库文件<a id="header_and_library_requirements"></a>

按所用API类别包含公共头文件。若具体API另有头文件或链接要求，应遵循该接口的说明。头文件中未在API资料中公开的接口，不作为对外接口使用。

**表6** 各类API的头文件和链接说明

| API类别 | 常用入口头文件 | 说明 |
| --- | --- | --- |
| C API | `${INSTALL_DIR}/asc/include/c_api/asc_simd.h` | 除试验接口外，可通过该入口包含接口。最细粒度公共头文件见[C API列表](c_api/c_api.md)。 |
| Tensor API | `${INSTALL_DIR}/asc/include/tensor_api/tensor.h` | 统一入口及与C API的配合方式见[Tensor API概述](tensor_api/tensor_api.md)。 |
| 基础API | `${INSTALL_DIR}/asc/include/kernel_operator.h` | 接口头文件位于`${INSTALL_DIR}/asc/include/basic_api`目录。 |
| 高阶API | `${INSTALL_DIR}/asc/include/kernel_operator.h` | 接口头文件位于`${INSTALL_DIR}/asc/include/adv_api`和`${INSTALL_DIR}/asc/include/tiling`目录。<br>使用配套Host Tiling接口时，需要链接`libtiling_api.a`。 |

> [!NOTE]说明
> `${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。以root用户安装为例，安装后文件默认存储路径为：`/usr/local/Ascend/cann`。

### 数据存储位置的描述方式

本小节按C API、Tensor API、基础API和高阶API的顺序说明各类API对数据存储位置的描述方式：

- C API直接通过`__gm__`、`__ubuf__`、`__cbuf__`、`__ca__`、`__cb__`和`__cc__`等地址空间限定符表达位置；
- Tensor API由Tensor的`location`、Layout和搬运原子对象共同确定访问粒度；
- 基础API通常通过`LocalTensor`、`GlobalTensor`或Layout描述位置和范围，调用者仍须保证底层地址满足硬件要求；
- 高阶API通常基于基础API的Tensor和Layout描述位置和范围，具体以接口说明为准。

#### C API：地址空间限定符

C API通常直接使用带地址空间限定符的指针表达操作数所在的存储位置。

**表7** C API常用地址空间限定符

| 地址空间限定符 | 物理存储位置 | 说明 |
| --- | --- | --- |
| `__gm__` | Global Memory | 设备端的主内存，AI Core的外部存储，用于存储大规模数据，但需要优化访问模式以提升性能。 |
| `__ubuf__` | Unified Buffer（UB） | AI Core内部存储单元，主要用于矢量计算。 |
| `__cbuf__` | L1 Buffer | AI Core内部物理存储单元，空间相对较大，通常用于缓存矩阵计算的输入数据。矩阵计算的输入一般需要从GM搬运到L1 Buffer，然后分别搬运到L0A Buffer和L0B Buffer。 |
| `__ca__` | L0A Buffer | AI Core内部物理存储单元，通常用于存储矩阵计算的左矩阵。 |
| `__cb__` | L0B Buffer | AI Core内部物理存储单元，通常用于存储矩阵计算的右矩阵。 |
| `__cc__` | L0C Buffer | AI Core内部物理存储单元，通常用于存储矩阵计算的结果。 |
| `__fbuf__` | Fixpipe Buffer | AI Core内部物理存储单元，通常用于存储Fixpipe搬运过程中所需的量化参数等数据。 |

#### Tensor API：location

如表8所示，Tensor API通过`location`枚举标记Tensor或指针的物理存储位置。

**表8** Tensor API物理存储位置

| location | 物理存储位置 | 说明 |
| --- | --- | --- |
| `location::gm` | Global Memory | 设备端的主内存，AI Core的外部存储，用于存储大规模数据，但需要优化访问模式以提升性能。 |
| `location::ub` | Unified Buffer（UB） | AI Core内部存储单元，主要用于矢量计算。 |
| `location::l1` | L1 Buffer | AI Core内部物理存储单元，空间相对较大，通常用于缓存矩阵计算的输入数据。矩阵计算的输入一般需要从GM搬运到L1 Buffer，然后分别搬运到L0A Buffer和L0B Buffer。 |
| `location::l0a` | L0A Buffer | AI Core内部物理存储单元，通常用于存储矩阵计算的左矩阵。 |
| `location::l0b` | L0B Buffer | AI Core内部物理存储单元，通常用于存储矩阵计算的右矩阵。 |
| `location::l0c` | L0C Buffer | AI Core内部物理存储单元，通常用于存储矩阵计算的结果。 |
| `location::bias` | BiasTable Buffer | 偏置存储，AI Core内部物理存储单元，通常用于存储矩阵计算所需的Bias（偏置）数据。 |
| `location::fixbuf` | Fixpipe Buffer | AI Core内部物理存储单元，通常用于存储Fixpipe搬运过程中所需的量化参数等数据。 |

#### 基础API和高阶API：TPosition

`TPosition`是基础API和部分高阶API使用的逻辑存储位置类型。常见取值及其物理映射见表9，同一逻辑位置在不同产品上的物理映射可能不同。

**表9** TPosition与物理内存映射关系

<a id="tposition_memory_mapping"></a>

| TPosition | 物理存储位置 |
| --- | --- |
| GM | Global Memory |
| VECIN | UB |
| VECCALC | UB |
| VECOUT | UB |
| A1 | L1 Buffer |
| A2 | L0A Buffer |
| B1 | L1 Buffer |
| B2 | L0B Buffer |
| C1 | <!-- npu="950" id1 -->Ascend 950PR&950DT系列产品，L1 Buffer。<br><!-- end id1 --><!-- npu="910" id2 -->Atlas训练系列产品，Unified Buffer（UB）。<br><!-- end id2 --><!-- npu="310p" id3 -->Atlas推理系列产品AI Core，UB。<br><!-- end id3 --><!-- npu="910b" id4 -->Atlas A2系列产品，L1 Buffer。<br><!-- end id4 --><!-- npu="A3" id5 -->Atlas A3系列产品，L1 Buffer。<br><!-- end id5 --><!-- npu="310b" id6 -->Atlas 200I/500 A2推理产品，UB。<!-- end id6 --> |
| C2 | <!-- npu="950" id7 -->Ascend 950PR&950DT系列产品，BiasTable Buffer。<br><!-- end id7 --><!-- npu="910" id8 -->Atlas训练系列产品，L0C Buffer。<br><!-- end id8 --><!-- npu="310p" id9 -->Atlas推理系列产品AI Core，L0C Buffer。<br><!-- end id9 --><!-- npu="910b" id10 -->Atlas A2系列产品，BiasTable Buffer。<br><!-- end id10 --><!-- npu="A3" id11 -->Atlas A3系列产品，BiasTable Buffer。<br><!-- end id11 --><!-- npu="310b" id12 -->Atlas 200I/500 A2推理产品，BiasTable Buffer。<!-- end id12 --> |
| CO1 | L0C Buffer |
| CO2 | <!-- npu="950" id13 -->Ascend 950PR&950DT系列产品，Global Memory。<br><!-- end id13 --><!-- npu="910" id14 -->Atlas训练系列产品，UB。<br><!-- end id14 --><!-- npu="310p" id15 -->Atlas推理系列产品AI Core，UB。<br><!-- end id15 --><!-- npu="910b" id16 -->Atlas A2系列产品，Global Memory。<br><!-- end id16 --><!-- npu="A3" id17 -->Atlas A3系列产品，Global Memory。<br><!-- end id17 --><!-- npu="310b" id18 -->Atlas 200I/500 A2推理产品，Global Memory。<!-- end id18 --> |
| TSCM | L1 Buffer |
| SPM | <!-- npu="950" id19 -->Ascend 950PR&950DT系列产品，Global Memory。<br><!-- end id19 --><!-- npu="910" id20 -->Atlas训练系列产品，L1 Buffer。<br><!-- end id20 --><!-- npu="310p" id21 -->Atlas推理系列产品AI Core，L1 Buffer。<br><!-- end id21 --><!-- npu="910b" id22 -->Atlas A2系列产品，Global Memory。<br><!-- end id22 --><!-- npu="A3" id23 -->Atlas A3系列产品，Global Memory。<!-- end id23 --> |
| C2PIPE2GM | <!-- npu="950" id24 -->Ascend 950PR&950DT系列产品，Fixpipe Buffer。<br><!-- end id24 --><!-- npu="910b" id25 -->Atlas A2系列产品，Fixpipe Buffer。<br><!-- end id25 --><!-- npu="A3" id26 -->Atlas A3系列产品，Fixpipe Buffer。<!-- end id26 --> |
