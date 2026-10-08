# NPU算子异常信息自动Dump样例

> English version: [README_en.md](README_en.md)

## 概述

本样例展示一种基于AscendCL Runtime异常回调的NPU算子异常信息自动dump方案。当NPU kernel运行时发生异常时，通过外挂库自动dump问题算子的异常上下文（时间戳、device/task/stream/thread ID、错误码、kernel名）与kernel参数（args），用于离线问题定位（可按算子args布局还原输入输出tensor）；另提供纯Python实现的输入/输出tensor dump能力，在算子调用边界自动落盘输入/输出tensor，可与异常产物中的device指针交叉验证。整个方案无需修改算子源码。

> **说明：** 本样例为实验特性（experimental），当前通过外挂库方式实现。后续该能力规划合入CANN（支持Python侧自动dump异常信息、dump算子输入/输出tensor等），届时本样例将简化，请以CANN最新版本为准。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|---|---|
| Ascend 950PR&950DT系列产品 | \>= CANN 9.1.0 |
| Atlas A3系列产品 | \>= CANN 9.1.0 |
| Atlas A2系列产品 | \>= CANN 9.1.0 |

## 目录结构介绍

```text
├── python_exception_dump
│   ├── examples
│   │   └── demo_ifa.py                  // demo：正常调用 + 故障注入 + 产物自检
│   ├── npuops_exception_dump
│   │   ├── include
│   │   │   └── npuops_exception_dump.h  // 外挂库对外C接口
│   │   ├── python/npuops                // Python API（debug.py 等）
│   │   ├── src                          // C++ 异常回调实现
│   │   ├── build.sh                     // 一键编译脚本（产物输出到 lib/）
│   │   └── CMakeLists.txt               // 编译工程文件
│   ├── README_en.md
│   └── README.md
```

## 样例描述

本样例提供两类dump能力：

| 能力 | 开启方式 | 产物 |
|---|---|---|
| 异常自动dump（C++外挂库回调） | `enable_exception_dump(["kernel前缀"])` | `{kernel}_dev{id}_task{id}_{ts}_info.txt`（异常公共信息）+ 同前缀`_args.bin`（原始kernel args：各输入/输出tensor的device指针 + tiling，结合算子args布局可离线还原） |
| 输入/输出tensor dump（纯Python，不依赖so、无需编译） | `run_with_tensor_dump(op, tag=...)`（调用边界自动）或`dump_tensors(..., stage=...)`（手动） | `input_{tag}_{name}_dev{idx}_{ts}.bin/.json`（数据 + 元信息；json中`data_ptr`与`_args.bin`对应指针一致，可交叉验证）；`output_*`同理，在算子返回后产出；注意kernel异步异常场景（下发后才检出）返回值虽已落盘，但kernel未写回，数据不可信，输出基准以正常路径dump为准 |

要点：kernel name前缀过滤（不匹配仅打一条skip日志）；重复开启幂等；回调注册接口按CANN版本运行期自适应（同一份so兼容新旧接口）；kernel异常时输出tensor未写回，异常路径产出的`output_*`数据不可信，输出基准以正常路径dump为准；输出指针现场由`_args.bin`提供。

配置（环境变量）：

| 环境变量 | 说明 | 默认值 |
|---|---|---|
| `NPUOPS_DUMP_DIR` | dump目录（两类能力共用） | `./exception_dump/` |
| `NPUOPS_DUMP_LEVEL` | dump级别：0=仅公共信息，1=公共信息+args（仅影响异常dump） | `1` |
| `NPUOPS_DUMP_LIB_DIR` | 外挂库so所在目录 | 工程内`lib/` |

## 编译运行

