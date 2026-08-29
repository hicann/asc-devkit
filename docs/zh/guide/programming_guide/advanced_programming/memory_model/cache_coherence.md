# 缓存一致性

本文介绍NPU编程中常见的缓存一致性问题，说明DCache、ICache、SIMT DCache与GM的数据一致性。芯片的硬件架构参考[NPU架构版本2201](../hardware_implementation/architecture_spec/npu_arch_2201.md)和[NPU架构版本3510](../hardware_implementation/architecture_spec/npu_arch_3510.md)。

## 缓存一致性的基本概念

### Cache、Cache Line与缓存副本

Cache用于缓存后续可能重复访问的数据或指令，降低访问GM的开销。一次Cache加载通常不是只加载被访问的单个字节或单个变量，而是把该地址所在的一段连续数据作为一个整体加载到Cache中，这个整体称为Cache Line。

Cache Line是Cache加载、失效、写回的最小操作粒度。例如访问GM内存时，会将GM里一个Cache Line长度内相邻的数据加载到L2 Cache缓存，利用空间局部性提高访问效率。Cache Line是硬件真正搬入、标记、替换和写回缓存数据的最小粒度，直接影响性能和一致性。<!-- npu="950" id1 -->[NPU架构版本3510](../../language_extension/simd_builtin_keywords.md)支持L2 Cache、DCache、ICache、SIMT DCache、NDDMA Cache；<!-- end id1 -->[NPU架构版本2201](../../language_extension/simd_builtin_keywords.md)支持L2 Cache、DCache和ICache。本文讨论的通用Cache单元信息如下：

<!-- npu="950" id15 -->
**NPU架构版本3510Cache单元信息**

| 存储单元 | 位置 | 数据一致性说明 | [NPU架构版本3510](../../language_extension/simd_builtin_keywords.md)Cache Line大小 | [NPU架构版本3510](../../language_extension/simd_builtin_keywords.md)Cache大小 |
| --- | --- | --- | --- | --- |
| L2 Cache | 核外共享缓存 | 多个核共享，不考虑多个核之间的Cache副本不一致问题 | 128B/512B | 128MB |
| AIC ICache | Scalar取指缓存 | 只读，不考虑多个核之间的数据不一致问题 | 128B | 32KB |
| AIV ICache | Scalar取指缓存 | 只读，不考虑多个核之间的数据不一致问题 | 128B | 16KB |
| AIC DCache | Scalar数据缓存 | 每个核独立缓存，需要考虑多个核之间的数据一致性问题 | 64B | 32KB |
| AIV DCache | Scalar数据缓存 | 每个核独立缓存，需要考虑多个核之间的数据一致性问题 | 64B | 32KB |
| SIMT DCache | SIMT数据缓存 | 每个核独立缓存，需要考虑多个核之间的数据一致性问题 | 128B | 32KB~128KB |
<!-- end id15 -->

<!-- npu="A3,910b" id20 -->
**NPU架构版本2201Cache单元信息**

| 存储单元 | 位置 | 数据一致性说明 | [NPU架构版本2201](../../language_extension/simd_builtin_keywords.md)Cache Line大小 | [NPU架构版本2201](../../language_extension/simd_builtin_keywords.md)Cache大小 |
| --- | --- | --- | --- | --- |
| L2 Cache | 核外共享缓存 | 多个核共享，不考虑多个核之间的Cache副本不一致问题 | 512B | 192MB |
| AIC ICache | Scalar取指缓存 | 只读，不考虑多个核之间的数据不一致问题 | 128B | 32KB |
| AIV ICache | Scalar取指缓存 | 只读，不考虑多个核之间的数据不一致问题 | 128B | 16KB |
| AIC DCache | Scalar数据缓存 | 每个核独立缓存，需要考虑多个核之间的数据一致性问题 | 64B | 16KB |
| AIV DCache | Scalar数据缓存 | 每个核独立缓存，需要考虑多个核之间的数据一致性问题 | 64B | 32KB |
<!-- end id20 -->

同一份数据可能同时存在于GM、L2 Cache、DCache<!-- npu="950" id2 -->、SIMT DCache<!-- end id2 -->等多个位置。对于每个核独立维护的Cache，同一地址或同一Cache Line可能在不同核内形成不同副本：

- 某个核已经在本核Cache中修改了数据，但GM或其他核的Cache中仍保留旧值；
- 某个核读取前已经缓存过旧值，即使GM后来被其他路径更新，后续读取仍可能继续命中旧副本。

这类副本不一致就是本文关注的缓存一致性问题。

### Clean、Dirty、Invalid、写策略与DCCI

Cache中的数据可以按与下一级存储的关系理解为以下几类状态：

- **Clean**：Cache Line中的数据与下一级存储中的数据一致，直接丢弃该Cache Line不会丢失更新。
- **Dirty**：Cache Line中的数据已经被当前Cache修改，但尚未写回下一级存储。此时Cache中的数据与下一级存储中的数据不一致，如果其他路径直接读取GM，可能读到旧值。
- **Invalid**：Cache Line无效，后续访问不能继续使用该Cache Line中的旧数据，需要重新从下一级存储加载。

Data Cache Clean and Invalid（[DCCI](../../../../api/SIMD-API/c_api/cache_ctrl/asc_dcci.md)）用于处理Cache副本。Clean操作用于把Dirty Cache Line写回到目标存储，Invalid操作用于使Cache Line失效，避免后续继续命中旧副本。

实际使用时，可根据一致性目标选择不同DCCI接口：使用[`asc_dcci_single`](../../../../api/SIMD-API/c_api/cache_ctrl/asc_dcci.md)刷新指定地址所在的单个Cache Line；使用[`asc_dcci_entire_out`](../../../../api/SIMD-API/c_api/cache_ctrl/asc_dcci.md)刷新面向GM的一致性目标；原子相关目标按产品支持情况选择[`asc_dcci_entire_atomic`](../../../../api/SIMD-API/c_api/cache_ctrl/asc_dcci.md)等接口。

对于需要等待DCCI触发的写回真正完成的场景，还需要配合Data Sync Barrier（[DSB](../../../../api/SIMD-API/c_api/sync/asc_sync_data_barrier.md)）等待前序内存访问完成，DSB只用于阻塞后续指令执行，直到之前的内存访问指令执行结束，参数可选择`DSB_ALL`、`DSB_DDR`或`DSB_UB`。

Cache写入策略决定写操作是否会形成Dirty副本，常见策略包括直写和写回：

- **直写（write-through）**：写操作更新Cache副本的同时，将数据写出到下一级存储。直写路径通常不会留下需要后续Clean回下一级存储的Dirty副本，但写操作是否已经完成仍需要通过DSB等同步机制确认；如果读端已经缓存旧副本，仍需要按读端路径执行Invalid。
- **写回（write-back）**：写操作先更新Cache副本，并将Cache Line标记为Dirty；下一级存储要等到DCCI Clean、Cache Line替换等时机才会被更新。写回路径更容易产生“写端已经执行Store，但GM或Unified Buffer（UB）仍是旧值”的问题。


