#include <stdio.h>

typedef unsigned char   INT8U ;
typedef unsigned short  INT16U ;
typedef unsigned int    INT32U ;
typedef signed int      INT32S ;
typedef signed char     INT8S ;
typedef signed short    INT16S ;

typedef struct
{
	INT8U a,flags;
	INT8U c,b;
	INT8U e,d;
	INT8U l,h;
	INT16U pc;
	INT16U sp;
	INT8U im;
}cpureg;

INT8U memory[65536];
INT8U ports[256];

typedef struct
{
	INT16U af;
	INT16U bc;
	INT16U de;
	INT16U hl;
	INT16U pc;
	INT16U sp;
	INT8U im;
}cpureg16;

cpureg      x;
INT8U       opcode;
cpureg16    *pairs;
INT16U      p;
INT8U       temp;
INT8U       data1,data2;
INT8U       temp_cmp;
INT8U       stop_flag;

INT8U       brkmatch , brktst , watchtst ;

extern INT8U    trace ;
extern INT8U    step ;
extern INT8U    brkno ;
extern INT8U    watchno ;
extern INT16U   brkpt[ 8 ] ;
extern INT16U   watchstrt[ 8 ] ;
extern INT16U   watchend[ 8 ] ;


extern INT8U my_prog[32];

extern INT8U my_data[32];
extern INT16U prog_start;
extern INT16U prog_end;
extern INT16U data_start;
extern INT16U data_end;


INT8U no_of_ones(INT8U data)
{
	INT8U count=0,mask=0x80,i;
	for(i=0;i<8;i++)
	{
		if(( data & mask ) == mask) count++;
		mask>>=1;
	}
	return count;
}

void p_flags_ADD(void)
{
	if(x.a==0) x.flags|=0x40; else x.flags &= ~0x40;
	if( ( x.a & 0x80 ) == 0x80) x.flags|=0x80; else x.flags &= ~0x80;
	if((data1 & 0xF) + (data2 & 0xF) > 0xF) x.flags|=0x10; else x.flags &= ~0x10;
	if(no_of_ones(x.a) & 0x01) x.flags|=0x04; else x.flags &= ~0x04;
	if(((INT16U)data1+(INT16U)data2) & 0x100) x.flags|=0x01; else x.flags &= ~0x01;
}

void p_flags_ADC(void)
{
	if(x.a==0) x.flags|=0x40; else x.flags &= ~0x40;
	if((x.a & 0x80) == 0x80) x.flags|=0x80; else x.flags &= ~0x80;
	if(((data1 & 0xF) + (data2 & 0xF) + (INT16U)(x.flags & 0x01)) > 0xF) x.flags|=0x10; else x.flags &= ~0x10;
	if(no_of_ones(x.a) & 0x01) x.flags|=0x04; else x.flags &= ~0x04;
	if(((INT16U)data1 + (INT16U)data2 + (INT16U)(x.flags & 0x01)) & 0x100) x.flags|=0x01; else x.flags &= ~0x01;
}

void p_flags_SUB(void)
{
	if(x.a==0) x.flags|=0x40; else x.flags &= ~0x40;
	if((x.a & 0x80) == 0x80) x.flags|=0x80; else x.flags &= ~0x80;
	if((data1 & 0xF) < (data2 & 0xF)) x.flags|=0x10; else x.flags &= ~0x10;
	if(no_of_ones(x.a) & 0x01) x.flags|=0x04; else x.flags &= ~0x04;
	if(data1 < data2 ) x.flags|=0x01; else x.flags &= ~0x01;
}

void p_flags_SBB(void)
{
	if(x.a==0) x.flags|=0x40; else x.flags &= ~0x40;
	if((x.a & 0x80) == 0x80) x.flags|=0x80; else x.flags &= ~0x80;
	if((data1 & 0xF) < ((data2 & 0xF) + (x.flags & 0x01))) x.flags|=0x10; else x.flags &= ~0x10;
	if(no_of_ones(x.a) & 0x01) x.flags|=0x04; else x.flags &= ~0x04;
	if((data1) < (data2 + (x.flags & 0x01))) x.flags|=0x01; else x.flags &= ~0x01;
}

void p_flags_INR(INT8U reg)
{
	if(reg==0) x.flags|=0x40; else x.flags &= ~0x40;
	if((reg & 0x80) == 0x80) x.flags|=0x80; else x.flags &= ~0x80;
    if(no_of_ones(reg) & 0x01) x.flags|=0x04; else x.flags &= ~0x04;
    if((reg & 0x0F) == 0) x.flags|=0x10; else x.flags &= ~0x10;
}

void p_flags_DCR(INT8U reg)
{
	if(reg==0) x.flags|=0x40; else x.flags &= ~0x40;
	if((reg & 0x80) == 0x80) x.flags|=0x80; else x.flags &= ~0x80;
    if(no_of_ones(reg) & 0x01) x.flags|=0x04; else x.flags &= ~0x04;
    if((reg & 0x0F) == 0x0F) x.flags|=0x10; else x.flags &= ~0x10;
}

void p_flags_ANA(void)
{
	if(x.a==0) x.flags|=0x40; else x.flags &= ~0x40;
	if((x.a & 0x80) == 0x80) x.flags|=0x80; else x.flags &= ~0x80;
	x.flags|=0x10;
	if(no_of_ones(x.a) & 0x01) x.flags|=0x04; else x.flags &= ~0x04;
	x.flags &= ~0x01;
}

void p_flags_ORA_XRA(void)
{
	if(x.a==0) x.flags|=0x40; else x.flags &= ~0x40;
	if((x.a & 0x80) == 0x80) x.flags|=0x80; else x.flags &= ~0x80;
	x.flags&=~0x10;
	if(no_of_ones(x.a) & 0x01) x.flags|=0x04; else x.flags &= ~0x04;
	x.flags &= ~0x01;
}

void p_flags_CMP(void)
{
	if(temp_cmp==0) x.flags|=0x40; else x.flags &= ~0x40;
	if((temp_cmp & 0x80) == 0x80) x.flags|=0x80; else x.flags &= ~0x80;
	if((data1 & 0xF) < (data2 & 0xF)) x.flags|=0x10; else x.flags &= ~0x10;
	if(no_of_ones(temp_cmp) & 0x01) x.flags|=0x04; else x.flags &= ~0x04;
	if(data1 < data2 ) x.flags|=0x01; else x.flags &= ~0x01;
}

