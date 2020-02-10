#include "func.h"

void configure_file(FILE *config){
    char name[51];
    char phone[13];
    int number_tables;

	/*ENTRADA DE DADOS DA EMPRESA E ESCRITA NO ARQUIVO CONFIG BIN*/
    printf("Enter company's name: ");
    fgets(name,51,stdin);
    fwrite(name,sizeof(char),51,config);
    printf("Enter company's phone number: ");
    fgets(phone,13,stdin);
    fwrite(phone,sizeof(char),13,config);
    printf("Enter how many tables: ");
    scanf("%d",&number_tables);
    fwrite(&number_tables,sizeof(int),1,config);

}

FILE* exist_config(){
    FILE *config;
    config = fopen("config.bin","rb");

	/*CASO NÃO ESTEJA CRIADO, CRIAR ARQUIVO CONFIG BIN E CONFIGURAR*/
    if(config == NULL) {
        config = fopen("config.bin","wb+");
        if(config == NULL) {
            printf("Impossible to create configuration file.\n");
            exit(1);
        }
        else {
            ///printf("Nao existia arquivo.\n");
            configure_file(config);
            system("cls");
            return config;
        }
    }
    ///printf("Agora existe arquivo.\n");
    return config;
}

void create_stock(FILE *stock){
    Product p;
    int leave = 0;

	/*ENTRADA DE DADOS PARA O ARQUIVO DE stock E GRAVAÇÃO EM stock BIN*/
    printf("\t\tStock creation.\n\n");
    while(!leave) {
        printf("Enter product code: ");
        scanf("%d",&p.cdg);
        printf("Enter product name: ");
        scanf(" %[^\n]s",p.name);
        printf("Enter product price: ");
        scanf("%f",&p.price);
        fwrite(&p,sizeof(Product),1,stock);
        printf("Press 0 to enter another product or press 1 to leave: ");
        scanf("%d",&leave);
    }
    fclose(stock);

}

FILE* exist_stock(){
    FILE *stock;
    stock = fopen("stock.bin","rb");

	/*CASO NÃO ESTEJA CRIADO, CRIAR ARQUIVO stock BIN E PREENCHER*/
    if(stock == NULL) {
        stock = fopen("stock.bin","wb+");
        if(stock == NULL) {
            printf("Impossible to create stock's file.\n");
            exit(1);
        }
        else {
            ///printf("Nao existia arquivo.\n");
            create_stock(stock);
            return stock;
        }
    }
    ///printf("Agora existe arquivo.\n");
    return stock;
}

void print_title(FILE *config){
    char name[51];
    char phone[13];
    int num_tables;
	
	/*VOLTA PONTEIRO PARA INCIO DO ARQUIVO CONFIG BIN E MOSTRA NA TELA*/
    rewind(config);

    fread(name,sizeof(char),51,config);
    fread(phone,sizeof(char),13,config);
    fread(&num_tables,sizeof(int),1,config);

    printf("Company: %s",name);
    printf("Telephone: %s\n",phone);
    printf("Numbers of tables: %d\n",num_tables);
    printf("________________________________________________________________________________\n\n");
}

Table* allocate_tables(FILE *config){
    Table *tables;
    int num_tables;
	
	/*O VALOR 64 BYTES É EXATAMENTE ONDE FICA O NUMERO DE tables NO ARQUIVO CONFIG BIN*/
    rewind(config);
    fseek(config,64,SEEK_CUR);
    fread(&num_tables,sizeof(int),1,config);

    /*ALOCAÇÃO DE N tables DISPONÍVEIS NA EMPRESA----TIPO Table PERSONALIZADO FUNC.H*/
    tables = (Table*)calloc(num_tables,sizeof(Table));

    if(!tables){
        printf("Impossible to allocate tables.\n");
        exit(1);
    }
	
    start_orders(tables,num_tables);

    return tables;
}

void start_orders(Table* tables, int num_tables){
    int i;
	
	/*ZERANDO TODOS OS PREÇOS FINAIS DAS tables PARA GARANTIR QUE NÃO TENHA LIXO*/
    for(i=0;i<num_tables;i++){
        (tables+num_tables)->price = 0;
    }
}

