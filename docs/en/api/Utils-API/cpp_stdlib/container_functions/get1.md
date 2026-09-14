# get

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-06T12:25:40.263Z pushedAt=2026-09-11T04:53:20.722Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

**get** extracts the element at a specified position from a tuple container.

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

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| N | N is a compile-time constant that represents the index of the element to be extracted. The index starts from 0 and ranges from [0, 64). |
| Tps... | Tps... is the template parameter pack passed to the tuple. The number of tuple parameters ranges from (0, 64].<br><br>Ascend 950PR/Ascend 950DT supports the following data types: bool, int4b_t, int8_t, uint8_t, fp8_e8m0_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, LocalTensor, GlobalTensor.<br><br>Atlas A3 training products/Atlas A3 inference products support the following data types: bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, LocalTensor, GlobalTensor.<br><br>Atlas A2 training products/Atlas A2 inference products support the following data types: bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, LocalTensor, GlobalTensor. |
| t | t is a tuple object, which can be an lvalue reference, a const lvalue reference, or an rvalue reference. |

## Constraints

The **get** function supports only **const** and **constexpr** constant indexes, with the index value range being \[0, 64\).

## Return Value

Element at the corresponding position in the tuple object.

## Example

```
AscendC::Std::tuple<uint32_t, float, bool> test{11, 2.2, true};
uint32_t const_uint32_t = AscendC::Std::get<0>(test);
```

For more examples, see [Example](tuple.md#example).

