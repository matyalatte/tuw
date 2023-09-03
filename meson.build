project('SimpleCommandRunner', ['c', 'cpp'],
    meson_version: '>=0.63.0',
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

scr_sources = []
scr_manifest = []
scr_link_args = []
scr_cpp_args = ['-DRAPIDJSON_HAS_STDSTRING']
scr_OS = host_machine.system()
scr_compiler = meson.get_compiler('c').get_id()
scr_is_release = get_option('buildtype').startswith('release')

if scr_OS == 'windows'
    windows = import('windows')
    scr_manifest += [
        windows.compile_resources('src/app.rc',
        depend_files: ['src/app.manifest']),
    ]
    if scr_compiler == 'msvc'
        scr_cpp_args = ['/DRAPIDJSON_HAS_STDSTRING']
        scr_link_args = [
            '/LARGEADDRESSAWARE',
            '/INCREMENTAL:NO',
            '/MANIFEST:NO',
            '/ENTRY:wmainCRTStartup'
        ]
    else
        warning('This project has NOT been tested with your compiler. (' + scr_compiler + ')')
        scr_link_args += [
            '-static',
            '-static-libgcc',
            '-static-libstdc++',
        ]
    endif
elif scr_OS == 'darwin'
    add_languages('objc', required: true)
    languages = ['c', 'cpp', 'objc']
    macosx_version_min = '-mmacosx-version-min=10.9'
    add_project_arguments(macosx_version_min, language: languages)
    add_project_link_arguments(macosx_version_min, language: languages)
    if get_option('macos_build_universal')
        arch = ['-arch', 'x86_64', '-arch', 'arm64']
        add_global_arguments(arch, language: languages)
        add_global_link_arguments(arch, language: languages)
    endif
    if not scr_compiler.startswith('clang')
        warning('This project has NOT been tested with your compiler. (' + scr_compiler + ')')
    endif
else
    if scr_OS != 'linux'
        warning('This OS is unsupported. (' + scr_OS + ')')
    endif
    scr_link_args += ['-no-pie']
    if scr_compiler != 'gcc'
        warning('This project has NOT been tested with your compiler. (' + scr_compiler + ')')
    endif
endif

if scr_OS != 'windows'
    scr_cpp_args += [
        '-Wno-unused-parameter',
        '-Wno-switch',
    ]
    if scr_compiler.startswith('clang')
        scr_cpp_args += ['-stdlib=libc++']
    endif
    if scr_is_release
        scr_cpp_args += ['-ffunction-sections', '-fdata-sections']
        if scr_compiler == 'gcc'
            scr_link_args += ['-Wl,--gc-sections']
        elif scr_compiler.startswith('clang')
            scr_link_args += ['-Wl,-dead_strip']
        endif
    endif
endif

libui_dep = dependency('libui', fallback : ['libui', 'libui_dep'])
rapidjson_dep = dependency('rapidjson', fallback : ['rapidjson', 'rapidjson_dep'])
subprocess_dep = dependency('subprocess', fallback : ['subprocess', 'subprocess_dep'])

scr_sources += [
    'src/main_frame.cpp',
    'src/component.cpp',
    'src/exe_container.cpp',
    'src/json_utils.cpp',
    'src/env_utils.cpp',
    'src/exec.cpp',
    'src/string_utils.cpp'
]

if get_option('build_exe')
    executable('SimpleCommandRunner',
        scr_manifest + scr_sources + ['src/main.cpp'],
        dependencies : [libui_dep, rapidjson_dep, subprocess_dep],
        cpp_args: scr_cpp_args,
        link_args: scr_link_args,
        include_directories: include_directories('include'),
        install : false)
endif

if get_option('build_test')
    # build codes as a library
    scr_lib = library('scr_lib',
        scr_manifest + scr_sources,
        dependencies : [libui_dep, rapidjson_dep, subprocess_dep],
        cpp_args: scr_cpp_args,
        link_args: scr_link_args,
        include_directories: include_directories('include'),
        build_rpath: '',
        install_rpath: '',
        name_prefix: 'lib',		# always call it libui, even in Windows DLLs
        install: false,
        gnu_symbol_visibility: 'hidden',
        soversion: '',
        darwin_versions: [])

    scr_dep = declare_dependency(include_directories: include_directories('include'),
        dependencies : [libui_dep, rapidjson_dep, subprocess_dep],
	    link_with : scr_lib)

    # get gtest
    gtest_proj = subproject('gtest')
    gtest_dep = gtest_proj.get_variable('gtest_dep')
    gmock_dep = gtest_proj.get_variable('gmock_dep')

    # build tests
    subdir('tests')
endif
