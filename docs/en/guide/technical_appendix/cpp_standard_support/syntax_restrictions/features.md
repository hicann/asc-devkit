# Features<a name="ZH-CN_TOPIC_0000002510059252"></a>

<!-- md-trans-meta sourceCommit=2951a900b5d1975728ac7f0f437c6d752ed9447e translatedAt=2026-10-28T09:39:40.270Z pushedAt=2026-11-03T07:08:03.577Z nl=2 -->

-   Virtual functions are not supported.
-   Virtual inheritance is not supported.
-   Runtime recursion is not supported.
-   Dynamic malloc, new/free are not supported.
-   STL is not supported.
-   Runtime typeid is not supported.
-   File system I/O is not supported.
-   Tuple and algorithm operations in the standard library are not supported (related library function calls need to be marked with aicore).
-   Lambda expressions are supported, but in hybrid programming scenarios, the function attributes of the lambda expression (`__aicore__`, `__simd_callee__`, `__simt_callee__`, etc.) must be explicitly marked. An example is as follows:
    ```cpp
    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void simt_lambda_by_value(
        __ubuf__ float* in, __ubuf__ float* out, uint32_t count, float scale, float bias)
    {
        if (threadIdx.x >= count) {
            return;
        }
        // __simt_callee__ marks the attribute of this lambda function
        auto affine = [scale, bias](float v) __simt_callee__ {
            return v * scale + bias; };
        out[threadIdx.x] = affine(in[threadIdx.x]);
    }
    ```

