cmake_minimum_required(VERSION 3.13)

# ===RP2350ターゲットを強制的に設定する行 ===
# CMakeのキャッシュに「PICO_TARGET_CHIP=rp2350」を書き込む
set(PICO_TARGET_CHIP "rp2350" CACHE STRING "PICO Build platform" FORCE)

# RP2350のボード定義（必須でないが、より正確）
set(PICO_BOARD "pico2" CACHE STRING "PICO target board" FORCE)

# Pico SDKのインポート（このファイルが同じフォルダにあること）
include(pico_sdk_import.cmake)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

project(RQS-SSS_2025 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# SDK初期化
pico_sdk_init()

add_executable(rqs_sss
    rqs_sss.c
)

# 共通ライブラリをリンク
target_link_libraries(rqs_sss pico_stdlib hardware_i2c)

# WiFiチップ搭載のボード(Pico Wなど)のとき
if (PICO_CYW43_SUPPORTED)
    target_link_libraries(rqs_sss pico_cyw43_arch_none)
endif()

#元々あったexample_auto_set_url(blink)の箇所は削除してある
# map/bin/hex/uf2ファイルを生成
pico_add_extra_outputs(rqs_sss)
