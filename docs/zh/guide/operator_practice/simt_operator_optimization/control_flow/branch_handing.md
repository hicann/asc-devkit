# Warp分支发散predicate优化

【优先级】中

【描述】SIMT编程模式下，Warp是基本的调度和执行单位，同一Warp内的32个线程执行同一条指令。流控制语句（if、switch、do、for、while）会产生分支跳转。当同一Warp内不同线程的跳转目标不一致时线程执行路径产生分歧，引起分支发散（Warp Divergence）。各分支只能串行执行，执行某一分支时，未进入该分支的线程被活跃线程掩码屏蔽、空转等待；硬件将汇合点地址与各方向的活跃线程掩码压入分支栈，全部分支执行完毕后在汇合点重新汇合。
以一个简单的if语句为例：

  ```cpp
  if (cond) {
      r = a + b;    // if 体
  } else {
      r = a - b;    // else 体
  }
  ```

**图1** divergence与predicate优化对比图

![](../../../figures/branch_handing.png)

为避免分支切换产生的开销，编译器可通过predicate优化对简单分支进行改写，将分支体内的指令改写为带谓词的指令，Warp内32个线程**全部发射**，指令流保持一条直线，仅谓词为真的线程写回结果，谓词为假的线程丢弃结果，从而消除跳转和分支栈开销，提高算子性能。

【样例介绍】样例以[hlog10](../../../../api/SIMT-API/math_functions/half_type/half_math_functions/hlog10.md)接口为例。hlog10接口使用$\log_{10}(x) = \dfrac{\log(x)}{\log(10)}$计算输入数据的常用对数，并对6个精度有偏差的特殊值做修正，以8192个half元素为输入。对比不同特殊值处理方式的性能，展示predicate优化的收益来源。

【反例】通过提前return的方式处理精度异常的特殊值。

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

上述提前return写法的实际控制流为：

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

接口中有6层分支判断会产生divergence，带来额外的跳转与分支栈开销。

| execute cycle | aiv_total_cycles |
| :---------------: | :--------------: |
|      224        |      2498.75      |

为防止warp调度带来影响，execute cycle采集时配置为1个block、32个线程。

【正例】先完整执行正常值路径得到临时变量temp，然后逐级覆盖临时变量处理精度异常的特殊值。

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

上述实现中，先完整执行正常值路径得到临时变量temp，然后逐级覆盖临时变量处理精度异常的特殊值。正例算子的性能数据如下（输入与反例相同）：

| execute cycle | aiv_total_cycles |
| :---------------: | :--------------: |
|      190       |      2475.50      |

- predicate优化是编译器自动优化的，当编译器识别到分支内的指令较简单且没有return指令时，会自动消除跳转与分支栈开销来提高性能，因此异常值处理这类简单分支处理逻辑建议使用predicate优化。
- 但有些场景，保留分支return不触发predicate优化的性能会更好。经典的场景是：

    ```cpp
    if(A){ //命中率高
        ...
    }else if(B){
        ...
    }
    ```

  A条件分支命中率高且后续B条件分支指令繁多。在这种场景下，如果A分支不提前return，就意味着绝大多数线程是命中A分支的，却因为编译器predicate优化而需要执行后续B分支中的无用指令，指令数暴涨带来的性能劣化将会掩盖predicate带来的性能收益。

后续样例将对不同命中率的分支采用不同处理方式，展示predicate优化的适用范围。

【样例介绍】样例以[j0f](../../../../api/SIMT-API/math_functions/float_math_functions/j0f.md)接口为例。j0f接口计算输入数据第一类零阶贝塞尔函数。接口按输入数据绝对值分三段计算，并对两个特殊值进行单独处理。样例以8192个float元素为输入，按分支命中情况构造两批输入：

| 输入数据 | 数据范围 |
| :---------------: | :--------------------------------------------------- |
| input1 | 在实数域随机取值，并包含特殊值 |
| input2 | 在`[-1.0e13f,1.0e13f]`范围内随机取值 |

【反例】使用提前return的方式处理所有分支。

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

input2取值范围为`[-1.0e13f,1.0e13f]`，各线程只执行命中方向的分支体，未命中的分支仍产生跳转与分支栈开销，execute cycle为1139。性能数据如下：

| execute cycle | aiv_total_cycles |
| :---------------: | :--------------: |
|      1139        |      2837.75      |

【正例】使用提前return的方式处理命中率高的分支，剩余分支使用临时变量处理，最后统一return。

```cpp
__aicore__ inline float j0f(float x)
{
    float ax = fabsf(x);
    if (ax <= 8.0f) {
        return __internal_j0f_less8(ax);    // 分支体长：命中即返回，只执行命中方向
    }
    if (ax <= 1.0e13f) {
        return __internal_j0f_middle_range(ax);
    }
    float result = __internal_j0f_huge_range(ax);
    if (isnan(x)) {
        result = x;                         // 分支体短：谓词化，统一return
    }
    if (isinf(ax)) {
        result = 0.0f;
    }
    return result;
}
```

该实现使用提前return处理高命中率分支，用临时变量暂存低命中率分支的计算结果，从而引导编译器对低命中率分支进行predicate优化。性能数据如下：

| 输入数据 | execute cycle | aiv_total_cycles |
| :---------------: | :--------------: | :--------------: |
| input1 | 4014 | 20727.38 |
| input2 | 1090 | 2760.50 |

对于input1输入场景：输入数据覆盖实数域并混入特殊值，warp内分段跳转分支产生divergence，execute cycle为4014。对于input2输入场景：所有线程都提前return，不执行后续未命中的分支计算，execute cycle为1090，低于反例基线1139；aiv_total_cycles为2760.50，低于反例的2837.75，整体开销低于基线。

【总结】当warp divergence出现在简单分支时，可使用临时变量的写法引导编译器进行predicate优化消除分支跳转开销。predicate优化会执行所有分支的所有指令，divergence只执行必要的路径，当分支链路中各分支命中率不同时，命中率高的分支保留divergence，命中率低的分支进行predicate优化，既可以避免执行不必要的指令，又可以消除分支跳转的开销。
