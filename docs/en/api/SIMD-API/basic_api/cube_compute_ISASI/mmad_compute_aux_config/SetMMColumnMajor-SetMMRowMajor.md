# SetMMColumnMajor/SetMMRowMajor

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T17:01:39.338Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
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

Controls which direction, M or N, **Mmad**/**MmadWithSparse** traverses first.

## Prototype

```cpp
__aicore__ inline void SetMMColumnMajor() // Indicates that CUBE generates results first along the M direction and then along the N direction.
__aicore__ inline void SetMMRowMajor()   // Indicates that CUBE generates results first along the N direction and then along the M direction.
```

## Parameters

None

## Return Value

None

## Constraints

None

## Example

```cpp
AscendC::SetMMRowMajor();
```
