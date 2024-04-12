#ifndef INVENTORY_H
#define INVENTORY_H

// Inventory of an individual subject
struct Inventory {
    char* name;
    char* items[MAX_ENTRY];
    int quantities[MAX_ENTRY];
    int num_of_items;
    char* location;
};

int num_inventories = 0;
struct Inventory inventories[MAX_ENTRY];

// creating inventory for a subject. If the inventory already exists, then do nothing
void create_inventory(char* name) {
    for (int i = 0; i < num_inventories; i++) {
        if (strcmp(inventories[i].name, name) == 0) {
            return;
        }
    }
    inventories[num_inventories].name = (char *) malloc(strlen(name) + 1);
    strcpy(inventories[num_inventories].name, name);
    inventories[num_inventories].num_of_items = 0;
    inventories[num_inventories].location = (char *) malloc(strlen("NOWHERE"));
    strcpy(inventories[num_inventories].location, "NOWHERE");
    num_inventories++;
}

// create item in an inventory if item is not in that inventory
void create_item(struct Inventory* inv, char* item, int quantity) {
    inv->items[inv->num_of_items] = (char *) malloc(strlen(item) + 1);
    strcpy(inv->items[inv->num_of_items], item);
    inv->quantities[inv->num_of_items] = quantity;
    inv->num_of_items++;
}

// finding the inventory of a specific subject
struct Inventory* find_inventory(char *name) {
    for (int i = 0; i < num_inventories; i++) {
        if (strcmp(inventories[i].name, name) == 0) {
            return &inventories[i];
        }
    }
    return NULL;
}

// find specifit item in the specific inventory
int find_item(struct Inventory* inv, char* item) {
    for (int i = 0; i < inv->num_of_items; i++) {
        if (strcmp(inv->items[i], item) == 0) {
            return i;
        }
    }
    return -1;
}

// set location of a subject
void set_location(char *subjects[], int num_subjects, char* location) {
    for (int i = 0; i < num_subjects; i++) {
        struct Inventory* inv = find_inventory(subjects[i]);
        inv->location = (char *) malloc(strlen(location) + 1);
        strcpy(inv->location, location);
    }
}

// buying from infinite source. No need to check if the item is available
void buy_item(char *subjects[], char *items[], int num_subjects, int num_items) {
    for (int i = 0; i < num_subjects; i++) {
        struct Inventory* inv = find_inventory(subjects[i]);
        for (int j = 0; j < num_items; j++) {
            char* temp_item = (char *) malloc(strlen(items[j]) + 1);
            strcpy(temp_item, items[j]);
            char* tokens[2];
            tokens[0] = strtok(temp_item, " ");
            tokens[1] = strtok(NULL, " ");
            int item_index = find_item(inv, tokens[1]);

            if (item_index != -1)
                inv->quantities[item_index] += atoi(tokens[0]);
            else
                create_item(inv, tokens[1], atoi(tokens[0]));

            free(temp_item);
        }
    }
}

// Buying item(s) from a specific seller. Need to check if the seller has enough items.
void buy_from(char *subjects[], char *items[], int num_subjects, int num_items, char* subject) {
    int subject_can_sell = 0;
    struct Inventory* inv = find_inventory(subject);
    // checking if the seller has enough items to sell
    for (int i = 0; i < num_items; i++) {
        int total_sell = 0;
        char* temp_item = (char *) malloc(strlen(items[i]) + 1);
        strcpy(temp_item, items[i]);
        char* tokens[2];
        tokens[0] = strtok(temp_item, " ");
        tokens[1] = strtok(NULL, " ");

        for (int j = 0; j < num_subjects; j++) {
            total_sell += atoi(tokens[0]);
        }

        int item_index = find_item(inv, tokens[1]);
        if (item_index != -1 && inv->quantities[item_index] >= total_sell) {
            subject_can_sell++;
        }
        free(temp_item);
    }
    // if the seller has enough items to sell, then subjects buy items and seller sells items
    if (subject_can_sell == num_items) {
        buy_item(subjects, items, num_subjects, num_items);
        
        for (int i = 0; i < num_items; i++) {
            char* temp_item = (char *) malloc(strlen(items[i]) + 1);
            strcpy(temp_item, items[i]);
            char* tokens[2];
            tokens[0] = strtok(temp_item, " ");
            tokens[1] = strtok(NULL, " ");
            int item_index = find_item(inv, tokens[1]);
            if (item_index != -1) {
                inv->quantities[item_index] -= atoi(tokens[0]) * num_subjects;
            }
            free(temp_item);
        }
    }
}

