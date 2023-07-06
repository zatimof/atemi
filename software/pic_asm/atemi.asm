;ATEMI source code
;PIC16F84A    
;
;SPEED++: RA1=1, RA3=1, RA3=0, RA1=0
;SPEED--: RA2=1, RA3=1, RA3=0, RA2=0
;
;RA0 - OUTPUT OPTO
;RA1-RA3 - INPUTS CHANGE SPEED     
;
;RB0 - INPUT RELAY ON
;RB1 - OUTPUT RELAY BLOCK    
;RB2 - INPUT FROM HALL SENSOR    
;RB3-RB7 - NOT USE
    
#include <p16f84.inc>
    
    __CONFIG _CP_OFF & _XT_OSC & _WDTE_OFF & _PWRTE_OFF
    
    CBLOCK	10H
    CNT	    :1
    PROT    :1
    SPEED   :1
    CMD1    :1
    CMD2    :1
    PRT	    :1
    ENDC
    
    ORG	    0
    GOTO    BEGIN
    
    ORG	    100H
    
BEGIN:
    BCF	    STATUS,RP0	    ;????? ???????? ??????
    CLRF    CNT		    ;????? ?????????? ? ??????
    CLRF    SPEED
    CLRF    CMD1
    CLRF    CMD2
    CLRF    PRT
    CLRF    PORTA
    CLRF    PORTB
    
    MOVLW   64H		    ;????????????? ??????????
    MOVWF   PROT
    BSF	    STATUS,RP0	    ;????? ???????? ??????
    MOVLW   0FEH	    ;????????????? ??????????
    MOVWF   TRISA
    MOVLW   0DH		    ;????????????? ??????????
    MOVWF   TRISB
    BCF	    STATUS,RP0	    ;????? ???????? ??????
    
CYCLE:			    ;???? ?????????
    INCF    CNT,1	    ;????????? ??????????
    MOVF    PORTA,0	    ;?????? ????? ?
    ANDLW   0EH		    ;????????? ??? ??????
    MOVWF   PRT		    ;?????????? ??????????
    
    SUBLW   02H		    ;?????????? ???????
    BTFSC   STATUS,Z
    BSF	    CMD1,0
    
    MOVF    PRT,0
    SUBLW   04H
    BTFSC   STATUS,Z
    BSF	    CMD2,0
    
    MOVF    PRT,0
    SUBLW   0AH
    BTFSS   STATUS,Z
    GOTO    MET1
    MOVF    CMD1,0
    SUBLW   01H
    BTFSC   STATUS,Z
    BSF	    CMD1,1
    
MET1:
    MOVF    PRT,0
    SUBLW   0CH
    BTFSS   STATUS,Z
    GOTO    MET2
    MOVF    CMD2,0
    SUBLW   01H
    BTFSC   STATUS,Z
    BSF	    CMD2,1
    
MET2:
    MOVF    PRT,0
    SUBLW   02H
    BTFSS   STATUS,Z
    GOTO    MET3
    MOVF    CMD1,0
    SUBLW   03H
    BTFSC   STATUS,Z
    BSF	    CMD1,2
    
MET3:
    MOVF    PRT,0
    SUBLW   04H
    BTFSS   STATUS,Z
    GOTO    MET4
    MOVF    CMD2,0
    SUBLW   03H
    BTFSC   STATUS,Z
    BSF	    CMD2,2
    
MET4:
    MOVF    PRT,0
    BTFSS   STATUS,Z
    GOTO    MET5
    MOVF    CMD1,0
    SUBLW   07H
    BTFSS   STATUS,Z
    GOTO    MET5
    CLRF    CMD1
    INCFSZ  SPEED,1	    ;?????????? ????????
    GOTO    MET5
    DECF    SPEED,1	    
    
MET5:
    MOVF    PRT,0
    BTFSS   STATUS,Z
    GOTO    MET6
    MOVF    CMD2,0
    SUBLW   07H
    BTFSS   STATUS,Z
    GOTO    MET6
    CLRF    CMD2
    MOVF    SPEED,0
    BTFSS   STATUS,Z
    DECF    SPEED,1	    ;?????????? ????????
 
MET6: 
    BTFSC   PORTB,RB0	    ;???? RB0=0,??
    GOTO    MET7
    CLRF    SPEED	    ;????????? ????????
    MOVLW   64H
    MOVWF   PROT	    ;?????????? PROT
    
MET7:
    MOVF    CNT,0	    ;???? CNT=0
    BTFSS   STATUS,Z
    GOTO    MET9
    BTFSS   PORTB,RB0	    ;? RB0=1
    GOTO    MET9
    MOVF    SPEED,0	    ;? SPEED!=0
    BTFSS   STATUS,Z
    BSF	    PORTA,RA0	    ;?? ?????????? RA0
    
    MOVF    PROT,0	    ;???? PROT!=0, ??
    BTFSC   STATUS,Z
    GOTO    MET8
    DECF    PROT,1	    ;????????? PROT ?? 1
    BCF	    PORTB,RB1	    ;???????? ?????????? ????
    GOTO    MET9
   
MET8:    
    BSF	    PORTB,RB1	    ;????? ?????????? ?????????? ????
    
MET9:
    MOVF    CNT,0	    ;???? CNT=SPEED
    SUBWF   SPEED,0
    BTFSS   STATUS,Z
    GOTO    MET10
    MOVF    CNT,0	    ;? CNT!=255
    SUBLW   0FFH
    BTFSS   STATUS,Z
    BCF	    PORTA,RA0	    ;?? ???????? RA0

MET10:
    BTFSC   PORTB,RB2	    ;???? RB2=0
    GOTO    CYCLE
    MOVLW   64H		    ;?? ???????? PROT
    MOVWF   PROT
    GOTO    CYCLE
    
    END