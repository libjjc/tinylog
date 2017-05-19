#ifndef TINYNOG_TICK_HH
#define TINYNOG_TICK_HH
#include <time.h>

struct _log_ts {
    time_t sec;
    long usec;
};

void
timestamp(struct _log_ts* ts);

void
tsprocess(struct _log_ts* ts);

void
tsepoch(struct _log_ts* ts);

long
delta_us(const struct _log_ts* far, const struct _log_ts* near);

long
delta_ms(const struct _log_ts* far, const struct _log_ts* near);

long
delta_s(const struct _log_ts* far, const struct _log_ts* near);

long
detla_us_epoch(const struct _log_ts* ts);

long
delta_ms_epoch(const struct _log_ts* ts);

long
delta_s_epoch(const struct _log_ts* ts);

int
delta_tm(const struct _log_ts* far, const struct _log_ts* near, struct tm* t);

long
tick();
#endif//TINYNOG_TICK_HH