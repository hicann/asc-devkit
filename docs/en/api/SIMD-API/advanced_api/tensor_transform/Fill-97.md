# Fill

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-09-08T15:55:35.358Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Initializes data in Global Memory to a specified value. This API can be used to clear workspace addresses or output data.

## Prototype

```
template <typename T>
__aicore__ inline void Fill(GlobalTensor<T>& gmWorkspaceAddr, const uint64_t size, const T value)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. For the data types supported by different models, see [Supported Data Types](#li194619420173). |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gmWorkspaceAddr | Input | **gmWorkspaceAddr** is the user-defined global space to be initialized, of the **GlobalTensor** type. For the definition of the **GlobalTensor** data structure, see [GlobalTensor](../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). |
| size | Input | Size of the space to be initialized, in number of elements. |
| value | Input | Value to be initialized, whose data type is the same as that of **gmWorkspaceAddr**. |

## Return Value

None

## Constraints

-   When this API is called on a single core, if subsequent operations involve the use of Unified Buffer, set the MTE2 pipeline to wait for the MTE3 pipeline ([MTE3\_MTE2](../../basic_api/sync_control/intra_core_sync/SetFlag_WaitFlag_ISASI.md#section622mcpsimp)) for synchronization after calling the API.
-   When this API is called multiple times consecutively on a single core with different initialization values, set the V pipeline to wait for the MTE3 pipeline ([MTE3\_V](../../basic_api/sync_control/intra_core_sync/SetFlag_WaitFlag_ISASI.md#section622mcpsimp)) for synchronization between the calls. Because this API internally uses the same Unified Buffer as the intermediate space for value initialization, if pipeline synchronization is not performed between consecutive calls, a later write operation may overwrite data that has not been fully written by a previous operation, causing the result of the previous Global Memory initialization to be unexpected.
-   When multiple cores call this API to initialize Global Memory, the initialization of Global Memory by all cores may not finish at the same time, and data dependency issues such as read-after-write, write-after-read, and write-after-write may exist between cores. In this scenario, call [SyncAll](../../basic_api/sync_control/inter_core_sync/SyncAll.md) after this API to ensure correct multi-core synchronization.
-   This API can only be used before the program memory allocation API [InitBuffer](../../basic_api/resource_management/TPipe/InitBuffer.md) is called.
-   Supported data types<a id="li194619420173"></a>

    Ascend 950PR/Ascend 950DT, the supported data types are: uint8\_t, int8\_t, uint16\_t, int16\_t, bfloat16\_t, half, uint32\_t, int32\_t, float, uint64\_t, int64\_t.

    Atlas A3 training products/Atlas A3 inference products support the following data types: uint16\_t, int16\_t, half, uint32\_t, int32\_t, float.

    Atlas A2 training products/Atlas A2 inference products support the following data types: uint16\_t, int16\_t, half, uint32\_t, int32\_t, float.

    The AI Core of Atlas inference products supports the following data types: uint16\_t, int16\_t, half, uint32\_t, int32\_t, float.

## Examples

This example uses 8 cores, and each core initializes 65536 elements on zGm with the value of the current blockIdx.

```
// Number of elements to be initialized.
constexpr int32_t INIT_SIZE = 65536;

// Set the start address of the source operand in Global Memory to z+INIT_SIZE*blockIdx, with an external storage size of INIT_SIZE floats.
zGm.SetGlobalBuffer((__gm__ float*)z + INIT_SIZE * AscendC::GetBlockIdx(), INIT_SIZE);
// Initialize the value of the source operand zGm, whose start address is z+INIT_SIZE*blockIdx and whose size is INIT_SIZE floats, to the current blockIdx of each core.
AscendC::Fill(zGm, INIT_SIZE, (float)(AscendC::GetBlockIdx()));
```

The results are as follows:

```
// zGm data after initialization:
[0. 0. 0. ... 0. 0. 0.
 1. 1. 1. ... 1. 1. 1.
 2. 2. 2. ... 2. 2. 2.
 ...
 5. 5. 5. ... 5. 5. 5.
 6. 6. 6. ... 6. 6. 6.
 7. 7. 7. ... 7. 7. 7.]
```
