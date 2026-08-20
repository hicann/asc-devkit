<!--
  Copyright (c) 2026 Huawei Technologies Co., Ltd.
  This program is free software, you can redistribute it and/or modify it under the terms and conditions of
  CANN Open Software License Agreement Version 2.0 (the "License").
  Please refer to the License for details. You may not use this file except in compliance with the License.
  THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
  See LICENSE in the root of the software repository for the full text of the License.
-->

<script lang="ts" setup>
import { computed, nextTick, onMounted, ref, watch } from 'vue'

import { Command } from 'vue-command-palette'
import { getScrollOffset, useData, useRouter, withBase } from 'vitepress'
import { useLocalStorage, useMagicKeys } from '@vueuse/core'

import { searchConfig as _searchConfig } from 'virtual:pagefind'
import { isApiRouteUnsupported } from '../../../scripts/api-support.mjs'
import LogoPagefind from './LogoPagefind.vue'
import { selectedFilter } from './filter-state.js'
import { highlightSearchText, initializePagefindSearch, MAX_QUERY_LENGTH, searchPagefind } from './pagefind-search.mjs'
import type { SearchConfig } from './type'
import { formatShowDate } from './utils'

if (typeof window !== 'undefined' && !window.__pagefind__) {
  import('/pagefind/pagefind.js').then(m => {
    if (!window.__pagefind__) {
      window.__pagefind__ = m
    }
  }).catch(() => {})
}

const searchResult = ref<{ route: string; meta: Record<string, any>; score: number }[]>([])
const searchConfig: SearchConfig = _searchConfig

const { localeIndex, site, lang, theme } = useData()

const finalSearchConfig = computed<SearchConfig>(() => ({
  ...searchConfig,
  ...(searchConfig?.locales?.[localeIndex.value] || {})
}))

const ignorePublish = computed(() => finalSearchConfig.value?.ignorePublish ?? false)
const showDateInfo = computed(() => finalSearchConfig.value?.showDate ?? false)
const formatShowDateFn = computed(() => typeof finalSearchConfig.value.showDate === 'function' ? finalSearchConfig.value.showDate : formatShowDate)

const headingText = computed(() => {
  return finalSearchConfig.value?.heading
    ? finalSearchConfig.value.heading.replace(
      /\{\{searchResult\}\}/,
      `${searchResult.value.length}`
    )
    : `Total: ${searchResult.value.length} search results.`
})

const metaKey = ref('')
onMounted(() => {
  metaKey.value = /(Mac|iPhone|iPod|iPad)/i.test(navigator?.platform)
    ? '⌘'
    : 'Ctrl'
})

const searchModal = ref(false)
function showSearchModal() {
  searchModal.value = true
}
function hideSearchModal() {
  searchModal.value = false
}

const keys = useMagicKeys({
  passive: false,
  onEventFired(e) {
    if (e.ctrlKey && e.key === 'k' && e.type === 'keydown')
      e.preventDefault()
  }
})

const CmdK = keys['Meta+K']
const CtrlK = keys['Ctrl+K']
const Escape = keys['Escape']

watch(CmdK, (v) => {
  if (v) {
    showSearchModal()
  }
})
watch(CtrlK, (v) => {
  if (v) {
    showSearchModal()
  }
})
watch(Escape, (v) => {
  if (v) {
    hideSearchModal()
  }
})

const searchWords = ref('')
const searching = ref(false)
function inlineSearch() {
  if (!searchWords.value) {
    searchResult.value = []
    return
  }
  searchResult.value = [{
    route: '#',
    meta: {
      title: 'Only works after build',
      description: '<mark>only support after build</mark>, only support after build'
    },
    score: 0,
  }]
}

const searchDelayTime = computed(() => finalSearchConfig.value?.delay ?? 300)
let activeSearchRequest = 0
let pagefindInitialization: Promise<any> | null = null

function getPagefind() {
  if (!pagefindInitialization) {
    pagefindInitialization = initializePagefindSearch(window.__pagefind__).catch((error: unknown) => {
      pagefindInitialization = null
      throw error
    })
  }
  return pagefindInitialization
}

