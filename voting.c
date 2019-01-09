#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define CANDIDATE_TEXT_LEN 64
#define CANDIDATES_COUNT 7

typedef struct Candidate
{
    char _name[CANDIDATE_TEXT_LEN];
    
    unsigned int _vote;
    

    struct Candidate * _next;
    struct Candidate * _prev;

}Candidate;


typedef struct ElectionStats
{
    unsigned int _votes_total;
    unsigned int _votes_correct;
    unsigned int _votes_wrong;
}ElectionStats;


ElectionStats election_stats;

typedef void (*parse_line_callback)(char*); 
typedef void (*analyze_vote_callback)(int);

Candidate * p_ar_candidates_root = 0;
Candidate * p_ar_candidates = 0;


void parse_candidate(char * data_line);
void parse_vote(char * data_line, analyze_vote_callback callback );



void print_error(char * error_message)
{
    printf("%s\n",error_message);
}

void debug_message(char * message)
{
    printf("%s\n",message);
}

void debug_pointer(void * pointer)
{
    printf("%p", pointer);
}

Candidate * create_candidate(char * name, Candidate * p_prev_candidate)
{
    Candidate * p_candidate = malloc(sizeof(Candidate));
    if (p_candidate == 0)
    {
        print_error("Allocazione memoria fallita per struttura Candidate.");
        return 0;
    }else{
        printf("Allocazione eseguita : ");
        debug_pointer(p_candidate);
    }

    //copio il nome
    strcpy(p_candidate->_name, name);
    p_candidate->_vote = 0;
    p_candidate->_prev = p_prev_candidate; //puntatore al precedente candidato
    p_candidate->_next = 0; //il successivo sarà sempre 0

    if (p_prev_candidate != 0)
        p_prev_candidate->_next = p_candidate;

    return p_candidate;
}


void free_memory(Candidate * p_ar_candidates_root)
{
    Candidate * p_node = p_ar_candidates_root;
    Candidate * cursor = 0 , * tmp = 0;
    
    if (p_node != 0)
    {
        cursor = p_node;
        while (cursor != 0)
        {
            printf("Free area : ");
            debug_pointer(cursor);
            printf("\n");

            tmp = cursor->_next;
            free(cursor);
            cursor = tmp;

          
        }

    }
  
    
}

void list_candidates(Candidate * p_ar_candidate_root)
{
    Candidate * p_node = p_ar_candidate_root;

    while (p_node != 0)  {
        printf("\nNome : %s\nVoti : %d\n_addr : %p\n,_next : %p\n_prev : %p\n ---\n", 
            p_node->_name,p_node->_vote, p_node, p_node->_next, p_node->_prev);

        p_node = p_node->_next;

    };

}

Candidate * get_winner_candidate()
{
    Candidate * p_node = p_ar_candidates_root;
    Candidate * winner  = p_node;

    //trovo il valore più ALTO e lo imposto come primo elemento
    while(p_node != NULL)
    {
        
        if (p_node->_vote > winner->_vote)
            winner = p_node;

        p_node = p_node->_next;
    }

    return winner;
}


Candidate * find_another_winner(Candidate * winner)
{
    
    Candidate * p_node = winner->_next;
    
    while (p_node != NULL)
    {
        if (p_node->_vote == winner->_vote)
        {
            return p_node;
        }

        p_node = p_node->_next;
    }

    return 0;
}





Candidate * get_candidate_by_index(Candidate * p_ar_candidate_root, int index_search)
{
    Candidate * p_node = p_ar_candidate_root;
    int index = 1;
    while(p_node != NULL)
    {
        if (index == index_search)
            return p_node;

        p_node = p_node->_next;
        index++;
    }

    return 0;
    
}

//1 is valid, 0 none
unsigned short add_vote_to_candidate(int vote)
{
    Candidate * candidate = get_candidate_by_index(p_ar_candidates_root, vote);
    if (candidate != NULL)
    {
        candidate->_vote++;
        return 1;
    }

    return 0;


}


//si occupa di analizzare il voto
void analyze_vote(int voto)
{

    printf("Voto ricevuto : %d - ", voto);

    election_stats._votes_total++;
    int vote_is_valid = add_vote_to_candidate(voto);
    
    if (vote_is_valid)
        election_stats._votes_correct++;
    else
        election_stats._votes_wrong++;

}




