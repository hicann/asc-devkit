# 基本流程

本文属于基础内容，介绍aclnn工程化算子开发方式中Kernel侧的基本接入流程，重点说明Kernel入口函数格式、参数顺序、TilingData处理和编译期信息获取方式。Global Memory地址绑定、UB管理、数据搬运、计算和同步等Kernel核心实现方法，请参考[核函数](../../../programming_model/ai_core_simd_programming/kernel_function.md)。

## Kernel函数格式

### 命名规则

Kernel入口函数名需要和算子类型对应。算子类型采用大驼峰命名，Kernel入口函数采用小写下划线命名，框架在编译和运行时根据命名规则完成匹配。

| 算子类型 | Kernel入口函数名 |
|---|---|
| `AddCustom` | `add_custom` |
| `MatmulCustom` | `matmul_custom` |
| `ReduceMaxCustom` | `reduce_max_custom` |
| `Conv2DCustom` | `conv2_d_custom` |
| `GatherV2Custom` | `gather_v2_custom` |

完整转换规则和更多示例会在[命名转换规则对照表](../appendix/naming_conversion_table.md)中展开。

### 函数形式

Kernel入口函数可以使用非模板入口或模板化入口。

**非模板入口**使用`extern "C"`指定C语言链接规则，msOpGen生成的基础模板通常采用这种形式。

```cpp
extern "C" __global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
{
    AscendC::InitSocState();

    REGISTER_TILING_DEFAULT(AddCustomTilingData);
    GET_TILING_DATA(tilingData, tiling);

    KernelAdd op;
    op.Init(x, y, z, tilingData.totalLength, tilingData.tileNum);
    op.Process();

    AscendC::PipeBarrier<PIPE_ALL>();
}
```

**模板化入口**通过模板参数承载dtype、Tile数量和分支选择信息。下面示例对应的算子类型是`AddCustomTemplate`，因此Kernel入口名为`add_custom_template`。在aclnn工程化算子开发方式中，需要先声明模板参数及其可用组合，再由Host侧Tiling通过`ASCENDC_TPL_SEL_PARAM`选择组合，编译工具根据配置实例化对应的Kernel。

```cpp
template <typename D_T_X, typename D_T_Y, typename D_T_Z, int TILE_NUM, int IS_SPLIT>
__global__ __aicore__ void add_custom_template(GM_ADDR x, GM_ADDR y, GM_ADDR z,
                                               GM_ADDR workspace, GM_ADDR tiling)
{
    AscendC::InitSocState();

    REGISTER_TILING_DEFAULT(TilingDataTemplate);
    GET_TILING_DATA_WITH_STRUCT(TilingDataTemplate, tilingData, tiling);

    KernelAdd<D_T_X, D_T_Y, D_T_Z> op;
    op.Init(x, y, z, tilingData.totalLength, TILE_NUM);

    if constexpr (IS_SPLIT == 0) {
        op.Process1();
    } else {
        op.Process2();
    }

    AscendC::PipeBarrier<PIPE_ALL>();
}
```

上述代码使用`REGISTER_TILING_DEFAULT`注册默认TilingData结构体，并通过`GET_TILING_DATA_WITH_STRUCT`按指定类型解析Tiling数据。模板参数声明、Host侧选择和编译配置请参考[多分支策略](./multi_branch_strategy.md)的Tiling模板编程章节。

### 参数顺序

Kernel入口函数参数需要按照“输入、输出、workspace、tiling”的顺序排列。框架调用Kernel时，会先按照算子原型定义中的输入输出顺序传入Tensor地址，再追加workspace和tiling两个参数。

```cpp
extern "C" __global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z,
                                                  GM_ADDR workspace, GM_ADDR tiling);
```

参数含义如下：

| 参数 | 来源 | 作用 |
|---|---|---|
| `x`、`y` | 算子原型中的输入 | 指向输入Tensor的Global Memory地址 |
| `z` | 算子原型中的输出 | 指向输出Tensor的Global Memory地址 |
| `workspace` | 框架追加 | 指向workspace内存地址 |
| `tiling` | 框架追加 | 指向Host侧Tiling写入的数据 |

> **说明：输入输出同名场景**
>
> 如果算子原型中的输入和输出同名，msOpGen生成Kernel入口参数时会给输出参数增加`ref`后缀，用于区分输入和输出地址。
>
> ```cpp
> extern "C" __global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y,
>                                                   GM_ADDR x_ref, GM_ADDR workspace,
>                                                   GM_ADDR tiling);
> ```

Kernel实现类的内部组织方式由算子计算逻辑和Ascend C编程模式决定。在aclnn工程化算子开发方式中，Kernel入口只需要保持框架要求的参数顺序，读取TilingData，并将输入输出地址和Tiling参数传给Kernel实现类。

