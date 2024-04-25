idf_component_register(SRCS "command_tools.c" "wifi.c" "tcp_server.c" 
                    INCLUDE_DIRS "."
                    REQUIRES esp_wifi lwip nvs_flash )  
