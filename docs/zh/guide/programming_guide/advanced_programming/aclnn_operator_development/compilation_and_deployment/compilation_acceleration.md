# 编译加速

本文是扩展内容，介绍如何降低算子工程的编译耗时。算子工程编译耗时主要来自核函数（Kernel）侧代码的逐芯片、逐TilingKey编译；本文从缓存复用、选择性编译、并行度和增量构建四个维度给出加速方案。

## ccache编译缓存

ccache是编译缓存工具。第一次编译时，它把编译结果写入缓存；后续编译如果源码、编译命令、编译器等输入一致，就可以复用缓存，减少重复编译耗时。

在算子工程中，可通过CMake的`CMAKE_CXX_COMPILER_LAUNCHER`配置ccache。该变量会作用于Host侧C++编译；在CANN算子工程中，CMake模块也会将ccache路径传给核函数（Kernel）侧编译脚本，用于Ascend C核函数（Kernel）编译过程。

### 单机ccache

ccache安装及查找路径命令如下：
```bash
apt install ccache
command -v ccache
ccache --version
```

后续配置中的ccache路径以`command -v ccache`输出为准。ccache清空命令如下：
```bash
ccache -z          #仅清空统计，不删除缓存内容
ccache -C          #仅清空缓存内容，不清空统计
ccache -Cz         #同时清空缓存内容和统计
```

#### 启用方式

**命令行临时启用**

开发调试时，推荐在CMake配置阶段通过命令行启用ccache，不需要修改工程文件：

```bash
cmake -S . -B build_out --preset=default \
    -DCMAKE_CXX_COMPILER_LAUNCHER=$(command -v ccache)
```

该命令会把ccache配置写入`build_out/CMakeCache.txt`。后续继续使用同一个`build_out`构建目录时，会沿用这个配置。

**写入顶层CMakeLists.txt**

如果希望工程长期默认启用ccache，也可以写入算子工程的顶层`CMakeLists.txt`，并放在创建编译目标之前，例如：

```cmake
cmake_minimum_required(VERSION 3.19.0)
project(opp)

set(CMAKE_CXX_COMPILER_LAUNCHER /usr/bin/ccache)
```

如果ccache安装在其他路径，请将`/usr/bin/ccache`替换为`command -v ccache`查到的实际路径。

#### 验证缓存是否生效

验证时需要注意：第一次构建通常是写缓存，命中率不高；第二次触发重新编译时，才应该看到缓存命中。

**步骤1：清空缓存和统计，执行第一次构建。**

```bash
ccache -Cz
cmake -S . -B build_out --preset=default -DCMAKE_CXX_COMPILER_LAUNCHER=$(command -v ccache)
cmake --build build_out --target binary package -j$(nproc)
ccache --show-stats -v
```

第一次构建后，`Misses`通常较多，这是正常现象，表示缓存正在写入。

**步骤2：保留缓存，只清空统计，再触发第二次编译。**

如果源码没有变化而直接重复执行`cmake --build`，CMake可能判断无需重新编译，此时ccache统计不会明显变化。因此验证命中率时，可先执行`clean`目标清理构建产物，再重新构建：

```bash
ccache -z
cmake --build build_out --target clean
cmake --build build_out --target binary package -j$(nproc)
ccache --show-stats -v
```

第二次构建后，`Hits`或`Primary storage`中的`Hits`应明显提升。如果仍然没有命中，优先检查：

- configure阶段是否传入了`-DCMAKE_CXX_COMPILER_LAUNCHER=$(command -v ccache)`。
- 构建目录中的编译命令是否包含ccache，可搜索`build_out`下的`build.make`。
- 两次编译的源码、编译器和编译选项是否一致。

