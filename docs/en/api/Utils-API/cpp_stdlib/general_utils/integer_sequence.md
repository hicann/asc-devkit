# integer\_sequence

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T12:41:54.429Z pushedAt=2026-09-11T04:55:59.175Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

index\_sequence is a class template provided by Ascend C for generating a compile-time integer sequence, suitable for template metaprogramming.

make\_index\_sequence is a template provided by Ascend C. It is typically used with make\_index\_sequence to create an object of the index\_sequence type, which generates an integer sequence from 0 to N-1.

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

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| ...Idx | Parameter pack representing the sequence.<br><br>size_t, which is long unsigned int on 64-bit systems and unsigned int on non-64-bit systems. |
| N | Size of the generated integer sequence.<br><br>size_t, which is long unsigned int on 64-bit systems and unsigned int on non-64-bit systems. |

## Constraints

-   The range of N is \[0, 64\].
-   index\_sequence as a sequence has a maximum length of 64.

## Return Value

None

## Example

Generate and print an integer sequence of length 5.

```
template<size_t... Is>
__aicore__  inline void PrintIndexSequence(AscendC::Std::index_sequence<Is...>) {
   ((AscendC::printf(" Is:%lu", Is)), ...);
}
__aicore__ inline void Process()
{
    PrintIndexSequence(AscendC::Std::make_index_sequence<5>{}); // Print result: 0, 1, 2, 3, 4
    PrintIndexSequence(AscendC::Std::index_sequence<0,1,2,10,8000>{}); // Print result: 0, 1, 2, 10, 8000
}
```

