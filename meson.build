project('SimpleCommandRunner', ['c', 'cpp'],
    meson_version: '>=0.58.0',
    default_options: [
        'buildtype=debug',              # build debug by default
        'default_library=shared',       # build shared libraries by default
        'warning_level=3',              # always max warnings
        'b_pch=false',                  # we don't want precompiled headers
        'b_staticpic=true',             # use PIC even for static libraries
        'c_std=c99',                    # strict C99
        'c_winlibs=',                   # we define our own Windows libraries
        'cpp_std=c++11',                # strict C++11
        'cpp_eh=sc',                    # shut the compiler up in some cases
        'cpp_winlibs=',                 # likewise as with c_winlibs
    ],)

libui_dep = dependency('libui', fallback : ['libui', 'libui_dep'])
rapidjson_dep = dependency('rapidjson', fallback : ['rapidjson', 'rapidjson_dep'])
subprocess_dep = dependency('subprocess', fallback : ['subprocess', 'subprocess_dep'])

scr_sources = []
scr_link_args = []
scr_cpp_args = ['-DRAPIDJSON_HAS_STDSTRING']
if host_machine.system() == 'windows'
    windows = import('windows')
    scr_sources += [
        windows.compile_resources('src/app.rc',
        depend_files: ['src/app.manifest']),
    ]
    if meson.get_compiler('c').get_id() == 'msvc'
        scr_cpp_args = ['/DRAPIDJSON_HAS_STDSTRING']
        scr_link_args = [
            '/LARGEADDRESSAWARE',
            '/INCREMENTAL:NO',
            '/MANIFEST:NO',
            '/ENTRY:wmainCRTStartup'
        ]
    else
        scr_link_args += [
            '-static',
            '-static-libgcc',
            '-static-libstdc++',
        ]
    endif
else
    scr_cpp_args += [
        '-Wno-unused-parameter',
        '-Wno-switch',
    ]
endif

scr_sources += [
    'src/main.cpp',
    'src/main_frame.cpp',
    'src/component.cpp',
    'src/exe_container.cpp',
    'src/json_utils.cpp',
    'src/std_path.cpp',
    'src/exec.cpp',
    'src/string_utils.cpp'
]

executable('SimpleCommandRunner',
    scr_sources,
    dependencies : [libui_dep, rapidjson_dep, subprocess_dep],
    cpp_args: scr_cpp_args,
    link_args: scr_link_args,
    include_directories: include_directories('include'),
    install : false)
