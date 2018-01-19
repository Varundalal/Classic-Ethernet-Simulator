COMPUTER NETWORKS 

CLASSIC ETHERNET SIMULATION

Varun Dalal
ID : A04743047
12/07/2016

All files (server.c,client.c,input.txt,log.txt) should be copied into a separate folder.

Compilation:
For compiling the program the following commands should be used
In terminal 1:
gcc server.c -o server.out ( this creates a server.out file )
./server.out
In terminal 2:
gcc client.c -o client.out ( this creates a client.out file ) 
./client.out
Number of terminals depends on number of clients.

Server inputs:
After executing the ./server.out we are prompted to enter the port number
Enter port number: ( enter any value which is grater that 1024)
Enter number of clients: ( number of clients ex:2 says two clients can be connected)

Client terminal inputs:
The frame input is present in the file input.txt
Log.txt is the file into which the log details are written by the client 
When ./client.out is executed from the terminal the following inputs should be provided
Enter Client ID: ( can give any value from 1-9) 
Enter any number from 1 to 10 : ( this will be the wait time) 
Enter Port number : ( port number of the server) 
Enter server’s IP: ( it is 147.26.100.200 for Athena server, server ip varies with different server)
If connection is successful it gives
Sending frames:
Else
Connection error: (this output depends on the error) 

Output:
The output of the whole project can be seen in the log.txt file.
The output varies according to the collisions occurred and number of clients connected. 


Communication Bus Process
The CBP is implemented in server.c file
Communication between valve actuators and wide networks ó such as used in control systems ó or between valves and other field instruments is critical in most processes. 
This file keeps the server in waiting state until all clients are connected to it. After all clients are connected it then fd_set has been done message is displayed and it starts receiving the frames from clients. 

Station Process
The SP is implemented in client.c file
The input is taken from the file "input.txt" from the client. After the client is successfully connected to the server it then starts sending the frame. while it is connected to the server we will be able to see the clients id on the server.
 The output is written into the log file.

Collision
When the collision occurs, a flag is set by assigning a value to an integer variable, and a counter for keeping track of the number of successive collisions is set to 1.When the next successive collision occurs, the counter is incremented and the time slot in the sleep function is incremented according to the value in the array of random waiting times.If no collision occurs, the counter for collisions and the flag are set to zero.

 
