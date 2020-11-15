#include <stdio.h>
#define NODE_NUM 2

extern struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent 
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
  };

extern int TRACE;
extern int YES;
extern int NO;
int connectcosts2[4] = {3,1,0,4};

struct distance_table 
{
  int costs[4][4];
} dt2;


/* students to write the following two routines, and maybe some others */

void send2neighbour2(){
  struct rtpkt tempPkt[3];
  int distanceVector[4];
  int neighboursIndex[3] = {0,1,3};
  for(int i=0;i<4;i++){
    distanceVector[i] = dt2.costs[i][NODE_NUM];
  }
// send to 2's neighbours 
  for(int i=0;i<3;i++){
    creatertpkt(&tempPkt[i],NODE_NUM,neighboursIndex[i],distanceVector);
    tolayer2(tempPkt[i]);
  }
  
  printf("%s\n", "send to neighbour from node 2");

}

void rtinit2() 
{

  for(int i=0;i<4;i++){
    for(int j=0;j<4;j++){
      dt2.costs[i][j] = 999;
    }
  }

  for(int i=0;i<4;i++){
    dt2.costs[i][NODE_NUM] = connectcosts2[i];
  }

    printf("%s\n", "init 2 was called");

  printdt2(&dt2);
  send2neighbour2();

}


void rtupdate2(rcvdpkt)
  struct rtpkt *rcvdpkt;
  
{

  printf("%s\n","update dt2 called");
  int updatedFlag = 0;

  int sourceid = rcvdpkt->sourceid;
  for(int i=0;i<4;i++){
    dt2.costs[i][sourceid] = rcvdpkt->mincost[i];
  }

  for(int i=0;i<4;i++){
    if(i!=NODE_NUM){
      int tempCost = 999;

      for(int j=0;j<4;j++){
        if(j!=NODE_NUM && tempCost>connectcosts2[j]+dt2.costs[i][j]){

          tempCost = connectcosts2[j]+dt2.costs[i][j];
        }
      }
      if(tempCost!=dt2.costs[i][NODE_NUM]){
        updatedFlag = 1;
      }

      dt2.costs[i][NODE_NUM] = tempCost;
    }
  }
  if(updatedFlag==1){
        printdt2(&dt2);

      send2neighbour2();
  }
}


printdt2(dtptr)
  struct distance_table *dtptr;
  
{
  printf("                via     \n");
  printf("   D2 |    0     1    3 \n");
  printf("  ----|-----------------\n");
  printf("     0|  %3d   %3d   %3d\n",dtptr->costs[0][0],
	 dtptr->costs[0][1],dtptr->costs[0][3]);
  printf("dest 1|  %3d   %3d   %3d\n",dtptr->costs[1][0],
	 dtptr->costs[1][1],dtptr->costs[1][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][0],
	 dtptr->costs[3][1],dtptr->costs[3][3]);
}







