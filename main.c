#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define HASHTABLE_SIZE 11
FILE * fp2;
FILE * fp3;

typedef struct node{
  int val;
  char name[20];
  int threshold;
  int stock;
  float price;
  struct node *next;
}node_t;

typedef struct g_item{  //grocery_item is a struct nested in c_node. grocery_item is a linked list that contains a customer's grocery list
  int KeyofItem;
  char NameofItem[30];
  float PriceofItem;
  int NumberofItem;
  struct g_item* g_next; //grocery_item*next is a pointer to the next grocery item in the list
}g_item;


typedef struct c_node{ //c_node is a struct nested in c_queue. c_node is a customer node. It holds the NameofClient clients Balance
  float Balance;
  char NameofClient[30];
  struct g_item* g_head;
  struct c_node*c_next;
}c_node;

int search_list (node_t *head, node_t **previous, node_t **current, int data) { //searches chained hash lists to find item nodes
  int found = 0;
  *previous = NULL;
  *current = head;
  while (*current != NULL && !found) {
    if ((*current)->val == data){
      found = 1;
    }
    else {
      *previous = *current;
      *current = (*current)->next;
    }
  }
  return(found); 
}

void delete_node(node_t** head, int val, node_t**hashtable, int location){//Deletes an item from the hash table
  node_t* previous;
  node_t* current;
  int found;

  found = search_list(*head, &previous, &current, val);

  if (found) {
    if (previous == NULL){
      *head = current->next;
      hashtable[location]=NULL;
    }
    else {
      previous->next = current->next;
      free(current);
    }
  }
}

void restock_node(node_t** head, int val, int num){//restocks an item in the hash table by a given restock number
  node_t* previous;
  node_t* current;
  int found;

  found = search_list(*head, &previous, &current, val);

  if (found) {
    current->stock = (current->stock) + num;
  }
  else{
    printf("ITEM NOT FOUND IN INVENTORY");
  }
}

void decrement_node(node_t** head, int val, int num){//decrement the stock of an item in the hash table by a given number
  node_t* previous;
  node_t* current;
  int found;

  found = search_list(*head, &previous, &current, val);

  if (found) {
    current->stock = (current->stock) - num;
  }
  else{
    printf("ITEM NOT FOUND IN INVENTORY");
  }
}

void insert_new_node(node_t** head, int val, char* name, int threshold, int stock, float price){//inserts a new item into the hash table
  node_t* new_node = (node_t*)malloc(sizeof(node_t));
  new_node->val = val;
  new_node->next = NULL;
  strcpy(new_node->name, name);
  new_node->threshold = threshold;
  new_node->stock = stock;
  new_node->price = price;

  if(*head == NULL){
    *head = new_node;
  }
  else{
    node_t* ptr =*head;

    while(ptr->next != NULL){
      ptr = ptr->next;
    }

    ptr->next = new_node;
  }
}

int get_location(int val){//finds the hash value of a key value
  return(val % HASHTABLE_SIZE);
}

void insert_into_table(node_t** hashtable, int val, char* name, int threshold, int stock, float price){//inserts a given node into the hash table. Calls on the function insert_new_node
  int location = get_location(val);

  if (hashtable[location]==NULL){
    hashtable[location] = (node_t*)malloc(sizeof(node_t));

    node_t* head = NULL;
    insert_new_node(&head, val, name, threshold, stock, price);

    hashtable[location] = head;
  }
  else{
    node_t* head = hashtable[location];
    insert_new_node(&head, val, name, threshold, stock, price);
    hashtable[location] = head;
  }
}

void delete_from_table(node_t** hashtable, int val){//deletes an item from the hashtable. Calls on the function delete_node
  int location = get_location(val);
  node_t* head = hashtable[location];

  if(head == NULL){
    printf("Location is empty");
  }
  else{
    if(head == NULL){
      printf("Location is empty");
    }
    else{
      delete_node(&head, val, hashtable, location);
    }
  }
}

