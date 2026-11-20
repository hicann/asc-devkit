# integer\_sequence

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T10:09:12.299Z pushedAt=2026-11-13T08:46:49.200Z nl=2 -->

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
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/general_utils/integer_sequence_res.md#id1 -->

## Description

index\_sequence is a class template provided by Ascend C for generating a compile-time integer sequence, suitable for template metaprogramming.

make\_index\_sequence is a template provided by Ascend C. It is typically used with make\_index\_sequence to create an object of type index\_sequence, which generates an integer sequence from 0 to N-1.

## Prototype

```
template<size_t... Idx>
using index_sequence = IntegerSequence<size_t, Idx...>;
```

```
template<size_t N>
using make_index_sequence = MakeIntegerSequence<size_t, N>;
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| ...Idx | Parameter pack representing the sequence.<br><br>size_t, which is long unsigned int on 64-bit systems and unsigned int on non-64-bit systems. |
| N | Size of the generated integer sequence.<br><br>size_t, which is long unsigned int on 64-bit systems and unsigned int on non-64-bit systems. |

## Constraints

-   The range of N is \[0, 64\].
-   index\_sequence, as a sequence, has a maximum length of 64.

## Return Value

None

## Examples

Generate and print an integer sequence of length 5.

```
template<size_t... Is>
__aicore__  inline void PrintIndexSequence(AscendC::Std::index_sequence<Is...>) {
   ((AscendC::printf(" Is:%lu", Is)), ...);
}
__aicore__ inline void Process()
{
    PrintIndexSequence(AscendC::Std::make_index_sequence<5>{}); // Print the result: 0, 1, 2, 3, 4.
    PrintIndexSequence(AscendC::Std::index_sequence<0,1,2,10,8000>{}); // Print the result: 0, 1, 2, 10, 8000.
}
```
