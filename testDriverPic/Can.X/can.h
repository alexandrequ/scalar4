/**
 * @file can.h
 * @brief Bibliothèque contenant toutes les fonctions pour intéragir avec le bus
 * CAN.
 * 
 * @warning
 * Afin de permettre à la bibliothèque de fonctionner, il faut définir dans les
 * options du projet une constante préprocesseur CPU_FREQ contenant la fréquence
 * d'horloge du PIC, en Hz.
 * 
 * Utilisation de base :
 * @code
 *     // Code d'initialisation
 *     // Attribution des pin au module CAN
 *     __builtin_write_OSCCONL(OSCCON & 0xbf); // Déverrouillage du PPS
 *     RPINR26bits.C1RXR = 39; // Réception CAN sur RP39
 *     RPOR3bits.RP40R = 0x000E; // Transmission CAN sur RP40
 *     __builtin_write_OSCCONL(OSCCON | 0x40); // Verrouillage du PPS
 *    
 *     // Configuration de la réception
 *     // On veut recevoir les trames de SID 0x123 dans le buffer 8
 *     // et les trames de SID 0x450 à 0x45F dans le buffer 9
 *     set_can_mask(CAN_MASK0, 0x7FF);
 *     set_can_mask(CAN_MASK1, 0x7F0);
 *     enable_can_rx_buffer(CAN_RX8, 0x123, CAN_MASK0);
 *     enable_can_rx_buffer(CAN_RX9, 0x450, CAN_MASK1);
 *     
 *     // Configuration initiale du module CAN à 125 kbit/s, avec triple-sampling,
 *     // sjw = 1tq, prop = 6tq et seg1 = 7 tq
 *     config_can(BR_125_KBPS, true, 1, 6, 7);
 *     
 *     // ...
 *     
 *     // Dans la boucle principale
 *     // Transmission d'un octet unique
 *     uint8_t myByte = 0x12;
 *     transmit_can((Canframe){0x100, &myByte, 1}, true);
 *     
 *     // Transmission d'une valeur sur plusieurs octets
 *     uint16_t myWord = 0xabcd;
 *     // Lit chaque valeur de droite à gauche, donc ici : 0xcd, puis 0xab
 *     transmit_can((Canframe){0x200, (uint8_t*)(&myWord), 2}, true);
 *     
 *     // Transmission de plusieurs valeurs
 *     uint16_t data[2] = { 0x1234, 0x5678 };
 *     // Lit chaque valeur de droite à gauche : 0x34, puis 0x12, puis 0x78
 *     transmit_can((Canframe){0x400, (uint8_t*)(&data), 3}, true);
 *     
 *     // Réception
 *     Canframe fr1 = receive_can(CAN_RX8, true); // SID=0x123
 *     Canframe fr2 = receive_can(CAN_RX9, true); // SID=0x450 à 0x45F
 * @endcode
 * 
 * @author Gregor Vindry
 */

#ifndef CAN_H
#define	CAN_H

#include <stdint.h> // définit uint8_t, uint16_t et uint32_t
#include <stdbool.h> // définit bool

#ifndef CPU_FREQ
#error "Please define CPU_FREQ with the device frequency in Hz as value"
#endif

#if CPU_FREQ != 8000000 && CPU_FREQ != 16000000 && CPU_FREQ != 128000000
#warning "The CAN library has never been tested at this frequency"
#endif

#ifdef DOUBLE_FCAN
/** @brief Constante représentant la fréquence du module CAN, en Hz. */
#define FCAN (CPU_FREQ)
#pragma message("Doubling FCAN value")
#else
/** @brief Constante représentant la fréquence du module CAN, en Hz. */
#define FCAN (CPU_FREQ/2)
#endif

/**
 * @brief Constante représentant le nombre (entier) de time quanta composant un bit.
 * @warning NTQ doit être un entier compris entre 8 inclus et 25 inclus.
 */
#define NTQ 16

#if NTQ < 8 || NTQ > 25
#error "NTQ must be an integer comprised between 8 and 25 included"
#endif

#if (FCAN/(64*2*NTQ)) > 125000
// Dans ce cas, la fréquence CPU est tellement rapide que le bit rate minimum
// est supérieur à 125 kb/s
#error "FCAN is too high"
#endif

