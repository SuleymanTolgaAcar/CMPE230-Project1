#ifndef __EXECUTE_H__
#define __EXECUTE_H__

int execute_operation(struct Operation* op) {
    // Create inventories for all subjects included in the operation
    for (int i = 0; i < op->num_subjects; i++) {
        create_inventory(op->subjects[i]);
    }
    // Execute action type operations
    if (op->type == 'a') {
        if (strcmp(op->name, "buy") == 0) {
            buy_item(op->subjects, op->items, op->num_subjects, op->num_items);
        }
        if (strcmp(op->name, "buy from") == 0) {
            create_inventory(op->subject);
            buy_from(op->subjects, op->items, op->num_subjects, op->num_items, op->subject);
        }
        if (strcmp(op->name, "sell") == 0) {
            sell_item(op->subjects, op->items, op->num_subjects, op->num_items);
        }
        if (strcmp(op->name, "sell to") == 0) {
            create_inventory(op->subject);
            sell_to(op->subjects, op->items, op->num_subjects, op->num_items, op->subject);
        }
        if (strcmp(op->name, "go to") == 0) {
            set_location(op->subjects, op->num_subjects, op->location);
        }
    }
    // Evaluate condition type operations
    if (op->type == 'c') {
        if (strcmp(op->name, "at") == 0) {
            return check_location(op->subjects, op->num_subjects, op->location);
        }
        if (strcmp(op->name, "has") == 0) {
            return check_has(op->subjects, op->num_subjects, op->items, op->num_items);
        }
        if (strcmp(op->name, "has less") == 0) {
            return check_less_than(op->subjects, op->num_subjects, op->items, op->num_items);
        }
        if (strcmp(op->name, "has more") == 0) {
            return check_more_than(op->subjects, op->num_subjects, op->items, op->num_items);
        }
    }
    // Execute query type operations
    if (op->type == 'q') {
        if (strcmp(op->name, "total") == 0) {
            printf("%d\n", total(op->subjects, op->num_subjects, op->item));
        }
        if (strcmp(op->name, "where") == 0) {
            printf("%s\n", get_location(op->subject));
        }
        if (strcmp(op->name, "who at") == 0) {
            char **subjects = presence(op->location);
            int size = atoi(subjects[MAX_ENTRY - 1]);
            
            if (size == 0)
                printf("NOBODY\n");
            else {
                for (int i = 0; i < size; i++) {
                    if (i == size - 1)
                        printf("%s\n", subjects[i]);
                    else
                        printf("%s and ", subjects[i]);
                }
            }
        }
        if (strcmp(op->name, "inventory") == 0) {
            create_inventory(op->subject);
            char** items = inventory_inquiry(op->subject);
            int size = atoi(items[MAX_ENTRY - 1]);
            if (size == 0)
                printf("NOTHING\n");
            else {
                for (int i = 0; i < size; i++) {
                    if (i == size - 1)
                        printf("%s\n", items[i]);
                    else
                        printf("%s and ", items[i]);
                }
            }
        }
    }
    return 1;
}

// Print all inventories, for debugging purposes
void print_inventory() {
    for (int i = 0; i < num_inventories; i++) {
        printf("\n");
        printf("Name: %s\n", inventories[i].name);
        printf("Location: %s\n", inventories[i].location);
        for (int j = 0; j < inventories[i].num_of_items; j++) {
            printf("Item: %s %d\n", inventories[i].items[j], inventories[i].quantities[j]);
        }
    }
}

// Execute all operations in the ops array
void execute_operations(struct Operation* ops, int num_ops) {
    int l, r;
    // 0 means not executing, 1 means executing in this array
    int executing_ops[num_ops];
    // Initialize all operations to not executing
    for (int i = 0; i < num_ops; i++){
        executing_ops[i] = 0;
    }
    // Used a two pointer approach to find which operations to execute
    for (l = 0, r = 1; r < num_ops; r++) {
        if (ops[r].type == 'c'){
            int result = execute_operation(&ops[r]);
            if (result == 0){
                l = r + 1;
            }
        }
        else if (ops[r].type == 'a'){
            if (ops[r - 1].type == 'c'){
                for (int i = l; i < r; i++){
                    if (ops[i].type == 'a'){
                        executing_ops[i] = 1;
                    }
                }
                l = r;
            }
        }
    }
    // Mark all remaining operations to be executed
    for (int i = l; i < num_ops; i++){
        if (ops[i].type == 'a'){
            executing_ops[i] = 1;
        }
    }
    // Execute all operations that are marked to be executed
    for (int i = 0; i < num_ops; i++){
        if (executing_ops[i] == 1){
            execute_operation(&ops[i]);
        }
    }
}

#endif