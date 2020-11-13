#include "prog2.h"
#include "string.h"
#define WINDOW_SIZE 8
#define BUFFER_SIZE 1024
#define TIME_TO_INTP 30.0

#define SENDER_SEND_PACKET 0 
#define RECEIVER_RECEIVE_PACKET 1
#define RECEIVER_SEND_ACK 2
#define SENDER_RECEIVE_ACK 3
#define DROP_MESSAGE 4
#define LOST_OR_CORRUPT_MESSAGE 5
#define LOST_OR_CORRUPT_ACK 6
#define A_TIMER_INTERRUPT 7
#define OTHER 8

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
static int senderSeqnum;
static int receiverSeqnum;
static int senderWindowLeft;
static int bufferIndex; 
static int timerPointer; 
static struct pkt senderPacketBuffer[BUFFER_SIZE];
static struct pkt receiverPacket[BUFFER_SIZE];
static int ackStatus[BUFFER_SIZE];
static int revStatus[BUFFER_SIZE];





void printPacketHelper(struct pkt packet,int eventType){
  if(eventType==SENDER_SEND_PACKET){
    printf("%s\n", "Sender send packet:");
  }else if(eventType==RECEIVER_RECEIVE_PACKET){
    printf("%s\n", "Receiver receive the packet:");
  }else if(eventType==RECEIVER_SEND_ACK){
    printf("%s\n", "Receiver send ACK:");
  }else if(eventType==SENDER_RECEIVE_ACK){
    printf("%s\n", "Sender receive ACK:");
  }else if(eventType==LOST_OR_CORRUPT_MESSAGE){
    printf("%s\n", "Lost or corrupted message.");
    return;
  }else if(eventType==LOST_OR_CORRUPT_ACK){
    printf("seqnum: %d  ", packet.seqnum);
    printf("acknum: %d  ", packet.acknum);
    printf("%s\n", "Lost or corrupted ACK.");
    return;
  }else if(eventType==DROP_MESSAGE){
    printf("Sender's Window Left Num: %d\n", senderWindowLeft);
    printf("Sender Seqnum: %d\n", senderSeqnum);
    printf("%s\n", "Drop Message(buffer full):");
    // return;
  }else if(eventType==A_TIMER_INTERRUPT){
        printf("Timer interrupt: ");
      printf("seqnum: %d  ", packet.seqnum);
      printf("acknum: %d  \n", packet.acknum);
      return;

  }
  printf("seqnum: %d  ", packet.seqnum);
  printf("acknum: %d  ", packet.acknum);
  printf("payload:");
  for(int i=0;i<20 && packet.payload[i]!='\0';i++){
    printf("%c", packet.payload[i]);
  }
  printf("\n");

}

void stoptimer(int AorB)
{
  struct event * q;

  if (TRACE > 2) {
    printf("          STOP TIMER: stopping timer at %f\n", time);
  }

  for (q = evlist; q != NULL; q = q->next) {
    if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB)) {
      /* remove this event */
      if (NULL == q->next && NULL == q->prev) {
        evlist = NULL;              /* remove first and only event on list */
      } else if (NULL == q->next) { /* end of list - there is one in front */
        q->prev->next = NULL;
      } else if (q == evlist) { /* front of list - there must be event after */
        q->next->prev = NULL;
        evlist = q->next;
      } else { /* middle of list */
        q->next->prev = q->prev;
        q->prev->next = q->next;
      }
      free(q);
      return;
    }
  }
  printf("Warning: unable to cancel your timer. It wasn't running.\n");
}

