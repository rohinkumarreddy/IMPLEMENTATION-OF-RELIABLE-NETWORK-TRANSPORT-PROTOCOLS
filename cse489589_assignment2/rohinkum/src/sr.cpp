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
static struct pkt packet_A,ack_packet_B,win_buffer[100],buffer[1000],rcv_buffer[100];
struct tmp {double cur_time_out_val,start_time_val; int seqnum;};//definition
static struct tmp timer_buffer[1000];
static int buff_count=0,exp_seqnum=0,exp_acknum=0,base=0,next_seqnum=0,win_size_l=getwinsize(),win_buffer_count=0,timer_A=0,time_buff_count=0,no_val=0,rcv_base=0,rcv_buff_count=0,ackseqnum[1000],ackseq_count=0,timer_temp=0;
static float time_out=20.0;
/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
	int trp_seqnum=0;double trp_cur_time_out_val=0.0,trp_start_time_val=0.0,tmp_cur_time_out_val=0.0;
	//sorting time_buffer by current time out value
	if(time_buff_count>0)
	{
		for(int k=0;k<time_buff_count;k++)
		{
			for(int y=0;y<time_buff_count-1;y++)
			{
				if(timer_buffer[y].cur_time_out_val>timer_buffer[y+1].cur_time_out_val)
				{
					trp_seqnum=timer_buffer[y].seqnum;
					trp_cur_time_out_val=timer_buffer[y].cur_time_out_val;
					trp_start_time_val=timer_buffer[y].start_time_val;
					timer_buffer[y].seqnum=timer_buffer[y+1].seqnum;
					timer_buffer[y].start_time_val=timer_buffer[y+1].start_time_val;
					timer_buffer[y].cur_time_out_val=timer_buffer[y+1].cur_time_out_val;
					timer_buffer[y+1].seqnum=trp_seqnum;
					timer_buffer[y+1].start_time_val=trp_start_time_val;
					timer_buffer[y+1].cur_time_out_val=trp_cur_time_out_val;
				}
			}
		}
		//update time array remaining cur_time_out
		for(int p=0;p<time_buff_count;p++)
		{
			tmp_cur_time_out_val=get_sim_time();
			tmp_cur_time_out_val-=timer_buffer[p].start_time_val;//cur_time-start
			timer_buffer[p].cur_time_out_val=time_out-tmp_cur_time_out_val;//time_out-cur_time
			if(timer_buffer[p].cur_time_out_val==0)//most likely will not occur
			{cout<<"occured"<<endl;
				for(int b=0;b<win_buffer_count;b++)
				{
					if(win_buffer[b].seqnum==timer_buffer[p].seqnum)
					{
						tolayer3(0,win_buffer[p]);//resending for custom timeout
						timer_buffer[p].cur_time_out_val=time_out;
					}
				}
			}
		}
	}
	if(next_seqnum<base+win_size_l)//lies within the window
	{cout<<"lies within window"<<endl;
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
		if(timer_A==0)//no real timer is active
		{
			if(time_buff_count>0)//if there are any virtual timers
			{
				if(timer_buffer[0].cur_time_out_val<=time_out)
				{
					starttimer(0,timer_buffer[0].cur_time_out_val);//starting timer for the remaining time out(least)
					timer_temp=timer_buffer[0].seqnum;//storing the seqnum of the packet whose real timer is running
					timer_A=1;
					cout<<"started timer for timer_buffer[0]\n"<<"timer_A=1\n"<<"time buff count is "<<time_buff_count<<endl;
					no_val=0;//timer is not started for current packet, it's time values must be buffered
				}
			}
			else if(time_buff_count==0)
			{
				starttimer(0,time_out);//starting timer for 10 time units
				timer_A=1;
				cout<<"started timer for packet\n"<<"timer_A=1\n"<<"time buff count is "<<time_buff_count<<endl;
				no_val=1;//this packet's time values are not stored in time buffer
				timer_temp=packet_A.seqnum;//seqnum of packet whose real timer is running
			}
		}
		if(no_val==0)//timer is not started for the current packet
		{
			time_buff_count+=1;
			timer_buffer[time_buff_count-1].start_time_val=get_sim_time();
			timer_buffer[time_buff_count-1].cur_time_out_val=time_out;//starting time_out, will change along with events
			timer_buffer[time_buff_count-1].seqnum=packet_A.seqnum;
			cout<<"Time is "<<timer_buffer[time_buff_count-1].start_time_val<<"\n"<<"time buff count is "<<time_buff_count<<endl;
		}
		else
		{
			no_val=0;
		}
		next_seqnum+=1;
		//sorting time_buffer by current time out value
		if(time_buff_count>0)
		{
			for(int k=0;k<time_buff_count;k++)
			{
				for(int y=0;y<time_buff_count-1;y++)
				{
					if(timer_buffer[y].cur_time_out_val>timer_buffer[y+1].cur_time_out_val)
					{
						trp_seqnum=timer_buffer[y].seqnum;
						trp_cur_time_out_val=timer_buffer[y].cur_time_out_val;
						trp_start_time_val=timer_buffer[y].start_time_val;
						timer_buffer[y].seqnum=timer_buffer[y+1].seqnum;
						timer_buffer[y].start_time_val=timer_buffer[y+1].start_time_val;
						timer_buffer[y].cur_time_out_val=timer_buffer[y+1].cur_time_out_val;
						timer_buffer[y+1].seqnum=trp_seqnum;
						timer_buffer[y+1].start_time_val=trp_start_time_val;
						timer_buffer[y+1].cur_time_out_val=trp_cur_time_out_val;
					}
				}
			}
		}

	}
	else
	{ cout<<"msg buffered not within window"<<endl;
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
	for(int u=0;u<win_buffer_count;u++)
	{
		if(packet.seqnum==win_buffer[u].seqnum)
		{
			cout<<"recv ack: "<<win_buffer[u].payload[0]<<win_buffer[u].payload[1]<<win_buffer[u].payload[2]<<endl;
		}
	}
	int temp_ack_checksum=0,tmp_time=0,tmpa=0,tm_sq=0,tm_ackseqnum=0,ab=0,hg=0;
	int trp_seqnum=0;double trp_cur_time_out_val=0.0,trp_start_time_val=0.0,tmp_cur_time_out_val=0.0;
	//sorting time_buffer by current time out value
	if(time_buff_count>0)
	{
		for(int k=0;k<time_buff_count;k++)
		{
			for(int y=0;y<time_buff_count-1;y++)
			{
				if(timer_buffer[y].cur_time_out_val>timer_buffer[y+1].cur_time_out_val)
				{
					trp_seqnum=timer_buffer[y].seqnum;
					trp_cur_time_out_val=timer_buffer[y].cur_time_out_val;
					trp_start_time_val=timer_buffer[y].start_time_val;
					timer_buffer[y].seqnum=timer_buffer[y+1].seqnum;
					timer_buffer[y].start_time_val=timer_buffer[y+1].start_time_val;
					timer_buffer[y].cur_time_out_val=timer_buffer[y+1].cur_time_out_val;
					timer_buffer[y+1].seqnum=trp_seqnum;
					timer_buffer[y+1].start_time_val=trp_start_time_val;
					timer_buffer[y+1].cur_time_out_val=trp_cur_time_out_val;
				}
			}
		}
		//update time array remaining cur_time_out
		for(int p=0;p<time_buff_count;p++)
		{
			tmp_cur_time_out_val=get_sim_time();
			tmp_cur_time_out_val-=timer_buffer[p].start_time_val;//cur_time-start
			timer_buffer[p].cur_time_out_val=time_out-tmp_cur_time_out_val;//time_out-cur_time
			if(timer_buffer[p].cur_time_out_val==0)//most likely will not occur
			{cout<<"occured"<<endl;
				for(int b=0;b<win_buffer_count;b++)
				{
					if(win_buffer[b].seqnum==timer_buffer[p].seqnum)
					{
						tolayer3(0,win_buffer[p]);//resending for custom timeout
						timer_buffer[p].cur_time_out_val=time_out;
					}
				}
			}
		}
	}
	if((packet.acknum>=base)&&(packet.acknum<base+win_size_l))//ack lies with in window
	{cout<<"ack lies within window"<<endl;
		for(int g=0;g<win_buffer_count;g++)
		{
			if(win_buffer[g].seqnum==packet.acknum)
			{
				cout<<"ack for: "<<win_buffer[g].payload[0]<<win_buffer[g].payload[1]<<win_buffer[g].payload[2]<<endl;
			}
		}
		temp_ack_checksum=packet.acknum+packet.seqnum;//initial checksum value
		for(int i=0;i<20;i++)
		{
			temp_ack_checksum+=packet.payload[i];//computing ack_checksum
		}
		if(temp_ack_checksum==packet.checksum)//not corrupt
		{cout<<"checksum verified"<<endl;
			if(base==packet.acknum)
			{cout<<"base==ack"<<endl;
				base+=1;cout<<"ack deliverd base increased"<<endl;
				//for the prev buffered acks
				if(ackseq_count>0)
				{
					tm_sq=base;
					for(int j=0;j<ackseq_count;j++)
					{
						if(tm_sq==ackseqnum[j])//until discontinuity
						{
							tm_sq+=1;
							base+=1;cout<<"buffered acks deliverd"<<endl;
						}
					}
					//reordering ackseqnum
					int b=0;
					for(int l=0;l<ackseq_count;l++)
					{
						if(ackseqnum[l]<base)
						{
							ackseqnum[l]=0;
							b+=1;
						}
						else
						{
							ackseqnum[l-b]=ackseqnum[l];
						}//cout<<"\nwin_buffer["<<m<<"].payload "<<win_buffer[m].payload<<"\n"<<endl;
					}
					ackseq_count-=b;//buff_count--
				}
			}
			else//base!=acknum
			{
				if(ackseq_count>0)
				{
					//sorting ackseqnum
					for(int s=0;s<ackseq_count;s++)
					{
						for(int d=0;d<ackseq_count-1;d++)
						{
							if(ackseqnum[d]>ackseqnum[d+1])
							{
								tm_ackseqnum=ackseqnum[d];
								ackseqnum[d]=ackseqnum[d+1];//order sort
								ackseqnum[d+1]=tm_ackseqnum;
							}
						}
					}
				}
				//buffering the ack
				for(int g=0;g<ackseq_count;g++)
				{
					if(ackseqnum[g]==packet.acknum)
					{
						hg=1;//duplicate ack
					}
				}
				if(hg==0)
				{
					ackseq_count+=1;
					ackseqnum[ackseq_count-1]=packet.acknum;
					cout<<"ack is buffered"<<endl;
				}else{cout<<"duplicate ack"<<endl;}
			}
			if(timer_temp==packet.seqnum)//this is the packet for which the timer was started
			{cout<<"timer stopped as timer temp=recv ack"<<endl;
				stoptimer(0);//stopping timer when base==next_seqnum
				timer_A=0;cout<<"timer_A=0"<<endl;
				if(time_buff_count>0)
				{
					//if the received ack is also in timer_buffer-remove it from buffer
					for(int r=0;r<time_buff_count;r++)
					{
						if(packet.seqnum==timer_buffer[r].seqnum)
						{
							timer_buffer[r].cur_time_out_val=999999;ab=1;
						}
					}
					trp_seqnum=0;trp_cur_time_out_val=0.0;trp_start_time_val=0.0;tmp_cur_time_out_val=0.0;
					//sorting time_buffer by current time out value
					for(int k=0;k<time_buff_count;k++)
					{
						for(int y=0;y<time_buff_count-1;y++)
						{
							if(timer_buffer[y].cur_time_out_val>timer_buffer[y+1].cur_time_out_val)
							{
								trp_seqnum=timer_buffer[y].seqnum;
								trp_cur_time_out_val=timer_buffer[y].cur_time_out_val;
								trp_start_time_val=timer_buffer[y].start_time_val;
								timer_buffer[y].seqnum=timer_buffer[y+1].seqnum;
								timer_buffer[y].start_time_val=timer_buffer[y+1].start_time_val;
								timer_buffer[y].cur_time_out_val=timer_buffer[y+1].cur_time_out_val;
								timer_buffer[y+1].seqnum=trp_seqnum;
								timer_buffer[y+1].start_time_val=trp_start_time_val;
								timer_buffer[y+1].cur_time_out_val=trp_cur_time_out_val;
							}
						}
					}
					if(ab==1)
					{
						time_buff_count-=1;//removing the last value
						cout<<"ack removed from time buffer\n"<<"time buff count is "<<time_buff_count<<endl;
					}
				}
				//starting timer again for next value
				if(time_buff_count>0)
				{cout<<"buffered timers available starting timer with least time out"<<endl;
					starttimer(0,timer_buffer[0].cur_time_out_val);//starting timer with the least current time out
					timer_A=1;
					timer_temp=timer_buffer[0].seqnum;
					//removing this from timer buffer
					for(int i=0;i<time_buff_count-1;i++)
					{
						timer_buffer[i].cur_time_out_val=timer_buffer[i+1].cur_time_out_val;
						timer_buffer[i].seqnum=timer_buffer[i+1].seqnum;
						timer_buffer[i].start_time_val=timer_buffer[i+1].start_time_val;
					}
					time_buff_count-=1;cout<<"timer removed\n"<<"time buff count is "<<time_buff_count<<endl;
				}
			}
			else
			{//the timer is not running for the received ack
				if(time_buff_count>0)
				{cout<<"timer is still running the recv ack has buffered timer"<<endl;
					ab=0;
					//removing the packet for which the ack is received from time_buffer
					for(int i=0;i<time_buff_count;i++)
					{
						if(packet.seqnum==timer_buffer[i].seqnum)
						{
							timer_buffer[i].cur_time_out_val=99999;ab=1;
						}
					}
					//sorting time_buffer by current time out value - to remove the packet fo which ack is received
					for(int k=0;k<time_buff_count;k++)
					{
						for(int y=0;y<time_buff_count-1;y++)
						{
							if(timer_buffer[y].cur_time_out_val>timer_buffer[y+1].cur_time_out_val)
							{
								trp_seqnum=timer_buffer[y].seqnum;
								trp_cur_time_out_val=timer_buffer[y].cur_time_out_val;
								trp_start_time_val=timer_buffer[y].start_time_val;
								timer_buffer[y].seqnum=timer_buffer[y+1].seqnum;
								timer_buffer[y].start_time_val=timer_buffer[y+1].start_time_val;
								timer_buffer[y].cur_time_out_val=timer_buffer[y+1].cur_time_out_val;
								timer_buffer[y+1].seqnum=trp_seqnum;
								timer_buffer[y+1].start_time_val=trp_start_time_val;
								timer_buffer[y+1].cur_time_out_val=trp_cur_time_out_val;
							}
						}
					}
					if(ab==1)
					{
						time_buff_count-=1;
						cout<<"timer removed from buffer\n"<<"time buff count is "<<time_buff_count<<endl;
					}
				}
			}
			//removing the received ack-packet from window buffer as this is a selective ACK
			int a=0;
			for(int m=0;m<win_buffer_count;m++)
			{	if(win_buffer[m].seqnum==packet.seqnum)//received packet
				{
					win_buffer[m].seqnum=0;
					win_buffer[m].acknum=0;
					win_buffer[m].checksum=0;
					memset(win_buffer[m].payload,'\0',sizeof(win_buffer[m].payload));
					a=1;
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
				if(timer_A==0)//no real timer is active
				{
					if(time_buff_count>0)//if there are any virtual timers
					{
						if(timer_buffer[0].cur_time_out_val<=time_out)
						{
							starttimer(0,timer_buffer[0].cur_time_out_val);//starting timer for the remaining time out(least)
							timer_temp=timer_buffer[0].seqnum;//storing the seqnum of the packet whose real timer is running	
							timer_A=1;cout<<"timer started for buffer_timer[0]\n"<<"timer_A=1\n"<<"time buff count is "<<time_buff_count<<endl;
						}
					}
				}
				while(next_seqnum<base+win_size_l)
				{
					tolayer3(0,buffer[0]);//sending packet to layer3
					if(timer_A==0)//no real timer is active
					{
						if(time_buff_count==0)
						{
							starttimer(0,time_out);//starting timer for 10 time units
							timer_A=1;cout<<"started timer for bufferd msg\n"<<"timer_A=1"<<endl;
							timer_temp=buffer[0].seqnum;//seqnum of packet whose real timer is running
						}
					}
					else if(timer_A==1)//timer is not started for the current packet
					{
						time_buff_count+=1;cout<<"buffered timer for bufferd msg\n"<<endl;
						timer_buffer[time_buff_count-1].start_time_val=get_sim_time();
						timer_buffer[time_buff_count-1].cur_time_out_val=time_out;//starting time_out, will change along with events
						timer_buffer[time_buff_count-1].seqnum=packet_A.seqnum;
						cout<<"Time is "<<timer_buffer[time_buff_count-1].start_time_val<<"\n"<<"time buff count is "<<time_buff_count<<endl;
					}
					//building win_buffer
					win_buffer_count+=1;
					win_buffer[win_buffer_count-1].seqnum=buffer[0].seqnum;
					win_buffer[win_buffer_count-1].acknum=buffer[0].acknum;
					win_buffer[win_buffer_count-1].checksum=buffer[0].checksum;
					for(int k=0;k<20;k++)
					{
						win_buffer[win_buffer_count-1].payload[k]=buffer[0].payload[k];
					}
					//removing sent packet from buffer
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
	trp_seqnum=0;trp_cur_time_out_val=0.0;trp_start_time_val=0.0;tmp_cur_time_out_val=0.0;
	//sorting time_buffer by current time out value
	if(time_buff_count>0)
	{
		for(int k=0;k<time_buff_count;k++)
		{
			for(int y=0;y<time_buff_count-1;y++)
			{
				if(timer_buffer[y].cur_time_out_val>timer_buffer[y+1].cur_time_out_val)
				{
					trp_seqnum=timer_buffer[y].seqnum;
					trp_cur_time_out_val=timer_buffer[y].cur_time_out_val;
					trp_start_time_val=timer_buffer[y].start_time_val;
					timer_buffer[y].seqnum=timer_buffer[y+1].seqnum;
					timer_buffer[y].start_time_val=timer_buffer[y+1].start_time_val;
					timer_buffer[y].cur_time_out_val=timer_buffer[y+1].cur_time_out_val;
					timer_buffer[y+1].seqnum=trp_seqnum;
					timer_buffer[y+1].start_time_val=trp_start_time_val;
					timer_buffer[y+1].cur_time_out_val=trp_cur_time_out_val;
				}
			}
		}
	}
}

/* called when A's timer goes off */
void A_timerinterrupt()
{cout<<"timeout!"<<endl;
	for(int u=0;u<win_buffer_count;u++)
	{
		if(win_buffer[u].seqnum==timer_temp)
		{
			cout<<"for: "<<win_buffer[u].payload[0]<<win_buffer[u].payload[1]<<win_buffer[u].payload[2]<<endl;
		}
	}
	timer_A=0;cout<<"timer_A=0"<<endl;
	int trp_seqnum=0,ind=0,snt_seqnum=0;double trp_cur_time_out_val=0.0,trp_start_time_val=0.0,tmp_cur_time_out_val=0.0;
	//sorting time_buffer by current time out value
	if(time_buff_count>0)
	{
		for(int k=0;k<time_buff_count;k++)
		{
			for(int y=0;y<time_buff_count-1;y++)
			{
				if(timer_buffer[y].cur_time_out_val>timer_buffer[y+1].cur_time_out_val)
				{
					trp_seqnum=timer_buffer[y].seqnum;
					trp_cur_time_out_val=timer_buffer[y].cur_time_out_val;
					trp_start_time_val=timer_buffer[y].start_time_val;
					timer_buffer[y].seqnum=timer_buffer[y+1].seqnum;
					timer_buffer[y].start_time_val=timer_buffer[y+1].start_time_val;
					timer_buffer[y].cur_time_out_val=timer_buffer[y+1].cur_time_out_val;
					timer_buffer[y+1].seqnum=trp_seqnum;
					timer_buffer[y+1].start_time_val=trp_start_time_val;
					timer_buffer[y+1].cur_time_out_val=trp_cur_time_out_val;
				}
			}
		}
		//update time array remaining cur_time_out
		for(int p=0;p<time_buff_count;p++)
		{
			tmp_cur_time_out_val=get_sim_time();
			tmp_cur_time_out_val-=timer_buffer[p].start_time_val;//cur_time-start
			timer_buffer[p].cur_time_out_val=time_out-tmp_cur_time_out_val;//time_out-cur_time
			if(timer_buffer[p].cur_time_out_val<1)//most likely will not occur
			{cout<<"occured"<<endl;
				for(int b=0;b<win_buffer_count;b++)
				{
					if(win_buffer[b].seqnum==timer_buffer[p].seqnum)
					{
						tolayer3(0,win_buffer[p]);//resending for custom timeout
						timer_buffer[p].cur_time_out_val=time_out;
					}
				}
			}
		}
	}
	for(int p=0;p<win_buffer_count;p++)
	{
		if(win_buffer[p].seqnum==timer_temp)//selecting the sent packet for which timer was previously started
		{
			tolayer3(0,win_buffer[p]);
			snt_seqnum=win_buffer[p].seqnum;//storing sent
		}
	}
	if(timer_A==0)//Always true
	{
		if(time_buff_count>0)//if there are buffered virtual timers
		{
			if(timer_buffer[0].cur_time_out_val<=time_out)
			{
				starttimer(0,timer_buffer[0].cur_time_out_val);//starting timer with the least current timeout value buffered
				timer_A=1;ind=1;
				cout<<"timer started for least buffered timer\n"<<"timer_A=1\n"<<"time buff count is "<<time_buff_count<<endl;
				timer_temp=packet_A.seqnum;//seqnum of packet whose real timer is running	
			}
			//removing the buffered timer
			timer_buffer[0].cur_time_out_val=99999;
			trp_seqnum=0;trp_cur_time_out_val=0.0;trp_start_time_val=0.0;tmp_cur_time_out_val=0.0;
			//sorting time_buffer by current time out value
			for(int k=0;k<time_buff_count;k++)
			{
				for(int y=0;y<time_buff_count-1;y++)
				{
					if(timer_buffer[y].cur_time_out_val>timer_buffer[y+1].cur_time_out_val)
					{
						trp_seqnum=timer_buffer[y].seqnum;
						trp_cur_time_out_val=timer_buffer[y].cur_time_out_val;
						trp_start_time_val=timer_buffer[y].start_time_val;
						timer_buffer[y].seqnum=timer_buffer[y+1].seqnum;
						timer_buffer[y].start_time_val=timer_buffer[y+1].start_time_val;
						timer_buffer[y].cur_time_out_val=timer_buffer[y+1].cur_time_out_val;
						timer_buffer[y+1].seqnum=trp_seqnum;
						timer_buffer[y+1].start_time_val=trp_start_time_val;
						timer_buffer[y+1].cur_time_out_val=trp_cur_time_out_val;
					}
				}
			}
			time_buff_count-=1;cout<<"timer removed from buffer\n"<<"time buff count is "<<time_buff_count<<endl;
		}
		else if(time_buff_count==0)
		{cout<<"no buffered timers\n"<<"starting timer for interrupt packet\n"<<"timer_A=1"<<endl;
			starttimer(0,time_out);//restarting timer
			timer_A=1;
		}
		if(ind==1)//timer is not started for this packet
		{
			time_buff_count+=1;
			timer_buffer[time_buff_count-1].start_time_val=get_sim_time();
			timer_buffer[time_buff_count-1].cur_time_out_val=time_out;//starting time_out, will change along with events
			timer_buffer[time_buff_count-1].seqnum=snt_seqnum;
			cout<<"Time is "<<timer_buffer[time_buff_count-1].start_time_val<<"\n"<<"time buff count is "<<time_buff_count<<endl;
		}
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
	char msg[20],tmp_msg[20];
	int temp_checksum=0,tmp_num=0,disc=0,tmp_seqnum=0,tmp_acknum=0,tmp_checksum=0,tmp_count=0,duplicate=0;
	memset(msg,'\0',sizeof(msg));
	ack_packet_B.seqnum=exp_seqnum;	
	memset(ack_packet_B.payload,'\0',sizeof(ack_packet_B.payload));//default value - not changed, used for checksum
	if((packet.seqnum>=rcv_base)&&(packet.seqnum<rcv_base+win_size_l))//within receiving window
	{cout<<"lies within window"<<endl;
		for(int g=0;g<win_buffer_count;g++)
		{
			if(win_buffer[g].seqnum==packet.seqnum)
			{
				cout<<"recv'd: "<<win_buffer[g].payload[0]<<win_buffer[g].payload[1]<<win_buffer[g].payload[2]<<endl;
			}
		}
		temp_checksum=packet.seqnum+packet.acknum;//initial value of checksum	
		for(int i=0;i<20;i++)
		{
			temp_checksum+=packet.payload[i];//computing checksum
		}
		if(temp_checksum==packet.checksum)//not corrupt
		{cout<<"not corrupt"<<endl;
			if(rcv_base==packet.seqnum)
			{cout<<"rcv_base==packet.seqnum"<<endl;
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
				rcv_base+=1;
				if(rcv_buff_count>0)
				{
					tmp_num=rcv_base;//tmp_num=rcv_buffer[0].seqnum;
					for(int j=0;j<rcv_buff_count;j++)
					{
						if(tmp_num!=rcv_buffer[j].seqnum)//discovering discontinuity
						{
							disc=1;
						}
						if(disc==0)
						{
							tmp_num+=1;
							tmp_count+=1;
						}
					}
					for(int a=0;a<tmp_count;a++)//sending all packets from base to discontinuity
					{
						memset(msg,'\0',sizeof(msg));
						for(int j=0;j<20;j++)
						{
							msg[j]=rcv_buffer[a].payload[j];//copying payload from packet to msg
						}cout<<"sending buffered msgs to layer5 "<<endl;
						tolayer5(1,msg);//sending message to layer5
						//building ack_packet_B
						ack_packet_B.seqnum=rcv_buffer[a].seqnum;//ack_packet_B.seqnum=0;
						ack_packet_B.acknum=rcv_buffer[a].seqnum;
						ack_packet_B.checksum=ack_packet_B.acknum+ack_packet_B.seqnum;
						for(int k=0;k<20;k++)
						{
							ack_packet_B.checksum+=ack_packet_B.payload[k];//computing checksum
						}
						tolayer3(1,ack_packet_B);//sending ack_packet_B to layer3
						exp_seqnum+=1;
						rcv_base+=1;
					}//end of buffer packet until discontinuity sent to layer5 and acknowledgements to layer3
					for(int a=0;a<tmp_count;a++)//removing packets in buffer from base to discontinuity
					{
						for(int j=0;j<20;j++)
						{
							rcv_buffer[a].payload[j]=rcv_buffer[tmp_count-a].payload[j];//payload[a]=payload[tco-a]
						}
						rcv_buffer[a].seqnum=rcv_buffer[tmp_count-a].seqnum;//rcv seqnum[a]=rcv seqnum[tco-a]
						rcv_buffer[a].acknum=rcv_buffer[tmp_count-a].acknum;//rcv acknum[a]=rcv acknum[tco-a]
						rcv_buffer[a].checksum=rcv_buffer[tmp_count-a].checksum;//rcv checksum[a]=rcv checksum[tco-a]
					}
					rcv_buff_count-=tmp_count;cout<<"sent buffered msgs removed from rcv buffer"<<endl;
				}//end of rcv_buff_count>0
			}//end of rcv_base==packet.seqnum
			else
			{//still within the rcv window but not equal to base
				cout<<"not = base but within window"<<endl;
				for(int d=0;d<rcv_buff_count;d++)
				{
					if(packet.seqnum==rcv_buffer[d].seqnum)
					{
						duplicate=1;cout<<"duplicate"<<endl;
					}
				}
				if(duplicate==0)//if not a duplicate packet
				{cout<<"not a duplicate"<<endl;
					//buffer the msg in rcv_buffer
					rcv_buff_count+=1;//buff_count++
					rcv_buffer[rcv_buff_count-1].seqnum=packet.seqnum;
					rcv_buffer[rcv_buff_count-1].acknum=packet.seqnum;
					rcv_buffer[rcv_buff_count-1].checksum=packet.checksum;
					for(int k=0;k<20;k++)
					{
						rcv_buffer[rcv_buff_count-1].payload[k]=packet.payload[k];//copying data in message to buffer
					}cout<<"msg rcv buffered"<<endl;
					//sorting buffer based on seqnum
					for(int p=0;p<rcv_buff_count;p++)
					{
						for(int q=0;q<rcv_buff_count-1;q++)
						{
							if(rcv_buffer[q].seqnum>rcv_buffer[q+1].seqnum)
							{
								tmp_seqnum=rcv_buffer[q].seqnum;
								tmp_acknum=rcv_buffer[q].acknum;
								tmp_checksum=rcv_buffer[q].checksum;
								for(int l=0;l<20;l++)
								{
									tmp_msg[l]=rcv_buffer[q].payload[l];
								}
								rcv_buffer[q].seqnum=rcv_buffer[q+1].seqnum;
								rcv_buffer[q].acknum=rcv_buffer[q+1].acknum;
								rcv_buffer[q].checksum=rcv_buffer[q+1].checksum;
								for(int m=0;m<20;m++)
								{
									rcv_buffer[q].payload[m]=rcv_buffer[q+1].payload[m];
								}
								rcv_buffer[q+1].seqnum=tmp_seqnum;
								rcv_buffer[q+1].acknum=tmp_acknum;
								rcv_buffer[q+1].checksum=tmp_checksum;
								for(int m=0;m<20;m++)
								{
									rcv_buffer[q+1].payload[m]=tmp_msg[m];
								}
							}
						}
					}//end of reordering buffer
				}//end of duplicate - ignore duplicate
				duplicate=0;//reinitialising
			}//end of not equal to base
		}//end of not corrupt - ignore corrupt
	}//end of within window
	else if((packet.seqnum>=rcv_base-win_size_l)&&(packet.seqnum<rcv_base-1))//lies in the previous window
	{cout<<"packet lies within prev window: "<<packet.payload[0]<<packet.payload[1]<<packet.payload[2]<<endl;
		temp_checksum=packet.seqnum+packet.acknum;//initial value of checksum	
		for(int i=0;i<20;i++)
		{
			temp_checksum+=packet.payload[i];//computing checksum
		}
		if(temp_checksum==packet.checksum)//not corrupt
		{
			//building ack_packet_B
			ack_packet_B.seqnum=packet.seqnum;//ack_packet_B.seqnum=0;
			ack_packet_B.acknum=packet.seqnum;
			ack_packet_B.checksum=ack_packet_B.acknum+ack_packet_B.seqnum;
			for(int k=0;k<20;k++)
			{
				ack_packet_B.checksum+=ack_packet_B.payload[k];//computing checksum
			}
			tolayer3(1,ack_packet_B);//sending ack_packet_B to layer3
		cout<<"ack sent"<<endl;
		}//no else for now
	}//end of within previous window - out of now and prev window ignore
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
