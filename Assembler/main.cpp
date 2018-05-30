//
//  main.cpp
//  Assembler
//
//  Created by 신예지 on 2017.05.01
//  Copyright © 2018년 신예지. All rights reserved.
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

/***************************** DECLERATE VARIABLE ****************************/
typedef struct OperationCodeTable
{
    char Mnemonic[8];    // ���ɾ�
    char Format;        // ���ɾ� ����
    unsigned short int  ManchineCode;
}SIC_OPTAB;

typedef struct SymbolTable
{
    char Label[10];
    int Address;
}SIC_SYMTAB;

// �߰��ڵ�
typedef struct IntermediateRecord {
    unsigned short int LineIndex;
    unsigned short int Loc;
    unsigned long int ObjectCode;
    char LabelField[32];
    char OperatorField[32];
    char OperandField[32];
}IntermediateRec;

// ���� ���ڵ�
typedef struct ModifyRecord
{
    unsigned short int Loc;
    unsigned short int length;
}ModifyRecord;

ModifyRecord Mtable[30];//�������ڵ� ���� ���̺�

int MCounter = 0;    // �������ڵ� ���� ����

int Counter;        // OPTAB���� ���° mnemonic���� ��Ÿ���� ����
int LOCCTR[100];    // Locctr�� ������� ������ �迭
int LocctrCounter = 0;    // Locctr ����
int ProgramLen;
int Index;
int j;
int ManchineCode;
int SymtabCounter = 0;    // SYMTAB ���� ����, RecordSymtab�� �� ���� ����
int start_address;
int program_length;
int ArrayIndex = 0;

// Symtab���� label ã�� �� �ִ��� ������ 0�� 1�� ǥ��
unsigned short int FoundOnSymtab_flag = 0;
unsigned short int FoundOnOptab_flag = 0;

char Buffer[256];
char Label[32];
char Mnemonic[32];
char Operand[32];

// label�� �ּҸ� ��� �ִ� �ɺ����̺�
SIC_SYMTAB SYMTAB[20];

// �߰��ڵ带 ������ �迭
// Loc, Label, Operator, Operand, ObjectCode�� ������ ��� ����
IntermediateRec* IMRArray[100];

// ���ɾ� ���İ� �ӽ��ڵ带 �����ϰ� �ִ� �迭
static SIC_OPTAB OPTAB[] =
{
    { "ADD",  '3',  0x18 },
    { "AND",  '3',  0x40 },
    { "COMP",  '3',  0x28 },
    { "DIV",  '3',  0x24 },
    { "J",  '3',  0x3C },
    { "JEQ",  '3',  0x30 },
    { "JGT",  '3',  0x34 },
    { "JLT",  '3',  0x38 },
    { "JSUB",  '3',  0x48 },
    { "LDA",  '3',  0x00 },
    { "LDCH",  '3',  0x50 },
    { "LDL",  '3',  0x08 },
    { "LDX",  '3',  0x04 },
    { "MUL",  '3',  0x20 },
    { "OR",  '3',  0x44 },
    { "RD",  '3',  0xD8 },
    { "RSUB",  '3',  0x4C },
    { "STA",  '3',  0x0C },
    { "STCH",  '3',  0x54 },
    { "STL",  '3',  0x14 },
    { "STSW",  '3',  0xE8 },
    { "STX",  '3',  0x10 },
    { "SUB",  '3',  0x1C },
    { "TD",  '3',  0xE0 },
    { "TIX",  '3',  0x2C },
    { "WD",  '3',  0xDC },
    
    //���� 4�߰�
    { "+ADD",  '4',  0x18 },
    { "+AND",  '4',  0x40 },
    { "+COMP",  '4',  0x28 },
    { "+DIV",  '4',  0x24 },
    { "+J",  '4',  0x3C },
    { "+JEQ",  '4',  0x30 },
    { "+JGT",  '4',  0x34 },
    { "+JLT",  '4',  0x38 },
    { "+JSUB",  '4',  0x48 },
    { "+LDA",  '4',  0x00 },
    { "+LDCH",  '4',  0x50 },
    { "+LDL",  '4',  0x08 },
    { "+LDX",  '4',  0x04 },
    { "+MUL",  '4',  0x20 },
    { "+OR",  '4',  0x44 },
    { "+RD",  '4',  0xD8 },
    { "+RSUB",  '4',  0x4C },
    { "+STA",  '4',  0x0C },
    { "+STCH",  '4',  0x54 },
    { "+STL",  '4',  0x14 },
    { "+STSW",  '4',  0xE8 },
    { "+STX",  '4',  0x10 },
    { "+SUB",  '4',  0x1C },
    { "+TD",  '4',  0xE0 },
    { "+TIX",  '4',  0x2C },
    { "+WD",  '4',  0xDC },
    
    // Instruction Set 1 �߰�
    { "ADDR",'2',0x90 },
    { "CLEAR",'2',0xB4 },
    { "COMPR", '2',0xA0 },
    { "DIVR", '2',0x9C },
    { "HIO", '1',0xF4 },
    { "LDB", '3',0x68 },
    { "+LDB", '4',0x68 },
    { "LDS", '3',0x6C },
    { "+LDS", '4',0x6C },
    { "LDT", '3',0x74 },
    { "+LDT", '4',0x74 },
    { "LPS", '3',0xD0 },
    { "+LPS", '4',0xD0 },
    { "MULR", '2',0x98 },
    { "RMO", '2',0xAC },
    { "SHIFTL", '2',0xA4 },
    { "SHIFTR", '2',0xA8 },
    { "SIO", '1',0xF0 },
    { "SSK", '3',0xEC },
    { "+SSK", '4',0xEC },
    { "STS", '3',0x7C },
    { "+STS", '4',0x7C },
    { "STT",'3',0x84 },
    { "+STT",'4',0x84 },
    { "SUBR",
        '2',0x94 },
    { "SVC",'2',0xB0 },
    { "TIO", '1',0xF8 },
    { "TIXR", '2',0xB8 },
    
    //Instruction Set 2 �߰�
    {"ADDF", '3',0x58},
    { "+ADDF", '4',0x58 },
    {"COMPF",'3',0x88},
    { "+COMPF",'4',0x88 },
    {"DIVF",'3',0x64},
    { "+DIVF",'4',0x64 },
    {"FIX",'1',0xC4},
    {"FLOAT",'1',0xC0},
    {"LDF",'3',0x70},
    { "+LDF",'4',0x70 },
    {"MULF",'3',0x60},
    { "+MULF",'4',0x60 },
    {"NORM",'1',0xC8},
    {"STF",'3',0x80},
    { "+STF",'4',0x80 },
    {"SUBF",'3',0x5C},
    { "+SUBF",'4',0x5C },
};


