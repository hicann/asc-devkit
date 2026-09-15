# MakeStride

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:57:52.089Z -->

> [!NOTE]Note
> This API is experimental and may be adjusted or improved in later versions, with no guarantee of backward compatibility. Developers are advised to keep track of updates in later versions during use.

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

Constructs a **Stride** object to describe the stride of each dimension in memory. It supports passing multiple stride values as well as nested **Stride** substructures to construct a hierarchical **Stride**.

## Prototype

```cpp
template <typename... Ts>
__aicore__ inline constexpr Stride<Ts...> MakeStride(const Ts&... t)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
|--------|-----------|------|
| Ts... | Input | Type of each element in the Stride, which can be an integer variable, an `Std::Int` integer constant, or a nested Stride substructure type. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
|--------|-----------|------|
| t | Input | Stride of each dimension or substructure, a variadic parameter. |

## Return Value

Returns a **Stride<Ts...>** object.

## Constraints

- Input parameters must not be null.
- Each parameter must be a non-negative integer.
- Supported data types include integer types such as **size_t** and **int**, or the **Std::Int** type.

## Example

```cpp
using namespace AscendC::Te;

// Construct a normal Stride.
auto stride = MakeStride(1, 100, 200);

// Construct a hierarchical Stride.
auto fractalStride = MakeStride(MakeStride(1, 16), MakeStride(32, 512));

auto stride0 = Std::get<0>(stride);                    // stride0 = 1
auto innerStride = Std::get<1>(Std::get<0>(fractalStride)); // innerStride = 16
```
