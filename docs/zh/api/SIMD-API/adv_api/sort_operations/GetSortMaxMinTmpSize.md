# GetSortMaxMinTmpSize

## 功能说明

带SortConfig模板参数的kernel侧Sort接口的计算需要开发者预留/申请临时空间，本接口用于在host侧获取预留/申请的最大最小临时空间大小，开发者基于此范围选择合适的空间大小作为Tiling参数传递到kernel侧使用。

-   为保证功能正确，预留/申请的临时空间大小不能小于最小临时空间大小；
-   在最小临时空间-最大临时空间范围内，随着临时空间增大，kernel侧接口计算性能会有一定程度的优化提升。为了达到更好的性能，开发者可以根据实际的内存使用情况进行空间预留/申请。

## 函数原型

```
void GetSortMaxMinTmpSize(const AscendC::TensorShape& srcShape, AscendC::TensorDataType valueType, AscendC::TensorDataType indexType, bool isReuseSource, const SortConfig& config, uint32_t& maxValue, uint32_t& minValue)
```

## 参数说明

**表1**  接口参数列表

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| srcShape | 输入 | 输入的shape信息，参数类型为[AscendC::TensorShape](../data_structures/TensorShape.md)。 |
| valueType | 输入 | 输入、输出Value的数据类型，参数类型为[AscendC::TensorDataType](../data_structures/TensorDataType.md)。与Sort接口的模板参数T保持一致。 |
| indexType | 输入 | 输入、输出Index的数据类型，参数类型为[AscendC::TensorDataType](../data_structures/TensorDataType.md)。与Sort接口的模板参数U保持一致。 |
| isReuseSource | 输入 | 是否复用源操作数输入的空间。与Sort接口的参数isReuseSource保持一致。 |
| config | 输入 | Sort的相应配置：选择的排序算法，排序结果的升降序，输入输出是否带有索引数据。数据类型SortConfig，定义如下方代码所示。其中的参数hasSrcIndex、hasDstIndex与使用的Sort接口是否带有输入索引、输出索引的情况保持一致；当前hasSrcIndex = true, hasDstIndex = false组合不支持。 |
| maxValue | 输出 | Sort接口能完成计算所需的最大临时空间大小，超出该值的空间不会被该接口使用。在最小临时空间-最大临时空间范围内，随着临时空间增大，kernel侧接口计算性能会有一定程度的优化提升。为了达到更好的性能，开发者可以根据实际的内存使用情况进行空间预留/申请。最大空间大小为0表示计算不需要临时空间。<br> 说明：maxValue仅作为参考值，有可能大于Unified Buffer（UB）剩余空间的大小，该场景下，开发者需要根据UB剩余空间的大小来选取合适的临时空间大小。 |
| minValue | 输出 | Sort接口能完成计算所需最小临时空间大小。为保证功能正确，接口计算时预留/申请的临时空间不能小于该数值。最小空间大小为0表示计算不需要临时空间。 |

```
enum class SortType {
    RADIX_SORT, // 使用基排序算法实现
    MERGE_SORT  // 使用归并排序实现
};
struct SortConfig {
    SortType type = SortType::RADIX_SORT; // 排序算法
    bool isDescend = false;               // 是否降序排序，默认值为false，输出结果升序排序
    bool hasSrcIndex = false;             // 是否带有输入索引
    bool hasDstIndex = false;             // 是否带有输出索引
};
```

## 返回值说明

无

## 约束说明

无

## 调用示例

完整的调用样例请参考[更多样例](../math_compute/more_examples.md)。

```
// 输入shape信息为1024;算子输入的数据类型为uint32_t;不允许修改源操作数
std::vector<int64_t> shape_vec = {1024};
AscendC::TensorShape srcShape(shape_vec);
AscendC::TensorDataType valueType = AscendC::TensorDataType::DT_UINT32;
AscendC::TensorDataType indexType = AscendC::TensorDataType::DT_UINT32;
bool isDescend = true;
bool hasSrcIndex = false;
bool hasDstIndex = false;
bool isReuseSource = false;
AscendC::SortConfig config;
config.type = AscendC::SortType::RADIX_SORT;
config.isDescend = isDescend;
config.hasSrcIndex = hasSrcIndex;
config.hasDstIndex = hasDstIndex;
uint32_t maxValue = 0;
uint32_t minValue = 0;
AscendC::GetSortMaxMinTmpSize(srcShape, valueType, indexType, isReuseSource, config, maxValue, minValue);
```
