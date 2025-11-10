#ifndef REQUEST_H__
#define REQUEST_H__

typedef enum {
    PRIVILEGED = 1,
    REGULAR
} req_type;

typedef struct {
    double arrival_time;
    double service_start_time;
    req_type type;
} request;

#endif