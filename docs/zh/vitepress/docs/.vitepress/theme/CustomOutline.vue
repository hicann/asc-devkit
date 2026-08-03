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
import { useData, useRoute } from 'vitepress'
import { isApiRouteUnsupported } from '../../../scripts/api-support.mjs'
import { selectedFilter } from './filter-state.js'

const route = useRoute()
const { page, theme } = useData()

const headers = computed(() => {
  const h = page.value?.outlineHeaders || []
  const unsupported = isApiRouteUnsupported(
    theme.value.apiUnsupportedIndex,
    route.path,
    selectedFilter.value
  )
  let h2n = 0, h3n = 0
  return h
    .filter(item => item.level === 2 || item.level === 3)
    .filter(item => !unsupported || item.title === '产品支持情况')
    .map(item => {
      if (item.level === 2) {
        h2n++
        h3n = 0
        return { ...item, num: `${h2n}. ` }
      }
      h3n++
      return { ...item, num: `${h2n}.${h3n} ` }
    })
})

</script>

<template>
  <div v-if="headers.length > 0" class="custom-outline">
    <div class="outline-title">本页内容</div>
    <ul class="outline-list">
      <template v-for="h in headers" :key="h.link">
        <li :class="['outline-item', h.level === 3 ? 'h3' : 'h2']">
          <a :href="h.link" class="outline-link">{{ h.num }}{{ h.title }}</a>
        </li>
      </template>
    </ul>
  </div>
</template>

<style scoped>
.custom-outline {
  border-left: 1px solid var(--vp-c-divider);
  padding-left: 16px;
  font-size: 13px;
}
.outline-title {
  font-weight: 600;
  margin-bottom: 8px;
  color: var(--vp-c-text-1);
}
.outline-list {
  list-style: none;
  padding: 0;
  margin: 0;
}
.outline-item {
  margin: 4px 0;
}
.outline-item.h3 {
  padding-left: 12px;
}
.outline-link {
  color: var(--vp-c-text-2);
  text-decoration: none;
  transition: color 0.2s;
  display: block;
  padding: 2px 0;
}
.outline-link:hover {
  color: var(--vp-c-brand-1);
}
</style>
