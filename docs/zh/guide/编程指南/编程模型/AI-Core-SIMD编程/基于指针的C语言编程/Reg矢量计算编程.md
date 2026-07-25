# Reg矢量计算编程<a name="ZH-CN_TOPIC_0000002600000001"></a>

> 📌 **提示**：Reg矢量编程仅在[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)进行SIMD矢量计算编程时支持。

本文介绍如何在Ascend C中使用语言扩展C API进行寄存器级**矢量计算编程**，该能力面向已经熟悉[Ascend C编程模型](../../编程模型概述.md)的开发者。这种基于硬件寄存器的编程方式称为Reg矢量计算编程。

## 为什么需要Reg矢量编程<a name="section_part1"></a>

### Memory矢量计算的瓶颈

在以往的SIMD矢量编程中，矢量计算单元的源操作数和目的操作数都直接落在LocalMemory（Unified Buffer，以下简称UB）上。每一条矢量计算的执行过程都包含"从UB读 → 在执行单元算 → 写回UB"。当一个算子由多个矢量计算串联组成时（例如`dst = (a + b) * c + d`），每一步的中间结果都必须先写回UB，再被下一条指令重新读出。

这种模式下存在三个典型瓶颈：

- **UB读写带宽被反复占用**：中间结果在UB上往返，访存压力随着矢量计算增加而线性放大。
- **UB Bank冲突严重**：对UB的反复读写，极大增加了UB Bank冲突的概率。
- **关键路径被访存拉长**：当计算指令执行时间很短，整体耗时会被UB读写时间占用。

这种“每步落地UB”的开销会成为优化的天花板。

### Reg矢量计算的定位

为了突破上述Memory矢量编程瓶颈限制，[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)在硬件中新增了一级缓存**SIMD Register File**（以下简称寄存器）这一级存储；Reg矢量编程正是面向这一级寄存器的编程方式。其核心思想是：将一段连续的矢量计算保留在寄存器中完成，仅在进入和退出这段计算时与UB交互一次，中间结果可不再回写UB。

Reg矢量计算与Memory矢量计算的对比如下：

| 维度 | Memory矢量计算 | Reg矢量计算 |
| --- | --- | --- |
| 数据位置 | UB | SIMD Registers |
| 中间结果 | 必须回写UB | 可在寄存器中连续消费 |
| 使用成本 | 较低 | 较高（需要理解寄存器等相关操作） |
| 适用目标 | 快速实现、不追求极致性能 | 追求极致性能、可接受更高代码复杂度 |

**在开放寄存器编程的硬件上，同时提供Memory矢量计算接口，保持与前序代际硬件兼容。在Memory矢量计算接口内部实现软件封装的Reg矢量计算。**

### 适用场景

当以下条件同时满足时，建议考虑使用Reg矢量编程：

1. 目标设备支持Reg矢量编程。
2. 核心耗时集中在连续矢量计算上。
3. 中间结果在UB中存在明显的反复读写。
4. 追求极致性能收益，可接受更高的代码复杂度。

## 硬件原理<a name="section_part2"></a>

### SIMD Register File与执行单元

AIV是AI Core内用于矢量计算的核，核内参与Reg矢量计算的硬件单元包括：

- **Reg矢量执行单元**：用于执行Reg矢量计算，从寄存器读取数据，完成计算后将结果写回寄存器。
- **DMA单元**：用于执行Reg矢量搬运，负责在寄存器和UB之间搬运数据。
- **Aux Scalar**：处理Reg矢量执行单元和Reg搬运单元所需的标量计算（例如地址计算）。

Reg矢量执行单元、DMA单元和Aux Scalar虽属于不同的硬件执行单元，但是在实际执行时都归属PIPE_V流水。这一架构带来两个直接结果：

