glob_recurse(source tests *.h *.cpp)

add_executable(zeno ${source})

target_link_libraries(zeno PRIVATE gtest_main)

include(GoogleTest)
gtest_discover_tests(zeno)
