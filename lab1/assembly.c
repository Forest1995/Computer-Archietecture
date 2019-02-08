/* Lab 1 for EE382N.1 Computer Architecture */

/*This program is an LC-3b Assembler, whose job is to translate assembly
language source code into the machine language (ISA) of the LC-3b.*/

/*
Name 1: Yuesen Lu
UTEID 1: YL33489
Name 2: Wencan Liu
UTEID 2: wl8784
*/

#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

#define MAX_LINE_LENGTH 255
#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255

/*construct a structure*/
typedef struct {
	int address;
	char label[MAX_LABEL_LEN + 1];     /* Question for the reader: Why do weneed to add 1? */
} TableEntry;

TableEntry symbolTable[MAX_SYMBOLS];


enum
{
	DONE, OK, EMPTY_LINE
};


/*Convert a String To a Number*/
int toNum(char * pStr) {
	char * t_ptr;
	char * orig_pStr;
	int t_length, k;
	int lNum, lNeg = 0;
	long int lNumLong;

	orig_pStr = pStr;
	if (*pStr == '#')                                /* decimal */
	{
		pStr++;
		if (*pStr == '-')                           /* dec is negative */
		{
			lNeg = 1;
			pStr++;
		}
		t_ptr = pStr;
		t_length = strlen(t_ptr);
		for (k = 0; k < t_length; k++)
		{
			if (!isdigit(*t_ptr))
			{
				printf("Error: invalid decimal operand, %s\n", orig_pStr);
				exit(4);
			}
			t_ptr++;
		}
		lNum = atoi(pStr);
		if (lNeg)
			lNum = -lNum;

		return lNum;
	}
	else if (*pStr == 'x')        /* hex     */
	{
		pStr++;
		if (*pStr == '-')                                /* hex is negative */
		{
			lNeg = 1;
			pStr++;
		}
		t_ptr = pStr;
		t_length = strlen(t_ptr);
		for (k = 0; k < t_length; k++)
		{
			if (!isxdigit(*t_ptr))
			{
				printf("Error: invalid hex operand, %s\n", orig_pStr);
				exit(4);
			}
			t_ptr++;
		}
		lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
		lNum = (lNumLong > INT_MAX) ? INT_MAX : lNumLong;
		if (lNeg)
			lNum = -lNum;
		return lNum;
	}
	else
	{
		printf("Error: invalid operand, %s\n", orig_pStr);
		exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
	}
}

/*Take a line of the input file and parse it into corresponding fields.*/
int readAndParse(FILE * pInfile, char * pLine, char ** pLabel, char
	** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4)
{
	/*char * lRet;*/
	char * lPtr;
	unsigned int i;
	if (!fgets(pLine, MAX_LINE_LENGTH, pInfile))
		return(DONE);
	for (i = 0; i < strlen(pLine); i++)
		pLine[i] = tolower(pLine[i]);    /* convert entire line to lowercase */
	*pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine); /* ignore the comments */
	lPtr = pLine;

	while (*lPtr != ';' && *lPtr != '\0' &&
		*lPtr != '\n')
		lPtr++;

	*lPtr = '\0';
	if (!(lPtr = strtok(pLine, "\t\n ,")))
		return(EMPTY_LINE);

	if (isOpcode(lPtr) == -1 && lPtr[0] != '.') /* found a label */
	{
		*pLabel = lPtr;
		if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);
	}

	*pOpcode = lPtr;
	if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);
	*pArg1 = lPtr;
	if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);
	*pArg2 = lPtr;
	if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);
	*pArg3 = lPtr;
	if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);
	*pArg4 = lPtr;

	return(OK);
}/* Note: MAX_LINE_LENGTH, OK, EMPTY_LINE, and DONE are defined values */

