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
import { ref, computed } from 'vue'
import { useRoute } from 'vitepress'
import { stripFrontMatter } from './frontmatter.mjs'

const route = useRoute()
const loading = ref(false)
const progress = ref('')
const isApiPage = computed(() => route.path.startsWith('/api/'))

function getFilterVersion() {
  try {
    return localStorage.getItem('cann-filter-selection') || 'all'
  } catch {
    return 'all'
  }
}

function filterNpuComments(src, version) {
  const tokenRegex = /<!--\s*(?:npu\s*=\s*"([^"]*)"\s+(\w+)|end\s+(\w+))\s*-->/g
  if (version === 'all') {
    return src.replace(tokenRegex, '')
  }
  const parts = []
  const stack = []
  let lastIdx = 0
  let match
  while ((match = tokenRegex.exec(src)) !== null) {
    const pos = match.index
    const npuType = match[1]
    const openId = match[2]
    const closeId = match[3]
    const before = src.slice(lastIdx, pos)
    const suppressed = stack.length > 0 && !stack[stack.length - 1].include
    if (!suppressed) parts.push(before)
    lastIdx = pos + match[0].length
    if (closeId) {
      if (stack.length > 0 && stack[stack.length - 1].id === closeId) {
        stack.pop()
      }
    } else if (npuType && openId) {
      const types = npuType.split(',').map(s => s.trim())
      stack.push({ id: openId, include: types.includes(version) })
    }
  }
  if (stack.length === 0 || stack[stack.length - 1].include) {
    parts.push(src.slice(lastIdx))
  }
  return parts.join('')
}

function filterContentByVersion(content, version) {
  const htmlBlockRegex = /<p>\s*CANNFILTER_DIV_([A-Za-z0-9_,]+)_OPEN\s*<\/p>([\s\S]*?)<p>\s*CANNFILTER_DIV_\1_CLOSE\s*<\/p>/g
  const plainBlockRegex = /CANNFILTER_DIV_([A-Za-z0-9_,]+)_OPEN[ \t]*\n?([\s\S]*?)CANNFILTER_DIV_\1_CLOSE/g

  content = filterNpuComments(content, version)
  content = filterCannFilterTags(content, version)

  if (version === 'all') {
    content = content.replace(htmlBlockRegex, '$2')
    content = content.replace(plainBlockRegex, '$2')
  } else {
    content = content.replace(htmlBlockRegex, (match, ver, inner) => ver === version ? inner : '')
    content = content.replace(plainBlockRegex, (match, ver, inner) => ver === version ? inner : '')
  }

  content = content.replace(/<term\b[^>]*>([\s\S]*?)<\/term>/gi, '$1')
  content = content.replace(/<ph\b[^>]*>([\s\S]*?)<\/ph>/gi, '$1')
  content = content.replace(/<__gm__\b[^>]*>([\s\S]*?)<\/__gm__>/gi, '$1')
  content = content.replace(/<__ubuf__\b[^>]*>([\s\S]*?)<\/__ubuf__>/gi, '$1')
  return content
}

function filterCannFilterTags(src, version) {
  const tagRegex = /<(\/?)(cann-filter)((?:\s[^>]*)?)>/gi
  const stack = []
  const parts = []
  let lastIdx = 0
  let match
  while ((match = tagRegex.exec(src)) !== null) {
    const pos = match.index
    const isClose = match[1] === '/'
    const attrs = match[3]
    const before = src.slice(lastIdx, pos)
    const suppress = stack.length > 0 && stack[stack.length - 1]

    if (isClose) {
      if (stack.length > 0) {
        stack.pop()
      }
      if (!suppress) {
        const curSuppress = stack.length > 0 && stack[stack.length - 1]
        if (!curSuppress) parts.push(before)
      }
      lastIdx = pos + match[0].length
    } else {
      if (!suppress) parts.push(before)
      const attrMatch = attrs.match(/npu[_-]type\s*=\s*"([^"]+)"/i)
      if (attrMatch && version !== 'all') {
        const types = attrMatch[1].split(',').map(s => s.trim())
        const include = types.includes(version)
        stack.push(!include)
      } else {
        stack.push(false)
      }
      lastIdx = pos + match[0].length
    }
  }
  parts.push(src.slice(lastIdx))
  const result = parts.join('')
  return result.replace(/<\/?cann-filter\b[^>]*>/gi, '')
}

function timestamp() {
  const now = new Date()
  const y = now.getFullYear()
  const m = String(now.getMonth() + 1).padStart(2, '0')
  const d = String(now.getDate()).padStart(2, '0')
  const h = String(now.getHours()).padStart(2, '0')
  const min = String(now.getMinutes()).padStart(2, '0')
  const s = String(now.getSeconds()).padStart(2, '0')
  return y + m + d + '-' + h + min + s
}

