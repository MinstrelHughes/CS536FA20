#include <stdio.h>
#define NODE_NUM 0

extern struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent 
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
  };

extern int TRACE;
extern int YES;
extern int NO;

int connectcosts0[4] = { 0,  1,  3, 9 };


struct distance_table 
{
  int costs[4][4];
} dt0;


/* students to write the following two routines, and maybe some others */

void send2neighbour0(){
  struct rtpkt tempPkt[3];
  int distanceVector[4];
  int neighboursIndex[4] = {1,2,3};
  for(int i=0;i<4;i++){
    distanceVector[i] = dt0.costs[i][NODE_NUM];
  }
// send to 0's neighbours 
  for(int i=0;i<3;i++){
    creatertpkt(&tempPkt[i],NODE_NUM,neighboursIndex[i],distanceVector);
    tolayer2(tempPkt[i]);
  }

  printf("%s\n", "send to neighbour from node 0");

}


void rtinit0() {
  for(int i=0;i<4;i++){
    for(int j=0;j<4;j++){
      dt0.costs[i][j] = 999;
    }
  }
  for(int i=0;i<4;i++){
    dt0.costs[i][NODE_NUM] = connectcosts0[i];
  }
   printf("%s\n", "init 0 was called");

  // printdt0(&dt0);
  send2neighbour0();

}


void rtupdate0(rcvdpkt)
  struct rtpkt *rcvdpkt;
{
    printf("%s\n", "update 0 was called");
  int updatedFlag = 0;
  int sourceid = rcvdpkt->sourceid;
  for(int i=0;i<4;i++){
    dt0.costs[i][sourceid] = rcvdpkt->mincost[i];
  }

  for(int i=0;i<4;i++){
    if(i!=NODE_NUM){
      int tempCost = 999;

      for(int j=0;j<4;j++){
        if(j!=NODE_NUM && tempCost>connectcosts0[j]+dt0.costs[i][j]){
          tempCost = connectcosts0[j]+dt0.costs[i][j];
        }
      }

      if(tempCost!=dt0.costs[i][NODE_NUM]){
        updatedFlag = 1;
      }

      dt0.costs[i][NODE_NUM] = tempCost;

    }
  }

    // printf("update dt0");
  if(updatedFlag==1){
      printdt0(&dt0);

      send2neighbour0();
  }else{
    printf("%s\n","node 0: not updated" );
  }

}


printdt0(dtptr)
  struct distance_table *dtptr;
  
{
  printf("                via     \n");
  printf("   D0 |    1     2    3 \n");
  printf("  ----|-----------------\n");
  printf("     1|  %3d   %3d   %3d\n",dtptr->costs[1][1],
	 dtptr->costs[1][2],dtptr->costs[1][3]);
  printf("dest 2|  %3d   %3d   %3d\n",dtptr->costs[2][1],
	 dtptr->costs[2][2],dtptr->costs[2][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][1],
	 dtptr->costs[3][2],dtptr->costs[3][3]);
}

linkhandler0(linkid, newcost)  

  int linkid, newcost;

/* called when cost from 0 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
	
{
  connectcosts0[linkid] = newcost;

  int updatedFlag = 0;

  for(int i=0;i<4;i++){
    if(i!=NODE_NUM){
      int tempCost = 999;

      for(int j=0;j<4;j++){
        if(j!=NODE_NUM && tempCost>connectcosts0[j]+dt0.costs[i][j]){
          updatedFlag = 1;
          tempCost = connectcosts0[j]+dt0.costs[i][j];
        }
      }

      if(tempCost!=dt0.costs[i][NODE_NUM]){
        updatedFlag = 1;
      }

      dt0.costs[i][NODE_NUM] = tempCost;

    }
  }

    // printf("update dt0");
  if(updatedFlag==1){
      printdt0(&dt0);

      send2neighbour0();
  }

}

