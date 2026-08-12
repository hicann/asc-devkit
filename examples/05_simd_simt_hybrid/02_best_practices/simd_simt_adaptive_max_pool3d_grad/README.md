# SIMD与SIMT混合编程实现adaptive_max_pool3d_grad算子样例

## 概述

本样例以adaptive_max_pool3d_grad算子为例，对比SIMD、混合编程两种实现方式的复杂度和性能数据，说明在连续和离散读写并存的场景中，混合编程可以兼顾性能和开发体验。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本

- \>= CANN 9.2.0

## 目录结构介绍

```text
├── simd_simt_adaptive_max_pool3d_grad
│   ├── figures                              // README中的图片资源。
│   ├── adaptive_max_pool3d_grad_common.h    // 四个Case共用的流水同步等待工具函数。
│   ├── adaptive_max_pool3d_grad_hybrid_ub.h // SIMD清零和SIMT静态UB回填实现。
│   ├── adaptive_max_pool3d_grad_hybrid.h    // SIMD清零和SIMT GM回填实现。
│   ├── adaptive_max_pool3d_grad_scalar.h    // SIMD清零和Scalar GM回填实现。
│   ├── adaptive_max_pool3d_grad_simd.h      // SIMD向量API实现清零和回填。
│   ├── adaptive_max_pool3d_grad_host.asc    // 统一main入口和Host侧运行逻辑。
│   ├── CMakeLists.txt                       // 编译工程文件。
│   ├── README.md                            // 中文样例说明。
│   └── README_en.md                         // 英文样例说明。
```

## 样例描述

**算子介绍：**

adaptive_max_pool3d_grad根据`argmax`给出的下标，将梯度`grad`回填到`output`的对应位置，`argmax`范围之外的位置则填0。如下图所示：

**图 1**  adaptive_max_pool3d_grad算子功能示意图

![](./figures/adaptive_max_pool3d_grad_function_overview.png)

计算公式可以理解为：
```text
output[nc, :] = 0                                       # 将output的所有位置置为0。
output[nc, argmax[nc, d, h, w]] = grad[nc, d, h, w]     # 根据argmax将grad的值填到对应的位置。
```

该算子的特点是包含清零和填充离散数据两个步骤，清零是典型的规整计算，适合使用SIMD完成，离散写入梯度则适合SIMT实现。

**样例规格：**

| 参数 | shape | 数据格式 |数据类型| 描述 |
|:---|:---|:---|:---|:---|
| `output` | [1,4096,32,32,32] | NCDHW |FLOAT| 存储计算结果的张量。 |
| `grad` | [1,4096,N,N,N] | NCDHW |FLOAT| 记录了梯度值。样例中，N的取值范围是1/2/4/8/16。 |
| `argmax` | [1,4096,N,N,N] | NCDHW |INT32| 记录了`grad`中每个梯度要填在`output`中的索引位置，样例中取值范围是[0,32767]，shape和`grad`一致。 |

**Case概览：**

样例使用了4种方式实现该算子，以此来对比既有离散又有规整计算场景不同实现的性能差异。

| Case | 运行参数| 实现文件 |numBlocks|threads_per_block | 对照目的 |
|---|---|---|---|---|---|
| 0 | `simd` | `adaptive_max_pool3d_grad_simd.h` |64|-| 展示纯SIMD实现算子功能。 |
| 1 | `scalar` | `adaptive_max_pool3d_grad_scalar.h` |64|-| 展示用SIMD实现清零和Scalar处理离散写操作的实现方案。 |
| 2 | `hybrid` | `adaptive_max_pool3d_grad_hybrid.h` |64|512| 展示用SIMD实现清零和SIMT实现离散梯度回填的混合方式。 |
| 3 | `hybrid_ub` | `adaptive_max_pool3d_grad_hybrid_ub.h` |64|512| 展示在Case 2基础上融入UB的实现方式。 |

## 样例实现

样例固定`output`为 [1,4096,32,32,32]，即每个`NC`平面有32\*32\*32=32768个元素。且`grad/argmax`的D/H/W（三者取相同值）的取值范围为1/2/4/8/16，因此每个梯度占据的区域互不重叠，在回填梯度时可以直接使用等号赋值，而不需要累加。样例会采集每种实现方式在不同梯度数下的耗时数据。

