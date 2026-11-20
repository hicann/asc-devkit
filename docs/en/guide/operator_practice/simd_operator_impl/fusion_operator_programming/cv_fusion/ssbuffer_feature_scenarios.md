# Inter-Core Communication Based on SSBuffer

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-09T10:29:00.712Z pushedAt=2026-11-19T06:32:19.729Z nl=2 -->

This section describes the basics of SSBuffer, communication rules, and operator implementation methods used in CV fusion operators.

>[!NOTE] Note
>
>Before reading this section, understand [CV fusion operators](./basics.md) and [address space qualifiers](../../../../programming_guide/language_extension/simd_builtin_keywords.md#address-space-qualifiers).
>
>The product support for this section is as follows:
><!-- npu="950" id1 -->
>- Ascend 950PR&950DT products: supported
><!-- end id1 -->
><!-- npu="A3" id2 -->
>- Atlas A3 products: not supported
><!-- end id2 -->
><!-- npu="910b" id3 -->
>- Atlas A2 products: not supported
><!-- end id3 -->
><!-- npu="310b" id4 -->
>- Atlas 200I/500 A2 inference products: not supported
><!-- end id4 -->
><!-- npu="310p" id5 -->
>- Atlas inference products AI Core: not supported
><!-- end id5 -->
><!-- npu="310p" id6 -->
>- Atlas inference products Vector Core: not supported
><!-- end id6 -->
><!-- npu="910" id7 -->
>- Atlas training products: not supported
><!-- end id7 -->

The matching executable samples are as follows:

- [AIV/AIC ring message queue communication C API sample](../../../../../../../examples/02_simd_c_api/05_best_practices/03_fusion_compute/ssbuf_aiv_aic_comm): uses the C API to implement two AIV-to-AIC message queues. This is also the complete sample used in this section.
- [AIV/AIC ring message queue communication](../../../../../../../examples/01_simd_cpp_api/05_best_practices/03_fusion_compute/ssbuf_aiv_aic_comm): uses the basic API to implement two AIV-to-AIC message queues.

The following uses the SSBuffer-based AIV/AIC communication operator as an example to describe the design and implementation of explicit SSBuffer communication. This kernel function uses the [`__mix__(1, 2)` execution space qualifier](../../../../programming_guide/language_extension/simd_builtin_keywords.md#section1074418132518) to indicate the ratio of Cube cores to Vector cores started by the kernel. Each AIV sends 12 messages through its own ring message queue. The AIC reads the messages, verifies the function number, sender, sequence number, and checksum, and writes the message fields and verification results to GM. The host verifies the `valid`, `sender`, `sequence`, `value`, and `checksum` fields of the 24 results one by one.

The operator design process consists of five parts: **operator analysis, data flow analysis, SSBuffer space planning, communication rule design, and operator implementation**. For the complete code, see [SSBuffer AIV/AIC communication C API sample](../../../../../../../examples/02_simd_c_api/05_best_practices/03_fusion_compute/ssbuf_aiv_aic_comm).

## Operator Analysis

This sample verifies message communication from AIV to AIC. The AIVs generate messages on the device side. The AIC parses the received messages, verifies the function number, sender, sequence number, and checksum, and organizes the message fields and verification results into `CommResult` and writes them to GM. The operator processing logic is as follows:

1. The AIC clears the two message queues and notifies AIV0 and AIV1 respectively to start sending.
2. AIV0 and AIV1 each generate 12 messages. Each message contains the sender ID, message sequence number, `value` field, and checksum.
3. After writing all fields of a message, the AIV sends a message readiness notification, indicating that the message in the corresponding SSBuffer can be read.
4. After receiving the message readiness notification, the AIC reads and verifies the message, clears the message header, and returns an acknowledgment (ACK) notification. This notification indicates that the AIC has read the message and the corresponding queue slot can be reused by the AIV. For details about the communication rules, see [Publish and Acknowledge Order](#publish-and-acknowledge-order) and [Queue Wraparound](#queue-wraparound).
5. The AIC receives 24 messages in total and writes them to GM. The host side reads back the results and verifies the `valid`, `sender`, `sequence`, `value`, and `checksum` fields of each `CommResult` one by one to confirm that no message is lost, overwritten, or placed in the wrong message queue.

The kernel function is named `c_api_ssbuf_aiv_aic_comm`, and its parameter is `output`. `output` points to the result array in GM, which contains 24 `CommResult` entries. Each result occupies 32B, and its fields are as follows. Note: If the message scale of the actual operator varies with the input, the related computation parameters should be designed and supplemented on the host side and then passed to the AIC and AIV, to avoid inconsistent message counts or exit conditions on the two sides.

**Table 1** `CommResult` field description

| Field | Data Type | Description |
| --- | --- | --- |
| `sender` | `uint32_t` | ID of the AIV that sends the message. AIV0 is 0, and AIV1 is 1. |
| `valid` | `uint32_t` | Message verification result. A value of **1** indicates that the function number, sender, sequence number, and checksum all meet expectations. |
| `sequence` | `uint64_t` | Message sequence number. Each AIV increments from 0 to 11. |
| `value` | `uint64_t` | Sample data carried by the message. |
| `checksum` | `uint64_t` | Checksum generated based on the sender, sequence number, and message data. |

The sample generates the message data and checksum using the following two formulas:

$$
value = 1000 + sender \times 100 + sequence
$$

$$
checksum = CHECKSUM\_MAGIC \oplus sender \oplus sequence \oplus value
$$

Here, **$\oplus$** indicates bitwise XOR. `CHECKSUM_MAGIC` is fixed to `0x5353425546434D4D`, whose bytes correspond to the ASCII string `SSBUFCMM`. It is a sample-defined, easily recognizable communication checksum constant, not a hardware-specified value. The message data refers to the `value` field, whose value is generated jointly by the sender number and the message sequence number. The host side uses the same formula to compute the expected value and compares it, thereby checking whether the message is written to the result as expected by the sender and sequence number. The checksum is used to detect whether the message content is complete.

## Data Flow Analysis

The operator involves two data flows: the synchronization notification at task startup, and message sending and receiving during task running.

1. Startup synchronization: After AIC clears the SSBuffer, it notifies AIV0 and AIV1 respectively through inter-core synchronization. This notification indicates that the message queue is ready for use.
2. Message sending and receiving: After AIV finishes writing the message, it sends a message readiness notification. After AIC receives the notification, it reads and verifies the message, writes the result to GM, clears the message header, and then returns an acknowledgment (ACK) notification.

The overall data flow is as follows.

```text
                       Inter-core synchronization: startup notification
                AIC --------------------------> AIV0, AIV1
                 ^                                  |
                 |               Message readiness notification        | Write message
                 +----------------------------------+
                 |                                  v
Host <---- GM <---- AIC <----------------------- SSBuffer
                     |
                     +-- Clear message header and return ACK notification ----> AIV0, AIV1
```

The startup notification ensures that AIV does not access a message queue that has not been initialized. The message readiness notification ensures that AIC reads the message only after AIV finishes writing it. The acknowledgment (ACK) notification ensures that AIV reuses the same message space only after AIC reads the message and releases the slot. For the specific sequence, see [Publish and Acknowledge Order](#publish-and-acknowledge-order). For how queue slots are reused, see [Queue Wraparound](#queue-wraparound). Therefore, all three stages involve inter-core synchronization.

## SSBuffer Space Planning

### Message Structure

Each message occupies 128B and contains the following parameters:

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

**Table 2** `CommMessage` field description

| Field | Description |
| --- | --- |
| `head` | Message header, which stores the function number, the `valid` bit, and the sender ID. The AIC uses it to determine whether the message can be read. |
| `reserved0` | Reserved field. When the AIC returns an ACK, it is cleared together with `head`. |
| `sequence` | Message sequence number, used to confirm that the message read after queue wraparound is the currently expected one. |
| `value` | Message content. In an actual operator, it can be replaced with control information such as the data readiness status, address, or length. |
| `checksum` | Message checksum. |
| `reserved` | Reserved space, used to pad the message to 128B. |

### Queue Layout

Each AIV exclusively owns a queue with a depth of 8, and the two message queues occupy 2 KB in total. A queue depth of 8 means that each queue has only 8 physical slots, not that each AIV sends only 8 messages. In this sample, each AIV sends 12 messages: the first 8 messages use the 8 slots respectively, and the 9th to 12th messages reuse the first 4 slots in sequence after receiving the corresponding acknowledgment (ACK) notifications. The two AIVs ultimately generate 24 `CommResult` objects in total. The remaining 1 KB is not used by this operator.

```text
SSBuffer [0, 1024)       AIV0 message queue: 8 messages, 128B each
SSBuffer [1024, 2048)    AIV1 message queue: 8 messages, 128B each
SSBuffer [2048, 3072)    Not used by this operator
```

3 KB is the hardware capacity of SSBuffer. The 2 KB workspace, queue depth of 8, and 128B per message are protocol designs of this sample, not the fixed format of SSBuffer. When adjusting these parameters, you need to recheck address alignment, whether the message queues overlap, and whether the total occupied space exceeds 3 KB.

After the two message queues are separated, AIV0 and AIV1 write only to their own regions and do not need to contend for the same production position. AIC reads by message queue and can also directly determine which AIV a message comes from.

## Communication Rule Design

### Message State

The sample uses a 32-bit `head` to indicate the message state: the upper 16 bits store the function number `0x5A01`, bit 15 is the `valid` bit, and the lower 8 bits store the sender number.

```cpp
constexpr uint32_t VALID_MASK = 0x00008000U;
constexpr uint32_t FUNCTION_ID = 0x5A01U;

__aicore__ inline uint32_t make_head(uint32_t sender) // __aicore__ indicates that this function runs on the AI Core.
{
    return (FUNCTION_ID << 16) | VALID_MASK | (sender & 0xFFU);
}
```

A `valid` value of **0** indicates that there is no pending message at this position; a `valid` value of **1** indicates that the AIV has published a message. After processing, the AIC clears the message header to restore the message state to **0**. The sample also uses acknowledgment (ACK) notifications to control space reuse, so `valid` is mainly used to check whether the message state conforms to the protocol.

### Inter-Core Synchronization

The sample uses [`asc_sync_intra_arrive`](../../../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_intra_arrive.md) and [`asc_sync_intra_wait`](../../../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_intra_wait.md). These two APIs are for inter-core synchronization, corresponding to mode 4, and can be used within the same AI Core to allow the AIC and a single AIV to send notifications to each other.

### Publish and Acknowledge Order

Both parties must follow the order below. The inter-core synchronization APIs are responsible for notifications and also establish the ordering relationship for SSBuffer access on `PIPE_S`.

| AIV Publishes Messages | AIC Consumes Messages |
| --- | --- |
| Before queue wraparound, wait for the corresponding acknowledgment (ACK) notification. | Wait for the message readiness notification from the corresponding AIV. |
| Write `sequence`, `value`, `checksum`, and `head`. | Read the message header and message body, and write the check result to GM. |
| Call `asc_sync_intra_arrive(PIPE_S, MESSAGE_READY_SYNC_ID)`. | Clear the message header. |
| Continue using the new message space, or wait for the remaining acknowledgment notifications after sending is complete. | Call `asc_sync_intra_arrive(PIPE_S, MESSAGE_ACK_SYNC_ID)` to return the acknowledgment notification. |

### Queue Wraparound

A ring message queue uses a fixed-size storage space to repeatedly carry consecutive messages. When the message sequence number exceeds the queue depth, a new message is mapped back to the beginning of the queue. This process is called queue wraparound. Wraparound only changes the position used by a message and does not change the message sequence number.

The sample uses `sequence & (QUEUE_DEPTH - 1)` to compute the position of a message in the queue. `QUEUE_DEPTH` is 8, which is a power of 2, so the bitwise AND operation is equivalent to `sequence % 8`.

| Message Sequence Number | Queue Position | Usage Condition |
| --- | --- | --- |
| 0-7 | 0-7 | First use; no need to wait for a historical ACK. |
| 8-11 | 0-3 | Reuse the positions used by sequence numbers 0-3 respectively; the corresponding ACK must be received before writing. |

For example, both sequence number 0 and sequence number 8 use position 0. Before AIV sends sequence number 8, it first waits for an ACK. This ACK indicates that AIC has finished reading sequence number 0 and cleared the message header. In this way, the limited space of eight messages can carry 12 or more consecutive messages without overwriting content that has not yet been consumed.

The number of messages that a producer has published but for which it has not yet received an ACK must not exceed the queue depth. This is the basic condition for a ring message queue to wraparound correctly.

## Operator Implementation

### Obtaining the SSBuffer Workspace

On the `dav-3510` NPU, address zero of the `__ssbuf__` address space indicates the SSBuffer base address. The sample converts this address into a workspace pointer and retains the `__ssbuf__` qualifier in subsequent accesses.

```cpp
__aicore__ inline __ssbuf__ SsbufWorkspace* get_workspace() // __aicore__ indicates that the function runs on the AI Core.
{
    return reinterpret_cast<__ssbuf__ SsbufWorkspace*>(0); // __ssbuf__ indicates that the pointer points to the SSBuffer.
}
```

### Initializing Message Queues

The initial content in SSBuffer is not guaranteed to be zero. At the kernel function entry, [`asc_init`](../../../../../api/SIMD-API/c_api/utils/sys_init/asc_init.md) is first called to restore the NPU control state, but this API does not clear SSBuffer. Therefore, the AIC still needs to clear all 16 message headers.

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

### Sending Messages by AIV

The AIV obtains its number (subblock ID) in the current execution combination through [`asc_get_sub_block_id`](../../../../../api/SIMD-API/c_api/utils/sys_var/asc_get_sub_block_id.md) and selects its own message queue. The sending process is as follows:

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

The first eight messages can be published consecutively. Starting from the ninth message, the AIV waits for an ACK before each write to ensure that the position to be reused has been released by the AIC. After all 12 messages are published, the first four ACKs have already been consumed during wraparound, and the AIV waits for the remaining eight ACKs before exiting.

### Receiving Messages by AIC

AIC reads the two message queues by sender number and message sequence number. After receiving a message readiness notification, it reads the message and verifies the function number, sender, sequence number, and checksum, writes the message fields and the verification result to GM, and finally clears the message header and returns an acknowledge notification.

```cpp
// __aicore__ indicates that the function runs on the AI Core, and __ssbuf__ and __gm__ indicate the SSBuffer and GM address spaces, respectively.
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

In the current sample, AIC first receives 12 messages from AIV0 and then receives messages from AIV1. The two message queues do not overlap. AIV1 can publish at most 8 messages first and then wait for ACKs, which does not affect AIV0 from continuing to send. In actual services, if the response time of the two AIVs is more important, you can receive the two message queues alternately, but you must still follow the same synchronization ID mapping and ACK rules.

### Kernel Entry

The kernel uses the `__mix__(1, 2)` execution space qualifier, indicating that each kernel function execution combination contains one AIC and two AIVs. `ASC_IS_AIC` and `ASC_IS_AIV` isolate the code on the two sides.

```cpp
// __global__ indicates the kernel entry, __mix__(1, 2) indicates starting one AIC and two AIVs, and __gm__ indicates that the parameter points to GM.
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

## Compilation and Execution

This sample supports both NPU and simulation run modes, and the NPU architecture is fixed to `dav-3510`. Taking the NPU mode as an example, the compilation and running commands are as follows:

```bash
source ${install_path}/cann/set_env.sh
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCMAKE_ASC_RUN_MODE=npu ..
make -j
./demo
```

## Verification and Usage Constraints

The sample verifies that the communication process is correct through the following checks:

- Each of the two AIVs sends 12 messages, and the AIC receives 24 messages in total. The host verifies each message one by one and all checks pass.
- Messages 9 through 12 reuse the first four positions of the queue, verifying that queue wraparound does not overwrite messages that have not yet been consumed.
- The message sequence number and checksum are checked together, making it easy to detect stale messages, messages entering the wrong queue, and incomplete content.

When using this method to implement other operators, the following constraints must also be observed:

- The AIC and AIV must access the SSBuffer through a combined kernel function execution that uses the same `__mix__(1, 2)` execution space qualifier.
- A unique initializer must clear the message state each time a task is started.
- A custom message layout cannot exceed 3 KB, and cannot occupy space that overlaps with KFC, Matmul advanced APIs, or other SSBuffer communication schemes.
- Cross-core shared fields must use `volatile`, and message readiness and space reuse must use paired inter-core synchronization notifications.
- The queue depth and the send/receive pace must ensure that the synchronization counter in inter-core synchronization does not exceed the upper limit of the instruction constraint.
- If the number of messages, queue ownership, synchronization ID mapping, or exit conditions are inconsistent, the task may wait indefinitely. Therefore, these items should be defined uniformly by the user.
