# Sort32<a name="ZH-CN_TOPIC_0000001487536634"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_proposal_intf.h"`

Sort32接口实现一次迭代内对32个数的降序排列操作。

如下图所示，src0和src1分别表示待排序的数据score和对应RP的索引index，排序完成后以（score, index）的结构存储在dst中。

src1固定为uint32\_t类型，无论src0是float还是half类型，dst中的（score, index）结构总是占据8B空间。其中：

- 当src0为float类型时，dst中的index存储在高4B，score存储在低4B；
- 当src0为half类型时，dst中index存储在高4B，score存储在低2B，中间的2B保留。

**图1**  Sort32排序示意图

![](../../../../figures/sort32.png)<a id="图1-Sort32排序示意图"></a>

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void Sort32(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<uint32_t>& src1, const int32_t repeatTime)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
|---|---|
| T | 操作数数据类型。|

**表2**  参数说明

| 参数名称 | 输入/输出 | 说明 |
|---|---|---|
| dst | 输出 | 目的操作数。<br> 类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT<br> LocalTensor的起始地址需要32字节对齐。 |
| src0 | 输入 | 源操作数。<br> 类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT<br> LocalTensor的起始地址需要32字节对齐。 |
| src1 | 输入 | 源操作数。<br> 类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT<br> LocalTensor的起始地址需要32字节对齐。<br> **注：src1的数据类型固定为uint32_t。** |
| repeatTime | 输入 | 重复迭代次数。每次迭代完成32个元素的排序，下次迭代src0跳过128字节空间，src1跳过128字节空间，dst跳过256字节空间。每次迭代输入src0要求128字节对齐，src1要求128字节对齐，输出dst要求256字节对齐。repeatTime∈[0, 255]。 |

## 数据类型

**表3**  数据类型组合情况

| src0数据类型 | src1数据类型 | dst数据类型 | 
|---|---|---|
| half | uint32_t | half |
| float | uint32_t | float |

## 约束说明<a name="section633mcpsimp"></a>

- 地址对齐约束参考[通用地址对齐约束](../../../通用说明和约束.md)。
<!-- npu="A3,910b,950" id12 -->
- 当参数repeatTime取值为0时，该接口的行为如下：
    <!-- npu="A3,910b" id10 -->
    - 针对如下型号，当参数repeatTime取值为0时，不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。
       <!-- npu="A3" id8 -->
       - Atlas A3 训练系列产品/Atlas A3 推理系列产品
       <!-- end id8 -->
       <!-- npu="910b" id9 -->
       - Atlas A2 训练系列产品/Atlas A2 推理系列产品
       <!-- end id9 -->
    <!-- end id10 -->
    <!-- npu="950" id11 -->
    - 针对Ascend 950PR/Ascend 950DT，该接口通过VF调用[Reg矢量计算](../../Reg矢量计算/Reg矢量计算.md)API实现兼容，当参数repeatTime取值为0时，不保证该接口被视为NOP（空操作）。
    <!-- end id11 -->
<!-- end id12 -->

- 如果存在score\[i\]==score\[j\]并且i\>j，将优先选取score\[j\]排在前面。
- 每次迭代内的数据会进行排序，不同迭代间的数据不会进行排序。
- 每次迭代输入src0要求128B\(float\)/64B\(half\)对齐，src1要求128B对齐，输出dst要求256B对齐。

## 调用示例<a name="section642mcpsimp"></a>

完整示例请参考[Sort32样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/mrg_sort)。

```cpp
AscendC::LocalTensor<float> srcLocal0 = inQueueSrc0.DeQue<float>();
AscendC::LocalTensor<uint32_t> srcLocal1 = inQueueSrc1.DeQue<uint32_t>();
AscendC::LocalTensor<float> dstLocal = outQueueDst.AllocTensor<float>();
// repeatTime = 4,对128个数分成4组进行排序，每次完成1组32个数的排序
AscendC::Sort32<float>(dstLocal, srcLocal0, srcLocal1, 4);
outQueueDst.EnQue<float>(dstLocal);
inQueueSrc0.FreeTensor(srcLocal0);
inQueueSrc1.FreeTensor(srcLocal1);
```