void show_orders(Table* tables, int table, FILE* stock){
    int i,j,tam;
    Product p;

    clean_screen();

	/*SABER QUANTOS ProductS ----TIPO Product FUNC.H -----*/
    fseek(stock,0,SEEK_END);
    tam = ftell(stock)/sizeof(Product);
    fseek(stock,0,SEEK_SET);

    printf("\t\torders Table[%d]\n\n",table);
    for(i=0;i<NUM_ORDERS;i++){
        j = 0;
        rewind(stock);
        if((tables+table)->ORDERS[i]){
            printf("[%d] - ",(tables+table)->ORDERS[i]);
			/*Table ARMAZENA CODIGO, Product TEM CODIGO, AQUI PROCURA O CODIGO DA Table ATÉ BATER COM O CODIGO DO Product E MOSTRA*/
            do {
                fread(&p,sizeof(Product),1,stock);
                if(p.cdg == (tables+table)->ORDERS[i])
                    printf("%s\n",p.name);
                j++;
            }while(j != tam);
        }
    }
    printf("\tTotal amount: %.2f\n",(tables+table)->price);

}

void add_orders(Table* tables, int table){
    int i,flag,cdg;
    Product p;
    FILE* stock;

	/*ABRIR NOVAMENTE ARQUIVO stock PARA LEITURA E MOSTRAR NA TELA PARA O RESPONSÁVEL ADICIONAR O PEDIDO CORRETO*/
    stock = fopen("stock.bin","rb");

    show_products(stock);

    printf("\tProduct's code: ");
    scanf("%d",&cdg);

    rewind(stock);
    i = 0;
	/*VERIFICA SE O CODIGO DIGITADO REALMENTE É UM Product SENÃO SAI COM FLAG = 0 PARA N DEIXAR EXECUTAR O INCREMENTO NO PREÇO*/
    do {
        fread(&p,sizeof(Product),1,stock);
        if (p.cdg == cdg){
            flag = 1;
            break;
        }
        if (feof(stock)){
            clean_screen();
            printf("\t\tProduct doesn't exist.\n");
            getch();
            flag = 0;
            break;
        }
    }while(1);

    if((tables+table)->ORDERS[NUM_ORDERS-1]){
        clean_screen();
        printf("\t\tNo more orders.\n");
        getch();
        flag = 0;
    }
	/*CASO O CODIGO DIGITADO É UM Product, ELE É INCREMENTADO NO PREÇO FINAL DA Table E ADICIONADO AO VETOR DE orders*/
    if(flag){
        for(i=0;i<NUM_ORDERS;i++){
            if(!(tables+table)->ORDERS[i]){
                (tables+table)->ORDERS[i] = cdg;
                (tables+table)->price += p.price;
                break;
            }
        }
    }
    clean_screen();
    fclose(stock);
}

void remove_orders(Table* tables, int table, int cdg){
    int i;
    int flag = 0;
    Product p;
    FILE* stock;

	/*ABRIR ARQUIVO stock PARA LER E MOSTRAR orders*/
    stock = fopen("stock.bin","rb");

	/*VERIFICAR SE EXISTE O PEDIDO NA Table*/
    do {
        fread(&p,sizeof(Product),1,stock);
        if (p.cdg == cdg){
            flag = 1;
            break;
        }
        if (feof(stock)){
            clean_screen();
            printf("\t\tNão existe este Product no stock.\n");
            getch();
            flag = 0;
            break;
        }
    }while(1);

	/*RETIRAR Product DO VETOR DE orders E DECREMENTAR O PREÇO FINAL DA Table*/
    if(flag){
        for(i=0;i<NUM_ORDERS;i++){
            if((tables+table)->ORDERS[i] == cdg){
                (tables+table)->ORDERS[i] = 0;
                (tables+table)->price -= p.price;
                clean_screen();
                printf("\t\tProduct removed.\n");
                getch();
                break;
            }
        }
    }
    fclose(stock);
}

void change_tables(Table* tables, int table1, int table2){
    Table aux;
	
	/*change DE PONTEIROS*/
    aux = *(tables+table1);
    *(tables+table1) = *(tables+table2);
    *(tables+table2) = aux;
}

void free_tables(Table* tables){

	/*free MEMORIA ALOCADA*/
    free(tables);
}

int num_tables(FILE* config){
    int qnt_tables;

	/*POSIÇÃO 64 BYTES INDICA QUANTAS tables TEM E RETORNA PARA USO EM OUTRAS FUNÇÕES*/
    rewind(config);
    fseek(config,64,SEEK_CUR);
    fread(&qnt_tables,sizeof(int),1,config);

    return qnt_tables;
}

void show_products(FILE* stock){
    int i,tam;
    Product p;

	/*SABER QUANTOS ProductS TEM EM stock E MOSTRAR NA TELA DE ADICIONAR orders*/
    fseek(stock,0,SEEK_END);
    tam = ftell(stock)/sizeof(Product);
    fseek(stock,0,SEEK_SET);

    i = 0;

    printf("\t\tOpcoes: \n\n");
    do {
        fread(&p,sizeof(Product),1,stock);
        printf("%d - ",p.cdg);
        printf("%s\n",p.name);
        i++;
    }while(i != tam);
}

