/*
 * -----------------------------------------------------------------------
 * Un mot/registre de NBITS bits (par défaut NBITS=16) est représenté par un tableau d'entiers égaux à 0 ou 1.
 * Une ALU est représentée par une structure ALU, avec registre accumulateur et registre d'état.
 * Un CPU (très très simplifié) est représenté par une ALU et quelques registres nécessaires pour stocker les résultats intermédiaires.
 * -----------------------------------------------------------------------
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// en ayant toujours NBITS < 32
#define NBITS 16
/////////////////////////////////////////////////////////
// définition de types
/////////////////////////////////////////////////////////

typedef struct {
    int* accu;
    int* flags; // indicateurs ZF CF OF NF
} ALU;

typedef struct {
    ALU alu;
    int* R0;
    int* R1;
    int* R2;
} CPU;

/////////////////////////////////////////////////////////
// fonctions d'initialisation
/////////////////////////////////////////////////////////

/*
 * allocation d'un mot entier de NBITS bits initialisé à 0
 */
int* word() {
    int* tab;
    int i;
    tab=(int*)malloc(NBITS*sizeof(int));
    for(i=0;i<NBITS;i++) tab[i]=0;
    // poids faible : tab[0]
    // poids fort : tab[NBITS-1]
    return tab;
}

/*
 * Initialisation du mot (mot de NBITS bits, codant un entier en Cà2) avec une valeur entière.
 */
void setValue(int* word,int n) {
    // revient à mettre à 0 le bit de poids fort en 32 bits
    // on peut alors travailler sur la partie positive du codage de n
    // remarque : si n est bien codable en Ca2 sur NBITS, et si n est négatif, on récupère quand même le codage de n sur NBITS en Ca2 en prenant les NBITS de poids faible de n_low
    int n_low = n & 0x7FFFFFFF;
    int i;
    // verification n codable en ca2 sur NBITS
    if( -pow(2, NBITS-1) <= n && n <= pow(2, NBITS-1) - 1) {

        for(i=0 ; i < NBITS; i++) {

            word[i] = n_low % 2;
            n_low   = n_low >> 1 ;
        }
    }
    else {
        printf("erreur: setValue(): n non codable en ca2 sur NBITS\n");
    }
}

/*
 * instanciation d'un mot de NBITS bits initialisé avec la valeur n
 */
int* initWord(int n) {
    int* tab=word();
    setValue(tab,n) ;
    return tab;
}

/*
 * Initialisation du mot (mot de NBITS bits) par recopie des bits du mot en paramètre.
 */
void copyValue(int* word,int* src) {
    for(int i=0;i<NBITS;i++){
        word[i]=src[i];
    }
}

/*
 * instanciation d'un mot de NBITS bits initialisé par recopie d'un mot
 */
int* copyWord(int* src) {
    int* tab=word();
    copyValue(tab,src) ;
    return tab;
}

/*
 * initialise l'ALU
 */
ALU initALU() {
    ALU res;
    res.accu=word();
    res.flags=(int*)malloc(4*sizeof(int));
    return res;
}

/*
 * initialise le CPU
 */
CPU initCPU() {
    CPU res;
    res.alu=initALU();
    res.R0=(int*)malloc(NBITS*sizeof(int));
    res.R1=(int*)malloc(NBITS*sizeof(int));
    res.R2=(int*)malloc(NBITS*sizeof(int));
    return res;
}

/////////////////////////////////////////////////////////
// fonctions de lecture
/////////////////////////////////////////////////////////

/*
 * Retourne la valeur entière signée représentée par le mot (complément à 2).
 */    
int intValue(int* word) {
    int i;
    int valeur = 0;
    int p2     = 1;

    // cas positif
    if (word[NBITS-1]==0){    
        for(i = 0; i < NBITS; i++) {
            valeur += word[i] * p2;
            p2      = p2 << 1 ;
        }
    }
    // cas negatif
    else {
        for(i = 0; i < NBITS-1; i++) {
            valeur += word[i] * p2;
            p2      = p2 << 1 ;
        }
        valeur = valeur - p2;
    }
    return valeur ;
}

/*
 * Retourne une chaîne de caractères décrivant les NBITS bits
 */
