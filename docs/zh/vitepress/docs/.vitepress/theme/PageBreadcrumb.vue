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
import { useData, withBase } from 'vitepress'

const { frontmatter } = useData()
const breadcrumbs = computed(() => frontmatter.value.breadcrumbs || [])
</script>

<template>
  <nav v-if="breadcrumbs.length > 1" class="page-breadcrumb" aria-label="面包屑">
    <ol>
      <template v-for="(item, index) in breadcrumbs" :key="`${item.link || ''}-${item.text}`">
        <li
          class="breadcrumb-item"
          :class="{
            current: index === breadcrumbs.length - 1,
            parent: index === breadcrumbs.length - 2,
            middle: index > 0 && index < breadcrumbs.length - 2,
          }"
        >
          <span v-if="index > 0" class="vpi-chevron-right separator" aria-hidden="true" />
          <a
            v-if="index < breadcrumbs.length - 1 && item.link"
            :href="withBase(item.link)"
            :title="item.text"
          >{{ item.text }}</a>
          <span v-else :title="item.text" :aria-current="index === breadcrumbs.length - 1 ? 'page' : undefined">
            {{ item.text }}
          </span>
        </li>
      </template>
      <li v-if="breadcrumbs.length > 3" class="breadcrumb-ellipsis" aria-hidden="true">
        <span class="vpi-chevron-right separator" />
        <span>...</span>
      </li>
    </ol>
  </nav>
</template>

<style scoped>
.page-breadcrumb {
  margin-bottom: 24px;
  color: var(--vp-c-text-2);
  font-size: 13px;
}

ol {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  gap: 4px 0;
  margin: 0;
  padding: 0;
  list-style: none;
}

.breadcrumb-item,
.breadcrumb-ellipsis {
  display: inline-flex;
  align-items: center;
  min-width: 0;
}

.separator {
  flex: none;
  margin: 0 6px;
  font-size: 14px;
  color: var(--vp-c-text-3);
}

a {
  color: var(--vp-c-text-2);
  text-decoration: none;
}

a:hover {
  color: var(--vp-c-brand-1);
}

.current {
  color: var(--vp-c-text-1);
  font-weight: 500;
}

.breadcrumb-ellipsis {
  display: none;
}

@media (max-width: 640px) {
  .page-breadcrumb {
    margin-bottom: 20px;
  }

  .breadcrumb-item.middle {
    display: none;
  }

  .breadcrumb-ellipsis {
    display: inline-flex;
    order: 1;
  }

  .breadcrumb-item:first-child {
    order: 0;
  }

  .breadcrumb-item.parent {
    order: 2;
  }

  .breadcrumb-item.current {
    order: 3;
  }
}
</style>