/****************************** DFINATE FUNCTION *****************************/

// Label�� �д� �Լ�
char* ReadLabel() {
    j = 0;//zeroing
    while (Buffer[Index] != ' ' && Buffer[Index] != '\t' && Buffer[Index] != '\n')
        Label[j++] = Buffer[Index++];
    Label[j] = '\0';
    return(Label);
}

// ��ĭ�� �ǳʶٴ� �Լ�
void SkipSpace() {
    while (Buffer[Index] == ' ' || Buffer[Index] == '\t')
        Index++;
}

// �����ڸ� �����ϴ� �Լ�
char* ReadOprator() {
    j = 0;//zeroing
    while (Buffer[Index] != ' ' && Buffer[Index] != '\t' && Buffer[Index] != '\n')
        Mnemonic[j++] = Buffer[Index++];
    Mnemonic[j] = '\0';
    return(Mnemonic);
}

// �ǿ����� �����ϴ� �Լ�
char* ReadOperand() {
    j = 0;//zeroing
    while (Buffer[Index] != ' ' && Buffer[Index] != '\t' && Buffer[Index] != '\n')
        Operand[j++] = Buffer[Index++];
    Operand[j] = '\0';
    return(Operand);
}

// SYMTAB�� label�� address �߰��ϴ� �Լ�
void RecordSymtab(char* label) {
    strcpy(SYMTAB[SymtabCounter].Label, label);
    SYMTAB[SymtabCounter].Address = LOCCTR[LocctrCounter - 1];
    SymtabCounter++;
}

// Symtab�� label�� ������ 1, ������ 0 ��ȯ
int SearchSymtab(char* label) {
    FoundOnSymtab_flag = 0;
    
    for (int k = 0; k <= SymtabCounter; k++) {
        
        // label�� SYMTAB�� �ִ� ���
        if (!strcmp(SYMTAB[k].Label, label)) {
            FoundOnSymtab_flag = 1;
            return (FoundOnSymtab_flag);
            break;
        }
    }
    return (FoundOnSymtab_flag);
}

