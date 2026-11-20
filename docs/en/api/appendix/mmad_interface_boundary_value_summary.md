# Mmad API Boundary Value Summary<a name="ZH-CN_TOPIC_0000002607849175"></a>

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-17T03:18:14.133Z pushedAt=2026-11-17T09:52:41.751Z nl=2 -->

This section summarizes the output results of the Mmad compute API under boundary value inputs. For the data types, see [mmad](../SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/Mmad.md) under the basic API. The following content applies to the following models:

<!-- npu="950" id1 -->
- Ascend 950PR & 950DT products
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products
<!-- end id3 -->

**Table 1**  Description of the computation results of Mmad special values/boundary value inputs

<a name="table7822641411"></a>

| Data Type of Matrix C | Mmad Sub-stage | Element Value of Matrix A | Element Value of Matrix B | element values of dst<br> (INF/NAN mode) |
| --- | --- | --- | --- | --- |
| float | multiplication stage | +inf | norm(> 0.0)/+inf | +inf |
| float | multiplication stage | +inf | norm(< 0.0)/-inf | -inf |
| float | multiplication stage | -inf | norm(> 0.0)/+inf | -inf |
| float | multiplication stage | -inf | norm(< 0.0)/-inf | +inf |
| float | multiplication stage | +inf/-inf | 0.0 | nan |
| float | multiplication stage | norm(> 0.0) | +inf | +inf |
| float | multiplication stage | norm(> 0.0) | -inf | -inf |
| float | multiplication stage | norm(< 0.0) | +inf | -inf |
| float | multiplication stage | norm(< 0.0) | -inf | +inf |
| float | multiplication stage | 0.0 | +inf/-inf | nan |
| float | multiplication stage | nan | arbitrary input | nan |
| float | multiplication stage | arbitrary input | nan | nan |
| float | multiplication stage | arbitrary input | arbitrary input | When the result overflows, positive overflow yields inf and negative overflow yields -inf |
| float | Addition stage | +inf | -inf | nan |
| float | Addition stage | +inf | +inf/norm | +inf |
| float | Addition stage | -inf | +inf | nan |
| float | Addition stage | -inf | -inf/norm | -inf |
| float | Addition stage | nan | arbitrary input | nan |
| float | Addition stage | arbitrary input | nan | nan |

Note:

In INF/NAN mode (non-saturating mode), in the multiplication stage: inf\* a non-zero value outputs inf, inf\*0 outputs nan, and when the input contains nan, the output is nan. In the addition stage: when positive and negative inf are added, the output is nan.
