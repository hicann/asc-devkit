# SetGroupName

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-02T05:54:18.700Z pushedAt=2026-11-18T04:58:02.270Z nl=2 -->

## Description

Sets the communicator where the communication task resides.

## Prototype

```
uint32_t SetGroupName(const std::string& groupName)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| groupName | Input | Communicator where the current communication task resides. String type, with a maximum supported length of 128 bytes. |

## Return Value

-   0 indicates success.
-   A non-zero value indicates failure.

## Constraints

None

## Examples

For an example of calling this API, see [Examples](SetOpType.md#examples).
