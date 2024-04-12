/*
Ringmaster program to parse the input and execute the operations
@authors Mehmet Ali Özdemir, Süleyman Tolga Acar
@ID 2021400000, 2021400237
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LEN 1025
#define MAX_ENTRY 256
#define MAX_TOKEN 514

// Operation struct to store either the action or the condition
struct Operation {
    char** subjects;
    int num_subjects;
    char* subject;
    char** items;
    int num_items;
    char* item;
    char type;
    char* name;
    char* location;
};

char* keywords[] = {"sell", "buy", "go", "to", "from", "and", "at", "has", "if", "less", "more", "than", "exit", "where", "total", "who", "NOBODY", "NOTHING", "NOWHERE"};
int num_keywords = 19;

#include "inventory.h"
#include "execute.h"

// Check if the token is consists of alphabets and underscores, and does not match any of the keywords
short is_valid(char* token) {
    if (token == NULL) {
        return 0;
    }
    for (int i = 0; i < num_keywords; i++) {
        if (strcmp(token, keywords[i]) == 0) {
            return 0;
        }
    }
    int i = 0;
    while (token[i] != '\0') {
        if (isalpha(token[i]) == 0 && token[i] != '_') {
            return 0;
        }
        i++;
    }
    return 1;
}

// Check if the token is consists of digits
short is_numeric(char* token) {
    if (token == NULL) {
        return 0;
    }
    int i = 0;
    while (token[i] != '\0') {
        if (isdigit(token[i]) == 0) {
            return 0;
        }
        i++;
    }
    return 1;
}

// Print the operation, for debugging purposes
void print_operation(struct Operation* op) {
    printf("--------------------\n");
    int i = 0;
    printf("subjects: \n");
    for (i = 0; i < op->num_subjects; i++) {
        printf("%d: %s,", i, op->subjects[i]);
    }
    printf("\n");
    printf("items: \n");
    i = 0;
    for (i = 0; i < op->num_items; i++) {
        printf("%d: %s,", i, op->items[i]);
    }
    printf("\n");
    printf("subject: %s\n", op->subject);
    printf("item: %s\n", op->item);
    printf("type: %c\n", op->type);
    printf("operation: %s\n", op->name);
    printf("location: %s\n", op->location);
    printf("--------------------\n");
}

// Finish the operation, reset the state, increment the number of operations and allocate memory for the next operation
void finish_operation(int *num_ops, char *state, struct Operation operations[]) {
    *num_ops += 1;
    strcpy(state, "finished");
    operations[*num_ops].subject = (char*)malloc(MAX_LEN * sizeof(char));
    operations[*num_ops].item = (char*)malloc(MAX_LEN * sizeof(char));
    operations[*num_ops].name = (char*)malloc(MAX_LEN * sizeof(char));
    operations[*num_ops].location = (char*)malloc(MAX_LEN * sizeof(char));

    operations[*num_ops].num_subjects = 0;
    operations[*num_ops].num_items = 0;

    operations[*num_ops].subjects = (char**)malloc(MAX_ENTRY * sizeof(char*));
    operations[*num_ops].items = (char**)malloc(MAX_ENTRY * sizeof(char*));

    for (int i = 0; i < MAX_ENTRY; i++) {
        operations[*num_ops].subjects[i] = (char*) malloc(MAX_LEN * sizeof(char));
        operations[*num_ops].items[i] = (char*) malloc(MAX_LEN * sizeof(char));
    }
}

// After each line of input, clear the memory allocated for the operations
void finish_sentence(int *num_ops, char *state, char* current_type, struct Operation operations[]) {
    if (strcmp(state, "invalid") == 0){
        *num_ops += 1;
    }
    for (int i = 0; i < *num_ops; i++) {
        free(operations[i].subject);
        free(operations[i].item);
        free(operations[i].name);
        free(operations[i].location);
        operations[i].type = 'a';
        for (int j = 0; j < MAX_ENTRY; j++) {
            free(operations[i].subjects[j]);
            free(operations[i].items[j]);
        }
        operations[i].num_subjects = 0;
        operations[i].num_items = 0;
    }
    *num_ops = 0;
    *current_type = 'a';
    strcpy(state, "finished");
}

// Handle the "and" token according to the state, it can be either subject, buy, sell, has, has more, has less
void handle_and(char* state, char* tokens[], int *i, struct Operation operations[], int *num_ops, char* current_type){
    // If the state is subject, then the next token should be an alphabet
    if (strcmp(state, "subject") == 0) {
        if (is_valid(tokens[*i + 1]) == 0) {
            strcpy(state, "invalid");
            return;
        }
        for (int j = 0; j < operations[*num_ops].num_subjects; j++) {
            if (strcmp(operations[*num_ops].subjects[j], tokens[*i + 1]) == 0) {
                strcpy(state, "invalid");
                return;
            }
        }
        int num_subs = operations[*num_ops].num_subjects;
        strcpy(operations[*num_ops].subjects[num_subs], tokens[*i + 1]);
        operations[*num_ops].num_subjects++;
        *i += 1;
    }
    // If the state is buy or sell, then the next token should be a number followed by an alphabet
    // Or if the next token is not numeric then finish the operation (finished the items for this operation)
    else if (strcmp(state, "buy") == 0 || strcmp(state, "sell") == 0){
        if (is_numeric(tokens[*i + 1]) == 0){
            finish_operation(num_ops, state, operations);
        }
        else {
            if (is_numeric(tokens[*i + 1]) == 0 || is_valid(tokens[*i + 2]) == 0){
                strcpy(state, "invalid");
                return;
            }
            for (int j = 0; j < operations[*num_ops].num_items; j++) {
                char *item = strdup(operations[*num_ops].items[j]); // Duplicate the string to avoid modifying the original
                char *item_number = strtok(item, " "); // Split the item by space
                char *item_name = strtok(NULL, " "); // Get the second part of the split string

                if (strcmp(item_name, tokens[*i + 2]) == 0) {
                    strcpy(state, "invalid");
                    free(item); // Free the duplicated string
                    return;
                }

                free(item); // Free the duplicated string
            }
            int num_items = operations[*num_ops].num_items;
            strcpy(operations[*num_ops].items[num_items], tokens[*i + 1]);
            strcat(operations[*num_ops].items[num_items], " ");
            strcat(operations[*num_ops].items[num_items], tokens[*i + 2]);
            operations[*num_ops].num_items++;
            *i += 2;
        }
    }
    // If the state is has, has more, or has less, then the next token should be a number followed by an alphabet
    else if (strcmp(state, "has") == 0 || strcmp(state, "has more") == 0 || strcmp(state, "has less") == 0){
        if (is_numeric(tokens[*i + 1]) == 0){
            finish_operation(num_ops, state, operations);
        }
        else {
            if (is_numeric(tokens[*i + 1]) == 0 || is_valid(tokens[*i + 2]) == 0){
                strcpy(state, "invalid");
                return;
            }
            for (int j = 0; j < operations[*num_ops].num_items; j++) {
                char *item = strdup(operations[*num_ops].items[j]); // Duplicate the string to avoid modifying the original
                char *item_number = strtok(item, " "); // Split the item by space
                char *item_name = strtok(NULL, " "); // Get the second part of the split string

                if (strcmp(item_name, tokens[*i + 2]) == 0) {
                    strcpy(state, "invalid");
                    free(item); // Free the duplicated string
                    return;
                }

                free(item); // Free the duplicated string
            }
            int num_items = operations[*num_ops].num_items;
            strcpy(operations[*num_ops].items[num_items], tokens[*i + 1]);
            strcat(operations[*num_ops].items[num_items], " ");
            strcat(operations[*num_ops].items[num_items], tokens[*i + 2]);
            operations[*num_ops].num_items++;
            *i += 2;
        }
    }
}

// Handle the buy and sell tokens, the next token should be a number followed by an alphabet
void handle_buy_sell(char* state, char* tokens[], int *i, struct Operation operations[], int *num_ops, char* current_type){
    if (is_numeric(tokens[*i + 1]) == 0 || is_valid(tokens[*i + 2]) == 0 || operations[*num_ops].type == 'c'){
        strcpy(state, "invalid");
        return;
    }
    operations[*num_ops].type = 'a';
    *current_type = 'a';
    strcpy(state, tokens[*i]);
    strcpy(operations[*num_ops].name, tokens[*i]);
    int num_items = operations[*num_ops].num_items;
    strcpy(operations[*num_ops].items[num_items], tokens[*i + 1]);
    strcat(operations[*num_ops].items[num_items], " ");
    strcat(operations[*num_ops].items[num_items], tokens[*i + 2]);
    operations[*num_ops].num_items++;
    *i += 2;
}

// Handle the from and to tokens, the next token should be an alphabet, and check if the token is appropriate for the state
void handle_from_to(char* state, char* tokens[], int *i, struct Operation operations[], int *num_ops, char* current_type){
    if ((strcmp(state, "buy") == 0 && strcmp(tokens[*i], "from") == 0) || (strcmp(state, "sell") == 0 && strcmp(tokens[*i], "to") == 0)){
        if (is_valid(tokens[*i + 1]) == 0){
            strcpy(state, "invalid");
            return;
        }
        for (int j = 0; j < operations[*num_ops].num_subjects; j++) {
            if (strcmp(operations[*num_ops].subjects[j], tokens[*i + 1]) == 0){
                strcpy(state, "invalid");
                return;
            }
        }
        strcat(operations[*num_ops].name, " ");
        strcat(operations[*num_ops].name, tokens[*i]);
        strcat(state, " ");
        strcat(state, tokens[*i]);
        strcpy(operations[*num_ops].subject, tokens[*i + 1]);
        *i += 1;
        finish_operation(num_ops, state, operations);
    }
    else {
        strcpy(state, "invalid");
    }
    
}

// Handle the go to token, the next token should be an alphabet
void handle_go_to(char* state, char* tokens[], int *i, struct Operation operations[], int *num_ops, char* current_type){
    if (tokens[*i + 1] == NULL || strcmp(tokens[*i + 1], "to") != 0 || tokens[*i + 2] == NULL || is_valid(tokens[*i + 2]) == 0 || operations[*num_ops].type == 'c'){
        strcpy(state, "invalid");
        return;
    }
    operations[*num_ops].type = 'a';
    *current_type = 'a';
    strcpy(operations[*num_ops].name, "go to");
    strcpy(state, "go to");
    strcpy(operations[*num_ops].location, tokens[*i + 2]);
    *i += 2;
    finish_operation(num_ops, state, operations);
}

// Handle the if token, if the state is not finished then finish the operation
void handle_if(char* state, char* tokens[], int *i, struct Operation operations[], int *num_ops, char* current_type){
    if (strcmp(state, "subject") == 0 || *current_type == 'c'){
        strcpy(state, "invalid");
        return;
    }
    if (strcmp(state, "finished") != 0){
        finish_operation(num_ops, state, operations);
    }
    operations[*num_ops].type = 'c';
    *current_type = 'c';
}

// Handle the at token, the next token should be an alphabet
void handle_at(char* state, char* tokens[], int *i, struct Operation operations[], int *num_ops, char* current_type){
    if (is_valid(tokens[*i + 1]) == 0 || *current_type != 'c'){
        strcpy(state, "invalid");
        return;
    }
    operations[*num_ops].type = 'c';
    strcpy(operations[*num_ops].name, "at");
    strcpy(operations[*num_ops].location, tokens[*i + 1]);
    finish_operation(num_ops, state, operations);
    *i += 1;
}

// Handle the has token, if the next token is more or less update the state and the operation accordingly
// The tokens after has, has more than, has less than should be a number followed by an alphabet
void handle_has(char* state, char* tokens[], int *i, struct Operation operations[], int *num_ops, char* current_type){
    if (*current_type != 'c'){
        strcpy(state, "invalid");
        return;
    }
    operations[*num_ops].type = 'c';
    if (strcmp(tokens[*i + 1], "more") == 0 || strcmp(tokens[*i + 1], "less") == 0){
        if (strcmp(tokens[*i + 2], "than") != 0){
            strcpy(state, "invalid");
            return;
        }
        if (is_numeric(tokens[*i + 3]) == 0 || is_valid(tokens[*i + 4]) == 0){
            strcpy(state, "invalid");
            return;
        }
        strcpy(operations[*num_ops].name, "has ");
        strcat(operations[*num_ops].name, tokens[*i + 1]);
        strcpy(state, "has ");
        strcat(state, tokens[*i + 1]);
        strcpy(operations[*num_ops].items[0], tokens[*i + 3]);
        strcat(operations[*num_ops].items[0], " ");
        strcat(operations[*num_ops].items[0], tokens[*i + 4]);
        operations[*num_ops].num_items++;
        *i += 4;
    }
    else {
        if (is_numeric(tokens[*i + 1]) == 0 || is_valid(tokens[*i + 2]) == 0){
            strcpy(state, "invalid");
            return;
        }
        strcpy(operations[*num_ops].name, "has");
        strcpy(state, "has");
        strcpy(operations[*num_ops].items[0], tokens[*i + 1]);
        strcat(operations[*num_ops].items[0], " ");
        strcat(operations[*num_ops].items[0], tokens[*i + 2]);
        operations[*num_ops].num_items++;
        *i += 2;
    }
}

// Handle the first token, if the token is an alphabet add the subject and set the state to "subject" for the "and" handler
void handle_first_token(char* state, char* tokens[], int *i, struct Operation operations[], int *num_ops, char* current_type){
    int num_subs = operations[*num_ops].num_subjects;
    if (is_valid(tokens[*i]) == 1) {
        strcpy(operations[*num_ops].subjects[num_subs], tokens[*i]);
        operations[*num_ops].num_subjects++;
        strcpy(state, "subject");
    }
    else {
        strcpy(state, "invalid");
    }
}

// Handle the questions, if the token is "where" then the next token should be a subject
// If the token is "total" then the next token should be a subject
// If the token is "who" then the next token should be "at" and the next token should be a location
// If the token is "total" then the next token should be an item and the previous tokens should be subjects
void handle_questions(char* state, char* tokens[], int *i, struct Operation operations[], int *num_ops, char* current_type){
    if (strcmp(tokens[*i - 2], "where") == 0){
        if (*i != 3 || is_valid(tokens[0]) == 0){
            strcpy(state, "invalid");
            return;
        }
        operations[*num_ops].type = 'q';
        *current_type = 'q';
        strcpy(operations[*num_ops].name, "where");
        strcpy(operations[*num_ops].subject, tokens[0]);
    }
    else if (strcmp(tokens[*i - 2], "total") == 0){
        if (*i != 3 || is_valid(tokens[0]) == 0){
            strcpy(state, "invalid");
            return;
        }
        operations[*num_ops].type = 'q';
        *current_type = 'q';
        strcpy(operations[*num_ops].name, "inventory");
        strcpy(operations[*num_ops].subject, tokens[0]);
    }
    else if (strcmp(tokens[0], "who") == 0){
        if (*i != 4 || strcmp(tokens[1], "at") != 0 || is_valid(tokens[2]) == 0 ){
            strcpy(state, "invalid");
            return;
        }
        operations[*num_ops].type = 'q';
        *current_type = 'q';
        strcpy(operations[*num_ops].name, "who at");
        strcpy(operations[*num_ops].location, tokens[2]);
    }
    else if (strcmp(tokens[*i - 3], "total") == 0){
        if (*i < 4 || is_valid(tokens[*i - 2]) == 0){
            strcpy(state, "invalid");
            return;
        }
        operations[*num_ops].type = 'q';
        *current_type = 'q';
        strcpy(operations[*num_ops].name, "total");
        strcpy(operations[*num_ops].item, tokens[*i - 2]);
        strcpy(operations[*num_ops].subjects[0], tokens[0]);
        operations[*num_ops].num_subjects++;
        for (int j = 1; j < *i - 3; j++) {
            if (strcmp(tokens[j], "and") == 0){
                if (is_valid(tokens[j + 1]) == 0){
                    strcpy(state, "invalid");
                    return;
                }
                strcpy(operations[*num_ops].subjects[operations[*num_ops].num_subjects], tokens[j + 1]);
                operations[*num_ops].num_subjects++;
            }
        }
    }
    else {
        strcpy(state, "invalid");
        return;
    }
    *num_ops += 1;
}

int main() {

    char line[MAX_LEN]; 

    while (1) {
        int i;
        char* tokens[MAX_TOKEN];
        int num_ops = 0;
        struct Operation operations[MAX_ENTRY];
        
        printf(">> ");
        fflush(stdout);

        // Read input line
        if (fgets(line, MAX_LEN, stdin) == NULL) {
            break;
        }

        // Tokenize the input
        tokens[0] = strtok(line, " \n");
        i = 0;
        while (tokens[i] != NULL) {
            i++;
            tokens[i] = strtok(NULL, " \n");
        }

        // Check if the input is "exit"
        if (strcmp(tokens[0], "exit") == 0 && tokens[1] == NULL) {
            break;
        }

        // Allocate memory for the first operation
        operations[num_ops].subject = (char*)malloc(MAX_LEN * sizeof(char));
        operations[num_ops].item = (char*)malloc(MAX_LEN * sizeof(char));
        operations[num_ops].name = (char*)malloc(MAX_LEN * sizeof(char));
        operations[num_ops].location = (char*)malloc(MAX_LEN * sizeof(char));
        operations[num_ops].type = 'a';

        operations[num_ops].num_subjects = 0;
        operations[num_ops].num_items = 0;

        operations[num_ops].subjects = (char**)malloc(MAX_ENTRY * sizeof(char*));
        operations[num_ops].items = (char**)malloc(MAX_ENTRY * sizeof(char*));

        for (i = 0; i < MAX_ENTRY; i++) {
            operations[num_ops].subjects[i] = (char*) malloc(MAX_LEN * sizeof(char));
            operations[num_ops].items[i] = (char*) malloc(MAX_LEN * sizeof(char));
        }


        // "subject" -> expecting a subject when "and" is encountered
        // "buy" or "sell" -> expecting a number followed by an alphabet when "and" is encountered
        // "has" -> expecting a number followed by an alphabet (or "more than" or "less than" followed by a number and an alphabet) when "and" is encountered
        // "finished" -> expecting a subject to start a new operation or end the input
        // "invalid" -> the input is invalid
        char* state = (char *)malloc(50 * sizeof(char));
        // To ensure that condition operations are only valid after "if", it is set to 'c' only after "if" and set to 'a' during every action operation, when condition related keywords are encountered, it is checked if the current type is 'c'
        char current_type = 'a';
        strcpy(state, "finished");

        // Sentence can't start with "if"
        if (strcmp(tokens[0], "if") == 0) {
            strcpy(state, "invalid");
        }
        
        // Check if the input is a question, this is done by checking if the last token is "?"
        for (i = 0; tokens[i] != NULL; i++) {
            continue;
        }
        if (strcmp(tokens[i - 1], "?") == 0) {
            handle_questions(state, tokens, &i, operations, &num_ops, &current_type);
        }

        // Parse the input
        for(i = 0; current_type != 'q' && tokens[i] != NULL && strcmp(state, "invalid") != 0; i++) {
            if (strcmp(tokens[i], "if") == 0){
                handle_if(state, tokens, &i, operations, &num_ops, &current_type);
            }

            else if (strcmp(tokens[i], "and") == 0) {
                handle_and(state, tokens, &i, operations, &num_ops, &current_type);
            }

            else if (strcmp(state, "finished") == 0) {
                handle_first_token(state, tokens, &i, operations, &num_ops, &current_type);
            }

            else if (strcmp(tokens[i], "buy") == 0 || strcmp(tokens[i], "sell") == 0){
                handle_buy_sell(state, tokens, &i, operations, &num_ops, &current_type);
            }

            else if(strcmp(tokens[i], "from") == 0 || strcmp(tokens[i], "to") == 0){
                handle_from_to(state, tokens, &i, operations, &num_ops, &current_type);
            }

            else if (strcmp(tokens[i], "go") == 0) {
                handle_go_to(state, tokens, &i, operations, &num_ops, &current_type);
            }

            else if (strcmp(tokens[i], "at") == 0){
                handle_at(state, tokens, &i, operations, &num_ops, &current_type);
            }

            else if (strcmp(tokens[i], "has") == 0){
                handle_has(state, tokens, &i, operations, &num_ops, &current_type);
            }

            else {
                strcpy(state, "invalid");
            }
        }
        
        // If the state is invalid or subject then print "INVALID" (here state=="subject" means the parser is expecting a subject but the input is finished)
        if (strcmp(state, "invalid") == 0 || strcmp(state, "subject") == 0) {
            printf("INVALID\n");
        }
        else {
            // Finish the last operation if the state is not finished
            if (strcmp(state, "finished") != 0) {
                finish_operation(&num_ops, state, operations);
            }
            // Execute the operations
            if (operations[0].type == 'q'){
                execute_operation(&operations[0]);
            }
            else {
                execute_operations(operations, num_ops);
                printf("OK\n");
            }
        }
        

        // Free the memory
        finish_sentence(&num_ops, state, &current_type, operations);
        fflush(stdout);
    }

    return 0;
}