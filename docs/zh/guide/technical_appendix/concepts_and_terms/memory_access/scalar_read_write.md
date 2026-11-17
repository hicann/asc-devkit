# Scalar读写数据<a name="ZH-CN_TOPIC_0000002375401226"></a>

AI Core中Scalar计算单元负责各类型的标量数据运算和程序的流程控制。根据[硬件架构](../../../programming_guide/advanced_programming/hardware_implementation/basic_architecture.md)设计，Scalar仅支持对Global Memory和Unified Buffer（UB）的读写操作，而不支持对L1 Buffer、L0A Buffer、L0B Buffer和L0C Buffer等其他类型存储的访问。下文分别介绍了Scalar读写Global Memory和UB的方式和Scalar读写数据时的同步机制。

## Scalar读写Global Memory<a name="section7480536235"></a>

<img src="../../../figures/aicore_31.png" alt="" width="600px" height="475px">

如上图所示，Scalar读写GM数据时会经过DataCache，DataCache主要用于提高标量访存指令的执行效率，每一个AIC/AIV核内均有一个独立的DataCache。下面通过一个具体示例来讲解DataCache的具体工作机制。

globalTensor1是位于GM上的Tensor：

-   执行完GetValue\(0\)后，globalTensor1的前8个元素会进入DataCache，后续GetValue\(1\)\~GetValue\(7\)不需要再访问GM，而可以直接从DataCache的Cache Line中读取数据，提高了标量连续访问的效率。
-   执行完SetValue\(8, val\)后，globalTensor1的index为8\~15的元素会进入DataCache，SetValue只会修改DataCache中的Cache Line数据，同时将Cache Line的状态设置为Dirty，表明Cache Line中的数据与GM中的数据不一致。

```cpp
AscendC::GlobalTensor<int64_t> globalTensor1;
globalTensor1.SetGlobalBuffer((__gm__ int64_t *)input);
// 从0~7共计8个uint64_t类型，DataCache的Cache Line长度为64字节
// 执行完GetValue(0)后，GetValue(1)~GetValue(7)可以直接从Cache Line中读取，不需要再访问GM
globalTensor1.GetValue(0);
globalTensor1.GetValue(1);
globalTensor1.GetValue(2);
globalTensor1.GetValue(3);
globalTensor1.GetValue(4);
globalTensor1.GetValue(5);
globalTensor1.GetValue(6);
globalTensor1.GetValue(7);

// 执行完SetValue(8)后，不会修改GM上的数据，只会修改DataCache中Cache Line数据
// 同时Cache Line的状态置为dirty，dirty表示DataCache中Cache Line数据与GM中的数据不一致
int64_t val = 32;
globalTensor1.SetValue(8, val);
globalTensor1.GetValue(8);
```

根据上文的工作机制（如下图所示），多核间访问globalTensor1会出现数据不一致的情况，如果其余核需要获取GM数据的变化，则需要开发者手动调用[DataCacheCleanAndInvalid](../../../../api/SIMD-API/basic_api/cache_control/DataCacheCleanAndInvalid.md)来保证数据的一致性。

<img src="../../../figures/aicore_32.png" alt="" width="600px" height="269px">

## Scalar读写UB<a name="section8156161471119"></a>

Scalar读写UB时，可以使用LocalTensor的SetValue和GetValue接口。示例如下：

```cpp
for (int32_t i = 0; i < 16; ++i) {
    inputLocal.SetValue(i, i); // 对inputLocal中第i个位置进行赋值为i
}

for (int32_t i = 0; i < srcLen; ++i) {
    auto element = inputLocal.GetValue(i); // 获取inputLocal中第i个位置的数值
}
```

## Scalar读写数据时的同步<a name="section554364118119"></a>

Scalar读写Global Memory和UB时属于PIPE\_S（Scalar流水）操作，当用户使用SetValue或者GetValue接口，且算子工程开启自动同步时，不需要手动插入同步事件。

如果用户关闭算子工程的自动同步功能时，需要根据依赖对象判断是否插入同步事件。需要区分以下两类依赖：

- 如果后续指令只使用`GetValue`返回的标量值作为入参，此时后续流水依赖的是Scalar流水产生的标量值。Scalar流水会在该标量值就绪后发射后续指令，后续指令不需要再从UB或GM读取该标量值，因此不需要额外插入`S_V`同步。
- 如果后续流水需要读取`SetValue`写入的UB或GM数据，依赖的是Scalar对存储位置的写入结果，必须通过对应的同步事件保证数据已写入后再读取。其他流水对UB或GM存在真实读写依赖时，也仍需保留相应同步。

常见流水依赖和同步方式如下。表中的“依赖对象”指后续流水真正需要等待的内容；如果后续指令只接收Scalar流水产生的标量值，不需要等待Scalar对存储地址的写入，则不插入额外同步。

| 生产者流水 | 消费者流水 | Scalar操作 | 依赖对象 | 同步方式 | 说明 |
| --- | --- | --- | --- | --- | --- |
| MTE2 | Scalar | `GetValue`/`SetValue` | UB数据 | `MTE2_S` | `DataCopy`从GM搬入UB后，Scalar读取或修改该UB地址。 |
| Vector | Scalar | `GetValue`/`SetValue` | UB数据 | `V_S` | Vector写入UB后，Scalar读取或修改该UB地址。 |
| MTE3 | Scalar | `GetValue`/`SetValue` | GM数据 | `MTE3_S` | 数据搬出到GM后，Scalar读取或修改该GM地址。 |
| Scalar | Vector | `SetValue` | UB数据 | `S_V` | `SetValue`写入UB后，Vector读取该UB地址。 |
| Scalar | Vector | `GetValue` | 标量值 | — | `GetValue`返回值直接作为后续Vector指令参数，不需要插入`S_V`。 |
| Scalar | MTE2 | `SetValue` | GM数据 | `S_MTE2` | `SetValue`写入GM后，MTE2读取该GM地址作为搬运源。 |
| Scalar | MTE3 | `SetValue` | UB数据 | `S_MTE3` | `SetValue`写入UB后，MTE3读取该UB地址作为搬运源。 |

其中，`S_V`、`S_MTE2`和`S_MTE3`用于保护Scalar对地址数据的写入；`MTE2_S`、`V_S`和`MTE3_S`用于保护其他流水对地址数据的写入。若Scalar操作只是通过`GetValue`产生一个供后续接口使用的标量值，则不属于这类地址数据依赖。实际使用时还需要根据接口访问的地址和读写方向确认同步事件，不能仅根据流水类型机械插入或删除同步。

```cpp
// GetValue返回标量值后，Duplicate直接使用该标量参数。
// Duplicate不需要重新读取srcLocal中的数据，因此无需插入S_V同步。
float inputVal = srcLocal.GetValue(0);
AscendC::Duplicate(dstLocal, inputVal, srcDataSize); 

// SetValue修改UB中的数据，后续DataCopy需要读取该数据。
// 这里依赖的是UB存储结果，而不是Scalar返回值，因此需要S_MTE3同步。
srcLocal.SetValue(0, value);
SetFlag<HardEvent::S_MTE3>(eventID2);
WaitFlag<HardEvent::S_MTE3>(eventID2);
AscendC::DataCopy(dstGlobal, srcLocal, srcDataSize); 
```
