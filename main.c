/* COP 3502C Assignment 6
   This program is written by: Jalil Fountain */

   #include <stdio.h>
   #include <stdlib.h>
   #include <string.h>
   
   
   #define MAXSTLEN 29
   #define TABLESIZE 300007


typedef struct merchandise {
    char name[MAXSTLEN + 1]; // e.g., "braziljersey"
    int quantity; // stock available
    int price; // sale price per item
} merchandise;
    
typedef struct inventoryNode {
    merchandise* merchPtr;
    struct inventoryNode* next;
} inventoryNode;
    
typedef struct inventoryTable { //Hash table
    inventoryNode** lists;
    int size; // number of buckets (table size)
} inventoryTable;
   
   
   int totalCash = 100000;
   int totalComplexity = 0;
   
   
   int hashfunction(char* word, int size) {
       int len = strlen(word);
       int hvalue = 0;
       for (int i = 0; i < len; i++)
           hvalue = (1151 * hvalue + (word[i] - 'a')) % size;
       return hvalue;
   }
   

inventoryTable* createInventoryTable() {
    inventoryTable* table = malloc(sizeof(inventoryTable)); // allocate memory for the inventory table 
    table->lists = malloc(TABLESIZE * sizeof(inventoryNode*)); // allocate memory for the array of linked list 
    for (int i = 0; i < TABLESIZE; i++)
        table->lists[i] = NULL; // initialize each bucket to NULL 
    table->size = TABLESIZE;  // set the size to the table size 
    return table; 
}


inventoryNode* findNode(inventoryTable* table, char* name, int* complexity) {
    int idx = hashfunction(name, table->size); // find the hash index for the items name
    inventoryNode* curr = table->lists[idx]; // start at the head of the linked list in that bucket
    *complexity = 0; 
    while (curr) { // traverse the list
        (*complexity)++; // increment complexity for each node checked
        if (strcmp(curr->merchPtr->name, name) == 0) // if the name matches return the node
            return curr; 
        curr = curr->next; 
    }
    return NULL; // item not found
}


void buyMerch(inventoryTable* table, char* name, int qty, int price) {
    int idx = hashfunction(name, table->size); // find hash index
    int complexity = 0;
    inventoryNode* node = findNode(table, name, &complexity); // try to find the item

    if (node) {
        node->merchPtr->quantity += qty; // if item exists add to the quantity
    } else {
        merchandise* m = malloc(sizeof(merchandise)); // create new merchandise
        strcpy(m->name, name); 
        m->quantity = qty;  
        m->price = 0; 

        inventoryNode* newNode = malloc(sizeof(inventoryNode)); // create new node for linked list
        newNode->merchPtr = m; // link merchandise
        newNode->next = table->lists[idx]; // point to current head
        table->lists[idx] = newNode; // insert at front
        complexity = complexity + 1; // update for insertion complexity
    }

    totalCash -= price; // subtract from total cash
    totalComplexity += complexity; // update total complexity 

    node = findNode(table, name, &complexity); // get the node to print its current quantity
    printf("%s %d %d\n", name, node->merchPtr->quantity, totalCash); 
}


void sellMerch(inventoryTable* table, char* name, int qty) {
    int complexity;
    inventoryNode* node = findNode(table, name, &complexity); // find the item in the table

    int sellQty;
    if (qty > node->merchPtr->quantity) { // if trying to sell more than available
        sellQty = node->merchPtr->quantity; // sell whatever is left
    } else {
        sellQty = qty; 
    }

    totalCash += sellQty * node->merchPtr->price; // add to cash total based on sale
    node->merchPtr->quantity -= sellQty; // subtract sold quantity from inventory
    totalComplexity += complexity; // update complexity

    printf("%s %d %d\n", name, node->merchPtr->quantity, totalCash); 
}


void changePrice(inventoryTable* table, char* name, int new_price) {
    int complexity;
    inventoryNode* node = findNode(table, name, &complexity); // find the item
    node->merchPtr->price = new_price; // update its price
    totalComplexity += complexity; // update complexity
}


void freeTable(inventoryTable* table) {
    for (int i = 0; i < table->size; i++) { // loop through each bucket
        inventoryNode* curr = table->lists[i]; // start at head of linked list
        while (curr) { // free each node in the list
            inventoryNode* temp = curr;
            curr = curr->next;
            free(temp->merchPtr); // free the merchandise
            free(temp); // free the node
        }
    }
    free(table->lists); // free the list array
    free(table); // free the table 
}


int main() {
    int n;
    scanf("%d", &n); // read number of commands

    inventoryTable* table = createInventoryTable(); // initialize hash table

    for (int i = 0; i < n; i++) {
        char command[20], name[MAXSTLEN + 1];
        int qty, totalprice, new_price;

        scanf("%s", command); 

        if (strcmp(command, "buy") == 0) { //if the command is buy thenn buy merch
            scanf("%s %d %d", name, &qty, &totalprice); 
            buyMerch(table, name, qty, totalprice); 
        } else if (strcmp(command, "sell") == 0) { //sell the merch 
            scanf("%s %d", name, &qty); 
            sellMerch(table, name, qty); // process sale
        } else if (strcmp(command, "change_price") == 0) { //change the price
            scanf("%s %d", name, &new_price); 
            changePrice(table, name, new_price); 
        }
    }

    printf("%d\n%d\n", totalCash, totalComplexity); // output final cash and complexity totals
    freeTable(table); 
    return 0;
}