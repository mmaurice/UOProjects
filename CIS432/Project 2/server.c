#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <string>
#include <map>
#include <iostream>
#include <time.h>
#include <list>

using namespace std;



//#include "hash.h"
#include "duckchat.h"


#define MAX_CONNECTIONS 10
#define HOSTNAME_MAX 100
#define MAX_MESSAGE_LEN 65536

//typedef map<string,string> channel_type; //<username, ip+port in string>
typedef map<string,struct sockaddr_in> channel_type; //<username, sockaddr_in of user>

int s; //socket for listening
struct sockaddr_in server;
//start
char this_Server[HOSTNAME_MAX];
int this_Port;
#define limit 60
//end


map<string,struct sockaddr_in> usernames; //<username, sockaddr_in of user>
map<string,int> active_usernames; //0-inactive , 1-active
//map<struct sockaddr_in,string> rev_usernames;
map<string,string> rev_usernames; //<ip+port in string, username>
map<string,channel_type> channels;
//start
map<string, list<struct sockaddr_in> > linkingCtoS;
list<struct sockaddr_in> serverAddrs;
list<struct sockaddr_in> hardstateAddrs;
list<long long> uids;
struct timeval timer;

int hardstate = 0;
//end



void handle_socket_input();
void handle_login_message(void *data, struct sockaddr_in sock);
void handle_logout_message(struct sockaddr_in sock);
void handle_join_message(void *data, struct sockaddr_in sock);
void handle_leave_message(void *data, struct sockaddr_in sock);
void handle_say_message(void *data, struct sockaddr_in sock);
void handle_list_message(struct sockaddr_in sock);
void handle_who_message(void *data, struct sockaddr_in sock);
void handle_keep_alive_message(struct sockaddr_in sock);
void send_error_message(struct sockaddr_in sock, string error_msg);
//start
void handle_join_S2S(void *data, struct sockaddr_in socket);
void handle_leave_S2S(void *data, struct sockaddr_in socket);
void handle_say_S2S(void *data, struct sockaddr_in socket);
void ip_port(struct sockaddr_in* socket, string info, string chan);
long long generating_unique_id();
void remove_Addr();
//end

int main(int argc, char *argv[]){
	if (argc < 3){
		printf("Usage: ./server domain_name port_num\n");
		exit(1);
	}
    s = socket(PF_INET, SOCK_DGRAM, 0);
    if (s < 0){
        perror ("socket() failed\n");
        exit(1);
    }
    /*
    char hostname[HOSTNAME_MAX];
	int port;
	strcpy(hostname, argv[1]);
	port = atoi(argv[2]);
	struct hostent     *he;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if ((he = gethostbyname(hostname)) == NULL) {
		puts("error resolving hostname..");
		exit(1);
	}
    memcpy(&server.sin_addr, he->h_addr_list[0], he->h_length);*/
    //start
    struct sockaddr_in t_oser;
    struct hostent *t_others;
    char t_hn[HOSTNAME_MAX];
    int t_port;
    for(int i=0; i<(argc-1); i+=2){
        strcpy(t_hn, argv[i+1]);
        t_port = atoi(argv[i+2]);
        
        if ((t_others = gethostbyname(t_hn)) == NULL) {
            cout << "cannot parse hostname" << endl;
            exit(1);
        }
        t_oser.sin_port = htons(t_port);
        t_oser.sin_family = AF_INET;
        memcpy(&t_oser.sin_addr, t_others->h_addr_list[0], t_others->h_length);
        
        if(i == 0){
            this_Port = t_port;
            strcpy(this_Server, inet_ntoa(t_oser.sin_addr));
            memcpy(&server.sin_addr, t_others->h_addr_list[0],t_others->h_length);
            server.sin_port = t_oser.sin_port;
            server.sin_family = t_oser.sin_family;
            //cout << "our server ip is : " << this_Server << " and port of: " << this_Port << endl;
        }else{
            serverAddrs.push_back(t_oser);
            //cout << "there is another server of ip: " << inet_ntoa(t_oser.sin_addr) << "and port of: " << t_port << endl;
        }
        
    }
    //end
    
	int err;
	err = bind(s, (struct sockaddr*)&server, sizeof server);
	if (err < 0){
		perror("bind failed\n");
	}
	else
	{
		//printf("bound socket\n");
	}


	


	//testing maps end

	//create default channel Common
	string default_channel = "Common";
	map<string,struct sockaddr_in> default_channel_users;
	channels[default_channel] = default_channel_users;


	//start
	timer.tv_sec = limit;
 	timer.tv_usec = 0;
    	//end

	while(1) //server runs for ever
	{

		//use a file descriptor with a timer to handle timeouts
		int rc;
		fd_set fds;

		FD_ZERO(&fds);
		FD_SET(s, &fds);
		


		rc = select(s+1, &fds, NULL, NULL, &timer);
		

		
		if (rc < 0)
		{
			printf("error in select\n");
            getchar();
		}
		else
		{
			int socket_data = 0;

			if (FD_ISSET(s,&fds))
			{
               
				//reading from socket
				handle_socket_input();
				socket_data = 1;

            }else{
                //start
                hardstate++;

                if(hardstate >= 2){
                	hardstate = 0;
                	remove_Addr();
                }
               
                timer.tv_sec = limit;
                timer.tv_usec = 0;
                map<string, list<struct sockaddr_in> >::iterator list_sss;
                for(list_sss = linkingCtoS.begin() ; list_sss != linkingCtoS.end() ; list_sss++){
                    struct request_join_S2S j;
                    strcpy(j.req_channel, list_sss->first.c_str());
                    j.req_type = REQ_S2S_J;
                    
                    list<struct sockaddr_in> servers_listt = list_sss->second;
                    list<struct sockaddr_in>::iterator servers_listt_it;
                    for(servers_listt_it = servers_listt.begin() ; servers_listt_it != servers_listt.end() ; servers_listt_it++){
			if(sendto(s, &j, sizeof(j), 0, (struct sockaddr*) &(*servers_listt_it) , sizeof(*servers_listt_it)) == -1){
                            perror("we have failed to send the join S2S after time expiration");
                        }
                    }
                    
                }
                
                //end
            }

			

			

			


		}

		
	}



	return 0;

}