### 缓存一致性与内存一致性的区别

缓存一致性（cache一致性）关注的是同一地址或同一Cache Line在不同缓存副本之间的值是否一致。例如，某个核的DCache已经把`A`修改为新值，但GM中仍是旧值；或者GM已经被其他路径更新，但当前核DCache仍保留旧副本。此时问题的关键是是否需要写回Dirty副本、失效旧副本，或者绕过Cache访问。

内存一致性关注的是不同执行单元、不同线程或不同核之间的读写顺序是否符合程序预期。例如，生产者先写数据再写标志，消费者看到标志后是否一定能按预期读取到数据。同步接口、内存栅栏主要解决执行顺序和访问完成问题；DCCI主要解决Cache副本写回和失效问题。实际编程时，两类问题经常同时出现，但本文重点讨论Cache副本与GM、UB之间的数据一致性。

以下以DCache为例展示缓存一致性问题，`A`和`B`位于GM上同一条64B DCache Cache Line内：

```text
初始状态：
GM上64B地址长度的数据分布[A=0, B=0, ...]

时间点    Core 0                                  Core 1
======    ====================================    ====================================
T0        Scalar.LOAD A
          // Core 0 DCache缓存整条Cache Line：
          // [A=0, B=0, ...]

T1                                                Scalar.LOAD B
                                                  // Core 1 DCache缓存整条Cache Line：
                                                  // [A=0, B=0, ...]

T2        Scalar.STORE A = 1
          // Core 0 DCache变成：
          // [A=1, B=0, ...]

T3                                                Scalar.STORE B = 2
                                                  // Core 1 DCache变成：
                                                  // [A=0, B=2, ...]
```

两个核虽然修改的是不同字段，但各自DCache里保存的是整条Cache Line的副本。后续写回GM时，如果以整条Cache Line为单位写回，就可能出现不同结果：

```text
Case 1：Core 0先写回，Core 1后写回

时间点    Core 0                                  Core 1
======    ====================================    ====================================
T4        Scalar.DCCI
          // Core 0以Cache Line为粒度写回：
          // GM Cache Line = [A=1, B=0, ...]

T5                                                Scalar.DCCI
                                                  // Core 1以Cache Line为粒度写回：
                                                  // GM Cache Line = [A=0, B=2, ...]

Case 2：Core 1先写回，Core 0后写回

时间点    Core 0                                  Core 1
======    ====================================    ====================================
T4                                                Scalar.DCCI
                                                  // Core 1以Cache Line为粒度写回：
                                                  // GM Cache Line = [A=0, B=2, ...]

T5        Scalar.DCCI
          // Core 0以Cache Line为粒度写回：
          // GM Cache Line = [A=1, B=0, ...]
```

期望结果是：
GM Cache Line = [A=1, B=2, ...]

但是因为每个核都有自己的缓存数据，各个核都是在自己的缓存旧副本上修改同一条Cache Line中的不同字段，所以实际可能结果是：
- Case 1：Core 0先写回，Core 1后写回，最终GM Cache Line = [A=0, B=2, ...]
- Case 2：Core 1先写回，Core 0后写回，最终GM Cache Line = [A=1, B=0, ...]

这个例子由于没有保证缓存一致性，最终没有得到期望的`GM Cache Line = [A=1, B=2, ...]`。

## 缓存一致性

### DCache与GM的缓存一致性

DCache与GM的一致性是NPU缓存一致性中最常见的一类问题。Scalar普通读写GM时会经过当前核内的DCache，而每个AIC/AIV核内都有独立的DCache用于缓存GM里的数据。某个核通过DCache修改GM地址对应的数据后，新值可能先停留在本核DCache中，GM和其他核DCache中的副本不会自动更新。

多个核或多条访问路径操作同一段GM数据时，DCache与GM的不一致通常来自两类副本：

- 写端Dirty副本：生产者已经写入，但新值仍停留在生产者DCache中，GM尚未更新。
- 读端旧副本：消费者此前已经缓存过同一地址或同一Cache Line，后续读取继续命中本核DCache旧值。

Scalar对GM的普通读写行为可以概括为：

- Scalar读GM后，数据会进入DCache；后续读取同一条Cache Line内的数据时，可能直接从DCache读取，不再访问GM。
- Scalar写GM时，只会修改DCache中的Cache Line，并把该Cache Line标记为Dirty。Dirty表示DCache中的Cache Line数据与GM中的数据不一致，但是并不会立刻将Dirty数据写回GM，而是需要用户手动刷新或者该数据段被替换时才会写回GM。

所以，生产侧要关心Dirty数据是否已经写回GM；消费侧要关心本核DCache里是否还保留旧副本。DCCI负责处理“副本是否写回或失效”，DSB和流水同步只负责处理“后续访问是否等到前序访问完成”。

#### Scalar访问GM带来的缓存一致性

Scalar普通读写GM时，`Scalar.STORE`已经执行不等于GM已经被改写。下面用两个核访问同一GM地址`A`说明生产侧Dirty副本和消费侧旧副本问题，示例中的`T0`、`T1`等时间点只表示代码逻辑先后顺序。

```text
初始状态：
GM[A] = 0
Core 0 DCache中没有缓存A
Core 1 DCache中没有缓存A

时间点    Core 0                                  Core 1
======    ====================================    ====================================
T0        Scalar.STORE A = 1
          // Core 0 DCache[A] = 1, Dirty
          // GM[A]仍可能是0

T1                                                // 如果此时Scalar.LOAD A -> 0
                                                  // Core 1可能从GM加载旧值0

T2        Scalar.DCCI A
T3        Scalar.DSB
          // Dirty Cache Line写回完成，GM[A] = 1

T4                                                Scalar.LOAD A -> 1
                                                  // 如果Core 1没有旧DCache副本，可以读到新值1
```

如果Core 1在T3之前没有缓存过`A`，T4再执行`Scalar.LOAD A`可以从GM获取新值`1`；如果Core 1缓存了旧值，则需要继续处理读端旧副本。

如果读端本核DCache中已经缓存过同一地址，还需要考虑旧副本。下面示例中，消费侧使用DCCI读取数据前刷新指定GM地址所在Cache Line：

```text
初始状态：
GM[A] = 0
Core 0 DCache中没有缓存A
Core 1 DCache中没有缓存A

时间点    Core 0                                  Core 1
======    ====================================    ====================================
T0        Scalar.LOAD A -> 0                      Scalar.LOAD A -> 0
          // Core 0 DCache[A] = 0                 // Core 1 DCache[A] = 0

T1        Scalar.STORE A = 1
          // Core 0 DCache[A] = 1, Dirty
          // GM[A]仍可能是0

T2        Scalar.DCCI A

T3        Scalar.DSB
          // Dirty Cache Line写回完成，GM[A] = 1

T4                                                Scalar.LOAD A -> 0
                                                  // Core 1 DCache[A] = 0，Core 1可能继续命中本核旧DCache副本

T5                                                Scalar.DCCI A
                                                  // Core 1旧Cache副本失效

T6                                                Scalar.LOAD A -> 1
                                                  // Core 1 DCache[A] = 1，Core 1重新从GM获取新值
```