1. 有寄存器依赖时，由硬件按指令顺序保证数据依赖正确性，用户无需显式插入同步；但跨寄存器对同一UB区域的读写需要显式插入同步，搬运单元与计算单元之间没有自动顺序约束，详见[流水线同步](#section_sync)。
2. Reg矢量执行单元和DMA单元在没有数据依赖时可以**同时发射、并行执行**。

**图1** SIMD Reg矢量执行关系

<img src="../../../../figures/Reg执行单元.png" title="Reg执行单元" style="zoom:80%;" />

### 内存层级

AIV的内存层级如下：

**图2** 内存层级关系

![](../../../../figures/Reg内存层级.png "Reg内存层级")

在如上的SIMD架构中，UB和寄存器均为每个AIV核内独享的存储空间。其中寄存器不支持从GM直接加载数据或者直接将数据写出到GM。当需要读取GM数据到寄存器时，必须先从GM搬运到UB，再将UB上的数据搬入到寄存器。当需要写出寄存器数据到GM时，必须先将寄存器上的数据搬出到UB上，再从UB搬运到GM。

**寄存器作为整体使用**：寄存器无法按索引或偏移访问其中的某个元素或者bit。根据不同的功能，主要分为五类：

1. **矢量计算寄存器**：参与矢量计算的主要寄存器，单个寄存器宽度定义为Vector Length（以下简称VL），可以通过DMA单元与UB进行数据交换。
2. **掩码寄存器**：用于控制参与矢量计算的有效元素或者代表单bit元素参与计算等，单个寄存器宽度定义为VL/8，可以通过DMA单元与UB进行数据交换。
3. **地址寄存器**：辅助Reg矢量搬运单元完成矢量计算寄存器和UB数据交换，用于存储UB地址的偏移，单个寄存器宽度为32bis，不能直接与UB进行数据交换。
4. **搬入非对齐寄存器**：辅助Reg矢量搬运单元将UB数据搬入到矢量计算寄存器，用于临时存储包含源非对齐UB地址数据的，起始对齐UB地址的1个DataBlock长度的数据，单个寄存器宽度为DataBlock，可以通过DMA单元将UB数据搬入非对齐寄存器。
5. **搬出非对齐寄存器**：辅助Reg矢量搬运单元将矢量计算寄存器数据搬出到UB，用于临时存储包含需写出非对齐UB地址的对应的矢量寄存器的数据，单个寄存器宽度为DataBlock，可以通过DMA单元将搬出非对齐寄存器数据搬出到UB。



## 编程模型<a name="section_part3"></a>

### 总体结构

基于寄存器（Regbase）的编程模型是在Memory矢量编程的`数据搬入` -> `Compute` -> `数据搬出`编程模型基础上扩展为`数据搬入` -> `Load` -> `Compute`  ->`Store` -> `数据搬出`的Reg矢量计算编程模型。

**图3** Regbase编程总体结构

![](../../../../figures/Regbase编程模型总体结构.png "Regbase编程模型总体结构")

### VF函数与执行域

Reg矢量计算编程模型中`Load` -> `Compute`  ->`Store`的执行过程，在实现中定义为Vector Function（以下简称VF函数）。为了区分不同的执行域，将在不同执行域的函数使用不同的函数标签，编译期就限制了API的调用关系：

| 标签 | 角色 | 调用方 | 可调用对象 |
| --- | --- | --- | --- |
| `__aicore__` | Device侧普通函数 | 核函数或其他`__aicore__`函数 | `__aicore__`函数，或通过`asc_vf_call`调用VF函数 |
| `__simd_vf__` | VF函数 | `__aicore__`函数通过`asc_vf_call`调用 | `__simd_callee__`修饰的Reg矢量API或者VF内部子函数 |
| `__simd_callee__` | VF内部子函数 | `__simd_vf__`或其他`__simd_callee__` | `__simd_callee__` |

> 📌 **提示**：在[硬件原理](#section_part2)章节中提到Vector计算单元中存在Aux Scalar单元，在前述[《抽象硬件架构》](../抽象硬件架构.md)文档中看到在AI Core内也存在Scalar计算单元（以下称为Main Scalar），这两个Scalar单元是相互独立的，且能力存在差异。在编程界面上为了区分两者执行域，采用VF函数进行隔离。VF函数内的标量计算执行在Aux Scalar单元；VF函数外的标量计算执行在Main Scalar单元。同理，在VF函数内调用的子函数内的标量计算必须满足Aux Scalar的能力；在VF函数外调用的子函数的标量计算必须符合Main Scalar能力。
>
> 为了区分上述三类函数的不同功能执行域，VF函数使用`__simd_vf__`进行标识，VF函数内子函数采用`__simd_callee__`标识，VF函数外子函数采用`__aicore__`标识。在编译期可以根据不同的函数标识完成执行域校验。
>
> Reg矢量API均采用`__simd_callee__`标识，只能在VF函数内被调用。

这种执行域隔离带来三方面好处：

- **编译期校验**：跨域错误调用（例如在`__aicore__`中直接调用Reg矢量API）会在编译阶段被发现。
- **职责清晰**：`__aicore__`负责GM数据搬运、地址计算等外层逻辑；`__simd_vf__`专注寄存器级计算。
- **优化边界明确**：编译器可以针对VF函数做独立优化（如VF融合），且不影响外层算子结构。

**图4** 函数标签调用关系

<img src="../../../../figures/reg_calculation_call_hierarchy.png" title="调用层级" style="zoom: 28%;" />

一个完整的实现由外层`__aicore__`逻辑和内层`__simd_vf__`逻辑两部分组成：

- **外层`__aicore__`代码**：完成核外GM数据交互等准备工作，并通过`asc_vf_call`调用VF函数。
- **内层`__simd_vf__`代码**：完成UB与寄存器之间的搬运以及寄存器上的矢量计算。



调用VF函数的典型骨架如下：

```cpp
__aicore__ inline void compute()
{
    // 1. Prepare input/output data.
    // ...

    // 2. Enter register-level VF function domain via asc_vf_call.
    asc_vf_call<add_vf>(dst_addr, src0_addr, src1_addr,
                        one_repeat_size, repeat_times);

    // 3. Finalize work.
    // ...
}
```

VF函数内部通常遵循如下骨架：

```text
for (i = 0; i < repeat_times; ++i) {
    1. 计算本轮UB地址偏移。
    2. 从UB Load到寄存器。
    3. 在寄存器上执行计算。
    4. 将结果Store回UB。
}
```

### 寄存器类型与申请

寄存器根据功能类型主要分以下五类，在实际编程代码中，寄存器作为变量直接使用，由编译器自动分配到硬件寄存器。

| 寄存器类型 | 变量类型定义 | [NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md) |
| --- | --- | --- |
| 矢量数据寄存器 | `vector_*`，例如vector_float | VL = 256B |
| 掩码寄存器 | `vector_bool` | VL/8 = 32B |
| 地址寄存器 | `addr_reg` | 32bits = 8B |
| 搬入非对齐寄存器 | `vector_load_unalign` | DataBlock = 32B |
| 搬出非对齐寄存器 | `vector_store_unalign` | DataBlock = 32B |

#### 矢量数据寄存器

在[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)中，矢量数据寄存器宽度为VL = 256B，按数据类型解释其中元素数量。


| 位宽      | 矢量数据类型| 元素数 |
| --------- | --------- | --------- |
| b8        | vector_hifloat8_t/vector_fp8_e4m3fn_t/vector_fp8_e5m2_t/vector_fp8_e8m0_t/vector_fp4x2_e1m2_t | 256 |
| b8 | vector_int8_t/vector_uint8_t/vector_int4x2_t/vector_fp4x2_e2m1_t | 256 |
| b16       | vector_half/vector_bfloat16_t/vector_int16_t/vector_uint16_t | 128 |
| b32       | vector_float/vector_int32_t/vector_uint32_t | 64 |
| b64       | vector_int64_t/vector_uint64_t | 32 |

在代码中矢量数据寄存器使用的方式如下：


```cpp
// Allocate vector data register with float data type
vector_float vector_reg;

// Allocate vector data register with half data type
vector_half vector_reg;

// Load data from UB address src_addr to vector_float src0_reg
asc_load(src0_reg, src_addr);

// Compute addition with vector_float src0_reg, src1_reg, dst_reg
asc_add(dst_reg, src0_reg, src1_reg, mask_reg);
```



#### 掩码寄存器

`vector_bool`宽度为`VL/8`。最小对应关系为1bit掩码对应1Byte数据；当数据位宽大于1Byte时，由每个N bytes元素（例如float是4Bytes）对应Nbits位置中的LSB（最低位）bit生效。如下表格中对应矢量数据不同元素宽度时，元素生效索引对应`vector_bool`中bit关系。例如：b32的数据对应`vector_bool`的第0/4/8/12/... bit生效，对应矢量数据寄存器中的b32的第0/1/2/3/..个元素。

| RegTensor单元素宽度 | 0th bit | 1st bit | 2nd bit | 3rd bit | 4th bit | 5th bit | 6th bit | 7th bit | 8th bit | 9th bit | 10th bit | 11th bit | 12th bit | ...  |
| ------------------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | -------- | -------- | -------- | ---- |
| b8                  | 0       | 1       | 2       | 3       | 4       | 5       | 6       | 7       | 8       | 9       | 10       | 11       | 12       | ...  |
| b16                 | 0       | -       | 1       | -       | 2       | -       | 3       | -       | 4       | -       | 5        | -        | 6        | ...  |
| b32                 | 0       | -       | -       | -       | 1       | -       | -       | -       | 2       | -       | -        | -        | 3        | ...  |
| b64                 | 0       | -       | -       | -       | -       | -       | -       | -       | -       | 1       | -        | -        | -        | ...  |



`vector_bool`既可作为计算接口的掩码输入，也可作为bit级元素承载比较类运算的结果和参与移位、逻辑、交织等计算，还可以作为搬入/搬出接口的目的/源参数。

`vector_bool`的创建以及常见的使用方式如下：

```cpp
// Directly allocate mask register
vector_bool mask_reg;

// Create mask register with all elements valid
vector_bool mask_reg = asc_create_mask_b32(PAT_ALL);

// Create mask register based on remaining elements update
vector_bool mask = asc_update_mask_b32(remainCount);

// Use vector_bool mask_reg as mask to control valid elements in computation
asc_add(dst_reg, src0_reg, src1_reg, mask_reg);
```



当用作1bit元素参与运算时，支持vector_* 和vector_bool的混合计算以及vector_bool和vector_bool的计算

```cpp
// Use vector_bool dst_mask_reg as x computation parameter with vector_* src0_vector_reg
asc_select(dst_mask_reg, src0_vector_reg, src1_vector_reg, mask_reg);

// vector_bool src0_mask_reg, src1_mask_reg, dst_mask_reg作为参数计算
asc_and(dst_mask_reg, src0_mask_reg, src1_mask_reg, mask_reg);
```

#### 地址迭代寄存器

`addr_reg`用于保存UB访问的地址偏移量。它把原本需要在循环体内反复执行的标量地址计算转化为寄存器化的地址管理，最多支持四维偏移叠加。`AddrReg`通过`asc_update_addr_reg_b*`初始化，偏移量计算公式为：`offset = index0*stride0 + index1*stride1 + ...`。

```cpp
// Allocate address iteration register
addr_reg a_reg;
// Use as 2D offset accumulation for b32 elements
for(uint16_t i = 0;i < extent1; i++){
    for(uint16_t j = 0;j < extent2; j++){
        a_reg = asc_update_addr_reg_b32(i, stride0, j, stride1);
        asc_loadalign(src_reg, src_addr, a_reg);
    }
}
```

#### 搬入非对齐寄存器

`vector_load_unalign`用于实现从UB非对齐地址连续搬入到矢量数据寄存器的临时缓存，首先通过`asc_loadunalign_pre`初始化，将包含非对齐的数据载入`vector_load_unalign`，然后调用`asc_loadunalign`搬运数据。具体的使用流程可以参考后续[特性描述](#section_part4)章节中对非对齐搬运的描述。

```
// Allocate load unalign register
vector_load_unalign ureg0;

// 使用非对齐UB地址src_addr,初始化vector_load_unalign
asc_loadunalign_pre(ureg0, src_addr);

// 将非对齐UB地址src_addr开始的数据，搬入矢量数据寄存器src_reg
asc_loadunalign(src_reg, ureg0, src_addr);
```

#### 搬出非对齐寄存器

`vector_store_unalign`用于实现从矢量数据寄存器搬出到UB非对齐地址的临时缓存，首先通过`asc_storeunalign`搬运数据，尾部数据会保留在`vector_store_unalign`，最后调用一次`asc_storeunalign_post`，将`vector_store_unalign`内的尾部数据搬出到UB，具体的使用流程可以参考后续[特性描述](#section_part4)章节中对非对齐搬运的描述。

```cpp
// Allocate store unalign register
vector_store_unalign ureg0;

// 搬出vreg0到非对齐UB地址dst_addr
asc_storeunalign(dst_addr, vreg0, ureg0, stride);

// Store tail data from store unalign register to UB
asc_storeunalign_post(dst_addr, ureg0, 0);
```

### 流水线同步<a name="section_sync"></a>

上文提到Reg矢量执行单元与DMA单元可以并行工作。判断是否需要同步的关键不在于代码顺序，而在于硬件流水之间是否已经建立必要的数据依赖：

| 场景 | 是否需要同步 | 原因 |
| --- | --- | --- |
| 同一寄存器内的写后读 | 不需要 | 寄存器依赖由硬件按指令顺序保证 |
| 不同寄存器访问同一UB地址，存在写后读 | 需要 | 搬运和计算执行单元之间没有自动顺序约束 |
| 不同寄存器访问同一UB地址，存在写后写 | 需要 | 需要保证写入顺序 |
| 地址无重叠、寄存器无依赖 | 不需要 | 不存在数据竞争 |

显式同步通过asc_mem_bar实现：

```cpp
// Insert source Store to destination Load synchronization when store and load UB addresses are the same
asc_store(ub_addr, vreg0);
asc_mem_bar(VST_LD);
asc_load(vreg1, ub_addr);
```

同步的方向包括多个指定的[枚举值](../../../../../api/SIMD-API/C-API/reg/sync_control/asc_mem_bar.md)，用于指定需要同步的两条流水方向。如下图**操作相同UB地址时**，读写依赖需要插入`Load->Store`同步`asc_mem_bar(VLD_ST)` ；写写依赖需要插入Store->Store同步`asc_mem_bar(VST_ST)`  。仅当在读写相同UB地址中间存在寄存器变量依赖计算时，同一数据串行处理，无需插入同步。

**图5** 流水线同步示意

![](../../../../figures/同步流水1.png "流水线同步")

> 📌 **提示**：在`asc_vf_call`调用VF函数后，外层无需关注PIPE_V的Barrier；VF函数退出时，所有内部Reg矢量指令均已执行完成。

### Reg矢量计算接口

Reg矢量计算C API（在源码中以`asc_`为前缀，属于语言扩展C API）是一组**直通硬件指令的语言扩展接口**。

- 每个C API在编译期基本对应一条硬件指令。
- 数据类型直接映射为硬件寄存器宽度，例如`vector_float`对应一个VL宽度的矢量寄存器。
- 接口形态、参数顺序、行为细节都按硬件能力组织的语言扩展层。

主要命名如下：

| 分类     | 典型接口                                                     |
| -------- | ------------------------------------------------------------ |
| 数据搬运 | `asc_load*` / `asc_store*`                                   |
| 矢量计算 | `asc_add` / `asc_sub` / `asc_mul` / `asc_div` / `asc_select` / `asc_reduce_*`等 |
| 同步控制 | `asc_mem_bar`                                                |

### 编程示例

下面是一个compute调用的VF函数示例，实现`dst[i] = src0[i] + src1[i]`：

```cpp
// VF function: complete vector addition in register domain
__simd_vf__ inline void add_vf(__ubuf__ float* dst_addr,
                               __ubuf__ float* src0_addr,
                               __ubuf__ float* src1_addr,
                               uint32_t one_repeat_size,
                               uint16_t repeat_times)
{
    vector_float src0_reg;
    vector_float src1_reg;
    vector_float dst_reg;
    vector_bool  mask = asc_create_mask_b32(MASK_PATTERN_ALL);

    for (uint16_t i = 0; i < repeat_times; ++i) {
        uint32_t offset = i * one_repeat_size;
        asc_loadalign(src0_reg, src0_addr + offset);
        asc_loadalign(src1_reg, src1_addr + offset);
        asc_add(dst_reg, src0_reg, src1_reg, mask);
        asc_storealign(dst_addr + offset, dst_reg, mask);
    }
}

// Caller: prepare LocalTensor and UB address in __aicore__ domain
__aicore__ inline void compute(__ubuf__ float* dst_addr,
                               __ubuf__ float* src0_addr,
                               __ubuf__ float* src1_addr,
                               uint32_t count)
{
    constexpr uint32_t one_repeat_size = 64;            // VL / sizeof(float) = 256 / 4
    uint16_t repeat_times = count / one_repeat_size;

    asc_vf_call<add_vf>(dst_addr, src0_addr, src1_addr,
                        one_repeat_size, repeat_times);
}
```

## 特性描述<a name="section_part4"></a>

Reg矢量编程的关键特性按用途分为三大类：

| 分类 | 特性 |
| --- | --- |
| 数据搬运 | 通用搬运、对齐搬运、非对齐搬运、PostUpdate、使用地址寄存器搬运 |
| 矢量计算 | 尾块处理、MaskMergeMode |
| VF函数优化 | VF融合、Hardware Loop、指令双发 |

### 数据搬运

#### 通用搬运

[`asc_load`](../../../../../api/SIMD-API/C-API/reg/reg_load/asc_load.md) / [`asc_store`](../../../../../api/SIMD-API/C-API/reg/reg_store/asc_store.md)是不区分UB地址是否对齐的通用搬运接口，用于将连续VL数据在UB与矢量数据寄存器之间搬运，或将不超过一个VL的元素数据从矢量数据寄存器搬出到UB。

```cpp
// Transfer 1 VL length data from UB address src_addr to vector data register src_reg
asc_load(src_reg, src_addr);

// Transfer 1 VL length data from vector data register to UB address dst_addr
asc_store(dst_addr, dst_reg);

// Transfer less than 1 VL length data (10 elements) from vector data register to UB address dst_addr
asc_store(dst_addr, dst_reg, 10);
```

通用搬运适合需要统一处理对齐与非对齐场景的代码。如果已知地址满足32B对齐要求，并追求更高性能，应优先选择对齐搬运接口。

> 📌 **提示**：通用搬运是软件封装接口，内部实现依赖非对齐搬运的相关接口和寄存器。

#### 对齐搬运

[`asc_loadalign`](../../../../../api/SIMD-API/C-API/reg/reg_load/asc_loadalign.md) / [`asc_storealign`](../../../../../api/SIMD-API/C-API/reg/reg_store/asc_storealign.md)要求UB地址满足32B对齐。相比通用搬运，对齐搬运的性能开销更低，是连续对齐场景下的首选接口。

```cpp
for (uint16_t i = 0; i < repeat_times; ++i) {
    uint32_t offset = i * one_repeat_size;
    asc_loadalign(srcReg, src_addr + offset);
    // Computation on RegTensor
    ...
    asc_storealign(dst_addr + offset, dstReg, mask);
}
```

如果访问模式存在固定步长或多维索引，也可以使用带`block_stride`、`repeat_stride`参数的接口形态。

#### 非对齐搬运

非对齐搬运是指UB地址非对齐，但仍需要在UB和矢量数据寄存器之间连续读写数据的场景。在直接与非对齐UB地址进行数据搬运时，硬件性能会急剧下降，为了提升此场景性能，通过`vector_load_unalign` / `vector_store_unalign`作为临时缓存，保存跨对齐边界的数据，通过多轮循环拼接成对齐地址的读写，减少直接访问非对齐UB地址的次数，从而减少非对齐访问的额外开销。

**非对齐搬入**由[`asc_loadunalign_pre`](../../../../../api/SIMD-API/C-API/reg/reg_load/asc_loadunalign_pre.md)和[`asc_loadunalign`](../../../../../api/SIMD-API/C-API/reg/reg_load/asc_loadunalign.md)组合完成：
例如在[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)，当想要搬入如下UB地址为48开始的256Bytes数据时:

**图6** 非对齐搬入预期

![](../../../../figures/Reg非对齐读1.png "Reg非对齐读1")

1. 先使用非对齐的UB地址初始化`vector_load_unalign`，会将UB地址截断到32B对齐地址32，再将一个DataBlock 32B数据搬入到`vector_load_unalign`.

   **图7**  初始化`vector_load_unalign`

   ![](../../../../figures/Reg非对齐读2.png "Reg非对齐读2")

2. 调用`asc_loadunalign`搬运数据，从原非对齐UB地址向上对齐到32B后，搬入240B数据，与`vector_load_unalign`中的部分数据拼接成预期读入256B数据后，写入矢量数据寄存器。同时将`vector_load_unalign`内的数据更新为下一次读取UB地址非对齐对应的DataBlock 32B.

   **图8**  读入预期数据

   ![](../../../../figures/Reg非对齐读4.png "Reg非对齐读4")

**非对齐搬出**由[`asc_storeunalign`](../../../../../api/SIMD-API/C-API/reg/reg_store/asc_storeunalign.md)和[`asc_storeunalign_post`](../../../../../api/SIMD-API/C-API/reg/reg_store/asc_storeunalign_post.md)组合完成：循环内使用`asc_storeunalign`搬运，循环结束后由`asc_storeunalign_post`将`vector_store_unalign`中的尾部数据搬出到UB。
例如在[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)，当想要搬出如下UB地址为48开始的512Bytes数据时：

**图9** 非对齐搬出预期

![](../../../../figures/Reg非对齐写0.png "Reg非对齐写0")

1. 先使用`asc_storeunalign`，首次循环时：将尾部对齐到32B的长度为240B的矢量数据寄存器数据写入UB，同时将剩余的16B写入`vector_store_unalign`; 

   **图10** 第一次写出预期数据

   ![](../../../../figures/Reg非对齐写1.png "Reg非对齐写1")

2. 当第二次循环时：将暂存在`vector_store_unalign`中的16B数据，与本次循环需要写出的矢量数据寄存器中的240B拼接成256B数据后，写出到UB。同时将本次循环中剩余的16B更新写入`vector_store_unalign`; 

   **图11** 第二次写出预期数据

   ![](../../../../figures/Reg非对齐写2.png "Reg非对齐写2")

3. 调用`asc_storeunalign_post`将`vector_store_unalign`缓存中尚未写出的尾部数据刷回UB.

   **图12** 剩余数据写出

   ![](../../../../figures/Reg非对齐写3.png "Reg非对齐写3")

推荐写法是把初始化和后处理放在循环外，循环体内只保留连续搬运和计算：

```cpp
vector_load_unalign  load_ureg;
vector_store_unalign store_ureg;

asc_loadunalign_pre(load_ureg, src_addr);
for (uint16_t i = 0; i < repeat_times; ++i) {
    asc_loadunalign(src_reg, load_ureg, src_addr, stride);
    // Reg vector computation
    ...
    asc_storeunalign(dst_addr, dst_reg, store_ureg, stride);
}
asc_storeunalign_post(dst_addr, store_ureg, 0);
```

#### PostUpdate模式
PostUpdate表示搬运接口在完成本次访问后自动更新UB地址指针。在搬运接口会以*_postupdate结尾表示。例如：使用对齐搬运的[`asc_loadalign_postupdate`](../../../../../api/SIMD-API/C-API/reg/reg_load/asc_loadunalign_postupdate.md)。当i = 0时：搬入数据地址src_addr，同时在搬运后硬件更新src_addr = src_addr + offset；当i = 1时：搬入数据地址为更新后的src_addr,同时在搬运后硬件再次更新src_addr = src_addr + stride；以此类推。

```cpp
for (uint16_t i = 0; i < repeat_times; ++i) {
    asc_loadalign_postupdate(srcReg, src_addr, stride);
}
```

PostUpdate的优势在于：

- 减少循环内显式计算`src_addr + i * stride`的标量指令数。
- 让地址更新语义与搬运动作绑定，便于编译器识别连续访问模式。

#### 使用地址寄存器搬运

当循环中存在固定stride或多维索引时，可以使用`addr_reg`管理地址偏移，由硬件自动计算偏移减少标量指令消耗。例如两层循环示例如下：

```cpp
addr_reg a_reg;
for (uint16_t i = 0; i < extent1; ++i) {
    for (uint16_t j = 0; j < extent2; ++j) {
        a_reg = asc_update_addr_reg_b32(i, stride0, j, stride1);
        asc_loadalign(src_reg, src_addr, a_reg);
    }
}
```

`addr_reg`最多支持四维寻址，偏移量公式为`offset = index0*stride0 + index1*stride1 + index2*stride2 + index3*stride3`。

### 矢量计算

Reg矢量计算接口直接操作矢量数据寄存器或掩码寄存器，通常带有mask参数控制有效元素。主要分为三类：

#### 矢量数据寄存器计算

基础算术接口完成寄存器之间的逐元素计算：

```cpp
vector_float src0_reg;
vector_float src1_reg;
vector_float dst_reg;
vector_bool mask = asc_create_mask_b32(MASK_PATTERN_ALL);

// 矢量加法
asc_add(dst_reg, src0_reg, src1_reg, mask);

// 矢量乘法
asc_mul(dst_reg, src0_reg, src1_reg, mask);

// 矢量减法
asc_sub(dst_reg, src0_reg, src1_reg, mask);

// 矢量除法
asc_div(dst_reg, src0_reg, src1_reg, mask);
```

标量算术接口完成寄存器与标量的逐元素计算：

```cpp
float scalar_value = 2.0f;

// 矢量加标量
asc_adds(dst_reg, src_reg, scalar_value, mask);

// 矢量乘标量
asc_muls(dst_reg, src_reg, scalar_value, mask);
```

#### 掩码寄存器计算

逻辑接口可操作矢量数据寄存器或掩码寄存器：

```cpp
vector_bool mask0;
vector_bool mask1;
vector_bool dst_mask;

// 逻辑与
asc_and(dst_mask, mask0, mask1, mask);

// 逻辑或
asc_or(dst_mask, mask0, mask1, mask);

// 逻辑非
asc_not(dst_mask, mask0, mask);
```

#### 比较与选择

比较接口输出比较结果，选择接口按mask在两个源操作数之间选择：

```cpp
vector_bool cmp_mask;

// 比较（输出比较结果mask）
asc_compare(cmp_mask, src0_reg, src1_reg, CMPMODE::EQ, mask);

// 选择（按cmp_mask选择src0或src1）
asc_select(dst_reg, cmp_mask, src0_reg, src1_reg, SELMODE::SEL, mask);
```

这些接口往往有更多参数或限制条件，建议在具体使用时查看对应API页面。

#### 尾块处理

当待计算数据总量不是`VL`的整数倍时，最后一轮迭代需要处理不足一个VL的数据。Reg矢量编程通过`asc_update_mask_b*`实现尾块处理：每次调用根据剩余元素数生成当前轮的mask，并在生成后自动减去一个VL对应的元素数。此种写法可以减少循环内通过if判断尾块的操作，避免违反Hardware Loop的约束条件。

```cpp
__simd_vf__ inline void add_vf(__ubuf__ float* dst_addr,
                                  __ubuf__ float* src0_addr,
                                  __ubuf__ float* src1_addr,
                                  uint32_t count,
                                  uint32_t one_repeat_size,
                                  uint16_t repeat_times)
{
    vector_float src0_reg;
    vector_float src1_reg;
    vector_float dst_reg;
    uint32_t remainCount = count;

    for (uint16_t i = 0; i < repeat_times; ++i) {
        vector_bool mask = asc_update_mask_b32(remainCount);
        uint32_t offset = i * one_repeat_size;
        asc_loadalign(src0_reg, src0_addr + offset);
        asc_loadalign(src1_reg, src1_addr + offset);
        asc_add(dst_reg, src0_reg, src1_reg, mask);
        asc_storealign(dst_addr + offset, dst_reg, mask);
    }
}
```

### VF函数优化

#### Hardware Loop

Hardware Loop是VF循环性能优化的基础。VF中的循环如果满足硬件约束，编译器可以将其转换为硬件级循环，减少软件循环中的计数、比较和跳转开销。无法识别为Hardware Loop的循环会退化为Software Loop，性能显著降低，并影响后续VF融合等优化。

> 提示：Hardware Loop主要编码规范如下：
>
> - 循环迭代变量必须是`uint16_t`类型。
> - 循环起始值从0开始，步长为1。
> - 循环边界在执行过程中不被修改。
> - 循环体内避免`if/else`等运行时跳转控制流。
> - VF内最多支持四层级的嵌套循环。

对编译时分支和尾块场景，可使用以下方式替代`if`：

```cpp
// 编译时分支：if constexpr无运行时开销
if constexpr (has_bias) {
    asc_add(dst_reg, src_reg, bias_reg, mask);
}

// 尾块场景：用for(1)替代if
uint16_t tail = count % one_repeat_size;
uint16_t has_tail = !!tail;
for (uint16_t i = 0; i < has_tail; ++i) {
    // 尾块处理内容
}
```

典型Hardware Loop写法：

```cpp
for (uint16_t i = 0; i < repeat_times; ++i) {
    asc_loadalign(src_reg, src_addr + i * one_repeat_size);
    asc_add(dst_reg, src_reg, bias_reg, mask);
    asc_storealign(dst_addr + i * one_repeat_size, dst_reg, mask);
}
```

#### VF融合

VF融合是Reg矢量计算编程最重要的优化手段。其是将多个计算以及其循环进行合并在1个VF函数内，融合后可以减少VF调用和Hardware Loop启动开销，并消除多个VF之间冗余的Load/Store和同步指令，使中间结果尽量保留在寄存器中连续消费。

VF融合可以分成两个层级：

- **VF浅融合**：将多个VF函数实现合并为一个VF函数，可以消除VF的调用开销。
- **VF深融合**：在VF浅融合基础上，将多个计算循环进行融合，减少Hardware Loop启动开销，消除冗余的Load/Store，充分复用寄存器。

为增加VF融合机会，建议遵循以下写法：

- 多个VF保持相同或等价的循环结构增加VF深度融合的机会，循环满足Hardware Loop规范。
- 控制寄存器使用量，避免融合后超过寄存器资源限制。

> 📌 **提示**：VF融合并非范围越大越好。融合粒度过大可能导致寄存器溢出，反而使性能下降。更多优化手段可以参考[VF融合优化](../../../../算子实践参考/SIMD算子性能优化/矢量计算/VF性能优化/VF融合优化.md)和[VF循环优化](../../../../算子实践参考/SIMD算子性能优化/矢量计算/VF性能优化/VF循环优化.md)。

#### 指令双发

指令双发依赖硬件**同时发射和执行多条互不依赖指令**的能力。DMA单元与Reg矢量执行单元在没有数据依赖时可以并行工作，Reg矢量计算单元的部分指令在无依赖时可以并行执行，DMA单元的多次对齐搬入之间、对齐搬入和对齐搬出之间在无地址依赖时可以并行执行；Reg矢量编程的优化目标之一就是为执行队列提供足够多的独立指令，避免长依赖链阻塞硬件并行。

如果计算链路过长，也可以适度拆分VF循环、必要时将阶段性结果写回UB，以换取更好的指令调度空间。同时需要控制VF内寄存器数量：一个VF中可同时使用的`vector_*`、`vector_bool`、`vector_load_unalign`、`vector_store_unalign`数量都受硬件规格限制，超出后编译器会插入换入换出和同步指令，反而降低性能。

>📌 **提示**：优化手段可以参考[指令双发优化](../../../../算子实践参考/SIMD算子性能优化/矢量计算/VF性能优化/指令双发优化.md)。



## 小结<a name="section_part7"></a>

Reg矢量计算编程基于[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)新增的SIMD Register File开放的寄存器级编程能力，使开发者直接组织寄存器搬运、计算、掩码控制和同步操作。它以`__aicore__`和`__simd_vf__`两层执行域为基本结构，其中`__aicore__`侧负责整体任务组织、数据切分和VF调用，`__simd_vf__`侧通过五类寄存器及其配套API完成细粒度矢量计算。

实际开发时，Reg矢量计算不仅要关注接口本身，还需要结合硬件执行特征开发代码。优先使用对齐搬运，保持循环满足Hardware Loop识别规范，并在寄存器资源允许的范围内进行VF融合，通常可以减少搬运、同步和循环启动开销，从而获得更稳定的性能表现。
