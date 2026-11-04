# Mask

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:51:11.058Z pushedAt=2026-10-28T03:14:39.594Z nl=2 -->

## Overview

A mask is a sequence of flag bits in Ascend C that controls whether data elements participate in computation. Each mask bit corresponds to the position of an element in the data. By marking bits as valid or invalid, the mask provides fine-grained on/off control over data operations.

A mask is represented by a fixed-width value:

- Valid bit (usually 1): The corresponding element participates in computation normally.
- Invalid bit (usually 0): The corresponding element is masked out and no operation is performed on it.

In the [continuous computation](continuous_compute.md) API (that is, the first-n-data computation API), the mask is the input parameter n, indicating that the first n consecutive elements participate in computation.

In the [high-dimensional slicing](high_dim_split.md) API, the mask controls the elements that participate in computation within each iteration. It can be set in two ways: continuous mode and bit-by-bit mode.

- Continuous mode: Indicates how many consecutive elements at the beginning of each iteration participate in computation.
- Bit-by-bit mode: Indicates which elements participate in computation on a bit-by-bit basis.

The mask application mechanism based on the first-n-data computation and high-dimensional slicing APIs is shown in [Figure 1 Mask mechanism](#Figure 1-mask_mechanism description).

**Figure 1** Mask mechanism description
![](../../../../figures/mask_mechanism.png)<a id="Figure 1-mask_mechanism description"></a>

> [!NOTE]Note
>
> <!-- npu="A3,910b" id1 -->
> - For the following models, Mask is a hardware-native capability. When the first-n-data computation API is called, the interface internal sets Mask to Counter mode and restores it to the system-default Normal mode after the computation is complete.
>   <!-- npu="A3" id2 -->
>   - Atlas A3 products
>   <!-- end id2 -->
>   <!-- npu="910b" id3 -->
>   - Atlas A2 products
>   <!-- end id3 -->
> <!-- end id1 -->
> <!-- npu="950" id4 -->
> - On Ascend 950PR&950DT products, Mask is implemented through software emulation to ensure compatibility, and the capabilities provided by its underlying hardware differ from those of Atlas A2 products and Atlas A3 products. It is recommended to use the first-n-data computation API, which achieves compatibility by calling the [Reg vector computation API](../../reg_vector_compute/reg_vector_compute.md) through VF. The first-n-data computation API does not switch the Mask mode to Normal mode internally. If Mask has been set to Counter mode through [SetMaskCount](../mask_operations/SetMaskCount.md) before calling this API, it remains in Counter mode after the call. When subsequent computation requires Normal mode, call [SetMaskNorm](../mask_operations/SetMaskNorm.md) manually to set it.
> <!-- end id4 -->

### Mask Configuration Method

The mask setting method is determined by the calling method of the specific API and the template parameter isSetMask. Two modes are supported: setting the mask inside the API and setting the mask outside the API.

**Table 1** Mask configuration methods

|Configuration Method|Description|
|:---|:---|
|[Setting the Mask Inside the API](#setting-the-mask-inside-the-api) (default)|&bull;Pass the mask value directly through the input parameter of the vector computation API.<br>&bull;The template parameter isSetMask of the vector computation API (supported by only some APIs) controls whether the mask is passed through the API parameter or configured by an external API. The default value is true, indicating that the mask is passed through the API parameter. Mask corresponds to the mask/mask[] parameter in the high-dimensional slicing computation API or the calCount parameter in the tensor first-n-data computation API.|
|[Setting the Mask Outside the API](#setting-the-mask-outside-the-api)|&bull;Set the mask value through the [SetVectorMask](../mask_operations/SetVectorMask.md) API. Set the template parameter isSetMask of the vector computation API to false. The mask parameter in the API input parameter (corresponding to the mask/mask[] parameter in the high-dimensional slicing computation API or the calCount parameter in the tensor first-n-data computation API) does not take effect.<br>&bull;This mode applies to scenarios where the mask parameter is the same and reused multiple times. It eliminates the need to repeatedly set the mask inside the vector computation API, providing a certain performance advantage.|

Mask supports the following working modes:

**Table 2** Mask working modes<a id="mask-mode"></a>

|Working Mode|Description|
|:---|:---|
|Normal mode|Default mode. Supports the mask capability within a single iteration. You need to configure the number of iterations and additionally compute the tail block.<br>**In Normal mode, Mask controls the number of elements that participate in computation within a single iteration.**<br>Set Normal mode by calling [SetMaskNorm](../mask_operations/SetMaskNorm.md).|
|Counter mode|Simplified mode. Directly passes the computation data volume and automatically infers the number of iterations. You do not need to be aware of the number of iterations or handle unaligned tail blocks. However, it does not support the mask capability within a single iteration.<br>**In Counter mode, Mask indicates the number of elements that participate in the entire vector computation.**<br>Set Counter mode by calling [SetMaskCount](../mask_operations/SetMaskCount.md).|

The usage of mask operations is as follows:

**Table 3** Usage of mask operations

|Configuration Method|Working Mode|first-n-data computation API|high-dimensional slicing computation API|
|:---|:---|:---|:---|
|[Setting the Mask Inside the API](#setting-the-mask-inside-the-api)|[Normal mode](#mask-mode)|Not applicable.|Set the isSetMask template parameter to true and pass the mask through the API input parameter. Configure the dataBlockStride, repeatStride, and repeatTime parameters based on the usage scenario.|
|[Setting the Mask Inside the API](#setting-the-mask-inside-the-api)|[Counter mode](#mask-mode)|Set the isSetMask template parameter to true and pass the mask through the API input parameter.|Set the isSetMask template parameter to true and pass the mask through the API input parameter.<br>Configure the dataBlockStride and repeatStride parameters based on the usage scenario. Pass a fixed value for repeatTime. It is recommended to set it to 1, as this value does not take effect.|
|[Setting the Mask Outside the API](#setting-the-mask-outside-the-api)|[Normal mode](#mask-mode)|Not applicable.|Call [SetVectorMask](../mask_operations/SetVectorMask.md) to set the mask, and then call the high-dimensional slicing computation API.<br>&bull;Set the isSetMask template parameter to false. Set the mask value in the API input parameter to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning.<br>&bull;Configure the repeatTime, dataBlockStride, and repeatStride parameters based on the usage scenario.|
|[Setting the Mask Outside the API](#setting-the-mask-outside-the-api)|[Counter mode](#mask-mode)|Set the isSetMask template parameter to false. Call [SetVectorMask](../mask_operations/SetVectorMask.md) to set the mask, and then call the first-n-data computation API. It is recommended to set count in the API input parameter to 1.|Call [SetVectorMask](../mask_operations/SetVectorMask.md) to set the mask, and then call the high-dimensional slicing computation API.<br>&bull;Set the isSetMask template parameter to false. Set the mask value in the API input parameter to `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning.<br>&bull;Configure the dataBlockStride and repeatStride parameters based on the usage scenario. Pass a fixed value for repeatTime. It is recommended to set it to 1, as this value does not take effect.|

You need to select the correct mask configuration method based on the API usage scenario and parameter configuration, as shown in [Figure 2 Code comparison of two configuration methods](#figure-2-code-comparison-of-two-configuration-methods):

**Figure 2**  Code comparison of two configuration methods  
![](../../../../figures/two_config_comparison.png)<a id="figure-2-code-comparison-of-two-configuration-methods"></a>

> [!CAUTION]Note:
>
> - Only some APIs support isSetMask. For details, see the function prototype of the corresponding API.
>
> - Setting Mask outside the API**Performance practice reference: [Performance optimization based on global mask reuse](../../../../../guide/operator_practice/simd_operator_optimization/vector_compute/mask_reuse_optimization.md)**.

## Setting the Mask Inside the API

### Counter mode

- **First-n-data computation**

    For first-n-data computation using [Counter mode](#mask-mode), the count parameter specifies the number of elements that participate in computation, and the API automatically performs the required Mask processing internally. The Mask mode recovery behavior differs across products. For details, see [Overview](#overview).

    Example:

    ```cpp
    uint64_t count = 128;
    half addsValue = 1.0;
    AscendC::Adds<half, true>(dstLocal, srcLocal, addsValue, count);
    ```

    As shown in Figure 3, for first-n-data computation using Counter mode, the count parameter specifies the number of elements that participate in computation as 128, and the API automatically completes the Mask configuration.

    **Figure 3**  Example of first-n-data computation using Counter mode  
    ![](../../../../figures/counter_first_n_calc.png)<a id="figure-3-example-of-first-n-data-computation-using-counter-mode"></a>

- **High-dimensional slicing: continuous computation**

    In Counter mode continuous computation, only the lower 64 bits of the value in the Mask register are valid and are treated as the number of elements that participate in computation. The repeatTime parameter of the high-dimensional slicing vector computation API is ignored, and the system automatically computes the number of iterations based on the element count.

    Example:

    ```cpp
    half addsValue = 1.0;
    uint8_t repeatTimes = 1;
    AscendC::UnaryRepeatParams repeatParams{2, 2, 16, 16};
    AscendC::SetMaskCount();
    uint64_t mask[2] = {130, 0};
    AscendC::Adds<half, true>(dstLocal, srcLocal, addsValue, mask, repeatTimes, repeatParams);
    AscendC::SetMaskNorm();
    ```

    As shown in Figure 4, when using the high-dimensional slicing continuous computation API in Counter mode, only the lower 64 bits of the mask array are valid, indicating that the total number of elements that participate in computation is 130. In this mode, the repeatTimes parameter within the API is invalid, and the system automatically computes the number of iterations based on the mask value. The repeatParams parameter configuration indicates a gap between DataBlocks and continuity between iterations.

    **Figure 4**  Counter mode high-dimensional slicing continuous computation example  
    ![](../../../../figures/counter_high_dim_continuous.png)<a id="figure-4-counter-mode-high-dimensional-slicing-continuous-computation-example"></a>

### Normal Mode

In [Normal computation mode](#mask-mode), the Mask register is used as the mask for each Repeat. Only high-dimensional slicing supports this mode, which is further divided into bitwise mode and continuous mode on the API.

**Note: Normal is the default mode. If no mode switching occurs, you do not need to configure this mode again.**

- **High-dimensional slicing: bitwise computation**

    Example:

    ```cpp
    half addsValue = 1.0;
    int8_t repeatTimes = 1;
    UnaryRepeatParams repeatParams {2, 2, 16, 16};
    uint64_t mask[2] = {0x69A569A569A569A5, 0x69A569A569A569A5};
    AscendC::Adds<half, true>(dstLocal, srcLocal, addsValue, mask, repeatTimes, repeatParams);
    ```

    As shown in Figure 5, when Normal mode is used for bitwise computation, the mask parameter controls whether each element participates in computation in the form of a bit array, and the computation process of the elements in the first DataBlock is shown in detail. The repeatParams parameter configuration indicates that there is an interval between DataBlocks, and iterations are continuous.

    **Figure 5**  Normal mode high-dimensional slicing bitwise computation example  
    ![](../../../../figures/normal_high_dim_bitwise.png)<a id="figure-5-normal-mode-high-dimensional-slicing-bitwise-computation-example"></a>

- **High-dimensional slicing: continuous computation**

    Example:

    ```cpp
    half addsValue = 1.0;
    int8_t repeatTimes = 1;
    UnaryRepeatParams repeatParams{2, 2, 16, 16};
    uint64_t mask = 128;
    AscendC::Adds<half, true>(dstLocal, srcLocal, addsValue, mask, repeatTimes, repeatParams);
    ```

    As shown in Figure 6, when Normal mode is used for continuous computation, the mask parameter indicates that the number of elements that continuously participate in computation within an iteration is 128. The repeatParams parameter configuration indicates that there is an interval between DataBlocks, and iterations are continuous.

    **Figure 6**  Example of high-dimensional slicing continuous computation in Normal mode  
    ![](../../../../figures/normal_high_dim_continuous.png)<a id="figure-6-example-of-high-dimensional-slicing-continuous-computation-in-normal-mode"></a>

> [!NOTE]Note
> 
> In the high-dimensional slicing continuous computation scenario in Normal mode, the number of elements that can be processed in each iteration is related to the bit width of the operand data type: when the operand is 16-bit, mask∈\[0, 128\]; when the operand is 32-bit, mask∈\[0, 64\]; when the operand is 64-bit, mask∈\[0, 32\].
>
> When mask is greater than the maximum number of elements that the current data type can process in a single repeat, the operation is still executed as one repeat, that is, it is executed according to the maximum number of elements. Mask control that exceeds the maximum number of elements is not supported. For example, a 16-bit operand can process a maximum of 128 elements in a single repeat, a 32-bit operand can process a maximum of 64 elements, and a 64-bit operand can process a maximum of 32 elements.

## Setting the Mask Outside the API

### Counter Mode

- **First-n-data computation**

    Example:

    ```cpp
    half addsValue = 1.0;
    AscendC::SetMaskCount();
    AscendC::SetVectorMask<half, AscendC::MaskMode::COUNTER>(128);
    AscendC::Adds<half, false>(dstLocal, srcLocal, addsValue, 1);
    AscendC::SetMaskNorm();
    // ResetMask does not need to be called after each computation API call. Call it only when the mask state must be explicitly reset.
    AscendC::ResetMask();
    ```

    **Figure 7**  Example of first-n-data computation in Counter mode with the mask set outside the API  
    ![](../../../../figures/external_counter_first_n.png)<a id="figure-7-example-of-first-n-data-computation-in-counter-mode-with-the-mask-set-outside-the-api"></a>

- **High-dimensional slicing: continuous computation**

    In [Counter computation mode](#mask-mode), only the lower 64 bits of the value in the Mask register are valid and are treated as the number of elements participating in computation. In addition, the repeatTime parameter of the high-dimensional slicing vector computation API is ignored, and the system automatically calculates it based on the number of elements.

    Example:

    ```cpp
    half addsValue = 1.0;
    int8_t repeatTimes = 1;
    UnaryRepeatParams repeatParams {2, 2, 16, 16};
    AscendC::SetMaskCount();
    AscendC::SetVectorMask<half, AscendC::MaskMode::COUNTER>(130);
    AscendC::Adds<half, false>(dstLocal, srcLocal, addsValue, AscendC::MASK_PLACEHOLDER, repeatTimes, repeatParams);
    AscendC::SetMaskNorm();
    // ResetMask does not need to be called after each computation API call. Call it only when the mask state must be explicitly reset.
    AscendC::ResetMask(); 
    ```

    **Figure 8**  Example of continuous computation in Counter mode with the mask set outside the API  
    ![](../../../../figures/external_counter_continuous.png)<a id="figure-8-example-of-continuous-computation-in-counter-mode-with-the-mask-set-outside-the-api"></a>

### Normal mode

In [Normal computation mode](#mask-mode), the Mask register is used as the mask for each Repeat. Only high-dimensional slicing supports this mode, which is distinguished on the API as bitwise mode and continuous mode.

**Note: Normal is the default mode. If no mode switching occurs, you do not need to configure this mode again.**

- **High-dimensional slicing: bitwise computation**

    Example:

    ```cpp
    half addsValue = 1.0;
    int8_t repeatTimes = 1;
    UnaryRepeatParams repeatParams{2, 2, 16, 16};
    AscendC::SetMaskNorm();
    AscendC::SetVectorMask<half, AscendC::MaskMode::NORMAL>(0x69A569A569A569A5, 0x69A569A569A569A5);
    AscendC::Adds<half, false>(dstLocal, srcLocal, addsValue, AscendC::MASK_PLACEHOLDER, repeatTimes, repeatParams);
    // ResetMask does not need to be called after each computation API call. It is used only when the mask state needs to be explicitly reset.
    AscendC::ResetMask();
    ```

    **Figure 9**  Setting Mask outside the API, Normal mode high-dimensional slicing bitwise computation example  
    ![](../../../../figures/external_normal_bitwise.png)<a id="Figure 9-Setting Mask outside the API-Normal mode high-dimensional slicing bitwise computation example"></a>

- **High-dimensional slicing: continuous computation**

    Example:

    ```cpp
    half addsValue = 1.0;
    uint8_t repeatTimes = 1;
    AscendC::UnaryRepeatParams repeatParams{2, 2, 16, 16};
    AscendC::SetMaskNorm();
    AscendC::SetVectorMask<half, AscendC::MaskMode::NORMAL>(128);
    AscendC::Adds<half, false>(dstLocal, srcLocal, addsValue, AscendC::MASK_PLACEHOLDER, repeatTimes, repeatParams);
    // ResetMask does not need to be called after each computation API call. It is used only when the mask state needs to be explicitly reset.
    AscendC::ResetMask();
    ```

    **Figure 10**  Setting Mask outside the API, Normal mode high-dimensional slicing continuous computation example  
    ![](../../../../figures/external_normal_continuous.png)<a id="Figure 10-Setting Mask outside the API-Normal mode high-dimensional slicing continuous computation example"></a>

> [!NOTE]Note
> 
> In the high-dimensional slicing continuous computation scenario of Normal mode, the number of elements that can be processed in each iteration depends on the bit width of the operand data type: for 16-bit operands, mask∈\[0, 128\]; for 32-bit operands, mask∈\[0, 64\]; for 64-bit operands, mask∈\[0, 32\].
>
> When the mask exceeds the maximum number of elements that the current data type can process in a single repeat, the operation is still performed as one repeat, that is, it is performed with that maximum number of elements. Mask control beyond the maximum number of elements is not supported. For example, a single repeat processes a maximum of 128 elements for 16-bit operands, 64 elements for 32-bit operands, and 32 elements for 64-bit operands.

## General Description

### Common Parameter Description of the API

When the API requires mask operations, common parameters must be passed in. These parameters control the computation element range, iteration behavior, and mask configuration. Using the Adds API as an example, this section describes the meaning, input type, and usage notes of the **mask-related parameters**.

- Tensor first n elements computation

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Adds(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count)
    ```

- Tensor high-dimensional slicing computation
    - **mask** bit-by-bit mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Adds(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - **mask** continuous mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Adds(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

**Table 4** Template parameter description

|Parameter|Description|
|:---|:---|
|isSetMask|Whether to set the Mask inside the API.<br>&bull; true: Set the Mask [Setting the Mask Inside the API](#setting-the-mask-inside-the-api).<br>&bull; false: Set the Mask [Setting the Mask Outside the API](#setting-the-mask-outside-the-api). You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 5** Mask-related parameters<a id="sheet2"></a>

|Parameter|Input/Output|Meaning|
|:---|:---|:---|
|count|Input|Number of elements that participate in computation.<br>In Counter mode, when isSetMask = false, count is converted into a mask representation. The first count bits are set to 1, indicating that the first count elements participate in computation and the remaining elements are masked.|
|mask[]/mask|Input|The mask is stored in a 128-bit register. In mask Normal mode, it controls the elements that participate in computation bit by bit from the low bit to the high bit. A bit value of 1 indicates that the element participates in computation, and 0 indicates that it does not. It can control whether up to 128 elements participate in computation, and the number of valid bits depends on the data type involved in computation.<br>The API provides two ways to set the mask in mask Normal mode to control the elements that participate in computation within each iteration.<br>&bull;Bit-by-bit mode: The input is an array mask[] of the uint64\_t type, which directly sets the value of the mask register. The length of the input mask array must be greater than or equal to 2, and the number of valid bits depends on the operand type:<br>When the operand is 16-bit, 128 elements participate in computation in a single iteration, and two uint64\_t type values are required for setting. The valid array length is 2, maskLow∈[0, 2<sup>64</sup>-1], maskHigh∈[0, 2<sup>64</sup>-1];<br>When the operand is 32-bit, 64 elements participate in computation in a single iteration, and only one uint64\_t value is required. The valid array length is 1, maskLow∈[0, 2<sup>64</sup>-1], maskHigh = 0;<br>When the operand is 64-bit, 32 elements participate in computation in a single iteration, the array length is 1, the number of valid bits of mask[0] is 32, maskLow∈[0, 2<sup>32</sup>-1], maskHigh = 0.<br>For example, mask=[8, 2], where 8 and 2 represent the low 64 bits and the high 64 bits respectively. 8=0b1000 indicates that only the 4th element participates in computation; 2=0b0010 indicates that the 66th (64+2) element participates in computation.<br>&bull;Continuous mode: The input is a mask value of the uint64\_t type, indicating how many consecutive bits of the mask register are set to 1 starting from the low bit, that is, how many consecutive leading elements participate in computation. The value range depends on the operand data type. Different data types have different maximum numbers of elements that can be processed in each iteration.<br>When the operand is 16-bit, mask∈[0, 128];<br>When the operand is 32-bit, mask∈[0, 64];<br>When the operand is 64-bit, mask∈[0, 32].<br>Note: In continuous mode, if mask exceeds the range, it is calculated as the maximum mask value of the corresponding data type; mask = 0 indicates that the instruction does not perform computation, and the API is treated as a NOP (no operation); if a negative value is set, undefined behavior may occur.|

> [!CAUTION]Note
>
> - The maximum value of the count parameter is limited by the total input data capacity, which is jointly constrained by the system Unified Buffer (UB) size limit and the operand data type: under different data types, the maximum number of elements that can be accommodated in the same UB capacity differs.
>
> - The mask\[\]/mask parameter is valid only for high-dimensional slicing computation. When mask is 0, the destination operand retains its original value.
