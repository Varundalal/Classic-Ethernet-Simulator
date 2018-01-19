//v_d68
//networks project
//12/7/2016
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>#include <ctype.h>
#include <sys/time.h>
#include <fcntl.h>
#define BUFF_SIZE 100

//PartNumb - Part number in the frame
//FrameNumb - Frame Number
//MsgCode - self defined to call the function for different purposes
//Stat1 - Details of station 1
//Stat2 - Details of station 2


//Function to log the output on cbp
void writeLog(int PartNumb, int FrameNumb, int MsgCode,int Stat1, int Stat2)
{
	FILE *fp;

	fp = fopen("log.txt","a");

	//log for recv
	if(MsgCode == 1)
	{
		fprintf(fp,"\nReceive part %d of frame %d from Station %d, to Station %d", PartNumb, FrameNumb, Stat1, Stat2);
	}

	//log for transfer
	if(MsgCode == 2)
	{
		fprintf(fp,"\nTransfer part %d of frame %d from Station %d, to Station %d", PartNumb, FrameNumb, Stat1, Stat2);
	}

	//log for collision
	if(MsgCode == 3)
	{
		fprintf(fp,"\nInform Station %d, Station %d, a collision",Stat1,Stat2);
	}
	fclose(fp);
}


int main()
{
	//Its to be used with select()
	fd_set fScks;

	//Its for storing timeout, which will be used in select()
	struct timeval strTimeOut;

	//Variables for logics
	int Ready_Socks;
	int StatNum,ConnectedTillNow = 0, StationId[1000], i = 0;
	int Part_Numb, Frame_Numb, Src_Stat, Dest_Stat, PrevFrmNum, Prev_Src_Stat, j2 = 2, Can_Send = 0;
	int Send_Part_Numb, Send_Frame_Numb, Send_Src_Sat, Send_Dest_Stat;
	char Tempij[10000];

	//Variables for network
	char    	Buffer[BUFF_SIZE];
	struct    	sockaddr_in sock_Serv_Add,Sock_Client_Add;
	int       	Sock_Lis,Size,Connected_Sockt[1000],Child_Cnt = 0, High_Sock, Out_Sock;
	int       	Sock_Des;
	int 		Port,Child_Pid,True = 1;


	//Initializing
	strTimeOut.tv_usec = 0;
	strTimeOut.tv_sec = 1;
	printf("\nEnter port number: ");
	scanf("%d",&Port);


	//Socket Creation
	if((Sock_Des = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == -1)
	{
		printf("\nError - unable to create a socket!!!");
		exit(0);
	}

	//Setting socket options to make a reusable socket
	setsockopt(Sock_Des,SOL_SOCKET,SO_REUSEADDR,&True,sizeof(int));

	//setting socket parameters
	sock_Serv_Add.sin_family = AF_INET;
	sock_Serv_Add.sin_port = htons(Port);
	sock_Serv_Add.sin_addr.s_addr = INADDR_ANY;
	bzero(&sock_Serv_Add.sin_zero,8);

	//binding
	if(bind(Sock_Des,(struct sockaddr *)&sock_Serv_Add, sizeof(struct sockaddr)) == -1)
	{
		printf("\nUnable to bind.");
		exit(0);
	}

	//10 is max pending conns
	if(listen(Sock_Des, 10) == -1)
	{
		printf("\nError in listening.");
		exit(1);
	}

	//Asks for the number of SPs to connect to create an initial pool of sockets to be read on
	printf("\nEnter number of Station Processes: ");
	scanf("%d",&StatNum);
	printf("\n Waiting for Connections...");
	fflush(stdout);
	i = 0;

	//loop creates the pool for connected sockets
	while(ConnectedTillNow < StatNum)
	{
		Size = sizeof(struct sockaddr_in);
		Connected_Sockt[ConnectedTillNow] = accept(Sock_Des, (struct sockaddr *)&Sock_Client_Add,&Size);

		printf("\nConnected with a client.");
		ConnectedTillNow++;
		//Get Station ID straightway
		read(Connected_Sockt[ConnectedTillNow-1],Buffer,1024);
		printf("\nReceived data: %s",Buffer);
		sscanf(Buffer,"%d",&StationId[ConnectedTillNow - 1]);
		printf("\nStation ID: %d",StationId[ConnectedTillNow - 1]);
		fflush(stdout);
	}

	printf("\n The simulation starts.... All connections received.");
	fflush(stdout);
	//Highsock to be used in select
	High_Sock = 0;
	for(i = 0; i < ConnectedTillNow; i++)
	{
		if(Connected_Sockt[i] > High_Sock) High_Sock = Connected_Sockt[i];
	}

	printf("\nHighSock : %d",High_Sock);
	//initialize fd_set structure
	FD_ZERO(&fScks);
	fflush(stdout);
	//setting fd_set
	for(i = 0; i < ConnectedTillNow; i++)
	{
		FD_SET(Connected_Sockt[i],&fScks);
	}

	int iSentJ = 2, iFirstEntry = 0;
	Can_Send = 0;

	printf("\nfd_set has all been done.");
	fflush(stdout);
	//All clients have connected, now start working
	while(1)
	{
		//select function - it checks for any input or times out
		Ready_Socks = select(High_Sock + 1, &fScks, (fd_set *) 0, (fd_set *) 0, &strTimeOut);

		//sends part of frame to destination
		if(Can_Send == 1)
		{
			//no other packets have come so no collision
			if(Ready_Socks == 0)
			{
				//first part is sent
				if(iFirstEntry == 0)
				{
					writeLog(1, Send_Frame_Numb, 2,Send_Src_Sat,  Send_Dest_Stat);
					iFirstEntry++;
				}
				//second part is sent
				else
				{
					iFirstEntry = 0;
					writeLog(2, Send_Frame_Numb, 2,Send_Src_Sat,  Send_Dest_Stat);
					write(Out_Sock,"Success\0",strlen("Success\0"));
					Can_Send = 0;
				}
			}
			//some packet has come, so collision has occurred
			else
			{
				write(Out_Sock,"Collision\0",strlen("Collision\0"));
			}
		}

		//no collision only one part has arrived - step 1 verification
		if(Ready_Socks == 1)
		{
			//loop through to find which SP has sent frame
			for(i = 0; i < ConnectedTillNow; i++)
			{
				if(FD_ISSET(Connected_Sockt[i],&fScks))
				{
					Out_Sock = Connected_Sockt[i];
					read(Connected_Sockt[i],Tempij,strlen(Tempij));
					sscanf(Tempij,"%d %d %d %d", &Part_Numb, &Frame_Numb, &Src_Stat, &Dest_Stat);

					//check if this is first part or if not check whether the frame id and source station id matches
					if((PrevFrmNum == 0)||((Src_Stat == Prev_Src_Stat) && (Frame_Numb == PrevFrmNum)))
					{
						writeLog(Part_Numb, Frame_Numb, 1,Src_Stat,  Dest_Stat);
						if(Part_Numb %2 != 0)
						{
							j2 = 2;
							PrevFrmNum = Frame_Numb;
							Prev_Src_Stat = Src_Stat;
						}
						else
						{
							j2 = 3;
							PrevFrmNum = 0;
							Can_Send = 1;
							Send_Src_Sat = Src_Stat;
							Send_Dest_Stat = Dest_Stat;
							Send_Frame_Numb = Frame_Numb;
						}

					}
					else //part 2 of received frame is different so generate collision alert
					{
						writeLog(Part_Numb, Frame_Numb, 3,Src_Stat,  Dest_Stat);
						write(Connected_Sockt[i],"Collision\0",strlen("Collision\0"));
						Can_Send = 0;
					}
				}
			}

		}
		//Frames have come from more than one - so generate collision
		else if(Ready_Socks > 1)
		{
			for(i = 0; i < ConnectedTillNow; i++)
			{
				if(FD_ISSET(Connected_Sockt[i],&fScks))
				{
					j2 = 2;
					read(Connected_Sockt[i],Tempij,strlen(Tempij));
					sscanf(Tempij,"%d %d %d %d", &Part_Numb, &Frame_Numb, &Src_Stat, &Dest_Stat);
					writeLog(Part_Numb, Frame_Numb, 3,Src_Stat,  Dest_Stat);
					write(Connected_Sockt[i],"Collision\0",strlen("Collision\0"));
				}
			}
			Can_Send = 0;
		}
		else{}
	}

	close(Sock_Des);
}