void starttimer(int AorB, float increment)
{
  struct event * q;
  struct event * evptr;

  if (TRACE > 2) {
    printf("          START TIMER: starting timer at %f\n", time);
  }

  /* be nice: check to see if timer is already started, if so, then  warn */
  for (q = evlist; q != NULL; q = q->next) {
    if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB)) {
      printf("Warning: attempt to start a timer that is already started\n");
      return;
    }
  }

  /* create future event for when timer goes off */
  evptr = (struct event *)malloc(sizeof(struct event));
  evptr->evtime = time + increment;
  evptr->evtype = TIMER_INTERRUPT;
  evptr->eventity = AorB;
  insertevent(evptr);
}
int checksumHelper(int acknum, int seqnum, char* data){
  int checksum = acknum + seqnum;
  if(data == NULL){
    return checksum;
  }else{
    for(int i=0;i<20;i++){

    checksum+=data[i];
      if(data[i]=='\0'){
        break;
      }
    }  
  }

  return checksum;
}

struct pkt pktHelper(struct msg message, int acknum,int seqnum){

  struct pkt res;
  res.seqnum = seqnum;
  res.acknum = acknum;
  res.checksum = checksumHelper(acknum,seqnum,message.data);

  for(int i=0;i<21;i++){
    res.payload[i] = message.data[i];
    // printf("message data [i]%c\n", message.data[i]);
      if(message.data[i]=='\0'){
        break;
      }

  }
  // strcpy(res.payload, message.data);
  return res;
}

/* called from layer 5, passed the data to be sent to other side */
int A_output(struct msg message){
  printf("Sender A output packet:\n");

  // make the packet
  struct pkt packet = pktHelper(message,bufferIndex,bufferIndex);
  // save in sender buffer, buffer right move
  senderPacketBuffer[bufferIndex] = packet;
  // printf("put into buffer::::%s\n",packet.payload);
  bufferIndex++;
  // send to receiver
  for(;senderSeqnum<bufferIndex;senderSeqnum++){
    if (senderSeqnum >= senderWindowLeft + WINDOW_SIZE){
      printPacketHelper(senderPacketBuffer[senderSeqnum],DROP_MESSAGE);
      return 0;
    }
    // start timer
    starttimer(senderSeqnum, TIME_TO_INTP);
    printf("%s\n","A output" );
    printPacketHelper(senderPacketBuffer[senderSeqnum],SENDER_SEND_PACKET);
    tolayer3(A, senderPacketBuffer[senderSeqnum]);
  }

  return 0;
}

/* called from layer 3, when a packet arrives for layer 4 */
int A_input(struct pkt packet){
  printf("Sender A input:\n");
  // check whether the ACK is corrupted
  int checkSum = checksumHelper(packet.acknum,packet.seqnum,packet.payload);
  //correct checksum
  if(checkSum == packet.checksum){
    printPacketHelper(packet,SENDER_RECEIVE_ACK);
    
    // acked on packet, update status and stop the timer
    ackStatus[packet.acknum] = 1;
    stoptimer(packet.acknum);
    
    // senderWindowLeft right move to next unacked packet
    if(packet.acknum==senderWindowLeft && ackStatus[packet.acknum]==1){
      for(int i=packet.acknum;ackStatus[i]==1;i++){
          if(i > senderWindowLeft){
            senderWindowLeft = i;
          }
      }
      senderWindowLeft++;
    }
    // out of sender window or sender buffer
    for(;(senderSeqnum < senderWindowLeft + WINDOW_SIZE&& senderSeqnum < bufferIndex);senderSeqnum++){
      starttimer(senderSeqnum, TIME_TO_INTP);
      tolayer3(A, senderPacketBuffer[senderSeqnum]);

      printf("%s\n","A input" );
      printPacketHelper(senderPacketBuffer[senderSeqnum],SENDER_SEND_PACKET);
    }
    
    return 0;
  }else{
    // corrupt ack by checksum
    printPacketHelper(packet,LOST_OR_CORRUPT_ACK); 
    return 0;
  }
  

}

/* called when A's timer goes off */
int A_timerinterrupt() {
  printf("A timer interrupt\n");
  if (ackStatus[timerPointer]!=1 || senderSeqnum> timerPointer){
    printPacketHelper(senderPacketBuffer[timerPointer],A_TIMER_INTERRUPT);
    // restart timer and resend packet
    starttimer(timerPointer, TIME_TO_INTP);
    tolayer3(A, senderPacketBuffer[timerPointer]);
  }
  return 0;
}

