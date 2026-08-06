# 访存函数简介

SIMT编程通常需要大量Thread并行访问Global Memory。访存函数用于显式控制Global Memory访问时的缓存路径和缓存一致性维护方式，帮助开发者根据数据复用特征、写回方式和跨核可见性要求选择合适的加载、存储或Cache刷新接口。

参考通用SIMT编程经验，同一Warp内线程访问连续、对齐的Global Memory地址时，通常更有利于形成高效访存；访存地址离散、未对齐或频繁跨Cache Line访问时，可能增加访存事务数量并降低吞吐。开发者应结合数据布局、访问模式和缓存复用情况选择访存接口。

## 基本概念

| 概念 | 说明 |
| --- | --- |
| Global Memory | 所有Thread均可访问的全局内存空间，是访存函数的主要访问对象。 |
| L2 Cache | Global Memory访问路径上的缓存层级，可缓存从Global Memory读取或写入的数据。 |
| Data Cache | SIMT线程访问Global Memory时使用的缓存空间，适合存在局部复用的数据访问。 |
| Cache Line | Cache维护和刷新时的基本粒度。`asc_dcci_single`接口用于刷新指定地址所在的Cache Line。 |

## API列表

| 类别 | 接口名 | 功能说明 |
| --- | --- | --- |
| 加载函数 | [asc\_ldcg](asc_ldcg.md) | 从L2 Cache加载数据；未命中时从Global Memory读取，并缓存到L2 Cache。 |
| 加载函数 | [asc\_ldca](asc_ldca.md) | 优先从Data Cache加载数据；未命中时尝试从L2 Cache或Global Memory读取，并缓存到L2 Cache和Data Cache。 |
| 存储函数 | [asc\_stcg](asc_stcg.md) | 将数据存储到Global Memory，并缓存到L2 Cache，不缓存到Data Cache。 |
| 存储函数 | [asc\_stwt](asc_stwt.md) | 将数据存储到Global Memory，并缓存到Data Cache和L2 Cache。 |
| Cache刷新函数 | [asc\_dcci\_single](asc_dcci_single.md) | 刷新指定地址所在的Cache Line，保证后续读取时Cache一致性。 |
| Cache刷新函数 | [asc\_dcci\_entire](asc_dcci_entire.md) | 刷新核内整个Data Cache，保证后续读取时Cache一致性。该接口开销较大，性能敏感场景慎用。 |
| 空操作函数 | [asc\_nop](asc_nop.md) | 生成一条空操作指令，占用当前线程15个时钟周期，但不执行任何实际计算与访存操作。 |

## 接口选择

### 加载路径选择

当数据存在局部复用，且后续仍可能由当前线程或当前核再次访问时，可以考虑使用`asc_ldca`，使数据同时进入Data Cache和L2 Cache。若数据主要依赖L2 Cache，或希望避免写入Data Cache，可使用`asc_ldcg`。

```cpp
float value = asc_ldca(input + idx);
```

### 存储路径选择

`asc_stcg`会将数据写入Global Memory并缓存到L2 Cache，不缓存至Data Cache；`asc_stwt`会将数据写入Global Memory，并缓存至Data Cache和L2 Cache。若写入后的数据可能很快被当前核再次读取，可根据访问复用情况选择`asc_stwt`；若不希望污染Data Cache，可考虑`asc_stcg`。

```cpp
asc_stcg(output + idx, value);
```

### Cache一致性维护

SIMT程序中，写入数据时会立即写入Global Memory，使其他核可见。当从Global Memory读取的数据可能已被其他核修改时，可以使用DCCI类接口刷新Data Cache，以获取Global Memory中的最新数据。

`asc_dcci_single`只刷新指定地址所在的Cache Line，适合轮询标志位等小范围刷新场景；`asc_dcci_entire`刷新整个Data Cache，开销较大，应仅在确实需要全量刷新时使用。

```cpp
while (flag[0] != 1U) {
    asc_dcci_single(static_cast<void *>(flag));
}
```

## 使用建议

-   优先让同一Warp内相邻Thread访问连续、对齐的Global Memory地址，以提升访存效率。
-   对连续且对齐的数据，可结合接口支持的数据类型选择`float2`、`float4`、`int2`、`int4`等向量类型，减少单线程访存指令数量。
-   根据数据复用情况选择加载和存储路径，避免不必要地占用Data Cache。
-   跨Thread Block或跨核通过Global Memory传递数据和标志时，需要结合[同步与内存栅栏](../sync_and_memory_fence/sync_and_memory_fence.md)、[原子操作](../atomic_operations/atomic_operations.md)和DCCI类接口设计同步协议。
-   DCCI类接口用于处理Cache一致性问题，不等价于线程同步接口，也不能替代原子操作。
-   `asc_nop`主要用于避免多个线程读写同地址情况下可能发生的卡死问题。
