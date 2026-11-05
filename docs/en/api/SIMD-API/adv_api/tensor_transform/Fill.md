# Fill

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:09:14.686Z pushedAt=2026-10-27T11:54:46.671Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/tensor_transform/Fill_res.md#id1 -->

## Description

Initializes the data in Global Memory to a specified value. This API can be used to clear the workspace address or output data to zero.

## Prototype

```
template <typename T>
__aicore__ inline void Fill(GlobalTensor<T>& gmWorkspaceAddr, const uint64_t size, const T value)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. For the data types supported by different models, see [Supported Data Types](#li194619420173). |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gmWorkspaceAddr | Input | **gmWorkspaceAddr** is the user-defined global space to be initialized, of the **GlobalTensor** type. For the definition of the **GlobalTensor** data structure, see [GlobalTensor](../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). |
| size | Input | Size of the space to be initialized, in number of elements. |
| value | Input | Value to be initialized, whose data type is consistent with **gmWorkspaceAddr**. |

## Return Value

None

## Constraints

-   When this API is called on a single core, if subsequent operations involve the use of the Unified Buffer (UB), set the synchronization for the MTE2 pipeline to wait for the MTE3 pipeline ([MTE3\_MTE2](../../basic_api/sync_control/intra_core_sync/SetFlag_WaitFlag_ISASI.md#section622mcpsimp)) after calling this API.
-   When this API is called multiple times consecutively on a single core with inconsistent initialization values, set the synchronization for the V pipeline to wait for the MTE3 pipeline ([MTE3\_V](../../basic_api/sync_control/intra_core_sync/SetFlag_WaitFlag_ISASI.md#section622mcpsimp)) between the calls. Because this API uses the same UB block internally as the transfer space for value initialization, if pipeline synchronization is not performed between consecutive calls, a later write operation may overwrite data that has not been fully written by a previous operation, causing the result of the previous Global Memory initialization to be unexpected.
-   When multiple cores call this API to initialize Global Memory, the initialization of Global Memory by all cores may not finish at the same time, and data dependency issues such as read-after-write, write-after-read, and write-after-write may exist between cores. In this scenario, call [SyncAll](../../basic_api/sync_control/inter_core_sync/SyncAll.md) after this API to ensure correct inter-core synchronization.
-   This API can only be used before the program memory allocation [InitBuffer](../../basic_api/resource_management/TPipe/InitBuffer.md) API.
-   Supported data types<a id="li194619420173"></a>

    <!-- npu="950" id7 -->
    For Ascend 950PR&950DT products, the supported data types are: uint8\_t, int8\_t, uint16\_t, int16\_t, bfloat16\_t, half, uint32\_t, int32\_t, float, uint64\_t, int64\_t.
    <!-- end id7 -->
    <!-- npu="A3" id8 -->
    For Atlas A3 products, the supported data types are: uint16\_t, int16\_t, half, uint32\_t, int32\_t, float.
    <!-- end id8 -->
    <!-- npu="910b" id9 -->
    For Atlas A2 products, the supported data types are: uint16\_t, int16\_t, half, uint32\_t, int32\_t, float.
    <!-- end id9 -->
    <!-- npu="310p" id10 -->
    For the Atlas inference products AI Core, the supported data types are: uint16\_t, int16\_t, half, uint32\_t, int32\_t, float.
    <!-- end id10 -->

## Examples

This example uses 8 cores, and each core initializes 65536 elements on zGm with the value of the current blockIdx.

```
// Number of elements to be initialized.
constexpr int32_t INIT_SIZE = 65536;

// Set the start address of the source operand on Global Memory to z+INIT_SIZE*blockIdx, with an external storage size of INIT_SIZE floats.
zGm.SetGlobalBuffer((__gm__ float*)z + INIT_SIZE * AscendC::GetBlockIdx(), INIT_SIZE);
// Initialize the value of the source operand zGm, whose start address is z+INIT_SIZE*blockIdx and whose size is INIT_SIZE floats, to the current blockIdx of each core.
AscendC::Fill(zGm, INIT_SIZE, (float)(AscendC::GetBlockIdx()));
```

The result example is as follows:

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
