# make\_tuple

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-06T12:30:31.364Z pushedAt=2026-09-11T04:54:01.855Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

make\_tuple is a practical function template that conveniently creates tuple objects. It can automatically infer the element types to make the code more concise, and can also construct an element list.

## Prototype

```
template <typename ...Tps>
__aicore__ inline constexpr tuple<unwrap_decay_t<Tps>...> make_tuple(Tps&& ...args)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| Tps... | **Tps...** is the template parameter pack passed to the **tuple**, representing the parameter types passed to **make_tuple**. The number of parameters ranges from [0, 64].<br><br>Ascend 950PR/Ascend 950DT: Supported data types are bool, int4b_t, int8_t, uint8_t, fp8_e8m0_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, LocalTensor, and GlobalTensor.<br><br>Atlas A3 training products/Atlas A3 inference products: Supported data types are bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, LocalTensor, and GlobalTensor.<br><br>Atlas A2 training products/Atlas A2 inference products: Supported data types are bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, LocalTensor, and GlobalTensor. |
| args | **args...** is the function parameter pack, representing the actual arguments passed to **make_tuple**. The number of arguments ranges from [0, 64]. |

## Constraints

-   The tuple instantiation depth is 64, which means aggregation of basic data types with up to 64 elements is supported.
-   For elements in make\_tuple that require a specific data type, an explicit cast must be added to the data type of the element. Otherwise, the compiler infers the type on its own, which may differ from the expected type.
-   Variable-length data types such as arrays are not supported.
-   Implicit conversion constructors are not supported.

## Return Value

Tuple object that contains copies of the passed parameters.

## Example

```
AscendC::Std::tuple<uint32_t, float, bool> test = AscendC::Std::make_tuple(22, 3.3, true);
```

For more examples, see [Example](tuple.md#example).

