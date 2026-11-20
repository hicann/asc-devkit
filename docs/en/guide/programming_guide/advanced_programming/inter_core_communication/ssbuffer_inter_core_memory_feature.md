# Using SSBuffer for Inter-Core Communication

<!-- md-trans-meta sourceCommit=31a38264724303fd61aceda1dcc05c8407b0c930 translatedAt=2026-11-09T10:54:21.073Z pushedAt=2026-11-19T06:32:19.904Z nl=2 -->

This document describes the storage scope, address space, and access methods of SSBuffer, as well as the programming method for message exchange in coordination with inter-core synchronization. For the specific design of the message queue, Host-side verification, and the complete operator implementation, see [SSBuffer-Based Inter-Core Communication Feature](../../../operator_practice/simd_operator_impl/fusion_operator_programming/cv_fusion/ssbuffer_feature_scenarios.md).

>[!NOTE] Note
>Before reading, understand the read/write ordering and synchronization concepts in [Address Space Qualifiers](../../language_extension/simd_builtin_keywords.md#section1624210295308) and [Memory Consistency](../memory_model/memory_consistency.md).
>The product support for this section is as follows:
><!-- npu="950" id1 -->
>- Ascend 950PR&950DT products: Supported
><!-- end id1 -->
><!-- npu="A3" id2 -->
>- Atlas A3 products: Not supported
><!-- end id2 -->
><!-- npu="910b" id3 -->
>- Atlas A2 products: Not supported
><!-- end id3 -->
><!-- npu="310b" id4 -->
>- Atlas 200I/500 A2 inference products: Not supported
><!-- end id4 -->
><!-- npu="310p" id5 -->
>- Atlas inference products AI Core: Not supported
><!-- end id5 -->
><!-- npu="310p" id6 -->
>- Atlas inference products Vector Core: Not supported
><!-- end id6 -->
><!-- npu="910" id7 -->
>- Atlas training products: Not supported
><!-- end id7 -->

## Overview

SSBuffer is a physical storage unit inside the AI Core and has an independent address space. In decoupled mode, when `__mix__(1, 2)` is used to configure the kernel function (MIX task), the AIC and the two AIVs on the same AI Core share the SSBuffer. They can read from and write to the SSBuffer through the Scalar unit to exchange control messages, addresses, lengths, and a small amount of scalar data. For the hardware architecture and its relationship with other storage units, see [NPU Architecture Version 3510](../hardware_implementation/architecture_spec/npu_arch_3510.md).

Communication in Cube (cube computation) and Vector (vector computation) fused operators (hereinafter referred to as CV fused operators) includes message exchange and service data transfer. For example, when an AIV uses the vector computation result as the input of cube computation, it needs to tell the AIC the input location, cube shape, and other messages, and it also needs to transfer the input data to the L1 Buffer. The SSBuffer can store these messages, while the cube data itself is transferred through data paths such as UB to L1 Buffer. The SSBuffer has limited capacity, so it is not used to store large blocks of intermediate computation results, nor can it replace Global Memory (GM), Unified Buffer (UB), or L1 Buffer.

SSBuffer read/write and inter-core synchronization are two related but different operations: writing a message is used to deliver content, while inter-core synchronization is used to ensure that the other side reads the message or data only after it is ready. The SSBuffer does not automatically generate a message queue, validity flag, or completion notification. Developers need to establish these relationships through APIs or communication rules.

## Scenarios and Usage

### Scenarios

SSBuffer is suitable for small-volume data communication within the same AI Core. Common scenarios are as follows.

**Table 1** SSBuffer communication scenarios

| Scenario | Content Transferred Through SSBuffer | Operations Requiring Coordination |
| --- | --- | --- |
| AIV submits a computation task to AIC | Computation mode, input address, cube shape, task sequence number, and so on. | AIV prepares the input. AIC waits for the corresponding notification through inter-core synchronization [mode 2](../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/inter_core_sync_overview.md) or [mode 4](../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/inter_core_sync_overview.md), then reads the message and executes the computation. |
| AIC feeds back a message to AIV | Target L1 Buffer address, next-round task parameters, processing result, and so on. | AIC notifies a single AIV through inter-core synchronization mode 4, or notifies two AIVs through mode 2. AIV waits for the notification before using the address or parameters. |
| AIC exchanges messages with a single AIV | Control messages produced by one side and consumed by the other. | Use inter-core synchronization mode 4 to coordinate AIC with AIV0 and AIC with AIV1 respectively. |
| AIC and two AIVs process collaboratively | Parameters produced by the two AIVs respectively, or parameters provided by AIC to the two AIVs. | Select inter-core synchronization mode 2 based on the dependency relationship to synchronize AIC with the two AIVs, or select mode 4 to synchronize AIC with each AIV respectively. |
| Two AIVs within the same AI Core exchange messages | A small amount of scalar data placed in an agreed area. | In a MIX task sharing SSBuffer, use inter-core synchronization [mode 1](../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/inter_core_sync_overview.md) to ensure the read/write order. |
| Communication for consecutive tasks | Multiple messages produced in sequence. | Establish a message queue for each AIV respectively, notify message readiness and processing completion through inter-core synchronization mode 4, and repeat sending and receiving in coordination with queue wraparound. |

In non-MIX mode, the SSBuffer space exclusively occupied by each core cannot be used in the shared manner described above. The SSBuffer shared layout described in this section should also not be used for data exchange between different AI Cores, between different tasks, or between the Host and the Device.

### Advantages

Compared with exchanging small messages through GM, SSBuffer communication does not require messages to be first written to GM and then read by the other side. It can reduce GM access and the associated bandwidth usage, lowering the overhead of inter-core communication for small amounts of data. For message parameters that need to be used repeatedly, you can read them into local variables of the current core after synchronization is complete, reducing repeated access to SSBuffer. When used together with data paths such as UB to L1 Buffer and L0C Buffer to UB, it can reduce the number of times intermediate data of a fused operator passes through GM.

### Selecting a Usage Mode

This section focuses on the usage mode in which developers explicitly access SSBuffer. When selecting a usage mode, first determine whether inter-core communication is managed by the developer or by the Matmul framework.

**Table 2** Communication modes and API boundaries

| Usage | Scenario | Content that Developers Need to Handle |
| --- | --- | --- |
| Explicit SSBuffer communication | AIC/AIV need to exchange custom control messages or a small amount of scalar data. | Obtain the SSBuffer base address, and plan the message layout, initialization, inter-core synchronization, space reuse, and exit conditions on your own. |
| CV fusion based on the Matmul advanced API | AIV initiates Matmul computation and combines it with Vector computation. | Use public APIs such as [REGIST_MATMUL_OBJ](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/REGIST_MATMUL_OBJ.md), and let the framework manage the corresponding KFC calls and messages. Do not directly modify the internal message structure on the SSBuffer path. |

## Storage Specifications and Access Constraints

### Storage Scope and Access Constraints

Before use, confirm the following specifications and constraints:

**Table 3** SSBuffer specifications and constraints

| Item | Description |
| --- | --- |
| Supported products | Ascend 950PR&950DT products, corresponding to the compilation target `dav-3510`. |
| Total capacity | 3KB. |
| Non-MIX mode | AIC, AIV0, and AIV1 each occupy 1KB independently, and do not form a 3KB workspace accessible to all three. |
| `__mix__(1, 2)` mode | When the AIC:AIV ratio is 1:2, AIC, AIV0, and AIV1 share the entire 3KB space. |
| Address space | Use the `__ssbuf__` qualifier. |
| Access method | Read and write through the Scalar unit. |
| Alignment Requirement | SSBuffer supports only 32-byte aligned access through read/write instructions. The start offset of the communication area, the message layout, and the access range must meet this requirement. |
| Initial content | Dirty data may exist, and zero is not guaranteed. Before reading a message, the current message write or the necessary state initialization must be completed. |
| Task constraint | When AIC and AIV start different tasks, they cannot access the same SSBuffer for communication. Shared communication should use the `__mix__(1, 2)` kernel function configuration, and the corresponding kernel function type of the engineered operator is `KERNEL_TYPE_MIX_AIC_1_2`. |
| Out-of-bounds access | Accessing the address beyond the end of the available space causes an exception. In addition to the message content, the space for structure padding, alignment, queues, and feedback information must also be accounted for. |
| Concurrent write | Different producers should use non-overlapping message areas. After the same message space is handed over to the consumer, the producer cannot overwrite it before consumption is complete. |

### Kernel Function and Compilation Configuration

Use the [function execution space qualifier](../../language_extension/simd_builtin_keywords.md#section1074418132518) to specify the execution mode of the kernel function. `__mix__(1, 2)` indicates that each AIC/AIV group runs at a 1:2 ratio. When multiple groups run, each group accesses its own SSBuffer. For engineering operators, configure the [kernel function type](../../../../api/SIMD-API/basic_api/Kernel-Tiling/set_Kernel_type.md) corresponding to `__mix__(1, 2)`, that is, `KERNEL_TYPE_MIX_AIC_1_2`.

Under the preceding kernel function configuration, determine whether to set the compilation macro [ENABLE_CV_COMM_VIA_SSBUF](../../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#section57020345148) based on the communication implementation mode:

- Explicit SSBuffer communication: Developers read and write messages through SSBuffer address pointers, and the access itself does not depend on this compilation macro. The message layout, initialization, inter-core synchronization, and space reuse are managed by developers.
- Matmul framework-managed communication: To switch the storage space for AIV sending call messages to AIC from GM to SSBuffer, set this compilation macro to `true` (the default is `false`). The message space is managed by the Matmul framework.

When SSBuffer communication of Matmul is enabled, the compilation configuration is as follows:

```bash
bisheng <source_file>.asc -o <output_file> --npu-arch=dav-3510 -DENABLE_CV_COMM_VIA_SSBUF=true
```

Note that `ENABLE_CV_COMM_VIA_SSBUF` does not set the kernel function type, nor does it allocate space for custom messages, clear states, or insert send/receive synchronization.

For version differences and data path selection, see [3510 New Feature Usage Instructions](../../../cross_gen_migration_guide/instructions_for_new_features/3510_new_features.md#section_ssbuf).

## Address Space and Message Layout

### Obtaining the SSBuffer Base Address

In the basic API, [GetSsbufBaseAddr](../../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetSsbufBaseAddr.md) is used to obtain the SSBuffer base address. Its header file is `"basic_api/kernel_operator_sys_var_intf.h"`, and it can also be included through `"kernel_operator.h"`. The base address is obtained as follows:

```cpp
// __ssbuf__ indicates that the pointer points to the SSBuffer address space. This qualifier is retained in subsequent offset computes.
__ssbuf__ void* base = AscendC::GetSsbufBaseAddr();
__ssbuf__ uint8_t* bytes = reinterpret_cast<__ssbuf__ uint8_t*>(base);
```

In the NPU domain, this API returns the zero address of the `__ssbuf__` address space. The zero address here is a special base address of SSBuffer and cannot be used to determine whether the acquisition failed in the same way as an ordinary GM null pointer. In the CPU debugging domain, the API returns the address allocated by CPU simulation. Therefore, do not fix the base address to 0 in basic API code that requires CPU debugging.

The C API currently has no base address acquisition API corresponding to **GetSsbufBaseAddr**. In the `dav-3510` NPU environment, you can convert the zero address to a structure pointer with the `__ssbuf__` qualifier as shown in the following example.

```cpp
// For the definition of SsbufWorkspace, see below. Only the address is obtained here, and no dynamic memory allocation is performed.
__ssbuf__ SsbufWorkspace* workspace = reinterpret_cast<__ssbuf__ SsbufWorkspace*>(0);
```

### Defining the Message Layout

SSBuffer has no fixed message format and does not provide a general-purpose dynamic memory allocation API. Developers define fields and layout through ordinary structures, and then use pointers qualified with `__ssbuf__` to locate existing storage space. Defining the structure itself does not allocate additional SSBuffer.

The following uses a 32B message and two independent communication regions as an example, where each region stores four 64-bit values:

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

In this layout, the start offsets of the two message regions are 0B and 32B, respectively. Both communicating parties should use consistent field types, offsets, and lengths, and account for the padding introduced by struct alignment. The start offset of a region plus its occupied size must not exceed the available space in the current mode.

```text
SSBuffer under MIX 1:2 (this example)
Byte offset: 0                 32                 64                 3072
          | Message region 0, 32B | Message region 1, 32B | Space unused in this example |
```

Message regions can be divided by communication peer or communication direction. Different producers write to non-overlapping regions, and the same message can be overwritten only after the consumer has finished reading it. The GM, UB, or L1 Buffer addresses stored in a message only represent the data at the corresponding location; they do not change the address space or access range to which that data belongs.

The 32B alignment constraint of SSBuffer read/write instructions cannot be simply inferred from C/C++ member types. The 32-bit and 64-bit fields in a struct are used to express content, and `alignas` is used to constrain the layout; neither can guarantee the atomicity of an entire message or replace inter-core synchronization.

### Structure Pointers and Arrays

In the `dav-3510` compilation mode used in the samples in this section, directly declaring an array object with the `__ssbuf__` qualifier is not supported, nor is applying the qualifier to a pointer to an array type. For example:

```cpp
__ssbuf__ uint8_t ssbuffer[3 * 1024]; // Not supported: this declaration cannot be used to allocate an SSBuffer array.
using SsbufByteArray = uint8_t[3 * 1024];
__ssbuf__ SsbufByteArray* arrayPtr;   // Not supported: a pointer to an array type carries this qualifier.
```

You can describe the layout using the struct member array approach described above. You can also store an array of SSBuffer pointers in the default Local Memory, with each pointer pointing to an agreed message region. In this case, the pointer array itself is not located in the SSBuffer, and the region offsets are still managed by the developer. It is recommended that both sides share the same struct definition to avoid inconsistencies caused by computing the layout separately.

During CPU debugging, use the simulated address returned by GetSsbufBaseAddr. The device-side approach of converting 0 to a `__ssbuf__` pointer cannot be used for host memory access. Type identifiers such as `location::ssbuf` in static Tensor are used to describe the storage location; they do not mean that any Tensor computation or transfer API supports SSBuffer, nor do they provide additional SSBuffer storage space.

## Inter-Core Synchronization and Communication Order

### Synchronization Mode Selection

### Basic API Synchronization Mode

The basic APIs use [CrossCoreSetFlag](../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md) and [CrossCoreWaitFlag](../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md) in pairs for inter-core synchronization. The `modeId` template parameter selects the mode, and the `pipe` template parameter selects the pipeline.

### C API Synchronization Mode

The C API selects the synchronization mode through different APIs and selects the pipeline through the `pipe` parameter:

| Mode | Send Notification | Wait for Notification |
| --- | --- | --- |
| Mode 1 | [asc_sync_subblock_arrive](../../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_subblock_arrive.md) | [asc_sync_subblock_wait](../../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_subblock_wait.md) |
| Mode 2 | [asc_sync_block_arrive](../../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_block_arrive.md) | [asc_sync_block_wait](../../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_block_wait.md) |
| Mode 4 | [asc_sync_intra_arrive](../../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_intra_arrive.md) | [asc_sync_intra_wait](../../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_intra_wait.md) |

### Publishing, Reading, and Space Reuse

The complete handover of a message involves two directions: after writing, the producer notifies the consumer, and after reading, the consumer notifies the producer that the space can be reused. The acknowledgment (ACK) is the name given by the communication protocol to the return notification, not a message automatically provided by SSBuffer.

```text
Producer: obtain writable space → write message → send message-ready notification → wait for ACK → write again
                                  │                 ↑
                                  ↓                 │
Consumer:                     wait for notification → read message → return ACK
```

If a message header is used to indicate the valid state, write the message body first, and then publish the message header and the corresponding notification. After waiting for the notification, the consumer checks the state and reads the content, and finally clears the state and returns the ACK according to the protocol. For messages that are sent and received only once, are fully written before the notification, and do not poll the initial state, the message header can be omitted. The state field of the ring queue must not be treated as a fixed requirement of the SSBuffer hardware.

SSBuffer does not have the data cache involved in GM communication, so reading a message does not require a GM cache flush, but read/write ordering still needs to be established. Fields polled across cores must be re-read each time. `volatile` does not guarantee the atomicity of a composite message, nor can it replace inter-core synchronization. Compiler barriers and intra-core barriers likewise cannot independently complete a cross-core handover.

Each synchronization ID corresponds to a 4-bit counter, and the number of notifications that have not yet been consumed cannot exceed 15. A task can exchange more than 15 messages, but it must consume notifications in a timely manner through matching waits. Notifications of different IDs are not guaranteed to take effect in the order in which they are written, so the code ordering of notifications cannot be used as a substitute for data dependencies.

## Communication Programming Process

### Sending and Receiving a Message

Explicit SSBuffer communication requires clarifying: who owns the message space, who writes and who reads, when a message becomes valid, when it can be reused, and when both sides exit. It is recommended to first establish the correct handover of a single message, and then extend it to a multi-message queue.

The following code snippet uses the basic APIs to implement AIV0 passing a 32B message to AIC as an example, showing the write, read, and ACK under mode 4. This snippet is located in the same `__mix__(1, 2)` kernel function, where `output` points to the 4 exclusive `uint64_t` GM output elements of this group; AIV1 does not participate in this mode 4 communication. The message is fully written before the notification and does not depend on the initial content.

```cpp
struct alignas(32) SsbufferMessage { uint64_t value[4]; };
// __ssbuf__ indicates the SSBuffer address space; the example exclusively occupies the 32B region starting from the base address.
auto message = reinterpret_cast<__ssbuf__ SsbufferMessage*>(AscendC::GetSsbufBaseAddr());
if ASCEND_IS_AIV {
    if (AscendC::GetSubBlockIdx() == 0) {
        for (uint32_t i = 0; i < 4; ++i) {
            message->value[i] = i + 1;
        }
        AscendC::CrossCoreSetFlag<4, PIPE_S>(0);
        AscendC::CrossCoreWaitFlag<4, PIPE_S>(1); // Wait for AIC to finish reading before overwriting this region.
    }
}
if ASCEND_IS_AIC {
    AscendC::CrossCoreWaitFlag<4, PIPE_S>(0);
    for (uint32_t i = 0; i < 4; ++i) {
        output[i] = message->value[i];
    }
    AscendC::CrossCoreSetFlag<4, PIPE_S>(1); // Notify AIV0 that the message has been read.
}
```

This snippet only expresses the message handover; a complete kernel also requires control state initialization, output region allocation, and necessary final synchronization. For the complete project organization and multi-message communication, see [SSBuffer AIV/AIC Communication Basic API Sample](../../../../../../examples/01_simd_cpp_api/05_best_practices/03_fusion_compute/ssbuf_aiv_aic_comm/README.md). For the implementation using the C API, see [SSBuffer AIV/AIC Communication C API Sample](../../../../../../examples/02_simd_c_api/05_best_practices/03_fusion_compute/ssbuf_aiv_aic_comm).

### Initializing, Publishing, and Acknowledging Consecutive Messages

For operators that use message headers and ring queues, implement the following steps:

1. **Plan the space**: Allocate non-overlapping queues for the two AIVs, and unify the message type, queue depth, message count, synchronization ID, and output offset.
2. **Initialize the state**: Have the AIC act as the sole initializer to clear the message headers, preventing old valid bits from being mistaken for new messages. Restoring the control state through [InitSocState](../../../../api/SIMD-API/basic_api/tool_interface/system_init/InitSocState.md) or the C API [asc_init](../../../../api/SIMD-API/c_api/utils/sys_init/asc_init.md) cannot replace custom SSBuffer queue initialization.
3. **Establish the startup order**: After the AIC finishes clearing, it notifies AIV0 and AIV1 separately. Only after the two AIVs receive their respective startup notifications can they begin writing to the queues, preventing initialization from overwriting new messages.
4. **Publish messages**: The producer first ensures that the current position is writable, writes the message body, then sets the valid flag in the message header, and finally sends the message-ready notification.
5. **Consume messages**: The AIC waits for the notification from the selected AIV, reads the message header and message body, verifies the computation mode, sender, and sequence number, and then processes the message. It must not ignore the notification required by the protocol just because the valid flag is observed.
6. **Acknowledge completion**: After the AIC completes the reads required for the message, it clears the message header and returns an ACK to the original sender. Before returning the ACK, it must ensure that subsequent operations no longer depend on the message content that is about to be overwritten.
7. **Reuse or exit**: The AIV reuses the message space after the corresponding ACK arrives; after sending ends, it waits for all messages that have not yet been acknowledged. The AIC exits based on the count or end message agreed upon by both parties, and must not stop serving on its own ahead of time.

```text
AIC: clear message header → send startup notification → wait for message notification → read and process → clear message header → return ACK
                         │             ↑                              │
                         ↓             │                              ↓
AIV:               Wait for start notification → Write message and notify → Use next position/Wait for ACK → Reuse
```

The valid bit in the message header indicates the protocol state of the message. Start, message readiness, and ACK are inter-core synchronization notifications, and both must be used according to the same set of rules. Software polling of the message header is another implementation approach. The read loop must continuously access the shared state and handle the publication order and exit conditions. You cannot simply remove the notification and ACK waits and turn the mode 4 communication operator into a correct software polling solution.

### Multi-Message Communication and Exit

For consecutive messages, you can plan separate ring queues for different AIVs. The total number of messages can be greater than the queue depth, but the producer must confirm that the old message has been read before reusing a position. The position can be computed with `sequence % depth`; only when the depth is a power of 2 can `sequence & (depth - 1)` be used instead.

The queue depth constrains the number of unacknowledged messages, and the synchronization counter constrains the number of unconsumed notifications; both must be satisfied. ACKs returned in queue order can be used for sequential reuse; if out-of-order consumption is allowed, the acknowledgment information must also be able to distinguish specific positions. Both sides should agree on the total number of messages or the meaning of the end message. The sender waits for all outstanding acknowledgments before exiting, and the receiver must not stop serving early.

The reusability of the acknowledgment message space does not mean that the UB, L1 Buffer, or GM data pointed to by the message can also be overwritten. Asynchronous transfer and cube computation must still satisfy the conditions for input consumption, output completion, and buffer reuse.

For the complete implementation of ring queue space partitioning, message sending and receiving, queue wraparound, and result verification, see [SSBuffer-Based Inter-Core Communication Feature](../../../operator_practice/simd_operator_impl/fusion_operator_programming/cv_fusion/ssbuffer_feature_scenarios.md). The message size and queue depth are determined by the operator design. Compute the space usage based on the actual message layout, and satisfy the constraints of the SSBuffer capacity and synchronization counter.

## Coordination with Data Transfer and Matmul Advanced APIs

### Service Data Transfer

SSBuffer transfers control messages, while large blocks of data use the corresponding transfer APIs. Select the following path based on the data stream of the CV fused operator. For details about the data type, format, length, and alignment requirements, see the API descriptions.

**Table 4** Related data paths

| Data Stream | Basic API | C API | Relationship with Communication |
| --- | --- | --- | --- |
| AIV UB → AIC L1 Buffer | [DataCopy continuous transfer](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_continuous.md), [high-dimensional sharding transfer](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_highdim_split.md), [ND2NZ transfer](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_ND2NZ.md), [DataCopyPad](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopyPad_UBToL1.md). | [asc_copy_ub2l1](../../../../api/SIMD-API/c_api/vector_datamove/asc_copy_ub2l1.md). | You can first exchange parameters such as the L1 address and then transfer through the hardware channel. A published message does not mean that the input has been fully transferred. |
| AIC L1 Buffer → AIV UB | [DataCopyL1ToUB](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/DataCopyL1ToUB.md). | [asc_copy_l12ub](../../../../api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l12ub.md). | Used to provide L1 data to the Vector side. The destination UB and read/write synchronization constraints must still be satisfied. |
| AIC L0C Buffer → AIV UB | [Fixpipe](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToUB.md). | [asc_copy_l0c2ub](../../../../api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2ub.md). | Directly hands the cube output to Vector for processing. Ensure that the FIX output is complete before consumption. |

The UB-to-L1 Buffer hardware channel path of the basic API differs from the GM software simulation path: the former transfers directly under the corresponding configuration, while the latter requires Matmul registration and GM relay space. The C API directly uses the corresponding hardware interface and cannot apply the registration requirements of the basic API compatibility path. Data transfer capability also does not mean that each call requires the user to write an additional SSBuffer message. Whether to exchange addresses and parameters depends on the operator design.

The corresponding actual data streams are as follows:

```text
Control message: AIV Scalar ↔ SSBuffer ↔ AIC Scalar
Vector as Cube input: UB → L1 Buffer → L0A/L0B Buffer → Cube computation
Cube as Vector input: Cube computation → L0C Buffer → UB → Vector computation
```

### How Matmul Uses SSBuffer

In the normal Matmul hybrid computation mode, the AIV-side client hands over call information to the AIC-side service. When the SSBuffer path is used, the framework manages the following information in SSBuffer:

- **Call message**: parameters such as the function number, instance number, input/output addresses, and shapes, which are passed to AIC through the message queue.
- **Tiling information**: AIV passes cube tiling parameters through a dedicated area. AIC obtains a copy when processing the initialization message, and then allows the area to be reused.
- **L1 Buffer address**: AIC publishes the L1 addresses of resources such as A, B, bias, and scale for applicable input types, based on which AIV prepares the data transfer from UB to L1 Buffer.

The framework also coordinates L1 space reuse, input readiness, and cube output completion through inter-core events. Message area release, Tiling area reusability, L1 address availability, and output computation completion are different conditions. Developers should use the results according to the synchronous/asynchronous constraints of APIs such as [SetTensorA](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorA.md), [GetTensorC](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetTensorC.md), and [WaitIterateAll](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/WaitIterateAll.md), and must not replace the completion conditions of these APIs with a shared flag.

The supporting transfer service also passes descriptions such as the address, block length, and stride required for GM-to-L1 Buffer transfer, while the actual cube content is still transferred on the data path.

When Matmul uses SSBuffer communication, it also supports capabilities such as UB/TSCM input, MX cube computation, and scale input. For example, [SetTensorScaleA](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorScaleA.md) and [SetTensorScaleB](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorScaleB.md) need to match configurations that support these capabilities. TSCM indicates the L1 Buffer location used by the relevant queue, not the cube storage area in SSBuffer. When setting UB/TSCM, AB sharing, batch iteration, or dual-main mode, you must also comply with the support scope of the corresponding Matmul configuration; you cannot assume that all combinations are applicable once SSBuffer is enabled. The current SSBuffer communication implementation uses 8 command positions per AIV, and the maximum number of supported Matmul objects is 4; the number of command positions is not equal to the number of objects. TSCM queues sourced from UB also consume inter-core synchronization resources. The current framework checks that the sum of the number of TSCM Buffers used and the number of Matmul objects does not exceed 10, and they cannot be configured independently at their respective maximum values.

You can refer to the following samples to understand different usages:

- [MatmulLeakyRelu](../../../../../../examples/01_simd_cpp_api/00_introduction/03_fusion_operation/matmul_leakyrelu_advanced_api): cube computation fused with Vector activation. The 3510 build configuration enables SSBuffer communication through `ENABLE_CV_COMM_VIA_SSBUF=true`.
- [matmul_vecout](../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_vecout): Uses VECOUT on the Vector side as the cube input.
- [matmul_mx](../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_mx) and [matmul_mx_scale_cache](../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_mx_scale_cache): MX computation and scale-related scenarios.
- [matmul_mx_ub_tscm_nz](../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_mx_ub_tscm_nz): MX cube input in UB, TSCM, and NZ formats.

## Resource Management and Verification

The custom message area must not overlap with the SSBuffer area of the Matmul framework or other communication schemes. In addition to call messages, Matmul also saves Tiling and L1 address information. You cannot treat other space as available simply by checking the command queue size. Initialization of an existing communication area must not overwrite messages that are still in use.

Synchronization IDs also need to be planned in a unified manner. Matmul, the TSCM queue, and [SyncAll](../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/SyncAll.md) may occupy inter-core synchronization resources. IDs such as 0 and 1 in standalone examples do not mean that they are always available in a fused operator. You should check resource usage according to the corresponding APIs and configurations, and must not change only the message offset while ignoring synchronization ID conflicts.

The SSBuffer message space, system workspace, and operator GM workspace serve different purposes. Enabling SSBuffer can reduce the corresponding GM message relay, but it does not mean that all Matmul calls or operators no longer require a workspace. You still need to comply with the parameters and resource management constraints of the public APIs.

When verifying communication, you should cover first read/write, communication by two AIVs separately, queue wraparound, and repeated startup, and check the sender, message sequence number, data content, and total count. When persistent waiting occurs, first check the synchronization mode, the ID mapping on both sides, the notification and wait counts, and the exit conditions. When messages are correct but the computation result is abnormal, then check the completion conditions of the actual transfer pipeline and the buffer reuse timing. CPU debugging can assist in verifying the layout and part of the logic, but it cannot replace inter-core synchronization and performance verification in the NPU environment.
