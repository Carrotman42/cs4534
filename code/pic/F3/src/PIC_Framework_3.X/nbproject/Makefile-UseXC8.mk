#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-UseXC8.mk)" "nbproject/Makefile-local-UseXC8.mk"
include nbproject/Makefile-local-UseXC8.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=UseXC8
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/PIC_Framework_3.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/PIC_Framework_3.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../src/interrupts.c ../src/main.c ../src/messages.c ../src/my_i2c.c ../src/my_uart.c ../src/timer0_thread.c ../src/timer1_thread.c ../src/uart_thread.c ../src/user_interrupts.c ../src/debug.c ../../../../common/communication/brain_rover.c ../src/sensorcomm.c ../src/testAD.c ../src/my_adc.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1360937237/interrupts.p1 ${OBJECTDIR}/_ext/1360937237/main.p1 ${OBJECTDIR}/_ext/1360937237/messages.p1 ${OBJECTDIR}/_ext/1360937237/my_i2c.p1 ${OBJECTDIR}/_ext/1360937237/my_uart.p1 ${OBJECTDIR}/_ext/1360937237/timer0_thread.p1 ${OBJECTDIR}/_ext/1360937237/timer1_thread.p1 ${OBJECTDIR}/_ext/1360937237/uart_thread.p1 ${OBJECTDIR}/_ext/1360937237/user_interrupts.p1 ${OBJECTDIR}/_ext/1360937237/debug.p1 ${OBJECTDIR}/_ext/746215922/brain_rover.p1 ${OBJECTDIR}/_ext/1360937237/sensorcomm.p1 ${OBJECTDIR}/_ext/1360937237/testAD.p1 ${OBJECTDIR}/_ext/1360937237/my_adc.p1
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1360937237/interrupts.p1.d ${OBJECTDIR}/_ext/1360937237/main.p1.d ${OBJECTDIR}/_ext/1360937237/messages.p1.d ${OBJECTDIR}/_ext/1360937237/my_i2c.p1.d ${OBJECTDIR}/_ext/1360937237/my_uart.p1.d ${OBJECTDIR}/_ext/1360937237/timer0_thread.p1.d ${OBJECTDIR}/_ext/1360937237/timer1_thread.p1.d ${OBJECTDIR}/_ext/1360937237/uart_thread.p1.d ${OBJECTDIR}/_ext/1360937237/user_interrupts.p1.d ${OBJECTDIR}/_ext/1360937237/debug.p1.d ${OBJECTDIR}/_ext/746215922/brain_rover.p1.d ${OBJECTDIR}/_ext/1360937237/sensorcomm.p1.d ${OBJECTDIR}/_ext/1360937237/testAD.p1.d ${OBJECTDIR}/_ext/1360937237/my_adc.p1.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1360937237/interrupts.p1 ${OBJECTDIR}/_ext/1360937237/main.p1 ${OBJECTDIR}/_ext/1360937237/messages.p1 ${OBJECTDIR}/_ext/1360937237/my_i2c.p1 ${OBJECTDIR}/_ext/1360937237/my_uart.p1 ${OBJECTDIR}/_ext/1360937237/timer0_thread.p1 ${OBJECTDIR}/_ext/1360937237/timer1_thread.p1 ${OBJECTDIR}/_ext/1360937237/uart_thread.p1 ${OBJECTDIR}/_ext/1360937237/user_interrupts.p1 ${OBJECTDIR}/_ext/1360937237/debug.p1 ${OBJECTDIR}/_ext/746215922/brain_rover.p1 ${OBJECTDIR}/_ext/1360937237/sensorcomm.p1 ${OBJECTDIR}/_ext/1360937237/testAD.p1 ${OBJECTDIR}/_ext/1360937237/my_adc.p1

