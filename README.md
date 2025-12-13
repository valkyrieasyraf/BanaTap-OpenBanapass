# BanaTap

An alternative to the Banapassport API DLL for Wangan Midnight Maximum Tune.  

A `card.ini` file needs to be placed in the same directory as the DLL.  

```ini
[card]
chipId=cardchipid
accessCode=accessCode
```

If the `card.ini` file is not detected, the DLL will create one automatically.  
The chipId and accessCode within it will be randomly generated.  

In the game, press the `C` key on your keyboard to trigger the sending of card data.  

## Supported revisions

| Game | Revision | Output file |
|:----:|:----:|:--------:|
| WMMT6RR | `3.05.03` | `dist/w6w/3.05.03/bngrw.dll` |
| WMMT5DX+ | `3.05.00` | `dist/w5p/3.05.00/bngrw.dll` |

## Build

### Environmental requirements

- `MSYS2 MINGW64`  
- `meson`  
- `ninja`  

Please perform all of the following operations using `MSYS2 MINGW64`.  

### Install dependencies

```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-meson mingw-w64-x86_64-ninja
```

### Compilation

```bash
./build.sh
```

The output file is located in the `dist/` directory.  

## Add support

1. Copy an existing `dllmain_xxx_xxxxx.cpp` as a template, or create a new one from scratch.  

> You may need to find and set the correct `PATCH_OFFSET` address for the game.  

2. Add the following to `meson.build`:  

   ```meson
   library(
       '<dllmain_game_revision>',
       include_directories: ['src'],
       sources: common_sources + ['src/<dllmain_game_revision>.cpp'],
       name_prefix: '',
       link_args: ['-static'],
   )
   ```

   example:  

   ```meson
   # W6W 3.05.03
   library(
       'bngrw_w6w_30503',
       include_directories: ['src'],
       sources: common_sources + ['src/dllmain_w6w_30503.cpp'],
       name_prefix: '',
       link_args: ['-static'],
   )
   ```

3. Add the following to `build.sh`:  

   ```bash
   mkdir -p dist/<game>/<revision>
   cp build/<dllmain_game_revision>.dll dist/<game>/<revision>/bngrw.dll
   ```

   example:  

   ```meson
   mkdir -p dist/w6w/3.05.03
   cp build/bngrw_w6w_30503.dll dist/w6w/3.05.03/bngrw.dll
   ```

## Special thanks

- [OpenBanapass](https://github.com/ProjectAsakura/OpenBanapass)  
- [WanganArcadeLoader](https://github.com/vixen256/WanganArcadeLoader)  