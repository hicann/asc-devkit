# 基于SSBuffer的核间通信特性

本章节介绍CV融合算子中使用SSBuffer的基础知识、通信规则和算子实现方法。

>[!NOTE]说明
>
>阅读本节前，请先了解[CV融合算子](./basics.md)以及[地址空间限定符](../../../../programming_guide/language_extension/simd_builtin_keywords.md#地址空间限定符)。
>
>本节内容的产品支持情况如下：
><!-- npu="950" id1 -->
>- Ascend 950PR/Ascend 950DT：支持
><!-- end id1 -->
><!-- npu="A3" id2 -->
>- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
><!-- end id2 -->
><!-- npu="910b" id3 -->
>- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
><!-- end id3 -->
><!-- npu="310b" id4 -->
>- Atlas 200I/500 A2 推理产品：不支持
><!-- end id4 -->
><!-- npu="310p" id5 -->
>- Atlas 推理系列产品AI Core：不支持
><!-- end id5 -->
><!-- npu="310p" id6 -->
>- Atlas 推理系列产品Vector Core：不支持
><!-- end id6 -->
><!-- npu="910" id7 -->
>- Atlas 训练系列产品：不支持
><!-- end id7 -->

配套的可执行样例如下：

- [AIV/AIC环形消息队列通信C API样例](../../../../../../../examples/02_simd_c_api/05_best_practices/03_fusion_compute/ssbuf_aiv_aic_comm)：使用C API实现两条AIV到AIC的消息队列，也是当前章节采用的完整样例。
- [AIV/AIC环形消息队列通信](../../../../../../../examples/01_simd_cpp_api/05_best_practices/03_fusion_compute/ssbuf_aiv_aic_comm)：使用基础API实现两条AIV到AIC的消息队列。

下文以基于SSBuffer的AIV/AIC通信算子为例，介绍显式SSBuffer通信的设计和实现过程。该核函数使用[`__mix__(1, 2)`执行空间限定符](../../../../programming_guide/language_extension/simd_builtin_keywords.md#section1074418132518)，表示核函数（Kernel）启动的Cube核和Vector核的配比。每个AIV通过自己的环形消息队列发送12条消息；AIC读取消息，校验功能号、发送方、序号和校验值，并将消息字段及校验结果写入GM；Host侧逐条校验24个结果中的`valid`、`sender`、`sequence`、`value`和`checksum`字段。

算子的设计过程分为**算子分析、数据流分析、SSBuffer空间规划、通信规则设计、算子实现**五部分。完整代码请参考[SSBuffer AIV/AIC通信C API样例](../../../../../../../examples/02_simd_c_api/05_best_practices/03_fusion_compute/ssbuf_aiv_aic_comm)。

## 算子分析

该样例用于验证AIV到AIC的消息通信。两个AIV在Device侧生成消息；AIC解析收到的消息，校验功能号、发送方、序号和校验值，并将消息字段及校验结果整理为`CommResult`写入GM。算子的处理逻辑如下：

1. AIC清理两条消息队列，并分别通知AIV0和AIV1开始发送。
2. AIV0和AIV1各自生成12条消息。消息中包含发送方编号、消息序号、`value`字段和校验值。
3. AIV写完一条消息的全部字段后发送消息就绪通知，表示对应SSBuffer的消息可以读取。
4. AIC收到消息就绪通知后读取并校验消息，清除消息头，并返回确认（ACK）通知。该通知表示AIC已读取消息，对应的队列槽位可以由AIV复用。具体通信规则请参见[发布和确认顺序](#发布和确认顺序)和[队列回绕](#队列回绕)。
5. AIC共接收24条消息并写入GM，Host侧回读结果，逐项校验每个`CommResult`的`valid`、`sender`、`sequence`、`value`和`checksum`字段，确认消息没有丢失、覆盖或进入错误的消息队列。

核函数（Kernel）名称为`c_api_ssbuf_aiv_aic_comm`，参数为`output`。`output`指向GM中的结果数组，数组包含24个`CommResult`。每个结果占32B，字段如下。注：实际算子的消息规模如果随输入变化，应在Host侧统一设计、补充计算相关参数，再传给AIC和AIV，避免两侧使用不一致的消息数量或退出条件。

**表1**  `CommResult`字段说明

| 字段 | 数据类型 | 说明 |
| --- | --- | --- |
| `sender` | `uint32_t` | 发送消息的AIV编号，AIV0为0，AIV1为1。 |
| `valid` | `uint32_t` | 消息检查结果。值为1表示功能号、发送方、序号和校验值均符合预期。 |
| `sequence` | `uint64_t` | 消息序号，每个AIV分别从0递增到11。 |
| `value` | `uint64_t` | 消息携带的示例数据。 |
| `checksum` | `uint64_t` | 根据发送方、序号和消息数据生成的校验值。 |

样例通过下面两个公式生成消息数据和校验值：

$$
value = 1000 + sender \times 100 + sequence
$$

$$
checksum = CHECKSUM\_MAGIC \oplus sender \oplus sequence \oplus value
$$

其中，$\oplus$表示按位异或。`CHECKSUM_MAGIC`固定为`0x5353425546434D4D`，其字节对应ASCII字符串`SSBUFCMM`，是样例自定义的易识别通信校验常量，不是硬件规定值。消息数据是指`value`字段，其值由发送方编号和消息序号共同生成；Host侧使用相同公式计算期望值并进行比较，从而检查消息是否按预期的发送方和序号写入结果。校验值用于发现消息内容是否完整。

## 数据流分析

算子涉及两条数据流：任务启动时的同步通知，以及任务运行中的消息收发。

1. 启动同步：AIC清理SSBuffer后，通过核间同步分别通知AIV0和AIV1。该通知表示消息队列已经可以使用。
2. 消息收发：AIV写完消息后发送消息就绪通知；AIC收到通知后读取并校验消息，将结果写入GM，清除消息头后再返回确认（ACK）通知。

整体数据流如下。

```text
                       核间同步：启动通知
                AIC --------------------------> AIV0、AIV1
                 ^                                  |
                 |               消息就绪通知        | 写消息
                 +----------------------------------+
                 |                                  v
Host <---- GM <---- AIC <----------------------- SSBuffer
                     |
                     +-- 清消息头并返回确认通知 ----> AIV0、AIV1
```

启动通知保证AIV不会访问尚未初始化的消息队列。消息就绪通知保证AIC在AIV写完消息后再读取；确认（ACK）通知保证AIV只在AIC读取消息并释放槽位后复用同一消息空间。具体顺序请参见[发布和确认顺序](#发布和确认顺序)，队列槽位的复用方式请参见[队列回绕](#队列回绕)。因此三个阶段都涉及核间同步。

## SSBuffer空间规划

### 消息结构

每条消息占用128B。包含参数如下：

```cpp
struct alignas(128) CommMessage {
    volatile uint32_t head;
    volatile uint32_t reserved0;
    volatile uint64_t sequence;
    volatile uint64_t value;
    volatile uint64_t checksum;
    uint64_t reserved[12];
};

struct SsbufWorkspace {
    CommMessage queue[2][8];
};
```

**表2**  `CommMessage`字段说明

| 字段 | 说明 |
| --- | --- |
| `head` | 消息头，保存功能号、`valid`位和发送方编号。AIC通过它判断消息是否可以读取。 |
| `reserved0` | 保留字段。AIC返回ACK时，与`head`一起清零。 |
| `sequence` | 消息序号，用于确认队列回绕后读到的是当前期望的消息。 |
| `value` | 消息内容。实际算子可以替换为数据准备状态、地址或长度等控制信息。 |
| `checksum` | 消息校验值。 |
| `reserved` | 预留空间，用于补齐128B。 |

### 队列布局

每个AIV独占一条深度为8的队列，两条消息队列共占2KB。队列深度8表示每条队列只有8个物理槽位，不是每个AIV只发送8条消息。本样例中每个AIV发送12条消息：前8条分别使用8个槽位，第9至第12条在收到对应的确认（ACK）通知后依次复用前4个槽位，两个AIV最终共生成24个`CommResult`。剩余1KB没有被本算子使用。

```text
SSBuffer [0, 1024)       AIV0消息队列：8条消息，每条128B
SSBuffer [1024, 2048)    AIV1消息队列：8条消息，每条128B
SSBuffer [2048, 3072)    本算子不使用
```

3KB是SSBuffer的硬件容量；2KB工作区、队列深度8和每条消息128B是本样例的协议设计，并不是SSBuffer的固定格式。调整这些参数时，需要重新检查地址对齐、消息队列是否重叠以及总占用空间是否超过3KB。

两条消息队列分开后，AIV0和AIV1只写自己的区域，不需要争抢同一个生产位置。AIC按消息队列读取，也能直接判断消息来自哪个AIV。

## 通信规则设计

### 消息状态

样例使用32位`head`表示消息状态：高16位保存功能号`0x5A01`，bit 15为`valid`位，低8位保存发送方编号。

```cpp
constexpr uint32_t VALID_MASK = 0x00008000U;
constexpr uint32_t FUNCTION_ID = 0x5A01U;

__aicore__ inline uint32_t make_head(uint32_t sender) // __aicore__表示该函数在AI Core上执行。
{
    return (FUNCTION_ID << 16) | VALID_MASK | (sender & 0xFFU);
}
```

`valid`为0表示该位置没有待处理消息；`valid`为1表示AIV已经发布消息。AIC处理完后清除消息头，使消息状态恢复为0。样例同时使用确认（ACK）通知控制空间复用，因此`valid`主要用于检查消息状态是否符合协议。

### 核间同步

样例使用[`asc_sync_intra_arrive`](../../../../../api/SIMD-API/c_api/sync/asc_sync_intra_arrive.md)和[`asc_sync_intra_wait`](../../../../../api/SIMD-API/c_api/sync/asc_sync_intra_wait.md)。这两个接口属于核间同步，对应模式4，可以在同一AI Core内让AIC与单个AIV互相发送通知。

### 发布和确认顺序

双方必须遵守下面的顺序。核间同步接口既负责通知，也为`PIPE_S`上的SSBuffer访问建立先后关系。

| AIV发布消息 | AIC消费消息 |
| --- | --- |
| 队列回绕前，等待对应的确认（ACK）通知。 | 等待对应AIV的消息就绪通知。 |
| 写入`sequence`、`value`、`checksum`和`head`。 | 读取消息头和消息体，并将检查结果写入GM。 |
| 调用`asc_sync_intra_arrive(PIPE_S, MESSAGE_READY_SYNC_ID)`。 | 清除消息头。 |
| 继续使用新的消息空间，或在发送结束后等待剩余确认通知。 | 调用`asc_sync_intra_arrive(PIPE_S, MESSAGE_ACK_SYNC_ID)`返回确认通知。 |

### 队列回绕

环形消息队列使用固定大小的存储空间反复承载连续消息。当消息序号超过队列深度时，新的消息会映射回队列开头，这个过程称为队列回绕。回绕只改变消息使用的位置，不会改变消息序号。

样例使用`sequence & (QUEUE_DEPTH - 1)`计算消息在队列中的位置。`QUEUE_DEPTH`为8，是2的幂，因此位与计算等价于`sequence % 8`。

| 消息序号 | 队列位置 | 使用条件 |
| --- | --- | --- |
| 0～7 | 0～7 | 首次使用，不需要等待历史ACK。 |
| 8～11 | 0～3 | 分别复用序号0～3使用过的位置，写入前必须收到对应ACK。 |

例如，序号0和序号8都会使用位置0。AIV发送序号8前先等待一个ACK；这个ACK表示AIC已经读完序号0并清除了消息头。这样，有限的8条消息空间可以承载12条乃至更多连续消息，同时不会覆盖尚未消费的内容。

生产者已发布但尚未收到ACK的消息数都不能超过队列深度，这是环形消息队列能够正确回绕的基本条件。

## 算子实现

### 获取SSBuffer工作区

在`dav-3510` NPU上，`__ssbuf__`地址空间的零地址表示SSBuffer基地址。样例将该地址转换为工作区指针，并在后续访问中保留`__ssbuf__`限定。

```cpp
__aicore__ inline __ssbuf__ SsbufWorkspace* get_workspace() // __aicore__表示函数在AI Core上执行。
{
    return reinterpret_cast<__ssbuf__ SsbufWorkspace*>(0); // __ssbuf__表示指针指向SSBuffer。
}
```

### 初始化消息队列

SSBuffer中的初始内容不保证为0。核函数入口先调用[`asc_init`](../../../../../api/SIMD-API/c_api/utils/sys_init/asc_init.md)恢复NPU控制状态，但该接口不会清零SSBuffer，因此仍需要由AIC清理全部16个消息头。

```cpp
__aicore__ inline void init_queues(__ssbuf__ SsbufWorkspace* workspace)
{
    for (uint32_t sender = 0; sender < AIV_COUNT; ++sender) {
        for (uint32_t queueIndex = 0; queueIndex < QUEUE_DEPTH; ++queueIndex) {
            clear_message(&workspace->queue[sender][queueIndex]);
        }
    }
}
```

### AIV发送消息

AIV通过[`asc_get_sub_block_id`](../../../../../api/SIMD-API/c_api/utils/sys_var/asc_get_sub_block_id.md)取得当前AIV在执行组合中的编号（subblock ID），并选择自己的消息队列。发送流程如下：

```cpp
__aicore__ inline void send_messages(__ssbuf__ SsbufWorkspace* workspace, uint32_t sender)
{
    asc_sync_intra_wait(PIPE_S, START_SYNC_ID);

    for (uint64_t sequence = 0; sequence < MESSAGE_COUNT_PER_AIV; ++sequence) {
        if (sequence >= QUEUE_DEPTH) {
            asc_sync_intra_wait(PIPE_S, MESSAGE_ACK_SYNC_ID);
        }

        __ssbuf__ CommMessage* message = &workspace->queue[sender][sequence & (QUEUE_DEPTH - 1)];
        const uint64_t value = make_value(sender, sequence);
        message->sequence = sequence;
        message->value = value;
        message->checksum = make_checksum(sender, sequence, value);
        message->head = make_head(sender);
        asc_sync_intra_arrive(PIPE_S, MESSAGE_READY_SYNC_ID);
    }

    for (uint32_t ackIndex = 0; ackIndex < QUEUE_DEPTH; ++ackIndex) {
        asc_sync_intra_wait(PIPE_S, MESSAGE_ACK_SYNC_ID);
    }
}
```

前8条消息可以连续发布；第9条消息开始，每次写入前先等待一个ACK，确保即将复用的位置已经由AIC释放。12条消息全部发布后，前4个ACK已经在回绕时被消费，AIV再等待剩余8个ACK即可退出。

### AIC接收消息

AIC按照发送方编号和消息序号读取两条消息队列。收到消息就绪通知后，读取消息并校验功能号、发送方、序号和校验值，将消息字段及校验结果写入GM，最后清除消息头并返回确认通知。

```cpp
// __aicore__表示函数在AI Core上执行，__ssbuf__和__gm__分别表示SSBuffer和GM地址空间。
__aicore__ inline void receive_messages(__ssbuf__ SsbufWorkspace* workspace, __gm__ CommResult* output)
{
    for (uint32_t expected_sender = 0; expected_sender < AIV_COUNT; ++expected_sender) {
        for (uint64_t expected_sequence = 0; expected_sequence < MESSAGE_COUNT_PER_AIV; ++expected_sequence) {
            asc_sync_intra_wait(PIPE_S, get_aic_sync_id(expected_sender, MESSAGE_READY_SYNC_ID));
            __ssbuf__ CommMessage* message =
                &workspace->queue[expected_sender][expected_sequence & (QUEUE_DEPTH - 1)];
            const uint32_t head = message->head;

            const uint32_t sender = get_sender(head);
            const uint64_t sequence = message->sequence;
            const uint64_t value = message->value;
            const uint64_t checksum = message->checksum;
            const bool valid = get_function_id(head) == FUNCTION_ID && is_valid(head) && sender == expected_sender &&
                sequence == expected_sequence && checksum == make_checksum(sender, sequence, value);

            __gm__ CommResult* result = output + expected_sender * MESSAGE_COUNT_PER_AIV + expected_sequence;
            result->sender = sender;
            result->valid = valid ? 1U : 0U;
            result->sequence = sequence;
            result->value = value;
            result->checksum = checksum;

            clear_message(message);
            asc_sync_intra_arrive(PIPE_S, get_aic_sync_id(expected_sender, MESSAGE_ACK_SYNC_ID));
        }
    }
}
```

当前样例先接收AIV0的12条消息，再接收AIV1。两条消息队列互不重叠，AIV1最多先发布8条消息，然后等待ACK，不会影响AIV0继续发送。实际业务如果更关注两个AIV的响应时间，可以改为轮流接收两条消息队列，但需要继续遵守相同的同步ID映射和ACK规则。

### Kernel入口

Kernel使用`__mix__(1, 2)`执行空间限定符，表示每个核函数执行组合包含1个AIC和2个AIV。`ASC_IS_AIC`和`ASC_IS_AIV`将两侧代码隔离开。

```cpp
// __global__表示Kernel入口，__mix__(1, 2)表示启动1个AIC和2个AIV，__gm__表示参数指向GM。
extern "C" __global__ __mix__(1, 2) void c_api_ssbuf_aiv_aic_comm(__gm__ CommResult* output)
{
    asc_init();
    __ssbuf__ SsbufWorkspace* workspace = get_workspace();

    if ASC_IS_AIC {
        init_queues(workspace);
        asc_sync_intra_arrive(PIPE_S, get_aic_sync_id(0, START_SYNC_ID));
        asc_sync_intra_arrive(PIPE_S, get_aic_sync_id(1, START_SYNC_ID));
        receive_messages(workspace, output);
    }

    if ASC_IS_AIV {
        const uint32_t sender = static_cast<uint32_t>(asc_get_sub_block_id());
        send_messages(workspace, sender);
    }
}
```

## 编译和运行

该样例支持NPU和仿真运行模式，NPU架构固定为`dav-3510`。以NPU模式为例，编译和运行命令如下：

```bash
source ${install_path}/cann/set_env.sh
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCMAKE_ASC_RUN_MODE=npu ..
make -j
./demo
```

## 验证和使用约束

样例通过以下检查确认通信过程正确：

- 两个AIV各发送12条消息，AIC共接收24条，Host侧逐条校验通过。
- 第9至第12条消息复用队列的前4个位置，验证回绕时不会覆盖尚未消费的消息。
- 消息序号和校验值同时参与检查，便于发现旧消息、消息进入错误队列和内容不完整。

使用这套方法实现其他算子时，还需要遵守以下约束：

- AIC和AIV必须由同一个使用`__mix__(1, 2)`执行空间限定符的核函数执行组合访问SSBuffer。
- 每次任务启动都要由唯一的初始化方清理消息状态。
- 自定义消息布局不能超过3KB，也不能与KFC、Matmul高阶API或其他SSBuffer通信方案占用重叠空间。
- 跨核共享字段需要使用`volatile`，消息就绪和空间复用需要使用配对的核间同步通知。
- 队列深度和收发节奏需保证核间同步中同步计数器不超过指令约束上限。
- 消息数量、队列归属、同步ID映射或退出条件不一致时，任务可能一直等待，因此这些内容应由用户统一定义。
