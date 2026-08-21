# IterateAll

## 产品支持情况

<!-- npu="950" id3 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas 训练系列产品：不支持
<!-- end id8 -->
<!-- npu="x90" id1 -->
- Kirin X90：支持
<!-- end id1 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/IterateAll_res.md#id1 -->

## 功能说明

调用一次IterateAll，会计算出singleCoreM \* singleCoreN大小的C矩阵。迭代顺序可通过tiling参数iterateOrder调整。

## 函数原型

```
template <bool sync = true> __aicore__ inline void IterateAll(const GlobalTensor<DstT>& gm, uint8_t enAtomic = 0, bool enSequentialWrite = false, bool waitIterateAll = false, bool fakeMsg = false)
```

```
template <bool sync = true> __aicore__ inline void IterateAll(const LocalTensor<DstT>& ubCmatrix, uint8_t enAtomic = 0)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| sync | 获取C矩阵过程分为同步和异步两种模式：<br>同步：需要同步等待IterateAll执行结束。<br>异步：不需要同步等待IterateAll执行结束。<br><br>通过该参数设置同步或者异步模式：同步模式设置为true；异步模式设置为false，默认为同步模式。<!-- npu="310b" id14 --><br><br>Atlas 200I/500 A2 推理产品只支持设置为true。<!-- end id14 --> |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| gm | 输出 | C矩阵。类型为[GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md)。不同型号支持的数据类型请参考[支持的数据类型](#li12616155731720)。 |
| ubCmatrix | 输出 | C矩阵。类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为TSCM、VECIN，输出到VECIN Tensor时建议使用[Iterate](Iterate.md)接口。不同型号支持的数据类型请参考[支持的数据类型](#li12616155731720)。 |
| enAtomic | 输入 | 是否开启Atomic操作，默认值为0。<br><br>参数取值：<br><br>0：不开启Atomic操作<br><br>1：开启AtomicAdd累加操作<br><br>2：开启AtomicMax求最大值操作<br><br>3：开启AtomicMin求最小值操作<!-- npu="310p" id15 --><br><br>对于Atlas 推理系列产品AI Core，只有输出位置是GM才支持开启Atomic操作。<!-- end id15 --><!-- npu="310b" id16 --><br><br>对于Atlas 200I/500 A2 推理产品，只有输出位置是GM才支持开启Atomic操作。<!-- end id16 --> |
| enSequentialWrite | 输入 | 是否开启连续写模式（[连续写](GetTensorC.md#fig580415103338)，写入[baseM, baseN]；[非连续写](GetTensorC.md#fig322951853119)，写入[singleCoreM, singleCoreN]中对应的位置），默认值false（非连续写模式）。<!-- npu="310b" id17 --><br><br>Atlas 200I/500 A2 推理产品<!-- end id17 -->不支持该参数。 |
| waitIterateAll | 输入 | 仅在异步场景下使用，是否需要通过[WaitIterateAll](WaitIterateAll.md)接口等待IterateAll执行结束。<br><br>true：需要通过WaitIterateAll接口等待IterateAll执行结束。<br><br>false：不需要通过WaitIterateAll接口等待IterateAll执行结束，开发者自行处理等待IterateAll执行结束的过程。 |
| fakeMsg | 输入 | 仅在IBShare场景（模板参数中开启了[doIBShareNorm](MatmulConfig.md#matmulconfig-params)开关）和IntraBlockPartSum场景（模板参数中开启了[intraBlockPartSum](MatmulConfig.md#matmulconfig-params)开关）使用。<br>IBShare场景：<br>该场景复用L1上相同的A矩阵或B矩阵数据，要求AIV分核调用IterateAll的次数必须匹配，此时需要调用IterateAll并设置fakeMsg为true，不执行真正的计算，仅用来保证IterateAll调用成对出现。默认值为false，表示执行真正的计算。<br><br>IntraBlockPartSum场景：<br>用于分离模式下的Vector计算、Cube计算融合，实现多个AIV核的一次Matmul计算结果（baseM * baseN大小的矩阵分片）在L0C Buffer上累加。默认值为false，表示执行各AIV核的Matmul计算结果在L0C Buffer上累加。 |

## 返回值说明

无

## 约束说明

-   传入的C矩阵地址空间大小需要保证不小于singleCoreM \* singleCoreN个元素。
<!-- npu="950,A3,910b,310p,310b,x90" id18 -->
-   支持的数据类型<a id="li12616155731720"></a>

    <!-- npu="950" id9 -->
    Ascend 950PR/Ascend 950DT，支持的数据类型为：half、float、bfloat16_t、int32_t、int8_t、fp8_e4m3fn_t、hifloat8_t。
    <!-- end id9 -->

    <!-- npu="A3" id10 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：half、float、bfloat16_t、int32_t、int8_t。
    <!-- end id10 -->

    <!-- npu="910b" id11 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：half、float、bfloat16_t、int32_t、int8_t。
    <!-- end id11 -->

    <!-- npu="310p" id12 -->
    Atlas 推理系列产品AI Core，不支持包含ubCmatrix参数的原型，对于包含gm参数的原型，支持的数据类型为：half、float、int8_t、int32_t。
    <!-- end id12 -->

    <!-- npu="310b" id13 -->
    Atlas 200I/500 A2 推理产品，支持的数据类型为：half、float、bfloat16_t、int32_t。
    <!-- end id13 -->

    <!-- npu="x90" id2 -->
    Kirin X90，支持的数据类型为：half、int8_t、int32_t。
    <!-- end id2 -->

<!-- end id18 -->

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/IterateAll_res.md#id2 -->

## 调用示例

IterateAll接口的调用示例如下。

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.IterateAll(gm_c);    // 计算
mm.End();
```
