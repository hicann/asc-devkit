# thread_group简介

`thread_group`是所有其他协作组类的基类，可视为通用的协作组类型，用于以统一方式表示不同类型的线程组。开发者可使用`thread_group`实现多态，具体可参考[reduce_sum样例](../../../../../../examples/03_simt_api/02_features/01_api_features/04_cooperative_groups/reduce_sum/README.md)。

通过`thread_group`只能调用所有通用协作组均支持的基础接口。

## Public成员函数

```cpp
void sync() const;
unsigned long long size() const;
unsigned long long num_threads() const;
unsigned long long thread_rank() const;
group_type get_type() const;
```