void restock_item(node_t** hashtable, int val, int num){//restocks an item in the hashtable. Calls on the function restock_node
  int location = get_location(val);
  node_t* head = hashtable[location];

  if(head == NULL){
    printf("Location is empty");
  }
  else{
    restock_node(&head, val, num);
  }

}

void decrement_item(node_t** hashtable, int val, int num){//decrements items in the hashtable. Calls on the function decrement_node
  int location = get_location(val);
  node_t* head = hashtable[location];

  if(head == NULL){
    printf("We do not carry item %d\n", val);
  }
  else{
    decrement_node(&head, val, num);
  }

}

void restock_all(node_t** hashtable){//restocks all items in the hashtable. Calls on the function restock_node
  for(int i=0; i < HASHTABLE_SIZE; i++){
    node_t* head = hashtable[i];
    

    if(head == NULL){
    }
    else{
      
      while(head != NULL){
        if(head->stock < head->threshold){
          restock_node(&head, head->val, head->threshold);
        }
        head=head->next;
      }
    }
  }

}

void check_stock(node_t** hashtable){//checks stock of items in the hashtable and returns a warning if they need to be replenished
  for(int i=0; i < HASHTABLE_SIZE; i++){
    node_t* head = hashtable[i];
    

    if(head == NULL){
    }
    else{
      
      while(head != NULL){
        if(head->stock < head->threshold){
          printf("\nWarning!\n %d (%s ): %d remain in stock, replenishment threshold is %d\n", head->val, head->name, head->stock, head->threshold);
        }
        head=head->next;
      }
    }
  }

}

node_t** get_hashtable(){//allocates memory for the hashtable
  node_t** hashtable = calloc(HASHTABLE_SIZE, sizeof(node_t*));
  return(hashtable);
}

void populate_table(node_t** hashtable, char* filename){//populates hashtable with items from an inventory file
  FILE *fp;
  int c;
  int i=0;
  int counter=0;
  int key_temp;
  char name_temp[20];
  int threshold_temp;
  int stock_temp;
  float price_temp;

  char temp_word[25];

  fp=fopen(filename,"r");
  if(fp==NULL){
    perror("Error in opening file");
  }

  while((c=fgetc(fp)) != EOF){
    
    if(c==44 || c == 10){
      i=0;
      if(counter == 0){
        key_temp=atoi(temp_word);
        counter++;
      }
      else if(counter == 1){
        strcpy(name_temp,temp_word);
        counter++;
      }
      else if(counter == 2){
        threshold_temp =atoi(temp_word);
        counter++;
      }
      else if(counter == 3){
        stock_temp = atoi(temp_word);
        counter++;
      }
      else{
        price_temp=atof(temp_word);
        counter++;
      }
      memset(temp_word,0,strlen(temp_word));
      
    }
    else{
      if(counter == 5){
        insert_into_table(hashtable, key_temp, name_temp, threshold_temp, stock_temp, price_temp);
        counter=0;
      }
      if( c== 123 || c==125 || c==44 || c==34)
      {}
      else{
        temp_word[i]=c;
        temp_word[i+1]=0;
        i++;
      }
    }
  }

  insert_into_table(hashtable, key_temp, name_temp, threshold_temp, stock_temp, price_temp);
  fclose(fp);
}

void display_table(node_t** hashtable){//displays the current state of the hashtable
  for(int i = 0; i < HASHTABLE_SIZE; i++){
    node_t * head = hashtable[i];

    printf("%d: ", i);

    if (head == NULL){
      printf("NULL");
    }
    else{
      node_t*ptr = head;
      while (ptr != NULL){
        printf("{%d, %s, %d, %d, %.2f} ", ptr->val, ptr->name, ptr->threshold, ptr->stock, ptr->price);
        ptr = ptr->next;
      }
    }
    printf("\n");
  }
}

