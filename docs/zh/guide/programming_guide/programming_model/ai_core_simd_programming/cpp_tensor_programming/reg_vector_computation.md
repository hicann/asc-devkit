# Reg矢量计算编程<a name="ZH-CN_TOPIC_0000002509863892"></a>

本文介绍如何使用基础API（C++模板接口）编写Reg矢量计算代码。基础API与[Reg矢量计算编程（语言扩展C API）](../c_pointer_programming/reg_vector_computation.md)描述的是同一套Reg矢量硬件能力，功能语义一致，主要区别在于API命名、类型表达和可配置参数的表达方式。

Reg矢量编程的硬件原理、内存层级、VF函数执行域、流水线同步、Hardware Loop、VF融合等与API接口命名无关的内容，请参见[Reg矢量计算编程（语言扩展C API）](../c_pointer_programming/reg_vector_computation.md)。本文只说明基础API的接口定位、模板参数、数据类型体系、API分类和典型调用方式。

## 基础API定位<a name="section_part1"></a>

基础API Reg矢量计算接口位于`AscendC::Reg`命名空间，是语言扩展C API的C++模板化封装。基础API并不改变Reg矢量计算的编程模型：仍然需要在`__aicore__`域通过`asc_vf_call`进入`__simd_vf__`函数，并在VF函数内完成`Load` -> `Compute` -> `Store`。

基础API的主要特点如下：