上面两个例子分别对应写端Dirty副本和读端旧副本。Scalar路径的Cache处理原则是：

- 生产者通过Scalar写GM后，如果消费者需要从GM或其他路径读取该数据，生产者需要执行DCCI（例如`asc_dcci_single`）把Dirty数据写回GM。
- 消费者如果担心本核DCache中有旧副本，读取前需要执行DCCI使旧副本失效。
- 如果算法本身不需要DCache缓存，可以使用[ReadGmByPassDCache](<../../../../api/SIMD-API/basic_api/scalar_compute/ReadGmByPassDCache_ISASI.md>)和[WriteGmByPassDCache](<../../../../api/SIMD-API/basic_api/scalar_compute/WriteGmByPassDCache_ISASI.md>)直接读写GM。

还有一类问题来自Cache Line粒度。普通Scalar写后续以64B Cache Line为单位写回；多个核即使写的是同一条Cache Line中的不同字段，也可能在各自写回整条Cache Line时覆盖对方结果。此类场景可以通过让不同核操作的地址至少相隔Cache Line大小，或使用ReadGmByPassDCache和WriteGmByPassDCache降低风险。

<!-- npu="950" id3 -->
#### Atomic访问导致的DCache缓存一致性

[NPU架构版本3510](../../language_extension/simd_builtin_keywords.md)支持Scalar原子操作，Scalar原子操作可以看成对某个GM地址执行的读-改-写事件，但是Scalar原子操作会绕过DCache，因此会导致缓存一致性。例如C API [asc_atomic_add](../../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_add.md)，该接口在指定GM地址上执行原子加操作。原子操作保证的是这一次读-改-写不会被其他原子更新打断，但它不会自动清理同一地址在DCache中的旧副本或Dirty副本。

因此，如果同一GM地址此前被普通Scalar路径访问过，就需要分析原子事件和DCache事件的一致性问题：

- 若普通Scalar写先发生，数据可能仍停留在DCache中。后续原子操作如果依赖这个写入，需要先把相关DCache Line写回GM。
- 若原子操作先发生，本核DCache中可能仍保留旧副本。后续再用普通Scalar读同一地址时，需要先使旧副本失效，避免读到原子操作之前的值。

下面用地址`A`和asc_atomic_add说明Atomic与DCache混用时的问题：

```text
Case 1：普通Scalar写在前，Atomic在后
初始状态：
GM[A] = 0
Core 0 DCache中没有缓存A

时间点    Core 0
======    ====================================
T0        Scalar.STORE A = 1
          // Core 0 DCache[A] = 1, Dirty
          // GM[A]仍可能是0

T1        asc_atomic_add(A, 1)
          // Atomic绕过DCache访问GM
          // 如果Atomic从GM旧值0开始累加，GM[A]可能变成1，而不是期望的2
```

上面的关键不是`asc_atomic_add`本身不原子，而是原子操作看到的GM值没有包含此前停留在DCache中的普通Scalar写入。

反过来，如果`asc_atomic_add(A, 1)`已经把GM更新为1，而本核DCache里还保留`A == 0`的旧副本，后续普通Scalar读也可能继续读到旧副本。
```text
Case 2：Atomic在前，普通Scalar读在后
初始状态：
GM[A] = 0
Core 0 DCache中没有缓存A

时间点    Core 0
======    ====================================
T0        Scalar.LOAD A -> 0
          // Core 0 DCache[A] = 0, Clean

T1        asc_atomic_add(A, 1)
          // Atomic绕过DCache更新GM
          // GM[A] = 1
          // Core 0 DCache[A]仍可能是0

T2        Scalar.LOAD A -> 0
          // 普通Scalar读可能继续命中本核旧DCache副本
```

为了避免上述问题，Atomic访问通常需要处理缓存一致性问题。正确的处理流程应该是：
- Case 1：普通Scalar写在前，Atomic在后，需要先把DCache写回GM，再执行Atomic操作。
- Case 2：Atomic在前，普通Scalar读在后，需要先使DCache旧副本失效，再执行普通Scalar读。

以下是正确流程示例：

```text
Case 1：普通Scalar写在前，Atomic在后的正确处理流程
初始状态：
GM[A] = 0
Core 0 DCache中没有缓存A

时间点    Core 0
======    ====================================
T0        Scalar.STORE A = 1
          // Core 0 DCache[A] = 1, Dirty
          // GM[A]仍可能是0

T1        Scalar.DCCI A
T2        Scalar.DSB
          // Dirty Cache Line写回完成，GM[A] = 1

T3        asc_atomic_add(A, 1)
          // Atomic从GM[A] = 1开始累加
          // GM[A] = 2

T4        Scalar.DSB
          // 等待Atomic产生的GM访问完成
```

```text
Case 2：Atomic在前，普通Scalar读在后的正确处理流程
初始状态：
GM[A] = 0
Core 0 DCache中没有缓存A

时间点    Core 0
======    ====================================
T0        Scalar.LOAD A -> 0
          // Core 0 DCache[A] = 0, Clean

T1        asc_atomic_add(A, 1)
T2        Scalar.DSB
          // 等待Atomic产生的GM访问完成，GM[A] = 1

T3        Scalar.DCCI A
          // Core 0旧Cache副本失效

T4        Scalar.LOAD A -> 1
          // 重新从GM获取Atomic更新后的新值
```

<!-- end id3 -->

#### 软件维护Scalar访问GM的DCache一致性

本节介绍如何从软件层面维护Scalar访问GM时导致的DCache一致性。硬件仅维护消费者和生产者都来自同核Scalar DCache操作的场景下的缓存一致性，不会主动维护各核DCache之间的一致性，也不会主动处理Scalar DCache与其他GM访问路径之间的旧副本或Dirty副本问题。因此，当同一GM地址或同一条Cache Line被不同访问路径先后访问时，需要开发者使用DCCI、DSB、流水同步等方式从软件层面维护缓存一致性。

Scalar写GM可以分为两类：
- cache: 普通Scalar写通过DCache修改GM，例如直接写`__gm__`地址；
- no cache: 不经过DCache，直接向GM发起写操作，例如`WriteGmByPassDCache`等。

按照访问顺序，常见需要软件维护一致性的场景可以分为RAW、WAW和WAR三类：

##### RAW：先写后读

RAW（Read After Write）场景要求后序读操作读取到前序写操作产生的新值。维护RAW场景下的缓存一致性时，需要同时处理写端和读端：

- 写端如果通过Scalar DCache写GM，可能只形成Dirty副本，GM中的数据尚未更新，因此通常需要在消费者读取前执行写端DCCI和DSB。
- 读端如果通过可能持有旧副本的带Cache路径读取，需要在真正读取前按表执行读端DCCI。
- 如果写端和读端位于不同流水，还需要插入对应流水同步，保证读操作不会早于写后处理执行。

下表列出同一AI Core内Scalar写GM之后、消费者读取GM完整的软件维护操作序列。假设生产者和消费者访问同一地址，每个单元格都是从写后处理到读前处理的完整操作序列。

