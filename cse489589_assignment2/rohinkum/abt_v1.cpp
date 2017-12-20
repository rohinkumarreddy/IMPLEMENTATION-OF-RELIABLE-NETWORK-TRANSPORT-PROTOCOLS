#include "../include/simulator.h"
#include<cstring>
#include<iostream>
using namespace std;
/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
/* called from layer 5, passed the data to be sent to other side */
struct pkt packet_A,ack_packet_B,buffer[1000];
static int buff_count=0,intransit=0,expected_seqnum=0,expected_acknum=0,*ptr=&expected_seqnum,*ptr1=&expected_acknum,*ptr2=&intransit,*ptr3=&buff_count;
static char data_B[20],data_A[20];
void A_output(struct msg message)
{
	int temp=0,tmp_buff=0;
	temp=*ptr2;//ptr2=intransit
	tmp_buff=*ptr3;//ptr3=buff_count
	memset(data_B,'\0',sizeof(data_B));
	char tmp[20];//used for checksum
	memset(packet_A.payload,'\0',sizeof(packet_A.payload));
	if(packet_A.seqnum==0)//if previous seqnum=0
	{
		packet_A.seqnum=1;
	}
	else
	{
		packet_A.seqnum=0;
	}
	strcpy(packet_A.payload,message.data);//copying data from message to payload in packet_A
	for(int j=0;j<20;j++)
	{
		tmp[j]=message.data[j];//copying message to temp
	}
	packet_A.acknum=0;//default value for packet from sender A
	packet_A.checksum=packet_A.seqnum;//start value of checksum
	for(int i=0;i<20;i++) 
	{
		packet_A.checksum+=tmp[i];//computing checksum
	}
	//if(temp==0)//no intransit no buffered msgs	
	//{
		tolayer3(0, packet_A);//send packet_A from layer5 to layer3 in A(0)
		*ptr2=1;//intransit
	//}
	/*else if(buff_count==0)
	{
		
	}*/
	/*if((temp==0)&&(buff_count==0))//no intransit no buffered msgs
	{
		tolayer3(0, packet_A);//send packet_A from layer5 to layer3 in A
		*ptr2=1;//intransit
	}
	else if(temp>0)//a msg is in transit
	{
		buff_cont+=1;//increasing buffer count
		for(int k=0;k<20;k++)
		{
			buffer[buff_count][k]=packet_A.payload[k];//copying to buffer
		}
	}*/

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
	int tmp21,tmp22,chsum2=0,chsum21=0;
	tmp21=expected_acknum;
	tmp22=packet.acknum;
	chsum2=packet.checksum;
	chsum21=tmp22;//ex chsum
	/*for(int i=0;i<20;i++) //we are not using this as the payload is not filled
	{
		data_A[i]=packet.payload[i];
		chsum21+=packet.payload[i];//computing checksum
	}*/
	if(tmp21==tmp22)//checking order of packets for losses(ex acknum==pk acknum)
	{
		if(chsum21==chsum2)//checking packet checksum for corruption(ex chsum==pk chsum)
		{
			*ptr2=0;//intransit
		}
	}
	if(tmp21==0)
	{
		*ptr1=1;//expected acknum
	}
	else
	{
		*ptr1=0;//expected acknum
	}
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
	
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
	packet_A.seqnum=1;
	*ptr1=0;//expected acknum
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{	
	int tmp1,tmp2,chsum=0,chsum1=0;
	tmp1=expected_seqnum;
	tmp2=packet.seqnum;
	chsum=packet.checksum;//pk chsum
	chsum1=tmp2;//calc chsum
	//strcpy(data_B, packet.payload);//copying received message
	for(int i=0;i<20;i++) 
	{
		data_B[i]=packet.payload[i];
		chsum1+=packet.payload[i];//computing checksum
	}
	if(tmp1==tmp2)//checking order of packets for losses(exp sqnum == pk sqnum)
	{
		if(chsum1==chsum)//checking packet checksum for corruption(calc chsum == pk chsum)
		{
			tolayer5(1,data_B);//sending recv msg data to layer5 in B(1)
			//ack mechanism
			ack_packet_B.seqnum=0;//default value of seqnum
			ack_packet_B.acknum=tmp2;//pk seqnum
			ack_packet_B.checksum=tmp2;
			//memset();//clearing payload memory	
			tolayer3(1, ack_packet_B);//send ack_packet_B to layer3 in B(1)
			if(tmp1==0)//prev ex seqnum
			{
				*ptr=1;//expected seqnum
			}
			else
			{
				*ptr=0;//expected seqnum
			}		
		}
	}
	
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
	*ptr=0;//expected seqnum
}