void write_inventory(node_t** hashtable){//writes current state of the hashtable to an output file
  fp3 = fopen("inventory.txt", "w");
  for(int i = 0; i < HASHTABLE_SIZE; i++){
    node_t * head = hashtable[i];

    if (head == NULL){
    }
    else{
      node_t*ptr = head;
      while (ptr != NULL){
        fprintf(fp3,"{%d, %s, %d, %d, %.2f}\n", ptr->val, ptr->name, ptr->threshold, ptr->stock, ptr->price);
        ptr = ptr->next;
      }
    }
  }
  fclose(fp3);
}
///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


c_node * get_cnode (char*c_name, float c_balance) {
  c_node * ptr;                            /* declare a list node pointer */
  ptr = (c_node *)malloc(sizeof(c_node));  /* dynamically allocate the list node from the heap */
  if (ptr != NULL) {                       /* check to be sure the new node was allocated successfully */
    strcpy(ptr->NameofClient, c_name);
    ptr->Balance = c_balance;                      /* initialize the new node data to data */
    ptr->c_next = NULL;                      /* initialize the new node next to NULL */

  }
  return(ptr);                             /* return a pointer to the new node */
}

void insert_customer(c_node**c_head, char*c_name, float c_balance){ //inserts a customer node into the customer queue
  c_node* ptr = NULL;
  c_node* temp_node = get_cnode(c_name, c_balance);

  if (temp_node == NULL) {              /* if the heap is full, exit the program */
    printf("\ninsert_node - list node creation failed\nExiting...\n");
    exit(1);
  }
  if(*c_head == NULL){
    *c_head = temp_node;
  }
  else{
    ptr = *c_head;

    while(ptr->c_next != NULL){
      ptr = ptr->c_next;
    }
    ptr->c_next = temp_node;
  }
}

void insert_gitem(c_node**c_head, int g_key, int g_quant){//insert item nodes into the customer struct
  g_item* ptr = NULL;

  g_item* temp_node = (g_item*)malloc(sizeof(g_item));
  temp_node->KeyofItem=g_key;
  if(g_key == 101){
    strcpy(temp_node->NameofItem, "Carrots");
    temp_node->PriceofItem = 1.99;
  }
  else if(g_key == 102){
    strcpy(temp_node->NameofItem, "Apples");
    temp_node->PriceofItem = 0.99;
  }
  else if(g_key == 216){
    strcpy(temp_node->NameofItem, "Swiss Cheese");
    temp_node->PriceofItem = 2.49;
  }
  else if(g_key == 39){
    strcpy(temp_node->NameofItem, "Wine");
    temp_node->PriceofItem = 12.99;
  }
  else if(g_key == 006){
    strcpy(temp_node->NameofItem, "Coffee");
    temp_node->PriceofItem = 5.00;
  }
  else{
    strcpy(temp_node->NameofItem, "Unknown Item");
    temp_node->PriceofItem = 0.00;
  }
  temp_node->NumberofItem = g_quant;
  

  
  if((*c_head)->g_head == NULL){
    ((*c_head)->g_head) = temp_node;
  }
  else{
    ptr = (*c_head)->g_head;
    while(ptr->g_next != NULL){
      ptr = ptr->g_next;
    }
    ptr->g_next = temp_node;
  }
}

void traverse_glist(g_item* g_head){//traverses and prints the grocery list in a given customer node
  g_item* ptr = g_head;
  if (ptr == NULL){
    printf("List is Empty\n");
  }
  else{
    while(ptr != NULL){
      printf("%s, %f, %d -> ", ptr->NameofItem, ptr->PriceofItem, ptr->NumberofItem);
      ptr = ptr->g_next;
    }
  }
}

void traverse_cqueue (c_node *c_head) {//traverses the customer queue and prints their grocery lists
  c_node *ptr = c_head;
  if (ptr == NULL){
    printf("Queue is Empty\n");
  }
  else {
    while(ptr != NULL){
      printf("\n%s, %f\n",ptr->NameofClient, ptr->Balance);
      traverse_glist(ptr->g_head);
      ptr = ptr->c_next;
    }
  }
}

