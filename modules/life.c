#include <stdio.h>
#include <stdlib.h>
#include "ADTMap.h"
#include "ADTVector.h"
#include "ADTList.h"
#include <string.h>
#include <limits.h>

int compare_ints(Pointer a, Pointer b) {
	return *(int*)a - *(int*)b;
}
char* create_char(char ch){
    char* pointer=malloc(sizeof(char));
    *pointer=ch;
    return pointer;
}
int* create_int(int value) {
	int* pointer = malloc(sizeof(int));	// δέσμευση μνήμης
	*pointer = value;					// αντιγραφή του value στον νέο ακέραιο
	return pointer;
}

//Σε αυτο το struct ειναι οι συντεταγμένες του καθε cell

typedef struct {
	int x;      //key
    int y;      //value
} LifeCell;

//Μια δομη που κραταει δύο map και μια μεταβλητή destroy που καθορίζει πότε πρεπει να κάνουμε destroy
struct lifestate{
    Map map1;
    Map map2;
};

typedef struct lifestate* LifeState;

//Δημιουργω ενα map στο ενα μελος της δομης και το δευτερο μελος το αρχικοποιω με NULL,αρχικα.
//Αυτο θα αλλάξει στην συνέχεια οσο προστιθονται στοιχεια.Σκοπος ειναι να εχουμε ενα map
//οπου καθε στοιχειο του map να εχει σαν τιμη,εναν δεικτη σε ενα αλλο map
//Αυτό προσομοιάζει έναν δισδιάστατο πίνακα όπου το key του map1 ειναι ο αριθμος της γραμμής
//ενώ το key του map2 ειναι ο αριθμος της στηλης

LifeState life_create(){
    LifeState life=malloc(sizeof(struct lifestate));
    life->map1=map_create(compare_ints,free,NULL);
    life->map2=NULL;
    return life;
}

// Δημιουργεί μία κατάσταση του παιχνιδιού με βάση τα δεδομένα του αρχείο file (RLE format)
LifeState life_create_from_rle(char* file){
    FILE* rle=fopen (file,"r");                             //για να ανοιξει το αρχειο
    LifeState life=life_create();
    int i=0,j=0,val=0;
    char ch;
    while((ch=getc(rle))!='!'){                             //Διαβάζω απο το αρχείο μέχρι να βρω '!'
        if(ch=='o'){
            do{
                if(life->map2==NULL) //Αν δεν έχει δημιουργηθεί map στο αντίστοιχο struct.
                    life->map2=map_create(compare_ints,free,free);
                map_insert(life->map2,create_int(j),create_int(1));
                map_insert(life->map1,create_int(i),life->map2);
                j++;
                val--;
            }while(val>0);
            val=0;
        }
        else if(ch=='b'){    //Δεν χρειάζεται να αποθηκεύω τα ''νεκρα'' κελία,αλλά μόνα τα ζωντανά
            do{         //καθώς μόνο απο αυτά επηρεάζονται τα υπόλοιπα
                j++;
                val--;
            }while(val>0);
            val=0;
        }
        else if(ch=='$'){   //Aν βρεθεί αλλαγή γραμμής θα πρέπει να αυξηθεί το i
            do{             //που συμβολίζει τις σειρές,eνω το j που συμβολίζει τις στήλες
                val--;      //θα πρέπει να μείνει 0,γιατι θα υποδεχθεί νεο στοιχείο
                i++;
                j=0;
            }while(val>0);
            val=0;
            life->map2=NULL;
        }
        else if (ch >= '0' && ch <= '9')   //Ωστε να υπολογίζει δυψηφιους αριθμούς,καθως με την getc διαβάζει ascii
            val = 10*val + (ch-'0');    //πχ στην περίπτωση που εχω '4 ο' αντι για 'οοοο'
        else{
            while(ch!='\n')                 //Aν αρχίζει απο # πρέπει να παραληφθεί η γραμμή καθως είναι σχολια
                ch=getc(rle);}
        }
        fclose(rle);                    //κλεισιμο του αρχειου
    return life;
}

// Επιστρέφει την τιμή του κελιού cell στην κατάσταση state (true: ζωντανό, false: νεκρό)

