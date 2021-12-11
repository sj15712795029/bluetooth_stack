CROSS_COMPILE		?= 
TARGET					?= bt_demo
BTSTACK_ROOT 		?= ../..

CC 					:= $(CROSS_COMPILE)gcc
LD 					:= $(CROSS_COMPILE)ld
OBJCOPY 		:= $(CROSS_COMPILE)objcopy
OBJDUMP			:= $(CROSS_COMPILE)objdump

# Header file directory
INCUDIRS		:= 	./ \
								${BTSTACK_ROOT}/component/bluetooth/src/include \
								${BTSTACK_ROOT}/component/bluetooth/src/include/classical \
								${BTSTACK_ROOT}/component/bluetooth/src/include/ble \
								${BTSTACK_ROOT}/component/bluetooth/vendor \
								${BTSTACK_ROOT}/component/bluetooth/wrapper \
								${BTSTACK_ROOT}/component/ringbuffer \
								${BTSTACK_ROOT}/component/bluetooth/third_party_lib/sbc/sbc \
								${BTSTACK_ROOT}/component/bluetooth/third_party_lib/crypto/include \
				 				

# C source code file directory			
SRCDIRS			:= 	./ \
								${BTSTACK_ROOT}/component/bluetooth/src/core \
								${BTSTACK_ROOT}/component/bluetooth/src/core/classical \
								${BTSTACK_ROOT}/component/bluetooth/src/core/ble \
								${BTSTACK_ROOT}/component/bluetooth/vendor \
								${BTSTACK_ROOT}/component/bluetooth/wrapper \
								${BTSTACK_ROOT}/component/ringbuffer \
								${BTSTACK_ROOT}/component/bluetooth/third_party_lib/sbc/sbc \
								${BTSTACK_ROOT}/component/bluetooth/third_party_lib/crypto/source \
				 				



INCLUDE 		:= 	$(patsubst %, -I %, $(INCUDIRS))


CFILES			:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))


CFILENDIR		:= $(notdir $(CFILES))

OBJS			:= $(patsubst %, obj/%, $(CFILENDIR:.c=.o))

VPATH			:= $(SRCDIRS)

#CFLAGS 	?= -g -Wall -Werror $(INCLUDE)
CFLAGS 	?= -g -Wall $(INCLUDE)
LDFLAGS ?= -lpthread -lasound

.PHONY:clean

$(TARGET) : $(CFILES)
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@


clean:
	rm -rf $(TARGET) $(OBJS)

	

