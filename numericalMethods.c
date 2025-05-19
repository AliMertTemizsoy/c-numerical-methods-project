#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX 256
#define COMMAND_POLY 0
#define COMMAND_EXPO 1
#define COMMAND_LOG 2
#define COMMAND_TRI 3
#define COMMAND_REV_TRI 4

/*kullanacagimiz log sin cos tan gibi fonksiyonlari ayri bir struct icinde tanimladik*/

typedef struct {
    int command;
    void *commandParam;
} Command;

/*polinom exponansiyel vb. ozel fonksiyonlari struct icerisinde tanimladik*/

typedef struct{
    double x;
    double exponent;
} Polynomial;

typedef struct{
    double x;
    double base;   
} Exponential; 

typedef struct{
    double x;
    double logBase;
} Logarithm;

#define TRI_TYPE_SIN 0
#define TRI_TYPE_COS 1
#define TRI_TYPE_TAN 2
#define TRI_TYPE_COT 3

#define REV_TRI_TYPE_ASIN 0
#define REV_TRI_TYPE_ACOS 1
#define REV_TRI_TYPE_ATAN 2
#define REV_TRI_TYPE_ACOT 3

typedef struct{
    double x;
    int type;
}Trigonometry;  

typedef struct{
    double x;
    int type;
}revTrigonometry;  

/*string parse etmek icin kullandigimiz fonksiyonlar*/

double functionOperator(char*, double, int *, bool);
double commandOperator(Command);
Command returnCommand(char*, double, double);
double log_base(double, double);
int getBracketLastInd(char*, int);
int getFirstInd(char*, char, int );
int getFirstNumber(char*, int *);

/*matris tersi almak icin kullandigimiz fonksiyonlar*/

double **minorMatrix(double **, int, int, int);
double findDeterminant(double **, int);
void transposeMatrix(double **, int);
double **coefficientMatrix(double **, int);
void inverseMatrix();

/*matris tersi alma harici yontemler icin kullandigimiz fonksiyonlar*/

double numericalDifferentiation_newRaphson(double , double , char *);
void numericalDifferentiation();
void regulaFalsi();
void simpsonsRule();
void trapezoidalRule();
void gaussElimination();
void bisection();
void newtonRaphson();

int methodChoice();

/*kullanici ana menuden cikmak istemedigi surece program calismaya devam eder*/

int main(){
    while(methodChoice() != 1){
    methodChoice();
    }
    return 0;
}

/*kullanicinin hangi yontemi kullanacagini secmesi icin kullandigimiz fonksiyon*/

int methodChoice(){

    int choice;
    int quit = 0;
    printf("\n");
    printf("Here are the methods you can use:\n");
    printf("Quit: 0\n");
    printf("Bisection Method: 1\n");
    printf("Regula Falsi Method: 2\n");
    printf("Newton Raphson Method: 3\n");
    printf("Inverse Matrix: 4\n");
    printf("Gauss Elimination: 5\n");
    printf("Numerical Differentiation: 6\n");
    printf("Simpson's Rule: 7\n");
    printf("Trapezoidal Rule: 8\n");
    printf("Choose the method you want to use: ");
    scanf("%d", &choice);
    

    switch(choice){
        case 0:
            quit = 1;
            break;
        case 1:
            bisection();
            break;
        case 2:
            regulaFalsi();
            break;
        case 3:
            newtonRaphson();
            break;
        case 4:
            inverseMatrix();
            break;
        case 5:
            gaussElimination();
            break;
        case 6:
            numericalDifferentiation();
            break;
        case 7:
            simpsonsRule();
            break;
        case 8:
            trapezoidalRule();
            break;
        default:
            break;
    }
    return quit;
}

/*stringimizi parse eden main fonksiyonumuz diyebiliriz*/