bool life_get_cell(LifeState state, LifeCell cell){
    Map map;
    int* number;
    int* p=create_int(cell.x);

    map=map_find(state->map1,p);
    if(map==NULL){      //Αν δεν βρέθηκαν οι συντεταγμένες στην δομή
        free(p);
        return false;   //κάθε κελί που δεν υπάρχει στην δομή θα είναι νεκρό
    }
    free(p);
    p=create_int(cell.y);
    number=map_find(map,p);
    if(number==NULL){
        free(p);
        return false;
    }
    free(p);
    return *number;

}
void life_set_cell(LifeState state, LifeCell cell, bool value){
                                         //Και εδώ δεν χρειάζεται να εισάγουμε τα cells που δεν ειναι ζωντανα
    int* p=create_int(cell.x);
    Map map=map_find(state->map1,p);

    if(map!=NULL){      //Aν πάω να εισάγω cell σε θέση που υπήρχε παλία στην δόμη.
        if(value==1)    
            map_insert(map,create_int(cell.y),create_int(value));
        else            //Αν πρέπει να αλλάξω ένα ζωντανό cell σε νεκρό,πρέπει να το αφαιρέσω απο την δομή,
            map_remove(map,&(cell.y));  //αφου στην δομή αποθηκευονται μόνο τα ζωντανά cell.
    }
    else{               //Αν εισάγω σε θέση που δεν υπήρχε στην δομή
        if(value==1){   //Αν εισάγω ζωντανό cell θα π΄ρέπει να δημιουργήσω τον αντίστοιχο χώρο
        state->map2=map_create(compare_ints,free,free);
        map_insert(state->map2,create_int(cell.y),create_int(value));
        map_insert(state->map1,create_int(cell.x),state->map2);}
    }
	free(p);
}
//Μια απλή συναρτηση που μετραει το πλήθος των ζωντανων cell σε 8 γειτονικα
int life_count(LifeState state,LifeCell cell){
    int count=0;
    int k,l;
    k=cell.x;
    l=cell.y;
    for(int i=k-1;i<=k+1;i++){
        
        for(int j=l-1;j<=l+1;j++){
            if(i==k && j==l)        //Οταν βρέθουν οι συντεταγμένες του cell δεν θα πρέπει
                continue;           //να μέτρηθει,καθώς μετράω μόνο τα 8 γύρω του
            cell.x=i;
            cell.y=j;
            count+=life_get_cell(state,cell);
            
        }
    }
    return count;

}

// Καταστρέφει την κατάσταση ελευθερώντας οποιαδήποτε μνήμη έχει δεσμευτεί
void life_destroy(LifeState state){
    //καταστρέφει κάθε map ''στηλη'',δηλαδη το map2 οπου υπάρχει
    for(MapNode node=map_first(state->map1);node !=MAP_EOF;node=map_next(state->map1,node)){
         Map m=map_node_value(state->map1,node);
         map_destroy(m);
        }
    //καταστρέφει τον map 'γραμμη' και ελευθερώνει και τον χώρο του δείκτη στο τέλος
	map_destroy(state->map1);
        free(state);
}
// Παράγει μια νέα κατάσταση που προκύπτει από την εξέλιξη της κατάστασης state
LifeState life_evolve(LifeState state){
    
    LifeState state2=life_create();     //δημιουργώ ενα καινουριο state μέσα στο οποίο θα αποθηκεύσω το νεο state
    int count=0;        //μετρητης γειτονων
    LifeCell cell;      //ωστε να μετραω τα γειτονικα κυταρα

    //Στο παρακάτω τμήμα ''σαρώνονται'' όλα τα μέλη της δόμης,ώστε να γίνει το evolve
    for(MapNode node=map_first(state->map1);node !=MAP_EOF;node=map_next(state->map1,node)){
        Map m=map_node_value(state->map1,node);
        for(MapNode node2=map_first(m);node2!=MAP_EOF;node2=map_next(m,node2)){
            int* key1=map_node_key(state->map1,node);   //αποθηκεύω τον αριθμό της γραμμής και της στήλης
            int* key2=map_node_key(m,node2);
            for(int i=*key1-1;i<=*key1+1;i++){      //Ψάχνω τους γειτονες σε κάθε γειτονικο κελι του κελιου που είμαι
                for(int j=(*key2)-1;j<=*key2+1;j++){    //στα όρια των συντεταγμένων μου
                    cell.x=i;
                    cell.y=j;
                    count=life_count(state,cell); 

                    if((count<2 || count>3) && life_get_cell(state,cell)==1)
                        life_set_cell(state2,cell,0);
                    else if(count==3 && life_get_cell(state,cell)==0) 
                        life_set_cell(state2,cell,1);
                    else
                        life_set_cell(state2,cell,life_get_cell(state,cell)); 
                }   
            }   
           

        }
    }
    return state2;  //επιστρέφω την νέα κατάσταση
}

