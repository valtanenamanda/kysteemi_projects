#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

// Määritellään linkitetyn listan solmu
typedef struct Node {
    char *line;
    struct Node *next;
} Node;

// Funktio uuden solmun luomiseksi
Node* create_node(char *line) {
    char error_message[30] = "An error has occurred\n";
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }
    new_node->line = strdup(line);
    new_node->next = NULL;
    return new_node;
}

// Funktio listan tyhjentämiseksi
void free_list(Node *head) {
    Node *tmp;
    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp->line);
        free(tmp);
    }
}
int suoritaProsessi(char *arg[], char *filename, int redirect, char *path[]);

char** maaritaPolku(char *arg[], char *path[]) {
    if (path != NULL) {
        free(path);
    }
    int i = 0;
    // Varataan uudelle polulle muistia
    path = (char **)malloc((1032) * sizeof(char *));
    while (arg[i+1] != NULL) {
            	path[i] = arg[i+1];
            	i++;
            }        
    return path;
}

int tulkitseKomento(char *arg[], char *path[]) {
    char error_message[30] = "An error has occurred\n";
    int redirect = 0;
    char *filename = NULL;
    int num_pros = 0;
    pid_t pids[100];
    memset(pids, 0, sizeof(pids));
    for (int i = 0; arg[i] != NULL; i++) {
        // Käsittele uudelleenohjaus
        if (*arg[i] == '>' && i != 0 && arg[i + 1] != NULL && arg[i + 2] == NULL) {
            filename = arg[i + 1];
            arg[i] = NULL;
            arg[i + 1] = NULL;
            redirect = 1;
        } else {
            char *redir_pos = strchr(arg[i], '>');
            char *komento = arg[i];
            int len = strlen(komento);
            if (redir_pos != NULL) {
            	if (redir_pos == arg[i] && i == 0) { // Jos '>' on argumentin alussa eli komento muotoa '>...', niin se on virhe.
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    exit(1);
            	} // Jos muotoa 'komento> tiedosto':
            	if (komento[len-1] == '>' && arg[i + 1] != NULL && arg[i + 2] == NULL) {
                    filename = arg[i + 1]; // Poimitaan tiedoston nimi talteen
                    komento[len - 1] = '\0'; // Poistetaan '>'
                    arg[i] = komento;
                    arg[i + 1] = NULL; // Lopetetaan komento tähän, jotta ">" ja tiedostonimi eivät ole mukana
                    redirect = 1;
            	} // Jos muotoa 'komento >tiedosto': 
            	else if (komento[0] == '>' && i != 0 && arg[i + 1] == NULL) {
                    komento = komento + 1; // Siirretän pointeria = poistetaan tiedostonimen alusta >-merkki.
                    filename = komento; // Tiedoston nimi talteen
                    arg[i] = NULL;  // Lopetetaan komento tähän, jotta ">" ja tiedostonimi eivät ole mukana execvp-kutsussa
                    redirect = 1;
            	} // Jos muotoa 'komento>tiedosto':
            	else if (*(redir_pos + 1) != '\0' && arg[i + 1] == NULL) {
            	    filename = redir_pos + 1;
                    *redir_pos = '\0'; // Erotetaan komento tiedostosta ja >-merkistä
                    redirect = 1;
            	}
            	// Muuten virhe, jos > löytyi.
            	else {
                    write(STDERR_FILENO, error_message, strlen(error_message));
                exit(1);
            	}
            }
        }
    }
    // Käsittele rinnakkaiset komennot
    int apu = 0;
    char **prosessi = (char **)malloc((sizeof(*arg)) * sizeof(char *));
    memset(prosessi, 0, sizeof(*prosessi) * sizeof(char *));
    int i = 0;
    while (arg[i] != NULL) {
        // Tutkitaan, onko rinnakkaisia komentoja
        // Pilkotaan &-merkin kohdalta
        char *par_pos = strchr(arg[i], '&');
        // Katsotaan, jos on muotoa 'komento & komento':
        if (*arg[i] == '&' && i != 0 && arg[i + 1] != NULL) {
            // Aloitetaan prosessi
            pids[num_pros++] = fork();
            if (pids[num_pros - 1] < 0) {
        	// Fork epäonnistui
        	write(STDERR_FILENO, error_message, strlen(error_message));
       		exit(1);
            // Mikäli kyseessä lapsiprosessi, niin suoritetaan loppuun:
            } else if (pids[num_pros - 1] == 0) {
                suoritaProsessi(prosessi, filename, redirect, path);
                memset(prosessi, 0, sizeof(*prosessi) * sizeof(char *));
                free(prosessi);
                exit(0);
            } 
            // Nollataan prosessimuuttuja
            memset(prosessi, 0, sizeof(*prosessi) * sizeof(char *));
            apu = 0;
            i++;
        // Jos arg[i] ei ollut &-merkki, niin katsotaan onko arg[i] muotoa 'komento&komento':
        } else if (par_pos != NULL && *(par_pos+1) != '\0') {
            // Tallennetaan apumuuttujaan koko 'komento&komento' 
            char *next= arg[i];
            // Pilkotaan arg[i] muotoon 'komento'
            *par_pos = '\0';
            // Laitetaan prosessimuuttujaan arg[i] eli komento, joka jäi &-merkin vasemmalle puolelle ja luodaan sille prosessi.
            prosessi[apu++] =  arg[i];
            // Tallennetaan &-merkin jälkeen oleva komento muuttujaan arg[i] ja jatketaan sillä while-silmukkaa.
            next = (par_pos+1);
            arg[i] = next;
            pids[num_pros++] = fork();
            if (pids[num_pros - 1] < 0) {
        	// Fork epäonnistui
        	write(STDERR_FILENO, error_message, strlen(error_message));
       		exit(1);
            } else if (pids[num_pros - 1] == 0) {
                suoritaProsessi(prosessi, filename, redirect, path);
                memset(prosessi, 0, sizeof(*prosessi) * sizeof(char *));
                free(prosessi);
                exit(0);
            } 
            memset(prosessi, 0, sizeof(*prosessi) * sizeof(char *));
            apu = 0;
            
        }
        else {
            // Muussa tapauksessa tallennetaan prosessimuuttujaan uusi alkio
            prosessi[apu++] = arg[i++];
        }
    }
    // Luodaan vielä prosessi &-merkin jälkeiselle komennolle tai jos rivillä ei ole ollut yhtäkään &-merkkiä:
    prosessi[apu] = NULL;
    pids[num_pros++] = fork();
    if (pids[num_pros - 1] < 0) {
        // Fork epäonnistui
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    } else if (pids[num_pros - 1] == 0) {
        suoritaProsessi(prosessi, filename, redirect, path);
        memset(prosessi, 0, sizeof(*prosessi) * sizeof(char *));
        free(prosessi);
        exit(0);
    } 
    
    // Odota kaikkien prosessien valmistumista
    for (int i = 0; i < num_pros; i++) {
    	//printf("%d", i);
        waitpid(pids[i], NULL, 0);
    }
    
    return 0;
}

