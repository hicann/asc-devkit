# GetTensorC

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:33:54.861Z pushedAt=2026-09-12T09:55:18.107Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference product: Supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

After **Iterate**, one or two C matrix slices are obtained and can be output directly to a GM tensor or to a VECIN tensor. When the value of **ScheduleType** in the [MatmulConfig](MatmulConfig.md#matmulconfig-params) parameter is ScheduleType::INNER\_PRODUCT, one C matrix slice is obtained; when the value of **ScheduleType** in the [MatmulConfig](MatmulConfig.md#matmulconfig-params) parameter is ScheduleType::OUTER\_PRODUCT, two C matrix slices are obtained.

This API is used together with the [Iterate](Iterate.md) API. After **Iterate** is called to complete the iterative computation, one or two matrix slices of baseM \* baseN in size are obtained according to the value of **ScheduleType** in the [MatmulConfig](MatmulConfig.md#matmulconfig-params) parameter.

The process of iteratively obtaining C matrix slices is divided into two modes: synchronous mode and asynchronous mode.

-   **Synchronous**: Execute **GetTensorC** once after each **Iterate** execution, and synchronously wait for the C matrix slice to be obtained.
-   **Asynchronous**: After calling **Iterate**, you do not need to call **GetTensorC** immediately to wait synchronously. You can execute other logic first and call **GetTensorC** when the result is needed. The asynchronous mode reduces synchronous waiting and improves parallelism. You can choose this mode when you have high requirements for computation performance.

## Prototype

-   Obtain the C matrix and output it to VECIN.

    ```
    template <bool sync = true>
    __aicore__ inline void GetTensorC(const LocalTensor<DstT>& co2Local, uint8_t enAtomic = 0, bool enSequentialWrite = false)
    ```

    -   Support synchronous mode.
    -   Support asynchronous mode.

-   Obtain the C matrix and output it to GM.

    ```
    template <bool sync = true>
    __aicore__ inline void GetTensorC(const GlobalTensor<DstT>& gm, uint8_t enAtomic = 0, bool enSequentialWrite = false)
    ```

    -   Support synchronous mode.
    -   Support asynchronous mode.

-   Obtain the C matrix and output it to both GM and VECIN.

    ```
    template <bool sync = true>
    __aicore__ inline void GetTensorC(const GlobalTensor<DstT> &gm, const LocalTensor<DstT> &co2Local, uint8_t enAtomic = 0, bool enSequentialWrite = false)
    ```

    -   Support synchronous mode.
    -   Support asynchronous mode.
    -   This API is not supported in pure Cube mode (matrix computation only).
    -   Atlas 200I/500 A2 inference products do not support outputting to both GM and VECIN simultaneously.

-   Obtain the C matrix cached in the Workspace for asynchronous scenarios. The subsequent usage is controlled by you.

    When the C matrix is output to VECIN, the size of the Unified Buffer allocated to VECIN affects the Matmul computation granularity. If the size allocated to VECIN is too small, the hardware computing power cannot be fully utilized. This API is provided to return the C matrix cached in the Workspace, allowing you to control the subsequent usage.

    Note that during initialization, the logical position of the C matrix should be set to TPosition::VECIN. After calling this API to obtain the cached C matrix, copy it to the Unified Buffer by yourself.

    ```
    template <bool sync = true>
    __aicore__ inline GlobalTensor<DstT> GetTensorC(uint8_t enAtomic = 0, bool enSequentialWrite = false)
    ```

    -   Supports asynchronous mode.

The **doPad**, **height**, **width**, **srcGap**, and **dstGap** parameters in the following APIs are to be deprecated. You do not need to pass them during use; keep the default values. The prototype for outputting to VECIN described above is actually the function prototype without the default values passed.

```
template <bool sync = true, bool doPad = false>
__aicore__ inline void GetTensorC(const LocalTensor<DstT>& c, uint8_t enAtomic = 0, bool enSequentialWrite = false, uint32_t height = 0, uint32_t width = 0, uint32_t srcGap = 0, uint32_t dstGap = 0)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| **sync** | Set the synchronous or asynchronous mode: set to true for synchronous mode; set to false for asynchronous mode.<br><br>Ascend 950PR/Ascend 950DT support asynchronous mode.<br><br>Atlas A3 training products/Atlas A3 inference products support asynchronous mode.<br><br>Atlas A2 training products/Atlas A2 inference products support asynchronous mode.<br><br>AI Core of Atlas inference products do not support asynchronous mode.<br><br>Atlas 200I/500 A2 inference product does not support asynchronous mode. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **c/co2Local** | Output | Retrieve the C matrix to VECIN. For the data types supported by different models, see [Supported Data Types](#li12616155731720). For the data formats supported by different models, see [Supported Data Formats](#li12616155731721). |
| **gm** | Output | Retrieve the C matrix to GM. The data format can be ND or NZ. For the data types supported by different models, see [Supported Data Types](#li12616155731720). |
| **enAtomic** | Input | Whether to enable the Atomic operation. The default value is **0**.<br><br>Parameter values:<br><br>0: Do not enable the Atomic operation.<br><br>1: Enable the AtomicAdd accumulation operation.<br><br>2: Enable the AtomicMax maximum operation.<br><br>3: Enable the AtomicMin minimum operation.<br><br>For AI Core of Atlas inference products, the Atomic operation can be enabled only when the output location is GM.<br><br>For Atlas 200I/500 A2 inference product, the Atomic operation can be enabled only when the output location is GM. |
| **enSequentialWrite** | Input | Whether to enable the continuous write mode (continuous write writes to [baseM, baseN]; non-continuous write writes to the corresponding position in [singleCoreM, singleCoreN]). The default value is **false**. (non-continuous write mode).<br><br>Note: In non-continuous write mode, the offset is calculated internally in iteration order, so you do not need to pay attention to it. If you need to determine the layout order, you can select the continuous write mode and perform the data movement operation based on the configured offset.<br><br>For Atlas 200I/500 A2 inference product, only the non-continuous write mode is supported. |

 <a id="non-sequential-write"></a>
**Figure 1** Non-continuous write mode diagram  
![](../../../../figures/non-continuous-write-mode-diagram.png "Non-continuous write mode diagram")

<a id="sequential-write"></a>
**Figure 2** Continuous write mode diagram  
![](../../../../figures/continuous-write-mode-diagram.png "Continuous write mode diagram")

## Return Value

None

## Constraints

-   The address space size of the passed C matrix must be no smaller than **baseM** \* **baseN**.
-   In asynchronous scenarios, a temporary workspace is required to cache the Iterate computation results. When **GetTensorC** is called, the C matrix slice is obtained from this temporary workspace. The temporary workspace is configured through the [SetWorkspace](SetWorkspace.md) API, which must be called before the **Iterate** API.
-   When MixDualMaster (dual-master mode) is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to **true**, this API is not supported.
-   Supported data types<a id="li12616155731720"></a>

    Ascend 950PR/Ascend 950DT: The supported data types are half, float, bfloat16_t, int32_t, int8_t, fp8_e4m3fn_t, and hifloat8_t.

    Atlas A3 training products/Atlas A3 inference products: The supported data types are half, float, bfloat16_t, int32_t, and int8_t.

    Atlas A2 training products/Atlas A2 inference products: The supported data types are half, float, bfloat16_t, int32_t, and int8_t.

    AI Core of Atlas inference products: The supported data types are half, float, int8_t, and int32_t.

    Atlas 200I/500 A2 inference products: The supported data types are half, float, bfloat16_t, and int32_t.

-   Supported data format<a id="li12616155731721"></a>

    Ascend 950PR/Ascend 950DT, supported data types: ND, NZ.

    Atlas A3 training products/Atlas A3 inference products, supported data types: ND, NZ.

    Atlas A2 training products/Atlas A2 inference products, supported data types: ND, NZ.

    AI Core of Atlas inference products, supported data type: NZ.

    Atlas 200I/500 A2 inference product, supported data types: ND, NZ.

## Examples

-   Obtain the C matrix and output it to VECIN.

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

-   Obtain the C matrix and output it to GM. Synchronous mode example.

    ```
    while (mm.Iterate()) {
        mm.GetTensorC(gm);
    }
    ```

-   Obtain the C matrix and output it to both GM and VECIN. Synchronous mode example.

    ```
    while (mm.Iterate()) {
        mm.GetTensorC(gm, ubCmatrix);
    }
    ```

-   Obtain the C matrix on GM returned by the API and manually copy it to UB. Asynchronous mode example.

    ```
    // BaseM * BaseN = 128 *256
    mm.SetTensorA(gmA);
    mm.SetTensorB(gmB);
    mm.SetTail(singleM, singleN, singleK);
    mm.template Iterate<false>();
    // Other operations.
    for (int i = 0; i < singleM / baseM * singleN / baseN; ++i) {
        // Obtain the BaseM*BaseN data (128*256) for each computation.
        GlobalTensor<T> global = mm.template GetTensorC<false>();
        for(int j = 0; j < 4; ++j) {
            LocalTensor local = que.AllocTensor<half>(); // Allocate UB space of size 64*128.
            DataCopy(local, global[64 * 128 * i], 64 * 128); // Copy the GM data into UB for subsequent Vector operations.
            // Other Vector operations.
        }
    }
    ```
