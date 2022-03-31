SRC_DIR		:=	src
OBJ_DIR		:=	obj
BIN_DIR		:=	bin

ifeq ($(shell uname -s), Darwin)
CXX		:=	clang++
INCLUDE_DIRS	:=	
LIBRARY_DIRS	:=	
DFLAGS		:=	-D__STDC_CONSTANT_MACROS
LDLIBS		:=	`pkg-config --libs-only-l libjpeg libavcodec libavdevice libavfilter libavformat libavutil libpostproc libswresample libswscale`
else
CXX		:=	$(PREFIX)g++
INCLUDE_DIRS	:=	
LIBRARY_DIRS	:=	
DFLAGS		:=
LDLIBS		:=	`pkg-config --libs-only-l libjpeg libavcodec libavdevice libavfilter libavformat libavutil libswresample libswscale` -pthread
endif
OPT		:=	-O0 -g
CXXFLAGS	:=	-Wall $(OPT) $(foreach dir,$(INCLUDE_DIRS), -I$(dir)) `pkg-config --cflags libjpeg libavcodec libavdevice libavfilter libavformat libavutil libswresample libswscale` $(DFLAGS)

LDFLAGS		:=	$(foreach dir,$(LIBRARY_DIRS), -I$(dir)) `pkg-config --libs-only-L libjpeg libavcodec libavdevice libavfilter libavformat libavutil libswresample libswscale`

TARGET		:=	$(BIN_DIR)/video2tjpeg

VPATH		:=  $(SRC_DIR):$(OBJ_DIR)
.SUFFIXES:	.cpp .o

all: $(TARGET)

$(TARGET): $(OBJ_DIR)/decode_video_jpg.o
	mkdir -p $(dir $@)
	$(CXX) -o $@ $(LDFLAGS) $^ $(LDLIBS)


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) -o $@ -c $(CXXFLAGS) $<

.PHONY: clean
clean:
	-rm -rfv $(OBJ_DIR) $(BIN_DIR) $(shell find . -name *~)
