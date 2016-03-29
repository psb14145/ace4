
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#define MAX 512
#define pointer ">"

char *par[MAX];
char input[1000];
char home[MAX];
char history[20][MAX];
int historyCount=0;
int histLoop = 0;
char aliasNames[10][MAX];
char aliasCommands[10][MAX];
int aliasCounter=0;

char *inter[] = {"exit", "getpath", "setpath", "cd","history", "alias", "unalias"};

void updateHistory(char c[MAX]); /*function for updating history*/
void exit(int status); /*function for exiting the program*/

/*prints history*/
void printHistory(){
	int i;
	int j;
	if(histLoop == 0){
		for(i = 0; i< historyCount;i++){
			printf("%i %s \n",i+1,history[i]);
		}
	}
	else{
	for(j = 0; j< 20;j++){
			printf("%i %s \n",j+1,history[j]);
		}
	}

}/*end printHistory() */

void loadHistory(){		/*load history from file */

/*loads path*/
char loadPath[MAX];

strcpy(loadPath, getenv("HOME"));
strcat(loadPath, "/.hist_list"); /*gets the path for users home dir*/

	FILE *fp;
	char line[MAX];
	int i;
	for (i = 0; i < 20; i++) {
		history[i][0] = '\0';
	}

	if( (fp = fopen(loadPath, "r")) != NULL){ /*check if file exists*/

		while(1) {
			if( (fgets(line, MAX, fp)) == NULL) /*checks if there is a history entry to be added */
				break;
			else{
			updateHistory(line);/*updates history*/
}
	 	}

	 	fclose(fp);

	}
} /*ends loadHistory() */


void saveHistoryFile(){

char savePath[MAX];

strcpy(savePath, getenv("HOME"));
strcat(savePath, "/.hist_list"); /*gets the path for users home dir*/

FILE *fp;
	fp = fopen(savePath, "w+");

if (fp == NULL)/*checks to see if history file exists*/
{
printf("history file not found");
}
else{ 
 	int i = 0;
	while(strcmp(history[i], "") != 0){ 	
		fprintf(fp, "%s", history[i]);
		i++;
}

 	fclose(fp);
}
} /*ends saveHistoyFile() */


void updateHistory(char c[MAX]){   /*updates history file*/

	if(historyCount < 20) {

 		strcpy(history[historyCount], c);
 		historyCount++;

 	} else {
int i;
 		for(i=1; i<20; i++) //Array contents shifted to left.
 			strcpy(history[i-1], history[i]);

 		strcpy(history[19], c);

 	}
} /*end updateHistory()*/


void saveHistory(){ /*saves history*/

	int i;
	for(i = 19;i >=0; i-- ){
		strcpy(history[i+1],history[i]);
	}

	strcpy(history[0],input);
	historyCount++;

	if(historyCount >19){
		histLoop = 1;
		historyCount = 0;
	}
} /*ends saveHistory() */



void executeCMD(){

	int status;
	pid_t pid;
	/* fork a child process */
	pid = fork();
	
	if (pid == 0) { /* child process */
		if (execvp(par[0], par)){
			printf("%s is an Unknown Command 1\n", *par);
		exit(0);
		}
	}
	else if (pid < 0) { /* error occurred */
		printf("Fork Failed");
		exit(1);
	}
	else { /* parent process */
		/* parent will wait for the child to complete */
		while(wait(&status)!=pid);
	}	
}

void addAlias() { /* adds an alias */
	int i, j, position; 
	int inArray = 0;
	if(aliasCounter < 10) {
		for(i = 0; i < aliasCounter; i++) {
			if(strcmp(aliasNames[i], par[1]) == 0) {				
				inArray = 1;
				position = i;
				
			}
		}
		if(inArray == 1) { /*Overwrites an alias*/
			j = 2;
			while(par[j] != NULL) {
				strcpy(aliasCommands[position], "");
				strcat(aliasCommands[position], par[j]);
				//strcat(aliasCommands[position], " ");
				j++;
			}	
			printf("Overriding alias");	
		}
		if(inArray == 0) {
			printf("\naliasCounter %d", aliasCounter);
			strcpy(aliasNames[aliasCounter], par[1]);
			i = 2;
			strcpy(aliasCommands[aliasCounter], "");
			while(par[i] != NULL) {				
				strcat(aliasCommands[aliasCounter], par[i]);
				//strcat(aliasCommands[aliasCounter], " ");
				i++;
			}	
			aliasCounter++;
		}
	}
	else {
		printf("\nNo more aliases can be set");
	}
}/*ends addAlias() */

