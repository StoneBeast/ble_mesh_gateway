#ifndef _IDF_FEATURE_INIT_H_
#define _IDF_FEATURE_INIT_H_

#include "esp_err.h"

esp_err_t idf_feature_init_nvs(void);
esp_err_t idf_feature_init_net(void);
esp_err_t idf_feature_init_os(void);

#endif // _IDF_FEATURE_INIT_H_
