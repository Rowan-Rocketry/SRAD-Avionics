#ifndef NMEA_PARSE_H
#define NMEA_PARSE_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/*
 * GPS data structure for storing parsed NMEA data.
 * Ported from GPS_TEST reference project (STM32F7) to STM32WL.
 */
typedef struct NMEA_DATA {
    double latitude;        /* decimal degrees */
    char   latSide;         /* 'N' or 'S' */
    double longitude;       /* decimal degrees */
    char   lonSide;         /* 'E' or 'W' */
    float  altitude;        /* metres */
    float  hdop;            /* horizontal dilution of precision */
    int    satelliteCount;
    int    fix;             /* 1 = fix acquired, 0 = no fix */
    char   lastMeasure[10]; /* hhmmss.ss UTC */
} GPS;

/* Individual sentence parsers — input string must NOT include the leading '$' */
int nmea_GPGGA(GPS *gps_data, char *inputString);
int nmea_GPGSA(GPS *gps_data, char *inputString);
int nmea_GPGLL(GPS *gps_data, char *inputString);

/* Full multi-sentence parser (splits buffer on '$') */
void nmea_parse(GPS *gps_data, uint8_t *buffer);

#endif /* NMEA_PARSE_H */