/**
 * @brief Un alias vers l'énumération de choix du débit.
 */
typedef enum Bitrate Bitrate;

/**
 * @brief Un alias vers l'énumération de sélection du masque de réception CAN.
 */
typedef enum Canmask Canmask;

/**
 * @brief Un alias vers l'énumération de sélection d'un buffer de transmission CAN.
 */
typedef enum Cantxbuf Cantxbuf;

/**
 * @brief Un alias vers l'énumération de sélection d'un buffer de réception CAN.
 */
typedef enum Canrxbuf Canrxbuf;

/**
 * @brief Un alias vers la structure représentant une trame CAN.
 */
typedef struct Canframe Canframe;

/**
 * @brief Enumération permettant de choisir le débit (bit rate) sur le bus CAN.
 * 
 * Contient des constantes représentant les 4 débits standards.
 * 
 * @warning Suivant la valeur de CPU_FREQ, certains débits peuvent ne pas être
 * réalisables, et sont donc retirés de l'énumération. Un débit est réalisable
 * si et seulement si il divise FCAN/(2*NTQ).
 * @remark Il est possible de rendre accessible un débit pour lequel la
 * fréquence CPU est 2 fois trop lente, en définissant la constante
 * préprocesseur DOUBLE_FCAN.
 * @remark Les valeurs prises par les constantes n'ont pas d'importance dans
 * l'utilisation de ces constantes. Ces valeurs sont utilisées en interne par la
 * fonction config_can() (il s'agit du BRP, Baud Rate Prescaler).
 * @see config_can()
 */
enum Bitrate {
#if (FCAN/(2*NTQ)) % 1000000 == 0
    /** Représente un débit de 1 Mbit/s. */
    BR_1_MBPS = (((uint32_t)FCAN)/(2*NTQ*1000000)) - 1, 
#endif
#if (FCAN/(2*NTQ)) % 500000 == 0
    /** Représente un débit de 500 kbit/s. */
    BR_500_KBPS = (((uint32_t)FCAN)/(2*NTQ*500000)) - 1,
#endif
#if (FCAN/(2*NTQ)) % 250000 == 0
    /** Représente un débit de 250 kbit/s. */
    BR_250_KBPS = (((uint32_t)FCAN)/(2*NTQ*250000)) - 1, 
#endif
#if (FCAN/(2*NTQ)) % 125000 == 0
    /** Représente un débit de 125 kbit/s. */
    BR_125_KBPS = (((uint32_t)FCAN)/(2*NTQ*125000)) - 1
#else
#error "No standard CAN bit rate achievable. Please choose another CPU frequency"
#endif
};

/**
 * @brief Enumération permettant de sélectionner un masque de réception sur le
 * bus CAN.
 * 
 * Contient des constantes représentant les 3 masques disponibles.
 * @see set_can_mask()
 * @see enable_can_rx_buffer()
 */
enum Canmask {
    /** Représente le masque numéro 0. */
    CAN_MASK0 = 0,
    /** Représente le masque numéro 1. */
    CAN_MASK1 = 1,
    /** Représente le masque numéro 2. */
    CAN_MASK2 = 2
};

/**
 * @brief Enumération permettant de sélectionner un buffer de transmission CAN.
 * 
 * Contient des constantes représentant les buffers de transmission disponibles.
 * Seul les buffers 0 à 7 peuvent servir à la transmission.
 * There is a natural order of priority for message buffers that are assigned to
 * the same user-application defined priority level. Message buffer 7 has the
 * highest natural order of priority, and message buffer 0 the lowest
 */
enum Cantxbuf {
    /** Représente le buffer de transmission numéro 0. */
    CAN_TX0 = 0,
    /** Représente le buffer de transmission numéro 1. */
    CAN_TX1 = 1,
    /** Représente le buffer de transmission numéro 2. */
    CAN_TX2 = 2,
    /** Représente le buffer de transmission numéro 3. */
    CAN_TX3 = 3,
    /** Représente le buffer de transmission numéro 4. */
    CAN_TX4 = 4,
    /** Représente le buffer de transmission numéro 5. */
    CAN_TX5 = 5,
    /** Représente le buffer de transmission numéro 6. */
    CAN_TX6 = 6,
    /** Représente le buffer de transmission numéro 7. */
    CAN_TX7 = 7
};

