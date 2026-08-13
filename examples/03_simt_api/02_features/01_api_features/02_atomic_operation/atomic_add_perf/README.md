# asc_atomic_add接口性能对比样例

## 概述

本样例以[asc_atomic_add()](../../../../../../docs/zh/api/SIMT-API/atomic_operations/asc_atomic_add.md)接口为例，通过五组对照实验，展示不同内存空间（Global Memory / Unified Buffer）、不同原子累加组织方式（直接原子加 / 线程块内先累加再写回GM）、是否使用返回值、同一地址上的竞争程度、目标地址的Sector分布，以及数据类型（int32_t、int64_t）等不同使用场景下的接口性能差异，并给出相应的优化指导。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本

- \>= CANN 9.1.0

## 目录结构介绍

```text
├── atomic_add_perf
│   ├── figures                         // README中的图片资源
│   ├── CMakeLists.txt                  // 样例构建脚本
│   ├── atomic_add_perf.asc             // 样例代码
│   ├── README.md
│   └── README_en.md
```

## 样例描述

本样例总共包含16个场景，分别测试`asc_atomic_add()`接口在下列使用场景中的性能表现：

- 不同内存空间（Global Memory、Unified Buffer）。
- 不同原子累加组织方式（直接原子加、线程块内先在UB累加再写回GM）。
- 是否使用返回值。
- 同一地址上的竞争程度和目标地址的Sector分布。
- 数据类型（int32_t、int64_t）。

16个场景归为5组对照实验（Case 1~5）进行性能对比，最终给出`asc_atomic_add()`接口的性能特性说明。样例通过`SCENARIO_NUM`选择一个待测场景，每次运行仅启动一个目标Kernel。

**5组对照**：

| 对照Case | 对照维度 | 参与场景 |
|:---:|---|---|
| Case 1 | 启动1个线程块时的内存位置：Global Memory → Unified Buffer | 场景1 ↔ 场景2 |
| Case 2 | 多线程块下的内存位置：Global Memory → Unified Buffer | 场景3 ↔ 场景4 |
| Case 3 | 是否使用返回值：使用返回值 → 不使用返回值 | 场景5 ↔ 场景6（GM）<br>场景7 ↔ 场景8（UB） |
| Case 4 | Padding布局下增加同一地址上的线程数；保持相同工作负载，对比Padding与紧密排布 | 场景9→10→11、场景11↔场景12 |
| Case 5 | 数据类型的取舍：int32_t → int64_t | 场景13 ↔ 场景14（无返回值）<br>场景15 ↔ 场景16（有返回值） |

## 样例实现

本节以五组对照（Case 1~5）逐一分析内存层级、原子组织方式、返回值、同一地址上的竞争程度、目标地址的Sector分布、数据类型的取舍对原子加性能的影响。每组对照只改变一个属性、其余保持一致，并给出对应的`msopprof`实测数据与性能表现根因分析。

### 性能指标说明

| 指标 | 说明 |
| --- | --- |
| Task Duration（μs） | Task整体耗时，包含调度到加速器的时间、加速器上的执行时间以及响应结束时间 |


---

### Case 1：不同内存空间性能差异（单线程块场景）

**样例目标**：在所有线程竞争同一个地址的场景下，对比原子加访问Global Memory与Unified Buffer时的性能差异。

**场景配置**：

| 场景 | 内存位置 | 数据类型 | 返回值 | 规模（Block×Thread） | 原子加目标地址数 |
|:---:|:---:|:---:|:---:|:---:|:---:|
| 1 | Global Memory | int32_t | 否 | 1×1024 | 1 |
| 2 | Unified Buffer | int32_t | 否 | 1×1024 | 1 |

**核心实现**：启动1个线程块，每个线程块包含1024个线程。GM场景下1024个线程对同一个GM地址执行原子加；UB场景下1024个线程对同一个UB地址执行原子加。Case 1只对比接口访问GM和访问UB的开销，UB场景不做正确性校验。

