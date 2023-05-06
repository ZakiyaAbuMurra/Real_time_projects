/* Zakiya   AbuMurra  1191636 */
/* Ibraheem Dhade     1190283 */
/* Qutaiba  Olayyan   1190760 */

#include "header.h"

using namespace std ;

int greenTeam[TEAM_SIZE] ,redTeam[TEAM_SIZE]; 
int pid ;
int parentId ; 
char teamName[10];
int i ;
int speed ;
int nextTeamMember = -1 ;
map<string,int>result ;
int numberOfRounds =5 ;
int numberOfRoundsCnt =0 ; 
bool endOfRound =false ;
/*---------------------------*/
void createTeam(int) ;
void  killAllChildrens() ;
void startingTheRace() ;
int getSpeedRandomly() ;
void signal_catcher(int);
void printResults() ;
void startRound() ;
void setTeamMemberReady() ;
void signalSet() ;
/*---------------------------*/

int main (int argc, char * args[]){


    printf("%d\n",argc) ;

    if(argc > 2)
    {
        printf("error in argumnets please enter one argument only\n") ;
        exit(1) ;
    }else {
        if(args[1] != nullptr){
          if(!(numberOfRounds = atoi(args[1])))
                {
                    printf("Error in argument format,only integer allowed\n") ;
                    exit(1) ;
                }
               
        }
        printf("number of rounds : %d\n\n\n",numberOfRounds) ;
    }
    //get parent pid
    parentId = getpid() ;

    //create both teams
    createTeam(RED_TEAM) ;
    createTeam(GREEN_TEAM) ;
    sleep(3) ;

    startRound() ;


    return 0 ; 
}

//flag : the zero represent red team,1 represt green team 
void createTeam(int flag){
    
     //generate green team processes
    for(i=0;i<TEAM_SIZE;i++){
        pid = fork() ;
       
        //check for fork failure
        if(pid < 0 )
        {
            perror("fork failure") ;
            exit(1);
        }
        else if(!pid){
                
                speed = getSpeedRandomly();
               // printf("my speed: %d,cpid: %d\n",speed,getpid()) ;
                
                if(flag==1){
                    strcpy(teamName,"green"); 
                    if(i>0)
                        nextTeamMember = greenTeam[i-1] ;
                 }
                     
                else {
                    strcpy(teamName,"red") ;
                    if(i>0)
                        nextTeamMember =redTeam[i-1] ;
                }
                    
                setTeamMemberReady();
                
            }

            else {
                char team[7];
                //store children pids in parent 
                if(flag==1){
                    greenTeam[i] = pid ;
                    strcpy(team,"green");  
                }
                    
                   
                else {
                    redTeam[i] = pid ; 
                    strcpy(team,"red"); 
                }
               
            }
    }

}

//kill all childes to finish the program
void killAllChildrens(){
     //killing childs processes
    for (int i = 0; i < TEAM_SIZE; i++)
    {
       kill(greenTeam[i],9) ; 
       kill(redTeam[i],9) ;
    }
}

/* begin the race */ 
void startingTheRace(){
  
   kill(greenTeam[TEAM_SIZE-1],2); 
   kill(redTeam[TEAM_SIZE-1],2) ;
   signalSet() ;
   pause() ;
}

/* signal catch function */
void signal_catcher(int signal_number){
    if(signal_number==1){
        if(!endOfRound){
            printf("red win this round\n");
            result["red"]++ ; 
            endOfRound=true ;
           
        }
        else 
            endOfRound =false ;
    }
    
    else if(signal_number==4){
        if(!endOfRound){
            printf("green win this round\n") ;
            result["green"]++;
            endOfRound=true ;
           
        }
        else
            endOfRound=false ;
    
    }
    sleep(5) ;
    if(!endOfRound){
            numberOfRoundsCnt++ ; 
            if (numberOfRoundsCnt == numberOfRounds)
            {
                killAllChildrens() ;
                printResults() ;
                exit(0) ;
            }
        else{
            startRound() ;
            }
    }
              
}

int getSpeedRandomly(){
    srand(time(NULL) ^ (getpid()<<8));
    int num = (rand() % 15) + 10;
    return num ; 
}


void printResults(){
    int red_result = result["red"] ;
    int green_result = result["green"] ; 
    printf("\tGreen team : %d\nred team : %d\n",green_result,red_result) ;
    if(red_result > green_result){
        printf("\tRed team win the race,,,number of wining round : %d of %d\n",red_result,numberOfRounds) ;}
            else 
        printf("\tGreen team win the race,,,number of wining round : %d of %d\n",green_result,numberOfRounds) ;

    printf("\n\n\n\n\n\n\n\\n") ;

}


void startRound(){
   
    printf("\n\nround number : %d\n\n",numberOfRoundsCnt) ;
    printf("starting the game\nparent pid : %d\n\n",parentId) ; 
    startingTheRace() ;
   
}

void setTeamMemberReady(){

        char teamMemberNumber[2],processId[7],parentIdString[7],speedString[3],nextTeamMemberpPidString[7];
        sprintf(teamMemberNumber,"%d",i);
        sprintf(processId,"%d",getpid());
        sprintf(parentIdString,"%d",getppid());
        sprintf(speedString,"%d",speed);
        sprintf(nextTeamMemberpPidString,"%d",nextTeamMember);
        execlp("./child",teamMemberNumber,processId,teamName,parentIdString,speedString,nextTeamMemberpPidString,(char *) NULL) ;
    
}



void signalSet(){
     //signal for red team
     if ( sigset(SIGHUP, signal_catcher )== SIG_ERR) 
        {
                    perror("Sigset can not set SIGHUP");
                    exit(SIGHUP );
        }
     
     //signal for green team
     if ( sigset(SIGILL, signal_catcher) == SIG_ERR) 
                {
                    perror("Sigset can not set SIGIIL");
                    exit(SIGILL);
                }
}
