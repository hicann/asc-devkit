# Continuous Computation

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T14:58:03.493Z -->

**Continuous Computation API** supports computation on the first n elements of a Tensor. It performs computation on n consecutive elements of the source operand and writes the results consecutively to the destination operand, addressing the continuous computation problem of one-dimensional Tensors.

Taking the Add API as an example, the API call format is as follows:

```cpp
Add(dst, src1, src2, n);
```

The following figure uses vector addition as an example to illustrate the features of the Continuous Computation API:

- The addresses of the source operands src1 and src2 and the destination operand dst are consecutive.
- The API is concise, requiring only the number of elements n to be specified.

**Figure 1** **Continuous Computation API**  
![Continuous Computation API](../../../../figures/continuous_calc_api.png)
