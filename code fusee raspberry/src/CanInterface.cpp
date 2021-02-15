#include "CanInterface.h"
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

/*
 * J'ai écrit le code de cette class en utilisant les exemples sur cette page:
 * https://www.kernel.org/doc/html/v4.18/networking/can.html
 * 
 * Je n'ai pas eu le temps de tester les cas d'erreurs (appels trop peu fréquents de
 * receive(), dans quels cas send() renvoit-elle 'false', ...), cependant cette classe
 * a très bien fonctionné pour ScalarIV.
 * */

CanInterface::CanInterface(){

}

void CanInterface::init(){
	struct sockaddr_can addr;
    struct ifreq ifr;

    cansocket = socket(PF_CAN, SOCK_RAW, CAN_RAW);

	// "can0" est le nom qui apparait quand l'on entre la commande "ifconfig" dans le shell de la Raspberry.
    strcpy(ifr.ifr_name, "can0" );
    ioctl(cansocket, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

	// Cette ligne permet de rendre la methode receive() non-bloquante.
	// Remplacer O_NONBLOCK par O_BLOCK rend receive() bloquante.
	fcntl(cansocket, F_SETFL, O_NONBLOCK);

    bind(cansocket, (struct sockaddr *)&addr, sizeof(addr));
}

bool CanInterface::receive(struct can_frame& frame){

    int nbytes = read(cansocket, &frame, sizeof(struct can_frame));
    
    // On vérifie que l'on a bien reçu une frame complète
    return (nbytes == sizeof(struct can_frame));
}

bool CanInterface::send(struct can_frame& frame){

    int nbytes = write(cansocket, &frame, sizeof(struct can_frame));
    
    return (nbytes == sizeof(struct can_frame));
}