// OPTAB���� �ϸ�� ã�� �Լ� ������ 1, ������ 0 ��ȯ
int SearchOptab(char * Mnemonic) {
    
    // size�� SIC_OPTAB�� ����� mnemonic ����
    int size = sizeof(OPTAB) / sizeof(SIC_OPTAB);
    FoundOnOptab_flag = 0;
    for (int i = 0; i<size; i++) {
        if (!strcmp(Mnemonic, OPTAB[i].Mnemonic)) {
            Counter = i;
            FoundOnOptab_flag = 1;
            break;
        }
    }
    return (FoundOnOptab_flag);
}

// �������� �ٲ��ִ� �Լ�
int StrToDec(char* c) {
    int dec_num = 0;
    char temp[10];
    strcpy(temp, c);
    
    int len = strlen(c);
    for (int k = len - 1, l = 1; k >= 0; k--)
    {
        if (temp[k] >= 48 && temp[k] <= 57) {
            dec_num = dec_num + (int)(temp[k] - '0')*l;
            l = l * 10;
        }
        else
            break;
    }
    return (dec_num);
}

// 16������ �ٲ��ִ� �Լ�
int StrToHex(char* c)
{
    int hex_num = 0;
    char temp[10];
    strcpy(temp, c);
    
    int len = strlen(temp);
    for (int k = len - 1, l = 1; k >= 0; k--)
    {
        if (temp[k] >= '0' && temp[k] <= '9')
            hex_num = hex_num + (int)(temp[k] - '0')*l;
        else if (temp[k] >= 'A' && temp[k] <= 'F')
            hex_num = hex_num + (int)(temp[k] - 'A' + 10)*l;
        else if (temp[k] >= 'a' && temp[k] >= 'f')
            hex_num = hex_num + (int)(temp[k] - 'a' + 10)*l;
        else;
        l = l * 16;
    }
    return (hex_num);
}

int ComputeLen(char* c) {
    unsigned int b;
    char len[32];
    
    strcpy(len, c);
    if (len[0] == 'C' || len[0] == 'c' && len[1] == '\'') {
        for (b = 2; b <= strlen(len); b++) {
            if (len[b] == '\'') {
                b -= 2;
                break;
            }
        }
    }
    if (len[0] == 'X' || len[0] == 'x' && len[1] == '\'')
        b = 1;
    return (b);
}

void CreateProgramList() {
    int loop;
    FILE *fptr_list;
    
    fptr_list = fopen("sic.list", "w");
    
    if (fptr_list == NULL)
    {
        printf("ERROE: Unable to open the sic.list.\n");
        exit(1);
    }
    
    fprintf(fptr_list, "%-4s\t%-10s%-10s%-10s\t%s\n", "LOC", "LABEL", "OPERATOR", "OPERAND", "OBJECT CODE");
    for (loop = 0; loop < ArrayIndex; loop++)
    {
        if (!strcmp(IMRArray[loop]->OperatorField, "START") || !strcmp(IMRArray[loop]->OperatorField, "RESW") || !strcmp(IMRArray[loop]->OperatorField, "RESB") || !strcmp(IMRArray[loop]->OperatorField, "END"))
            fprintf(fptr_list, "%04X\t%-10s%-10s%-10s\n", IMRArray[loop]->Loc, IMRArray[loop]->LabelField, IMRArray[loop]->OperatorField, IMRArray[loop]->OperandField);
        else if (!strcmp(IMRArray[loop]->OperatorField, "BASE"))        // BASE�� Loc�� �����ڵ� ����
            fprintf(fptr_list, "\t%-10s%-10s%-10s\n", IMRArray[loop]->LabelField, IMRArray[loop]->OperatorField, IMRArray[loop]->OperandField);
        
        else if ((strcmp(IMRArray[loop]->OperatorField, "BYTE") == 0) && (IMRArray[loop]->OperandField[0] == 'X' || IMRArray[loop]->OperandField[0] == 'x'))
            fprintf(fptr_list, "%04X\t%-10s%-10s%-10s\t%02X\n", IMRArray[loop]->Loc, IMRArray[loop]->LabelField, IMRArray[loop]->OperatorField, IMRArray[loop]->OperandField, IMRArray[loop]->ObjectCode);
        
        else if ((strcmp(IMRArray[loop]->OperatorField, "BYTE") == 0) && (IMRArray[loop]->OperandField[0] == 'C' || IMRArray[loop]->OperandField[0] == 'c'))
            fprintf(fptr_list, "%04X\t%-10s%-10s%-10s\t%06X\n", IMRArray[loop]->Loc, IMRArray[loop]->LabelField, IMRArray[loop]->OperatorField, IMRArray[loop]->OperandField, IMRArray[loop]->ObjectCode);
        
        else {
            if (SearchOptab(IMRArray[loop]->OperatorField)) {
                if (OPTAB[Counter].Format == '1')    // ���� 1�̸� 2�ڸ�
                    fprintf(fptr_list, "%04X\t%-10s%-10s%-10s\t%02X\n", IMRArray[loop]->Loc, IMRArray[loop]->LabelField, IMRArray[loop]->OperatorField, IMRArray[loop]->OperandField, IMRArray[loop]->ObjectCode);
                else if (OPTAB[Counter].Format == '2')    // ���� 2�̸� 4�ڸ�
                    fprintf(fptr_list, "%04X\t%-10s%-10s%-10s\t%04X\n", IMRArray[loop]->Loc, IMRArray[loop]->LabelField, IMRArray[loop]->OperatorField, IMRArray[loop]->OperandField, IMRArray[loop]->ObjectCode);
                else
                    fprintf(fptr_list, "%04X\t%-10s%-10s%-10s\t%06X\n", IMRArray[loop]->Loc, IMRArray[loop]->LabelField, IMRArray[loop]->OperatorField, IMRArray[loop]->OperandField, IMRArray[loop]->ObjectCode);
            }
        }
    }
    fclose(fptr_list);
}

