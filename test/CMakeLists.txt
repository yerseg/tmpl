add_executable(tmpl-tests)

file(GLOB_RECURSE all_headers LIST_DIRECTORIES FALSE "*.h")

target_sources(tmpl-tests PRIVATE
    ${all_headers}
    "main.cpp"
    "string_test.cpp"
    "worker_test.cpp"
)

target_link_libraries(tmpl-tests PRIVATE tmpl gtest)
