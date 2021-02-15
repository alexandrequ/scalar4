/**
 * @file can.h
 * @brief Biblioth�que contenant toutes les fonctions pour int�ragir avec le bus
 * CAN.
 * 
 * @warning
 * Afin de permettre � la biblioth�que de fonctionner, il faut d�finir dans les
 * options du projet une constante pr�processeur CPU_FREQ contenant la fr�quence
 * d'horloge du PIC, en Hz.
 * 
 * Utilisation de base :
 * @code
 *     // Code d'initialisation
 *     // Attribution des pin au module CAN
 *     __builtin_write_OSCCONL(OSCCON & 0xbf); // D�verrouillage du PPS
 *     RPINR26bits.C1RXR = 39; // R�ception CAN sur RP39
 *     RPOR3bits.RP40R = 0x000E; // Transmission CAN sur RP40
 *     __builtin_write_OSCCONL(OSCCON | 0x40); // Verrouillage du PPS
 *    
 *     // Configuration de la r�ception
 *     // On veut recevoir les trames de SID 0x123 dans le buffer 8
 *     // et les trames de SID 0x450 � 0x45F dans le buffer 9
 *     set_can_mask(CAN_MASK0, 0x7FF);
 *     set_can_mask(CAN_MASK1, 0x7F0);
 *     enable_can_rx_buffer(CAN_RX8, 0x123, CAN_MASK0);
 *     enable_can_rx_buffer(CAN_RX9, 0x450, CAN_MASK1);
 *     
 *     // Configuration initiale du module CAN � 125 kbit/s, avec triple-sampling,
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
 *     // Lit chaque valeur de droite � gauche, donc ici : 0xcd, puis 0xab
 *     transmit_can((Canframe){0x200, (uint8_t*)(&myWord), 2}, true);
 *     
 *     // Transmission de plusieurs valeurs
 *     uint16_t data[2] = { 0x1234, 0x5678 };
 *     // Lit chaque valeur de droite � gauche : 0x34, puis 0x12, puis 0x78
 *     transmit_can((Canframe){0x400, (uint8_t*)(&data), 3}, true);
 *     
 *     // R�ception
 *     Canframe fr1 = receive_can(CAN_RX8, true); // SID=0x123
 *     Canframe fr2 = receive_can(CAN_RX9, true); // SID=0x450 � 0x45F
 * @endcode
 * 
 * @author Gregor Vindry
 */

#ifndef CAN_H
#define	CAN_H

#include <stdint.h> // d�finit uint8_t, uint16_t et uint32_t
#include <stdbool.h> // d�finit bool

#ifndef CPU_FREQ
#error "Please define CPU_FREQ with the device frequency in Hz as value"
#endif

#if CPU_FREQ != 8000000 && CPU_FREQ != 16000000 && CPU_FREQ != 128000000
#warning "The CAN library has never been tested at this frequency"
#endif

#ifdef DOUBLE_FCAN
/** @brief Constante repr�sentant la fr�quence du module CAN, en Hz. */
#define FCAN (CPU_FREQ)
#pragma message("Doubling FCAN value")
#else
/** @brief Constante repr�sentant la fr�quence du module CAN, en Hz. */
#define FCAN (CPU_FREQ/2)
#endif

/**
 * @brief Constante repr�sentant le nombre (entier) de time quanta composant un bit.
 * @warning NTQ doit �tre un entier compris entre 8 inclus et 25 inclus.
 */
#define NTQ 16

#if NTQ < 8 || NTQ > 25
#error "NTQ must be an integer comprised between 8 and 25 included"
#endif

#if (FCAN/(64*2*NTQ)) > 125000
// Dans ce cas, la fr�quence CPU est tellement rapide que le bit rate minimum
// est sup�rieur � 125 kb/s
#error "FCAN is too high"
#endif

/**
 * @brief Un alias vers l'�num�ration de choix du d�bit.
 */
typedef enum Bitrate Bitrate;

/**
 * @brief Un alias vers l'�num�ration de s�lection du masque de r�ception CAN.
 */
typedef enum Canmask Canmask;

/**
 * @brief Un alias vers l'�num�ration de s�lection d'un buffer de transmission CAN.
 */
typedef enum Cantxbuf Cantxbuf;

/**
 * @brief Un alias vers l'�num�ration de s�lection d'un buffer de r�ception CAN.
 */
