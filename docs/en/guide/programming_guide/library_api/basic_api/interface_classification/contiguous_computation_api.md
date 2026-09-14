# Contiguous Computation API<a name="ZH-CN_TOPIC_0000002491670826"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T12:23:39.030Z pushedAt=2026-09-06T02:47:06.120Z -->

The contiguous computation API supports computation on the first *n* data items of a tensor. It computes the consecutive n data items of the source operand and writes the results consecutively to the destination operand, addressing the contiguous computation of one-dimensional tensors.

```
Add(dst, src1, src2, n);
```

The following figure uses vector addition as an example to illustrate the features of the **contiguous computation API**.

**Figure 1** **Contiguous computation API**<a name="zh-cn_topic_0000001762058545_fig6847134062319"></a>  
![](../../../../figures/contiguous_computation_api.png "Contiguous computation API")

