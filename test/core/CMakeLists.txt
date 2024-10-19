add_executable(test_cpu test_cpu.c)
target_link_libraries(test_cpu PRIVATE emueight unity)
add_test(NAME test_cpu COMMAND test_cpu)