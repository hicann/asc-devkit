# Continuous Compute API<a name="ZH-CN_TOPIC_0000002491670826"></a>

<!-- md-trans-meta sourceCommit=49eda737d7d7268a9ddb02fcaba4732176bfc076 translatedAt=2026-11-09T11:02:51.605Z pushedAt=2026-11-19T06:32:19.951Z nl=2 -->

The continuous compute API supports compute on the first *n* data items of a tensor. It computes the consecutive n data items of the source operand and writes the results consecutively to the destination operand, addressing the contiguous computation of one-dimensional tensors.

```
Add(dst, src1, src2, n);
```

The following figure uses vector addition as an example to illustrate the features of the **continuous compute API**.

**Figure 1** **Continuous compute API**<a name="zh-cn_topic_0000001762058545_fig6847134062319"></a>  
<img src="../../../../figures/cont_calc_api.png" alt="Continuous compute API" width="600px" height="218px">
