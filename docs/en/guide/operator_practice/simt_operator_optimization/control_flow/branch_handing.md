# Warp Divergence Predicate Optimization

<!-- md-trans-meta sourceCommit=05552f9ea27e5304609ca35e5b5743fa2827b759 translatedAt=2026-11-09T10:41:09.793Z pushedAt=2026-11-19T06:32:19.823Z nl=2 -->

[Priority] Medium

[Description] In SIMT programming mode, a warp is the basic scheduling and execution unit, and the 32 threads within the same warp execute the same instruction. Flow control statements (**if**, **switch**, **do**, **for**, **while**) generate branch divergence. When the divergence targets of different threads within the same warp are inconsistent, the thread execution paths diverge, causing warp divergence. Each branch can only be executed serially. When a branch is executed, the threads that do not enter that branch are masked by the active thread mask and wait idly. The hardware pushes the reconvergence point address and the active thread mask of each direction onto the branch stack. After all branches are executed, the threads reconverge at the reconvergence point.
Take a simple **if** statement as an example:

  ```cpp
  if (cond) {
      r = a + b;    // if body
  } else {
      r = a - b;    // else body
  }
  ```

**Figure 1** Comparison of divergence and predicate optimization

![](../../../figures/branch_handing.png)

To avoid the overhead caused by branch switching, the compiler can rewrite simple branches through predicate optimization, converting the instructions in the branch body into predicated instructions. All 32 threads in the warp **issue the instructions**, the instruction stream remains a straight line, only the threads whose predicate is true write back the results, and the threads whose predicate is false discard the results. This eliminates the divergence and branch stack overhead and improves operator performance.

[Sample Introduction] The sample uses the [hlog10](../../../../api/SIMT-API/math_functions/half_type/half_math_functions/hlog10.md) API as an example. The **hlog10** API uses $\log_{10}(x) = \dfrac{\log(x)}{\log(10)}$ to compute the common logarithm of the input data, and corrects six special values with precision deviations, taking 8192 half elements as input. It compares the performance of different special value handling methods to demonstrate the source of the benefits of predicate optimization.

[Negative example] Handle special values with precision anomalies by returning early.

```cpp
__aicore__ inline half hlog10(half x)
{
    if (x == static_cast<half>(0.2362060546875f)) {
        return static_cast<half>(-0.62646484375f);
    } else if (x == static_cast<half>(0.2490234375f)) {
        return static_cast<half>(-0.60400390625f);
    } else if (x == static_cast<half>(3.0703125f)) {
        return static_cast<half>(0.487060546875f);
    } else if (x == static_cast<half>(126.0625f)) {
        return static_cast<half>(2.099609375f);
    } else if (x == static_cast<half>(11496.0f)) {
        return static_cast<half>(4.05859375f);
    } else if (x == static_cast<half>(2976.0f)) {
        return static_cast<half>(3.474609375f);
    }
    float x_fp32 = __half2float(x);
    float result_fp32 = logf(x_fp32) / logf(10.0f);
    return __float2half_rn(result_fp32);
}
```

The actual control flow of the early-return approach above is as follows:

```cpp
__aicore__ inline half hlog10(half x)
{
    if (x == static_cast<half>(0.2362060546875f)) {
        return static_cast<half>(-0.62646484375f);
    } else {
        if (x == static_cast<half>(0.2490234375f)) {
            return static_cast<half>(-0.60400390625f);
        } else {
            if (x == static_cast<half>(3.0703125f)) {
                return static_cast<half>(0.487060546875f);
            } else {
                if (x == static_cast<half>(126.0625f)) {
                    return static_cast<half>(2.099609375f);
                } else {
                    if (x == static_cast<half>(11496.0f)) {
                        return static_cast<half>(4.05859375f);
                    } else {
                        if (x == static_cast<half>(2976.0f)) {
                            return static_cast<half>(3.474609375f);
                        } else {
                            float x_fp32 = __half2float(x);
                            float result_fp32 = logf(x_fp32) / logf(10.0f);
                            return __float2half_rn(result_fp32);
                        }
                    }
                }
            }
        }
    }
}
```

The six levels of branch judgment in the API cause divergence, incurring additional jump and branch stack overhead.

| execute cycle | aiv_total_cycles |
| :---------------: | :--------------: |
|      224        |      2498.75      |

To prevent warp scheduling from affecting the results, execute cycle is collected with 1 block and 32 threads.

[Positive example] First execute the normal-value path completely to obtain the temporary variable temp, and then overwrite temp level by level to handle special values with precision exceptions.

```cpp
__aicore__ inline half hlog10(half x)
{
    float x_fp32 = __half2float(x);
    float result_fp32 = logf(x_fp32) / logf(10.0f); // log10(x) = log(x) / log(10)
    half temp = __float2half_rn(result_fp32);
    if (x == static_cast<half>(0.2362060546875f)) {
        temp = static_cast<half>(-0.62646484375f);
    } else if (x == static_cast<half>(0.2490234375f)) {
        temp = static_cast<half>(-0.60400390625f);
    } else if (x == static_cast<half>(3.0703125f)) {
        temp = static_cast<half>(0.487060546875f);
    } else if (x == static_cast<half>(126.0625f)) {
        temp = static_cast<half>(2.099609375f);
    } else if (x == static_cast<half>(11496.0f)) {
        temp = static_cast<half>(4.05859375f);
    } else if (x == static_cast<half>(2976.0f)) {
        temp = static_cast<half>(3.474609375f);
    }
    return temp;
}
```

