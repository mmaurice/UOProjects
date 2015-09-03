#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <pthread.h>
#include <time.h>

#define MAXLINE 4096  /* Max line length */
#define MAX_PIDS 32
#define ENTRYSIZE 100
pthread_mutex_t mutexlock;
//pthread_mutex_t submutexlock;
volatile pid_t *pubpids;
volatile pid_t *subpids;

typedef struct 
{
  int   fileDescriptor[2];
  char  buf[1025];
  char  buf2[1025];
  char  buf3[1025];
  char  bufterm[1025];
  char  *pubConnect;
  char  *pubTopic;
  char  *pubEnd;
  char  *pubterm;

}Publisher;

typedef struct 
{
  int   fileDescriptor[2];
  char  buf[1025];
  char  buf2[1025];
  char  buf3[1025];
  char  bufterm[1025];
  char  *subConnect;
  char  *subTopic;
  char  *subEnd;
  char  *subterm;

}Subscriber;

typedef struct 
{
  char  *type;
  int   pid;
  char  *selectTopics;
  int   *pipe;
  int   *serverPipe;
  char  *term;


}Record;

struct Topic 
{
  int timestamp;
  int pubId;
  char data[ENTRYSIZE];
  struct Topic* next;
}*tail, *head;

//add topic to the queue

void enqueue(struct Topic *t1){
  struct Topic *temp;
  temp = (struct Topic *)malloc(sizeof(struct Topic));
  temp = t1;

  if(head==NULL){
    head = temp;
    head->next = NULL;
    tail = head;
  }
  else{
    head->next = temp;
    head = temp;
    head->next = tail;
  }

}

struct Topic * dequeue(){
  struct Topic *temp, *var;
  var = tail;
  if(var == tail){
    tail = tail->next;
    free(var);
    return tail;

  }
  else{
    printf("Queue empty\n");
    return NULL;
  }
}



void doPublisher(int pubNum)
{
  printf("publisher: %d\n", getpid());	
}

void doSubscriber(int subNum)
{
	printf("subscriber: %d\n", getpid());		
}

//controlling function for the Publisher procs communication with server 
void *
pubThr_fn(void *param) 
{

  //pthread_mutex_lock(&mutexlock);
  //printf("here\n");

  Record *threadRecord;
  threadRecord = (Record *) param;
  Publisher pubT;
  pubT.pubConnect = "Pub Connect";
  pubT.pubTopic = "Topic 1";
  pubT.pubEnd = "End";
  pubT.pubterm = "terminate";

  //struct Topic* topicEntry;
  //trcpy(topicEntry->data, pubT.pubTopic);
  //topicEntry->pubId = threadRecord->pid;
  int x;
  char  *data = "accept";
  char  *data2 = "reject";
  char  *terminate = "terminate";
  char  *success = "successful";
  char  *retry = "retry";
  char  *temp;
  if ((x = read(threadRecord->pipe[0], pubT.buf, 1024)) >= 0){
    //printf("pub read %d bytes from the Publisher pipe: \"%s\"\n", x, pubT.buf); 
    pubT.buf[x] = 0;
    if (strcmp(pubT.buf, pubT.pubConnect) == 0){
      write(threadRecord->serverPipe[1], data, strlen(data));
      if ((x = read(threadRecord->pipe[0], pubT.buf2, 1024)) >= 0){
        //printf("pub read %d bytes from the Publisher pipe: \"%s\"\n", x, pubT.buf2); 
        pubT.buf2[x] = 0;
        if (strcmp(pubT.buf2, pubT.pubTopic) == 0){
          write(threadRecord->serverPipe[1], data, strlen(data));
          if ((x = read(threadRecord->pipe[0], pubT.buf3, 1024)) >= 0){
            //printf("pub read %d bytes from the Publisher pipe: \"%s\"\n", x, pubT.buf3);
            pubT.buf3[x] = 0;
            if(strcmp(pubT.buf3, pubT.pubEnd) == 0){
              //threadRecord.term = "terminated";
              write(threadRecord->serverPipe[1], data, strlen(data));
              //printf("pub successful connection to server!\n");
/*
              if((x = read(threadRecord->pipe[0], pubT.buf, 1024)) >= 0){
                pubT.buf[x] = 0;
                temp = pubT.buf;
                write(threadRecord->serverPipe[1], success, strlen(success));
              }
              else
                write(threadRecord->serverPipe[1], retry, strlen(retry));
              //enqueue(topicEntry);
*/
              //continue;
            } 
            else{
              write(threadRecord->serverPipe[1], data2, strlen(data2));
            }
          }
        }
        else if (strcmp(pubT.buf2, pubT.pubterm) == 0){
          //threadRecord.term = "terminated";
          ;
        }
        else{
          write(threadRecord->serverPipe[1], data2, strlen(data2));
        }
      }
    }
    else if (strcmp(pubT.buf, pubT.pubterm) == 0){
      //threadRecord.term = "terminated";
      ;
    }
    else{
      write(threadRecord->serverPipe[1], data2, strlen(data2));
    }
  }
  //pthread_mutex_unlock (&mutexlock);

//sleep(1);
  pthread_exit(NULL);
}

