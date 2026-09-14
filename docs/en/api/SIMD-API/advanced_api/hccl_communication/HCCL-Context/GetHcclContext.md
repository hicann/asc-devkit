# GetHcclContext

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:10:04.106Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Supported
- Atlas training products: Not supported

## Description

Obtains the address of the context (message area) of the communication domain specified by **Index**.

## Function Prototype

```
template <uint32_t index>
__aicore__ inline __gm__ uint8_t* __gm__ GetHcclContext(void)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| index | Template parameter that specifies the ID of the communication domain to be set. Currently, only two communication domains are supported, so **index** can only be 0 or 1. |

## Return Value

Returns the context (message area) address of the specified communication domain.

## Constraints

Currently, a maximum of 2 communicators are supported.

## Examples

```
GM_ADDR contextGM = AscendC::GetHcclContext<0>();
```

