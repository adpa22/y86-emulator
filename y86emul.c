#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "y86emul.h"

unsigned char * emem; //where the memory is stored
int regs[8] = {0,0,0,0,0,0,0,0};
int pc = 0;
int last = 0;
int icall = 0;
char cread = ' ';
boolean of; //overflow
boolean zf; //zero flag
boolean sf; //negative flag
#define ESP 4


//goes through the program, does the instructions
void runProg(){
	boolean go=true;
	unsigned char op;
	unsigned char r1;
	unsigned char r2;
	int val = 0;
	while(go){
		op=emem[pc];
		switch(op){
			case 0x00: //nop
				pc ++;
				break;
			case 0x10: //halt
				printf("\nProgram stopped with program counter at %x. HLT\n", pc);
				go=false;
				pc++;
				break;
			case 0x20: //rrmovl
				r1=emem[pc+1]>>4&0xff;
				r2=emem[pc+1]&0x0f;
				regs[r2]=regs[r1];
				pc+=2;
				break;
			case 0x30: //irmovl
				r2=emem[pc+1]&0x0f;
				val=0;
				
				val =emem[pc+2] +
					(emem[pc+3]<<8) +
					(emem[pc+4]<<16) +
					(emem[pc+5]<<24);
				regs[r2]=val;
				pc+=6;
				break;
			case 0x40: //rmmovl
				r1=emem[pc+1]>>4&0xff;
				r2=emem[pc+1]&0x0f;
				val=0;
				val =emem[pc+2] +
					(emem[pc+3]<<8) +
					(emem[pc+4]<<16) +
					(emem[pc+5]<<24);
						
				emem[regs[r2]+val]=(regs[r1]&0x000000ff);
				emem[regs[r2]+val+1]=(regs[r1]&0x0000ff00)>>8;
				emem[regs[r2]+val+2]=(regs[r1]&0x00ff0000)>>16;
				emem[regs[r2]+val+3]=(regs[r1]&0xff000000)>>24;
				pc+=6;
				break;
			case 0x50: //mrmovl
				r1=emem[pc+1]>>4&0xff;
				r2=emem[pc+1]&0x0f;
				val=0;
				val =emem[pc+2] +
					(emem[pc+3]<<8) +
					(emem[pc+4]<<16) +
					(emem[pc+5]<<24);
				
				regs[r1]=(emem[regs[r2]+val])+
						(emem[regs[r2]+val+1]<<8)+
						(emem[regs[r2]+2]<<16)+
						(emem[regs[r2]+3]<<24);	
				pc+=6;
				break;
			case 0x60: //addl
				r1=emem[pc+1]>>4&0xff;
				r2=emem[pc+1]&0x0f;
				val=0;
				val=regs[r1]+regs[r2];
				of=false;
				zf=false;
				sf=false;
				if((regs[r1]>0&&regs[r2]>0&&val<0)||
				(regs[r1]<0&&regs[r2]<0&&val>0)){
					of=true;
				}
				if(val<0){
					sf=true;
				}
				if(val==0){
					zf=true;
				}
				regs[r2]=val;
				pc+=2;
				break;
			case 0x61: //subl
				r1=emem[pc+1]>>4&0xff;
				r2=emem[pc+1]&0x0f;
				val=0;
				val=regs[r2]-regs[r1];
				of=false;
				zf=false;
				sf=false;
				if((regs[r1]<0&&regs[r2]>0&&val<0)||
						(regs[r1]>0&&regs[r2]<0&&val>0)){
					of=true;
				}
				if(val<0){
					sf=true;
				}
				if(val==0){
					zf=true;
				}
				
			
				regs[r2]=val;
				pc+=2;
				break;
			case 0x62: //andl
				r1=emem[pc+1]>>4&0xff;
				r2=emem[pc+1]&0x0f;
				val=0;
				val=regs[r1]&regs[r2];
				zf=false;
				sf=false;
				if(val<0){
					sf=true;
				}
				if(val==0){
					zf=true;
				}
				pc+=2;
				break;
			case 0x63: //xorl
				r1=emem[pc+1]>>4&0xff;
				r2=emem[pc+1]&0x0f;
				val=0;
				val=regs[r1]^regs[r2];
				zf=false;
				sf=false;
				if(val<0){
					sf=true;
				}
				if(val==0){
					zf=true;
				}
				pc+=2;
				break;
			case 0x64: //mull
				r1=emem[pc+1]>>4&0xff;
				r2=emem[pc+1]&0x0f;
				val=0;
				val=regs[r1]*regs[r2];
				of=false;
				zf=false;
				sf=false;
				if((regs[r1]>0&&regs[r2]>0&&val<0)||
				(regs[r1]<0&&regs[r2]<0&&val<0)||
				(regs[r1]>0&&regs[r2]<0&&val>0)||
				(regs[r1]<0&&regs[r2]>0&&val>0)){
					of=true;
				}
				if(val<0){
					sf=true;
				}
				if(val==0){
					zf=true;
				}

				regs[r2]=val;
				pc+=2;
				break;
			case 0x65: //cmpl
				r1=emem[pc+1]>>4&0xff;
				r2=emem[pc+1]&0x0f;
				if(regs[r1]==regs[r2]){
					zf=true;
				}else{
					zf=false;
				}
				if(regs[r2]-regs[r1]<0){
					sf=true;
				}else{
					sf=false;
				}
				pc+=2;
				break;
			case 0x70: //jmp
				val=0;
				val+=emem[pc+1];
				val+=emem[pc+2]<<8;
				val+=emem[pc+3]<<16;
				val+=emem[pc+4]<<24;
				
				pc=val;
				break;
			case 0x71: //jle
				val=0;
				val=emem[pc+1] + 
					(emem[pc+2]<<8) + 
					(emem[pc+3]<<16) + 
					(emem[pc+4]<<24);
				
				if(zf==1||(sf!=of)){
				pc=val;
				}else{
					pc+=5;
				}
				break;
			case 0x72: //jl
				val=0;
				val=emem[pc+1] + 
					(emem[pc+2]<<8) + 
					(emem[pc+3]<<16) + 
					(emem[pc+4]<<24);
				
				if((sf!=of)){
				pc=val;	
				}else{
					pc+=5;
				}
				break;
			case 0x73: //je
				val=0;
				val=emem[pc+1] + 
					(emem[pc+2]<<8) + 
					(emem[pc+3]<<16) + 
					(emem[pc+4]<<24);
				
				if(zf==1){
				pc=val;	
				}else{
					pc+=5;
				}
				break;
			case 0x74: //jne
				val=0;
				val=emem[pc+1] + 
					(emem[pc+2]<<8) + 
					(emem[pc+3]<<16) + 
					(emem[pc+4]<<24);
				
				if(zf==0){
					pc=val;	
				}else{
					pc+=5;
				}
				break;
			case 0x75: //jge
				val=0;
				val=emem[pc+1] + 
					(emem[pc+2]<<8) + 
					(emem[pc+3]<<16) + 
					(emem[pc+4]<<24);
				
				if(zf==of){
				pc=val;	
				}else{
					pc+=5;
				}
				break;
			case 0x76: //jg
				val=0;
				val=emem[pc+1] + 
					(emem[pc+2]<<8) + 
					(emem[pc+3]<<16) + 
					(emem[pc+4]<<24);
				
				if(zf==0&&(sf==of)){
				pc=val;	
				}else{
					pc+=5;
				}
				break;
			case 0x80: //call
				val=0;
				val=emem[pc+1] + 
					(emem[pc+2]<<8) + 
					(emem[pc+3]<<16) + 
					(emem[pc+4]<<24);
				regs[ESP]-=4;
				
				icall=pc+5;
				emem[regs[ESP]]=(icall&0x000000ff);
				emem[regs[ESP]+1]=(icall&0x0000ff00)>>8;
				emem[regs[ESP]+2]=(icall&0x00ff0000)>>16;
				emem[regs[ESP]+3]=(icall&0xff000000)>>24;
				pc=val;
				break;
			case 0x90: //ret
				pc=(emem[regs[ESP]])+
						(emem[regs[ESP]+1]<<8)+
						(emem[regs[ESP]+2]<<16)+
						(emem[regs[ESP]+3]<<24);
							
				regs[ESP]+=4;
				break;
			case 0xa0: //pushl
				r1=emem[pc+1]>>4&0xff;
				r2=emem[pc+1]&0x0f;
				regs[ESP]-=4;
				
				emem[regs[ESP]]=(regs[r1]&0x000000ff);
				emem[regs[ESP]+1]=(regs[r1]&0x0000ff00)>>8;
				emem[regs[ESP]+2]=(regs[r1]&0x00ff0000)>>16;
				emem[regs[ESP]+3]=(regs[r1]&0xff000000)>>24;
				
				pc+=2;
				break;
			case 0xb0: //pop
				r1=emem[pc+1]>>4&0xff;
				r2=emem[pc+1]&0x0f;
				regs[r1]=(emem[regs[ESP]])+
							(emem[regs[ESP]+1]<<8)+
							(emem[regs[ESP]+2]<<16)+
							(emem[regs[ESP]+3]<<24);
							
				regs[ESP]+=4;
				pc+=2;
				break;
			case 0xc0: //readb
				r1=emem[pc+1]>>4&0xff;
				r2=emem[pc+1]&0x0f;
				val=0;
				val=emem[pc+2] + 
					(emem[pc+3]<<8) + 
					(emem[pc+4]<<16) + 
					(emem[pc+5]<<24);
				
				if(EOF==scanf("%c",&(cread))){
					zf=true;
				}else{
					zf=false;
				}
				emem[regs[r1]+val]=cread;
				pc+=6;
				break;
			case 0xc1: //readl
				r1=emem[pc+1]>>4&0xff;
				r2=emem[pc+1]&0x0f;
				val=0;
				val=emem[pc+2] + 
					(emem[pc+3]<<8) + 
					(emem[pc+4]<<16) + 
					(emem[pc+5]<<24);
				
				if(EOF==scanf("%i",&(icall))){
					zf=true;
				}else{
					zf=false;
				}
				
				emem[regs[r1]+val]=(icall&0x000000ff);
				emem[regs[r1]+1+val]=(icall&0x0000ff00)>>8;
				emem[regs[r1]+2+val]=(icall&0x00ff0000)>>16;
				emem[regs[r1]+3+val]=(icall&0xff000000)>>24;
				
				pc+=6;
				break;
			case 0xd0: //writeb
				r1=emem[pc+1]>>4&0xff;
				r2=emem[pc+1]&0x0f;
				val=0;
				val=emem[pc+2] + 
					(emem[pc+3]<<8) + 
					(emem[pc+4]<<16) + 
					(emem[pc+5]<<24);
				
				printf("%c",emem[regs[r1]+val]);
				pc+=6;
				break;
			case 0xd1: //writel
				r1=emem[pc+1]>>4&0xff;
				r2=emem[pc+1]&0x0f;
				val=0;
				int num=0;
				val=emem[pc+2] + 
					(emem[pc+3]<<8) + 
					(emem[pc+4]<<16) + 
					(emem[pc+5]<<24);
				
				num=(emem[regs[r1]+val])+
					(emem[regs[r1]+1+val]<<8)+
					(emem[regs[r1]+2+val]<<16)+
					(emem[regs[r1]+3+val]<<24);
							
				
				
				printf("%i",num);
				pc+=6;
				break;
			case 0xe0: //movsbl
				r1=emem[pc+1]>>4&0xff;
				r2=emem[pc+1]&0x0f;
				val=0;
				val=emem[pc+2] + 
					(emem[pc+3]<<8) + 
					(emem[pc+4]<<16) + 
					(emem[pc+5]<<24);
				int a=0;
				int b=0;
				a=(emem[regs[r2]+val])+
					(emem[regs[r2]+val+1]<<8)+
					(emem[regs[r2]+2]<<16)+
					(emem[regs[r2]+3]<<24);
				b=a&0x000000ff;
					regs[r1]=b;
					pc+=6;
					break;
			default:
				printf("\nExecution stopped at pc = 0x%x. STATUS: INS\n", pc);
				go=false;	
		}

	r1=0;
	r2=0;
	}
	
	
	
	
}

