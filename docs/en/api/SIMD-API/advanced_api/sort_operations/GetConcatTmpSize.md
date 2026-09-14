# GetConcatTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T17:06:31.651Z -->

## Description

Obtains the size of the temporary space required by the **Concat** API, in bytes.

## Prototype

```
uint32_t GetConcatTmpSize(const platform_ascendc::PlatformAscendC &ascendcPlatform, const uint32_t elemCount, const uint32_t dataTypeSize)
```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ascendcPlatform | Input | Passes the hardware platform information. For the definition of **PlatformAscendC**, see [Constructors and Destructors](../../../Utils-API/platform_info/PlatformAscendC/constructors_and_destructors.md). |
| elemCount | Input | Number of input elements. |
| dataTypeSize | Input | Size of the input data (in bytes). |

## Return Value

Returns the size of the temporary space required by the **Concat** API, in bytes.

## Constraints

None

## Examples

```
fe::PlatFormInfos platform_info;
auto plat = platform_ascendc::PlatformAscendC(&platform_info);
const uint32_t elemCount = 128;
AscendC::GetConcatTmpSize(plat, elemCount, 2);
```
