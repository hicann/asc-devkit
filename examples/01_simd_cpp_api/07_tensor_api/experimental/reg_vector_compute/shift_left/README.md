# operator<<接口样例

## 概述

本样例介绍如何使用实验性 Tensor API `asc::te::experimental::operator<<`，按照 `int32_t` 移位量对 `uint32_t` 元素执行逐元素左移。

```text
dst[i] = src[i] << shift[i]
```

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 目录结构介绍

```text
├── CMakeLists.txt                 // 编译工程
├── README.md                      // 样例说明
└── shift_left.asc                 // operator<<接口样例实现
```

## 样例描述

- 样例功能：

  样例将输入数据和移位量从 GM 搬运到 UB，通过 `load` 加载到寄存器，调用 `operator<<` 完成逐元素左移，再将结果写回 UB 和 GM。Host 侧直接生成输入并校验输出。

- 样例场景：

  | SCENARIO_NUM | Tensor Layout | 说明 |
  | --- | --- | --- |
  | 1 | 一维，shape 为 `[128]` | 按一维偏移加载和存储寄存器块。 |
  | 2 | 二维，shape 为 `[2, 64]` | 按二维坐标加载和存储每一行。 |

- 样例实现：

  核心计算代码如下：

  ```cpp
  const auto coord = asc::te::make_coord(0);
  auto mask = asc::te::experimental::all_mask<uint32_t>();
  auto src = asc::te::experimental::load(src_tensor, coord).with_mask(mask);
  auto shift = asc::te::experimental::load(shift_tensor, coord).with_mask(mask);
  auto dst = src << shift;
  asc::te::experimental::store(dst_tensor, coord, dst);
  ```

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  配置 CANN 软件包环境变量，详细操作请参考[环境变量配置](../../../../../../docs/zh/quick_start.md#prepare&install)。

  ```bash
  source ${install_path}/set_env.sh
  ```

  `${install_path}` 为 CANN 软件包安装目录，请根据实际安装路径替换。

- 样例执行

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build
  cmake -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..
  make -j
  ./demo
  ```

  使用 NPU 仿真模式时，在 CMake 命令中增加 `-DCMAKE_ASC_RUN_MODE=sim`。切换运行模式、芯片型号或场景时，建议清除 `build` 目录中的 CMake 缓存后重新配置。

- 执行结果

  验证成功时输出：

  ```text
  test pass!
  ```
