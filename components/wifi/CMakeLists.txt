idf_component_register(SRCS "wifi.c" "tcp_server.c" 
                    INCLUDE_DIRS "."
                    REQUIRES esp_wifi lwip)  
