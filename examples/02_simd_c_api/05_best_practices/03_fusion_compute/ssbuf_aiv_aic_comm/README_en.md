# SSBuffer AIV/AIC Communication C API Sample

## Overview

This sample shows how to use SSBuffer with the Ascend C API to exchange messages between AIV and AIC in one `__mix__(1, 2)` task. The task contains one AIC and two AIVs. Each AIV sends messages through its own ring queue, AIC receives and checks the messages, and the host validates the results written to GM.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

This sample uses the `dav-3510` architecture and supports only NPU and NPU simulation modes.

## Directory Structure

```
├── ssbuf_aiv_aic_comm
│   ├── CMakeLists.txt                 // Build project file
│   ├── ssbuf_aiv_aic_comm.asc         // Ascend C C API implementation, kernel invocation, and result validation
│   ├── README.md                      // Chinese sample documentation
│   └── README_en.md                   // English sample documentation
```

## Example Description

### Example Function

- AIC initializes the two message queues in SSBuffer.
- AIC uses mode 4 inter-core synchronization to notify the two AIVs that sending can start.
- Each AIV sends 12 messages. AIC receives 24 messages in total according to the sender and sequence number.
- AIC checks the message header, sender ID, sequence number, and checksum, then writes the results to GM.
- The host reads the results from GM and verifies that messages are not lost, overwritten, or sent to the wrong queue.

### Example Specifications

<table>
<caption>Table 1: Example Input/Output Specifications</caption>
<tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">SSBuffer AIV/AIC Communication C API</td></tr>
<tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">None</td><td align="center">-</td><td align="center">-</td><td align="center">-</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">output</td><td align="center">[24]</td><td align="center">CommResult</td><td align="center">-</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">c_api_ssbuf_aiv_aic_comm</td></tr>
<tr><td rowspan="1" align="center">Number of Cores</td><td colspan="4" align="center">1 AIC and 2 AIVs</td></tr>
</table>

### SSBuffer Layout

- SSBuffer has a total capacity of 3 KB. This sample uses the first 2 KB.
- AIV0 uses `[0, 1024)`, which contains eight 128-byte messages.
- AIV1 uses `[1024, 2048)`, which contains eight 128-byte messages.
- `[2048, 3072)` is not used by this sample.
- The two queues do not overlap, and each AIV writes only to its own queue.

Each message is represented by `CommMessage` and contains a message header, sequence number, message value, and checksum. `alignas(128)` keeps messages 128-byte aligned, and shared message fields are declared `volatile`.

## Example Implementation

### 1. Obtain the SSBuffer Workspace

The C API does not provide an interface named `GetSsbufBaseAddr`. On a `dav-3510` NPU, address zero in the `__ssbuf__` address space represents the SSBuffer base address. The sample converts it to a workspace pointer:

```cpp
__aicore__ inline __ssbuf__ SsbufWorkspace* get_workspace()
{
    return reinterpret_cast<__ssbuf__ SsbufWorkspace*>(0);
}
```

This zero address belongs to the `__ssbuf__` address space and is not a regular GM null address. The workspace pointer and all message pointers obtained from it must retain the `__ssbuf__` qualifier. The current compiler does not support directly declaring an array with the `__ssbuf__` qualifier, so the sample declares the two-dimensional message queue as a member of `SsbufWorkspace`.

### 2. Initialize the Message Queues on AIC

The initial contents of SSBuffer are not guaranteed to be zero, and `asc_init` does not clear SSBuffer. After starting, AIC clears all message headers in both queues and sends a start notification to each AIV:

```cpp
init_queues(workspace);
asc_sync_intra_arrive(PIPE_S, get_aic_sync_id(0, START_SYNC_ID));
asc_sync_intra_arrive(PIPE_S, get_aic_sync_id(1, START_SYNC_ID));
```

The start notifications use mode 4 inter-core synchronization. Each AIV waits for its `START_SYNC_ID` notification before accessing the message queue.

### 3. Send Messages from AIV

An AIV calls `asc_get_sub_block_id()` to obtain its Vector core ID and selects the corresponding queue. The queue position for message `sequence` is calculated as follows:

```cpp
queueIndex = sequence & (QUEUE_DEPTH - 1);
```

The queue depth is 8, which is a power of two, so the bitwise AND is equivalent to taking the remainder after division by 8. The AIV writes `sequence`, `value`, `checksum`, and `head`, then sends a message-ready notification:

```cpp
asc_sync_intra_arrive(PIPE_S, MESSAGE_READY_SYNC_ID);
```

The notification tells AIC that the message is ready to read. When the sequence number exceeds the queue depth, queue wrap-around occurs: a new message reuses storage at the beginning of the queue. Before reusing that storage, the AIV waits for an ACK to make sure AIC has consumed the previous message.

### 4. Receive Messages on AIC

AIC reads the two queues according to the sender ID and expected sequence number. Before each read, AIC waits for the selected AIV's message-ready notification and checks the following fields:

- Whether the function ID is `0x5A01`.
- Whether the `valid` bit is set.
- Whether the sender ID matches the queue being read.
- Whether the sequence number is the expected value.
- Whether the checksum matches the sender ID, sequence number, and message value.

AIC writes the check result and message contents to `CommResult` in GM. After reading, AIC clears the message header and sends an ACK through the sender-specific synchronization ID so that the AIV can reuse the message storage:

```cpp
clear_message(message);
asc_sync_intra_arrive(PIPE_S, get_aic_sync_id(expected_sender, MESSAGE_ACK_SYNC_ID));
```

### 5. Kernel Entry and Host Invocation

The kernel uses `ASC_IS_AIC` and `ASC_IS_AIV` to select the AIC and AIV branches. All branches access the same SSBuffer within one `__mix__(1, 2)` task. The host allocates GM space for 24 `CommResult` values, launches one Mix block with the kernel invocation operator, waits for the stream to finish, and validates the results.

## Build and Run

Run the following steps in the root directory of this sample to build and run it.

- Configure environment variables

  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Build and run

  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCMAKE_ASC_RUN_MODE=npu ..
  make -j
  ./demo
  ```

  To use NPU simulation mode, set `CMAKE_ASC_RUN_MODE` to `sim`:

  ```bash
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCMAKE_ASC_RUN_MODE=sim ..
  make -j
  ./demo
  ```

  > **Notice:** Clear the CMake cache in the build directory, or create a new build directory, before switching run modes.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution or NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture, corresponding to Ascend 950PR/Ascend 950DT |

## Execution Result

After successful execution, the host validates all 24 results and the terminal prints:

```text
test pass!
```

## Usage Constraints

- AIC and both AIVs must access SSBuffer within the same `__mix__(1, 2)` task.
- The custom message layout must fit within 3 KB and must not overlap with another SSBuffer protocol.
- Because the current compiler does not support an array directly qualified with `__ssbuf__`, declare the message queue as a member of `SsbufWorkspace`.
- Shared fields require `volatile`. Message publication and queue-space reuse must use the matching mode 4 inter-core synchronization notifications.
- Keep the queue depth as a power of two when using the bitwise queue-index calculation.