//controlling function for the Subscriber procs communication with server 
void *
subThr_fn(void *param) 
{
  //pthread_mutex_lock (&mutexlock);
  Record *threadRecord;
  threadRecord = (Record *) param;
  Subscriber subT;
  subT.subConnect = "Sub Connect";
  subT.subTopic = "Topic 1";
  subT.subEnd = "End";
  subT.subterm = "terminate";
  //subfileDescriptor = threadRecord->pipe;
  //ServerFD = threadRecord->serverPipe;
  int x;
  char  *data = "accept";
  char  *data2 = "reject";
  char  *terminate = "terminate";
  //struct Topic* topic;

  if ((x = read(threadRecord->pipe[0], subT.buf, 1024)) >= 0){
    //printf("sub read %d bytes from the Subscriber pipe: \"%s\"\n", x, subT.buf); 
    subT.buf[x] = 0;
    if (strcmp(subT.buf, subT.subConnect) == 0){
      write(threadRecord->serverPipe[1], data, strlen(data));
      if ((x = read(threadRecord->pipe[0], subT.buf2, 1024)) >= 0){
        //printf("sub read %d bytes from the Subscriber pipe: \"%s\"\n", x, subT.buf2); 
        subT.buf2[x] = 0;
        if (strcmp(subT.buf2, subT.subTopic) == 0){
          write(threadRecord->serverPipe[1], data, strlen(data));
          if ((x = read(threadRecord->pipe[0], subT.buf3, 1024)) >= 0){
            //printf("sub read %d bytes from the Subscriber pipe: \"%s\"\n", x, subT.buf3);
            subT.buf3[x] = 0;
            if(strcmp(subT.buf3, subT.subEnd) == 0){
              //threadRecord.term = "terminated";
              write(threadRecord->serverPipe[1], data, strlen(data));
              //printf("sub successful connection to server!\n");
              //topic = dequeue();
              //continue;
            } 
            else{
              write(threadRecord->serverPipe[1], data2, strlen(data2));
            }
          }
        }
        else if (strcmp(subT.buf2, subT.subterm) == 0){
          //threadRecord.term = "terminated";
          ;
        }
        else{
          write(threadRecord->serverPipe[1], data2, strlen(data2));
        }
      }
    }
    else if (strcmp(subT.buf, subT.subterm) == 0){
      //threadRecord.term = "terminated";
      ;
    }
    else{
      write(threadRecord->serverPipe[1], data2, strlen(data2));
    }
}
//sleep(1);
  //pthread_mutex_unlock (&mutexlock);

  pthread_exit(NULL);

}



