#include "lz77.h"

unsigned char buffer[SB_SIZE];
unsigned char lookahead[LA_SIZE];
int la_size;    // actual lookahead size

int main()
{
	// vars
	int i, ret;
	FILE *file;
    unsigned char c;
	struct token *t = NULL;
    int sb_index = 0, la_index = 0;
    la_size = LA_SIZE;
	
	// open the file
	file = fopen("./abc.txt", "r");
	if(file == NULL){
		printf("Error opening file.\n");
		exit(1);
	}
	
    for(i = 0; i < LA_SIZE; i++){
        ret = fread(&c, 1, 1, file);
        if(feof(file) == 0 && ret != 0)
            lookahead[i] = c;
        else
            break;
    }
    
    for(i = 0; i < LA_SIZE; i++)
        printf("%c ", lookahead[(la_index+i)%LA_SIZE]);
    
    t = match(sb_index, la_index);
    printf("\n<%d, %d, %c>\n\n",t->off, t->len, t->next);
    
	while(la_size > 0){
		
        for(i = 0; i < t->len + 1; i++){
            ret = fread(&c, 1, 1, file);
            if(feof(file) == 0){
                buffer[sb_index] = lookahead[la_index];
                lookahead[la_index] = c;
                sb_index = (sb_index + 1) % SB_SIZE;
                la_index = (la_index + 1) % LA_SIZE;
            }
            else{ // caso in cui EOF compaia prima del riempimento del lookahead
                buffer[sb_index] = lookahead[la_index];
                sb_index = (sb_index + 1) % SB_SIZE;
                la_index = (la_index + 1) % LA_SIZE;
                la_size--;
            }
        }
	
        for(i = 0; i < la_size; i++)
            printf("%c ", lookahead[(la_index+i)%LA_SIZE]);
        
        if(la_size > 0){
            t = match(sb_index, la_index);
            printf("\n<%d, %d, %c>\n\n",t->off, t->len, t->next);}
	}

	return 0;
}

struct token* match(int sb, int la)
{	
	struct token *t;
    int i, j, c = 0;
	
	t = (struct token*)malloc(sizeof(struct token));
	t->off = 0;
	t->len = 0;
	t->next = lookahead[la];

	
	while(c < SB_SIZE){
	   
		for(i = 0; buffer[(sb+i)%SB_SIZE] == lookahead[(la+i)%LA_SIZE]; i++){
            if((i >= la_size) || (c+i >= SB_SIZE))
                break;
		}
        
        // nel caso in cui il match continui nel lookahead buffer
        if((c+i >= SB_SIZE) && (i < la_size)){
            for(j = la; lookahead[j%LA_SIZE] == lookahead[(la+i)%LA_SIZE]; i++){
                j++;
                if(i >= la_size)
                    break;
            }
        }
		if(i > t->len){
			t->off = SB_SIZE - c;
			t->len = i;
			t->next = lookahead[(la+i)%LA_SIZE];
		}
		sb = (sb + 1) % SB_SIZE;
        c++;
	}
	
	return t;
}

