#include <stdio.h>
#define NODE_NUM 3
extern struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent 
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
  };

extern int TRACE;
extern int YES;
extern int NO;
int connectcosts3[4] = {9,999,4,0};
struct distance_table 
{
  int costs[4][4];
} dt3;

/* students to write the following two routines, and maybe some others */
void send2neighbour3(){
  int neighboursIndex[2] = {0,2};

  struct rtpkt tempPkt[2];
  int distanceVector[4];
  for(int i=0;i<4;i++){
    distanceVector[i] = dt3.costs[i][NODE_NUM];
  }
// send to 3's neighbours 
  for(int i=0;i<2;i++){
    creatertpkt(&tempPkt[i],NODE_NUM,neighboursIndex[i],distanceVector);
    tolayer2(tempPkt[i]);
  }

  
  printf("%s\n", "send to neighbour from node 3");

}


void rtinit3() 
{
  for(int i=0;i<4;i++){
    for(int j=0;j<4;j++){
      dt3.costs[i][j] = 999;
    }
  }

  for(int i=0;i<4;i++){
    dt3.costs[i][NODE_NUM] = connectcosts3[i];
  }

    printf("%s\n", "init 3 was called");

  printdt3(&dt3);


}


void rtupdate3(rcvdpkt)
  struct rtpkt *rcvdpkt;
  
{
    int updatedFlag = 0;

  int sourceid = rcvdpkt->sourceid;
  for(int i=0;i<4;i++){
    dt3.costs[i][sourceid] = rcvdpkt->mincost[i];
  }

  for(int i=0;i<4;i++){
    if(i!=NODE_NUM){
      int tempCost = 999;

      for(int j=0;j<4;j++){
        if(j!=NODE_NUM && tempCost>connectcosts3[j]+dt3.costs[i][j]){
          tempCost = connectcosts3[j]+dt3.costs[i][j];
        }
      }

      if(tempCost!=dt3.costs[i][NODE_NUM]){
        updatedFlag = 1;
      }

      dt3.costs[i][NODE_NUM] = tempCost;
    }
  }
  if(updatedFlag==1){
        printdt3(&dt3);

      send2neighbour3();
  }else{
    printf("%s\n", "node 3: not updated");
  }
}


printdt3(dtptr)
  struct distance_table *dtptr;
  
{
  printf("             via     \n");
  printf("   D3 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 1|  %3d   %3d\n",dtptr->costs[1][0], dtptr->costs[1][2]);
  printf("     2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);

}