void handle_socket_input()
{

	struct sockaddr_in recv_client;
	ssize_t bytes;
	void *data;
	size_t len;
	socklen_t fromlen;
	fromlen = sizeof(recv_client);
	char recv_text[MAX_MESSAGE_LEN];
	data = &recv_text;
	len = sizeof recv_text;


	bytes = recvfrom(s, data, len, 0, (struct sockaddr*)&recv_client, &fromlen);


	if (bytes < 0)
	{
		perror ("recvfrom failed\n");
	}
	else
	{
		//printf("received message\n");

		struct request* request_msg;
		request_msg = (struct request*)data;

		//printf("Message type:");
		request_t message_type = request_msg->req_type;
		if (message_type > 10 || message_type < 0){
			return;
		}

		//printf("%d\n", message_type);

		if (message_type == REQ_LOGIN)
		{
			handle_login_message(data, recv_client); //some methods would need recv_client
		}
		else if (message_type == REQ_LOGOUT)
		{
			handle_logout_message(recv_client);
		}
		else if (message_type == REQ_JOIN)
		{
			handle_join_message(data, recv_client);
		}
		else if (message_type == REQ_LEAVE)
		{
			handle_leave_message(data, recv_client);
		}
		else if (message_type == REQ_SAY)
		{
			handle_say_message(data, recv_client);
		}
		else if (message_type == REQ_LIST)
		{
			handle_list_message(recv_client);
		}
		else if (message_type == REQ_WHO)
		{
			handle_who_message(data, recv_client);
        }
        else if (message_type == REQ_S2S_J){
        	hardstateAddrs.push_back(recv_client);
            handle_join_S2S(data, recv_client);
        }
        else if (message_type == REQ_S2S_lev){
            handle_leave_S2S(data, recv_client);
        }
        else if (message_type == REQ_S2S_S){
            handle_say_S2S(data, recv_client);
        }
		else
		{
			//send error message to client
			send_error_message(recv_client, "*Unknown command");
		}




	}


}

void handle_login_message(void *data, struct sockaddr_in sock)
{
	struct request_login* msg;
	msg = (struct request_login*)data;

	string username = msg->req_username;
	usernames[username]	= sock;
	active_usernames[username] = 1;

	//rev_usernames[sock] = username;

	//char *inet_ntoa(struct in_addr in);
	string ip = inet_ntoa(sock.sin_addr);
	//cout << "ip: " << ip <<endl;
	int port = sock.sin_port;
	//unsigned short short_port = sock.sin_port;
	//cout << "short port: " << short_port << endl;
	//cout << "port: " << port << endl;

 	char port_str[6];
 	sprintf(port_str, "%d", port);
	//cout << "port: " << port_str << endl;

	string key = ip + "." +port_str;
	//cout << "key: " << key <<endl;
	rev_usernames[key] = username;

	//cout << "server: " << username << " logs in" << endl;

	



}

