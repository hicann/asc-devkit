# TopK

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-08T17:30:27.055Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the top k largest or smallest values along the last dimension and their corresponding indices.

If the input is a vector, the top k maximum or minimum values and their corresponding indices are found in the vector. If the input is a matrix, the top k maximum or minimum values and their corresponding indices in each row are computed along the last dimension. **This API supports input of up to two dimensions and does not support higher-dimensional input.**

As shown in the following figure, a two-dimensional matrix with the shape \(4, 32\) is sorted, k is set to 1, and the output result is \[\[32\] \[32\] \[32\] \[32\]\].

![](../../../figures/nz-reduce-48.png)

-   **Prerequisite Concepts**

    Based on the preceding sample, we introduce some prerequisite concepts: the number of rows is called the **outer axis length (outter)**, and the actual number of elements in each row is called the **actual inner axis length (n)**. This API requires the input inner axis length to be an integer multiple of 32. Therefore, when n is not an integer multiple of 32, you need to pad upward to an integer multiple of 32, and the padded length is called the **inner axis length (inner)**. For example, in the following sample, the actual length n of each row is 31, which is not an integer multiple of 32. After padding upward, inner becomes 32, and padding in the figure represents the padding operation. The relationship between n and inner is as follows: when n is an integer multiple of 32, inner = n; otherwise, inner \> n.

    ![](../../../figures/nz-reduce-49.png)

-   **API Modes**

    This API supports two modes: **Normal mode** and **Small mode**. Normal mode is a general-purpose mode. Small mode is a high-performance mode provided for scenarios where the inner axis length is fixed at 32 (unit: number of elements). Because inner is fixed at 32 in Small mode, more targeted processing can be performed, so there are fewer related constraints and higher performance. It is recommended to use Small mode when the inner axis length is 32.

-   **Additional Feature**: This API allows you to specify that the sorting of certain rows is invalid. This is controlled by passing the **finishLocal** parameter value. When the value of the row corresponding to **finishLocal** is true, the sorting of that row is invalid, and the k index values of **dstIndexLocal** output after sorting are all set to the invalid index n.

    ![](../../../figures/nz-reduce-50.png)

## Implementation Principle

TopK provides two different sorting algorithms, the MERGE\_SORT algorithm and the RADIX\_SELECT algorithm, which differ in execution speed, time complexity, and algorithm stability. The time complexity of the MERGE\_SORT algorithm is O\(![](../../../figures/zh-cn_formulaimage_0000002108561704.png)\), and it is a stable sorting algorithm with relatively slow execution speed; the time complexity of the RADIX\_SELECT algorithm is O\(n\), and it generally does not care about stability, with faster execution speed.