watch(
  [() => searchWords.value, () => selectedFilter.value],
  async () => {
    const request = ++activeSearchRequest
    if (!searchWords.value) {
      searchResult.value = []
      searching.value = false
      return
    }

    searching.value = true
    const searchText
      = typeof finalSearchConfig.value.customSearchQuery === 'function'
        ? finalSearchConfig.value.customSearchQuery(searchWords.value)
        : searchWords.value

    try {
      await new Promise(resolve => setTimeout(resolve, searchDelayTime.value))
      if (request !== activeSearchRequest) return
      if (!window?.__pagefind__?.search) {
        inlineSearch()
        return
      }

      const pagefind = await getPagefind()
      const rankedResults = await searchPagefind(
        pagefind,
        String(searchText).normalize('NFKC').trim(),
        route => !isApiRouteUnsupported(theme.value.apiUnsupportedIndex, route, selectedFilter.value)
      )
      if (request !== activeSearchRequest) return
      const allResults = rankedResults
        .map(result => ({
          route: result.route.startsWith(site.value.base) ? result.route : withBase(result.route),
          meta: result.meta,
          result: result.result,
          score: result.score,
        }))
        .filter(result => ignorePublish.value || result.meta.publish !== false)
      if (finalSearchConfig.value.sort) {
        allResults.sort(finalSearchConfig.value.sort)
      }
      searchResult.value = allResults.filter(
        finalSearchConfig.value.filter ?? (() => true)
      )
    }
    catch {
      if (request === activeSearchRequest) searchResult.value = []
    }
    finally {
      if (request === activeSearchRequest) searching.value = false
    }
  }
)

function handleClickMask(e: any) {
  if (e.target === e.currentTarget) {
    hideSearchModal()
  }
}
watch(
  () => searchModal.value,
  (newValue) => {
    if (newValue) {
      document.body.style.overflow = 'hidden'
      nextTick(() => {
        document
          .querySelector('div[command-dialog-mask]')
          ?.addEventListener('click', handleClickMask)
      })
    }
    else {
      document.body.style.overflow = ''
      document
        .querySelector('div[command-dialog-mask]')
          ?.removeEventListener('click', handleClickMask)
    }
  }
)

const pageSize = ref(999)
const currentPage = ref(0)
const showSearchResult = computed(() => {
  const pageIdx
    = currentPage.value % Math.ceil(searchResult.value.length / pageSize.value)
  const startIdx = pageIdx * pageSize.value
  return searchResult.value.slice(startIdx, startIdx + pageSize.value)
})

const router = useRouter()
let activeNavigation = 0

function alignSection(hash: string, navigation: number, attempt = 0) {
  if (navigation !== activeNavigation || location.hash !== hash) return
  try {
    const section = document.getElementById(decodeURIComponent(hash).slice(1))
    if (!section) return
    const top = window.scrollY + section.getBoundingClientRect().top - getScrollOffset()
    window.scrollTo(0, Math.max(0, top))
    if (attempt < 3) {
      window.setTimeout(() => alignSection(hash, navigation, attempt + 1), (attempt + 1) * 100)
    }
  }
  catch {}
}

async function handleSelect(target: string) {
  const navigation = ++activeNavigation
  hideSearchModal()
  await router.go(target)
  const hash = new URL(target, location.origin).hash
  if (!hash || navigation !== activeNavigation) return
  await nextTick()
  requestAnimationFrame(() => alignSection(hash, navigation))
}

const langReload = computed(() => finalSearchConfig.value.langReload ?? true)
watch(
  () => lang.value,
  () => {
    if (import.meta.env.DEV) {
      return
    }
    if (langReload.value) {
      window.location.reload()
    }
  }
)

const searchInput = ref<HTMLInputElement>()
function handleClearSearch() {
  searchWords.value = ''
  nextTick(() => {
    if (!searchInput.value)
      return
    searchInput.value.$el.value = ''
  })
}

const showDetail = useLocalStorage('pagefind-search-showDetail', false)
function handleToggleDetail() {
  showDetail.value = !showDetail.value
}

function stripExt(url: string) {
  return url.replace(/\.html?$/, '')
}

function breadcrumb(url: string): string[] {
  const pathPart = url.split('#')[0]
  const segments = stripExt(pathPart).replace(/\/$/, '').split('/').filter(Boolean).map(decodeURIComponent)
  if (segments.length <= 1) return []
  const topLevels = ['api', 'guide', 'zh', 'en']
  let start = topLevels.includes(segments[0]) ? 1 : 0
  return segments.slice(start)
}

function dirLabel(url: string) {
  return breadcrumb(url).join(' > ')
}

</script>

