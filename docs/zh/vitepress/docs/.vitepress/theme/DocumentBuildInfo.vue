<!--
  Copyright (c) 2026 Huawei Technologies Co., Ltd.
  This program is free software, you can redistribute it and/or modify it under the terms and conditions of
  CANN Open Software License Agreement Version 2.0 (the "License").
  Please refer to the License for details. You may not use this file except in compliance with the License.
  THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
  See LICENSE in the root of the software repository for the full text of the License.
-->

<script setup>
import { computed } from 'vue'
import { useData } from 'vitepress'

const { theme } = useData()
const build = computed(() => theme.value.documentationBuild)
const commitUrl = computed(() => build.value?.commit
  ? `https://gitcode.com/cann/asc-devkit/commit/${build.value.commit}`
  : '')
</script>

<template>
  <div v-if="build" class="document-build-info" aria-label="文档构建信息">
    <span>版本 <strong>{{ build.version }}</strong></span>
    <span v-if="build.ref && build.ref !== build.version">分支 <strong>{{ build.ref }}</strong></span>
    <span v-if="build.shortCommit">
      提交
      <a :href="commitUrl" target="_blank" rel="noopener noreferrer">{{ build.shortCommit }}</a>
    </span>
    <span>构建于 <time :datetime="build.builtAt">{{ build.builtAtText }}</time></span>
    <span v-if="build.dirty" class="dirty">包含未提交修改</span>
  </div>
</template>

<style scoped>
.document-build-info {
  display: flex;
  flex-wrap: wrap;
  gap: 6px 16px;
  margin-bottom: 10px;
  line-height: 22px;
  font-size: 12px;
  color: var(--vp-c-text-3);
}

strong,
a,
time {
  color: var(--vp-c-text-2);
  font-weight: 500;
}

a {
  text-decoration: none;
}

a:hover {
  color: var(--vp-c-brand-1);
}

.dirty {
  color: var(--vp-c-warning-1);
}
</style>