AddCustom的Kernel入口按以下步骤串联算子实现：

1. 通过`GET_TILING_DATA`读取Host侧传入的`totalLength`和`tileNum`。
2. 创建`KernelAdd`对象，将输入输出地址和Tiling参数传入`Init`。
3. 调用`Process`，将当前Tile的输入数据从Global Memory搬运到Local Memory。
4. 调用`Add`完成逐元素加法计算。
5. 将计算结果从Local Memory搬运回Global Memory。

`Process`根据`tileNum`循环执行步骤3至步骤5，直到当前Block的所有Tile处理完成。搬运和计算之间需要根据数据依赖做好同步，具体实现请参考[核函数](../../../programming_model/ai_core_simd_programming/kernel_function.md)。

## TilingData处理

Host侧Tiling函数会把Kernel运行需要的参数写入TilingData。TilingData可以使用标准C++语法或宏定义方式定义，两种方式的注册位置和注册接口不同。

### 注册TilingData

注册TilingData用于确定Kernel默认使用的TilingData结构体类型。标准C++结构体和宏定义结构体的注册位置不同。

**标准C++结构体**

使用标准C++语法定义TilingData时，Kernel侧在入口函数中使用[REGISTER_TILING_DEFAULT](../../../../../api/SIMD-API/basic_api/Kernel-Tiling/REGISTER_TILING_DEFAULT.md)注册默认结构体。

```cpp
extern "C" __global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z,
                                                  GM_ADDR workspace, GM_ADDR tiling)
{
    REGISTER_TILING_DEFAULT(AddCustomTilingData);
    // ...
}
```

**宏定义结构体**

使用`BEGIN_TILING_DATA_DEF`等宏定义TilingData时，在Host侧通过`REGISTER_TILING_DATA_CLASS`将默认结构体与算子类型绑定。

```cpp
REGISTER_TILING_DATA_CLASS(AddCustom, AddStruct)
```

宏定义结构体的Kernel入口不使用`REGISTER_TILING_DEFAULT`，直接通过构建生成的TilingData访问宏获取数据。

本节只介绍默认TilingData的注册方式。同一算子需要组织多个Kernel分支时，分支专用TilingData的注册方式以及Kernel模板参数的配置方式，请参见[多分支策略](./multi_branch_strategy.md)。

### 获取TilingData

Kernel可以将入口参数`GM_ADDR tiling`转换为GM指针，再按照TilingData的内存布局和字段偏移读取数据。但这种写法需要Kernel自行维护地址转换和偏移关系，并且会使代码直接依赖动态编译的GM地址和内存布局；静态编译场景仍需单独处理。

`GET_TILING_DATA`和`GET_TILING_DATA_WITH_STRUCT`封装了动态、静态两种场景下Tiling数据获取方式的差异。动态编译场景下，宏在Kernel运行时从入口参数tiling指向的GM内存读取Tiling数据，并初始化相应的Tiling结构体；静态编译场景下，Host Tiling在算子编译阶段执行，生成的Tiling数据被直接固化到Kernel代码中，宏使用这些内嵌数据初始化Tiling结构体，不再访问Kernel入口的tiling参数。两种场景使用相同的宏调用和字段访问方式，因此一套Kernel代码可以同时支持动态编译和静态编译，实现Kernel源码层面的动静归一。

两个宏的能力区别如下：

- [GET_TILING_DATA](../../../../../api/SIMD-API/basic_api/Kernel-Tiling/GET_TILING_DATA.md)`(tiling_data, tiling_arg)`：使用当前Kernel分支注册的默认TilingData类型进行解析，适用于只需访问默认结构体的场景。
- [GET_TILING_DATA_WITH_STRUCT](../../../../../api/SIMD-API/basic_api/Kernel-Tiling/GET_TILING_DATA_WITH_STRUCT.md)`(tiling_struct, tiling_data, tiling_arg)`：显式指定解析使用的TilingData结构体类型，既可用于不同TilingKey使用不同结构体的场景，也可用于TPL/SEL模板分支显式指定模板TilingData结构体的场景。

默认TilingData结构体通过`GET_TILING_DATA`解析。第一个参数是解析后的变量名，第二个参数是Kernel入口中的`tiling`地址。

```cpp
GET_TILING_DATA(tilingData, tiling);
```

解析完成后，Kernel侧可以读取`tilingData`中的字段，例如将`totalLength`和`tileNum`传给`KernelAdd::Init`。

```cpp
KernelAdd op;
op.Init(x, y, z, tilingData.totalLength, tilingData.tileNum);
```

