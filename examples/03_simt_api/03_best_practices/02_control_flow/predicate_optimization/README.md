# Warp分支发散predicate优化样例

## 概述

本样例通过`hlog10`和`j0f`接口展现SIMT编程模式下[Warp分支发散](../../../../../docs/zh/guide/programming_guide/programming_model/ai_core_simt_programming/thread_architecture.md#warp执行机制)的调优方法，重点呈现了predicate（谓词）优化的性能收益与适用场景。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍

```text
├── predicate_optimization
│   ├── predicate_optimization.asc  // SIMT实现和调用样例
│   ├── figures                     // README中的图片资源
│   ├── CMakeLists.txt              // cmake编译文件
│   ├── README.md                   // 样例说明文档
│   ├── README_en.md                // 英文样例说明文档
```

## 基础知识说明

- **Warp与分支发散（Warp Divergence）**

  在SIMT编程模式下，Warp是基本的调度和执行单位，当代码中出现分支判断时，Warp内的线程会产生分支发散（Warp divergence），各分支代码串行执行。以一个简单的if语句为例：

  ```cpp
  if (cond) {
      r = a + b;   
  } else {
      r = a - b;   
  }
  ```

  上述if-else在分支发散下的控制流如下：

  ![Warp分支发散的控制流](./figures/branch.png "Warp分支发散的控制流")

  该语句的执行顺序如下：
  1. START_DVG将当前active mask压栈
  2. branch cond，跳转到else分支，将if分支的PC和active mask压栈
  3. 执行else分支
  4. 第一次执行END_DVG，出栈并跳转到if分支的PC然后设置active mask
  5. 执行if分支
  6. 第二次执行END_DVG，出栈并设置active mask
  7. warp脱离divergence状态

- **Predicate（谓词）优化**

  为避免分支切换产生的开销，编译器可通过predicate优化对简单分支进行改写，将分支体内的指令改写为带谓词的指令，Warp内32个线程**全部发射**，指令流保持一条直线，仅谓词为真的线程写回结果，谓词为假的线程丢弃结果。predicate优化后的控制流如下：

  ![predicate优化后的控制流](./figures/predicate.png "predicate优化后的控制流")

出现warp divergence时，各分支只能串行执行，执行某一分支时，未进入该分支的线程被活跃线程掩码屏蔽、空转等待；硬件将汇合点地址与各方向的活跃线程掩码压入分支栈，全部分支执行完毕后在汇合点重新汇合。predicate优化可以消除跳转和分支栈开销，提高算子性能。

## hlog10样例描述

- 样例功能

  以[hlog10](../../../../../docs/zh/api/SIMT-API/math_functions/half_type/half_math_functions/hlog10.md)接口为例。hlog10接口使用$\log_{10}(x) = \dfrac{\log(x)}{\log(10)}$计算输入数据的常用对数，并对6个精度有偏差的特殊值做修正，以8192个half元素为输入。对比不同特殊值处理方法的性能，展示predicate优化的收益来源。

- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">hlog10</td></tr>
  <tr><td rowspan="3" align="center">样例输入/输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input / output</td><td align="center">[8192]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">hlog10_early_return_kernel / hlog10_predicate_kernel</td></tr>
  </table>

## hlog10样例实现

| Case | 实现特点 | 使用的核函数 | 分支链处理方式 |
|--------|---------------------------------------------------|--------------------|---------------------|
| Case 0 | 使用提前return的方法处理特殊值 | hlog10_early_return_kernel | 基线版本divergence |
| Case 1 | 使用临时变量处理特殊值，最后统一return | hlog10_predicate_kernel | predicate优化 |

各Case默认启动8个block，每个block 1024个线程，每个线程处理一个元素。

### 性能指标说明

|             字段名             | 字段含义                                             |
|:---------------------------:|:-------------------------------------------------|
|      execute cycle          | kernel内[clock](../../../../../docs/zh/api/Utils-API/tuning_interface/clock.md)采样到的执行周期数，反映kernel指令流的长度与分支开销。          |
|      aiv_total_cycles       | 单个block在Vector Core上执行所消耗的CPU周期（Cycle）数。          |

为防止warp调度带来影响，execute cycle采集时配置为1个block、32个线程。

### Case 0: hlog10分支链divergence版本

**实现方式**：通过提前return的方式处理精度异常的特殊值。

**关键代码**：

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

**性能数据**：

| execute cycle | aiv_total_cycles |
|:-----------:|:--------------:|
| 224 | 2498.75 |

**性能数据分析**：

接口中有6层分支判断会产生divergence，带来额外的跳转与分支栈开销。

### Case 1: hlog10分支链predicate优化版本

**实现方式**：先完整执行正常值路径得到临时变量temp，然后逐级覆盖临时变量处理精度异常的特殊值。

**关键代码**：

```cpp
__aicore__ inline half hlog10(half x)
{
    float x_fp32 = __half2float(x);
    float result_fp32 = logf(x_fp32) / logf(10.0f);
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

**性能数据**：

| execute cycle | aiv_total_cycles |
|:-----------:|:--------------:|
| 190 | 2475.50 |

**优化效果分析**：

- 提前return在实际场景中很难触发：SIMT代码实际以warp为单位执行，必须warp内所有线程的数据都是特殊值才能提前return，从概率上可以忽略不计，绝大部分场景下，特殊值线程需要等其他线程完成计算后再一起往下执行。
- 通过临时变量存储中间值，逐级覆盖最后统一return这种写法，可以引导编译器进行predicate优化，消除跳转与分支栈开销，相比Case 0的224，execute cycle降至190，下降约15.2%；aiv_total_cycles从2498.75降至2475.50。

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

## j0f样例描述

- 样例功能

  以[j0f](../../../../../docs/zh/api/SIMT-API/math_functions/float_math_functions/j0f.md)接口为例。j0f接口计算输入数据第一类零阶贝塞尔函数。接口按输入数据绝对值分三段计算，并对两个特殊值进行单独处理。

- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">j0f</td></tr>
  <tr><td rowspan="3" align="center">样例输入/输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input / output</td><td align="center">[8192]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">j0f_early_return_kernel / j0f_unified_return_kernel / j0f_hybrid_return_kernel</td></tr>
  </table>

  case2与case3在input2上对比divergence基线与predicate优化的性能；case4按分支命中情况构造两批输入，通过对比不同命中率下的性能数据探索predicate优化的适用边界：

  | 输入数据 | 数据范围 |
  | --- | --- |
  | input1 | 在实数域随机取值，并包含特殊值 |
  | input2 | 在`[-1.0e13f,1.0e13f]`范围内随机取值 |

## j0f样例实现

| Case | 实现特点 | 使用的核函数 | 分支链处理方式 |
|--------|---------------------------------------------------|--------------------|---------------------|
| Case 2 | 使用提前return的方式处理所有分支 | j0f_early_return_kernel | 基线版本divergence |
| Case 3 | 使用临时变量存放中间值，然后逐级覆盖，最后统一return | j0f_unified_return_kernel | predicate优化，统一return |
| Case 4 | 使用提前return的方式处理命中率高的分支，剩余分支使用临时变量处理，最后统一return | j0f_hybrid_return_kernel | divergence与predicate优化结合 |

本组各Case的性能指标含义与采集方式同上，参见[性能指标说明](#性能指标说明)。

### Case 2: j0f分支链divergence版本

**实现方式**：通过提前return的方式处理每一个分支。

**关键代码**：

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

**性能数据**：

| execute cycle | aiv_total_cycles |
|:-----------:|:--------------:|
| 1139 | 2837.75 |

**性能数据分析**：

input2取值范围为`[-1.0e13f,1.0e13f]`，各线程只执行命中方向的分支体，未命中的分支仍产生跳转与分支栈开销，execute cycle为1139。

### Case 3: j0f分支链计算后统一return版本

**实现方式**：使用临时变量存储中间结果，各分支命中时覆盖临时变量值，最后统一return。

**关键代码**：

```cpp
__aicore__ inline float j0f(float x)
{
    float ax = fabsf(x);
    float result = __internal_j0f_huge_range(ax);   
    if (ax <= 1.0e13f) {
        result = __internal_j0f_middle_range(ax);
    }
    if (ax <= 8.0f) {
        result = __internal_j0f_less8(ax);
    }
    if (isnan(x)) {
        result = x;                         
    }
    if (isinf(ax)) {
        result = 0.0f;                     
    }
    return result;
}
```

**性能数据**：

| execute cycle | aiv_total_cycles |
|:-----------:|:--------------:|
| 2930 | 18512.38 |

**优化效果分析**：

- 相比Case 2的1139，execute cycle升至2930，aiv_total_cycles从2837.75升至18512.38。Case 3显著慢于基线，原因在于统一return后大量线程做了无用计算，造成性能损耗。
- 对比Case 2与Case 3可知，warp divergence出现时，predicate优化并非总能带来性能收益。Case 4在此基础上引入分支命中情况，探究predicate优化的适用边界。

### Case 4: j0f分支链高命中率分支提前return，低命中率分支predicate优化版本

**实现方式**：使用提前return处理高命中率分支，用临时变量暂存低命中率分支的计算结果，引导编译器对低命中率分支进行predicate优化。

**关键代码**：

```cpp
__aicore__ inline float j0f(float x)
{
    float ax = fabsf(x);
    if (ax <= 8.0f) {
        return __internal_j0f_less8(ax);    
    }
    if (ax <= 1.0e13f) {
        return __internal_j0f_middle_range(ax);
    }
    float result = __internal_j0f_huge_range(ax);
    if (isnan(x)) {
        result = x;
    }
    if (isinf(ax)) {
        result = 0.0f;
    }
    return result;
}
```

**性能数据**：

| 数据批 | execute cycle | aiv_total_cycles |
|:---:|:-----------:|:--------------:|
| input1 | 4014 | 20727.38 |
| input2 | 1090 | 2760.50 |

**优化效果分析**：

- 对于input1输入场景：输入数据覆盖实数域并混入特殊值，warp内分段跳转分支产生divergence，execute cycle为4014。
- 对于input2输入场景：所有线程都提前return，不执行后续未命中的分支计算，execute cycle为1090，低于Case 2基线1139；aiv_total_cycles为2760.50，低于Case 2的2837.75，整体开销低于基线。

## 性能对比总结

**hlog10样例case0/1性能对比**：

| Case | execute cycle | aiv_total_cycles |
|--------|:-----------:|:--------------:|
| Case 0 | 224 | 2498.75 |
| Case 1 | **190** | 2475.50 |

**j0f组case2/3性能对比**：

| Case | execute cycle | aiv_total_cycles |
|--------|:-----------:|:--------------:|
| Case 2 | **1139** | 2837.75 |
| Case 3 | 2930 | 18512.38 |

**j0f组case4不同输入数据性能对比**：

| 用例 | execute cycle | aiv_total_cycles |
|--------|:-----------:|:--------------:|
| Case 4 input1 | 4014 | 20727.38 |
| Case 4 input2 | **1090** | 2760.50 |

**j0f组case3/4性能对比**

| 用例 | execute cycle | aiv_total_cycles |
|--------|:-----------:|:--------------:|
| Case 3 input2 | 2930 | 18512.38 |
| Case 4 input2 | **1090** | 2760.50 |

**综合优化效果**：

- hlog10样例：从Case 0基线版本到Case 1优化版本，execute cycle从224降低到190，下降约15.2%；aiv_total_cycles从2498.75降至2475.50；
- j0f样例：输入数据构成相同时，从Case 2基线版本到Case 3，execute cycle从1139升至2930；Case 4在input2上execute cycle为1090，低于Case 2，且明显快于Case 3。
## 调优建议

1. **使用predicate优化分支链路**：当warp divergence出现在简单分支时，可使用临时变量的写法引导编译器进行predicate优化，消除分支跳转开销。

2. **divergence和predicate优化结合使用**：predicate优化会执行所有分支的所有指令，divergence只执行必要的路径，当分支链路中各分支命中率不同时，命中率高的分支保留divergence，命中率低的分支进行predicate优化，既可以避免执行不必要的指令，又可以消除分支跳转的开销。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  mkdir -p build && cd build;                         # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=1 ..; make -j;   # 编译工程（选择用例1）
  ./predicate_optimization                            # 执行样例
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=1 ..; make -j;   # NPU仿真模式
  ```

  > **注意：** 切换用例或编译模式前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

- 编译选项说明

  | 选项                        | 可选值        | 说明                                                |
  |---------------------------|------------|---------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构：本样例仅支持 dav-3510（Ascend 950PR/Ascend 950DT） |
  | `SCENARIO_NUM` | `1` ~ `6`（默认`1`） | 编译期选择运行的用例，见下表 |
  | `CYCLE_PROFILE` | `ON`、`OFF`（默认） | 开启后编译execute cycle采集 |

  | SCENARIO_NUM | 用例 | 说明 |
  | --- | --- | --- |
  | 1 | hlog10_early_return_kernel | 基线版本 |
  | 2 | hlog10_predicate_kernel | 优化版本 |
  | 3 | j0f_early_return_kernel | 基线版本 |
  | 4 | j0f_unified_return_kernel | 统一return版本 |
  | 5 | j0f_hybrid_return_kernel | 提前return与predicate优化结合版本（input2） |
  | 6 | j0f_hybrid_return_kernel | 提前return与predicate优化结合版本（input1） |

  每次编译只包含选定的用例：换用不同的SCENARIO_NUM重新编译并运行，即可逐个完成各用例、各组的完整对比。execute cycle需以`-DCYCLE_PROFILE=ON`单独编译采集，aiv_total_cycles以默认编译配合msOpProf采集3次，取中位数，并取8个block的平均值。

## 性能调试

### msOpProf工具介绍

msOpProf工具是单算子性能分析工具。包含msopprof和msopprof simulator两种使用方式。该工具协助用户定位算子内存、算子代码以及算子指令的异常，实现全方位的算子调优。当前支持基于不同运行模式（上板或仿真）和不同文件形式（可执行文件或算子二进制.o文件）进行性能数据的采集和自动解析。

使用 `msOpProf` 工具获取单个组件上的性能数据：

```bash
msopprof ./predicate_optimization   # 分析样例的性能（用例由编译时的SCENARIO_NUM决定）
```

命令完成后，会在默认目录下生成以"OPPROF_{timestamp}_XXX"命名的文件夹，性能数据文件夹结构示例如下：

```text
├──dump                       # 原始的性能数据，用户无需关注
├──ArithmeticUtilization.csv  # cube/vector指令cycle占比
├──L2Cache.csv                # L2 Cache命中率
├──Memory.csv                 # UB、L1和主存储器读写带宽速率
├──MemoryL0.csv               # L0A、L0B和L0C读写带宽速率
├──MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率
├──OpBasicInfo.csv            # 算子基础信息
├──PipeUtilization.csv        # 采集计算单元和搬运单元耗时和占比
├──ResourceConflictRatio.csv  # UB上的 bank group、bank conflict和资源冲突率在所有指令中的占比
└──visualize_data.bin         # MindStudio Insight呈现文件
```

查看具体的性能分析结果：

```
# 如查看算子基本信息
cat ./OPPROF_*/OpBasicInfo.csv
```
