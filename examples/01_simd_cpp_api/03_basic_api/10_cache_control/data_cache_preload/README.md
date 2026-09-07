# DataCachePreload 间接索引预加载样例

## 概述

本样例演示[DataCachePreload](../../../../../docs/zh/api/SIMD-API/basic_api/cache_control/DataCachePreload.md)在间接索引（随机跳转）场景下的使用方法：通过Scalar单元读取GM中分散的数据时，利用DataCachePreload提前将即将访问的Cache Line加载到DCache，减少Scalar访存等待时间，提升性能。

**本样例为测试验证场景，并非实际算子使用场景。**

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```text
├── data_cache_preload
│   ├── scripts
│   │   ├── gen_data.py             // 输入数据和真值数据生成脚本
│   │   └── verify_result.py        // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt              // 编译工程文件
│   ├── data_utils.h                // 数据读入写出函数
│   ├── data_cache_preload.asc      // Ascend C样例实现 & 调用样例
│   └── README.md                   // 样例说明文档
```

## 样例描述

本样例通过SCENARIO_NUM控制执行分支，该变量的不同取值对应是否使用DataCachePreload。

| SCENARIO_NUM取值 | 业务场景 | 说明 |
|-----------------|---------|------|
| 1 | 间接索引累加（无preload） | 直接Scalar读取，不使用DataCachePreload |
| 2 | 间接索引累加（有preload） | 使用DataCachePreload提前预取下一轮数据到DCache |

### 计算公式与样例规格

两个场景的计算公式相同，均为间接索引累加求和：

$$
result = \sum_{i=0}^{255} data\_gm[indices[i]]
$$

- `data_gm`为64KB随机`uint8_t`数据缓冲区。
- `indices`为256个随机偏移量，每个指向不同Cache Line对齐位置。
- 由于`indices`随机分散在64KB范围内，绝大多数Scalar读都会触发DCache miss，导致大量访存延迟。
- 通过`DataCachePreload`在读取当前数据时，异步预取下一轮数据所在的Cache Line到DCache，使Scalar读与GM→DCache搬移形成流水线。

#### SCENARIO_NUM=1（间接索引累加，无DataCachePreload）

样例类型（OpType）：DataCachePreloadNoPreload

| name | size | data type | 描述 |
|------|------|-----------|------|
| data_gm | [65536] | uint8_t | 64KB随机数据缓冲区（1024个Cache Line） |
| indices | [256] | uint32_t | 随机索引，指向Cache Line对齐位置 |
| result | [1] | uint32_t | 累加和（样例输出） |

核函数名：kernel_no_preload

#### SCENARIO_NUM=2（间接索引累加，有DataCachePreload）

样例类型（OpType）：DataCachePreloadWithPreload

| name | size | data type | 描述 |
|------|------|-----------|------|
| data_gm | [65536] | uint8_t | 64KB随机数据缓冲区（1024个Cache Line） |
| indices | [256] | uint32_t | 随机索引，指向Cache Line对齐位置 |
| result | [1] | uint32_t | 累加和（样例输出） |

核函数名：kernel_with_preload

## 样例实现

### SCENARIO_NUM=1（无DataCachePreload）

`kernel_no_preload`核函数实现：

1. 创建`GlobalTensor<uint64_t>`绑定data_gm，将GM数据通过`DataCopy`搬到UB：

    ```cpp
    AscendC::GlobalTensor<uint64_t> dataGlobal;
    dataGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ uint64_t*>(data_gm),
                               bufferSize / sizeof(uint64_t));
    AscendC::LocalTensor<uint64_t> xLocal = ubAllocator.Alloc<uint64_t, dataLen / 8>();
    AscendC::DataCopy(xLocal, dataGlobal, dataLen / 8);
    ```

2. 循环中直接通过Scalar读取`data_gm[indices[i]]`并累加：

    ```cpp
    for (uint32_t i = 0; i < count; i++) {
        uint32_t idx = indices[i];
        sum += data_gm[idx];
    }
    ```

3. 由于indices随机分散在64KB范围内，绝大多数Scalar读触发DCache miss，访存延迟成为主要瓶颈。

### SCENARIO_NUM=2（有DataCachePreload）

`kernel_with_preload`核函数实现：