void handle_logout_message(struct sockaddr_in sock)
{

	//construct the key using sockaddr_in
	string ip = inet_ntoa(sock.sin_addr);
	//cout << "ip: " << ip <<endl;
	int port = sock.sin_port;

 	char port_str[6];
 	sprintf(port_str, "%d", port);
	//cout << "port: " << port_str << endl;

	string key = ip + "." +port_str;
	//cout << "key: " << key <<endl;

	//check whether key is in rev_usernames
	map <string,string> :: iterator iter;

	/*
    for(iter = rev_usernames.begin(); iter != rev_usernames.end(); iter++)
    {
        cout << "key: " << iter->first << " username: " << iter->second << endl;
    }
	*/

    cout << this_Server << ":" << this_Port << " " << ip << ":" << port
    << " recv Request Logout" << endl;


	iter = rev_usernames.find(key);
	if (iter == rev_usernames.end() )
	{
		//send an error message saying not logged in
		send_error_message(sock, "Not logged in");
	}
	else
	{
		//cout << "key " << key << " found."<<endl;
		string username = rev_usernames[key];
		rev_usernames.erase(iter);

		//remove from usernames
		map<string,struct sockaddr_in>::iterator user_iter;
		user_iter = usernames.find(username);
		usernames.erase(user_iter);

		//remove from all the channels if found
		map<string,channel_type>::iterator channel_iter;
		for(channel_iter = channels.begin(); channel_iter != channels.end(); channel_iter++)
		{
			//cout << "key: " << iter->first << " username: " << iter->second << endl;
			//channel_type current_channel = channel_iter->second;
			map<string,struct sockaddr_in>::iterator within_channel_iterator;
			within_channel_iterator = channel_iter->second.find(username);
			if (within_channel_iterator != channel_iter->second.end())
			{
				channel_iter->second.erase(within_channel_iterator);
			}

		}


		//remove entry from active usernames also
		//active_usernames[username] = 1;
		map<string,int>::iterator active_user_iter;
		active_user_iter = active_usernames.find(username);
		active_usernames.erase(active_user_iter);


		//cout << "server: " << username << " logs out" << endl;
	}


	/*
    for(iter = rev_usernames.begin(); iter != rev_usernames.end(); iter++)
    {
        cout << "key: " << iter->first << " username: " << iter->second << endl;
    }
	*/


	//if so delete it and delete username from usernames
	//if not send an error message - later

}

void handle_join_message(void *data, struct sockaddr_in sock)
{
	//get message fields
	struct request_join* msg;
	msg = (struct request_join*)data;

	string channel = msg->req_channel;

	string ip = inet_ntoa(sock.sin_addr);

	int port = sock.sin_port;

 	char port_str[6];
 	sprintf(port_str, "%d", port);
	string key = ip + "." +port_str;

    cout << this_Server << ":" << this_Port << " " << ip << ":" << port
    << " recv Request Join " << channel << endl;

	//check whether key is in rev_usernames
	map <string,string> :: iterator iter;


	iter = rev_usernames.find(key);
	if (iter == rev_usernames.end() )
	{
		//ip+port not recognized - send an error message
		send_error_message(sock, "Not logged in");
	}
	else
	{
		string username = rev_usernames[key];

		map<string,channel_type>::iterator channel_iter;

		channel_iter = channels.find(channel);

		active_usernames[username] = 1;

        if(linkingCtoS.find(channel) == linkingCtoS.end()){
            struct request_join_S2S jS2S;
            strcpy(jS2S.req_channel, channel.c_str());
            jS2S.req_type = (REQ_S2S_J);
            for(list<struct sockaddr_in>::iterator it = serverAddrs.begin(); it != serverAddrs.end() ; it++){
                cout << this_Server << ":" << this_Port << " " << ip << ":" << port
                << " send S2S Join " << channel << endl;
                
                if(strcmp((const char *) ip.c_str(),(const char *) this_Server) == 0){
                    if(port == this_Port){
                        continue;
                    }
                }
                
                if (sendto(s, &jS2S, sizeof(jS2S), 0, (struct sockaddr*) &(*it), sizeof(*it)) == -1){
                    perror("sendto fail");
                }        
            } 
        }
        //this line might be the following if the orig did not work if (channel_iter != channels.end())
		if (channel_iter == channels.end())
		{
			//channel not found
			map<string,struct sockaddr_in> new_channel_users;
			new_channel_users[username] = sock;
			channels[channel] = new_channel_users;
			//cout << "creating new channel and joining" << endl;

		}
		else
		{
			//channel already exits
			//map<string,struct sockaddr_in>* existing_channel_users;
			//existing_channel_users = &channels[channel];
			//*existing_channel_users[username] = sock;

			channels[channel][username] = sock;
			//cout << "joining exisitng channel" << endl;


		}

		//cout << "server: " << username << " joins channel " << channel << endl;


	}

	//check whether the user is in usernames
	//if yes check whether channel is in channels
	//if channel is there add user to the channel
	//if channel is not there add channel and add user to the channel


}