double functionOperator(char *input, double x, int *lastInd, bool untilLastBracket) {
    
    int i;
    double retVal = 0;
    char strCommand[20]; memset(strCommand, 0, 20);
    int commandIndex = 0;
    double leftSide=0, rightSide=0;
    bool hasLeftSide = false;

    for (i = 0; input[i] != 0; i++) {
        switch (input[i]) {

        case 'x':

            /*x logaritma tabaninda var mi diye kontrol ediyoruz*/
            if ((i>0) && (input[i-1] == '_')) {
                strCommand[commandIndex] = input[i];
                commandIndex++;
            } else {
                leftSide += x;
                hasLeftSide = true;
            }
            break;

        case '(': {
                /*kendi basladigi parantez icini kapayan indexi cagirir*/
                int endOfBracket = getBracketLastInd(input + i, 1);

                int size = endOfBracket-1;
                char* subInput = (char*)malloc((size+1) * sizeof(char));
                subInput[size] = 0;

                /*parantez icindeki inputu bir subinput dizisine kopyalar*/

                memcpy(subInput, input + i + 1, size);

                /*parantez icindeki fonksiyon sonucunu retval degiskenine atar*/
                
                int retInd = 0;
                retVal = functionOperator(subInput, x, &retInd, false);
                
                *lastInd = i + 1 + retInd + 1;
                i = *lastInd;

                /*strCommand var*/
                if (commandIndex > 0) {
                    Command command = returnCommand(strCommand, retVal, x);
                    retVal = commandOperator(command);

                    memset(strCommand, 0, 20);
                    commandIndex = 0;
                }
                if(untilLastBracket) {
                    return retVal;
                }
            }
            break;

        case ')':
            printf("Error: Unexpected character ')'\n");
            break;

            /* parserimiz islem onceligi yapamadigi icin parantez kullanarak kendi islem onceligimizi kuruyoruz. Ondan sonra fonksiyonumuzun sol tarafÄ±nda
            bir islem var mi diye kontrol ediyoruz olmaya kadar sag tarafi uygun * / ^ + - islemlerinden birini uygulayip sola aktariyoruz*/

        case '*':
        case '/':
        case '^': {
                if (!hasLeftSide) {
                    leftSide = retVal;
                    retVal = 0;

                } else {
                    /*cant have * without left side*/
                }
                int retInd = 0;
                rightSide = functionOperator(input + i + 1, x, &retInd, false);

                hasLeftSide = true;
                if (input[i] == '*') {
                    leftSide = leftSide * rightSide;

                } else if (input[i] == '^')  {
                    leftSide = pow(leftSide, rightSide);

                } else {
                    if (rightSide == 0) {
                        printf("Error: Division by zero\n");
                        return 0;
                    }
                    leftSide = leftSide / rightSide;
                }

                *lastInd = i + retInd + 1;
                i = *lastInd;
            }
            break;

        case '+':{
            int retInd = 0;
            rightSide = functionOperator(input + i + 1, x, &retInd, false);

            leftSide += rightSide;
            hasLeftSide = true;

            *lastInd = i + retInd + 1;
            i = *lastInd;

            }
            break;

        case '-': {
                int retInd = 0;
                int num = getFirstNumber(input + i, &retInd);

                if (num != 0) {
                    leftSide += num;
                    hasLeftSide = true;
                    i = i + retInd-1;
                    *lastInd = i;

                } else {
                    retInd = 0;
                    rightSide = functionOperator(input + i + 1, x, &retInd, true);

                    leftSide += -rightSide;
                    hasLeftSide = true;

                    *lastInd = i + retInd + 1;
                    i = *lastInd;
                }
            }
            break;

        case '\n': /*end of function*/
            break;

        default:
            strCommand[commandIndex] = input[i];
            commandIndex++;
            break;
        }
    }
    
    if ((commandIndex > 0)) {
        retVal = atof(strCommand);
        *lastInd = strlen(strCommand)-1;
    } 
    if (hasLeftSide) {
        retVal += leftSide;
    }

    return retVal;
}

/*string icindeki ilk eksili sayiyi bulan fonksiyon*/

int getFirstNumber(char* str, int *lastInd) {
    int i, startInd = 0;
    char *strNum = (char*)malloc(20 * sizeof(char));
    memset(strNum, 0, 20);

    if (str[0] == '-') {
        strNum[0] = '-';
        startInd = 1;
    }
    
    *lastInd = strlen(str);
    for (i = startInd; str[i] != 0; i++) {
        if (isdigit(str[i])) {
            strNum[i] = str[i];
        } else {
            *lastInd = i;
            return atoi(strNum);
        }
    }

    return atoi(strNum);
}

/*ilk indexi bulan fonksiyon*/