int A_init(){
  //initialize
  bufferIndex = 0;
  senderSeqnum = 0;
  senderWindowLeft = 0;
  return 0;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
int B_input(struct pkt packet){
  printf("B input:  ");
  if (receiverSeqnum >= BUFFER_SIZE || packet.seqnum >= senderWindowLeft + WINDOW_SIZE) {
    printPacketHelper(packet,DROP_MESSAGE);
    return 0;
  }
  // printf("%s\n", packet.payload);

  // printf("???%s\n", packet.payload);
  int checkSum = checksumHelper(packet.acknum,packet.seqnum,packet.payload);
  
  // Check whether the message is corrupted.
  if(checkSum == packet.checksum){

    printf("Corret \n");

    // save packet in receiver buffer, update status
    revStatus[packet.seqnum] = 1;
    receiverPacket[packet.seqnum] = packet;
    
    // send data in buffer to application, seq num right move
    for(;revStatus[receiverSeqnum] == 1;receiverSeqnum++){
      tolayer5(B, receiverPacket[receiverSeqnum].payload);
      printf("Next receiver seqnum: %d\n", receiverSeqnum);
    }

    // make ack packet and send to sender
    struct msg tempAckMsg;
    strcpy(tempAckMsg.data, "ACK");
    struct pkt ackPacket = pktHelper(tempAckMsg,packet.seqnum,packet.seqnum);
    tolayer3(B, ackPacket); 
  }else{
    printPacketHelper(packet,LOST_OR_CORRUPT_ACK);
    return 0;
  } 
  return 0;
}

/* called when B's timer goes off */
int B_timerinterrupt() {
  return 0;
}

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
int B_init() {
  receiverSeqnum = 0;
  return 0;
}


int TRACE = 1;   /* for my debugging */
int nsim = 0;    /* number of messages from 5 to 4 so far */
int nsimmax = 0; /* number of msgs to generate, then stop */
float time = 0.000;
float lossprob;    /* probability that a packet is dropped  */
float corruptprob; /* probability that one bit is packet is flipped */
float lambda;      /* arrival rate of messages from layer 5 */
int ntolayer3;     /* number sent into layer 3 */
int nlost;         /* number lost in media */
int ncorrupt;      /* number corrupted by media*/

int main(){
  memset(senderPacketBuffer, 0, sizeof(senderPacketBuffer));
  memset(receiverPacket, 0, sizeof(receiverPacket));
  struct event * eventptr;
  struct msg msg2give;
  struct pkt pkt2give;

  int i, j;

  init();
  A_init();
  B_init();

  for (;; ) {
    eventptr = evlist; /* get next event to simulate */
    if (NULL == eventptr) {
    printf("terminate");
    goto terminate;
    }
    evlist = evlist->next; /* remove this event from event list */
    if (evlist != NULL) {
      evlist->prev = NULL;
    }
    if (TRACE >= 2) {
      printf("\nEVENT time: %f,", eventptr->evtime);
      printf("  type: %d", eventptr->evtype);
      if (eventptr->evtype == 0) {
        printf(", timerinterrupt  ");
      } else if (eventptr->evtype == 1) {
        printf(", fromlayer5 ");
      } else {
        printf(", fromlayer3 ");
      }
      printf(" entity: %d\n", eventptr->eventity);
    }
    time = eventptr->evtime; /* update time to next event time */
    if (nsim == nsimmax) {
    printf("All Done with Simulation\n");
      break; /* all done with simulation */
    }
    if (eventptr->evtype == FROM_LAYER5) {
      generate_next_arrival(); /* set up future arrival */
      /* fill in msg to give with string of same letter */
      j = nsim % 26;
      for (i = 0; i < 20; i++) {
        msg2give.data[i] = 97 + j;
      }
      if (TRACE > 2) {
        printf("          MAINLOOP: data given to student: ");
        for (i = 0; i < 20; i++) {
          printf("%c", msg2give.data[i]);
        }
        printf("\n");
      }
      nsim++;
      if (eventptr->eventity == A) {
        A_output(msg2give);
      }
    } else if (eventptr->evtype == FROM_LAYER3) {
      pkt2give.seqnum = eventptr->pktptr->seqnum;
      pkt2give.acknum = eventptr->pktptr->acknum;
      pkt2give.checksum = eventptr->pktptr->checksum;
      for (i = 0; i < 20; i++) {
        pkt2give.payload[i] = eventptr->pktptr->payload[i];
      }
      if (eventptr->eventity == A) { /* deliver packet by calling */
        A_input(pkt2give);           /* appropriate entity */
      } else {
        B_input(pkt2give);
      }
      free(eventptr->pktptr); /* free the memory for packet */
    } else if (eventptr->evtype == TIMER_INTERRUPT) {
      timerPointer = eventptr->eventity;
      A_timerinterrupt();
    } else {
      printf("INTERNAL PANIC: unknown event type \n");
    }
    free(eventptr);
  }
  return 0;

terminate:
  printf(
    " Simulator terminated at time %f\n after sending %d msgs from layer5\n",
    time, nsim);
  return 0;
}

void init() /* initialize the simulator */
{
  int i;
  float sum, avg;

  printf("-----  Stop and Wait Network Simulator Version 1.1 -------- \n\n");
  printf("Enter the number of messages to simulate: ");
  scanf("%d", &nsimmax);
  printf("Enter  packet loss probability [enter 0.0 for no loss]:");
  scanf("%f", &lossprob);
  printf("Enter packet corruption probability [0.0 for no corruption]:");
  scanf("%f", &corruptprob);
  printf("Enter average time between messages from sender's layer5 [ > 0.0]:");
  scanf("%f", &lambda);
  printf("Enter TRACE:");
  scanf("%d", &TRACE);



  srand(rand_seed); /* init random number generator */
  sum = 0.0;   /* test random number generator for students */
  for (i = 0; i < 1000; i++) {
    sum = sum + jimsrand(); /* jimsrand() should be uniform in [0,1] */
  }
  avg = sum / 1000.0;
  if (avg < 0.25 || avg > 0.75) {
    printf("It is likely that random number generation on your machine\n");
    printf("is different from what this emulator expects.  Please take\n");
    printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
    exit(0);
  }

  ntolayer3 = 0;
  nlost = 0;
  ncorrupt = 0;

  time = 0.0;              /* initialize time to 0.0 */
  generate_next_arrival(); /* initialize event list */
}

/****************************************************************************/
/* jimsrand(): return a float in range [0,1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the*/
/* system-supplied rand() function return an int in therange [0,mmm]        */
/****************************************************************************/
float jimsrand()
{
  double mmm = INT_MAX;         /* largest int  - MACHINE DEPENDENT!!!!!!!!   */
  float x;                      /* individual students may need to change mmm */
  x = rand_r(&rand_seed) / mmm; /* x should be uniform in [0,1] */
  return x;
}

/************ EVENT HANDLINE ROUTINES ****************/
/*  The next set of routines handle the event list   */
/*****************************************************/
void generate_next_arrival()
{
  double x;
  struct event * evptr;

  if (TRACE > 2) {
    printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");
  }

  x = lambda * jimsrand() * 2; /* x is uniform on [0,2*lambda] */
  /* having mean of lambda        */
  evptr = (struct event *)malloc(sizeof(struct event));
  evptr->evtime = time + x;
  evptr->evtype = FROM_LAYER5;
  if (BIDIRECTIONAL && (jimsrand() > 0.5)) {
    evptr->eventity = B;
  } else {
    evptr->eventity = A;
  }
  insertevent(evptr);
}

void insertevent(struct event * p)
{
  struct event * q, * qold;

  if (TRACE > 2) {
    printf("            INSERTEVENT: time is %lf\n", time);
    printf("            INSERTEVENT: future time will be %lf\n", p->evtime);
  }
  q = evlist;      /* q points to header of list in which p struct inserted */
  if (NULL == q) { /* list is empty */
    evlist = p;
    p->next = NULL;
    p->prev = NULL;
  } else {
    for (qold = q; q != NULL && p->evtime > q->evtime; q = q->next) {
      qold = q;
    }
    if (NULL == q) { /* end of list */
      qold->next = p;
      p->prev = qold;
      p->next = NULL;
    } else if (q == evlist) { /* front of list */
      p->next = evlist;
      p->prev = NULL;
      p->next->prev = p;
      evlist = p;
    } else { /* middle of list */
      p->next = q;
      p->prev = q->prev;
      q->prev->next = p;
      q->prev = p;
    }
  }
}

void printevlist()
{
  struct event * q;
  printf("--------------\nEvent List Follows:\n");
  for (q = evlist; q != NULL; q = q->next) {
    printf("Event time: %f, type: %d entity: %d\n", q->evtime, q->evtype,
      q->eventity);
  }
  printf("--------------\n");
}

/********************** Student-callable ROUTINES ***********************/

/* called by students routine to cancel a previously-started timer */


/************************** TOLAYER3 ***************/
void tolayer3(int AorB, struct pkt packet)
{
  struct pkt * mypktptr;
  struct event * evptr, * q;
  float lastime, x;
  int i;

  ntolayer3++;

  /* simulate losses: */
  if (jimsrand() < lossprob) {
    nlost++;
    if (TRACE > 0) {
      printf("          TOLAYER3: packet being lost\n");
    }
    return;
  }

  /*
   * make a copy of the packet student just gave me since he/she may decide
   * to do something with the packet after we return back to him/her
   */

  mypktptr = (struct pkt *)malloc(sizeof(struct pkt));
  mypktptr->seqnum = packet.seqnum;
  mypktptr->acknum = packet.acknum;
  mypktptr->checksum = packet.checksum;
  for (i = 0; i < 20; ++i) {
    mypktptr->payload[i] = packet.payload[i];
  }
  if (TRACE > 2) {
    printf("          TOLAYER3: seq: %d, ack %d, check: %d ", mypktptr->seqnum,
      mypktptr->acknum, mypktptr->checksum);
    for (i = 0; i < 20; ++i) {
      printf("%c", mypktptr->payload[i]);
    }
    printf("\n");
  }

  /* create future event for arrival of packet at the other side */
  evptr = (struct event *)malloc(sizeof(struct event));
  evptr->evtype = FROM_LAYER3;      /* packet will pop out from layer3 */
  evptr->eventity = (AorB + 1) & 1; /* event occurs at other entity */
  evptr->pktptr = mypktptr;         /* save ptr to my copy of packet */

  /*
   * finally, compute the arrival time of packet at the other end.
   * medium can not reorder, so make sure packet arrives between 1 and 10
   * time units after the latest arrival time of packets
   * currently in the medium on their way to the destination
   */

  lastime = time;
  for (q = evlist; q != NULL; q = q->next) {
    if ((q->evtype == FROM_LAYER3 && q->eventity == evptr->eventity)) {
      lastime = q->evtime;
    }
  }
  evptr->evtime = lastime + 1 + 9 * jimsrand();

  /* simulate corruption: */
  if (jimsrand() < corruptprob) {
    ncorrupt++;
    if ((x = jimsrand()) < .75) {
      mypktptr->payload[0] = 'Z'; /* corrupt payload */
    } else if (x < .875) {
      mypktptr->seqnum = 999999;
    } else {
      mypktptr->acknum = 999999;
    }
    if (TRACE > 0) {
      printf("          TOLAYER3: packet being corrupted\n");
    }
  }

  if (TRACE > 2) {
    printf("          TOLAYER3: scheduling arrival on other side\n");
  }
  insertevent(evptr);
}

void tolayer5(int AorB, const char * datasent)
{
  (void)AorB;
  int i;
  if (TRACE > 2) {
    printf("          TOLAYER5: data received: ");
    for (i = 0; i < 20; i++) {
      printf("%c", datasent[i]);
    }
    printf("\n");
  }
}
