test_exe = executable('exe_container', 'main.cpp',
    dependencies : [scr_dep, gtest_dep, gmock_dep],
    cpp_args: scr_cpp_args,
    link_args: scr_link_args,
    install : false)

test_args = [
    meson.project_source_root() + '/examples/all_keys/gui_definition.json'
]

test('exe_container', test_exe, args: test_args)
