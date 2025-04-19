# copy json files for testing
subdir('json')

test_sources = [
    'main.cpp',
    'exe_container_test.cpp',
    'json_check_test.cpp',
    'main_frame_test.cpp',
    'string_utils_test.cpp',
    'validator_test.cpp',
    'string_test.cpp',
    'vector_test.cpp',
    'json_test.cpp',
    'process_utils.cpp',
    'ring_buffer_test.cpp',
]

# build tests
test_exe = executable('unit_test',
    tuw_manifest + test_sources,
    dependencies : [tuw_dep, gtest_dep, gmock_dep],
    cpp_args: tuw_cpp_args,
    link_args: tuw_link_args,
    install : false)

test('unit_test', test_exe)

python = import('python').find_installation()
configure_file(input : 'cli_test.py', output : 'cli_test.py', copy: true)
test('cli_test',
    python,
    args: ['cli_test.py'],
    workdir: meson.current_build_dir(),
)