表中的cache表示经过cache的访问，no cache表示不经过cache的访问,`VF_SIMT`表示经过cache的SIMT线程函数，MTE2表示经过MTE2的读取操作，MTE2（cache）指[多维数据搬运NDDMA](../../../../api/SIMD-API/c_api/vector_datamove/asc_ndim_copy_gm2ub.md)，MTE2.DCI指NDDMA Cache的[失效操作](../../../../api/SIMD-API/basic_api/memory_vector_compute/data_move_aux_config/NdDmaDci.md)或[asc_ndim_copy_dci](../../../../api/appendix/Release_Notes/CANN_9_1_0.md)。

| 后续读取方式（列，后发生）<br>╲<br>前序写入方式（行，先发生） | Scalar（cache） | Scalar（no cache） | VF_SIMT（cache） | VF_SIMT（no cache） | MTE2（cache） | MTE2（no cache） |
| --- | --- | --- | --- | --- | --- | --- |
| Scalar（cache） | 先写Scalar.STORE<br>后读Scalar.LOAD | 先写Scalar.STORE<br>先写Scalar.DCCI<br>先写Scalar.DSB<br>后读Scalar.LOAD | 先写Scalar.STORE<br>先写Scalar.DCCI<br>先写Scalar.DSB<br>sync S→V/VF<br>后读SIMT.DCCI<br>后读VF_SIMT.LOAD | 先写Scalar.STORE<br>先写Scalar.DCCI<br>先写Scalar.DSB<br>sync S→V/VF<br>后读VF_SIMT.LOAD | 先写Scalar.STORE<br>先写Scalar.DCCI<br>先写Scalar.DSB<br>sync S→MTE2<br>后读MTE2.DCI<br>后读MTE2.LOAD | 先写Scalar.STORE<br>先写Scalar.DCCI<br>先写Scalar.DSB<br>sync S→MTE2<br>后读MTE2.LOAD |
| Scalar（no cache） | 先写Scalar.STORE<br>先写Scalar.DSB<br>后读Scalar.DCCI<br>后读Scalar.LOAD | 先写Scalar.STORE<br>先写Scalar.DSB<br>后读Scalar.LOAD | 先写Scalar.STORE<br>先写Scalar.DSB<br>sync S→V/VF<br>后读SIMT.DCCI<br>后读VF_SIMT.LOAD | 先写Scalar.STORE<br>先写Scalar.DSB<br>sync S→V/VF<br>后读VF_SIMT.LOAD | 先写Scalar.STORE<br>先写Scalar.DSB<br>sync S→MTE2<br>后读MTE2.DCI<br>后读MTE2.LOAD | 先写Scalar.STORE<br>先写Scalar.DSB<br>sync S→MTE2<br>后读MTE2.LOAD |

例如，前序写入方式是`Scalar（cache）`，后续读取方式是`VF_SIMT（cache）`时，对应单元格要求执行：

```text
Scalar.STORE -> Scalar.DCCI -> Scalar.DSB -> sync S→V/VF -> SIMT.DCCI -> VF_SIMT.LOAD
```

含义是：先通过`Scalar.STORE`完成写入；由于该写入可能停留在Scalar DCache中，需要执行`Scalar.DCCI -> Scalar.DSB`将Dirty数据写回GM并等待写回完成；随后通过`sync S→V/VF`保证VF_SIMT读取发生在Scalar侧处理之后；最后，VF_SIMT读前执行`SIMT.DCCI`，避免SIMT DCache命中旧副本，再执行`VF_SIMT.LOAD`读取新值。

如果前序是`Scalar（no cache）`写，写端不会留下Dirty副本，但写入仍需要通过`Scalar.DSB`等待完成；如果后续读端走带Cache路径，仍要按表执行读端DCCI处理旧副本。

##### WAW：先写后写

WAW（Write After Write）场景要求最终写入GM的是后一次写操作的数据。因为DCache写回的原因，程序顺序上的后写不一定是内存序上的最后一次写。下面示例中，`A`和`B`位于同一条64B DCache Cache Line内。前一次写通过Scalar DCache修改`A`，后一次写通过MTE3修改`B`。虽然MTE3写发生在后，但如果Scalar DCache中的Dirty Cache Line最后才写回GM，就可能把`B`恢复成Scalar缓存副本中的旧值：

```text
初始状态：
GM Cache Line = [A=0, B=0, ...]
A和B位于同一条64B DCache Cache Line内。

时间点    Scalar流水                              MTE3流水
======    ====================================    ====================================
T0        Scalar.STORE A = 1
          // Scalar DCache中形成Dirty Cache Line：
          // [A=1, B=0, ...]
          // GM Cache Line仍可能是[A=0, B=0, ...]

T1                                                MTE3.STORE B = 2
                                                  // MTE3直接写GM：
                                                  // GM Cache Line = [A=0, B=2, ...]

T2        Scalar.DCCI A
T3        Scalar.DSB
          // Scalar Dirty Cache Line按64B写回GM：
          // GM Cache Line = [A=1, B=0, ...]
          // MTE3对B的后写被前序Scalar Dirty副本覆盖
```

因此，WAW场景的维护重点是避免前序Scalar Dirty副本在后写之后才写回GM。软件应先按表处理前一次写留下的Dirty副本或前序GM写等待，再执行后一次写；后一次写是否需要进一步等待完成或发布，由后一次写的访问路径决定。上面的例子应改成：

```text
初始状态：
GM Cache Line = [A=0, B=0, ...]
A和B位于同一条64B DCache Cache Line内。

时间点    Scalar流水                              MTE3流水
======    ====================================    ====================================
T0        Scalar.STORE A = 1
          // Scalar DCache中形成Dirty Cache Line：
          // [A=1, B=0, ...]

T1        Scalar.DCCI A
T2        Scalar.DSB
          // 前序Scalar Dirty Cache Line写回完成：
          // GM Cache Line = [A=1, B=0, ...]

T3        sync S→MTE3
          // 建立Scalar流水到MTE3流水的执行顺序

T4                                                MTE3.STORE B = 2
                                                  // GM Cache Line = [A=1, B=2, ...]
```

下表列出从前序写开始、完成必要的前序写处理和流水顺序、到后续写发起为止的操作序列。维护WAW场景下缓存一致性时先按行找到前一次写路径，再按列找到后一次写路径，按单元格顺序执行即可。表中的`后写xxx.STORE`只表示后一次写操作已经发起，如果后一次写需要立刻对GM或其他访问路径可见，仍需要在后写完成后按对应写路径执行必要的DCCI操作。

