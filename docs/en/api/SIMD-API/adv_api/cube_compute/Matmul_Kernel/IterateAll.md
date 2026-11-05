# IterateAll

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:29:03.029Z pushedAt=2026-10-27T11:25:55.908Z nl=2 -->

## Applicable Products

<!-- npu="950" id3 -->
- Ascend 950PR&950DT products: Supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 products: Supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 products: Supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/IterateAll_res.md#id1 -->

## Description

Calling **IterateAll** once computes a C cube of size **singleCoreM** \* **singleCoreN**. The iteration order can be adjusted through the **tiling** parameter **iterateOrder**.

## Prototype

```
template <bool sync = true> __aicore__ inline void IterateAll(const GlobalTensor<DstT>& gm, uint8_t enAtomic = 0, bool enSequentialWrite = false, bool waitIterateAll = false, bool fakeMsg = false)
```

```
template <bool sync = true> __aicore__ inline void IterateAll(const LocalTensor<DstT>& ubCmatrix, uint8_t enAtomic = 0)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| sync | The process of obtaining the C cube has two modes, synchronous and asynchronous:<br>Synchronous: The API waits for **IterateAll** execution completion.<br>Asynchronous: The API does not wait for **IterateAll** execution completion.<br><br>Set the synchronous or asynchronous mode through this parameter: set it to **true** for synchronous mode and **false** for asynchronous mode. The default is synchronous mode. <!-- npu="310b" id14 --><br><br>Atlas 200I/500 A2 inference products support only **true**. <!-- end id14 --> |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gm | Output | C cube. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). For the data types supported by different models, see [Supported Data Types](#li12616155731720). |
| ubCmatrix | Output | C cube. The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). The supported TPosition values are TSCM and VECIN. When outputting to a VECIN Tensor, it is recommended to use the [Iterate](Iterate.md) API. For the data types supported by different models, see [Supported Data Types](#li12616155731720). |
| enAtomic | Input | Whether to enable the Atomic operation. The default value is 0.<br><br>Parameter values:<br><br>0: Disable the Atomic operation.<br><br>1: Enable the AtomicAdd accumulation operation.<br><br>2: Enable the AtomicMax maximum operation.<br><br>3: Enable the AtomicMin minimum operation.<!-- npu="310p" id15 --><br><br>For the Atlas inference products AI Core, the Atomic operation can be enabled only when the output location is GM.<!-- end id15 --><!-- npu="310b" id16 --><br><br>For Atlas 200I/500 A2 inference products, the Atomic operation can be enabled only when the output location is GM.<!-- end id16 --> |
| enSequentialWrite | Input | Whether to enable the sequential write mode ([sequential write](GetTensorC.md#fig580415103338), writing to [baseM, baseN]; [non-sequential write](GetTensorC.md#fig322951853119), writing to the corresponding position in [singleCoreM, singleCoreN]). The default value is false (non-sequential write mode).<!-- npu="310b" id17 --><br><br>Atlas 200I/500 A2 inference products<!-- end id17 --> do not support this parameter. |
| waitIterateAll | Input | Used only in asynchronous scenarios. Whether to wait for IterateAll execution completion through the [WaitIterateAll](WaitIterateAll.md) API.<br><br>true: Wait for IterateAll execution completion through the WaitIterateAll API.<br><br>false: Do not wait for IterateAll execution completion through the WaitIterateAll API. The developer handles the process of waiting for IterateAll execution completion. |
| fakeMsg | Input | Used only in the IBShare scenario (where the [doIBShareNorm](MatmulConfig.md#matmulconfig-params) switch is enabled in the template parameter) and the IntraBlockPartSum scenario (where the [intraBlockPartSum](MatmulConfig.md#matmulconfig-params) switch is enabled in the template parameter).<br>IBShare scenario:<br>In this scenario, the same A cube or B cube data on L1 is reused, and the number of IterateAll calls by AIV cores must match. In this case, call IterateAll and set fakeMsg to true, which does not perform the actual computation but only ensures that IterateAll calls appear in pairs. The default value is false, indicating that the actual computation is performed.<br><br>IntraBlockPartSum scenario:<br>Used to fuse Vector computation and Cube computation in split mode, so that the result of one Matmul computation by multiple AIV cores (a cube tile of baseM * baseN size) is accumulated on the L0C Buffer. The default value is false, indicating that the Matmul computation result of each AIV core is accumulated on the L0C Buffer. |

## Return Value

None

## Constraints

-   The address space size of the input C cube must be no smaller than singleCoreM \* singleCoreN elements.
<!-- npu="950,A3,910b,310p,310b" id18 -->
-   Supported data types<a id="li12616155731720"></a>

    <!-- npu="950" id9 -->
    Ascend 950PR&950DT products, the supported data types are: half, float, bfloat16_t, int32_t, int8_t, fp8_e4m3fn_t, hifloat8_t.
    <!-- end id9 -->

    <!-- npu="A3" id10 -->
    For Atlas A3 products, the supported data types are: half, float, bfloat16_t, int32_t, int8_t.
    <!-- end id10 -->

    <!-- npu="910b" id11 -->
    For Atlas A2 products, the supported data types are: half, float, bfloat16_t, int32_t, int8_t.
    <!-- end id11 -->

    <!-- npu="310p" id12 -->
    The Atlas inference products AI Core does not support the prototype containing the ubCmatrix parameter. For the prototype containing the gm parameter, the supported data types are: half, float, int8_t, int32_t.
    <!-- end id12 -->

    <!-- npu="310b" id13 -->
    For Atlas 200I/500 A2 inference products, the supported data types are: half, float, bfloat16_t, int32_t.
    <!-- end id13 -->

<!-- end id18 -->

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/IterateAll_res.md#id2 -->

## Examples

The following is an example of calling the IterateAll API.

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
