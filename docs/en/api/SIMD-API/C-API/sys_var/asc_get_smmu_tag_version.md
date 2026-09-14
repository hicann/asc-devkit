# asc_get_smmu_tag_version

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T13:13:57.897Z pushedAt=2026-09-08T03:45:05.423Z -->

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
- Atlas 200I/500 A2 inference products: Not supported
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

Obtains the System Memory Management Unit (SMMU) version information.

## Prototype

```cpp
__aicore__ inline int64_t asc_get_smmu_tag_version()
```

## Parameters

None

## Return Value

SMMU version information. The meaning of each bit is as follows:
| Bit Range    | Meaning |
| ----------- |:----|
| 63:28 | Reserved bits. |
| 27:0 | SMMU tag version. |
| 27:14 | Year. Format: YY-YYYY-YYYY. |
| 13:8 | Month. Format: MM-MMMM. |
| 7:2 | Day. Format: DD-DDDD. |
| 1:0 | Sub-version of the same date. |

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```cpp
int64_t smmu_tag_version = asc_get_smmu_tag_version();
printf("smmu tag version is %x", smmu_tag_version);// Print it in hexadecimal using %x.
```