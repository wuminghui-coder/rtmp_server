
PROJECT_PATH 	 = $(shell pwd)
OBJ_PATH 	     = $(PROJECT_PATH)/obj
EXAMPLE_PATH     = $(PROJECT_PATH)/example

RTMP_SERVER      = $(EXAMPLE_PATH)/rtmp-server
RTMP_CLIENT      = $(EXAMPLE_PATH)/rtmp-client
LIB_S_RTMP       = librtmp.a
LIB_RMTP         = librtmp.so

CFLAGS   	     = -lpthread -O0  -DOS_LINUX -g -lm
LIBS     	     = -I./include  -I./libs/include -L./libs/lib -lschudule

SOURCE_C 	     = $(wildcard $(PROJECT_PATH)/src/*.c)
SOURCE_O 	     = $(patsubst %.c, $(OBJ_PATH)/%.o, $(notdir $(SOURCE_C)))

EXAMPLE_C        = $(wildcard $(EXAMPLE_PATH)/*.c)
EXAMPLE_O 	     = $(patsubst %.c, $(OBJ_PATH)/%.o, $(notdir $(EXAMPLE_C)))

.PHONY: clean obj

all: clean obj example $(LIB_S_RTMP) install

example: $(RTMP_CLIENT) $(RTMP_SERVER)
#######################################################################################
$(RTMP_SERVER): $(OBJ_PATH)/rtmp-server.o $(SOURCE_O)
	$(CC) $^ -o $@ $(CFLAGS) $(LIBS) 

$(RTMP_CLIENT): $(OBJ_PATH)/rtmp-client.o $(SOURCE_O)
	$(CC) $^ -o $@ $(CFLAGS) $(LIBS) 

#######################################################################################
$(OBJ_PATH)/rtmp-server.o: $(EXAMPLE_PATH)/rtmp-server.c
	$(CC) -c $^ -o $@ $(CFLAGS) $(LIBS)

$(OBJ_PATH)/rtmp-client.o: $(EXAMPLE_PATH)/rtmp-client.c 
	$(CC) -c $^ -o $@ $(CFLAGS) $(LIBS)

#######################################################################################
#$(LIB_RMTP): $(SOURCE_C)
#	$(CC) -fPIC -shared -o $@ $^ $(LIBS)

$(LIB_S_RTMP): $(SOURCE_O)
	$(AR) rcs $@ $^

$(OBJ_PATH)/%.o: $(PROJECT_PATH)/src/%.c
	$(CC) -c $^ -o $@ $(CFLAGS) $(LIBS)

#######################################################################################
obj:
ifeq ("$(wildcard $(OBJ_PATH))","")
	mkdir $(OBJ_PATH)
endif

clean:
	-$(RM) -r $(OBJ_PATH)/*.o 
	-$(RM) -r __install
	-$(RM) -r $(RTMP_CLIENT) $(RTMP_SERVER)

install:
	mkdir __install && mkdir -p  __install/lib
#mv $(LIB_RMTP) __install/lib
	mv $(LIB_S_RTMP)  __install/lib
	cp -r include __install
