project('BanaTap', 'c', 'cpp', version: '1.0.0',
    default_options: [
        'buildtype=release',
        'optimization=s',
        'strip=true',
        'b_lto=true',
        'cpp_std=c++11',
    ]
)

cpp = meson.get_compiler('cpp')

add_project_arguments(
    cpp.get_supported_arguments(
        '-D_WIN32_WINNT=_WIN32_WINNT_WIN10',
        '-fno-exceptions',
        '-fno-rtti',
        '-ffunction-sections',
        '-fdata-sections',
        '-fno-ident',
    ),
    language: 'cpp',
)

add_project_link_arguments(
    cpp.get_supported_arguments(
        '-s',
        '-static-libgcc',
        '-static-libstdc++',
        '-Wl,--gc-sections',
        '-Wl,--exclude-all-symbols',
    ),
    language: 'cpp',
)

# MinHook subproject
minhook_proj = subproject('minhook')
minhook_dep = minhook_proj.get_variable('minhook_dep')

# Shared source files
common_sources = [
    'src/banapass.cpp',
]

# W6W 3.05.03
library(
    'bngrw_w6w_30503',
    include_directories: ['src'],
    sources: common_sources + ['src/dllmain_w6w_30503.cpp'],
    name_prefix: '',
    link_args: ['-static'],
)

# W5P 3.05.00
library(
    'bngrw_w5p_30500',
    include_directories: ['src'],
    sources: common_sources + ['src/dllmain_w5p_30500.cpp'],
    dependencies: [minhook_dep],
    name_prefix: '',
    link_args: ['-static'],
)
