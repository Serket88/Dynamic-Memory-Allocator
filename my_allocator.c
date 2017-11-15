/* 
Jessica Fang
Colin Banigan
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <cassert>
#include "my_allocator.h"

using namespace std;

Addr mem_ptr;
Addr* free_ptrs;
int list_size;
int block_size;
int header_size;
int adjuster;
//use log2(n) <-- returns a double
unsigned int init_allocator(unsigned int _basic_block_size, unsigned int _length){ //STEP 1
// ??????????
//**NOTE** location of pointers in the free list will be 2^(i + log_2(block size)))

	//TODO
	/*The function returns the amount of 
   memory made available to the allocator. If an error occurred, 
   it returns 0.*/ 

	block_size = _basic_block_size;
	
	adjuster = (int) log2(block_size) - 1;
	
	list_size = (int) log2(_length / block_size) + 2; //do we need this Ceiling'd or not, should we start at 0? -- 2/19/16 
	
	header_size = sizeof(header); //should work, if not, ask Austin for bit arithmetic help
	
	int should_allocate = (int)pow(2, (int)log2(_length) + adjuster);
	
	mem_ptr = (char*) malloc(should_allocate); //(char*) malloc(_length + (_length / block_size) * header_size);
	
	//now need to make the free pointer list, it's going to be an array of linked lists
	free_ptrs = (Addr*)malloc(list_size*sizeof(header*));
	
	cout << list_size << "\n";
	//pre-split all the linked lists?
	/*for(int i = 0; i < list_size; i++) {
		for(int j = 0; j < pow(2,i); j++){
			//cout << i << " " << j << "\n";
			free_ptrs[i][j] = NULL;
		}
	}*/
	
	header* h = (header*) mem_ptr; //creating initial header for our memory
	h->empty = true; // 0 = empty
	h->next = NULL;
	h->size = (should_allocate - header_size);
	
	free_ptrs[list_size-1] = (Addr) h; //storing the largest block size on the last row. smaller block sizes on top
	
		
	//if (h->size > something) return 0;
	
	return h->size;
}

int release_allocator(){ //LAST STEP WITH OTHER
	free(mem_ptr);
    return 0;
}

void print_list() {
	header* temp;
	for(int i = list_size - 1; i >= 0; i--){
	    temp = (header*)free_ptrs[i];
	    cout<<"{ ";
	    while(temp){
		    cout << ((header*)temp)->size << " "; 
		    temp = ((header*)temp)->next;
	    }
	    cout<<"}"<<endl;
	}
	cout<<endl;
}

bool every_above_empty(int index){
	if(index == list_size - 1) return free_ptrs[index] == NULL;
	else{
		return free_ptrs[index] == NULL && every_above_empty(index+1);
	}
}

void block_split(int index){
	//cout << "split at " << index << endl;
	
    if(index > list_size - 1){
		return;
    }
    
    Addr tmp = free_ptrs[index];
    
	if(tmp == NULL){
		block_split(index+1);
	}else{
		Addr tmp = free_ptrs[index];
		Addr hold_this = (Addr)((header*) tmp)->next;
    	
    	int new_size = (int)pow(2, (index+adjuster));
		header* other_half = (header*)tmp + header_size + new_size;
		other_half->size = new_size - header_size;
		other_half->next = NULL;
		other_half->empty = true;
		
		((header*) tmp)->next = other_half;
		((header*) tmp)->size = new_size - header_size;
		((header*) tmp)->empty = true;
		
		free_ptrs[index-1] = tmp;
		free_ptrs[index] = hold_this;
		
		//print_list();
		
	}
}

extern Addr my_malloc(unsigned int _length){ //STEP 2
	
	printf("\n\n -- allocating: [%i]\n\n",_length);
	
	Addr for_user = NULL;
	
	int row_to_use = 0;
	for(; row_to_use < list_size; row_to_use++){
		if(pow(2,row_to_use+adjuster+1)-header_size > _length) break;
	}
	cout << _length << " will be in " << row_to_use <<endl;
	
	while(free_ptrs[row_to_use] == NULL && row_to_use < list_size-1){
		if(every_above_empty(row_to_use+1)) break;
		block_split(row_to_use);
	}
	
	if(free_ptrs[row_to_use] != NULL){
		for_user = free_ptrs[row_to_use];
		Addr hold_this = (Addr)((header*) for_user)->next;
		
		((header*) for_user)->next = NULL;
		((header*) for_user)->empty = false;
		
		free_ptrs[row_to_use] = hold_this;
		
		print_list();
		
		return for_user + header_size;
	}else{
		cout << "no memes" << endl;
		exit(EXIT_FAILURE);
	}
	
 	return malloc((size_t)_length);
}