更多`ccache`配置和缓存行为说明请参考[ccache官方文档](https://ccache.dev/documentation.html)。

### 分布式缓存（ccache + Redis）

适用于多机共享缓存场景：机器A编译后将结果推送至Redis，机器B在相同源码、相同编译选项和相同工具链版本下可从Redis命中缓存，减少重复编译。

分布式场景下，`ccache`以各机器本地缓存作为一级缓存，Redis作为共享二级缓存。机器A首次编译时会调用实际编译器并将结果写入本地缓存和Redis；机器B在相同源码、相同编译命令、相同编译器和相同构建目录路径下再次编译时，可直接从Redis命中共享缓存，减少编译动作的重复执行。若两台机器编译器路径不同但内容一致，建议设置`compiler_check=content`。


组网要求：

- 机器A：首次编译机器，将缓存写入Redis，IP为`<A_IP>`
- 机器B：二次编译机器，从Redis验证共享缓存命中，IP为`<B_IP>`
- 机器C：Redis服务器，保存共享缓存数据，IP为`<C_IP>`
- A、B、C三台机器需处于同一网络下，A/B机器都必须能够访问`C_IP:6379`

建议机器A和机器B使用相同的源码内容、相同的编译命令、相同版本的编译器，并保持一致的源码路径和构建目录路径，否则可能出现缓存未命中的情况。

**1. 机器C：部署Redis服务**
```bash
apt install redis-server
# 启动Redis服务
redis-server --daemonize yes --bind 0.0.0.0 --port 6379 --requirepass <PASSWORD>
# 验证Redis连接
redis-cli -h <C_IP> -p 6379 -a <PASSWORD> ping
```

> 说明：上述配置仅用于受控测试环境。共享环境或生产环境建议开启访问控制、认证和网络隔离。

**2. 机器A / 机器B：配置ccache**
```bash
apt install redis-tools
# 验证Redis连接
redis-cli -h <C_IP> -p 6379 -a <PASSWORD> ping
# 配置Redis作为二级存储，带密码认证格式
# 格式：redis://default:<PASSWORD>@<C_IP>:6379
ccache --set-config=secondary_storage=redis://default:<PASSWORD>@<C_IP>:6379
# 配置编译器内容校验，避免路径差异导致缓存未命中
ccache --set-config=compiler_check=content
```

**3. 机器A / 机器B：执行构建**

机器A构建会写入本地缓存和Redis:

```bash
ccache -Cz
cmake -S . -B build_out --preset=default -DCMAKE_CXX_COMPILER_LAUNCHER=$(command -v ccache)
cmake --build build_out --target binary package -j$(nproc)
ccache --show-stats -v
```

机器B本地缓存为空，可从Redis读取共享缓存。机器A构建后，在机器B上执行构建:

```bash
ccache -z
cmake --build build_out --target clean
cmake --build build_out --target binary package -j$(nproc)
ccache --show-stats -v
```

对比两次`ccache --show-stats -v`结果，可看到在机器B上远程缓存命中率`Remote storage Hits`显著提升。


<a id="选择性编译"></a>
## 选择性编译

选择性编译的核心思路是：开发调试阶段只编译当前真正需要验证的部分，减少无关核函数（Kernel）变体、无关芯片和无关产物带来的编译开销。常用策略如下：

| 策略 | 适用场景 | 配置方式 | 效果 |
|------|---------|---------|------|
| `--tiling_key=1,2` | TilingKey编程算子，仅调试特定分支。 | `npu_op_kernel_options`的OPTIONS参数。 | 只编译指定TilingKey的核函数（Kernel），跳过其余。 |
| `--kernel-template-input="D_T_X=float"` | 模板编程算子，仅调试特定模板参数组合。 | `npu_op_kernel_options`的OPTIONS参数。 | 只编译指定模板参数组合的核函数（Kernel）。 |
| `ASCEND_COMPUTE_UNIT`收窄 | 开发阶段只跑当前SOC。 | 修改`CMakePresets.json`或cmake的ASCEND_COMPUTE_UNIT参数。 | 跳过其他芯片的核函数（Kernel）编译。 |
| `ENABLE_BINARY_PACKAGE=False` | 源码发布模式，不需要核函数（Kernel）二进制。 | CMakeLists.txt或cmake参数。 | 跳过核函数（Kernel） `.o`生成，保留核函数（Kernel）源码和必要配置。 |

### TilingKey选择编译

只编译指定[TilingKey](../design_and_implementation/multi_branch_strategy.md#compile-selected-tiling-keys)相关的核函数（Kernel）代码，用于加速编译过程。若不指定TilingKey编译，则默认编译所有的TilingKey。配置多个TilingKey时，TilingKey之间不能有空格。示例如下，其中1、2为TilingKey取值：

```bash
# 在op_kernel/CMakeLists.txt中添加
npu_op_kernel_options(ascendc_kernels AddCustom COMPUTE_UNIT Ascendxxyy OPTIONS --tiling_key=1,2)
```

### 模板参数选择编译

对于多分支与模板化算子，可只编译指定的模板参数组合。设置`--kernel-template-input`选项后，只编译指定的模板参数组合相关的核函数（Kernel）代码，用于加速编译过程。若不设置该选项，则默认编译所有的模板参数组合。传入的参数为键值对列表，整体需用双引号或单引号包裹。不同模板参数之间用英文分号（;）分隔，相同模板参数配置多个值时用英文逗号（,）分隔。配置时不能有空格。示例如下：

```bash
npu_op_kernel_options(ascendc_kernels AddCustomTemplate COMPUTE_UNIT Ascendxxyy OPTIONS --kernel-template-input="D_T_X=float;D_T_Y=float;D_T_Z=float")
```

配置模板参数组合时，模板参数名需要与核函数（Kernel）入口处以及Host侧定义的模板参数名匹配。对于模板参数组合的值，如果存在自定义类型，需要替换为其对应数字值，如果为原生支持数据类型，则与核函数（Kernel）入口处入参保持一致。示例如下：

```CPP
// Host侧模板参数定义
#define ADD_TPL_FP16 10
#define ADD_TPL_FP32 20

ASCENDC_TPL_ARGS_DECL(AddCustomTemplateNativeDtype,
    ASCENDC_TPL_DATATYPE_DECL(D_T_X, C_DT_FLOAT, C_DT_FLOAT16, ASCENDC_TPL_INPUT(0)),
    ASCENDC_TPL_DTYPE_DECL(D_T_Y, ADD_TPL_FP16, ADD_TPL_FP32),
    ASCENDC_TPL_DATATYPE_DECL(D_T_Z, C_DT_FLOAT, C_DT_FLOAT16, ASCENDC_TPL_OUTPUT(0)),
    ASCENDC_TPL_UINT_DECL(TILE_NUM, ASCENDC_TPL_8_BW, ASCENDC_TPL_UI_MIX, 2, 0, 2, 3, 5, 10, 12, 13, 9, 8),
    ASCENDC_TPL_BOOL_DECL(IS_SPLIT, 0, 1),
);

ASCENDC_TPL_SEL(
    ASCENDC_TPL_ARGS_SEL(
    ASCENDC_TPL_DATATYPE_SEL(D_T_X, C_DT_FLOAT),
    ASCENDC_TPL_DTYPE_SEL(D_T_Y, ADD_TPL_FP32),
    ASCENDC_TPL_DATATYPE_SEL(D_T_Z, C_DT_FLOAT),
    ASCENDC_TPL_UINT_SEL(TILE_NUM, ASCENDC_TPL_UI_LIST, 1, 8),
    ASCENDC_TPL_BOOL_SEL(IS_SPLIT, 0, 1),
    ASCENDC_TPL_DETERMINISTIC_SEL(true),
    ASCENDC_TPL_KERNEL_TYPE_SEL(ASCENDC_TPL_AIV_ONLY),
    ),
    ASCENDC_TPL_ARGS_SEL(
    ASCENDC_TPL_DATATYPE_SEL(D_T_X, C_DT_FLOAT16),
    ASCENDC_TPL_DTYPE_SEL(D_T_Y, ADD_TPL_FP16),
    ASCENDC_TPL_DATATYPE_SEL(D_T_Z, C_DT_FLOAT16),
    ASCENDC_TPL_UINT_SEL(TILE_NUM, ASCENDC_TPL_UI_LIST, 1, 8),
    ASCENDC_TPL_BOOL_SEL(IS_SPLIT, 0, 1),
    ASCENDC_TPL_DETERMINISTIC_SEL(false),
    ASCENDC_TPL_KERNEL_TYPE_SEL(ASCENDC_TPL_AIV_ONLY),
    ),
);
#endif

// 核函数（Kernel）入口。模板参数名需要与Host侧模板参数定义保持一致。
template <typename D_T_X, typename D_T_Y, typename D_T_Z, uint32_t TILE_NUM, bool IS_SPLIT>
__global__ __aicore__ void add_custom_template(GM_ADDR x, GM_ADDR y, GM_ADDR z,
                                               GM_ADDR workspace, GM_ADDR tiling)
{
    GET_TILING_DATA(tiling_data, tiling);

    if constexpr (std::is_same_v<D_T_X, float> && std::is_same_v<D_T_Z, float>) {
        KernelAdd<D_T_X, float, D_T_Z> op;
        op.Init(x, y, z, tiling_data.totalLength, TILE_NUM);
        op.Process1();
    } else if constexpr (std::is_same_v<D_T_X, half> && std::is_same_v<D_T_Z, half>) {
        KernelAdd<D_T_X, half, D_T_Z> op;
        op.Init(x, y, z, tiling_data.totalLength, TILE_NUM);
        op.Process2();
    }
}
```

编译选项示例如下：

```bash
npu_op_kernel_options(ascendc_kernels AddCustomTemplate COMPUTE_UNIT Ascendxxyy OPTIONS --kernel-template-input="D_T_X=half;D_T_Y=10;D_T_Z=half")
```

### 收窄芯片范围

`ASCEND_COMPUTE_UNIT`配置了多少个芯片型号，核函数（Kernel）侧通常就会按芯片型号分别编译。开发阶段如果只在当前机器上验证，可修改CMakeLists.txt里的ASCEND_COMPUTE_UNIT保留当前SOC或者临时只构建某个SOC。以下是CMake配置阶段临时设置只构建ascendxxyy SOC的示例：

```bash
cmake -S . -B build_out --preset=default -DASCEND_COMPUTE_UNIT=ascendxxyy
```

### 跳过核函数（Kernel）二进制编译

如果当前只需要生成源码包，或暂时不需要核函数（Kernel） `.o`二进制，可关闭二进制包生成。顶层`CMakeLists.txt`中应使用变量形式传递`ENABLE_BINARY_PACKAGE`：

```cmake
npu_op_package(${package_name}
    TYPE RUN
    CONFIG
        ENABLE_SOURCE_PACKAGE ${ENABLE_SOURCE_PACKAGE}
        ENABLE_BINARY_PACKAGE ${ENABLE_BINARY_PACKAGE}
        INSTALL_PATH ${CMAKE_BINARY_DIR}/
)
```

然后在configure阶段关闭二进制编译：

```bash
cmake -S . -B build_out --preset=default -DENABLE_BINARY_PACKAGE=False
cmake --build build_out --target binary package -j$(nproc)
```

该模式下仍会执行Host侧库、Tiling库和配置文件生成，但传给核函数（Kernel）编译脚本的`--enable-binary`为`False`，不会生成核函数（Kernel） `.o`二进制文件；`op_kernel/ascendc_kernels/binary/dynamic/`下会保留核函数（Kernel）源码等文件。

> [!NOTE]说明
>
> 如果顶层`CMakeLists.txt`在`npu_op_package(... CONFIG ...)`中硬编码了`ENABLE_BINARY_PACKAGE True`，命令行传入的`-DENABLE_BINARY_PACKAGE=False`不会生效。要允许命令行覆盖，应改为`${ENABLE_BINARY_PACKAGE}`变量形式，或不要在CONFIG中覆盖该变量。

## 并行度控制

`cmake --build ... -j`用来控制构建阶段的并行任务数。合理提高并行度可以缩短编译时间，但线程数过高会带来CPU、内存和I/O资源争抢，严重时反而变慢，甚至触发编译失败。

**线程数选择建议**：

- 一般场景可先使用`-j$(nproc)`，让构建系统按可用CPU核数并行。
- 如果机器内存较小，或核函数（Kernel）编译阶段出现明显卡顿、系统负载过高，可改为`nproc / 2`或固定较小值。

```bash
cmake --build build_out --target binary package -j$(nproc)
cmake --build build_out --target binary package -j8
```

## 增量编译

增量编译指不删除`build_out`，继续使用同一个构建目录，CMake复用上一次configure生成的构建规则、中间文件和部分已编译产物。避免每次构建都从零开始，加快构建编译速度。

第一次构建时，需要先执行configure，再执行build：

```bash
cmake -S . -B build_out --preset=default
cmake --build build_out --target binary package -j$(nproc)
```

**增量编译**：之后如果只是修改已有源码文件，通常只需要重新执行build。Host侧C++文件、Tiling实现等CMake可追踪依赖的目标，一般可以根据文件变更按需触发对应目标重新编译。

```bash
cmake --build build_out --target binary package -j$(nproc)
```

**全量编译**：但是如果修改了工程结构、CMake配置或会影响代码生成的内容，例如修改算子原型、修改CMakeLists.txt配置或者新增/删除算子文件等，需要先重新执行configure，再build：

```bash
cmake -S . -B build_out --preset=default
cmake --build build_out --target binary package -j$(nproc)
```

大多数源码小改动都不需要清空`build_out`。只有在怀疑构建目录里残留了错误状态时，才需要重新创建干净的build目录：

```bash
rm -rf build_out
cmake -S . -B build_out --preset=default
cmake --build build_out --target binary package -j$(nproc)
```

**增量编译和ccache**可以配合使用。增量编译减少需要重新处理的目标；当某些文件仍被触发重编时，ccache可以在源码、编译命令和编译器一致的情况下复用缓存：

```bash
cmake -S . -B build_out --preset=default -DCMAKE_CXX_COMPILER_LAUNCHER=$(command -v ccache)
cmake --build build_out --target binary package -j$(nproc)
```

后续继续使用同一个`build_out`时，`CMAKE_CXX_COMPILER_LAUNCHER`会保存在`build_out/CMakeCache.txt`中，不需要每次build都重新传入。
