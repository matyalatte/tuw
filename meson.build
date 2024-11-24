project('Tuw', ['c', 'cpp'],
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

tuw_sources = []
tuw_manifest = []
tuw_link_args = []
tuw_cpp_args = []
tuw_OS = host_machine.system()
tuw_compiler = meson.get_compiler('c').get_id()
tuw_is_release = get_option('buildtype').startswith('release') or (get_option('buildtype').startswith('custom') and not get_option('debug'))

if tuw_OS == 'windows'
    windows = import('windows')
    tuw_manifest += [
        windows.compile_resources('src/app.rc',
        depend_files: ['src/app.manifest']),
    ]
    if tuw_compiler == 'msvc'
        tuw_link_args = [
            '/LARGEADDRESSAWARE',
            '/INCREMENTAL:NO',
            '/ENTRY:wmainCRTStartup'
        ]
        if get_option('use_ucrt')
            # These linker options make exe much smaller with UCRT,
            # but the built binary only supports Windows10 or later.
            tuw_link_args += ['/NODEFAULTLIB:libucrt.lib', '/DEFAULTLIB:ucrt.lib']
        endif
        if meson.version().version_compare('!=1.4.0')
            tuw_link_args += ['/MANIFEST:NO']
        endif

        if (get_option('optimization') == 's')
            tuw_cpp_args += ['/Os']
        endif

        if tuw_is_release
            # Note: b_lto doesn't seem to work with MSVC.
            tuw_cpp_args += ['/GL']
            tuw_link_args += ['/OPT:REF', '/OPT:ICF', '/LTCG']
        endif
    else
        warning('Your compiler (' + tuw_compiler + ') makes exe larger.'
                + ' It is recommended to use MSVC for Windows build.')
        tuw_link_args += [
            '-static',
            '-static-libgcc',
            '-static-libstdc++',
        ]
        # need this option for wmain()
        tuw_link_args += ['-municode']
    endif
elif tuw_OS == 'darwin'
    add_languages('objc', required: true)
    languages = ['c', 'cpp', 'objc']
    macosx_version_min = '-mmacosx-version-min=' + get_option('macosx_version_min')
    add_global_arguments(macosx_version_min, language: languages)
    add_global_link_arguments(macosx_version_min, language: languages)

    # Check if SDKs support universal binaries or not.
    arch = ['-arch', 'x86_64', '-arch', 'arm64']
    c_compiler = meson.get_compiler('c')
    result = c_compiler.run(
        'int main(void) { return 0; }',
        name : 'universal binary test',
        args: arch)
    if result.compiled()
        add_global_arguments(arch, language: languages)
        add_global_link_arguments(arch, language: languages)
    else
        warning('Universal build is disabled since your SDKs do not support it.')
    endif

    if not tuw_compiler.startswith('clang')
        warning('This project has NOT been tested with your compiler. (' + tuw_compiler + ')')
    endif
else
    tuw_link_args += ['-no-pie']
    tuw_cpp_args += ['-D__TUW_UNIX__']
    if tuw_compiler != 'gcc'
        warning('This project has NOT been tested with your compiler. (' + tuw_compiler + ')')
    endif
endif

if tuw_OS != 'windows'
    tuw_cpp_args += [
        '-Wno-unused-parameter',
        '-Wno-switch',
    ]
    if tuw_compiler.startswith('clang')
        tuw_cpp_args += ['-stdlib=libc++']
    endif
    if tuw_is_release and tuw_OS != 'sunos'
        # Note: solaris requires gld (not ld) to use --gc-sections
        tuw_cpp_args += ['-ffunction-sections', '-fdata-sections']
        if tuw_compiler == 'gcc'
            tuw_link_args += ['-Wl,--gc-sections']
        elif tuw_compiler.startswith('clang') and tuw_OS == 'darwin'
            tuw_link_args += ['-Wl,-dead_strip']
        endif
    endif
endif

libui_dep = dependency('libui', fallback : ['libui', 'libui_dep'])
rapidjson_dep = dependency('rapidjson', fallback : ['rapidjson', 'rapidjson_dep'])
subprocess_dep = dependency('subprocess', fallback : ['subprocess', 'subprocess_dep'])
env_utils_dep = dependency('env_utils', fallback : ['env_utils', 'env_utils_dep'])
tiny_str_match_dep = dependency('tiny_str_match', fallback : ['tiny_str_match', 'tiny_str_match_dep'])

tuw_dependencies = [libui_dep, rapidjson_dep, subprocess_dep, env_utils_dep, tiny_str_match_dep]

tuw_sources += [
    'src/main_frame.cpp',
    'src/component.cpp',
    'src/exe_container.cpp',
    'src/json_utils.cpp',
    'src/exec.cpp',
    'src/string_utils.cpp',
    'src/validator.cpp',
    'src/noex/string.cpp',
]

if get_option('build_exe')
    executable('Tuw',
        tuw_manifest + tuw_sources + ['src/main.cpp'],
        dependencies : tuw_dependencies,
        cpp_args: tuw_cpp_args,
        link_args: tuw_link_args,
        gnu_symbol_visibility: 'hidden',
        include_directories: include_directories('include'),
        install : false)
endif

if get_option('build_test')
    # build codes as a library
    tuw_lib = library('tuw_lib',
        tuw_manifest + tuw_sources,
        dependencies : tuw_dependencies,
        cpp_args: tuw_cpp_args,
        link_args: tuw_link_args,
        include_directories: include_directories('include'),
        build_rpath: '',
        install_rpath: '',
        name_prefix: 'lib',
        install: false,
        gnu_symbol_visibility: 'hidden',
        soversion: '',
        darwin_versions: [])

    tuw_dep = declare_dependency(include_directories: include_directories('include'),
        dependencies : tuw_dependencies,
	    link_with : tuw_lib)

    # get gtest
    gtest_proj = subproject('gtest')
    gtest_dep = gtest_proj.get_variable('gtest_dep')
    gmock_dep = gtest_proj.get_variable('gmock_dep')

    # build tests
    subdir('tests')
endif