```cpp
// 场景1：GM单地址，所有线程竞争同一个GM计数器
__global__ void atomic_add_gm_single_no_return(int32_t* counter)
{
    asc_atomic_add(counter, 1);  // 1024个线程访问同一GM地址
}

// 场景2：UB单地址，所有线程调用同一个UB地址上的原子加接口
__global__ void atomic_add_ub_local_no_return()
{
    __ubuf__ int32_t local[1];
    asc_atomic_add(&local[0], 1);  // 1024个线程访问同一UB地址
}
```

**性能数据**：

| 场景 | 内存 | Task Duration（μs） |
|:---:|:---:|---:|
| 1 | Global Memory | 87.60 |
| 2 | Unified Buffer | 1.84 |

**分析**：

性能数据呈现出一个现象：

**UB上的原子操作显著快于GM原子操作。** 同样由1024个线程竞争一个地址时，GM场景耗时约87.60μs，UB场景耗时约1.84μs，前者约为后者的**48倍**。

两个场景的线程数、数据类型、是否使用返回值和目标地址数均相同，唯一变化是原子加的内存位置。为解释这一现象，需要先了解原子加的原理与GM、UB的存储位置。

**原理一：同一地址上的原子操作只能串行排队，无法并行执行。**

原子加需完成“读出旧值 → 计算新值 → 写回新值”三个步骤。若多个线程同时更新同一地址且缺乏保护，可能出现多个线程读到相同旧值、后写回的结果覆盖先写回的结果，导致部分加法丢失。原子操作为避免此类丢失，保证“读—算—写”三步作为一个不可分割的整体完成。因此，同一地址上的多个原子操作无法并行执行，只能串行执行。

**原理二：GM与UB的原子访问路径和处理开销不同。**

GM是位于AI Core外部的设备全局内存，访问路径较长；UB是位于AI Core内部的共享内存，访问路径更短、延迟更低，因此UB上的单次原子操作处理开销低于GM。
在本组同地址竞争场景中，GM与UB上的1024次原子操作均需串行执行，处理开销的差异会随串行执行不断叠加，最终使GM的总耗时显著高于UB。

**结论**：对于可在UB中完成的原子累加，应避免在GM上执行。

---

### Case 2：不同内存空间性能差异（多线程块场景）

**样例目标**：在多个线程块同时竞争一个GM地址的场景下，对比“所有线程直接原子加到同一个GM地址”与“先在各线程块自己的UB中累加、再由每个线程块写回GM”两种方式的性能差异。

**场景配置**：

| 场景 | 内存位置 | 数据类型 | 返回值 | 规模（Block×Thread） | 原子加目标地址数 |
|:---:|:---:|:---:|:---:|:---:|:---:|
| 3 | Global Memory | int32_t | 否 | 8×1024 | 1 |
| 4 | Unified Buffer | int32_t | 否 | 8×1024 | 1 |

**核心实现**：启动8个线程块、每个线程块1024个线程（共8192个线程）。GM场景下8192个线程直接对同一个GM地址执行原子加；UB场景下每个线程块先在自己的UB数组`local[1]`上累加，再由每个线程块各做一次GM原子加写回。

```cpp
// 场景3：GM单地址，8192个线程都直接竞争同一个GM计数器
__global__ void atomic_add_gm_single_no_return(int32_t* counter)
{
    asc_atomic_add(counter, 1);
}

// 场景4：每个线程块先在自己的UB中累加，再由线程0写回GM
__global__ void atomic_add_ub_block_accum_no_return(int32_t* gm_result)
{
    __ubuf__ int32_t local[1];
    if (threadIdx.x == 0) {
        local[0] = 0;
    }
    asc_syncthreads();

    asc_atomic_add(&local[0], 1);  // 1024个线程访问本线程块的UB地址
    asc_syncthreads();

    if (threadIdx.x == 0) {
        asc_atomic_add(gm_result, local[0]);  // 每个线程块仅1次GM原子加
    }
}
```

