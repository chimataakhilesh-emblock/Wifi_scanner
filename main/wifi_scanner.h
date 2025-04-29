#ifndef WIFI_SCANNER_H
#define WIFI_SCANNER_H

#define MAX_SSID_LEN 32

typedef struct {
    char ssid[MAX_SSID_LEN];
    int rssi;
    int is_open;
} wifi_ap_info_t;

void wifi_init_scan(void);
int wifi_scan_networks(wifi_ap_info_t *ap_list, int max_aps);

#endif // WIFI_SCANNER_H