1. 创建`GlobalTensor<uint64_t>`绑定data_gm，将GM数据通过`DataCopy`搬到UB（与场景1相同）。
2. 计算Cache Line对齐偏移：`cacheOffset = idx & ~63ULL`（对齐到64B边界）。
3. 首轮先调用`DataCachePreload(dataGlobal, cacheOffset)`预加载第一个索引所在的Cache Line。
4. 循环中：读取当前轮数据 → 预加载下一轮所在Cache Line，形成流水线：

    ```cpp
    // 首轮preload
    uint32_t idxCurr = indices[0];
    int64_t cacheOffset = (int64_t)(idxCurr & ~63ULL);
    AscendC::DataCachePreload(dataGlobal, cacheOffset);

    for (uint32_t i = 0; i < count; i++) {
        // 读取当前轮（上一轮已preload，DCache大概率命中）
        uint8_t val = data_gm[idxCurr];
        sum += val;
        // 预取下一轮
        if (i + 1 < count) {
            uint32_t idxNext = indices[i + 1];
            int64_t offsetNext = (int64_t)(idxNext & ~63ULL);
            AscendC::DataCachePreload(dataGlobal, offsetNext);
            idxCurr = idxNext;
        }
    }
    ```

## 性能测试数据与分析

通过`msprof`运行后，可以对比两个核函数的执行时间：

- `kernel_no_preload`：DCache miss率高，Scalar读延迟大。
- `kernel_with_preload`：通过提前preload下一轮数据，DCache命中率大幅提升。

### 性能对比数据

以下性能数据为排除单次执行的随机波动，取5次独立运行的平均值。

#### Atlas A2 训练系列产品（dav-2201架构）

| 指标 | kernel_no_preload | kernel_with_preload | 优化效果 |
|------|------------------|---------------------|---------|
| **Task Duration (us)** | 31.237 | 19.840 | **加速1.57x** |
| **aiv_scalar_time (us)** | 29.254 | 17.748 | **降低39.3%** |

#### Ascend 950系列产品（dav-3510架构）

| 指标 | kernel_no_preload | kernel_with_preload | 优化效果 |
|------|------------------|---------------------|---------|
| **Task Duration (us)** | 31.018 | 20.445 | **加速1.52x** |
| **aiv_scalar_time (us)** | 28.344 | 17.910 | **降低36.8%** |

指标说明：
**Task Duration**：核函数端到端执行耗时，是最直观的性能指标。
**aiv_scalar_time**：Scalar单元耗时，间接索引场景下Scalar访存是主要瓶颈。

#### 5次独立运行详细数据（Atlas A2 训练系列产品）

| 执行次数 | no_preload Duration(us) | with_preload Duration(us) | 加速比 |
|---------|------------------------|--------------------------|--------|
| 第1次 | 31.261 | 20.280 | 1.54x |
| 第2次 | 30.941 | 19.361 | 1.60x |
| 第3次 | 30.361 | 19.341 | 1.57x |
| 第4次 | 31.120 | 19.940 | 1.56x |
| 第5次 | 32.501 | 20.280 | 1.60x |
| **平均** | **31.237** | **19.840** | **1.57x** |

#### 5次独立运行详细数据（Ascend 950系列产品）

| 执行次数 | no_preload Duration(us) | with_preload Duration(us) | 加速比 |
|---------|------------------------|--------------------------|--------|
| 第1次 | 31.130 | 20.375 | 1.53x |
| 第2次 | 30.914 | 20.344 | 1.52x |
| 第3次 | 30.526 | 20.552 | 1.49x |
| 第4次 | 31.554 | 20.399 | 1.55x |
| 第5次 | 30.968 | 20.554 | 1.51x |
| **平均** | **31.018** | **20.445** | **1.52x** |

### 分析结论

**DataCachePreload在间接索引（随机跳转）场景下具有显著的优化效果**，通过将下一轮数据提前加载到DCache，**可有效隐藏Scalar访存延迟，实现约1.5倍的性能加速**。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 样例执行
  本样例通过`SCENARIO_NUM`参数分别运行两个核函数，便于独立采集性能数据：
  - `SCENARIO_NUM=1`：运行`kernel_no_preload`（无DataCachePreload）。
  - `SCENARIO_NUM=2`：运行`kernel_with_preload`（有DataCachePreload）。

  在本样例目录下执行如下命令。

  ```bash
  SCENARIO_NUM=1;                                                           # 设置场景编号（1=无preload，2=有preload）
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py                                            # 生成测试输入数据
  msprof ./demo $SCENARIO_NUM
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用CPU调试或NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=cpu`或`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU架构：dav-2201对应Atlas A2训练系列产品/Atlas A2推理系列产品及Atlas A3训练系列产品/Atlas A3推理系列产品，dav-3510对应Ascend 950系列产品 |
  | `SCENARIO_NUM` | `1`（默认）、`2` | 场景编号：1（无preload）、2（有preload） |

- 执行结果
  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
