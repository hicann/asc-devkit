# DataCacheCleanAndInvalid<a name="ZH-CN_TOPIC_0000001787873306"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_cache_intf.h"`。

如图1所示，在AI Core内部，Scalar单元和DMA单元都可能对GM进行访问：

- DMA搬运单元读写GM，数据通过DataCopy等接口在UB等Local Memory和GM间交互，没有Cache一致性问题。
- Scalar单元访问GM，首先会访问每个核内的Data Cache，因此存在Data Cache与GM的Cache一致性问题。

**图1**  DataCache内存层次示意图<a name="zh-cn_topic_0000002530021782_fig1161014168448"></a>  

![](../../../figures/datacache.png)

DataCacheCleanAndInvalid接口用于解决上述一致性问题，其功能可以拆解为两部分：

- Clean：将缓存中被修改过的数据（脏数据，Dirty Data）写回到GM中，避免数据丢失。
- Invalid：
    - 将指定地址范围的Cache Line标记为“无效”，使其从缓存中移除。
    - 保证下一次访问这些内存地址时，会从GM重新加载数据，而不是使用可能过期的缓存数据。

该接口的典型使用场景如下：

- 读取GM的数据，但该数据可能在外部被其余核修改，此时需要使用DataCacheCleanAndInvalid接口，直接访问GM，获取最新数据。
- 用户通过Scalar单元写GM的数据，希望立刻写出，也需要使用DataCacheCleanAndInvalid接口。

<!-- npu="950" id15 -->
- 针对Ascend 950PR/Ascend 950DT，原子操作过程中，如果希望改变后续数据的饱和模式，需要先使用DataCacheCleanAndInvalid接口将Cache Line中现存的数据立刻写出，再调用[SetCtrlSpr](../特殊寄存器访问/SetCtrlSpr_ISASI.md)设置后续数据的饱和模式。
<!-- end id15 -->

## 函数原型<a name="section620mcpsimp"></a>

- 配置dcciDst的原型

  ```cpp
  // 支持通过配置dcciDst确保DCache与GM存储的一致性。
  template <typename T, CacheLine entireType, DcciDst dcciDst>
  __aicore__ inline void DataCacheCleanAndInvalid(const GlobalTensor<T>& dst)
  // 支持通过配置dcciDst确保DCache与Local Memory存储的一致性。
  template <typename T, CacheLine entireType, DcciDst dcciDst>
  __aicore__ inline void DataCacheCleanAndInvalid(const LocalTensor<T>& dst)
  ```

- 不支持配置dcciDst，仅支持保证DCache与GM的一致性。

    ```cpp
    template <typename T, CacheLine entireType>
    __aicore__ inline void DataCacheCleanAndInvalid(const GlobalTensor<T>& dst)
    ```

<!-- npu="310p" id14 -->
> [!NOTE]说明 
> 对于Atlas 推理系列产品AI Core，仅支持不配置dcciDst参数的原型。
<!-- end id14 -->


## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
|--------|------|
| T | dst的数据类型。 |
| entireType | 指令操作的模式：<br>SINGLE_CACHE_LINE：只刷新传入地址所在的Cache Line，**注意如果该地址非64B对齐，只会操作传入地址到64B对齐的部分。**<br>ENTIRE_DATA_CACHE：此时传入的地址无效，核内会刷新整个DCache，但是耗时较大，**性能敏感的场景慎用**。 |
| dcciDst | 表示使用该接口来保证DCache与哪一种存储保持一致性，类型为DcciDst枚举类。<br>&bull; CACHELINE_ALL：与CACHELINE_OUT效果一致。<br>&bull; CACHELINE_UB：表示通过该接口来保证DCache与UB的一致性。<br>&bull; CACHELINE_OUT：表示通过该接口来保证DCache与GM的一致性。<br>&bull; CACHELINE_ATOMIC：原子操作过程中保证DCache和GM的一致性。<br><!-- npu="950" id16 -->&ensp;&ensp;- Ascend 950PR/Ascend 950DT，支持。<br><!-- end id16 --><!-- npu="A3" id10 -->&ensp;&ensp;- Atlas A3 训练系列产品/Atlas A3 推理系列产品，暂未支持。<br><!-- end id10 --><!-- npu="910b" id11 -->&ensp;&ensp;- Atlas A2 训练系列产品/Atlas A2 推理系列产品，暂未支持。<br><!-- end id11 --><!-- npu="310b" id12 -->&ensp;&ensp;- Atlas 200I/500 A2 推理产品，暂未支持。<br><!-- end id12 --><!-- npu="310p" id13 -->&ensp;&ensp;- Atlas 推理系列产品AI Core，暂未支持。<!-- end id13 --> |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
|--------|-----------|------|
| dst | 输入 | 需要刷新Cache的Tensor。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section837496171220"></a>

