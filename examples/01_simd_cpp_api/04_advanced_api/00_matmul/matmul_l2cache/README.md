# Matmul L2 Cache切分功能直调样例
## 概述
使能L2 Cache切分特性的Matmul样例，提高L2 Cache的利用率。

Matmul样例M方向或者N方向切分输入的矩阵，将矩阵切分成多块数据，整体按数据切分的块数，分多次进行计算。每次计算前，第一个核第一次访问Global Memory上的矩阵时，会将切分后的一块矩阵数据加载到L2 Cache，其它核或第一个核后续数据访问都可以命中L2 Cache，以提高样例性能。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.2.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.2.0 |

## 目录结构介绍
```
├── matmul_l2cache
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── l2_cache_optimizer.h    // L2Cache切分算法实现函数
│   ├── matmul_l2cache.asc      // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```
## 样例描述
- 样例功能：  
  本样例以AI处理器的L2 Cache大小192M为例，根据样例的输入、输出shape，计算样例输入和输出的总数据量为((30720 * 4096) + (4096 * 1024) + (1024) + (30720 * 1024)) * 2 = 322963456字节（约308M），大于L2 Cache（192M），无法保证计算前读取的数据能够命中L2 Cache，而Global Memory带宽低于L2 Cache，且两者之间差距较大，搬运数据成为样例运行的性能瓶颈。因此需要将输入数据切分成多块，使得每个数据块的计算数据量（包含输入和输出），能够命中L2 Cache。本样例提供了L2CacheOptimizer类，其中GetTileNum接口用于自动根据左右矩阵的Shape获取左右矩阵总L2切分份数，GetBlockShape接口获取L2切分后M、N、K轴的长度，GetBlockCoord接口返回对应切块的位置坐标，即M、N、K方向相对矩阵起始位置的偏移。

- 样例规格：  
  本样例中：M = 30720, N = 1024, K = 4096。
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="5" align="center">Matmul</td></tr>
  <tr><td rowspan="4" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">c</td>
  <td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="5" align="center">matmul_l2cache_custom</td></tr>
  </table>
- 样例实现： 
  - Kernel关键步骤
    - 设置左矩阵A、右矩阵B、Bias。根据L2CacheOptimizer类的GetTileNum接口获取左右矩阵总L2切分份数，循环多次计算。
      ```cpp
      L2CacheOpt l2Opt(shapes, blockNum);
      matmulObj.SetOrgShape(shapes.m, shapes.n, shapes.k);
      for (int64_t tileIdx = curBlockIdx; tileIdx < l2Opt.GetTileNum(); tileIdx += blockNum) {
          auto blockShape = l2Opt.GetBlockShape(tileIdx);  // 获取单次计算L2切分块大小
          if (Get<0>(blockShape) <= 0 || Get<1>(blockShape) <= 0) {
              return;
          }
          auto blockCoord = l2Opt.GetBlockCoord(tileIdx);  // 获取当前计算下标blockCoord
          matmulObj.SetTail(Get<0>(blockShape), Get<1>(blockShape), Get<2>(blockShape));
          const auto& offsetCoord = CalcOffset(shapes, blockCoord); // 基于下标计算矩阵偏移
          int64_t offsetA = Get<0>(offsetCoord);
          int64_t offsetB = Get<1>(offsetCoord);
          int64_t offsetC = Get<2>(offsetCoord);
          matmulObj.SetTensorA(aGlobal[offsetA], false);
          matmulObj.SetTensorB(bGlobal[offsetB], false);
          if (shapes.isBias) {
              matmulObj.SetBias(biasGlobal);
          }
          matmulObj.IterateAll(cGlobal[offsetC]);  // 计算L2切分块
      }
      matmulObj.End();
      ```

  - Tiling关键步骤
    - 本样例采取了常量化Tiling计算，在Kernel侧通过设置一组固定的基本块信息，其他Tiling信息在Kernel侧通过常量化推导，Kernel侧不再需要运行时Tiling信息。基于这组最优的基本块信息，能够适用输入Shape中M，N较大的场景。样例中提供了一种L2Cache切分算法（参考样例L2CacheOptimizer类）。该算法当前在Kernel侧完成计算L2切块份数，也可以自行迁移代码至Host侧计算。
    - L2CacheOptimizer具体计算步骤：
      - 判断是否需要进行L2分块
        ```cpp
        bool smallDim = mTileNum_ < L1_MIN_UST_DIM && nTileNum_ < L1_MIN_UST_DIM;
        if (smallDim || (!EnableL2Tile())) { // 判断计算数据总量是否小于L2Cache阈值
            mL2TileNum_ = mTileNum_;
            nL2TileNum_ = nTileNum_;
            mL2BlockNum_ = 1;
            nL2BlockNum_ = 1;
            return; // 不需要切分，提前返回
        }
        InitL2TileTail(); // 计算L2切分
        ```
      - 基于负载均衡，计算L2最优分块
        ```cpp
        int64_t mConflict = INT64_MAX;
        int64_t nConflict = INT64_MAX;
        constexpr bool isNMajor = l1N > l1M; // 根据shape大小，判断主维度
        for (int64_t i = maxMajor; i >= L1_MIN_UST_DIM; i--) {
            for (int64_t j = maxMinor; j >= minMinor; j--) {
                if (GetTotalSize(j * l1M, i * l1N, k_) <= L2_TILE_THRESHOLD) { // 确保分块小于L2Cache阈值
                    uint64_t mConflictTmp = AscendC::Std::ceil_div(blockNum_, mL2TileNumTailTmp); // 计算负载冲突值
                    uint64_t nConflictTmp = AscendC::Std::ceil_div(blockNum_, nL2TileNumTailTmp);
                    if (mConflict >= mConflictTmp && nConflict >= nConflictTmp) { // 若冲突值更小，更新分块数量
                        mConflict = mConflictTmp;
                        nConflict = nConflictTmp;
                        mL2TileNum_ = curMajorDim;
                        nL2TileNum_ = curMinorDim;
                    }
                }
            }
        }
        ```

  - 调用实现  
    使用内核调用符<<<>>>调用核函数。

## 编译运行
在本样例根目录下执行如下步骤，编译并执行样例。
- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build;    # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py    # 生成测试输入数据
  ./demo                        # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin    # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　|
  | ----------------| -----------------------------| ---------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功：
  ```bash
  test pass!
  ```
