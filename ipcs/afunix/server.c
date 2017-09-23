#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
	int server_sockfd, client_sockfd;
	int server_len, client_len;
	struct sockaddr_un server_address;      /*����һ��UNIX���׽��ֽṹ*/
	struct sockaddr_un client_address;
	int i, bytes;
	char ch_send, ch_recv;
	
	unlink ("server_socket");       /*ɾ��ԭ��server_socket����*/
	
	/*���� socket, ͨ��Э��ΪAF_UNIX, SCK_STREAM ���ݷ�ʽ*/
	server_sockfd = socket (AF_UNIX, SOCK_STREAM, 0);
	
	/*���÷�������Ϣ(ͨ��Э��)*/
	server_address.sun_family = AF_UNIX;
	
	/*���÷�������Ϣ(socket ����)*/
	strcpy (server_address.sun_path, "server_socket");
	
	/*���÷�������Ϣ(��������ַ����)*/
	server_len = sizeof (server_address);
	
	/*�� socket ����*/
	bind (server_sockfd, (struct sockaddr *)&server_address, server_len);
	
	/*��������,������Ϊ5*/
	listen (server_sockfd, 5);
	
	printf ("Server is waiting for client connect...\n");
	
	client_len = sizeof (client_address);
	
	/*���ܿͻ�������; ��2�����������洢�ͻ��˵�ַ; ��3�����������洢�ͻ��˵�ַ�Ĵ�С*/
	/*����(����)һ�����ͻ��˵��ļ�������,���ԶԿͻ��˵Ķ�д����*/
	client_sockfd = accept (server_sockfd, (struct sockaddr *)&server_address, (socklen_t *)&client_len);
	if (client_sockfd == -1)
	{
	  perror ("accept");
	  exit (EXIT_FAILURE);
	}
	
	printf ("The server is waiting for client data...\n");
	
	for (i = 0, ch_send = '1'; i < 5; i++, ch_send++)
	{
    if((bytes = read (client_sockfd, &ch_recv, 1)) == -1)
    {
      perror ("read");
      exit (EXIT_FAILURE);
    }

    printf ("The character receiver from client is %c\n", ch_recv);
    sleep (1);

    if((bytes = write (client_sockfd, &ch_send, 1)) == -1)
    {
    	perror ("read");
      exit (EXIT_FAILURE);
    }
	}
	
	close(client_sockfd);
	unlink ("server socket");
}