void removeAlias() { /*removes an alias*/
	int i;
	int position = 0;
	int inArray = 0;
	
	for(i = 0; i < aliasCounter; i++) {		
		if(strcmp(aliasNames[i], par[1]) == 0) {		
			inArray = 1;
			position = i;	
		}
	}
	
	if(inArray == 1) { 
		if(aliasCounter == 1) {
			strcpy(aliasNames[0], " ");
			strcpy(aliasCommands[0], " ");
		}
		else {
			i = position;
		
			for(i = i; i < aliasCounter-1; i++) {
				strcpy(aliasNames[i], aliasNames[i+1]);
				strcpy(aliasCommands[i], aliasCommands[i+1]);				
			}
			strcpy(aliasNames[aliasCounter], " ");
			strcpy(aliasCommands[aliasCounter], " ");	
		}
		aliasCounter--;	
		printf("\nalias has been removed");
	}
	else { 
		
		printf("\nno alias exists for this command");	
	}
	
}/*ends removeAlias() */

void printAliases() { /*prints aliases*/
	int i;
	if(aliasCounter == 0) { /*if there are no aliases*/
		printf("There are no aliases to display");
	}
	else {
		for(i = 0;i < aliasCounter;i++) { /*prints list of aliases next to their command*/
			printf("\n name: %s command: %s", aliasNames[i], aliasCommands[i]);
		}
	}
}/*ends printAliases()*/

void runInternal(int m){
int result;
char *promptBuff;
promptBuff = NULL;
	switch(m){
		case 0:
			strcpy(home, getenv("HOME"));
			int i = chdir(home);
			if(i<0) printf("Couldn't restore\n");
			else{
				printf("directory changed\n");
				printf("HOME: %s\n", home);
			}
			exit(0);
		case 1:
			/*if(par[1] != NULL){
					printf("%s is an Unknown command 2\n", *par);
			}
			else{*/
				printf("PATH = %s\n", getenv("PATH"));
				/*}*/
			break;
		case 2:
			if(par[1] == NULL){
					printf("%s is an Unknown command 3\n", *par);
			}
			else{
				int newPath = setenv(par[1], par[1], 0);
				printf("%d", newPath);
				/*const char* path = getenv("PATH");
				printf("PATH :%s\n",(path!=NULL)? path : "getenv returned NULL");*/
			}
			break;
		case 3:
			
			result= 0;
			if(par[1] == NULL){
				chdir("..");
			}
			else{
				result = chdir(par[1]);
				
				if(result == 0){
					printf("\nDirectory changed to %s\n", get_current_dir_name());
				}	
				else{
					switch(errno){
					case EACCES: perror("Permission denied");
						break;
					case EIO:	 perror("");
						break;
					case ENAMETOOLONG: perror("");
						break;
					case ENOTDIR: perror(""); 
						break;
					case ENOENT: perror(""); 
      						break;
					}
				}
				
			}
			break;
		case 4:
			printHistory();			
			break;
		case 5: if(par[1] == NULL || par[2] == NULL) {
				printAliases();
			} else {
				addAlias();
			}
			break;
		case 6:	
			if(par[1] == NULL) {
				printf("No command has been specified");
			} else {
				removeAlias();				
			}
	}
}

void run(){
	int m;
	int intersize = sizeof(inter)/sizeof(char *);
	int in;
	in = 0;

		if(par[0] == NULL){
			printf("No command entered");
		}
	
		for(m = 0; m < intersize; m++){
			if(strcmp(par[0], inter[m]) == 0){
				in = 1;
				runInternal(m);
			}
		}
		if(in == 0) executeCMD();
		memset(par,0,sizeof(par));
		
}

void invokeAlias() { /*runs an alias*/
	int i;
	for(i = 0; i < aliasCounter; i++) {
		if(strcmp(aliasNames[i], par[1]) == 0) {
			strcpy(aliasCommands[i], par[1]);
		}
	}	
}/*ends invokeAlias()*/

void tokenise(){
	char tokeniser[50] = "\t \n <>&|;";
	char *p = strtok(input, tokeniser);
	int i = 0;
			
	if(NULL == p){
		return;
	}
	
	while(p != NULL){
		par[i] = p;		
		++i;
		p = strtok(NULL, tokeniser);
	}
	for(i = 0;i < aliasCounter;i++){
		if(strcmp(par[0],aliasNames[i]) == 0) {
			strcpy(par[0], aliasCommands[i]);
		}
	}		

	
//printf("%s", par[1]);
	run();
}

