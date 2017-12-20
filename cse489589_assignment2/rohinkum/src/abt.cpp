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
static struct pkt packet_A,ack_packet_B;
struct tmp {char buff[20];};//definition
static struct tmp buffer[1000];
static int buff_count=0,in_transit=0,exp_seqnum=0,exp_acknum=0;
void A_output(struct msg message)
{//cout<<"in transit "<<in_transit<<"\nbuff count "<<buff_count<<"message "<<message.data<<endl;
	if(buff_count>0)
	{
		if(in_transit==0)
		{
			buff_count+=1;//buff_count++
			for(int p=0;p<20;p++)
			{
				buffer[buff_count-1].buff[p]=message.data[p];//copying data in message to buffer
			}
			memset(message.data,'\0',sizeof(message.data));
			//cout<<"msg buffered "<<buff_count<<endl;
			//sending buffered msg buffer[0]
			//building packet_A
			memset(packet_A.payload,'\0',sizeof(packet_A.payload));//cout<<"\nbuffer value "<<buffer[0].buff<<endl;
			for(int l=0;l<20;l++)
			{
				packet_A.payload[l]=buffer[0].buff[l];//copying data in buffer[0] to packet payload
			}memset(buffer[0].buff,'\0',sizeof(buffer[0].buff));
			packet_A.acknum=0;
			packet_A.checksum=packet_A.seqnum+packet_A.acknum;//initial checksum value
			for(int l=0;l<20;l++)
			{
				packet_A.checksum+=packet_A.payload[l];//computing checksum
			}
			tolayer3(0,packet_A);//sending packet to layer3
			in_transit=1;// wait for ack - packet in transit
			starttimer(0,15.0);//starting timer for 10 time units
			//reordering buffer
			memset(buffer[0].buff,'\0',sizeof(buffer[0].buff));
			for(int m=0;m<buff_count-1;m++)
			{
				for(int d=0;d<20;d++)
				{
					buffer[m].buff[d]=buffer[m+1].buff[d];
				}
			}
			buff_count-=1;//buff_count--
		}
		else if(in_transit==1)
		{//buffer the msg
			buff_count+=1;//buff_count++
			//cout<<"updated buff count "<<buff_count<<endl;
			for(int q=0;q<20;q++)
			{
				buffer[buff_count-1].buff[q]=message.data[q];//copying data in message to buffer
			}
			//cout<<"\nbuffer["<<buff_count-1<<"] value "<<buffer[buff_count-1].buff<<"\nbuffer[0] "<<buffer[0].buff<<"\nmessage "<<message.data<<endl;
			memset(message.data,'\0',sizeof(message.data));
			//cout<<"msg buffered "<<buff_count<<endl;
		}
	}
	else if(in_transit==0)
	{
		//building packet_A
		memset(packet_A.payload,'\0',sizeof(packet_A.payload));
		for(int i=0;i<20;i++)
		{
			packet_A.payload[i]=message.data[i];//copying data in message to packet payload
		}
		memset(message.data,'\0',sizeof(message.data));
		packet_A.acknum=0;
		packet_A.checksum=packet_A.seqnum+packet_A.acknum;//initial checksum value
		for(int j=0;j<20;j++)
		{
			packet_A.checksum+=packet_A.payload[j];//computing checksum
		}
		tolayer3(0,packet_A);//sending packet to layer3
		in_transit=1;// wait for ack - packet in transit
		starttimer(0,15.0);//starting timer for 10 time units
		/*if(packet_A.seqnum==0)//this should be in A_input as in the case of wrong ack received resend packet_A(seqnum should not change)
		{
			packet_A.seqnum=1;
		}
		else
		{
			packet_A.seqnum=0;
		}*/
	}
	else if(in_transit==1)
	{//buffer the msg
		buff_count+=1;//buff_count++
		for(int k=0;k<20;k++)
		{
			buffer[buff_count-1].buff[k]=message.data[k];//copying data in message to buffer
		}
		memset(message.data,'\0',sizeof(message.data));
		//cout<<"buffer["<<buff_count-1<<"] value "<<buffer[buff_count-1].buff<<endl;
		//cout<<"msg buffered "<<buff_count<<endl;
		/*if(buff_count>1)//buff_count previously incremented
		{
			//sending buffered msg buffer[0]
			//building packet_A
			strcpy(packet_A.payload,buffer[0]);//copying data in buffer[0] to packet payload
			packet_A.checksum=packet_A.seqnum;//initial checksum value
			for(int l=0;l<20;l++)
			{
				packet_A.checksum+=packet_A.payload[l];//computing checksum
			}
			tolayer3(0,packet_A);//sending packet to layer3
			in_transit=1;// wait for ack - packet in transit
			//reordering buffer
			for(int m=0;m<buff_count-1;m++)
			{
				strcpy(buffer[m],buffer[m+1]);
			}
			buff_count-=1;//buff_count--
		}*/
	}
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
	int temp_ack_checksum=0;
	if(exp_acknum==packet.acknum)
	{//cout<<"exp_acknum==packet.acknum"<<endl;
		temp_ack_checksum=packet.acknum+packet.seqnum;//initial checksum value
		for(int i=0;i<20;i++)
		{
			temp_ack_checksum+=packet.payload[i];//computing ack_checksum
		}
		if(temp_ack_checksum==packet.checksum)
		{
			in_transit=0;//ack received - no packet in transit
			//cout<<"in_transit "<<in_transit<<endl;
			stoptimer(0);//stopping timer as the packet ack is received
			if(exp_acknum==0)
			{
				exp_acknum=1;
			}
			else
			{
				exp_acknum=0;
			}
			if(packet_A.seqnum==0)//this should be in A_input as in the case of wrong ack received, resend packet_A(seqnum should not change)
			{
				packet_A.seqnum=1;
			}
			else
			{
				packet_A.seqnum=0;
			}
			if(buff_count>0)
			{
				//building packet_A
				memset(packet_A.payload,'\0',sizeof(packet_A.payload));//cout<<"\nbuffer[0] value is "<<buffer[0].buff<<endl;
				for(int n=0;n<20;n++)
				{
					packet_A.payload[n]=buffer[0].buff[n];//copying data in buffer[0] to packet payload
				}
				packet_A.acknum=0;
				packet_A.checksum=packet_A.seqnum+packet_A.acknum;//initial checksum value
				for(int l=0;l<20;l++)
				{
					packet_A.checksum+=packet_A.payload[l];//computing checksum
				}
				tolayer3(0,packet_A);//sending packet to layer3
				in_transit=1;// wait for ack - packet in transit
				starttimer(0,15.0);//starting timer for 10 time units
				//reordering buffer
				memset(buffer[0].buff,'\0',sizeof(buffer[0].buff));
				for(int m=0;m<buff_count-1;m++)
				{
					for(int d=0;d<20;d++)
					{
						buffer[m].buff[d]=buffer[m+1].buff[d];
					}
				}
				buff_count-=1;//buff_count--
				//cout<<"buff_count is updated "<<buff_count<<endl;
			}
		}
	}
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
	//cout<<"TIMEOUT!!"<<endl;
	tolayer3(0,packet_A);//sending packet to layer3
	in_transit=1;// wait for ack - packet in transit
	//stoptimer(0);
	starttimer(0,15.0);//starting timer for 10 time units
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
	//initializing all data fields in the packet_A struct
	packet_A.seqnum=0;
	packet_A.acknum=0;//default value - not used
	packet_A.checksum=0;
	memset(packet_A.payload,'\0',sizeof(packet_A.payload));
	exp_acknum=0;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{	
	char msg[20];
	int temp_checksum=0;
	memset(msg,'\0',sizeof(msg));
	ack_packet_B.seqnum=0;//default value - not used	
	memset(ack_packet_B.payload,'\0',sizeof(ack_packet_B.payload));//default value - not changed, used for checksum
	if(exp_seqnum==packet.seqnum)
	{
		temp_checksum=packet.seqnum+packet.acknum;//initial value of checksum	
		for(int i=0;i<20;i++)
		{
			temp_checksum+=packet.payload[i];//computing checksum
		}
		if(temp_checksum==packet.checksum)
		{
			for(int j=0;j<20;j++)
			{
				msg[j]=packet.payload[j];//copying payload from packet to msg
			}
			tolayer5(1,msg);//sending message to layer5
			//building ack_packet_B
			ack_packet_B.seqnum=0;
			ack_packet_B.checksum=ack_packet_B.acknum+ack_packet_B.seqnum;
			for(int k=0;k<20;k++)
			{
				ack_packet_B.checksum+=ack_packet_B.payload[k];//computing checksum
			}
			tolayer3(1,ack_packet_B);//sending ack_packet_B to layer3
			if(exp_seqnum==0)
			{
				exp_seqnum=1;
			}
			else
			{
				exp_seqnum=0;
			}
			if(ack_packet_B.acknum==0)
			{
				ack_packet_B.acknum=1;
			}
			else
			{
				ack_packet_B.acknum=0;
			}
		}
		else
		{//send previous ack
			if(ack_packet_B.acknum==0)//resetting acknum to prev value
			{
				ack_packet_B.acknum=1;
			}
			else
			{
				ack_packet_B.acknum=0;
			}
			tolayer3(1,ack_packet_B);//Resending ack_packet_B to layer3
			if(ack_packet_B.acknum==0)//changing acknum value for next packet
			{
				ack_packet_B.acknum=1;
			}
			else
			{
				ack_packet_B.acknum=0;
			}
		}
	}
	else
	{//send previous ack
		if(ack_packet_B.acknum==0)//resetting acknum to prev value
		{
			ack_packet_B.acknum=1;
		}
		else
		{
			ack_packet_B.acknum=0;
		}
		tolayer3(1,ack_packet_B);//Resending ack_packet_B to layer3
		if(ack_packet_B.acknum==0)//changing acknum value for next packet
		{
			ack_packet_B.acknum=1;
		}
		else
		{
			ack_packet_B.acknum=0;
		}
	}
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
	//initializing all data fields in the ack_packet_B struct
	ack_packet_B.seqnum=0;//default value - not used
	ack_packet_B.acknum=0;
	ack_packet_B.checksum=0;
	memset(ack_packet_B.payload,'\0',sizeof(ack_packet_B.payload));//default value - not changed, used for checksum
	exp_seqnum=0;
}
