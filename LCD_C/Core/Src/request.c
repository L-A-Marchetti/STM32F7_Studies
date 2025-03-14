/*
 * request.c
 *
 *  Created on: Mar 12, 2025
 *      Author: lam
 */


#include "request.h"

void Custom_LWIP_Init(void)
{
    struct netif gnetif;
    ip4_addr_t ipaddr, netmask, gw;
    uint16_t font_size = BSP_LCD_GetFont()->Height;

    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_DisplayStringAt(0, 0, (uint8_t *)"Initializing network", LEFT_MODE);
    HAL_Delay(1000);

    BSP_LCD_DisplayStringAt(0, font_size, (uint8_t *)"Initializing TCP/IP stack", LEFT_MODE);
    tcpip_init(NULL, NULL);
    HAL_Delay(1000);

    BSP_LCD_DisplayStringAt(0, font_size * 2, (uint8_t *)"Adding network interface", LEFT_MODE);

    HAL_Delay(1000);

    BSP_LCD_DisplayStringAt(0, font_size * 3, (uint8_t *)"Setting default interface", LEFT_MODE);
    netif_set_default(&gnetif);
    HAL_Delay(1000);

    BSP_LCD_DisplayStringAt(0, font_size * 4, (uint8_t *)"Bringing up interface", LEFT_MODE);
    netif_set_up(&gnetif);
    HAL_Delay(1000);

    BSP_LCD_DisplayStringAt(0, font_size * 5, (uint8_t *)"Checking link status...", LEFT_MODE);
    HAL_Delay(500);
    if (netif_is_up(&gnetif)) {
        BSP_LCD_DisplayStringAt(0, font_size * 6, (uint8_t *)"Link UP", LEFT_MODE);
    } else {
        BSP_LCD_DisplayStringAt(0, font_size * 6, (uint8_t *)"Link DOWN", LEFT_MODE);
    }
    HAL_Delay(1000);


    BSP_LCD_DisplayStringAt(0, font_size * 7, (uint8_t *)"Starting DHCP", LEFT_MODE);
    dhcp_cleanup(&gnetif);
    dhcp_start(&gnetif);
    HAL_Delay(1000);


    char ip_str[16];
    ipaddr_ntoa_r(&gnetif.ip_addr, ip_str, sizeof(ip_str));
    BSP_LCD_DisplayStringAt(0, font_size * 10, (uint8_t *)"IP Address obtained:", LEFT_MODE);
    BSP_LCD_DisplayStringAt(0, font_size * 11, (uint8_t *)ip_str, LEFT_MODE);
    HAL_Delay(2000);

    BSP_LCD_DisplayStringAt(0, font_size * 12, (uint8_t *)"Pinging google.com", LEFT_MODE);
    /*ip4_addr_t ping_addr;
    ip4addr_aton("8.8.8.8", &ping_addr);  // Google's DNS server
    err_t ping_err = ping_send(&ping_addr, 3);
    if (ping_err == ERR_OK) {
        BSP_LCD_DisplayStringAt(0, font_size * 11, (uint8_t *)"Ping successful", LEFT_MODE);
    } else {
        BSP_LCD_DisplayStringAt(0, font_size * 11, (uint8_t *)"Ping failed", LEFT_MODE);
    }
    */
}


void send_lichess_request(void)
{
    int sock;
    struct sockaddr_in server_addr;
    ip4_addr_t ip_addr;
    char *hostname = "lichess.org";
    char *path = "/api/tv/feed";
    char buffer[1024];

    uint16_t font_size = BSP_LCD_GetFont()->Height;

    BSP_LCD_DisplayStringAt(0, font_size*4, (uint8_t *)"Resolving DNS", LEFT_MODE);

    err_t err = dns_gethostbyname(hostname, &ip_addr, NULL, NULL);
    if (err == ERR_INPROGRESS) {
    	BSP_LCD_DisplayStringAt(0, font_size*5, (uint8_t *)"Resolving DNS in progress", LEFT_MODE);
    	return;
    }
    if (err != ERR_OK) {
    	char err_msg[50];
    	    sprintf(err_msg, "DNS Failed: Error %d", (int)err);
    	    BSP_LCD_DisplayStringAt(0, font_size*6, (uint8_t *)err_msg, LEFT_MODE);
    	    return;
    }

    BSP_LCD_DisplayStringAt(0, font_size*6, (uint8_t *)"Creating socket...", LEFT_MODE);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        BSP_LCD_DisplayStringAt(0, font_size*7, (uint8_t *)"Socket Creation Failed", LEFT_MODE);
        return;
    }

    BSP_LCD_DisplayStringAt(0, font_size*8, (uint8_t *)"Connecting to server...", LEFT_MODE);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    server_addr.sin_addr.s_addr = ip_addr.addr;

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        BSP_LCD_DisplayStringAt(0, font_size*9, (uint8_t *)"Connection Failed", LEFT_MODE);
        close(sock);
        return;
    }

    BSP_LCD_DisplayStringAt(0, font_size*9, (uint8_t *)"Sending request...", LEFT_MODE);

    snprintf(buffer, sizeof(buffer),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n\r\n", path, hostname);
    send(sock, buffer, strlen(buffer), 0);

    BSP_LCD_DisplayStringAt(0, font_size*10, (uint8_t *)"Receiving response...", LEFT_MODE);

    while (recv(sock, buffer, sizeof(buffer)-1, 0) > 0) {
        buffer[sizeof(buffer) - 1] = '\0';  // Assurer la terminaison de la chaîne

        BSP_LCD_Clear(LCD_COLOR_BLACK);
        BSP_LCD_DisplayStringAt(0, 0, (uint8_t *)buffer, LEFT_MODE); // Affichage écran
    }

    BSP_LCD_DisplayStringAt(0, 200, (uint8_t *)"Request complete", LEFT_MODE);

    close(sock);
}
