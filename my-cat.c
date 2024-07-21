#include <stdio.h>
#include <stdlib.h>

// Katsotaan ensin, montako parametria on annettu.
int main(int argc, char *argv[]) {
    if (argc < 2) {
        // Jos tiedostonimiä ei ole annettu, ohjelma poistuu ja palauttaa 0
        return 0;
    }
    // Tiedostonnimiä on annettu yksi tai useampi, iteroidaan ne läpi yksi kerrallaan
    for (int i = 1; i < argc; i++) {
    	// Avataan tiedosto
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            // Jos tiedoston avaaminen epäonnistuu, tulostetaan virheilmoitus ja poistutaan koodilla 1
            fprintf(stderr, "my-cat: cannot open file\n");
            exit(1);
        }
        
        // Varataan puskuri, johon rivit luetaan
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), fp)) {
            // Tulostetaan luettu rivi
            printf("%s", buffer);
        }

        // Suljetaan käsitelty tiedosto
        fclose(fp);
    }
    // Ohjelman onnistuessa poistutaan koodilla 0
    return 0;
}










