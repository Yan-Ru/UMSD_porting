#Makefile for Topaz API test program

include /home/roscube/Desktop/UMSD_v7.0.0_ADLINK/UMSD_v7.0.0/host/linux/makedef

CFLAGS += $(MSD_INCLUDE) -DUSE_SEMAPHORE $(TOPAZ) $(PERIDOT) $(AGATE) $(PEARL) $(AMETHYST) $(OAK) $(SPRUCE) $(BONSAIZ1) $(FIR) $(BONSAI) $(DEBUG_DEFINES)
LDFLAGS += -fPIC -L$(MSD_PATH)/host/linux/libMRegAccess -L$(MSD_PATH)/host/lib/libcli -lMRegAccess -lusb-1.0 -lpcap -lcli

EXEC = UMSD_MCLI
MSD_LIB = $(MSD_PATH)/library/$(MSD_PROJ_NAME).o

OBJECTS = $(SAMPLE_OBJS) $(UNITTEST_OBJS) $(SWTEST_OBJS)

all: $(EXEC) $(MSD_LIB)

$(MSD_LIB): 
	make -C $(MSD_PATH)

$(EXEC): $(OBJECTS) $(MSD_LIB)
	make -C $(MSD_PATH)/host/linux/libMRegAccess;
	make -C $(MSD_PATH)/host/lib/libcli;	
	$(CC) $(CFLAGS) -o $(EXEC) $(MSD_LIB) $(addprefix obj/,$(notdir $(OBJECTS))) $(LDFLAGS)
	@cp ../../sample/IMPSampleFiles/*.ihx .

%.o: %.c	
	@if [ ! -d obj ]; then mkdir obj; fi
	$(CC) $(CFLAGS) -c -O3 $< -o obj/$(notdir $@)
	
clean: 
	make -C $(MSD_PATH) clean;
	make -C $(MSD_PATH)/host/linux/libMRegAccess clean;
	make -C $(MSD_PATH)/host/lib/libcli clean;
	-@rm -f $(EXEC) *.o
	-@rm -rf obj/
	-@rm -f *.ihx