void simulate(void)
{
	    opcode=memory[x.pc];
	    pairs=(cpureg16*)(&x);
        if ( trace )
        {
            printf("\n");
            printf("PC=0x%x\t\t",x.pc);
        }
    	switch(opcode)
		{
			case 0x7F : x.a=x.a;          //MOV A,A
                        if ( trace ) printf( "mov   A,A\n");
			            x.pc++;
						break ;
			case 0x78 : x.a=x.b;          //MOV A,B
                        if ( trace ) printf( "mov   A,B\n");
			            x.pc++;
						break;
			case 0x79 : x.a=x.c;          //MOV A,C
                        if ( trace ) printf( "mov   A,C\n");
			            x.pc++;
						break;
			case 0x7A : x.a=x.d;          //MOV A,D
                        if ( trace ) printf( "mov   A,D\n");
			            x.pc++;
						break;
			case 0x7B : x.a=x.e;          //MOV A,E
			            if ( trace ) printf( "mov   A,E\n");
			            x.pc++;
						break;
			case 0x7C : x.a=x.h;          //MOV A,H
			            if ( trace ) printf( "mov   A,H\n");
			            x.pc++;
						break;
		    case 0x7D : x.a=x.l;          //MOV A,L
		                if ( trace ) printf( "mov   A,L\n");
			            x.pc++;
						break;
		    case 0x7E : p=pairs->hl;          //MOV A,M
		                if ( trace ) printf( "mov   A,M\n");
			            x.a=memory[p];
			            x.pc++;
						break;

			case 0x47 : x.b=x.a;          //MOV B,A
			            if ( trace ) printf( "mov   B,A\n");
			            x.pc++;
						break;
		    case 0x40 : x.b=x.b;          //MOV B,B
		                if ( trace ) printf( "mov   B,B\n");
			            x.pc++;
						break;
			case 0x41 : x.b=x.c;          //MOV B,C
			            if ( trace ) printf( "mov   B,C\n");
			            x.pc++;
						break;
			case 0x42 : x.b=x.d;          //MOV B,D
			            if ( trace ) printf( "mov   B,D\n");
			            x.pc++;
						break;
			case 0x43 : x.b=x.e;          //MOV B,E
			            if ( trace ) printf( "mov   B,E\n");
			            x.pc++;
						break;
			case 0x44 : x.b=x.h;          //MOV B,H
			            if ( trace ) printf( "mov   B,H\n");
			            x.pc++;
						break;
			case 0x45 : x.b=x.l;          //MOV B,L
			            if ( trace ) printf( "mov   B,L\n");
			            x.pc++;
						break;
			case 0x46 : p=pairs->hl;          //MOV B,M
			            if ( trace ) printf( "mov   B,M\n");
			            x.b=memory[p];
			            x.pc++;
						break;

			case 0x4F : x.c=x.a;          //MOV C,A
			            if ( trace ) printf( "mov   C,A\n");
			            x.pc++;
						break;
		    case 0x48 : x.c=x.b;          //MOV C,B
		                if ( trace ) printf( "mov   C,B\n");
			            x.pc++;
						break;
			case 0x49 : x.c=x.c;          //MOV C,C
			            if ( trace ) printf( "mov   C,C\n");
			            x.pc++;
						break;
		    case 0x4A : x.c=x.d;          //MOV C,D
		                if ( trace ) printf( "mov   C,D\n");
			            x.pc++;
						break;
			case 0x4B : x.c=x.e;          //MOV C,E
			            if ( trace ) printf( "mov   C,E\n");
			            x.pc++;
						break;
			case 0x4C : x.c=x.h;          //MOV C,H
			            if ( trace ) printf( "mov   C,H\n");
			            x.pc++;
						break;
			case 0x4D : x.c=x.l;          //MOV C,L
			            if ( trace ) printf( "mov   C,L\n");
			            x.pc++;
						break;
			case 0x4E : p=pairs->hl;          //MOV C,M
			            if ( trace ) printf( "mov   C,M\n");
			            x.c=memory[p];
			            x.pc++;
						break;

			case 0x57 : x.d=x.a;          //MOV D,A
			            if ( trace ) printf( "mov   D,A\n");
			            x.pc++;
						break;
			case 0x50 : x.d=x.b;          //MOV D,B
			            if ( trace ) printf( "mov   D,B\n");
			            x.pc++;
						break;
		    case 0x51 : x.d=x.c;          //MOV D,C
		                if ( trace ) printf( "mov   D,C\n");
			            x.pc++;
						break;
		    case 0x52 : x.d=x.d;          //MOV D,D
		                if ( trace ) printf( "mov   D,D\n");
			            x.pc++;
						break;
			case 0x53 : x.d=x.e;          //MOV D,E
			            if ( trace ) printf( "mov   D,E\n");
			            x.pc++;
						break;
		    case 0x54 : x.d=x.h;          //MOV D,H
		                if ( trace ) printf( "mov   D,H\n");
			            x.pc++;
						break;
 		    case 0x55 : x.d=x.l;          //MOV D,L
 		                if ( trace ) printf( "mov   D,L\n");
			            x.pc++;
						break;
			case 0x56 : p=pairs->hl;          //MOV D,M
			            if ( trace ) printf( "mov   D,M\n");
			            x.a=memory[p];
			            x.pc++;
						break;

			case 0x5F : x.e=x.a;          //MOV E,A
			            if ( trace ) printf( "mov   E,A\n");
			            x.pc++;
						break;
			case 0x58 : x.e=x.b;          //MOV E,B
			            if ( trace ) printf( "mov   E,B\n");
			            x.pc++;
						break;
			case 0x59 : x.e=x.c;          //MOV E,C
			            if ( trace ) printf( "mov   E,C\n");
			            x.pc++;
						break;
			case 0x5A : x.e=x.d;          //MOV E,D
			            if ( trace ) printf( "mov   E,D\n");
			            x.pc++;
						break;
		    case 0x5B : x.e=x.e;          //MOV E,E
		                if ( trace ) printf( "mov   E,E\n");
			            x.pc++;
						break;
		    case 0x5C : x.e=x.h;          //MOV E,H
		                if ( trace ) printf( "mov   E,H\n");
			            x.pc++;
						break;
			case 0x5D : x.e=x.l;          //MOV E,L
			            if ( trace ) printf( "mov   E,L\n");
			            x.pc++;
						break;
		    case 0x5E : p=pairs->hl;          //MOV E,M
		                if ( trace ) printf( "mov   E,M\n");
			            x.e=memory[p];
			            x.pc++;
						break;

			case 0x67 : x.h=x.a;          //MOV H,A
			            if ( trace ) printf( "mov   H,A\n");
			            x.pc++;
						break;
			case 0x60 : x.h=x.b;          //MOV H,B
			            if ( trace ) printf( "mov   H,B\n");
			            x.pc++;
						break;
			case 0x61 : x.h=x.c;          //MOV H,C
			            if ( trace ) printf( "mov   H,C\n");
			            x.pc++;
						break;
			case 0x62 : x.h=x.d;          //MOV H,D
			            if ( trace ) printf( "mov   H,D\n");
			            x.pc++;
						break;
			case 0x63 : x.h=x.e;          //MOV H,E
			            if ( trace ) printf( "mov   H,E\n");
			            x.pc++;
						break;
			case 0x64 : x.h=x.h;          //MOV H,H
			            if ( trace ) printf( "mov   H,H\n");
			            x.pc++;
						break;
		    case 0x65 : x.h=x.l;          //MOV H,L
		                if ( trace ) printf( "mov   H,L\n");
			            x.pc++;
						break;
		    case 0x66 : p=pairs->hl;          //MOV H,M
		                if ( trace ) printf( "mov   H,M\n");
			            x.a=memory[p];
			            x.pc++;
						break;

			case 0x6F : x.l=x.a;          //MOV L,A
			            if ( trace ) printf( "mov   L,A\n");
			            x.pc++;
						break;
			case 0x68 : x.l=x.b;          //MOV L,B
			            if ( trace ) printf( "mov   L,B\n");
			            x.pc++;
						break;
			case 0x69 : x.l=x.c;          //MOV L,C
			            if ( trace ) printf( "mov   L,C\n");
			            x.pc++;
						break;
			case 0x6A : x.l=x.d;          //MOV L,D
			            if ( trace ) printf( "mov   L,D\n");
			            x.pc++;
						break;
			case 0x6B : x.l=x.e;          //MOV L,E
			            if ( trace ) printf( "mov   L,E\n");
			            x.pc++;
						break;
			case 0x6C : x.l=x.h;          //MOV L,H
			            if ( trace ) printf( "mov   L,H\n");
			            x.pc++;
						break;
		    case 0x6D : x.l=x.l;          //MOV L,L
		                if ( trace ) printf( "mov   L,L\n");
			            x.pc++;
						break;
		    case 0x6E : p=pairs->hl;          //MOV L,M
		                if ( trace ) printf( "mov   L,M\n");
			            x.a=memory[p];
			            x.pc++;
						break;

			case 0x77 : p=pairs->hl;          //MOV M,A
			            if ( trace ) printf( "mov   M,A\n");
			            memory[p]=x.a;
			            x.pc++;
						break;
			case 0x70 : p=pairs->hl;          //MOV M,B
			            if ( trace ) printf( "mov   M,B\n");
			            memory[p]=x.b;
			            x.pc++;
						break;
			case 0x71 : p=pairs->hl;          //MOV M,C
                        if ( trace ) printf( "mov   M,C\n");
			            memory[p]=x.c;
			            x.pc++;
						break;
			case 0x72 : p=pairs->hl;          //MOV M,D
			            if ( trace ) printf( "mov   M,D\n");
			            memory[p]=x.d;
			            x.pc++;
						break;
			case 0x73 : p=pairs->hl;          //MOV M,E
			            if ( trace ) printf( "mov   M,E\n");
			            memory[p]=x.e;
			            x.pc++;
						break;
			case 0x74 : p=pairs->hl;          //MOV M,H
			            if ( trace ) printf( "mov   M,H\n");
			            memory[p]=x.h;
			            x.pc++;
						break;
		    case 0x75 : p=pairs->hl;          //MOV M,L
		                if ( trace ) printf( "mov   M,L\n");
			            memory[p]=x.l;
			            x.pc++;
						break;

			case 0x3E : x.a=memory[x.pc+1];        //MVI A,data8
			            if ( trace ) printf( "mvi   A,%x\n",x.a);
			            x.pc+=2;
				  		break;
			case 0x06 : x.b=memory[x.pc+1];        //MVI B,data8
			            if ( trace ) printf( "mvi   B,%x\n",x.b);
			            x.pc+=2;
				  		break;
			case 0x0E : x.c=memory[x.pc+1];        //MVI C,data8
			            if ( trace ) printf( "mvi   C,%x\n",x.c);
			            x.pc+=2;
				  		break;
	        case 0x16 : x.d=memory[x.pc+1];        //MVI D,data8
	                    if ( trace ) printf( "mvi   D,%x\n",x.d);
			            x.pc+=2;
				  		break;
			case 0x1E : x.e=memory[x.pc+1];        //MVI E,data8
			            if ( trace ) printf( "mvi   E,%x\n",x.e);
			            x.pc+=2;
				  		break;
			case 0x26 : x.h=memory[x.pc+1];        //MVI H,data8
			            if ( trace ) printf( "mvi   H,%x\n",x.h);
			            x.pc+=2;
				  		break;
			case 0x2E : x.l=memory[x.pc+1];        //MVI L,data8
			            if ( trace ) printf( "mvi   L,%x\n",x.l);
			            x.pc+=2;
				  		break;
			case 0x36 : p=pairs->hl;             //MVI M,data8
			            if ( trace ) printf( "mvi   M,%x\n",memory[x.pc+1]);
			            memory[p]=memory[x.pc+1];
			            x.pc+=2;
				  		break;

			case 0x01 : x.b=memory[x.pc+2];         //LXI B,data16
			            x.c=memory[x.pc+1];
                        if ( trace ) printf( "lxi   B,%x\n",pairs->bc);
						x.pc+=3;
						break;
			case 0x11 : x.d=memory[x.pc+2];         //LXI D,data16
			            x.e=memory[x.pc+1];
                        if ( trace ) printf( "lxi   D,%x\n",pairs->de);
						x.pc+=3;
						break;
			case 0x21 : x.h=memory[x.pc+2];         //LXI H,data16
			            x.l=memory[x.pc+1];
			            if ( trace ) printf( "lxi   H,%x\n",pairs->hl);
						x.pc+=3;
						break;
			case 0x31 : x.sp=memory[x.pc+2];        //LXI SP,data16
			            x.sp<<=8;
						x.sp+=memory[x.pc+1];
						if ( trace ) printf( "lxi   SP,%x\n",x.sp);
						x.pc+=3;
						break;

			case 0x3A : p=memory[x.pc+2];           //LDA addr
			            p<<=8;
			            p+=memory[x.pc+1];
			            x.a=memory[p];
			            if ( trace ) printf( "lda   %x\n",p);
						x.pc+=3;
						break;
			case 0x32 : p=memory[x.pc+2];           //STA addr
			            p<<=8;
			            p+=memory[x.pc+1];
			            memory[p]=x.a;
			            if ( trace ) printf( "sta   %x\n",p);
						x.pc+=3;
						break;

		    case 0x0A : p=memory[x.pc+2];           //LDAX B
			            p<<=8;
			            p+=memory[x.pc+1];
			            if ( trace ) printf( "ldax   b,%x\n",p);
			            x.c=memory[p];
						x.b=memory[p+1];
						x.pc+=3;
						break;
			case 0x1A : p=memory[x.pc+2];           //LDAX D
			            p<<=8;
			            p+=memory[x.pc+1];
			            if ( trace ) printf( "ldax   d,%x\n",p);
			            x.e=memory[p];
						x.d=memory[p+1];
						x.pc+=3;
						break;
			case 0x02 : p=memory[x.pc+2];           //STAX B
			            p<<=8;
			            p+=memory[x.pc+1];
			            if ( trace ) printf( "stax   b,%x\n",p);
			            memory[p]=x.c;
						memory[p+1]=x.b;
						x.pc+=3;
						break;
		  	case 0x12 : p=memory[x.pc+2];           //STAX D
			            p<<=8;
			            p+=memory[x.pc+1];
			            if ( trace ) printf( "stax   d,%x\n",p);
			            memory[p]=x.e;
						memory[p+1]=x.d;
						x.pc+=3;
						break;

			case 0x2A : p=memory[x.pc+2];           //LHLD addr
			            p<<=8;
			            p+=memory[x.pc+1];
			            if ( trace ) printf( "lhld    %x\n",p);
			            x.l=p;
						x.h=p+1;
						x.pc+=3;
						break;
		    case 0x22 : p=memory[x.pc+2];           //SHLD addr
			            p<<=8;
			            p+=memory[x.pc+1];
			            if ( trace ) printf( "shld    %x\n",p);
			            memory[p]=x.l;
						memory[p+1]=x.h;
						x.pc+=3;
						break;

			case 0xEB : temp=x.d;              //XCHG
			            x.d=x.h;
						x.h=temp;
						temp=x.e;
						x.e=x.l;
						x.l=temp;
						if ( trace ) printf( "xchg\n");
						x.pc++;
						break;

			case 0X87 : data1=data2=x.a;               //ADD A
			            x.a+=x.a;
						p_flags_ADD();
						if ( trace ) printf( "add   A\n");
						x.pc++;
						break;
			case 0X80 : data1=x.a;data2=x.b;           //ADD B
			            x.a+=x.b;
                        p_flags_ADD();
                        if ( trace ) printf( "add   B\n");
			            x.pc++;
						break;
			case 0X81 : data1=x.a;data2=x.c;           //ADD C
			            x.a+=x.c;
                        p_flags_ADD();
                        if ( trace ) printf( "add   C\n");
			            x.pc++;
						break;
		    case 0X82 : data1=x.a;data2=x.d;           //ADD D
			            x.a+=x.d;
                        p_flags_ADD();
                        if ( trace ) printf( "add   D\n");
			            x.pc++;
						break;
		    case 0X83 : data1=x.a;data2=x.e;           //ADD E
			            x.a+=x.e;
                        p_flags_ADD();
                        if ( trace ) printf( "add   E\n");
			            x.pc++;
						break;
			case 0X84 : data1=x.a;data2=x.h;           //ADD H
			            x.a+=x.h;
                        p_flags_ADD();
                        if ( trace ) printf( "add   H\n");
			            x.pc++;
						break;
			case 0X85 : data1=x.a;data2=x.l;           //ADD L
			            x.a+=x.l;
                        p_flags_ADD();
                        if ( trace ) printf( "add   L\n");
			            x.pc++;
						break;
			case 0X86 : p=pairs->hl;                   //ADD M
			            data1=x.a;data2=memory[p];
						x.a+=memory[p];
						if ( trace ) printf( "add   M\n");
                        p_flags_ADD();
			            x.pc++;
						break;

			case 0X8F : data1=x.a;data2=x.a;       //ADC A
			            x.a+=x.a;
			            x.a+=(x.flags & 0x01);
			            p_flags_ADC();
			            if ( trace ) printf( "adc   A\n");
						x.pc++;
						break;
			case 0X88 : data1=x.a;data2=x.b;       //ADC B
			            x.a+=x.b;
			            x.a+=(x.flags & 0x01);
						p_flags_ADC();
						if ( trace ) printf( "adc   B\n");
						x.pc++;
						break;
			case 0X89 : data1=x.a;data2=x.c;       //ADC C
			            x.a+=x.c;
			            x.a+=(x.flags & 0x01);
                        p_flags_ADC();
                        if ( trace ) printf( "adc   C\n");
						x.pc++;
						break;
		    case 0X8A : data1=x.a;data2=x.d;       //ADC D
			            x.a+=x.d;
			            x.a+=(x.flags & 0x01);
						p_flags_ADC();
						if ( trace ) printf( "adc   D\n");
						x.pc++;
						break;
		    case 0X8B : data1=x.a;data2=x.e;       //ADC E
			            x.a+=x.e;
			            x.a+=(x.flags & 0x01);
						p_flags_ADC();
						if ( trace ) printf( "adc   E\n");
						x.pc++;
						break;
			case 0X8C : data1=x.a;data2=x.h;       //ADC H
			            x.a+=x.h;
			            x.a+=(x.flags & 0x01);
						p_flags_ADC();
						if ( trace ) printf( "adc   H\n");
						x.pc++;
						break;
			case 0X8D : data1=x.a;data2=x.l;       //ADC L
			            x.a+=x.l;
			            x.a+=(x.flags & 0x01);
						p_flags_ADC();
						if ( trace ) printf( "adc   L\n");
						x.pc++;
						break;
			case 0X8E : p=pairs->hl;               //ADC M
			            data1=x.a;data2=memory[p];
			            x.a+=memory[p];
			            x.a+=(x.flags & 0x01);
						p_flags_ADC();
						if ( trace ) printf( "adc   M\n");
						x.pc++;
						break;

			case 0X97 : data1=x.a;data2=x.l;     //SUB A
			            x.a=x.a-x.a;
			            p_flags_SUB();
			            if ( trace ) printf( "sub   A\n");
						x.pc++;
						break;
			case 0X90 : data1=x.a;data2=x.l;     //SUB B
			            x.a=x.a-x.b;
			            p_flags_SUB();
			            if ( trace ) printf( "sub   B\n");
						x.pc++;
						break;
			case 0X91 : data1=x.a;data2=x.l;     //SUB C
			            x.a=x.a-x.c;
			            p_flags_SUB();
			            if ( trace ) printf( "sub   C\n");
						x.pc++;
						break;
		    case 0X92 : data1=x.a;data2=x.l;     //SUB D
			            x.a=x.a-x.d;
			            p_flags_SUB();
			            if ( trace ) printf( "sub   D\n");
						x.pc++;
						break;
		    case 0X93 : data1=x.a;data2=x.l;      //SUB E
			            x.a=x.a-x.e;
			            p_flags_SUB();
			            if ( trace ) printf( "sub   E\n");
						x.pc++;
						break;
			case 0X94 : data1=x.a;data2=x.l;      //SUB H
			            x.a=x.a-x.h;
			            p_flags_SUB();
			            if ( trace ) printf( "sub   H\n");
						x.pc++;
						break;
			case 0X95 : data1=x.a;data2=x.l;      //SUB L
			            x.a=x.a-x.l;
			            p_flags_SUB();
			            if ( trace ) printf( "sub   L\n");
						x.pc++;
						break;
			case 0X96 : p=pairs->hl;              //SUB M
			            data1=x.a;data2=memory[p];
						x.a=x.a-memory[p];
			            p_flags_SUB();
			            if ( trace ) printf( "sub   M\n");
						x.pc++;
						break;

			case 0X9F : data1=x.a;data2=x.a;       //SBB A
			            x.a-=x.a;
			            x.a-=(x.flags & 0x01);
			            p_flags_SBB();
			            if ( trace ) printf( "sbb   A\n");
						x.pc++;
						break;
			case 0X98 : data1=x.a;data2=x.b;       //SBB B
			            x.a-=x.b;
			            x.a-=(x.flags & 0x01);
						p_flags_SBB();
						if ( trace ) printf( "sbb   B\n");
						x.pc++;
						break;
			case 0X99 : data1=x.a;data2=x.c;       //SBB C
			            x.a-=x.c;
			            x.a-=(x.flags & 0x01);
                        p_flags_SBB();
                        if ( trace ) printf( "sbb   C\n");
						x.pc++;
						break;
		    case 0X9A : data1=x.a;data2=x.d;       //SBB D
			            x.a-=x.d;
			            x.a-=(x.flags & 0x01);
						p_flags_SBB();
						if ( trace ) printf( "sbb   D\n");
						x.pc++;
						break;
		    case 0X9B : data1=x.a;data2=x.e;       //SBB E
			            x.a-=x.e;
			            x.a-=(x.flags & 0x01);
						p_flags_SBB();
						if ( trace ) printf( "sbb   E\n");
						x.pc++;
						break;
			case 0X9C : data1=x.a;data2=x.h;       //SBB H
			            x.a-=x.h;
			            x.a-=(x.flags & 0x01);
						p_flags_SBB();
						if ( trace ) printf( "sbb   H\n");
						x.pc++;
						break;
			case 0X9D : data1=x.a;data2=x.l;       //SBB L
			            x.a-=x.l;
			            x.a-=(x.flags & 0x01);
						p_flags_SBB();
						if ( trace ) printf( "sbb   L\n");
						x.pc++;
						break;
			case 0X9E : p=pairs->hl;               //SBB M
			            data1=x.a;data2=memory[p];
			            x.a-=memory[p];
			            x.a-=(x.flags & 0x01);
						p_flags_SBB();
						if ( trace ) printf( "sbb   M\n");
						x.pc++;
						break;

			case 0xC6 : data1=x.a;data2=memory[x.pc+1];      //ADI data8
			            x.a=data1+data2;
						p_flags_ADD();
						if ( trace ) printf( "adi   %x\n",data2);
						x.pc+=2;
			            break;
			case 0xCE : data1=x.a;data2=memory[x.pc+1];      //ACI data8
			            x.a=data1+data2+(x.flags & 0x01);
						p_flags_ADC();
			            x.a+=(x.flags & 0x01);
			            if ( trace ) printf( "aci   %x\n",data2);
						x.pc+=2;
			            break;
		    case 0xD6 : data1=x.a;data2=memory[x.pc+1];      //SUI data8
			            x.a=data1-data2;
						p_flags_SUB();
						if ( trace ) printf( "sui   %x\n",data2);
			            x.pc+=2;
			            break;
			case 0xDE : data1=x.a;data2=memory[x.pc+1];      //SBI data8
			            x.a=data1-data2-(x.flags & 0x01);
						p_flags_SBB();
						if ( trace ) printf( "sbi   %x\n",data2);
			            x.pc+=2;
			            break;

			case 0x3C : x.a++;                             //INR A
			            p_flags_INR(x.a);
			            if ( trace ) printf( "inr   A\n");
			            x.pc++;
			            break;
			case 0x04 : x.b++;                             //INR B
			            p_flags_INR(x.b);
			            if ( trace ) printf( "inr   B\n");
			            x.pc++;
			            break;
			case 0x0C : x.c++;                             //INR C
			            p_flags_INR(x.c);
			            if ( trace ) printf( "inr   C\n");
			            x.pc++;
			            break;
		    case 0x14 : x.d++;                             //INR D
			            p_flags_INR(x.d);
			            if ( trace ) printf( "inr   D\n");
			            x.pc++;
			            break;
			case 0x1C : x.e++;                             //INR E
			            p_flags_INR(x.e);
			            if ( trace ) printf( "inr   E\n");
			            x.pc++;
			            break;
			case 0x24 : x.h++;                             //INR H
			            p_flags_INR(x.h);
			            if ( trace ) printf( "inr   H\n");
			            x.pc++;
			            break;
			case 0x2C : x.l++;                             //INR L
			            p_flags_INR(x.l);
			            if ( trace ) printf( "inr   L\n");
			            x.pc++;
			            break;
			case 0x34 : p=pairs->hl;                       //INR M
			            memory[p]++;
			            p_flags_INR(memory[p]);
			            if ( trace ) printf( "inr   M\n");
			            x.pc++;
			            break;

			case 0x3D : x.a--;                             //DCR A
			            p_flags_DCR(x.a);
			            if ( trace ) printf( "dcr   A\n");
			            x.pc++;
			            break;
			case 0x05 : x.b--;                             //DCR B
			            p_flags_DCR(x.b);
			            if ( trace ) printf( "dcr   B\n");
			            x.pc++;
			            break;
			case 0x0D : x.c--;                             //DCR C
			            p_flags_DCR(x.c);
			            if ( trace ) printf( "dcr   C\n");
			            x.pc++;
			            break;
		    case 0x15 : x.d--;                             //DCR D
			            p_flags_DCR(x.d);
			            if ( trace ) printf( "dcr   D\n");
			            x.pc++;
			            break;
			case 0x1D : x.e--;                             //DCR E
			            p_flags_DCR(x.e);
			            if ( trace ) printf( "dcr   E\n");
			            x.pc++;
			            break;
			case 0x25 : x.h--;                             //DCR H
			            p_flags_DCR(x.h);
			            if ( trace ) printf( "dcr   H\n");
			            x.pc++;
			            break;
			case 0x2D : x.l--;                             //DCR L
			            p_flags_DCR(x.l);
			            if ( trace ) printf( "dcr   L\n");
			            x.pc++;
			            break;
			case 0x35 : p=pairs->hl;                       //DCR M
			            memory[p]--;
			            p_flags_DCR(memory[p]);
			            if ( trace ) printf( "dcr   M\n");
			            x.pc++;
			            break;

			case 0x09 : data1=x.l;data2=x.c;                                                            //DAD B
			            if((((INT16U)data1+(INT16U)data2)&0x100)==0x100) temp=1; else temp=0;
			            x.l=data1+data2;
			            data1=x.h;data2=x.b;
			            if((((INT16U)data1+(INT16U)data2+(INT16U)temp) &0x100)==0x100) x.flags|=0x01; else x.flags&= ~0x01;
			            x.h=data1+data2+temp;
			            if ( trace ) printf( "dad   B\n");
			            x.pc++;
			            break;
			case 0x19 : data1=x.l;data2=x.e;                                                            //DAD D
			            if((((INT16U)data1+(INT16U)data2)&0x100)==0x100) temp=1; else temp=0;
			            x.l=data1+data2;
			            data1=x.h;data2=x.d;
			            if((((INT16U)data1+(INT16U)data2+(INT16U)temp) &0x100)==0x100) x.flags|=0x01; else x.flags&= ~0x01;
			            x.h=data1+data2+temp;
			            if ( trace ) printf( "dad   D\n");
			            x.pc++;
			            break;
			case 0x29 : data1=x.l;data2=x.l;                                                            //DAD H
			            if((((INT16U)data1+(INT16U)data2)&0x100)==0x100) temp=1; else temp=0;
			            x.l=data1+data2;
			            data1=x.h;data2=x.h;
			            if((((INT16U)data1+(INT16U)data2+(INT16U)temp) &0x100)==0x100) x.flags|=0x01; else x.flags&= ~0x01;
			            x.h=data1+data2+temp;
			            if ( trace ) printf( "dad   H\n");
			            x.pc++;
			            break;
			case 0x39 : if(((((INT32U)pairs->hl + (INT32U)x.sp)) & 0x10000) == 0x10000) x.flags|=0x01; else x.flags&= ~0x01;
			            pairs->hl += x.sp;           //DAD SP
			            if ( trace ) printf( "dad   SP\n");
			            x.pc++;
			            break;

			case 0x03 : pairs->bc=pairs->bc+1;      //INX B
                        if ( trace ) printf( "inx   B\n");
			            x.pc++;
			            break;
			case 0x13 : pairs->de=pairs->de+1;      //INX D
                        if ( trace ) printf( "inx   D\n");
			            x.pc++;
			            break;
			case 0x23 : pairs->hl=pairs->hl+1;      //INX H
			            if ( trace ) printf( "inx   H\n");
			            x.pc++;
			            break;
			case 0x33 : x.sp++;                     //INX SP
			            if ( trace ) printf( "inx   SP\n");
			            x.pc++;
						break;

            case 0x0B : pairs->bc=pairs->bc-1;      //DCX B
                        if ( trace ) printf( "dcx   B\n");
			            x.pc++;
			            break;
			case 0x1B : pairs->de=pairs->bc-1;      //DCX D
			            if ( trace ) printf( "dcx   D\n");
			            x.pc++;
			            break;
			case 0x2B : pairs->hl=pairs->bc-1;      //DCX H
			            if ( trace ) printf( "dcx   H\n");
			            x.pc++;
			            break;
			case 0x3B : x.sp--;                     //DCX SP
			            if ( trace ) printf( "dcx   SP\n");
			            x.pc++;
						break;

			case 0x27 : if (((x.a & 0x0F) > 9)	|| (x.flags & 0x10)) x.a+=6;           //DAA
			            p_flags_ADD();
			            if (((x.a & 0xF0) > 0x90) || (x.flags & 0x01)) x.a+=0x60;
						p_flags_ADD();
                        if ( trace ) printf( "daa\n");
						x.pc++;
						break;

			case 0x2F : x.a=~x.a;					//CMA
                        if ( trace ) printf( "cma\n");
			            x.pc++;
						break;

		    case 0x3F : x.flags ^= 0x01;            //CMC
		                if ( trace ) printf( "cmc\n");
			            x.pc++;
						break;

           	case 0x37 : x.flags |= 0x01;            //STC
           	            if ( trace ) printf( "stc\n");
                        x.pc++;
                        break;

            case 0xA7 : x.a &= x.a;                 //ANA A
                        p_flags_ANA();
                        if ( trace ) printf( "ana   A\n");
			            x.pc++;
			            break;
			case 0xA0 : x.a &= x.b;                 //ANA B
                        p_flags_ANA();
                        if ( trace ) printf( "ana   B\n");
			            x.pc++;
			            break;
			case 0xA1 : x.a &= x.c;                 //ANA C
                        p_flags_ANA();
                        if ( trace ) printf( "ana   C\n");
			            x.pc++;
			            break;
			case 0xA2 : x.a &= x.d;                 //ANA D
                        p_flags_ANA();
                        if ( trace ) printf( "ana   D\n");
			            x.pc++;
			            break;
			case 0xA3 : x.a &= x.e;                 //ANA E
                        p_flags_ANA();
                        if ( trace ) printf( "ana   E\n");
			            x.pc++;
			            break;
			case 0xA4 : x.a &= x.h;                 //ANA H
                        p_flags_ANA();
                        if ( trace ) printf( "ana   H\n");
			            x.pc++;
			            break;
			case 0xA5 : x.a &= x.l;                 //ANA L
                        p_flags_ANA();
                        if ( trace ) printf( "ana   L\n");
			            x.pc++;
			            break;
			case 0xA6 : p=pairs->hl;                //ANA M
			            x.a &= memory[p] ;
                        p_flags_ANA();
                        if ( trace ) printf( "ana   M\n");
			            x.pc++;
			            break;

            case 0xB7 : x.a &= x.a;                 //ORA A
                        p_flags_ORA_XRA();
                        if ( trace ) printf( "ora   A\n");
			            x.pc++;
			            break;
			case 0xB0 : x.a |= x.b;                 //ORA B
                        p_flags_ORA_XRA();
                        if ( trace ) printf( "ora   B\n");
			            x.pc++;
			            break;
			case 0xB1 : x.a |= x.c;                 //ORA C
                        p_flags_ORA_XRA();
                        if ( trace ) printf( "ora   C\n");
			            x.pc++;
			            break;
			case 0xB2 : x.a |= x.d;                 //ORA D
                        p_flags_ORA_XRA();
                        if ( trace ) printf( "ora   D\n");
			            x.pc++;
			            break;
			case 0xB3 : x.a |= x.e;                 //ORA E
                        p_flags_ORA_XRA();
                        if ( trace ) printf( "ora   E\n");
			            x.pc++;
			            break;
			case 0xB4 : x.a |= x.h;                 //ORA H
                        p_flags_ORA_XRA();
                        if ( trace ) printf( "ora   H\n");
			            x.pc++;
			            break;
			case 0xB5 : x.a |= x.l;                 //ORA L
                        p_flags_ORA_XRA();
                        if ( trace ) printf( "ora   L\n");
			            x.pc++;
			            break;
			case 0xB6 : p=pairs->hl;                //ORA M
			            x.a |= memory[p] ;
                        p_flags_ORA_XRA();
                        if ( trace ) printf( "ora   M\n");
			            x.pc++;
			            break;

            case 0xAF : x.a ^= x.a;                 //XRA A
                        p_flags_ORA_XRA();
                        if ( trace ) printf( "xra   A\n");
			            x.pc++;
			            break;
			case 0xA8 : x.a ^= x.b;                 //XRA B
                        p_flags_ORA_XRA();
                        if ( trace ) printf( "xra   B\n");
			            x.pc++;
			            break;
			case 0xA9 : x.a ^= x.c;                 //XRA C
                        p_flags_ORA_XRA();
                        if ( trace ) printf( "xra   C\n");
			            x.pc++;
			            break;
			case 0xAA : x.a ^= x.d;                 //XRA D
                        p_flags_ORA_XRA();
                        if ( trace ) printf( "xra   D\n");
			            x.pc++;
			            break;
			case 0xAB : x.a ^= x.e;                 //XRA E
                        p_flags_ORA_XRA();
                        if ( trace ) printf( "xra   E\n");
			            x.pc++;
			            break;
			case 0xAC : x.a ^= x.h;                 //XRA H
                        p_flags_ORA_XRA();
                        if ( trace ) printf( "xra   H\n");
			            x.pc++;
			            break;
			case 0xAD : x.a ^= x.l;                 //XRA L
                        p_flags_ORA_XRA();
                        if ( trace ) printf( "xra   L\n");
			            x.pc++;
			            break;
			case 0xAE : p=pairs->hl;                //XRA M
			            x.a ^= memory[p] ;
                        p_flags_ORA_XRA();
                        if ( trace ) printf( "xra   M\n");
			            x.pc++;
			            break;

			case 0XBF : data1=x.a;data2=x.a;     //CMP A
			            temp_cmp=x.a-x.a;
			            p_flags_CMP();
			            if ( trace ) printf( "cmp   A\n");
						x.pc++;
						break;
			case 0XB8 : data1=x.a;data2=x.b;     //CMP B
			            temp_cmp=x.a-x.b;
			            p_flags_CMP();
			            if ( trace ) printf( "cmp   B\n");
						x.pc++;
						break;
			case 0XB9 : data1=x.a;data2=x.c;     //CMP C
			            temp_cmp=x.a-x.c;
			            p_flags_CMP();
			            if ( trace ) printf( "cmp   C\n");
						x.pc++;
						break;
		    case 0XBA : data1=x.a;data2=x.d;     //CMP D
			            temp_cmp=x.a-x.d;
			            p_flags_CMP();
			            if ( trace ) printf( "cmp   D\n");
						x.pc++;
						break;
		    case 0XBB : data1=x.a;data2=x.e;      //CMP E
			            temp_cmp=x.a-x.e;
			            p_flags_CMP();
			            if ( trace ) printf( "cmp   E\n");
						x.pc++;
						break;
			case 0XBC : data1=x.a;data2=x.h;      //CMP H
			            temp_cmp=x.a-x.h;
			            p_flags_CMP();
			            if ( trace ) printf( "cmp   H\n");
						x.pc++;
						break;
			case 0XBD : data1=x.a;data2=x.l;      //CMP L
			            temp_cmp=x.a-x.l;
			            p_flags_CMP();
			            if ( trace ) printf( "cmp   L\n");
						x.pc++;
						break;
			case 0XBE : p=pairs->hl;              //CMP M
			            data1=x.a;data2=memory[p];
						temp_cmp=x.a-memory[p];
			            p_flags_CMP();
			            if ( trace ) printf( "cmp   M\n");
						x.pc++;
						break;

			case 0x07 : if((x.a & 0x80)) x.flags |= 0x01; else x.flags &= ~0x01;     //RLC
			            x.a<<=1;
						if((x.flags & 0x01)) x.a |= 0x01; else x.a &= ~0x01;
						if ( trace ) printf( "rlc\n");
						x.pc++;
						break;
			case 0x0F : if((x.a & 0x01)) x.flags |= 0x01; else x.flags &= ~0x01;     //RRC
			            x.a>>=1;
						if((x.flags & 0x01)) x.a |= 0x80; else x.a &= ~0x80;
						if ( trace ) printf( "rrc\n");
						x.pc++;
						break;
			case 0x17 : temp=(x.flags & 0x01);                                       //RAL
			            if((x.a & 0x80)) x.flags |= 0x01; else x.flags &= ~0x01;
						x.a<<=1;
						if(temp) x.a |= 0x01; else x.a &= ~0x01;
						if ( trace ) printf( "ral\n");
						x.pc++;
						break;
			case 0x1F : temp=(x.flags & 0x01);                                       //RAR
			            if((x.a & 0x01)) x.flags |= 0x01; else x.flags &= ~0x01;
						x.a>>=1;
						if(temp) x.a |= 0x80; else x.a &= ~0x80;
						if ( trace ) printf( "rar\n");
						x.pc++;
						break;

		    case 0xE6 : x.a &= memory[x.pc+1];                 //ANI data8
                        p_flags_ANA();
                        if ( trace ) printf( "ani   %x\n",memory[x.pc+1]);
			            x.pc+=2;
			            break;
			case 0xEE : x.a ^= memory[x.pc+1];                 //XRI data8
                        p_flags_ORA_XRA();
                        if ( trace ) printf( "xri   %x\n",memory[x.pc+1]);
			            x.pc+=2;
			            break;
			case 0xF6 : x.a |= memory[x.pc+1];                 //ORI data8
                        p_flags_ORA_XRA();
                        if ( trace ) printf( "ori   %x\n",memory[x.pc+1]);
			            x.pc+=2;
			            break;
			case 0XFE : data1=x.a;data2=memory[x.pc+1];        //CPI data8
			            temp_cmp=data1-data2;
			            p_flags_CMP();
			            if ( trace ) printf( "cpi   %x\n",memory[x.pc+1]);
						x.pc+=2;
						break;

		    case 0xC3 : p=memory[x.pc+2];                      //JMP addr
			            p<<=8;
			            p+=memory[x.pc+1];
			            if ( trace ) printf("jmp    %x\n",p);
			            x.pc=p;
			            break;
			case 0xC2 : p=memory[x.pc+2];                      //JNZ addr
			            p<<=8;
			            p+=memory[x.pc+1];
			            if ( trace ) printf("jnz    %x\n",p);
			            if((x.flags & 0x40) == 0)x.pc=p; else x.pc+=3;
			            break;
			case 0xCA : p=memory[x.pc+2];                      //JZ addr
			            p<<=8;
			            p+=memory[x.pc+1];
			            if ( trace ) printf("jz     %x\n",p);
			            if(x.flags & 0x40) x.pc=p; else x.pc+=3;
			            break;
			case 0xD2 : p=memory[x.pc+2];                      //JNC addr
			            p<<=8;
			            p+=memory[x.pc+1];
			            if ( trace ) printf("jnc    %x\n",p);
			            if((x.flags & 0x01) == 0) x.pc=p; else x.pc+=3;
			            break;
			case 0xDA : p=memory[x.pc+2];                      //JC addr
			            p<<=8;
			            p+=memory[x.pc+1];
			            if ( trace ) printf("jc     %x\n",p);
			            if(x.flags & 0x01) x.pc=p; else x.pc+=3;
			            break;
			case 0xE2 : p=memory[x.pc+2];                      //JPO addr
			            p<<=8;
			            p+=memory[x.pc+1];
			            if ( trace ) printf("jpo    %x\n",p);
			            if(x.flags & 0x04) x.pc=p; else x.pc+=3;
			            break;
			case 0xEA : p=memory[x.pc+2];                      //JPE addr
			            p<<=8;
			            p+=memory[x.pc+1];
			            if ( trace ) printf("jpe    %x\n",p);
			            if((x.flags & 0x04) == 0) x.pc=p; else x.pc+=3;
			            break;
			case 0xF2 : p=memory[x.pc+2];                      //JP addr
			            p<<=8;
			            p+=memory[x.pc+1];
			            if ( trace ) printf("jp     %x\n",p);
			            if((x.flags & 0x80) == 0) x.pc=p; else x.pc+=3;
			            break;
			case 0xFA : p=memory[x.pc+2];                      //JM addr
			            p<<=8;
			            p+=memory[x.pc+1];
			            if ( trace ) printf("jm     %x\n",p);
			            if(x.flags & 0x80) x.pc=p; else x.pc+=3;
			            break;

			case 0xCD : memory[x.sp-2]=(x.pc+3);               //CALL addr
			            memory[x.sp-1]=(x.pc+3)>>8;
						x.sp-=2;
			            p=memory[x.pc+2];
			            p<<=8;
			            p+=memory[x.pc+1];
			            if ( trace ) printf("call   %x\n",p);
			            x.pc=p;
			            break;

			case 0xC9 : x.pc=memory[x.sp];                     //RET
			            x.pc|=(INT16U)memory[x.sp+1]<<8;
			            if ( trace ) printf("ret\n");
						x.sp+=2;
			            break;

			case 0xC4 : if((x.flags & 0x40) == 0)                //CNZ addr
			            {
							memory[x.sp-2]=x.pc+3;
			                memory[x.sp-1]=(x.pc+3)>>8;
							x.sp-=2;
			                p=memory[x.pc+2];
			            	p<<=8;
			            	p+=memory[x.pc+1];
			            	x.pc=p;
						}
						else x.pc+=3;
						if ( trace ) printf("cnz   %x\n",p);
						break;
			case 0xCC : if(x.flags & 0x40)                    //CZ addr
			            {
							memory[x.sp-2]=x.pc+3;
			                memory[x.sp-1]=(x.pc+3)>>8;
							x.sp-=2;
			                p=memory[x.pc+2];
			            	p<<=8;
			            	p+=memory[x.pc+1];
			            	x.pc=p;
						}
						else x.pc+=3;
						if ( trace ) printf("cz   %x\n",p);
						break;
			case 0xD4 : if((x.flags & 0x01) == 0)                //CNC addr
			            {
							memory[x.sp-2]=x.pc+3;
			                memory[x.sp-1]=(x.pc+3)>>8;
							x.sp-=2;
			                p=memory[x.pc+2];
			            	p<<=8;
			            	p+=memory[x.pc+1];
			            	x.pc=p;
						}
						else x.pc+=3;
						if ( trace ) printf("cnc   %x\n",p);
						break;
			case 0xDC : if(x.flags & 0x01)                     //CC addr
			            {
							memory[x.sp-2]=x.pc+3;
			                memory[x.sp-1]=(x.pc+3)>>8;
							x.sp-=2;
			                p=memory[x.pc+2];
			            	p<<=8;
			            	p+=memory[x.pc+1];
			            	x.pc=p;
						}
						else x.pc+=3;
						if ( trace ) printf("cc   %x\n",p);
						break;
			case 0xE4 : if(x.flags & 0x04)                     //CPO addr
			            {
							memory[x.sp-2]=x.pc+3;
			                memory[x.sp-1]=(x.pc+3)>>8;
							x.sp-=2;
			                p=memory[x.pc+2];
			            	p<<=8;
			            	p+=memory[x.pc+1];
			            	x.pc=p;
						}
						else x.pc+=3;
						if ( trace ) printf("cpo   %x\n",p);
						break;
			case 0xEC : if((x.flags & 0x04) == 0)                //CPE addr
			            {
							memory[x.sp-2]=x.pc+3;
			                memory[x.sp-1]=(x.pc+3)>>8;
							x.sp-=2;
			                p=memory[x.pc+2];
			            	p<<=8;
			            	p+=memory[x.pc+1];
			            	x.pc=p;
						}
						else x.pc+=3;
						if ( trace ) printf("cpe   %x\n",p);
						break;
			case 0xF4 : if((x.flags & 0x80) == 0)                //CP addr
			            {
							memory[x.sp-2]=x.pc+3;
			                memory[x.sp-1]=(x.pc+3)>>8;
							x.sp-=2;
			                p=memory[x.pc+2];
			            	p<<=8;
			            	p+=memory[x.pc+1];
			            	x.pc=p;
						}
						else x.pc+=3;
						if ( trace ) printf("cp   %x\n",p);
						break;
			case 0xFC : if(x.flags & 0x40)                     //CM addr
			            {
							memory[x.sp-2]=x.pc+3;
			                memory[x.sp-1]=(x.pc+3)>>8;
							x.sp-=2;
			                p=memory[x.pc+2];
			            	p<<=8;
			            	p+=memory[x.pc+1];
			            	x.pc=p;
						}
						else x.pc+=3;
						if ( trace ) printf("cm   %x\n",p);
						break;

			case 0xC0 : if((x.flags & 0x40) == 0)                //RNZ addr
			            {
							x.pc=memory[x.sp];
			                x.pc|=(INT16U)memory[x.sp+1]<<8;
							x.sp+=2;
			            	break;
						}
						else x.pc+=3;
						if ( trace ) printf("rnz\n");
						break;
			case 0xC8 : if(x.flags & 0x40)                     //RZ addr
			            {
							x.pc=memory[x.sp];
			            	x.pc|=(INT16U)memory[x.sp+1]<<8;
							x.sp+=2;
			            	break;
						}
						else x.pc+=3;
						if ( trace ) printf("rz\n");
						break;
			case 0xD0 : if((x.flags & 0x01) == 0)                //RNC addr
			            {
							x.pc=memory[x.sp];
			            	x.pc|=(INT16U)memory[x.sp+1]<<8;
							x.sp+=2;
			            	break;
						}
						else x.pc+=3;
						if ( trace ) printf("rnc\n");
						break;
			case 0xD8 : if(x.flags & 0x01)                     //RC addr
			            {
							x.pc=memory[x.sp];
				            x.pc|=(INT16U)memory[x.sp+1]<<8;
							x.sp+=2;
				            break;
						}
						else x.pc+=3;
						if ( trace ) printf("rc\n");
						break;
			case 0xE0 : if(x.flags & 0x04)                     //RPO addr
			            {
							x.pc=memory[x.sp];
				            x.pc|=(INT16U)memory[x.sp+1]<<8;
							x.sp+=2;
				            break;
						}
						else x.pc+=3;
						if ( trace ) printf("rpo\n");
						break;
			case 0xE8 : if((x.flags & 0x04) == 0)                //RPE addr
			            {
							x.pc=memory[x.sp];
			    	        x.pc|=(INT16U)memory[x.sp+1]<<8;
							x.sp+=2;
			        	    break;
						}
						else x.pc+=3;
						if ( trace ) printf("rpe\n");
						break;
			case 0xF0 : if((x.flags & 0x80) == 0)                //RP addr
			            {
							x.pc=memory[x.sp];
			            	x.pc|=(INT16U)memory[x.sp+1]<<8;
							x.sp+=2;
			            	break;
						}
						else x.pc+=3;
						if ( trace ) printf("rp\n");
						break;
			case 0xF8 : if(x.flags & 0x40)                     //RM addr
			            {
							x.pc=memory[x.sp];
			  	        	x.pc|=(INT16U)memory[x.sp+1]<<8;
							x.sp+=2;
			    	    	break;
						}
						else x.pc+=3;
						if ( trace ) printf("rm\n");
						break;

			case 0xE9 : x.pc=pairs->hl;                        //PCHL
                        if ( trace ) printf("pchl\n");
			            break;

			case 0xDB : x.a=ports[memory[x.pc+1]];              //IN port
                        if ( trace ) printf("in    %x\n",memory[x.pc+1]);
			            x.pc+=2;
						break;
			case 0xD3 : ports[memory[x.pc+1]]=x.a;              //OUT port
                        if ( trace ) printf("out    %x\n",memory[x.pc+1]);
			            x.pc+=2;
			            break;

			case 0xC5 : memory[x.sp-1]=x.b;                    //PUSH B
			            memory[x.sp-2]=x.c;
			            x.sp-=2;
			            if ( trace ) printf("push   B\n");
			            x.pc++;
			            break;
			case 0xD5 : memory[x.sp-1]=x.d;                    //PUSH D
			            memory[x.sp-2]=x.e;
			            x.sp-=2;
			            if ( trace ) printf("push   D\n");
			            x.pc++;
			            break;
			case 0xE5 : memory[x.sp-1]=x.h;                    //PUSH H
			            memory[x.sp-2]=x.l;
			            x.sp-=2;
			            if ( trace ) printf("push   H\n");
			            x.pc++;
			            break;
			case 0xF5 : memory[x.sp-1]=x.a;                    //PUSH PSW
			            memory[x.sp-2]=x.flags;
			            x.sp-=2;
			            if ( trace ) printf("push   PSW\n");
			            x.pc++;
			            break;

		    case 0xC1 : x.c=memory[x.sp];                     //POP B
		                x.b=memory[x.sp+1];
		                x.sp+=2;
		                if ( trace ) printf("pop    B\n");
		                x.pc++;
		                break;
		    case 0xD1 : x.e=memory[x.sp];                     //POP D
		                x.d=memory[x.sp+1];
		                x.sp+=2;
		                if ( trace ) printf("pop    D\n");
		                x.pc++;
		                break;
		    case 0xE1 : x.l=memory[x.sp];                     //POP H
		                x.h=memory[x.sp+1];
		                x.sp+=2;
		                if ( trace ) printf("pop    H\n");
		                x.pc++;
		                break;
		    case 0xF1 : x.flags=memory[x.sp];                 //POP PSW
		                x.a=memory[x.sp+1];
		                x.sp+=2;
		                if ( trace ) printf("pop    PSW\n");
		                x.pc++;
		                break;

		    case 0xE3 : data1=x.l;data2=x.h;                   //XTHL
		                x.l=memory[x.sp];
		                x.h=memory[x.sp+1];
		                memory[x.sp]=data1;
		                memory[x.sp+1]=data2;
		                x.pc++;
		                if ( trace ) printf("xthl\n");
		                break;

		    case 0xF9 : x.sp=pairs->hl;                        //SPHL
		                x.pc++;
		                if ( trace ) printf("sphl\n");
		                break;

		    case 0xFB : x.im|=8;                               //EI
			            x.pc++;
			            if ( trace ) printf("ei\n");
						break;
			case 0xF3 : x.im &= ~8;                            //DI
			            x.pc++;
			            if ( trace ) printf("di\n");
						break;
			case 0x20 : x.a = x.im;                            //RIM
			            x.pc++;
			            if ( trace ) printf("rim\n");
			            break;
			case 0x30 : x.im = x.a;                            //SIM
			            x.pc++;
			            if ( trace ) printf("sim\n");
			            break;

		    case 0x00 : x.pc++;                                //NOP
                        if ( trace ) printf("nop\n");
			            break;

			case 0x76 : stop_flag=1;                           //HLT
                        if ( trace ) printf("hlt\n");
			            break;

			case 0xC7 : memory[x.sp-2]= x.pc+1;                //RST 0
			            memory[x.sp-1]= (x.pc+1)>>8;
			            x.sp-=2;
			            x.pc=0;
			            if ( trace ) printf("rst    0\n");
			            break;
            case 0xCF : memory[x.sp-2]= x.pc+1;                //RST 1
			            memory[x.sp-1]= (x.pc+1)>>8;
			            x.sp-=2;
			            x.pc=8;
			            if ( trace ) printf("rst    1\n");
			            break;
            case 0xD7 : memory[x.sp-2]= x.pc+1;                //RST 2
			            memory[x.sp-1]= (x.pc+1)>>8;
			            x.sp-=2;
			            x.pc=16;
			            if ( trace ) printf("rst    2\n");
			            break;
            case 0xDF : memory[x.sp-2]= x.pc+1;                //RST 3
			            memory[x.sp-1]= (x.pc+1)>>8;
			            x.sp-=2;
			            x.pc=24;
			            if ( trace ) printf("rst    3\n");
			            break;
            case 0xE7 : memory[x.sp-2]= x.pc+1;                //RST 4
			            memory[x.sp-1]= (x.pc+1)>>8;
			            x.sp-=2;
			            x.pc=32;
			            if ( trace ) printf("rst    4\n");
			            break;
            case 0xEF : memory[x.sp-2]= x.pc+1;                //RST 5
			            memory[x.sp-1]= (x.pc+1)>>8;
			            x.sp-=2;
			            x.pc=40;
			            if ( trace ) printf("rst    5\n");
			            break;
            case 0xF7 : memory[x.sp-2]= x.pc+1;                //RST 6
			            memory[x.sp-1]= (x.pc+1)>>8;
			            x.sp-=2;
			            x.pc=48;
			            if ( trace ) printf("rst    6\n");
			            break;
            case 0xFF : memory[x.sp-2]= x.pc+1;                //RST 7
			            memory[x.sp-1]= (x.pc+1)>>8;
			            x.sp-=2;
			            x.pc=56;
			            if ( trace ) printf("rst    7\n");
			            break;

			default : printf("invalid opcode\n");
						   break;
    }
}

