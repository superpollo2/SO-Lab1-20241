#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


void ensureNewlineAtEnd(const char *filename) {
    FILE *file = fopen(filename, "a");  // Abre el archivo en modo "append"
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }

    fseek(file, -1, SEEK_END);  // Mueve el puntero al último carácter del archivo

    // Verifica si el último carácter es un salto de línea
    if (ftell(file) > 0 && fgetc(file) != '\n') {
        fputc('\n', file);  // Agrega un salto de línea al final del archivo
    }

    fclose(file);
}

int areSameFiles(const char *file1, const char *file2) {
    struct stat input_stat, output_stat;

    // Obtener información del primer archivo
    if (stat(file1, &input_stat) != 0) {
        perror("stat");
        exit(1);
    }

    // Obtener información del segundo archivo
    if (stat(file2, &output_stat) != 0) {
        perror("stat");
        exit(1);
    }
    // Comparar los números de inodo y los dispositivos
    return (input_stat.st_ino == output_stat.st_ino) && (input_stat.st_dev == output_stat.st_dev);
}
// Función principal
int main(int argc, char *argv[]) {
    // Manejar el número incorrecto de argumentos
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        return 1;
    }

    // Archivos de entrada y salida
    FILE *inputFile = stdin;
    FILE *outputFile = stdout;

    // Verificar si se proporcionaron archivos de entrada y salida
    if (argc > 1) {
        
        ensureNewlineAtEnd(argv[1]);
        // Intentar abrir el archivo de entrada
        inputFile = fopen(argv[1], "r");

        // Manejar errores al abrir el archivo de entrada
        if (inputFile == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            return 1;
        }
    }
    
    if (argc == 3) {
        if (areSameFiles(argv[1], argv[2])) {
            fprintf(stderr, "reverse: input and output file must differ\n");
            return 1;
        }

        // Intentar abrir el archivo de salida
        outputFile = fopen(argv[2], "w");

        // Manejar errores al abrir el archivo de salida
        if (outputFile == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[2]);

            // Cerrar el archivo de entrada si está abierto
            if (inputFile != stdin) {
                fclose(inputFile);
            }

            return 1;
        }
    }

    // Leer y almacenar las líneas del archivo de entrada
    char **lines = NULL;
    size_t lineCount = 0;
    size_t maxLines = 10; // Puedes ajustar según sea necesario

    lines = malloc(maxLines * sizeof(char *));
    if (lines == NULL) {
        fprintf(stderr, "malloc failed\n");

        // Cerrar el archivo de entrada si está abierto
        if (inputFile != stdin) {
            fclose(inputFile);
        }

        return 1;
    }

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, inputFile) != -1) {
        // Almacenar la línea en el arreglo
        lines[lineCount] = strdup(line);
        lineCount++;

        // Ajustar el tamaño del arreglo si es necesario
        if (lineCount == maxLines) {
            maxLines *= 2;
            lines = realloc(lines, maxLines * sizeof(char *));
            if (lines == NULL) {
                fprintf(stderr, "malloc failed\n");

                // Liberar memoria y cerrar archivos
                free(line);
                for (size_t i = 0; i < lineCount; i++) {
                    free(lines[i]);
                }
                free(lines);

                // Cerrar el archivo de entrada si está abierto
                if (inputFile != stdin) {
                    fclose(inputFile);
                }

                return 1;
            }
        }
    }

    // Imprimir las líneas en orden inverso en el archivo de salida
    for (size_t i = lineCount; i > 0; i--) {
        fprintf(outputFile, "%s", lines[i - 1]);
        free(lines[i - 1]);
    }



    // Liberar memoria y cerrar archivos
    free(line);
    free(lines);

    if (inputFile != stdin) {
        fclose(inputFile);
    }

    if (outputFile != stdout) {
        fclose(outputFile);
    }

    return 0;
}