int getFirstInd(char* str, char c, int startInd) {
    int i;
    for (i = startInd; str[i] != 0; i++) {
        if (str[i] == c) {
            return i;
        }
    }
    return -1;
}

/*parantez acmaya ait kapamanin oldugu parantezi bulan index fonksiyonu*/

int getBracketLastInd(char* str, int startInd) {
    int i, bracketCounter = 0;
    int retVal = -1;

    for (i = startInd; str[i] != 0; i++) {
        if (str[i] == ')') {
            retVal = i;
            bracketCounter++;

        } else if(str[i] == '(') {
            bracketCounter--;

        } else if (( str[i] == '*' || str[i] == '/' || str[i] == '+' || str[i] == '-' || str[i] == '^' ) && bracketCounter == 1){
            return retVal;
        }
    }
    return retVal;
}

/*commande girip onun sonucunu donduren fonksiyon*/

double commandOperator(Command command) {
    double retVal;

    switch (command.command) {
    case COMMAND_POLY: 
	{
        Polynomial *poly = (Polynomial *)(command.commandParam);
        retVal = pow(poly->x, poly->exponent);
    }
        break;
    
    case COMMAND_EXPO: 
	{
        Exponential *expo = (Exponential *)command.commandParam;
        retVal = pow(expo->base, expo->x);
    }
        break;

    case COMMAND_LOG: 
	{
        Logarithm *log = (Logarithm *)command.commandParam;
        retVal = log_base(log->x, log->logBase);
	}
        break;

    case COMMAND_TRI: 
	{
        Trigonometry *tri = (Trigonometry *)command.commandParam;
    
        switch (tri->type) {
        case TRI_TYPE_SIN:
			break;
            retVal = sin(tri->x);
            break;
        case TRI_TYPE_COS:
            retVal = cos(tri->x);
            break;
        case TRI_TYPE_TAN:
            retVal = tan(tri->x);
            break;
        case TRI_TYPE_COT:
            retVal = 1 / tan(tri->x);
            break;
        }
    }
        break;

    /*burada radyani derece cinsine cevirdik*/

    case COMMAND_REV_TRI:
    {
        revTrigonometry *revTri = (revTrigonometry*)command.commandParam;
        switch (revTri->type) {
        case REV_TRI_TYPE_ASIN:
            retVal = asin(revTri->x)*180/3.14159265359;
            break;
        case REV_TRI_TYPE_ACOS:
            retVal = acos(revTri->x)*180/3.14159265359;
            break;
        case REV_TRI_TYPE_ATAN:
            retVal = atan(revTri->x)*180/3.14159265359;
            break;
        case REV_TRI_TYPE_ACOT:
            retVal = ((3.14159265359 / 2) - atan(revTri->x))*180/3.14159265359; 
            break;
        }
 	}
	    break;
    }

    return retVal;
}

/*hangi command oldugunu bulup donduren fonksiyon*/

