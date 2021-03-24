#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h> 
//-------------------------------------------------------------------------
//			  enums
//-------------------------------------------------------------------------
typedef enum{FAILURE,SUCCESS} status_code;
typedef enum{L,M,U,S} berth_code;
typedef enum{SL=1,AC} coach_type;
#define HASH_SIZE 199
//-------------------------------------------------------------------------
//				nodes
//-------------------------------------------------------------------------
typedef struct coach_node_tag
{
	int coachId;
	coach_type class;
	struct sm_tag *seatsArr[4];
	int avalSeats;
	struct coach_node_tag *prev;
	struct coach_node_tag *next;
}coach_node;
typedef struct pass_node_tag
{
	int passId;
	char address[40];
	int phoneNo;
	char name[40];
	int age;
	berth_code berthPref;
	struct pass_node_tag *prev;
	struct pass_node_tag *next;
	int ticketNo;
	int trainId;
	int AllocCoachId;
	coach_node *AllocCoach_ptr;
	berth_code AllocBerthType;
	int AllocBerthNo;
}pass_node;
typedef struct sm_tag
{
	struct pass_node_tag *pass_data;
	int colNo;
	struct sm_tag *next;
	
}sm_node;
typedef struct train_node_tag
{
	int trainId;
	struct coach_node_tag *coachList;
	struct queue_node_tag *passList;
	struct queue_node_tag *RAC;
	struct queue_node_tag *WL;
	struct train_node_tag *next;
}train_node;
typedef struct hash_node_tag
{
	pass_node *pass_data;
	struct hash_node_tag *next;
}hash_node;
typedef struct queue_node_tag
{
	pass_node *front;
	pass_node *rear;
	int count;
}queue_node;
//-------------------------------------------------------------------------
//				hash table
//-------------------------------------------------------------------------
int isEmpty(queue_node *qptr)
{
	int retval=0;
	if(qptr->rear==NULL && qptr->front==NULL)
	{
		retval=1;
	}
	return retval;
}
status_code enqueueRAC(train_node *tptr,pass_node *pass_ptr)
{
	queue_node *qptr;
	qptr=tptr->RAC;
	status_code sc=SUCCESS;
	if(qptr->count==0)
	{
		sc=FAILURE;
	}
	else
	{
		if(isEmpty(qptr))
		{
			qptr->front=pass_ptr;
			qptr->rear=pass_ptr;
			pass_ptr->next=NULL;
			pass_ptr->prev=NULL;
		}
		else{
			(qptr->rear)->next=pass_ptr;
			pass_ptr->prev=qptr->rear;
			qptr->rear=pass_ptr;
			pass_ptr->next=NULL;
		}
		(qptr->count)--;
	}
	return sc;
}
pass_node *dequeueRAC(train_node *tptr)
{
	queue_node *qptr;
	pass_node *retpass=NULL;
	qptr=tptr->RAC;
	if(!(isEmpty(qptr)))
	{
		retpass=qptr->front;
		qptr->front=(qptr->front)->next;
		if(qptr->front==NULL)
		{
			qptr->rear=NULL;
		}
		else{
			qptr->front->prev=NULL;
		}
		retpass->next=NULL;
	}
	return retpass;
}
pass_node *dequeue(queue_node *qptr)
{
	pass_node *retpass=NULL;
	if(!(isEmpty(qptr)))
	{
		retpass=qptr->front;
		qptr->front=(qptr->front)->next;
		if(qptr->front==NULL)
		{
			qptr->rear=NULL;
		}
		else{
			qptr->front->prev=NULL;
		}
		retpass->next=NULL;
	}
	return retpass;
	
}
status_code enqueue(queue_node *qptr,pass_node *pass_ptr)
{
	status_code sc=SUCCESS;
		if(isEmpty(qptr))
		{
			qptr->front=pass_ptr;
			qptr->rear=pass_ptr;
			pass_ptr->next=NULL;
			pass_ptr->prev=NULL;
		}
		else{
			(qptr->rear)->next=pass_ptr;
			pass_ptr->prev=qptr->rear;
			qptr->rear=pass_ptr;
			pass_ptr->next=NULL;
		}
	return sc;
}
//-------------------------------------------------------------------------
//				hash table
//-------------------------------------------------------------------------
hash_node *HASH_TABLE[HASH_SIZE];
void initialize_hash()
{
	for(int i=0;i<HASH_SIZE;i++)
	{
		HASH_TABLE[i]=NULL;
	}
}
void print_hash_data()
{
	hash_node *ptr;
	for(int i=0;i<HASH_SIZE;i++)
	{
		if(HASH_TABLE[i]!=NULL)
		{
			printf("%d\t",i);
			ptr=HASH_TABLE[i];
			while(ptr)
			{
				printf("%s\n",ptr->pass_data->name);
				ptr=ptr->next;
			}
		}
	}
}
int get_hash_index(int id)
{
	int retval;
	retval=id%HASH_SIZE;
	return retval;
}
void insert_hash_node(int i,hash_node *nptr)
{
	hash_node *ptr;
	ptr=HASH_TABLE[i];
	if(HASH_TABLE[i]==NULL)
	{
		HASH_TABLE[i]=nptr;
	}	
	else
	{
		while(ptr->next!=NULL)
		{
			ptr=ptr->next;
		}
		ptr->next=nptr;
	}
}
void insert_hash(pass_node *pass_ptr)
{
	int index;
	hash_node *hash_ptr;
	hash_ptr=(hash_node *)malloc(sizeof(hash_node));
	hash_ptr->pass_data=pass_ptr;
	hash_ptr->next=NULL;
	index=get_hash_index(pass_ptr->ticketNo);
	insert_hash_node(index,hash_ptr);
}
pass_node *delete_hash_node(int i,int ticketNo)
{
	hash_node *ptr,*next_ptr;
	pass_node *retptr=NULL;
	ptr=HASH_TABLE[i];
	if(ptr==NULL)
	{
		retptr=NULL;
	}
	else if((ptr->pass_data)->ticketNo==ticketNo)
	{
		retptr=ptr->pass_data;
		ptr=ptr->next;
		HASH_TABLE[i]=ptr;
	}
	else{
		while(ptr->next!=NULL)
		{
			if(ptr->next->pass_data->ticketNo==ticketNo)
			{
				next_ptr=ptr->next;
				retptr=next_ptr->pass_data;
				ptr->next=next_ptr->next;
			}
			else{
				ptr=ptr->next;
			}
		}
	}
	return retptr;
}
pass_node *delete_hash(int ticketNo)
{
	int index;
	pass_node *retptr;
	index=get_hash_index(ticketNo);
	retptr=delete_hash_node(index,ticketNo);
	return retptr;
}
//-------------------------------------------------------------------------
//			 functions
//-------------------------------------------------------------------------
void initialize_train(train_node **train_pptr)
{
	*train_pptr=NULL;
}
void initialize_coach(coach_node **coach_pptr)
{
	*coach_pptr=NULL;
}
void initialize_pass(pass_node **pass_pptr)
{
	*pass_pptr=NULL;
}
coach_node *makeCoach_init(int id)
{
	coach_node *nptr;
	nptr=(coach_node *)malloc(sizeof(coach_node));
	
	nptr->coachId=100+id;
	
	for(int i=0;i<4;i++)
	{
			nptr->seatsArr[i]=NULL;
	}
	
	nptr->class=SL;
	
	nptr->avalSeats=40;
	nptr->next=NULL;
	nptr->prev=NULL;
	
	return nptr;
}
coach_node *makeCoach(int id)
{
	coach_node *nptr;
	nptr=(coach_node *)malloc(sizeof(coach_node));
	
	nptr->coachId=100+id;
	
	for(int i=0;i<4;i++)
	{
			nptr->seatsArr[i]=NULL;
	}
	
	printf("enter coach class\n1.Sleeper class\t2.AC class\n");
	int cls;
	scanf("%d",&cls);
	if(cls==2)
	{
		nptr->class=AC;
	}
	else{
		nptr->class=SL;
	}
	
	nptr->avalSeats=40;
	nptr->next=NULL;
	nptr->prev=NULL;
	
	return nptr;
}
coach_node *insertAtEnd_coach(coach_node *list_ptr,coach_node *nptr)
{
	coach_node *ptr;
	ptr=list_ptr;
	if(list_ptr==NULL)
	{
		list_ptr=nptr;
	}	
	else
	{
		while(ptr->next!=NULL)
		{
			ptr=ptr->next;
		}
		ptr->next=nptr;
		nptr->prev=ptr;
		nptr->next=NULL;
	}
	return list_ptr;
}
coach_node *createCoachList(coach_node *cptr,int num)
{
	coach_node *nptr;
	int i;
	for(i=1;i<=num;i++)
	{
		nptr=makeCoach_init(i);
		cptr=insertAtEnd_coach(cptr,nptr);
	}
	return cptr;
}
train_node *makeTrain(int id)
{
	int n;
	train_node *nptr;
	nptr=(train_node *)malloc(sizeof(train_node));
	nptr->RAC=(queue_node *)malloc(sizeof(queue_node));
	nptr->WL=(queue_node *)malloc(sizeof(queue_node));
	nptr->passList=(queue_node *)malloc(sizeof(queue_node));
	nptr->trainId=id;
	nptr->next=NULL;
	nptr->passList->count=0;
	nptr->passList->front=NULL;
	nptr->passList->rear=NULL;
	nptr->WL->count=0;
	nptr->WL->front=NULL;
	nptr->WL->rear=NULL;
	nptr->RAC->count=10;
	nptr->RAC->front=NULL;
	nptr->RAC->rear=NULL;
	coach_node *coach_ptr;
	initialize_coach(&coach_ptr);
	coach_ptr=createCoachList(coach_ptr,0);//no of coach per train
	nptr->coachList=coach_ptr;
	return nptr;
}
train_node *insertAtEnd_train(train_node *list_ptr,train_node *nptr)
{
	train_node *ptr;
	ptr=list_ptr;
	if(list_ptr==NULL)
	{
		list_ptr=nptr;
	}	
	else
	{
		while(ptr->next!=NULL)
		{
			ptr=ptr->next;
		}
		ptr->next=nptr;
		nptr->next=NULL;
	}
	return list_ptr;
}
train_node *createTrainList(train_node *tptr,int num)
{
	int i;
	train_node *nptr;
	for(i=1;i<=num;i++)
	{
		nptr=makeTrain(100+i);
		tptr=insertAtEnd_train(tptr,nptr);
	}
	return tptr;
}
void addCoach(train_node *trainList_ptr,int id)
{

	int found=0;
	train_node *nptr;
	nptr=trainList_ptr;
	while(nptr!=NULL && (nptr->trainId)!=id)
	{
		nptr=nptr->next;
	}
	if(nptr==NULL)
	{
		printf("!!!  TRAIN NOT FOUND  !!!");
	}
	if(nptr!=NULL)
	{
		coach_node *c1ptr,*cptr;
		int CId=1;
		cptr=nptr->coachList;
		if(cptr!=NULL)
		{
			while(cptr->next!=NULL)
			{
				cptr=cptr->next;
				CId++;
			}
			CId++;
		}
		c1ptr=makeCoach(CId);
		cptr=insertAtEnd_coach(cptr,c1ptr);
		nptr->coachList=cptr;
		if(c1ptr->class==SL)
		{
			(nptr->RAC->count)+=10;
		}
	}
}
void coach_detail(coach_node *nptr)
{
	sm_node *ptr;
	printf("\n->\tcoach ID:%d\n\tnumber of empty seats:%d\n",nptr->coachId,nptr->avalSeats);
		if(nptr->class==SL)
		{
			printf("\tclass:Sleeper\n");
		}
		else if(nptr->class==AC)
		{
			printf("\tclass:Air Conditioned\n");
		}
		for(int i=0;i<4;i++)
		{
			switch(i)
			{
				case 0:printf(" L\n");
					break;
				case 1:printf(" M\n");
					break;
				case 2:printf(" U\n");
					break;
				case 3:printf(" S\n");
					break;
				default:printf("problem!!!");
					break;
			}
			ptr=nptr->seatsArr[i];
			while(ptr)
			{
				printf("%d",ptr->colNo);
				ptr=ptr->next;
			}
			printf("\n");
		}
}
void traverse_coach_list(coach_node *cptr)
{
	coach_node *nptr;
	nptr=cptr;
	while(nptr!=NULL)
	{
		coach_detail(nptr);
		nptr=nptr->next;
	}
	printf("\n");
}
void train_detail(train_node *tptr)
{
	coach_node *cptr;
	printf("train id: %d",tptr->trainId);
	cptr=tptr->coachList;
	traverse_coach_list(cptr);
}
void traverse_train_list(train_node *tptr)
{
	train_node *nptr;
	nptr=tptr;
	while(nptr!=NULL)
	{
		train_detail(nptr);
		nptr=nptr->next;
	}
}
void traverse_passList(pass_node *nptr)
{
	pass_node *ptr;
	ptr=nptr;
	while(ptr!=NULL)
	{
		printf("%s\n",ptr->name);
		ptr=ptr->next;
	}
}
pass_node *insertAtEnd_pass(pass_node *dlptr,pass_node *nptr)
{
	pass_node *ptr;
	
	ptr=dlptr;
	if(dlptr==NULL)
	{
		dlptr=nptr;
	}
	else
	{
		while(ptr->next!=NULL)
		{
			ptr=ptr->next;
		}
		ptr->next=nptr;
		nptr->prev=ptr;
		nptr->next=NULL;
	}
	return dlptr;
}
int insertTicket_sm(sm_node *sm_seats[],pass_node *pass_ptr,int b)
{
	int retval=1,i;
	sm_node *ptr,*nptr,*next;
	nptr=(sm_node *)malloc(sizeof(sm_node));
	pass_ptr->AllocBerthType=b;
	nptr->pass_data=pass_ptr;
	ptr=sm_seats[b];
	if(ptr==NULL)
	{
		sm_seats[b]=nptr;
		pass_ptr->AllocBerthNo=0;
	}
	else if(b<3)
	{	i=0; //i is column number
		while(ptr->next!=NULL && i<9 && ptr->next->colNo==i+1)
		{
				ptr=ptr->next;
				i++;
		}
		if(i<9)
		{
			
			pass_ptr->AllocBerthNo=i+1;
			next=ptr->next;
			ptr->next=nptr;
			nptr->colNo=i+1;
			nptr->next=next;
		}
		else{
			retval=0;
		}
	}
	else if(b==3)
	{
		i=0; //i is column number
		while(ptr->next!=NULL && i<8 && ptr->next->colNo==i+2)
		{
				ptr=ptr->next;
				i+=2;
		}
		if(i<8)
		{
			
			pass_ptr->AllocBerthNo=i+3;
			next=ptr->next;
			ptr->next=nptr;
			nptr->colNo=i+2;
			nptr->next=next;
		}
		else{
			retval=0;
		}
	}
	return retval;
}
pass_node *deleteAtStart_pass(pass_node *passList,pass_node **pptr)
{
	pass_node *next_node,*ptr;
	if(passList!=NULL)
	{
		next_node=passList->next;
		ptr=passList;
		ptr->next=NULL;
		ptr->prev=NULL;
		passList=next_node;
		if(next_node!=NULL)
		{
			next_node ->prev=NULL;
		}
		*pptr=ptr;
	}
	return passList;
}
pass_node *insertAtStart_pass(pass_node *passList,pass_node *pptr)
{
	pass_node *next_node,*ptr;
	if(passList==NULL)
	{
		passList=pptr;
	}
	else if(passList!=NULL)
	{
		pptr->next=passList;
		passList->prev=pptr;
		passList=pptr;
	}
	return passList;
}
pass_node *makePass(int passId,char addr[],int phNo,int trainId)
{
	pass_node *nptr;
	int berthType;
		nptr=(pass_node *)malloc(sizeof(pass_node));
		nptr->passId=passId;
		//strcpy(nptr->address,addr);
		strcpy(nptr->address,addr);
		nptr->phoneNo=phNo;
		printf("enter name of passenger:");
		scanf("%s",nptr->name);
		printf("enter age of the passenger:");
		scanf("%d",&nptr->age);
		printf("enter prefrences of berths:\n1.lower 2.middle 3.upper 4.side upper   :");
		scanf("%d",&berthType);
		nptr->berthPref=(berthType-1);
		nptr->next=NULL;
		nptr->prev=NULL;
		nptr->ticketNo=-1;
		nptr->trainId=trainId;
		nptr->AllocCoachId=-1;
		nptr->AllocCoach_ptr=NULL;
		nptr->AllocBerthType=-1;
		nptr->AllocBerthNo=-1;
	return nptr;
}
void bookTicket(train_node *train_ptr,coach_type class,pass_node *passList)
{
	coach_node *coach;
	pass_node *pass_ptr;
	coach=train_ptr->coachList;
	int booked,found=0,ticketNo;
	while(coach!=NULL && !found)
	{
		if(coach->avalSeats>5 && coach->class==class)
			found=1;
		else
			coach=coach->next;
	}
	if(coach!=NULL)
	{
		berth_code b;
		while(passList!=NULL && coach!=NULL)
		{
			
			if(coach->class==class)
			{
				b=passList->berthPref;
				passList=deleteAtStart_pass(passList,&pass_ptr);
				booked=insertTicket_sm(coach->seatsArr,pass_ptr,b);
				if((!booked) && (pass_ptr->age<=15 || pass_ptr->age>=60))
				{
					b=0;
				}
				int count=0;
				while(!booked && count<=4)
				{
					booked=insertTicket_sm(coach->seatsArr,pass_ptr,b);
					b=(b+1)%4;
					count++;
				}
				if(booked)
				{
					pass_ptr->trainId=train_ptr->trainId;
					pass_ptr->AllocCoachId=coach->coachId;
					pass_ptr->AllocCoach_ptr=coach;
					ticketNo=(pass_ptr->trainId*100000)+(pass_ptr->AllocCoachId*100)+(pass_ptr->AllocBerthType*10)+(pass_ptr->AllocBerthNo);
					pass_ptr->ticketNo=ticketNo;
					insert_hash(pass_ptr); //hash function
					enqueue(train_ptr->passList,pass_ptr);//prr
					//enqueue(train_ptr->passList,pass_ptr)
					(coach->avalSeats)-=1;//booking data here
					printf("name:%s\t ticket number:%d\n",pass_ptr->name,pass_ptr->ticketNo); ////////
				}
				else
				{
					coach=coach->next;
					passList=insertAtStart_pass(passList,pass_ptr);
				}
			}
			else{
				coach=coach->next;
			}
		}
		
	}
	while(passList)
	{
		passList=deleteAtStart_pass(passList,&pass_ptr);
		int RAC;
		RAC=enqueueRAC(train_ptr,pass_ptr);
		if(!RAC)
		{
			enqueue(train_ptr->WL,pass_ptr);
		}
		pass_ptr->trainId=train_ptr->trainId;
		srand(time(0));
		ticketNo=(pass_ptr->trainId*100000)+rand()%100000;
		pass_ptr->ticketNo=ticketNo;
		insert_hash(pass_ptr); 
		
	}
}
void bookTickets(train_node *trainList)
{
	int trainId;
	train_node *tptr;
	printf("Enter train ID:");
	scanf("%d",&trainId);
	tptr=trainList;
	while(tptr!=NULL && tptr->trainId!=trainId)
	{		
		tptr=tptr->next;
	}
	if(tptr==NULL)
	{
		printf("Sorry no train found for given ID\n");
	}
	else{
		char addr[40];
		coach_type class;
		int phNo,classInt,passId,num;
		pass_node *passIdList,*pass_ptr;
		initialize_pass(&passIdList);
	
		printf("Enter your Passenger ID:");
		scanf("%d",&passId);
		printf("enter your address:");
		scanf("%s",addr);
		printf("enter your phone number:");
		scanf("%d",&phNo);
		printf("enter class\n1.sleeper\t2.ac:");
		scanf("%d",&classInt);
		if(classInt==AC)
		{
			class=AC;
		}
		else
		{
			class=SL;
		}
		printf("Enter number of tickets you want to book:");
		scanf("%d",&num);
		for(int i=1;i<=num;i++)
		{
			pass_ptr=makePass(passId,addr,phNo,trainId);
			passIdList=insertAtEnd_pass(passIdList,pass_ptr);
		}
		bookTicket(tptr,class,passIdList);
	}
	
}
void getBookingDetail(train_node *train_list)
{
	pass_node *ptr;
	ptr=train_list->passList->front;//prr
	printf("------------confirmed----------------\n");
	while(ptr)
	{
		printf("%s :  %d\n\tcoach No:%d berth:",ptr->name,ptr->ticketNo,ptr->AllocCoachId);
		switch(ptr->AllocBerthType)
		{
			case 0:printf(" L");
				break;
			case 1:printf(" M");
				break;
			case 2:printf(" U");
				break;
			case 3:printf(" S");
				break;
			default:printf("problem!!!");
				break;
		}
		printf(":%d\n",ptr->AllocBerthNo+1);
		ptr=ptr->next;
	}
	ptr=train_list->RAC->front;
	printf("----------RAC-----------\n");
	int i=0;
	while(ptr)
	{
		printf("%d.\t%s\t%d \n",++i,ptr->name,ptr->ticketNo);
		ptr=ptr->next;
	}
	ptr=train_list->WL->front;
	printf("----------Waiting list-----------\n");
	i=0;
	while(ptr)
	{
		printf("%d.\t %s \t %d \n ",++i,ptr->name,ptr->ticketNo);
		ptr=ptr->next;
	}
}
void empty_seat(coach_node *coach_ptr,int number,int index)
{
	sm_node *ptr,*nptr;
	int found=0;
	ptr=coach_ptr->seatsArr[index];
	if(ptr->colNo==number)
	{
		printf("first nodeee!!\n");
		nptr=ptr;
		ptr=ptr->next;
		coach_ptr->seatsArr[index]=ptr;
	}
	else{
		while(ptr->next!=NULL)
		{
			if(ptr->next->colNo==number)
			{
				nptr=ptr->next;
				ptr->next=nptr->next;
			}
			else{
				ptr=ptr->next;
			}
		}
	}
	(coach_ptr->avalSeats)++;
}
void delete_pass_node_fromTrainPassList(int trainId,train_node *trainList,pass_node *ptr)
{
	status_code sc=SUCCESS;
	
	while(trainList->trainId!=trainId && trainList!=NULL)
	{
		trainList=trainList->next;
	}
	pass_node *dlptr,*prev_node,*next_node;
	dlptr=trainList->passList->front; //prr
	if(ptr==NULL||dlptr==NULL)
	{
		sc=FAILURE;
	}
	else{
		prev_node=ptr->prev;
		next_node=ptr->next;
		if(prev_node!=NULL)
		{
			prev_node->next=next_node;
		}
		else
		{
			dlptr=next_node;
		}
		if(next_node!=NULL)
		{
			next_node->prev=prev_node;
		}
		trainList->passList->front=dlptr;
	}
}
void cancelTicket(train_node *trainList_ptr,int ticketNo)
{
	pass_node *ptr,*nextPass,*wlpass;
	ptr=delete_hash(ticketNo);
	if(ptr!=NULL)
	{ 	
		delete_pass_node_fromTrainPassList(ptr->trainId,trainList_ptr,ptr);
		empty_seat(ptr->AllocCoach_ptr,ptr->AllocBerthNo,ptr->AllocBerthType);
		nextPass=dequeueRAC(trainList_ptr);

		wlpass=dequeue(trainList_ptr->WL);
		if(wlpass!=NULL)
		{
			enqueueRAC(trainList_ptr,wlpass);
		}
		if(nextPass!=NULL)
		{
			nextPass->AllocCoach_ptr=ptr->AllocCoach_ptr;
			nextPass->AllocBerthNo=ptr->AllocBerthNo;
			nextPass->AllocBerthType=ptr->AllocBerthType;
			nextPass->AllocCoachId=ptr->AllocCoachId;
			nextPass->trainId=ptr->trainId;
			enqueue(trainList_ptr->passList,nextPass); //prr
			insertTicket_sm(ptr->AllocCoach_ptr->seatsArr,nextPass,ptr->AllocBerthType);
		}
		free(ptr);
	}
	else{
		printf("incorrect ticket number!! try again\n  ");
	}
}
int freezeBooking(train_node *trainList)
{
	printf("\nbooking freezed\n");
	return 1;
}
void printCoachData(train_node *tptr,coach_node *nptr,int frz)
{
	sm_node *ptr;
	pass_node *pass1,*pass2;
	printf("\n->\tcoach ID:%d\n\tnumber of empty seats:%d\n",nptr->coachId,nptr->avalSeats);
	if(nptr->class==SL)
	{
		printf("\tclass:Sleeper\n");
	}
	else if(nptr->class==AC)
	{
		printf("\tclass:Air Conditioned\n");
	}
	printf("passengers on this coach:\n");
	for(int i=0;i<4;i++)
	{
		char bt;
		switch(i)
		{
			case 0:bt='L';
				break;
			case 1:bt='M';
				break;
			case 2:bt='U';
				break;
			case 3:bt='S';
				break;
			default:printf("problem!!!");
				break;
		}
		ptr=nptr->seatsArr[i];
		while(ptr)
		{
			printf("%c%d\t%s\n",bt,ptr->colNo+1,ptr->pass_data->name);
			ptr=ptr->next;
		}
		printf("\n");
	}

	if(frz)
	{	int i=2;
		printf("RAC passengers\n");
		while(!(isEmpty(tptr->RAC)) && i<=10)
		{
			printf("S%d",i);
			pass1=dequeueRAC(tptr);
			if(pass1)
			{
				printf("\t%s",pass1->name);
			}
			pass2=dequeueRAC(tptr);
			if(pass2)
			{
				printf("\n\t%s",pass2->name);
			}
			i+=2;
			printf("\n");
		}
	}
}
void displayCoachData(int trainId,int coachId,train_node *tptr,int frz)
{
	coach_node *cptr;
	while(tptr!=NULL && tptr->trainId!=trainId)
	{
		tptr=tptr->next;
	}
	if(tptr)
	{
		cptr=tptr->coachList;
		while(cptr!=NULL && cptr->coachId!=coachId)
		{
			cptr=cptr->next;
		}
		if(cptr!=NULL)
		{
			printCoachData(tptr,cptr,frz);
		}
		else
		{
			printf("wrong coach id!!!!\n");
		}
	}
	else{
		printf("wrong train id!!!!\n");
	}
}
//-------------------------------------------------------------------------
//			main function
//-------------------------------------------------------------------------
int main()
{
	int choice,trainId,stop=0,ticketNo,coachId,freeze=0;
	train_node *train_list;
	initialize_train(&train_list);
	initialize_hash();
	train_list=createTrainList(train_list,2);
	while(!stop)
	{
		printf("-------------------\n1.add Coach to train\n2.make booking\n3.cancel ticket\n4.Freeze booking\n5.display coach data\n6.hash table data\n7.get booking list of train\n8.get train detail\n------------------->");
		scanf("%d",&choice);
		switch(choice)
		{
			case 1:	printf("enter train ID:");
				scanf("%d",&trainId);
				addCoach(train_list,trainId);
				break;
			case 2:	if(!freeze)
				{
					bookTickets(train_list);
					break;
				}	
			case 3:	if(!freeze)
				{
					printf("enter ticket Number:");
					scanf("%d",&ticketNo);
					cancelTicket(train_list,ticketNo);
					break;
				}
			case 4:freeze=freezeBooking(train_list);
				break;
			case 5:	printf("enter train id:");
				scanf("%d",&trainId);
				printf("enter coach id:");
				scanf("%d",&coachId);
				displayCoachData(trainId,coachId,train_list,freeze);
				break;
			case 8:	traverse_train_list(train_list);
				break;
			case 6:	print_hash_data();
				break;	
			case 7:	getBookingDetail(train_list);
				break;
			default:stop=1;
				break;
			}
	}
	return 0;
}
