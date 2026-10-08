# 使用SSBuffer实现核间通信

本文介绍SSBuffer的存储范围、地址空间、访问方式以及配合核间同步进行消息交换的编程方法。消息队列的具体设计、Host侧校验和完整算子实现请参见[基于SSBuffer的核间通信特性](../../../operator_practice/simd_operator_impl/fusion_operator_programming/cv_fusion/ssbuffer_feature_scenarios.md)。

>[!NOTE]说明
>阅读前，请先了解[地址空间限定符](../../language_extension/simd_builtin_keywords.md#section1624210295308)和[内存一致性](../memory_model/memory_consistency.md)中的读写顺序与同步概念。
>本节内容的产品支持情况如下：
><!-- npu="950" id1 -->
>- Ascend 950PR&950DT系列产品：支持
><!-- end id1 -->
><!-- npu="A3" id2 -->
>- Atlas A3系列产品：不支持
><!-- end id2 -->
><!-- npu="910b" id3 -->
>- Atlas A2系列产品：不支持
><!-- end id3 -->
><!-- npu="310b" id4 -->
>- Atlas 200I/500 A2推理产品：不支持
><!-- end id4 -->
><!-- npu="310p" id5 -->
>- Atlas推理系列产品AI Core：不支持
><!-- end id5 -->
><!-- npu="310p" id6 -->
>- Atlas推理系列产品Vector Core：不支持
><!-- end id6 -->
><!-- npu="910" id7 -->
>- Atlas训练系列产品：不支持
><!-- end id7 -->

## 特性概述

SSBuffer是AI Core内部的物理存储单元，具有独立的地址空间。在分离模式下，使用`__mix__(1, 2)`配置核函数时（MIX任务），同一AI Core上的AIC与两个AIV共享SSBuffer，可以通过Scalar单元读写SSBuffer，交换控制消息、地址、长度和少量标量数据。硬件架构及与其他存储单元的关系请参见[NPU架构版本3510](../hardware_implementation/architecture_spec/npu_arch_3510.md)。

Cube（矩阵计算）和Vector（矢量计算）融合算子（以下简称CV融合算子）中的通信包含消息交换和业务数据搬运。例如，AIV将向量计算结果作为Cube计算的输入时，需要告诉AIC输入位置、矩阵形状等消息，还需要将输入数据搬运至L1 Buffer。SSBuffer可以保存这些消息，矩阵数据本身则通过UB到L1 Buffer等数据通路搬运。SSBuffer容量有限，不用于保存大块中间计算结果，也不能替代Global Memory（GM）、Unified Buffer（UB）或L1 Buffer。

SSBuffer读写与核间同步是两个相关但不同的操作：写入消息用于传递内容，核间同步用于保证另一侧在消息或数据准备完成后再读取。SSBuffer不会自动生成消息队列、有效标记或完成通知，开发者需要通过接口或通信规则建立这些关系。

## 适用场景与使用方式

### 使用场景

SSBuffer适用于同一AI Core内的小数据量通信，常见场景如下。

**表1**  SSBuffer通信场景

| 场景 | SSBuffer中传递的内容 | 需要配合的操作 |
| --- | --- | --- |
| AIV向AIC提交计算任务 | 计算方式、输入地址、矩阵形状、任务序号等。 | AIV准备输入，AIC通过核间同步[模式2](../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/inter_core_sync_overview.md)或[模式4](../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/inter_core_sync_overview.md)等待相应通知后读取消息并执行计算。 |
| AIC向AIV反馈消息 | 目标L1 Buffer地址、下一轮任务参数、处理结果等。 | AIC通过核间同步模式4通知单个AIV，或通过模式2通知两个AIV；AIV等待通知后再使用地址或参数。 |
| AIC与单个AIV交换消息 | 一侧产生、另一侧消费的控制消息。 | 使用核间同步模式4分别协调AIC与AIV0、AIC与AIV1。 |
| AIC与两个AIV协同处理 | 两个AIV分别产生的参数，或AIC提供给两个AIV的参数。 | 根据依赖关系选择核间同步模式2，使AIC与两个AIV同步；或选择模式4，使AIC分别与各AIV同步。 |
| 同一AI Core内两个AIV交换消息 | 放在约定区域的少量标量数据。 | 在共享SSBuffer的MIX任务中，使用核间同步[模式1](../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/inter_core_sync_overview.md)保证读写顺序。 |
| 连续任务通信 | 按顺序产生的多条消息。 | 为各AIV分别建立消息队列，通过核间同步模式4通知消息就绪和处理完成，配合队列回绕重复收发。 |

非MIX模式下每个核独占的SSBuffer空间不能按上述共享方式使用。不同AI Core之间、不同任务之间以及Host与Device之间的数据交换，也不应使用本节介绍的SSBuffer共享布局。

### 使用优势

相较于通过GM交换小消息，SSBuffer通信不需要将消息先写入GM再由另一侧读取，可以减少GM访问和相关带宽占用，降低小数据量核间通信的开销。对于需要反复使用的消息参数，可在同步完成后读入本核局部变量，减少重复访问SSBuffer。与UB到L1 Buffer、L0C Buffer到UB等数据通路配合，可以减少融合算子的中间数据经过GM的次数。

### 使用方式选择

本节以开发者显式访问SSBuffer的通信方式为主。选择使用方式时，应先确定核间通信由开发者管理还是由Matmul框架管理。

**表2**  通信方式及接口边界

| 使用方式 | 适用场景 | 开发者需要处理的内容 |
| --- | --- | --- |
| 显式SSBuffer通信 | AIC/AIV需要交换自定义控制消息或少量标量数据。 | 获取SSBuffer基地址，自行规划消息布局、初始化、核间同步、空间复用及退出条件。 |
| 基于Matmul高阶API的CV融合 | AIV发起Matmul计算，并与Vector计算组合。 | 使用[REGIST_MATMUL_OBJ](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/REGIST_MATMUL_OBJ.md)等公开接口，由框架管理相应的KFC调用及消息。SSBuffer路径下不要直接修改内部消息结构。 |

## 存储规格与访问约束

### 存储范围和访问约束

使用前应确认以下规格和约束：

**表3**  SSBuffer规格和约束

| 项目 | 说明 |
| --- | --- |
| 支持产品 | Ascend 950PR&950DT系列产品，对应编译目标`dav-3510`。 |
| 总容量 | 3KB。 |
| 非MIX模式 | AIC、AIV0、AIV1各自独立占用1KB，不构成供三者共同访问的3KB工作区。 |
| `__mix__(1, 2)`模式 | AIC:AIV为1:2时，AIC、AIV0、AIV1共享整个3KB空间。 |
| 地址空间 | 使用`__ssbuf__`限定符。 |
| 访问方式 | 通过Scalar单元读写。 |
| 对齐要求 | SSBuffer只支持通过读写指令32字节的对齐访问。通信区域起始偏移、消息布局和访问范围需要满足该要求。 |
| 初始内容 | 可能存在脏数据，不保证为0。读取消息之前必须完成本次消息写入或必要的状态初始化。 |
| 任务约束 | AIC和AIV启动不同任务时，不能访问同一SSBuffer进行通信；共享通信应采用`__mix__(1, 2)`核函数配置，工程化算子的对应核函数类型为`KERNEL_TYPE_MIX_AIC_1_2`。 |
| 越界访问 | 访问超过可用空间的最末端地址会产生异常。除消息内容外，还需计入结构体填充、对齐、队列和反馈信息的空间。 |
| 并发写入 | 不同生产者应使用互不重叠的消息区域。同一消息空间在交给消费者后，生产者不能在消费完成前覆盖。 |

### 核函数与编译配置

使用[函数执行空间限定符](../../language_extension/simd_builtin_keywords.md#section1074418132518)明确核函数的执行方式。`__mix__(1, 2)`表示每组AIC/AIV按1:2配比运行，多组执行时，各组访问其各自的SSBuffer。工程化算子应配置与`__mix__(1, 2)`对应的[核函数类型](../../../../api/SIMD-API/basic_api/Kernel-Tiling/set_Kernel_type.md)，即`KERNEL_TYPE_MIX_AIC_1_2`。

在上述核函数配置下，还需根据通信的实现方式确定是否设置编译宏[ENABLE_CV_COMM_VIA_SSBUF](../../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#section57020345148)：

- 显式SSBuffer通信：开发者通过SSBuffer地址指针读写消息，访问本身不依赖该编译宏。消息布局、初始化、核间同步和空间复用由开发者管理。
- Matmul框架管理通信：若要将AIV向AIC发送调用消息的存储空间由GM切换为SSBuffer，需要将该编译宏设置为`true`（默认为`false`），消息空间由Matmul框架管理。

启用Matmul的SSBuffer通信时，编译配置示意如下：

```bash
bisheng <source_file>.asc -o <output_file> --npu-arch=dav-3510 -DENABLE_CV_COMM_VIA_SSBUF=true
```

需要注意，`ENABLE_CV_COMM_VIA_SSBUF`不负责设置核函数类型，也不会为自定义消息划分空间、清理状态或插入收发同步。

版本差异及数据通路的选择请参见[3510新特性使用说明](../../../cross_gen_migration_guide/instructions_for_new_features/3510_new_features.md#section_ssbuf)。

## 地址空间与消息布局

### 获取SSBuffer基地址

在基础API中，[GetSsbufBaseAddr](../../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetSsbufBaseAddr.md)用于获取SSBuffer基地址，头文件为`"basic_api/kernel_operator_sys_var_intf.h"`，也可通过`"kernel_operator.h"`引入。基地址获取方式如下：

```cpp
// __ssbuf__表示指针指向SSBuffer地址空间，后续偏移计算保留该限定符。
__ssbuf__ void* base = AscendC::GetSsbufBaseAddr();
__ssbuf__ uint8_t* bytes = reinterpret_cast<__ssbuf__ uint8_t*>(base);
```

在NPU域中，该接口返回`__ssbuf__`地址空间的零地址。这里的零地址是SSBuffer的特殊基地址，不能按普通GM空指针判断是否获取失败。CPU调试域中，接口返回CPU模拟分配的地址，因此不要在需要CPU调试的基础API代码中将基地址固定为0。

C API当前没有与GetSsbufBaseAddr对应的基地址获取接口。在`dav-3510` NPU环境下，可以按以下示例的方式，将零地址转换为带`__ssbuf__`限定的结构体指针。

```cpp
// SsbufWorkspace的定义见下文；这里只取得地址，不进行动态内存分配。
__ssbuf__ SsbufWorkspace* workspace = reinterpret_cast<__ssbuf__ SsbufWorkspace*>(0);
```

### 定义消息布局

SSBuffer没有固定的消息格式，也不提供通用的动态内存分配接口。开发者通过普通结构体定义字段和布局，再使用带`__ssbuf__`限定的指针定位已有存储空间。定义结构体本身不会另外分配SSBuffer。

以下以32B消息和两个独立通信区域为例，每个区域存放4个64位值：

```cpp
struct alignas(32) SsbufferMessage {
    uint64_t value[4];
};
struct SsbufWorkspace {
    SsbufferMessage message[2];
};
static_assert(sizeof(SsbufferMessage) == 32, "Message must be 32 bytes");
static_assert(sizeof(SsbufWorkspace) <= 3 * 1024, "Workspace exceeds SSBuffer capacity");
```

该布局中，两个消息区域的起始偏移分别为0B和32B。通信双方应使用一致的字段类型、偏移和长度，并计入结构体对齐产生的填充。区域起始偏移加上占用大小不得超过当前模式的可用空间。

```text
MIX 1:2下的SSBuffer（本例）
字节偏移： 0                 32                 64                 3072
          | 消息区域0，32B   | 消息区域1，32B    | 本例未使用的空间    |
```

消息区域可按通信对象或通信方向划分。不同生产者分别写入互不重叠的区域，同一消息由消费者读完后才能再次覆盖。消息里保存的GM、UB或L1 Buffer地址只表示对应位置的数据，不会改变这些数据所属的地址空间或访问范围。

SSBuffer读写指令的32B对齐约束不能简单由C/C++成员类型推导。结构体中的32位、64位字段用于表达内容，`alignas`用于约束布局；二者都不能保证整条消息的原子性或代替核间同步。

### 结构体指针与数组

在本节样例使用的`dav-3510`编译方式下，不支持直接声明带`__ssbuf__`限定的数组对象，也不支持让该限定符修饰指向数组类型的指针。例如：

```cpp
__ssbuf__ uint8_t ssbuffer[3 * 1024]; // 不支持：不能用该声明分配SSBuffer数组。
using SsbufByteArray = uint8_t[3 * 1024];
__ssbuf__ SsbufByteArray* arrayPtr;   // 不支持：指向数组类型的指针带该限定符。
```

可以采用上文结构体成员数组的方式描述布局。也可以在默认Local Memory中保存SSBuffer指针数组，各指针分别指向约定的消息区域；这时指针数组本身不位于SSBuffer，区域偏移仍由开发者管理。优先让双方共用同一份结构体定义，避免分别计算布局造成不一致。

CPU调试时应使用GetSsbufBaseAddr返回的模拟地址。设备侧将0转换为`__ssbuf__`指针的写法不能用于主机内存访问。静态Tensor中的`location::ssbuf`等类型标识用于描述存储位置，不表示任意Tensor计算或搬运接口都支持SSBuffer，也不提供额外的SSBuffer存储空间。

## 核间同步与通信顺序

### 同步方式选择

### 基础API同步方式

基础API使用[CrossCoreSetFlag](../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md)和[CrossCoreWaitFlag](../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md)配对进行核间同步，通过`modeId`模板参数选择模式，通过`pipe`模板参数选择流水。

### C API同步方式

C API通过不同的接口选择同步模式，通过`pipe`参数选择流水：

| 模式 | 发送通知 | 等待通知 |
| --- | --- | --- |
| 模式1 | [asc_sync_subblock_arrive](../../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_subblock_arrive.md) | [asc_sync_subblock_wait](../../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_subblock_wait.md) |
| 模式2 | [asc_sync_block_arrive](../../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_block_arrive.md) | [asc_sync_block_wait](../../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_block_wait.md) |
| 模式4 | [asc_sync_intra_arrive](../../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_intra_arrive.md) | [asc_sync_intra_wait](../../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_intra_wait.md) |

### 发布、读取与空间复用

一条消息的完整交接包括两个方向：生产者写入后通知消费者，消费者读取后通知生产者可以复用空间。确认（ACK）是通信协议对返回通知的命名，不是SSBuffer自动提供的消息。

```text
生产者：取得可写空间 → 写消息 → 发送消息就绪通知 → 等待确认 → 再次写入
                                  │                 ↑
                                  ↓                 │
消费者：                     等待通知 → 读取消息 → 返回确认
```

若采用消息头表示有效状态，应先写消息体，再发布消息头和相应通知。消费者等待通知后再检查状态并读取内容，最后按协议清理状态和返回确认。对于只收发一次、在通知前完整写入且不轮询初始状态的消息，可以不设消息头；不能把环形队列的状态字段当成SSBuffer硬件的固定要求。

SSBuffer不具有GM通信所涉及的数据缓存，读取消息无需执行GM缓存刷新，但仍需要建立读写顺序。跨核轮询的字段需确保每次重新读取，`volatile`不保证复合消息原子性，也不能代替核间同步。编译器屏障和核内屏障同样不能独立完成跨核交接。

每个同步ID对应4位计数器，尚未消费的通知数不能超过15。一个任务可以交换超过15条消息，但必须通过匹配的等待及时消费通知。不同ID的通知不保证按书写顺序生效，不能用通知的代码排列顺序替代数据依赖。

## 通信编程流程

### 一次消息收发

显式SSBuffer通信需要明确：消息空间属于谁、谁写谁读、何时消息有效、何时可以复用，以及双方何时退出。推荐先建立单条消息的正确交接，再扩展到多条消息队列。

以下代码片段以使用基础API实现AIV0向AIC传递32B消息为例，展示模式4下的写入、读取和ACK。该片段位于同一个`__mix__(1, 2)`核函数内，`output`指向本组独占的4个`uint64_t` GM输出元素；AIV1不参与这次模式4通信。消息在通知前被完整写入，不依赖初始内容。

```cpp
struct alignas(32) SsbufferMessage { uint64_t value[4]; };
// __ssbuf__表示SSBuffer地址空间；示例独占从基址开始的32B区域。
auto message = reinterpret_cast<__ssbuf__ SsbufferMessage*>(AscendC::GetSsbufBaseAddr());
if ASCEND_IS_AIV {
    if (AscendC::GetSubBlockIdx() == 0) {
        for (uint32_t i = 0; i < 4; ++i) {
            message->value[i] = i + 1;
        }
        AscendC::CrossCoreSetFlag<4, PIPE_S>(0);
        AscendC::CrossCoreWaitFlag<4, PIPE_S>(1); // 等待AIC读完，之后才能覆盖该区域。
    }
}
if ASCEND_IS_AIC {
    AscendC::CrossCoreWaitFlag<4, PIPE_S>(0);
    for (uint32_t i = 0; i < 4; ++i) {
        output[i] = message->value[i];
    }
    AscendC::CrossCoreSetFlag<4, PIPE_S>(1); // 通知AIV0消息已读取。
}
```

该片段只表达消息交接；完整Kernel还需进行控制状态初始化、输出区域分配及必要的收尾同步。完整工程组织与多消息通信可参考[SSBuffer AIV/AIC通信基础 API样例](../../../../../../examples/01_simd_cpp_api/05_best_practices/03_fusion_compute/ssbuf_aiv_aic_comm/README.md)，使用C API的实现可参考[SSBuffer AIV/AIC通信C API样例](../../../../../../examples/02_simd_c_api/05_best_practices/03_fusion_compute/ssbuf_aiv_aic_comm)。

### 连续消息的初始化、发布和确认

对于使用消息头和环形队列的算子，按以下步骤实现：

1. **规划空间**：为两个AIV划分互不重叠的队列，统一消息类型、队列深度、消息数量、同步ID和输出偏移。
2. **初始化状态**：由AIC作为唯一初始化方清理消息头，避免将旧有效位当成新消息。通过[InitSocState](../../../../api/SIMD-API/basic_api/tool_interface/system_init/InitSocState.md)或C API的[asc_init](../../../../api/SIMD-API/c_api/utils/sys_init/asc_init.md)恢复控制状态，不能代替自定义SSBuffer队列初始化。
3. **建立启动顺序**：AIC清理完成后分别通知AIV0、AIV1。两个AIV收到各自的启动通知后，才能开始向队列写入，避免初始化覆盖新消息。
4. **发布消息**：生产者先确保当前位置可写，写入本次消息体，再设置消息头中的有效标记，最后发送消息准备完成通知。
5. **消费消息**：AIC等待所选AIV的通知，读取消息头和消息体，校验计算方式、发送方和序号，再处理消息。不能只看到有效标记就忽略协议要求的通知。
6. **确认完成**：AIC完成该消息所需的读取后清理消息头，向原发送方返回ACK。返回ACK前，必须保证后续操作不再依赖即将被覆盖的消息内容。
7. **复用或退出**：AIV在相应ACK到达后复用消息空间；发送结束后等待所有尚未确认的消息。AIC按双方约定的数量或结束消息退出，不能自行提前停止服务。

```text
AIC：清理消息头 → 发送启动通知 → 等待消息通知 → 读取并处理 → 清消息头 → 返回ACK
                         │             ↑                              │
                         ↓             │                              ↓
AIV：               等待启动通知 → 写消息并通知 → 使用下一位置/等待ACK → 复用
```

消息头有效位表示消息的协议状态，启动、消息准备完成和ACK是核间同步通知，两者需要按同一套规则使用。软件轮询消息头是另一种实现方式，读取循环必须持续访问共享状态，并处理发布顺序和退出条件；不能只删除通知和ACK等待，就把模式4通信算子改成正确的软件轮询方案。

### 多消息通信与退出

对于连续消息，可以为不同AIV分别规划环形队列。消息总数可以大于队列深度，但生产者复用位置前必须确认旧消息已经读取完成。位置可用`sequence % depth`计算，只有深度为2的幂时，才可使用`sequence & (depth - 1)`代替。

队列深度约束未确认的消息数量，同步计数器约束未消费的通知数量，两者都需要满足。按队列顺序返回的ACK可以用于顺序复用；若允许乱序消费，确认信息还需能区分具体位置。双方应统一消息总数或结束消息的含义，发送端退出前等待所有尚未完成的确认，接收端不能提前停止服务。

确认消息空间可复用，并不表示消息所指向的UB、L1 Buffer或GM数据也可覆盖。异步搬运和矩阵计算仍需满足输入消费、输出完成及缓冲区复用条件。

环形队列的空间划分、消息收发、队列回绕和结果校验的完整实现，参见[基于SSBuffer的核间通信特性](../../../operator_practice/simd_operator_impl/fusion_operator_programming/cv_fusion/ssbuffer_feature_scenarios.md)。消息大小和队列深度由算子设计决定，应根据实际消息布局计算空间占用，并满足SSBuffer容量和同步计数器的约束。

## 与数据搬运和Matmul高阶API配合

### 业务数据搬运

SSBuffer传递控制消息，大块数据使用相应搬运接口。根据CV融合算子的数据流选择如下通路，具体数据类型、格式、长度和对齐要求参见接口说明。

**表4**  相关数据通路

| 数据流 | 基础API | C API | 与通信的关系 |
| --- | --- | --- | --- |
| AIV UB → AIC L1 Buffer | [DataCopy连续搬运](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_continuous.md)、[高维切分搬运](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_highdim_split.md)、[ND2NZ搬运](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_ND2NZ.md)、[DataCopyPad](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopyPad_UBToL1.md)。 | [asc_copy_ub2l1](../../../../api/SIMD-API/c_api/vector_datamove/asc_copy_ub2l1.md)。 | 可以先交换L1地址等参数，再通过硬通道搬运；消息已发布不表示输入已搬完。 |
| AIC L1 Buffer → AIV UB | [DataCopyL1ToUB](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/DataCopyL1ToUB.md)。 | [asc_copy_l12ub](../../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12ub.md)。 | 用于L1数据提供给Vector侧，仍需满足目的UB及读写同步约束。 |
| AIC L0C Buffer → AIV UB | [Fixpipe](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToUB.md)。 | [asc_copy_l0c2ub](../../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2ub.md)。 | 将矩阵输出直接交给Vector处理，需要保证FIX输出完成后再消费。 |

基础API的UB到L1 Buffer硬通道路径与GM软件仿真路径不同：前者在相应配置下直接搬运，后者需要借助Matmul注册及GM中转空间。C API直接使用对应硬件接口，不能套用基础API兼容路径的注册要求。数据搬运能力也不意味着每次调用都要求用户另写一条SSBuffer消息，是否交换地址和参数取决于算子设计。

对应的实际数据流为：

```text
控制消息：AIV Scalar ↔ SSBuffer ↔ AIC Scalar
Vector作为Cube输入：UB → L1 Buffer → L0A/L0B Buffer → Cube计算
Cube作为Vector输入：Cube计算 → L0C Buffer → UB → Vector计算
```

### Matmul使用SSBuffer的方式

在普通的Matmul混合计算模式下，AIV侧客户端将调用信息交给AIC侧服务。使用SSBuffer路径时，框架会在SSBuffer中管理以下信息：

- **调用消息**：函数编号、实例编号、输入输出地址及形状等参数，通过消息队列传给AIC。
- **Tiling信息**：AIV通过专用区域传递矩阵分块参数，AIC在处理初始化消息时取得副本，再允许该区域复用。
- **L1 Buffer地址**：AIC在适用输入类型下发布A、B、bias和scale等资源的L1地址，AIV据此准备UB到L1 Buffer的数据搬运。

框架还通过核间事件协调L1空间复用、输入就绪和矩阵输出完成。消息区域释放、Tiling区域可复用、L1地址可用和输出计算完成是不同条件。开发者应按[SetTensorA](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorA.md)、[GetTensorC](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetTensorC.md)及[WaitIterateAll](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/WaitIterateAll.md)等接口的同步/异步约束使用结果，不能以某个共享标志代替这些接口的完成条件。

配套的搬运服务也会传递GM到L1 Buffer搬运所需的地址、块长和步长等描述，实际矩阵内容仍在数据通路上搬运。

Matmul使用SSBuffer通信时，还支持UB/TSCM输入、MX矩阵计算及scale输入等能力。例如[SetTensorScaleA](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorScaleA.md)、[SetTensorScaleB](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorScaleB.md)需要匹配支持该能力的配置。TSCM表示相关队列使用的L1 Buffer位置，不是SSBuffer中的矩阵存储区。设置UB/TSCM、AB共享、批量迭代或双主模式时，还需遵守对应Matmul配置的支持范围，不能认为开启SSBuffer后所有组合都适用。当前SSBuffer通信实现每个AIV使用8个命令位置，支持的Matmul对象数上限为4；命令位置数不等于对象数。UB来源的TSCM队列还会占用核间同步资源，当前框架对所用TSCM Buffer数与Matmul对象数之和检查上限为10，不能分别按各自最大值独立配置。

可结合以下样例了解不同用法：

- [MatmulLeakyRelu](../../../../../../examples/01_simd_cpp_api/00_introduction/03_fusion_operation/matmul_leakyrelu_advanced_api)：矩阵计算与Vector激活融合，3510构建配置通过`ENABLE_CV_COMM_VIA_SSBUF=true`启用SSBuffer通信。
- [matmul_vecout](../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_vecout)：以Vector侧VECOUT作为矩阵输入。
- [matmul_mx](../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_mx)、[matmul_mx_scale_cache](../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_mx_scale_cache)：MX计算及scale相关场景。
- [matmul_mx_ub_tscm_nz](../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_mx_ub_tscm_nz)：UB、TSCM和NZ格式的MX矩阵输入。

## 资源管理与验证

自定义消息区域不能与Matmul框架或其他通信方案的SSBuffer区域重叠。Matmul除调用消息外还会保存Tiling和L1地址信息，不能只查看命令队列大小就把其他空间视为可用。对已有通信区域的初始化也不能覆盖仍在使用的消息。

同步ID也需要统一规划。Matmul、TSCM队列和[SyncAll](../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/SyncAll.md)可能占用核间同步资源，独立示例中的0、1等ID不表示在融合算子中始终可用。应按相应接口和配置检查资源占用，不能只改变消息偏移而忽略同步ID冲突。

SSBuffer消息空间、系统workspace和算子GM workspace用途不同。启用SSBuffer可以减少相应GM消息中转，但不表示所有Matmul调用或算子都不再需要workspace，仍需遵守公开接口的参数及资源管理约束。

验证通信时应覆盖首次读写、两个AIV分别通信、队列回绕及重复启动，并检查发送方、消息序号、数据内容和总数。出现持续等待时，先核对同步模式、双侧ID映射、通知与等待次数及退出条件；消息正确但计算结果异常时，再核对实际搬运流水的完成条件和缓冲区复用时机。CPU调试可辅助验证布局和部分逻辑，不能代替NPU环境下的核间同步及性能验证。