void handle_leave_message(void *data, struct sockaddr_in sock)
{

	//check whether the user is in usernames
	//if yes check whether channel is in channels
	//check whether the user is in the channel
	//if yes, remove user from channel
	//if not send an error message to the user


	//get message fields
	struct request_leave* msg;
	msg = (struct request_leave*)data;

	string channel = msg->req_channel;

	string ip = inet_ntoa(sock.sin_addr);

	int port = sock.sin_port;

 	char port_str[6];
 	sprintf(port_str, "%d", port);
	string key = ip + "." +port_str;
    
    cout << this_Server << ":" << this_Port << " " << ip << ":" << port
    << " recv Request Leave " << channel  << endl;

	//check whether key is in rev_usernames
	map <string,string> :: iterator iter;


	iter = rev_usernames.find(key);
	if (iter == rev_usernames.end() )
	{
		//ip+port not recognized - send an error message
		send_error_message(sock, "Not logged in");
	}
	else
	{
		string username = rev_usernames[key];

		map<string,channel_type>::iterator channel_iter;

		channel_iter = channels.find(channel);

		active_usernames[username] = 1;

		if (channel_iter == channels.end())
		{
			//channel not found
			send_error_message(sock, "No channel by the name " + channel);
			cout << "server: " << username << " trying to leave non-existent channel " << channel << endl;

		}
		else
		{
			//channel already exits
			//map<string,struct sockaddr_in> existing_channel_users;
			//existing_channel_users = channels[channel];
			map<string,struct sockaddr_in>::iterator channel_user_iter;
			channel_user_iter = channels[channel].find(username);

			if (channel_user_iter == channels[channel].end())
			{
				//user not in channel
				send_error_message(sock, "You are not in channel " + channel);
				cout << "server: " << username << " trying to leave channel " << channel  << " where he/she is not a member" << endl;
			}
			else
			{
				channels[channel].erase(channel_user_iter);
				//existing_channel_users.erase(channel_user_iter);
				//cout << "server: " << username << " leaves channel " << channel <<endl;

				//delete channel if no more users
				if (channels[channel].empty() && (channel != "Common"))
				{
					channels.erase(channel_iter);
					cout << "server: " << "removing empty channel " << channel <<endl;
				}

			}


		}




	}



}




