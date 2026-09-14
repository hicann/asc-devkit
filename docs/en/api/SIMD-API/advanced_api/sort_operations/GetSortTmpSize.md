# GetSortTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T17:11:19.679Z -->

## Description

Obtains the size of the temporary space required by the Sort API.

## Prototype

```
uint32_t GetSortTmpSize(const platform_ascendc::PlatformAscendC &ascendcPlatform, const uint32_t elemCount, const uint32_t dataTypeSize)
```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ascendcPlatform | Input | Passes the hardware platform information. For the definition of PlatformAscendC, see [Constructors and Destructors](../../../Utils-API/platform_info/PlatformAscendC/constructors_and_destructors.md). |
| elemCount | Input | Number of input elements. |
| dataTypeSize | Input | Size of the input data (in bytes). |

## Return Value

Size of the temporary space required by the Sort API.

## Constraints

None

## Examples

```
fe::PlatFormInfos platform_info;
auto plat = platform_ascendc::PlatformAscendC(&platform_info);
const uint32_t elemCount = 128;
AscendC::GetSortTmpSize(plat, elemCount, 4);
```
