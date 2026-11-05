# Sort

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:27:34.570Z pushedAt=2026-10-27T11:53:19.179Z nl=2 -->

## Applicable Products

<!-- npu="950" id11 -->
- Ascend 950PR&950DT products: Supported
<!-- end id11 -->
<!-- npu="A3" id12 -->
- Atlas A3 products: Supported
<!-- end id12 -->
<!-- npu="910b" id13 -->
- Atlas A2 products: Supported
<!-- end id13 -->
<!-- npu="310b" id14 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas training products: Not supported
<!-- end id16 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/sort_operations/Sort_res.md#id1 -->

## Description

-   Corresponds to the function prototype without the **SortConfig** template parameter.

    The sorting function sorts data in descending order by value. The sorted data is stored in the following layout:

    <!-- npu="950" id24 -->
    Ascend 950PR&950DT products adopt method 1.
    <!-- end id24 -->

    <!-- npu="A3" id25 -->
    Atlas A3 products adopt method 1.
    <!-- end id25 -->

    <!-- npu="910b" id26 -->
    Atlas A2 products adopt method 1.
    <!-- end id26 -->

    <!-- npu="310p" id27 -->
    The Atlas inference products AI Core adopts method 2.
    <!-- end id27 -->

-   Layout method 1:

    One iteration can complete the sorting of 32 numbers. The sorted score and its corresponding index are stored together in dst as a (score, index) structure. Regardless of whether score is of the half or float type, the (score, index) structure in dst always occupies 8 Bytes of space, as shown below:

    -   When score is of the float type and index is of the uint32 type, in the computation result, index is stored in the high 4 Bytes and score is stored in the low 4 Bytes.

        ![](../../../figures/zh-cn_image_0000002007005918.png)

    -   When score is of the half type and index is of the uint32 type, in the computation result, index is stored in the high 4 Bytes, score is stored in the low 2 Bytes, and the middle 2 Bytes are reserved.

        ![](../../../figures/zh-cn_image_0000002042886661.png)

