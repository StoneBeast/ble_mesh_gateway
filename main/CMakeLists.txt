set(srcs "main.c")

idf_component_register(SRCS "board.c" "${srcs}" "idf_feature_init.c"
                    INCLUDE_DIRS  "."
                    REQUIRES ble_mesh nvs_flash driver freertos wifi)
