# Iterate

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:28:23.718Z pushedAt=2026-10-27T11:25:36.510Z nl=2 -->

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
- Atlas training products: not supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Iterate_res.md#id1 -->

## Description

Each call to **Iterate** computes a **baseM** \* **baseN** C cube. The API internals maintain the iteration progress, and after each call, the base addresses of the A and B cubes are offset. By default, the iteration order is M-axis first and then N-axis; you can also change it to N-axis first and then M-axis by adjusting the **iterateOrder** tiling parameter. When the input data is not aligned and a tail block exists, the computation result of the tail block is output in the last iteration.

The C cube result of a single **Iterate** cube multiplication is stored in the L0C Buffer (CO1). The following two methods are currently supported for obtaining the computation result from the L0C Buffer (CO1):

-   Users do not need to manage the allocation and release of the L0C Buffer (CO1) memory that stores the cube multiplication result; this is managed by the Matmul API internals. After calling the Iterate function prototype for [API-internal CO1 management](#li135771283591), call the [GetTensorC](GetTensorC.md) API to move the computation result out of the L0C Buffer (CO1).
-   Users can flexibly and autonomously control the movement of the cube multiplication computation result. For example, users can cache the cube multiplication results of multiple Iterate computations in the L0C Buffer (CO1) memory, and move out multiple baseM \* baseN C cubes at once when the result needs to be moved out. In this flexible movement scenario, users need to allocate the L0C Buffer (CO1) memory in advance. After calling the Iterate function prototype for [user-managed CO1](#li4843165185812), the computation result of one Iterate is output to the user-allocated L0C Buffer (CO1) memory. When the computation result needs to be moved out, call the [Fixpipe](../../../basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md) API to move the result from the L0C Buffer (CO1), and release the allocated L0C Buffer (CO1) memory after completion. For a specific example, see [the cube multiplication scenario with user-managed CO1](Matmul_usage.md#user-managed-co1).

## Prototype

-   <a name="li135771283591"></a>API-internal management of the L0C Buffer (CO1)

    ```
    template <bool sync = true> __aicore__ inline bool Iterate(bool enPartialSum = false)
    ```

-   <a name="li4843165185812"></a>User-managed L0C Buffer (CO1)

    ```
    template <bool sync = true, typename T> __aicore__ inline bool Iterate(bool enPartialSum, const LocalTensor<T>& localCmatrix)
    ```

    <!-- npu="310p" id9 -->
    -   The Atlas inference products AI Core does not support user-managed L0C Buffer (CO1).
    <!-- end id9 -->
    <!-- npu="310b" id10 -->
    -   The Atlas 200I/500 A2 inference product does not support user-managed L0C Buffer (CO1).
    <!-- end id10 -->

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| sync | The process of iteratively obtaining the C cube tile is divided into synchronous and asynchronous modes. This parameter sets the synchronous or asynchronous mode: set to **true** for synchronous mode; set to **false** for asynchronous mode. The default is synchronous mode. For the introduction and usage of the specific modes, see [GetTensorC](GetTensorC.md). |
| T | Data type of the LocalTensor on the user-allocated L0C Buffer (CO1) memory, that is, the data type of the C cube output by the cube multiplication. The currently supported data types are float and int32_t. |

**Table 2**  Function parameter description for API-internal management of the L0C Buffer (CO1)

| Parameter | Input/Output | Description |
| --- | --- | --- |
| enPartialSum | Input | Whether to accumulate the cube multiplication result onto the existing L0C Buffer (CO1) data, with a default value of **false**. During L0C accumulation, only the C cube specification singleCoreM==baseM && singleCoreN==baseN is supported.<!-- npu="310b" id11 --><br><br>For the Atlas 200I/500 A2 inference product, this parameter can only be set to **false**.<!-- end id11 --> |

**Table 3**  Function parameter description for user-managed L0C Buffer (CO1)

| Parameter | Input/Output | Description |
| --- | --- | --- |
| enPartialSum | Input | Whether to accumulate the cube multiplication result onto the existing L0C Buffer (CO1) data. When accumulating in L0C, only the C cube specification singleCoreM==baseM && singleCoreN==baseN is supported. |
| localCmatrix | Output | LocalTensor memory on the L0C Buffer (CO1) applied by the user, used to store the cube multiplication computation result. |

## Return Value

false: All data on a single core has been fully computed.

true: The data is still being iteratively computed.

## Constraints

-   When the MixDualMaster (dual-master mode) scenario is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to true, this API is not supported.
-   For the Iterate function with user-managed CO1, when creating the Matmul object, the memory logical position of the C cube must be defined as TPosition::CO1, the data layout format as CubeFormat::NZ, and the data type as float or int32\_t.

## Examples

Simple call examples for the synchronous mode and the asynchronous mode are as follows.

```
// Synchronous mode example.
while (mm.Iterate()) {
    mm.GetTensorC(ubCmatrix);
}

// Asynchronous mode example.
mm.template Iterate<false>();
// ... ... Other computations.
for (int i = 0; i < singleM/baseM*singleN/baseN; ++i) {
    mm.template GetTensorC<false>(ubCmatrix);
}
```
