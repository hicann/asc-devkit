# 特性<a name="ZH-CN_TOPIC_0000002510059252"></a>

-   不支持虚函数
-   不支持虚继承
-   不支持运行时递归
-   不支持动态malloc、new/free
-   不支持STL
-   不支持运行时typeid
-   不支持文件系统IO
-   不支持标准库下的tuple及算法类运算（相关库函数调用需要标记aicore）
-   支持lamda表达式，但在混合编程场景，需要显示标记lamda表达式的函数属性（`__aicore__`、`__simd_callee__`、`__simt_callee__`等）示例如下：
    ```cpp
    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void simt_lambda_by_value(
        __ubuf__ float* in, __ubuf__ float* out, uint32_t count, float scale, float bias)
    {
        if (threadIdx.x >= count) {
            return;
        }
        // __simt_callee__ 标识该lamda函数属性
        auto affine = [scale, bias](float v) __simt_callee__ {
            return v * scale + bias; };
        out[threadIdx.x] = affine(in[threadIdx.x]);
    }
    ```

