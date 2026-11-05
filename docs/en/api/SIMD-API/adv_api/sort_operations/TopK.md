# TopK

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:30:30.612Z pushedAt=2026-10-27T11:54:01.025Z nl=2 -->

## Applicable Products

<!-- npu="950" id13 -->
- Ascend 950PR&950DT products: Supported
<!-- end id13 -->
<!-- npu="A3" id14 -->
- Atlas A3 products: Supported
<!-- end id14 -->
<!-- npu="910b" id15 -->
- Atlas A2 products: Supported
<!-- end id15 -->
<!-- npu="310b" id16 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id16 -->
<!-- npu="310p" id17 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id17 -->
<!-- npu="910" id18 -->
- Atlas training products: Not supported
<!-- end id18 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/sort_operations/TopK_res.md#id1 -->

## Description

Obtains the top k maximum or minimum values and their corresponding indices in the last dimension.

If the input is a vector, the top k maximum or minimum values and their corresponding indices are found in the vector. If the input is a cube, the top k maximum or minimum values and their corresponding indices are computed for each row along the last dimension. **This API supports input of up to two dimensions and does not support higher-dimensional input.**

As shown in the following figure, a two-dimensional cube with shape \(4, 32\) is sorted, with k set to 1, and the output result is \[\[32\] \[32\] \[32\] \[32\]\].

![](../../../figures/nz_reduce_48.png)

-   **Essential Concepts**

    Based on the sample above, we introduce some essential concepts: the number of rows is called the **Outer Axis Length (outter)**, and the actual number of elements in each row is called the **Actual Inner Axis Length (n)**. This API requires the input inner axis length to be an integer multiple of 32. Therefore, when n is not an integer multiple of 32, the developer needs to round it up to an integer multiple of 32, and the padded length is called the **Inner Axis Length (inner)**. For example, in the sample below, the actual length n of each row is 31, which is not an integer multiple of 32. After rounding up, inner becomes 32, and padding in the figure represents the padding operation. The relationship between n and inner is as follows: when n is an integer multiple of 32, inner = n; otherwise, inner \> n.

    ![](../../../figures/nz_reduce_49.png)

-   **API Modes**

    This API supports two modes: **Normal mode** and **Small mode**. Normal mode is the general-purpose mode; Small mode is a high-performance mode provided for scenarios where the inner axis length is fixed at 32 (unit: number of elements). Because inner is fixed at 32 in Small mode, more targeted processing can be performed, so there are fewer related constraints and higher performance. It is recommended to use Small mode when the inner axis length inner is 32.

-   **Additional Feature**: This API allows developers to specify that the sorting of certain rows is invalid. This is controlled by passing the **finishLocal** parameter value. When the value of the row corresponding to **finishLocal** is true, it indicates that the sorting of that row is invalid, and in this case, the k index values of **dstIndexLocal** output after sorting are all set to the invalid index n.

    ![](../../../figures/nz_reduce_50.png)

## Implementation Principle

TopK provides two different sorting algorithms, the MERGE\_SORT algorithm and the RADIX\_SELECT algorithm, which differ in execution speed, time complexity, and algorithm stability. The time complexity of the MERGE\_SORT algorithm is O\(![](../../../figures/zh-cn_formulaimage_0000002108561704.png)\); it is a stable sorting algorithm with a relatively slow execution speed. The time complexity of the RADIX\_SELECT algorithm is O\(n\); it generally does not guarantee stability and has a faster execution speed.