-   **MERGE\_SORT Algorithm**

    Taking the input Tensor of float type, ND format, and shape \[outter,  inner\] as an example, the internal algorithm block diagram of the TopK high-level API is described as shown in the following figure.

    **Figure 1**  TopK algorithm block diagram  
    ![](../../../figures/TopK_algorithm_block_diagram.png "TopK algorithm block diagram")

    **Figure 2**  TopK algorithm block diagram  
    ![](../../../figures/TopK_algorithm_block_diagram_51.png "TopK algorithm block diagram-51")

    Based on the selected TopKMode, it can be divided into two branches.

    -   Calculate the TopK NORMAL mode. The process is as follows:

        1.  When the template parameter **isInitIndex** is **false**, indices from 0 to inner - 1 need to be generated.

            Ascend 950PR/Ascend 950DT adopt method 1.

            Atlas A3 training products/Atlas A3 inference products adopt method 2.

            Atlas A2 training products/Atlas A2 inference products adopt method 2.

            Atlas inference products adopt method 2.

            -   Method 1: Use **CreateVecIndex** to generate indices from 0 to **inner** - 1.
            -   Method 2: Use **Arange** to generate indices from 0 to **inner** - 1.

        2.  When the **isLargest** parameter is **false**, since the **Sort32** instruction sorts in descending order by default, multiply the data by -1.
        3.  Complete full sorting of the input data.

            Ascend 950PR/Ascend 950DT adopts method 1.

            Atlas A3 training products/Atlas A3 inference products adopt method 2.

            Atlas A2 training products/Atlas A2 inference products adopt method 2.

            Atlas inference products adopt method 2.

            Method 1:

            Use the high-level API **Sort** to fully sort the data.

            Method 2:

            1.  Use **Sort32** to sort the data, ensuring that every 32 data items are ordered.
            2.  Use the **MrgSort** instruction to merge-sort all sorted data blocks.

        4.  Use the **GatherMask** instruction to extract the first k data items and their indices;
        5.  When finishLocal\[i\] is true, update the sorting result corresponding to this row to the invalid index n;
        6.  If the **isLargest** parameter is **false**, multiply the data by -1 to restore the data.

        Note: On Atlas inference products, use the **ProposalConcat** basic API to combine data and index, then use the **RpSort16** basic API to sort the data; use **MrgSort4** for merging; and use the **ProposalExtract** basic API to extract data and index.

    -   Compute the TopK SMALL mode as follows:

        1.  When the template parameter **isInitIndex** is **false**, generate indices from 0 to inner - 1, and use the **Copy** instruction to replicate the data into **outter** items;

            Ascend 950PR/Ascend 950DT adopts method 1.

            Atlas A3 training products/Atlas A3 inference products adopt method 2.

            Atlas A2 training products/Atlas A2 inference products adopt method 2.

            Atlas inference products adopt method 2.

            -   Method 1: Use **CreateVecIndex** to generate indices from 0 to inner - 1.
            -   Method 2: Use **Arange** to generate indices from 0 to inner - 1.

        2.  When the **isLargest** parameter is **false**, since the **Sort32** instruction sorts in descending order by default, multiply the input data by -1;
        3.  Use **Sort32** to sort the data;
        4.  Use the GatherMask instruction to extract the first k data items and indices;
        5.  If the **isLargest** parameter is **false**, multiply the input data by -1 to restore the data.

        Note: On Atlas inference products, use the ProposalConcat basic API to combine data and index, and then use the RpSort16 basic API to sort the data. Because inner is 32 in small mode and RpSort16 leaves the data sorted in groups of 16, use the MrgSort4 basic API to perform a merge-sort between step 3 and step 4.

-   **RADIX\_SELECT Algorithm**

    This algorithm is supported only on Ascend 950PR/Ascend 950DT.

    **Figure 3** TopK algorithm block diagram  
    ![](../../../figures/TopK_algorithm_block_diagram_52.png "TopK algorithm block diagram-52")

    The computation process consists of the following steps:

    1.  Generate indices. Depending on the TopKMode, the process is divided into:
        -   TopK NORMAL mode: The template parameter **isInitIndex** is **false**. Use **CreateVecIndex** to generate indices from 0 to inner-1.
        -   TopK SMALL mode: The template parameter **isInitIndex** is **false**. Use CreateVecIndex to generate indices from 0 to inner-1, and use the Copy instruction to copy the data into outter items.

    2.  Determine whether to preprocess the data according to the template parameter. If the data type is a floating-point type or a signed integer type, perform the twiddle in operation (convert the floating-point number or signed integer to an unsigned integer type). If TopK obtains the minimum values, perform a negation operation on the unsigned integer type data;
    3.  Find the K-th largest value and save it to kValue;
    4.  Extract the data and indices greater than kValue;
    5.  Extract the data and indices equal to kValue;
    6.  Determine whether to sort the k values according to the template parameter sorted, and store the sorted data results in the temporary space;
    7.  According to the template parameter, if TopK obtains the minimum values, perform a negation operation; if the data type is a floating-point type or a signed integer type, perform the twiddle out operation (convert the unsigned integer type to a floating-point number or a signed integer type).

## Prototype

-   The API applies for temporary space internally

    ```
    template <typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false, enum TopKMode topkMode = TopKMode::TOPK_NORMAL>
    __aicore__ inline void TopK(const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal, const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const int32_t k, const TopkTiling& tilling, const TopKInfo& topKInfo, const bool isLargest = true)
    ```

    ```
    template <typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false, enum TopKMode topkMode = TopKMode::TOPK_NORMAL, const TopKConfig& config = defaultTopKConfig>
    __aicore__ inline void TopK(const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal, const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const int32_t k, const TopkTiling& tilling, const TopKInfo& topKInfo, const bool isLargest = true)
    ```

