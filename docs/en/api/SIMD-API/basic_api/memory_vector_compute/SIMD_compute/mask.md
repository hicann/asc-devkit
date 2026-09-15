# Mask

<!-- md-trans-meta sourceCommit=27a8d829e1498b83498ed8bfc14b3c9e69ae662d translatedAt=2026-09-09T15:01:40.174Z -->

## Overview

A mask is a sequence of flag bits used in Ascend C to control whether data elements participate in computation. Each mask bit corresponds to the position of an element in the data. By marking valid bits and invalid bits, the mask enables fine-grained on/off control over data operations.

A mask is represented by a numeric value with a fixed bit width:

- Valid bit (usually 1): The corresponding element participates in computation normally.
- Invalid bit (usually 0): The corresponding element is masked and no operation is performed on it.

In the [Continuous Computation](continuous_computation.md) API (that is, the first n data computation API of a Tensor), the mask is the input parameter n, indicating that the first n consecutive elements participate in computation.

In the [High-Dimensional Splitting](high_dimension_slicing.md) API, the mask is used to control the elements that participate in computation in each iteration. It can be set in two ways: continuous mode and bit-by-bit mode:

- Continuous mode: Indicates how many consecutive elements at the beginning of each iteration participate in computation.
- Bit-by-bit mode: Indicates, bit by bit, which elements participate in computation.