/**
 * @brief Enumération permettant de sélectionner un buffer de réception CAN.
 * 
 * Contient des constantes représentant les buffers de réception disponibles.
 * Afin d'éviter des conflits avec les buffers de transmission, seuls certains
 * buffers sont listés ici.
 * @see enable_can_rx_buffer()
 * @see receive_can()
 */
enum Canrxbuf {
    /** Représente le buffer de réception numéro 8. */
    CAN_RX8 = 8,
    /** Représente le buffer de réception numéro 9. */
    CAN_RX9 = 9,
    /** Représente le buffer de réception numéro 10. */
    CAN_RX10 = 10,
    /** Représente le buffer de réception numéro 11. */
    CAN_RX11 = 11,
    /** Représente le buffer de réception numéro 12. */
    CAN_RX12 = 12,
    /** Représente le buffer de réception numéro 13. */
    CAN_RX13 = 13,
    /** Représente le buffer de réception numéro 14. */
    CAN_RX14 = 14
};


/**
 * @brief Structure représentant une trame de donnée CAN standard.
 */
struct Canframe {
    /** Identifiant de trame standard (SID), sur 11 bits. */
    uint16_t id;
    /** Pointeur vers les données contenues dans la trame. */
    const uint8_t* databytes;
    /** Nombre d'octets de données contenues dans la trame (entier de 0 à 8). */
    unsigned short bytes_count;
};

/**
 * @brief Assigne une valeur à l'un des masques de réception CAN.
 * 
 * Règle la valeur du masque de réception CAN sélectionné.
 * 
 * @param mask_sel Le masque à régler.
 * @param mask_value La valeur à placer dans le masque.
 * 
 * @pre config_can() n'a jamais été appelée.
 * @pre @p mask_sel est une constante d'énumération valide de type #Canmask.
 * @pre @p mask_value est un entier compris entre 0x000 et 0x7FF inclus.
 * @post Le masque sélectionné contient maintenant la valeur @p mask_value.
 * 
 * @see enable_can_rx_buffer() pour une explication du fonctionnement des filtres
 * et masques.
 */
void set_can_mask(Canmask mask_sel, uint16_t mask_value);

/**
 * @brief Active la réception CAN sur un buffer donné.
 * 
 * Active la réception CAN pour stockage dans le buffer spécifié, et règle le
 * filtre de réception associé.
 * 
 * Afin de savoir si une trame CAN reçue nous est effectivement destinée, il
 * faut observer son identifiant (SID). Cette opération peut certes être
 * effectuée au moyen d'un simple if-else, mais il est également possible (et
 * préférable) de régler un filtre, qui va rejeter automatiquement les trames
 * inintéressantes.
 * Concrètement, il faut placer la valeur d'identifiant désirée dans le filtre,
 * et sélectionner dans le masque quels bits interviendront dans la comparaison.
 * 
 * Par exemple, si l'on veut recevoir toutes les trames ayant un identifiant de
 * la forme 0x1 1101 1x01 (les x représentant un bit indifférent), on placera
 * dans le filtre la valeur binaire 001 1101 1001, et dans le masque la valeur
 * 101 1111 1011 (un 0 pour chaque bit indifférent).
 * 
 * @param buffer Le buffer de réception utilisé.
 * @param sid_filter La valeur à placer dans le filtre.
 * @param mask_sel Le masque à utiliser.
 * 
 * @pre config_can() n'a jamais été appelée.
 * @pre @p buffer est une constante d'énumération valide de type #Canrxbuf.
 * @pre @p sid_filter est un entier compris entre 0x000 et 0x7F0 inclus.
 * @pre @p mask_sel est une constante d'énumération valide de type #Canmask.
 * @pre La valeur du masque sélectionné a été réglée par set_can_mask().
 * @post La réception CAN est activée.
 * @post Les trames d'identifiant accepté par le filtre seront placées à leur
 * réception dans le buffer spécifié.
 * @see set_can_mask()
 */
void enable_can_rx_buffer(Canrxbuf buffer, uint16_t sid_filter,
        Canmask mask_sel);