void historyCommand(){


	char strin[1000];
	char *ptr;
	long ret;
	int choice;
	int i;
	int k = strncmp(input, "!-", 2);
	char tempArray[1000];
	int len;
	printf("\n%s\n", input);
if(k!=0){
	strncpy(strin, &input[1],3); //Ignores the first character of input and copies
					// the next 3 chars

	ret = strtol(strin,&ptr,10);
	choice = ret;
	choice--;
	


	if(choice >= 0 && choice < 10){
		strncpy(tempArray,&input[2],MAX-3);
		
	}
	else if(choice >= 10){
		strncpy(tempArray,&input[3],MAX-4);
	}
	else{
		printf("Invalid History number entered");
		return;
	}
	
		
	if(histLoop == 0 && choice >= 0 && choice <= historyCount){
		
			strcpy(input,history[choice]);
			len = strlen(input);
			strcpy(&input[len],tempArray);
			
			tokenise();	
	}
	else if(histLoop == 1 && choice >= 0 && choice < 21){
			strcpy(input,history[choice]);
			len = strlen(input);
			strcpy(&input[len],tempArray);
			tokenise();
	}
	else{
		printf("%s : event not found here 4",input);
	}
}
else{
		strncpy(strin, &input[2],3); //Ignores the first character of input and copies
					// the next 3 chars

	ret = strtol(strin,&ptr,10);
	choice = ret;
	printf("REACH HEHRE 1:%i",choice);



if(choice > 0 && choice < 10){
		strncpy(tempArray,&input[4],MAX-3);
		
	}
	else if(choice >= 10 && choice <20){
		strncpy(tempArray,&input[5],MAX-4);
	}
	else{
	printf("Invalid History number entered");
	return;
	}
	

	if(histLoop == 0 && choice > 0 && choice <= historyCount){	
		i = historyCount - choice;
		i--;
		printf("REACH HEHRE 3:%i",choice);
			if(i > 0){
			strcpy(input,history[i]);
			len = strlen(input);
			strcpy(&input[len],tempArray);
			tokenise();
			}
	}
	/*else if(histLoop == 0 && choice >-20){
		choice = choice * -1;
		i = historyCount - choice;
		i--;
	printf("TA IS:%i", choice);
		if(i > 0){
			strcpy(input,history[i]);
			len = strlen(input);
			strcpy(&input[len],tempArray);
			tokenise();
			}
	}*/

	


	else if(histLoop == 1 && choice > 0 && choice < 20){
			i = 20 - choice;
			i--;
			printf("REACH HEHRE 4:%i",choice);
			if(i>0){
			strcpy(input,history[i]);
			len = strlen(input);
			strcpy(&input[len],tempArray);
			tokenise();	
			}
	}
	/*else if (histLoop == 1 && choice >-20){
		printf("REACH HEHRE 2:%i",choice);
		choice = choice * -1;
		i = 20 - choice;
		i--;
	printf("TA IS:%i", choice);
		if(i > 0){
			strcpy(input,history[i]);
			len = strlen(input);
			strcpy(&input[len],tempArray);
			tokenise();
			}
	
	}*/

}
	
}


void getInput(){
	/*char input[1000];*/
	int i, t, k;
	
	while(1){
		printf(pointer);
		if(fgets(input, MAX, stdin) == NULL){
		exit(0);
		}

		for(i = 0;i < 10;i++){
		int len = strlen(input);
		int ret = strncmp(input,aliasNames[i],len-1);
		if(ret == 0){
		strcpy(input,aliasCommands[i]);
		}
		}
		 t = strncmp(input, "!", 1);
		 k = strncmp(input, "!-", 2);
		if( t != 0){
		saveHistory();
		saveHistoryFile();
		}

		if(t == 0 || k==0){
		historyCommand();
		}
		else tokenise() ;

	}

		/*if(t == 0){
			if(input[3] == '\0' || input[2] == '\0' || input[4] == '\0'){
				if((isdigit(input[1]) && input[2] == '\0') || (isdigit(input[1]) && isdigit(input[2])	)){
			printf("hehEHE111");
			historyCommand();
				}
			}*/



			/*if(input[3] == '\0'){
				
				if(isdigit(input[1])){
					
					historyCommand();
				}
				else{
					printf("%s : event not found here 1",input);
				}
			}
			else if(input[4] == '\0' && k !=0){
				if(isdigit(input[1]) && isdigit(input[2])){
					
					historyCommand();
				}
				else{
					printf("%s : event not found here 2",input);
				}
			}
			else if(k==0){
				if(input[4]=='\0' && isdigit(input[2])){
					historyCommand();
				}
				else if((input[5]=='\0') && isdigit(input[2]) && isdigit(input[3])){
					historyCommand();
				}
				else{
				printf("%s : event not found here 3",input);
			}
			}
			
			
			else{
				printf("%s : event not found",input);
			}
		}
		else tokenise();
	
		}*/
}

int main(){

char path[MAX];
	strcpy(path, getenv("PATH"));
	strcpy(home, getenv("HOME"));
	loadHistory();
	chdir(home);
	printf("PATH :%s\n",(path!=NULL)? path : "getenv returned NULL");	
	printf("Home: %s\n ",home); 	
	getInput();
	return(0);
}
