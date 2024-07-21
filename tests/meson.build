# copy json files for testing
subdir('json')

# build tests
test_exe = executable('unit_test',
    tuw_manifest + ['main.cpp'],
    dependencies : [tuw_dep, gtest_dep, gmock_dep],
    cpp_args: tuw_cpp_args,
    link_args: tuw_link_args,
    install : false)

test('unit_test', test_exe)