char* toString(int* word) {
    int i,j=0;
    char* s=(char*)malloc((2+NBITS)*sizeof(char));
    for (i=NBITS-1;i>=0;i--) {
        if (word[i]==1) s[j]='1';
        else s[j]='0';
        j++;
    }
    s[j]=0;
    return s;
}

/*
 * Retourne l'écriture des indicateurs associés à l'ALU.
 */
char* flagsToString(ALU alu) {
    char *string=(char*)malloc(10*sizeof(char));
    sprintf(string,"z%dc%do%dn%d",alu.flags[0],alu.flags[1],alu.flags[2],alu.flags[3]);
    return string;
}

/*
 * affiche à l'écran le contenu d'une ALU
 */
void printing(ALU alu) {
    for(int i=NBITS-1;i>=0;i--){
        printf("%d",alu.accu[i]);
    }
    printf("\n");
    char *string=flagsToString(alu);
    printf("%s\n",string);
}

/////////////////////////////////////////////////////////
// fonctions de manipulations élémentaires
/////////////////////////////////////////////////////////

/*
 * Mise à la valeur b du bit spécifié dans le mot
 */
void set(int* word,int bitIndex,int b) {
    if ((bitIndex > NBITS-1) || (bitIndex < 0)) 
        printf("erreur: set() index en dehors de la plage\n");
    word[bitIndex] = b ;
}

/*
 * Retourne la valeur du bit spécifié dans le mot
 */
int get(int* word,int bitIndex) {
    if ((bitIndex > NBITS-1) || (bitIndex < 0)) 
        printf("erreur: get() index en dehors de la plage\n");
    return word[bitIndex] ;
}

/*
 * Positionne l'indicateur ZF en fonction de l'état de l'accumulateur
 */
void setZ(ALU alu) {
    int valeur   = intValue(alu.accu);
    alu.flags[0] = (valeur == 0) ? 1 : 0 ;
}

/////////////////////////////////////////////////////////
// opérateurs de base de l'ALU
// IMPORTANT : les indicateurs doivent être mis à jour
/////////////////////////////////////////////////////////

/*
 * Stocke le paramètre dans le registre accumulateur
 */
void pass(ALU alu,int* B) {
    for(int i=0; i<NBITS;i++){
        alu.accu[i]=B[i];
    }
    alu.flags[0] = 0;
    alu.flags[1] = 0;
    alu.flags[2] = 0;
    alu.flags[3] = 0;
}

/*
 * Effectue un NAND (NON-ET) entre le contenu de l'accumulateur et le paramètre.
 */
void nand(ALU alu,int* B) {
    int i;
    for(i=0; i < NBITS;i++) {
        if(alu.accu[i] == 1 && B[i] == 1) {
            alu.accu[i] = 0;
        }
        else{
            alu.accu[i] = 1;
        }
    }
    alu.flags[0] = 0;
    alu.flags[1] = 0;
    alu.flags[2] = 0;
    alu.flags[3] = 0;
}

/*
 * Décale le contenu de l'accumulateur de 1 bit vers la droite
 */
void shift(ALU alu) {
    int i;
    for(i=1; i < NBITS; i++) {
        alu.accu[i-1] = alu.accu[i];
    }
    alu.accu[NBITS-1] = 0; 
    
    alu.flags[0] = 0;
    alu.flags[1] = 0;
    alu.flags[2] = 0;
    alu.flags[3] = 0;   
}

/*
 * module Full Adder : a+b+c_in = s + 2 c_out
 * retourne un tableau contenant s et c_out
 */
int* fullAdder(int a,int b,int c_in) {
    int* res=(int*)malloc(2*sizeof(int));

    int s     = (a + b + c_in) % 2 ;
    int c_out = (a + b + c_in) >= 2 ;

    res[0] = s;
    res[1] = c_out;

    return res;
}

/*
 * Additionne le paramètre au contenu de l'accumulateur (addition entière Cà2).
 * Les indicateurs sont positionnés conformément au résultat de l'opération.
 */
void add(ALU alu,int* B) {
    // retenue courante & precedente
    int c = 0 ;
    int cp;

    int t;
    int i;

    // fullAdder res
    int* s;

    for(i=0; i < NBITS; i++) {
        cp = c;

        s  = fullAdder(alu.accu[i], B[i], c);
        
        c  = s[1];
        t  = s[0];

        alu.accu[i] = t;

        free(s);
    }

    // zero flag
    setZ(alu);
    
    // carry flag
    alu.flags[1] = c ;
    
    // overflow flag
    if ( c != cp ) {
         alu.flags[2] = 1 ;
    }
    
    // negative flag
    if( intValue(alu.accu) < 0) {
        alu.flags[3] = 1 ;
    }
}

