#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netdb.h> 
#include <netinet/in.h>
#include<iostream>
#include <fstream>
#include <sstream>
#include <csignal>
#include <ctime>
#include <sstream>
#define MAXELEMENTSIZE 22000
using namespace std;

void * clientThread(void *arg);
void * serverThread(void *arg);
fstream file;
void * listenOnPort(void *arg);
int noOfProcesses;
int avgClock=0;
int randomlocalclock=0;
int portno=0;
void error(const char *msg)
{
    perror(msg);
    exit(1);
}
char * machineName;
int flag=0;

/* Converts string to integer
*/
int convertStringToInt(string a){
	stringstream s(a);
	int b=0;
	s >> b;
	return b;
}


/* Converts integer to string
*/
string convertIntToString(int a){
	stringstream s2;
	s2 << a;
	return s2.str();
}	


/*Calculates the average of all the logical clocks received from processes*/
int calculateAvg(int logicalClock[],int noOfProcesses){
	int sum=0;
	for(int j=0;j<noOfProcesses;j++){
		cout << "logicalClock[j]: "<< logicalClock[j]<< endl;
		sum+=logicalClock[j];
	}
	cout << "noOfProcesses\t: " << noOfProcesses << endl;
	int avg=sum/noOfProcesses;
	return avg;
}
/**
* readwrite thread method is called for each client
*/
void * readwrite(void *arg){
	int newsockfd=*((int *)arg);
	
	char buffer[256];
	
    	bzero(buffer,256);
	
	string response;
	cout << "local clock: "<< randomlocalclock << endl;
	
	int n;
    	n = read(newsockfd,buffer,256);//this read call reads the clock of the deamon process
		if(n==0){
			cout << "cannot read from the client";
			exit(0);
		}
		string s;		
     		if (n < 0){
			perror("ERROR reading from socket");
		}
			
		s.append(buffer, buffer+n);
		stringstream geek(s);
		cout << endl << "Time received from Deamon \t" <<s << endl;
		int x=0;
		geek >> x;
		x=randomlocalclock-x;//calculates the offset by subtracting local clock with the clock received from deamon process
     		
		cout.flush();
		bzero(buffer,256);
		stringstream s2;
		s2 << x;
		response=s2.str();
		cout << "Offset sent to deamon\t : " << response << endl;
		
		strcpy(buffer,response.c_str());
     		n = write(newsockfd,buffer,strlen(buffer));//writes the offset to deamon process
		if (n < 0){ 
			perror("ERROR writing to client");
		}
		n = read(newsockfd,buffer,256);//reads the adjusted time sent by the edamon process
		if(n==0){
			cout << "cannot read from the client";
			exit(0);
		}
		if (n < 0){
			perror("ERROR reading from socket");
		}
		s="";	
		s.append(buffer, buffer+n);
		x=convertStringToInt(s);
		randomlocalclock=randomlocalclock+x;//adjusts it's local clock to synchronize the time with all other processes
     		cout << endl <<endl << "Synchronized time is\t: " <<randomlocalclock << endl;
		
}

