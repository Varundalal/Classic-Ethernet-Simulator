//v_d68
//networksProject
//12/7/2016
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <fcntl.h>

fd_set socks;
int iHighSocket;

void writeLog(int Part, int Frame, int Mesg,int Stat1, int Stat2);

int main()
{

	FILE *fptr;
	char Buffer[1024],Temp[3][1024], Tempij[1024];
	char *cPtr;
	int i, FrameId[1111], ToStation[1111], Maxi, Sending = 0, iStationID, Part = 2, TimeSlot = 0, RecurrColl = 0, RandWait[10] = {1,2,13,24,45,56,67,78,99,110}, 		BusBusy = 0, j2 = 0, iTemp = 0,Temp2 = 0,Temp3 = 0,Temp4 = 0, Seed = 1;


//To create network Variables.
	int 		PortNo;
	int       	SocketDesc;
	struct 		hostent *hostH1;
	struct    	sockaddr_in socketServAddrss;
	char 		CServIp[100];

//Reading input_data file and storing in program memory

	printf("\nEnter Client id #: ");
	scanf("%d",&iStationID);
	printf("\nEnter a number between 1 to 10 ");
	scanf("%d",&Seed);
	fptr = fopen("input_data.txt","r");
	i = 0;
	while(!feof(fptr))
	{
		cPtr = fgets(Buffer,1024,fptr);
		if(cPtr!=NULL)
		{
			sscanf(cPtr,"%s %d, %s %s %d",Temp[0], &FrameId[i], Temp[1], Temp[2], &ToStation[i]);
			i++;
		}
	}
	Maxi = i;
	fclose(fptr);
	//Sample file is securely stored 
	//Network initialization

	printf("\nEnter port number: ");
	scanf("%d",&PortNo);

	printf("\nEnter server\'s ip: ");
	scanf("%s",CServIp);

	hostH1 = gethostbyname(CServIp);

	if((SocketDesc = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == -1)
	{
		printf("\nError - unable to create a socket");
		exit(0);
	}

	socketServAddrss.sin_family = AF_INET;
	socketServAddrss.sin_port = htons(PortNo);
	bcopy((char *)hostH1->h_addr, (char *) &socketServAddrss.sin_addr.s_addr, hostH1->h_length);
	bzero(&(socketServAddrss.sin_zero),8);

	if(connect(SocketDesc,(struct sockaddr *)&socketServAddrss, sizeof(struct sockaddr)) == -1)
	{
		printf("\n Unable to connect, Connection Error");
		exit(0);
	}

	//send the client station id
	Temp4 = sprintf(Tempij,"%d",iStationID);
	strcpy(Buffer,Tempij);
	printf("\nBuffer Station ID: %s",Buffer);
	write(SocketDesc,Buffer,strlen(Buffer));

	i = 2;
	printf("\nStarting frames...");
	fflush(stdout);
	while(1)
	{
		sleep((TimeSlot+1)*1*Seed);
		printf("\nnext time slot");
		//part num, frame num, src stat, dest stat
		iTemp = (i%2)+1;
		Temp2 = FrameId[j2];
		Temp3 = ToStation[j2];
		Temp4 = sprintf(Tempij, "%d %d %d %d",iTemp,Temp2,iStationID, Temp3);
		write(SocketDesc,Tempij,strlen(Tempij));
		writeLog((i%2)+1, FrameId[j2], 1, iStationID, 0);

		read(SocketDesc,Tempij,1024);

		// Determining the waiting time using BEBO Algorithm

		if(strcmp(Tempij,"Success") != 0)
		{
			if(RecurrColl < 10)
			{
				writeLog(0,0,3,0,RandWait[RecurrColl]);
				TimeSlot = RandWait[RecurrColl];
			}
			else
			{
				writeLog(0,0,3,0,1023);
				TimeSlot = 1023;
			}
			//it is second part of the frame
			if(i%2 == 1) i--;
			RecurrColl++;
		}
		else
		{
			RecurrColl = 0;
			TimeSlot = 0;
			if(i%2 == 0) j2++;
			i++;
		}

	}
}

void writeLog(int Part, int Frame, int Mesg,int Stat1, int TimeSlot)
{
	FILE *fptr;

	fptr = fopen("log.txt","a");


	if(Mesg == 1)
	{
		fprintf(fptr,"\n Send part %d of the frame %d to the Station %d", Part, Frame, Stat1);
	}

	if(Mesg == 3)
	{
		fprintf(fptr,"\n A collision informed, wait for the %d time slot",TimeSlot);
	}
	fclose(fptr);
}


