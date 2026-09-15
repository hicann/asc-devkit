# ICachePreLoad\(ISASI\)<a name="ZH-CN_TOPIC_0000001834752537"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T12:24:46.761Z -->

## Applicable Products<a name="section1550532418810"></a>

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
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description<a name="section618mcpsimp"></a>

The header file path is: `"basic_api/kernel_operator_cache_intf.h"`.

Developers can manually call **ICachePreLoad** to preload instructions from the GM address where the instructions reside into the ICache.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void ICachePreLoad(const int64_t preFetchLen)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

| Parameter | Input/Output | Description |
|--------|-----------|------|
| **preFetchLen** | Input | Prefetch length. |


<!-- npu="950,A3,910b" id8 -->
For the following models:
<!-- npu="950" id9 -->
- Ascend 950PR/Ascend 950DT
<!-- end id9 -->
<!-- npu="A3" id10 -->
- Atlas A3 training products/Atlas A3 inference products
<!-- end id10 -->
<!-- npu="910b" id11 -->
- Atlas A2 training products/Atlas A2 inference products
<!-- end id11 -->
The unit of the **preFetchLen** parameter is 2K bytes, and its value must meet the following requirement:

$$
\text{preFetchLen}< \dfrac{\text{the size of ICache}}{2K}
$$

The ICache sizes of AIC and AIV are 32KB and 16KB, respectively.
<!-- end id8 -->
<!-- npu="310p" id110 -->
For the AI Core of Atlas inference products, passing this parameter has no effect, and the prefetch length is always 128 bytes.
<!-- end id110 --> 

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section837496171220"></a>

```cpp
int64_t preFetchLen = 2; // Prefetch instruction length.
AscendC::ICachePreLoad(preFetchLen);
```