void handle_say_message(void *data, struct sockaddr_in sock)
{

	//check whether the user is in usernames
	//if yes check whether channel is in channels
	//check whether the user is in the channel
	//if yes send the message to all the members of the channel
	//if not send an error message to the user


	//get message fields
	struct request_say* msg;
	msg = (struct request_say*)data;

	string channel = msg->req_channel;
	string text = msg->req_text;


	string ip = inet_ntoa(sock.sin_addr);

	int port = sock.sin_port;

 	char port_str[6];
 	sprintf(port_str, "%d", port);
	string key = ip + "." +port_str;

    cout << this_Server << ":" << this_Port << " " << ip << ":" << port
    << " recv Request Say " << channel << " \"" << text << "\"" << endl;

	//check whether key is in rev_usernames
	map <string,string> :: iterator iter;


	iter = rev_usernames.find(key);
	if (iter == rev_usernames.end() )
	{
		//ip+port not recognized - send an error message
		send_error_message(sock, "Not logged in ");
	}
	else
	{
		string username = rev_usernames[key];

		map<string,channel_type>::iterator channel_iter;

		channel_iter = channels.find(channel);

		active_usernames[username] = 1;
        
        if (linkingCtoS.find(channel) != linkingCtoS.end()){
            list<struct sockaddr_in> list_of_serv = linkingCtoS[channel];
            long long ID = generating_unique_id();
            if(ID == 0){
                perror("Failed to generate Unique ID");
            }
            struct request_say_S2S sS2S;
            sS2S.req_type = (REQ_S2S_S);
            //printf("This is the say req_type: %d\n", sS2S.req_type);
            sS2S.unique_id = ID;
            //printf("ATTENTION the long long uid that we made %llu\n", sS2S.unique_id);
            strcpy(sS2S.req_channel, channel.c_str());
            strcpy(sS2S.req_username, username.c_str());
            strcpy(sS2S.req_text, text.c_str());
            for(list< struct sockaddr_in>::iterator it = list_of_serv.begin(); it != list_of_serv.end() ; it++ ){
                struct sockaddr_in ad = *it;
                if (sendto(s, &sS2S, sizeof(sS2S), 0, (struct sockaddr*)&ad, sizeof ad) == -1){
                    perror("cannot set to that server at the moment");
                }
            }
        }
        
        // if this did not work try this if (channel_iter != channels.end())
		if (channel_iter == channels.end())
		{
			//channel not found
			send_error_message(sock, "No channel by the name " + channel);
			cout << "server: " << username << " trying to send a message to non-existent channel " << channel << endl;

		}
		else
		{
			//channel already exits
			//map<string,struct sockaddr_in> existing_channel_users;
			//existing_channel_users = channels[channel];
			map<string,struct sockaddr_in>::iterator channel_user_iter;
			channel_user_iter = channels[channel].find(username);

			if (channel_user_iter == channels[channel].end())
			{
				//user not in channel
				send_error_message(sock, "You are not in channel " + channel);
				cout << "server: " << username << " trying to send a message to channel " << channel  << " where he/she is not a member" << endl;
			}
			else
			{
				map<string,struct sockaddr_in> existing_channel_users;
				existing_channel_users = channels[channel];
				for(channel_user_iter = existing_channel_users.begin(); channel_user_iter != existing_channel_users.end(); channel_user_iter++)
				{
					//cout << "key: " << iter->first << " username: " << iter->second << endl;

					ssize_t bytes;
					void *send_data;
					size_t len;

					struct text_say send_msg;
					send_msg.txt_type = TXT_SAY;

					const char* str = channel.c_str();
					strcpy(send_msg.txt_channel, str);
					str = username.c_str();
					strcpy(send_msg.txt_username, str);
					str = text.c_str();
					strcpy(send_msg.txt_text, str);
					//send_msg.txt_username, *username.c_str();
					//send_msg.txt_text,*text.c_str();
					send_data = &send_msg;

					len = sizeof send_msg;

					//cout << username <<endl;
					struct sockaddr_in send_sock = channel_user_iter->second;


					//bytes = sendto(s, send_data, len, 0, (struct sockaddr*)&send_sock, fromlen);
					bytes = sendto(s, send_data, len, 0, (struct sockaddr*)&send_sock, sizeof send_sock);

					if (bytes < 0)
					{
						perror("Message failed\n"); //error
					}
					else
					{
						//printf("Message sent\n");

					}

				}
				//cout << "server: " << username << " sends say message in " << channel <<endl;

			}


		}




	}



}