- 安装PyTorch以及Ascend Extension for PyTorch插件

  请参考[pytorch: Ascend Extension for PyTorch](https://gitcode.com/Ascend/pytorch)开源代码仓或[Ascend Extension for PyTorch昇腾社区](https://hiascend.com/document/redirect/Pytorch-index)的安装说明，选取支持的`Python`版本配套发行版，完成`torch`和`torch-npu`的安装。

- 安装前置依赖

  ```bash
  pip3 install ascend_ops    # demo调用的IFA算子所在pip包
  ```

- 配置环境变量

  请根据当前环境上CANN开发套件包的安装方式，配置环境变量。

  ```bash
  source ${install_path}/ascend-toolkit/set_env.sh
  ```

  > **说明：** `${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 样例执行

  在本样例根目录下执行如下步骤，编译外挂库并运行demo。

  ```bash
  cd npuops_exception_dump && bash build.sh && cd ..    # 编译外挂库so（产物输出到 lib/）
  python3 examples/demo_ifa.py                          # 执行demo
  ```

  > **说明：** 外挂库so为环境相关二进制、不入库，运行前须先编译；tensor dump为纯Python，无此要求。

- 执行结果

  demo流程：正常调用IFA算子（调用边界自动dump输入/输出）→ 故障注入污染block_table触发AICore kernel异常 → 外挂库回调自动dump异常信息与args → 脚本自检产物。全部通过时退出码0，关键输出如下：

  ```text
  [SIZE-OK] args.bin: 1264 bytes == expected 1264
  [INFO-OK] kernel_name / error_code / args_dumped（3条）
  [INPUT-OK] / [OUTPUT-OK] / query大小 / 污染block_table读回
  [PASS] IFA exception dump OK
  ```

  产物输出至包根`./exception_dump_ifa/`。错误码具体数值与CANN版本相关（不同版本可能不同，判定以kernel_name命中 + args落盘为准）。

## FAQ

| 现象 | 处理 |
|---|---|
| 日志`kernel '...' not in enabled prefix list, skip dump` | 按日志实际kernel名修正前缀，或不传参（对全部kernel生效） |
| dump目录没有`_args.bin` | `NPUOPS_DUMP_LEVEL`被设为0（仅公共信息），改回1 |
| 异常调用后`output_*`缺失或数据无效 | 正常行为：kernel异常时输出未写回（异步检出场景可能落盘部分输出，其数据无效）；输出基准需在正常路径dump（见样例描述要点） |
| `_info.txt`中`args_dumped : no` / 日志`args unavailable` | CANN异常信息缺args，检查CANN版本 |
| `FileNotFoundError: libnpuops_exception_dump.so not found` | 先编译：`cd npuops_exception_dump && bash build.sh`（so二进制不入库）；或设`NPUOPS_DUMP_LIB_DIR`指向so目录（仅异常dump需要so，tensor dump不受影响） |
| 找不到`npuops`模块 | 将`npuops_exception_dump/python`加入`PYTHONPATH`（demo脚本已自动处理） |
| 离线读回`input_*` / `output_*` tensor | 任意有torch的环境即可（无需NPU）：按`.json`元信息用`torch.frombuffer`以字节视角还原 |

## 接入自己的算子

```python
from npuops.debug import enable_exception_dump, run_with_tensor_dump, dump_tensors

# 1) 开启异常dump：前缀取demangle后的kernel函数名（模板参数已去掉），
#    不确定时以异常日志Kernel Name打印值为准；不传参则对全部kernel生效；幂等
enable_exception_dump(["my_kernel"])

# 2)（可选）自动dump输入/输出tensor：异常时输入已落盘、输出不可得（kernel未写回）
out = run_with_tensor_dump(torch.ops.my_lib.my_op, tag="my_op", *args, **kwargs)

# 3)（可选）手动dump指定tensor
dump_tensors({"query": q, "block_table": bt}, stage="input")
```

无需修改任何C++代码、无需重编业务工程；仅异常dump需加载so，tensor dump为纯Python。验证：触发kernel异常后，确认dump目录生成`_args.bin`与`_info.txt`（error_code / kernel_name与Runtime日志一致），使用了步骤2/3时另确认`input_*` / `output_*`产物已生成。
