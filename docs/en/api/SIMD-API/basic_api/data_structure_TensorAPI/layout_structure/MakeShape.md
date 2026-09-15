# MakeShape

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:56:50.715Z -->

> [!NOTE]Note
> This API is an experimental API. It may be adjusted or improved in later versions, and backward compatibility is not guaranteed. Developers should pay attention to later version updates during use.

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

Constructs a **Shape** object to describe the logical shape of data. It supports passing multiple dimension values, and also supports passing nested **Shape** substructures to construct a hierarchical **Shape**.

## Function Prototype

```cpp
template <typename... Ts>
__aicore__ inline constexpr Shape<Ts...> MakeShape(const Ts&... t)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
|--------|-----------|------|
| Ts... | Input | Type of each element of **Shape**, which can be an integer variable, an integer constant of `Std::Int`, or a nested **Shape** substructure type. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
|--------|-----------|------|
| t | Input | Value or substructure of each dimension of **Shape**, a variadic parameter. |

## Return Value

Returns a **Shape<Ts...>** object.

## Constraints

- The input parameter cannot be empty.
- The input parameter must be a positive integer.
- Supported data types include integer types such as **size_t** and **int**, or the **Std::Int** type.

## Example

```cpp
using namespace AscendC::Te;

// Construct a regular Shape.
auto shape = MakeShape(10, 20, 30);

// Construct a hierarchical Shape.
auto fractalShape = MakeShape(MakeShape(16, 8), MakeShape(32, 4));

auto dim0 = Std::get<0>(shape);                // dim0 = 10
auto innerRow = Std::get<0>(Std::get<0>(fractalShape)); // innerRow = 16
```
