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
examples = [
    ['all_keys/gui_definition.json', 'gui_definition.json'],
    ['other_features/help/gui_definition.json', 'help.json'],
    ['other_features/cross_platform/gui_definition.json', 'platform.json'],
]
foreach e : examples
    configure_file(
        input : meson.project_source_root() + '/examples/' + e[0],
        output : e[1],
        copy: true)
endforeach