void handle_list_message(struct sockaddr_in sock)
{

	//check whether the user is in usernames
	//if yes, send a list of channels
	//if not send an error message to the user



	string ip = inet_ntoa(sock.sin_addr);

	int port = sock.sin_port;

 	char port_str[6];
 	sprintf(port_str, "%d", port);
	string key = ip + "." +port_str;


	//check whether key is in rev_usernames
	map <string,string> :: iterator iter;

    cout << this_Server << ":" << this_Port << " " << ip << ":" << port
    << " recv Request List " << endl;

	iter = rev_usernames.find(key);
	if (iter == rev_usernames.end() )
	{
		//ip+port not recognized - send an error message
		send_error_message(sock, "Not logged in ");
	}
	else
	{
		string username = rev_usernames[key];
		int size = channels.size();
		//cout << "size: " << size << endl;

		active_usernames[username] = 1;

		ssize_t bytes;
		void *send_data;
		size_t len;


		//struct text_list temp;
		struct text_list *send_msg = (struct text_list*)malloc(sizeof (struct text_list) + (size * sizeof(struct channel_info)));


		send_msg->txt_type = TXT_LIST;

		send_msg->txt_nchannels = size;


		map<string,channel_type>::iterator channel_iter;



		//struct channel_info current_channels[size];
		//send_msg.txt_channels = new struct channel_info[size];
		int pos = 0;

		for(channel_iter = channels.begin(); channel_iter != channels.end(); channel_iter++)
		{
			string current_channel = channel_iter->first;
			const char* str = current_channel.c_str();
			//strcpy(current_channels[pos].ch_channel, str);
			//cout << "channel " << str <<endl;
			strcpy(((send_msg->txt_channels)+pos)->ch_channel, str);
			//strcpy(((send_msg->txt_channels)+pos)->ch_channel, "hello");
			//cout << ((send_msg->txt_channels)+pos)->ch_channel << endl;

			pos++;

		}



		//send_msg.txt_channels =
		//send_msg.txt_channels = current_channels;
		send_data = send_msg;
		len = sizeof (struct text_list) + (size * sizeof(struct channel_info));

					//cout << username <<endl;
		struct sockaddr_in send_sock = sock;


		//bytes = sendto(s, send_data, len, 0, (struct sockaddr*)&send_sock, fromlen);
		bytes = sendto(s, send_data, len, 0, (struct sockaddr*)&send_sock, sizeof send_sock);

		if (bytes < 0)
		{
			perror("Message failed\n"); //error
		}
		else
		{
			//printf("Message sent\n");

		}

		//cout << "server: " << username << " lists channels"<<endl;


	}



}


void handle_who_message(void *data, struct sockaddr_in sock)
{


	//check whether the user is in usernames
	//if yes check whether channel is in channels
	//if yes, send user list in the channel
	//if not send an error message to the user


	//get message fields
	struct request_who* msg;
	msg = (struct request_who*)data;

	string channel = msg->req_channel;

	string ip = inet_ntoa(sock.sin_addr);

	int port = sock.sin_port;

 	char port_str[6];
 	sprintf(port_str, "%d", port);
	string key = ip + "." +port_str;

    cout << this_Server << ":" << this_Port << " " << ip << ":" << port
    << " recv Request Who " << channel << endl;

	//check whether key is in rev_usernames
	map <string,string> :: iterator iter;


	iter = rev_usernames.find(key);
	if (iter == rev_usernames.end() )
	{
		//ip+port not recognized - send an error message
		send_error_message(sock, "Not logged in ");
	}
	else
	{
		string username = rev_usernames[key];

		active_usernames[username] = 1;

		map<string,channel_type>::iterator channel_iter;

		channel_iter = channels.find(channel);

		if (channel_iter == channels.end())
		{
			//channel not found
			send_error_message(sock, "No channel by the name " + channel);
			cout << "server: " << username << " trying to list users in non-existing channel " << channel << endl;

		}
		else
		{
			//channel exits
			map<string,struct sockaddr_in> existing_channel_users;
			existing_channel_users = channels[channel];
			int size = existing_channel_users.size();

			ssize_t bytes;
			void *send_data;
			size_t len;


			//struct text_list temp;
			struct text_who *send_msg = (struct text_who*)malloc(sizeof (struct text_who) + (size * sizeof(struct user_info)));


			send_msg->txt_type = TXT_WHO;

			send_msg->txt_nusernames = size;

			const char* str = channel.c_str();

			strcpy(send_msg->txt_channel, str);



			map<string,struct sockaddr_in>::iterator channel_user_iter;

			int pos = 0;

			for(channel_user_iter = existing_channel_users.begin(); channel_user_iter != existing_channel_users.end(); channel_user_iter++)
			{
				string username = channel_user_iter->first;

				str = username.c_str();

				strcpy(((send_msg->txt_users)+pos)->us_username, str);


				pos++;



			}

			send_data = send_msg;
			len = sizeof(struct text_who) + (size * sizeof(struct user_info));

						//cout << username <<endl;
			struct sockaddr_in send_sock = sock;


			//bytes = sendto(s, send_data, len, 0, (struct sockaddr*)&send_sock, fromlen);
			bytes = sendto(s, send_data, len, 0, (struct sockaddr*)&send_sock, sizeof send_sock);

			if (bytes < 0)
			{
				perror("Message failed\n"); //error
			}
			else
			{
				//printf("Message sent\n");

			}

			//cout << "server: " << username << " lists users in channnel "<< channel << endl;




			}




	}




}



