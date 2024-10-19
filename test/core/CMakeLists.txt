add_executable(test_cpu test_cpu.c)
target_link_libraries(test_cpu PRIVATE emueight unity)
add_test(NAME test_cpu COMMAND test_cpu)

list(APPEND COVERAGE_EXCLUDES "")
list(APPEND GCOVR_XML_CMD_COMMAND "")
list(APPEND GCOVR_XML_CMD_COMMENT "")
list(APPEND GCOVR_XML_CMD_BYPRODUCTS "")
list(APPEND COVERAGE_GCOVR_EXCLUDES "")
list(APPEND GCOVR_ADDITIONAL_ARGS "")


setup_target_for_coverage_gcovr_xml(
     NAME ctest_coverage                    # New target name
     EXECUTABLE test_cpu  # Executable in PROJECT_BINARY_DIR
     DEPENDENCIES test_cpu emueight unity        # Dependencies to build first
     BASE_DIRECTORY "${CMAKE_SOURCE_DIR}"                                       #  (defaults to PROJECT_SOURCE_DIR)
     #EXCLUDE "src/dir1/*" "src/dir2/*"      # Patterns to exclude (can be relative
                                            #  to BASE_DIRECTORY, with CMake 3.4+)
)