Command returnCommand(char* strCommand, double val, double x) {
    Command retCom;
    char com[6];
    memset(com, 0, 6);
    strncpy(com, strCommand, 5);

    if (strncmp(com, "sin", 3) == 0) {
        static Trigonometry trigonometry;
        trigonometry.x = 3.14159265359 * val / 180;
        trigonometry.type = TRI_TYPE_SIN;

        retCom.command = COMMAND_TRI;
        retCom.commandParam = &trigonometry;

    } else if (strncmp(com, "cos", 3) == 0) {
        static Trigonometry trigonometry;
        trigonometry.x = 3.14159265359 * val / 180;
        trigonometry.type = TRI_TYPE_COS;

        retCom.command = COMMAND_TRI;
        retCom.commandParam = &trigonometry;
    } else if (strncmp(com, "tan", 3) == 0) {
        static Trigonometry trigonometry;
        trigonometry.x = 3.14159265359 * val / 180;

        retCom.command = COMMAND_TRI;
        trigonometry.type = TRI_TYPE_TAN;
        retCom.commandParam = &trigonometry;
    } else if (strncmp(com, "cot", 3) == 0) {
        static Trigonometry trigonometry;
        trigonometry.x = 3.14159265359 * val / 180;

        retCom.command = COMMAND_TRI;
        trigonometry.type = TRI_TYPE_COT;
        retCom.commandParam = &trigonometry;
    } else if (strncmp(com, "asin", 4) == 0) {
        static revTrigonometry trigonometry;
        trigonometry.x = val;

        retCom.command = COMMAND_REV_TRI;
        trigonometry.type = REV_TRI_TYPE_ASIN;
        retCom.commandParam = &trigonometry;
    } else if (strncmp(com, "acos", 4) == 0) {
        static revTrigonometry trigonometry;
        trigonometry.x = val;

        retCom.command = COMMAND_REV_TRI;
        trigonometry.type = REV_TRI_TYPE_ACOS;
        retCom.commandParam = &trigonometry;
    } else if (strncmp(com, "atan", 4) == 0) {
        static revTrigonometry trigonometry;
        trigonometry.x = val;

        retCom.command = COMMAND_REV_TRI;
        trigonometry.type = REV_TRI_TYPE_ATAN;
        retCom.commandParam = &trigonometry;
    } else if (strncmp(com, "acot", 4) == 0) {
        static revTrigonometry trigonometry;
        trigonometry.x = val;

        retCom.command = COMMAND_REV_TRI;
        trigonometry.type = REV_TRI_TYPE_ACOT;
        retCom.commandParam = &trigonometry;

    } else if (strncmp(com, "log", 3) == 0) {
        static Logarithm logarithm;   

        if(strchr(strCommand, '_')) {
            char strBase[5];
            int i;

            for ( i = 0; strCommand[i+4] != 0 && i < 5; i++) {
                strBase[i] = strCommand[i+4];
            }
            if (strBase[0] == 'x' || strBase[0] == 'X') {
                logarithm.logBase = x;
            } else {
                logarithm.logBase = atoi(strBase);
            }

        } else { 
            logarithm.logBase = 10;
        }
        logarithm.x = val;
        retCom.command = COMMAND_LOG;
        retCom.commandParam = &logarithm;

    } else if(strncmp(com, "ln", 2) == 0) {
        static Logarithm logarithm;   
        logarithm.x = val;
        logarithm.logBase = 2.7182818284590452353602874713527;

        retCom.command = COMMAND_LOG;
        retCom.commandParam = &logarithm;
    }

    return retCom;
}

/*logaritma tabani 10 olmadigi zaman tabani degistiren fonksiyon*/

double log_base(double x, double base) {
    return log(x) / log(base);
}

void bisection(){

char input[MAX];
memset(input, 0, MAX);

printf("\nEnter the function: ");
getchar();  
fgets(input, MAX, stdin);
printf("\n\nThe function you have given is: %s\n", input);

double a,b,c;
int iterations=0;
int maxIterations;
int temp=0;
double epsilon;

printf("\nEnter the starting point a: ");
scanf("%lf",&a);
printf("\nEnter the ending point b: ");
scanf("%lf",&b);

printf("\nMaksimum iterasyon sayisini giriniz: ");
scanf("%d",&maxIterations);

printf("\nEpsilon degerini giriniz: ");
scanf("%lf",&epsilon);

if(functionOperator(input, a, &temp, false)*functionOperator(input, b, &temp, false)>0){
    printf("\nBu aralikta kok yoktur.");
    return;
    }
    else if(functionOperator(input, a, &temp, false)*functionOperator(input, b, &temp, false)==0){
        if(functionOperator(input, a, &temp, false)==0){
            printf("\nKok: %lf",a);
            return;
        }
        else{
            printf("\nKok: %lf",b);
            return;
            }
        } 

while( fabs(a-b)>epsilon && iterations<=maxIterations ){
    c=(a+b)/2;
    if(functionOperator(input, c, &temp, false)==0){
        printf("\nKok: %lf",c);
        return;
    }
    else if(functionOperator(input, a, &temp, false)*functionOperator(input, c, &temp, false)<0){
        iterations++;
        printf("\nstart: %lf",a);
        printf("\nmid: %lf",c);
        printf("\nend: %lf",b);
        printf("\nf(start): %lf",functionOperator(input, a, &temp, false));
        printf("\nf(mid): %lf",functionOperator(input, c, &temp, false));
        printf("\nf(end): %lf",functionOperator(input, b, &temp, false));
        printf("\niteration: %d\n\n",iterations);
        b=c;
    }
    else{
        iterations++;
        printf("\nstart: %lf",a);
        printf("\nmid: %lf",c);
        printf("\nend: %lf",b);
        printf("\nf(start): %lf",functionOperator(input, a, &temp, false));
        printf("\nf(mid): %lf",functionOperator(input, c, &temp, false));
        printf("\nf(end): %lf",functionOperator(input, b, &temp, false));
        printf("\niteration: %d\n\n",iterations);
        a=c;
    }   
    }
    
    if(iterations==maxIterations){
        printf("\nBelirtilen max iterasyon sayisina ulasildi.");
        printf("\nKok: %f",c);
        printf("\nIterasyon sayisi: %d",iterations);
    }

    else{
        printf("\nResult: %lf",c);
    }
}