void send_error_message(struct sockaddr_in sock, string error_msg)
{
	ssize_t bytes;
	void *send_data;
	size_t len;

	struct text_error send_msg;
	send_msg.txt_type = TXT_ERROR;

	const char* str = error_msg.c_str();
	strcpy(send_msg.txt_error, str);

	send_data = &send_msg;

	len = sizeof send_msg;


	struct sockaddr_in send_sock = sock;



	bytes = sendto(s, send_data, len, 0, (struct sockaddr*)&send_sock, sizeof send_sock);

	if (bytes < 0)
	{
		perror("Message failed\n"); //error
	}
	else
	{
		//printf("Message sent\n");

	}





}

void handle_join_S2S(void *data, struct sockaddr_in socket){
    struct request_join_S2S *j = (struct request_join_S2S *)data;
    string chan = j->req_channel;
    string ip = inet_ntoa(socket.sin_addr);
    int port = ntohs(socket.sin_port);
    char PtoS[6];
    sprintf(PtoS, "%d", port);
    string combined = ip + "." + PtoS;
    
    cout << this_Server << ":" << this_Port << " " << ip << ":" << port
    << " recv S2S join " << chan << endl;
    
    if(linkingCtoS.find(chan) == linkingCtoS.end()){
        list<struct sockaddr_in> serversAD;
        for(list<struct sockaddr_in>::iterator miter = serverAddrs.begin(); miter != serverAddrs.end(); miter++){
        	//printf("%d\n", miter->sin_port);
        	//printf("%d\n", ntohs(socket.sin_port));
            if(socket.sin_addr.s_addr == miter->sin_addr.s_addr && ntohs(socket.sin_port) == miter->sin_port)
                continue;
            /*--------checking
             printf(" ppppppppppppppppppppppppppppppppppppp   %d\n", port);
             if(strcmp((const char *) ip.c_str(),(const char *) this_Server) == 0){
             if(port == this_Port){
             printf("heeeeeeeerrererreererer");
             continue;
             }
             
             }
             
             */
            cout << this_Server << ":" << this_Port << " " << inet_ntoa(miter->sin_addr) << ":" << ntohs(miter->sin_port)
            << " send S2S join " << chan << endl;
            serversAD.push_back(*miter);
            if (sendto(s, data, sizeof(*(struct request_join_S2S*)data), 0,  (struct sockaddr*) &(*miter), sizeof(*miter)) == -1){
                perror("cannot send join");
            }
        }
        linkingCtoS[chan] = serversAD;
    }
/*
	cout << "servers that are in the channel: " << chan.c_str() << endl;
	for(list<struct sockaddr_in>::iterator miter = linkingCtoS[chan].begin(); miter != linkingCtoS[chan].end(); miter++){
		cout << inet_ntoa(miter->sin_addr) << ":" << ntohs(miter->sin_port) << endl;
	}
    cout << "Size of linking c to s: " << linkingCtoS[chan].size() << endl;
	cout << "size of usernames " << usernames.size() << endl;
	cout << "I am exiting from this port " << this_Port << endl;
	exit(1);*/
    
}

void handle_leave_S2S(void *data, struct sockaddr_in socket){
    struct request_leave_S2S *l = (struct request_leave_S2S *)data;
    string chan = l->req_channel;
    string ip = inet_ntoa(socket.sin_addr);
    int port = ntohs(socket.sin_port);
    char PtoS[6];
    sprintf(PtoS, "%d", port);
    string combined = ip + "." + PtoS;
    
    cout << this_Server << ":" << this_Port << " " << ip << ":" << port
    << " recv S2S leave " << chan << endl;
    if(linkingCtoS.find(chan) != linkingCtoS.end()){
        list<struct sockaddr_in> l_serverAD = linkingCtoS[chan];
        for(list<struct sockaddr_in>::iterator miter = l_serverAD.begin(); miter != l_serverAD.end(); miter++){
            socket.sin_addr = miter->sin_addr;
            if(miter->sin_addr.s_addr == socket.sin_addr.s_addr && miter->sin_port == (socket.sin_port)){
                l_serverAD.erase(miter);
                break;
            }
        }
        linkingCtoS.erase(chan);
        linkingCtoS[chan] = l_serverAD;
    }
}