**性能数据**：

| 场景 | 组织方式 | Task Duration（μs） |
|:---:|:---:|---:|
| 3 | GM直接原子加 | 351.47 |
| 4 | UB块内累加后写回GM | 2.25 |

**分析**：

两种方式的原子加总数相同，耗时却相差约**156倍**（351.47μs vs 2.25μs）。差距来自两个叠加因素：

- **并行 vs 串行。** 落在同一地址上的原子操作只能串行执行（原理见Case 1）。GM是所有线程块的线程都可以直接访问的内存，场景3中8192个原子加集中在唯一的GM地址上，只能串行执行；UB只有当前线程块内部的线程可以访问，不同线程块使用各自的UB。场景4中各线程块先在UB上累加，同一地址上的原子操作仅限于同一线程块内的1024个线程，并且8个线程块之间的UB原子操作互不竞争、可真正并行，仅最后8次GM写回需串行执行。
- **UB上的原子操作性能优于GM。** UB上原子加延迟远低于GM（见Case 1）。因此场景4虽多了UB初始化、`asc_syncthreads`同步和GM写回等操作，但绝大部分原子加都在UB上完成、仅8次落在GM。

这两个因素共同作用：绝大部分原子加不仅转移到了更快的UB上，还被拆分到各线程块并行执行，二者带来的收益远大于UB初始化、同步、GM写回等额外操作的开销，因此场景4整体仍大幅快于场景3。

**结论**：当多个线程块要进行同地址原子累加时，不建议让每个线程都直接对GM做原子加——这会让所有线程在同一个GM地址上串行排队。更推荐先在每个线程块的UB中并行完成块内累加，再由每个线程块向GM写回一次，既将大部分GM上的原子操作转移到性能更优的UB上、又把它们拆分为可并行执行的任务。

---

### Case 3：返回值开销

**样例目标**：分别在GM和UB上，对比`asc_atomic_add()`使用返回值与不使用返回值的性能差异。

**场景配置**：

| 场景 | 内存位置 | 数据类型 | 返回值 | 规模（Block×Thread） | 原子加目标地址数 |
|:---:|:---:|:---:|:---:|:---:|:---:|
| 5 | Global Memory | int32_t | 是 | 1×2048 | 1 |
| 6 | Global Memory | int32_t | 否 | 1×2048 | 1 |
| 7 | Unified Buffer | int32_t | 是 | 1×2048 | 1 |
| 8 | Unified Buffer | int32_t | 否 | 1×2048 | 1 |

**核心实现**：启动1个线程块，每个线程块包含2048个线程，分别在GM和UB上测试使用返回值与不使用返回值的场景，除是否使用返回值外，其余条件保持一致。使用返回值的场景将旧值写入线程本地`volatile`变量，避免返回值路径被编译器优化消除。

```cpp
// 场景5：GM单地址，使用返回值
__global__ void atomic_add_gm_single_return(int32_t* counter)
{
    volatile int32_t old_value_sink = asc_atomic_add(counter, 1);
    (void)old_value_sink;
}

// 场景6：GM单地址，不使用返回值
__global__ void atomic_add_gm_single_no_return(int32_t* counter)
{
    asc_atomic_add(counter, 1);
}

// 场景7：UB单地址，使用返回值
__global__ void atomic_add_ub_local_return()
{
    __ubuf__ int32_t local[1];
    volatile int32_t old_value_sink = asc_atomic_add(&local[0], 1);
    (void)old_value_sink;
}

// 场景8：UB单地址，不使用返回值
__global__ void atomic_add_ub_local_no_return()
{
    __ubuf__ int32_t local[1];
    asc_atomic_add(&local[0], 1);
}
```

**性能数据**：

| 内存 | 使用返回值（μs） | 不使用返回值（μs） | 耗时比值 |
|:---:|---:|---:|:---:|
| GM int32_t（场景5、6） | 421.59 | 159.88 | 2.64× |
| UB int32_t（场景7、8） | 3.17 | 2.76 | 1.15× |

