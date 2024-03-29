typedef unsigned char   INT8U ;
typedef unsigned short  INT16U ;
typedef unsigned int    INT32U ;
typedef signed int      INT32S ;
typedef signed char     INT8S ;
typedef signed short    INT16S ;

#define MOV_A_A 0x7F
#define MOV_A_B 0x78
#define MOV_A_C 0x79
#define MOV_A_D 0x7A
#define MOV_A_E 0x7B
#define MOV_A_H 0x7C
#define MOV_A_L 0x7D
#define MOV_A_M 0x7E

#define MOV_B_A 0x47
#define MOV_B_B 0x40
#define MOV_B_C 0x41
#define MOV_B_D 0x42
#define MOV_B_E 0x43
#define MOV_B_H 0x44
#define MOV_B_L 0x45
#define MOV_B_M 0x46

#define MOV_C_A 0x4F
#define MOV_C_B 0x48
#define MOV_C_C 0x49
#define MOV_C_D 0x4A
#define MOV_C_E 0x4B
#define MOV_C_H 0x4C
#define MOV_C_L 0x4D
#define MOV_C_M 0x4E

#define MOV_D_A 0x57
#define MOV_D_B 0x50
#define MOV_D_C 0x51
#define MOV_D_D 0x52
#define MOV_D_E 0x53
#define MOV_D_H 0x54
#define MOV_D_L 0x55
#define MOV_D_M 0x56

#define MOV_E_A 0x5F
#define MOV_E_B 0x58
#define MOV_E_C 0x59
#define MOV_E_D 0x5A
#define MOV_E_E 0x5B
#define MOV_E_H 0x5C
#define MOV_E_L 0x5D
#define MOV_E_M 0x5E


#define MOV_H_A 0x67
#define MOV_H_B 0x60
#define MOV_H_C 0x61
#define MOV_H_D 0x62
#define MOV_H_E 0x63
#define MOV_H_H 0x64
#define MOV_H_L 0x65
#define MOV_H_M 0x66

#define MOV_L_A 0x6F
#define MOV_L_B 0x68
#define MOV_L_C 0x69
#define MOV_L_D 0x6A
#define MOV_L_E 0x6B
#define MOV_L_H 0x6C
#define MOV_L_L 0x6D
#define MOV_L_M 0x6E

#define MOV_M_A 0x77
#define MOV_M_B 0x70
#define MOV_M_C 0x71
#define MOV_M_D 0x72
#define MOV_M_E 0x73
#define MOV_M_H 0x74
#define MOV_M_L 0x75

#define MVI_A 0x3E
#define MVI_B 0x06
#define MVI_C 0x0E
#define MVI_D 0x16
#define MVI_E 0x1E
#define MVI_H 0x26
#define MVI_L 0x2E
#define MVI_M 0x36

#define LXI_B 0x01
#define LXI_D 0x11
#define LXI_H 0x21
#define LXI_SP 0x31

#define LDAX_B 0x0A
#define LDAX_D 0x1A
#define STAX_B 0x2A
#define STAX_D 0x3A

#define LDA 0x3A
#define STA 0x32

#define LHLD 0x2A
#define SHLD 0x22

#define XCHG 0xEB

#define ADD_A 0x87
#define ADD_B 0x80
#define ADD_C 0x81
#define ADD_D 0x82
#define ADD_E 0x83
#define ADD_H 0x84
#define ADD_L 0x85
#define ADD_M 0x86

#define ADC_A 0x8F
#define ADC_B 0x88
#define ADC_C 0x89
#define ADC_D 0x8A
#define ADC_E 0x8B
#define ADC_H 0x8C
#define ADC_L 0x8D
#define ADC_M 0x8E

#define SUB_A 0x97
#define SUB_B 0x91
#define SUB_C 0x92
#define SUB_D 0x93
#define SUB_E 0x97
#define SUB_H 0x94
#define SUB_L 0x95
#define SUB_M 0x96

#define SBB_A 0x9F
#define SBB_B 0x98
#define SBB_C 0x99
#define SBB_D 0x9A
#define SBB_E 0x9B
#define SBB_H 0x9C
#define SBB_L 0x9D
#define SBB_M 0x9E

#define ADI 0xCE
#define ACI 0xC6
#define SUI 0xDE
#define SBI 0xD6

#define DAD_B 0x09
#define DAD_D 0x19
#define DAD_H 0x29
#define DAD_SP 0x39

#define INR_A 0x3C
#define INR_B 0x04
#define INR_C 0x0C
#define INR_D 0x14
#define INR_E 0x1C
#define INR_H 0x24
#define INR_L 0x2C
#define INR_M 0x34

#define DCR_A 0x3D
#define DCR_B 0x05
#define DCR_C 0x0D
#define DCR_D 0x15
#define DCR_E 0x1D
#define DCR_H 0x25
#define DCR_L 0x2D
#define DCR_M 0x35

