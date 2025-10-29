# RP2350_Project/hal/CMakeLists.txt

# ライブラリ名を "hal_lib" と定義
add_library(hal_lib
    mad_gpio.c
    mad_timer.c
    mad_system.c
)

# このディレクトリ (hal/) もインクルードパスに追加
target_include_directories(hal_lib
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}    # hal/ 内部のヘッダー用
    PUBLIC ${PROJECT_SOURCE_DIR}       # <--- この行を追加
)

# pico-sdk のライブラリに依存することを指定
target_link_libraries(hal_lib
    pico_stdlib
    hardware_gpio
    hardware_timer
    hardware_uart
    # hardware_rtc
    hardware_i2c
    hardware_spi
    hardware_flash
)