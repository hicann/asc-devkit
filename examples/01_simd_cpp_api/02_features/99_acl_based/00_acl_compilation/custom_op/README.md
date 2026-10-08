# 自定义算子工程编译、打包和部署样例

## 概述

本样例以简单自定义算子为基础，展示了其编译、打包成自定义算子包，并部署到CANN环境中的流程。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |
| Atlas 200I/500 A2推理产品 | >= CANN 9.0.0 |
| Atlas推理系列产品 | >= CANN 9.0.0 |

> 注意: 本样例中涉及多个算子示例，请以各个算子示例实际支持的产品型号为准。

## 目录结构介绍

```
├── custom_op
│   ├── CMakeLists.txt               // 顶层编译工程文件
│   ├── framework                    // 框架适配插件目录
│   │   ├── CMakeLists.txt           // 框架适配编译工程文件
│   │   ├── onnx_plugin              // ONNX框架适配插件
│   │   └── tf_plugin                // TensorFlow框架适配插件
│   ├── op_host                      // Host侧算子实现目录
│   │   ├── CMakeLists.txt           // Host侧编译工程文件
│   │   ├── add_custom               // Add算子Host实现
│   │   ├── add_custom_template      // Add模板算子Host实现
│   │   ├── add_custom_tiling_sink   // Add Tiling下沉算子Host实现
│   │   └── leaky_relu_custom        // LeakyRelu算子Host实现
│   ├── op_kernel                    // Kernel侧算子实现目录
│   │   ├── CMakeLists.txt           // Kernel侧编译工程文件
│   │   ├── add_custom               // Add算子Kernel实现
│   │   ├── add_custom_template      // Add模板算子Kernel实现
│   │   ├── add_custom_tiling_sink   // Add Tiling下沉算子Kernel实现
│   │   └── leaky_relu_custom        // LeakyRelu算子Kernel实现
│   └── README.md                    // 样例说明文档
```

## 样例描述

Add计算公式为：

```
z = x + y
```

AddCustomTilingSink、AddCustomTemplate与Add的核函数功能一致。其中，
  - AddCustomTemplate展示了Tiling模板编程，添加的模板参数包括输入的数据类型、shape等，根据模板参数，简化或统一样例的实现逻辑，开发者可以在模板参数中定义需要的信息，如输入输出的数据类型，其他扩展参数等；
  - AddCustomTilingSink用于展示Tiling下沉场景，通过`DEVICE_IMPL_OP_OPTILING`将Tiling函数同时注册到host和device上执行。

LeakyRelu计算公式为：

$$
y=
\begin{cases}
x, \quad x\geq 0\\
a*x, \quad x<0
\end{cases}
$$
其中a为scalar值。

## 样例规格描述

- AddCustom
  <table border="2" align="center">
  <caption>表1：AddCustom样例规格描述</caption>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">AddCustom</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[8, 2048]</td><td align="center">float16</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[8, 2048]</td><td align="center">float16</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[8, 2048]</td><td align="center">float16</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">add_custom</td></tr>
  </table>

- AddCustomTilingSink

  <table border="2" align="center">
  <caption>表2：AddCustomTilingSink样例规格描述</caption>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">AddCustomTilingSink</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">add_custom_tiling_sink</td></tr>
  </table>

- AddCustomTemplate

  <table border="2" align="center">
  <caption>表3：AddCustomTemplate样例规格描述</caption>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">AddCustomTemplate</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[8, 2048]</td><td align="center">float16/float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[8, 2048]</td><td align="center">float16/float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[8, 2048]</td><td align="center">float16/float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">add_custom_template</td></tr>
  <tr><td rowspan="6" align="center">模板参数</td><td colspan="4" align="center">template&lt;typename D_T_X, typename D_T_Y, typename D_T_Z, int TILE_NUM, int IS_SPLIT&gt;</td>
      <tr><td>D_T_X</td><td colspan="1">typename</td><td colspan="2">数据类型(half，float)</td></tr>
      <tr><td>D_T_Y</td><td colspan="1">typename</td><td colspan="2">数据类型(half，float)</td></tr>
      <tr><td>D_T_Z</td><td colspan="1">typename</td><td colspan="2">数据类型(half，float)</td></tr>
      <tr><td>TILE_NUM</td><td colspan="1">int</td><td colspan="2">切分数量</td></tr>
      <tr><td>IS_SPLIT</td><td colspan="1">int</td><td colspan="2">是否切分</td></tr>
  </tr>
  </table>