<template>
  <div class="blog-search" data-pagefind-ignore="all">
    <div class="nav-search-btn-wait" @click="searchModal = true">
      <span>
        <svg width="14" height="14" viewBox="0 0 20 20">
          <path
            d="M14.386 14.386l4.0877 4.0877-4.0877-4.0877c-2.9418 2.9419-7.7115 2.9419-10.6533 0-2.9419-2.9418-2.9419-7.7115 0-10.6533 2.9418-2.9419 7.7115-2.9419 10.6533 0 2.9419 2.9418 2.9419 7.7115 0 10.6533z"
            stroke="currentColor" fill="none" fill-rule="evenodd" stroke-linecap="round" stroke-linejoin="round"
          />
        </svg>
      </span>
      <span class="search-tip">{{
        finalSearchConfig?.btnPlaceholder || 'Search'
      }}</span>
      <span v-if="metaKey" class="metaKey"> {{ metaKey }} K </span>
    </div>
    <ClientOnly>
      <Command.Dialog :visible="searchModal" theme="algolia">
        <template #header>
          <div class="search-bar">
            <div class="search-actions before">
              <button class="back-button" title="Close search" @click="searchModal = false">
                <span class="vpi-arrow-left local-search-icon" />
              </button>
            </div>
            <Command.Input
              ref="searchInput" v-model:value="searchWords"
              :maxlength="MAX_QUERY_LENGTH"
              :placeholder="finalSearchConfig?.placeholder || 'Search Docs'"
            />
            <div class="search-actions">
              <button
                :class="{ active: showDetail }" class="toggle-layout-button" type="button"
                title="Display detailed list" @click="handleToggleDetail"
              >
                <span class="vpi-layout-list local-search-icon" />
              </button>
              <button
                :disabled="!searchWords" class="clear-button" type="reset" title="Reset search"
                @click="handleClearSearch"
              >
                <span class="vpi-delete local-search-icon" />
              </button>
            </div>
          </div>
        </template>
        <template #body>
          <div class="search-dialog" :class="{ 'detail-list': showDetail }">
            <Command.List>
              <div v-if="searching" class="search-loading">
                <span class="search-spinner"></span> 搜索中...
              </div>
              <Command.Empty v-else-if="!searchResult.length">
                {{ finalSearchConfig?.emptyText || 'No results found.' }}
              </Command.Empty>
              <Command.Group v-else :heading="headingText">
                <Command.Item
                  v-for="item in showSearchResult" :key="item.route" :data-route="item.route"
                  :data-value="`${searchWords} ${item.meta.title}`" @select="handleSelect(item.route)"
                >
                  <div class="link">
                    <div v-if="dirLabel(item.route)" class="breadcrumb-row">{{ dirLabel(item.route) }}</div>
                    <div class="title">
                      <span
                        class="headings"
                        v-html="highlightSearchText(item.meta.title, searchWords)"
                      />
                      <span
                        v-if="item.meta.sectionTitle" class="section-title"
                        v-html="highlightSearchText(item.meta.sectionTitle, searchWords)"
                      />
                      <span v-if="showDateInfo && item.meta.date" class="date">
                        {{ formatShowDateFn(item.meta.date, lang) }}</span>
                    </div>
                    <div class="des" v-html="item.meta.description" />
                  </div>
                </Command.Item>
              </Command.Group>
            </Command.List>
          </div>
        </template>
        <template v-if="searchResult.length" #footer>
          <div class="command-palette-logo">
            <a href="https://github.com/cloudcannon/pagefind" target="_blank" rel="noopener noreferrer">
              <span class="command-palette-Label">{{ finalSearchConfig?.searchBy || 'Search by' }}</span>
              <LogoPagefind style="width: 77px" />
            </a>
          </div>
          <ul class="command-palette-commands">
            <li>
              <kbd class="command-palette-commands-key"><svg width="15" height="15" aria-label="Enter key" role="img">
                <g
                  fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round"
                  stroke-width="1.2"
                >
                  <path d="M12 3.53088v3c0 1-1 2-2 2H4M7 11.53088l-3-3 3-3" />
                </g>
              </svg></kbd><span class="command-palette-Label">{{ finalSearchConfig?.toSelect || 'to select' }}</span>
            </li>
            <li>
              <kbd class="command-palette-commands-key"><svg width="15" height="15" aria-label="Arrow down" role="img">
                <g
                  fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round"
                  stroke-width="1.2"
                >
                  <path d="M7.5 3.5v8M10.5 8.5l-3 3-3-3" />
                </g>
              </svg></kbd><kbd class="command-palette-commands-key"><svg
                width="15" height="15" aria-label="Arrow up"
                role="img"
              >
                <g
                  fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round"
                  stroke-width="1.2"
                >
                  <path d="M7.5 11.5v-8M10.5 6.5l-3-3-3 3" />
                </g>
              </svg></kbd><span class="command-palette-Label">{{ finalSearchConfig?.toNavigate || 'to navigate' }}</span>
            </li>
            <li>
              <kbd class="command-palette-commands-key"><svg width="15" height="15" aria-label="Escape key" role="img">
                <g
                  fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round"
                  stroke-width="1.2"
                >
                  <path
                    d="M13.6167 8.936c-.1065.3583-.6883.962-1.4875.962-.7993 0-1.653-.9165-1.653-2.1258v-.5678c0-1.2548.7896-2.1016 1.653-2.1016.8634 0 1.3601.4778 1.4875 1.0724M9 6c-.1352-.4735-.7506-.9219-1.46-.8972-.7092.0246-1.344.57-1.344 1.2166s.4198.8812 1.3445.9805C8.465 7.3992 8.968 7.9337 9 8.5c.032.5663-.454 1.398-1.4595 1.398C6.6593 9.898 6 9 5.963 8.4851m-1.4748.5368c-.2635.5941-.8099.876-1.5443.876s-1.7073-.6248-1.7073-2.204v-.4603c0-1.0416.721-2.131 1.7073-2.131.9864 0 1.6425 1.031 1.5443 2.2492h-2.956"
                  />
                </g>
              </svg></kbd><span class="command-palette-Label">{{ finalSearchConfig?.toClose || 'to close' }}</span>
            </li>
          </ul>
        </template>
      </Command.Dialog>
    </ClientOnly>
  </div>
