#include <stdio.h>
#include <stdlib.h>

// Funktio, joka purkaa tiedoston sisältöä RLE-menetelmällä
void puraTiedosto(FILE *inputFile) {
    int laskuri;
    char ch;
    // Luetaan tiedostosta ensin luku	
    while (fread(&laskuri, sizeof(int), 1, inputFile) == 1) {
        // Luetaan luvun perään merkki ja varmistetaan, että tiedoston sisältö myös on haluttua muotoa
        if (fread(&ch, sizeof(char), 1, inputFile) != 1) {
            fprintf(stderr, "my-unzip: invalid file format\n");
            exit(1);
        }
        // Printataan luvun määräämä määrä merkkiä 
        for (int i = 0; i < laskuri; i++) {
            printf("%c", ch);
        }
    }
}
int main(int argc, char *argv[]) {
    // Tarkistetaan komentoriviparametrien määrä
    if (argc < 2) {
        fprintf(stderr, "my-unzip: file1 [file2 ...]\n");
        exit(1);
    }
    FILE *inputFile;
    for (int i = 1; i < argc; i++) {
        // Avataan annettu tiedosto ja puretaan se
        inputFile = fopen(argv[i], "r");
        if (inputFile == NULL) {
            fprintf(stderr, "my-unzip: cannot open file\n");
            exit(1);
        }
        
        puraTiedosto(inputFile);
        fclose(inputFile);
    }
    // Printataan lopussa rivinvaihto, niin näyttää lopputulos näyttää siistimmältä
    printf("\n");
    
    return 0;
}