- LeakyRelu

  <table border="2" align="center">
  <caption>表4：LeakyRelu样例规格描述</caption>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">LeakyRelu</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[8, 200, 1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">negative_slope</td><td align="center">0.0</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">y</td><td align="center">[8, 200, 1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">leaky_relu_custom</td></tr>
  </table>

## 代码实现介绍

- Add</br>
  - kernel实现：</br>
    基于Ascend C提供的矢量计算接口`Add`实现。</br>

  - tiling实现：</br>
    TilingData参数设计，`AddCustomTilingData`参数本质上是和并行数据切分相关的参数，本样例使用了2个tiling参数：`totalLength`和`tileNum`。`totalLength`是指需要计算的数据量大小，`tileNum`是指每个核上总计算数据分块个数。</br>

- AddCustomTemplate</br>
  - kernel实现：</br>
    与Add一致。
  - Tiling模板设计：</br>
    本示例使用了5个模板参数，`D_T_X`、`D_T_Y`、`D_T_Z`分别是指输入x、输入y、输出z的数据类型，`TILE_NUM`是指每个核上总计算数据分块个数，`IS_SPLIT`是是否使能数据分块计算，`IS_SPLIT`为0时`TILE_NUM`无效。通过模板参数组合替代传统的TilingKey。

  - TilingData参数设计：</br>
    本示例样例使用了1个tiling参数，`totalLength`是指所有核需要计算的数据量总大小。</br>


- AddCustomTilingSink</br>
  - kernel实现：</br>
    功能与Add一致，kernel通过`KERNEL_TASK_TYPE_DEFAULT`接口将样例强制指定在AIC、AIV混合场景运行，满足Tiling下沉算子条件。将所有的Tiling函数逻辑单独在`add_custom_tiling_sink_tiling.cpp`中实现，并通过`DEVICE_IMPL_OP_OPTILING`接口注册下沉的Tiling函数。</br>

- LeakyRelu</br>
  - kernel实现：</br>
    基于基础API接口组合实现，使用`Maxs`、`Mins`、`Muls`、`Add`完成LeakyRelu计算。</br>

  - tiling实现：</br>
    TilingData参数设计，`LeakyReluCustomTilingData`参数本质上是和并行数据切分相关的参数，本示例样例使用了3个tiling参数：`totalLength`、`tileNum`、`negativeSlope`。`totalLength`、`tileNum`与Add样例类似，`negativeSlope`表示LeakyRelu的负半轴斜率系数，作为计算参数传递给kernel侧。</br>


## 编译运行

在本样例根目录下执行如下步骤，编译、打包并部署自定义样例包。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 编译、打包样例并部署

  ```bash
  mkdir -p build && cd build
  cmake .. && make -j binary package
  ./custom_opp_*.run
  ```

  执行结果如下，说明执行成功。

  ```text
  SUCCESS
  ```

## 缓存编译加速（可选）

本样例支持通过 ccache 加速重复编译，提供单机缓存和分布式缓存两种模式。

### 前提条件

- 已安装 `ccache`，建议使用 `ccache >= 4.6.1`

  ```bash
  apt install ccache
  ```

- 分布式缓存场景需确认当前版本支持 Redis 存储

  ```bash
  ccache --version
  # 预期：Features 中包含 redis-storage
  ```

### 单机缓存

开启 `-DENABLE_CCACHE=ON` 后，`cmake` 会将 `ccache` 挂接到编译流程中；当源码、编译命令、编译器和构建目录路径一致时，重复构建可直接复用本地缓存结果。