-   **MERGE\_SORT algorithm**

    Taking an input Tensor of the float type, ND format, and shape \[outter,  inner\] as an example, the internal algorithm block diagram of the TopK high-level API is described as shown in the following figure.

    **Figure 1**  TopK Algorithm Block Diagram  
    ![](../../../figures/topk_diagram.png "TopK Algorithm Block Diagram")

    **Figure 2**  TopK Algorithm Block Diagram  
    ![](../../../figures/topk_diagram_1.png "TopK Algorithm Block Diagram-51")

    Depending on the selected **TopKMode**, the process can be divided into two branches.

    -   To compute the TopK NORMAL mode, the process is as follows:

        1.  When the template parameter **isInitIndex** is **false**, an index from 0 to inner - 1 needs to be generated;

            <!-- npu="950" id29 -->
            The Ascend 950PR&950DT products adopt method 1.
            <!-- end id29 -->

            <!-- npu="A3" id30 -->
            Atlas A3 products use method 2.
            <!-- end id30 -->

            <!-- npu="910b" id31 -->
            Atlas A2 products use method 2.
            <!-- end id31 -->

            <!-- npu="310p" id32 -->
            Atlas inference products use method 2.
            <!-- end id32 -->

            -   Method 1: Use **CreateVecIndex** to generate indices from 0 to inner - 1.
            -   Method 2: Use **Arange** to generate indices from 0 to inner - 1.

        2.  When the **isLargest** parameter is false, since the **Sort32** instruction sorts in descending order by default, multiply the data by -1;
        3.  Complete the full sorting of the input data.

            <!-- npu="950" id33 -->
            Ascend 950PR&950DT products use method 1.
            <!-- end id33 -->

            <!-- npu="A3" id34 -->
            Atlas A3 products use method 2.
            <!-- end id34 -->

            <!-- npu="910b" id35 -->
            Atlas A2 products use method 2.
            <!-- end id35 -->

            <!-- npu="310p" id36 -->
            Atlas inference products adopt method 2.
            <!-- end id36 -->

            Method 1:

            Use the high-level API **Sort** to fully sort the data.

            Method 2:

            1.  Use **Sort32** to sort the data, ensuring that every 32 data items are in order.
            2.  Use the **MrgSort** instruction to merge-sort all sorted data blocks.

        4.  Use the **GatherMask** instruction to extract the first k data items and their indices;
        5.  When finishLocal\[i\] is true, update the sorting result of the corresponding row to the invalid index n;
        6.  If the **isLargest** parameter is false, multiply the data by -1 to restore the data.

        <!-- npu="310p" id41 -->
        Note: On Atlas inference products, use the **ProposalConcat** basic API to combine data and index, then use the **RpSort16** basic API to sort the data; use **MrgSort4** for merging; and use the **ProposalExtract** basic API to extract data and index.
        <!-- end id41 -->

    -   The process of computing the TopK SMALL mode is as follows:

        1.  When the template parameter **isInitIndex** is false, generate indices from 0 to **inner** - 1, and use the **Copy** instruction to copy the data into **outter** items;

            <!-- npu="950" id37 -->
            Ascend 950PR&950DT products adopt method 1.
            <!-- end id37 -->

            <!-- npu="A3" id38 -->
            Atlas A3 products adopt method 2.
            <!-- end id38 -->

            <!-- npu="910b" id39 -->
            Atlas A2 products adopt method 2.
            <!-- end id39 -->

            <!-- npu="310p" id40 -->
            Atlas inference products adopt method 2.
            <!-- end id40 -->

            -   Method 1: Use **CreateVecIndex** to generate indices from 0 to **inner** - 1.
            -   Method 2: Use **Arange** to generate indices from 0 to **inner** - 1.

        2.  When the **isLargest** parameter is false, since the **Sort32** instruction sorts in descending order by default, multiply the input data by -1;
        3.  Use **Sort32** to sort the data;
        4.  Use the GatherMask instruction to extract the first k data items and their indices;
        5.  If the isLargest parameter is false, multiply the input data by -1 to restore the data.

        <!-- npu="310p" id42 -->
        Note: On Atlas inference products, use the ProposalConcat basic API to combine data and index, and then use the RpSort16 basic API to sort the data. Because inner is 32 in small mode, after RpSort16 sorting, every 16 data items are in order, so between step 3 and step 4, use the MrgSort4 basic API to perform a merge sort.
        <!-- end id42 -->

-   **RADIX\_SELECT Algorithm**

    <!-- npu="950" id43 -->
    This algorithm is supported only on Ascend 950PR&950DT products.
    <!-- end id43 -->

    **Figure 3**  TopK algorithm block diagram  
    ![](../../../figures/topk_diagram_2.png "TopK algorithm block diagram-52")

    The computation process is divided into the following steps:

    1.  Generate indices. Depending on the TopKMode, the process is divided into:
        -   TopK NORMAL mode: The template parameter isInitIndex is false. Use CreateVecIndex to generate indices from 0 to inner-1;
        -   TopK SMALL mode: The template parameter isInitIndex is false. Use CreateVecIndex to generate indices from 0 to inner-1, and use the Copy instruction to copy the data into outter items;

    2.  According to the template parameter, determine whether to preprocess the data. If the data type is a floating-point type or a signed integer type, perform the twiddle in operation (convert floating-point numbers or signed integers to unsigned integer types). If TopK obtains the minimum values, perform the negation operation on the unsigned integer type data;
    3.  Find the K-th largest number and save it to **kValue**;
    4.  Extract the data and indices greater than **kValue**;
    5.  Extract the data and indices equal to **kValue**;
    6.  According to the template parameter **sorted**, determine whether to sort the k numbers, and store the sorted data result in the temporary space;
    7.  According to the template parameter, if TopK obtains the minimum values, perform the negation operation; if the data type is a floating-point type or a signed integer type, perform the twiddle out operation (convert unsigned integer types to floating-point numbers or signed integer types).