</template>

<style lang="css" scoped>
.blog-search {
  flex: 1;
  display: flex;
  padding-left: 32px;
}

.blog-search>.nav-search-btn-wait {
  cursor: pointer;
  display: flex;
  align-items: center;
  justify-content: center;
  box-sizing: border-box;
  border: 1px solid transparent;
  border-radius: 6px;
  transition: .2s border;
  border-radius: 8px;
  padding: 0 10px 0 12px;
  height: 40px;
  background-color: var(--vp-c-bg-alt);
}

.blog-search .nav-search-btn-wait .metaKey {
  margin-left: 10px;
  font-size: 12px;
  border: 1px solid var(--vp-c-divider);
  border-radius: 4px;
  padding: 0 6px;
}

.blog-search .nav-search-btn-wait:hover {
  border: 1px solid var(--vp-c-brand-1);
  border-radius: 6px;
}

.blog-search .nav-search-btn-wait .search-tip {
  color: #909399;
  font-size: 12px;
  padding-left: 8px;
  padding-right: 16px;
}

@media screen and (max-width: 759px) {
  .blog-search>.nav-search-btn-wait {
    background-color: inherit;
  }

  .metaKey {
    display: none;
  }

  .search-tip {
    display: none;
  }

  .blog-search {
    flex: 0;
  }
}

.search-bar {
  display: flex;
  cursor: text;
  align-items: center;
  border-radius: 4px;
  border: 1px solid var(--vcp-c-brand);
}

.search-bar input {
  width: 100%;
}

.search-bar .search-actions {
  display: flex;
  gap: 4px;
  padding-right: 12px;
}

.search-bar .search-actions.before {
  padding: 0;
}

.search-actions button {
  padding: 8px;
}

.local-search-icon {
  display: block;
  font-size: 18px;
}

.search-actions button.clear-button:disabled {
  opacity: 0.37;
}

.search-actions button:not([disabled]):hover,
.search-actions button.active:not([disabled]) {
  color: var(--vp-c-brand-1);
}

.search-actions.before {
  display: none;
}

@media screen and (max-width: 560px) {
  .search-actions.before {
    display: flex;
  }
}

.breadcrumb-row {
  font-size: 12px;
  color: var(--vp-c-text-2);
  line-height: 1.4;
}

.link .des {
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
}

.detail-list .link .des {
  white-space: wrap;
  display: -webkit-box;
  -webkit-line-clamp: 3;
  -webkit-box-orient: vertical;
}
</style>

<style lang="css">
.search-loading {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 8px;
  padding: 32px 0;
  color: var(--vp-c-text-2);
  font-size: 14px;
}

.search-spinner {
  width: 18px;
  height: 18px;
  border: 2px solid var(--vp-c-divider);
  border-top-color: var(--vp-c-brand-1);
  border-radius: 50%;
  animation: search-spin 0.6s linear infinite;
}

@keyframes search-spin {
  to { transform: rotate(360deg); }
}
</style>

<style lang="css">
@import './assets/scss/search.css';
</style>