#define INX_B 0x03
#define INX_D 0x13
#define INX_H 0x23
#define INX_SP 0x33

#define DCX_B 0x0B
#define DCX_D 0x1B
#define DCX_H 0x2B
#define DCX_SP 0x3B

#define DAA 0x27

#define CMA 0x2F

#define CMC 0x3F
#define STC 0X37

#define ANA_A 0xA7
#define ANA_B 0xA0
#define ANA_C 0xA1
#define ANA_D 0xA2
#define ANA_E 0xA3
#define ANA_H 0xA4
#define ANA_L 0xA5
#define ANA_M 0xA6

#define ORA_A 0xB7
#define ORA_B 0xB0
#define ORA_C 0xB1
#define ORA_D 0xB2
#define ORA_E 0xB3
#define ORA_H 0xB4
#define ORA_L 0xB5
#define ORA_M 0xB6

#define XRA_A 0xAF
#define XRA_B 0xA8
#define XRA_C 0xA9
#define XRA_D 0xAA
#define XRA_E 0xAB
#define XRA_H 0xAC
#define XRA_L 0xAD
#define XRA_M 0xAE

#define CMP_A 0xBF
#define CMP_B 0xB8
#define CMP_C 0xB9
#define CMP_D 0xBA
#define CMP_E 0xBB
#define CMP_H 0xBC
#define CMP_L 0xBD
#define CMP_M 0xBE

#define RLC 0x07
#define RRC 0x0F
#define RLA 0x17
#define RAR 0x1F

#define ANI 0xE6
#define XRI 0xEE
#define ORI 0xF6
#define CPI 0xFE

#define JMP 0xC3
#define JNZ 0xC2
#define JZ 0xCA
#define JNC 0xD2
#define JC 0xDA
#define JPO 0xE2
#define JPE 0xEA
#define JP 0xF2
#define JM 0xFA

#define CALL 0xCD
#define CNZ 0xC4
#define CZ 0xCC
#define CNC 0xD4
#define CC 0xDC
#define CPO 0xE4
#define CPE 0xEC
#define CP 0xF4
#define CM 0xFC

#define RET 0xC9
#define RNZ 0xC0
#define RZ 0xC8
#define RNC 0xD0
#define RC 0xD8
#define RPO 0xE0
#define RPE 0xE8
#define RP 0xF0
#define RM 0xF8

#define PCHL 0xE9

#define IN 0xDB
#define OUT 0xD3

#define PUSH_B 0xC5
#define PUSH_D 0xD5
#define PUSH_H 0xE5
#define PUSH_PSW 0xF5

#define POP_B 0xC1
#define POP_D 0xD1
#define POP_H 0xE1
#define POP_PSW 0xF1

#define XTHL 0xE3
#define SPHL 0xF9

#define EI 0xFB
#define DI 0xF3
#define RIM 0x20
#define SIM 0x30

#define NOP 0x00
#define HLT 0x76

#define RST_0 0xC7
#define RST_1 0xCF
#define RST_2 0xD7
#define RST_3 0xDF
#define RST_4 0xE7
#define RST_5 0xEF
#define RST_6 0xF7
#define RST_7 0xFF

INT8U my_prog[32]=
{
         LXI_H,0x00,0xc1,      //C000          LXI     H,C100H
         MVI_B,0x20,           //C003          MVI     B,20H
         MVI_C,0x00,           //C005          MVI     C,00H
         MOV_A_M,              //C007  LOOP:   MOV     A,M
         CPI,0xAA,             //C008          CPI     0AAH
         JNZ,0x0e,0xc0,        //C00A          JNZ     SKIP
         INR_C,                //C00D          INR     C
         INX_H,                //C00E  SKIP:   INX     H
         DCR_B,                //C00F          DCR     B
         JNZ,0x07,0xc0,        //C010          JNZ     LOOP
         MOV_A_C,              //C013          MOV     A,C
         STA,0x20,0xc1,        //C014          STA     C120H
         HLT                   //C017          HLT
};

INT8U my_data[32]=
{
         0x01,0x02,0x55,0x04,0x01,0x02,0x55,0x04,
         0x01,0x02,0x54,0x04,0xAA,0xAA,0x55,0x04,
	     0x01,0x02,0x55,0x04,0x01,0x02,0x56,0x04,
	     0xAA,0x02,0x00,0x04,0x01,0x02,0x55,0x04
};
INT16U prog_start = 0xc000 ;
INT16U prog_end   = 0xc017 ;
INT16U data_start = 0xc100 ;
INT16U data_end   = 0xc120 ;

INT8U    trace = 1 ;
INT8U    step = 0 ;
INT8U    brkno = 2 ;
INT8U    watchno = 1 ;
INT16U   brkpt[ 8 ]     = { 0xc00d , 0xc013 } ;
INT16U   watchstrt[ 8 ] = { 0xc100 } ;
INT16U   watchend[ 8 ]  = { 0xc11f } ;
