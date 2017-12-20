#include "../include/simulator.h"
#include<iostream>
#include<cstring>
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
static struct pkt packet_A,ack_packet_B,win_buffer[100],buffer[1000];
struct tmp {double cur_time_val; int seqnum;};//definition
static struct tmp timer_buffer[1000];
static int buff_count=0,exp_seqnum=0,exp_acknum=0,base=0,next_seqnum=0,win_size_l=getwinsize(),win_buffer_count=0,timer_A=0,time_buf_count=0,no_val=0;
/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
	if(next_seqnum<base+win_size_l)
	{
		//building packet_A
		memset(packet_A.payload,'\0',sizeof(packet_A.payload));
		for(int i=0;i<20;i++)
		{
			packet_A.payload[i]=message.data[i];//copying data in message to packet payload
		}
		memset(message.data,'\0',sizeof(message.data));
		packet_A.acknum=0;
		packet_A.seqnum=next_seqnum;
		packet_A.checksum=packet_A.seqnum+packet_A.acknum;//initial checksum value
		for(int j=0;j<20;j++)
		{
			packet_A.checksum+=packet_A.payload[j];//computing checksum
		}
		tolayer3(0,packet_A);//sending packet to layer3
		//building win_buffer
		win_buffer_count+=1;
		win_buffer[win_buffer_count-1].seqnum=packet_A.seqnum;
		win_buffer[win_buffer_count-1].acknum=packet_A.acknum;
		win_buffer[win_buffer_count-1].checksum=packet_A.checksum;
		for(int a=0;a<20;a++)
		{
			win_buffer[win_buffer_count-1].payload[a]=packet_A.payload[a];
		}
		if(base==next_seqnum)
		{
			starttimer(0,100.0);//starting timer for 10 time units
			timer_A=1;
			no_val=1;//no value in the time array for base
		}
		if(no_val==0)
		{
			timer_buffer[time_buf_count].cur_time_val=get_sim_time();
			timer_buffer[time_buf_count].seqnum=packet_A.seqnum;
			cout<<"Time is "<<timer_buffer[time_buf_count].cur_time_val<<endl;
			time_buf_count+=1;
		}
		else
		{
			no_val=0;
		}
		next_seqnum+=1;
	}
	else
	{ //cout<<"buffered"<<endl;
		//buffer the msg
		buff_count+=1;//buff_count++
		buffer[buff_count-1].seqnum=next_seqnum+buff_count-1;//because two or more msgs can arrive when the window has not moved
		buffer[buff_count-1].acknum=0;
		buffer[buff_count-1].checksum=buffer[buff_count-1].seqnum+buffer[buff_count-1].acknum;
		for(int n=0;n<20;n++)
		{
			buffer[buff_count-1].checksum+=buffer[buff_count-1].payload[n];
		}
		for(int k=0;k<20;k++)
		{
			buffer[buff_count-1].payload[k]=message.data[k];//copying data in message to buffer
		}
		memset(message.data,'\0',sizeof(message.data));
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
		{//cout<<"checksum verified"<<endl;
			base=packet.acknum+1;
			if(base==next_seqnum)
			{
				stoptimer(0);//stopping timer when base==next_seqnum
				timer_A=0;
			}
			else
			{
				if(timer_A==1)
				{
					stoptimer(0);
					timer_A=0;
				}
				starttimer(0,100.0);
				timer_A=1;
			}
			//reordering buffer as cummulative ACK
			int a=0;
			for(int m=0;m<win_buffer_count;m++)
			{	if(win_buffer[m].seqnum<packet.acknum)
				{
					win_buffer[m].seqnum=0;
					win_buffer[m].acknum=0;
					win_buffer[m].checksum=0;
					memset(win_buffer[m].payload,'\0',sizeof(win_buffer[m].payload));
					a+=1;
				}
				else
				{
					win_buffer[m-a].seqnum=win_buffer[m].seqnum;
					win_buffer[m-a].acknum=win_buffer[m].acknum;
					win_buffer[m-a].checksum=win_buffer[m].checksum;				
					for(int d=0;d<20;d++)
					{
						win_buffer[m-a].payload[d]=win_buffer[m].payload[d];
					}
				}//cout<<"\nwin_buffer["<<m<<"].payload "<<win_buffer[m].payload<<"\n"<<endl;
			}
			win_buffer_count-=a;//buff_count--
			exp_acknum+=1;
			if(buff_count>0)
			{
				while(next_seqnum<base+win_size_l)
				{
					tolayer3(0,buffer[0]);//sending packet to layer3
					//building win_buffer
					win_buffer_count+=1;
					win_buffer[win_buffer_count-1].seqnum=buffer[0].seqnum;
					win_buffer[win_buffer_count-1].acknum=buffer[0].acknum;
					win_buffer[win_buffer_count-1].checksum=buffer[0].checksum;
					for(int k=0;k<20;k++)
					{
						win_buffer[win_buffer_count-1].payload[k]=buffer[0].payload[k];
					}
					//reordering buffer
					for(int m=0;m<buff_count-1;m++)
					{
						buffer[m].seqnum=buffer[m+1].seqnum;
						buffer[m].acknum=buffer[m+1].acknum;
						buffer[m].checksum=buffer[m+1].checksum;				
						for(int d=0;d<20;d++)
						{
							buffer[m].payload[d]=buffer[m+1].payload[d];
						}
					}
					buff_count-=1;
					next_seqnum+=1;
				}
			}
		}
	}
}

/* called when A's timer goes off */
void A_timerinterrupt()
{//cout<<"timeout!"<<endl;
	starttimer(0,100.0);
	for(int p=0;p<win_buffer_count;p++)
	{
		tolayer3(0,win_buffer[p]);
	}
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
	win_size_l=getwinsize();
	base=0;
	next_seqnum=0;
	buff_count=0;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
	char msg[20];
	int temp_checksum=0;
	memset(msg,'\0',sizeof(msg));
	ack_packet_B.seqnum=exp_seqnum;	
	memset(ack_packet_B.payload,'\0',sizeof(ack_packet_B.payload));//default value - not changed, used for checksum
	if(exp_seqnum==packet.seqnum)
	{//cout<<"exp_seqnum==packet.seqnum"<<endl;
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
			ack_packet_B.seqnum=exp_seqnum;//ack_packet_B.seqnum=0;
			ack_packet_B.acknum=exp_seqnum;
			ack_packet_B.checksum=ack_packet_B.acknum+ack_packet_B.seqnum;
			for(int k=0;k<20;k++)
			{
				ack_packet_B.checksum+=ack_packet_B.payload[k];//computing checksum
			}
			tolayer3(1,ack_packet_B);//sending ack_packet_B to layer3
			exp_seqnum+=1;
		}
		else//if corrupt
		{//send previous ack
			tolayer3(1,ack_packet_B);//Resending ack_packet_B to layer3
		}
	}
	else//if out of order
	{//send previous ack
		tolayer3(1,ack_packet_B);//Resending ack_packet_B to layer3
	}
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
	//initializing all data fields in the ack_packet_B struct
	ack_packet_B.seqnum=0;//default value
	ack_packet_B.acknum=0;
	ack_packet_B.checksum=0;
	memset(ack_packet_B.payload,'\0',sizeof(ack_packet_B.payload));//default value - not changed, used for checksum
	exp_seqnum=0;
}