// selling to infinite source. Need to check if the subjects has enough items.
int sell_item(char *subjects[], char *items[], int num_subjects, int num_items) {
    int subjects_can_sell = 0;
    
    // checking if all subjects have enough items to sell
    for (int i = 0; i < num_subjects; i++) {
        struct Inventory* inv = find_inventory(subjects[i]);

        for (int j = 0; j < num_items; j++) {
            char* temp_item = (char *) malloc(strlen(items[j]) + 1);
            strcpy(temp_item, items[j]);
            char* tokens[2];
            tokens[0] = strtok(temp_item, " ");
            tokens[1] = strtok(NULL, " ");

            int item_index = find_item(inv, tokens[1]);
            if (item_index != -1 && inv->quantities[item_index] >= atoi(tokens[0])) {
                subjects_can_sell++;
            }
            free(temp_item);
        }
    }

    // if all subjects have enough items to sell, then sell the items
    if (subjects_can_sell == num_subjects * num_items) {
        for (int i = 0; i < num_subjects; i++) {
            struct Inventory* inv = find_inventory(subjects[i]);
            for (int j = 0; j < num_items; j++) {
                char* temp_item = (char *) malloc(strlen(items[j]) + 1);
                strcpy(temp_item, items[j]);
                char* tokens[2];
                tokens[0] = strtok(temp_item, " ");
                tokens[1] = strtok(NULL, " ");

                int item_index = find_item(inv, tokens[1]);
                if (item_index != -1) {
                    inv->quantities[item_index] -= atoi(tokens[0]);
                }
                free(temp_item);
            }
        }
    }
    return subjects_can_sell;
}

// selling item(s) to a specific buyer. Need to check if the subjects has enough items.
void sell_to(char *subjects[], char *items[], int num_subjects, int num_items, char* subject) {
    int subjects_can_sell = sell_item(subjects, items, num_subjects, num_items);
    if (subjects_can_sell == num_subjects * num_items) {
        struct Inventory* inv = find_inventory(subject);
        for (int i = 0; i < num_items; i++) {
            char* temp_item = (char *) malloc(strlen(items[i]) + 1);
            strcpy(temp_item, items[i]);
            char* tokens[2];
            tokens[0] = strtok(temp_item, " ");
            tokens[1] = strtok(NULL, " ");

            int item_index = find_item(inv, tokens[1]);

            if (item_index != -1)
                inv->quantities[item_index] += atoi(tokens[0]) * num_subjects;
            else
                create_item(inv, tokens[1], atoi(tokens[0]) * num_subjects);

            free(temp_item);
        }
    }
}

// check if all subjects are at the specified location
int check_location(char* subjects[], int num_subjects, char* location) {
    int all_at_location = 0;
    for (int i = 0; i < num_subjects; i++) {
        struct Inventory* inv = find_inventory(subjects[i]);

        if (strcmp(inv->location, location) == 0)
            all_at_location++;
    }

    if (all_at_location == num_subjects)
        return 1;
    return 0;
}

// check if the subject have exactly the specified amounts of items
int check_has(char* subjects[], int num_subjects, char* items[], int num_items) {
    int subjects_has = 0;
    
    for (int i = 0; i < num_subjects; i++) {
        struct Inventory* inv = find_inventory(subjects[i]);

        for (int j = 0; j < num_items; j++) {
            char* temp_item = (char *) malloc(strlen(items[j]) + 1);
            strcpy(temp_item, items[j]);
            char* tokens[2];
            tokens[0] = strtok(temp_item, " ");
            tokens[1] = strtok(NULL, " ");

            int item_index = find_item(inv, tokens[1]);
            if (item_index != -1 && inv->quantities[item_index] == atoi(tokens[0]))
                subjects_has++;

            else if (item_index == -1 && atoi(tokens[0]) == 0)
                subjects_has++;

            free(temp_item);
        }
    }

    if (subjects_has == num_subjects * num_items)
        return 1;
    return 0;
}