typedef enum Canrxbuf Canrxbuf;

/**
 * @brief Un alias vers la structure repr�sentant une trame CAN.
 */
typedef struct Canframe Canframe;

/**
 * @brief Enum�ration permettant de choisir le d�bit (bit rate) sur le bus CAN.
 * 
 * Contient des constantes repr�sentant les 4 d�bits standards.
 * 
 * @warning Suivant la valeur de CPU_FREQ, certains d�bits peuvent ne pas �tre
 * r�alisables, et sont donc retir�s de l'�num�ration. Un d�bit est r�alisable
 * si et seulement si il divise FCAN/(2*NTQ).
 * @remark Il est possible de rendre accessible un d�bit pour lequel la
 * fr�quence CPU est 2 fois trop lente, en d�finissant la constante
 * pr�processeur DOUBLE_FCAN.
 * @remark Les valeurs prises par les constantes n'ont pas d'importance dans
 * l'utilisation de ces constantes. Ces valeurs sont utilis�es en interne par la
 * fonction config_can() (il s'agit du BRP, Baud Rate Prescaler).
 * @see config_can()
 */
enum Bitrate {
#if (FCAN/(2*NTQ)) % 1000000 == 0
    /** Repr�sente un d�bit de 1 Mbit/s. */
    BR_1_MBPS = (((uint32_t)FCAN)/(2*NTQ*1000000)) - 1, 
#endif
#if (FCAN/(2*NTQ)) % 500000 == 0
    /** Repr�sente un d�bit de 500 kbit/s. */
    BR_500_KBPS = (((uint32_t)FCAN)/(2*NTQ*500000)) - 1,
#endif
#if (FCAN/(2*NTQ)) % 250000 == 0
    /** Repr�sente un d�bit de 250 kbit/s. */
    BR_250_KBPS = (((uint32_t)FCAN)/(2*NTQ*250000)) - 1, 
#endif
#if (FCAN/(2*NTQ)) % 125000 == 0
    /** Repr�sente un d�bit de 125 kbit/s. */
    BR_125_KBPS = (((uint32_t)FCAN)/(2*NTQ*125000)) - 1
#else
#error "No standard CAN bit rate achievable. Please choose another CPU frequency"
#endif
};

/**
 * @brief Enum�ration permettant de s�lectionner un masque de r�ception sur le
 * bus CAN.
 * 
 * Contient des constantes repr�sentant les 3 masques disponibles.
 * @see set_can_mask()
 * @see enable_can_rx_buffer()
 */
enum Canmask {
    /** Repr�sente le masque num�ro 0. */
    CAN_MASK0 = 0,
    /** Repr�sente le masque num�ro 1. */
    CAN_MASK1 = 1,
    /** Repr�sente le masque num�ro 2. */
    CAN_MASK2 = 2
};

/**
 * @brief Enum�ration permettant de s�lectionner un buffer de transmission CAN.
 * 
 * Contient des constantes repr�sentant les buffers de transmission disponibles.
 * Seul les buffers 0 � 7 peuvent servir � la transmission.
 * There is a natural order of priority for message buffers that are assigned to
 * the same user-application defined priority level. Message buffer 7 has the
 * highest natural order of priority, and message buffer 0 the lowest
 */
enum Cantxbuf {
    /** Repr�sente le buffer de transmission num�ro 0. */
    CAN_TX0 = 0,
    /** Repr�sente le buffer de transmission num�ro 1. */
    CAN_TX1 = 1,
    /** Repr�sente le buffer de transmission num�ro 2. */
    CAN_TX2 = 2,
    /** Repr�sente le buffer de transmission num�ro 3. */
    CAN_TX3 = 3,
    /** Repr�sente le buffer de transmission num�ro 4. */
    CAN_TX4 = 4,
    /** Repr�sente le buffer de transmission num�ro 5. */
    CAN_TX5 = 5,
    /** Repr�sente le buffer de transmission num�ro 6. */
    CAN_TX6 = 6,
    /** Repr�sente le buffer de transmission num�ro 7. */
    CAN_TX7 = 7
};

/**
 * @brief Enum�ration permettant de s�lectionner un buffer de r�ception CAN.
 * 
 * Contient des constantes repr�sentant les buffers de r�ception disponibles.
 * Afin d'�viter des conflits avec les buffers de transmission, seuls certains
 * buffers sont list�s ici.
 * @see enable_can_rx_buffer()
 * @see receive_can()
 */