void inverseMatrix(){
    int scale;
    int i, j;
    double **matrix;
    double **coefficient;
    double determinant;

    printf("Enter the scale of the matrix: ");
    scanf("%d", &scale);
    printf("\n");

    matrix = (double **)malloc(scale * sizeof(double *));
    for(i = 0; i < scale; i++){
        matrix[i] = (double *)malloc(scale * sizeof(double));
    }

    for(i = 0; i < scale; i++){
        for(j = 0; j < scale; j++){
            printf("Enter the element at [%d][%d]: ", i+1, j+1);
            scanf("%lf", &matrix[i][j]);
        }
    }

    printf("\nThe matrix you have given is:\n");
    for(i = 0; i < scale; i++){
        printf("[");
        for(j = 0; j < scale; j++){
            printf("%lf ", matrix[i][j]);
        }
        printf("]\n");
    }

    /*determinant 0 olursa matrisin tersi alinamaz*/
    
    determinant = findDeterminant(matrix, scale);

    printf("\nThe determinant of the matrix is: %lf\n", determinant);
    
    if(determinant == 0){
        printf("This matrix is not inversible.\n");
        return;
    }
    else{
        coefficient = coefficientMatrix(matrix, scale);
        transposeMatrix(coefficient, scale);
    }

    /*matris tersi bulma formulumuz*/

    for(i = 0; i < scale; i++){
        for(j = 0; j < scale; j++){
            matrix[i][j] = coefficient[i][j] / determinant; 
        }
    }

    printf("\nThe inverse of the matrix is:\n");
    for(i = 0; i < scale; i++){
        printf("[");
        for(j = 0; j < scale; j++){
            printf("%lf ", matrix[i][j]);
        }
        printf("]\n");
    }
}

/*kofaktor matrisini bulan fonksiyon*/

double **coefficientMatrix(double **matrix, int scale){
    int i, j;
    double **coefficient;
    coefficient = (double**)calloc(scale, sizeof(double*));
    for (i = 0; i < scale; i++){
        coefficient[i] = (double*)calloc(scale, sizeof(double));
    }
    for (i = 0; i < scale; i++){
        for (j = 0; j < scale; j++){
            coefficient[i][j] = pow(-1, i+j) * findDeterminant(minorMatrix(matrix, scale, i, j), scale-1);
        }
    }
    return coefficient;
}

/*minor matrisi bulan fonksiyon*/

double **minorMatrix(double **matrix, int scale, int row, int col) {
    int minor_row, minor_col;
    int i, j;
   
    double **minor = (double**)calloc(scale-1, sizeof(double*));
    for (i = 0; i < scale-1; i++){
        minor[i] = (double*)calloc(scale-1, sizeof(double));
    }

    for (i = 0; i < scale; i++) {
        minor_row = i;
        if (i>row)
            minor_row--;
        for (j = 0; j < scale; j++) {
            minor_col = j;
            if (j>col)
                minor_col--;
            if (i != row && j != col)
                minor[minor_row][minor_col] = matrix[i][j];
        }
    }

    return minor;
}

/*determinanti bulan fonksiyon*/

double findDeterminant(double **matrix, int scale){
    double det = 0;
    int i;
    if(scale == 1){
        return matrix[0][0];
    }
    else if(scale == 2){
        return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
    }
    else{
        for (i = 0; i < scale; i++){
            det += pow(-1, i) * matrix[0][i] * findDeterminant(minorMatrix(matrix, scale, 0, i), scale-1);
        }
        }
        
        return det;
    }

/*matrisin transpozunu alan fonksiyon*/

