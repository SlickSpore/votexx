#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define THRESH 6
#define LOWEST 1

typedef struct {
    float v;
    int w;
}WVote_t;


void print_usage(){
    printf("=== VOTEX: MARKS AND MEAN EVALUATOR === \n");
    printf("usage: \tvotex\t[mean [votex, ..., votey]]\n\t\t[find [votex, ... votey]]\n\t\t[wmean [(votex, w), ..., (votey, w)]]\n\t\t[wfind x, y (votex, w), ..., (votey, w)]]\n");
    exit(0);
}

int parse_mode(int c, char *argv[]){

    if (c == 1){
        print_usage();
    }

    /*
        *** OPERATING MODES ***
        1 = MEAN
        2 = FIND X TO MATCH Y MEAN OF VOTES
        3 = WEIGHTED 1
        4 = WEIGHTED 2

        *** ARGS ***
        ./votex mean [votex, ..., votey]
        ./votex find x y [votex, ... votey]
        ./votex wmean [(votex, w), ..., (votey, w)]
        ./votex wfind x, y (votex, w), ..., (votey, w)]
    */

    for (int i = 1; i < c; i++){
        if (!strcasecmp("mean", argv[i])){
            return 1;
        }
        if (!strcasecmp("find", argv[i])){
            return 2;
        }
        if (!strcasecmp("wmean", argv[i])){
            return 3;
        }
        if (!strcasecmp("wfind", argv[i])){
            return 4;
        }
    }
    printf("Mode invalid or no mode specified!\n");
    exit(-1);
}

float *get_unweighted_votes(int c, char *votes[]){
    float *marks = malloc(c*sizeof(float));
    float *mark = marks;

    for (int i = 0; i < c; i++){
        *mark = atof(votes[i+2]);
        ++mark;
    }

    return marks;
}

float sum(float *__numbers, int __tot){
    float __sum = __numbers[0];

    for (int i = 1; i < __tot; i++){
        __sum += __numbers[i];
    }
    return __sum;
}

float unweighted_mean(float *marks, int no){
    return (float) sum(marks, no) / no;
}

float weighted_mean(WVote_t *marks, int no){
    
    float mark_sum = 0;
    float wght_sum = 0;

    for (int i = 0; i < no; i++){
        mark_sum += marks[i].v * marks[i].w;
        wght_sum += marks[i].w;
    }

    return (float) mark_sum / wght_sum;
}

int get_vts_no(int argc, int mode){
    if (argc == 2) mode = 0;
    if (mode){
        return argc - 2;
    }
    if (mode == 3){
        return (argc - 2)/2;
    }
    
    printf("Error: No valid votes Specified!\n");
    exit(-1);
}

int split_weight(char *arg, int *w, float *vote){
    char tmp[10];
    char *c = arg;
    char *t = tmp;
    memset(tmp, 0, 10*sizeof(char));


    while (*c != '\0'){
        if (*c == '-'){
            *vote = atof(tmp);
            ++c;
            memset(tmp, 0, 10*sizeof(char));
            t = tmp;
        }
        *t = *c;
        ++c;
        ++t;
    }
    *w = atoi(tmp);

    if(*vote < 1 || *w < 1) return 1;

    return 0;
}

WVote_t *get_weighted_votes(int c, char *votes[]){
    WVote_t *marks = malloc(c*sizeof(WVote_t));
    WVote_t *mark = marks;
    float vote;
    int weight;

    for (int i = 0; i < c; i++){
        if (split_weight(votes[i+2], &weight, &vote)){
            printf("Error: Something went wrong, check input data's spelling!\n");
            exit(-1);
        }
        mark->v = vote;
        mark->w = weight;
        vote = 0;
        weight = 0;
        ++mark;
    }


    return marks;
}

float unweighted_vote_to(int c, char *votes[]){
    float votes_sum = sum(get_unweighted_votes(c, votes), c);
    
    return THRESH*(c+1)-votes_sum;
}

int main(int argc, char *argv[]){
    int mode = parse_mode(argc, argv);
    int mks_no = get_vts_no(argc, mode);
    float vote;
    float *marks;
    WVote_t *wmarks; 

    switch (mode)
    {
    case 1:
        marks = get_unweighted_votes(mks_no, argv);
        printf("Your unweighted mean is [%.2f].\n", unweighted_mean(marks, mks_no));
        break;
    
    case 3:
        wmarks = get_weighted_votes(mks_no, argv);
        printf("Your weighted mean is [%.2f].\n", weighted_mean(wmarks, mks_no));
        break;
    case 2:
        marks = get_unweighted_votes(mks_no, argv);
        vote = unweighted_vote_to(mks_no, argv);
        if (vote < LOWEST){
            printf("You shouldn't worry, you'll be fine this time! ([%.2f])\n", vote);
            break;
        }
        if (vote < THRESH){
            printf("You must get at least [%.2f] to be fine!\n", vote);
            break;
        }
        printf("You can get [%.2f] and you'll still be fine.\n", vote);
            break;
    default:
        break;
    }

    return 0;
}