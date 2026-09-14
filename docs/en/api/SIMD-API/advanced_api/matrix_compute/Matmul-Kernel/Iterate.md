# Iterate

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:35:35.540Z pushedAt=2026-09-12T09:55:18.109Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference product: Supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

Each call to **Iterate** computes a block of the C matrix of size baseM \* baseN. The API maintains the iteration progress internally, and after each call, the base addresses of the A and B matrices are offset. By default, the iteration order is M-axis first and then N-axis; you can also change it to N-axis first and then M-axis by adjusting the **iterateOrder** tiling parameter. When the input data is not aligned and a tail block exists, the computation result of the tail block is output in the last iteration.

The C matrix result of one **Iterate** matrix multiplication is stored in the memory at logical location CO1. The following two methods are currently supported for obtaining the computation result from the CO1 memory:

-   You do not need to manage the application and release of the CO1 memory that stores the matrix multiplication result; this is managed internally by the **Matmul** API. After calling the Iterate function prototype of [API-managed CO1](#li135771283591), call the [GetTensorC](GetTensorC.md) API to move the computation result out of CO1.
-   You can flexibly and autonomously control the movement of the matrix multiplication computation result. For example, you can cache the matrix multiplication results of multiple **Iterate** calls in the CO1 memory and move out multiple blocks of the C matrix of size baseM \* baseN at once when needed. In this flexible movement scenario, you need to request the CO1 memory in advance. After calling the Iterate function prototype of [user-managed CO1](#li4843165185812), the computation result of one **Iterate** is output to the CO1 memory that you applied for. When you need to move out the computation result, call the [Fixpipe](../../../basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md) API to move the result out of CO1, and release the applied CO1 memory after completion. For a specific example, see [user-managed CO1 matmul](matmul_usage.md#user-managed-co1-matmul).

## Prototype

-   <a name="li135771283591"></a>CO1 managed internally by the API  

    ```
    template <bool sync = true> __aicore__ inline bool Iterate(bool enPartialSum = false)
    ```

-   <a name="li4843165185812"></a>CO1 managed by the user

    ```
    template <bool sync = true, typename T> __aicore__ inline bool Iterate(bool enPartialSum, const LocalTensor<T>& localCmatrix)
    ```

    -   Atlas inference products do not support user autonomy in managing CO1 on the AI Core for the time being.
    -   Atlas 200I/500 A2 inference products do not support user autonomy in managing CO1 for the time being.

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| sync | The process of iteratively obtaining C matrix tiles is divided into synchronous and asynchronous modes. This parameter sets the synchronous or asynchronous mode: set it to **true** for synchronous mode and **false** for asynchronous mode. The default is synchronous mode. For details about the modes and their usage, see [GetTensorC](GetTensorC.md). |
| T | Data type of the **LocalTensor** on the **CO1** memory applied for by the user, that is, the data type of the C matrix output by matrix multiplication. Currently supported data types are **float** and **int32_t**. |

**Table 2** Parameter description for the function that manages CO1 internally

| Parameter | Input/Output | Description |
| --- | --- | --- |
| enPartialSum | Input | Whether to accumulate the matrix multiplication result into the existing **CO1** data. The default value is **false**. When accumulating in L0C, only the C matrix specification **singleCoreM==baseM && singleCoreN==baseN** is supported.<br><br>For the Atlas 200I/500 A2 inference product, this parameter can only be set to **false**. |

**Table 3** Parameter description for the function that manages CO1 with user autonomy

| Parameter | Input/Output | Description |
| --- | --- | --- |
| enPartialSum | Input | Whether to accumulate the matrix multiplication result into the existing **CO1** data. When accumulating in L0C, only the C matrix specification **singleCoreM==baseM && singleCoreN==baseN** is supported. |
| localCmatrix | Output | **LocalTensor** memory on **CO1** applied for by the user, used to store the matrix multiplication computation result. |

## Return Value

**false**: All data on a single core has been fully computed.

**true**: The data is still being iteratively computed.

## Constraints

-   When the MixDualMaster (dual-master) scenario is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to `true`, this API is not supported.
-   For the Iterate function with user autonomy in managing CO1, when creating a Matmul object, you must define the logical memory position of the C matrix as **TPosition::CO1**, the data layout format as **CubeFormat::NZ**, and the data type as `float` or int32\_t.

## Examples

Simple examples of the synchronous and asynchronous modes are as follows.

```
// Synchronous mode sample.
while (mm.Iterate()) {
    mm.GetTensorC(ubCmatrix);
}

// Asynchronous mode sample.
mm.template Iterate<false>();
// ... Other computations.
for (int i = 0; i < singleM/baseM*singleN/baseN; ++i) {
    mm.template GetTensorC<false>(ubCmatrix);
}
```