-   Pass temporary space through the tmpLocal input parameter

    ```
    template <typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false, enum TopKMode topkMode = TopKMode::TOPK_NORMAL>
    __aicore__ inline void TopK(const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal, const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const LocalTensor<uint8_t>& tmpLocal, const int32_t k, const TopkTiling& tilling, const TopKInfo& topKInfo, const bool isLargest = true)
    ```

    ```
    template <typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false, enum TopKMode topkMode = TopKMode::TOPK_NORMAL, const TopKConfig& config = defaultTopKConfig>
    __aicore__ inline void TopK(const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal, const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const LocalTensor<uint8_t>& tmpLocal, const int32_t k, const TopkTiling& tilling, const TopKInfo& topKInfo, const bool isLargest = true)
    ```

Because the internal implementation of this API involves complex logical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space supports two methods: **application by the API** and **passing through the tmpLocal input parameter** by you.

-   The API applies for temporary space internally. You do not need to apply for it, but must reserve the size of the temporary space.

-   Passed in through the **tmpLocal** input parameter. This tensor is used as the temporary space for processing, and the API does not apply for additional space internally. With this method, you can manage the **tmpLocal** memory space by yourself and reuse this memory after the API call completes. The memory is not repeatedly applied for and released, offering high flexibility and high memory utilization. The method for obtaining the temporary space size, that is, the BufferSize of **tmpLocal**, is as follows: use the **GetTopKMaxMinTmpSize** API provided in [TopK Tiling](TopK-Tiling.md) to obtain the required maximum and minimum temporary space sizes.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the data to be sorted. For the data types supported by different models, see [Supported Data Types](#li197551749192811). |
| isInitIndex | Whether to pass in the index of the input data.<br>**true** indicates that the index is passed in. When set to **true**, the index of the input data must be passed in through the **srcIndexLocal** parameter. For specific rules, see the description of the **srcIndexLocal** parameter in Table 2.<br>**false** indicates that the index is not passed in, and the index output by the TopK API is unavailable. |
| isHasfinish | The TopK API allows you to specify, through the **finishLocal** parameter, that the sorting of certain rows is invalid. This template parameter controls whether to enable the preceding function. **true** indicates enabled, and **false** indicates disabled.<br>Supported values in Normal mode: **true** / **false**.<br>Supported value in Small mode: **false**.<br>For the combined usage of the **isHasfinish** parameter and **finishLocal**, see the description of the **finishLocal** parameter in Table 2.<br><br>For Ascend 950PR/Ascend 950DT, with the RADIX_SELECT algorithm, this parameter is reserved and not yet enabled. It is reserved for future function extension. Keep the default value. |
| isReuseSrc | Whether to allow modification of the source operand. The default value is **false**.<br>For Ascend 950PR/Ascend 950DT, this parameter takes effect only when the input data type is float. Values are as follows:<br>**true**: You allow the source operand to be overwritten. This parameter can be set to **true** to enable this. After it is enabled, the API reuses the memory space of **srcTensor** during internal computation, saving some memory space.<br>**false**: The API does not reuse the memory space of **srcTensor** during internal computation.<br>For Atlas A3 training products/Atlas A3 inference products, this parameter is reserved. Pass the default value **false**.<br>For Atlas A2 training products/Atlas A2 inference products, this parameter is reserved. Pass the default value **false**.<br>For Atlas inference products AI Core, this parameter is reserved. Pass the default value **false**. |
| topkMode | Mode selection of TopK. Values are as follows:<br>**TopKMode::TOPK_NORMAL**: Normal mode.<br>**TopKMode::TOPK_NSMALL**: Small mode. |
| config | This parameter is supported only on Ascend 950PR/Ascend 950DT.<br><br>Configuration related to TopK computation, including algorithm selection, obtaining maximum or minimum values, and whether to sort the results. This parameter is optional and of the **TopKConfig** type. Its specific definition is shown in the following code:<br>**algo**: Sorting algorithm to be selected. The default is the MERGE_SORT algorithm. Currently only the RADIX_SELECT algorithm is supported, and users must explicitly specify **algo** as **TopKAlgo::RADIX_SELECT**.<br>**order**: Indicates whether to obtain the top k maximum values or the top k minimum values. Values are as follows: **UNSET**: Default value, implemented according to the configuration of the function parameter **isLargest**. When **isLargest** is **true**, the top k maximum values and their corresponding indices are obtained. When **isLargest** is **false**, the top k minimum values and their corresponding indices are obtained. **LARGEST**: Indicates obtaining the top k maximum values and their corresponding indices. When the value is **LARGEST**, the configuration of the function parameter **isLargest** does not take effect. **SMALLEST**: Indicates obtaining the top k minimum values and their corresponding indices. When the value is **SMALLEST**, the configuration of the function parameter **isLargest** does not take effect.<br>**sorted**: Indicates whether to sort the output results. When the value is **true**, the output results are sorted. When the value is **false**, the output results are not sorted.<br><br>The default value **defaultTopKConfig** of this parameter is: **TopKAlgo::MERGE_SORT**, **TopKOrder::UNSET**, **true**. |

```
struct TopKConfig {
    TopKAlgo algo = TopKAlgo::MERGE_SORT;
    TopKOrder order = TopKOrder::UNSET;
    bool sorted = true;
};
enum class TopKAlgo {
    RADIX_SELECT,
    MERGE_SORT
};
enum class TopKOrder {
    UNSET,
    LARGEST,
    SMALLEST
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstValueLocal | Output | Destination operand. Used to save the k values obtained by sorting.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Normal mode:<br>The output shape is outter * k_pad, that is, outter data items are output, and the length of each data item is k_pad. k_pad is the value obtained by padding k upward to 32-byte alignment based on the input data type.<br>You need to allocate space of size k_pad * outter * sizeof(T) for **dstValueLocal**.<br>The first k values of each output data item are the top k maximum/minimum values of that item. The k+1 to k_pad elements of each data item are not filled with values and are random values.<br>The calculation method of k_pad is as follows:<br>When the types of the input **srcLocal** and **dstValueLocal** are float, float is 4 bytes. Therefore, k is rounded up to the nearest multiple of 8 to obtain k_pad, which can satisfy 32-byte alignment.<br>k_pad = (k + 7) / 8 * 8<br>When the types of the input **srcLocal** and **dstValueLocal** are half, half is 2 bytes. Therefore, k is rounded up to the nearest multiple of 16 to obtain k_pad, which can satisfy 32-byte alignment.<br>    k_pad = (k + 15) / 16 * 16<br><br>Small mode:<br>The output shape is outter * k, that is, outter data items are output, and the length of each data item is k.<br>The output values require space of size k * outter * sizeof(T) for storage. You need to allocate the actual memory space for **dstValueLocal** based on this size and the alignment requirements of the framework.<br>**Note the following: Follow the framework's requirements for memory allocation (the size of the allocated memory must satisfy 32-byte alignment). That is, when k * outter * sizeof(T) is not 32-byte aligned, it needs to be padded upward to 32-byte alignment. The extra memory space allocated for alignment is not filled with values and contains random values.** |
| dstIndexLocal | Output | Destination operand. Used to save the indices corresponding to the k values obtained by sorting.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Normal mode:<br>The output shape is outter * kpad_index, that is, outter data items are output, and the length of each data item is kpad_index. kpad_index is the value obtained by padding k upward to 32-byte alignment based on the input index type.<br>You need to allocate space of size kpad_index * outter * sizeof(int32_t) for **dstIndexLocal**.<br>The first k values of each data item are the indices corresponding to the top k maximum/minimum values of that item. The k+1 to kpad_index indices of each data item are not filled with values and are random values.<br>The calculation method of k_pad is as follows: Since **dstIndexLocal** is of the int32_t type, which is 4 bytes, k is rounded up to the nearest multiple of 8 to obtain kpad_index, which can satisfy 32-byte alignment.<br>kpad_index = (k + 7) / 8 * 8<br><br>Small mode:<br>The output shape is outter * k, that is, outter data items are output, and the length of each data item is k.<br>The output indices require space of size k * outter * sizeof(int32_t) for storage. You need to allocate the actual memory space for **dstIndexLocal** based on this size and the alignment requirements of the framework.<br>**Note: Follow the framework's requirements for memory allocation (the size of the allocated memory must satisfy 32-byte alignment). That is, when k * outter * sizeof(int32_t) is not 32-byte aligned, it needs to be padded upward to 32-byte alignment. The extra memory space allocated for alignment is not filled with values and contains random values.** |
| srcLocal | Input | Source operand. Used to save the values to be sorted.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>On Atlas inference products AI Core:<br>The shape of the input data is outter * inner. You need to allocate space of size outter * inner * sizeof(T) for it.<br>When n < inner, you need to pad the outter data items in **srcLocal**, and each data item needs to be padded from n to the inner length.<br>Padding rule: The filled data must not affect the overall sorting. It is recommended to use the following filling method: when obtaining the top k maximum values, the filled value needs to be the minimum value of the **input data type**; when obtaining the top k minimum values, the filled value needs to be the maximum value of the **input data type**. |
| srcIndexLocal | Input | Source operand. Used to save the indices corresponding to the values to be sorted.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>This parameter is used together with the template parameter **isInitIndex**. When **isInitIndex** is **false**, **srcIndexLocal** only needs to be defined and does not need to be assigned a value. Pass the defined **srcIndexLocal** to the API. When **isInitIndex** is **true**, you need to pass in the index values through the **srcIndexLocal** parameter. The rules for setting the **srcIndexLocal** parameter are as follows:<br><br>Normal mode:<br>The shape of the input index data is 1 * inner. Here, all outter data items use the same index. You need to allocate space of size inner * sizeof(int32_t) for it.<br>When n < inner, you need to pad the index data, padding this data item from n to the inner length.<br>Padding rule: The filled indices must not affect the overall sorting. It is recommended to use the following filling method: the filled values increase based on the original indices. For example, if the original indices are 0, 1, 2, ..., n-1, the padded indices are 0, 1, 2, ..., n, n + 1, ..., inner-1.<br><br>Small mode:<br>The shape of the input index data is outter * inner. You need to allocate space of size outter * inner * sizeof(int32_t) for it.<br>When n < 32, you need to pad the outter data items, and each data item needs to be padded from n to a length of 32.<br>Padding rule: The filled data must not affect the overall sorting. It is recommended to use the following filling method: the filled values increase based on the original indices. For example, if the original indices are 0, 1, 2, ..., n-1, the padded indices are 0, 1, 2, ..., n, n + 1, ..., inner-1. |
| finishLocal | Input | Source operand. Used to specify that the sorting of certain rows is invalid. Its shape is (outter, 1).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>For Ascend 950PR/Ascend 950DT, this parameter is reserved and not yet enabled. It is reserved for future function extension, and its value is **false**.<br><br>This parameter is used together with the template parameter **isHasfinish**. In Normal mode, **isHasfinish** can be configured as **true**/**false**. In Small mode, only **false** is supported for **isHasfinish**.<br>When **isHasfinish** is configured as **true**:<br>When the value of the outter row corresponding to **finishLocal** is **true**, the sorting of that row is invalid, and the k index values output in **dstIndexLocal** after sorting are all set to n.<br>When the value of the outter row corresponding to **finishLocal** is **false**, the sorting of that row is valid.<br><br>When **isHasfinish** is configured as **false**, **finishLocal** only needs to be defined and does not need to be assigned a value. Pass the defined **finishLocal** to the API. A definition example is as follows: LocalTensor<bool> finishLocal; |
| tmpLocal | Input | Temporary space. Used to store intermediate variables during complex internal computation of the API. It is provided by you. For the method of obtaining the temporary space size, see [TopK Tiling](TopK-Tiling.md). The data type is fixed to uint8_t.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). **The logical position supports only VECCALC and does not support other logical positions.** |
| k | Input | Obtains the top k maximum or minimum values and their corresponding indices. The data type is int32_t.<br><br>The size of k should satisfy: 1 <= k <= n. |
| tilling | Input | Tiling information required for TopK computation. For the method of obtaining the Tiling information, see [TopK Tiling](TopK-Tiling.md). |
| topKInfo | Input | Shape information of **srcLocal**. It is of the **TopKInfo** type. Its specific definition is shown in the following code, where the meanings of the parameters are as follows:<br>**outter**: Indicates the outer axis length of the input data to be sorted.<br>**inner**: Indicates the inner axis length of the input data to be sorted. **inner** must be an integer multiple of 32.<br>**n**: Indicates the actual length of the inner axis of the input data to be sorted.<br>Note the following:<br>**topKInfo.inner** must be an integer multiple of 32.<br>**topKInfo.inner** is the value obtained by padding **topKInfo.n** upward to an integer multiple of 32. Therefore, the size of **topKInfo.n** should satisfy: 1 <= **topKInfo.n** <= **topKInfo.inner**.<br>In Small mode, **topKInfo.inner** must be set to 32.<br>In Normal mode, the maximum value of **topKInfo.inner** is 4096. |
| isLargest | Input | The type is bool. When the value is **true**, descending order is used by default, and the top k maximum values are obtained. When the value is **false**, ascending order is used, and the top k minimum values are obtained. |

```
struct TopKInfo {
    int32_t outter = 1;
    int32_t inner; 
    int32_t n; 
};
```

## Return Value

None

## Constraints

-   For the operand address offset alignment requirements, see [General Description and Constraints](../../general_description_and_constraints.md).
-   **Address overlap between the source operand and the destination operand is not supported.**
-   When srcLocal\[i\] and srcLocal\[j\] are the same, if i\>j, then srcLocal\[j\] will be selected first and placed in front.
-   inf is considered a maximum value in TopK.
-   In TopK sorting, nan is placed in front regardless of whether the order is descending or ascending.
-   For Atlas inference products AI Core:
    -   When the input **srcLocal** type is half and the template parameter **isInitIndex** is **false**, the passed-in **topKInfo.inner** cannot be greater than 2048.

    -   When the input **srcLocal** type is half and the template parameter **isInitIndex** is **true**, the index values in the passed-in **srcIndexLocal** cannot be greater than 2048.

-   Supported data types<a id="li197551749192811"></a>

    Ascend 950PR/Ascend 950DT: For the MERGE\_SORT algorithm, the currently supported data types are half and float. For the RADIX\_SELECT algorithm, the currently supported data types are uint8\_t, int8\_t, uint16\_t, int16\_t, uint32\_t, int32\_t, bfloat16\_t, half, float, uint64\_t, and int64\_t.

    Atlas A3 training products/Atlas A3 inference products support the following data types: half and float.

    Atlas A2 training products/Atlas A2 inference products support the following data types: half and float.

    Atlas inference products AI Core support the following data types: half and float.

## Examples

This sample implements the Normal mode code logic.

```
// dstLocalValue: Save the k sorted values.
// dstLocalIndex: Save the indices corresponding to the k sorted values.
// srcLocal: Save the values to be sorted.
// srcLocalIndex: Save the indices corresponding to the values to be sorted.
// srcLocalFinish: Used to specify that the sorting of certain rows is invalid, with a shape of (outter, 1).
// sharedTmpBuffer: Tensor that stores the temporary buffer during sorting.
// Obtain the top k maximum values or minimum values and their corresponding indices.
// topKTiling: Stores the tiling information required for TopK computation, which can be obtained through the TopKTilingFunc API.
// topKInfo: Shape information of srcLocal.
// isLargest: When set to true, sorts in descending order by default and obtains the top k maximum values; when set to false, sorts in ascending order and obtains the top k minimum values.

// Pass the temporary space through the sharedTmpBuffer input parameter.
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NORMAL>(dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, sharedTmpBuffer, k, topKTilingData, topKInfo, isLargest);
// The API framework applies for temporary space.
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NORMAL>(dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, k, topKTilingData, topKInfo, isLargest);
```

```
// dstLocalValue: Stores the k sorted values.
// dstLocalIndex: Stores the indices corresponding to the k sorted values.
// srcLocal: Stores the values to be sorted.
// srcLocalIndex: Stores the indices corresponding to the values to be sorted.
// srcLocalFinish: Specifies that the sorting of certain rows is invalid, with a shape of (outter, 1).
// sharedTmpBuffer: Stores the Tensor used as a temporary buffer during sorting.
// Obtain the top k maximum values or minimum values and their corresponding indices.
// topKTiling: Stores the Tiling information required for TopK computation, which can be obtained through the TopKTilingFunc API.
// topKInfo: Shape information of srcLocal.
// isLargest: When set to true, sorts in descending order by default and obtains the top k maximum values; when set to false, sorts in ascending order and obtains the top k minimum values.
// defaultTopKConfig: Related configuration for TopK computation, including algorithm selection, obtaining maximum or minimum values, and whether to sort the results.

// Pass the temporary space through the sharedTmpBuffer input parameter.
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NORMAL, defaultTopKConfig>(dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, sharedTmpBuffer, k, topKTilingData, topKInfo, isLargest);
// The API framework applies for temporary space.
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NORMAL, defaultTopKConfig>(dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, k, topKTilingData, topKInfo, isLargest);
```

**Table 3** Sample analysis of Normal mode

| Sample Description | This sample sorts a matrix with shape (2, 32) and data type float, and obtains the first 5 minimum values of each row.<br><br>It uses the Normal mode API. You pass the input data index and pass finishLocal to specify that the sorting of certain rows is invalid. |
| --- | --- |
| Input | Template parameter T: float; template parameter isInitIndex: true; template parameter isHasfinish: true; template parameter topkMode: TopKMode::TOPK_NORMAL; template parameter topkConfig: defaultTopKConfig.<br>Input data finishLocal:<br>[False  True  False  False False False  False False False  False False False <br> False  False False False False False False False False False False False <br> False False False False False False False False]<br><br>**Note: The DataCopy transfer amount must be an integer multiple of 32 bytes. Therefore, the actual valid input of finishLocal here is the first two values False and True, and the remaining values are padded upward to 32 bytes and do not actually participate in the computation.**<br>Input data k: 5<br>Input data topKInfo:<br>int32_t outter = 2;<br>int32_t inner = 32;<br>int32_t n = 32;<br>Input data isLargest: false<br>Input data srcLocal:<br>[[-18096.555   -11389.83    -43112.895   -21344.77     57755.918<br>   50911.145    24912.621   -12683.089    45088.004   -39351.043<br>  -30153.293    11478.329    12069.15     -9215.71     45716.44<br>  -21472.398   -37372.16    -17460.414    22498.03     21194.838<br>  -51229.17    -51721.918   -47510.38     47899.11     43008.176<br>    5495.8975  -24176.97    -14308.27     53950.695     7652.6035<br>  -45169.168   -26275.518  ]<br> [ -9196.681   -31549.518    18589.23    -12427.927    50491.81<br>  -20078.11    -25606.107   -34466.773   -42512.805    50584.48<br>   35919.934   -17283.5       6488.137   -12885.134     1942.2147<br>  -50611.96     52671.477    23179.662    25814.875      -69.73492<br>   33906.797   -34662.61     46168.71    -52391.258    57435.332<br>   50269.414    40935.05     21164.176     4028.458   -29022.918<br>  -46391.133     1971.2042 ]]<br>Input data srcIndexLocal:<br>[ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23<br> 24 25 26 27 28 29 30 31] |
| Output Data | Output data dstValueLocal is as follows. The length of each row is k_pad, where the first 5 values of each data item are the first 5 minimum values of that item. The last three values are random values.<br>[[-51721.918 -51229.17  -47510.38  -45169.168 -43112.895     0.     0.   0.   ]<br> [-52391.258 -50611.96  -46391.133 -42512.805 -34662.61      0.     0.   0.   ]]<br>Output data dstIndexLocal is as follows.<br>The length of each row is kpad_index, where the first 5 values of each data item are the indices corresponding to the first 5 minimum values of that item. The last three values are random values.<br>Because the finishLocal corresponding to the second row of data is true, the sorting of the second row of data is invalid, so all its output index values are the actual inner axis length 32.<br>[[21 20 22 30  2  0  0  0]<br> [32 32 32 32 32  0  0  0]] |

This sample implements the code logic of Small mode.

```
// dstLocalValue: Stores the k sorted values.
// dstLocalIndex: Stores the indices corresponding to the k sorted values.
// srcLocal: Stores the values to be sorted.
// srcLocalIndex: Stores the indices corresponding to the values to be sorted.
// srcLocalFinish: Specifies that the sorting of certain rows is invalid. Its shape is (outter, 1).
// sharedTmpBuffer: Stores the Tensor used as a temporary buffer during sorting.
// Obtain the top k maximum values or minimum values and their corresponding indices.
// topKTiling: Stores the Tiling information required for TopK computation, which can be obtained through the TopKTilingFunc API.
// topKInfo: Shape information of srcLocal.
// isLargest: When set to true, sorts in descending order by default and obtains the top k maximum values; when set to false, sorts in ascending order and obtains the top k minimum values.

// Pass the temporary space through the sharedTmpBuffer input parameter.
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NSMALL>(dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, sharedTmpBuffer, k, topKTilingData, topKInfo, isLargest);
// The API framework allocates temporary space.
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NSMALL>(dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, k, topKTilingData, topKInfo, isLargest);
```

```
// dstLocalValue: Stores the k sorted values.
// dstLocalIndex: Stores the indices corresponding to the k sorted values.
// srcLocal: Stores the values to be sorted.
// srcLocalIndex: Stores the indices corresponding to the values to be sorted.
// srcLocalFinish: Specifies that the sorting of certain rows is invalid, with a shape of (outter, 1).
// sharedTmpBuffer: Tensor used to save the temporary buffer during sorting.
// Obtain the top k maximum values or minimum values and their corresponding indices.
// topKTiling: Stores the tiling information required for TopK computation, which can be obtained through the TopKTilingFunc API.
// topKInfo: Shape information of srcLocal.
// isLargest: When set to true, sorts in descending order by default and obtains the top k maximum values; when set to false, sorts in ascending order and obtains the top k minimum values.
// defaultTopKConfig: Related configuration for TopK computation, including algorithm selection, obtaining maximum or minimum values, and whether to sort the results.

// Pass the temporary space through the sharedTmpBuffer input parameter.
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NSMALL, defaultTopKConfig>(dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, sharedTmpBuffer, k, topKTilingData, topKInfo, isLargest);
// The API framework allocates the temporary space.
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NSMALL, defaultTopKConfig>(dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, k, topKTilingData, topKInfo, isLargest);
```

**Table 4** Sample analysis of the Small mode

| Sample Description | This sample sorts input data with a shape of (4, 17) and type float, and obtains the first 8 maximum values of each row of data.<br><br>It uses the Small mode API, where you pass the input data indices. |
| --- | --- |
| Input | **Template parameter T**: float; **Template parameter isInitIndex**: true; **Template parameter isHasfinish**: false; **Template parameter topkMode**: TopKMode::TOPK_NSMALL; **Template parameter topkConfig**: defaultTopKConfig; Input data finishLocal: LocalTensor<bool> finishLocal, no assignment required. Input data k: 8.<br>Input data topKInfo:<br>int32_t outter = 4;<br>int32_t inner = 32;<br>int32_t n = 17;<br>Input data isLargest: true<br>Input data srcLocal: Here n=17, which is not an integer multiple of 32, so it is padded upward to 32, with the padding content being -inf.<br>[[ 55492.18     27748.229   -51100.11     19276.926    14828.149<br>  -20771.824    57553.4     -21504.092   -57423.414      142.36443<br>   -5223.254    54669.473    54519.184    10165.924     -658.4564<br>    2264.2397  -52942.883           -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf]<br> [-52849.074    57778.72     37069.496    16273.109   -25150.637<br>  -35680.5     -15823.097     4327.308   -35853.86     -7052.2627<br>   44148.117   -17515.457   -18926.059    -1650.6737   21753.582<br>   -2589.2822   39390.4             -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf]<br> [-17539.186   -15220.923    29945.332    -4088.1514   28482.525<br>   29750.484   -46082.03     31141.16     23140.047     8461.174<br>   39955.844    29401.35     53757.543    33584.566    -3543.6284<br>  -38318.344    22212.41            -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf]<br> [ -9970.768    -9191.963   -17903.045     2211.4912   47037.562<br>  -41114.824    13305.985    59926.07    -24316.797    -6462.8896<br>    5699.733    -5873.5015   15695.861   -38492.004    19581.654<br>  -36877.68     27090.158           -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf]]<br>Input data srcIndexLocal:<br>[[ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23<br>  24 25 26 27 28 29 30 31]<br> [ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23<br>  24 25 26 27 28 29 30 31]<br> [ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23<br>  24 25 26 27 28 29 30 31]<br> [ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23<br>  24 25 26 27 28 29 30 31]] |
| Output Data | Output data dstValueLocal: Outputs the first 8 maximum values of each row of data.<br>[[57553.4    55492.18   54669.473  54519.184  27748.229  19276.926<br>  14828.149  10165.924 ]<br> [57778.72   44148.117  39390.4    37069.496  21753.582  16273.109<br>   4327.308  -1650.6737]<br> [53757.543  39955.844  33584.566  31141.16   29945.332  29750.484<br>  29401.35   28482.525 ]<br> [59926.07   47037.562  27090.158  19581.654  15695.861  13305.985<br>   5699.733   2211.4912]]<br>Output data dstIndexLocal: Outputs the indices of the first 8 maximum values of each row of data.<br>[[ 6  0 11 12  1  3  4 13]<br> [ 1 10 16  2 14  3  7 13]<br> [12 10 13  7  2  5 11  4]<br> [ 7  4 16 14 12  6 10  3]] |
