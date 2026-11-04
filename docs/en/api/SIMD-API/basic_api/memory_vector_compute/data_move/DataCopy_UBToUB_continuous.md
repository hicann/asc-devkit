# DataCopy (Contiguous Data Movement Within UB)<a name="ZH-CN_TOPIC_0000002382908021"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:43:55.886Z pushedAt=2026-10-28T03:14:39.495Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id16 -->
- Ascend 950PR&950DT products: supported
<!-- end id16 -->
<!-- npu="A3" id17 -->
- Atlas A3 products: supported
<!-- end id17 -->
<!-- npu="910b" id18 -->
- Atlas A2 products: supported
<!-- end id18 -->
<!-- npu="310b" id19 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id19 -->
<!-- npu="310p" id20 -->
- Atlas inference products AI Core: supported
<!-- end id20 -->
<!-- npu="310p" id21 -->
- Atlas inference products Vector Core: not supported
<!-- end id21 -->
<!-- npu="910" id22 -->
- Atlas training products: supported
<!-- end id22 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopy_UBToUB_continuous_res.md#id1 -->

## Description<a name="section474617392321"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

Supports contiguous data movement within the Unified Buffer (UB). The data remains in its original format and content during transmission.

The specific supported data paths are (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- UB -> UB
    - VECIN -> VECCALC
    - VECCALC -> VECOUT

## Prototype<a name="section1954364615315"></a>

```cpp
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :--- | :--- |
| T | Data type of the operand. For supported data types, see [Data Types](#section4219135304818). |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand, of the [LocalTensor](../../data_structures/LocalTensor/LocalTensor_intro.md) type. It is stored in the UB, and the destination address must be 32-byte aligned. |
| src | Input | Source operand, of the LocalTensor type. It is stored in the UB, and the source address must be 32-byte aligned. |
| count | Input | Number of elements to be copied.<br>**Note: count * sizeof(T) must be 32-byte aligned. If it is not aligned, the copy amount is rounded down to 32-byte alignment.** |

Taking the half data type as an example, the shape of the source operand is 1 \* 128. When count = 128, [Figure 1](#zh-cn_topic_0000002567699425_fig13481537531) continuously copies 128 elements from the source operand to the destination operand.

**Figure 1**  Contiguous data movement diagram<a name="zh-cn_topic_0000002567699425_fig13481537531"></a>  
![](../../../../figures/continuous_data_copy_diagram.png "Contiguous data movement diagram")

## Data Type<a name="section4219135304818"></a>

The source operand and destination operand support the same data types, as follows:

<!-- npu="950" id1 -->
- For Ascend 950PR&950DT products, the supported data types are: bool, int8_t, uint8_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64.
<!-- end id1 -->

<!-- npu="A3" id2 -->
- For the Atlas A3 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id2 -->

<!-- npu="910b" id3 -->
- For the Atlas A2 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id3 -->

<!-- npu="310p" id4 -->
- For Atlas inference products AI Core, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id4 -->

<!-- npu="910" id5 -->
- For Atlas training products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id5 -->

## Return Value<a name="section44801012174220"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The address in the UB must be 32-byte aligned.
- When calling the continuous copy API, **count** \* sizeof\(T\) must be 32-byte aligned. If it is not aligned, the copy amount is rounded down to 32-byte alignment.
<!-- npu="910b,A3" id6 -->
- For the following product models:
    - Atlas A2 products
    - Atlas A3 products

    In the cross-card communication operator development scenario, the DataCopy class APIs support cross-card data copy only over the HCCS physical link, and do not support other paths. During development, pay attention to the physical path involved in inter-card communication. You can run the `npu-smi info -t topo` command to query the HCCS physical link.
<!-- end id6 -->

## Examples<a name="section122101199486"></a>

```cpp
// srcLocal and dstLocal are LocalTensors of the half type.
// Use the copy API that takes the count parameter to perform continuous copy.
AscendC::DataCopy(dstLocal, srcLocal, 512);
```

Result example:

```text
Input data srcLocal: [1 2 3 ... 512]
Output data dstLocal: [1 2 3 ... 512]
```
