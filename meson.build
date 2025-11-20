# Copyright (C) 2025  ilobilo

project('img2ascii', 'cpp',
    version: '0.0.1',
    license: 'EUPL-1.2',
    default_options: [
        'cpp_std=c++23',
        'warning_level=3'
    ]
)

src = files(
    'source/main.cpp',
    'source/stb.cpp'
)

inc = include_directories('source')

exe = executable('img2ascii', src,
    include_directories: inc,
    install: true
)