# GetTensorC

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:26:34.014Z pushedAt=2026-10-27T11:24:57.897Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetTensorC_res.md#id1 -->

## Description

After Iterate, get one or two C cube slices, which can be output directly to a GM tensor or to a Unified Buffer (UB, VECIN) tensor. When the value of ScheduleType in the [MatmulConfig](MatmulConfig.md#matmulconfig-params) parameter is ScheduleType::INNER\_PRODUCT, get one C cube slice; when the value of ScheduleType in the [MatmulConfig](MatmulConfig.md#matmulconfig-params) parameter is ScheduleType::OUTER\_PRODUCT, get two C cube slices.

This API is used together with the [Iterate](Iterate.md) API. After Iterate completes the iterative computation, it gets one or two cube slices of baseM \* baseN size according to the value of ScheduleType in the [MatmulConfig](MatmulConfig.md#matmulconfig-params) parameter.

The process of iteratively obtaining C cube slices is divided into two modes: synchronous and asynchronous.

-   **Synchronous**: After each Iterate execution, GetTensorC is executed once, and it is necessary to synchronously wait for the C cube slice to be obtained.
<a id="li17508136205415"></a>
-   **Asynchronous**: After calling Iterate, there is no need to immediately call GetTensorC to wait synchronously. You can first execute other logic, and then call GetTensorC when the result is needed. The asynchronous mode reduces synchronous waiting and improves parallelism. Developers can choose this mode when they have high requirements for computation performance.

## Prototype

-   Get the C cube and output it to the UB (VECIN).

    ```
    template <bool sync = true>
    __aicore__ inline void GetTensorC(const LocalTensor<DstT>& co2Local, uint8_t enAtomic = 0, bool enSequentialWrite = false)
    ```

    -   Supports synchronous mode.
    -   Supports asynchronous mode.

-   Get the C cube and output it to the GM.

    ```
    template <bool sync = true>
    __aicore__ inline void GetTensorC(const GlobalTensor<DstT>& gm, uint8_t enAtomic = 0, bool enSequentialWrite = false)
    ```

    -   Supports synchronous mode.
    -   Supports asynchronous mode.

-   Get the C cube and output it to both GM and UB (VECIN) simultaneously.

    ```
    template <bool sync = true>
    __aicore__ inline void GetTensorC(const GlobalTensor<DstT>& gm, const LocalTensor<DstT>& co2Local, uint8_t enAtomic = 0, bool enSequentialWrite = false)
    ```

    -   Support synchronous mode.
    -   Support asynchronous mode.
    -   This API is not supported in pure Cube mode (cube computation only).
        <!-- npu="310b" id26 -->
    -   Atlas 200I/500 A2 inference products do not support outputting to both GM and UB (VECIN) simultaneously.
        <!-- end id26 -->

-   Get the C cube cached in the Workspace for asynchronous scenarios. The subsequent usage is controlled by the developer.

    When the C cube is output to UB (VECIN), the size of the space allocated to UB (VECIN) affects the granularity of Matmul computation. If the space allocated to UB (VECIN) is too small, the hardware computing power cannot be fully utilized. This API returns the C cube cached in the Workspace, and the developer controls the subsequent usage.

    Note that during initialization, the logical position of the C cube should be set to **TPosition::VECIN**. After calling this API to get the cached C cube, copy it to UB yourself.

    ```
    template <bool sync = true>
    __aicore__ inline GlobalTensor<DstT> GetTensorC(uint8_t enAtomic = 0, bool enSequentialWrite = false)
    ```

    -   Support asynchronous mode.

The **doPad**, **height**, **width**, **srcGap**, and **dstGap** parameters in the following APIs are to be deprecated. You do not need to pass them during use; keep the default values. The prototype for outputting to UB (VECIN) described above is actually the function prototype without the default values passed.

```
template <bool sync = true, bool doPad = false>
__aicore__ inline void GetTensorC(const LocalTensor<DstT>& c, uint8_t enAtomic = 0, bool enSequentialWrite = false, uint32_t height = 0, uint32_t width = 0, uint32_t srcGap = 0, uint32_t dstGap = 0)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| sync | Set synchronous or asynchronous mode: in synchronous mode, set to true; in asynchronous mode, set to false.<!-- npu="950" id9 --><br><br>Ascend 950PR&950DT products support asynchronous mode.<!-- end id9 --><!-- npu="A3" id10 --><br><br>Atlas A3 products support asynchronous mode.<!-- end id10 --><!-- npu="910b" id11 --><br><br>Atlas A2 products support asynchronous mode.<!-- end id11 --><!-- npu="310p" id12 --><br><br>The Atlas inference products AI Core does not support asynchronous mode.<!-- end id12 --><!-- npu="310b" id13 --><br><br>Atlas 200I/500 A2 inference products do not support asynchronous mode.<!-- end id13 --> |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| c/co2Local | Output | Load the C cube to UB (VECIN). For supported data types of different models, see [Supported data types](#li12616155731720). For supported data formats of different models, see [Supported data formats](#li12616155731721). |
| gm | Output | Load the C cube to GM. The data format can be ND or NZ. For supported data types of different models, see [Supported data types](#li12616155731720). |
| enAtomic | Input | Whether to enable the Atomic operation. The default value is 0.<br><br>Parameter values:<br><br>0: Do not enable the Atomic operation.<br><br>1: Enable the AtomicAdd accumulation operation.<br><br>2: Enable the AtomicMax maximum operation.<br><br>3: Enable the AtomicMin minimum operation.<!-- npu="310p" id24 --><br><br>For the Atlas inference products AI Core, the Atomic operation can be enabled only when the output location is GM.<!-- end id24 --><!-- npu="310b" id25 --><br><br>For Atlas 200I/500 A2 inference products, the Atomic operation can be enabled only when the output location is GM.<!-- end id25 --> |
| enSequentialWrite | Input | Whether to enable continuous write mode (continuous write, write to [baseM, baseN]; non-continuous write, write to the corresponding position in [singleCoreM, singleCoreN]). The default value is false (non-continuous write mode).<br><br>Note: In non-continuous write mode, the offset is calculated internally in iteration order, and developers do not need to pay attention to it. If developers need to determine the layout order, they can select continuous write mode and perform the data movement operation according to the set offset.<!-- npu="310b" id27 --><br><br>For Atlas 200I/500 A2 inference products, only the non-continuous write mode is supported.<!-- end id27 --> |

<a name="fig322951853119"></a>
**Figure 1**  Non-continuous write mode diagram  
![](../../../../figures/non_continuous_write_mode.png "Non-continuous write mode diagram")

<a name="fig580415103338"></a>
**Figure 2**  Continuous write mode diagram  
![](../../../../figures/continuous_write_mode.png "Continuous write mode diagram")

## Return Value

None

## Constraints

-   The address space size of the input C cube must be no smaller than baseM \* baseN.
-   In asynchronous scenarios, a temporary space is required to cache the Iterate computation results. When GetTensorC is called, the cube slice of C is obtained from this temporary space. The temporary space is set through the [SetWorkspace](SetWorkspace.md) API. The SetWorkspace API must be called before the Iterate API.
-   When the MixDualMaster (dual-master mode) scenario is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to true, this API is not supported.
<!-- npu="950,A3,910b,310p,310b" id28 -->
-   Supported data types<a id="li12616155731720"></a>

    <!-- npu="950" id14 -->
    Ascend 950PR&950DT products support the following data types: half, float, bfloat16_t, int32_t, int8_t, fp8_e4m3fn_t, hifloat8_t.
    <!-- end id14 -->

    <!-- npu="A3" id15 -->
    Atlas A3 products support the following data types: half, float, bfloat16_t, int32_t, int8_t.
    <!-- end id15 -->

    <!-- npu="910b" id16 -->
    For Atlas A2 products, supported data types are: half, float, bfloat16_t, int32_t, int8_t.
    <!-- end id16 -->

    <!-- npu="310p" id17 -->
    For the Atlas inference products AI Core, supported data types are: half, float, int8_t, int32_t.
    <!-- end id17 -->

    <!-- npu="310b" id18 -->
    For Atlas 200I/500 A2 inference products, supported data types are: half, float, bfloat16_t, int32_t.
    <!-- end id18 -->

-   Supported data format<a id="li12616155731721"></a>

    <!-- npu="950" id19 -->
    For Ascend 950PR&950DT products, supported data types are: ND, NZ.
    <!-- end id19 -->

    <!-- npu="A3" id20 -->
    For Atlas A3 products, supported data types are: ND, NZ.
    <!-- end id20 -->

    <!-- npu="910b" id21 -->
    For Atlas A2 products, supported data types are: ND, NZ.
    <!-- end id21 -->

    <!-- npu="310p" id22 -->
    For the Atlas inference products AI Core, supported data type is: NZ.
    <!-- end id22 -->

    <!-- npu="310b" id23 -->
    For Atlas 200I/500 A2 inference products, supported data types are: ND, NZ.
    <!-- end id23 -->
<!-- end id28 -->

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetTensorC_res.md#id2 -->

## Examples

-   Get the C cube and output it to UB (VECIN).

    ```
    // Synchronous mode example.
    while (mm.Iterate()) {
        mm.GetTensorC(ubCmatrix);
    }

    // Asynchronous mode example.
    mm.template Iterate<false>();
    // Other operations.
    for (int i = 0; i < singleM / baseM * singleN / baseN; ++i) {
        mm.template GetTensorC<false>(ubCmatrix);
        // Other operations.
    }
    ```

-   Get the C cube, output to GM, synchronous mode example.

    ```
    while (mm.Iterate()) {
        mm.GetTensorC(gm);
    }
    ```

-   Get the C cube, output simultaneously to GM and UB (VECIN), synchronous mode example.

    ```
    while (mm.Iterate()) {
        mm.GetTensorC(gm, ubCmatrix);
    }
    ```

-   Example of getting the C cube on GM returned by the API and manually copying it to UB in asynchronous mode.

    ```
    // BaseM * BaseN = 128 *256
    mm.SetTensorA(gmA);
    mm.SetTensorB(gmB);
    mm.SetTail(singleM, singleN, singleK);
    mm.template Iterate<false>();
    // Other operations.
    for (int i = 0; i < singleM / baseM * singleN / baseN; ++i) {
        // Get the 128*256 data of BaseM*BaseN for each computation.
        GlobalTensor<T> global = mm.template GetTensorC<false>();
        for (int j = 0; j < 4; ++j) {
            LocalTensor local = que.AllocTensor<half>();     // Allocate 64*128 UB space.
            DataCopy(local, global[64 * 128 * i], 64 * 128); // Copy the GM data into the UB for subsequent Vector operations.
            // Other Vector operations.
        }
    }
    ```
