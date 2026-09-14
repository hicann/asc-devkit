# Cube Instruction Theoretical Performance Summary<a name="ZH-CN_TOPIC_0000002607728945"></a>

<!-- md-trans-meta sourceCommit=c14ae98d8405d60be337fd017a9005dd55037dc9 translatedAt=2026-09-06T08:39:26.035Z pushedAt=2026-09-07T03:55:46.113Z -->

This section summarizes the theoretical performance of the main Cube instructions. The following content applies to the following models:

<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products
<!-- end id3 -->

**Table 1** Values of parallelism and k0 in the Mmad theoretical performance calculation formula

<a name="table1877123815211"></a>
| API | Left Matrix A | Right Matrix B | cube<sub>m</sub> | cube<sub>n</sub> | cube<sub>k</sub> | k<sub>0</sub> |
| --- | --- | --- | --- | --- | --- | --- |
| Mmad | int8_t | int8_t | 16 | 16 | 32 | 32 |
| Mmad | half | half | 16 | 16 | 16 | 16 |
| Mmad | float | float | 16 | 16 | 4 | 8 |
| Mmad | bfloat16_t | bfloat16_t | 16 | 16 | 16 | 16 |
| Mmad | int4b_t | int4b_t | 16 | 16 | 64 | 64 |
| MmadWithSparse | int8_t | int8_t | 16 | 16 | 32 | 32 |

