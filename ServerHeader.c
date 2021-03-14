/*
ServerHeader: returns HEADER information about an HTTP server

Distribuited according to GPL license v2.
For more details: http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
There's no implicit or explicit warranty, you use it at your own risk!

Author: Fabrizio Pani - fabph@hotmail.com


compile with:
    
    gcc ServerHeader.c -o ServerHeader
    
    or in Windows
    
    mingw32-gcc ServerHeader.c -o ServerHeader.exe -mconsole -mwindows -lws2_32
    
        
    from Dev-Cpp pass the following options:
      compiler: -mwindows -console
      linker: -lws2_32 
     
    */
    
#include<stdio.h> //for puts(), getchar(), printf(), fprintf(), perror()
#include<stdlib.h>//for system()
#include<string.h> //for memset()
#define BUFSIZE 512 //MAX BYTES at once
#define MIN_VER 5   //Minor version
#define MAJ_VER 0   //Major version
#ifdef _WIN32
    #include <winsock.h>
    WSADATA wsaData;
    WORD    wVersionRequested;
    DWORD dwError;
#else
    #include<sys/socket.h> //for structures, socket(), connect(), send(), recv(), 
    #include<netdb.h> //for herror(), hostent structure, gethostbyname()
    #include<netinet/in.h> //for ntohs(), htons()
    #include<errno.h> //for perror() errors
#endif    
    
void cls(void);
void header(void); 

    
int main(int argc, char *argv[]){
     
    short int sockd, retop;
    unsigned int port = 80;
    struct hostent *hp;
    struct sockaddr_in address;
    char* HTTP_MSG = "GET / HTTP/1.0\n\n";
    char **parg; 
    char buf[BUFSIZE];

	cls();     
	header();

     
	#ifdef _WIN32
		wVersionRequested=MAKEWORD(2, 2);             
                                         
		if(WSAStartup(wVersionRequested, &wsaData)<0){
			dwError = WSAGetLastError();                                         
			printf("Unable to initialize winsock\r\n%ld", dwError);       
	        return 1;
	    }                                      
	#endif     

                 
	if(argc>3||argc<2){
		fprintf(stderr, "Usage: ServerHeader <IP> or <hostname>\n");
		fprintf(stderr, "Usage: ServerHeader <IP> or <hostname> <port number>\
 (default: 80)\n");
		return 1;
   }         
        
        
	if(argc==3){
				
		parg = &argv[2];
		port = atoi(*parg); //conversion into integer
			
		if (port<1 || port>65535){                
			puts("\nPlease insert a port number between 1 and 65535\n"); 
			return 1;
			  }
		}


	if ((hp = gethostbyname(argv[1])) == NULL) {  // get IP address 


	#ifdef _WIN32
		dwError = WSAGetLastError();
			
		switch(dwError){
						  
			case WSAHOST_NOT_FOUND:
				printf("Host not found\n");
			break;    
			case WSANO_DATA:
				printf("No data record found\n");
			break;        
			default:
				printf("Function failed with error: %ld\n", dwError);
					  }   
	#else
		herror("IP address resolution error: ");
	#endif     
		return 1;
		}
	   
              
	sockd=socket(AF_INET,SOCK_STREAM,0);
			
		if(sockd == -1){
					  
			#ifdef _WIN32
			    dwError = WSAGetLastError();
			    fprintf(stderr,"Socket error: %ld\n", dwError);       
			    WSACleanup();
		    #else
			    perror("Socket error: ");       
			#endif 
			printf("\n\nPress enter to exit");
			getchar();
			return 1;
				
					}            

	address.sin_family = AF_INET;                //host byte order
	address.sin_port = htons((u_short)port);    // short  network byte error 
	address.sin_addr = *((struct in_addr *)hp->h_addr);
	memset(address.sin_zero, '\0', sizeof(address.sin_zero));

	printf("\nConnecting to : %s\n", inet_ntoa(*((struct in_addr *)hp->h_addr)));
	printf("Port : %d\n", ntohs(address.sin_port));


	retop=connect(sockd, (struct sockaddr *) &address, sizeof(address));
         
	if(retop==-1){
         
	    #ifdef _WIN32
			dwError = WSAGetLastError();
		    fprintf(stderr,"\nConnection error: %ld\n", dwError);       
		    WSACleanup();
	    #else
		    perror("Connection error: ");       
	    #endif 
	    if(sockd)
			close(sockd); 
	    puts("Press enter to exit");
	    getchar();
	    return 1;

                     }    

	printf("\n");
	retop=send(sockd,HTTP_MSG,strlen(HTTP_MSG),0);           

      if(retop==-1){
                   
		    #ifdef _WIN32
				dwError = WSAGetLastError();
			    fprintf(stderr,"Server communication error: %ld\n", dwError);       
			    WSACleanup();
			#else
				perror("Server communication error: ");       
		    #endif 
		    if(sockd)     
				close(sockd); 
			printf("\n\nPress enter to exit");
			getchar();
			return 1;
                   
                     }    

    for(;;) {
        retop = recv(sockd, buf, BUFSIZE-1, 0);
        
        if(retop){
           printf("%s",buf);
           printf("\n\nPress ENTER to continue..");
           getchar();
                     }    
        
        else if (retop ==-1){
            #ifdef _WIN32
			    dwError = WSAGetLastError();
			    fprintf(stderr,"Reception error: %ld\n", dwError);       
			    WSACleanup();
			#else
			    perror("Reception error: ");       
            #endif 
            return 1;
                   }
        else { 
            printf("\n\nConnection closed\n");
            break;
             }
    } 
    
    if(sockd)
		close(sockd);           

    #ifdef _WIN32
		WSACleanup();
	#endif           
                  
    printf("\n\nPress enter to exit");
    getchar();

	return 0;
        
 }   
 
void cls(){
	//portable way to clear screen (at least on Windows, and Unix systems)


	#ifdef _WIN32
		system("cls");
	#else
		system("clear");       
	#endif     
     }

void header(){
      
	char* msg="This utility returns the data header of an HTTP server.\nIt's \
	distribuited according to GPL license v2.\nFor more details: \
	http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt\nThere's no implicit \
	or explicit warranty, you use it at your own risk!\n\nAuthor: \
	Fabrizio Pani - fabph@hotmail.com\n\n";
	printf("ServerHeader %d.%d\n %s", MAJ_VER, MIN_VER, msg);
      
      }