//Δημιουργεί έναν vector απο τον κώδικο του RLE.
//Συνάρτηση που χρησιμεύει στην σύγκριση δυο καταστάσεων ,καθως και στην αποθηκευση κατάστασης σε RLE
Vector rle_format(LifeState state){
//Θα πρέπει να βρω το μικρότερο αριθμό που αποθήκευεται 1 ώστε να ξεκινήσει απο εκει η καταμέτρηση
    int* min_y;
    int min=INT_MAX;
    Vector vec=vector_create(0,free);   //Vector που θα χρησιμοποιηθέι για την αποθήκευση του format

    MapNode node=map_first(state->map1);
    if(node==MAP_EOF){                  //σε περίπτωση που καταλήξαμε σε κενή κατάσταση
        vector_insert_last(vec,create_char('!'));
        return vec;
    }
    for(node=map_first(state->map1);node !=MAP_EOF;node=map_next(state->map1,node)){
        Map m=map_node_value(state->map1,node);
        for(MapNode node2=map_first(m);node2!=MAP_EOF;node2=map_next(m,node2)){
            min_y=map_node_key(m,node2);
            if(*min_y<=min)
                min= *min_y;
        }
    }
    node=map_first(state->map1);
    int* min_x=map_node_key(state->map1,node);
    int differ=*min_x;
    int* j;
    for(MapNode node=map_first(state->map1);node !=MAP_EOF;node=map_next(state->map1,node)){
        Map m=map_node_value(state->map1,node);
        min_x=map_node_key(state->map1,node);       //to min tis ''seiras''
        for(int j=differ-*min_x;j<0;j++)
            vector_insert_last(vec,create_char('$'));
        for(MapNode max=map_first(m);max!=MAP_EOF;max=map_next(m,max))
            j=map_node_key(m,max);
        for(int i=min;i<=*j;i++){
            LifeCell cell;
            cell.x= *min_x;
            cell.y=i;
            if(life_get_cell(state,cell)==0)
                vector_insert_last(vec,create_char('b'));
            else if(life_get_cell(state,cell)==true){
                vector_insert_last(vec,create_char('o'));
            }
        }
        differ=*min_x;
    }
    vector_insert_last(vec,create_char('!'));
    return vec;

}

// Αποθηκεύει την κατάσταση state στο αρχείο file (RLE format)
//Eιναι σχεδιασμένο έτσι,ωστε να αποθηκευει την κατάσταση μόνο και όχι και την σχετική του θέση σε σχέση με την προηγούμενη
void life_save_to_rle(LifeState state, char* file){
    FILE* rle=fopen(file,"w");
    Vector vec=rle_format(state);
    char* prev=vector_get_at(vec,0);
    int count=0;
    char* ch;
    for(int i=0;i<=vector_size(vec)-1;i++){
        ch=vector_get_at(vec,i);
        if(*ch==*prev){
            count++;
            *prev=*ch;
        }else{
            if(count>1)
                fprintf(rle,"%d%c",count,*prev);
            if(count==1)
                fprintf(rle,"%c",*prev);
            *prev=*ch;
            count=1;
        }
    }   
    fprintf(rle,"!");
    vector_destroy(vec);
    fclose(rle);
}
//Παίρνει σαν όρισμα έναν vector απο χαρακτήρες και τον κάνει συμβολοσειρα 
char* make_string(Vector vec){
    char* s=malloc((vector_size(vec)+1)*sizeof(char));
    int i=0;
    for(i=0;i<vector_size(vec);i++){
        char* character=vector_get_at(vec,i);
        s[i]=*character;
    }s[i]='\0';
    vector_destroy(vec);
    return s;
}

int compare_states(Pointer a,Pointer b){
    return strcmp(a,b);}
