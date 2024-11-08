#include "test_utils.h"


void wlm_test_compare_buffers(const unsigned char* buffer,
                              const unsigned char* write_buffer,
                              const size_t buffer_size,
                              const char* location)
{
    for (size_t i = 0; i < buffer_size; ++i)
    {
        if (buffer[i] != write_buffer[i])
        {
            printf("%s failed write:\n", location);
            printf(__FILE__ ":" STRINGIFY(__LINE__) " Failed comparison at index %llu:\n", (long long unsigned int)i);
            printf("    buffer:       '0x%02x'\n", buffer[i]);
            printf("    write_buffer: '0x%02x'\n", write_buffer[i]);
            puts("");

            int line_number = 0;
            for (size_t j = 0; j < buffer_size; ++j)
            {
                if (j == i)
                {
                    break;
                }

                if (line_number >= 60)
                {
                    puts("");
                    line_number = 0;
                }

                printf("0x%02x ", buffer[j]);
                line_number += 5;
            }

            puts("");
            puts("");

            printf("buffer:       ");
            line_number = sizeof("buffer:       ");
            for (size_t j = i; j < buffer_size; ++j)
            {
                if (line_number >= 60)
                {
                    puts("");
                    line_number = 0;
                }

                printf("0x%02x ", buffer[j]);
                line_number += 5;
            }
            puts("");

            printf("write_buffer: ");
            line_number = sizeof("buffer:       ");
            for (size_t j = i; j < buffer_size; ++j)
            {
                if (line_number >= 60)
                {
                    puts("");
                    line_number = 0;
                }

                printf("0x%02x ", write_buffer[j]);
                line_number += 5;
            }
            puts("");

            exit(EXIT_FAILURE);
        }
    }
}