void * clientThread(void *arg){
	string str1; 
	int i=0;
	int sockfd[MAXELEMENTSIZE], n;	
	char buffer[256];
	string strLocalClock=convertIntToString(randomlocalclock);		
	int logicalClock[MAXELEMENTSIZE];
	while(1){
		
		if(flag==1){//flag is given as command line argument, value of 1 is given to deamon process only
			cout << "Connecting Socket "<< endl;
			string str;
			ifstream file("Berkley.txt");//this file contains the ports number of processes in the distributed system
			i=0;
			while (getline(file, str1))//reads the ports from the file in order to connect to all the processes
		    	{
			int x=convertStringToInt(str1);
			
			struct sockaddr_in serv_addr;
			struct hostent *server;
			sockfd[i] = socket(AF_INET, SOCK_STREAM, 0);//client socket is created
			if (sockfd[i] < 0) 
			cout << "ERROR opening socket";
			server = gethostbyname(machineName);//server address
			if (server == NULL) {
				perror("ERROR, no such host\n");
				exit(0);
			}
			memset((char *) &serv_addr,0, sizeof(&serv_addr));
			serv_addr.sin_family = AF_INET;
			bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
			serv_addr.sin_port = htons(x);
			if (connect(sockfd[i],(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) //connect call
				cout << "ERROR connecting";
			else{
				bzero(buffer,256);
				strcpy(buffer, strLocalClock.c_str());//string is converted to char buffer
				n = write(sockfd[i],buffer,strlen(buffer));//Deamon sents it's won logical clock to all the proceses
				if (n < 0) 
					cout << "Instruction cannot be written" << endl;
				bzero(buffer,256);
				n = read(sockfd[i],buffer,MAXELEMENTSIZE);//reads the offsets sent by processes in reply
		
				if (n < 0) 
					cout << "ERROR reading from socket" << endl;
				cout << "offset received" << buffer << endl;
				string s="";	
				s.append(buffer);			
				logicalClock[i]=convertStringToInt(s);//creates an array of logical clocks of all the processes
			
			}
			i++;
		}
		break;		
		}//flag end
	}//while end
	int avg=calculateAvg(logicalClock,i);
	cout <<"Average is\t : " << avg<< endl;
	for(int j=0;j<i;j++){
		int a=avg-logicalClock[j];
		string s2=convertIntToString(a);
		cout<<"logical clock at client was\t : "<< logicalClock[j] <<endl;
		cout <<"Sending time to synchronize\t : "<< s2<<endl<<endl;
		bzero(buffer,256);
		strcpy(buffer, s2.c_str());//string is converted to char buffer
		n = write(sockfd[j],buffer,strlen(buffer));//adjusted time is sent to all the processes
		if (n < 0) 
			cout<<"Instruction cannot be written"<<endl;
		close(sockfd[j]);
		}
	}

void * serverThread(void *arg){
	cout<<"server thread"<<endl;
	int portno=*((int *)arg);
	int sockfd, newsockfd,n,i=0,no_of_thread=0;
    	socklen_t clilen;
    	char buffer[MAXELEMENTSIZE];	
    	pthread_t t2[MAXELEMENTSIZE];
	
    	struct sockaddr_in serv_addr, cli_addr;	

     	sockfd = socket(AF_INET, SOCK_STREAM, 0);
     	if (sockfd < 0) 
        	error("ERROR opening socket");

     	bzero((char *) &serv_addr, sizeof(serv_addr));
     	serv_addr.sin_family = AF_INET;
     	serv_addr.sin_addr.s_addr = INADDR_ANY;
     	serv_addr.sin_port = htons(portno);
     	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) //bind the socket
	        error("ERROR on binding");
     	listen(sockfd,5);
     	clilen = sizeof(cli_addr);
     	while(1){
		
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		no_of_thread++;
		pthread_create(&t2[no_of_thread], NULL, &readwrite, &newsockfd);//pthread created for each processes that is connected
	   	  
	}  
	close(newsockfd);
	close(sockfd);
}


int main(int argc, char *argv[])
{
	if (argc < 4) {
        	 fprintf(stderr,"ERROR, no port or file name provided\n");
        	 exit(1);
   	}
	portno = atoi(argv[1]);
	noOfProcesses=atoi(argv[2]);
	flag=atoi(argv[4]);
	machineName=(argv[3]);
	randomlocalclock=rand() %10;
	randomlocalclock=randomlocalclock*getpid();
	pthread_t t1,t2;
	pthread_create(&t1, NULL, &serverThread,(void *)&portno);// thread created for deamon process which sends connection request to all processes
	pthread_create(&t2, NULL, &clientThread,(void *)&portno);// thread is used by other processes to synchronize their time with each other
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	cout.flush();
	
	return 0; 
}