# Source Files
SOURCEFILES=../src/interrupts.c ../src/main.c ../src/messages.c ../src/my_i2c.c ../src/my_uart.c ../src/timer0_thread.c ../src/timer1_thread.c ../src/uart_thread.c ../src/user_interrupts.c ../src/debug.c ../../../../common/communication/brain_rover.c ../src/sensorcomm.c ../src/testAD.c ../src/my_adc.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-UseXC8.mk dist/${CND_CONF}/${IMAGE_TYPE}/PIC_Framework_3.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F45J10
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1360937237/interrupts.p1: ../src/interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/interrupts.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/interrupts.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/interrupts.p1  ../src/interrupts.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/interrupts.d ${OBJECTDIR}/_ext/1360937237/interrupts.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/interrupts.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/main.p1: ../src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/main.p1  ../src/main.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/main.d ${OBJECTDIR}/_ext/1360937237/main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/messages.p1: ../src/messages.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/messages.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/messages.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/messages.p1  ../src/messages.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/messages.d ${OBJECTDIR}/_ext/1360937237/messages.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/messages.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/my_i2c.p1: ../src/my_i2c.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/my_i2c.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/my_i2c.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/my_i2c.p1  ../src/my_i2c.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/my_i2c.d ${OBJECTDIR}/_ext/1360937237/my_i2c.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/my_i2c.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/my_uart.p1: ../src/my_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/my_uart.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/my_uart.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/my_uart.p1  ../src/my_uart.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/my_uart.d ${OBJECTDIR}/_ext/1360937237/my_uart.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/my_uart.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/timer0_thread.p1: ../src/timer0_thread.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/timer0_thread.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/timer0_thread.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/timer0_thread.p1  ../src/timer0_thread.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/timer0_thread.d ${OBJECTDIR}/_ext/1360937237/timer0_thread.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/timer0_thread.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/timer1_thread.p1: ../src/timer1_thread.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/timer1_thread.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/timer1_thread.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/timer1_thread.p1  ../src/timer1_thread.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/timer1_thread.d ${OBJECTDIR}/_ext/1360937237/timer1_thread.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/timer1_thread.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/uart_thread.p1: ../src/uart_thread.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/uart_thread.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/uart_thread.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/uart_thread.p1  ../src/uart_thread.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/uart_thread.d ${OBJECTDIR}/_ext/1360937237/uart_thread.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/uart_thread.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/user_interrupts.p1: ../src/user_interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/user_interrupts.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/user_interrupts.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/user_interrupts.p1  ../src/user_interrupts.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/user_interrupts.d ${OBJECTDIR}/_ext/1360937237/user_interrupts.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/user_interrupts.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/debug.p1: ../src/debug.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/debug.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/debug.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/debug.p1  ../src/debug.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/debug.d ${OBJECTDIR}/_ext/1360937237/debug.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/debug.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/746215922/brain_rover.p1: ../../../../common/communication/brain_rover.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/746215922 
	@${RM} ${OBJECTDIR}/_ext/746215922/brain_rover.p1.d 
	@${RM} ${OBJECTDIR}/_ext/746215922/brain_rover.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/746215922/brain_rover.p1  ../../../../common/communication/brain_rover.c 
	@-${MV} ${OBJECTDIR}/_ext/746215922/brain_rover.d ${OBJECTDIR}/_ext/746215922/brain_rover.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/746215922/brain_rover.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/sensorcomm.p1: ../src/sensorcomm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/sensorcomm.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/sensorcomm.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/sensorcomm.p1  ../src/sensorcomm.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/sensorcomm.d ${OBJECTDIR}/_ext/1360937237/sensorcomm.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/sensorcomm.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/testAD.p1: ../src/testAD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/testAD.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/testAD.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/testAD.p1  ../src/testAD.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/testAD.d ${OBJECTDIR}/_ext/1360937237/testAD.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/testAD.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/my_adc.p1: ../src/my_adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/my_adc.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/my_adc.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/my_adc.p1  ../src/my_adc.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/my_adc.d ${OBJECTDIR}/_ext/1360937237/my_adc.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/my_adc.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/_ext/1360937237/interrupts.p1: ../src/interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/interrupts.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/interrupts.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/interrupts.p1  ../src/interrupts.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/interrupts.d ${OBJECTDIR}/_ext/1360937237/interrupts.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/interrupts.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/main.p1: ../src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/main.p1  ../src/main.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/main.d ${OBJECTDIR}/_ext/1360937237/main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/messages.p1: ../src/messages.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/messages.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/messages.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/messages.p1  ../src/messages.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/messages.d ${OBJECTDIR}/_ext/1360937237/messages.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/messages.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/my_i2c.p1: ../src/my_i2c.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/my_i2c.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/my_i2c.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/my_i2c.p1  ../src/my_i2c.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/my_i2c.d ${OBJECTDIR}/_ext/1360937237/my_i2c.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/my_i2c.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/my_uart.p1: ../src/my_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/my_uart.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/my_uart.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/my_uart.p1  ../src/my_uart.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/my_uart.d ${OBJECTDIR}/_ext/1360937237/my_uart.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/my_uart.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/timer0_thread.p1: ../src/timer0_thread.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/timer0_thread.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/timer0_thread.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/timer0_thread.p1  ../src/timer0_thread.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/timer0_thread.d ${OBJECTDIR}/_ext/1360937237/timer0_thread.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/timer0_thread.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/timer1_thread.p1: ../src/timer1_thread.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/timer1_thread.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/timer1_thread.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/timer1_thread.p1  ../src/timer1_thread.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/timer1_thread.d ${OBJECTDIR}/_ext/1360937237/timer1_thread.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/timer1_thread.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/uart_thread.p1: ../src/uart_thread.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/uart_thread.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/uart_thread.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/uart_thread.p1  ../src/uart_thread.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/uart_thread.d ${OBJECTDIR}/_ext/1360937237/uart_thread.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/uart_thread.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/user_interrupts.p1: ../src/user_interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/user_interrupts.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/user_interrupts.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/user_interrupts.p1  ../src/user_interrupts.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/user_interrupts.d ${OBJECTDIR}/_ext/1360937237/user_interrupts.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/user_interrupts.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/debug.p1: ../src/debug.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/debug.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/debug.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/debug.p1  ../src/debug.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/debug.d ${OBJECTDIR}/_ext/1360937237/debug.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/debug.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/746215922/brain_rover.p1: ../../../../common/communication/brain_rover.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/746215922 
	@${RM} ${OBJECTDIR}/_ext/746215922/brain_rover.p1.d 
	@${RM} ${OBJECTDIR}/_ext/746215922/brain_rover.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/746215922/brain_rover.p1  ../../../../common/communication/brain_rover.c 
	@-${MV} ${OBJECTDIR}/_ext/746215922/brain_rover.d ${OBJECTDIR}/_ext/746215922/brain_rover.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/746215922/brain_rover.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/sensorcomm.p1: ../src/sensorcomm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/sensorcomm.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/sensorcomm.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/sensorcomm.p1  ../src/sensorcomm.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/sensorcomm.d ${OBJECTDIR}/_ext/1360937237/sensorcomm.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/sensorcomm.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/testAD.p1: ../src/testAD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/testAD.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/testAD.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/testAD.p1  ../src/testAD.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/testAD.d ${OBJECTDIR}/_ext/1360937237/testAD.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/testAD.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1360937237/my_adc.p1: ../src/my_adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/my_adc.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/my_adc.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: (%%n) %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"    -o${OBJECTDIR}/_ext/1360937237/my_adc.p1  ../src/my_adc.c 
	@-${MV} ${OBJECTDIR}/_ext/1360937237/my_adc.d ${OBJECTDIR}/_ext/1360937237/my_adc.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1360937237/my_adc.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/PIC_Framework_3.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) --chip=$(MP_PROCESSOR_OPTION) -G -mdist/${CND_CONF}/${IMAGE_TYPE}/PIC_Framework_3.X.${IMAGE_TYPE}.map  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"        -odist/${CND_CONF}/${IMAGE_TYPE}/PIC_Framework_3.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
	@${RM} dist/${CND_CONF}/${IMAGE_TYPE}/PIC_Framework_3.X.${IMAGE_TYPE}.hex 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/PIC_Framework_3.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) --chip=$(MP_PROCESSOR_OPTION) -G -mdist/${CND_CONF}/${IMAGE_TYPE}/PIC_Framework_3.X.${IMAGE_TYPE}.map  --double=24 --float=24 --emi=wordwrite --opt=none,+asm,-asmfile,+speed,-space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%%f:%%l: error: %%s" "--warnformat=%%f:%%l: warning: (%%n) %%s" "--msgformat=%%f:%%l: advisory: (%%n) %%s"     -odist/${CND_CONF}/${IMAGE_TYPE}/PIC_Framework_3.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/UseXC8
	${RM} -r dist/UseXC8

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