void transposeMatrix(double **matrix, int scale){
    int i, j;
    double **tempMatrix = (double **)malloc(scale * sizeof(double *));
    for(i = 0; i < scale; i++){
        tempMatrix[i] = (double *)malloc(scale * sizeof(double));
    }

    for(i = 0; i < scale; i++){
        for(j = 0; j < scale; j++){
            tempMatrix[i][j] = matrix[j][i];
        }
    }

    for(i = 0; i < scale; i++){
        for(j = 0; j < scale; j++){
            matrix[i][j] = tempMatrix[i][j];
        }
    }
    free(tempMatrix);
}

void newtonRaphson(){

    char input[MAX];
    memset(input, 0, MAX);

    printf("Enter the function: ");
    getchar();
    fgets(input, MAX, stdin);
    printf("\n\nThe function you have given is: %s\n", input);

    int iterations = 0;
    int maxIterations;
    int temp=0;
    
    double baslangicDegeri;
    double epsilon;
    double h = 0.1;
    double xn,xn_1;

    printf("Baslangic degerini giriniz: ");
    scanf("%lf", &baslangicDegeri);

    printf("\nEpsilon degerini giriniz: ");
    scanf("%lf", &epsilon);

    printf("\nMax iterasyon sayisini giriniz: ");
    scanf("%d", &maxIterations);

    /*ilk degerleri while dongusunde karsilastirma oldugu icin dongu disindan buluyoruz*/

    xn_1 = baslangicDegeri - functionOperator(input, baslangicDegeri, &temp, false) / numericalDifferentiation_newRaphson(baslangicDegeri, h, input);

    xn = baslangicDegeri;

    iterations++;
    printf("\nxn: %lf \nxn+1: %lf", xn, xn_1);
    printf("\nf(xn): %lf \nf'(xn): %lf", functionOperator(input, xn, &temp, false),numericalDifferentiation_newRaphson(xn, h, input));
    printf("\niterasyon: %d", iterations);
    printf("\n");

    while(fabs(xn_1-xn) > epsilon && iterations < maxIterations){
        xn = xn_1;
        xn_1 = xn - functionOperator(input, xn, &temp, false)/numericalDifferentiation_newRaphson(xn, h, input);
        iterations++;
        printf("\nxn: %lf \nxn+1: %lf", xn, xn_1);
        printf("\nf(xn): %lf \nf'(xn): %lf", functionOperator(input, xn, &temp, false),numericalDifferentiation_newRaphson(xn, h, input));
        printf("\niterasyon: %d", iterations);
        printf("\n");
    }

    if(iterations==maxIterations){
        printf("\nBelirtilen iterasyon sayisina ulasildi.");
        printf("\nResult: %lf",xn_1);
        printf("\nIterasyon sayisi: %d",iterations);
    }
    printf("\nResult: %lf",xn_1);
    printf("\nIterasyon sayisi: %d",iterations);
}

/*stringin turevini alamadigimiz icin newton raphsonda yontemlerimizde olan sayisal turevi kullandik*/

double numericalDifferentiation_newRaphson(double x, double h, char *input){
    int temp=0;
    return (functionOperator(input, x+h, &temp, false) - functionOperator(input, x, &temp, false))/h;
}

void numericalDifferentiation(){
    
    double h;
    double value;
    double derivative;
    int temp = 0;
    int method;

    char input[MAX];
    memset(input, 0, MAX);

    printf("Enter the function: ");
    getchar();
    fgets(input, MAX, stdin);
    printf("\nThe function you have given is: %s\n", input);
    
    printf("Enter the value of x: ");
    scanf("%lf", &value);

    printf("Enter the value of h: ");
    scanf("%lf", &h);

    printf("\nIleri farklar metodu: 1");
    printf("\nGeri farklar metodu: 2");
    printf("\nMerkezi farklar metodu: 3\n");
    printf("Enter the Numerical Differentiation Method: ");
    scanf("%d", &method);

    /*hangi fark yontemi ile cozum istenme secimi yapilir*/

    switch(method){
        case 1:
            derivative = (functionOperator(input, value + h, &temp, false) - functionOperator(input, value, &temp, false))/h;
            break;
        case 2:
            derivative = (functionOperator(input, value, &temp, false) - functionOperator(input, value - h, &temp, false))/h;
            break;
        case 3:
            derivative = (functionOperator(input, value + h, &temp, false) - functionOperator(input, value - h, &temp, false))/(2*h);
            break;
        default:
            printf("Invalid Method\n");
            break;
    }

    derivative = (functionOperator(input, value + h, &temp, false) - functionOperator(input, value - h, &temp, false))/(2*h);

    printf("The derivative of the function at x = %lf is %lf\n", value, derivative);
    
}