通过 cmake 参数启用，无需修改 CMakeLists.txt：

```bash
mkdir -p build && cd build
cmake -DENABLE_CCACHE=ON .. && make -j binary package
```

可按以下方式验证本地缓存是否生效：

```bash
ccache -Cz
# 首次构建：写入缓存
rm -rf build && mkdir -p build && cd build
cmake -DENABLE_CCACHE=ON .. && make -j binary package
cd ..
ccache -z
# 二次构建：命中缓存
rm -rf build && mkdir -p build && cd build
cmake -DENABLE_CCACHE=ON .. && make -j binary package
```

每次编译后通过 `ccache --show-stats -v` 查看缓存命中情况，可看到二次构建的本地缓存命中率 `Local storage Hits` 显著提升。

若需要清空本地缓存，可按以下方式操作：

```bash
# 仅清空统计信息，不删除缓存内容
ccache -z

# 清空本地缓存内容
ccache -C

# 同时清空本地缓存内容和统计信息
ccache -Cz
```

### 分布式缓存（ccache + Redis）

适用于多机共享缓存场景：机器 A 编译后将结果推送至 Redis，机器 B 在相同源码、相同编译选项和相同工具链版本下可从 Redis 命中缓存，减少重复编译。

分布式场景下，`ccache` 以各机器本地缓存作为一级缓存，Redis 作为共享二级缓存。机器 A 首次编译时会调用实际编译器并将结果写入本地缓存和 Redis；机器 B 在相同源码、相同编译命令、相同编译器和相同构建目录路径下再次编译时，可直接从 Redis 命中共享缓存，减少编译动作的重复执行。若两台机器编译器路径不同但内容一致，建议设置 `compiler_check=content`。


更多 `ccache` 配置和缓存行为说明可参考[ccache官方文档](https://ccache.dev/documentation.html)。

组网要求：

- 机器 A：首次编译机器，将缓存写入 Redis，IP 为 `<A_IP>`
- 机器 B：二次编译机器，从 Redis 验证共享缓存命中，IP 为 `<B_IP>`
- 机器 C：Redis 服务器，保存共享缓存数据，IP 为 `<C_IP>`
- A、B、C 三台机器需处于同一网络下，A/B 机器都必须能够访问 `C_IP:6379`

建议机器 A 和机器 B 使用相同的源码内容、相同的编译命令、相同版本的编译器，并保持一致的源码路径和构建目录路径，否则可能出现缓存未命中的情况。

**1. 机器 C：部署 Redis 服务**
```bash
apt install redis-server
# 启动 Redis 服务
redis-server --daemonize yes --bind 0.0.0.0 --port 6379 --requirepass <PASSWORD>
# 验证 Redis 连接
redis-cli -h <C_IP> -p 6379 -a <PASSWORD> ping
```

> 说明：上述配置仅用于受控测试环境。共享环境或生产环境建议开启访问控制、认证和网络隔离。

**2. 机器 A / 机器 B：配置 ccache**
```bash
apt install redis-tools
# 验证 Redis 连接
redis-cli -h <C_IP> -p 6379 -a <PASSWORD> ping
# 配置 Redis 作为二级存储，带密码认证格式
# 格式：redis://default:<PASSWORD>@<C_IP>:6379
ccache --set-config=secondary_storage=redis://default:<PASSWORD>@<C_IP>:6379
# 配置编译器内容校验，避免路径差异导致缓存未命中
ccache --set-config=compiler_check=content
```

**3. 机器 A / 机器 B：执行构建**
```bash
ccache -Cz
rm -rf build && mkdir -p build && cd build
cmake -DENABLE_CCACHE=ON .. && make -j binary package
```

机器 A 构建会写入本地缓存和 Redis，机器 B 本地缓存为空，可从 Redis 读取共享缓存。机器 A 构建后，在机器 B 上执行构建，对比两次 `ccache --show-stats -v` 结果，可看到在机器 B 上远程缓存命中率 `Remote storage Hits` 显著提升。