/**
 * @brief Effectue la configuration initiale du bus CAN.
 * 
 * Configure le bus CAN selon le débit et la répartition des time quanta
 * spécifiés.
 * 
 * Le time quantum (TQ) est l'unité temporelle de base à partir de laquelle un
 * bit est construit sur le bus : un bit a une durée correspondant à un nombre
 * entier de time quanta, compris entre 8 et 25. Le nombre de time quanta par
 * bit est défini par #NTQ, et on a alors la durée d'un bit (Nominal Bit Time
 * NBT) : NBT = NTQ*TQ.
 * 
 * Un bit lui-même se décompose en 4 segments (chacun durant un nombre entier
 * de TQ), comme indiqué sur la figure ci-dessous (exemple avec NBT=10*TQ) :
 * 
 * @image html 530px-CAN_Bit_Timing2.png
 * @image latex 530px-CAN_Bit_Timing2.png
 * 
 * - Le segment de synchronisation a une durée de 1*TQ et doit contenir la
 * transition de 0 à 1 ou de 1 à 0. Si ce n'est pas le cas, cette erreur de
 * désynchronisation est détectée et la longueur d'un segment de phase réajustée
 * pour compenser. Le nombre maximal de TQ dont on peut rallonger le segment de
 * phase 1 ou raccourcir le segment de phase 2 est fixé par le Synchronization
 * Jump Width (sjw).
 * - Le segment de propagation est utilisé pour compenser les retards liés aux
 * phénomènes de propagation sur le bus.
 * - Le segment de phase 1 peut au besoin être rallongé en cas de détection
 * d'une erreur de synchronisation.
 * - Le segment de phase 2 peut au besoin être raccourci en cas de détection
 * d'une erreur de synchronisation. Dans l'implémentation actuelle, sa longueur
 * est déduite de #NTQ et des autres paramètres.
 * 
 * La valeur effective du bit est lue 1 fois à la limite entre les segments de
 * phase 1 et 2. Il est également possible de lire la valeur 3 fois, en la
 * lisant à la limite ainsi qu'aux 2 time quanta précédents, la valeur effective
 * étant celle majoritaire sur ces 3 mesures. Cette fonctionnalité est appelée
 * "triple sampling".
 * 
 * @warning Seules certaines combinaisons de paramètres ont un sens, il faut
 * donc veiller à respecter *strictement* les préconditions ci-dessous. Il faut
 * de plus utiliser le même jeu de paramètres pour tous les noeuds communiquant
 * entre eux sur le bus.
 * 
 * @param br débit de transmission et réception sur le bus.
 * @param trisample booléen spécifiant l'activation du "triple sampling".
 * @param sjw nombre maximal de time quanta autorisé pour le réajustement des
 * segments de phase lors d'une resynchronisation.
 * @param propseg nombre de time quanta du segment de propagation.
 * @param seg1 nombre de time quanta du segment de phase 1.
 * 
 * @pre La fonction n'a jamais été appelée.
 * @pre Le module CAN a été associé à des pins par configuration du PPS.
 * @pre Les channels DMA 0 et 1 sont inutilisées.
 * @pre @p sjw est un entier compris entre 1 inclus et 4 inclus.
 * @pre @p propseg est un entier compris entre 1 inclus et 8 inclus.
 * @pre @p seg1 est un entier compris entre 1 inclus et 8 inclus.
 * @pre Si trisample vaut @c true, seg1 est compris entre 3 inclus et 8 inclus.
 * @pre @p propseg + @p seg1 < @c NTQ - 1.
 * @pre 2*(@p propseg + @p seg1) >= @c NTQ - 1.
 * @pre @c NTQ - (@p propseg + @p seg1) > @p sjw + 1.
 * @post Le module CAN est prêt à transmettre.
 * @post Si enable_can_rx_buffer() a été appelée au moins une fois, alors le
 * module CAN est prêt à recevoir.
 */
void config_can(Bitrate br, bool trisample, unsigned short sjw, 
        unsigned short propseg, unsigned short seg1);