void handle_say_S2S(void *data, struct sockaddr_in socket){
    struct request_say_S2S *s_s = (struct request_say_S2S*)data;
    
    long long uid = s_s->unique_id;;
    string chan = s_s->req_channel;
    string user = s_s->req_username;
    string text = s_s->req_text;

    s_s->req_type = (REQ_S2S_S);

    //printf("%d\n", s_s->req_type);
    
    string ip = inet_ntoa(socket.sin_addr);
    int port = (socket.sin_port);
    char PtoS[6];
    sprintf(PtoS, "%d", port);
    string combined = ip + "." + PtoS;
    
    cout << this_Server << ":" << this_Port << " " << ip << ":" << port
    << " recv S2S say " << user << " " << chan << " \"" << text << "\"" << endl;
    
    map <string, string>::iterator iter;
    
    if(linkingCtoS.find(chan) != linkingCtoS.end()){
    	//cout << "Size of linking c to s: " << linkingCtoS[chan].size() << endl;
        if(linkingCtoS[chan].size() <= 1 && usernames.empty()){
            struct request_leave_S2S l;
            l.req_type = (REQ_S2S_lev);
            strcpy(l.req_channel, chan.c_str());
            if (sendto(s, &l, sizeof(l), 0,  (struct sockaddr*)&socket, sizeof(socket)) == -1){
                perror("cannot send leave from handle_say_S2S");
            }
            return;
        }
    }
    
    for(list<long long>::iterator miter = uids.begin(); miter != uids.end(); miter++){
        if (*miter == uid){
            struct request_leave_S2S ll;
            ll.req_type = (REQ_S2S_lev);
            strcpy(ll.req_channel, chan.c_str());
            if (sendto(s, &ll, sizeof(ll), 0,  (struct sockaddr*)&socket, sizeof(socket)) == -1){
                perror("cannot send leave from handle_say_S2S, in 1st loop");
            }
            return;
        }

    }
    
    uids.push_back(uid);
    for(;uids.size() >= 64;){
        uids.pop_front();
    }
    
    if (channels.find(chan) != channels.end()){
        map<string, struct sockaddr_in> list_of_clients = channels[chan];
        for(map<string, struct sockaddr_in>::iterator miter = list_of_clients.begin(); miter != list_of_clients.end() ; miter++ ){
            struct text_say ss;
            ss.txt_type = TXT_SAY;
            strcpy(ss.txt_channel, chan.c_str());
            strcpy(ss.txt_username, user.c_str());
            strcpy(ss.txt_text, text.c_str());
            struct sockaddr_in ssend = miter->second;
            if (sendto(s, &ss, sizeof(ss), 0, (struct sockaddr*)&ssend, sizeof (ssend)) == -1){
                perror("cannot send text_say from handle_say_S2S");
            }
        }
    }
    if (linkingCtoS.find(chan) != linkingCtoS.end()){
        list<struct sockaddr_in> list_serv = linkingCtoS[chan];
        for(list< struct sockaddr_in>::iterator miter = list_serv.begin(); miter != list_serv.end(); miter++ ){
            if(socket.sin_addr.s_addr == miter->sin_addr.s_addr && socket.sin_port == miter->sin_port )
                continue;
            struct sockaddr_in socketsend = *miter;
            if (sendto(s, &s_s, sizeof(*s_s), 0, (struct sockaddr*)&socketsend, sizeof(socketsend)) == -1){
                perror("cannpt send text_say from 2nd loop");
            }            
        } 
    }
    
}

void ip_port(struct sockaddr_in* socket){
    string ip = inet_ntoa(socket->sin_addr);
    int port = socket->sin_port;
    char PtoS[6];
    sprintf(PtoS, "%d", port);
    //string combined = ip + ":" + PtoS;
    
    //cout << this_Server << ":" << this_Port << " " << combined
    //<< info << " " << chan << endl;

}

long long generating_unique_id(){
    long long user_unique_id= 0LL;
    int file= open("/dev/urandom", O_RDONLY);
    if (file == -1){
        return 0;
    }
    else if (read(file, &user_unique_id, 8) == 0){
        return 0;
    }
    return user_unique_id;
}

void remove_Addr(){
	for(list<struct sockaddr_in>::iterator outside = serverAddrs.begin(); outside != serverAddrs.end() ; outside++){
		int flag = 0;
        for(list<struct sockaddr_in>::iterator inside = hardstateAddrs.begin(); inside != hardstateAddrs.end() ; inside++){
        	if(outside->sin_addr.s_addr == inside->sin_addr.s_addr && outside->sin_port == inside->sin_port)
        		flag = 1;
        }
        if(flag == 0){
        	serverAddrs.erase(outside);
        }

	}

}