enum Canrxbuf {
    /** Repr�sente le buffer de r�ception num�ro 8. */
    CAN_RX8 = 8,
    /** Repr�sente le buffer de r�ception num�ro 9. */
    CAN_RX9 = 9,
    /** Repr�sente le buffer de r�ception num�ro 10. */
    CAN_RX10 = 10,
    /** Repr�sente le buffer de r�ception num�ro 11. */
    CAN_RX11 = 11,
    /** Repr�sente le buffer de r�ception num�ro 12. */
    CAN_RX12 = 12,
    /** Repr�sente le buffer de r�ception num�ro 13. */
    CAN_RX13 = 13,
    /** Repr�sente le buffer de r�ception num�ro 14. */
    CAN_RX14 = 14
};


/**
 * @brief Structure repr�sentant une trame de donn�e CAN standard.
 */
struct Canframe {
    /** Identifiant de trame standard (SID), sur 11 bits. */
    uint16_t id;
    /** Pointeur vers les donn�es contenues dans la trame. */
    const uint8_t* databytes;
    /** Nombre d'octets de donn�es contenues dans la trame (entier de 0 � 8). */
    unsigned short bytes_count;
};

/**
 * @brief Assigne une valeur � l'un des masques de r�ception CAN.
 * 
 * R�gle la valeur du masque de r�ception CAN s�lectionn�.
 * 
 * @param mask_sel Le masque � r�gler.
 * @param mask_value La valeur � placer dans le masque.
 * 
 * @pre config_can() n'a jamais �t� appel�e.
 * @pre @p mask_sel est une constante d'�num�ration valide de type #Canmask.
 * @pre @p mask_value est un entier compris entre 0x000 et 0x7FF inclus.
 * @post Le masque s�lectionn� contient maintenant la valeur @p mask_value.
 * 
 * @see enable_can_rx_buffer() pour une explication du fonctionnement des filtres
 * et masques.
 */
void set_can_mask(Canmask mask_sel, uint16_t mask_value);

/**
 * @brief Active la r�ception CAN sur un buffer donn�.
 * 
 * Active la r�ception CAN pour stockage dans le buffer sp�cifi�, et r�gle le
 * filtre de r�ception associ�.
 * 
 * Afin de savoir si une trame CAN re�ue nous est effectivement destin�e, il
 * faut observer son identifiant (SID). Cette op�ration peut certes �tre
 * effectu�e au moyen d'un simple if-else, mais il est �galement possible (et
 * pr�f�rable) de r�gler un filtre, qui va rejeter automatiquement les trames
 * inint�ressantes.
 * Concr�tement, il faut placer la valeur d'identifiant d�sir�e dans le filtre,
 * et s�lectionner dans le masque quels bits interviendront dans la comparaison.
 * 
 * Par exemple, si l'on veut recevoir toutes les trames ayant un identifiant de
 * la forme 0x1 1101 1x01 (les x repr�sentant un bit indiff�rent), on placera
 * dans le filtre la valeur binaire 001 1101 1001, et dans le masque la valeur
 * 101 1111 1011 (un 0 pour chaque bit indiff�rent).
 * 
 * @param buffer Le buffer de r�ception utilis�.
 * @param sid_filter La valeur � placer dans le filtre.
 * @param mask_sel Le masque � utiliser.
 * 
 * @pre config_can() n'a jamais �t� appel�e.
 * @pre @p buffer est une constante d'�num�ration valide de type #Canrxbuf.
 * @pre @p sid_filter est un entier compris entre 0x000 et 0x7F0 inclus.
 * @pre @p mask_sel est une constante d'�num�ration valide de type #Canmask.
 * @pre La valeur du masque s�lectionn� a �t� r�gl�e par set_can_mask().
 * @post La r�ception CAN est activ�e.
 * @post Les trames d'identifiant accept� par le filtre seront plac�es � leur
 * r�ception dans le buffer sp�cifi�.
 * @see set_can_mask()
 */
void enable_can_rx_buffer(Canrxbuf buffer, uint16_t sid_filter,
        Canmask mask_sel);

