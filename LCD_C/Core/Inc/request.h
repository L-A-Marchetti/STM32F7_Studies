/*
 * request.h
 *
 *  Created on: Mar 12, 2025
 *      Author: lam
 */

#ifndef INC_REQUEST_H_
#define INC_REQUEST_H_

#include "lwip/api.h"
#include "lwip/sys.h"
#include "lwip/netif.h"
#include "lwip/dns.h"
#include "lwip/sockets.h"
#include "lwip/init.h"
#include "lwip.h"
#include "lwip/ip4_addr.h"

void Custom_LWIP_Init(void);
void send_lichess_request(void);


#endif /* INC_REQUEST_H_ */
