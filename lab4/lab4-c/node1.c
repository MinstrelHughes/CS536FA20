#include <stdio.h>
#define NODE_NUM 1

extern struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent 
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
  };


extern int TRACE;
extern int YES;
extern int NO;

int connectcosts1[4] = { 1,  0,  1, 999 };

struct distance_table 
{
  int costs[4][4];
} dt1;


/* students to write the following two routines, and maybe some others */
void send2neighbour1(){
  struct rtpkt tempPkt[2];
  int distanceVector[4];
  int neighboursIndex[2] = {0,2};
  for(int i=0;i<4;i++){
    distanceVector[i] = dt1.costs[i][NODE_NUM];
  }
// send to 1's neighbours 
  for(int i=0;i<2;i++){
    creatertpkt(&tempPkt[i],NODE_NUM,neighboursIndex[i],distanceVector);
    tolayer2(tempPkt[i]);
  } 

  printf("%s\n", "send to neighbour from node 1");

}

rtinit1() 
{

  for(int i=0;i<4;i++){
    for(int j=0;j<4;j++){
      dt1.costs[i][j] = 999;
    }
  }

  for(int i=0;i<4;i++){
    dt1.costs[i][NODE_NUM] = connectcosts1[i];
  }

  printf("%s\n", "init 1 was called");

  printdt1(&dt1);
  send2neighbour1();
  
}


rtupdate1(rcvdpkt)
  struct rtpkt *rcvdpkt;
  
{

    int updatedFlag = 0;

  int sourceid = rcvdpkt->sourceid;
  for(int i=0;i<4;i++){
    dt1.costs[i][sourceid] = rcvdpkt->mincost[i];
  }

  for(int i=0;i<4;i++){
    if(i!=NODE_NUM){
      int tempCost = 999;

      for(int j=0;j<4;j++){
        if(j!=NODE_NUM && tempCost>connectcosts1[j]+dt1.costs[i][j]){

          tempCost = connectcosts1[j]+dt1.costs[i][j];

        }

      }
      if(tempCost!=dt1.costs[i][NODE_NUM]){
        updatedFlag = 1;
      }

      dt1.costs[i][NODE_NUM] = tempCost;
    }
  }


  if(updatedFlag==1){
      send2neighbour1();
      printdt1(&dt1);

  }else{
    printf("%s\n", "node 2: not updated");
  }

}


printdt1(dtptr)
  struct distance_table *dtptr;
  
{
  printf("             via   \n");
  printf("   D1 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);
  printf("     3|  %3d   %3d\n",dtptr->costs[3][0], dtptr->costs[3][2]);

}



linkhandler1(linkid, newcost)   
int linkid, newcost;   
/* called when cost from 1 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
	
{

    connectcosts1[linkid] = newcost;

  int updatedFlag = 0;

  for(int i=0;i<4;i++){
    if(i!=NODE_NUM){
      int tempCost = 999;

      for(int j=0;j<4;j++){
        if(j!=NODE_NUM && tempCost>connectcosts1[j]+dt1.costs[i][j]){
          updatedFlag = 1;
          tempCost = connectcosts1[j]+dt1.costs[i][j];
        }
      }

      if(tempCost!=dt1.costs[i][NODE_NUM]){
        updatedFlag = 1;
      }

      dt1.costs[i][NODE_NUM] = tempCost;

    }
  }

    // printf("update dt0");
  if(updatedFlag==1){
      printdt1(&dt1);

      send2neighbour1();
  }
}