// check if the subject have less than the specified amounts of items
int check_less_than(char* subjects[], int num_subjects, char* items[], int num_items) {
    int subjects_has_less_than = 0;
    
    for (int i = 0; i < num_subjects; i++) {
        struct Inventory* inv = find_inventory(subjects[i]);

        for (int j = 0; j < num_items; j++) {
            char* temp_item = (char *) malloc(strlen(items[j]) + 1);
            strcpy(temp_item, items[j]);
            char* tokens[2];
            tokens[0] = strtok(temp_item, " ");
            tokens[1] = strtok(NULL, " ");

            int item_index = find_item(inv, tokens[1]);
            if (item_index != -1 && inv->quantities[item_index] < atoi(tokens[0]))
                subjects_has_less_than++;
            if (item_index == -1 && atoi(tokens[0]) != 0)
                subjects_has_less_than++;

            free(temp_item);
        }
    }
    if (subjects_has_less_than == num_subjects * num_items)
        return 1;
    return 0;
}

// check if the subject have more than the specified amounts of items
int check_more_than(char* subjects[], int num_subjects, char* items[], int num_items) {
    int subjects_has_more_than = 0;
    
    for (int i = 0; i < num_subjects; i++) {
        struct Inventory* inv = find_inventory(subjects[i]);

        for (int j = 0; j < num_items; j++) {
            char* temp_item = (char *) malloc(strlen(items[j]) + 1);
            strcpy(temp_item, items[j]);
            char* tokens[2];
            tokens[0] = strtok(temp_item, " ");
            tokens[1] = strtok(NULL, " ");

            int item_index = find_item(inv, tokens[1]);
            if (item_index != -1 && inv->quantities[item_index] > atoi(tokens[0]))
                subjects_has_more_than++;

            free(temp_item);
        }
    }
    if (subjects_has_more_than == num_subjects * num_items)
        return 1;
    return 0;
}

// return total quantity of the specified items in the specified subjects
int total(char* subjects[], int num_subjects, char* item) {
    int total_items = 0;
    for (int i = 0; i < num_subjects; i++) {
        struct Inventory* inv = find_inventory(subjects[i]);
        int item_index = find_item(inv, item);
        if (item_index != -1) {
            total_items += inv->quantities[item_index];
        }
    }
    return total_items;
}

// return the location of the subject
char *get_location(char *subject) {
    struct Inventory* inv = find_inventory(subject);
    if (inv == NULL)
        return "NOWHERE";
    return inv->location;
}

// return the names of the subjects that are at the specified location
char** presence(char* location) {
    char** subjects = (char **) malloc(MAX_ENTRY * sizeof(char *));
    int num_subjects = 0;
    for (int i = 0; i < num_inventories; i++) {
        if (strcmp(inventories[i].location, location) == 0) {
            subjects[num_subjects] = (char *) malloc(strlen(inventories[i].name) + 1);
            strcpy(subjects[num_subjects], inventories[i].name);
            num_subjects++;
        }
    }
    char size[20];
    snprintf(size, 20, "%d", num_subjects);
    subjects[MAX_ENTRY - 1] = (char *) malloc(strlen(size) + 1);
    strcpy(subjects[MAX_ENTRY - 1], size);
    return subjects;
}

// return the names and the quantity of the items that exist in the inventory of the subject
char** inventory_inquiry(char* name) {
    char** items = (char **) malloc(MAX_ENTRY * sizeof(char *));
    struct Inventory* inv = find_inventory(name);
    int j = 0;
    for (int i = 0; i < inv->num_of_items; i++) {
        if (inv->quantities[i] != 0) {
            char quantity[20];
            snprintf(quantity, 20, "%d", inv->quantities[i]);
            items[j] = (char *) malloc(strlen(inv->items[i]) + 21);
            strcpy(items[j], quantity);
            strcat(items[j], " ");
            strcat(items[j], inv->items[i]);
            j++;
        }
    }
    char size[20];
    snprintf(size, 20, "%d", j);
    items[MAX_ENTRY - 1] = (char *) malloc(strlen(size) + 1);
    strcpy(items[MAX_ENTRY - 1], size);
    return items;
}

#endif