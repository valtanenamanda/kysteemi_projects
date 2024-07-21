#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Funktio, joka pakkaa tiedoston sisältöä RLE-menetelmällä
int pakkaaTiedosto(FILE *inputFile) {
    int laskuri;
    char nykyinen, edellinen;
    // Poistutaan, jos tiedosto on tyhjä tai määritellään ensimmäinen arvo muuttujalle nykyinen
    if ((nykyinen = fgetc(inputFile)) == EOF) {
        return 0;
    }
    laskuri = 1;
    edellinen = nykyinen;
    // Käydään tiedostoa läpi merkki kerrallaan, kasvatetaan laskuria jos uusi merkki on sama kuin edellinen
    while ((nykyinen = fgetc(inputFile)) != EOF) {
        if (nykyinen == edellinen) {
            laskuri++;
        } else {
            // Kirjoitetaan laskuri (4-bittinen kokonaisluku) ja merkki tiedostoon
            fwrite(&laskuri, 4, 1, stdout);
            fwrite(&edellinen, 1, 1, stdout);
            // Nollataan laskuri
            laskuri = 1;
        } edellinen = nykyinen;
    }	
    return 0;
}
int main(int argc, char *argv[]) {
    // Tarkistetaan komentoriviparametrien määrä
    if (argc < 2) {
        fprintf(stderr, "my-zip: file1 [file2 ...]\n");
        exit(1);
    }
    for (int i = 1; i < argc; i++) {
        FILE *inputFile = fopen(argv[i], "r");
        if (inputFile == NULL) {
            fprintf(stderr, "my-zip: cannot open file\n");
            exit(1);
        }   
        pakkaaTiedosto(inputFile);
        fclose(inputFile);
    }
    return 0;
}