**性能数据标题说明**

| 列名 | 含义 |
|:---|:---|
| `grad/argmax`的D/H/W | `grad`和`argmax`的D、H、W三个维度的取值，三者相同，通过编译选项`GRAD_DHW`配置，可选1/2/4/8/16。 |
| 每个`NC`平面回填的梯度数 | 该档位下每个`NC`平面上要离散回填的梯度个数，取值为`grad/argmax`的D×H×W，用于衡量离散写的数据量。 |
| Task Duration(us) | `msOpProf`工具输出的Task整体耗时，包含调度到加速器的时间、加速器上的执行时间以及响应结束时间，本样例的性能对比以该列为准。 |

### Case 0：SIMD API清零并回填梯度

**实现方式：**

SIMD操作的对象是向量而不是单个元素，Case 0选择操作的向量如下图所示：

**图 2**  Case 0 SIMD构造向量思路示意图

![](./figures/adaptive_max_pool3d_grad_simd_overview.png)

使用纯SIMD实现的关键步骤如下：

- **遍历每个梯度**：`grad/argmax`中的每个梯度各有一个值和一个`argmax`下标。对于每一个梯度值g以及它对应的坐标a，反推出它在`output`上占据的区域，遍历区域内的每个位置`p`，如果p和a相等，则该位置填值为g，否则填0。
- **构造SIMD向量**：纯SIMD以向量为单位处理数据，因此本实现将output中连续64个`NC`平面上的同一个位置`p`看作一个待处理的向量input_index_reg，同样的方式得到向量grad_reg和argmax_reg，然后使用SIMD API通过Compare-Select-Move的步骤计算得到最终结果。

**关键代码：**

```cpp
asc_duplicate_scalar(output_index_reg, static_cast<int32_t>(base_output_index + tile_index), active_mask);
asc_eq(cmp_mask, argmax_reg, output_index_reg, active_mask);
asc_select(selected_reg, grad_reg, zero_reg, cmp_mask);
asc_storealign(selected_tile + tile_index * NC_TILE, selected_reg, active_mask);
```

**性能数据：**

| `grad/argmax`的D/H/W | 每个`NC`平面回填的梯度数 | Task Duration(us) |
|---:|---:|---:|
| 1 | 1 | 6574.811 |
| 2 | 8 | 6745.974 |
| 4 | 64 | 6763.949 |
| 8 | 512 | 6997.266 |
| 16 | 4096 | 8182.059 |

**性能数据分析：**

在Case 0的实现方式下，`output`中的每一个位置都要经历compare、select、scatter、move等步骤，计算量非常大，导致耗时较高。

各档耗时都在6500us以上，且随梯度数增长很缓慢（4096个梯度相对1个梯度仅增加约24%），这是因为本样例中梯度的区域正好铺满`output`且互不重叠：梯度数为N³时每个区域有(32/N)³个位置，两者相乘恒为32768，即`output`的每个位置都被访问且仅被访问一次。因此比较次数和写回次数都与梯度数无关，耗时主要由`output`的规模决定，这就是6500us这条基线的来源。

梯度数增大并不增加总计算量，只是把同样的工作切得更碎，带来两类额外开销：一是`select_w_tile`的调用次数正比于梯度数在W方向的切分（每次调用都要重新付出更新mask、生成零向量、两次对齐加载这些固定开销，而区域越碎，每次调用内部真正做的比较越少）；二是每个梯度都要单独搬入一次`grad/argmax`，搬运次数随梯度数线性增长。这两项在前几档被恒定的主体计算量盖住，到4096个梯度时才显现为那24%的增幅。

**结论：**

回填梯度的位置在内存中是离散的，通过SIMD实现该算子功能需构造向量利用mask-select的方式，代码复杂度较高，无论是编写还是阅读都非常困难，并且性能表现也较差，因此纯SIMD并不太适合这种Scatter离散写的场景。