void close_table(Table* tables, int table, FILE* stock){
    FILE* receipt;
	int i,j,tam;
	Product p;

	/*RETORNA PONTEIRO PARA receipt FISCAL E SALVAR COMO TXT NA PASTA receiptS*/
	receipt = generate_receipt(tables,table);

	/*CONTAR QUANTOS ProductS EXISTEM NO stock BIN*/
	fseek(stock,0,SEEK_END);
    tam = ftell(stock)/sizeof(Product);
    fseek(stock,0,SEEK_SET);

    for(i=0;i<NUM_ORDERS;i++){
		j=0;
		rewind(stock);
        if((tables+table)->ORDERS[i]){
			/*AQUI PROCURA NO ARQUIVO stock TODOS OS orders DA Table PARA IMPRIMIR NA receipt FISCAL*/
			do {
                fread(&p,sizeof(Product),1,stock);
                if(p.cdg == (tables+table)->ORDERS[i]){
                    ///fputs(p.name,receipt);
					fprintf(receipt,"%-30.30s %-10.2f",p.name,p.price);
					fputs("\n",receipt);
				}
                j++;
            }while(j != tam);
			/*ZERAR O PREÇO FINAL DA Table*/
            (tables+table)->ORDERS[i] = 0.00;
        }
    }

	/*FORMATAÇÃO DO CORPO DA receipt FISCAL*/
	fputs("\n",receipt);
	fputs("TOTAL: ",receipt);
	fprintf(receipt,"%29.2f\n\n",(tables+table)->price);
	fprintf(receipt,"%24.15s\n","volte sempre!");

    printf("\t\tEncerrando Table %d.\n\n",table);
    printf("Valor Total: %.2f\n",(tables+table)->price);
    printf("\n");
    (tables+table)->price = 0.00;
	fclose(receipt);
}

void getch(){

	/*SISTEMA ESPERA QUALQUER TECLA*/
    system("pause");
}

int check_limit(int table,int qnt_tables){

	/*FUNÇÃO AUXILIAR PARA VERIFICAR SE AS tables EM QUESTÃO EXISTEM*/
    if((table >= 0) && (table < qnt_tables)){
        return 1;
    } else {
        printf("\t\tNao existe Table %d.\n\n",table);
        getch();
        clean_screen();
        return 0;
    }
}

void option_error(){
    clean_screen();
	/*FUNÇÃO AUXILIAR PARA CASO DEFAULT DE SWITCH CASE*/
    printf("\t\tNao existe essa option no menu.\n\n");
    getch();
    clean_screen();
}

void clean_screen(){
	/*LIMPAR TELA*/
    system("cls");
}

void show_menu(FILE *config){
	/*SEMPRE IMPRIMIR O CABEÇALHO E O MENU AO VOLTAR PARA A TELA PRINCIPAL*/
	print_title(config);
    printf(".________________________________. \n");
    printf("| 1 - Adicionar orders em Table. | \n");
    printf("| 2 - Verificar orders de Table. | \n");
    printf("| 3 - Remover orders de Table.   | \n");
    printf("| 4 - changer tables.              | \n");
    printf("| 5 - Fechar conta de Table.      | \n");
    printf("| 0 - Finalizar o programa.      | \n");
    printf("|________________________________|\n\n");
    printf("Entre com a option desejada: ");
}

FILE* generate_receipt(Table* tables, int table){
	FILE* receipt;

	/*ESTRUTURAS PARA ARMAZENAR A DATA E HORA DO SISTEMA TM*/
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	char name[21];
	char n_table[3];

	/*CONCATENAÇÃO DOS nameS DAS receiptS FISCAIS DE CADA Table DENTRO DA PASTA receiptS*/
	sprintf(n_table,"%d",table);
	strcpy(name,"Receipt/receiptF_0");
	strcat(name,n_table);
	strcat(name,".txt");
    receipt = fopen(name,"w+");

	/*FORMATAÇÃO E MOSTRAR DATA E HORA DO SISTEMA NA HORA DO PREENCHIMENTO DA receipt FISCAL*/
	fprintf(receipt,"Data: %.2d/%.2d/%d\nHora: %.2d:%.2d:%.2d\n\n",tm.tm_mday, tm.tm_mon + 1,  tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
	fprintf(receipt,"%24.15s\n","CUPOM FISCAL");
	fputs("_____________________________________\n",receipt);
	fprintf(receipt,"%10.15s %26.6s\n","ITEM","PREÇO");
	fputs("_____________________________________\n",receipt);

	return receipt;
}

















