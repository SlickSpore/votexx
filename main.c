#include <stdio.h>
#include <stdlib.h>
#include <string.h>

    /*

    *******************************
    ***     === VOTEX V1 ===    ***
    *******************************

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

    @slickspore

    */


#define LOWEST_SUFFICIENT_AVERAGE 6
#define LOWEST_POSSIBLE_VOTE 1

#define MEAN 1
#define FIND 2
#define WMEAN 3
#define WFIND 4

typedef struct {
    float v;
    int w;
}WMark_t;

/* Prompts a simple help menu */
void print_usage(){
    printf("=== VOTEX: MARKS AND AVERAGE EVALUATOR === \n");
    printf("usage: \tvotex\t[mean [votex, ..., votey]]\n\t\t[find [votex, ... votey]]\n\t\t[wmean [(votex, w), ..., (votey, w)]]\n\t\t[wfind [weight] (votex, w), ..., (votey, w)]]\n");
    exit(0);
}

/* Parses the first argument to check the operating mode */
int parse_mode(int c, char *argv[]){

    char *mode = *(argv+1);

    if (c == 1){
        print_usage();
    }
    if (!strcasecmp("mean", mode)){
        return MEAN;
    }
    if (!strcasecmp("find", mode)){
        return FIND;
    }
    if (!strcasecmp("wmean", mode)){
        return WMEAN;
    }
    if (!strcasecmp("wfind", mode)){
        return WFIND;
    }

    printf("Mode invalid or no mode specified!\n");
    exit(-1);
}

/* Retrieves all the unweighted votes/marks from the terminal */
float *get_unweighted_votes(int c, char *votes[]){
    float *marks = malloc(c*sizeof(float));
    float *mark = marks;

    for (int i = 0; i < c; i++){
        *mark = atof(*(votes+2+i));
        ++mark;
    }

    return marks;
}

/* Does a simple sum of unweighted votes -> Should I do the same with the weighted ones? */
float sum(float *__numbers, int __tot){
    float __sum = *__numbers;

    for (int i = 1; i < __tot; i++){
        __sum += *(__numbers+i);
    }
    return __sum;
}

/* Gets the number of votes inputed by the user */
int get_vts_no(int argc, int mode){
    if (argc == 2){     //checks if any vote is specified
        printf("Error: No valid votes Specified!\n");
        exit(-1);
    }
    if (mode == MEAN || mode == FIND || mode == WMEAN){
        return argc - 2;
    }
    return argc - 3;
}

/* Divides each argument in vote-weight format to each corrisponding weight and vote */
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

/* Converts each argument in vote/weight format and returns it */
WMark_t *get_weighted_votes(int c, char *votes[], int pos){
    WMark_t *marks = malloc(c*sizeof(WMark_t));
    WMark_t *mark = marks;
    float vote;
    int weight;

    for (int i = 0; i < c; i++){
        if (split_weight(votes[i+pos], &weight, &vote)){
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

/* Finds the right unweighted vote to keep the average up the threshold */
float unweighted_vote_to(int c, char *votes[]){
    float votes_sum = sum(get_unweighted_votes(c, votes), c);
    return LOWEST_SUFFICIENT_AVERAGE*(c+1)-votes_sum;
}

/* Finds the right weighted vote to keep the average up the threshold */
float weighted_vote_to(int c, char* votes[], int expected_weight){
    int wsum = expected_weight;
    float msum = 0;

    WMark_t *marks = get_weighted_votes(c, votes, 3);

    for (int i = 0; i < c; i++){
        wsum += marks[i].w;
        msum += marks[i].v * marks[i].w;
    }
    return ((LOWEST_SUFFICIENT_AVERAGE*wsum)-msum)/expected_weight;
}

/* Does the average of the specified unweighted votes/marks */
float unweighted_mean(float *marks, int no){
    return (float) sum(marks, no) / no;
}

/* Does the average of the specified weighted votes/marks */
float weighted_mean(WMark_t *marks, int no){
    
    float mark_sum = 0;
    float wght_sum = 0;

    for (int i = 0; i < no; i++){
        mark_sum += marks[i].v * marks[i].w;
        wght_sum += marks[i].w;
    }

    return (float) mark_sum / wght_sum;
}

/* Bumping heart of this guy */
int main(int argc, char *argv[]){
    int mode = parse_mode(argc, argv);
    int mks_no = get_vts_no(argc, mode);
    float vote;
    float *marks;
    int exp_w = 100;
    WMark_t *wmarks;

    switch (mode)
    {
    case 1:
        marks = get_unweighted_votes(mks_no, argv);
        vote =  unweighted_mean(marks, mks_no);
        break;
    
    case 3:
        wmarks = get_weighted_votes(mks_no, argv, 2);
        vote =  weighted_mean(wmarks, mks_no);
        break;
    case 2:
        vote = unweighted_vote_to(mks_no, argv);
        break;
    case 4:
        exp_w = atoi(argv[2]);
        vote = weighted_vote_to(mks_no, argv, exp_w);
    default:
        break;
    }

    if ((mode%2)==0){
        if (vote > 10){
            printf("You have very little chances to be able to get a sufficient average \\: [(%.2f*%d)] \n", vote, exp_w);
            return 0;
        }
        if (vote < LOWEST_POSSIBLE_VOTE){
            printf("You shouldn't worry, you'll be fine this time! ([%.2f*%d])\n", vote, exp_w);
            return 0;
        }
        if (vote < LOWEST_SUFFICIENT_AVERAGE){
            printf("You must get at least [%.2f*%d] to be somewhat fine!\n", vote, exp_w);
            return 0;
        }
        printf("You have to get [%.2f*%d] and you'll be fine.\n", vote, exp_w);
        return 0;
    }

    printf("Your average is [%.2f].\n", vote);

    return 0;
}