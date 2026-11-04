# get

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T01:14:55.992Z pushedAt=2026-10-23T02:42:59.076Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/container_functions/get_res.md#id1 -->

## Description

Extracts the element at a specified position from a tuple container.

## Prototype

```
template <size_t N, typename ...Tps>
__aicore__ inline typename tuple_element<N, tuple<Tps...> >::type& get(tuple<Tps...>& t) noexcept
```

```
template <size_t N, typename ...Tps>
__aicore__ inline const typename tuple_element<N, tuple<Tps...> >::type& get(const tuple<Tps...>& t) noexcept
```

```
template <size_t N, typename ...Tps>
__aicore__ inline typename tuple_element<N, tuple<Tps...> >::type&& get(tuple<Tps...>&& t) noexcept
```

```
template <size_t N, typename ...Tps>
__aicore__ inline const typename tuple_element<N, tuple<Tps...> >::type&& get(const tuple<Tps...>&& t) noexcept
```

## Parameters

**Table 1** Template parameters

| parameter name | Description |
| --- | --- |
| N | N is a compile-time constant that represents the index of the element to be extracted. The index starts from 0 and ranges from [0, 64). |
| Tps... | Tps... is the template parameter pack passed to the tuple. The number of tuple parameters is in the range (0, 64].<br><br>Ascend 950PR&950DT products, the supported data types are: bool, int4b_t, int8_t, uint8_t, fp8_e8m0_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, LocalTensor, GlobalTensor.<br><br>Atlas A3 products, the supported data types are: bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, LocalTensor, GlobalTensor.<br><br>Atlas A2 products, the supported data types are: bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, LocalTensor, GlobalTensor. |
| t | t is the tuple object, which can be an lvalue reference, a const lvalue reference, or an rvalue reference. |

## Constraints

The get function supports only const and constexpr constant indexes. The index ranges from \[0, 64\).

## Return Value

Returns the element at the corresponding position in the tuple object.

## Examples

```
AscendC::Std::tuple<uint32_t, float, bool> test{11, 2.2, true};
uint32_t const_uint32_t = AscendC::Std::get<0>(test);
```

For more examples, see [Examples](tuple.md#examples).
