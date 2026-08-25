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
import { ref, watch, onMounted, onUnmounted, nextTick } from 'vue'
import { useData, useRoute } from 'vitepress'
import {
  isApiRouteUnsupported,
} from '../../../scripts/api-support.mjs'
import { selectedFilter } from './filter-state.js'

const route = useRoute()
const { theme } = useData()
const isApiPage = ref(false)

const filterOptions = [
  { value: 'all', label: '全量' },
  { value: '950', label: 'Ascend 950' },
  { value: 'A3', label: 'Atlas A3' },
  { value: '910b', label: 'Atlas A2' },
  { value: '910', label: 'Atlas 训练系列' },
  { value: '310p', label: 'Atlas 推理系列' },
  { value: '310b', label: 'Atlas 200I/500 A2' },
  { value: 'x90', label: 'Kirin X90' },
  { value: '9030', label: 'Kirin 9030' },
]

function loadSavedFilter() {
  try {
    const saved = localStorage.getItem('cann-filter-selection')
    if (saved && filterOptions.some(o => o.value === saved)) {
      selectedFilter.value = saved
    }
  } catch {}
}

function applyPageSupportFilter(content) {
  content.querySelectorAll('.api-page-support-hidden').forEach(el => {
    el.classList.remove('api-page-support-hidden')
  })

  const unsupported = isApiRouteUnsupported(
    theme.value.apiUnsupportedIndex,
    route.path,
    selectedFilter.value
  )
  if (!unsupported) return

  const supportHeading = Array.from(content.querySelectorAll('h2')).find(
    heading => heading.id === '产品支持情况' ||
      heading.textContent.replace(/\u200b/g, '').trim() === '产品支持情况'
  )
  if (!supportHeading) return

  let section = supportHeading.nextElementSibling
  while (section && section.tagName !== 'H2') {
    section = section.nextElementSibling
  }
  while (section) {
    section.classList.add('api-page-support-hidden')
    section = section.nextElementSibling
  }
}

function applyFilter() {
  if (typeof document === 'undefined') return

  const content = document.querySelector('.vp-doc')
  if (!content) return

  const allFilterElements = content.querySelectorAll('[data-filter]')
  content.querySelectorAll('tr').forEach(tr => { tr.style.display = '' })

  if (selectedFilter.value === 'all') {
    allFilterElements.forEach(el => { el.style.display = '' })
  } else {
    allFilterElements.forEach(el => {
      if (el.tagName === 'TR') return
      const filterValue = el.getAttribute('data-filter')
      const match = filterValue.split(',').map(s => s.trim()).includes(selectedFilter.value)
      el.style.display = match ? '' : 'none'
    })
    content.querySelectorAll('tr').forEach(tr => {
      if (tr.hasAttribute('data-filter')) {
        const filterValue = tr.getAttribute('data-filter')
        const match = filterValue.split(',').map(s => s.trim()).includes(selectedFilter.value)
        tr.style.display = match ? '' : 'none'
        return
      }
      const filterDivs = tr.querySelectorAll('[data-filter]')
      if (filterDivs.length === 0) return
      const anyVisible = Array.from(filterDivs).some(el => el.style.display !== 'none')
      tr.style.display = anyVisible ? '' : 'none'
    })
  }

  applyPageSupportFilter(content)
}

function onFilterChange() {
  localStorage.setItem('cann-filter-selection', selectedFilter.value)
  applyFilter()
}

let filterTimer = null

onMounted(async () => {
  loadSavedFilter()
  isApiPage.value = route.path.startsWith('/api/')
  await nextTick()
  filterTimer = setTimeout(applyFilter, 100)
})

watch(() => route.path, async (path) => {
  isApiPage.value = path.startsWith('/api/')
  if (isApiPage.value) {
    await nextTick()
    clearTimeout(filterTimer)
    filterTimer = setTimeout(applyFilter, 200)
  }
})

onUnmounted(() => clearTimeout(filterTimer))
</script>

<template>
  <div class="cann-filter-bar" :class="{ hidden: !isApiPage }">
    <span class="filter-label">版本</span>
    <select v-model="selectedFilter" class="filter-select" :disabled="!isApiPage" @change="onFilterChange">
      <option v-for="opt in filterOptions" :key="opt.value" :value="opt.value">
        {{ opt.label }}
      </option>
    </select>
  </div>
</template>