void print_election_stats(ElectionStats * stat)
{
    printf("Number of voters : %d\n",stat->_votes_total);
    printf("Number of valid votes: %d\n", stat->_votes_correct);
    printf("Number of spoilt votes : %d\n",stat->_votes_wrong);
    printf("\n");
    printf("Candidate\tScore\n");
    printf("\n");
    

    Candidate * p_node = p_ar_candidates_root;
    while (p_node!=NULL)
    {
        printf("%s\t%d\n",p_node->_name,p_node->_vote);

        p_node = p_node->_next;
    }

    printf("\n");
    printf("The winner(s):\n");
    Candidate * winner = get_winner_candidate();
    printf("%s\n",winner->_name);

    Candidate * alt_winner = winner;
    while( (alt_winner = find_another_winner(alt_winner)) != NULL)
    {
        printf("%s\n",alt_winner->_name);
    }


}



void parse_line_callback_function(char *  data)
{
    // printf("-----\n");
    // printf("linea ricevuta :%s\n",data);

    //comtrllo se è il nome di un candidato o una stringa di voto
    if (isdigit(data[0]))
    {
        parse_vote(data, &analyze_vote);
    }
    else
    {

        parse_candidate(data);
   
        
    }
    printf("-----\n");
}


void parse_candidate(char * data_line)
{
    char buffer[64];
    sscanf(data_line, "%[^\n]",buffer);
    printf("%s\n", buffer);


    //creiamo un candidato
    unsigned short is_first_node = 0;
    if (p_ar_candidates == 0) is_first_node = 1;
    p_ar_candidates = create_candidate(buffer,p_ar_candidates);
    if (is_first_node) p_ar_candidates_root = p_ar_candidates;
}

unsigned short is_last_vote(char * data_line)
{

    unsigned int data_line_len = strlen(data_line);
    unsigned short res = 0;

    if (data_line_len >= 1)
    {
        char * ptr = strstr(data_line," ");
        if (ptr == 0)
            res = 1;

    }

    return res;
}

void parse_vote(char * data_line, analyze_vote_callback callback )
{
    //la fine dei voti è indicata con uno 0
    
    int parsed_count;
    int cursor = 0;
    char voto_buffer[6];
    int voto;
    int data_line_len = strlen(data_line);
    unsigned short is_last_vote_of_the_string = 0;
    do{
        
        char * data_temp = data_line + cursor;
        parsed_count = sscanf(data_line + cursor, "%s ",voto_buffer);  
        int voto_buffer_len = (strlen(voto_buffer) + 1);

        
        if (!is_last_vote(data_temp))
            cursor += voto_buffer_len;
        else
            is_last_vote_of_the_string = 1;
        
        

        if (cursor >= data_line_len)
            break;
        
        voto = atoi(voto_buffer);
        //printf("Voto : %d\n", voto); 

        if (voto != 0)
        {
            callback(voto);
        }
        else
        {
            break;
        }
        
        

    }while(parsed_count != 0 && is_last_vote_of_the_string == 0);
    
}


void parse_file_line_by_line(char * file, parse_line_callback callback )
{
    char c[128];
    int cursor = 0;

    FILE *p_file;
    p_file = fopen(file,"r");


    if(p_file == NULL)
    {
      printf("Error!");   
      exit(1);             
    }


    // int pos = 0;
    // fseek(p_file,0,SEEK_END);
    // int len_file = ftell(p_file);
    // printf("len file : %d",len_file);
    char ch;
    while( (ch = getc(p_file)) != EOF)
    {
        if (cursor < (128 - 1))
        {
            if (ch != 10 && ch != 13)
            {

                c[cursor] = ch;
                cursor++; 
            }
            else
            {
                c[cursor] = '\0';
                callback(c);

                cursor = 0;
            }
    
        }
    }

    fclose(p_file); 
}



int main(int argc, char ** argv)
{
    printf("Esercizio candidati ... \n");

    // for (int i=0; i<7; i++)
    // {
        
    //     p_ar_candidates = create_candidate("Daniele Ciarapica", p_ar_candidates);
    //     if (i==0) p_ar_candidates_root =p_ar_candidates; //imposta la root sul primo elemento creato
    // }

    parse_file_line_by_line("votes.txt",&parse_line_callback_function);
    
    list_candidates(p_ar_candidates_root);

    print_election_stats(&election_stats);

    printf("\n");
    free_memory(p_ar_candidates_root);

/**
 * Debug
 * */
    // p_ar_candidates = create_candidate("DAX1",p_ar_candidates); //ora punta al primo elemento
    // p_ar_candidates_root = p_ar_candidates;

    // //secondo elemento
    // p_ar_candidates = create_candidate("DAX2",p_ar_candidates);

    // //terzo elemento
    // p_ar_candidates = create_candidate("DAX3",p_ar_candidates);


    // list_candidates(p_ar_candidates_root);

    // free_memory(p_ar_candidates_root);

}