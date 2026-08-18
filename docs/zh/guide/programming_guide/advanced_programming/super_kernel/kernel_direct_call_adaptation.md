# 核函数（Kernel）直调算子额外适配说明

对于使用`<<<>>>`方式开发的Ascend C算子，除了遵循[算子适配说明](operator_adaptation.md)中的通用约束外，还需要在算子核函数（Kernel）入口侧增加SuperKernel入口函数。本文档介绍具体的适配方法。

>[!NOTE]说明
>核函数（Kernel）直调算子目前仅支持在npugraph_ex后端进入SuperKernel，不支持GE图模式。

## SuperKernel与普通核函数（Kernel）的区别

### 普通核函数（Kernel）入口函数

- 函数签名：`__global__ __vector__ void func(参数列表)`
- 参数直接作为函数参数传递。
- 直接调用即可。

### SuperKernel入口函数

- 函数签名：`__sk__ __vector__ void func(const ArgsStruct *args, sk::SkSystemArgs *sysArgs)`
- 参数通过结构体指针传递，需要用户定义参数结构体并封装入参。
- 可选添加`sk::SkSystemArgs *sysArgs`参数，用于获取系统信息（如block num）。
- 需要与普通核函数（Kernel）通过SK_BIND宏绑定。

## SuperKernel适配规则

### SK子函数编写规则

#### 规则1：函数签名格式

```cpp
__sk__ __vector__ void function_name_sk(const ArgsStruct *args, sk::SkSystemArgs *sysArgs)
```

> **注意**：只有`__sk__`是强约束，必须出现在函数签名中。

#### 规则2：Kernel Type标识与原global函数保持一致

- 如果原global函数使用`__cube__`，则SK子函数也必须使用`__cube__`。
- 如果原global函数使用`__vector__`，则SK子函数也必须使用`__vector__`。
- 如果原global函数是`__mix__(c, v)`，则SK子函数也必须使用`__mix__(c, v)`。

**示例**：

```cpp
// 原global函数为cube类型
__global__ __cube__ void func(GM_ADDR x, GM_ADDR y);
// SK子函数也需要cube标识
__sk__ __cube__ void func_sk(const ArgsStruct *args, sk::SkSystemArgs *sysArgs);

SK_BIND(func, 4, func_sk);

// 原global函数为vector类型
__global__ __vector__ void func_vec(GM_ADDR x, GM_ADDR y);
// SK子函数也需要vector标识
__sk__ __vector__ void func_vec_sk(const ArgsStruct *args, sk::SkSystemArgs *sysArgs);

SK_BIND(func_vec, 4, func_vec_sk);

// 原global函数为mix类型
__global__ __mix__(1, 2) void func_mix(GM_ADDR x, GM_ADDR y);
// SK子函数需要mix标识
__sk__ __mix__(1, 2) void func_mix_sk(const ArgsStruct *args, sk::SkSystemArgs *sysArgs);

SK_BIND(func_mix, 4, func_mix_sk);

// 原global函数为mix(1, 0)类型
__global__ __mix__(1, 0) void func_mix_10(GM_ADDR x, GM_ADDR y);
// SK子函数需要cube标识
__sk__ __mix__(1, 0) void func_mix_10_sk(const ArgsStruct *args, sk::SkSystemArgs *sysArgs);

SK_BIND(func_mix_10, 4, func_mix_10_sk);

// 原global函数为mix(0, 1)类型
__global__ __mix__(0, 1) void func_mix_01(GM_ADDR x, GM_ADDR y);
// SK子函数需要vector标识
__sk__ __mix__(0, 1) void func_mix_01_sk(const ArgsStruct *args, sk::SkSystemArgs *sysArgs);

SK_BIND(func_mix_01, 4, func_mix_01_sk);
```

#### 规则3：定义参数结构体

用户需要根据原global函数的实际参数定义一个结构体来封装所有入参：

```cpp
// 示例：假设原global函数为
// __global__ __vector__ void my_kernel(GM_ADDR x, GM_ADDR y, uint32_t length, int16_t flag)
// 则对应的参数结构体定义如下
struct MyArgs {
    GM_ADDR x;                      // 第一个参数
    GM_ADDR y;                      // 第二个参数
    uint32_t length;                // 第三个参数
    alignas(4) int16_t flag;        // 第四个参数（小于4字节，需使用alignas(4)）
};
```

> **注意**：
> - 结构体必须根据用户实际算子核函数（Kernel）（global函数）的参数进行定义，不要直接复制示例中的字段名。
> - 结构体字段必须按原函数参数顺序排列。
> - 对于size小于4字节的类型（如`int16_t`、`int8_t`、`uint16_t`、`uint8_t`），必须使用`alignas(4)`确保内存对齐（ABI要求）。
> - 示例：`alignas(4) int16_t val;`、`alignas(4) uint8_t flag;`

#### 规则4：可选添加SkSystemArgs参数

如果原算子使用了`AscendC::GetBlockNum()`接口，在SK场景下可以通过添加`sk::SkSystemArgs *sysArgs`参数来获取期望的block num：

