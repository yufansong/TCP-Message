#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/if_ether.h>
#include <fcntl.h>
#include <signal.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <syslog.h>
#include <sys/shm.h>
/*--------------------------�궨��-----------------------------------*/
#define MAX_CONFIG 20
#define MAX_LENGTH 128
#define MAX_ETHERNET_LENGTH 14
#define MAX_IP_LENGTH 20
#define MAX_TCP_LENGTH 20
/*  ???
����� 0�ֽ�
������·�� Ethernetͷ(14�ֽ�)
����� 20�ֽ�ipͷ
����� 20�ֽ�tcpͷ
Ӧ�ò� 20�ֽ�����
*/
#define W_FILE_NAME_0 "receiver.dat"
#define W_FILE_NAME_1 "sender.dat"
#define W_FILE_NAME_2 "network.dat"
#define W_FILE_NAME_3 "tcp.dat"
#define W_FILE_NAME_4 "ip.dat"
#define R_FILE_NAME "../common/network.conf"

//��ȡ�����ļ�
#define SUCCESS 0x00 /*�ɹ�*/
#define FAILURE 0x01 /*ʧ��*/

#define FILENAME_NOTEXIST 0x02    /*�����ļ���������*/
#define SECTIONNAME_NOTEXIST 0x03 /*����������*/
#define KEYNAME_NOTEXIST 0x04     /*����������*/
#define STRING_LENNOTEQUAL 0x05   /*�����ַ������Ȳ�ͬ*/
#define STRING_NOTEQUAL 0x06      /*�����ַ������ݲ���ͬ*/
#define STRING_EQUAL 0x00         /*�����ַ���������ͬ*/

struct _Ether_pkg
{
    /* ǰ����ethernetͷ */
    unsigned char ether_dhost[6];  /* Ŀ��Ӳ����ַ */
    unsigned char ether_shost[6];  /* ԴӲ����ַ */
    unsigned short int ether_type; /* �������� */
};

/*--------------------------sender����-----------------------------------*/
int send_application(char *buf);
void send_transport(char *buf);
void send_network(char *buf);
void send_datalink(char *buf);
void send_physics(char *buf);

/*--------------------------reciever����-----------------------------------*/
void recieve_application(unsigned char *buf, int data_len);
void recieve_transport(unsigned char *buf);
int recieve_network(unsigned char *buf);
void recieve_datalink(unsigned char *buf);
void recieve_physics(unsigned char *buf);

/*--------------------------���+д�ļ�����-----------------------------------*/
void get_random_byte(char *buf, int length); //ÿ�ζ�ȡһ���ֽڵ��������ֽ�
void write_file(char *str_write, const char *file_name,int length);
unsigned char str_to_hex(const char *str);

/*--------------------------��������ļ�����-----------------------------------*/
int get_config_str(char *SectionName, char *KeyName, char *str); //����ȥ��str����Ҫ��ȡ���ַ���
int get_config_int(char *SectionName, char *KeyName);            //����ֵ�Ƕ�ȡ��int8
int CompareString(char *pInStr1, char *pInStr2);
int GetKeyValue(FILE *fpConfig, char *pInKeyName, char *pOutKeyValue);
int GetConfigIntValue(char *pInFileName, char *pInSectionName, char *pInKeyName, int *pOutKeyValue);
int GetConfigStringValue(char *pInFileName, char *pInSectionName, char *pInKeyName, char *pOutKeyValue);

/*--------------------------tcp����-----------------------------------*/
int creator_tcphdr_ip(struct ip *header_ip, struct sockaddr_in *src_addr,
                      struct sockaddr_in *dst_addr, int len_total);
int creator_header_tcp(struct tcphdr *header_tcp);
unsigned short creator_check_sum(const char *buf, int protocol);

int creator_tcp(void);