/*The First Pass: Creating the Symbol Table*/
int first_pass(FILE* lInfile) {
	char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
	int lRet;
	int AddressCounter = 0;
	int NumberOfSymbol = 0;
	int orginal_address = 0;
	int i = 0;
	int flag = 0;
	int flag2 = 0;
	do
	{
		lRet = readAndParse(lInfile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
		if (lRet != DONE && lRet != EMPTY_LINE)
		{
			if (!strcmp(lOpcode, ".orig"))
			{
				AddressCounter = toNum(lArg1) - 2;
				orginal_address = AddressCounter;
				flag = 1;
			}
			if (strcmp(lOpcode, ".orig") && flag == 0)
			{
				exit(4);
			}
			if (*lLabel)
			{
				for (i = 0; (i < MAX_LABEL_LEN && lLabel[i] != '\0'); i++)
					if (!isalnum(lLabel[i])) {
						exit(4);
					}
				for (i = 0; i < NumberOfSymbol; i++)
					if (!strcmp(lLabel, symbolTable[i].label)) {
						exit(4);
					}
				if (!strcmp(lLabel, "r0") || !strcmp(lLabel, "r1") || !strcmp(lLabel, "r2") || !strcmp(lLabel, "r3") || !strcmp(lLabel, "r4") || !strcmp(lLabel, "r5") || !strcmp(lLabel, "r6") || !strcmp(lLabel, "r7"))
					exit(4);
				strncpy(symbolTable[NumberOfSymbol].label, lLabel, MAX_LABEL_LEN);
				symbolTable[NumberOfSymbol].address = AddressCounter;
				NumberOfSymbol++;
				if (NumberOfSymbol >= 256)
				{
					exit(4);
				}
			}
			AddressCounter = AddressCounter + 2;
			if (!strcmp(lOpcode, ".end"))
			{
				flag2 = 1;
			}
		}
	} while (lRet != DONE);
	if (flag2 == 0)
		exit(4);
	else return (orginal_address);
}
/*check whether the string is an opcode*/
int isOpcode(char* opcode) {
	if (!strcmp(opcode, "add") || !strcmp(opcode, "and") ||
		!strcmp(opcode, "br") || !strcmp(opcode, "brn") ||
		!strcmp(opcode, "brp") || !strcmp(opcode, "brz") ||
		!strcmp(opcode, "brzp") || !strcmp(opcode, "brnp") ||
		!strcmp(opcode, "brnz") || !strcmp(opcode, "brnzp") ||
		!strcmp(opcode, "jmp") || !strcmp(opcode, "ret") ||
		!strcmp(opcode, "jsr") || !strcmp(opcode, "jsrr") ||
		!strcmp(opcode, "ldb") || !strcmp(opcode, "ldw") ||
		!strcmp(opcode, "lea") || !strcmp(opcode, "not") ||
		!strcmp(opcode, "rti") || !strcmp(opcode, "lshf") ||
		!strcmp(opcode, "rshfl") || !strcmp(opcode, "rshfa") ||
		!strcmp(opcode, "stb") || !strcmp(opcode, "stw") ||
		!strcmp(opcode, "trap") || !strcmp(opcode, "xor") ||
		!strcmp(opcode, "nop") || !strcmp(opcode, "halt") ||
		!strcmp(opcode, ".orig") || !strcmp(opcode, ".fill")) {
		return 1;
	}
	else {
		return -1;
	}
}
int registerToNum(char* reg) {
	if (strcmp(reg, "r0") == 0)
		return 0;
	if (strcmp(reg, "r1") == 0)
		return 1;
	if (strcmp(reg, "r2") == 0)
		return 2;
	if (strcmp(reg, "r3") == 0)
		return 3;
	if (strcmp(reg, "r4") == 0)
		return 4;
	if (strcmp(reg, "r5") == 0)
		return 5;
	if (strcmp(reg, "r6") == 0)
		return 6;
	if (strcmp(reg, "r7") == 0)
		return 7;
	exit(4);
}
/*To get the address of label from symbol table*/
int GetLabelAddress(char * Label) {
	int OrderOfSymbols = 0;
	for (OrderOfSymbols = 0; OrderOfSymbols < MAX_LINE_LENGTH; OrderOfSymbols++) {
		if (!strcmp(symbolTable[OrderOfSymbols].label, Label)) {
			return symbolTable[OrderOfSymbols].address;
		}
	}
	return 0;
}

int translate(char *opcode, char *arg1, char*arg2, char *arg3, char *arg4, int PC)
{
	if (!strcmp(opcode, "add")) {
		if (!*arg1 || !*arg2 || !*arg3 || *arg4)
			exit(4);
		int machinecode = 0x1000;
		machinecode += registerToNum(arg1) << 9;
		machinecode += registerToNum(arg2) << 6;
		if (arg3[0] == 'r')
			machinecode += registerToNum(arg3);
		else {
			machinecode += 1 << 5;
			int imm = toNum(arg3);
			if ((imm > 15) || (imm < -16)) exit(3);
			imm &= 31;
			machinecode += imm;
		}
		return(machinecode);
	}
	if (!strcmp(opcode, "and")) {
		if (!*arg1 || !*arg2 || !*arg3 || *arg4)
			exit(4);
		int machinecode = 0x5000;
		machinecode += registerToNum(arg1) << 9;
		machinecode += registerToNum(arg2) << 6;
		if (arg3[0] == 'r')
			machinecode += registerToNum(arg3);
		else {
			machinecode += 1 << 5;
			int imm = toNum(arg3);
			if ((imm > 15) || (imm < -16))
				exit(3);
			imm &= 31;
			machinecode += imm;
		}
		return(machinecode);
	}
	if (!strcmp(opcode, "br") || !strcmp(opcode, "brnzp")) {
		if (!*arg1 || *arg2 || *arg3 || *arg4)
			exit(4);
		int machinecode = 0x0e00;
		int BrAddress = GetLabelAddress(arg1);
		if (!BrAddress) exit(1);
		else {
			int PCinc = PC + 2;
			int offset = (BrAddress - PCinc) / 2;
			if ((offset > 255) || (offset < -256)) exit(4);
			offset &= 0x1ff;
			machinecode += offset;
			return(machinecode);
		}
	}
	if (!strcmp(opcode, "brp")) {
		if (!*arg1 || *arg2 || *arg3 || *arg4)
			exit(4);
		int machinecode = 0x0200;
		int BrAddress = GetLabelAddress(arg1);
		if (!BrAddress) exit(1);
		else {
			int PCinc = PC + 2;
			int offset = (BrAddress - PCinc) / 2;
			if ((offset > 255) || (offset < -256)) exit(4);
			offset &= 0x1ff;
			machinecode += offset;
			return(machinecode);
		}
	}
	if (!strcmp(opcode, "brz")) {
		if (!*arg1 || *arg2 || *arg3 || *arg4)
			exit(4);
		int machinecode = 0x0400;
		int BrAddress = GetLabelAddress(arg1);
		if (!BrAddress) exit(1);
		else {
			int PCinc = PC + 2;
			int offset = (BrAddress - PCinc) / 2;
			if ((offset > 255) || (offset < -256)) exit(4);
			offset &= 0x1ff;
			machinecode += offset;
			return(machinecode);
		}
	}
	if (!strcmp(opcode, "brzp")) {
		if (!*arg1 || *arg2 || *arg3 || *arg4)
			exit(4);
		int machinecode = 0x0600;
		int BrAddress = GetLabelAddress(arg1);
		if (!BrAddress) exit(1);
		else {
			int PCinc = PC + 2;
			int offset = (BrAddress - PCinc) / 2;
			if ((offset > 255) || (offset < -256)) exit(4);
			offset &= 0x1ff;
			machinecode += offset;
			return(machinecode);
		}
	}
	if (!strcmp(opcode, "brn")) {
		if (!*arg1 || *arg2 || *arg3 || *arg4)
			exit(4);
		int machinecode = 0x0800;
		int BrAddress = GetLabelAddress(arg1);
		if (!BrAddress) exit(1);
		else {
			int PCinc = PC + 2;
			int offset = (BrAddress - PCinc) / 2;
			if ((offset > 255) || (offset < -256)) exit(4);
			offset &= 0x1ff;
			machinecode += offset;
			return(machinecode);
		}
	}

	if (!strcmp(opcode, "brnp")) {
		if (!*arg1 || *arg2 || *arg3 || *arg4)
			exit(4);
		int machinecode = 0x0a00;
		int BrAddress = GetLabelAddress(arg1);
		if (!BrAddress) exit(1);
		else {
			int PCinc = PC + 2;
			int offset = (BrAddress - PCinc) / 2;
			if ((offset > 255) || (offset < -256)) exit(4);
			offset &= 0x1ff;
			machinecode += offset;
			return(machinecode);
		}
	}

	if (!strcmp(opcode, "brnz")) {
		if (!*arg1 || *arg2 || *arg3 || *arg4)
			exit(4);
		int machinecode = 0x0c00;
		int BrAddress = GetLabelAddress(arg1);
		if (!BrAddress) exit(1);
		else {
			int PCinc = PC + 2;
			int offset = (BrAddress - PCinc) / 2;
			if ((offset > 255) || (offset < -256)) exit(4);
			offset &= 0x1ff;
			machinecode += offset;
			return(machinecode);
		}
	}
	if (!strcmp(opcode, "jmp"))
	{
		if (!*arg1 || *arg2 || *arg3 || *arg4)
			exit(4);
		int machinecode = 0xC000;
		machinecode += registerToNum(arg1) << 6;
		return(machinecode);
	}
	if (!strcmp(opcode, "ret")) {
		if (*arg1 || *arg2 || *arg3 || *arg4) exit(4);
		return(0xc1c0);
	}
	if (!strcmp(opcode, "jsr"))
	{
		if (!*arg1 || *arg2 || *arg3 || *arg4)
			exit(4);
		int machinecode = 0x4800;
		int BrAddress = GetLabelAddress(arg1);
		if (!BrAddress) exit(1);
		else {
			int PCinc = PC + 2;
			int offset = (BrAddress - PCinc) / 2;
			if ((offset > 1023) || (offset < -1024)) exit(4);
			offset &= 0x07ff;
			machinecode += offset;
			return(machinecode);
		}
	}
	if (!strcmp(opcode, "jsrr")) {
		if (!*arg1 || *arg2 || *arg3 || *arg4) exit(4);
		int machinecode = 0x4000;
		machinecode += registerToNum(arg1) << 6;
		return(machinecode);
	}
	if (!strcmp(opcode, "ldb"))
	{
		if (!*arg1 || !*arg2 || !*arg3 || *arg4)
			exit(4);
		int machinecode = 0x2000;
		int boffset6 = toNum(arg3);
		if (-32 <= boffset6 && boffset6 <= 31)
		{
			machinecode += registerToNum(arg1) << 9;
			machinecode += registerToNum(arg2) << 6;
			boffset6 &= 0x3f;
			machinecode += boffset6;
			return(machinecode);
		}
		else exit(3);
	}
	if (!strcmp(opcode, "ldw"))
	{
		if (!*arg1 || !*arg2 || !*arg3 || *arg4)
			exit(4);
		int machinecode = 0x6000;
		int offset6 = toNum(arg3);
		if (-32 <= offset6 && offset6 <= 31)
		{
			machinecode += registerToNum(arg1) << 9;
			machinecode += registerToNum(arg2) << 6;
			offset6 &= 0x3f;
			machinecode += offset6;
			return(machinecode);
		}
		else exit(3);
	}
	if (!strcmp(opcode, "lea"))
	{
		if (!*arg1 || !*arg2 || *arg3 || *arg4)
			exit(4);
		int machinecode = 0xE000;
		int BrAddress = GetLabelAddress(arg2);
		if (!BrAddress) exit(1);
		else {
			int PCinc = PC + 2;
			int offset = (BrAddress - PCinc) / 2;
			if ((offset > 255) || (offset < -256)) exit(3);
			offset &= 0x1ff;
			machinecode += offset;
			machinecode += registerToNum(arg1) << 9;
			return(machinecode);
		}
	}
	if (!strcmp(opcode, "rti"))
	{
		if (*arg1 || *arg2 || *arg3 || *arg4)
			exit(4);
		return(0x8000);
	}
	if (!strcmp(opcode, "lshf"))
	{
		if (!*arg1 || !*arg2 || !*arg3 || *arg4)
			exit(4);
		int machinecode = 0xD000;
		machinecode += registerToNum(arg1) << 9;
		machinecode += registerToNum(arg2) << 6;
		int lshfbits = toNum(arg3);
		if ((lshfbits > 15) || lshfbits < 0) exit(3);
		machinecode += lshfbits;
		return(machinecode);
	}
	if (!strcmp(opcode, "rshfl"))
	{
		if (!*arg1 || !*arg2 || !*arg3 || *arg4)
			exit(4);
		int machinecode = 0xD010;
		machinecode += registerToNum(arg1) << 9;
		machinecode += registerToNum(arg2) << 6;
		int rshfbits = toNum(arg3);
		if ((rshfbits > 15) || rshfbits < 0) exit(3);
		machinecode += rshfbits;
		return(machinecode);
	}
	if (!strcmp(opcode, "rshfa"))
	{
		if (!*arg1 || !*arg2 || !*arg3 || *arg4)
			exit(4);
		int machinecode = 0xD030;
		machinecode += registerToNum(arg1) << 9;
		machinecode += registerToNum(arg2) << 6;
		int rshfbits = toNum(arg3);
		if ((rshfbits > 15) || rshfbits < 0) exit(3);
		machinecode += rshfbits;
		return(machinecode);
	}
	if (!strcmp(opcode, "stb"))
	{
		if (!*arg1 || !*arg2 || !*arg3 || *arg4)
			exit(4);
		int boffset6 = toNum(arg3);
		if (-32 <= boffset6 && boffset6 <= 31)
		{
			int machinecode = 0x3000;
			machinecode += registerToNum(arg1) << 9;
			machinecode += registerToNum(arg2) << 6;
			boffset6 &= 0x3f;
			machinecode += boffset6;
			return(machinecode);
		}
		else exit(3);
	}
	if (!strcmp(opcode, "stw"))
	{
		if (!*arg1 || !*arg2 || !*arg3 || *arg4)
			exit(4);
		int offset6 = toNum(arg3);
		if (-32 <= offset6 && offset6 <= 31)
		{
			int machinecode = 0x7000;
			machinecode += registerToNum(arg1) << 9;
			machinecode += registerToNum(arg2) << 6;
			offset6 &= 0x3f;
			machinecode += offset6;
			return(machinecode);
		}
		else exit(3);
	}
	if (!strcmp(opcode, "trap"))
	{
		char * t_ptr;
		char * orig_pStr;
		int t_length, k;
		int lNum, lNeg = 0;
		long int lNumLong;
		orig_pStr = arg1;
		if (!*arg1 || *arg2 || *arg3 || *arg4)
			exit(4);
		if (*arg1 == 'x')        /* hex     */
		{
			arg1++;
			if (*arg1 == '-')                                /* hex is negative */
			{
				lNeg = 1;
				arg1++;
			}
			t_ptr = arg1;
			t_length = strlen(t_ptr);
			for (k = 0; k < t_length; k++)
			{
				if (!isxdigit(*t_ptr))
				{
					printf("Error: invalid hex operand, %s\n", orig_pStr);
					exit(4);
				}
				t_ptr++;
			}
			lNumLong = strtol(arg1, NULL, 16);    /* convert hex string into integer */
			lNum = (lNumLong > INT_MAX) ? INT_MAX : lNumLong;
			if (lNeg)
				lNum = -lNum;
			if (lNum >= 0x00 && lNum <= 0xFF)
			{
				int machinecode = 0xF000;
				machinecode += lNum;
				return(machinecode);
			}
			else exit(3);
		}
		else exit(4);
	}
	if (!strcmp(opcode, "halt"))
	{
		if (*arg1 || *arg2 || *arg3 || *arg4)
			exit(4);
		int machinecode = 0xF025;
		return(machinecode);
	}
	if (!strcmp(opcode, "nop"))
	{
		if (*arg1 || *arg2 || *arg3 || *arg4)
			exit(4);
		int machinecode = 0x0000;
		return(machinecode);
	}
	if (!strcmp(opcode, "xor"))
	{
		if (!*arg1 || !*arg2 || !*arg3 || *arg4)
			exit(4);
		int machinecode = 0x9000;
		if (arg3[0] == 'r')
		{
			machinecode += registerToNum(arg1) << 9;
			machinecode += registerToNum(arg2) << 6;
			machinecode += registerToNum(arg3);
		}
		else
		{
			int imm5 = toNum(arg3);
			if (-16 <= imm5 && imm5 <= 15)
			{
				machinecode += registerToNum(arg1) << 9;
				machinecode += registerToNum(arg2) << 6;
				machinecode += 1 << 5;
				imm5 &= 0x1f;
				machinecode += imm5;
			}
			else exit(3);
		}
		return(machinecode);
	}
	if (!strcmp(opcode, "not"))
	{
		if (!*arg1 || !*arg2 || *arg3 || *arg4)
			exit(4);
		int machinecode = 0x9000;
		machinecode += registerToNum(arg1) << 9;
		machinecode += registerToNum(arg2) << 6;
		machinecode += 0x3F;
		return(machinecode);
	}
	if (!strcmp(opcode, ".orig"))
	{
		if (!*arg1 || *arg2 || *arg3 || *arg4)
			exit(4);
		int begin = toNum(arg1);
		int machinecode;
		if (0x0000 <= begin && begin <= 0xFFFF && begin % 2 == 0)
		{
			machinecode = begin;
			return(machinecode);
		}
		else exit(3);
	}
	if (!strcmp(opcode, ".end"))
	{
		if (*arg1 || *arg2 || *arg3 || *arg4)
			exit(4);
		else
		{
			return 0;
		}
	}
	if (!strcmp(opcode, ".fill"))
	{
		if (!*arg1 || *arg2 || *arg3 || *arg4)
			exit(4);
		int fill = toNum(arg1);
		if (fill >= -0x8000 && fill <= 0xFFFF)
		{
			fill &= 0xffff;
			int machinecode = fill;
			return(machinecode);
		}
		else exit(3);
	}
	else
		exit(2);
}


FILE* infile = NULL;
FILE* outfile = NULL;

int main(int argc, char* argv[])
{
	char *prgName = NULL;
	char *iFileName = NULL;
	char *oFileName = NULL;
	prgName = argv[0];
	iFileName = argv[1];
	oFileName = argv[2];


	printf("program name = '%s'\n", prgName);
	printf("input file name = '%s'\n", iFileName);
	printf("output file name = '%s'\n", oFileName);

	//iFileName = "D:\\fresh_graduate\\computer_architecture\\lab1\\assembly.txt";
	//oFileName = "D:\\fresh_graduate\\computer_architecture\\lab1\\machinecode.txt";

	infile = fopen(argv[1], "r");
	outfile = fopen(argv[2], "w");

	if (!infile) {
		printf("Error: Cannot open file %s\n", argv[1]);
		exit(4);
	}
	if (!outfile) {
		printf("Error: Cannot open file %s\n", argv[2]);
		exit(4);
	}
	int PC, lRet;
	PC = first_pass(infile);  /*create the label table and get the original address*/
	rewind(infile);
	int startaddress = 0;
	char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
	/* assembly process*/
	do
	{
		lRet = readAndParse(infile, lLine, &lLabel,
			&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
		if (lRet != DONE && lRet != EMPTY_LINE)
		{
			if (!strcmp(lOpcode, ".end"))
			{
				translate(lOpcode, lArg1, lArg2, lArg3, lArg4, PC);
				break;
			}
			else
			{
				fprintf(outfile, "0x%.4X\n", translate(lOpcode, lArg1, lArg2, lArg3, lArg4, PC));
				PC += 2;
			}
		}
	} while (lRet != DONE);
	fclose(infile);
	fclose(outfile);
	exit(0);
}