```cpp
// 场景一：算子使用了AscendC::GetBlockNum()，需要添加sysArgs参数
template<uint32_t splitNum>
__sk__ __vector__ void func_sk(const ArgsStruct *args, sk::SkSystemArgs *sysArgs)
{
    // 从系统参数获取block num（替代AscendC::GetBlockNum()）
    uint32_t blockNum = sysArgs->skNumBlocks;
    // 或者使用接口获取
    uint32_t blockNum = sysArgs->SkGetNumBlocks();
    // 后续使用blockNum的代码
    ...
}

// 场景二：算子未使用AscendC::GetBlockNum()，无需添加sysArgs参数
template<uint32_t splitNum>
__sk__ __vector__ void func_sk(const ArgsStruct *args)
{
    // 直接使用参数进行计算，无需访问系统信息
    GM_ADDR x = args->x;
    GM_ADDR y = args->y;
    // 后续计算代码
    ...
}
```

> **注意**：
> - `sk::SkSystemArgs *sysArgs`参数是可选的，如果算子未使用block num信息，或者`sk::SkSystemArgs`提供的功能（当前仅支持获取block num），则无需在函数入口传入`sysArgs`参数，这样可以避免不必要的代码修改。
> - 用户需要根据实际情况修改原代码中的`AscendC::GetBlockNum()`调用：
>   - 直接使用`sysArgs->skNumBlocks`访问成员变量。
>   - 或者使用`sysArgs->SkGetNumBlocks()`接口获取。
> - 与`AscendC::GetBlockNum()`的逻辑差异是，MIX 1:2场景中，vector核中的`skBlockNum`为`blockNum * 2`。

#### 规则5：使用TPipe场景下，需要手动调用DestroyWithoutPipeAll接口

为了优化SK函数之间的同步，若使用了TPipe对象，需要在其自然析构之前调用`DestroyWithoutPipeAll()`接口主动销毁TPipe（不调用PipeBarrierAll）。

```cpp
template<uint32_t splitNum>
__sk__ __vector__ void func_sk(const ArgsStruct *args, sk::SkSystemArgs *sysArgs)
{
    ...
    // 使用TPipe场景
    TPipe pipe;
    ...
    // 手动调用接口销毁TPipe，不依赖默认析构函数
    pipe.DestroyWithoutPipeAll();
}
```

#### 规则6：逻辑与global函数一致

SK子函数内部的逻辑实现必须与原global函数完全一致，包括数据初始化、计算处理、结果输出等所有后续逻辑。确保SK子函数在功能上与原global函数完全等价。

### SK_BIND绑定规则

#### 规则7：使用SK_BIND宏绑定

```cpp
SK_BIND(original_func, 4, sk_func<0>, sk_func<1>, sk_func<2>, sk_func<3>);
```

**参数说明**：

| 参数 | 说明 |
| --- | --- |
| `original_func` | 原global函数，如果是模板函数需要手动实例化 |
| `4` | 掩码参数（uint64_t类型），用于配置SuperKernel特性 |
| `sk_func<0>` ~ `sk_func<3>` | 最多4个SK子函数，如果是模板函数需要手动实例化 |

**掩码参数取值**：
1/2/4/8分别代表不同bit位上置值的结果，比如8为第4bit置值的结果

| 值 | 含义 |
| --- | --- |
| 1 | early start wait flag |
| 2 | early start set flag |
| 4 | disable_dcci |
| 8 | disable_batchmode_check |

> **注意**：
> - SK子函数使用模板参数实例化出多个符号（推荐，代码更简洁）。
> - 模板参数仅用于实例化出不同的符号，不影响函数逻辑。

## 完整适配示例

### 原始普通核函数（Kernel）

```cpp
// 普通核函数（Kernel）定义
__global__ __vector__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, uint32_t totalLength)
{
    KernelAdd op;
    op.Init(x, y, z, totalLength);
    op.Process();
}
```

### SuperKernel适配版本

```cpp
// 原普通核函数（Kernel）保留（用于非SuperKernel场景）
__global__ __vector__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, uint32_t totalLength)
{
    KernelAdd op;
    op.Init(x, y, z, totalLength);
    op.Process();
}

// 规则3：定义参数结构体（根据原global函数的实际参数定义）
struct AddCustomArgs {
    GM_ADDR x;                      // 对应add_custom的第一个参数
    GM_ADDR y;                      // 对应add_custom的第二个参数
    GM_ADDR z;                      // 对应add_custom的第三个参数
    uint32_t totalLength;           // 对应add_custom的第四个参数
};

// 定义一个带模板参数的SK子函数
// 模板参数仅用于实例化出不同的符号，不影响函数逻辑
template<uint32_t splitNum>
__sk__ __vector__ void add_custom_sk(const AddCustomArgs *args, sk::SkSystemArgs *sysArgs /* 可选添加sysArgs参数 */)
{
    // 从结构体中获取参数
    GM_ADDR x = args->x;
    GM_ADDR y = args->y;
    GM_ADDR z = args->z;
    uint32_t totalLength = args->totalLength;
    // 规则6：逻辑与global函数一致
    KernelAdd op;
    op.Init(x, y, z, totalLength);
    op.Process();
}

// 规则7：使用SK_BIND绑定
// 通过指定模板参数实例化出4个不同的符号
SK_BIND(add_custom, 4, add_custom_sk<0>, add_custom_sk<1>, add_custom_sk<2>, add_custom_sk<3>);
```