void join_left(Addr a, Addr b){ //TODO IS THIS LOGICALLY CORRECT??
	//always put left buddy in a
	//keep header for a
	//change size
	
	int new_size = 2*((header*)a)->size + header_size;
	header* hold_next = ((header*)a)->next;
	((header*)a)->size = new_size;

	//find where block with new size belongs with a for loop
	int new_index = 0;
	for(; new_index < list_size; new_index++){
		if(new_size == pow(2,new_index+adjuster+1)-header_size) break;
	}
	
	header* search = (header*)free_ptrs[new_index + 1];
	if(search == NULL || search == (header*)a) search = hold_next;
	else{
		while(search->next != NULL && search->next != (header*)a){
			search = search->next;
		}
		search->next = hold_next;
	}
	
	search = (header*)free_ptrs[new_index];
	if(search == NULL) search = (header*)a;
	else{
		while(search->next != NULL){
			search = search->next;
		}
		search->next = (header*)a;
	}
	
	//TODO recursively check if new joined node has a buddy
	//if(!check_buddy(a)) search->next = (header*)a;
	
}

void join_right(Addr a, Addr b){ //TODO IS THIS LOGICALLY CORRECT??
	//always put left buddy in a
	//keep header for a
	//change size
	int new_size = 2*((header*)a)->size + header_size;
	header* hold_next = ((header*)b)->next;
	((header*)a)->size = new_size;

	//find where block with new size belongs with a for loop
	int new_index = 0;
	for(; new_index < list_size; new_index++){
		if(new_size == pow(2,new_index+adjuster+1)-header_size) break;
	}
	
	cout << "put into " << new_index << endl;
	
	header* search = (header*)free_ptrs[new_index + 1];
	if(search == NULL || search == (header*)b) search = hold_next;
	else{
		//assert(search > (header*)0x100);
		while(search->next != NULL && search->next != ((header*)b)){
			//assert(search > (header*)0x100);
			search = search->next;
		}
		search->next = hold_next;
	}
	
	search = (header*)free_ptrs[new_index];
	if(search == NULL) search = (header*)a;
	else{
		while(search->next != NULL){
			search = search->next;
		}
		search->next = (header*)a;
	}
	
	//TODO recursively check if new joined node has a buddy
	//if(!check_buddy(a))  search->next = (header*)a;
	
}

void check_buddy(Addr a){ //** IMPORTANT ** WE'RE ASSUMING a is the whole block not the block without the header
	int hold_size = header_size + ((header*)a)->size;
	int difference = ((char*)a - (char*)mem_ptr)/hold_size;
	
	if (difference % 2 == 0){ //even node
		//this is the left and we need to check the right
		header* right = (header*)a + hold_size;
		if((right->size + header_size) != hold_size || !right->empty){
			cout << "right has been split or is not empty and therefore cannot be joined"<<endl;
			
			//put the thing into the free list where it belongs
			int index = 0;
			for(; index < list_size; index++){
				if(hold_size == pow(2,index+adjuster+1)) break;
			}
			
			//cout << "put it back " << index <<endl;
			
			header* search = (header*)free_ptrs[index];
			if(search == NULL){
				search = (header*)a;
			}else{
				while(search->next != NULL){
					search = search->next;
				}
				search->next = (header*)a;
			}
			
		}else{
			join_right(a, (Addr)right);
		}
		
		
		
	} else { //odd node
		//this is the right and we need to check the left
		header* left = (header*)a - hold_size;
		if(left->size + header_size != hold_size || !left->empty){
			cout << "left has been split or is not empty and therefore cannot be joined"<<endl;
			
			//put the thing into the free list where it belongs
			int index = 0;
			for(; index < list_size; index++){
				if(hold_size == pow(2,index+adjuster+1)) break;
			}
			
			cout << "put it back " << index <<endl;
			
			header* search = (header*)free_ptrs[index];
			if(search == NULL){
				search = (header*)a;
			}else{
				while(search->next != NULL){
					search = search->next;
				}
				search->next = (header*)a;
			}
			
			print_list();
			
		}else{
			join_left((Addr)left, a);
			
		}
	}
	
}

extern int my_free(Addr _a){ //LAST STEP WITH OTHER
	//printf("Colin is dumb\n");
	//header* delete_me = (header*) _a;
	//delete_me->empty = '0'; //mark node as empty
	//int tmp_size = delete_me->size + header_size; //get size and use it to find which i its supposed to go in?
	
	_a = _a - header_size;
	
	printf("\n\n -- freeing: [%i]\n\n", ((header*)_a)->size + header_size);
	
	print_list();
	
	((header*)_a)->empty = true;
	check_buddy(_a); //inside of check_buddy(_a), join will be called once we know if it is a left or a right node
	
	
	//free(_a);
	return 0;
}