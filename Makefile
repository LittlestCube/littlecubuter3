####################################################
#                                                  #
#                 everything sucks                 #
#                                                  #
####################################################

INCLUDES		:= ../include

LIBS			:=
LIBDIRS			:=

SOURCES			:= src
BUILD			:= build

export INCLUDE	:= $(foreach dir, $(INCLUDES), -I$(dir))

SRCS			:= $(foreach f, $(SOURCES), $(notdir $(wildcard $(f)/*.cpp)))

FILES			:= $(CPPFILES:.cpp=)

export OBJS		:= $(foreach f, $(SRCS), $(notdir $(f:.cpp=.o)))

LDFLAGS			:=

TARGET			:= lc3



VPATH			 = $(foreach dir, $(SOURCES), ../$(dir)) $(foreach dir, $(INCLUDES), $(dir))

OPTFLAGS		:= -Ofast -ffast-math \
				   -ffunction-sections \
				   -fmerge-all-constants -Wl,--gc-sections \
				   -flto -fomit-frame-pointer \

DEBUGOPTFLAGS	:= -g

CPPFLAGS		:= $(INCLUDE) $(OPTFLAGS) -Wno-unused-command-line-argument -std=gnu++11



.PHONY: all dev clean



all: clean dev

dev: | $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f ../Makefile $(OBJS) -j8
	
	@echo "linking \`$(TARGET)'..."
	
	@$(MAKE) --no-print-directory -C $(BUILD) -f ../Makefile link -j8

$(BUILD):
	mkdir $@

link:
	@$(CXX) ./*.o -o ../$(TARGET) $(LDFLAGS)

clean:
	@echo clean ...
	
	@rm -f $(TARGET)
	
	@rm -rf $(BUILD)

run:
	@./$(TARGET)