//initializes memory, passes it to runProg to process
int main(int argc, char **argv){
	FILE * input = fopen(argv[1], "r");
	if(strcmp(argv[1],"-h")==0){
		printf("This program takes in a .y86 file and compiles and then executes it\n");
		exit(0);
	}

    char * dir =(char*)malloc(sizeof(char)*10); //first '.' or every line
	char * adr=(char*)malloc(sizeof(char)*10); //where it is stored, second thing
	char * byte=(char*)malloc(sizeof(char)*16);
	char * data=(char*)malloc(sizeof(char)*16); //what is actually stored, what goes into the emem
	char * text;
	int memToAdd=0;

	int err = 0;

	//reads file and puts into memory
	while(fscanf(input,"%1000s",dir)!=EOF){ //should be first one
		err ++;
			//.size
		if(strcmp(".size", dir) == 0 && err == 1){
			
			fscanf(input,"%1000s",dir);
			memToAdd= strtol(dir, NULL, 16);//HexToDec(dir);
			emem=(unsigned char*)malloc(memToAdd*sizeof(unsigned char));
		
		//.string putting string into memory
		} else if (strcmp(".size", dir) == 0 && err != 1){ //error checking for .size since it has to be first
			fprintf(stderr, "ERROR: No .size or its not the first one");
			exit(0);
		}
		else if(strcmp(".string", dir) == 0){

			unsigned char next;
			fscanf(input,"%1000s",adr);

			int loc= strtol(adr, NULL, 16) -2;

			do { //does it until the end of line to get everything
		      next = fgetc (input);
			   
			  if(next!='"'){
			 	 emem[loc]=next;
			  } 
			  if (next == '\n'){
				  break;
			  }
				loc++;  
		    } while (next != '\n');
		
		//.long putting a long value into memory
		}else if(strcmp(".long", dir) == 0){
				
			fscanf(input,"%1000s",adr);
			long loc = strtol(adr, NULL, 16);
			fscanf(input,"%1000s",data);
			long tmp=atoi(data);
			longh.num=tmp;
			emem[loc]=longh.byte[0];
		
		//.byte putting bytes into memory
		}else if(strcmp(".byte",dir) == 0){
			
		fscanf(input,"%1000s",adr);
		long loc = strtol(adr, NULL, 16);
		fscanf(input,"%1000s",data);
		byte[0]=data[1];
		byte[1]='\0';
		storeByte.half.upper=strtol(byte, NULL, 16);
		byte[0]=data[0];
		byte[1]='\0';
		storeByte.half.lower=strtol(byte, NULL, 16);
		emem[loc]=storeByte.overbyte;
		
		//.text putting the text into memory
		}else if(strcmp(".text", dir) == 0){
			text=( char*)malloc(memToAdd*sizeof( char));
			fscanf(input,"%1000s",adr);
			fscanf(input,"%1000s",text);
			int loc = strtol(adr, NULL, 16);
			pc= loc;
			int j=0;
			int len=strlen(text);
			while(j<len){
				byte[0]=text[j+1];
				byte[1]='\0';
				
				storeByte.half.upper=strtol(byte, NULL, 16);
				byte[0]=text[j+0];
				byte[1]='\0';
				storeByte.half.lower=strtol(byte, NULL, 16);
				
				emem[loc]=storeByte.overbyte;
				
				j+=2;
				loc++;
			}
			last=loc;
		}
	}
	runProg();
	
	fclose(input);
	free(emem);
	
	return 0;
}





