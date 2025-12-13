project('minhook', 'c')

minhook_inc = include_directories('include')

minhook_src = files(
    'src/buffer.c',
    'src/hook.c',
    'src/trampoline.c',
    'src/hde/hde32.c',
    'src/hde/hde64.c',
)

minhook_lib = static_library(
    'minhook',
    minhook_src,
    include_directories: minhook_inc,
)

minhook_dep = declare_dependency(
    include_directories: minhook_inc,
    link_with: minhook_lib,
)
