<!--
  Copyright (c) 2026 Huawei Technologies Co., Ltd.
  This program is free software, you can redistribute it and/or modify it under the terms and conditions of
  CANN Open Software License Agreement Version 2.0 (the "License").
  Please refer to the License for details. You may not use this file except in compliance with the License.
  THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
  See LICENSE in the root of the software repository for the full text of the License.
-->

<template>
  <section class="home-learning-head" aria-label="学习路径">
    <h2 class="home-learning-head__title">入门到调优</h2>
    <p class="home-learning-head__desc">
      将教程与参考资料放在接口导航之后，支持持续学习与工程落地
    </p>
  </section>
</template>

<style scoped>
.home-learning-head {
  width: min(1392px, calc(100% - 48px));
  margin: 52px auto 0;
  padding: 0 4px;
}

.home-learning-head__title {
  margin: 0;
  color: var(--vp-c-text-1);
  font-size: 26px;
  line-height: 1.3;
  font-weight: 750;
}

.home-learning-head__desc {
  max-width: 820px;
  margin: 10px 0 0;
  color: var(--vp-c-text-2);
  font-size: 14px;
  line-height: 1.75;
  font-weight: 500;
}

@media (max-width: 760px) {
  .home-learning-head {
    width: min(100%, calc(100% - 32px));
    margin-top: 36px;
  }

  .home-learning-head__title {
    font-size: 22px;
  }
}
</style>
