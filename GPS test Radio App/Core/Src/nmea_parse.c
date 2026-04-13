/*
 * nmea_parse.c
 *
 * NMEA 0183 sentence parser for NEO-6M GPS module.
 * Ported from GPS_TEST reference project (originally for STM32F7).
 * Supports GPGGA, GPGSA, GPGLL sentences.
 *
 * Supports u-blox "GN" prefix variants (GNGGA, GNGLL, GNGSA) by passing
 * the correct sentence pointer from the caller.
 */

#include "nmea_parse.h"

static char *data[15];

/* --------------------------------------------------------------------------
 * Validate NMEA checksum.
 * Expects a sentence that ends with  *XX\r\n  (5 trailing chars).
 * Returns 1 if valid, 0 otherwise.
 * -------------------------------------------------------------------------- */
int gps_checksum(char *nmea_data)
{
    if (strlen(nmea_data) < 5) return 0;

    char recv_crc[3] = {0};
    recv_crc[0] = nmea_data[strlen(nmea_data) - 4];
    recv_crc[1] = nmea_data[strlen(nmea_data) - 3];

    int crc = 0;
    for (int i = 0; i < (int)strlen(nmea_data) - 5; i++)
    {
        crc ^= nmea_data[i];
    }

    int receivedHash = (int)strtol(recv_crc, NULL, 16);
    return (crc == receivedHash) ? 1 : 0;
}

/* --------------------------------------------------------------------------
 * Parse a GPGGA sentence.
 * inputString: sentence text WITHOUT the leading '$', e.g.
 *   "GPGGA,123519.00,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47"
 * Returns 1 on success, 0 on bad / incomplete data.
 * -------------------------------------------------------------------------- */
int nmea_GPGGA(GPS *gps_data, char *inputString)
{
    char *values[25];
    int   counter = 0;
    memset(values, 0, sizeof(values));

    char *marker = strtok(inputString, ",");
    while (marker != NULL && counter < 25)
    {
        values[counter] = malloc(strlen(marker) + 1);
        if (values[counter] == NULL) { break; }
        strcpy(values[counter], marker);
        counter++;
        marker = strtok(NULL, ",");
    }

    if (counter < 10) { goto cleanup_fail; }

    char latSide = values[3][0];
    char lonSide = values[5][0];
    strcpy(gps_data->lastMeasure, values[1]);

    if (latSide == 'S' || latSide == 'N')
    {
        char lat_d[3] = {0};
        char lat_m[8] = {0};
        for (int z = 0; z < 2; z++) lat_d[z] = values[2][z];
        for (int z = 0; z < 7; z++) lat_m[z] = values[2][z + 2];

        int   lat_deg_i = (int)strtol(lat_d, NULL, 10);
        float lat_min_f = strtof(lat_m, NULL);
        double lat_deg  = lat_deg_i + lat_min_f / 60.0;

        char lon_d[4] = {0};
        char lon_m[8] = {0};
        for (int z = 0; z < 3; z++) lon_d[z] = values[4][z];
        for (int z = 0; z < 7; z++) lon_m[z] = values[4][z + 3];

        int   lon_deg_i = (int)strtol(lon_d, NULL, 10);
        float lon_min_f = strtof(lon_m, NULL);
        double lon_deg  = lon_deg_i + lon_min_f / 60.0;

        if (lat_deg != 0 && lon_deg != 0 && lat_deg < 90 && lon_deg < 180)
        {
            gps_data->latitude      = lat_deg;
            gps_data->latSide       = latSide;
            gps_data->longitude     = lon_deg;
            gps_data->lonSide       = lonSide;

            float alt = strtof(values[9], NULL);
            gps_data->altitude = (alt != 0) ? alt : gps_data->altitude;

            gps_data->satelliteCount = (int)strtol(values[7], NULL, 10);

            int fixQuality = (int)strtol(values[6], NULL, 10);
            gps_data->fix = (fixQuality > 0) ? 1 : 0;

            float hdop = strtof(values[8], NULL);
            gps_data->hdop = (hdop != 0) ? hdop : gps_data->hdop;
        }
        else
        {
            goto cleanup_fail;
        }
    }

    for (int i = 0; i < counter; i++) free(values[i]);
    return 1;

cleanup_fail:
    for (int i = 0; i < counter; i++) { if (values[i]) free(values[i]); }
    return 0;
}

/* --------------------------------------------------------------------------
 * Parse a GPGSA sentence (active satellites + DOP).
 * -------------------------------------------------------------------------- */