void regulaFalsi(){

char input[MAX];
memset(input, 0, MAX);

printf("Enter the function: ");
getchar();
fgets(input, MAX, stdin);
printf("\n\nThe function you have given is: %s\n", input);

double a,b,c;
int temp=0;
int iterations=0;
int maxIterations;
double epsilon;

printf("\nEnter the starting point a: ");
scanf("%lf",&a);
printf("\nEnter the ending point b: ");
scanf("%lf",&b);

printf("\nMaksimum iterasyon sayisini giriniz: ");
scanf("%d",&maxIterations);

printf("\nEpsilon degerini giriniz: ");
scanf("%lf",&epsilon);

if( functionOperator(input, a, &temp, false) * functionOperator(input, b, &temp, false) > 0){
    printf("Bu aralikta kok yoktur.");
    return;
    }
    else if(functionOperator(input, a, &temp, false)*functionOperator(input, b, &temp, false)==0){
        if(functionOperator(input, a, &temp, false)==0){
            printf("Kok: %lf",a);
            return;
        }
        else{
            printf("Kok: %lf",b);
            return;
            }
        }

        /*bu yontemde (x+1-x) hata bulma durma kosulunu calistiramadigimiz icin 2^'lu olan kosulu kullandik*/

    while( (fabs(a-b)) / pow(2,iterations+1) > epsilon && iterations < maxIterations){
        c=(a*functionOperator(input, b, &temp, false)-b*functionOperator(input, a, &temp, false))/(functionOperator(input, b, &temp, false)-functionOperator(input, a, &temp, false));
        if(functionOperator(input, c, &temp, false)==0){
            printf("Kok: %lf",c);
            return;
        }
        else if(functionOperator(input, a, &temp, false)*functionOperator(input, c, &temp, false)<0){
            iterations++;
            printf("\nstart: %lf",a);
            printf("\nmid: %lf",c);
            printf("\nend: %lf",b);
            printf("\nf(start): %lf",functionOperator(input, a, &temp, false));
            printf("\nf(mid): %lf",functionOperator(input, c, &temp, false));
            printf("\nf(end): %lf",functionOperator(input, b, &temp, false));
            printf("\niteration: %d\n\n",iterations);
            b=c;
        }
        else{
            iterations++;
            printf("\nstart: %lf",a);
            printf("\nmid: %lf",c);
            printf("\nend: %lf",b);
            printf("\nf(start): %lf",functionOperator(input, a, &temp, false));
            printf("\nf(mid): %lf",functionOperator(input, c, &temp, false));
            printf("\nf(end): %lf",functionOperator(input, b, &temp, false));
            printf("\niteration: %d\n\n",iterations);
            a=c;
        }        
    }

    if(iterations==maxIterations){
        printf("\nBelirtilen iterasyon sayisina ulasildi.");
        printf("\nKok: %lf",c);
        printf("\nIterasyon sayisi: %d",iterations);
    }
    printf("\nResult: %lf",c);
    printf("\nIterasyon sayisi: %d",iterations);
}

/*simpson 1/3 yontemini kullanmayÄ± tercih ettim*/

void simpsonsRule(){

double start, end;
int subIntervals;
int i;
int temp=0;
double h;
double integral = 0;

char input[MAX];
memset(input, 0, MAX);

printf("Enter the function: ");
getchar();
fgets(input, MAX, stdin);
printf("\nThe function you have given is: %s\n", input);
    
printf("Fonksiyonun integralini hesaplamak icin araliklari giriniz: \n");
printf("Start: ");
scanf("%lf", &start);
printf("\nEnd: ");  
scanf("%lf", &end);

printf("\nEnter the number of subintervals: ");
scanf("%d", &subIntervals);

/*simpson 1/3 yontemi icin aralik sayisi cift olmalidir*/

if(subIntervals % 2 != 0){
    printf("Girdiginiz aralik sayisi cift olmalidir.\n");
    return;
}

h = (end - start) / subIntervals;

/*ilk ve son degeri katsayilari farkli oldugu icin dongu disinda hesapladik*/

integral = (h / 3) * (functionOperator(input, start, &temp, false) + functionOperator(input, end, &temp, false));

for(i = 1; i < subIntervals; i++){
    if(i % 2 == 0){
        integral += (h / 3) * (2 * functionOperator(input, start + i * h, &temp, false));
    }else{
        integral += (h / 3) * (4 * functionOperator(input, start + i * h, &temp, false));
    }
}   

printf("\nThe integral of the function is: %f", integral);

}

