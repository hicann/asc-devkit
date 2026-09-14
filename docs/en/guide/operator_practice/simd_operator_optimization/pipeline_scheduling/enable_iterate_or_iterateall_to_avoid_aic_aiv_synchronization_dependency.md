# Enabling Iterate or IterateAll Asynchronous APIs to Avoid AIC/AIV Synchronization Dependency<a name="ZH-CN_TOPIC_0000001893695885"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:16:40.749Z -->

[Priority] High

[Description] In MIX scenarios, that is, mixed programming of AIC (AI Cube core) and AIV (AI Vector core), when Matmul Iterate or IterateAll is called, AIV sends a message to AIC to start Matmul computation. In the synchronous mode using Iterate<true\>, as shown in [Figure 1 Synchronous mode message sending diagram](#fig99236286201), each call triggers one message sending. In the asynchronous mode using Iterate<false\>, as shown in [Figure 2 Asynchronous mode message sending diagram](#fig1511392207), a message is sent only for the first call, and no message needs to be sent for subsequent calls. This reduces the interaction between the Cube and Vector cores and reduces inter-core communication overhead. Therefore, in MIX scenarios, it is recommended to use the Iterate<false\> or IterateAll<false\> asynchronous APIs. Note that Workspace must be set when using asynchronous APIs.

**Figure 1**  Synchronous mode message sending diagram<a name="fig99236286201"></a>  
![](../../../figures/synchronous_message_sending_diagram.png "Synchronous mode message sending diagram")

**Figure 2**  Asynchronous mode message sending diagram<a name="fig1511392207"></a>  
![](../../../figures/asynchronous_message_sending_diagram.png "Asynchronous mode message sending diagram")

[Negative Example]

The synchronous mode of the Iterate API is used in the MIX scenario.

```
TQueBind<TPosition::CO2, TPosition::VECIN>  qVecIn;
TQueBind<TPosition::VECIN, TPosition::VECOUT>  qVecOut;
mm.SetTensorA(gmA);
mm.SetTensorB(gmB);
int16_t scalar = 2;

while(mm.template Iterate()){
    auto cInUB = qVecIn.AllocTensor<float>();
    mm.GetTensorC(cInUB);
    qVecIn.EnQue(cInUB);
    cInUB = qVecIn.DeQue<float>();
    auto cOutUB = qVecOut.AllocTensor<float>();
    Muls(cOutUB, cInUB, scalar, baseM*baseN);
    qVecIn.FreeTensor(cInUB);
    ...
}
```

[Positive Example]

The asynchronous mode of the Iterate API is used in the MIX scenario.

```
TQueBind<TPosition::CO2, TPosition::VECIN>  qVecIn;
TQueBind<TPosition::VECIN, TPosition::VECOUT>  qVecOut;
mm.SetTensorA(gmA);
mm.SetTensorB(gmB);
mm.SetWorkspace(workspace, size);//Here, workspace is the physical address of the temporary space, and size is the memory size occupied by matrix C of singleCoreM*singleCoreN: singleCoreM*singleCoreN*sizeof(float)
int16_t scalar = 2;

while(mm.template Iterate<false>()){
    auto cInUB = qVecIn.AllocTensor<float>();
    mm.GetTensorC(cInUB);
    qVecIn.EnQue(cInUB);
    cInUB = qVecIn.DeQue<float>();
    auto cOutUB = qVecOut.AllocTensor<float>();
    Muls(cOutUB, cInUB, scalar, baseM*baseN);
    qVecIn.FreeTensor(cInUB);
    ...
}
```