**分析**：

是否使用返回值对性能的影响在GM与UB上差异明显：

- **GM int32_t**：不使用返回值时约159.88μs，使用返回值时约421.59μs，相差约**2.64倍**。
- **UB int32_t**：不使用返回值时约2.76μs，使用返回值时约3.17μs，仅相差约**1.15倍**。

产生该差异的原因是：`asc_atomic_add()`接口是否使用返回值，会让编译器生成**两条不同的原子加指令**——不使用返回值时，编译器会选择性能更优的指令；使用返回值时生成的指令性能开销更高。这一指令性能差异在GM上被显著放大，在UB上差距较小，因此GM上是否使用返回值对耗时的影响远大于UB。

**结论**：建议在业务允许的条件下优先选择不使用返回值的方式。

---

### Case 4：同一地址上的竞争程度与Sector分布的影响

**样例目标**：分别对比同一地址上的不同线程数导致的竞争程度和目标地址的Sector分布对原子加性能的影响。

为理解上述两类因素对原子加性能的影响，下面先介绍相关原理。

**原理一：同一地址上的原子操作串行排队。**

原子加需完成“读出旧值—计算新值—写回新值”三个步骤，并保证整个过程不可分割。多个线程更新同一个地址时，原子操作只能依次完成；同一地址上的线程数越多，等待执行的原子加越多，总耗时越高。

**原理二：GM原子操作以Sector为粒度。**

原子加访问GM地址时，会经过L2缓存层级，在L2侧完成“读—算—写”。L2 Cache以512B Cache Line为缓存管理单位，每条Cache Line包含4个连续的128B Sector；GM原子操作以128B Sector为处理粒度。原子操作的目标地址集中在同一个Sector内时，处理效率较低；目标地址分布在更多Sector内时，处理效率较高。

**对照设计**：

1. **竞争程度对照（场景9→10→11）**：使用Padding布局保证每个目标地址位于一个独立的Sector中，将每个地址上的线程数从1增加到12和384。每个活跃线程执行1次原子加，因此原子加总次数随线程数同步增加。
2. **Sector分布对照（场景11↔场景12）**：保持目标地址数、每地址线程数和原子加总次数相同，仅将地址布局从Padding改为紧密排布，让多个目标地址共用一个Sector。

**场景配置**：所有场景均使用32个GM目标地址，且均使用原子加返回值；每个活跃线程仅执行1次原子加。

| 场景 | 内存位置 | 数据类型 | 规模（Block×Thread） | 地址布局 | 每地址线程数 | 原子加总次数 | 目标地址覆盖Sector数 |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 9 | Global Memory | int32_t | 48×256 | Padding | 1 | 32 | 32 |
| 10 | Global Memory | int32_t | 48×256 | Padding | 12 | 384 | 32 |
| 11 | Global Memory | int32_t | 48×256 | Padding | 384 | 12288 | 32 |
| 12 | Global Memory | int32_t | 48×256 | 紧密排布 | 384 | 12288 | 1 |

**地址布局示意**：一个128B Sector可容纳32个`int32_t`地址。Padding布局使32个目标地址分别位于32个Sector中；紧密排布使32个目标地址连续存放在同一个Sector中。

两种地址布局的对比如下图所示：

<img src="figures/case4_address_layout.png" alt="Case 4 Padding布局与紧密排布对比" style="width: 90%; height: auto;">

**场景逻辑**：四个场景均启动48个线程块、每个线程块256个线程；每个活跃线程仅对一个目标地址执行1次原子加。

- **场景9**：启用32个线程，分别操作32个Padding布局的目标地址，每个地址由1个线程操作，32个地址分布在32个Sector中。
- **场景10**：启用384个线程，分别操作32个Padding布局的目标地址，每个地址由12个线程操作，32个地址分布在32个Sector中。
- **场景11**：启用12288个线程，分别操作32个Padding布局的目标地址，每个地址由384个线程操作，32个地址分布在32个Sector中。
- **场景12**：启用12288个线程，分别操作32个紧密排布的目标地址，每个地址由384个线程操作，32个地址集中在1个Sector中。

