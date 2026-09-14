# SetHcclContext

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:10:06.549Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Supported
- Atlas training products: Not supported

## Description

Sets the context (message area) address corresponding to each communication domain of the communication-computation fusion operator.

## Prototype

```
template <uint32_t index>
__aicore__ inline void SetHcclContext(__gm__ uint8_t* context)
```

## Parameters

**Table 1**  Parameters

| Parameter | Description |
| --- | --- |
| index | Template parameter, indicating the ID of the communication domain to be set. Currently only two communication domains are supported, so **index** can only be 0 or 1. |
| context | Address of the context (message area) of the corresponding communication domain. |

## Return Value

None

## Constraints

Currently, a maximum of 2 communication domains are supported.

## Examples

```
// Set the message area address for GROUP_0.
AscendC::SetHcclContext<0>(contextGM);
```