-   Layout method 2: Region Proposal layout

    Both the input and output data are Region Proposals. One iteration can sort 16 region proposals. Each Region Proposal occupies 8 consecutive elements of the **half**/**float** type, with the following format:

    ```
    [x1, y1, x2, y2, score, label, reserved_0, reserved_1]
    ```

    For the **half** data type, each Region Proposal occupies 16 bytes, Byte\[15:12\] is invalid data, and Byte\[11:0\] contains 6 elements of the **half** type, where Byte\[11:10\] is defined as **label**, Byte\[9:8\] is defined as **score**, Byte\[7:6\] is defined as **y2**, Byte\[5:4\] is defined as **x2**, Byte\[3:2\] is defined as **y1**, and Byte\[1:0\] is defined as **x1**.

    As shown in the following figure, a total of 16 Region Proposals are included.

    ![](../../../figures/zh-cn_image_0000002043918785.png)

    For the **float** data type, each Region Proposal occupies 32 bytes, Byte\[31:24\] is invalid data, and Byte\[23:0\] contains 6 elements of the **float** type, where Byte\[23:20\] is defined as **label**, Byte\[19:16\] is defined as **score**, Byte\[15:12\] is defined as **y2**, Byte\[11:8\] is defined as **x2**, Byte\[7:4\] is defined as **y1**, and Byte\[3:0\] is defined as **x1**.

    As shown in the following figure, a total of 16 Region Proposals are included.

    ![](../../../figures/zh-cn_image_0000002043797849.png)

-   Corresponds to the function prototype with the **SortConfig** template parameter.

    <!-- npu="950" id28 -->
    Supported only on Ascend 950PR&950DT products.
    <!-- end id28 -->

    Sorts the input data according to the sorting algorithm specified in the template parameter **SortConfig**. The sorting result can be specified as ascending or descending order.

    When the function prototype includes the output index parameter **dstIndexTensor**, the indices corresponding to the sorted result data need to be output. If the input includes the index parameter **srcIndexTensor**, the output indices are the original input indices. If the input does not carry indices, generate indices for the input data starting from 0 up to the required number of elements to be sorted. The final output indices are the indices corresponding to the input data. The following two figures show sorting examples for input with indices and input without indices, respectively.

    **Figure 1** Sorting sample with input index srcIndex  
    ![](../../../figures/sort_with_srcindex.png "Sorting sample with input index srcIndex")

    **Figure 2** Sorting sample without input index srcIndex  
    ![](../../../figures/sort_without_srcindex.png "Sorting sample without input index srcIndex")

## Prototype

-   Without **SortConfig**

    ```
    template <typename T, bool isFullSort>
    __aicore__ inline void Sort(const LocalTensor<T>& dst, const LocalTensor<T>& concat, const LocalTensor<uint32_t>& index, LocalTensor<T>& tmp, const int32_t repeatTime)
    ```

-   With **SortConfig**

    <!-- npu="950" id29 -->
    Supported only on Ascend 950PR&950DT products.
    <!-- end id29 -->

    -   The API framework applies for temporary space
        -   Without the **srcIndexTensor** and **dstIndexTensor** parameters

            ```
            template <typename T, bool isReuseSource = false, const SortConfig& config = DEFAULT_SORT_CONFIG>
            __aicore__ inline void Sort(LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
            ```

        -   Without the **srcIndexTensor** parameter, with the **dstIndexTensor** parameter

            ```
            template <typename T, bool isReuseSource = false, const SortConfig& config = DEFAULT_SORT_CONFIG>
            __aicore__ inline void Sort(LocalTensor<T>& dstTensor, LocalTensor<uint32_t>& dstIndexTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
            ```

        -   With the **srcIndexTensor** and **dstIndexTensor** parameters.

            ```
            template <typename T, typename U, bool isReuseSource = false, const SortConfig& config = DEFAULT_SORT_CONFIG>
            __aicore__ inline void Sort(const LocalTensor<T>& dstTensor, const LocalTensor<U>& dstIndexTensor, const LocalTensor<T>& srcTensor, const LocalTensor<U>& srcIndexTensor, const uint32_t calCount)
            ```

    -   Passes temporary space via the **sharedTmpBuffer** input parameter.
        -   Without the **srcIndexTensor** and **dstIndexTensor** parameters.

            ```
            template <typename T, bool isReuseSource = false, const SortConfig& config = DEFAULT_SORT_CONFIG>
            __aicore__ inline void Sort(LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
            ```

        -   Without the **srcIndexTensor** parameter, with the **dstIndexTensor** parameter.

            ```
            template <typename T, bool isReuseSource = false, const SortConfig& config = DEFAULT_SORT_CONFIG>
            __aicore__ inline void Sort(LocalTensor<T>& dstTensor, LocalTensor<uint32_t>& dstIndexTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
            ```

        -   With the **srcIndexTensor** and **dstIndexTensor** parameters.

            ```
            template <typename T, typename U, bool isReuseSource = false, const SortConfig& config = DEFAULT_SORT_CONFIG>
            __aicore__ inline void Sort(LocalTensor<T>& dstTensor, LocalTensor<U>& dstIndexTensor, const LocalTensor<T>& srcTensor, const LocalTensor<U>& srcIndexTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
            ```

    Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during computation. Temporary space supports two methods: **passing it via the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

    -   Passing it via the **sharedTmpBuffer** input parameter: this tensor is used as the temporary space for handling, and the API framework no longer applies for it. With this method, developers can manage the **sharedTmpBuffer** memory space by themselves and reuse this memory after the API call completes. The memory is not repeatedly applied for and released, offering high flexibility and high memory utilization.
    -   The API framework applies for the temporary space. Developers do not need to apply for it, but they need to reserve the size of the temporary space.

    When the space is passed via **sharedTmpBuffer**, developers need to apply for space for the tensor; when the API framework applies for it, developers need to reserve the temporary space. The temporary space size **BufferSize** is obtained as follows: obtain the size of the space range to be reserved through the API provided in [GetSortMaxMinTmpSize](GetSortMaxMinTmpSize.md).

## Parameters

-   Corresponds to the function prototype without **SortConfig**.

    **Table 1** Template parameter description

    | Parameter | Description |
    | --- | --- |
    | T | Data type of the operand. Supported data types: half and float. |
    | isFullSort | Whether to enable full sort mode. Full sort mode sorts all inputs in descending order. In non-full sort mode, refer to the repeatTime description in Table 2 for the sorting method. |

    **Table 2**  Parameters

    | Parameter | Input/Output | Description |
    | --- | --- | --- |
    | dst | Output | Destination operand, with shape [2n].<br>    <br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and supported TPosition values are VECIN/VECCALC/VECOUT.<br>    <br>The start address of LocalTensor needs 32-byte alignment. |
    | concat | Input | Source operand, that is, score in the API description, with shape [n].<br>    <br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and supported TPosition values are VECIN/VECCALC/VECOUT.<br>    <br>The start address of LocalTensor needs 32-byte alignment.<br>    <br>The data type of this source operand needs to be consistent with that of the destination operand. |
    | index | Input | Source operand, with shape [n].<br>    <br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and supported TPosition values are VECIN/VECCALC/VECOUT.<br>    <br>The start address of LocalTensor needs 32-byte alignment.<br>    <br>This source operand is fixed to the uint32_t data type. |
    | tmp | Input | Temporary space. Used to store intermediate variables during complex internal computations of the API. It is provided by the developer. For how to obtain the temporary space size BufferSize, see [GetSortTmpSize](GetSortTmpSize.md). The data type is consistent with that of the source operand.<br>    <br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and supported TPosition values are VECIN/VECCALC/VECOUT.<br>    <br>The start address of LocalTensor needs 32-byte alignment. |
    | repeatTime | Input | Number of repeated iterations, of the int32_t type. <!-- npu="950" id17 --><br>    Ascend 950PR&950DT products: Each iteration sorts 32 elements. In the next iteration, concat and index each skip 32 elements, and dst skips 32*8 bytes of space. Value range: repeatTime∈[0,255]. <!-- end id17 --><!-- npu="A3" id18 --><br>    Atlas A3 products: Each iteration sorts 32 elements. In the next iteration, concat and index each skip 32 elements, and dst skips 32*8 bytes of space. Value range: repeatTime∈[0,255]. <!-- end id18 --><!-- npu="910b" id19 --><br> Atlas A2 products: Each iteration sorts 32 elements. In the next iteration, concat and index each skip 32 elements, and dst skips 32*8 bytes of space. Value range: repeatTime∈[0,255]. <!-- end id19 --><!-- npu="310p" id20 --><br>Atlas inference products AI Core: Each iteration sorts 16 region proposals. In the next iteration, concat and dst each skip 16 region proposals. Value range: repeatTime∈[0,255]. <!-- end id20 --> |

-   Corresponding to the function prototype with SortConfig.

    **Table 3**  Template parameters

    | Parameter | Function |
    | --- | --- |
    | T | Data type of the srcTensor and dstTensor operands. Data types supported by the RADIX_SORT sorting algorithm: uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, bfloat16_t, float, uint64_t, int64_t. Data types supported by the MERGE_SORT sorting algorithm: half and float. |
    | U | Data type of the srcIndexTensor and dstIndexTensor operands. Data types supported by the RADIX_SORT sorting algorithm: uint32_t, int32_t, uint64_t, int64_t. Data type supported by the MERGE_SORT sorting algorithm: uint32_t. |
    | isReuseSource | Optional parameter. Whether the input Tensor space can be reused. |
    | config | Optional parameter. Corresponding configuration of the Sort API: the selected sorting algorithm and the ascending/descending order of the sorting result. The data type is SortConfig, defined as shown in the following code.<br>    <br>Sort provides two different sorting algorithms: the MERGE_SORT merge sort algorithm and the RADIX_SORT radix sort algorithm. The two algorithms differ in execution speed, time complexity, and algorithm stability.<br>    MERGE_SORT is a stable sorting algorithm whose time complexity is O(nlogn) in all cases.<br>The time complexity of the RADIX_SORT algorithm is O(n). When handling a large amount of data, if the maximum number has few digits, this algorithm is highly efficient and can approach linear time complexity. However, if the maximum number has many digits, the time complexity approaches O(n^2).<br>    <br>The default value of config, DEFAULT_SORT_CONFIG, is as follows: it uses the radix sort RADIX_SORT and sorts the sorting result in ascending order.<br>    constexpr SortConfig DEFAULT_SORT_CONFIG = {SortType::RADIX_SORT, false}; |

    ```
    enum class SortType {
        RADIX_SORT, // Use the radix sort algorithm.
        MERGE_SORT  // Use the merge sort algorithm.
    };
    struct SortConfig {
        SortType type = SortType::RADIX_SORT; // Sorting algorithm.
        bool isDescend = false;               // Whether to sort in descending order. The default value is false, and the output result is sorted in ascending order.
    };
    ```

    **Table 4**  Parameters

    | Parameter | Input/Output | Description |
    | --- | --- | --- |
    | dstTensor | Output | Value destination operand, with shape [n]. Under the MERGE_SORT algorithm, each element of the output data needs to reserve 8-byte space.<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>    <br>The start address of LocalTensor needs 32-byte alignment. |
    | dstIndexTensor | Output | Index destination operand, with shape [n]. When the input does not carry srcIndexTensor, only the uint32_t type is supported.<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>    <br>The start address of LocalTensor needs 32-byte alignment. |
    | srcTensor | Input | Value source operand, with shape [n].<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>    <br>The start address of LocalTensor needs 32-byte alignment.<br>    <br>The data type of this source operand needs to be consistent with that of the value destination operand. |
    | srcIndexTensor | Input | Index source operand, with shape [n].<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>    <br>The start address of LocalTensor needs 32-byte alignment.<br>    <br>The data type of this source operand needs to be consistent with that of the index destination operand. |
    | sharedTmpBuffer | Input | Temporary space. Used to store intermediate variables during complex internal computation, provided by the developer. For how to obtain the temporary space size BufferSize, see [GetSortMaxMinTmpSize](GetSortMaxMinTmpSize.md). The data type is uint8_t.<br>    <br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>    <br>The start address of LocalTensor needs 32-byte alignment. |
    | calCount | Input | Number of data elements to be sorted. The type is uint32_t. |

## Return Value

None

## Constraints

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   API without SortConfig:
    -   When score\[i\] and score\[j\] are the same, if i\>j, then score\[j\] is selected first and placed in front, that is, the order of index is consistent with the input order.
    -   In non-full sort mode, the data within each iteration is sorted, while the data across different iterations is not sorted.

-   API with SortConfig:
    -   Both RadixSort and MergeSort are stable sorting algorithms, that is, the relative order of equal values remains unchanged after sorting.
    -   The value destination operand, value source operand, index destination operand, and index source operand have the same number of elements, and the calCount parameter value cannot exceed the number of elements.
    -   Address overlap between the source operand and the destination operand is not supported.
    -   Overlapping of **sharedTmpBuffer** with the source operand and destination operand addresses is not supported.
    -   When the MERGE\_SORT algorithm is used for sorting, the number of elements to be sorted must be a multiple of 32. If it is not a multiple of 32, you need to manually pad the data volume to a multiple of 32.

## Examples

-   Handles 128 half-type data elements.

    This sample applies to:

    <!-- npu="950" id21 -->
    Ascend 950PR&950DT products
    <!-- end id21 -->

    <!-- npu="910b" id22 -->
    Atlas A2 products
    <!-- end id22 -->

    <!-- npu="A3" id23 -->
    Atlas A3 products
    <!-- end id23 -->

    ```
    // sortedLocal: sorting result
    // concatLocal: preprocessing result of the data to be sorted
    // indexLocal: index data
    // sortTmpLocal: temporary space.
    // Number of elements to be sorted.
    uint32_t m_elementCount = 128;
    // Number of iterations. Each iteration sorts 32 elements.
    uint32_t m_sortRepeatTimes = m_elementCount / 32;
    uint32_t m_extractRepeatTimes = m_elementCount / 32;
    // Preprocessing.
    AscendC::Concat(concatLocal, valueLocal, concatTmpLocal, m_concatRepeatTimes);
    // Perform sorting.
    AscendC::Sort<T, isFullSort>(sortedLocal, concatLocal, indexLocal, sortTmpLocal, m_sortRepeatTimes);
    AscendC::Extract(dstValueLocal, dstIndexLocal, sortedLocal, m_extractRepeatTimes);
    ```

    ```
    Example result.
    Input data (srcValueGm): 128 half-type data.
    [31 30 29 ... 2 1 0
     63 62 61 ... 34 33 32
     95 94 93 ... 66 65 64
     127 126 125 ... 98 97 96]
    Input data (srcIndexGm):
    [31 30 29 ... 2 1 0
     63 62 61 ... 34 33 32
     95 94 93 ... 66 65 64
     127 126 125 ... 98 97 96]
    Output data (dstValueGm):
    [127 126 125 ... 2 1 0]
    Output data (dstIndexGm):
    [127 126 125 ... 2 1 0]
    ```

-   Handles 64 half-type data.

    This sample applies to:

    <!-- npu="310p" id30 -->
    Atlas inference products AI Core
    <!-- end id30 -->

    ```
    uint32_t m_elementCount = 64;
    uint32_t m_sortRepeatTimes = m_elementCount / 16;
    uint32_t m_extractRepeatTimes = m_elementCount / 16;
    AscendC::Concat(concatLocal, valueLocal, concatTmpLocal, m_concatRepeatTimes);
    AscendC::Sort<T, isFullSort>(sortedLocal, concatLocal, indexLocal, sortTmpLocal, m_sortRepeatTimes);
    AscendC::Extract(dstValueLocal, dstIndexLocal, sortedLocal, m_extractRepeatTimes);
    ```

    ```
    Example result
    Input data (srcValueGm): 64 half-type data
    [15 14 13 ... 2 1 0
     31 30 29 ... 18 17 16
     47 46 45 ... 34 33 32
     63 62 61 ... 50 49 48]
    Input data (srcIndexGm):
    [15 14 13 ... 2 1 0
     31 30 29 ... 18 17 16
     47 46 45 ... 34 33 32
     63 62 61 ... 50 49 48]
    Output data (dstValueGm):
    [63 62 61 ... 2 1 0]
    Output data (dstIndexGm):
    [63 62 61 ... 2 1 0]
    ```

-   With SortConfig
    -   Handles 1024 half-type data elements, with the input index and output index being 1024 uint32\_t type data elements.

        This sample applies to:

        <!-- npu="950" id31 -->
        Ascend 950PR&950DT products
        <!-- end id31 -->

        ```
        static constexpr AscendC::SortConfig config = {AscendC::SortType::RADIX_SORT, false};
        Sort<T, false, config>(dstLocal, dstIndexLocal, srcLocal, 1024);
        ```

        ```
        Example result
        Input data (srcGm): 1024 half-type data elements
        [1023 1022 ... 2 1 0]
        Input data (srcIndexGm): 1024 uint32_t data elements
        [0 1 2 ... 1022 1023]
        Output data (dstGm):
        [0 1 2 ... 1022 1023]
        Output data (dstIndexGm):
        [1023 1022 ... 2 1 0]
        ```
