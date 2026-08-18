# Warp函数

Warp是SIMT线程执行和调度的基本单位。一个线程块内的线程会按照线性线程号被划分为多个Warp，当前一个Warp包含32个线程。同一Warp内的线程执行相同的指令流，但每个线程拥有独立的寄存器和Lane ID，可处理不同的数据地址和分支路径。

Warp函数用于同一Warp内的轻量级线程协作，常见能力包括条件投票、寄存器数据交换、Warp内归约和Lane信息查询。与通过Unified Buffer（UB）进行数据交换相比，Warp函数通常直接在Warp内完成寄存器级通信，适合小范围、低开销的线程协作场景。

## 基本概念

| 概念 | 说明 |
| --- | --- |
| Warp | SIMT执行中的线程分组，当前一个Warp包含32个线程。 |
| Lane | Warp内单个线程对应的执行位置。 |
| Lane ID | 线程在当前Warp内的编号，取值范围为[0, 31]。 |
| 活跃线程 | 当前指令路径上实际参与执行的线程。分支发散时，未进入当前路径的线程不参与该路径上的Warp函数计算。 |
| 分支发散 | 同一Warp内不同线程进入不同分支路径，硬件会分批执行不同路径，未执行当前路径的线程处于非活跃状态。 |

## Warp函数分类

| 类别 |  功能说明 |
| --- |  --- |
| Warp Vote类函数 | 对Warp内活跃线程的条件值进行汇总，获取全真、任意为真、位图或活跃线程掩码。 |
| Warp Shfl类函数 | 在Warp内按Lane ID交换寄存器数据，可用于邻近线程通信、扫描和归约。 |
| Warp Reduce类函数 | 对Warp内活跃线程的输入值执行求和、最大值或最小值归约。 |
| Lane ID类函数 | 获取当前线程的Lane ID或生成基于Lane ID的Lane Mask。 |

## 典型场景

### Warp内条件汇总

Vote类函数适合判断同一Warp内是否存在满足条件的线程，或将每个Lane的条件结果压缩成bit mask。

```cpp
uint32_t active_mask = asc_activemask();
uint32_t hit_mask = asc_ballot(value > threshold);
int32_t has_hit = asc_any(value > threshold);
```

### Warp内寄存器交换

Shfl类函数可直接读取同一Warp内其他Lane的寄存器值，减少通过UB共享内存中转数据的开销。

```cpp
int32_t lane = laneid();
int32_t next_value = asc_shfl_down(value, 1);
```

在使用Shfl类函数时，需要确保目标Lane处于活跃状态。如果目标Lane为非活跃状态，读取结果可能是未初始化值。

### Warp内归约

Reduce类函数用于同一Warp内快速完成求和、最大值或最小值计算。

```cpp
float warp_sum = asc_reduce_add(value);
float warp_max = asc_reduce_max(value);
```

对于浮点归约，归约树的计算顺序可能与顺序累加不同，低位舍入结果可能存在差异。

## 使用建议

-   Warp函数只在当前Warp内生效，不能用于跨Warp或跨线程块的数据同步。
-   Warp函数面向活跃线程执行。若代码存在分支发散，需要确认参与计算的Lane集合符合预期。
-   Shfl类函数读取其他Lane的寄存器值时，应避免读取非活跃Lane的数据。
-   Reduce类函数适合Warp内小范围归约；跨Warp或跨线程块归约通常需要结合UB、同步机制或原子操作。
-   Warp函数不等价于内存栅栏。需要约束内存可见性或线程块内阶段同步时，请使用[同步与内存栅栏](../sync_and_memory_fence/sync_and_memory_fence.md)相关接口。
