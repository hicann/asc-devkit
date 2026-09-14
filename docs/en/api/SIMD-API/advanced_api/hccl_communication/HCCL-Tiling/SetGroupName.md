# SetGroupName

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:31:49.810Z -->

## Description

Sets the communicator where the communication task resides.

## Function Prototype

```
uint32_t SetGroupName(const std::string &groupName)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| groupName | Input | Communicator where the current communication task resides. String type, with a maximum supported length of 128 bytes. |

## Return Value

-   0 indicates success.
-   A non-zero value indicates failure.

## Constraints

None

## Examples

For an example of this API, see [Examples](SetOpType.md#examples).