| 后续写入方式（列，后发生）<br>╲<br>前序写入方式（行，先发生） | Scalar（cache） | Scalar（no cache） | VF_SIMT（cache） | VF_SIMT（no cache） | MTE3 | FixPipe |
| --- | --- | --- | --- | --- | --- | --- |
| Scalar<br>（cache） | 先写Scalar.STORE<br>后写Scalar.STORE | 先写Scalar.STORE<br>先写Scalar.DCCI<br>先写Scalar.DSB<br>后写Scalar.STORE | 先写Scalar.STORE<br>先写Scalar.DCCI<br>先写Scalar.DSB<br>sync S→V/VF<br>后写VF_SIMT.STORE | 先写Scalar.STORE<br>先写Scalar.DCCI<br>先写Scalar.DSB<br>sync S→V/VF<br>后写VF_SIMT.STORE | 先写Scalar.STORE<br>先写Scalar.DCCI<br>先写Scalar.DSB<br>sync S→MTE3<br>后写MTE3.STORE | 先写Scalar.STORE<br>先写Scalar.DCCI<br>先写Scalar.DSB<br>sync S→FixPipe<br>后写FixPipe.STORE |
| Scalar<br>（no cache） | 先写Scalar.STORE<br>先写Scalar.DSB<br>后写Scalar.STORE | 先写Scalar.STORE<br>先写Scalar.DSB<br>后写Scalar.STORE | 先写Scalar.STORE<br>先写Scalar.DSB<br>sync S→V/VF<br>后写VF_SIMT.STORE | 先写Scalar.STORE<br>先写Scalar.DSB<br>sync S→V/VF<br>后写VF_SIMT.STORE | 先写Scalar.STORE<br>先写Scalar.DSB<br>sync S→MTE3<br>后写MTE3.STORE | 先写Scalar.STORE<br>先写Scalar.DSB<br>sync S→FixPipe<br>后写FixPipe.STORE |

如果用户无法保证多个写者之间的先后顺序，应避免多个核通过DCache写同一地址或同一Cache Line。

##### WAR：先读后写

WAR（Write After Read）场景要求前序读操作读取到后续写操作发生之前的旧值。此类场景的关键不是写入结果，而是先保证读操作已经完成，再允许后续写发起。否则，后写可能过早改变GM中的值，导致前序读观察到本不应该看到的新值。

下表列出前序读取方式为`Scalar（cache）`或`Scalar（no cache）`时，从前序读开始、完成必要的等待和流水顺序、到后续写发起为止的操作序列。如果是跨核场景需要用核间通知替代表中的核内`sync`。

| 后续写入方式（列，后发生）<br>╲<br>前序读取方式（行，先发生） | Scalar（cache） | Scalar（no cache） | VF_SIMT（cache） | VF_SIMT（no cache） | MTE3 | FixPipe |
| --- | --- | --- | --- | --- | --- | --- |
| Scalar（cache） | 前读Scalar.LOAD<br>后写Scalar.STORE | 前读Scalar.LOAD<br>前读Scalar.DSB<br>后写Scalar.STORE | 前读Scalar.LOAD<br>sync S→V/VF<br>后写VF_SIMT.STORE | 前读Scalar.LOAD<br>sync S→V/VF<br>后写VF_SIMT.STORE | 前读Scalar.LOAD<br>sync S→MTE3<br>后写MTE3.STORE | 前读Scalar.LOAD<br>sync S→FixPipe<br>后写FixPipe.STORE |
| Scalar（no cache） | 前读Scalar.LOAD<br>前读Scalar.DSB<br>后写Scalar.STORE | 前读Scalar.LOAD<br>前读Scalar.DSB<br>后写Scalar.STORE | 前读Scalar.LOAD<br>sync S→V/VF<br>后写VF_SIMT.STORE | 前读Scalar.LOAD<br>sync S→V/VF<br>后写VF_SIMT.STORE | 前读Scalar.LOAD<br>sync S→MTE3<br>后写MTE3.STORE | 前读Scalar.LOAD<br>sync S→FixPipe<br>后写FixPipe.STORE |

WAR场景下维护缓存一致性的目标是读取写前旧值，维护重点是确保这个读已经完成。表中的`DSB`只负责等待前序GM访问完成，不能替代流水间或核间同步。`sync S→V/VF`、`sync S→MTE3`和`sync S→FixPipe`只用于同一AI Core内建立流水顺序。如果前序访问和后续访问位于不同AI Core，表中的核内流水同步不能跨核生效，还需要按以下顺序组织跨核通信：

```text
前序访问核：完成前序访问侧Cache处理和必要DSB -> 发送核间通知
后续访问核：等待核间通知 -> Cache副本处理 -> 访问数据
```

### ICache与GM的缓存一致性

ICache用于缓存Scalar单元最近使用或频繁使用的指令。Scalar读取GM指令时，优先从ICache读取；ICache未命中时，再从L2 Cache读取；L2 Cache也未命中时，再从GM加载指令并更新到L2 Cache和ICache。相关说明见[系统缓存概述](../../../../api/SIMD-API/basic_api/cache_control/system_cache_overview.md)。

从缓存一致性角度看，ICache缓存的是指令而不是普通数据，并且是只读Cache，因此一般不涉及普通数据路径上的Cache副本不一致问题。

<!-- npu="950" id4 -->
### SIMT DCache与GM的缓存一致性

#### SIMT DCache访问路径

[NPU架构版本3510](../../language_extension/simd_builtin_keywords.md)新增SIMT相关硬件单元。SIMT访存会涉及SIMT DCache：SIMT DCache复用UB中的一部分空间，用于缓存SIMT线程访问GM时的数据。SIMT DCache空间可配置范围为32KB到128KB，实际大小由UB总大小扣除静态内存、动态内存和预留空间后确定，若剩余空间超过128KB，SIMT DCache大小固定为128KB。SIMT也提供[`asc_ldcg`](../../../../api/SIMT-API/memory_access_functions/asc_ldcg.md)、[`asc_stcg`](../../../../api/SIMT-API/memory_access_functions/asc_stcg.md)等接口表达不占用或绕过SIMT DCache的访问意图，因此本节按带Cache路径和no cache路径分别分析。

SIMT读GM的底层实现：先从SIMT DCache读取，未命中再访问L2 Cache或GM，并把数据缓存到L2 Cache和SIMT DCache中。L2 Cache与GM之间的数据一致性由硬件保证，但SIMT DCache与GM之间不保证一致性。因此，如果同一GM地址已经被其他核、Scalar路径或MTE路径更新，而当前SIMT DCache中仍有旧副本，后续SIMT读可能继续命中旧值。

SIMT DCache采用直写（write-through）策略：

- SIMT写GM时，写入会进入SIMT DCache，同时写出到GM，不形成需要后续Clean回GM的Dirty副本；
- no cache写入不经过SIMT DCache，也不会在SIMT DCache中缓存写入数据。no cache写GM时会检查该地址的数据是否已缓存在SIMT DCache里，如果命中，硬件会自己执行DCCI。

判断SIMT写GM后是否需要Cache处理，可以按两步看：

- **先看写端是否需要等待GM写完成**：SIMT DCache是直写Cache，写端不需要执行DCCI把Dirty副本写回GM。
- **再看读端是否可能命中旧副本**：如果消费者通过Scalar或MTE2等其他带Cache路径读取，读端路径中可能已有旧副本，需要在读前按表执行对应的DCCI。

如果线程或核之间还通过GM上的标志位通信，需要另行使用[`asc_threadfence`](../../../../api/SIMT-API/sync_and_memory_fence/memory_fence/asc_threadfence.md)等内存栅栏处理写入顺序关系。内存栅栏属于内存顺序控制，不会自动写回生产者侧Cache，也不会自动失效消费者侧Cache。

