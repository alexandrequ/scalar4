Ce dossier contient les codes de tous les s�quenceurs de la fus�e.

Programmation :
	- Logiciels n�cessaires (installer dans cet ordre) :
		* Microchip MPLAB X IDE, v3.15
		* Microchip XC16, v1.29
		* Legacy PIC24 MCU & dsPIC DSC Peripheral Libraries, v2.00
	- Instructions :
		* Mettre le s�quenceur sous tension
		* Brancher le PICKit 3 � l'ordinateur et au s�quenceur
		* Lancer MPLAB X IDE, et ouvrir le projet Sequenceur.X
		* Choisir la configuration d�sir�e dans la liste : 
		  seq_recup_h, seq_pyro, seq_recup_b, seq_cansat, seq_ssie
		* Cliquer sur 'Make and Program Device Main Project'
		* D�s l'apparition de la ligne 'Program/Verify complete',
		  le s�quenceur est programm�.