float sumGrocery(g_item* g_head){//sums their grocery list
  g_item*ptr = g_head;
  float sum=0;
  if (ptr == NULL){
    printf("List is Empty\n");
  }
  else{
    while(ptr != NULL){
      sum = sum + (ptr->NumberofItem * ptr->PriceofItem);
      ptr = ptr->g_next;
    }
  }
  return(sum);
}

void printReceipt(c_node* cust, float sum, node_t** hashtable){//prints the receipts of the customers after checkout
  fprintf(fp2, "Customer - %s\n\n\n", cust->NameofClient);

  g_item* ptr = cust->g_head;
  if (ptr == NULL){
    printf("List is Empty\n");
  }
  else{
    if(sum < cust->Balance){
      while(ptr != NULL){
        fprintf(fp2, "%s x%d @ $%.2f\n", ptr->NameofItem, ptr->NumberofItem, ptr->PriceofItem);
        decrement_item(hashtable, ptr->KeyofItem, ptr->NumberofItem);
        ptr = ptr->g_next;
      }
      fprintf(fp2, "\n\nTotal: $%.2f", sum);
      fprintf(fp2, "\nThank you come back soon!\n");
      fprintf(fp2,"----------------------------------------------------------\n\n\n");
    }

    else{
      while(ptr != NULL){
        fprintf(fp2, "%s x%d @ $%.2f\n", ptr->NameofItem, ptr->NumberofItem, ptr->PriceofItem);
        ptr = ptr->g_next;
      }
      fprintf(fp2, "\n\nTotal: $%.2f", sum);
      fprintf(fp2, "\nThank you come back soon!\n");
      fprintf(fp2, "\nCustomer did not have enough money and was REJECTED\nThank you come back soon!\n");
      fprintf(fp2, "-----------------------------------------------------------\n\n\n");
    }
  }

}

void processCustomer( c_node* c_head, node_t** hashtable){//process the customers at the checkout and updates the inventory hashtable. Calls on the printReceipt function
  c_node *ptr = c_head;
  if (ptr == NULL){
    printf("Queue is Empty\n");
  }
  else{

    fp2 = fopen("Text_Receipt.txt", "w");
    while(ptr !=NULL){
      float grocery_sum = sumGrocery(ptr->g_head);
      printReceipt(ptr, grocery_sum, hashtable);
      ptr = ptr->c_next;
    }
    fclose(fp2);
  }
}



c_node* populate_customers(char *);
c_node* populate_groceries(char *, c_node* head);