////////////////////////////////////////////////////////////////////
// Opérations logiques :
////////////////////////////////////////////////////////////////////

/*
 * Négation.
 */
void not(CPU cpu){
    nand(cpu.alu, cpu.alu.accu); 
}

/*
 * Et.
 */
void and(CPU cpu,int* B) {
    nand(cpu.alu, B);
    nand(cpu.alu, cpu.alu.accu);
}


/*
 * Ou.
 */
void or(CPU cpu,int* B) {
    int i;
    nand(cpu.alu, cpu.alu.accu);

    // sauvegarde du res dans le registre R0
    for(i=0; i < NBITS ; i++) {
        cpu.R0[i] = cpu.alu.accu[i];
    }
    pass(cpu.alu, B);
    nand(cpu.alu, cpu.alu.accu);

    // le registre accumulateur contient le res du AND
    nand(cpu.alu, cpu.R0);

}

/*
 * Xor.
 */
void xor(CPU cpu,int* B) {
    int i;

    // sauvegarde de accu dans R1
    for(i=0; i < NBITS; i++) {
        cpu.R1[i] = cpu.alu.accu[i];
    }

    not(cpu);
    and(cpu, B);

    // sauvegarde de NON(acc) et B dans R2
    for(i=0; i < NBITS; i++) {
        cpu.R2[i] = cpu.alu.accu[i];
    }

    pass(cpu.alu, B);
    not(cpu);

    // calcul acc et NON(B)
    and(cpu, cpu.R1);
    
    // XOR final dans accu
    or(cpu, cpu.R2);
}

/*
 * Décale le receveur de |n| positions.
 * Le décalage s'effectue vers la gauche si n>0 vers la droite dans le cas contraire.
 * C'est un décalage logique (pas de report du bit de signe dans les positions 
 * libérées en cas de décalage à droite).
 * L'indicateur CF est positionné avec le dernier bit "perdu".
 */
void logicalShift(CPU cpu, int n) {
    int i;
    int lastBit;

    if( n > 0) {
        for (i=0; i < n; i++) {
            lastBit = cpu.alu.accu[NBITS-1];
            add(cpu.alu, cpu.alu.accu) ;
        }
            
        // carry flag
        cpu.alu.flags[1] = lastBit ;
    }
    else if( n < 0 ) {

        for(i=0; i > n; i--) {
            lastBit = cpu.alu.accu[0];
            shift(cpu.alu);
        }
        // carry flag
        cpu.alu.flags[1] = lastBit ;
    }
}

/////////////////////////////////////////////////////////
// Opérations arithmétiques entières
/////////////////////////////////////////////////////////

/*
 * Opposé.
 */
void opp(CPU cpu) {
    int* t = (int*)malloc(NBITS*sizeof(int));
    setValue(t,1);
    // on inverse les bits
    not(cpu);
    // on ajoute 1
    add(cpu.alu, t);

    free(t);
}

/*
 * Soustraction.
 */
void sub(CPU cpu,int* B) {
    // sub revient a inverser B et ensuite faire une addition
    int i;

    for(i=0; i < NBITS; i++) {
        cpu.R0[i] = cpu.alu.accu[i];
    }
    pass(cpu.alu, B);
    opp(cpu);
    add(cpu.alu, cpu.R0);
}

/*
 * Multiplication.
 */
