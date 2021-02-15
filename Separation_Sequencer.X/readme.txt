Ce dossier contient les codes de tous les séquenceurs de la fusée.

Programmation :
	- Logiciels nécessaires (installer dans cet ordre) :
		* Microchip MPLAB X IDE, v3.15
		* Microchip XC16, v1.29
		* Legacy PIC24 MCU & dsPIC DSC Peripheral Libraries, v2.00
	- Instructions :
		* Mettre le séquenceur sous tension
		* Brancher le PICKit 3 à l'ordinateur et au séquenceur
		* Lancer MPLAB X IDE, et ouvrir le projet Sequenceur.X
		* Choisir la configuration désirée dans la liste : 
		  seq_recup_h, seq_pyro, seq_recup_b, seq_cansat, seq_ssie
		* Cliquer sur 'Make and Program Device Main Project'
		* Dès l'apparition de la ligne 'Program/Verify complete',
		  le séquenceur est programmé.