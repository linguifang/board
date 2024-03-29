/**
  ******************************************************************************
  * @file    server.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   A sample UDP/TCP server application.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define UDP_SERVER_PORT    7   /* define the UDP local connection port */
#define UDP_CLIENT_PORT    4   /* define the UDP remote connection port */
#define TCP_PORT    4	/* define the TCP connection port */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void udp_server_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port);
err_t tcp_server_accept(void *arg, struct tcp_pcb *pcb, err_t err);
static err_t tcp_server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize the server application.
  * @param  None
  * @retval None
  */
void server_init(void)
{
   struct udp_pcb *upcb;                                 
   
   /* Create a new UDP control block  */
   upcb = udp_new();
   
   /* Bind the upcb to the UDP_PORT port */
   /* Using IP_ADDR_ANY allow the upcb to be used by any local interface */
   udp_bind(upcb, IP_ADDR_ANY, UDP_SERVER_PORT);
   
   /* Set a receive callback for the upcb */
   udp_recv(upcb, udp_server_callback, NULL);
  
}

/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
void udp_server_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{
  struct tcp_pcb *pcb;
  __IO uint8_t iptab[4];
  uint8_t iptxt[20];
  
  /* We have received the UDP Echo from a client */
  /* read its IP address */
  iptab[0] = (uint8_t)((uint32_t)(addr->addr) >> 24);
  iptab[1] = (uint8_t)((uint32_t)(addr->addr) >> 16);
  iptab[2] = (uint8_t)((uint32_t)(addr->addr) >> 8);
  iptab[3] = (uint8_t)((uint32_t)(addr->addr));

  sprintf((char*)iptxt, "is: %d.%d.%d.%d     ", iptab[3], iptab[2], iptab[1], iptab[0]);	                           	                             
	
  /* Display the client's IP address */
  /* If there is more than one client connected, the IP address of the last one connected will be displayed */
  GLCD_displayStringLn(Line4, "Client's IP address");
  GLCD_displayStringLn(Line5, iptxt);
 
  /* Connect to the remote client */
  udp_connect(upcb, addr, UDP_CLIENT_PORT);
    
  /* Tell the client that we have accepted it */
  udp_send(upcb, p);

  /* free the UDP connection, so we can accept new clients */
  udp_disconnect(upcb);
	
  /* Bind the upcb to IP_ADDR_ANY address and the UDP_PORT port*/
  /* Be ready to get a new request from another client */  
  udp_bind(upcb, IP_ADDR_ANY, UDP_SERVER_PORT);
	
  /* Set a receive callback for the upcb */
  udp_recv(upcb, udp_server_callback, NULL);    	
	
  /* Create a new TCP control block  */
  pcb = tcp_new();
	
  if(pcb !=NULL)
  {
    err_t err;	  
	      
    /* Assign to the new pcb a local IP address and a port number */
    err = tcp_bind(pcb, addr, TCP_PORT);
	  
	if(err != ERR_USE)
	{
	  /* Set the connection to the LISTEN state */
      pcb = tcp_listen(pcb);
    
      /* Specify the function to be called when a connection is established */
      tcp_accept(pcb, tcp_server_accept);
	}
	else
	{
	  /* We enter here if a conection to the addr IP address already exists */
	  /* so we don't need to establish a new one */
	  tcp_close(pcb);
	}            
  }

  /* Free the p buffer */
  pbuf_free(p);
   
}

/**
  * @brief  This funtion is called when a TCP connection has been established on the port TCP_PORT.
  * @param  arg	user supplied argument 
  * @param  pcb	the tcp_pcb which accepted the connection
  * @param  err error value
  * @retval ERR_OK
  */
err_t tcp_server_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{ 
  /* Specify the function that should be called when the TCP connection receives data */
  tcp_recv(pcb, tcp_server_recv);

  return ERR_OK;  
}

/**
  * @brief  This function is called when a data is received over the TCP_PORT.
  *         The received data contains the number of the led to be toggled.
  * @param  arg	user supplied argument 
  * @param  pcb	the tcp_pcb which accepted the connection
  * @param  p the packet buffer that was received
  * @param  err error value
  * @retval ERR_OK
  */
static err_t tcp_server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
  char data;

  /* Read the led number */
  data = *(((char *)p->payload));  

  /* Toggle the specified led */
  switch (data)
  {
  	case LED1:
	    STM_EVAL_LEDToggle(LED1);
		break;
	
	case LED2:
	    STM_EVAL_LEDToggle(LED2);
		break;
	
	case LED3:
	    STM_EVAL_LEDToggle(LED3);
		break;
	
	case LED4:
	    STM_EVAL_LEDToggle(LED4);
		break;
	
	default:
		break;
  }

  /* Free the p buffer */
  pbuf_free(p);

  return ERR_OK;
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
