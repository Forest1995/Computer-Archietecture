/*
	Name 1: Yuesen Lu
	Name 2: Wencan Liu
	UTEID 1: yl33489
	UTEID 2: wl8784
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct {

	int PC,		/* program counter */
		N,		/* n condition bit */
		Z,		/* z condition bit */
		P;		/* p condition bit */
	int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {
	printf("----------------LC-3b ISIM Help-----------------------\n");
	printf("go               -  run program to completion         \n");
	printf("run n            -  execute program for n instructions\n");
	printf("mdump low high   -  dump memory from low to high      \n");
	printf("rdump            -  dump the register & bus values    \n");
	printf("?                -  display this help menu            \n");
	printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {

	process_instruction();
	CURRENT_LATCHES = NEXT_LATCHES;
	INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
	int i;

	if (RUN_BIT == FALSE) {
		printf("Can't simulate, Simulator is halted\n\n");
		return;
	}

	printf("Simulating for %d cycles...\n\n", num_cycles);
	for (i = 0; i < num_cycles; i++) {
		if (CURRENT_LATCHES.PC == 0x0000) {
			RUN_BIT = FALSE;
			printf("Simulator halted\n\n");
			break;
		}
		cycle();
	}
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {
	if (RUN_BIT == FALSE) {
		printf("Can't simulate, Simulator is halted\n\n");
		return;
	}

	printf("Simulating...\n\n");
	while (CURRENT_LATCHES.PC != 0x0000)
		cycle();
	RUN_BIT = FALSE;
	printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {
	int address; /* this is a byte address */

	printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
	printf("-------------------------------------\n");
	for (address = (start >> 1); address <= (stop >> 1); address++)
		printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
	printf("\n");

	/* dump the memory contents into the dumpsim file */
	fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
	fprintf(dumpsim_file, "-------------------------------------\n");
	for (address = (start >> 1); address <= (stop >> 1); address++)
		fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
	fprintf(dumpsim_file, "\n");
	fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {
	int k;

	printf("\nCurrent register/bus values :\n");
	printf("-------------------------------------\n");
	printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
	printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
	printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
	printf("Registers:\n");
	for (k = 0; k < LC_3b_REGS; k++)
		printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
	printf("\n");

	/* dump the state information into the dumpsim file */
	fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
	fprintf(dumpsim_file, "-------------------------------------\n");
	fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
	fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
	fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
	fprintf(dumpsim_file, "Registers:\n");
	for (k = 0; k < LC_3b_REGS; k++)
		fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
	fprintf(dumpsim_file, "\n");
	fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {
	char buffer[20];
	int start, stop, cycles;

	printf("LC-3b-SIM> ");

	scanf("%s", buffer);
	printf("\n");

	switch (buffer[0]) {
	case 'G':
	case 'g':
		go();
		break;

	case 'M':
	case 'm':
		scanf("%i %i", &start, &stop);
		mdump(dumpsim_file, start, stop);
		break;

	case '?':
		help();
		break;
	case 'Q':
	case 'q':
		printf("Bye.\n");
		exit(0);

	case 'R':
	case 'r':
		if (buffer[1] == 'd' || buffer[1] == 'D')
			rdump(dumpsim_file);
		else {
			scanf("%d", &cycles);
			run(cycles);
		}
		break;

	default:
		printf("Invalid Command\n");
		break;
	}
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {
	int i;

	for (i = 0; i < WORDS_IN_MEM; i++) {
		MEMORY[i][0] = 0;
		MEMORY[i][1] = 0;
	}
       // MEMORY[0x2000][0]=0x1a;
        //MEMORY[0x2000][1]=0x2b;
        //MEMORY[0x2001][0]=0x3c;
        //MEMORY[0x2001][1]=0X4d;
        //MEMORY[0x2002][0]=0xe4;
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {
	FILE * prog;
	int ii, word, program_base;

	/* Open program file. */
	prog = fopen(program_filename, "r");
	if (prog == NULL) {
		printf("Error: Can't open program file %s\n", program_filename);
		exit(-1);
	}

	/* Read in the program. */
	if (fscanf(prog, "%x\n", &word) != EOF)
		program_base = word >> 1;
	else {
		printf("Error: Program file is empty\n");
		exit(-1);
	}

	ii = 0;
	while (fscanf(prog, "%x\n", &word) != EOF) {
		/* Make sure it fits. */
		if (program_base + ii >= WORDS_IN_MEM) {
			printf("Error: Program file %s is too long to fit in memory. %x\n",
				program_filename, ii);
			exit(-1);
		}

		/* Write the word to memory array. */
		MEMORY[program_base + ii][0] = word & 0x00FF;
		MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
		ii++;
	}

	if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

	printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) {
	int i;

	init_memory();
	for (i = 0; i < num_prog_files; i++) {
		load_program(program_filename);
		while (*program_filename++ != '\0');
	}
	CURRENT_LATCHES.Z = 1;
	NEXT_LATCHES = CURRENT_LATCHES;

	RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
	FILE * dumpsim_file;

	/* Error Checking */
	if (argc < 2) {
		printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
			argv[0]);
		exit(1);
	}

	printf("LC-3b Simulator\n\n");

	initialize(argv[1], argc - 1);

	if ((dumpsim_file = fopen("dumpsim", "w")) == NULL) {
		printf("Error: Can't open dumpsim file\n");
		exit(-1);
	}

	while (1)
		get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

   /***************************************************************/
void SETNZP(int num)
{
	int num16 = num & 0x8000;
	if (num == 0)
	{
		NEXT_LATCHES.N = 0;
		NEXT_LATCHES.Z = 1;
		NEXT_LATCHES.P = 0;
	}
	else if (num16 == 0)
	{
		NEXT_LATCHES.N = 0;
		NEXT_LATCHES.Z = 0;
		NEXT_LATCHES.P = 1;
	}
	else
	{
		NEXT_LATCHES.N = 1;
		NEXT_LATCHES.Z = 0;
		NEXT_LATCHES.P = 0;
	}
}

int fetch() {
	int instruction = MEMORY[CURRENT_LATCHES.PC >> 1][0] & 0x00FF;
	instruction |= (MEMORY[CURRENT_LATCHES.PC >> 1][1] & 0x00FF) << 8;
	instruction = Low16bits(instruction);
	return(instruction);
}
void process_instruction() {
	/*  function: process_instruction
	 *
	 *    Process one instruction at a time
	 *       -Fetch one instruction
	 *       -Decode
	 *       -Execute
	 *       -Update NEXT_LATCHES
	 */
	int machinecode;
	int opcode;
	int sr1, sr2, dr;
	int PCoffset9;
	machinecode = fetch();
        printf("Entered process instruction with machine code of %x\n",machinecode);
	opcode = (machinecode >> 12) & 0xf;
	//add
	if (opcode == 0x1)
	{
	        sr1=(machinecode>>6)&0x7;
                dr=(machinecode>>9)&0x7;
          	if ((machinecode & 0x20) == 0)
		{
			sr2 = machinecode & 0x7;
			NEXT_LATCHES.REGS[dr] = (CURRENT_LATCHES.REGS[sr1] + CURRENT_LATCHES.REGS[sr2]) & 0xffff;
		      //  printf("%x",NEXT_LATCHES.REGS[dr]);
                 	SETNZP(NEXT_LATCHES.REGS[dr]);
		}
		else
		{
			NEXT_LATCHES.REGS[dr] = (((machinecode & 0x10)?((machinecode&0x1f)|0xffe0):(machinecode&0x1f)) + CURRENT_LATCHES.REGS[sr1]) & 0xffff;
			SETNZP(NEXT_LATCHES.REGS[dr]);
		}
		NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
	}
	//and
	if (opcode == 0x5)
	{
                sr1=(machinecode>>6)&0x7;
                dr=(machinecode>>9)&0x7;
		if ((machinecode & 0x20) == 0)
		{
		      // printf("ipoi");
                 	sr2 = machinecode & 0x7;
			NEXT_LATCHES.REGS[dr] = (CURRENT_LATCHES.REGS[sr1] & CURRENT_LATCHES.REGS[sr2]) & 0xffff;
			SETNZP(NEXT_LATCHES.REGS[dr]);
		}
		else
		{
		      // printf("fda");
                       
            NEXT_LATCHES.REGS[dr] = (((machinecode & 0x10)?((machinecode&0x1f)|0xffe0):(machinecode&0x1f)) & CURRENT_LATCHES.REGS[sr1]) & 0xffff;
			SETNZP(NEXT_LATCHES.REGS[dr]);
		}
		NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
	}
	//br
	if (opcode == 0x0)
	{
		if ((machinecode & 0x0800) && (CURRENT_LATCHES.N == 1) || (machinecode & 0x0400) && (CURRENT_LATCHES.Z == 1) || (machinecode & 0x0200) && (CURRENT_LATCHES.P == 1))
		{
			int PCoffset9 = (machinecode & 0x100) ? ((machinecode & 0x1ff) | 0xfe00) : (machinecode & 0x1ff);
			NEXT_LATCHES.PC = (CURRENT_LATCHES.PC + 2 + (PCoffset9<<1)) & 0xffff;
		}
		else NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
	}

	//halt
	if (opcode == 0xf && (machinecode & 0xff) == 0x25)
		NEXT_LATCHES.PC = 0;
	//jmp
	if (opcode == 0xc)
	{
		int baser = (machinecode >> 6) & 0x7;
		NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[baser];
	}
	//jsr jsrr
	if (opcode == 0x4)
	{
		if ((machinecode & 0x0800) == 0)
		{
			int baser = (machinecode >> 6) & 0x7;
			NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[baser];
		}
		else
		{
			int PCoffset11 = (machinecode & 0x400) ? ((machinecode & 0x7ff) | 0xf800) : (machinecode & 0x7ff);
			NEXT_LATCHES.PC = (CURRENT_LATCHES.PC + 2 + (PCoffset11<<1)) & 0xffff;
		}
		NEXT_LATCHES.REGS[7] = CURRENT_LATCHES.PC + 2;
	}
	//ldb
	if (opcode == 0x2)
	{
                NEXT_LATCHES.PC=CURRENT_LATCHES.PC+2;
		int baser = (machinecode >> 6) & 0x7;
		int dr = (machinecode >> 9) & 0x7;
		int boffset6 = (machinecode & 0x20) ? ((machinecode & 0x3f) | 0xffc0) : (machinecode & 0x3f);
		int load = MEMORY[((CURRENT_LATCHES.REGS[baser] + boffset6) & 0xfffF)>>1][(CURRENT_LATCHES.REGS[baser]+boffset6)&0x1];
               // printf("%x",load);		
        NEXT_LATCHES.REGS[dr] = (load & 0x80) ? ((load & 0xff) | 0xff00) : (load& 0xff);
		SETNZP(NEXT_LATCHES.REGS[dr]);
	}
	//ldw
	if (opcode == 0x6) {
                NEXT_LATCHES.PC=CURRENT_LATCHES.PC+2; 
               // printf("entered ldw\n");
		int baser = (machinecode >> 6) & 0x7;
		int dr = (machinecode >> 9) & 0x7;
		int boffset6 = (machinecode & 0x20) ? ((machinecode & 0x3f) | 0xffa0) : (machinecode & 0x3f);
		boffset6 = boffset6 << 1;
		boffset6 = Low16bits(boffset6);
		int base = Low16bits(CURRENT_LATCHES.REGS[baser]);
		base += boffset6;
            //    printf("bae is %x",base);
		int load =(MEMORY[Low16bits(base) >> 1][0]) & 0x00FF;
                //int load=MEMORY[Low16bits(base)]&0x00FF;     
             //   printf("load is %x",load);
		load |= ((MEMORY[Low16bits(base) >> 1][1]) & 0x00FF) << 8;
               //load |=(MEMORY[Low16bits(base+1)]&0x00ff)<<8;
              //   printf("load is %x",load);
		NEXT_LATCHES.REGS[dr] = Low16bits(load);
		SETNZP(NEXT_LATCHES.REGS[dr]);
	}
	//lea
	if (opcode == 0x0e) {
             //   printf("Entered 0e:");
                NEXT_LATCHES.PC=CURRENT_LATCHES.PC+2;
		int dr = (machinecode >> 9) & 0x7;
		int PCoffset9 = (machinecode & 0x100) ? ((machinecode & 0x1ff) | 0xfe00) : (machinecode & 0x1ff);
		PCoffset9 = PCoffset9 << 1;
		PCoffset9 = Low16bits(PCoffset9);
		int load = PCoffset9 + NEXT_LATCHES.PC;
		NEXT_LATCHES.REGS[dr] = Low16bits(load);
		//SETNZP(NEXT_LATCHES.REGS[dr]);
	}

	//shf
	if (opcode == 0x0d) {
                NEXT_LATCHES.PC=CURRENT_LATCHES.PC+2;
		int sr = (machinecode >> 6) & 0x7;
		int dr = (machinecode >> 9) & 0x7;
		int load = Low16bits(CURRENT_LATCHES.REGS[sr]);
		int amount = machinecode & 0x0F;
		switch ((machinecode >> 4) & 0x03) {
		case 0: load = load << (amount); break;
		case 1: load = load >> (amount); break;
		case 3: load = (load & 0x8000) ? ((load & 0xffff) | 0xffff0000) : (load & 0xffff); load = load >> (amount); break;
		}
		NEXT_LATCHES.REGS[dr] = Low16bits(load);
		SETNZP(NEXT_LATCHES.REGS[dr]);
	}
	//stb
	if (opcode == 0x03) {
		NEXT_LATCHES.PC=CURRENT_LATCHES.PC+2;
                int sr = (machinecode >> 9) & 0x7;
		int baser = (machinecode >> 6) & 0x7;
		int offset6 = (machinecode & 0x20) ? ((machinecode & 0x3f) | 0xffc0) : (machinecode & 0x3f);
		int base = Low16bits(CURRENT_LATCHES.REGS[baser]);
		base +=offset6;
               int base0=base&0x1;
              // printf("%x\n",base0);
            //   printf("%x\n",base>>1);
		MEMORY[base >> 1][base0] = CURRENT_LATCHES.REGS[sr] & 0x00FF;
        
    
	}
	//stw
	if (opcode == 0x07) {
                NEXT_LATCHES.PC=CURRENT_LATCHES.PC+2;
		int sr = (machinecode >> 9) & 0x7;
		int baser = (machinecode >> 6) & 0x7;
		int offset6 = (machinecode & 0x20) ? ((machinecode & 0x3f) | 0xffc0) : (machinecode & 0x3f);
		offset6=offset6 << 1;
		int base = Low16bits(CURRENT_LATCHES.REGS[baser]);
		base += offset6;
		MEMORY[Low16bits(base)>> 1][0] = CURRENT_LATCHES.REGS[sr] & 0x00FF;
		MEMORY[Low16bits(base) >> 1][1] = (CURRENT_LATCHES.REGS[sr] >> 8) & 0x00FF;
	}
	//trap
	if (opcode == 0x0f) {
		NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
		NEXT_LATCHES.REGS[7] = Low16bits(NEXT_LATCHES.PC);
		int trapvector = Low16bits((machinecode & 0x00FF) << 1);
		int address = MEMORY[trapvector >> 1][0] & 0x00FF;
		address |= (MEMORY[trapvector >> 1][1] & 0x00FF) << 8;
		NEXT_LATCHES.PC = address;
	}
	//xor
	if (opcode == 0x09) {
                NEXT_LATCHES.PC=CURRENT_LATCHES.PC+2;
		int dr = (machinecode >> 9) & 0x07;
		int sr1 = (machinecode >> 6) & 0x07;
		int load = Low16bits(CURRENT_LATCHES.REGS[sr1]);
		if (machinecode & 0x20)
			load ^= (machinecode & 0x10) ? ((machinecode & 0x1f) | 0xffe0) : (machinecode & 0x1f);
		else {
			int sr2 = machinecode & 0x07;
			load ^= Low16bits(CURRENT_LATCHES.REGS[sr2]);
		}
		NEXT_LATCHES.REGS[dr] = Low16bits(load);
		SETNZP(NEXT_LATCHES.REGS[dr]);
	}


}
