#include "header.h"
using namespace std ; 
/*-------------------*/

/*-------------------------*/
void callNextTeamMember() ;
void callParent() ;
void signal_catcher(int) ;
void  readyState() ;
void signalSet() ;
/*-------------------------*/
int nextMemberPid ;
int childNumber ;
int parentId ;
int speed ;
char teamName[7];
char fifo[10] ="./fifo" ;
int delay;
float timeDelay ;
char childNumberString[2]; 
string buffer ="" ;
/*-------------------------*/

int main (int argc, char * args[]){
     
     speed = atoi(args[4]);
     nextMemberPid = atoi(args[5]) ;
     strcpy(teamName,args[2]) ;
     childNumber = atoi(args[0]) ;
     parentId = atoi(args[3]) ;
     strcpy(childNumberString,args[0]) ;
     
    printf("i am child number %d with pid %s from team %s,Parent Pid ==>%s,my speed=>%s,my next teammate pid : %s,start running,i wanna to win(:(:\n",5-childNumber,args[1],args[2],args[3],args[4],args[5]);
    
    readyState() ;
    return 0 ; 
}

void callNextTeamMember(){
  if(nextMemberPid !=-1)
    kill(nextMemberPid,2) ;
     readyState() ;
}

void callParent()
{

     if( strcmp(teamName,"red")==0){
        kill(parentId,1) ;
     }
    else 
        kill(parentId,4) ;
     readyState() ;
}
void signal_catcher(int signal_number){
   
     if(signal_number == 2){
          
          int fo ;
         
          timeDelay =DISTANCE_TO_RUN/(float)speed;

          srand(time(NULL) ^ (getpid()<<16));

          speed = (rand() % 15) + 25;
         
          sleep(timeDelay);

          printf("iam child number %d,from team %s,join my team mate after %.2f\n",5-childNumber,teamName,timeDelay) ; 

          if(childNumber==0){
               callParent() ;
          }   
    else 
          callNextTeamMember(); 
     }
    }


void readyState(){
     signalSet() ;
     pause() ;
}

void signalSet(){
      if ( sigset(SIGINT, signal_catcher) == SIG_ERR) 
          {
               perror("Sigset can not set SIGINT");
               exit(SIGINT);
          }
}