> 样例中adaptive_max_pool3d_grad_simd.h是针对特定shape实现的简化版本，完整的泛化算子实现请参考[adaptive_max_pool3d_grad_normal.h](https://gitcode.com/cann/ops-nn/blob/master/pooling/adaptive_max_pool3d_grad/op_kernel/adaptive_max_pool3d_grad_normal.h)。

### Case 1：SIMD清零 + Scalar回填梯度

**实现方式：**

Case 1将清零和梯度回填分成两阶段，每个Block内先使用SIMD将负责的`NC`平面全部填零，然后遍历`grad/argmax`，通过标量循环直接计算目标地址并填入梯度值。

**关键代码：**

```cpp
//...
__aicore__ inline void zero_output(__gm__ float* output_data, uint64_t count)
{
    __ubuf__ float zero_ub[ZERO_TILE_COUNT];
    asc_vf_call<fill_float_ub>(zero_ub, 0.0F, ZERO_TILE_COUNT);
    wait_v_to_mte3();

    for (uint64_t offset = 0; offset < count; offset += ZERO_TILE_COUNT) {
        const uint32_t copy_count = static_cast<uint32_t>(min<uint64_t>(ZERO_TILE_COUNT, count - offset));
        const uint32_t copy_bytes = copy_count * sizeof(float);
        asc_copy_ub2gm(output_data + offset, zero_ub, copy_bytes);
    }
    wait_mte3_to_scalar();
}

//...
for (uint32_t i = 0; i < tile_grad_count; ++i) {
    const uint64_t local_grad_index = static_cast<uint64_t>(grad_offset + i);
    const uint64_t local_nc_index = local_grad_index / grad_spatial;
    const uint64_t target = local_nc_index * output_spatial + static_cast<uint64_t>(argmax_ub[i]);
    output[target] = grad_ub[i];
}
```

**性能数据：**

| `grad/argmax`的D/H/W | 每个`NC`平面回填的梯度数 | Task Duration(us) |
|---:|---:|---:|
| 1 | 1 | 272.131 |
| 2 | 8 | 298.759 |
| 4 | 64 | 455.944 |
| 8 | 512 | 2010.422 |
| 16 | 4096 | 9140.783 |

**性能数据分析：**

相比Case 0，清零这一步由于是操作连续内存，Case 1直接通过SIMD API将UB中的数据0搬运到GM，每个`NC`平面只回填1个梯度时耗时仅272.131us，可见SIMD非常适合连续内存读写。而针对离散梯度回填，Case 1不再枚举每个梯度的区域、遍历区域内的每一个位置，而是直接使用`argmax`计算目标回填地址，计算量比Case 0小了很多。

但Case 1的耗时并不是在所有档位都低于Case 0：Case 0的开销由`output`的规模决定，对梯度数不敏感；而Case 1每回填一个梯度就要执行一次标量循环，耗时与梯度数成正比。因此梯度数较少时Case 1相对Case 0有一个数量级的优势（1个梯度时272.131us对6574.811us），但随着每个`NC`平面回填的梯度数增长到4096，标量循环成为主要的性能瓶颈，耗时增长到9140.783us，反而超过了Case 0的8182.059us。

**结论：**

在SIMD编程中可以使用Scalar计算单元处理离散数据读写，代码要比Case 0构造向量的方式简洁许多，但Scalar计算单元性能较弱，不适合处理大量数据。

> 样例中adaptive_max_pool3d_grad_scalar.h是针对特定shape实现的简化版本，完整的泛化算子实现请参考[max_pool3d_grad_scatter_unified.h](https://gitcode.com/cann/ops-nn/blob/master/pooling/pool_3d_common/op_kernel/arch22/max_pool3d_grad_scatter_unified.h)。

### Case 2：SIMD清零 + SIMT回填梯度

**实现方式：**

Case 2保留Case 1的SIMD清零方式，但是把梯度回填改为采用SIMT处理，每个线程负责一个（或多个）梯度值的回填。

**关键代码：**

```cpp
__simt_vf__ __launch_bounds__(THREAD_COUNT) inline void scatter_grad(
    const __gm__ float* grad, const __gm__ int32_t* argmax, __gm__ float* output, uint32_t nc_count, uint32_t output_spatial, uint32_t grad_spatial)
{
    const uint64_t block_grad_count = static_cast<uint64_t>(nc_count) * grad_spatial;
    for (uint64_t local_grad_index = static_cast<uint64_t>(threadIdx.x); local_grad_index < block_grad_count;
         local_grad_index += static_cast<uint64_t>(blockDim.x)) {
        const uint64_t local_nc_index = static_cast<uint32_t>(local_grad_index) / grad_spatial;
        const uint64_t output_index = local_nc_index * output_spatial + static_cast<uint64_t>(argmax[local_grad_index]);
        output[output_index] = grad[local_grad_index];
    }
}
```

**性能数据：**

| `grad/argmax`的D/H/W | 每个`NC`平面回填的梯度数 | Task Duration(us) |
|---:|---:|---:|
| 1 | 1 | 269.544 |
| 2 | 8 | 283.102 |
| 4 | 64 | 340.51 |
| 8 | 512 | 406.567 |
| 16 | 4096 | 748.313 |

**性能数据分析：**

Case 2相对Case 1各个shape下性能均有提升，并且随shape增加耗时增长幅度远小于Case 1，因为SIMT中能够多线程并行处理离散数据，并且在处理读写操作时能够通过切换线程的方式实现流水掩盖。

**结论：**

在Scatter这种离散读写场景下，SIMT的多线程并行效率高于Main Scalar，并且处理的数据量越大，性能收益越高。并且SIMT处理离散写的代码相比纯SIMD简单许多，开发体验较好。

### Case 3：使用UB进一步提升混合编程的性能

**实现方式：**

在Case 2混合编程的基础上，还可以继续使用SIMT的常用优化手段，这里可以将两阶段合成一步，在UB中完成清零和梯度回填后再将数据搬到GM中。

**关键代码：**

```cpp
// 在UB中申请一段内存，初始值全置为0。
asc_vf_call<hybrid_ub_impl::fill_float_ub>(output_ub, 0.0F, output_spatial);
// ...
// 处理这一段数据需要回填的梯度。
asc_vf_call<hybrid_ub_impl::scatter_grad<float, int32_t>>(
    dim3(hybrid_ub_impl::THREAD_COUNT), grad_data + grad_base_offset,
    argmax_data + grad_base_offset, output_ub, output_spatial, grad_spatial);
asc_copy_ub2gm_align(output_data + output_base_offset, output_ub, active_output_bytes);
```

**性能数据：**

| `grad/argmax`的D/H/W | 每个`NC`平面回填的梯度数 | Task Duration(us) |
|---:|---:|---:|
| 1 | 1 | 278.926 |
| 2 | 8 | 285.910 |
| 4 | 64 | 294.406 |
| 8 | 512 | 336.848 |
| 16 | 4096 | 551.461 |

**性能数据分析：**

要回填的梯度数较少时，GM离散写数量也较少，Case 3额外引入的静态UB缓存和完整平面写回开销会抵消收益，因此回填1个和8个梯度那两档与Case 2基本持平（略慢）。梯度数增大后，SIMT在UB内离散写、MTE连续写回GM的路径开始占优；每个`NC`平面回填4096个梯度时，Case 3相对Case 2耗时从748.313us降至551.461us，约1.36倍加速。

**结论：**

使用混合编程时，SIMD和SIMT的性能优化手段可以复用，性能优化的上限更高。

## 对比总结

所有Case的性能数据如下：

| 每个`NC`平面回填的梯度数 | Case 0 SIMD(us) | Case 1 Scalar(us) | Case 2 Hybrid(us) | Case 3 Hybrid UB(us) |
|---:|---:|---:|---:|---:|
| 1 | 6574.811 | 272.131 | 269.544 | 278.926 |
| 8 | 6745.974 | 298.759 | 283.102 | 285.910 |
| 64 | 6763.949 | 455.944 | 340.51 | 294.406 |
| 512 | 6997.266 | 2010.422 | 406.567 | 336.848 |
| 4096 | 8182.059 | 9140.783 | 748.313 | 551.461 |

在该样例中，存在连续（清零）和离散（梯度回填）两部分数据处理。对比四份核函数实现代码以及上面的性能数据，可以发现在代码编写方面，SIMT和Main Scalar的方式相比SIMD API更简单，而性能方面，SIMT比SIMD和Main Scalar耗时更低，且离散写的数据量越大，收益更高。因此，连续和离散（内存操作）并存的场景下，建议采用混合编程的方式，可以兼顾性能和良好的开发体验。

> 数据清零部分也可以使用SIMT完成，但是性能不如混合版本，可参考[adaptive_max_pool3d_grad_simt.h](https://gitcode.com/cann/ops-nn/blob/master/pooling/adaptive_max_pool3d_grad/op_kernel/arch35/adaptive_max_pool3d_grad_simt.h)。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build                           # 创建并进入build目录。
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j  # 编译工程。
  ./adaptive_max_pool3d_grad simd                      # 执行样例选择simd版本核函数实现。
  ./adaptive_max_pool3d_grad scalar                    # 执行样例选择scalar版本核函数实现。
  ./adaptive_max_pool3d_grad hybrid                    # 执行样例选择hybrid版本核函数实现。
  ./adaptive_max_pool3d_grad hybrid_ub                 # 执行样例选择hybrid_ub版本核函数实现。
  ```

  上述命令使用默认的`grad/argmax`形状 [1,4096,8,8,8]。通过`GRAD_DHW`选项可以改变`grad/argmax`的D/H/W，复现前面各Case性能数据表格中的其他档位。例如采集每个`NC`平面回填4096个梯度那一档的数据：

  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DGRAD_DHW=16 ..;make -j
  ./adaptive_max_pool3d_grad hybrid_ub
  ```

- 编译选项说明

  | 选项             | 可选值      | 说明              |
  | ---------------- | ----------- | ----------------- |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构：本样例仅支持dav-3510（Ascend 950PR/Ascend 950DT）。 |
  | `GRAD_DHW` | `1`、`2`、`4`、`8`、`16` | `grad/argmax`的D/H/W取值，三者相同，默认为`8`。每个`NC`平面回填的梯度数即`grad/argmax`的D×H×W，用于复现性能对比表格中的各档数据。 |

- 运行参数说明

  样例可执行文件的运行命令格式如下。

  ```bash
  ./adaptive_max_pool3d_grad <implementation> [profile]
  ```

  | 参数 | 可选值 | 是否必选 | 说明 |
  | ---- | ------ | -------- | ---- |
  | `implementation` | `simd`、`scalar`、`hybrid`、`hybrid_ub` | 是 | 选择样例运行的核函数实现版本。 |
  | `profile` | `profile` | 否 | 性能分析模式参数。指定该参数后仅执行算子并跳过结果精度对比，便于配合msOpProf采集性能数据，采集性能时可能会对计算结果有影响。 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```text
  test pass!
  ```

## 性能分析

### msOpProf工具介绍
msOpProf工具是单算子性能分析工具。包含msopprof和msopprof simulator两种使用方式。该工具协助用户定位算子内存、算子代码以及算子指令的异常，实现全方位的算子调优。当前支持基于不同运行模式（上板或仿真）和不同文件形式（可执行文件或算子二进制.o文件）进行性能数据的采集和自动解析。

- 上板性能采集

    通过上板性能采集，可以直接测定算子昇腾AI处理器上的运行时间。该方式适合在板环境中快速定位算子性能问题。

    使用`msOpProf`工具获取详细性能数据：
    ```bash
    msopprof ./adaptive_max_pool3d_grad simd profile   # 分析性能
    ```

    - 性能数据说明

      命令完成后，会在默认目录下生成以“OPPROF_{timestamp}_XXX”命名的文件夹，性能数据文件夹结构示例如下：

      ```bash
      ├──dump                       # 原始的性能数据，用户无需关注
      ├──ArithmeticUtilization.csv  # cube/vector指令cycle占比
      ├──L2Cache.csv                # L2 Cache命中率，影响MTE2，建议合理规划数据搬运逻辑，增加命中率
      ├──Memory.csv                 # UB，L1和主存储器读写带宽速率
      ├──MemoryL0.csv               # L0A，L0B，和L0C读写带宽速率
      ├──MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率
      ├──OpBasicInfo.csv            # 算子基础信息
      ├──PipeUtilization.csv        # 采集计算单元和搬运单元耗时和占比
      ├──ResourceConflictRatio.csv  # UB上的bank group、bank conflict和资源冲突率在所有指令中的占比
      └──visualize_data.bin         # MindStudio Insight呈现文件
      ```

查看具体的性能分析结果：

```bash
# 查看Task Duration以及各项数据。
cat ./OPPROF_*/OpBasicInfo.csv
```
