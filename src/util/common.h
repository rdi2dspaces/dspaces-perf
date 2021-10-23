#ifndef _DS_PERF_UTIL_COMMON_H_
#define _DS_PERF_UTIL_COMMON_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <apex.h>

#define APEX_FUNC_TIMER_START(fn)                                              \
    apex_profiler_handle profiler0 = apex_start(APEX_FUNCTION_ADDRESS, &fn);
#define APEX_NAME_TIMER_START(num, name)                                       \
    apex_profiler_handle profiler##num = apex_start(APEX_NAME_STRING, name);
#define APEX_TIMER_STOP(num) apex_stop(profiler##num);

/* parse common-delimited dimensions. Modifies str */
uint64_t *parse_cdd(char *str, int *ndim)
{
    uint64_t *dims, *dim_p;
    int ncomma = 0;
    int i;

    for(i = 0; i < strlen(str); i++) {
        if(str[i] == ',') {
            ncomma++;
        }
    }

    dims = (uint64_t *)malloc(sizeof(*dims) * (ncomma + 1));
    dim_p = dims;

    for(i = 0; i <= ncomma; i++) {
        dims[i] = atol(strtok(i ? NULL : str, ","));
    }

    *ndim = ncomma + 1;

    return dims;
}

#endif /* _DS_PERF_UTIL_COMMON_H_ */