int suoritaProsessi(char *arg[], char *filename, int redirect, char *path[]) {
    char error_message[30] = "An error has occurred\n";
    // Jos on aiemmin tunnistettu riviltä >-merkki, niin kirjoitetaan tiedostoon komennon printti:
    if (redirect) {
	// Tähän kohtaan löysin netistä apua: https://stackoverflow.com/questions/70369289/why-cant-i-open-my-file-using-open-method
        int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        }
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);
    }

        // Käydään läpi löydetäänkö annetuista poluista komentoamme:
        int i = 0;
        while (path[i] != NULL) {
            //printf("%s", path[i]);
            if (path[i] == NULL) { 
                write(STDERR_FILENO, error_message, strlen(error_message));
	        exit(1);
            } 
            char* path_real = (char *)malloc((1032) * sizeof(char *));
            sprintf(path_real, "%s/%s", path[i], arg[0]);
            // Jos päästään polkua komentoomme, niin ajetaan komento parametrein:
            if (access(path_real, X_OK) == 0) {
                //printf("%s", path[i]);
                if (execv(path_real, arg) == -1) {
	            write(STDERR_FILENO, error_message, strlen(error_message));
	            exit(1);
	        } 
            }    
            i++;
            // Vapautetaan muodostettu polku uutta kierrosta varten
            free(path_real);
            
            
        }
        
        
    return 0;


}

