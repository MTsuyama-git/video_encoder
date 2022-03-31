SRC_DIR		:=	src
OBJ_DIR		:=	obj
BIN_DIR		:=	bin

ifeq ($(shell uname -s), Darwin)
CXX		:=	clang++
INCLUDE_DIRS	:=	
LIBRARY_DIRS	:=	
DFLAGS		:=	-D__STDC_CONSTANT_MACROS
LDLIBS		:=	`pkg-config --libs-only-l libjpeg libavcodec libavdevice libavfilter libavformat libavutil libpostproc libswresample libswscale sdl2`
else
CXX		:=	$(PREFIX)g++
INCLUDE_DIRS	:=	
LIBRARY_DIRS	:=	
DFLAGS		:=
LDLIBS		:=	`pkg-config --libs-only-l libjpeg libavcodec libavdevice libavfilter libavformat libavutil libswresample libswscale sdl2` -pthread
endif
OPT		:=	-O0 -g
CXXFLAGS	:=	-Wall $(OPT) $(foreach dir,$(INCLUDE_DIRS), -I$(dir)) `pkg-config --cflags libjpeg libavcodec libavdevice libavfilter libavformat libavutil libswresample libswscale sdl2` $(DFLAGS)

LDFLAGS		:=	$(foreach dir,$(LIBRARY_DIRS), -I$(dir)) `pkg-config --libs-only-L libjpeg libavcodec libavdevice libavfilter libavformat libavutil libswresample libswscale sdl2`

TARGET		:=	$(BIN_DIR)/video2tjpeg
TARGET2		:=	$(BIN_DIR)/validatetjpeg

VPATH		:=  $(SRC_DIR):$(OBJ_DIR)
.SUFFIXES:	.cpp .o

all: $(TARGET) $(TARGET2)

$(TARGET): $(OBJ_DIR)/decode_video_jpg.o
	mkdir -p $(dir $@)
	$(CXX) -o $@ $(LDFLAGS) $^ $(LDLIBS)

$(TARGET2): $(OBJ_DIR)/validatetjpeg.o
	mkdir -p $(dir $@)
	$(CXX) -o $@ $(LDFLAGS) $^ $(LDLIBS)


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) -o $@ -c $(CXXFLAGS) $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CXX) -o $@ -c $(CXXFLAGS) $<

.PHONY: clean
clean:
	-rm -rfv $(OBJ_DIR) $(BIN_DIR) $(shell find . -name *~)