In the implementation above, the normal-value path is first executed completely to obtain the temporary variable temp, and then temp is overwritten level by level to handle special values with precision exceptions. The performance data of the positive-example operator is as follows (with the same input as the negative example):

| execute cycle | aiv_total_cycles |
| :---------------: | :--------------: |
|      190       |      2475.50      |

- Predicate optimization is performed automatically by the compiler. When the compiler recognizes that the instructions in a branch are simple and contain no return instruction, it automatically eliminates the jump and branch stack overhead to improve performance. Therefore, for simple branch processing logic such as exception-value handling, predicate optimization is recommended.
- However, in some scenarios, retaining the branch return without triggering predicate optimization yields better performance. A classic scenario is:

    ```cpp
    if(A){ //High hit rate
        ...
    }else if(B){
        ...
    }
    ```

  The A condition branch has a high hit rate, and the subsequent B condition branch contains numerous instructions. In this scenario, if branch A does not return early, it means that the vast majority of threads hit branch A, yet due to compiler predicate optimization they still have to execute the useless instructions in the subsequent branch B. The performance degradation caused by the surge in instruction count will outweigh the performance gain brought by predicate optimization.

The following examples apply different handling methods to branches with different hit rates to demonstrate the applicable scope of predicate optimization.

[Sample introduction] The sample uses the [j0f](../../../../api/SIMT-API/math_functions/float_math_functions/j0f.md) API as an example. The **j0f** API computes the zero-order Bessel function of the first kind for the input data. The API performs computation in three segments based on the absolute value of the input data, and handles two special values separately. The example takes 8192 float elements as input and constructs two batches of input based on branch hit conditions:

| Input Data | Data Range |
| :---------------: | :--------------------------------------------------- |
| input1 | Random values in the real number domain, including special values |
| input2 | Random values in the range `[-1.0e13f,1.0e13f]` |

[Negative example] Use early return to handle all branches.

```cpp
__aicore__ inline float j0f(float x)
{
    if (isnan(x)) {
        return x;
    }
    float ax = fabsf(x);
    if (isinf(ax)) {
        return 0.0f;
    }
    if (ax <= 8.0f) {
        return __internal_j0f_less8(ax);
    }
    if (ax <= 1.0e13f) {
        return __internal_j0f_middle_range(ax);
    }
    return __internal_j0f_huge_range(ax);
}
```

The value range of **input2** is `[-1.0e13f,1.0e13f]`. Each thread executes only the branch body in the hit direction, while the non-hit branches still incur jump and branch stack opening overhead, with an execute cycle of 1139. The performance data is as follows:

| execute cycle | aiv_total_cycles |
| :---------------: | :--------------: |
|      1139        |      2837.75      |

[Positive example] Use early return to handle branches with a high hit rate, use temporary variables to handle the remaining branches, and finally return uniformly.

```cpp
__aicore__ inline float j0f(float x)
{
    float ax = fabsf(x);
    if (ax <= 8.0f) {
        return __internal_j0f_less8(ax);    // Long branch body: return immediately upon hit, executing only the hit direction
    }
    if (ax <= 1.0e13f) {
        return __internal_j0f_middle_range(ax);
    }
    float result = __internal_j0f_huge_range(ax);
    if (isnan(x)) {
        result = x;                         // Short branch body: predicated, return uniformly
    }
    if (isinf(ax)) {
        result = 0.0f;
    }
    return result;
}
```

This implementation uses early return to handle high-hit-rate branches and uses temporary variables to store the computation results of low-hit-rate branches, thereby guiding the compiler to perform predicate optimization on the low-hit-rate branches. The performance data is as follows:

| Input Data | execute cycle | aiv_total_cycles |
| :---------------: | :--------------: | :--------------: |
| input1 | 4014 | 20727.38 |
| input2 | 1090 | 2760.50 |

For the input1 scenario: the input data covers the real number domain and includes special values, and the segmented jump branches within a warp cause divergence, resulting in an **execute cycle** of 4014. For the input2 scenario: all threads return early and do not execute the subsequent branch computes for non-hit branches, resulting in an **execute cycle** of 1090, which is lower than the negative example baseline of 1139; the **aiv_total_cycles** is 2760.50, lower than the negative example's 2837.75, so the overall overhead is lower than the baseline.

[Summary] When warp divergence occurs in simple branches, you can use the temporary variable approach to guide the compiler to perform predicate optimization and eliminate the branch divergence overhead. Predicate optimization executes all instructions of all branches, while divergence executes only the necessary paths. When the hit rates of the branches in a branch chain differ, keep divergence for high-hit-rate branches and apply predicate optimization to low-hit-rate branches. This avoids executing unnecessary instructions while also eliminating the branch divergence overhead.