#### 软件维护VF_SIMT访问GM的缓存一致性

本节从软件维护角度说明VF_SIMT访问GM时如何维护SIMT DCache一致性。硬件只维护单核内SIMT流水的缓存一致性，不维护SIMT和其他流水的一致性。由于SIMT DCache采用直写策略，VF_SIMT写GM不会形成需要后续Clean回GM的Dirty副本，但写操作是否已经完成、读端是否可能命中旧副本、不同流水之间是否已经建立顺序，仍需要由软件按访问依赖关系处理。

表中的两类VF_SIMT访问路径含义如下：

- **VF_SIMT（cache）**：写入经过SIMT DCache直写路径，数据会缓存至SIMT DCache和L2 Cache，同时写出到GM。
- **VF_SIMT（no cache）**：写GM不经过SIMT DCache，不在SIMT DCache中缓存写入数据。

由于SIMT DCache为直写Cache，且no cache路径也不会产生Dirty副本，二者在RAW、WAW、WAR场景下需要的软件一致性维护动作基本相同，下文表格将`VF_SIMT（cache）`和`VF_SIMT（no cache）`合并为`VF_SIMT（cache/no cache）`。

下面按RAW、WAW和WAR三类依赖关系分析。本节表格同样按"先看行、再看列"阅读：行表示先发生的访问，列表示后发生的访问；单元格中的操作按从上到下的顺序执行。表中的`xxx.STORE`表示对应路径的写操作，`xxx.LOAD`表示对应路径的读操作。`SIMT.DCCI`用于使SIMT DCache中的旧副本失效，`sync X→Y`用于同一AI Core内建立流水X到流水Y的执行顺序，`Scalar.DCCI`和`MTE2.DCI`表示读端在读取前处理本路径可能存在的旧Cache副本。`DSB`和`sync`不改变Cache副本状态，不能替代DCCI。

##### RAW：先写后读

RAW场景要求后续读操作读取到VF_SIMT前序写操作产生的新值。由于SIMT DCache是直写Cache，写端不需要执行DCCI写回Dirty数据；如果后续读端是Scalar或MTE2等可能持有旧副本的带Cache路径，还需要在读前执行对应DCCI，避免命中旧副本。

下表描述核内VF_SIMT作为生产者写GM之后、消费者真正读取之前，生产者和消费者需要按顺序执行哪些Cache处理以及必要的等待/流水顺序操作。

| 后续读取方式（列，后发生）<br>╲<br>前序写入方式（行，先发生） | Scalar（cache） | Scalar（no cache） | VF_SIMT（cache） | VF_SIMT（no cache） | MTE2（cache） | MTE2（no cache） |
| --- | --- | --- | --- | --- | --- | --- |
| VF_SIMT<br>（cache/no cache） | 先写VF_SIMT.STORE<br>sync V/VF→S<br>后读Scalar.DCCI<br>后读Scalar.LOAD | 先写VF_SIMT.STORE<br>sync V/VF→S<br>后读Scalar.LOAD | 先写VF_SIMT.STORE<br>sync SIMT->SIMT（同线程内无需同步）<br>后读VF_SIMT.LOAD | 先写VF_SIMT.STORE<br>sync SIMT->SIMT（同线程内无需同步）<br>后读VF_SIMT.LOAD | 先写VF_SIMT.STORE<br>sync V/VF→MTE2<br>后读MTE2.DCI<br>后读MTE2.LOAD | 先写VF_SIMT.STORE<br>sync V/VF→MTE2<br>后读MTE2.LOAD |

例如，前序写入方式是`VF_SIMT（cache/no cache）`，后续读取方式是普通`Scalar（cache）`时，需要执行：

```text
VF_SIMT.STORE -> sync V/VF→S -> Scalar.DCCI -> Scalar.LOAD
```

含义是：SIMT DCache是直写Cache，SIMT写GM时新值会写出到GM；消费者读前再通过`sync V/VF→S`保证同一AI Core内Scalar读取发生在VF_SIMT写入之后，并执行`Scalar.DCCI`避免Scalar DCache中已有旧副本；之后Scalar读取才能从正确位置获得新值。

其他单元格按同样方式阅读：先完成行中描述的VF_SIMT写，再根据列中后续读取路径补充等待、流水同步以及读端Cache失效操作。是否需要读端DCCI，取决于后续读取路径是否可能命中旧Cache副本。

其中，表格里的sync SIMT->SIMT（同线程内无需同步）指如果是线程间操作同地址，则需要通过[asc_syncthreads](../../../../api/SIMT-API/sync_and_memory_fence/sync_interface/asc_syncthreads.md)建立阶段同步；如果是SIMT的某个单线程内VF_SIMT写入和读取同地址无需同步。

##### WAW：先写后写

WAW场景要求最终写入GM的是后一次写操作的数据。由于SIMT DCache是直写Cache，前一次VF_SIMT写不会留下待写回GM的Dirty副本；软件维护的重点是按表保证前后两次写的执行顺序，避免前一次写延迟到达GM后影响后一次写的结果。

下表列出从前序写开始、完成必要的前序写等待和流水顺序、到后续写发起为止的操作序列。使用时先按行选择前一次VF_SIMT写路径，再按列选择后一次写路径，按单元格顺序执行。表中的`后写xxx.STORE`只表示后一次写操作已经发起，不表示后一次写已经完成，也不表示后一次写结果已经对GM或其他访问路径可见；后一次写完成后的等待和发布，需要按后写路径另行处理。

| 后续写入方式（列，后发生）<br>╲<br>前序写入方式（行，先发生） | Scalar（cache） | Scalar（no cache） | VF_SIMT（cache） | VF_SIMT（no cache） | MTE3 | FixPipe |
| --- | --- | --- | --- | --- | --- | --- |
| VF_SIMT<br>（cache/no cache） | 先写VF_SIMT.STORE<br>sync V/VF→S<br>后写Scalar.STORE | 先写VF_SIMT.STORE<br>sync V/VF→S<br>后写Scalar.STORE | 先写VF_SIMT.STORE<br>sync SIMT->SIMT（同线程内无需同步）<br>后写VF_SIMT.STORE | 先写VF_SIMT.STORE<br>sync SIMT->SIMT（同线程内无需同步）<br>后写VF_SIMT.STORE | 先写VF_SIMT.STORE<br>sync V/VF→MTE3<br>后写MTE3.STORE | 先写VF_SIMT.STORE<br>sync V/VF→FixPipe<br>后写FixPipe.STORE |

如果后一次写需要立刻对GM或其他访问路径可见，仍需要在后写完成后按对应写路径执行必要的等待或发布操作。例如，后写为Scalar（cache）时，需要执行`后写Scalar.DCCI -> 后写Scalar.DSB`；后写为VF_SIMT、MTE3或FixPipe时，需要等待对应写GM操作完成。

##### WAR：先读后写

