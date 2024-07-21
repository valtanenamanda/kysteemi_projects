#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {
    // Tarkistetaan, onko käyttäjä syöttänyt yhtään komentoriviparametreja
    if (argc < 2) {
        fprintf(stderr, "my-grep: searchterm [file ...]\n");
        exit(1);
    }
    // Määritellään hakutermi, joka on siis ensimmäinen annetuista komentoriviparametreista
    char *searchTerm = argv[1];
    if (strlen(searchTerm) == 0) {
        // Jos hakutermi on tyhjä merkkijono, lopetetaan ohjelma eikä printata mitään
        return 0;
    }
    
    // Jos ei ole annettu tiedosto(ja) luettavaksi, otetaan vastaan ja luetaan standard input 
    if (argc == 2) {
    	FILE *fp = stdin;
    	char buffer[1024];
    	while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            if (strstr(buffer, searchTerm) != NULL) {
                printf("%s", buffer);
            }  
   	}   
        return(0);
    }
    // Jos tiedostoja on annettu, käsitellään ne tässä
    if (argc > 2) {	
        // Iteroidaan tiedostot
    	for (int i = 2; i < argc; i++) {
    	    // Avataan tiedosto
            FILE *fp = fopen(argv[i], "r");
            if (fp == NULL) {
                // Jos tiedoston avaaminen epäonnistuu, tulostetaan virheilmoitus ja poistutaan koodilla 1
                fprintf(stderr, "my-grep: cannot open file\n");
                exit(1);
            }
	    // Alustetaan getlinea varten muuttujat
            char *line = NULL;
            size_t len = 0;

            // Luetaan tiedoston rivit yksi kerrallaan ja tulostetaan ne, mikäli ne sopivat hakutermiin
            while ((getline(&line, &len, fp)) != -1) {
	        if (strstr(line, searchTerm) != NULL) {
	    	    printf("%s", line);
    	        } 
            }

            // Vapautetaan getline-funktion varaama muisti
            free(line);

            // Suljetaan tiedosto
            fclose(fp);
        }    
    }        
    // Palautetaan 0 onnistuneen ajon merkiksi
    return 0;
}
