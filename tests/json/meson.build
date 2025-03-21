# copy json files for testing
files = [
    'broken.json',
    'config_ascii.json',
    'config_utf.json',
    'relaxed.jsonc',
]

foreach f : files
    configure_file(input : f,
        output : f,
        copy: true)
endforeach

# copy example files for testing
configure_file(
    input : meson.project_source_root() +
            '/examples/all_keys/gui_definition.json',
    output : 'gui_definition.json',
    copy: true)

configure_file(
    input : meson.project_source_root() +
            '/examples/other_features/help/gui_definition.json',
    output : 'help.json',
    copy: true)
