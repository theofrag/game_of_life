/////////////////////////////////////////////////////////////////////////
//
//  Δημιουργία ενός GIF χρησιμοποιώντας τη βιβλιοθήκη libbmp
//
///////////////////////////////////////////////////////////////////////// 

#include "bmp.h"
#include "gif.h"
#include "life.h"
#include "ADTMap.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[]) {

	//default τιμες των οριων αν δεν δωθουν τα καταλληλα ορίσματα
	int top=-100;		//Aρνητικο,γιατι ο θετικος αξονας αρχιζει απο την πανω αριστερα μερια της οθονης
						//Μετατοπιζω ομως τους αξονες στο κεντρο,αρα το top θα ειναι αρνητικο
	if(argc>=3) top=-atoi(argv[2]);

	int left=-100;
	if(argc>=4) left=atoi(argv[3]);

	int bottom=100;			//Αντίστοιχα αυτο πρέπει να είναι θετικό
	if(argc>=5) bottom=-atoi(argv[4]);
	
	int right=100;			
	if(argc>=6) right=atoi(argv[5]);

	int zoom=1;			//default zoom
		if(argc >=8) zoom =round(atof(argv[7]));	
	LifeState life;

	char* name_of_file="test.RLE";		//default test

	if(argc>=2){
		name_of_file=malloc(sizeof(char)*(strlen(argv[1])+1));
		strcpy(name_of_file,argv[1]);
	}

	int speed=1;					//default speed
		if(argc>=9) speed=atoi(argv[8]);
	
    life=life_create_from_rle("test.RLE");		//Δημιουρ΄γώ την αρχική κατάσταση απο το αρχείο .RLE

	int width=(right-left)*zoom;
	int height=(bottom-top)*zoom;
	// Δημιουργία ενός GIF και ενός bitmap στη μνήμη
	GIF* gif = gif_create(width, height);		//πλατος x υψος
	Bitmap* bitmap = bm_create(width, height);

	// Default καθυστέρηση μεταξύ των frames, σε εκατοστά του δευτερολέπτου
	gif->default_delay = 5;
	if(argc>=10)	//Αν δοθει απο τον χρηστη,αλλιως ειναι default
		gif->default_delay=atoi(argv[9]);

	int frames=500;		//default frames που θα παραχθούν
	 if(argc>=7)
	 	frames=atoi(argv[6]);
	
	List list;
	ListNode loop=NULL;
	list=life_evolve_many(life,frames,&loop);
	ListNode ev=list_first(list);
	
	printf("size of list is: %d\n ",list_size(list));
	for(int k = 0; k < frames/speed; k++) {		//frames/speed διοτι αν μεγαλωσω ταχυτητα,δεν θα πρεπει να παραξω περισσοτερα frames
		// Σε κάθε frame, πρώτα μαυρίζουμε ολόκληρο το bitmap
		bm_set_color(bitmap, bm_atoi("white"));
		bm_clear(bitmap);																		
		bm_set_color(bitmap, bm_atoi("black"));		//το χρώμα του gif
		
		if(ev==NULL){				//αν το ev ειναι null επείδη κάποια στιγμή φτάσαμε σε κενή κατάσταση,σταματάμε
			bm_set_color(bitmap, bm_atoi("white"));
			bm_clear(bitmap);
			gif_add_frame(gif, bitmap);
			break;
		}
		
		LifeState life=list_node_value(list,ev);

    	for(MapNode node=map_first(life->map1);node!=MAP_EOF;node=map_next(life->map1,node)){
			Map m=map_node_value(life->map1,node);
			for(MapNode node2=map_first(m);node2!=MAP_EOF;node2=map_next(m,node2)){
				int* key1=map_node_key(life->map1,node);
            	int* key2=map_node_key(m,node2);
				int* ptr=map_node_value(m,node2);
				if(*ptr==1){		//Αν το cell είναι ζωντανο,εμφανισέ το,αν βρίσκεται μέσα στα όρια που δίνει ο χρήστης
					if(*key1*zoom>top*zoom&& *key2*zoom<right*zoom && *key1*zoom<bottom*zoom && *key2*zoom>left*zoom){
						if(zoom>1)	//Αν το zoom ειναι μεγαλύτερο απο 1,ουσιαστικά θα πρέπει να πολ/σω τους άξονες xy επι τον βαθμό του zoom
							bm_fillrect(bitmap, (*key2*zoom+width/2), (*key1*zoom+height/2), (*key2*zoom+width/2)+zoom, (*key1*zoom+height/2)+zoom);
						else	//Αν zoom=1 αρκέι να χρωματίζω ένα pixel κάθε φορά
							bm_putpixel(bitmap,*key2+width/2,*key1+height/2);
					}
				}
			}
		}
		for(int i=0;i<speed;i++){	//Αν το speed είναι 1 θα πηγαίνει στον επόμενο κόμβο αλλιως στους speed επόμενου
			ev=list_next(list,ev);	//Δηλαδή αν speed ==2 θα πρέπει να πάει 2 καταστάσεις μετα
			if(ev==LIST_EOF)		//Αν σε αυτη την διαδικασία πέσει σε LIST_EOF,ξανααρχίζουμε απο τον κόμβο που
				ev=loop;			//υπάρχει η επανάληψη,μπορεί να σημαινει όμως οτι η λίστα είναι κενη
									//και πρέπει να το ελενξουμε
		}
		// Τέλος προσθέτουμε το bitmap σαν frame στο GIF (τα περιεχόμενα αντιγράφονται)
		gif_add_frame(gif, bitmap);
	}
	// Αποθήκευση σε αρχείο
	char* gif_ex="gif_example.gif";		//default ονομα σε περιπτωση που δεν δωθει τιποτα
	if(argc==11){
		gif_ex=malloc(sizeof(char)*(strlen(argv[10])+1));
		strcpy(gif_ex,argv[10]);
	}
	gif_save(gif, gif_ex);
	// Αποδέσμευση μνήμης
	bm_free(bitmap);
	gif_free(gif);
	//Καταστροφή λίστας και όλων των στοιχείων που περιέχει
	list_destroy(list);
	printf("\nend\n");
}