- **示例1：SINGLE\_CACHE\_LINE模式（64B对齐场景）**  
  假设mmAddr\_为0x40（64B对齐），调用一次指令即可刷新整个Cache Line。

    ```cpp
    AscendC::GlobalTensor<uint64_t> global;
    global.SetGlobalBuffer((__gm__ uint64_t*)mmAddr_ + AscendC::GetBlockIdx() * 1024);
    for( int i = 0; i < 8; i++) {
       global.SetValue(i, AscendC::GetBlockIdx());
    }
    // 由于首地址64B对齐，调用DataCacheCleanAndInvalid指令后，会立刻刷新前8个数。
    AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::SINGLE_CACHE_LINE, AscendC::DcciDst::CACHELINE_OUT>(global);
    ```

- **示例2：SINGLE\_CACHE\_LINE模式（非64B对齐场景）**  
  假设mmAddr\_为0x20（非64B对齐），需要调用两次指令才能刷新完整Cache Line。

    ```cpp
    AscendC::GlobalTensor<uint64_t> global;
    global.SetGlobalBuffer((__gm__ uint64_t*)mmAddr_ + AscendC::GetBlockIdx() * 1024);
    for( int i = 0; i < 8; i++) {
       global.SetValue(i, AscendC::GetBlockIdx());
    }
    // 由于首地址非64B对齐，调用1条指令，只会刷新起始地址至64B字节对齐的部分，即前4个数。
    AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::SINGLE_CACHE_LINE, AscendC::DcciDst::CACHELINE_OUT>(global);
    // 需要再次调用DataCacheCleanAndInvalid指令，刷新后4个数。
    AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::SINGLE_CACHE_LINE, AscendC::DcciDst::CACHELINE_OUT>(global[4]);
    ```

- **<a name="example3_multi_core"></a>示例3：SINGLE\_CACHE\_LINE模式（多核处理场景）**  
  假设mmAddr\_为0x40（64B对齐），多核操作同一条Cache Line时可能出现随机覆盖。本样例仅做示例说明，便于开发者理解使用限制，非正常使用样例。

    ```cpp
    AscendC::GlobalTensor<uint64_t> global;
    global.SetGlobalBuffer((__gm__ uint64_t*)mmAddr_);
    global.SetValue(AscendC::GetBlockIdx(), AscendC::GetBlockIdx());
    // 算子中多核操作虽然不在同一个地址，但在同一个Cache Line，会出现数据的随机覆盖，和通用CPU的行为不同。
    // 调用DataCacheCleanAndInvalid指令后，由于多核操作的时间不一致，最终结果存在随机性，后执行的核会覆盖前面核的结果。
    AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::SINGLE_CACHE_LINE, AscendC::DcciDst::CACHELINE_OUT>(global);
    ```

- **示例4：ENTIRE\_DATA\_CACHE模式**  
  假设mmAddr\_为0x20（非64B对齐），刷新整个DCache。本样例仅做示例说明，便于开发者理解使用限制，非正常使用样例。

    ```cpp
    AscendC::GlobalTensor<uint64_t> global;
    global.SetGlobalBuffer((__gm__ uint64_t*)mmAddr_ + AscendC::GetBlockIdx() * 1024);
    for( int i = 0; i < 8; i++) {
       global.SetValue(i, AscendC::GetBlockIdx());
    }
    // 刷新整个DCache，性能较差。
    AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::ENTIRE_DATA_CACHE, AscendC::DcciDst::CACHELINE_OUT>(global);
    ```
