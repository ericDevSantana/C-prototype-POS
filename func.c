#include "func.h"

void configure_file(FILE *config){
    char name[51];
    char phone[13];
    int number_tables;

	//Your company's info that goes into config.bin file

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

	//if config.bin doesn't exist, create a new one

    if(config == NULL) {
        config = fopen("config.bin","wb+");
        if(config == NULL) {
            printf("Impossible to create configuration file.\n");
            exit(1);
        }
        else {
            configure_file(config);
            system("cls");
            return config;
        }
    }
    return config;
}

void create_stock(FILE *stock){
    Product p;
    int leave = 0;

	// Your stock's info that goes into config.bin file
	// Loop until press 1

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

	//if stock.bin doesn't exist, create a new one

    if(stock == NULL) {
        stock = fopen("stock.bin","wb+");
        if(stock == NULL) {
            printf("Impossible to create stock's file.\n");
            exit(1);
        }
        else {
            create_stock(stock);
            return stock;
        }
    }
    return stock;
}

void print_title(FILE *config){
    char name[51];
    char phone[13];
    int num_tables;
	
	//Return pointer to the begining of config.bin file and show on the screen
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
	
	//Offset 64 Bytes to get the number of tables in the config.bin file
    rewind(config);
    fseek(config,64,SEEK_CUR);
    fread(&num_tables,sizeof(int),1,config);

	//Allocate a number X of tables (Type Table)
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
	
	//Initializing all tables with 0 to prevent garbage values
    for(i=0;i<num_tables;i++){
        (tables+num_tables)->price = 0;
    }
}

void show_orders(Table* tables, int table, FILE* stock){
    int i,j,tam;
    Product p;

    clean_screen();

	//Know how many products (Type Product) there are in the stock.bin
    fseek(stock,0,SEEK_END);
    tam = ftell(stock)/sizeof(Product);
    fseek(stock,0,SEEK_SET);

    printf("\t\tOrders Table[%d]\n\n",table);
    for(i=0;i<NUM_ORDERS;i++){
        j = 0;
        rewind(stock);
        if((tables+table)->ORDERS[i]){
            printf("[%d] - ",(tables+table)->ORDERS[i]);

			//Table stores product's codes so here we search for the code to match with any existent product and show on the screen
            do {
                fread(&p,sizeof(Product),1,stock);
                if(p.cdg == (tables+table)->ORDERS[i])
                    printf("%s\n",p.name);
                j++;
            }while(j != tam);
        }
    }
    printf("\tTotal: %.2f\n",(tables+table)->price);

}

void add_orders(Table* tables, int table){
    int i,flag,cdg;
    Product p;
    FILE* stock;

	//Open stock.bin to read and show on the screen the orders available to add in the table
    stock = fopen("stock.bin","rb");

    show_products(stock);

    printf("\tProduct's code: ");
    scanf("%d",&cdg);

    rewind(stock);
    i = 0;

	//Verify is the code entered is in fact an existent product, else variable FLAG gets 0 to prevent from adding something in the table's total
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

	//If the code entered is a product, add the price in the table's total and add the code to Order's vector of that table
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

	//Open stock.bin file to read and show all orders
    stock = fopen("stock.bin","rb");

	//Verify if there is the order code in the order's vector
    do {
        fread(&p,sizeof(Product),1,stock);
        if (p.cdg == cdg){
            flag = 1;
            break;
        }
        if (feof(stock)){
            clean_screen();
            printf("\t\tThis product doesn't exist.\n");
            getch();
            flag = 0;
            break;
        }
    }while(1);

	//Remove product from the Order's vector and decrease the price from the total of the table
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
	
	//Change Table pointers --arithmetic
    aux = *(tables+table1);
    *(tables+table1) = *(tables+table2);
    *(tables+table2) = aux;
}

void free_tables(Table* tables){

	//Free memory allocated for tables
    free(tables);
}

int num_tables(FILE* config){
    int qnt_tables;

	//Offset 64 bytes to find how many tables there are in the system -- Return number of tables
    rewind(config);
    fseek(config,64,SEEK_CUR);
    fread(&qnt_tables,sizeof(int),1,config);

    return qnt_tables;
}

