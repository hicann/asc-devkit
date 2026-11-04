# TQueSync

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T19:08:23.229Z pushedAt=2026-10-26T07:41:44.834Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/intra_core_sync/TQueSync_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_block_sync_intf.h"`.

The **TQueSync** class is used for synchronization between multiple intra-core pipelines. The template parameters of the **TQueSync** class specify the source pipeline and the destination pipeline, where the destination pipeline waits for the source pipeline, thereby resolving data access dependencies between multiple pipelines. The **SetFlag**/**WaitFlag** member functions of the **TQueSync** class are used to perform the specific synchronization operations. These are synchronization instructions between different pipelines within the same core, and they must be inserted between instructions of different pipelines that have data dependencies. Their functions are described as follows:

- **SetFlag**: After all read and write operations of the preceding instructions are completed, the current instruction starts to execute and sets the corresponding flag in the hardware to 1.
- **WaitFlag**: When this instruction is executed, if the corresponding flag is found to be 0, the subsequent instructions of this queue are blocked; if the corresponding flag is found to be 1, the corresponding flag is set to 0 and the subsequent instructions start to execute.

> [!NOTE]Note
> This API and the [SetFlag/WaitFlag(ISASI)](SetFlag_WaitFlag_ISASI.md) API can achieve the same functionality. The difference between the two is that the **TQueSync** class API guarantees cross-hardware-version compatibility, whereas the APIs in [SetFlag/WaitFlag(ISASI)](SetFlag_WaitFlag_ISASI.md) are marked as the ISASI category and cannot guarantee cross-hardware-version compatibility.

## Prototype

```cpp
using TEventID = int8_t;

template<pipe_t src, pipe_t dst>
class TQueSync {
public:
    __aicore__ inline void SetFlag(TEventID id)
    __aicore__ inline void WaitFlag(TEventID id)
};
```

## Parameters

**Table 1** Class template parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src | Input | Source pipeline. For supported pipelines, see [Hardware Pipeline Type](intra_core_sync_overview.md#hardware-pipeline-type). |
| dst | Input | Destination pipeline. For supported pipelines, see [Hardware Pipeline Type](intra_core_sync_overview.md#hardware-pipeline-type). |

**Table 2** Member function parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| id | Input | Event ID, whose data type is **TEventID**. It is recommended to obtain it through [AllocEventID](../../resource_management/TPipe/AllocEventID.md) or [FetchEventID](../../resource_management/TPipe/FetchEventID.md).<br>For the value range of **id** on different products, see [id value range description](#id-value-range-description). |

### **id** value range description

<!-- npu="950" id8 -->
- Ascend 950PR&950DT products: value range is 0-7.
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 products: value range is 0-7.
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 products: value range is 0-7.
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: value range is 0-7.
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: value range is 0-7.
<!-- end id12 -->
<!-- npu="910" id13 -->
- Atlas training products: value range is 0-3.
<!-- end id13 -->

## Return Value

None

## Constraints

- `src` and `dst` must be supported hardware pipelines, and they cannot be the same.

- **SetFlag** only sets the hardware flag and does not block the next instruction in the source pipeline.

- **SetFlag** and **WaitFlag** must be used in pairs, and the template parameters `src` and `dst` and the input parameter `id` must be exactly the same. If they do not match, a timeout issue occurs.

- When using the [TPipe-TQue framework programming paradigm](../../../../../guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_paradigm.md), `id` must be obtained through [AllocEventID](../../resource_management/TPipe/AllocEventID.md) or [FetchEventID](../../resource_management/TPipe/FetchEventID.md).

- When using the [static Tensor programming paradigm](../../../../../guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md), the event type and `id` are managed by the developer. It is recommended to use event IDs 0 to 5. Event ID 6 is reserved for internal system planning (currently unused), and event ID 7 is used for the **automatic synchronization** feature in TPipe programming. Direct use of event IDs 6 and 7 is currently not recommended.

- With the same source pipeline, destination pipeline, and `id`, consecutive use of **SetFlag** causes undefined behavior. In this case, executing PipeBarrier<PIPE\_ALL> again causes a hang:

    ```cpp
    AscendC::TQueSync<PIPE_M, PIPE_MTE1> sync;
    sync.SetFlag(0);  // Call SetFlag for the first time.
    sync.SetFlag(0);  // Call SetFlag for the second time (consecutive use with the same source pipeline, destination pipeline, and id causes undefined behavior).
    ...
    AscendC::PipeBarrier<PIPE_ALL>();  // Trigger a hang.
    ...
    sync.WaitFlag(0);
    sync.WaitFlag(0);
    ```

## Example

If **DataCopy** needs to wait for **SetValue** to complete before it can execute, synchronization from PIPE\_S to PIPE\_MTE3 must be inserted.

```cpp
AscendC::GlobalTensor<half> dstGlobal;
AscendC::LocalTensor<half> dstLocal;
dstLocal.SetValue(0, 0);
AscendC::TQueSync<PIPE_S, PIPE_MTE3> sync;
sync.SetFlag(0);
sync.WaitFlag(0);
AscendC::DataCopy(dstGlobal, dstLocal, dataSize);
```
