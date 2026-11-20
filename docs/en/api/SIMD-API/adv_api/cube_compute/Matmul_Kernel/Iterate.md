# Iterate

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-02T06:22:32.027Z pushedAt=2026-11-18T04:52:50.836Z nl=2 -->

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
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- npu="x90" id1 -->
- Kirin X90 processor products: Supported
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030 processor products: Supported
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Iterate_res.md#id1 -->

## Description

Each call to Iterate computes a baseM \* baseN C cube. The API maintains the iteration progress internally and offsets the base addresses of cubes A and B after each call. By default, iteration proceeds along the M axis first and then the N axis. You can change this to iterate along the N axis first and then the M axis by adjusting the tiling parameter iterateOrder. When the input data is not aligned and a tail block exists, the tail block result is output in the last iteration.

The result C cube of a single Iterate cube multiplication is stored in the L0C Buffer (CO1). The following two methods are currently supported for obtaining the computation result in the L0C Buffer (CO1):

-   You do not need to manage the allocation and release of the L0C Buffer (CO1) memory that stores the cube multiplication result; the Matmul API manages it internally. After calling the Iterate function prototype for [API-managed CO1](#li135771283591), call the [GetTensorC](GetTensorC.md) API to move the computation result out of the L0C Buffer (CO1).
-   You can flexibly control the movement of the cube multiplication result. For example, you can cache the results of multiple Iterate cube multiplications in the L0C Buffer (CO1) memory and move multiple baseM \* baseN C cubes at once when needed. In this flexible movement scenario, you need to allocate the L0C Buffer (CO1) memory in advance. After calling the Iterate function prototype for [user-managed CO1](#li4843165185812), the result of a single Iterate is output to the L0C Buffer (CO1) memory that you allocated. When you need to move the result out, call the [Fixpipe](../../../basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md) API to move the result from the L0C Buffer (CO1), and release the allocated L0C Buffer (CO1) memory after completion. For a specific example, see [Cube multiplication with user-managed CO1](Matmul_usage.md#user-managed-co1).

## Prototype

-   <a name="li135771283591"></a>API-managed L0C Buffer (CO1)

    ```
    template <bool sync = true> __aicore__ inline bool Iterate(bool enPartialSum = false)
    ```

-   <a name="li4843165185812"></a>User-managed L0C Buffer (CO1)

    ```
    template <bool sync = true, typename T> __aicore__ inline bool Iterate(bool enPartialSum, const LocalTensor<T>& localCmatrix)
    ```

    <!-- npu="310p" id9 -->
    -   Atlas inference products: AI Core does not support user-managed L0C Buffer (CO1) yet.
    <!-- end id9 -->
    <!-- npu="310b" id10 -->
    -   Atlas 200I/500 A2 inference products do not support user-managed L0C Buffer (CO1) yet.
    <!-- end id10 -->

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| sync | The process of iteratively obtaining C cube fragments has two modes: synchronous and asynchronous. This parameter sets the synchronous or asynchronous mode: set to true for synchronous mode; set to false for asynchronous mode. The default is synchronous mode. For details about the modes and how to use them, see [GetTensorC](GetTensorC.md). |
| T | Data type of the LocalTensor on the L0C Buffer (CO1) memory allocated by you, that is, the data type of the C cube output by the cube multiplication. The supported data types are float and int32_t. |

**Table 2**  Function parameter description for API-managed L0C Buffer (CO1)

| Parameter | Input/Output | Description |
| --- | --- | --- |
| enPartialSum | Input | Whether to accumulate the cube multiplication result into the existing L0C Buffer (CO1) data, defaulting to false. When accumulating in L0C, only the C cube specification singleCoreM==baseM && singleCoreN==baseN is supported.<!-- npu="310b" id11 --><br><br>For Atlas 200I/500 A2 inference products, this parameter can only be set to false.<!-- end id11 --> |

**Table 3**  Function parameter description for user-managed L0C Buffer (CO1)

| Parameter | Input/Output | Description |
| --- | --- | --- |
| enPartialSum | Input | Whether to accumulate the cube multiplication result into the existing L0C Buffer (CO1) data. When accumulating in L0C, only the C cube specification singleCoreM==baseM && singleCoreN==baseN is supported. |
| localCmatrix | Output | LocalTensor memory on the L0C Buffer (CO1) allocated by you, used to store the cube multiplication result. |

## Return Value

**false**: All data on a single core has been fully computed.

**true**: Data is still being iteratively computed.

## Constraints

-   When MixDualMaster (dual-master mode) is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to true, this API is not supported.
-   For the Iterate function with user-managed CO1, when creating the Matmul object, the memory logical position of the C cube must be defined as TPosition::CO1, the data format as CubeFormat::NZ, and the data type as float or int32\_t.

## Examples

The following are simple examples of calling in synchronous and asynchronous modes.

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