void show_products(FILE* stock){
    int i,tam;
    Product p;

	//Know how many products there are in the stock.bin and show it in the ADD ORDER screen
    fseek(stock,0,SEEK_END);
    tam = ftell(stock)/sizeof(Product);
    fseek(stock,0,SEEK_SET);

    i = 0;

    printf("\t\tOptions: \n\n");
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

	//Return the pointer to the receipt created and save it in the folder Receipts
	receipt = generate_receipt(tables,table);

	//Count how many products there are in the stock.bin file
	fseek(stock,0,SEEK_END);
    tam = ftell(stock)/sizeof(Product);
    fseek(stock,0,SEEK_SET);

    for(i=0;i<NUM_ORDERS;i++){
		j=0;
		rewind(stock);
        if((tables+table)->ORDERS[i]){

			//Search all orders of the table in the stock.bin file to print in the sales receipt
			do {
                fread(&p,sizeof(Product),1,stock);
                if(p.cdg == (tables+table)->ORDERS[i]){
                    ///fputs(p.name,receipt);
					fprintf(receipt,"%-30.30s %-10.2f",p.name,p.price);
					fputs("\n",receipt);
				}
                j++;
            }while(j != tam);

			//Reset table's Total to U$00
            (tables+table)->ORDERS[i] = 0.00;
        }
    }

	//Body format of sales receipt
	fputs("\n",receipt);
	fputs("TOTAL: ",receipt);
	fprintf(receipt,"%29.2f\n\n",(tables+table)->price);
	fprintf(receipt,"%24.28s\n","Thank you for your business!");

    printf("\t\tClosing table %d.\n\n",table);
    printf("Total: %.2f\n",(tables+table)->price);
    printf("\n");
    (tables+table)->price = 0.00;
	fclose(receipt);
}

void getch(){

	//Wait for any key -- pause
    system("pause");
}

int check_limit(int table,int qnt_tables){

	//Additional function to verify if tables exist within the system
    if((table >= 0) && (table < qnt_tables)){
        return 1;
    } else {
        printf("\t\tThere is no table %d.\n\n",table);
        getch();
        clean_screen();
        return 0;
    }
}

void option_error(){
    clean_screen();

	//Additional procedure in case default at main screen
    printf("\t\tOption doesn't exist.\n\n");
    getch();
    clean_screen();
}

void clean_screen(){
	//clean screen
    system("cls");
}

void show_menu(FILE *config){

	//Always print Title and Menu when come back to main screen
	print_title(config);
    printf(".________________________________. \n");
    printf("| 1 - Add orders to table.       | \n");
    printf("| 2 - Verify table order.        | \n");
    printf("| 3 - Remove table order.        | \n");
    printf("| 4 - Change tables.             | \n");
    printf("| 5 - Close table.               | \n");
    printf("| 0 - End program.               | \n");
    printf("|________________________________|\n\n");
    printf("Enter option number: ");
}

FILE* generate_receipt(Table* tables, int table){
	FILE* receipt;

	//Time struct to get Date and Time from the system//
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);

	char name[21];
	char n_table[3];

	//Concatenate and generate receipts names and create the receipt.txt file to write the receipt info
	sprintf(n_table,"%d",table);
	strcpy(name,"Receipt/receiptF_0");
	strcat(name,n_table);
	strcat(name,".txt");
	receipt = fopen(name,"w+");

	//Format and show Date and Time of the system in the receipt

	//fprintf(receipt,"Date: %.2d/%.2d/%d\nTime: %.2d:%.2d:%d\n\n",tm.tm_mday, tm.tm_mon + 1,  tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
	fprintf(receipt, "Date and Time: %s", asctime(tm));
	fprintf(receipt,"%24.15s\n","RECEIPT");
	fputs("_____________________________________\n",receipt);
	fprintf(receipt,"%10.15s %26.6s\n","ITEM","PRICE");
	fputs("_____________________________________\n",receipt);

	return receipt;
}

















