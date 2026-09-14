# Sort

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-08T17:17:30.997Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

-   Corresponds to the function prototype without the template parameter **SortConfig**.

    Sorting function that sorts values in descending order. The sorted data is stored in the following layout:

    Ascend 950PR/Ascend 950DT uses layout mode 1.

    Atlas A3 training products/Atlas A3 inference products use layout mode 1.

    Atlas A2 training products/Atlas A2 inference products use layout mode 1.

    The AI Core of Atlas inference products uses layout mode 2.

-   Layout mode 1:

    Each iteration can sort 32 numbers. The sorted scores and their corresponding indexes are stored in **dst** as a (score, index) structure. Regardless of whether **score** is of the **half** or **float** type, the (score, index) structure in **dst** always occupies 8 bytes of space, as shown below:

    -   When **score** is of the **float** type and **index** is of the **uint32** type, the **index** is stored in the upper 4 bytes and the **score** in the lower 4 bytes of the result.

        ![](../../../figures/zh-cn_image_0000002007005918.png)

    -   When score is of the half type and index is of the uint32 type, in the computation result, index is stored in the high 4 bytes, score is stored in the low 2 bytes, and the middle 2 bytes are reserved.

        ![](../../../figures/zh-cn_image_0000002042886661.png)

-   Layout mode 2: Region Proposal layout

    Both the input and output data are Region Proposals. One iteration can complete the sorting of 16 region proposals. Each Region Proposal occupies 8 consecutive half/float elements, with the following agreed format:

    ```
    [x1, y1, x2, y2, score, label, reserved_0, reserved_1]
    ```

    For the half data type, each Region Proposal occupies 16 bytes. Byte\[15:12\] is invalid data, and Byte\[11:0\] contains 6 half elements, where Byte\[11:10\] is defined as label, Byte\[9:8\] is defined as score, Byte\[7:6\] is defined as y2, Byte\[5:4\] is defined as x2, Byte\[3:2\] is defined as y1, and Byte\[1:0\] is defined as x1.

    As shown in the following figure, there are 16 Region Proposals in total.

    ![](../../../figures/zh-cn_image_0000002043918785.png)

    For the float data type, each Region Proposal occupies 32 bytes. Byte\[31:24\] is invalid data, and Byte\[23:0\] contains 6 float elements, where Byte\[23:20\] is defined as label, Byte\[19:16\] is defined as score, Byte\[15:12\] is defined as y2, Byte\[11:8\] is defined as x2, Byte\[7:4\] is defined as y1, and Byte\[3:0\] is defined as x1.

    As shown in the following figure, there are 16 Region Proposals in total.

    ![](../../../figures/zh-cn_image_0000002043797849.png)

-   Corresponds to the function prototype with the template parameter **SortConfig**.

    Only supported on Ascend 950PR/Ascend 950DT.

    According to the template parameter **SortConfig**, the input data is sorted using the sorting algorithm specified therein, and the sort result can be specified as ascending or descending order.

    When the function prototype has the output index **dstIndexTensor** parameter, the indexes corresponding to the sort result data need to be output. If the input has the index **srcIndexTensor** parameter, the output indexes are the indexes of the original input. If the input does not have an index, indexes are generated for the input data starting from 0 for the required number of sorted elements, and the final output indexes are the indexes of the corresponding input data. The following two figures show the sorting examples for input with an index and input without an index, respectively.

    **Figure 1**  Sorting example with input index srcIndex  
    ![](../../../figures/sorting-example-with-input-index-srcIndex.png "Sorting example with input index srcIndex")

    **Figure 2**  Sorting example without input index srcIndex  
    ![](../../../figures/sorting-example-without-input-index-srcIndex.png "Sorting example without input index srcIndex")

## Prototype

-   Without **SortConfig**

    ```
    template <typename T, bool isFullSort>
    __aicore__ inline void Sort(const LocalTensor<T>& dst, const LocalTensor<T>& concat, const LocalTensor<uint32_t>& index, LocalTensor<T>& tmp, const int32_t repeatTime)
    ```

