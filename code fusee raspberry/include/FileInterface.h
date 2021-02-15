#ifndef FILEINTERFACE_H
#define FILEINTERFACE_H

#include <linux/can.h>
#include <iostream>
#include <fstream>
#include <string>

/**
 *
 * 
 **/
class FileInterface
{
	public:
	FileInterface();

	/**
	 * @brief Initialize le ficher
	 * 
	 * Cette fonction crée un nouveau fichier "basenameXXX"
	 *  - basename est la chaine de caractères passée en paramètre (qui inclu le chemin du fichier).
	 *  - XXX est un numéro automatiquement déterminé: la fonction parcours les fichiers
	 *    "basename001',"basename002",... Jusqu'à trouver un numéro non existant. Ainsi, les nouveaux
	 *    fichier sont crées avec un numéro qui incrémente à chaque execution. 
	 * 
	 * Le fichier est ensuite associé à l'instance de cette classe.
	 * */
	void init(std::string basename);

	/**
	 * @brief Ecrit une chaine de charactères de le fichier.
	 * */
	void write(std::string s);

	/**
	 * @brief Ecrit une chaine de charactères de le fichier en ajoutant un retour à la ligne.
	 * */
	void write_line(std::string line);

	/* *
	 * Les methodes ci-dessous sont bordeliques et ne sont que des fonctions utilitaires
	 * pour écrire rapidement certains objets dans le fichier. Il est fortement conseillé
	 * de les réecrires.
	 * */
	 
	void write_canframe(struct can_frame frame, unsigned long timestamp);

	void write_array(uint8_t array[], int len);

	static std::string canframe_string(struct can_frame frame, unsigned long timestamp);

	static std::string array_string(uint8_t array[], int len, unsigned long timestamp);

	private:
	/**
	 * @brief Vérifie si un fichier existe.
	 * */
	bool fexist(const std::string& filename);
	
	/**
	 * Converti les entiers de 0 à 999 en un string de 3 chiffres
	 * ex: generateIndex(22) --> "022"
	 * */
	std::string generateIndex(uint8_t i);
	
	/// Le fichier texte associé à cette classe.
	std::ofstream file;
};

#endif
