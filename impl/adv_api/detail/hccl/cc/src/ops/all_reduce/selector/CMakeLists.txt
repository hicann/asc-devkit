set(src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/all_reduce_auto_selector.cc
)

target_sources(mc2_client PRIVATE
    ${src_list}
)
