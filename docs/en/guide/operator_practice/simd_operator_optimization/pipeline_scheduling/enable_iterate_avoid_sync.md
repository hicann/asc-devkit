# Enabling Iterate or IterateAll Asynchronous APIs to Avoid AIC/AIV Synchronous Dependency<a name="ZH-CN_TOPIC_0000001893695885"></a>

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-10-23T12:51:32.673Z pushedAt=2026-11-03T07:05:13.348Z nl=2 -->

[Priority] High

[Description] In MIX scenarios, that is, mixed programming of AIC (AI Cube core) and AIV (AI Vector core), when Matmul Iterate or IterateAll is called, the AIV sends a message to the AIC to start Matmul computation. In the Iterate<true\> synchronous mode, as shown in [Figure 1 Synchronous mode message sending diagram](#fig99236286201), each call triggers a message sending. In the Iterate<false\> asynchronous mode, as shown in [Figure 2 Asynchronous mode message sending diagram](#fig1511392207), only the first call requires message sending, and subsequent calls do not, thereby reducing the interaction between the Cube and Vector cores and reducing inter-core communication overhead. Therefore, in MIX scenarios, it is recommended to use the Iterate<false\> or IterateAll<false\> asynchronous API (note: when using the asynchronous API, Workspace must be set).

**Figure 1**  Synchronous mode message sending diagram<a name="fig99236286201"></a>  
![](../../../figures/sync_msg.png "Synchronous mode message sending diagram")

**Figure 2**  Asynchronous mode message sending diagram<a name="fig1511392207"></a>  
![](../../../figures/async_msg.png "Asynchronous mode message sending diagram")

[Negative example]

Using the synchronous mode of the Iterate API in MIX scenarios.

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

[Positive example]

Using the asynchronous mode of the Iterate API in MIX scenarios.

```
TQueBind<TPosition::CO2, TPosition::VECIN>  qVecIn;
TQueBind<TPosition::VECIN, TPosition::VECOUT>  qVecOut;
mm.SetTensorA(gmA);
mm.SetTensorB(gmB);
mm.SetWorkspace(workspace, size);//Here, workspace is the physical address of the temporary space, and size is the memory size occupied by cube C of singleCoreM*singleCoreN: singleCoreM*singleCoreN*sizeof(float)
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
