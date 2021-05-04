////////////////////////////////
//
// Υλοποίηση του pq_sort module
//
////////////////////////////////

#include <stdlib.h>

#include "pq_sort.h"
#include "ADTPriorityQueue.h"
#include <stdio.h>

void pq_sort_vector(Vector vec, CompareFunc compare) {
	// Προσοχή! Μέσα στη συνάρτηση αυτή θα χρειαστεί να αντικαταστήσουμε τα περιεχόμενα του vector. Δε
	// θέλουμε όμως όσο το κάνουμε αυτό το vector να καλέσει τη destroy! Οπότε αλλάζουμε προσωρινά τη
	// συνάρτηση destroy σε NULL (αποθηκεύοντας την παλιά τιμή).
	DestroyFunc old_destroy = vector_set_destroy_value(vec, NULL);

	
	PriorityQueue pq=pqueue_create(compare,NULL,vec);	
	int* number;
	int size=pqueue_size(pq);
	for(int i=size-1;i>=0;i--){
		number=pqueue_max(pq);
		vector_set_at(vec,i,number);
		pqueue_remove_max(pq);
		}

	// επαναφορά της destroy
	vector_set_destroy_value(vec, old_destroy);
	pqueue_destroy(pq);
}

void pq_sort_list(List list, CompareFunc compare) {

	DestroyFunc old_destroy = list_set_destroy_value(list, NULL);

	PriorityQueue pq=pqueue_create(compare,NULL,NULL);
	int* number;
	int size=list_size(list);

	for(ListNode i=list_first(list);i!=LIST_EOF;i=list_next(list,i)){		//Βαλε τα στοιζεία της λίστας σε μια pq
		int* current=list_node_value(list,i);
		pqueue_insert(pq,current);
	}
	 ListNode node=list_last(list);

	for(int i=1;i<=size;i++){		//Βάζε καθε φορα στην αρχή της λίστας αυτό που αφαιρείς απο την pq
		number=pqueue_max(pq);
		list_insert_next(list,node,number);
		list_remove_next(list,LIST_BOF);
		pqueue_remove_max(pq);
	}
	list_set_destroy_value(list,old_destroy);
	pqueue_destroy(pq);
}