void CreateObjectCode() {
    int first_address;
    int last_address;
    int temp_address;
    int temp_objectcode[30];
    int first_index;
    int last_index;
    int x, xx;
    int loop;
    
    char temp_operator[12][10];
    char temp_operand[12][10];
    
    FILE *fptr_obj;
    fptr_obj = fopen("sic.obj", "w");
    if (fptr_obj == NULL)
    {
        printf("ERROE: Unable to open the sic.obj.\n");
        exit(1);
    }
    
    printf("Creating Object Code...\n\n");
    
    loop = 0;
    if (!strcmp(IMRArray[loop]->OperatorField, "START"))
    {
        printf("H%-6s%06X%06X\n", IMRArray[loop]->LabelField, start_address, program_length);
        fprintf(fptr_obj, "H^%-6s^%06X^%06X\n", IMRArray[loop]->LabelField, start_address, program_length);
        loop++;
    }    // Head�κ� ���
    
    while (1)
    {
        
        first_address = IMRArray[loop]->Loc;
        last_address = IMRArray[loop]->Loc + 27;
        first_index = loop;
        
        for (x = 0, temp_address = first_address; temp_address <= last_address; loop++)
        {
            if (!strcmp(IMRArray[loop]->OperatorField, "END"))
                break;
            else if (strcmp(IMRArray[loop]->OperatorField, "RESB") && strcmp(IMRArray[loop]->OperatorField, "RESW"))
            {
                temp_objectcode[x] = IMRArray[loop]->ObjectCode;
                strcpy(temp_operator[x], IMRArray[loop]->OperatorField);
                strcpy(temp_operand[x], IMRArray[loop]->OperandField);
                last_index = loop + 1;
                x++;
            }
            else;
            temp_address = IMRArray[loop + 1]->Loc;
        }
        
        // Text �κ� ���
        printf("T%06X%02X", first_address, (IMRArray[last_index]->Loc - IMRArray[first_index]->Loc));
        fprintf(fptr_obj, "T^%06X^%02X", first_address, (IMRArray[last_index]->Loc - IMRArray[first_index]->Loc));
        
        for (xx = 0; xx<x; xx++)
        {
            if ((strcmp(temp_operator[xx], "BYTE") == 0) && (temp_operand[xx][0] == 'X' || temp_operand[xx][0] == 'x')) {
                printf("%02X", temp_objectcode[xx]);
                fprintf(fptr_obj, "^%02X", temp_objectcode[xx]);
            }
            else if ((strcmp(temp_operator[xx], "BYTE") == 0) && (temp_operand[xx][0] == 'C' || temp_operand[xx][0] == 'c')) {
                printf("%06X", temp_objectcode[xx]);
                fprintf(fptr_obj, "^%06X", temp_objectcode[xx]);
            }
            else {
                if (strcmp(temp_operator[xx], "BASE") == 0);    // BASE�� �����ڵ� ����
                else {
                    if (SearchOptab(temp_operator[xx])) {
                        if (OPTAB[Counter].Format == '1') {    // ���� 1�̸� 2�ڸ�
                            printf("%02X", temp_objectcode[xx]);
                            fprintf(fptr_obj, "^%02X", temp_objectcode[xx]);
                        }
                        else if (OPTAB[Counter].Format == '2') {// ���� 2�̸� 4�ڸ�
                            printf("%04X", temp_objectcode[xx]);
                            fprintf(fptr_obj, "^%04X", temp_objectcode[xx]);
                        }
                        else {
                            printf("%06X", temp_objectcode[xx]);
                            fprintf(fptr_obj, "^%06X", temp_objectcode[xx]);
                        }
                    }
                }
            }
        }
        
        printf("\n");
        fprintf(fptr_obj, "\n");
        
        if (!strcmp(IMRArray[loop]->OperatorField, "END"))
            break;
    }
    
    // Modify �κ� ���
    for (x = 0; x < MCounter; x++)//�������ڵ��� ������ŭ Loop�� �����鼭
    {
        printf("M%06X%02X\n", Mtable[x].Loc, Mtable[x].length);//������ ���� loc�� ���� ����� ���
        fprintf(fptr_obj, "M^%06X^%02X\n", Mtable[x].Loc, Mtable[x].length);//������ ���� loc�� ���� ���� ���
    }
    
    // End �κ� ���
    printf("E%06X\n\n", start_address);
    fprintf(fptr_obj, "E^%06X\n\n", start_address);
    fclose(fptr_obj);
}

