CC=gcc -g
RM=rm -rf
NOPRINT=@
CC_FLAG=-Wall -lrt -lm -pthread -rdynamic -I ./include/
##lwebsockets -lssl -lcrypto都是为了链接websocket库，ssl是openssl相关，crypto是X509相关
LIB=-L ./lib -lwebsockets
PROJECT_PATH:=.
SOURCES=$(wildcard $(PROJECT_PATH)/src/*.c)
#HEADERS=$(wildcard $(PROJECT_PATH)/include/*.h)
OBJDIR=./build
DIROBJS=$(patsubst %.c, ${OBJDIR}/%.o, $(notdir ${SOURCES}))

TARGET:=run

#“%”用于wildcard通配符，相当于循环每次取出wildcard中一个元素来获取；“*”包括了所有文件一次全部取出
${TARGET}: $(DIROBJS)
	@echo building ${TARGET}
	@echo Linking $@ from $^
	${NOPRINT} ${CC} ${CC_FLAG} -o $@ $^ ${LIB}

${OBJDIR}/%.o: ./src/%.c ${HEADERS}
	@echo Compiling $@ from $<..
	$(NOPRINT) ${CC} -c ${CC_FLAGS} -o $@ $<

clean:
	${RM} ${DIROBJS} ${TARGET}

staticlib: $(DIROBJS)
	@echo build static lib lib${TARGET}.a
	${NOPRINT} ar rcs lib${TARGET}.a $^

#dynamiclib: $(DIROBJS)
#	@echo build dynamic lib lib${TARGET}.so
#	${NOPRINT} gcc -shared -fPIC -o lib${TARGET}.so $^





#FILE_NAME:=test.txt
#var:=$(file < $(FILE_NAME))
#var1:=aa
#ifeq ($(var),30)
#       var1=bb
#else
#       var1=cc
#endif
