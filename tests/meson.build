source_root = meson.project_source_root().replace('\\', '\\\\')
current_dir = meson.current_source_dir().replace('\\', '\\\\')
all_keys_json_path = source_root + '/examples/all_keys/gui_definition.json'
test_json_dir = current_dir + '/json/'

test_conf = configuration_data()
test_conf.set('ALL_KEYS_JSON_PATH', all_keys_json_path)
test_conf.set('TEST_JSON_DIR', test_json_dir)
configure_file(input : 'json_paths.h.in',
               output : 'json_paths.h',
               configuration : test_conf)

test_exe = executable('unit_test',
    tuw_manifest + ['main.cpp'],
    dependencies : [tuw_dep, gtest_dep, gmock_dep],
    cpp_args: tuw_cpp_args,
    link_args: tuw_link_args,
    install : false)

test('unit_test', test_exe)