//H παρακάτω συνάρτηση συγκρίνει αν βρήκε διαφορετική κατάσταση,αλλα με ίδιο format
//Η βασική ιδέα είναι πως δυο states με ολοιδιο format ε΄ίναι ίδια αν έχουν κοινή αρχή
//Σε κάθε άλλη περίπτωση δυο states με ίδιο format δεν είναι ίδια,αλλα το ένα είναι μετατοπισμένο σε
//σχέση με το άλλο. 
int compare_s(Pointer a,Pointer b){
    LifeState state1=a;
    LifeState state2=b;
    MapNode node1,node2;
    node1=map_first(state1->map1);
    node2=map_first(state2->map1);
    int* pt1=map_node_key(state1->map1,node1);
    int* pt2=map_node_key(state2->map1,node2);
    if(*pt1!=*pt2)  //Αν τα key του map1 είναι διαφορετικα δεν εχει νόημα να κοιτάξω παρακάτω
        return 1;   //Οποτε δεν είναι ίδια επιστρέφω 1 ,δεν με ενδιαφέρει η σχέση διαταξης
    Map m1=map_node_value(state1->map1,node1);
    Map m2=map_node_value(state2->map1,node2);
    node1=map_first(m1);
    node2=map_first(m2);
    pt1=map_node_key(m1,node1);
    pt2=map_node_key(m2,node2);
    if(*pt1!=*pt2)
        return 1;
    else
    return 0;       //Αν βρω κοινή αρχη επιστρέφω 0,δηλαδή οτί είναι ίδια κατάσταση.

    
}

// Επιστρέφει μία λίστα από το πολύ steps εξελίξεις, ξεκινώνας από την κατάσταση
// state. Αν βρεθεί επανάληψη τότε στο *loop αποθηκεύεται ο κόμβος στον οποίο
// συνεχίζει η εξέλιξη μετά τον τελευταίο κόμβο της λίστας, διαφορετικά NULL

List life_evolve_many(LifeState state, int steps,ListNode* loop){
    *loop=NULL;     //αν δεν βρεθεί επανάληψη,επιστρέφει NULL στη θέση του loop
    bool cont=true;
    Vector vec;
    List list=list_create((DestroyFunc)life_destroy);
    ListNode node=LIST_BOF;
    Map m=map_create(compare_states,free,NULL);
    vec=rle_format(state);
    char* s=make_string(vec);
    if(strcmp(s,"!")==0){                    //Αν υπάρχει κενή κατάσταση
        free(s);
        life_destroy(state);
        map_destroy(m);
        return list;}
    list_insert_next(list,node,state);      //Βάζω στην λίστα την αρχική κατάσταση
    node=list_last(list);                   //Για να εισάγω πάντα στο τέλος
    map_insert(m,s,state);                  //Βάζω στο map την αρχική κατάσταση
    for(int i=1;i<steps;i++){               //Βάζω στην λίστα τις υπόλοιπες καταστάσεις
	    state=life_evolve(state);
//Οταν το cont ειναι false σημαίνει πως βρέθηκε μια μετατοπισμένη κατάσταση.Οποτε δεν είναι ανάγκη πλέον να ει΄σαγω 
//στο map για να δω αν έχω ξαναβρεί παρομοια κα΄τάσταση απο εδώ και πέρα.Αυτό διότι πλέον θα επαναληφθούν ξάνα αυτά
//που έγιναν πριν,αλλά μετατοπισμένα. Επομένως αφού δεν είχαμε πριν loop δεν θα έχουμε ούτε τώρα
        if(cont==true){
            vec=rle_format(state);              //Φτιάχνω το format του state
            char* s=make_string(vec);
            if(strcmp(s,"!")==0){                //Αν υπάρχει κενή κατάσταση
                free(s);
                map_destroy(m);
                life_destroy(state);
                return list;}
            LifeState state1=map_find(m,s);     //Ψάχνω να βρω αν υπήρχε μέσα στον map αυτό το format
            if(state1!=NULL){                   //Αν βρέθει ίδιο format θα πρέπει να ελεγχθεί αν ειναι ίδια και η κατάσταση
                if(compare_s(state,state1)==0){ //Αν είναι ίδια και η καταση απόθηκευω τον κόμβο που θα
                    ListNode node=list_find_node(list,state1,compare_s);    //συνεχίσει η επανάληψη
                    *loop =node;
                    map_destroy(m);
                    return list;
                }
                cont=false;
            }
            map_insert(m,s,state); 
        }   //Αν δεν υπάρχει ίδια κατάσταση συνέχιζει κανονικά,ακόμα και αν υπάρχει ίδιο format.
	list_insert_next(list,node,state);
    node=list_last(list);
	}
	map_destroy(m);
    return list;
}
