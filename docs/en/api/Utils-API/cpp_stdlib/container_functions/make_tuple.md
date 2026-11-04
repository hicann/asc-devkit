# make\_tuple

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T01:14:21.611Z pushedAt=2026-10-23T02:41:19.219Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/container_functions/make_tuple_res.md#id1 -->

## Description

make\_tuple is a practical function template that conveniently creates tuple objects. It can automatically deduce element types to make code more concise, and it can also construct element lists.

## Prototype

```
template <typename ...Tps>
__aicore__ inline constexpr tuple<unwrap_decay_t<Tps>...> make_tuple(Tps&& ...args)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| Tps... | Tps... is the template parameter pack passed to the tuple, representing the parameter types passed to make_tuple. The number of parameters ranges from 0 to 64.<br><br>For the Ascend 950PR&950DT products, the supported data types are: bool, int4b_t, int8_t, uint8_t, fp8_e8m0_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, LocalTensor, GlobalTensor.<br><br>For the Atlas A3 products, the supported data types are: bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, LocalTensor, GlobalTensor.<br><br>For the Atlas A2 products, the supported data types are: bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, LocalTensor, GlobalTensor. |
| args | args... is the function parameter pack, representing the actual arguments passed to make_tuple. The number of parameters ranges from 0 to 64. |

## Constraints

-   The tuple instantiation depth is 64, meaning that it supports aggregation of basic data types with up to 64 elements.
-   For elements in make\_tuple that require a specific data type, you must add an explicit cast to the element's data type. Otherwise, the compiler deduces the type on its own, which may not match the expectation.
-   Variable-length data types such as arrays are not supported.
-   Implicit conversion constructors are not supported.

## Return Value

Returns a tuple object that contains copies of the passed arguments.

## Examples

```
AscendC::Std::tuple<uint32_t, float, bool> test = AscendC::Std::make_tuple(22, 3.3, true);
```

For more call examples, see [Examples](tuple.md#examples).