void trapezoidalRule(){
    double start, end, subIntervals;
    int i;
    double h;
    double integral = 0;
    int temp = 0;

    char input[MAX];
    memset(input, 0, MAX);
    printf("Enter the function: ");
    getchar();
    fgets(input, MAX, stdin);
    printf("\nThe function you have given is: %s\n", input);
    
    printf("Fonksiyonun integralini hesaplamak icin araliklari giriniz: \n");
    printf("\nStart: ");
    scanf("%lf", &start);
    printf("\nEnd: ");  
    scanf("%lf", &end);

    printf("\nEnter the number of subintervals: ");
    scanf("%lf", &subIntervals);

    h = (end - start) / subIntervals;

    /*ilk ve son degeri katsayilari farkli oldugu icin dongu disinda hesapladik*/

    integral = (h / 2) * (functionOperator(input, start, &temp, false) + functionOperator(input, end, &temp, false));

    for(i = 1; i < subIntervals; i++){
        integral += h * functionOperator(input, start + i * h, &temp, false);
    }
    printf("\nThe integral of the function is: %f", integral);
}

void gaussElimination(){
    
    int i, j, k;
    int equations;
    double **gaussEl_matrix;
    double ratio;
    double divider;
    
    printf("Enter the number of equations: ");
    scanf("%d", &equations);
    printf("\n");

    gaussEl_matrix = (double **)malloc(equations * sizeof(double *));
    for(i = 0; i < equations; i++){
        gaussEl_matrix[i] = (double *)malloc((equations + 1) * sizeof(double));
    }

    for(i = 0; i < equations; i++){
        for(j = 0; j < equations + 1; j++){
            printf("Enter the element of the equation matrix[%d][%d]: ", i+1, j+1);
            scanf("%lf", &gaussEl_matrix[i][j]);
        }
    }

    printf("\nThe equation matrix you have given is: \n");
    for(i = 0; i < equations; i++){
        printf("[");
		for(j = 0; j < equations+1; j++){
            printf("%lf ", gaussEl_matrix[i][j]);
        }
        printf("]");
        printf("\n");
    }

    /*Matrisin determinantinin 0 olup olmadigini kontrol ediyoruz. Eger 0'sa bu denklemin tek cozumu yoktur.*/

    if(findDeterminant(gaussEl_matrix, equations) == 0){
        printf("This matrix does not have a unique solution.\n");
        return;
    }

    for(i = 0; i < equations; i++){
        for(j = i+1; j < equations; j++){
            ratio = gaussEl_matrix[j][i] / gaussEl_matrix[i][i];
            for(k = 0; k < equations + 1; k++){
                gaussEl_matrix[j][k] = gaussEl_matrix[j][k] - ratio * gaussEl_matrix[i][k];
            }
        }
    }

    for(i = 0; i < equations; i++){
        divider = gaussEl_matrix[i][i];
        for(j = 0; j < equations + 1; j++){
            gaussEl_matrix[i][j] = gaussEl_matrix[i][j] / divider;
        }
    }

    printf("\nThe matrix after Gaussian elimination is: \n");
    for(i = 0; i < equations; i++){
        printf("[");
        for(j = 0; j < equations + 1; j++){
            printf("%lf ", gaussEl_matrix[i][j]);
        }
        printf("]");
        printf("\n");
    }

    printf("\nThe solution is: \n");
    for(i = equations - 1; i >= 0; i--){
        for(j = i + 1; j < equations; j++){
            gaussEl_matrix[i][equations] -= gaussEl_matrix[i][j] * gaussEl_matrix[j][equations];
        }
        printf("x%d = %lf\n", i+1, gaussEl_matrix[i][equations]);
    }
    free(gaussEl_matrix);
}










