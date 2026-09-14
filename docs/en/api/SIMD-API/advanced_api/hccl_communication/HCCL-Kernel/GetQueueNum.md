# GetQueueNum

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:16:06.455Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the number of communication queues for a **BatchWrite** communication task.

## Prototype

```
__aicore__ inline uint16_t GetQueueNum()
```

## Parameters

None

## Return Value

Number of communication queues for the **BatchWrite** communication task.

## Constraints

None

## Examples

See [the example of BatchWrite](BatchWrite.md#multi-queue-p2p).