/**
 * @brief Transmet une trame sur le bus CAN.
 * 
 * Transmet une trame standard (identifiant 11 bits) portant l'identifiant et le
 * contenu spécifiés. Un exemple d'utilisation se trouve dans la description de
 * la bibliothèque.
 * 
 * @warning Si une transmission est en cours sur le buffer au moment où cette fonction est
 * appelée, elle sera interrompue et définitivement perdue.
 * 
 * @param id Identifiant de trame standard.
 * @param databytes Pointeur vers les données contenues dans la trame.
 * @param bytes_count Nombre d'octets de données contenues dans la trame (entier de 0 à 8).
 * 
 * @pre @p id est un entier compris entre 0x000 et 0x7F0 inclus.
 * @pre @p databytes est un pointeur non nul valide vers un octet ou tableau
 * d'octets consécutifs en mémoire à transmettre.
 * @pre @p bytes_count est inférieur ou égal au nombre d'octets contenus dans
 * @p databytes.
 * @pre Le nombre d'octets à transmettre est un entier compris entre 0 inclus
 * et 8 inclus.
 * @pre The numero of @p buffer is comprised between 0 included and 7 included
 * @pre config_can() a été appelée une et une seule fois.
 * @post La trame a été émise sur le bus.
 * @see can_transmission_complete()
 * 
 * @todo Tester ce qui se passe si on l'appelle plusieurs fois alors que la
 * transmission échoue.
 * @todo Gestion des erreurs.
 */
void transmit_can(uint16_t id, const uint8_t* databytes, unsigned short bytes_count, Cantxbuf buffer);

/**
 * @brief Use the transmit_can function to transmit the canframe
 * 
 * @param frame Trame de données à transmettre.
 * @param buffer The buffer that will be used to transmit the data.
 */
void transmit_canframe(Canframe frame, Cantxbuf buffer);

/**
 * @brief Vérifie la bonne transmission d'une trame CAN.
 * 
 * Vérifie que la dernière trame transmise a été correctement placée sur le bus,
 * c'est-à-dire qu'un bit d'acquittement (ACK) a été reçu.
 * 
 * @pre transmit_can() a été appelée.
 * @pre Le dernier appel à transmit_can() a été effectué avec @p wait_completion
 * valant @c false.
 * @return @c true si la trame a été bien transmise, @c false sinon
 */
bool is_buffer_available();

/**
 * @brief Lit une trame standard reçue sur le bus CAN.
 * 
 * Consulte le buffer spécifié et retourne la trame contenue à l'intérieur.
 * Si demandé, la fonction attend préalablement que le buffer soit marqué comme
 * rempli (ie qu'une nouvelle trame soit arrivée); sinon, elle retourne
 * immédiatement. Dans ce dernier cas, si aucune nouvelle trame n'a été reçue,
 * la dernière trame à avoir été reçue sera retournée. Si aucune trame n'a
 * jamais été reçue, une trame vide (longueur 0) sera retournée.
 * 
 * @param buffer Le buffer de réception à lire.
 * @param wait_reception Booléen indiquant d'attendre ou non la réception d'une
 * nouvelle trame sur ce buffer avant de lire.
 * 
 * @pre config_can() a été appelée une et une seule fois.
 * @pre @p buffer est une constante d'énumération valide de type #Canrxbuf.
 * @pre Le buffer spécifié a été configuré en réception par enable_can_rx_buffer().
 * @post Le buffer spécifié est marqué comme vidé (pas de nouvelle trame).
 * 
 * @return la trame contenue dans le buffer spécifié
 * @see new_canframe_received()
 * 
 * @todo Gestion des erreurs.
 */
Canframe receive_can(Canrxbuf buffer, bool wait_reception);

/**
 * @brief Vérifie si une nouvelle trame CAN a été reçue.
 * 
 * Vérifie si le buffer spécifié est marqué comme rempli, ie a reçu une nouvelle
 * trame CAN.
 * 
 * @param buffer Le buffer de réception à lire.
 * 
 * @pre config_can() a été appelée une et une seule fois.
 * @pre @p buffer est une constante d'énumération valide de type #Canrxbuf.
 * @pre Le buffer spécifié a été configuré en réception par enable_can_rx_buffer().
 * 
 * @return @c true si le buffer spécifié contient une nouvelle trame, @c false sinon
 * @see receive_can()
 */
bool new_canframe_received(Canrxbuf buffer);

#endif

