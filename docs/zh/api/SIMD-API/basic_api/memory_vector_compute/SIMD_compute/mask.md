# 掩码

## 概述

Mask（掩码）是Ascend C中用于控制数据元素是否参与计算的标识位序列。每一位掩码对应数据中的一个元素的位置，通过有效位和无效位标记，实现对数据操作的精细化开关控制。

掩码由固定位宽的数值表示：

- 有效位（通常为1）：对应元素正常参与计算。
- 无效位（通常为0）：对应元素被屏蔽，不执行操作。

在[连续计算](continuous_compute.md)接口（即Tensor前n个数计算接口）中，Mask为输入参数n，表示前n个连续元素参与计算。

在[高维切分](high_dim_split.md)接口中，Mask用于控制每次迭代内参与计算的元素。可通过连续模式和逐bit模式两种方式设置：

- 连续模式：表示每个迭代内前面连续的多少个元素参与计算。
- 逐bit模式：表示按位控制哪些元素参与计算。

基于Tensor前n个数计算及高维切分接口的Mask应用机制如[图1掩码机制说明](#图1-掩码机制说明)所示。

**图1**  掩码机制说明  
![](../../../../figures/mask_mechanism.png)<a id="图1-掩码机制说明"></a>

> [!NOTE]说明
>
> <!-- npu="A3,910b" id1 -->
> - 针对如下型号，Mask为硬件原生支持能力。调用Tensor前n个数据计算接口时，接口内部会将Mask设置为Counter模式，并在计算完成后恢复为系统默认的Normal模式。
>   <!-- npu="A3" id2 -->
>   - Atlas A3 训练系列产品/Atlas A3 推理系列产品
>   <!-- end id2 -->
>   <!-- npu="910b" id3 -->
>   - Atlas A2 训练系列产品/Atlas A2 推理系列产品
>   <!-- end id3 -->
> <!-- end id1 -->
> <!-- npu="950" id4 -->
> - 在Ascend 950PR/Ascend 950DT上，Mask为保证兼容性采用软仿实现，其硬件底层提供的能力与Atlas A2 训练系列产品/Atlas A2 推理系列产品和Atlas A3 训练系列产品/Atlas A3 推理系列产品存在一定差异。推荐使用Tensor前n个数据计算接口，该接口通过VF调用[Reg矢量计算API](../../reg_vector_compute/reg_vector_compute.md)实现兼容。Tensor前n个数据计算接口内部不会将Mask模式切换为Normal模式。若调用该接口前已通过[SetMaskCount](../mask_operations/SetMaskCount.md)设置为Counter模式，接口调用后仍为Counter模式；后续计算需要使用Normal模式时，需手动调用[SetMaskNorm](../mask_operations/SetMaskNorm.md)进行设置。
> <!-- end id4 -->

### 掩码配置方式

掩码的设置方式由具体接口的调用方式和模板参数isSetMask共同决定，支持接口内设置Mask和接口外设置Mask两种模式。

**表1**  Mask配置方式

|配置方式|说明|
|:---|:---|
|[接口内设置Mask](#接口内设置Mask)（默认）|&bull;通过矢量计算API的入参直接传递mask值。<br>&bull;矢量计算API的模板参数isSetMask（仅部分API支持）用于控制接口传参还是外部API配置，默认值为true，表示接口传参。Mask对应于高维切分计算API中的mask/mask[]参数或者tensor前n个数据计算API中的calCount参数。|
|[接口外设置Mask](#接口外设置Mask)|&bull;通过[SetVectorMask](../mask_operations/SetVectorMask.md)接口设置Mask值，矢量计算API的模板参数isSetMask设置为false，接口入参中的mask参数（对应于高维切分计算API中的mask/mask[]参数或者tensor前n个数据计算API中的calCount参数）不生效。<br>&bull;适用于mask参数相同、多次重复使用的场景，无需在矢量计算API内部反复设置，会有一定的性能优势。|

Mask支持以下工作模式：

**表2**  Mask工作模式<a id="mask-mode"></a>

|工作模式|说明|
|:---|:---|
|Normal模式|默认模式，支持单次迭代内的Mask能力，需要开发者配置迭代次数，额外进行尾块的计算。<br>**Normal模式下，Mask用来控制单次迭代内参与计算的元素个数。**<br>通过调用[SetMaskNorm](../mask_operations/SetMaskNorm.md)设置Normal模式。|
|Counter模式|简化模式，直接传入计算数据量，自动推断迭代次数，不需要开发者去感知迭代次数、处理非对齐尾块的操作；但是不具备单次迭代内的Mask能力。<br>**Counter模式下，Mask表示整个矢量计算参与计算的元素个数。**<br>通过调用[SetMaskCount](../mask_operations/SetMaskCount.md)设置Counter模式。|

Mask操作的使用方式如下：

**表3**  Mask操作的使用方式

|配置方式|工作模式|前n个数据计算API|高维切分计算API|
|:---|:---|:---|:---|
|[接口内设置Mask](#接口内设置mask)|[Normal模式](#mask-mode)|不涉及。|isSetMask模板参数设置为true，通过接口入参传入mask，根据使用场景配置dataBlockStride、repeatStride、repeatTime参数。|
|[接口内设置Mask](#接口内设置mask)|[Counter模式](#mask-mode)|isSetMask模板参数设置为true，通过接口入参传入mask。|isSetMask模板参数设置为true，通过接口入参传入mask。<br>根据使用场景配置dataBlockStride、repeatStride参数。repeatTime传入固定值即可，建议统一设置为1，该值不生效。|
|[接口外设置Mask](#接口外设置mask)|[Normal模式](#mask-mode)|不涉及。|调用[SetVectorMask](../mask_operations/SetVectorMask.md)设置Mask，之后调用高维切分计算API。<br>&bull; isSetMask模板参数设置为false，接口入参中的mask值设置为占位符`MASK_PLACEHOLDER`，用于占位，无实际含义。<br>&bull;根据使用场景配置repeatTime、dataBlockStride、repeatStride参数。|
|[接口外设置Mask](#接口外设置mask)|[Counter模式](#mask-mode)|isSetMask模板参数设置为false，调用[SetVectorMask](../mask_operations/SetVectorMask.md)设置Mask，之后调用前n个数据计算API；接口入参中的count建议设置成1。|调用[SetVectorMask](../mask_operations/SetVectorMask.md)设置Mask，之后调用高维切分计算API。<br>&bull; isSetMask模板参数设置为false；接口入参中的mask值设置为`MASK_PLACEHOLDER`，用于占位，无实际含义。<br>&bull;根据使用场景配置dataBlockStride、repeatStride参数。repeatTime传入固定值即可，建议统一设置为1，该值不生效。|

用户需根据接口使用场景和参数配置正确选择掩码配置方式，如[图2 两种配置方式代码对比](#图2-两种配置方式代码对比)所示：

**图2**  两种配置方式代码对比  
![](../../../../figures/two_config_comparison.png)<a id="图2-两种配置方式代码对比"></a>

> [!CAUTION]注意：
>
> - 仅部分API支持isSetMask，具体支持情况请参考对应API的函数原型；
>
> - 接口外设置Mask**性能实践参考：[基于全局掩码复用的计算性能优化](../../../../../guide/operator_practice/simd_operator_optimization/vector_compute/mask_reuse_optimization.md)**。

## 接口内设置Mask

### Counter模式

- **前n个数据计算**

    采用[Counter模式](#mask-mode)进行前n个数据计算，通过接口参数count指定参与计算的元素个数，接口内部会自动完成计算所需的Mask处理。不同产品的Mask模式恢复行为存在差异，具体请参考[概述](#概述)中的说明。

    调用示例：

    ```cpp
    uint64_t count = 128;
    half addsValue = 1.0;
    AscendC::Adds<half, true>(dstLocal, srcLocal, addsValue, count);
    ```

    如图3所示，采用Counter模式进行前n个数据计算，通过count参数指定参与计算的元素数量为128，接口内部会自动完成Mask的设置。

    **图3**  Counter模式进行前n个数据计算示例  
    ![](../../../../figures/counter_first_n_calc.png)<a id="图3-counter模式进行前n个数据计算示例"></a>

- **高维切分：连续计算**

    在Counter模式连续计算中，Mask寄存器中的值只有低64bit有效，被当做参与计算的元素数量。高维切分矢量计算接口的repeatTime参数会被忽略，由系统内部根据元素数量自动计算。

    调用示例：

    ```cpp
    half addsValue = 1.0;
    uint8_t repeatTimes = 1;
    AscendC::UnaryRepeatParams repeatParams{2, 2, 16, 16};
    AscendC::SetMaskCount();
    uint64_t mask[2] = {130, 0};
    AscendC::Adds<half, true>(dstLocal, srcLocal, addsValue, mask, repeatTimes, repeatParams);
    AscendC::SetMaskNorm();
    ```

    如图4所示，采用Counter模式下的高维切分连续计算接口，mask数组中仅低64bit有效，表示参与计算的元素总数为130。该模式接口内部的repeatTimes参数无效，系统根据mask值自动计算迭代次数。repeatParams参数配置表示DataBlock间存在间隔，迭代间连续。

    **图4**  Counter模式进行高维切分连续计算示例  
    ![](../../../../figures/counter_high_dim_continuous.png)<a id="图4-counter模式进行高维切分连续计算示例"></a>

### Normal模式

在[Normal计算模式](#mask-mode)中，Mask寄存器会作为每个Repeat的掩码使用。只有高维切分支持此模式，在接口上区分为逐bit模式和连续模式。

**注：Normal为默认模式。若未发生模式切换，则无需重复配置该模式。**

- **高维切分：逐bit计算**

    调用示例：

    ```cpp
    half addsValue = 1.0;
    int8_t repeatTimes = 1;
    UnaryRepeatParams repeatParams {2, 2, 16, 16};
    uint64_t mask[2] = {0x69A569A569A569A5, 0x69A569A569A569A5};
    AscendC::Adds<half, true>(dstLocal, srcLocal, addsValue, mask, repeatTimes, repeatParams);
    ```

    如图5所示，采用Normal模式进行逐bit计算时，mask参数以位数组形式控制每个元素是否参与计算，详细展示了第一个DataBlock中元素的计算过程。repeatParams参数配置表示DataBlock间存在间隔，迭代间连续。

    **图5**  Normal模式高维切分逐bit计算示例  
    ![](../../../../figures/normal_high_dim_bitwise.png)<a id="图5-normal模式高维切分逐bit计算示例"></a>

- **高维切分：连续计算**

    调用示例：

    ```cpp
    half addsValue = 1.0;
    int8_t repeatTimes = 1;
    UnaryRepeatParams repeatParams{2, 2, 16, 16};
    uint64_t mask = 128;
    AscendC::Adds<half, true>(dstLocal, srcLocal, addsValue, mask, repeatTimes, repeatParams);
    ```

    如图6所示，采用Normal模式进行连续计算时，mask参数表示迭代内连续参与计算的元素数量为128。repeatParams参数配置表示DataBlock间存在间隔，迭代间连续。

    **图6**  Normal模式进行高维切分连续计算示例  
    ![](../../../../figures/normal_high_dim_continuous.png)<a id="图6-normal模式进行高维切分连续计算示例"></a>

> [!NOTE]说明
> 
> 在Normal模式的高维切分连续计算场景下，每次迭代内能够处理的元素个数与操作数的数据类型位宽有关：操作数为16位时，mask∈\[0, 128\]；操作数为32位时，mask∈\[0, 64\]；操作数为64位时，mask∈\[0, 32\]。
>
> 当mask大于当前数据类型在单次repeat内可处理的最大元素个数时，仍按一次repeat执行，即按该最大元素个数执行，不支持超过最大元素个数的mask控制。例如：16位操作数单次repeat最多处理128个元素，32位操作数最多处理64个元素，64位操作数最多处理32个元素。

## 接口外设置Mask

### Counter模式

- **前n个数据计算**

    调用示例：

    ```cpp
    half addsValue = 1.0;
    AscendC::SetMaskCount();
    AscendC::SetVectorMask<half, AscendC::MaskMode::COUNTER>(128);
    AscendC::Adds<half, false>(dstLocal, srcLocal, addsValue, 1);
    AscendC::SetMaskNorm();
    // ResetMask无需在每次计算接口调用后调用，仅在需显式重置掩码状态时使用
    AscendC::ResetMask();
    ```

    **图7**  接口外设置Mask，Counter模式前n个数据计算示例  
    ![](../../../../figures/external_counter_first_n.png)<a id="图7-接口外设置Mask-counter模式前n个数据计算示例"></a>

- **高维切分：连续计算**

    在[Counter计算模式](#mask-mode)中，Mask寄存器中的值只有低64bit有效，被当做参与计算的元素数量。同时高维切分矢量计算接口的repeatTime参数会被忽略，由系统内部根据元素数量自动计算。

    调用示例：

    ```cpp
    half addsValue = 1.0;
    int8_t repeatTimes = 1;
    UnaryRepeatParams repeatParams {2, 2, 16, 16};
    AscendC::SetMaskCount();
    AscendC::SetVectorMask<half, AscendC::MaskMode::COUNTER>(130);
    AscendC::Adds<half, false>(dstLocal, srcLocal, addsValue, AscendC::MASK_PLACEHOLDER, repeatTimes, repeatParams);
    AscendC::SetMaskNorm();
    // ResetMask无需在每次计算接口调用后调用，仅在需显式重置掩码状态时使用
    AscendC::ResetMask(); 
    ```

    **图8**  接口外设置Mask，Counter模式连续计算示例  
    ![](../../../../figures/external_counter_continuous.png)<a id="图8-接口外设置Mask-counter模式连续计算示例"></a>

### Normal模式

在[Normal计算模式](#mask-mode)中，Mask寄存器会作为每个Repeat的掩码使用。只有高维切分支持此模式，在接口上区分为逐bit模式和连续模式。

**注：Normal为默认模式。若未发生模式切换，则无需重复配置该模式。**

- **高维切分：逐bit计算**

    调用示例：

    ```cpp
    half addsValue = 1.0;
    int8_t repeatTimes = 1;
    UnaryRepeatParams repeatParams{2, 2, 16, 16};
    AscendC::SetMaskNorm();
    AscendC::SetVectorMask<half, AscendC::MaskMode::NORMAL>(0x69A569A569A569A5, 0x69A569A569A569A5);
    AscendC::Adds<half, false>(dstLocal, srcLocal, addsValue, AscendC::MASK_PLACEHOLDER, repeatTimes, repeatParams);
    // ResetMask无需在每次计算接口调用后调用，仅在需显式重置掩码状态时使用
    AscendC::ResetMask();
    ```

    **图9**  接口外设置Mask，Normal模式高维切分逐bit计算示例  
    ![](../../../../figures/external_normal_bitwise.png)<a id="图9-接口外设置Mask-normal模式高维切分逐bit计算示例"></a>

- **高维切分：连续计算**

    调用示例：

    ```cpp
    half addsValue = 1.0;
    uint8_t repeatTimes = 1;
    AscendC::UnaryRepeatParams repeatParams{2, 2, 16, 16};
    AscendC::SetMaskNorm();
    AscendC::SetVectorMask<half, AscendC::MaskMode::NORMAL>(128);
    AscendC::Adds<half, false>(dstLocal, srcLocal, addsValue, AscendC::MASK_PLACEHOLDER, repeatTimes, repeatParams);
    // ResetMask无需在每次计算接口调用后调用，仅在需显式重置掩码状态时使用
    AscendC::ResetMask();
    ```

    **图10**  接口外设置Mask，Normal模式高维切分连续计算示例  
    ![](../../../../figures/external_normal_continuous.png)<a id="图10-接口外设置Mask-normal模式高维切分连续计算示例"></a>

> [!NOTE]说明
> 
> 在Normal模式的高维切分连续计算场景下，每次迭代内能够处理的元素个数与操作数的数据类型位宽有关：操作数为16位时，mask∈\[0, 128\]；操作数为32位时，mask∈\[0, 64\]；操作数为64位时，mask∈\[0, 32\]。
>
> 当mask大于当前数据类型在单次repeat内可处理的最大元素个数时，仍按一次repeat执行，即按该最大元素个数执行，不支持超过最大元素个数的mask控制。例如：16位操作数单次repeat最多处理128个元素，32位操作数最多处理64个元素，64位操作数最多处理32个元素。

## 通用说明

### 接口通用参数说明

接口需要进行掩码操作时，需要传入通用参数，这些参数用于控制计算元素范围、迭代行为及掩码配置。以API Adds为例，展开说明了**掩码相关参数**的含义、输入类型及使用注意事项。

- tensor前n个数据计算

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Adds(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count)
    ```

- tensor高维切分计算
    - mask逐bit模式

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Adds(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - mask连续模式

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Adds(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

**表4**  模板参数说明

|参数名|描述|
|:---|:---|
|isSetMask|是否在接口内部设置Mask。<br>&bull; true，表示在[接口内设置Mask](#接口内设置Mask)。<br>&bull; false，表示在[接口外设置Mask](#接口外设置Mask)，开发者需要使用[SetVectorMask](../mask_operations/SetVectorMask.md)接口设置mask值。这种模式下，接口入参中的mask值设置为占位符`MASK_PLACEHOLDER`，用于占位，无实际含义。 |

**表5**  掩码相关参数<a id="sheet2"></a>

|参数名称|输入/输出|含义|
|:---|:---|:---|
|count|输入|参与计算的元素个数。<br>在Counter模式下，当isSetMask = false，count会被转换成掩码表示。前count位设置为1，表示前count个元素参与计算，其余元素被屏蔽。| 
|mask[]/mask|输入|mask存储在一个128bit的寄存器中，在掩码Normal模式下从低位到高位按位控制参与计算的元素，该位的值为1表示参与计算，0表示不参与，最多可以控制128个元素是否参与计算，有效位数与参与计算的数据类型有关。<br>接口提供两种方式设置掩码Normal模式下的mask，控制每次迭代内参与计算的元素。<br>&bull;逐bit模式：输入为uint64\_t类型的数组mask[]，直接设置mask寄存器的值，输入的mask数组长度大于等于2，其中有效的位数与操作数的类型有关：<br>当操作数为16位时，一次迭代中有128个元素参与计算，需要使用两个uint64\_t类型的数据进行设置，数组有效长度为2，maskLow∈[0, 2<sup>64</sup>-1]， maskHigh∈[0, 2<sup>64</sup>-1]；<br>当操作数为32位时，一次迭代有64个数参与计算，仅需一个uint64\_t的数据即可，数组有效长度为1，maskLow∈[0, 2<sup>64</sup>-1]， maskHigh = 0；<br>当操作数为64位时，一次迭代有32个元素参与计算，数组长度为1，mask[0]的有效位数为32，maskLow∈[0, 2<sup>32</sup>-1]，maskHigh = 0。<br>例如，mask=[8, 2]，8和2分别表示低位的64bit和高位的64bit，8=0b1000，表示仅第4个元素参与计算；2=0b0010表示第66（64+2）个元素参与计算。<br>&bull;连续模式：输入为uint64\_t的值mask，表示mask寄存器从低位开始连续多少位被设置为1，表示前面连续的多少个元素参与计算。取值范围和操作数的数据类型有关，数据类型不同，每次迭代内能够处理的元素个数最大值不同。<br>当操作数为16位时，mask∈[0, 128]；<br>当操作数为32位时，mask∈[0, 64]；<br>当操作数为64位时，mask∈[0, 32]。<br>注：连续模式下，mask超出范围按照对应数据类型下mask最大值计算；mask = 0表示指令不会执行计算操作，该接口将被视为NOP（空操作）；若设成了负值可能产生未定义行为。|

> [!CAUTION]注意
>
> - 参数count的最大取值受限于输入数据总容量，该容量由系统Unified Buffer（UB）大小限制及操作数数据类型共同约束：不同数据类型下，相同UB容量内可容纳的最大元素数量不同。
>
> - 参数mask\[\]/mask仅在高维切分计算时有效，mask为0时，目的操作数保持原值。
