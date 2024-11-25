#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* retirarTag(char *linha, char* tagRetirar_Inicio, char* tagRetirar_Final) {
    
    char *inicio_tag = strstr(linha, tagRetirar_Inicio);
    inicio_tag += strlen(tagRetirar_Inicio);
    
    char *fim_tag = strstr(inicio_tag, tagRetirar_Final);

    int tam_tag = fim_tag - inicio_tag;
    char *tag = (char *)malloc(tam_tag + 1);
    
    if (tag == NULL) {
        fprintf(stderr, "Erro na alocação da Memória\n");
        return NULL;
    }

    strncpy(tag, inicio_tag, tam_tag);
    tag[tam_tag] = '\0';

    return tag;
}

void encontrar_tag(FILE *fp2, char *linha, int nLines, int *finalCabecalhoxml) {

    char *tag = NULL;
    
    char *inicio = NULL;
    char *final = NULL;
    static char *tag_description = NULL; 
    static int tam = 0;

    if (nLines > *finalCabecalhoxml){
        
        if ((strstr(linha, "<title>") != NULL)) {
            
            tag = retirarTag(linha, "<title>", "</title>");
            if (tag != NULL) {
                fprintf(fp2,"\n\t<hr>\n\n");
                fprintf(fp2, "\t<h2>%s</h2>\n", tag);
                free(tag);
            }

        } else if ((strstr(linha, "<link>") != NULL)) {
            
            tag = retirarTag(linha, "<link>", "</link>");
            if (tag != NULL) {
                fprintf(fp2, "\t<a href=%s> %s </a>\n", tag, tag);
                free(tag);
            }
        
        } else if ((strstr(linha, "<description>")) != NULL) {
            
            inicio = strstr(linha, "<description>") + strlen("<description>");
            tam = strlen(inicio);
            tag_description = (char *)malloc(tam + 1);
            
            if (tag_description == NULL) {
                fprintf(stderr, "Erro na alocação da Memória\n");
                exit(1);
            }

            strcpy(tag_description, inicio);

        } else if ((strstr(linha, "]]></description>")) != NULL) {
            final = strstr(linha, "]]></description>");
            tam += final - linha;  

            tag_description = (char *)realloc(tag_description, tam + 1);
            
            if (tag_description == NULL) {
                fprintf(stderr, "Erro na realocação da Memória\n");
                exit(1);
            }

            strncat(tag_description, linha, final - linha);
            tag_description[tam] = '\0';  
            
            fprintf(fp2, "\t<p> %s </p>\n", tag_description);
            
            free(tag_description);
            tag_description = NULL; 
            
        } else if (tag_description != NULL) {
            
            tam += strlen(linha);  
            tag_description = (char *)realloc(tag_description, tam + 1);
            
            if (tag_description == NULL) {
                fprintf(stderr, "Erro na realocação da Memória\n");
                exit(1);
            }

            strcat(tag_description, linha);


        } else if ((strstr(linha, "<pubDate>") != NULL)) {
            
            tag = retirarTag(linha, "<pubDate>", "</pubDate>");
            if (tag != NULL) {
                fprintf(fp2, "\t<p> %s </p>\n", tag);
                free(tag);
            }
        }
            
    }else{
        if ((strstr(linha, "<title>") != NULL)) {
            tag = retirarTag(linha, "<title>", "</title>");
            
            if (tag != NULL) {
                fprintf(fp2, "\t<h1>%s</h1>\n", tag);
                free(tag);
            }

        } else if ((strstr(linha, "<link>") != NULL)) {
            
            tag = retirarTag(linha, "<link>", "</link>");
            if (tag != NULL) {
                fprintf(fp2, "\t<a href=%s> %s </a>\n", tag, tag);
                free(tag);
            }
    
        } else if ((strstr(linha, "<description>")) != NULL) {
            
            tag = retirarTag(linha, "<description>", "</description>");
            if (tag != NULL) {
                fprintf(fp2, "\t<a href=%s> %s </a>\n", tag, tag);
                free(tag);
            }
        }

    }
}

int main(int argc, char const *argv[]) {

    FILE *fp, *fp2;
    char linha[1000];
    int nLines = 0;

    fp = fopen("<arquivo.xml>", "r");
    if (fp == NULL) {
        fprintf(stderr, "Erro!! Não foi possível abrir o arquivo\n");
        return 1;
    }

    fp2 = fopen("index.html", "w");
  
    if (fp2 == NULL) {
        fclose(fp);
        fprintf(stderr, "Erro!! Não foi possível abrir o arquivo\n");
        return 1;
    }

    fprintf(fp2, "<!DOCTYPE html>\n<html>\n<head>\n<title>RSS-Fead</title>\n</head>\n<body>\n");

    int finalCabecalhoxml = 0;

    while (fgets(linha, sizeof(linha), fp) != NULL) {
        finalCabecalhoxml++;
        if (strstr(linha, "</managingEditor>") != NULL) {
            break;
        }
    }
    
    rewind(fp);

    while (fgets(linha, sizeof(linha), fp) != NULL) {        
        encontrar_tag(fp2, linha, nLines, &finalCabecalhoxml);
        nLines++;
    }

    fprintf(fp2, "\n</body>\n</html>");

    fclose(fp);
    fclose(fp2);

    return 0;
}