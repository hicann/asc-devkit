# Contiguous Computation API<a name="ZH-CN_TOPIC_0000002491670826"></a>

<!-- md-trans-meta sourceCommit=e2a2709368f65375f9b4d904990c7884199bd22e translatedAt=2026-10-23T15:24:50.917Z pushedAt=2026-11-03T07:05:13.475Z nl=2 -->

The contiguous computation API supports computation on the first *n* data items of a tensor. It computes the consecutive n data items of the source operand and writes the results consecutively to the destination operand, addressing the contiguous computation of one-dimensional tensors.

```
Add(dst, src1, src2, n);
```

The following figure uses vector addition as an example to illustrate the features of the **contiguous computation API**.

**Figure 1** **Continuous computing API**<a name="zh-cn_topic_0000001762058545_fig6847134062319"></a>  
![](../../../../figures/cont_calc_api.png "Continuous Computing API")