WAR场景要求前序读操作读取到后续写操作发生之前的旧值。此类场景不需要把新值尽快发布给读端，而是需要先确认VF_SIMT侧读操作已经完成，再允许后续写端执行写GM操作或发送写完成通知。

下表列出前序读取方式为`VF_SIMT（cache/no cache）`时，从前序读开始、完成必要等待和流水顺序、到后续写发起为止的操作序列。使用时先按行选择前序VF_SIMT读路径，再按列选择后续写路径；单元格中的`前读VF_SIMT.LOAD`表示旧值读取点，`后写xxx.STORE`表示新值写入点。跨核场景需要用核间通知替代表中的核内`sync`。

| 后续写入方式（列，后发生）<br>╲<br>前序读取方式（行，先发生） | Scalar（cache） | Scalar（no cache） | VF_SIMT（cache） | VF_SIMT（no cache） | MTE3 | FixPipe |
| --- | --- | --- | --- | --- | --- | --- |
| VF_SIMT<br>（cache/no cache） | 前读VF_SIMT.LOAD<br>sync V/VF→S<br>后写Scalar.STORE | 前读VF_SIMT.LOAD<br>sync V/VF→S<br>后写Scalar.STORE | 前读VF_SIMT.LOAD<br>sync SIMT->SIMT（同线程内无需同步）<br>后写VF_SIMT.STORE | 前读VF_SIMT.LOAD<br>sync SIMT->SIMT（同线程内无需同步）<br>后写VF_SIMT.STORE | 前读VF_SIMT.LOAD<br>sync V/VF→MTE3<br>后写MTE3.STORE | 前读VF_SIMT.LOAD<br>sync V/VF→FixPipe<br>后写FixPipe.STORE |

WAR表不要求读端失效Cache副本，因为目标是读取写前旧值；维护重点是等待前序读完成并建立到后续写路径的顺序。若前序访问和后续访问位于不同AI Core，表中的`sync V/VF→S`、`sync V/VF→MTE3`和`sync V/VF→FixPipe`不能跨核生效，应先在前序访问所在核完成读取之后发送核间通知；后序访问所在核收到通知后，再处理本核读端或写端可能存在的Cache副本。
<!-- end id4 -->

### UB与DCache的缓存一致性

UB与DCache的一致性问题主要出现在Scalar开启DCache模式读取UB的场景：Scalar访问UB路径时可以`CTRL[49]`控制Scalar读取UB时是否通过DCache读取；`CTRL[49] = 1'b0`时，Scalar直接从UB读取；`CTRL[49] = 1'b1`时，Scalar读取UB数据会经过DCache。相关说明见[asc_dcci](../../../../api/SIMD-API/c_api/cache_ctrl/asc_dcci.md)和[asc_set_ctrl()](../../../../api/SIMD-API/c_api/spr/asc_set_ctrl.md)。

#### Scalar以DCache模式访问UB带来的缓存一致性

Scalar读取UB时默认是直接读取UB，但是如果用户显式设置`CTRL[49] = 1'b1`，Scalar以DCache模式读取UB时，此时需要处理Scalar以DCache模式访问UB带来的缓存一致性问题。

Scalar读UB后，数据进入DCache；后续Scalar再次读取同一段UB地址时，可能继续命中DCache副本，而不是直接读取UB。下面用同一核内的一个UB地址`A`说明旧副本问题。

```text
初始状态：
UB[A] = 0
Scalar DCache中没有缓存A
CTRL[49] = 1'b1    // Scalar读UB时从DCache读取

时间点    Vector流水                              Scalar流水
======    ====================================    ====================================
T0                                                Scalar.LOAD UB[A] -> 0
                                                  // Scalar DCache缓存A所在Cache Line

T1        Vector.STORE UB[A] = 1
          // UB[A] = 1

T2        sync V→S
          // 保证Scalar后续读取发生在Vector写UB之后

T3                                                Scalar.LOAD UB[A] -> 0
                                                  // Scalar可能继续命中旧DCache副本
                                                  // 即使UB[A]已经是1，也可能读到0
```

这个场景包含两个独立问题：一是Vector写UB是否已经完成，二是Scalar DCache中是否已经缓存过旧UB副本。即使Vector写UB已经完成，Scalar也可能继续命中旧DCache副本。因此，消费者通过Scalar DCache模式读取UB前，需要同时处理流水顺序和DCache旧副本：

```text
初始状态：
UB[A] = 0
Scalar DCache中没有缓存A
CTRL[49] = 1'b1    // Scalar读UB时从DCache读取

时间点    Vector流水                              Scalar流水
======    ====================================    ====================================
T0                                                Scalar.LOAD UB[A] -> 0
                                                  // Scalar DCache缓存A所在Cache Line

T1        Vector.STORE UB[A] = 1
          // UB[A] = 1

T2        sync V→S
          // 保证Scalar后续读取发生在Vector写UB之后

T3                                                Scalar.DCCI
                                                  // Scalar DCache中缓存的UB[A]旧副本失效

T4                                                Scalar.DSB
                                                  // 等待DCCI相关操作完成

T5                                                Scalar.LOAD UB[A] -> 1
                                                  // Scalar重新从UB获取新值
```

其中，`sync V->S`用于保证同一AI Core内Scalar读取发生在Vector写UB之后；`Scalar.DCCI`用于处理DCache与UB的一致性，使Scalar后续读取不再使用旧DCache副本；`Scalar.DSB`用于等待DCCI相关操作完成。若生产者是MTE2把GM数据搬入UB，也需要先建立`MTE2 -> S`的流水顺序，再处理Scalar DCache中的旧UB副本。

#### Cache处理原则

C API中提供了[`asc_ub_dcci_single`](../../../../api/SIMD-API/c_api/cache_ctrl/asc_dcci.md)、[`asc_dcci_entire_ub`](../../../../api/SIMD-API/c_api/cache_ctrl/asc_dcci.md)等面向UB地址空间的DCCI接口。实际使用时，应根据当前产品和CANN版本选择支持的UB DCCI接口；DCCI发出后，还需要使用DSB等待相关操作完成，再执行后续Scalar读取。

实际编程时，应先判断消费者从哪里读取，再选择对应的一致性目标：

- 如果后续消费者以Scalar DCache模式直接读取UB，关注的是DCache与UB的一致性，应使用DCCI处理旧副本，并在读取前建立生产流水到Scalar流水的同步。
- 如果Scalar读取UB时没有开启DCache模式，即`CTRL[49] = 1'b0`，Scalar直接从UB读取，此时通常只需要处理Scalar流水与Vector/MTE流水之间的执行顺序，不需要通过DCCI处理旧DCache副本。


## 原子操作与一致性

原子操作用于多个执行单元、多个线程或多个核并发更新同一地址时，将一次读-改-写过程作为不可被其他更新打断的事件。原子操作可以避免同一地址上的更新丢失，但不等价于缓存一致性处理，也不等价于同步屏障。判断atomic操作下的缓存一致性，需要同时看两个问题：atomic操作实际更新的是GM还是UB，以及同一地址是否还被普通DCache<!-- npu="950" id5 -->、SIMT DCache<!-- end id5 -->或MTE等其他路径访问。

