# RpSort16<a name="ZH-CN_TOPIC_0000001443137898"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：不支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：支持
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/sort_and_merge_ISASI/RpSort16_res.md#id1 -->


## 功能说明<a name="section618mcpsimp"></a>

根据Region Proposals中的score域对其进行排序（score大的排前面），每次排16个Region Proposals。

## 函数原型<a name="section620mcpsimp"></a>

```
template <typename T>
__aicore__ inline void RpSort16(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

<a name="table4835205712588"></a>
| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。 |

**表2**  参数说明

<a name="table16397194910519"></a>
| 参数名称 | 输入/输出 | 含义 |
| --- | --- | --- |
| dst | 输出 | 目的操作数，存储经过排序后的Region Proposals。<br>类型为[LocalTensor](../../data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。 |
| src | 输入 | 源操作数，存储未经过排序的Region Proposals。<br>类型为[LocalTensor](../../data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。 |
| repeatTime | 输入 | 重复迭代次数，int32_t类型，每次排16个Region Proposals。取值范围：repeatTime∈[0,255]。 |

## 数据类型

<!-- npu="910" id8 -->
Atlas训练系列产品，支持的数据类型为：half。
<!-- end id8 -->

<!-- npu="310p" id9 -->
Atlas推理系列产品AI Core，支持的数据类型为：half/float。
<!-- end id9 -->

## 约束说明<a name="section633mcpsimp"></a>

-   用户需保证src和dst中存储的Region Proposal数目大于实际所需数据，否则会存在tensor越界错误。
-   当存在proposal\[i\]与proposal\[j\]的score值相同时，如果i\>j，则proposal\[j\]将首先被选出来，排在前面。
-   操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/sort_and_merge_ISASI/RpSort16_res.md#id2 -->

## 调用示例<a name="section642mcpsimp"></a>

详细示例请参考[RegionProposalSort样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/region_proposal_sort)。

-   接口使用样例

    ```
    // ProposalConcat将连续元素合入Region Proposal内对应位置
    // repeatTime = 2,对2个Region Proposal进行排序，model=4起始位置为4
    AscendC::ProposalConcat(dstLocal, srcLocal, 2, 4);
    AscendC::RpSort16(dstLocal, dstLocal, 2);
    ```

    ```
    示例结果
    输入数据srcLocal:
    [ -1.624 -42.3   -54.12   91.25  -99.4    36.72   67.44  -66.3   -52.53
       3.377 -62.47  -15.85  -31.47    3.143  58.47  -83.75 21.58   63.47    
       7.234  35.16  -39.72   37.8    73.06  -98.7    44.1 -77.2    67.2    
       19.62  -87.9   -14.875  15.86  -77.75]
    经过ProposalConcat后的dstLocal数据，repeat=2计算32个元素，model=4起始位置为4
    [  
     0.        0.      0.      0.
    -1.624     0.      0.      0.      0.      0.      0.      0.
    -42.3      0.      0.      0.      0.      0.      0.      0.
    -54.12     0.      0.      0.      0.      0.      0.      0.
    91.25      0.      0.      0.      0.      0.      0.      0.
    -99.4      0.      0.      0.      0.      0.      0.      0.
    36.72      0.      0.      0.      0.      0.      0.      0.
    67.44      0.      0.      0.      0.      0.      0.      0.
    -66.3      0.      0.      0.      0.      0.      0.      0.
    -52.53     0.      0.      0.      0.      0.      0.      0.
    3.377      0.      0.      0.      0.      0.      0.      0.
    -62.47     0.      0.      0.      0.      0.      0.      0.
    -15.85     0.      0.      0.      0.      0.      0.      0.
    -31.47     0.      0.      0.      0.      0.      0.      0.
    3.143      0.      0.      0.      0.      0.      0.      0.
    58.47      0.      0.      0.      0.      0.      0.      0.
    -83.75     0.      0.      0.      0.      0.      0.      0.
    21.58      0.      0.      0.      0.      0.      0.      0.
    63.47      0.      0.      0.      0.      0.      0.      0.
    7.234      0.      0.      0.      0.      0.      0.      0.
    35.16      0.      0.      0.      0.      0.      0.      0.
    -39.72     0.      0.      0.      0.      0.      0.      0.
    37.8       0.      0.      0.      0.      0.      0.      0.
    73.06      0.      0.      0.      0.      0.      0.      0.
    -98.7      0.      0.      0.      0.      0.      0.      0.
    44.1       0.      0.      0.      0.      0.      0.      0.
    -77.2      0.      0.      0.      0.      0.      0.      0.
    67.2       0.      0.      0.      0.      0.      0.      0.
    19.62      0.      0.      0.      0.      0.      0.      0.
    -87.9      0.      0.      0.      0.      0.      0.      0.
    -14.875    0.      0.      0.      0.      0.      0.      0.
    15.86      0.      0.      0.      0.      0.      0.      0.
    -77.75     0.      0.      0.     
    ]
    输出数据(dst_gm):
     [
     0.      0.      0.      0.
     91.25   0.      0.      0.      0.      0.      0.      0.
     67.44   0.      0.      0.      0.      0.      0.      0.
     58.47   0.      0.      0.      0.      0.      0.      0.
     36.72   0.      0.      0.      0.      0.      0.      0.
     3.377   0.      0.      0.      0.      0.      0.      0.
     3.143   0.      0.      0.      0.      0.      0.      0.
     -1.624  0.      0.      0.      0.      0.      0.      0.
     -15.85  0.      0.      0.      0.      0.      0.      0.
     -31.47  0.      0.      0.      0.      0.      0.      0.
     -42.3   0.      0.      0.      0.      0.      0.      0.
     -52.53  0.      0.      0.      0.      0.      0.      0.
     -54.12  0.      0.      0.      0.      0.      0.      0.
     -62.47  0.      0.      0.      0.      0.      0.      0.
     -66.3   0.      0.      0.      0.      0.      0.      0
     -83.75  0.      0.      0.      0.      0.      0.      0.
     -99.4   0.      0.      0.      0.      0.      0.      0.
     73.06   0.      0.      0.      0.      0.      0.      0.
     67.2    0.      0.      0.      0.      0.      0.      0.
     63.47   0.      0.      0.      0.      0.      0.      0.
     44.1    0.      0.      0.      0.      0.      0.      0.
     37.8    0.      0.      0.      0.      0.      0.      0.
     35.16   0.      0.      0.      0.      0.      0.      0.
     21.58   0.      0.      0.      0.      0.      0.      0.
     19.62   0.      0.      0.      0.      0.      0.      0.
     15.86   0.      0.      0.      0.      0.      0.      0.
     7.234   0.      0.      0.      0.      0.      0.      0.
     -14.875 0.      0.      0.      0.      0.      0.      0.
     -39.72  0.      0.      0.      0.      0.      0.      0.
     -77.2   0.      0.      0.      0.      0.      0.      0.
     -77.75  0.      0.      0.      0.      0.      0.      0.
     -87.9   0.      0.      0.      0.      0.      0.      0.
     -98.7   0.      0.      0.
     ]
    ```