> [!NOTE]说明
> 本组场景统一使用返回值：由Case 3可知，不使用返回值时`int32_t`会触发指令优化，其耗时主要由该优化决定，会掩盖同一地址上的线程数和Sector分布对性能的影响。

```cpp
__global__ void atomic_add_gm_case4_i32_return(
    int32_t* counters, uint64_t target_count, uint64_t threads_per_target,
    uint64_t target_stride)
{
    uint64_t global_tid = blockIdx.x * blockDim.x + threadIdx.x;
    uint64_t active_threads = target_count * threads_per_target;
    if (global_tid >= active_threads) {
        return;
    }

    uint64_t target_id = global_tid % target_count;
    int32_t* target = counters + target_id * target_stride;
    volatile int32_t old_value_sink = asc_atomic_add(target, 1);
    (void)old_value_sink;
}
```

**性能数据**：

| 场景 | Task Duration（μs） |
|:---:|---:|
| 9 | 2.53 |
| 10 | 5.11 |
| 11 | 80.17 |
| 12 | 2164.72 |

**分析**：

1. **Padding布局下，每个地址上的线程数和原子加总次数增加时，总耗时单调增加。** 每地址线程数从1增加到12和384时，耗时从2.53μs增加到5.11μs和80.17μs。这是因为每个线程执行1次原子加，线程数增加使原子加总次数同步增加，并且同一地址上的原子操作需要串行排队，等待执行的原子加随之增多，导致耗时越高。
2. **相同工作负载下，目标地址紧密排布时耗时显著增加。** 紧密排布布局的场景12与Padding场景11相比，耗时从80.17μs增加到2164.72μs，约为原来的27.0倍。这是因为Padding布局将32个目标地址分散在32个Sector中，而紧密排布将其集中在1个Sector中；GM原子操作以128B Sector为处理粒度，目标地址集中时处理效率较低。

**结论**：

- 同一地址上执行原子操作的线程数越多，整体耗时越高。
- 当必须在GM上执行原子操作时，竞争强度应以Sector为单位评估；若业务场景允许，可采用Padding方式分散目标地址，以减少耗时。

---

### Case 5：数据类型的取舍

**样例目标**：在GM上存在同地址竞争的场景下，对比int32_t与int64_t原子加的性能，说明数据类型的选择取决于是否使用返回值，不能仅依据“数据类型位宽越小、性能越优”的经验进行判断。

**场景配置**：

| 场景 | 内存位置 | 数据类型 | 返回值 | 规模（Block×Thread） | 原子加目标地址数 |
|:---:|:---:|:---:|:---:|:---:|:---:|
| 13 | Global Memory | int32_t | 否 | 48×256 | 1024 |
| 14 | Global Memory | int64_t | 否 | 48×256 | 1024 |
| 15 | Global Memory | int32_t | 是 | 48×256 | 1024 |
| 16 | Global Memory | int64_t | 是 | 48×256 | 1024 |

> [!NOTE]说明
> 由于int64_t原子加仅支持GM，本样例的数据类型对照仅在GM场景下进行。

**核心实现**：启动48个线程块、每个线程块256个线程（共12288个线程），每个线程执行1次原子加1操作，原子加结果写入GM中由若干连续地址组成的内存区域。通过`target_count`控制GM中参与原子加的目标地址数，本组固定`target_count = 1024`，即12个线程竞争同一个地址。在相同的地址竞争程度下，分别测试`int32_t`和`int64_t`使用返回值与不使用返回值的四种组合。

