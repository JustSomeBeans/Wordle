#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ANSI color codes for boxed in letters
#define GREEN "\e[38;2;255;255;255;1m\e[48;2;106;170;100;1m"
#define YELLOW "\e[38;2;255;255;255;1m\e[48;2;201;180;88;1m"
#define RED "\e[38;2;255;255;255;1m\e[48;2;220;20;60;1m"
#define RESET "\e[0;39m"

struct ColorArrays {
    int greenArray[16];
    int compareArray[16];
    int duplicates;
    int duplicatesBeforeComparison;
    bool contains;
    bool arraysAreDifferent;
    bool greenArrayIsBlank;
    bool comparisonSafe;
};

char *getRandomWord(int wordLength) {
    char filename[10];
    sprintf(filename, "%d.txt", wordLength); // setting file to open as one with a list of n letter words

    FILE *file = fopen(filename, "r"); // open in read mode
    if (file == NULL) {
        perror("Error opening word list file");
        exit(1);
    }

    char word[16];

    char *chosenWord = NULL;
    int wordCount = 0;

    // Count the number of words in the file
    while (fgets(word, sizeof(word), file) != NULL) {
        wordCount++;
    }

    // Reset the file pointer to the beginning of the file
    fseek(file, 0, SEEK_SET);

    // Choose a random word
    int selectedWordIndex = rand() % wordCount;
    int currentIndex = 0;

    while (fgets(word, sizeof(word), file) != NULL) {
        if (currentIndex == selectedWordIndex) {
            chosenWord = malloc(strlen(word) + 1);
            strcpy(chosenWord, word);
            break;
        }
        currentIndex++;
    }

    fclose(file);
    return chosenWord;
}

int main(int argc, char *argv[]) {
    if (argc != 2) { // one command line argument only
        printf("Usage: ./wordle wordsize\n");
        exit(1);
    }

    int wordLength = atoi(argv[1]);
    srand(time(NULL));

    char *targetWord = getRandomWord(wordLength);
    targetWord[strcspn(targetWord, "\n")] = '\0'; // null terminates the target word

    if (wordLength < 5 || wordLength > 8) { // ensures argv[1] is in the correct range
        printf("Error: wordsize must be either 5, 6, 7, or 8\n");
        exit(1);
    }

    int maxGuesses = wordLength + 1;
    int currentGuess = 0;

    char guess[atoi(argv[1]) + 1];
    guess[strcspn(guess, "\n")] = '\0';

    struct ColorArrays colorArrays;
    memset(colorArrays.compareArray, 0, sizeof(colorArrays.compareArray));
    memset(colorArrays.greenArray, 0, sizeof(colorArrays.greenArray));
    colorArrays.contains = false;
    colorArrays.arraysAreDifferent = false;
    colorArrays.greenArrayIsBlank = true;
    colorArrays.comparisonSafe = true;

    printf(GREEN "This is WORDLE50" RESET "\nYou have %d tries to guess the %d-letter word I'm thinking of\n", wordLength + 1,
           wordLength);

    while (currentGuess < maxGuesses) {
        printf("Guess %d/%d, Input a %d-letter word: ", currentGuess + 1, maxGuesses, wordLength); // collecting user input
        scanf("%s", guess);
        while (getchar() != '\n');

        if (strlen(guess) != wordLength) { // reprompt the user if they don't input an n-letter word
            continue;
        }

        for (int printLoop = 0; printLoop < wordLength; printLoop++) {

            if (guess[printLoop] == targetWord[printLoop]) { // if the character matches the character in the target word, it is correct
                printf(GREEN "%c" RESET, guess[printLoop]);
                continue;
            }
            else {
                memset(colorArrays.compareArray, 0, sizeof(colorArrays.compareArray));
                memset(colorArrays.greenArray, 0, sizeof(colorArrays.greenArray));
                colorArrays.contains = false;
                colorArrays.arraysAreDifferent = false;
                colorArrays.greenArrayIsBlank = true;
                colorArrays.comparisonSafe = true;

                for (int greenLoop = 0; greenLoop < wordLength; greenLoop++) {
                    if (guess[printLoop] == targetWord[greenLoop] && guess[greenLoop] == targetWord[greenLoop]) {
                        colorArrays.greenArray[greenLoop] = 1;
                    }
                }
                for (int compareLoop = 0; compareLoop < wordLength; compareLoop++) {
                    if (guess[printLoop] == targetWord[compareLoop]) {
                        colorArrays.compareArray[compareLoop] = 1;
                        colorArrays.contains = true;
                        colorArrays.duplicates++;
                    }
                }
                for (int differenceLoop = 0; differenceLoop < wordLength; differenceLoop++) {
                    if (colorArrays.compareArray[differenceLoop] != colorArrays.greenArray[differenceLoop]) {
                        colorArrays.arraysAreDifferent = true;
                        break;
                    }
                }
                if (colorArrays.contains && colorArrays.arraysAreDifferent) {
                    if (colorArrays.duplicates > 1) {
                        for (int dupeLoop = 0; dupeLoop < printLoop; dupeLoop++) {
                            if (guess[printLoop] == guess[dupeLoop]) {
                                colorArrays.duplicatesBeforeComparison++;
                            }
                            if (colorArrays.duplicatesBeforeComparison + 1 >= colorArrays.duplicates) {
                                colorArrays.comparisonSafe = false;
                                break;
                            }
                        }
                        if (colorArrays.comparisonSafe) {
                            printf(YELLOW "%c" RESET, guess[printLoop]);
                        }
                        else {
                            printf(RED "%c" RESET, guess[printLoop]);
                        }
                    }
                    else {
                        printf(YELLOW "%c" RESET, guess[printLoop]);
                    }
                }
                else {
                    printf(RED "%c" RESET, guess[printLoop]);
                }
            }
        }
        printf("\033[0m\n"); // Reset color
        printf("\n");

        currentGuess++;
        if (strcmp(guess, targetWord) == 0) {
            printf("You won!\n");
            break; // No need to continue if the word is correctly guessed
        }
    }
    if (currentGuess == maxGuesses) {
        printf("You've run out of guesses! The word was: %s\n", targetWord);
    }
    free(targetWord);
    return 0;
}
