# Mmad API Boundary Value Summary<a name="ZH-CN_TOPIC_0000002607849175"></a>

<!-- md-trans-meta sourceCommit=c14ae98d8405d60be337fd017a9005dd55037dc9 translatedAt=2026-09-06T08:53:12.834Z pushedAt=2026-09-11T01:04:43.621Z -->

This section summarizes the output results of the Mmad computation API under boundary value inputs. The following content applies to the following models.

<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products
<!-- end id3 -->

**Table 1**  Computation results of Mmad special value/boundary value inputs

<a name="table7822641411"></a>
| C Matrix Data Type | Mmad Substage | A Matrix Element Value | B Matrix Element Value | dst Element Value<br>(INF/NAN Mode) |
| --- | --- | --- | --- | --- |
| float | Multiplication stage | +inf | norm(> 0.0)/+inf | +inf |
| float | Multiplication stage | +inf | norm(< 0.0)/-inf | -inf |
| float | Multiplication stage | -inf | norm(> 0.0)/+inf | -inf |
| float | Multiplication stage | -inf | norm(< 0.0)/-inf | +inf |
| float | Multiplication stage | +inf/-inf | 0.0 | nan |
| float | Multiplication stage | norm(> 0.0) | +inf | +inf |
| float | Multiplication stage | norm(> 0.0) | -inf | -inf |
| float | Multiplication stage | norm(< 0.0) | +inf | -inf |
| float | Multiplication stage | norm(< 0.0) | -inf | +inf |
| float | Multiplication stage | 0.0 | +inf/-inf | nan |
| float | Multiplication stage | nan | Arbitrary input | nan |
| float | Multiplication stage | Arbitrary input | nan | nan |
| float | Multiplication stage | Arbitrary input | Arbitrary input | In the case of result overflow, positive overflow is inf and negative overflow is -inf. |
| float | Addition stage | +inf | -inf | nan |
| float | Addition stage | +inf | +inf/norm | +inf |
| float | Addition stage | -inf | +inf | nan |
| float | Addition stage | -inf | -inf/norm | -inf |
| float | Addition stage | nan | Arbitrary input | nan |
| float | Addition stage | Arbitrary input | nan | nan |

Note:

(1) In INF/NAN mode (non-saturation mode), in the multiplication stage: inf multiplied by a non-zero value outputs inf, inf\*0 outputs nan, and when the input contains nan, the output is nan. In the addition stage: when positive inf and negative inf are added, the output is nan.

(2) In saturation mode, +inf takes the maximum value corresponding to its data type, -inf takes the minimum value corresponding to its data type to participate in computation, and NaN participates in computation as 0.

