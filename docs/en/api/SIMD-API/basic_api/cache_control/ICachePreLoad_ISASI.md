# ICachePreLoad\(ISASI\)<a name="ZH-CN_TOPIC_0000001834752537"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T11:15:59.063Z pushedAt=2026-10-26T07:41:44.046Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cache_control/ICachePreLoad_ISASI_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_cache_intf.h"`.

Developers can manually call the **ICachePreLoad** API to preload instructions from the GM address where the instructions reside into the ICache.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void ICachePreLoad(const int64_t preFetchLen)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Parameters

| Parameter | Input/Output | Description |
|--------|-----------|------|
| **preFetchLen** | Input | Prefetch length.|


<!-- npu="950,A3,910b" id8 -->
For the following models:
<!-- npu="950" id9 -->
- Ascend 950PR&950DT products
<!-- end id9 -->
<!-- npu="A3" id10 -->
- Atlas A3 products
<!-- end id10 -->
<!-- npu="910b" id11 -->
- Atlas A2 products
<!-- end id11 -->
The unit of the **preFetchLen** parameter is 2K Byte, and its value must meet the following requirements:

$$
\text{preFetchLen}< \dfrac{\text{ICache size}}{2K}
$$

The ICache sizes of AIC and AIV are 32KB and 16KB, respectively.
<!-- end id8 -->
<!-- npu="310p" id110 -->
For the Atlas inference products AI Core, this parameter is invalid, and the prefetch length is always 128 bytes.
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