需要显式指定TilingData结构体类型时，通过`GET_TILING_DATA_WITH_STRUCT`解析：

```cpp
GET_TILING_DATA_WITH_STRUCT(TilingDataTemplate, tilingData, tiling);
```

第一个参数可以是按TilingKey注册的分支专用结构体，也可以是TPL/SEL模板分支使用的模板TilingData结构体。两种分支组织方式的宏调用和字段访问方式相同；分支和结构体的具体配置方式请参见[多分支策略](./multi_branch_strategy.md)。使用默认结构体的分支仍通过`GET_TILING_DATA`解析。

TilingData定义方式和Host侧写入流程请参考[Host侧Tiling实现](./host_tiling_implementation.md)。

## 信息获取

Kernel侧除了输入输出地址和TilingData，还可以通过框架生成的宏获取输入输出的dtype和format信息。这些信息通常用于为GlobalTensor、LocalTensor等对象指定元素数据类型，选择计算路径或处理format差异。

### 获取输入输出dtype和format

Kernel侧可以通过`DTYPE_<Arg>`、`ORIG_DTYPE_<Arg>`、`FORMAT_<Arg>`三类宏获取输入输出的数据类型和格式信息。`<Arg>`由Kernel入口参数名转换成全大写形式。

| 原型定义name | Kernel入口参数名 | dtype宏 | 原始dtype宏 | format宏 |
|---|---|---|---|---|
| `x` | `x` | `DTYPE_X` | `ORIG_DTYPE_X` | `FORMAT_X` |
| `y` | `y` | `DTYPE_Y` | `ORIG_DTYPE_Y` | `FORMAT_Y` |
| `z` | `z` | `DTYPE_Z` | `ORIG_DTYPE_Z` | `FORMAT_Z` |
| `input_data` | `input_data` | `DTYPE_INPUT_DATA` | `ORIG_DTYPE_INPUT_DATA` | `FORMAT_INPUT_DATA` |
| `output_result` | `output_result` | `DTYPE_OUTPUT_RESULT` | `ORIG_DTYPE_OUTPUT_RESULT` | `FORMAT_OUTPUT_RESULT` |

三类宏的含义如下：

| 宏 | 含义 | 常见用途 |
|---|---|---|
| `DTYPE_<Arg>` | Kernel侧可直接使用的C++数据类型，如`half`、`float`、`int32_t` | 声明变量、`GlobalTensor`、`LocalTensor`或模板参数 |
| `ORIG_DTYPE_<Arg>` | 算子原型中声明的dtype编号，如`DT_FLOAT16`、`DT_FLOAT` | 与dtype编号比较，选择不同计算路径 |
| `FORMAT_<Arg>` | 算子原型中声明的format编号，如`FORMAT_ND`、`FORMAT_NZ` | 根据format选择不同数据组织方式 |

`DTYPE_X`和`ORIG_DTYPE_X`关注点不同。以`x`为例，`DTYPE_X`可以直接作为Kernel侧C++类型使用；`ORIG_DTYPE_X`用于和dtype编号比较。`ORIG_DTYPE_<Arg>`和`FORMAT_<Arg>`由编译选项传入，通常用于预处理分支。

```cpp
extern "C" __global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z,
                                                  GM_ADDR workspace, GM_ADDR tiling)
{
    GET_TILING_DATA(tilingData, tiling);

    DTYPE_X temp;
    func<DTYPE_Z>();

    #if FORMAT_Y == FORMAT_ND
    // ND格式处理逻辑
    #endif

    #if ORIG_DTYPE_Y == DT_FLOAT
    // float类型处理逻辑
    #endif
}
```

> [!NOTE] Kernel模板场景下的宏使用
> 在Kernel模板场景中，应谨慎使用`DTYPE_<Arg>`、`ORIG_DTYPE_<Arg>`和`FORMAT_<Arg>`等宏配合`#if/#endif`分割代码。预处理宏和模板实例化都会影响最终生成的代码；若同时使用，需要检查每个模板参数组合下的宏取值、保留的代码分支和模板实参是否一致，避免宏裁剪掉某个模板实例所需的代码，或产生重复定义、符号冲突等问题。由模板参数决定的分支，建议优先使用`if constexpr`表达。

## 相关文档

- [Host侧Tiling实现](./host_tiling_implementation.md)：了解TilingData定义、Tiling函数编写和workspace设置。
- [算子原型定义](./operator_prototype_definition.md)：了解输入输出顺序、dtype/format声明和AI处理器配置。
- [多分支策略](./multi_branch_strategy.md)：了解`TilingKey`如何选择不同Kernel实现分支。
- [输出shape依赖计算](./kernel_output_shape_computation.md)：了解输出shape依赖Kernel计算结果时的处理方式。