/******************************* MAIN FUNCTION *******************************/
void main(void)
{
    FILE* fptr;
    
    char filename[20];
    char label[32];
    char opcode[32];
    char operand[32];
    
    int loc = 0;
    int line = 0;
    int loop;
    int is_empty_line;
    int is_comment;
    int loader_flag = 0;
    
    int BaseLoc = 0;    // BASE�� �ּҸ� ����
    char BaseOperand[10];
    
    printf(" ******************************************************************************\n");
    printf(" * Program: SIC ASSEMBYER                                                     *\n");
    printf(" *                                                                            *\n");
    printf(" * Procedure:                                                                 *\n");
    printf(" *   - Enter file name of source code.                                        *\n");
    printf(" *   - Do pass 1 process.                                                     *\n");
    printf(" *   - Do pass 2 process.                                                     *\n");
    printf(" *   - Create \"program list\" data on sic.list.(Use Notepad to read this file) *\n");
    printf(" *   - Create \"object code\" data on sic.obj.(Use Notepad to read this file)   *\n");
    printf(" *   - Also output object code to standard output device.                     *\n");
    printf(" ******************************************************************************\n");
    
    
    printf("\nEnter the file name you want to assembly (sic.asm):");
    scanf("%s", filename);
    fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        printf("ERROE: Unable to open the %s file.\n", filename);
        exit(1);
    }
    
    /********************************** PASS 1 ***********************************/
    printf("Pass 1 Processing...\n");
    while (fgets(Buffer, 256, fptr) != NULL)
    {
        // �� �������� �ƴ� �� ���ڿ� ���̷� �Ǵ�
        is_empty_line = strlen(Buffer);
        
        Index = 0;
        j = 0;
        strcpy(label, ReadLabel());
        
        // .���� �����ϸ� �ּ�
        if (Label[0] == '.')
            is_comment = 1;
        else
            is_comment = 0;
        
        // �� ������ �ƴϰ� �ּ��� �ƴ϶��
        if (is_empty_line>1 && is_comment != 1)
        {
            Index = 0;
            j = 0;
            
            IMRArray[ArrayIndex] = (IntermediateRec*)malloc(sizeof(IntermediateRec));/* [A] */
            
            IMRArray[ArrayIndex]->LineIndex = ArrayIndex;
            strcpy(label, ReadLabel());
            strcpy(IMRArray[ArrayIndex]->LabelField, label);
            SkipSpace();
            
            // ù��° �� �̶��
            if (line == 0)
            {
                strcpy(opcode, ReadOprator());
                strcpy(IMRArray[ArrayIndex]->OperatorField, opcode);/* [A] */
                
                // opcode�� START�̸�
                if (!strcmp(opcode, "START"))
                {
                    SkipSpace();
                    strcpy(operand, ReadOperand());
                    strcpy(IMRArray[ArrayIndex]->OperandField, operand);/* [A] */
                    
                    // ���� �ּ� 16������ ����
                    LOCCTR[LocctrCounter] = StrToHex(operand);
                    start_address = LOCCTR[LocctrCounter];
                }
                else
                {
                    LOCCTR[LocctrCounter] = 0;
                    start_address = LOCCTR[LocctrCounter];
                }
            }
            
            // line 0 �ƴϸ�
            else
            {
                strcpy(opcode, ReadOprator());
                strcpy(IMRArray[ArrayIndex]->OperatorField, opcode);
                SkipSpace();
                strcpy(operand, ReadOperand());
                strcpy(IMRArray[ArrayIndex]->OperandField, operand);
                
                // opcode END���
                if (strcmp(opcode, "END"))
                {
                    // label�� �ִٸ�
                    if (label[0] != '\0')
                    {
                        // SYMTAB�� label�� �ִٸ�
                        if (SearchSymtab(label))
                        {
                            fclose(fptr);
                            printf("ERROE: Duplicate Symbol\n");
                            FoundOnSymtab_flag = 0;
                            exit(1);
                        }
                        RecordSymtab(label);
                    }
                    
                    
                    // �ּ� �����, LOCCTR�� ����
                    if (SearchOptab(opcode))
                        LOCCTR[LocctrCounter] = loc + (int)(OPTAB[Counter].Format - '0');
                    else if (!strcmp(opcode, "WORD"))
                        LOCCTR[LocctrCounter] = loc + 3;    // WORD 3��ŭ ����
                    else if (!strcmp(opcode, "RESW"))
                        LOCCTR[LocctrCounter] = loc + 3 * StrToDec(operand);    // WORD���� * 3 ��ŭ ����
                    else if (!strcmp(opcode, "RESB"))
                        LOCCTR[LocctrCounter] = loc + StrToDec(operand);    // BYTE ���� ��ŭ ����
                    else if (!strcmp(opcode, "BYTE"))
                        LOCCTR[LocctrCounter] = loc + ComputeLen(operand);
                    else if (!strcmp(opcode, "BASE"))    // BASE�� �Ǵ� operand ����
                        strcpy(BaseOperand, operand);
                    else {
                        fclose(fptr);
                        printf("ERROE: Invalid Operation Code\n");
                        exit(1);
                    }
                }
            }
            
            // BASE�� ���
            if (!strcmp(opcode, "BASE")) {
                IMRArray[ArrayIndex]->Loc = LOCCTR[LocctrCounter - 2];
                ArrayIndex++;
            }
            else {
                loc = LOCCTR[LocctrCounter];
                IMRArray[ArrayIndex]->Loc = LOCCTR[LocctrCounter - 1];
                LocctrCounter++;
                ArrayIndex++;
            }
        }
        FoundOnOptab_flag = 0;
        line += 1;
    }
    program_length = LOCCTR[LocctrCounter - 2] - LOCCTR[0];
    
    /********************************** PASS 2 ***********************************/
    printf("Pass 2 Processing...\n");
    
    unsigned long inst_fmt;
    unsigned long inst_fmt_opcode;
    unsigned long inst_fmt_index;
    long inst_fmt_address;
    
    char operandPure[10];    // #,@���� ���� operand�� ����
    
    
    for (loop = 1; loop<ArrayIndex; loop++) {
        inst_fmt_opcode = 0;
        inst_fmt_index = 0;
        inst_fmt_address = 0;
        
        strcpy(opcode, IMRArray[loop]->OperatorField);
        
        if (SearchOptab(opcode)) {
            inst_fmt_opcode = OPTAB[Counter].ManchineCode;    // int�� �ӽ��ڵ� ����
            
            if (OPTAB[Counter].Format == '2')
                inst_fmt_opcode <<= 8;    // ���� 2�̸�, �������� 8��Ʈ �̵�
            else if (OPTAB[Counter].Format == '3')
                inst_fmt_opcode <<= 16;    // ���� 3�̸�, �������� 16��Ʈ �̵�
            else if (OPTAB[Counter].Format == '4')
                inst_fmt_opcode <<= 24;    // ���� 4�̸�, �������� 24��Ʈ �̵�
            
            IMRArray[loop]->ObjectCode = inst_fmt_opcode;
            strcpy(operand, IMRArray[loop]->OperandField);
            
            
            // ���� 1�� ���
            if (OPTAB[Counter].Format == '1') {
                inst_fmt_index = 0;
                inst_fmt_address = 0;
            }
            
            // ���� 2�� ���
            else if (OPTAB[Counter].Format == '2') {
                // A:0                X:1                L:2                PC:8                SW:9
                // B:3                S:4                T:5                F:6
                
                
                char r1;    // ù ��° ��������
                r1 = operand[0];
                
                if (r1 >= 65 && r1 <= 90) {
                    switch (r1) {
                        case'A':
                            inst_fmt_index = 0x0000;
                            break;
                        case'X':
                            inst_fmt_index = 0x0010;
                            break;
                        case'L':
                            inst_fmt_index = 0x0020;
                            break;
                        case'B':
                            inst_fmt_index = 0x0030;
                            break;
                        case'S':
                            inst_fmt_index = 0x0040;
                            break;
                        case'T':
                            inst_fmt_index = 0x0050;
                            break;
                        case'F':
                            inst_fmt_index = 0x0060;
                            break;
                    }
                    
                    if (operand[1] == ',') {
                        char r2;        // �� ��° ��������
                        r2 = operand[2];
                        
                        if (r2 >= 65 && r2 <= 90) {
                            switch (r2) {
                                case'A':
                                    inst_fmt_index += 0x0000;
                                    break;
                                case'X':
                                    inst_fmt_index += 0x0001;
                                    break;
                                case'L':
                                    inst_fmt_index += 0x0002;
                                    break;
                                case'B':
                                    inst_fmt_index += 0x0003;
                                    break;
                                case'S':
                                    inst_fmt_index += 0x0004;
                                    break;
                                case'T':
                                    inst_fmt_index += 0x0005;
                                    break;
                                case'F':
                                    inst_fmt_index += 0x0006;
                                    break;
                            }
                        }
                    }
                }
                
                else
                    inst_fmt_index = 0x0000;
            }
            
            else {
                
                // extended format (e=1), ���� 4
                // e=1�� ���
                if (OPTAB[Counter].Format == '4') {
                    inst_fmt_index = 0x03100000;
                    
                    for (int search_symtab = 0; search_symtab < SymtabCounter; search_symtab++) {
                        if (!strcmp(operand, SYMTAB[search_symtab].Label))
                            inst_fmt_address = (long)SYMTAB[search_symtab].Address;
                    }
                    
                    // immediate ����̶��
                    if (operand[0] == '#') {
                        inst_fmt_index = 0x01100000;
                        int temp = StrToDec(operand);
                        inst_fmt_address = (long)temp;
                    }
                    else {
                        Mtable[MCounter].Loc = LOCCTR[loop - 2] + 1;
                        Mtable[MCounter++].length = 0x05;
                    }    // Mtable�� �߰�
                }
                
                // ���� 3�� ���
                else {
                    
                    // index addressing mode (x=1)
                    // x=1�� ���
                    if (operand[strlen(operand) - 2] == ',' && operand[strlen(operand) - 1] == 'X') {
                        inst_fmt_index = 0x038000;
                        strcpy(operandPure, operand);
                        operandPure[strlen(operand) - 2] = '\0';
                        
                        for (int search_symtab = 0; search_symtab < SymtabCounter; search_symtab++) {
                            if (!strcmp(operandPure, SYMTAB[search_symtab].Label))
                                inst_fmt_address = (long)SYMTAB[search_symtab].Address;
                        }
                    }
                    //x=0�� ���
                    else {
                        inst_fmt_index = 0x030000;
                        
                        for (int search_symtab = 0; search_symtab < SymtabCounter; search_symtab++) {
                            if (!strcmp(operand, SYMTAB[search_symtab].Label))
                                inst_fmt_address = (long)SYMTAB[search_symtab].Address;
                        }
                    }
                    
                    
                    // immediate addressing mode (n=0, i=1)
                    if (operand[0] == '#') {
                        inst_fmt_index = 0x010000;
                        
                        // operand���� #���� operandPure�� ����
                        for (int i = 0; i < strlen(operand); i++)
                            operandPure[i] = operand[i + 1];
                        
                        // ������ ���    ex) #3
                        if (operandPure[0] >= 48 && operandPure[0] <= 57) {
                            int temp = StrToHex(operand);
                            inst_fmt_address = (long)temp;
                        }
                        
                        // ������ ���    ex) #LENGTH
                        else
                            for (int search_symtab = 0; search_symtab < SymtabCounter; search_symtab++) {
                                if (!strcmp(operandPure, SYMTAB[search_symtab].Label))
                                    inst_fmt_address = (long)SYMTAB[search_symtab].Address;
                            }
                    }
                    
                    // indirect addressing mode (n=1, i=0)
                    if (operand[0] == '@') {
                        inst_fmt_index = 0x020000;
                        
                        // operand���� @���� operandPure�� ����
                        for (int i = 0; i < strlen(operand); i++)
                            operandPure[i] = operand[i + 1];
                        
                        // operand�� �ش��ϴ� �ּ� ã��
                        for (int search_symtab = 0; search_symtab < SymtabCounter; search_symtab++) {
                            if (!strcmp(operandPure, SYMTAB[search_symtab].Label))
                                inst_fmt_address = (long)SYMTAB[search_symtab].Address;
                        }
                    }
                    
                    if (!(operand[1] >= 48 && operand[1] <= 57)) {
                        // Relative addressing mode
                        
                        // PC Relative addressing mode (b=0, p=1)
                        inst_fmt_index += 0x002000;
                        
                        for (int i = 1; i < ArrayIndex; i++) {
                            if (!strcmp(operand, IMRArray[i]->OperandField) && !strcmp(opcode, IMRArray[i]->OperatorField)) {
                                if (!strcmp(IMRArray[i + 1]->OperatorField, "BASE")) {
                                    inst_fmt_address -= (long)IMRArray[i + 2]->Loc;
                                    
                                    // PC ���ذ� �Ѿ�� Base�� �������� ���
                                    // Base Relative addressing mode (b=1, p=1)
                                    if (BaseLoc != 0 && inst_fmt_address > 2047 || inst_fmt_address < -2048) {
                                        inst_fmt_index -= 0x002000;
                                        inst_fmt_index += 0x004000;
                                        inst_fmt_address += (long)IMRArray[i + 2]->Loc;
                                        inst_fmt_address -= BaseLoc;
                                    }    // Base�� ��
                                }
                                else {
                                    inst_fmt_address -= (long)IMRArray[i + 1]->Loc;
                                    
                                    // PC ���ذ� �Ѿ�� Base�� �������� ���
                                    // Base Relative addressing mode (b=1, p=1)
                                    if (BaseLoc != 0 && inst_fmt_address > 2047 || inst_fmt_address < -2048) {
                                        inst_fmt_index -= 0x002000;
                                        inst_fmt_index += 0x004000;
                                        inst_fmt_address += (long)IMRArray[i + 1]->Loc;
                                        inst_fmt_address -= BaseLoc;
                                    }    // Base�� ��
                                }
                            }
                        }    // PC�� �ּҸ� ��
                        
                        if (inst_fmt_address < 0)
                            inst_fmt_address += 0x001000;
                    }
                }
                
                if (!strcmp(opcode, "RSUB")) {
                    inst_fmt_index = 0x030000;
                    inst_fmt_address = 0x0000;
                }
            }
            
            // opcode�� index, address�� ���� �����ڵ� ����
            inst_fmt = inst_fmt_opcode + inst_fmt_index + inst_fmt_address;
            IMRArray[loop]->ObjectCode = inst_fmt;
        }
        
        else if (!strcmp(opcode, "WORD")) {
            strcpy(operand, IMRArray[loop]->OperandField);
            IMRArray[loop]->ObjectCode = StrToDec(operand);
        }
        
        // BYTE : �������, ��� ǥ���� �ʿ��� ��ŭ ��Ʈ ���        ex) C'EOF', X'01'
        else if (!strcmp(opcode, "BYTE")) {
            strcpy(operand, IMRArray[loop]->OperandField);
            IMRArray[loop]->ObjectCode = 0;
            
            if (operand[0] == 'C' || operand[0] == 'c' && operand[1] == '\'') {
                for (int x = 2; x <= (int)(strlen(operand) - 2); x++) {
                    IMRArray[loop]->ObjectCode = IMRArray[loop]->ObjectCode + (int)operand[x];
                    IMRArray[loop]->ObjectCode <<= 8;
                }
            }
            
            if (operand[0] == 'X' || operand[0] == 'x' && operand[1] == '\'') {
                char *operand_ptr;
                operand_ptr = &operand[2];
                *(operand_ptr + 2) = '\0';
                for (int x = 2; x <= (int)(strlen(operand) - 2); x++) {
                    IMRArray[loop]->ObjectCode = IMRArray[loop]->ObjectCode + StrToHex(operand_ptr);
                    IMRArray[loop]->ObjectCode <<= 8;
                }
            }
            
            IMRArray[loop]->ObjectCode >>= 8;
        }
        
        // BASE�� �ּҸ� ����
        else if (!strcmp(opcode, "BASE")) {
            for (int i = 0; i < ArrayIndex; i++)
                if (!strcmp(BaseOperand, IMRArray[i]->LabelField)) {
                    BaseLoc = IMRArray[i]->Loc;
                    break;
                }
        }
        
        else
        /* do nothing */;
    }
    
    CreateProgramList();
    CreateObjectCode();
    
    for (loop = 0; loop<ArrayIndex; loop++)
        free(IMRArray[loop]);
    
    printf("Compeleted Assembly\n");
    fclose(fptr);
}
