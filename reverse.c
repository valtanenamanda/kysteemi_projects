#include <stdio.h> // Tiedostojen käsittely
#include <stdlib.h> // Muu yleishyödyllinen, kuten exit
#include <string.h> // Merkkijonojen käsittelyyn

// Määritellään rakenne nimeltä Node, joka edustaa yksittäistä solmua linkitetyssä listassa
struct Node {
    char *line; // Osoitin merkkijonoon, joka sisältää yhden rivin tiedostosta
    struct Node *next; // Osoitin seuraavaan solmuun linkitetyssä listassa
};

// Funktio uuden solmun lisäämiseksi linkitetyn listan alkuun, head on osoitin osoittimeen, joka osoittaa listan alkuun, ja new_line on merkkijono, joka sisältää lisättävän rivin
void push(struct Node **head, char *new_line) {
    // Varataan muistia uudelle solmulle
    struct Node *new_node = (struct Node*) malloc(sizeof(struct Node));
    // Tarkistetaan, että muistinvaraus onnistui
    if (new_node == NULL) {
        // Jos muistinvaraus epäonnistuu, tulostetaan virheviesti ja lopetetaan ohjelma
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    // Kopioidaan uusi rivi uuteen solmuun käyttämällä strdup-funktiota
    new_node->line = strdup(new_line);
    // Tarkistetaan, että rivin kopiointi onnistui
    if (new_node->line == NULL) {
        // Jos rivin kopiointi epäonnistuu, tulostetaan virheviesti ja lopetetaan ohjelma
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    // Asetetaan uuden solmun seuraava osoitin osoittamaan listan ensimmäiseen solmuun
    new_node->next = *head;
    // Asetetaan uusi solmu listan ensimmäiseksi solmuksi
    *head = new_node;
}


// Funktio, joka tulostaa linkitetyn listan sisällön tiedostoon
void printList(struct Node *node, FILE *outputFile) {
    while (node != NULL) { // Käydään läpi linkitetty lista
        fprintf(outputFile, "%s", node->line); // Tulostetaan nykyisen solmun rivin sisältö tiedostoon
        node = node->next; // Siirrytään seuraavaan solmuun
    }
}


// Funktio, joka vapauttaa koko linkitetyn listan varaaman muistin
void freeList(struct Node *node) {
    struct Node *temp; // Väliaikainen osoitin nykyisen solmun tallentamiseksi
    while (node != NULL) { // Käydään läpi koko linkitetty lista
        temp = node; // Tallennetaan nykyinen solmu väliaikaiseen osoittimeen
        node = node->next; // Siirrytään seuraavaan solmuun
        free(temp->line); // Vapautetaan nykyisen solmun tallentama merkkijono
        free(temp); // Vapautetaan itse solmu
    }
}


// Pääohjelma, joka lukee annetun tiedoston ja tulostaa sen sisällön
int main(int argc, char *argv[]) {
    if (argc > 3) { // Tarkistetaan, että komentoriviparametreja ei ole liikaa
        fprintf(stderr, "usage: reverse <input> <output>\n"); // Jos parametreja on liikaa, tulostetaan virheviesti ja lopetetaan ohjelma
        exit(1);
    }
    // Tarkista, että syöttö- ja tulostustiedostot eivät ole samoja, jos on annettu output-tiedostokin.
    if ((argc == 3) && (strcmp(argv[1], argv[2]) == 0)) {
    	fprintf(stderr, "Input and output file must differ\n"); // Jos input- ja output-tiedostot samat, tulostetaan virheviesti ja lopetetaan ohjelma
    	exit(1);
    }
    
    // Määritellään input- ja output-tiedostot:
    FILE *inputFile = NULL;
    FILE *outputFile = NULL;

    if (argc > 1) { // Jos komentoriviparametreja ainakin yksi ohjelman nimen lisäksi (ohjelman nimi + tiedostonimi (+ ...)):
        // Yritetään avata syöttötiedosto lukemista varten, otetaan toinen komentoriviparametri komennolla argc[1]

    	inputFile = fopen(argv[1], "r");
    	if (inputFile == NULL) { // Jos tiedoston avaaminen epäonnistuu, tulostetaan virheviesti ja lopetetaan ohjelma
	     fprintf(stderr, "error: cannot open file '%s'\n", argv[1]);
	     exit(1);
    	}
    } else { // Jos input-tiedostoa ei ole annettu, niin otetaan rivit käyttäjältä standard inputina.
	inputFile = stdin;
    }
    if (argc == 3) { // Jos myös output-tiedosto annetaan:
        // Avaa tulostustiedosto kirjoittamista varten
        outputFile = fopen(argv[2], "w");
        if (outputFile == NULL) { // Jos tiedoston avaaminen epäonnistuu, tulostetaan virheviesti ja lopetetaan ohjelma
            fprintf(stderr, "error: cannot open file '%s'\n", argv[2]);
            if (inputFile != stdin) fclose(inputFile); // Suljetaan lisäksi jo mahdollisesti avattu input-tiedosto
            exit(1);
        }
    } else { // Jos output-tiedostoa ei ole annettu, niin tulostetaan output näytölle
        outputFile = stdout;
    }

    
    // Muuttujat rivin lukemista varten
    struct Node *head = NULL; // Uusi solmu luetun rivin tallentamista varten
    char *line = NULL; // Osoitin merkkijonoon, joka tallentaa luetun rivin
    size_t len = 0; // Merkkijonon pituus
    // Lue syöttötiedosto rivi kerrallaan ja lisää se listan alkuun
    while (getline(&line, &len, inputFile) != -1) {
        push(&head, line);
    }
    // Tulostetaan linkitetyn listan sisältö tiedostoon tai näytölle
    printList(head, outputFile);
    // Vapautetaan getline-funktion käsittelemä muisti
    free(line);
    // Tyhjennetään linkitetty lista
    freeList(head);
    // Suljetaan tiedosto, jota luettiin, jos se ei ole stdin
    if (inputFile != stdin) {
        fclose(inputFile);
    }
    
    // Sulje tulostustiedosto, jos se ei ole stdout
    if (outputFile != stdout) {
        fclose(outputFile);
    }
    return 0;
}