int main(void) {
  node_t** table =get_hashtable();
  populate_table(table, "inventory.txt");


  //int menu=0;
  char menu[20];

  while(strncmp(menu, "quit", sizeof("quit")-1)!=0){
    int key;
    char grocery_name[20];
    char submenu[15];
    int threshold;
    int quantity;
    float price;

    printf("\nWhat operation do you want to conduct:\n1.Checkout Queue of Customers(checkout test.txt)\n2.Manage Inventory Submenu(inventory)\n3.Close Program(quit)\n>>");
    fgets(menu, 20, stdin);

    if(strncmp(menu, "checkout test.txt", sizeof("checkout test.txt")-1)==0){
      c_node* c_head = populate_customers("test.txt");
      c_head = populate_groceries("test.txt", c_head);
      processCustomer(c_head, table);
      check_stock(table);

    }
    else if(strncmp(menu, "inventory", sizeof("inventory")-1)==0){
      while(strncmp(submenu, "return", sizeof("return")-1)!=0){
        printf("\nWhat inventory management operation do you want to conduct:\n1.Add items (add)\n2.Delete items (delete)\n3.Restock an item (restock item)\n4.Restock all items under threshold (restock all)\n5.Return to main menu (return)\n>>");
        fgets(submenu, 15, stdin);

        if(strncmp(submenu, "add", sizeof("add")-1)==0){
          printf("Enter item key:");
          scanf("%d", &key);
          printf("Enter item threshold:");
          scanf("%d", &threshold);
          printf("Enter item stock:");
          scanf("%d", &quantity);
          printf("Enter item price:");
          scanf("%f", &price);
          getchar();
          printf("Enter item name:");
          fgets(grocery_name, 20, stdin);
          int len_grocery_name = strlen(grocery_name);
          if(grocery_name[len_grocery_name-1]=='\n'){
            grocery_name[len_grocery_name-1]=0;
          }
          insert_into_table(table, key, grocery_name, threshold, quantity, price);
        }
        else if(strncmp(submenu, "delete", sizeof("delete")-1)==0){
          int val;
          printf("Enter item key to delete:");
          scanf("%d", &val);
          getchar();
          delete_from_table(table, val);
        }
        else if(strncmp(submenu, "restock item", sizeof("restock item")-1)==0){
          int val, num;
          printf("Enter item key to restock:");
          scanf("%d", &val);
          printf("Enter how much to restock:");
          scanf("%d", &num);
          getchar();
          restock_item(table, val, num);
        }
        else if(strncmp(submenu, "restock all", sizeof("restock all")-1)==0){
          printf("Restocking all items below threshold......\n");
          restock_all(table);
        }
        else{
          if(strncmp(submenu, "return", sizeof("return")-1) != 0)
            printf("INVALID INSTRUCTION. Enter valid submenu instruction\n");
        }
      }
    }
    else{
      if(strncmp(menu, "quit", sizeof("quit")-1) != 0){
        printf("ENTER VALID MENU INSTRUCTION. Enter valid menu instruction\n");
      }
    }
  }
  printf("Writing current inventory to inventory.txt, shutting down...\n");
  write_inventory(table);
  return 0;
}

c_node* populate_customers(char *filename){//populate customers to the checkout queue
  FILE *fp;
  int c;
  int i=0;
  int counter=0;
  char name_temp[20];
  float balance_temp;
  c_node* c_head = NULL;
  char temp_word[25];

  fp=fopen(filename,"r");
  if(fp==NULL){
    perror("Error in opening file");
  }

  while((c=fgetc(fp)) != EOF){

      if(c == 44){
        i=0;
        if(counter == 0){
          strcpy(name_temp, temp_word);
          counter++;
        }
        else if(counter == 1){
          balance_temp = atof(temp_word);
          counter++;
        }
      }
      else{
        if(c == 10){
          insert_customer(&c_head, name_temp, balance_temp);
          counter=0;
        }
        if(c== 123 || c== 125 || c==44 || c==34 || c == 10)
        {}
        else{
          temp_word[i]=c;
          temp_word[i+1]=0;
          i++;
        }
      }
  }

  return(c_head);
}

c_node* populate_groceries(char *filename, c_node* c_head){//populates each customer node with their respective groceries
  FILE *fp;
  int check =0;
  int c;
  int i=0;
  int counter=0;
  int key_temp;
  float price_temp;
  c_node* ptr = c_head;
  char temp_word[25];

  fp=fopen(filename,"r");
  if(fp==NULL){
    perror("Error in opening file");
  }

  while((c=fgetc(fp)) != EOF){

    if(c == 91 || check == 1){
      check = 1;

      if(c == 44 || c == 93){
        i=0;
        if(counter == 0){
          key_temp = atoi(temp_word);
          counter++;
        }
        else if(counter == 1){
          price_temp = atof(temp_word);
          counter++;
        }
        
      }
      else{
        if(counter == 2){
          insert_gitem(&ptr, key_temp, price_temp);
          counter = 0;
        }
        if(c == 10){
          check=0;
          //increment customer here
          if(ptr->c_next != NULL){
            ptr = ptr->c_next;
          }
          //printf("\nNext customer\n");
        }
        if(c== 123 || c== 125 || c==44 || c==34 || c == 10 || c == 91 || c == 93)
        {}
        else{
          temp_word[i]=c;
          temp_word[i+1]=0;
          i++;
        }
      }
    }
  }

  return(c_head);
}