function downloadBlob(blob, filename) {
  const url = URL.createObjectURL(blob)
  const a = document.createElement('a')
  a.href = url
  a.download = filename
  document.body.appendChild(a)
  a.click()
  document.body.removeChild(a)
  URL.revokeObjectURL(url)
}

async function handleDownload() {
  const version = getFilterVersion()
  const versionLabel = version === 'all' ? 'all' : ('v' + version)

  loading.value = true
  progress.value = '下载数据中...'

  try {
    const resp = await fetch('/api-source/bundle.json')
    if (!resp.ok) throw new Error(`Bundle not found (HTTP ${resp.status})`)

    const total = Number(resp.headers.get('content-length') || '0')
    const reader = resp.body.getReader()
    const chunks = []
    let received = 0

    while (true) {
      const { done, value } = await reader.read()
      if (done) break
      chunks.push(value)
      received += value.length
      if (total > 0) {
        progress.value = `下载中 ${Math.round(received / total * 100)}%`
      }
    }

    progress.value = '解析数据中...'
    const rawBlob = new Blob(chunks)
    const text = await rawBlob.text()
    const bundle = JSON.parse(text)

    const entries = Object.entries(bundle)
    const fileTotal = entries.length
    if (fileTotal === 0) {
      console.error('No source files in bundle')
      return
    }

    progress.value = '打包中 0%'
    const JSZip = (await import('jszip')).default
    const zip = new JSZip()
    let processed = 0

    for (const [relPath, content] of entries) {
      const filtered = stripFrontMatter(filterContentByVersion(content, version))
      zip.file(relPath, filtered)
      processed++
      if (processed % 100 === 0) {
        const pct = Math.round(processed / fileTotal * 100)
        progress.value = `打包中 ${pct}%`
        await new Promise(r => setTimeout(r, 0))
      }
    }

    progress.value = '生成 ZIP...'
    const blob = await zip.generateAsync({ type: 'blob' })
    const ts = timestamp()
    downloadBlob(blob, 'AscendC-API_' + versionLabel + '_' + ts + '.zip')
    progress.value = ''
  } catch (err) {
    console.error('Download failed:', err)
    progress.value = `失败: ${err.message}`
    setTimeout(() => {
      if (progress.value.startsWith('失败:')) {
        progress.value = ''
      }
    }, 5000)
  } finally {
    loading.value = false
  }
}
</script>

<template>
  <div v-if="isApiPage" class="api-download-wrapper">
    <button
      class="api-download-btn"
      :class="{ loading }"
      :disabled="loading"
      :title="loading ? progress : '下载全部 API Markdown'"
      @click="handleDownload"
    >
      <svg
        v-if="!loading"
        width="20"
        height="20"
        viewBox="0 0 24 24"
        fill="none"
        stroke="currentColor"
        stroke-width="2"
        stroke-linecap="round"
        stroke-linejoin="round"
      >
        <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4" />
        <polyline points="7 10 12 15 17 10" />
        <line x1="12" y1="15" x2="12" y2="3" />
      </svg>
      <span v-else class="spinner" />
    </button>
    <div v-if="progress" class="download-progress" :class="{ error: progress.startsWith('失败:') }">{{ progress }}</div>
  </div>
</template>

<style scoped>
.api-download-wrapper {
  position: fixed;
  bottom: 40px;
  right: 96px;
  z-index: 99;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 6px;
}

.api-download-btn {
  width: 44px;
  height: 44px;
  border: 1px solid var(--vp-c-divider);
  border-radius: 50%;
  background: var(--vp-c-bg-soft);
  color: var(--vp-c-text-2);
  cursor: pointer;
  display: flex;
  align-items: center;
  justify-content: center;
  transition: all 0.3s;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.api-download-btn:hover:not(:disabled) {
  border-color: var(--vp-c-brand-1);
  color: var(--vp-c-brand-1);
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
}

.api-download-btn:disabled {
  cursor: wait;
  opacity: 0.8;
}

.api-download-btn.loading {
  border-color: var(--vp-c-brand-1);
  color: var(--vp-c-brand-1);
}

.spinner {
  width: 18px;
  height: 18px;
  border: 2px solid var(--vp-c-divider);
  border-top-color: var(--vp-c-brand-1);
  border-radius: 50%;
  animation: spin 0.8s linear infinite;
}

.download-progress {
  font-size: 11px;
  color: var(--vp-c-text-2);
  white-space: nowrap;
  background: var(--vp-c-bg-soft);
  padding: 2px 8px;
  border-radius: 4px;
  border: 1px solid var(--vp-c-divider);
}

.download-progress.error {
  color: var(--vp-c-danger-1);
  border-color: var(--vp-c-danger-1);
}

@keyframes spin {
  to { transform: rotate(360deg); }
}
</style>
