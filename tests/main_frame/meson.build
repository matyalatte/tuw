test_exe = executable('main_frame',
    tuw_manifest + ['main.cpp'],
    dependencies : [tuw_dep, gtest_dep, gmock_dep],
    cpp_args: tuw_cpp_args,
    link_args: tuw_link_args,
    install : false)

test_args = [
    meson.project_source_root() + '/examples/all_keys/gui_definition.json',
    meson.current_source_dir() + '/config_ascii.json',
    meson.current_source_dir() + '/config_utf.json'
]

test('main_frame', test_exe, args: test_args)
