#ifndef FUNC_H_INCLUDED
#define FUNC_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

#define NUM_ORDERS 20

typedef struct {
    int ORDERS[NUM_ORDERS];
    float price;
}Table;

typedef struct {
    int cdg;
    float price;
    char name[91];
}Product;

void configure_file(FILE *config);
FILE* exist_config();
void create_stock(FILE *stock);
FILE* exist_stock();
void print_title(FILE *config);
Table* allocate_tables(FILE *config);
void start_orders(Table* tables, int num_tables);
void show_orders(Table* tables, int table, FILE *stock);
void add_orders(Table* tables, int table);
void remove_orders(Table* tables, int table, int cdg);
void change_tables(Table* tables, int table1, int table2);
void free_tables(Table* tables);
int num_tables(FILE* config);
void show_products(FILE* stock);
void close_table(Table* tables, int table, FILE* stock);
void getch();
int check_limit(int table,int qnt_tables);
void option_error();
void clean_screen();
void show_menu(FILE *config);
FILE* generate_receipt(Table* tables, int table);

#endif // FUNC_H_INCLUDED
