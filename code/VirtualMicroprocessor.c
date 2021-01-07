/*	
BENEMERITA UNIVERSIDAD AUTONOMA DE PUEBLA
        PUEBLA, PUE. APRIL 2012
		
           ELECTRONICS ENGINEERING
		   
      Course: COMPUTER ARCHITECTURE
    Professor: AURELIO JACINTO NOLASCO
    Student: EDGAR DANIEL JAVIER HERNANDEZ
	
	The simulation of a microcontroller of 8-bits is done in this project.

    */


#include<stdio.h>
#include <dos.h>
#include <stdlib.h>

typedef unsigned short int WORD;
typedef unsigned char BYTE;


struct BUSES
{
       WORD BA, BB;
       WORD BLA, BLB;
       WORD BMBR;
       WORD BMX;
       WORD BALU;
       WORD BRC;
       WORD Bdat;
       WORD Bdir;
} BUS;

struct CONTROLES
{
       BYTE RA, RB, WRC, ENA;
       BYTE MAR;
       BYTE MBR, eMBR, lMBR ;
       BYTE cMX;
       BYTE cALU1,cALU0;
       BYTE cRC1,cRC0;
       BYTE JMP0, JMP1, DIRS;
       } CTRL;

struct BANDERAS
{
       BYTE BN;
       BYTE BP;
} BAN;

struct SALTO
{
       BYTE SALTA;
       } SLT;



void REG16x16(WORD BeR,BYTE LaA, BYTE LaB, BYTE act, BYTE EdR, WORD *BA, WORD *BB)
{
  static WORD Reg[16];
    Reg[4] = 0xFFFF; // AS
    Reg[5] = 0x0001; // +1
    Reg[6] = 0x0000; //  0
    Reg[7] = 0xFFFF; // -1
    Reg[8] = 0X00FF; // MASCAS
    Reg[9] = 0x0FFF; // MASDIR

	if(act)
      Reg[EdR]=BeR;
	  *BA=Reg[LaA];
	  *BB=Reg[LaB];
}

void latchAB(WORD BeA, WORD BeB,WORD *BsLA,WORD *BsLB)   /* esta siempre atrapan*/
{
	static WORD regA, regB;
		*BsLA = regA = BeA;
		*BsLB = regB = BeB;
}

void MUX(WORD BsMBR, WORD BsLA, BYTE act, WORD *BsMx)
{
	*BsMx = (act? BsMBR : BsLA);
}

void ALU (WORD BeMX, WORD BeB, BYTE cod1, BYTE cod0, BYTE *pos,BYTE *neg, WORD *BsAL)
{
	BYTE alu;
	alu = (cod1<<1) + cod0;
    printf("ALU: ");
	switch (alu)
	{
		case 0: *BsAL = BeMX;       printf("TRANSPARENTE\t");           break;
		case 1: *BsAL = BeMX + BeB; printf("SUMA %x + %x\t",BeMX, BeB); break;
		case 2: *BsAL = BeMX & BeB; printf("AND\t");                    break;
		case 3: *BsAL = ~BeMX;      printf("NEG\t");                    break;
	}
		if(*BsAL !=0)
		{
        *pos = ((0x8000 & *BsAL)? 0:1);
		*neg = ((0x8000 & *BsAL)? 1:0);
		}
		else
		{
		    *pos = *neg = 0;
		}
}

void RegCo(WORD BsAL, BYTE act1, BYTE act0, WORD *BsRC)
{
	BYTE RC;
	RC = (act1<<1) + act0;
	printf("REGCORR:");
	switch (RC)
	{
		case 0:	*BsRC = BsAL;    printf("TRANSPARENTE\t");                           break;
		case 1: *BsRC = BsAL>>1; printf("DERECHA: IN:%X OUT:%X\t",BsAL, *BsRC);      break;
		case 2: *BsRC = BsAL<<1; printf("IZQUIERDA: IN:%X OUT:%X\t",BsAL, *BsRC);    break;
		case 3: *BsRC = BsAL;    printf("TRANSPARENTE\t");                           break;
	}
}

void MBR(WORD *BusExt, WORD BsRC, BYTE mbr, BYTE r, BYTE w, WORD *BsMBR )
{
	static WORD latch;
	if(mbr)
	{
		if(r)
		{
			*BsMBR = latch = *BusExt;
        }
		if(w)
		{
			*BusExt = latch = BsRC;
		}
	}
}