void mul(CPU cpu,int* B) {
    int i,j;
    
    // sauvegarde de A dans R0 
    for(i=0; i < NBITS; i++) {
        cpu.R0[i] = cpu.alu.accu[i] ;
    }
    
    // sauvegarde de B dans R1
    for(i=0; i < NBITS; i++) {
        cpu.R1[i] = B[i] ;
    }
    
    // initialisation de R2
    for(i=0; i < NBITS; i++) {
        cpu.R2[i] = 0 ;
    }
    
    for(i=0; i < NBITS; i++) {
        
        // on regarde le bit de B, on envoie donc B
        pass(cpu.alu, cpu.R1);

        logicalShift(cpu,-1);

        // on sauvegarde la nouvelle valeur de B
        for(j=0; j < NBITS; j++) {
            cpu.R1[j] = cpu.alu.accu[j] ;
        }
        
        // si CF = 1 alors on fait une addition
        if(cpu.alu.flags[1] == 1) {

            // on envoie la R2
            pass(cpu.alu, cpu.R2);
            
            // on addition avec A
            add(cpu.alu, cpu.R0);
        
            // sauvegarde le resultat dans R2
            for(j=0; j < NBITS; j++) {
                cpu.R2[j] = cpu.alu.accu[j] ;
            }
        }
        // on envoie A
        pass(cpu.alu, cpu.R0);

        // on shift A
        logicalShift(cpu, 1);
        
        // on sauvegarde la nouvelle valeur de A
        for(j=0; j < NBITS; j++) {
            cpu.R0[j] = cpu.alu.accu[j] ;
        }
    }

    // le resultat est dans R2 on l'envoie dans le registre accumulateur
    pass(cpu.alu, cpu.R2) ;
}

/////////////////////////////////////////////////////////
// Programme de test
/////////////////////////////////////////////////////////

int main(int argc,char *argv[]) {

    int* operand;
    ALU alu;
    CPU cpu;

    int chosenInt,integer ;
    int go_on = 1 ;

    char* menu =     
        "              Programme de test\n\n0  Quitter\n1  setValue(operande,int)\n2  pass(alu,operande)\n3  printing(alu)\n4  afficher toString(operande)\n5  afficher intValue(operande)\n6  afficher intValue(accu)\n7  accu=nand(accu,operande)\n8  accu=add(accu,operande)\n9  accu=sub(accu,operande)\n10  accu=and(accu,operande)\n11 accu=or(accu,operande)\n12 accu=xor(accu,operande)\n13 accu=not(accu)\n14 accu=opp(accu)\n15 accu=shift(accu)\n16 accu=logicalShift(accu,int)\n17 accu=mul(accu,operande)\n\n" ;

    char* invite = "--> Quel est votre choix  ? " ;

    printf("%s",menu) ; 

    operand=word();
    cpu=initCPU();
    alu=cpu.alu;

    while (go_on==1) {
        printf("%s",invite);
        scanf("%d",&chosenInt);
        switch (chosenInt) {
            case 0 : 
                go_on = 0 ;
                break ;
            case 1 :
                printf("Entrez un nombre :"); 
                scanf("%d",&integer);
                setValue(operand,integer);
                break ;
            case 2 : 
                pass(alu,operand);
                break ;
            case 3 : 
                printing(alu);
                break ;
            case 4 : 
                printf("%s\n",toString(operand));
                break ;
            case 5 : 
                printf("intValue(operand)=%d\n",intValue(operand));
                break ;
            case 6 : 
                printf("intValue(accu)=%d\n",intValue(alu.accu));
                break ;
            case 7 : 
                nand(alu,operand);
                printf("apres nand(), accu = ");
                printing(alu);
                break ;
            case 8 : 
                add(alu,operand);
                printf("apres add(), accu = ");
                printing(alu);
                break ;
            case 9 : 
                sub(cpu,operand);
                printf("apres sub(), A = ");
                printing(alu);
                break ;
            case 10 : 
                and(cpu,operand);
                printf("apres and(), A = ");
                printing(alu);
                break ;
            case 11 : 
                or(cpu,operand);
                printf("apres or(), A = ");
                printing(alu);
                break ;
            case 12 : 
                xor(cpu,operand);
                printf("apres xor(), A = ");
                printing(alu);
                break ;
            case 13 : 
                not(cpu);
                printf("apres not(), A = ");
                printing(alu);
                break ;
            case 14 : 
                opp(cpu);
                printf("apres opp(), A = ");
                printing(alu);
                break ;
            case 15 : 
                shift(alu);
                printf("apres shift(), A = ");
                printing(alu);
                break ;
            case 16 : 
                printf("Entrez un entier :") ;
                scanf("%d",&integer);
                logicalShift(cpu,integer);
                printf("apres logicalshift(%d), A = ",integer);
                printing(alu);
                break ;
            case 17 : 
                mul(cpu,operand);
                printf("apres mul(), A = ");
                printing(alu);
                break ;
            default : 
                printf("Choix inexistant !!!!\n");
                printf("%s",menu);
        }
    }
    return 0;
}