-   With **SortConfig**

    Only supported on Ascend 950PR/Ascend 950DT.

    -   The API framework applies for temporary space.
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

        -   With the **srcIndexTensor** and **dstIndexTensor** parameters

            ```
            template <typename T, typename U, bool isReuseSource = false, const SortConfig& config = DEFAULT_SORT_CONFIG>
            __aicore__ inline void Sort(const LocalTensor<T>& dstTensor, const LocalTensor<U>& dstIndexTensor, const LocalTensor<T>& srcTensor, const LocalTensor<U>& srcIndexTensor, const uint32_t calCount)
            ```

    -   Pass the temporary space through the **sharedTmpBuffer** input parameter.
        -   Without the **srcIndexTensor** and **dstIndexTensor** parameters

            ```
            template <typename T, bool isReuseSource = false, const SortConfig& config = DEFAULT_SORT_CONFIG>
            __aicore__ inline void Sort(LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
            ```

        -   Without the **srcIndexTensor** parameter, with the **dstIndexTensor** parameter

            ```
            template <typename T, bool isReuseSource = false, const SortConfig& config = DEFAULT_SORT_CONFIG>
            __aicore__ inline void Sort(LocalTensor<T>& dstTensor, LocalTensor<uint32_t>& dstIndexTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
            ```

        -   With the **srcIndexTensor** and **dstIndexTensor** parameters

            ```
            template <typename T, typename U, bool isReuseSource = false, const SortConfig& config = DEFAULT_SORT_CONFIG>
            __aicore__ inline void Sort(LocalTensor<T>& dstTensor, LocalTensor<U>& dstIndexTensor, const LocalTensor<T>& srcTensor, const LocalTensor<U>& srcIndexTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
            ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by passing it via the sharedTmpBuffer input parameter**.

    -   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer applies for it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, offering high flexibility and high memory utilization.
    -   The API framework applies for temporary space. You do not need to apply for it, but must reserve the size of the temporary space.

    When the temporary space is passed through **sharedTmpBuffer**, you need to apply for space for the tensor. When the API framework applies for it, you need to reserve the temporary space. The size of the temporary space, **BufferSize**, is obtained as follows: obtain the size of the space range that needs to be reserved through the API provided in [GetSortMaxMinTmpSize](GetSortMaxMinTmpSize.md).

## Parameters

-   Corresponding to the prototype without **SortConfig**

    **Table 1**  Template parameter description

    | Parameter | Meaning |
    | --- | --- |
    | T | Data type of the operand. Supported data types are half and float. |
    | isFullSort | Whether to enable full sort mode. Full sort mode means sorting all inputs in descending order. In non-full-sort mode, refer to the **repeatTime** description in Table 2 for the sorting method. |

    **Table 2**  Parameter Description

    | Parameter | Input/Output | Description |
    | --- | --- | --- |
    | dst | Output | Destination operand, with shape [2n].<br>    <br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br>    <br>The start address of LocalTensor must be 32-byte aligned. |
    | concat | Input | Source operand, that is, the score in the API description, with shape [n].<br>    <br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br>    <br>The start address of LocalTensor must be 32-byte aligned.<br>    <br>The data type of this source operand must be consistent with that of the destination operand. |
    | index | Input | Source operand, with shape [n].<br>    <br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br>    <br>The start address of LocalTensor must be 32-byte aligned.<br>    <br>This source operand is fixed to the uint32_t data type. |
    | tmp | Input | Temporary space. It is used to store intermediate variables during complex computation inside the API and is provided by the developer. For how to obtain the temporary space size BufferSize, see [GetSortTmpSize](GetSortTmpSize.md). The data type is consistent with that of the source operand.<br>    <br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br>    <br>The start address of LocalTensor must be 32-byte aligned. |
    | repeatTime | Input | Number of repeated iterations, of the int32_t type.<br>    Ascend 950PR/Ascend 950DT: Each iteration completes the sorting of 32 elements. In the next iteration, concat and index each skip 32 elements, and dst skips 32*8 bytes of space. Value range: repeatTime∈[0,255].<br>    Atlas A3 training products/Atlas A3 inference products: Each iteration completes the sorting of 32 elements. In the next iteration, concat and index each skip 32 elements, and dst skips 32*8 bytes of space. Value range: repeatTime∈[0,255].<br> Atlas A2 training products/Atlas A2 inference products: Each iteration completes the sorting of 32 elements. In the next iteration, concat and index each skip 32 elements, and dst skips 32*8 bytes of space. Value range: repeatTime∈[0,255].<br>Atlas inference products AI Core: Each iteration completes the sorting of 16 region proposals. In the next iteration, concat and dst each skip 16 region proposals. Value range: repeatTime∈[0,255]. |

-   Corresponding to the function prototype with SortConfig

    **Table 3**  Template parameter description

    | Parameter | Description |
    | --- | --- |
    | T | Data type of the operands srcTensor and dstTensor. The data types supported by the RADIX_SORT sorting algorithm are: uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, bfloat16_t, float, uint64_t, int64_t. The data types supported by the MERGE_SORT sorting algorithm are: half, float. |
    | U | Data type of the operands srcIndexTensor and dstIndexTensor. The data types supported by the RADIX_SORT sorting algorithm are: uint32_t, int32_t, uint64_t, int64_t. The data types supported by the MERGE_SORT sorting algorithm are: uint32_t. |
    | isReuseSource | Optional parameter. Whether the input Tensor space can be reused. |
    | config | Optional parameter. Corresponding configuration of the Sort API: the selected sorting algorithm and the ascending/descending order of the sort result. The data type is SortConfig, defined as shown in the following code.<br>    <br>Sort provides two different sorting algorithms: the MERGE_SORT merge sort algorithm and the RADIX_SORT radix sort algorithm. The two algorithms differ in execution speed, time complexity, and algorithm stability.<br>    MERGE_SORT is a stable sorting algorithm whose time complexity is O(nlogn) in all cases.<br>The time complexity of the RADIX_SORT algorithm is O(n). When processing a large amount of data, if the maximum number has few digits, this algorithm is highly efficient and can approach linear time complexity. However, if the maximum number has many digits, the time complexity approaches O(n^2).<br>    <br>The default value DEFAULT_SORT_CONFIG of config is as follows: it uses the radix sort RADIX_SORT and sorts the sort result in ascending order.<br>    constexpr SortConfig DEFAULT_SORT_CONFIG = {SortType::RADIX_SORT, false}; |

    ```
    enum class SortType {
        RADIX_SORT,  // Use the radix sort algorithm.
        MERGE_SORT   // Use the merge sort algorithm.
    };
    struct SortConfig {
        SortType type = SortType::RADIX_SORT; // Sorting algorithm
        bool isDescend = false; // Whether to sort in descending order. The default value is false, and the output result is sorted in ascending order.
    };
    ```

    **Table 4**  Parameter description

    | Parameter | Input/Output | Description |
    | --- | --- | --- |
    | dstTensor | Output | Value destination operand, with shape [n]. Under the MERGE_SORT algorithm, each element of the output data must reserve 8 bytes of space.<br>    <br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br>    <br>The start address of LocalTensor must be 32-byte aligned. |
    | dstIndexTensor | Output | Index destination operand, with shape [n]. When the input does not carry srcIndexTensor, only the uint32_t type is supported.<br>    <br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br>    <br>The start address of LocalTensor must be 32-byte aligned. |
    | srcTensor | Input | Value source operand, with shape [n].<br>    <br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br>    <br>The start address of LocalTensor must be 32-byte aligned.<br>    <br>The data type of this source operand must be consistent with that of the value destination operand. |
    | srcIndexTensor | Input | Index source operand, with shape [n].<br>    <br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br>    <br>The start address of LocalTensor must be 32-byte aligned.<br>    <br>The data type of this source operand must be consistent with that of the index destination operand. |
    | sharedTmpBuffer | Input | Temporary space. It is used to store intermediate variables during complex computation inside the API and is provided by the developer. For how to obtain the temporary space size BufferSize, see [GetSortMaxMinTmpSize](GetSortMaxMinTmpSize.md). The data type is uint8_t.<br>    <br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br>    <br>The start address of LocalTensor must be 32-byte aligned. |
    | calCount | Input | Number of data elements to be sorted. Data type: uint32_t. |

## Return Value

None

## Constraints

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   API without **SortConfig**:
    -   When score\[i\] is the same as score\[j\], if i\>j, then score\[j\] is selected first and placed in front, that is, the order of **index** is consistent with the input order.
    -   In non-full-sort mode, data within each iteration is sorted, while data across different iterations is not sorted.

-   API with **SortConfig**:
    -   Both radix sort **RadixSort** and merge sort **MergeSort** are stable sorts, that is, the relative order of equal values remains unchanged after sorting.
    -   The value destination operand, value source operand, index destination operand, and index source operand have the same number of elements, and the value of the **calCount** parameter cannot exceed the number of elements.
    -   Address overlap between the source operand and the destination operand is not supported.
    -   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
    -   When the MERGE\_SORT algorithm is used for sorting, the number of elements to be sorted must be a multiple of 32. If it is not a multiple of 32, the user needs to manually pad the data volume to a multiple of 32.

## Examples

-   Process 128 half-type data elements.

    This example applies to:

    Ascend 950PR/Ascend 950DT

    Atlas A2 training products/Atlas A2 inference products

    Atlas A3 training products/Atlas A3 inference products

    ```
    // sortedLocal: Sort Result
    // concatLocal: Preprocessing result of the data to be sorted
    // indexLocal: Index data
    // sortTmpLocal: Temporary Space
    // Number of elements to be sorted
    uint32_t m_elementCount = 128;
    // Number of iterations. Each Iteration sorts 32 elements.
    uint32_t m_sortRepeatTimes = m_elementCount / 32;
    uint32_t m_extractRepeatTimes = m_elementCount / 32;
    // Preprocess.
    AscendC::Concat(concatLocal, valueLocal, concatTmpLocal, m_concatRepeatTimes);
    // Perform sorting.
    AscendC::Sort<T, isFullSort>(sortedLocal, concatLocal, indexLocal, sortTmpLocal, m_sortRepeatTimes);
    AscendC::Extract(dstValueLocal, dstIndexLocal, sortedLocal, m_extractRepeatTimes);
    ```

    ```
    Example Result
    Input data (srcValueGm): 128 half-type data elements
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

-   Processes 64 half-type data elements.

    This example applies to:

    Atlas inference products AI Core

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
    -   Processes 1024 half data items, with 1024 uint32\_t data for both input and output indexes.

        This example applies to:

        Ascend 950PR/Ascend 950DT

        ```
        static constexpr AscendC::SortConfig config = {AscendC::SortType::RADIX_SORT, false};
        Sort<T, false, config>(dstLocal, dstIndexLocal, srcLocal, 1024);
        ```

        ```
        Example result
        Input data (srcGm): 1024 half-type data items
        [1023 1022 ... 2 1 0]
        Input data (srcIndexGm): 1024 uint32_t-type data items
        [0 1 2 ... 1022 1023]
        Output data (dstGm):
        [0 1 2 ... 1022 1023]
        Output data (dstIndexGm):
        [1023 1022 ... 2 1 0]
        ```