```cpp
// 不使用返回值
__global__ void atomic_add_gm_dense_i32_no_return(int32_t* counters, uint64_t target_count)
{
    asc_atomic_add(counters + ((blockIdx.x * blockDim.x + threadIdx.x) % target_count), 1);
}

// 使用返回值
__global__ void atomic_add_gm_dense_i32_return(int32_t* counters, uint64_t target_count)
{
    volatile int32_t old_value_sink =
        asc_atomic_add(counters + ((blockIdx.x * blockDim.x + threadIdx.x) % target_count), 1);
    (void)old_value_sink;
}
```

**性能数据**：

| 是否使用返回值 | int32_t（μs） | int64_t（μs） | 性能对比 |
|:---:|---:|---:|:---:|
| 不使用返回值（场景13、14） | 1.89 | 58.46 | int32_t耗时较短，约为int64_t的1/31 |
| 使用返回值（场景15、16） | 110.95 | 54.28 | int32_t耗时较长，约为int64_t的2倍 |

**分析**：

int32_t与int64_t在两种情形下的性能关系相反：不使用返回值时，int32_t与int64_t的耗时分别为1.89μs和58.46μs；使用返回值时，int32_t与int64_t的耗时分别为110.95μs和54.28μs。两种情形分别由不同机制主导：

- **不使用返回值时**，int32_t原子加执行优化后的指令。在约12个线程竞争同一地址的场景下，int32_t与int64_t的耗时分别为1.89μs和58.46μs，表明int32_t的处理效率显著高于int64_t。
- **使用返回值时**，int32_t无法触发上述优化，实测耗时约为int64_t的2倍。相同地址数量紧密排布时，int32_t与int64_t分别覆盖不同数量的Sector；同时，改变数据类型也会改变原子指令的数据位宽，因此本组跨类型数据仅用于比较实际性能，不单独作为Sector影响的证据。Case 4通过同一类型的Padding与紧密排布对照说明Sector分布的影响。

上述结果表明，int32_t与int64_t的性能关系并非仅由数据类型位宽决定，还取决于是否使用返回值。

**结论**：应根据是否使用返回值选择数据类型。纯计数等不使用返回值的场景可优先选择int32_t，编译器可为该类型生成优化后的指令；在使用返回值的场景下，应根据实测结果选择数据类型。目标地址分布密集时，可通过Padding或结构体对齐分散目标地址。

---

## 性能对比总结

**全场景Task Duration汇总**：

| 场景 | 对照Case编号 | 内存 | 数据类型 | 返回值 | 规模 | 原子加配置 | Task Duration（μs） |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|---:|
| 1 | Case 1 | GM |   int32_t | 否 | 1×1024 | 1个地址 | 87.60 |
| 2 | Case 1 | UB |  int32_t | 否 | 1×1024 | 1个地址 | 1.84 |
| 3 | Case 2 | GM |   int32_t | 否 | 8×1024 | 1个地址 | 351.47 |
| 4 | Case 2 | UB |  int32_t | 否 | 8×1024 | 1个地址 | 2.25 |
| 5 | Case 3 | GM |   int32_t | 是 | 1×2048 | 1个地址 | 421.59 |
| 6 | Case 3 | GM |   int32_t | 否 | 1×2048 | 1个地址 | 159.88 |
| 7 | Case 3 | UB |  int32_t | 是 | 1×2048 | 1个地址 | 3.17 |
| 8 | Case 3 | UB |  int32_t | 否 | 1×2048 | 1个地址 | 2.76 |
| 9 | Case 4 | GM |   int32_t | 是 | 48×256 | 32个Padding地址，每地址1个线程，每线程1次 | 2.53 |
| 10 | Case 4 | GM |   int32_t | 是 | 48×256 | 32个Padding地址，每地址12个线程，每线程1次 | 5.11 |
| 11 | Case 4 | GM |   int32_t | 是 | 48×256 | 32个Padding地址，每地址384个线程，每线程1次 | 80.17 |
| 12 | Case 4 | GM |   int32_t | 是 | 48×256 | 32个紧密排布地址，每地址384个线程，每线程1次 | 2164.72 |
| 13 | Case 5 | GM |   int32_t | 否 | 48×256 | 1024个紧密排布地址 | 1.89 |
| 14 | Case 5 | GM |   int64_t | 否 | 48×256 | 1024个紧密排布地址 | 58.46 |
| 15 | Case 5 | GM |   int32_t | 是 | 48×256 | 1024个紧密排布地址 | 110.95 |
| 16 | Case 5 | GM |  int64_t | 是 | 48×256 | 1024个紧密排布地址 | 54.28 |


