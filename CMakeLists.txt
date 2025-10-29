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

add_subdirectory(hal)

# 2. メインの実行ファイル (main.c と、ロジック部の .c)
add_executable(${CMAKE_PROJECT_NAME}
    main.c
    # mad_gps.c
)

# 9. メインの実行ファイルに、移植したHALライブラリをリンクする
target_link_libraries(${CMAKE_PROJECT_NAME}
    pico_stdlib
    hardware_gpio
    hardware_timer
    hardware_uart  # (mad_usart.c で使用)
    # hardware_rtc   # (mad_system.c / mad_rtc.c で使用)
    hal_lib        # 5. hal/CMakeLists.txt で定義したライブラリ名
)

# USB経由の printf (stdio) を有効化
pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)
pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 0) # 0 = UART0 (PC_UART)

pico_add_extra_outputs(${CMAKE_PROJECT_NAME})