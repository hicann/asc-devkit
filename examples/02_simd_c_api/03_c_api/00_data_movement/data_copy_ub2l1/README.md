# data_copy_ub2l1样例

## 概述

本样例使用Ascend C C API将矩阵数据从UB（Unified Buffer）搬运到L1 Buffer。样例保留原有Nz输入和ND输入两种UB->L1矩阵乘场景，将矩阵规格调整为非对齐shape；同时新增GM->UB->L1 Buffer->UB->GM双向数据通路场景。本样例适用于Ascend 950PR/Ascend 950DT（`dav-3510`），可在NPU运行模式或NPU仿真模式下执行。

## 本样例支持的产品和CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## 目录结构介绍

```
├── data_copy_ub2l1
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   │   └── verify_result.py           // 输出数据与真值数据比对脚本
│   ├── CMakeLists.txt                 // CMake编译文件
│   ├── data_utils.h                   // 数据读写辅助函数
│   ├── data_copy_ub2l1.asc            // C API样例实现及调用代码
│   ├── README.md                      // 中文样例说明文档
│   └── README_en.md                   // 英文样例说明文档
```

## 场景详细说明

通过编译参数`SCENARIO_NUM`选择输入场景。场景1和场景2是原样例已有的UB->L1矩阵乘场景，仅将矩阵规格改为非对齐`[M, K, N] = [127, 129, 130]`；场景3为新增的GM->UB->L1 Buffer->UB->GM双向通路场景。参与Nz格式搬运和矩阵乘的L1 Buffer存储按照C0或Cube块补齐：A按`[128, 144]`存储，B按`[144, 144]`存储。核函数名称为`data_copy_ub2l1`。

**表1：SCENARIO_NUM取值**

| SCENARIO_NUM | 输入格式 | 输入数据类型 | 输出数据类型 | 数据通路 |
|---|---|---|---|---|
| 1 | 补齐后的Nz | half | float | 原有Nz输入场景：GM->UB->L1 Buffer->L0->GM，UB到L1连续搬运 |
| 2 | 紧凑ND | half | float | 原有ND输入场景：GM->UB补零对齐，在UB中ND转Nz后搬运到L1，再完成矩阵乘 |
| 3 | 补齐后的Nz | half | half | 新增双向通路场景：GM->UB->L1 Buffer->UB->GM |

`SCENARIO_NUM`由CMake作为编译期宏传入，核函数通过`if constexpr`选择对应场景。切换场景后，需要重新编译。

**场景1：原有Nz输入场景，输入数据类型half**

- 输入：A逻辑规格为`[127, 129]`，按`[128, 144]`补齐后以`half` Nz格式存储；B逻辑规格为`[129, 130]`，按`[144, 144]`补齐后以`half` Nz格式存储
- 输出：C `[127, 130]`，`float`类型，ND格式
- 实现：AIV侧先通过`asc_copy_gm2ub`将A、B从GM搬入UB；`asc_sync_notify`和`asc_sync_wait`建立MTE2到MTE3依赖后，使用`asc_copy_ub2l1`将补齐后的Nz格式数据连续搬运到L1 Buffer

**场景2：原有ND输入场景，输入数据类型half**

- 输入：A `[127, 129]`，`half`类型，紧凑ND格式；B `[129, 130]`，`half`类型，紧凑ND格式
- 输出：C `[127, 130]`，`float`类型，ND格式
- 实现：AIV侧通过`asc_ndim_copy_gm2ub`将非对齐ND输入从GM搬入UB，并在右侧列和底部行补零到对齐规格。在MTE2到V同步后，`copy_ub_nd_to_nz`按C0列块调用`asc_copy_ub2ub`，将补齐后的ND格式数据重排为Nz格式。A和B使用独立的UB临时区，避免两次重排和后续UB到L1搬运之间发生读写覆盖。V到MTE3同步完成后，使用`asc_copy_ub2l1`将Nz格式数据连续搬运到L1 Buffer

**场景3：新增GM->UB->L1 Buffer->UB->GM双向通路**