## 调优建议

1. **优先使用UB而非GM完成原子累加**：能在UB完成的原子累加应避免放到GM上执行；多线程块累加同一地址时，应先在各线程块的UB中完成累加，再由每个线程块写回GM一次，而非所有线程直接对GM执行原子加。
2. **分散原子操作的目标地址**：GM原子操作以128B Sector为硬件处理粒度。目标地址集中在同一个Sector内时处理效率较低，分布在更多Sector内时处理效率较高，因此应尽量分散不同线程访问的目标地址。
3. **非必要不使用返回值**：不使用返回值时编译器会生成性能更优的指令；纯计数场景应优先选择int32_t（不使用返回值时具备指令优化），若业务要求使用返回值且目标地址分布密集，可选用int64_t或对int32_t进行padding。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 样例执行

  编译并运行场景1：

  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=1 ..
  make -j
  ./atomic_add_perf
  ```

  编译选项说明：

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构：本样例仅支持dav-3510（Ascend 950PR/Ascend 950DT） |
  | `SCENARIO_NUM` | `1`-`16` | 待测场景编号。每个场景的规模（Block×Thread）、数据类型、是否使用返回值、`target_count`、`threads_per_target`和`target_stride`等均由源码中`get_scenario_config()`统一配置 |
  | `SKIP_VALIDATION` | `ON`/`OFF` | 是否跳过结果校验，默认`OFF`。Case 1/3的UB-only场景没有GM结果，会自动跳过校验；使用`msopprof`采集GM写回场景时建议设为`ON` |

  执行结果如下，说明正确性校验通过。

  ```text
  Scenario 1: Case1 GM int32 single-address, no return value
  Workload: blocks=1 threads_per_block=1024 total_atomic_adds=1024
  Validation passed
  ```

## 性能分析

使用`msopprof`工具获取详细性能数据：

```bash
msopprof ./atomic_add_perf
```

命令完成后，会在默认目录下生成以“OPPROF_{timestamp}_XXX”命名的文件夹，性能数据文件夹结构示例如下：

```text
├──dump                       # 原始的性能数据，用户无需关注
├──ArithmeticUtilization.csv  # cube/vector指令cycle占比
├──L2Cache.csv                # L2 Cache命中率
├──Memory.csv                 # UB，L1和主存储器读写带宽速率
├──MemoryL0.csv               # L0A，L0B，和L0C读写带宽速率
├──MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率
├──OpBasicInfo.csv            # 算子基础信息
├──PipeUtilization.csv        # 采集计算单元和搬运单元耗时和占比
├──ResourceConflictRatio.csv  # UB上的bank group、bank conflict和资源冲突率在所有指令中的占比
└──visualize_data.bin         # MindStudio Insight呈现文件
```

> [!NOTE]说明
> **关于性能采集时的Validation failed：** 除Case 1/3的UB-only场景外，样例场景的最终累加结果会写回GM计数器，该计数器仅在host侧分配时清零一次，kernel仅累加、不在入口清零。`msopprof`的warmup+replay会在同一块GM内存上重复执行kernel，计数器被累加多次，因此严格校验模式下会报`Validation failed`（计数为期望值的整数倍）。
>
> Case 2的UB块内累加场景虽然UB上的中间累加器在kernel入口清零、对replay幂等，但最终仍通过`asc_atomic_add`写回GM计数器，该GM计数器在replay下同样被多次累加，因此校验同样会失败。
>
> 该现象是replay机制与校验逻辑的固有冲突。采集所有场景时均建议通过`-DSKIP_VALIDATION=ON`跳过校验。