void CALDIR(BYTE jmp1, BYTE jmp0, BYTE bn, BYTE bp, BYTE *salta)
{
    printf("SALTA : ");
    if(jmp1)
    {
        if(jmp0)               /* 1 1  salta si ac = 0*/
        {
            printf("BN:%X BP%X\t",bn, bp);
            *salta = (((!bn )&(!bp))? 1 : 0);
            printf("SI AC = 0\t");
        }
        else                    /* 1 0 salta si ac < 0*/
        {
            *salta = (bn?1:0);
            printf("SI AC < 0\t");
        }
    }
    else
    {
      if(jmp0)                  /* 0 1  No salta*/
      {
          *salta = 0;
          printf("No Salta\t");
      }
      else                      /* 0 0  salta siempre*/
      {
          *salta = 1;
          printf("SIEMPRE\t");
      }
    }
}


void sum (BYTE en, BYTE *sal)
{
	*sal = ++en;
}



void latch(WORD en, BYTE *sdir)
{
	static WORD latch;
	*sdir = latch = en;
}

void MAR(WORD BsLB, BYTE mar, WORD *BsMAR)
{
	static WORD latch;
	if(mar)
	{
        *BsMAR = latch = (BsLB & 0x0FFF);    /* 0xFFF = 0000111111111111;*/
	}
}


unsigned  int short RAM[4096]=

{  /*0        1       2       3       4       5       6       7       8       9       A       B       C       D       E       F       10      11      12      13*/
   0x000d, 0x200a, 0x100d, 0x000a, 0x200c, 0x100a, 0x300c, 0x300b, 0x7000, 0x5009, 0x0000, 0x0005, 0x0001, 0x0000,
};

const unsigned long ROM [256] =
{
    0x21C10000, 0xA0100200, 0x28115100, 0x44102309, 0x4410330E, 0x45D23309, 0x40003008,
    0x80100000, 0x01A20000, 0x4000300B, 0x88100000, 0xB8100300, 0x28153300, 0x08103000,
    0x44103314, 0x44103312, 0x40000000, 0x10192100, 0x60000011, 0x00000000, 0x40003017, 0x40000011, 0x00000000, 0x60000000, 0x10192100,
    0x44103325, 0x4411131F, 0x4411331D, 0x10192100, 0x40000000, 0x00102000, 0x40003023, 0x28143300, 0x21A30000, 0x80100000, 0x28143300, 0x01B30000,
    0x44103328, 0x28142A00, 0x01CA0009, 0x4410332B, 0x24147400, 0x01A4101C, 0x4410333E, 0x44103337, 0x44103333,
    0x60003000, 0x28147400, 0x21C00000, 0x81A40000, 0x60003000, 0xA1C40000, 0xA1A00000, 0x08145400, 0x4411333B,
    0x60003000, 0x28147400, 0x01A40000, 0x60003000, 0x21C40000, 0x80100036, 0x44103348, 0x44113343, 0x60003000,
    0x21C40000, 0x80100136, 0x60003000, 0x20100A00, 0x20104000, 0x0010A400, 0x20182A00, 0x4411334A, 0x081A4400,
    0x3810AA00, 0x281A5A00, 0x0814A400,
 };

void AcRAM(WORD DIR, BYTE RD, BYTE WR, WORD *DATOS)
{

    if(RD & !WR)
	{
		*DATOS=RAM[DIR];
	}
	if(!RD & WR)
	{
		RAM[DIR]=*DATOS;


	}
}


void MIR(unsigned long int mirA, BYTE *Dirs, BYTE *WRC, BYTE *RA, BYTE *RB,BYTE *ENA,BYTE *eMBR, BYTE *lMBR,BYTE *MAR,BYTE *MBR,BYTE *cRC0,BYTE *cRC1,BYTE *cALU0,BYTE *cALU1,BYTE *JMP0,BYTE *JMP1,BYTE *cMUX)
{
     (BYTE)*Dirs  = (BYTE) (mirA & 0x000000FF);
     (BYTE)*WRC   = (BYTE)((mirA & 0x00000F00)>>8);
     (BYTE)*RA    = (BYTE)((mirA & 0x0000F000)>>12);
     (BYTE)*RB    = (BYTE)((mirA & 0x000F0000)>>16);
     (BYTE)*ENA   = (BYTE)((mirA & 0x00100000)>>20);
     (BYTE)*eMBR  = (BYTE)((mirA & 0x00200000)>>21);
     (BYTE)*lMBR  = (BYTE)((mirA & 0x00400000)>>22);
     (BYTE)*MAR   = (BYTE)((mirA & 0x00800000)>>23);
     (BYTE)*MBR   = (BYTE)((mirA & 0x01000000)>>24);
     (BYTE)*cRC0  = (BYTE)((mirA & 0x02000000)>>25);
     (BYTE)*cRC1  = (BYTE)((mirA & 0x04000000)>>26);
     (BYTE)*cALU0 = (BYTE)((mirA & 0x08000000)>>27);
     (BYTE)*cALU1 = (BYTE)((mirA & 0x10000000)>>28);
     (BYTE)*JMP0  = (BYTE)((mirA & 0x20000000)>>29);
     (BYTE)*JMP1  = (BYTE)((mirA & 0x40000000)>>30);
     (BYTE)*cMUX  = (BYTE)((mirA & 0x80000000)>>31);
}