下面按atomic事件发起路径分类说明：<!-- npu="950" id6 -->Scalar原子、<!-- end id6 -->MTE3随路原子、FixPipe随路原子<!-- npu="950" id7 -->和SIMT原子<!-- end id7 -->。本节只说明这些atomic事件放入一致性分析时需要关注的Cache行为，接口细节可参考对应API文档。

<!-- npu="950" id8 -->
### Scalar原子操作与DCache一致性

[NPU架构版本3510](../../language_extension/simd_builtin_keywords.md)支持Scalar原子操作，Scalar原子操作和普通Scalar读写GM混用时，需要按DCache与GM的一致性来分析，详细处理方式参考[Atomic访问导致的DCache缓存一致性](#atomic访问导致的dcache缓存一致性)。
<!-- end id8 -->

### MTE3随路原子操作与DCache一致性

随路原子操作不是单独读取某个地址的Scalar事件，而是改变后续目的地址为GM的数据搬运行为。MTE3负责将UB等核内数据搬出到GM，因此`UB -> GM`是常见的随路原子使用路径。接口说明可参考C API [asc_set_atomic_add](../../../../api/SIMD-API/c_api/atomic/datamove_atomic/asc_set_atomic_add.md)和[asc_disable_dma_atomic](../../../../api/SIMD-API/c_api/atomic/datamove_atomic/asc_disable_dma_atomic.md)。

从一致性角度看，MTE3随路原子的目标是GM，不会自动处理Scalar DCache中已有的副本，因此需要关注以下关系：

- 开启随路原子前，如果GM目标地址此前被普通Scalar写过，需要先确保Scalar DCache中的Dirty数据已经写回GM，否则原子操作读取到的"GM已有内容"可能不是程序期望的初值。

- 随路原子完成后，如果后续通过普通Scalar读取同一GM地址，需要处理读端DCache旧副本，并等待MTE3写GM完成。


### FixPipe随路原子操作与DCache一致性

FixPipe常用于`L0C Buffer -> GM`或相关矩阵搬出路径。对一致性分析来说，FixPipe随路原子可以看成另一类"向GM写入的随路原子事件"：数据来源不同于MTE3，但最终更新的仍是GM目标地址。

因此，FixPipe随路原子的缓存一致性行为与MTE3随路原子类似：它不会自动清理Scalar DCache旧副本，也不会自动把此前停留在DCache中的Dirty数据合入GM。

- 如果GM目标地址的初值来自普通Scalar写GM，需要先按[DCache缓存一致性](#DCache与GM的缓存一致性)把DCache中的Dirty数据写回GM，再执行FixPipe随路原子。
- 如果FixPipe随路原子后，消费者通过Scalar普通路径读取GM，需要等待FixPipe写GM完成，并在读端处理DCache旧副本。
- 如果多个流水或多个核都对同一GM地址做随路原子，原子操作只保证每次GM读-改-写不被打断；不同流水、不同核之间的启动顺序仍应由同步或算法协议保证。

<!-- npu="950" id9 -->
### SIMT原子操作与缓存一致性

[NPU架构版本3510](../../language_extension/simd_builtin_keywords.md)支持SIMT原子操作，SIMT原子操作可能作用于UB，也可能作用于GM；接口例子可参考[asc_atomic_add](../../../../api/SIMT-API/atomic_operations/asc_atomic_add.md)。它保证的是同一地址上这一次读-改-写具有原子性，不保证多个线程之间的执行顺序。

分析缓存一致性时，需要先区分目标地址：

- 目标是UB时，主要关注同一Thread Block内线程之间对共享UB数据的可见性和执行顺序；如果同一段UB数据还被Scalar以DCache模式读取，需要结合处理[DCache与UB的一致性](#ub与dcache的缓存一致性)。
- 目标是GM时，需要把SIMT原子更新GM、SIMT普通读写GM、Scalar普通读写GM、MTE搬运GM放在同一张可见性图里分析。原子操作本身不能替代SIMT DCache或Scalar DCache的旧副本处理；若后续通过默认SIMT读或Scalar普通读读取同一GM地址，仍需要按[SIMT缓存一致性](#simt-dcache与gm的缓存一致性)或[DCache缓存一致性](#DCache与GM的缓存一致性)处理读端可能存在的旧Cache副本。
- 如果不同Thread Block或不同AI Core通过GM标志位配合SIMT原子操作通信，仍需要结合内存栅栏、原子操作、轮询协议以及必要的Cache处理建立完整同步关系，不能只依赖原子操作本身。
<!-- end id9 -->

## 预取

预取用于提前把后续可能访问的数据或指令放入Cache，目的是降低访问延迟。相关说明见[系统缓存概述](../../../../api/SIMD-API/basic_api/cache_control/system_cache_overview.md)。从一致性角度看，预取会改变数据或指令进入Cache的时机，可能会导致一致性问题。

#### L2 Cache预取

L2 Cache和GM之间没有一致性问题，因此L2 Cache的预取不会导致多核间数据不一致问题。但这不表示L2 Cache预取获得的数据就是期望的：如果生产者写入的新值还停留在DCache中、MTE3写GM尚未完成，或者消费者读取顺序早于生产者写入完成，那么L2 Cache中提前加载到的仍可能是GM可见的旧值。此时仍应回到对应章节处理生产者写回、访问完成等待和读端旧副本失效。

#### DCache预取

[asc_datacache_preload](../../../../api/SIMD-API/c_api/cache_ctrl/asc_datacache_preload.md)用于从指定GM地址预加载数据到DCache中，每次调用只能预加载一个Cache Line大小的数据。它的作用是让后续Scalar读取更可能命中DCache，而不是建立新的同步或一致性关系。

DCache是每个核内独立的缓存，因此DCache预取会在当前核DCache中提前形成GM数据副本。如果预取之后，同一GM地址被其他核、MTE、原子操作或SIMT路径更新，当前核后续普通Scalar读可能继续命中预取形成的旧副本。需要读取最新GM数据时，仍要按[DCache缓存一致性](#DCache与GM的缓存一致性)执行DCCI失效旧副本，或使用绕过DCache的读接口。

#### ICache预取

[asc_icache_preload](../../../../api/SIMD-API/c_api/cache_ctrl/asc_icache_preload.md)用于从指令所在GM地址预加载指令到ICache中。ICache预取主要影响取指性能，不参与普通数据的生产者-消费者可见性判断，也没有一致性问题。

## 总结

缓存一致性问题的核心是同一份数据可能同时存在于GM、DCache、<!-- npu="950" id10 -->SIMT DCache、<!-- end id10 -->UB等不同位置。软件需要根据访问路径和RAW、WAW、WAR依赖关系，显式处理Cache副本和执行顺序。

实际编程时重点关注以下规则：

- 写端可能存在Dirty副本时，需要通过DCCI写回，并配合DSB等待完成。
- 读端可能存在旧Cache副本时，需要在读取前通过DCCI失效旧副本。
- DCCI处理Cache副本状态，DSB等待前序内存访问完成，流水同步或核间通知建立执行顺序，三者不能互相替代。