/**
 * @brief Effectue la configuration initiale du bus CAN.
 * 
 * Configure le bus CAN selon le d�bit et la r�partition des time quanta
 * sp�cifi�s.
 * 
 * Le time quantum (TQ) est l'unit� temporelle de base � partir de laquelle un
 * bit est construit sur le bus : un bit a une dur�e correspondant � un nombre
 * entier de time quanta, compris entre 8 et 25. Le nombre de time quanta par
 * bit est d�fini par #NTQ, et on a alors la dur�e d'un bit (Nominal Bit Time
 * NBT) : NBT = NTQ*TQ.
 * 
 * Un bit lui-m�me se d�compose en 4 segments (chacun durant un nombre entier
 * de TQ), comme indiqu� sur la figure ci-dessous (exemple avec NBT=10*TQ) :
 * 
 * @image html 530px-CAN_Bit_Timing2.png
 * @image latex 530px-CAN_Bit_Timing2.png
 * 
 * - Le segment de synchronisation a une dur�e de 1*TQ et doit contenir la
 * transition de 0 � 1 ou de 1 � 0. Si ce n'est pas le cas, cette erreur de
 * d�synchronisation est d�tect�e et la longueur d'un segment de phase r�ajust�e
 * pour compenser. Le nombre maximal de TQ dont on peut rallonger le segment de
 * phase 1 ou raccourcir le segment de phase 2 est fix� par le Synchronization
 * Jump Width (sjw).
 * - Le segment de propagation est utilis� pour compenser les retards li�s aux
 * ph�nom�nes de propagation sur le bus.
 * - Le segment de phase 1 peut au besoin �tre rallong� en cas de d�tection
 * d'une erreur de synchronisation.
 * - Le segment de phase 2 peut au besoin �tre raccourci en cas de d�tection
 * d'une erreur de synchronisation. Dans l'impl�mentation actuelle, sa longueur
 * est d�duite de #NTQ et des autres param�tres.
 * 
 * La valeur effective du bit est lue 1 fois � la limite entre les segments de
 * phase 1 et 2. Il est �galement possible de lire la valeur 3 fois, en la
 * lisant � la limite ainsi qu'aux 2 time quanta pr�c�dents, la valeur effective
 * �tant celle majoritaire sur ces 3 mesures. Cette fonctionnalit� est appel�e
 * "triple sampling".
 * 
 * @warning Seules certaines combinaisons de param�tres ont un sens, il faut
 * donc veiller � respecter *strictement* les pr�conditions ci-dessous. Il faut
 * de plus utiliser le m�me jeu de param�tres pour tous les noeuds communiquant
 * entre eux sur le bus.
 * 
 * @param br d�bit de transmission et r�ception sur le bus.
 * @param trisample bool�en sp�cifiant l'activation du "triple sampling".
 * @param sjw nombre maximal de time quanta autoris� pour le r�ajustement des
 * segments de phase lors d'une resynchronisation.
 * @param propseg nombre de time quanta du segment de propagation.
 * @param seg1 nombre de time quanta du segment de phase 1.
 * 
 * @pre La fonction n'a jamais �t� appel�e.
 * @pre Le module CAN a �t� associ� � des pins par configuration du PPS.
 * @pre Les channels DMA 0 et 1 sont inutilis�es.
 * @pre @p sjw est un entier compris entre 1 inclus et 4 inclus.
 * @pre @p propseg est un entier compris entre 1 inclus et 8 inclus.
 * @pre @p seg1 est un entier compris entre 1 inclus et 8 inclus.
 * @pre Si trisample vaut @c true, seg1 est compris entre 3 inclus et 8 inclus.
 * @pre @p propseg + @p seg1 < @c NTQ - 1.
 * @pre 2*(@p propseg + @p seg1) >= @c NTQ - 1.
 * @pre @c NTQ - (@p propseg + @p seg1) > @p sjw + 1.
 * @post Le module CAN est pr�t � transmettre.
 * @post Si enable_can_rx_buffer() a �t� appel�e au moins une fois, alors le
 * module CAN est pr�t � recevoir.
 */
void config_can(Bitrate br, bool trisample, unsigned short sjw, 
        unsigned short propseg, unsigned short seg1);

