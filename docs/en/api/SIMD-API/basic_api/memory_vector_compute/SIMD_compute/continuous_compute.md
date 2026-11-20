# Continuous Computing

<!-- md-trans-meta sourceCommit=7f211052f814e6d340cc6bf2dfc44c8393b5424f translatedAt=2026-11-05T01:55:08.643Z pushedAt=2026-11-18T01:25:30.747Z nl=2 -->

**Continuous Computing API** supports computing on the first n data elements of a tensor. It computes on n consecutive data elements of the source operands and writes the results consecutively to the destination operand, addressing continuous computing for 1D tensors.

The following uses the Add API as an example to show the call format:

```cpp
Add(dst, src1, src2, n);
```

The following figure uses vector addition as an example to illustrate the features of the continuous computing API:

- The addresses of source operands src1 and src2 and destination operand dst are consecutive.
- The API is concise, requiring only the number of elements n to be specified.

**Figure 1** **Continuous Computing API**  
![continuous computing API](../../../../figures/continuous_calc_api.png)
