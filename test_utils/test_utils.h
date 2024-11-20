#ifndef WOW_LOGIN_MESSAGES_TEST_UTILS_H
#define WOW_LOGIN_MESSAGES_TEST_UTILS_H

#include "wow_login_messages/wow_login_messages.h"

#include <stdio.h>

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)

#ifdef __cplusplus
extern "C" {
#endif


void wlm_test_compare_buffers(const unsigned char* buffer,
                              const unsigned char* write_buffer,
                              size_t buffer_size,
                              const char* location);

#ifdef __cplusplus
}
#endif


#endif