/**
 * @brief Transmet une trame sur le bus CAN.
 * 
 * Transmet une trame standard (identifiant 11 bits) portant l'identifiant et le
 * contenu sp�cifi�s. Un exemple d'utilisation se trouve dans la description de
 * la biblioth�que.
 * 
 * @warning Si une transmission est en cours sur le buffer au moment o� cette fonction est
 * appel�e, elle sera interrompue et d�finitivement perdue.
 * 
 * @param id Identifiant de trame standard.
 * @param databytes Pointeur vers les donn�es contenues dans la trame.
 * @param bytes_count Nombre d'octets de donn�es contenues dans la trame (entier de 0 � 8).
 * 
 * @pre @p id est un entier compris entre 0x000 et 0x7F0 inclus.
 * @pre @p databytes est un pointeur non nul valide vers un octet ou tableau
 * d'octets cons�cutifs en m�moire � transmettre.
 * @pre @p bytes_count est inf�rieur ou �gal au nombre d'octets contenus dans
 * @p databytes.
 * @pre Le nombre d'octets � transmettre est un entier compris entre 0 inclus
 * et 8 inclus.
 * @pre The numero of @p buffer is comprised between 0 included and 7 included
 * @pre config_can() a �t� appel�e une et une seule fois.
 * @post La trame a �t� �mise sur le bus.
 * @see can_transmission_complete()
 * 
 * @todo Tester ce qui se passe si on l'appelle plusieurs fois alors que la
 * transmission �choue.
 * @todo Gestion des erreurs.
 */
void transmit_can(uint16_t id, const uint8_t* databytes, unsigned short bytes_count, Cantxbuf buffer);

/**
 * @brief Use the transmit_can function to transmit the canframe
 * 
 * @param frame Trame de donn�es � transmettre.
 * @param buffer The buffer that will be used to transmit the data.
 */
void transmit_canframe(Canframe frame, Cantxbuf buffer);

/**
 * @brief V�rifie la bonne transmission d'une trame CAN.
 * 
 * V�rifie que la derni�re trame transmise a �t� correctement plac�e sur le bus,
 * c'est-�-dire qu'un bit d'acquittement (ACK) a �t� re�u.
 * 
 * @pre transmit_can() a �t� appel�e.
 * @pre Le dernier appel � transmit_can() a �t� effectu� avec @p wait_completion
 * valant @c false.
 * @return @c true si la trame a �t� bien transmise, @c false sinon
 */
bool is_buffer_available();

/**
 * @brief Lit une trame standard re�ue sur le bus CAN.
 * 
 * Consulte le buffer sp�cifi� et retourne la trame contenue � l'int�rieur.
 * Si demand�, la fonction attend pr�alablement que le buffer soit marqu� comme
 * rempli (ie qu'une nouvelle trame soit arriv�e); sinon, elle retourne
 * imm�diatement. Dans ce dernier cas, si aucune nouvelle trame n'a �t� re�ue,
 * la derni�re trame � avoir �t� re�ue sera retourn�e. Si aucune trame n'a
 * jamais �t� re�ue, une trame vide (longueur 0) sera retourn�e.
 * 
 * @param buffer Le buffer de r�ception � lire.
 * @param wait_reception Bool�en indiquant d'attendre ou non la r�ception d'une
 * nouvelle trame sur ce buffer avant de lire.
 * 
 * @pre config_can() a �t� appel�e une et une seule fois.
 * @pre @p buffer est une constante d'�num�ration valide de type #Canrxbuf.
 * @pre Le buffer sp�cifi� a �t� configur� en r�ception par enable_can_rx_buffer().
 * @post Le buffer sp�cifi� est marqu� comme vid� (pas de nouvelle trame).
 * 
 * @return la trame contenue dans le buffer sp�cifi�
 * @see new_canframe_received()
 * 
 * @todo Gestion des erreurs.
 */
Canframe receive_can(Canrxbuf buffer, bool wait_reception);

/**
 * @brief V�rifie si une nouvelle trame CAN a �t� re�ue.
 * 
 * V�rifie si le buffer sp�cifi� est marqu� comme rempli, ie a re�u une nouvelle
 * trame CAN.
 * 
 * @param buffer Le buffer de r�ception � lire.
 * 
 * @pre config_can() a �t� appel�e une et une seule fois.
 * @pre @p buffer est une constante d'�num�ration valide de type #Canrxbuf.
 * @pre Le buffer sp�cifi� a �t� configur� en r�ception par enable_can_rx_buffer().
 * 
 * @return @c true si le buffer sp�cifi� contient une nouvelle trame, @c false sinon
 * @see receive_can()
 */
bool new_canframe_received(Canrxbuf buffer);

#endif