int
main(int argc, char *argv[])
{
	pid_t 	pid;
	int		status;
  int   ServerFD[2];
  int   ServerFD2[2];
  char  *data = "accept";
  char  *data2 = "reject";
  char  *terminate = "terminate";
  char  pubbuf[1025];
  char  pubbufend[1025];
  char  pubbufterm[1025];
  char  subbuf[1025];
  char  subbufend[1025];
  char  subbufterm[1025];

  pipe(ServerFD);
  pipe(ServerFD2);
	pid = fork();

	if(pid == 0){//this is the child of the main process, the DIServer
    int i;
		//n is # of publisher
		int n = atoi(argv[1]);
		//m is number of subscribers
		int m = atoi(argv[2]);
    //t are topics	
    int t = atoi(argv[3]);
    pthread_t pubthreads[n];
    pthread_t subthreads[m];
    int   rc;
    int   rc2;
    void  *pubthreadstatus;
    void  *subthreadstatus;

    pthread_attr_t pubattr;
    pthread_attr_t subattr;

    pthread_attr_init(&pubattr);
    pthread_attr_setdetachstate(&pubattr, PTHREAD_CREATE_JOINABLE);

    pthread_attr_init(&subattr);
    pthread_attr_setdetachstate(&subattr, PTHREAD_CREATE_JOINABLE);

    pthread_mutex_init(&mutexlock,NULL);


    Record record[n+m];
		/*need to use n and m to create child procs of the 
		DIServer, publishers and subscriers*/
		//loop to create publisher procs and stroe pid in array;
		pid_t pubpid;
		pid_t subpid;

		//map space for the pub array
		pubpids = mmap(0, MAX_PIDS*sizeof(pid_t), PROT_READ|PROT_WRITE,
              MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		 if (!pubpids) {
    		perror("mmap failed");
    		exit(1);
 		 }
  		memset((void *)pubpids, 0, MAX_PIDS*sizeof(pid_t)); 		 
 		 //map space for sub array
		subpids = mmap(0, MAX_PIDS*sizeof(pid_t), PROT_READ|PROT_WRITE,
              MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		 if (!subpids) {
    		perror("mmap failed");
    		exit(1);
 		 }
  		memset((void *)subpids, 0, MAX_PIDS*sizeof(pid_t));

  		//loop to creat forked pubs
  		for(i=0;i<n;i++)
  		{
        //create the record struct
        //create the publisher struct
        int z;
        Publisher pub;
        pub.pubConnect = "Pub Connect";
        pub.pubTopic = "Topic 1";//topic of interest
        pub.pubEnd = "End";
        pub.pubterm = "terminate";
        pipe(pub.fileDescriptor);
  			pubpid = fork();
        int articles;
        char article[12];
  			if(pubpid == 0)
  			{
          //doPublisher(n);
          write(pub.fileDescriptor[1], pub.pubConnect, strlen(pub.pubConnect));
          if ((z = read(ServerFD[0], pubbuf, 1024)) >= 0) {
              pubbuf[z] = 0; /* terminate the string */ 
              //printf("pub read %d bytes from the DIServer pipe: \"%s\"\n", z, pubbuf);
              if (strcmp(pubbuf, data) == 0)
              {
                write(pub.fileDescriptor[1], pub.pubTopic, strlen(pub.pubTopic));
                if ((z = read(ServerFD[0], pubbufend, 1024)) >= 0) {
                  pubbufend[z] = 0;
                  //printf("pub read %d bytes from the DIServer pipe: \"%s\"\n", z, pubbufend);
                  if (strcmp(pubbufend, data) == 0){
                    write(pub.fileDescriptor[1], pub.pubEnd, strlen(pub.pubEnd));
                    if((z = read(ServerFD[0], pubbufend, 1024)) >= 0) {
                     // printf("pub read %d bytes from the DIServer pipe: \"%s\"\n", z, pubbufend);
                      /*if (strcmp(pubbufend, data)==0){
                        for (articles = 0; articles<10;articles++){
                          if(articles%2 != 0){
                            sprintf(article, "Topic 1 Article %d", articles);
                            write(pub.fileDescriptor[1], article, strlen(article));
                          }
                        }
                      }*/
                    }

                  }
                  else{
                    write(pub.fileDescriptor[1], pub.pubterm, strlen(pub.pubterm));
                    continue;
                 }
                }
              }
              else{
                write(pub.fileDescriptor[1], pub.pubterm, strlen(pub.pubterm));
                continue;
              }

          } 
          else 
              perror("read");
          //if reads accept then write to publisher pipe 
  				exit(0);
  			}
  			else if (pubpid < 0){
  				perror("fork failed");
  			}
        
  			else{//back to the DIServer

          record[i].pipe = pub.fileDescriptor;
          record[i].type = "Publisher";
          record[i].pid = i+1;
          record[i].selectTopics = pub.pubTopic;
          record[i].serverPipe = ServerFD;
          rc=pthread_create(&pubthreads[i],&pubattr,pubThr_fn,(void *) &record[i]);
          if(rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
          }
          pthread_attr_destroy(&pubattr);
          rc = pthread_join(pubthreads[i], &pubthreadstatus);
          // want to create the thread here, then handle all of reading a writng of pipes in thread handler}
  		  }
      }

  		//loop to create forked subs;
      for(i=0;i<m;i++)
      {
        //create the subscriber struct
        int z;
        Subscriber sub;
        sub.subConnect = "Sub Connect";
        sub.subTopic = "Topic 1"; //topic of interest
        sub.subEnd = "End";
        sub.subterm = "terminate";
        pipe(sub.fileDescriptor);
        subpid = fork();
        if(subpid == 0)
        {
          //doPublisher(n);
          write(sub.fileDescriptor[1], sub.subConnect, strlen(sub.subConnect));
          if ((z = read(ServerFD2[0], subbuf, 1024)) >= 0) {
              subbuf[z] = 0; /* terminate the string */ 
              //printf("sub read %d bytes from the DIServer pipe: \"%s\"\n", z, subbuf);
              if (strcmp(subbuf, data) == 0)
              {
                write(sub.fileDescriptor[1], sub.subTopic, strlen(sub.subTopic));
                if ((z = read(ServerFD2[0], subbufend, 1024)) >= 0) {
                  subbufend[z] = 0;
                 // printf("sub read %d bytes from the DIServer pipe: \"%s\"\n", z, subbufend);
                  if (strcmp(subbufend, data) == 0){
                    write(sub.fileDescriptor[1], sub.subEnd, strlen(sub.subEnd));
                    if((z = read(ServerFD2[0], subbufend, 1024)) >= 0) {
                      //printf("sub read %d bytes from the DIServer pipe: \"%s\"\n", z, subbufend);
                    }
                  }
                  else{
                    write(sub.fileDescriptor[1], sub.subterm, strlen(sub.subterm));
                    continue;
                 }
                }
              }
              else{
                write(sub.fileDescriptor[1], sub.subterm, strlen(sub.subterm));
                continue;
              }

          } 
          else 
              perror("read");
          //if reads accept then write to publisher pipe 
          exit(0);
        }
        else if (subpid < 0){
          perror("fork failed");
        }
        else{//back to the DIServer
          record[i+n].pipe = sub.fileDescriptor;
          record[i+n].type = "Subscriber";
          record[i+n].pid = i+1;
          record[i+n].selectTopics = sub.subTopic;
          record[i+n].serverPipe = ServerFD2;

          rc2 = pthread_create(&subthreads[i],&subattr,subThr_fn,(void *) &record[i+n]);
          if(rc2){
            printf("ERROR; return code from pthread_create() is %d\n", rc2);
            exit(-1);
          }
          pthread_attr_destroy(&subattr);
          rc2 = pthread_join(subthreads[i], &subthreadstatus);


        }
      }
      /*
      pthread_attr_destroy(&subattr);
      for(i=0;i<n;i++){
        printf("here\n");
        rc = pthread_join(pubthreads[i], &pubthreadstatus);
        if (rc) {
          printf("ERROR; return code from pthread_join() is %d\n", rc);
          exit(-1);
        }
      }
      pthread_attr_destroy(&subattr);
      for(i=0;i<m;i++){
        printf("here\n");
        rc2 = pthread_join(subthreads[i], &subthreadstatus);
        if (rc2) {
          printf("ERROR; return code from pthread_join() is %d\n", rc2);
          exit(-1);
        }
      }
      */
      /*this works because the way code is written, if it 
      makes it here all connections have been made*/
      //printf("all pubs and subs have connected to server\n");
      for(i = 0; i<n+m; i++){
        printf("Type:%s, ID: %d, Topic: %s\n", record[i].type, record[i].pid, record[i].selectTopics);
      }
      //begin termination

      write(ServerFD[1], terminate, strlen(terminate));
      close(ServerFD[1]);
      for(i = 0; i<n+m; i++){
        printf("%s-%d: terminated\n", record[i].type,record[i].pid);
      }
      //printf("%s\n", );
      pthread_exit(NULL);

	
}
	else
		wait(&status);
		//main process just needs to wait for everything else to be done.


	return(0);
}