The mask application mechanism based on the first n data computation API of a Tensor and the High-Dimensional Splitting API is shown in [Figure 1 Mask Mechanism](#figure-1-mask-mechanism).

**Figure 1**  Mask mechanism  
![](../../../../figures/mask_mechanism.png)<a id="figure-1-mask-mechanism"></a>

> [!NOTE] Note
>
> <!-- npu="A3,910b" id1 -->
> - For the following models, Mask is a hardware-native capability. When the Tensor First N Data Computation API is called, the API internally sets Mask to Counter mode and restores it to the system-default Normal mode after the computation is complete.
>   <!-- npu="A3" id2 -->
>   - Atlas A3 training products/Atlas A3 inference products
>   <!-- end id2 -->
>   <!-- npu="910b" id3 -->
>   - Atlas A2 training products/Atlas A2 inference products
>   <!-- end id3 -->
> <!-- end id1 -->
> <!-- npu="950" id4 -->
> On Ascend 950PR/Ascend 950DT, Mask is implemented through software emulation to ensure compatibility, and the capabilities provided by its underlying hardware differ to a certain extent from those of Atlas A2 training products/Atlas A2 inference products and Atlas A3 training products/Atlas A3 inference products.<br>
> It is recommended to use the Tensor First N Data Computation API, which achieves compatibility by calling the [Reg Vector Computation API](../../reg_vector_compute/reg_vector_computation.md) through VF. The Tensor First N Data Computation API does not switch Mask mode to Normal mode internally. If Mask has been set to Counter mode through [SetMaskCount](../mask_operations/SetMaskCount.md) before this API is called, it remains in Counter mode after the call. When Normal mode is required for subsequent computation, [SetMaskNorm](../mask_operations/SetMaskNorm.md) must be called manually to set it.
> <!-- end id4 -->

### Mask Configuration Method

The mask setting method is jointly determined by the call method of the specific API and the template parameter isSetMask. Two modes are supported: setting the Mask inside the API and setting the Mask outside the API.

**Table 1**  Mask configuration methods

|Configuration Method|Description|
|:---|:---|
|[Setting Mask Inside the API](#setting-mask-inside-the-api) (default)|&bull;The mask value is directly passed through the input parameter of the vector computation API.<br>&bull;The template parameter isSetMask of the vector computation API (supported by only some APIs) controls whether the mask is passed through the API parameter or configured by an external API. The default value is true, indicating that the mask is passed through the API parameter. The Mask corresponds to the mask/mask[] parameter in the high-dimensional splitting computation API or the calCount parameter in the first n data computation API.|
|[Setting Mask Outside the API](#setting-mask-outside-the-api)|&bull;The Mask value is set through the [SetVectorMask](../mask_operations/SetVectorMask.md) API. The template parameter isSetMask of the vector computation API is set to false, and the mask parameter in the API input parameter (corresponding to the mask/mask[] parameter in the high-dimensional splitting computation API or the calCount parameter in the first n data computation API) does not take effect.<br>&bull;This method applies to scenarios where the mask parameter is the same and is used repeatedly. It eliminates the need to set the mask repeatedly inside the vector computation API, providing certain performance advantages.|

The Mask supports the following working modes:

**Table 2**  Mask working modes<a id="mask-mode"></a>

|Working Mode|Description|
|:---|:---|
|Normal mode|The default mode. It supports the Mask capability within a single iteration. Developers need to configure the number of iterations and additionally compute the tail block.<br>**In Normal mode, the Mask is used to control the number of elements that participate in computation within a single iteration.**<br>Call [SetMaskNorm](../mask_operations/SetMaskNorm.md) to set the Normal mode.|
|Counter mode|A simplified mode. The amount of data to be computed is directly passed in, and the number of iterations is automatically inferred. Developers do not need to perceive the number of iterations or handle non-aligned tail blocks. However, it does not support the Mask capability within a single iteration.<br>**In Counter mode, the Mask indicates the number of elements that participate in computation in the entire vector computation.**<br>Call [SetMaskCount](../mask_operations/SetMaskCount.md) to set the Counter mode.|

The Mask operation is used as follows:

**Table 3**  Usage of the Mask operation

|Configuration Method|Working Mode|First N Data Computation API|High-Dimensional Splitting Computation API|
 |:---|:---|:---|:---|
 |[Setting Mask Inside the API](#setting-mask-inside-the-api)|[Normal mode](#mask-mode)|Not involved.|Set the isSetMask template parameter to true, pass the mask through the API input parameter, and configure the dataBlockStride, repeatStride, and repeatTime parameters based on the usage scenario.|
 |[Setting Mask Inside the API](#setting-mask-inside-the-api)|[Counter mode](#mask-mode)|Set the isSetMask template parameter to true and pass the mask through the API input parameter.|Set the isSetMask template parameter to true and pass the mask through the API input parameter.<br>Configure the dataBlockStride and repeatStride parameters based on the usage scenario. Pass a fixed value for repeatTime. It is recommended to set it to 1 uniformly, and this value does not take effect.|
 |[Setting the Mask Outside the API](#setting-mask-outside-the-api)|[Normal mode](#mask-mode)|Not involved.|Call [SetVectorMask](../mask_operations/SetVectorMask.md) to set the Mask, and then call the high-dimensional splitting computation API.<br>&bull; Set the isSetMask template parameter to false, and set the mask value in the API input parameter to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning.<br>&bull;Configure the repeatTime, dataBlockStride, and repeatStride parameters based on the usage scenario.|
 |[Setting the Mask Outside the API](#setting-mask-outside-the-api)|[Counter mode](#mask-mode)|Set the isSetMask template parameter to false, call [SetVectorMask](../mask_operations/SetVectorMask.md) to set the Mask, and then call the first n data computation API. It is recommended to set count in the API input parameter to 1.|Call [SetVectorMask](../mask_operations/SetVectorMask.md) to set the Mask, and then call the high-dimensional splitting computation API.<br>&bull; Set the isSetMask template parameter to false, and set the mask value in the API input parameter to `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning.<br>&bull;Configure the dataBlockStride and repeatStride parameters based on the usage scenario. Pass a fixed value for repeatTime. It is recommended to set it to 1 uniformly, and this value does not take effect.|

Users must correctly select the mask configuration method based on the API usage scenario and parameter configuration, as shown in [Figure 2 Code comparison of the two configuration methods](#figure-2-code-comparison-of-the-two-configuration-methods):

**Figure 2**  Code comparison of the two configuration methods  
![](../../../../figures/two_config_comparison.png)<a id="figure-2-code-comparison-of-the-two-configuration-methods"></a>

> [!CAUTION] Caution:
>
> - Only some APIs support isSetMask. For details about the supported APIs, see the function prototype of the corresponding API.
>
> - **For performance practice reference on setting the Mask outside the API, see [Computation Performance Optimization Based on Global Mask Reuse](../../../../../guide/operator_practice/simd_operator_optimization/vector_compute/compute_performance_optimization_based_on_global_mask_reuse.md)**.

## Setting Mask Inside the API

### Counter Mode

- **First N Data Computation**

    The [Counter Mode](#mask-mode) is used for first N data computation. The number of elements that participate in computation is specified by the API parameter `count`, and the API automatically completes the Mask processing required for computation. The Mask mode recovery behavior varies by product. For details, see the description in [Overview](#overview).

    Example:

    ```cpp
    uint64_t count = 128;
    half addsValue = 1.0;
    AscendC::Adds<half, true>(dstLocal, srcLocal, addsValue, count);
    ```

    As shown in Figure 3, the Counter Mode is used for first N data computation. The number of elements that participate in computation is specified as 128 by the `count` parameter, and the API automatically completes the Mask setting.

    **Figure 3**  Example of first N data computation in Counter Mode  
    ![](../../../../figures/counter_first_n_calc.png)<a id="figure-3-counter-mode-first-n-data-computation-example"></a>

- **High-Dimensional Splitting: Continuous Computation**

    In continuous computation in Counter Mode, only the lower 64 bits of the value in the Mask register are valid and are treated as the number of elements that participate in computation. The `repeatTime` parameter of the high-dimensional splitting vector computation API is ignored, and the system automatically calculates it based on the number of elements.

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

    As shown in Figure 4, when the high-dimensional splitting continuous computation API in Counter Mode is used, only the lower 64 bits of the mask array are valid, indicating that the total number of elements that participate in computation is 130. In this mode, the `repeatTimes` parameter inside the API is invalid, and the system automatically calculates the number of iterations based on the mask value. The `repeatParams` parameter configuration indicates that there is a gap between DataBlocks and that iterations are continuous.

    **Figure 4**  Example of high-dimensional splitting continuous computation in Counter mode  
    ![](../../../../figures/counter_high_dim_continuous.png)<a id="figure-4-example-of-high-dimensional-splitting-continuous-computation-in-counter-mode"></a>

### Normal Mode

In [Normal computation mode](#mask-mode), the Mask register is used as the mask for each Repeat. Only high-dimensional splitting supports this mode, which is distinguished at the API level into bitwise mode and continuous mode.

**Note: Normal is the default mode. If no Mode Switching occurs, this mode does not need to be configured repeatedly.**

- **High-Dimensional Splitting: Bitwise Computation**

    Example:

    ```cpp
    half addsValue = 1.0;
    int8_t repeatTimes = 1;
    UnaryRepeatParams repeatParams {2, 2, 16, 16};
    uint64_t mask[2] = {0x69A569A569A569A5, 0x69A569A569A569A5};
    AscendC::Adds<half, true>(dstLocal, srcLocal, addsValue, mask, repeatTimes, repeatParams);
    ```

    As shown in Figure 5, when bitwise computation is performed in Normal mode, the mask parameter controls whether each element participates in computation in the form of a bit array, and the computation process of the elements in the first DataBlock is illustrated in detail. The repeatParams parameter configuration indicates that there is an interval between DataBlocks and that iterations are continuous.

    **Figure 5**  Normal mode high-dimensional splitting bitwise computation example  
    ![](../../../../figures/normal_high_dim_bitwise.png)<a id="figure-5-normal-mode-high-dimensional-splitting-bitwise-computation-example"></a>

- **High-Dimensional Splitting: Continuous Computation**

    Example:

    ```cpp
    half addsValue = 1.0;
    int8_t repeatTimes = 1;
    UnaryRepeatParams repeatParams{2, 2, 16, 16};
    uint64_t mask = 128;
    AscendC::Adds<half, true>(dstLocal, srcLocal, addsValue, mask, repeatTimes, repeatParams);
    ```

    As shown in Figure 6, when Continuous Computation is performed in Normal mode, the mask parameter indicates that the number of elements that continuously participate in computation within an iteration is 128. The repeatParams parameter configuration indicates that there is an interval between DataBlocks and that iterations are continuous.

    **Figure 6**  Example of high-dimensional splitting continuous computation in Normal mode  
    ![](../../../../figures/normal_high_dim_continuous.png)<a id="figure-6-example-of-high-dimensional-splitting-continuous-computation-in-normal-mode"></a>

> [!NOTE] Note
> 
> When the data type is half (the operand is 16 bits), the number of elements that can be processed in each iteration is mask∈\[0, 128\]. When mask\>128, it is still executed as one repeat (128 elements), and mask control exceeding 128 elements is not supported.

## Setting Mask Outside the API

### Counter Mode

- **First N Data Computation**

    Example:

    ```cpp
    half addsValue = 1.0;
    AscendC::SetMaskCount();
    AscendC::SetVectorMask<half, AscendC::MaskMode::COUNTER>(128);
    AscendC::Adds<half, false>(dstLocal, srcLocal, addsValue, 1);
    AscendC::SetMaskNorm();
    // ResetMask does not need to be called after each computation API call. It is used only when the mask state needs to be explicitly reset.
    AscendC::ResetMask();
    ```

    **Figure 7**  Example of first N data computation in Counter mode with the mask set outside the API  
    ![](../../../../figures/external_counter_first_n.png)<a id="fig7-external-mask-setting-counter-mode-first-n-data-computation-example"></a>

- **High-Dimensional Splitting: Continuous Computation**

    In [Counter computation mode](#mask-mode), only the lower 64 bits of the value in the Mask register are valid and are used as the number of elements that participate in computation. In addition, the repeatTime parameter of the high-dimensional splitting vector computation API is ignored and is automatically calculated by the system based on the number of elements.

    Example:

    ```cpp
    half addsValue = 1.0;
    int8_t repeatTimes = 1;
    UnaryRepeatParams repeatParams {2, 2, 16, 16};
    AscendC::SetMaskCount();
    AscendC::SetVectorMask<half, AscendC::MaskMode::COUNTER>(130);
    AscendC::Adds<half, false>(dstLocal, srcLocal, addsValue, AscendC::MASK_PLACEHOLDER, repeatTimes, repeatParams);
    AscendC::SetMaskNorm();
    // ResetMask does not need to be called after each computation API call. It is used only when the mask state needs to be explicitly reset.
    AscendC::ResetMask(); 
    ```

    **Figure 8**  Example of continuous computation in Counter mode with the mask set outside the API  
    ![](../../../../figures/external_counter_continuous.png)<a id="fig8-external-mask-setting-counter-mode-continuous-computation-example"></a>

### Normal Mode

In [Normal computation mode](#mask-mode), the Mask register is used as the mask for each Repeat. Only high-dimensional splitting supports this mode, which is distinguished on the API as bitwise mode and continuous mode.

**Note: Normal is the default mode. If no mode switching has occurred, this mode does not need to be configured repeatedly.**

- **High-dimensional splitting: bitwise computation**

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

    **Figure 9**  Example of bitwise computation with high-dimensional splitting in Normal mode when the Mask is set outside the API  
    ![](../../../../figures/external_normal_bitwise.png)<a id="fig9-external-mask-setting-normal-mode-high-dimensional-splitting-bitwise-computation-example"></a>

- **High-dimensional splitting: continuous computation**

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

    **Figure 10**  Example of high-dimensional splitting continuous computation in Normal mode with Mask set outside the API  
    ![](../../../../figures/external_normal_continuous.png)<a id="figure-10-mask-set-outside-the-api-normal-mode-high-dimensional-splitting-continuous-computation-example"></a>

> [!NOTE] Note
> 
> When the data type is half (the operand is 16-bit), the number of elements that can be processed in each iteration is mask∈\[0, 128\]. When mask\>128, it is still executed as one repeat (128 elements), and mask control over more than 128 elements is not supported.

## General Description

### Common API Parameters

When an API needs to perform a mask operation, common parameters must be passed in. These parameters are used to control the computation element range, iteration behavior, and mask configuration. Taking the API Adds as an example, the meaning, input type, and usage precautions of the **mask-related parameters** are described in detail.

- Computation of the first n data of a tensor

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Adds(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count)
    ```

- High-dimensional splitting computation of a tensor
    - Bitwise mask mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Adds(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - Continuous mask mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Adds(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

**Table 4** Template parameters

|Parameter|Description|
|:---|:---|
|isSetMask|Whether to set the Mask inside the API.<br>&bull; true: the Mask is set [inside the API](#setting-mask-inside-the-api).<br>&bull; false: the Mask is set [outside the API](#setting-mask-outside-the-api). In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 5** Mask-related parameters<a id="sheet2"></a>

|Parameter|Input/Output|Meaning|
|:---|:---|:---|
|count|Input|Number of elements that participate in computation.<br>In Counter mode, when isSetMask = false, count is converted into a mask representation. The first count bits are set to 1, indicating that the first count elements participate in computation and the remaining elements are masked.|
 |mask[]/mask|Input|The mask is stored in a 128-bit register. In Normal mask mode, it controls the elements that participate in computation bit by bit from the low bit to the high bit. A bit value of 1 indicates that the element participates in computation, and 0 indicates that it does not. A maximum of 128 elements can be controlled, and the number of valid bits depends on the data type involved in computation.<br>The API provides two ways to set the mask in Normal mask mode to control the elements that participate in computation in each iteration.<br>&bull; Bit-by-bit mode: the input is an array mask[] of the uint64\_t type, which directly sets the value of the mask register. The length of the input mask array must be greater than or equal to 2, and the number of valid bits depends on the operand type:<br>When the operand is 16-bit, 128 elements participate in computation in one iteration, and two uint64\_t type values are required for setting. The valid array length is 2, maskLow∈[0, 2<sup>64</sup>-1], and maskHigh∈[0, 2<sup>64</sup>-1].<br>When the operand is 32-bit, 64 elements participate in computation in one iteration, and only one uint64\_t value is required. The valid array length is 1, maskLow∈[0, 2<sup>64</sup>-1], and maskHigh = 0.<br>When the operand is 64-bit, 32 elements participate in computation in one iteration, the array length is 1, the number of valid bits of mask[0] is 32, maskLow∈[0, 2<sup>32</sup>-1], and maskHigh = 0.<br>For example, mask=[8, 2], where 8 and 2 represent the low 64 bits and the high 64 bits respectively. 8=0b1000 indicates that only the 4th element participates in computation; 2=0b0010 indicates that the 66th (64+2) element participates in computation.<br>&bull; Continuous mode: the input is a value mask of the uint64\_t type, indicating how many consecutive bits from the low bit of the mask register are set to 1, that is, how many consecutive elements from the beginning participate in computation. The value range depends on the operand data type. Different data types have different maximum numbers of elements that can be processed in each iteration.<br>When the operand is 16-bit, mask∈[0, 128].<br>When the operand is 32-bit, mask∈[0, 64].<br>When the operand is 64-bit, mask∈[0, 32].<br>Note: In continuous mode, if mask exceeds the range, it is calculated as the maximum mask value of the corresponding data type. mask = 0 indicates that the instruction does not perform any computation operation, and the API is treated as a NOP (no operation). If mask is set to a negative value, undefined behavior may occur.|

> [!CAUTION] Caution
>
> - The maximum value of the count parameter is limited by the total capacity of the input data, which is jointly constrained by the system UB size limit and the operand data type: under different data types, the maximum number of elements that can be accommodated in the same UB capacity differs.
>
> - The mask\[\]/mask parameter is valid only in high-dimensional splitting computation. When mask is 0, the destination operand retains its original value.
