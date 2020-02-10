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

	/*VARIAVEL PARA VERIFICAR SE EXISTE PASTA PARA AS NOTAS FISCAIS*/
	DIR* dir = opendir("Receipt");

	/*CASO NÃO TENHA, CRIAR COM PERMISSÃO PARA LER E ESCREVER 0700*/
	if(dir){
		closedir(dir);
	} else if (ENOENT == errno){
		mkdir("Receipt");
		closedir(dir);
	}

    file_config = exist_config();
    stock = exist_stock();
    tables = allocate_tables(file_config);
    qnt_tables = num_tables(file_config);
	clean_screen();

    /*LOOP PRINCIPAL DO SISTEMA*/
    while(op) {
        show_menu(file_config);
        /*LEITURA DE EVENTOS*/
        scanf("%d",&op);

        switch(op) {
		/*ADICIONA PEDIDOS*/
        case 1:
            printf("\nTable: ");
            scanf("%d",&table);
            clean_screen();
            if(check_limit(table,qnt_tables)){
                add_orders(tables,table);
            }
            break;
		/*VERIFICAR PEDIDOS*/
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
		/*REMOVER PEDIDOS*/
        case 3:
            printf("\nTable: ");
            scanf("%d",&table);

            clean_screen();
            if(check_limit(table,qnt_tables)){
                //mostra_produtos(stock);
                show_orders(tables,table,stock);
                printf("\nProduct Code: ");
                scanf("%d",&cdg);
                remove_orders(tables,table,cdg);
            }
            clean_screen();
            break;
		/*TROCAR MESAS*/
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
		/*FECHAR CONTA*/
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
		/*SAIR*/
        case 0:
            break;
		/*OPÇAO INDESEJADA*/
        default:
            option_error();
            break;
        }
    }
    fclose(file_config);
    free_tables(tables);
    /*FIM LOOP PRINCIPAL E DESALOCAÇÃO DE MESAS*/
    return 0;
}