- 使用C++类型表达寄存器对象，例如`RegTensor<T>`、`MaskReg`、`AddrReg`、`UnalignRegForLoad`、`UnalignRegForStore` ,对应关系参考[数据对象与寄存器类型](#section_part4)。
- 使用模板参数表达数据类型、搬运模式、PostUpdate模式等配置。
- 多数接口可通过参数自动推导寄存器类型。
- 搬运接口可以与`LocalTensor`衔接，可以支持不操作UB地址，也可以外层`__aicore__`代码从`LocalTensor::GetPhyAddr()`获取UB物理地址，再传入VF函数内的Reg搬运接口。
- 部分计算接口提供`MaskMergeMode`模板参数，用于控制未被mask选中的元素如何写入目的寄存器。

> 📌 **提示**：
>
> 1. 基础API和C API功能一致。若需要理解“为什么需要Reg矢量编程”“寄存器与UB/GM的关系”“哪些场景需要同步”等模型性内容，请直接阅读C API版本文档。
>
> 2. 搬运接口支持`LocalTensor`参数，在后续版本提供，暂未支持。

## 与C API的对应关系<a name="section_part2"></a>

基础API与C API的核心差异是表达形式不同。

| 维度 | 基础API（C++模板） | C API（语言扩展） |
| --- | --- | --- |
| 命名空间/前缀 | `AscendC::Reg::*` | `asc_*` |
| 命名风格 | `LoadAlign`、`StoreUnAlignPost` | `asc_loadalign`、`asc_storeunalign_post` |
| 矢量数据寄存器 | `RegTensor<T>` | `vector_float`、`vector_half`、`vector_int32_t`等 |
| 掩码寄存器 | `MaskReg` | `vector_bool` |
| 地址寄存器 | `AddrReg` | `addr_reg` |
| 非对齐搬入/搬出寄存器 | `UnalignRegForLoad` / `UnalignRegForStore` | `vector_load_unalign` / `vector_store_unalign` |
| 数据类型选择 | 模板参数`T`，通常可由参数推导 | 类型名或接口后缀，例如`_b8`、`_b16`、`_b32` |
| PostUpdate | `PostLiteral`模板参数 | 接口名后缀，例如`_postupdate` |
| Mask合并模式 | `MaskMergeMode`模板参数 | 暂不支持 |

常见API映射如下：

| 功能 | 基础API | C API |
| --- | --- | --- |
| 创建全量mask | `CreateMask<T, MaskPattern::ALL>()` | `asc_create_mask_b*()` |
| 更新尾块mask | `UpdateMask<T>(count)` | `asc_update_mask_b*()` |
| 创建地址寄存器 | `CreateAddrReg<T>(...)` | `asc_update_addr_reg_b*()` |
| 通用搬入/搬出 | `Load` / `Store` | `asc_load` / `asc_store` |
| 对齐搬入/搬出 | `LoadAlign` / `StoreAlign` | `asc_loadalign` / `asc_storealign` |
| 非对齐搬入 | `LoadUnAlignPre` + `LoadUnAlign` | `asc_loadunalign_pre` + `asc_loadunalign` |
| 非对齐搬出 | `StoreUnAlign` + `StoreUnAlignPost` | `asc_storeunalign` + `asc_storeunalign_post` |
| 流水线同步 | `LocalMemBar<src, dst>()` | `asc_mem_bar(...)` |

## 模板参数体系<a name="section_part3"></a>

基础API把C API中“后缀、枚举、特殊接口名”表达的能力统一收敛到模板参数中。常见模板参数如下。

| 模板参数 | 作用 | 典型取值/类型 | 常见接口 |
| --- | --- | --- | --- |
| `T` | 数据类型 | `float`、`half`、`int32_t`等 | `RegTensor<T>`、`LoadAlign<T>`、`Add<T>` |
| `PostLiteral` | 搬运后是否更新UB地址 | `POST_MODE_NORMAL`、`POST_MODE_UPDATE` | `LoadAlign`、`StoreAlign`、`LoadUnAlign`、`StoreUnAlign` |
| `LoadDist` | 搬入模式 | `DIST_NORM`、broadcast类取值等 | `LoadAlign` |
| `StoreDist` | 搬出模式 | 正常搬出、首元素搬出、pack类取值等 | `StoreAlign` |
| `MaskMergeMode` | mask未选中元素处理方式 | `ZEROING`、`MERGING` | 部分计算接口，例如Add |
| 计算模式枚举 | 指定算法或计算变体 | 例如`ReduceType`、`PairReduce`等 | 规约、复合计算等 |

### 数据类型模板参数

基础API使用`RegTensor<T>`表达矢量数据寄存器。`T`决定寄存器内元素解释方式，寄存器宽度仍由硬件VL决定。例如在VL为256B时，`RegTensor<float>`可表示64个`float`元素，`RegTensor<half>`可表示128个`half`元素。

```cpp
AscendC::Reg::RegTensor<float> srcReg;
AscendC::Reg::RegTensor<float> dstReg;

AscendC::Reg::LoadAlign(srcReg, srcAddr);
AscendC::Reg::Add(dstReg, srcReg, srcReg, mask);
```

当接口参数已经包含`RegTensor<T>`时，模板参数`T`通常可由编译器推导；当使用特定模式、转换接口或需要消除歧义时，可以显式填写。

### 搬运模式模板参数

基础API使用`LoadDist`、`StoreDist`、`PostLiteral`等模板参数表达搬运模式。

```cpp
// 对齐搬入，使用LoadDist模板参数
AscendC::Reg::LoadAlign<float, AscendC::Reg::LoadDist::DIST_BRC_B32>(srcReg, srcAddr);

// PostUpdate对齐搬入，接口执行后更新srcAddr。
AscendC::Reg::LoadAlign<float, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(srcReg, srcAddr, stride);

// 对齐搬出，使用StoreDist模板参数
AscendC::Reg::StoreAlign<float, AscendC::Reg::StoreDist::DIST_FIRST_ELEMENT_B32>(dstAddr, dstReg, mask);
```

### MaskMergeMode模板参数

部分Reg计算接口提供`MaskMergeMode`模板参数，用于描述mask未选中元素在目的寄存器中的处理方式：

- `MaskMergeMode::ZEROING`：mask未选中的元素在`dstReg`中置零。
- `MaskMergeMode::MERGING`：mask未选中的元素保留`dstReg`原值。

```cpp
AscendC::Reg::Add<T, AscendC::Reg::MaskMergeMode::ZEROING>(dstReg, src0Reg, src1Reg, mask);

AscendC::Reg::Add<T, AscendC::Reg::MaskMergeMode::MERGING>(dstReg, src0Reg, src1Reg, mask);
```

> 📌 **提示**：`MaskMergeMode`只有部分接口形态支持，由软件仿真实现。实际使用时应以单个API文档中的函数原型和参数说明为准。对于不需要保留未选中元素旧值的场景，使用默认的`ZEROING`语义。

## 数据对象与寄存器类型<a name="section_part4"></a>

基础API使用C++类型表达Reg矢量编程中的寄存器资源。

| 寄存器分类 | 基础API类型 | 用途 |
| --- | --- | --- |
| 矢量数据寄存器 | `RegTensor<T, regTrait>` | 保存VL或多VL长度的矢量数据，是计算接口的主要源/目的操作数 |
| 掩码寄存器 | `MaskReg` | 控制哪些元素参与计算，也可承载比较、逻辑等结果 |
| 地址寄存器 | `AddrReg` | 保存UB访问偏移，适合固定stride、多维索引等搬运场景 |
| 搬入非对齐寄存器 | `UnalignRegForLoad` | 连续非对齐搬入的临时缓存 |
| 搬出非对齐寄存器 | `UnalignRegForStore` | 连续非对齐搬出的临时缓存 |

`RegTensor`和`MaskReg`等寄存器对象只能作为整体使用，不能按C++数组方式访问其中某个元素。相关硬件含义、宽度、mask bit对应关系请参考[Reg矢量计算编程（语言扩展C API）](../c_pointer_programming/reg_vector_computation.md)中“寄存器类型与申请”章节。

### 与LocalTensor衔接

Reg矢量搬运接口实际访问的是UB地址。基于Tensor的Cpp编程中，外层通常使用`LocalTensor`管理UB数据。

```cpp
template <typename T>
__aicore__ inline void Compute()
{
    AscendC::LocalTensor<T> dst = ubAllocator.Alloc<T, TILE_LENGTH>();
    // 参数计算等
    ......
    asc_vf_call<AddVF<T>>(dst, src0, src1, oneRepeatSize, repeatTimes);
    // 尾处理
    ......
}
```

这种写法使基础API能够与`__aicore__`中`LocalTensor`流程衔接。

> 📌 **提示**：使用LocalTensor作为VF函数参数时，不能使用引用LocalTensor &。因为在`__aicore__`和`__simd_vf__`是两个相互隔离的执行域，无法通过跨执行域通过引用访问。

## 特性描述<a name="section_part5"></a>

### 寄存器构造类

#### Mask创建

[`CreateMask`](../../../../../api/SIMD-API/basic_api/reg_vector_compute/register_data_types/MaskReg.md)用于创建固定模式的`MaskReg`，[`UpdateMask`](../../../../../api/SIMD-API/basic_api/reg_vector_compute/register_data_types/MaskReg.md)用于根据剩余元素数生成尾块mask。

```cpp
AscendC::Reg::MaskReg fullMask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();

AscendC::Reg::MaskReg mask = AscendC::Reg::UpdateMask<T>(remainCount);
```

`UpdateMask<T>(remainCount)`适合放在VF循环中进行尾块处理。其语义与C API中的`asc_update_mask_b*`一致。

#### 地址寄存器创建

[`CreateAddrReg`](../../../../../api/SIMD-API/basic_api/reg_vector_compute/register_data_types/AddrReg.md)用于创建保存UB偏移量的`AddrReg`，最多支持四层循环跳转。

```cpp
AscendC::Reg::AddrReg aReg;
for(uint16_t i = 0;i < extent1; i++){
    for(uint16_t j = 0;j < extent2; j++){
        for(uint16_t k = 0;k < extent3; k++){
            for(uint16_t m = 0;m < extent4; m++){
                aReg = AscendC::Reg::CreateAddrReg(i, const1, j, const2, k, const3, m, const4);
                AscendC::Reg::LoadAlign(srcReg, srcAddr, aReg);
            }
        }
    }
}
```

### 数据搬运类

#### 通用搬运

`Load` / `Store`不要求用户区分UB地址是否32B对齐，适合希望统一处理对齐和非对齐地址的场景。

```cpp
AscendC::Reg::Load(srcReg, srcAddr);
AscendC::Reg::Store(dstAddr, dstReg);
AscendC::Reg::Store(dstAddr, dstReg, count);
```

通用搬运是软件封装，性能敏感且地址满足对齐条件时，优先使用对齐搬运。

#### 对齐搬运

[`LoadAlign`](../../../../../api/SIMD-API/basic_api/reg_vector_compute/reg_data_load/LoadAlign_continuous.md) / [`StoreAlign`](../../../../../api/SIMD-API/basic_api/reg_vector_compute/reg_data_store/StoreAlign_continuous.md)适用于UB地址满足对齐要求的连续搬入/搬出，是Reg矢量编程中最常见的搬运接口。在搬运时可以按照内置的功能随路进行数据排布等操作，具体功能以对应API章节为准。

```cpp
AscendC::Reg::LoadAlign(srcReg, srcAddr);
AscendC::Reg::StoreAlign(dstAddr, dstReg, mask);
```

#### 非对齐搬运

连续非对齐搬入由[`LoadUnAlignPre`](../../../../../api/SIMD-API/basic_api/reg_vector_compute/reg_data_load/LoadUnAlign_continuous.md)和[`LoadUnAlign`](../../../../../api/SIMD-API/basic_api/reg_vector_compute/reg_data_load/LoadUnAlign_continuous.md)组合使用；连续非对齐搬出由[`StoreUnAlign`](../../../../../api/SIMD-API/basic_api/reg_vector_compute/reg_data_store/StoreUnAlign_continuous.md)和[`StoreUnAlignPost`](../../../../../api/SIMD-API/basic_api/reg_vector_compute/reg_data_store/StoreUnAlign_continuous.md)组合使用。

```cpp
AscendC::Reg::UnalignRegForLoad loadUreg;
AscendC::Reg::UnalignRegForStore storeUreg;

AscendC::Reg::LoadUnAlignPre(loadUreg, srcAddr);
for (uint16_t i = 0; i < repeatTimes; ++i) {
    AscendC::Reg::LoadUnAlign(srcReg, loadUreg, srcAddr, postUpdateStride);
    // Reg计算
    ...
    AscendC::Reg::StoreUnAlign(dstAddr, dstReg, storeUreg, postUpdateStride);
}
AscendC::Reg::StoreUnAlignPost(dstAddr, storeUreg, 0);
```

### 矢量计算类

矢量计算接口直接操作`RegTensor<T>`或`MaskReg`，通常带有`MaskReg mask`控制有效元素。

####  `RegTensor<T>`与`RegTensor<T>`计算

基础算术接口完成寄存器之间的逐元素计算，标量算术接口完成寄存器与标量的逐元素计算。

```cpp
AscendC::Reg::Add(dstReg, src0Reg, src1Reg, mask);
AscendC::Reg::Mul(dstReg, src0Reg, src1Reg, mask);
AscendC::Reg::Adds(dstReg, srcReg, scalarValue, mask);
AscendC::Reg::Muls(dstReg, srcReg, scalarValue, mask);
```

####  `MaskReg`计算

逻辑接口可操作`RegTensor`或`MaskReg`；比较接口通常输出比较结果，选择接口按mask在两个源操作数之间选择。

```cpp
AscendC::Reg::And(dstMaskReg, src0MaskReg, src1MaskReg, mask);
AscendC::Reg::Compare(cmpMaskReg, src0Reg, src1Reg, mask);
AscendC::Reg::Select(dstReg, src0Reg, src1Reg, cmpMaskReg, mask);
```

这些接口往往有更多模板参数或限制条件，建议在具体使用时查看对应API页面。

### 同步控制类

基础API使用[`LocalMemBar`](../../../../../api/SIMD-API/basic_api/reg_vector_compute/sync_control/LocalMemBar.md)表达VF内部对UB访问流水线的同步约束。

```cpp
AscendC::Reg::Store(dstAddr, dstReg);
AscendC::Reg::LocalMemBar<AscendC::Reg::MemType::VEC_STORE,
                          AscendC::Reg::MemType::VEC_LOAD>();
AscendC::Reg::Load(srcReg, dstAddr);
```

何时需要同步与API命名无关，判断规则请参考[Reg矢量计算编程（语言扩展C API）](../c_pointer_programming/reg_vector_computation.md)中的“流水线同步”章节。

## 编程示例<a name="section_part6"></a>

下面示例展示基础API的典型调用方式。外层`__aicore__`函数仍负责`LocalTensor`管理，VF函数内使用Reg基础API完成搬入、计算和搬出。

```cpp
template <typename T>
__simd_vf__ inline void AddVFv1(AscendC::LocalTensor<T> dst,
                              AscendC::LocalTensor<T> src0,
                              AscendC::LocalTensor<T> src1,
                              uint32_t oneRepeatSize,
                              uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> src0Reg;
    AscendC::Reg::RegTensor<T> src1Reg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask =
        AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();

    for (uint16_t i = 0; i < repeatTimes; ++i) {
        uint32_t offset = i * oneRepeatSize;
        AscendC::Reg::LoadAlign(src0Reg, src0 + offset);
        AscendC::Reg::LoadAlign(src1Reg, src1 + offset);
        AscendC::Reg::Add(dstReg, src0Reg, src1Reg, mask);
        AscendC::Reg::StoreAlign(dst + offset, dstReg, mask);
    }
}

template <typename T>
__simd_vf__ inline void AddVFv2(__ubuf__ float* dstAddr,
                                __ubuf__ float* src0Addr,
                                __ubuf__ float* src1Addr,
                                uint32_t oneRepeatSize,
                                uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> src0Reg;
    AscendC::Reg::RegTensor<T> src1Reg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask =
        AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();

    for (uint16_t i = 0; i < repeatTimes; ++i) {
        uint32_t offset = i * oneRepeatSize;
        AscendC::Reg::LoadAlign(src0Reg, src0Addr + offset);
        AscendC::Reg::LoadAlign(src1Reg, src1Addr + offset);
        AscendC::Reg::Add(dstReg, src0Reg, src1Reg, mask);
        AscendC::Reg::StoreAlign(dstAddr + offset, dstReg, mask);
    }
}

template <typename T>
__aicore__ inline void Compute()
{
    AscendC::LocalTensor<T> dst = ubAllocator.Alloc<T, TILE_LENGTH>();

    constexpr uint32_t oneRepeatSize = 256 / sizeof(T);
    uint16_t repeatTimes = TILE_LENGTH / oneRepeatSize;
	// 使用LocalTensor作为参数
    asc_vf_call<AddVFv1<T>>(dst, src0, src1, oneRepeatSize, repeatTimes);
    
    __ubuf__ T* dstAddr = (__ubuf__ T*)dst.GetPhyAddr();
    __ubuf__ T* src0Addr = (__ubuf__ T*)src0.GetPhyAddr();
    __ubuf__ T* src1Addr = (__ubuf__ T*)src1.GetPhyAddr();
    // 使用UB地址作为参数
    asc_vf_call<AddVFv2<T>>(dstAddr, src0Addr, src1Addr, oneRepeatSize, repeatTimes);
}
```

如果需要处理尾块，可以在VF循环中使用`UpdateMask<T>`：

```cpp
uint32_t remainCount = count;
for (uint16_t i = 0; i < repeatTimes; ++i) {
    AscendC::Reg::MaskReg mask = AscendC::Reg::UpdateMask<T>(remainCount);
    // Load / Compute / Store
}
```

## 小结<a name="section_part7"></a>

基础API与C API描述的是同一套Reg矢量计算能力，两者的编程模型和硬件语义保持一致。基础API的差异主要体现在接口表达方式上：它通过`AscendC::Reg::*`命名空间组织接口，使用C++寄存器类型描述VF内部的数据对象，并通过模板参数承载数据类型、搬运模式、计算模式等配置。与`LocalTensor`衔接时，可以在`__aicore__`函数中直接将`LocalTensor`作为参数传入VF函数，也可以通过`LocalTensor::GetPhyAddr()`取得UB地址后传入VF函数，再在VF函数内部使用Reg搬运接口完成访问。
