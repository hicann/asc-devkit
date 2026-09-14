# IterateAll

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:38:57.924Z pushedAt=2026-09-12T09:55:18.110Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference product: Supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

Calling **IterateAll** once computes the C matrix of **singleCoreM** \* **singleCoreN** size. The iteration order can be adjusted through the **tiling** parameter **iterateOrder**.

## Prototype

```
template <bool sync = true> __aicore__ inline void IterateAll(const GlobalTensor<DstT>& gm, uint8_t enAtomic = 0, bool enSequentialWrite = false, bool waitIterateAll = false, bool fakeMsg = false)
```

```
template <bool sync = true> __aicore__ inline void IterateAll(const LocalTensor<DstT>& ubCmatrix, uint8_t enAtomic = 0)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| sync | The process of obtaining the C matrix is divided into synchronous and asynchronous modes:<br>Synchronous: The API waits synchronously for IterateAll execution completion.<br>Asynchronous: The API does not wait synchronously for IterateAll execution completion.<br><br>This parameter sets the synchronous or asynchronous mode: set it to true for synchronous mode and false for asynchronous mode. The default value is synchronous mode.<br><br>Atlas 200I/500 A2 inference product supports only true. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gm | Output | C matrix. The data type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). For the supported data types of different models, see [Supported Data Types](#li12616155731720). |
| ubCmatrix | Output | C matrix. The data type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). The supported TPosition values are TSCM and VECIN. When outputting to the VECIN tensor, use the [Iterate](Iterate.md) API. For the supported data types of different models, see [Supported Data Types](#li12616155731720). |
| enAtomic | Input | Whether to enable the Atomic operation. The default value is 0.<br><br>Parameter values:<br><br>0: Disable the Atomic operation.<br><br>1: Enable the AtomicAdd accumulation operation.<br><br>2: Enable the AtomicMax maximum operation.<br><br>3: Enable the AtomicMin minimum operation.<br><br>For AI Core of Atlas inference products, the Atomic operation can be enabled only when the output location is GM.<br><br>For Atlas 200I/500 A2 inference product, the Atomic operation can be enabled only when the output location is GM. |
| enSequentialWrite | Input | Whether to enable the sequential write mode ([Sequential Write](GetTensorC.md#sequential-write), writing to [baseM, baseN]; [Non-sequential Write](GetTensorC.md#non-sequential-write), writing to the corresponding position in [singleCoreM, singleCoreN]). The default value is false (non-sequential write mode).<br><br>Atlas 200I/500 A2 inference product does not support this parameter. |
| waitIterateAll | Input | Used only in the asynchronous scenario. Whether to wait for IterateAll execution completion through the [WaitIterateAll](WaitIterateAll.md) API.<br><br>true: Wait for IterateAll execution completion through the WaitIterateAll API.<br><br>false: Do not wait for IterateAll execution completion through the WaitIterateAll API. You handle the process of waiting for IterateAll execution completion yourself. |
| fakeMsg | Input | Used only in the IBShare scenario (where the [doIBShareNorm](MatmulConfig.md#matmulconfig-params) switch is enabled in the template parameter) and the IntraBlockPartSum scenario (where the [intraBlockPartSum](MatmulConfig.md#matmulconfig-params) switch is enabled in the template parameter).<br>IBShare scenario:<br>In this scenario, the same A matrix or B matrix data on L1 is reused, and the number of IterateAll calls by AIV cores must match. In this case, call IterateAll and set fakeMsg to true, which does not perform the actual computation but only ensures that IterateAll calls appear in pairs. The default value is false, which means the actual computation is performed.<br><br>IntraBlockPartSum scenario:<br>Used for fusing Vector computation and Cube computation in the separated mode, so that the Matmul computation results of multiple AIV cores (matrix slices of baseM * baseN size) are accumulated on the L0C Buffer. The default value is false, which means the Matmul computation results of each AIV core are accumulated on the L0C Buffer. |

## Return Value

None

## Constraints

-   The address space size of the passed C matrix must be no smaller than **singleCoreM** \* **singleCoreN** elements.
-   Supported data types<a id="li12616155731720"></a>

    Ascend 950PR/Ascend 950DT: The supported data types are half, float, bfloat16_t, int32_t, int8_t, fp8_e4m3fn_t, and hifloat8_t.

    Atlas A3 training products/Atlas A3 inference products: The supported data types are half, float, bfloat16_t, int32_t, and int8_t.

    Atlas A2 training products/Atlas A2 inference products: The supported data types are half, float, bfloat16_t, int32_t, and int8_t.

    AI Core of Atlas inference products: The prototype containing the **ubCmatrix** parameter is not supported. For the prototype containing the **gm** parameter, the supported data types are half, float, int8_t, and int32_t.

    Atlas 200I/500 A2 inference products: The supported data types are half, float, bfloat16_t, and int32_t.

## Examples

The following is an example of calling the **IterateAll** API.

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.IterateAll(gm_c);    // Compute.
mm.End();
```