int main(int argc, char *argv[]) {
    char error_message[30] = "An error has occurred\n";
    char** original_path = (char **)malloc(1032 * sizeof(char *));
    original_path[1] = strdup("/bin");
    char** path = NULL;
    // Määritetään oletuspolku
    path = maaritaPolku(original_path, path);
    // Tutkitaan alussa syötettyjä komentoriviparametreja:
    if (argc > 2) {
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }
    // Mikäli on annettu ./wish tiedosto.txt:
    if (argc == 2) { 
        // Kokeillaan avata tiedosto
        FILE *file = fopen(argv[1], "r");
        if (file == NULL) {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        }
        // Muodostetaan ensimmäinen node aktivoimalla oletuspolku
        Node *head = NULL;
    	Node *current = NULL;
    	size_t len = 0;
        ssize_t read;
        char *line = NULL;
        char startline[10] = "path /bin";
        Node *new_node = create_node(startline);
                head = new_node;
                current = head;
        // Lähdetään lukemaan tiedoston rivit linkitettyyn listaan
        while ((read = getline(&line, &len, file)) != -1) {
            if (line[read - 1] == '\n') {
                line[read - 1] = '\0';
            }
            Node *new_node = create_node(line);
                current->next = new_node;
                current = current->next;
            
        } 
            
        current = head;    
        // Käydään linkitetty lista läpi
        while (current != NULL) {    
            char *rivi_kopio = strdup(current->line);
            
            int i = 0;
            char **arg = (char **)malloc((read + 1) * sizeof(char *));
            char *apu = NULL;
            // Poistetaan ylimäärääiset välilyönnit ja pilkotaan rivi aina välilyöntien kohdalta listan arg alkioiksi
            if ((apu = strtok(rivi_kopio, " ")) != NULL) {
                arg[i] = strdup(apu);
            
                while ((apu = strtok(NULL, " ")) != NULL) {
                    arg[++i] = strdup(apu);
                }
            } else {
            	arg[0] = NULL;
            }
            // Jos on ylipäätään luettu mitään, niin ajetaan komento:
            if (arg[0] != NULL) {
                // Katsotaan ensin onko se sisäänrakennettu exit, cd tai path
                if (strcmp(arg[0], "exit") == 0) {
            	    free(path);
            	    memset(arg, 0, sizeof(*arg) * sizeof(char *));
            	    free(arg);
            	    free_list(head);
                    fclose(file);
                    exit(0);
                }
                if (strcmp(arg[0], "cd") == 0) {
		    if (arg[2] != NULL || arg[1] == NULL) {
		        write(STDERR_FILENO, error_message, strlen(error_message));
		        exit(1);
		    } else {
		        if (chdir(arg[1]) != 0) {
		            write(STDERR_FILENO, error_message, strlen(error_message));
		            exit(1);
		        }
		}

    	    	} else if (strcmp(arg[0], "path") == 0) {
		    if (arg[1] == NULL) {
		    	write(STDERR_FILENO, error_message, strlen(error_message));
		    	exit(1);
		  
		    } else {
		        // Määritetään uusi polku aliohjelmassa ja tyhjennetään vanha.
		    	path = maaritaPolku(arg, path);
	    	    }
	    	// Mikäli ei ole sisäänrakennettu komento, niin mennään aliohjelmaan ajamaan se
	        } else {
            	    tulkitseKomento(arg, path);
                }
            // Tyhjennetään tallennettu rivi
            memset(arg, 0, sizeof(*arg) * sizeof(char *));
            free(arg);
            
            
        }
        // Vapautetaan lopussa varattu muisti ja suljetaan tiedosto
        free(rivi_kopio);
        current = current->next;
        }
        free_list(head);
        fclose(file);
        
    
    }
    else {
    // Tässä sama rakenne kuin tiedostoa luettaessa, mutta ei tallenneta linkitettyyn listaan, luetaan stdinistä ja printataan joka kierroksella wish
    while (1) {
        size_t len = 0;
        ssize_t read;
        char *line = NULL;
        printf("wish> ");
        read = getline(&line, &len, stdin);
        if (read == -1) {
            write(STDERR_FILENO, error_message, strlen(error_message));
            free(line);
            exit(1);
        }
        if (line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        int i = 0;
        char **arg = (char **)malloc((read + 1) * sizeof(char *));
        char *apu = NULL;
        if ((apu = strtok(line, " ")) != NULL) {
            arg[i] = strdup(apu);
            while ((apu = strtok(NULL, " ")) != NULL) {
                arg[++i] = strdup(apu);
            }
        } else {
            arg[0] = NULL;
        }

        if (arg[0] != NULL) {
            
            if (strcmp(arg[0], "exit") == 0) {
            	free(path);
            	memset(arg, 0, sizeof(*arg) * sizeof(char *));
            	free(arg);
                exit(0);
            }
            else if (strcmp(arg[0], "cd") == 0) {
		if (arg[2] != NULL || arg[1] == NULL) {
		    write(STDERR_FILENO, error_message, strlen(error_message));
		    exit(1);
		} else {
		    if (chdir(arg[1]) != 0) {
		        write(STDERR_FILENO, error_message, strlen(error_message));
		        exit(1);
		    }
		}
    	    } else if (strcmp(arg[0], "path") == 0) {
		    if (arg[1] == NULL) {
		    	write(STDERR_FILENO, error_message, strlen(error_message));
		    	exit(1);
		  
		    } else {
		    	path = maaritaPolku(arg, path);
	    	    
	    	    }
	    } else {
            	tulkitseKomento(arg, path);
            }
            memset(arg, 0, sizeof(*arg) * sizeof(char *));
            free(arg);     
        }
        }
    }
    return 0;
}