## 适配步骤总结

**步骤1：保留原global函数**

原有的`__global__ __vector__`函数保持不变，继续用于非SuperKernel场景。

**步骤2：创建SK子函数**

1. 复制原global函数的代码
2. 根据原global函数的实际参数定义参数结构体（对于小于4字节的类型使用`alignas(4)`）
3. 修改函数签名为`__sk__ __vector__ void func_sk(const ArgsStruct *args, sk::SkSystemArgs *sysArgs)`（根据原函数的Kernel Type添加对应标识）
4. 如果算子未使用`AscendC::GetBlockNum()`，可以省略`sk::SkSystemArgs *sysArgs`参数
5. 使用模板参数实例化出多个符号（模板参数仅用于实例化出不同的符号，不影响函数逻辑）
6. 从结构体中获取参数
7. 如果使用了`AscendC::GetBlockNum()`，修改为使用`sysArgs->skNumBlocks`或`sysArgs->SkGetNumBlocks()`
8. 使用TPipe场景下，需要手动调用`DestroyWithoutPipeAll`接口销毁TPipe
9. 保持函数内部逻辑与原global函数完全一致

**步骤3：添加SK_BIND绑定**

1. 使用`SK_BIND`宏将原global函数与SK子函数绑定
2. 支持最多4个SK子函数
3. 第二个参数是掩码（uint64_t类型），用于配置SuperKernel特性：
   - `8`表示disable batchmode check
   - `4`表示disable dcci
   - `2`表示early start set flag
   - `1`表示early start wait flag

## 注意事项

### 参数结构体定义注意事项

- 结构体必须根据用户实际算子核函数（Kernel）（global函数）的参数进行定义，不要直接复制示例中的字段名。
- 结构体字段必须按原函数参数顺序排列。
- 对于size小于4字节的类型（如`int16_t`、`int8_t`、`uint16_t`、`uint8_t`），必须使用`alignas(4)`确保内存对齐（ABI要求）。
- 示例：`alignas(4) int16_t val;`、`alignas(4) uint8_t flag;`。

### 系统参数使用注意事项

- `sk::SkSystemArgs *sysArgs`是可选参数，只有在原算子使用了`AscendC::GetBlockNum()`接口时才需要添加。
- 如果原算子未使用`AscendC::GetBlockNum()`接口，无需在函数入口传入`sysArgs`参数，这样可以避免不必要的代码修改。
- 如果原算子使用了`AscendC::GetBlockNum()`接口，可以通过添加`sk::SkSystemArgs *sysArgs`参数在SK场景下获取期望的block num：
  - 直接使用`sysArgs->skNumBlocks`访问成员变量。
  - 或者使用`sysArgs->SkGetNumBlocks()`接口获取。
- 用户需要根据实际情况修改原代码，将`AscendC::GetBlockNum()`的调用替换为上述两种方式之一。
- 与`AscendC::GetBlockNum()`的逻辑差异是，MIX 1:2场景中，vector核中的`skBlockNum`为`blockNum * 2`。

### 绑定限制

- `SK_BIND`最多支持绑定4个SK子函数。
- SK子函数的模板参数从0开始递增（`<0>`到`<3>`）。
- 可以根据实际需求绑定少于等于4个函数。

### 绑定掩码参数说明

`SK_BIND`宏的第二个参数是掩码（uint64_t类型），用于配置SuperKernel特性：

| 值 | 含义 |
| --- | --- |
| 8 | disable batchmode check |
| 4 | disable dcci |
| 2 | early start set flag |
| 1 | early start wait flag |

## 常见问题

**Q1：为什么要定义参数结构体？**

A：SuperKernel使用结构体传递参数，用户需要根据原global函数的实际参数定义一个结构体来封装所有入参，这样可以实现参数的统一管理和跨split共享。注意结构体定义必须根据用户实际算子核函数（Kernel）（global函数）的参数进行定义，不要直接复制示例中的字段名。

**Q2：为什么小于4字节的类型需要alignas(4)？**

A：这是ABI（Application Binary Interface）的要求，确保内存对齐，避免访问异常或性能问题。

**Q3：SK子函数的逻辑可以与原函数不同吗？**

A：不可以。SK子函数必须与原global函数在获取参数后的所有逻辑保持一致，否则可能导致计算结果错误。

**Q4：如果只需要1个或2个SK子函数怎么办？**

A：可以在`SK_BIND`中只填写需要的函数数量，例如：

```cpp
SK_BIND(add_custom, 4, add_custom_sk<0>, add_custom_sk<1>);
```

**Q5：sk::SkSystemArgs参数是必须的吗？**

A：不是必须的。`sk::SkSystemArgs *sysArgs`参数是可选的，只有算子需要使用该功能时添加，当前仅支持获取block num。

**Q6：原global函数还需要保留吗？**

A：是的，原global函数继续用于非SuperKernel场景，可以同时支持两种部署方式。
