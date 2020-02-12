#include "func.h"

int main()
{
    int op = 999;
    char c;
    Table *tables;
    int table,cdg;
    int table1,table2,qnt_tables;
    FILE *file_config;
    FILE *stock;

	//Variable to verify if there is a folder called Receipt
	DIR* dir = opendir("Receipt");

	//if there's no such file in the directory create a new one, read and write permission
	if(dir){
		closedir(dir);
	} else if (ENOENT == errno){
		_mkdir("Receipt");
		closedir(dir);
	}

    file_config = exist_config();
    stock = exist_stock();
    tables = allocate_tables(file_config);
    qnt_tables = num_tables(file_config);
	clean_screen();

    //MAIN LOOP
    while(op) {
        show_menu(file_config);

        //Options reading
        scanf("%d",&op);

        switch(op) {
		/*Add orders*/
        case 1:
            printf("\nTable: ");
            scanf("%d",&table);
            clean_screen();
            if(check_limit(table,qnt_tables)){
                add_orders(tables,table);
            }
            break;
		/*Verify orders*/
        case 2:
            printf("\nTable: ");
            scanf("%d",&table);
            clean_screen();
            if(check_limit(table,qnt_tables)){
                show_orders(tables,table,stock);
                getch();
                clean_screen();
            }
            break;
		/*Remove orders*/
        case 3:
            printf("\nTable: ");
            scanf("%d",&table);

            clean_screen();
            if(check_limit(table,qnt_tables)){
                show_orders(tables,table,stock);
                printf("\nProduct Code: ");
                scanf("%d",&cdg);
                remove_orders(tables,table,cdg);
            }
            clean_screen();
            break;
		/*Change tables*/
        case 4:
            printf("\nChange table: ");
            scanf("%d",&table1);
            printf("\nChange table %d to: ",table1);
            scanf("%d",&table2);
            clean_screen();
            if(check_limit(table1,qnt_tables) && check_limit(table2,qnt_tables)){
                change_tables(tables,table1,table2);
            }
            break;
		/*Close table*/
        case 5:
            printf("\nTable: ");
            scanf("%d",&table);
            clean_screen();
            if(check_limit(table,qnt_tables)){
                close_table(tables,table,stock);
                getch();
                clean_screen();
            }
            break;
		/*OUT of main loop*/
        case 0:
            break;
		/*Inexistent option*/
        default:
            option_error();
            break;
        }
    }
    fclose(file_config);
    free_tables(tables);
    /*End of main loop and desalocating tables*/
    return 0;
}