int nmea_GPGSA(GPS *gps_data, char *inputString)
{
    char *values[25];
    int   counter = 0;
    memset(values, 0, sizeof(values));

    char *marker = strtok(inputString, ",");
    while (marker != NULL && counter < 25)
    {
        values[counter] = malloc(strlen(marker) + 1);
        if (values[counter] == NULL) { break; }
        strcpy(values[counter], marker);
        counter++;
        marker = strtok(NULL, ",");
    }

    if (counter < 3) { goto gsa_fail; }

    int fix = (int)strtol(values[2], NULL, 10);
    gps_data->fix = (fix > 1) ? 1 : 0;

    int satelliteCount = 0;
    for (int i = 3; i < 15 && i < counter; i++)
    {
        if (values[i][0] != '\0') { satelliteCount++; }
    }
    gps_data->satelliteCount = satelliteCount;

    for (int i = 0; i < counter; i++) free(values[i]);
    return 1;

gsa_fail:
    for (int i = 0; i < counter; i++) { if (values[i]) free(values[i]); }
    return 0;
}

/* --------------------------------------------------------------------------
 * Parse a GPGLL sentence (position only).
 * -------------------------------------------------------------------------- */
int nmea_GPGLL(GPS *gps_data, char *inputString)
{
    char *values[25];
    int   counter = 0;
    memset(values, 0, sizeof(values));

    char *marker = strtok(inputString, ",");
    while (marker != NULL && counter < 25)
    {
        values[counter] = malloc(strlen(marker) + 1);
        if (values[counter] == NULL) { break; }
        strcpy(values[counter], marker);
        counter++;
        marker = strtok(NULL, ",");
    }

    if (counter < 5) { goto gll_fail; }

    char latSide = values[2][0];
    if (latSide == 'S' || latSide == 'N')
    {
        char lat_d[3] = {0};
        char lat_m[8] = {0};
        for (int z = 0; z < 2; z++) lat_d[z] = values[1][z];
        for (int z = 0; z < 7; z++) lat_m[z] = values[1][z + 2];

        int   lat_deg_i = (int)strtol(lat_d, NULL, 10);
        float lat_min_f = strtof(lat_m, NULL);
        double lat_deg  = lat_deg_i + lat_min_f / 60.0;

        char lon_d[4] = {0};
        char lon_m[8] = {0};
        char lonSide   = values[4][0];
        for (int z = 0; z < 3; z++) lon_d[z] = values[3][z];
        for (int z = 0; z < 7; z++) lon_m[z] = values[3][z + 3];

        int   lon_deg_i = (int)strtol(lon_d, NULL, 10);
        float lon_min_f = strtof(lon_m, NULL);
        double lon_deg  = lon_deg_i + lon_min_f / 60.0;

        /* reject null-island */
        if (lon_deg_i == 0 || lon_min_f == 0.0f || lat_deg_i == 0 || lat_min_f == 0.0f)
        {
            goto gll_fail;
        }

        gps_data->latitude  = lat_deg;
        gps_data->longitude = lon_deg;
        gps_data->latSide   = latSide;
        gps_data->lonSide   = lonSide;

        for (int i = 0; i < counter; i++) free(values[i]);
        return 1;
    }

gll_fail:
    for (int i = 0; i < counter; i++) { if (values[i]) free(values[i]); }
    return 0;
}

/* --------------------------------------------------------------------------
 * Full multi-sentence parser.
 * Splits buffer on '$', validates checksum, routes each sentence.
 * NOTE: requires sentences to end with \r\n for checksum validation.
 * For single-sentence use, call nmea_GPGGA / nmea_GPGLL / nmea_GPGSA directly.
 * -------------------------------------------------------------------------- */
void nmea_parse(GPS *gps_data, uint8_t *buffer)
{
    memset(data, 0, sizeof(data));
    int cnt = 0;

    char *token = strtok((char *)buffer, "$");
    while (token != NULL && cnt < 15)
    {
        data[cnt] = malloc(strlen(token) + 1);
        if (data[cnt] == NULL) { break; }
        strcpy(data[cnt], token);
        cnt++;
        token = strtok(NULL, "$");
    }

    for (int i = 0; i < cnt; i++)
    {
        if (strstr(data[i], "\r\n") != NULL && gps_checksum(data[i]))
        {
            if      (strstr(data[i], "GPGLL") != NULL || strstr(data[i], "GNGLL") != NULL)
                nmea_GPGLL(gps_data, data[i]);
            else if (strstr(data[i], "GPGSA") != NULL || strstr(data[i], "GNGSA") != NULL)
                nmea_GPGSA(gps_data, data[i]);
            else if (strstr(data[i], "GPGGA") != NULL || strstr(data[i], "GNGGA") != NULL)
                nmea_GPGGA(gps_data, data[i]);
        }
    }

    for (int i = 0; i < cnt; i++) free(data[i]);
}