## Prototype

-   The API allocates temporary space internally

    ```
    template <typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false, enum TopKMode topkMode = TopKMode::TOPK_NORMAL>
    __aicore__ inline void TopK(const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal, const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const int32_t k, const TopkTiling& tiling, const TopKInfo& topKInfo, const bool isLargest = true)
    ```

    ```
    template <typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false, enum TopKMode topkMode = TopKMode::TOPK_NORMAL, const TopKConfig& config = defaultTopKConfig>
    __aicore__ inline void TopK(const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal, const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const int32_t k, const TopkTiling& tiling, const TopKInfo& topKInfo, const bool isLargest = true)
    ```

-   Pass temporary space through the **tmpLocal** input parameter

    ```
    template <typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false, enum TopKMode topkMode = TopKMode::TOPK_NORMAL>
    __aicore__ inline void TopK(const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal, const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const LocalTensor<uint8_t>& tmpLocal, const int32_t k, const TopkTiling& tiling, const TopKInfo& topKInfo, const bool isLargest = true)
    ```

    ```
    template <typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false, enum TopKMode topkMode = TopKMode::TOPK_NORMAL, const TopKConfig& config = defaultTopKConfig>
    __aicore__ inline void TopK(const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal, const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const LocalTensor<uint8_t>& tmpLocal, const int32_t k, const TopkTiling& tiling, const TopKInfo& topKInfo, const bool isLargest = true)
    ```

Because the internal implementation of this API involves complex logical computation, additional temporary space is required to store intermediate variables during the computation. The temporary space supports two methods: **applied for by the API** and **passed by the developer through the tmpLocal input parameter**.

-   When the API applies for temporary space internally, the developer does not need to apply for it, but needs to reserve the size of the temporary space.

