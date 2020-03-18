#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    // we are using dictionary.txt as our file.
    // go through each word in file.
    char* current = nextWord(file);
    while(current != NULL){
        hashMapPut(map, current, 1);
        // Need to free since malloc'd in nextWord.
        free(current);
        current = nextWord(file);
    }
    free(current);
}

/**
 * this helper function gets a string and converts it to all lower case.
 */

void toLowerCase(char* word){
    // Use ASCII values.
    for(int i = 0; (word[i] != '\n' && word[i] != 0); i++){
        if(word[i] <= 'Z' && word[i] >= 'A'){
            word[i] += 32;
        }
    }
}

/**
 * minimum of three integer helper funciton
 * for lev distance.
 */

int minimum(int num1, int num2, int num3){
    int min = num1;
    if(num2 < min){
        min = num2;
    }
    if(num3 < min){
        min = num3;
    }

    return min;
}

/**
 * Levenshtein Distance
 * SOURCE: wikipedia
 * @param two strings as char ptrs
 * returns desired "distance" (pretty much number of
 * edits between two strings.)
 */

int levDistance(char* word1, char* word2){

    // Using "full matrix" iterative method.
    int length1 = strlen(word1);
    int length2 = strlen(word2);
    int matrix[length1 + 1][length2 + 1];

    // Initialize.
    // (from wikipedia: "source prefixes can be transformed into
    // empty string by dropping all characters").
    for(int i = 0; i <= length1; i++){
        matrix[i][0] = i;
    }
    // (from wikipedia: "target prefixes can be reached from empty
    // source prefix by inserting every character").
    for(int j = 0; j <= length2; j++){
        matrix[0][j] = j;
    }

    // declare COST vars.
    int substitute;
    int insert;
    int delete;

    for(int i = 1; i <= length1; i++){

        for(int j = 1; j <= length2; j++){

            if(word1[i-1] == word2[j-1]) {
                // Case for identical letters
                matrix[i][j] = matrix[i-1][j-1];
            }else {
                delete = matrix[i-1][j] + 1;
                insert = matrix[i][j-1] + 1;
                substitute = matrix[i-1][j-1] + 1;

                matrix[i][j] = minimum(delete, insert, substitute);
            }
        }
    }

    return matrix[length1][length2];
}

/**
 * Checks the spelling of the word provided by the user. If the word is spelled incorrectly,
 * print the 5 closest words as determined by a metric like the Levenshtein distance.
 * Otherwise, indicate that the provided word is spelled correctly. Use dictionary.txt to
 * create the dictionary.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    HashMap* map = hashMapNew(1000);

    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    char inputBuffer[256];
    int quit = 0;
    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);

        /**
         * Use my toLower helper function on inputBuffer.
         */
        toLowerCase(inputBuffer);

        if (strcmp(inputBuffer, "quit") == 0)
        {
            quit = 1;
        }

        /**
        * My code starts here.
        */

        if(hashMapContainsKey(map, inputBuffer)){
            printf("The word \"%s\" is spelled correctly.\n", inputBuffer);
        }else{
            printf("The word \"%s\" is spelled incorrectly.\n", inputBuffer);
            printf("Did you mean any of the following words? Please choose by typing again.\n");

            // now find the similar words.
            int words = 0;
            HashLink* traverse = NULL;

            // Traverse the dictionary.
            // Need multiple for-loops for correct minimum distance.
            for(int i = 0; i < map->capacity; i++){
                traverse = map->table[i];
                while(traverse != NULL){
                    hashMapPut(map, traverse->key, levDistance(inputBuffer, traverse->key));
                    if(words > 5){
                        break;
                    }
                    if(levDistance(inputBuffer, traverse->key) == 1 && words < 5){
                        words++;
                        printf("Word %d: %s\n", words, traverse->key);
                    }
                    traverse = traverse->next;
                }
            }

            for(int i = 0; i < map->capacity; i++){
                traverse = map->table[i];
                while(traverse != NULL){
                    hashMapPut(map, traverse->key, levDistance(inputBuffer, traverse->key));
                    if(words > 5){
                        break;
                    }
                    if(levDistance(inputBuffer, traverse->key) == 2 && words < 5){
                        words++;
                        printf("Word %d: %s\n", words, traverse->key);
                    }
                    traverse = traverse->next;
                }
            }

            for(int i = 0; i < map->capacity; i++){
                traverse = map->table[i];
                while(traverse != NULL){
                    hashMapPut(map, traverse->key, levDistance(inputBuffer, traverse->key));
                    if(words > 5){
                        break;
                    }
                    if(levDistance(inputBuffer, traverse->key) == 3 && words < 5){
                        words++;
                        printf("Word %d: %s\n", words, traverse->key);
                    }
                    traverse = traverse->next;
                }
            }
        }
        /**
        * My code ends here.
        */
    }

    hashMapDelete(map);
    return 0;
}