void AcROM(BYTE DIR, unsigned long int *DAT)
{
         *DAT = ROM[DIR];
}

void main()
{

    BYTE reset = 0, clk = 0, dirom , en = 0, sal, smux;
    WORD DAT, RA;

    unsigned long  int inst;


    while(1)
    {
       printf("\n\nINICIO\n");

       if(reset)
    {
             printf("RESET");
             en = 0;
             CTRL.ENA = 1;
             CTRL.WRC = 1;
             BUS.BRC = 0;
             REG16x16(BUS.BRC, CTRL.RA, CTRL.RB, CTRL.ENA, CTRL.WRC, &BUS.BA, &BUS.BB);
    }
    while(!reset)
    {
           if(clk == 0)
           {
                  AcROM(en, &inst);
                  printf("\n\nINS: %x CuP: %x\n", inst, en);
                  MIR (inst, &CTRL.DIRS, &CTRL.WRC, &CTRL.RA, &CTRL.RB, &CTRL.ENA, &CTRL.eMBR, &CTRL.lMBR, &CTRL.MAR, &CTRL.MBR, &CTRL.cRC0, &CTRL.cRC1, &CTRL.cALU0, &CTRL.cALU1, &CTRL.JMP0, &CTRL.JMP1, &CTRL.cMX);
                  printf("%08X C:%04X A:%04X B:%04X E:%0X E:%0X L:%0X MAR:%0X MBR:%0X RC:%0X %0X ALU:%0X %0X Slt:%0X %0X Mx:%0X \n\n", CTRL.DIRS, CTRL.WRC, CTRL.RA, CTRL.RB, CTRL.ENA, CTRL.eMBR, CTRL.lMBR, CTRL.MAR, CTRL.MBR, CTRL.cRC0, CTRL.cRC1, CTRL.cALU0, CTRL.cALU1, CTRL.JMP0, CTRL.JMP1, CTRL.cMX);
                  clk++;
           }
           if(clk == 1)
           {
                  REG16x16(BUS.BRC, CTRL.RA, CTRL.RB, 0, CTRL.WRC, &BUS.BA, &BUS.BB);
                  latchAB(BUS.BA, BUS.BB, &BUS.BLA, &BUS.BLB);
                  clk++;
           }

           if(clk == 2)
           {

                  MAR(BUS.BLB, CTRL.MAR, &BUS.Bdir);
                  clk++;
           }

           MUX(BUS.BMBR, BUS.BLA, CTRL.cMX, &BUS.BMX);
           ALU(BUS.BMX, BUS.BLB, CTRL.cALU1, CTRL.cALU0, &BAN.BP, &BAN.BN, &BUS.BALU);
           RegCo(BUS.BALU, CTRL.cRC1, CTRL.cRC0, &BUS.BRC);
           CALDIR(CTRL.JMP1, CTRL.JMP0, BAN.BN, BAN.BP, &SLT.SALTA);

           if(clk == 3)
           {
                  sum(en, &sal);                                                            /* Calculo de direccion */
	              printf("SALTO: %X ds:%X Sec:%X \n", SLT.SALTA, CTRL.DIRS, sal);
                  MUX(CTRL.DIRS, sal, SLT.SALTA, &smux);
                  latch(smux, &en);                                                                   /*FIN*/

                if(CTRL.lMBR | CTRL.eMBR)
                {
                      AcRAM(BUS.Bdir, CTRL.lMBR, 0, &BUS.Bdat);
                      MBR(&BUS.Bdat, BUS.BRC, CTRL.MBR, CTRL.lMBR, CTRL.eMBR, &BUS.BMBR);
                      AcRAM(BUS.Bdir, 0, CTRL.eMBR, &BUS.Bdat);
                }
                      clk = 0;


           }

           REG16x16(BUS.BRC, CTRL.RA, CTRL.RB, CTRL.ENA, CTRL.WRC, &BUS.BA, &BUS.BB);



           for(CTRL.RA = 0; CTRL.RA <= 15; CTRL.RA++)
           {
                      REG16x16(BUS.BRC, CTRL.RA, CTRL.RB, CTRL.ENA, CTRL.WRC, &BUS.BA, &BUS.BB);
                      printf("%04x ", BUS.BA);

                      }

            printf("\n");

            for(RA = 0x0000; RA <= 0x13; RA++)
            {
                AcRAM(RA, 1, 0, &DAT);
                printf("%04x ", DAT);
            }
            printf("\n\n");
            system("cls");
    }

}
}