void print_reg(void)
{
 	printf("A=0x%x\tB=0x%x\tC=0x%x\tD=0x%x\tE=0x%x\tH=0x%x\tL=0x%x\n",x.a,x.b,x.c,x.d,x.e,x.h,x.l);
	printf("PC=0x%x\tSP=0x%x\tIM=0x%x\n",x.pc,x.sp,x.im);
	printf("Flags : S=%x\tZ=%x\tAC=%x\tP=%x\tC=%x\n",(x.flags>>7)&0x01,(x.flags>>6)&0x01,(x.flags>>4)&0x01,(x.flags>>2)&0x01,x.flags&0x01);
}

void print_mem(INT16U start,INT16U end)
{
	INT16U addr,i;
	for(addr=start;addr<=end;)
	{
		printf("0x%x\t",addr);
		for( i = 0 ; i < 8 ; i++)
		{
            printf("0x%x\t",memory[addr]);
            addr++;
            if(addr>end)break;
        }
        printf("\n");
	}
}

INT32S main()
{
    INT16U i;
    for(i=0;i<(prog_end-prog_start+1);i++)
    {
        memory[prog_start+i]=my_prog[i];
    }
    for(i=0;i<(data_end-data_start+1);i++)
    {
    	memory[data_start+i]=my_data[i];
	}
	stop_flag=0;
	printf("Developed by Sameehan Deodhar\n");
	printf("May freely be copied for educational purposes without tampering the above line\n");
	printf("Press Enter to continue\n");
	getchar();
	x.pc = prog_start ;
	do
	{
		simulate();
		if ( trace ) print_reg();
		brkmatch = 0 ;
		for ( brktst = 0 ; brktst < brkno ; brktst++ )
		{
            if ( x.pc == brkpt[ brktst ] ) brkmatch = 1 ;
		}
		if (( step ) || ( brkmatch ))
		{
            if ( !trace ) print_reg();
            for ( watchtst = 0 ; watchtst < watchno ; watchtst++ )
            {
                print_mem( watchstrt[ watchtst ] , watchend[ watchtst] ) ;
            }
            getchar() ;
		}
	}while(stop_flag == 0);
	printf("\n");
	printf("8085 Program Halted\n");
	print_reg();
	print_mem(data_start,data_end);
	return 1 ;
}