-   Passed in through the **tmpLocal** input parameter. This tensor is used as the temporary space for processing, and the API no longer allocates it internally. With this method, developers can manage the **tmpLocal** memory space themselves and reuse this memory after the API call completes, avoiding repeated allocation and release of memory, which provides higher flexibility and memory utilization. The **BufferSize** of the temporary space **tmpLocal** is obtained as follows: use the **GetTopKMaxMinTmpSize** API provided in [TopK Tiling](TopK-Tiling.md) to obtain the required maximum and minimum temporary space sizes.

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the data to be sorted. For the data types supported by different models, see [Supported Data Types](#li197551749192811). |
| isInitIndex | Whether to pass in the index of the input data.<br>**true** indicates that the index is passed in. When set to **true**, the index of the input data must be passed in through the **srcIndexLocal** parameter. For details, see the **srcIndexLocal** parameter description in Table 2.<br>**false** indicates that the index is not passed in, and the index output by the TopK API is unavailable. |
| isHasfinish | The TopK API allows developers to specify, through the **finishLocal** parameter, that the sorting of certain rows is invalid. This template parameter controls whether to enable the above feature. **true** indicates enabled, and **false** indicates disabled.<br>Supported values in Normal mode: **true** / **false**.<br>Supported value in Small mode: **false**.<br>For the combined usage of the **isHasfinish** parameter and **finishLocal**, see the **finishLocal** parameter description in Table 2.<!-- npu="950" id19 --><br><br>For Ascend 950PR&950DT products, for the RADIX_SELECT algorithm, this parameter is reserved and not yet enabled. It is retained for future feature extension. Keep the default value.<!-- end id19 --> |
| isReuseSrc | Whether to allow modification of the source operand. The default value is **false**.<!-- npu="950" id20 --><br>For Ascend 950PR&950DT products, this parameter takes effect only when the input data type is float. The values are as follows:<br>**true**: The developer allows the source operand to be overwritten. Set this parameter to **true** to enable it. After enabling, the API reuses the memory space of **srcTensor** during internal computation, saving some memory space.<br>**false**: The API does not reuse the memory space of **srcTensor** during internal computation.<!-- end id20 --><!-- npu="A3" id21 --><br>For Atlas A3 products, this parameter is reserved. Pass the default value **false**.<!-- end id21 --><!-- npu="910b" id22 --><br>For Atlas A2 products, this parameter is reserved. Pass the default value **false**.<!-- end id22 --><!-- npu="310p" id23 --><br>For the Atlas inference products AI Core, this parameter is reserved. Pass the default value **false**.<!-- end id23 --> |
| topkMode | Mode selection of TopK. The values are as follows:<br>**TopKMode::TOPK_NORMAL**: Normal mode.<br>**TopKMode::TOPK_NSMALL**: Small mode. |
| config | <!-- npu="950" id24 -->This parameter is supported only on Ascend 950PR&950DT products.<br><br><!-- end id24 -->Related configuration of TopK computation, including algorithm selection, obtaining maximum or minimum values, and whether to sort the results. This parameter is optional and of the **TopKConfig** type. Its specific definition is shown in the following code:<br>**algo**: The sorting algorithm to be selected. The default is the MERGE_SORT algorithm. Currently only the RADIX_SELECT algorithm is supported, and the user must explicitly specify **algo** as **TopKAlgo::RADIX_SELECT**.<br>**order**: Indicates whether to obtain the top k maximum values or the top k minimum values. The values are as follows: **UNSET**: Default value, implemented according to the configuration of the function parameter **isLargest**. When **isLargest** is **true**, the top k maximum values and their corresponding indices are obtained; when **isLargest** is **false**, the top k minimum values and their corresponding indices are obtained. **LARGEST**: Indicates obtaining the top k maximum values and their corresponding indices. When the value is **LARGEST**, the configuration of the function parameter **isLargest** does not take effect. **SMALLEST**: Indicates obtaining the top k minimum values and their corresponding indices. When the value is **SMALLEST**, the configuration of the function parameter **isLargest** does not take effect.<br>**sorted**: Indicates whether to sort the output results. When the value is **true**, the output results are sorted; when the value is **false**, the output results are not sorted.<br><br>The default value **defaultTopKConfig** of this parameter is: **TopKAlgo::MERGE_SORT**, **TopKOrder::UNSET**, **true**. |

```
struct TopKConfig {
    TopKAlgo algo = TopKAlgo::MERGE_SORT;
    TopKOrder order = TopKOrder::UNSET;
    bool sorted = true;
};
enum class TopKAlgo { RADIX_SELECT, MERGE_SORT };
enum class TopKOrder { UNSET, LARGEST, SMALLEST };
```

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstValueLocal | Output | Destination operand. Used for saving the k sorted values.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Normal mode:<br>The output shape is outter * k_pad, that is, outter data items are output, and the length of each data item is k_pad. k_pad is the value obtained by aligning k upward to 32 bytes according to the input data type.<br>The developer needs to allocate k_pad * outter * sizeof(T) space for **dstValueLocal**.<br>The first k values of each output data item are the top k maximum/minimum values of that item. The k+1 to k_pad elements of each data item are not filled with values and are some random values.<br>The k_pad calculation method is as follows:<br>When the types of the input **srcLocal** and **dstValueLocal** are float, float is 4 bytes, so round up k to the nearest multiple of 8 as k_pad, which can satisfy 32-byte alignment.<br>k_pad = (k + 7) / 8 * 8<br>When the types of the input **srcLocal** and **dstValueLocal** are half, half is 2 bytes, so round up k to the nearest multiple of 16 as k_pad, which can satisfy 32-byte alignment.<br>    k_pad = (k + 15) / 16 * 16<br><br>Small mode:<br>The output shape is outter * k, that is, outter data items are output, and the length of each data item is k.<br>The output values require k * outter * sizeof(T) space for saving. The developer needs to allocate the actual memory space for **dstValueLocal** according to this size and the alignment requirements of the framework.<br>**Note: Follow the framework's memory allocation requirements (the allocated memory size must satisfy 32-byte alignment). That is, when k * outter * sizeof(T) is not 32-byte aligned, it needs to be aligned upward to 32 bytes. The extra memory space allocated for alignment is not filled with values and contains some random values.** |
| dstIndexLocal | Output | Destination operand. Used for saving the indices corresponding to the k sorted values.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Normal mode:<br>The output shape is outter * kpad_index, that is, outter data items are output, and the length of each data item is kpad_index. kpad_index is the value obtained by aligning k upward to 32 bytes according to the input index type.<br>The developer needs to allocate kpad_index * outter * sizeof(int32_t) space for **dstIndexLocal**.<br>The first k values of each data item are the indices corresponding to the top k maximum/minimum values of that item. The k+1 to kpad_index indices of each data item are not filled with values and are some random values.<br>The k_pad calculation method is as follows: Since **dstIndexLocal** is of the int32_t type, which is 4 bytes, round up k to the nearest multiple of 8 as kpad_index, which can satisfy 32-byte alignment.<br>kpad_index = (k + 7) / 8 * 8<br><br>Small mode:<br>The output shape is outter * k, that is, outter data items are output, and the length of each data item is k.<br>The output indices require k * outter * sizeof(int32_t) space for saving. The developer needs to allocate the actual memory space for **dstIndexLocal** according to this size and the alignment requirements of the framework.<br>**Note: Follow the framework's memory allocation requirements (the allocated memory size must satisfy 32-byte alignment). That is, when k * outter * sizeof(int32_t) is not 32-byte aligned, it needs to be aligned upward to 32 bytes. The extra memory space allocated for alignment is not filled with values and contains some random values.** |
| srcLocal | Input | Source operand. Used for saving the values to be sorted.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<!-- npu="310p" id44 --><br>On the Atlas inference products AI Core:<br>The shape of the input data is outter * inner. The developer needs to allocate outter * inner * sizeof(T) space for it.<br>When n < inner, the developer needs to perform padding on the outter data items in **srcLocal**, padding each data item from n to the inner length.<br>Padding rule: The filled data must not affect the overall sorting. The following filling method is recommended: when obtaining the top k maximum values, the filled value must be the minimum value of the **input data type**; when obtaining the top k minimum values, the filled value must be the maximum value of the **input data type**.<!-- end id44 --> |
| srcIndexLocal | Input | Source operand. Used for saving the indices corresponding to the values to be sorted.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>This parameter is used together with the template parameter **isInitIndex**. When **isInitIndex** is **false**, **srcIndexLocal** only needs to be defined and does not need to be assigned; pass the defined **srcIndexLocal** to the API. When **isInitIndex** is **true**, the developer needs to pass in the index values through the **srcIndexLocal** parameter. The rules for setting the **srcIndexLocal** parameter are as follows:<br><br>Normal mode:<br>The shape of the input index data is 1 * inner, where all outter data items use the same index. The developer needs to allocate inner * sizeof(int32_t) space for it.<br>When n < inner, the developer needs to perform padding on the index data, padding this data item from n to the inner length.<br>Padding rule: The filled indices must not affect the overall sorting. The following filling method is recommended: the filled values increment on the basis of the original indices. For example, if the original indices are 0, 1, 2, ..., n-1, the filled indices are 0, 1, 2, ..., n, n + 1, ..., inner-1.<br><br>Small mode:<br>The shape of the input index data is outter * inner. The developer needs to allocate outter * inner * sizeof(int32_t) space for it.<br>When n < 32, the developer needs to perform padding on the outter data items, padding each data item from n to the length of 32.<br>Padding rule: The filled data must not affect the overall sorting. The following filling method is recommended: the filled values increment on the basis of the original indices. For example, if the original indices are 0, 1, 2, ..., n-1, the filled indices are 0, 1, 2, ..., n, n + 1, ..., inner-1. |
| finishLocal | Input | Source operand. Used for specifying that the sorting of certain rows is invalid, with a shape of (outter, 1).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<!-- npu="950" id45 --><br><br>For Ascend 950PR&950DT products, this parameter is reserved and not yet enabled. It is retained for future feature extension, with a value of **false**.<!-- end id45 --><br><br>This parameter is used together with the template parameter **isHasfinish**. In Normal mode, **isHasfinish** can be configured as **true**/**false**; in Small mode, **isHasfinish** can only be configured as **false**.<br>When **isHasfinish** is configured as **true**:<br>When the value of the outter row corresponding to **finishLocal** is **true**, the sorting of that row is invalid, and the k index values of **dstIndexLocal** output after sorting are all set to n.<br>When the value of the outter row corresponding to **finishLocal** is **false**, the sorting of that row is valid.<br><br>When **isHasfinish** is configured as **false**, **finishLocal** only needs to be defined and does not need to be assigned; pass the defined **finishLocal** to the API. A definition example is as follows: `LocalTensor<bool> finishLocal;` |
| tmpLocal | Input | Temporary space. Used for storing intermediate variables during complex internal computation of the API, provided by the developer. For how to obtain the temporary space size, see [TopK Tiling](TopK-Tiling.md). The data type is fixed as uint8_t.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). **The logical position supports only VECCALC and does not support other logical positions.** |
| k | Input | Obtain the top k maximum or minimum values and their corresponding indices. The data type is int32_t.<br><br>The size of k must satisfy: 1 <= k <= n. |
| tiling | Input | Tiling information required for TopK computation. For how to obtain the Tiling information, see [TopK Tiling](TopK-Tiling.md). |
| topKInfo | Input | Shape information of **srcLocal**. It is of the **TopKInfo** type, and its specific definition is shown in the following code, where the meanings of the parameters are as follows:<br>**outter**: Indicates the outer axis length of the input data to be sorted.<br>**inner**: Indicates the inner axis length of the input data to be sorted. **inner** must be an integer multiple of 32.<br>**n**: Indicates the actual length of the inner axis of the input data to be sorted.<br>Note:<br>**topKInfo.inner** must be an integer multiple of 32.<br>**topKInfo.inner** is the value obtained by rounding up **topKInfo.n** to an integer multiple of 32, so the size of **topKInfo.n** must satisfy: 1 <= topKInfo.n <= topKInfo.inner.<br>In Small mode, **topKInfo.inner** must be set to 32.<br>In Normal mode, the maximum value of **topKInfo.inner** is 4096. |
| isLargest | Input | The type is bool. When the value is **true**, descending order is used by default to obtain the top k maximum values; when the value is **false**, ascending order is used to obtain the top k minimum values. |

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
-   **Overlapping of source operand and destination operand addresses is not supported.**
-   When srcLocal\[i\] and srcLocal\[j\] are the same, if i\>j, then srcLocal\[j\] will be selected first and placed in front.
-   inf is considered a maximum value in TopK.
-   In TopK sorting, nan is placed in front regardless of whether the order is descending or ascending.
<!-- npu="310p" id46 -->
-   For the Atlas inference products AI Core:
    -   When the input **srcLocal** type is half and the template parameter **isInitIndex** is **false**, the passed-in **topKInfo.inner** cannot be greater than 2048.

    -   When the input **srcLocal** is of type **half** and the template parameter **isInitIndex** is set to **true**, the index values passed in **srcIndexLocal** cannot be greater than 2048.
<!-- end id46 -->

-   Supported data types<a id="li197551749192811"></a>

    <!-- npu="950" id25 -->
    For Ascend 950PR&950DT products, the MERGE\_SORT algorithm currently supports the **half** and **float** data types. The RADIX\_SELECT algorithm currently supports the uint8\_t, int8\_t, uint16\_t, int16\_t, uint32\_t, int32\_t, bfloat16\_t, **half**, **float**, uint64\_t, and int64\_t data types.
    <!-- end id25 -->

    <!-- npu="A3" id26 -->
    For Atlas A3 products, the supported data types are **half** and **float**.
    <!-- end id26 -->

    <!-- npu="910b" id27 -->
    For Atlas A2 products, the supported data types are **half** and **float**.
    <!-- end id27 -->

    <!-- npu="310p" id28 -->
    For the Atlas inference products AI Core, the supported data types are **half** and **float**.
    <!-- end id28 -->

## Examples<a name="section94691236101419"></a>

This sample implements the code logic of the Normal mode.

```
// dstLocalValue: Save the k sorted values.
// dstLocalIndex: Save the indices corresponding to the k sorted values.
// srcLocal: Save the values to be sorted.
// srcLocalIndex: Save the indices corresponding to the values to be sorted.
// srcLocalFinish: Specify that the sorting of certain rows is invalid, with a shape of (outter, 1).
// sharedTmpBuffer: Save the Tensor temporarily cached during sorting.
// Obtain the top k maximum or minimum values and their corresponding indices.
// topKTiling: Store the Tiling information required for TopK computation, which can be obtained through the TopKTilingFunc API.
// topKInfo: Shape information of srcLocal.
// isLargest: When set to true, sorting is performed in descending order by default to obtain the top k maximum values; when set to false, sorts in ascending order to obtain the top k minimum values.

// Pass temporary space through the sharedTmpBuffer input parameter.
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NORMAL>(
    dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, sharedTmpBuffer, k, topKTilingData,
    topKInfo, isLargest);
// The API framework applies for temporary space.
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NORMAL>(
    dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, k, topKTilingData, topKInfo, isLargest);
```

```
// dstLocalValue: stores the k sorted values.
// dstLocalIndex: stores the indices corresponding to the k sorted values.
// srcLocal: stores the values to be sorted.
// srcLocalIndex: stores the indices corresponding to the values to be sorted.
// srcLocalFinish: specifies that sorting of certain rows is invalid, with a shape of (outter, 1).
// sharedTmpBuffer: stores the Tensor temporarily cached during sorting.
// Obtain the top k maximum or minimum values and their corresponding indices.
// topKTiling: stores the Tiling information required for TopK computation, which can be obtained through the TopKTilingFunc API.
// topKInfo: Shape information of srcLocal.
// isLargest: When set to true, sorting is performed in descending order by default to obtain the top k maximum values; when set to false, sorts in ascending order to obtain the top k minimum values.
// defaultTopKConfig: Related configuration for TopK computation, including algorithm selection, obtaining maximum or minimum values, and whether to sort the results.

// Pass the temporary space through the sharedTmpBuffer input parameter.
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NORMAL, defaultTopKConfig>(
    dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, sharedTmpBuffer, k, topKTilingData,
    topKInfo, isLargest);
// The API framework applies for temporary space.
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NORMAL, defaultTopKConfig>(
    dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, k, topKTilingData, topKInfo, isLargest);
```

**Table 3** Sample analysis of Normal mode

| Sample Description | This sample sorts a cube with a shape of (2, 32) and data type float, and obtains the first 5 minimum values of each row.<br><br>It uses the Normal mode API. The developer passes in the input data index and passes finishLocal to specify that the sorting of certain rows is invalid. |
| --- | --- |
| Input | Template parameter T: float; template parameter isInitIndex: true; template parameter isHasfinish: true; template parameter topkMode: TopKMode::TOPK_NORMAL; template parameter topkConfig: defaultTopKConfig.<br>Input data finishLocal:<br>[False  True  False  False False False  False False False  False False False <br> False  False False False False False False False False False False False <br> False False False False False False False False]<br><br>**Note: The transfer amount of DataCopy must be a multiple of 32 bytes. Therefore, the actual valid input of finishLocal here is the first two bits False and True, and the remaining values are padded up to 32 bytes, which do not actually participate in the computation.**<br>Input data k: 5<br>Input data topKInfo:<br>int32_t outter = 2;<br>int32_t inner = 32;<br>int32_t n = 32;<br>Input data isLargest: false<br>Input data srcLocal:<br>[[-18096.555   -11389.83    -43112.895   -21344.77     57755.918<br>   50911.145    24912.621   -12683.089    45088.004   -39351.043<br>  -30153.293    11478.329    12069.15     -9215.71     45716.44<br>  -21472.398   -37372.16    -17460.414    22498.03     21194.838<br>  -51229.17    -51721.918   -47510.38     47899.11     43008.176<br>    5495.8975  -24176.97    -14308.27     53950.695     7652.6035<br>  -45169.168   -26275.518  ]<br> [ -9196.681   -31549.518    18589.23    -12427.927    50491.81<br>  -20078.11    -25606.107   -34466.773   -42512.805    50584.48<br>   35919.934   -17283.5       6488.137   -12885.134     1942.2147<br>  -50611.96     52671.477    23179.662    25814.875      -69.73492<br>   33906.797   -34662.61     46168.71    -52391.258    57435.332<br>   50269.414    40935.05     21164.176     4028.458   -29022.918<br>  -46391.133     1971.2042 ]]<br>Input data srcIndexLocal:<br>[ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23<br> 24 25 26 27 28 29 30 31] |
| Output Data | The output data dstValueLocal is as follows. The length of each row is k_pad, where the first 5 values of each row are the first 5 minimum values of that item. The following three values are random values.<br>[[-51721.918 -51229.17  -47510.38  -45169.168 -43112.895     0.     0.   0.   ]<br> [-52391.258 -50611.96  -46391.133 -42512.805 -34662.61      0.     0.   0.   ]]<br>The output data dstIndexLocal is as follows.<br>The length of each row is kpad_index, where the first 5 values of each row are the indices corresponding to the first 5 minimum values of that item. The following three values are random values.<br>Since the finishLocal corresponding to the second row of data is true, the sorting of the second row of data is invalid, so all its output index values are the actual inner axis length 32.<br>[[21 20 22 30  2  0  0  0]<br> [32 32 32 32 32  0  0  0]] |

This sample implements the code logic of Small mode.

```
// dstLocalValue: stores the k sorted values.
// dstLocalIndex: stores the indices corresponding to the k sorted values.
// srcLocal: stores the values to be sorted.
// srcLocalIndex: stores the indices corresponding to the values to be sorted.
// srcLocalFinish: specifies that the sorting of certain rows is invalid. Its shape is (outter, 1).
// sharedTmpBuffer: stores the Tensor used as temporary cache during sorting.
// Obtain the top k maximum or minimum values and their corresponding indices.
// topKTiling: stores the Tiling information required for Topk computation, which can be obtained through the TopKTilingFunc API.
// topKInfo: Shape information of srcLocal.
// isLargest: When set to true, sort in descending order by default and obtain the top k maximum values; when set to false, sort in ascending order and obtain the top k minimum values.

// Pass temporary space through the sharedTmpBuffer input parameter.
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NSMALL>(
    dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, sharedTmpBuffer, k, topKTilingData,
    topKInfo, isLargest);
// Allocate temporary space through the API framework.
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NSMALL>(
    dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, k, topKTilingData, topKInfo, isLargest);
```

```
// dstLocalValue: stores the k sorted values.
// dstLocalIndex: stores the indices corresponding to the k sorted values.
// srcLocal: stores the values to be sorted.
// srcLocalIndex: stores the indices corresponding to the values to be sorted.
// srcLocalFinish: specifies that the sorting of certain rows is invalid, with a shape of (outter, 1).
// sharedTmpBuffer: stores the Tensor used as temporary cache during sorting.
// Obtain the top k maximum or minimum values and their corresponding indices.
// topKTiling: stores the Tiling information required for Topk computation, which can be obtained through the TopKTilingFunc API.
// topKInfo: Shape information of srcLocal.
// isLargest: When set to true, sorting is performed in descending order by default to obtain the top k maximum values; when set to false, sorts in ascending order to obtain the top k minimum values.
// defaultTopKConfig: Related configuration for TopK computation, including algorithm selection, obtaining maximum or minimum values, and whether to sort the results.

// Pass temporary space through the sharedTmpBuffer input parameter.
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NSMALL, defaultTopKConfig>(
    dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, sharedTmpBuffer, k, topKTilingData,
    topKInfo, isLargest);
// The API framework allocates temporary space.
AscendC::TopK<T, isInitIndex, isHasfinish, isReuseSrc, AscendC::TopKMode::TOPK_NSMALL, defaultTopKConfig>(
    dstLocalValue, dstLocalIndex, srcLocalValue, srcLocalIndex, srcLocalFinish, k, topKTilingData, topKInfo, isLargest);
```

**Table 4** Sample analysis of Small mode

| Sample Description | This sample sorts input data of a shape of (4, 17) and type float, obtaining the top 8 maximum values of each row.<br><br>It uses the Small mode API, where the developer passes in the input data indices. |
| --- | --- |
| Input | Template parameter T: float; template parameter isInitIndex: true; template parameter isHasfinish: false; template parameter topkMode: TopKMode::TOPK_NSMALL; template parameter topkConfig: defaultTopKConfig; input data finishLocal: `LocalTensor<bool> finishLocal`, no assignment required. Input data k: 8.<br>Input data topKInfo:<br>int32_t outter = 4;<br>int32_t inner = 32;<br>int32_t n = 17;<br>Input data isLargest: true<br>Input data srcLocal: here n=17, which is not an integer multiple of 32, so it is padded up to 32, filling with -inf.<br>[[ 55492.18     27748.229   -51100.11     19276.926    14828.149<br>  -20771.824    57553.4     -21504.092   -57423.414      142.36443<br>   -5223.254    54669.473    54519.184    10165.924     -658.4564<br>    2264.2397  -52942.883           -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf]<br> [-52849.074    57778.72     37069.496    16273.109   -25150.637<br>  -35680.5     -15823.097     4327.308   -35853.86     -7052.2627<br>   44148.117   -17515.457   -18926.059    -1650.6737   21753.582<br>   -2589.2822   39390.4             -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf]<br> [-17539.186   -15220.923    29945.332    -4088.1514   28482.525<br>   29750.484   -46082.03     31141.16     23140.047     8461.174<br>   39955.844    29401.35     53757.543    33584.566    -3543.6284<br>  -38318.344    22212.41            -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf]<br> [ -9970.768    -9191.963   -17903.045     2211.4912   47037.562<br>  -41114.824    13305.985    59926.07    -24316.797    -6462.8896<br>    5699.733    -5873.5015   15695.861   -38492.004    19581.654<br>  -36877.68     27090.158           -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf         -inf         -inf         -inf<br>          -inf         -inf]]<br>Input data srcIndexLocal:<br>[[ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23<br>  24 25 26 27 28 29 30 31]<br> [ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23<br>  24 25 26 27 28 29 30 31]<br> [ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23<br>  24 25 26 27 28 29 30 31]<br> [ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23<br>  24 25 26 27 28 29 30 31]] |
| Output Data | Output data dstValueLocal: outputs the top 8 maximum values of each row.<br>[[57553.4    55492.18   54669.473  54519.184  27748.229  19276.926<br>  14828.149  10165.924 ]<br> [57778.72   44148.117  39390.4    37069.496  21753.582  16273.109<br>   4327.308  -1650.6737]<br> [53757.543  39955.844  33584.566  31141.16   29945.332  29750.484<br>  29401.35   28482.525 ]<br> [59926.07   47037.562  27090.158  19581.654  15695.861  13305.985<br>   5699.733   2211.4912]]<br>Output data dstIndexLocal: outputs the indices of the top 8 maximum values of each row.<br>[[ 6  0 11 12  1  3  4 13]<br> [ 1 10 16  2 14  3  7 13]<br> [12 10 13  7  2  5 11  4]<br> [ 7  4 16 14 12  6 10  3]] |