- 输入：A逻辑规格为`[127, 129]`，按`[128, 144]`补齐后以`half` Nz格式存储；B输入不参与该场景
- 输出：A经L1往返后的`[128, 144]` Nz格式数据，`half`类型
- 实现：场景3使用`asc_get_phy_buf_addr`按固定offset管理UB和L1地址，避免AIC写入的UB地址与AIV读取的UB地址不一致。AIV侧通过`asc_copy_gm2ub`和`asc_copy_ub2l1`完成GM->UB->L1搬运；AIC侧等待L1数据就绪后，通过`asc_copy_l12ub_sync`完成L1 Buffer->UB同步搬运；随后AIV侧等待AIC通知，再通过`asc_copy_ub2gm`将UB数据写回GM。

### 数据流与同步

1. 场景1中，AIV侧调用`asc_copy_gm2ub`，通过MTE2将补齐后的Nz格式数据从GM搬运到UB。场景2中，AIV侧调用`asc_ndim_copy_gm2ub`，将紧凑ND输入搬入补齐后的UB区域，并通过`asc_set_ndim_pad_value`和`asc_set_ndim_pad_count`补零。
2. 场景1中，AIV侧建立PIPE_MTE2和PIPE_MTE3同步后，调用`asc_copy_ub2l1`将Nz格式数据从UB连续搬运到L1 Buffer。场景2中，AIV侧先建立PIPE_MTE2到PIPE_V同步，再调用`copy_ub_nd_to_nz`，由其按C0列块调用`asc_copy_ub2ub`将UB中的ND格式数据重排为Nz格式数据；随后建立V到MTE3同步，并调用`asc_copy_ub2l1`将数据从UB连续搬运到L1 Buffer。
3. 场景1和场景2中，`__mix__(1, 2)`启动两个AIV。AIV0完成实际数据搬运后调用`asc_sync_intra_arrive`通知AIC侧，两个AIV均调用[`asc_sync_block_arrive`](../../../../../docs/zh/api/SIMD-API/c_api/sync/asc_sync_block_arrive.md)参与组内同步；AIC侧先通过`asc_sync_intra_wait`等待，再通过`asc_sync_block_wait`等待组内同步完成后读取L1 Buffer数据。
4. 场景1和场景2中，AIC侧调用`asc_copy_l12l0a`和`asc_copy_l12l0b_transpose`，将L1 Buffer中的Nz格式数据分别搬运到L0A Buffer和L0B Buffer；再通过`asc_sync_notify`和`asc_sync_wait`建立MTE1到M同步，调用`asc_mmad`完成非对齐逻辑规格的矩阵乘计算。
5. 场景1和场景2中，AIC侧通过`asc_sync_notify`和`asc_sync_wait`建立M到FIX同步后，调用`asc_set_l0c2gm_nz2nd`配置Nz格式到ND格式的转换，再通过Fixpipe接口`asc_copy_l0c2gm`将`[127, 130]`的计算结果搬运到GM。
6. 场景3中，AIV0使用固定UB offset完成GM->UB->L1 Buffer后通过`asc_sync_intra_arrive`通知AIC侧，两个AIV均调用`asc_sync_block_arrive`参与组内同步；AIC侧将L1 Buffer数据同步搬运到AIV0对应的固定UB offset，并通过`asc_sync_intra_arrive`通知AIV0；AIV0通过`asc_copy_ub2gm`将往返后的数据写回GM。

上述过程按需建立流水依赖；核函数结束时调用`asc_sync_pipe(PIPE_ALL)`，确保全部流水完成。

## 编译运行

在样例根目录下执行以下步骤编译并运行样例。

- 配置环境变量

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：**`${install_path}`为CANN软件包安装目录；root用户默认目录为`/usr/local/Ascend`，非root用户默认目录为`${HOME}/Ascend`。

- 运行样例

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # 编译工程，默认NPU模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据和真值数据
  ./demo                           # 执行样例
  python3 ../scripts/verify_result.py output/output.bin ./output/golden.bin -scenarioNum=$SCENARIO_NUM  # 验证输出结果
  ```

  `SCENARIO_NUM`与`-scenarioNum`必须设置为相同值，取值为1、2或3。场景3输出为`half`类型的Nz格式往返数据，校验脚本会按`half`逐元素比对。

  使用NPU仿真模式时，在CMake命令中增加`-DCMAKE_ASC_RUN_MODE=sim`：

  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;
  ```

  > **注意：**切换运行模式或场景前，需要清理CMake缓存。可在build目录中执行`rm CMakeCache.txt`后重新执行CMake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构，对应Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`、`2`、`3` | 场景编号 |

- 执行结果

  精度比对成功时，输出如下：

  ```bash
  test pass!
  ```
