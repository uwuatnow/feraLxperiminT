CXX := ccache g++
BUILD_DIR := build
SRC_DIR := src
PCH_SRC := $(SRC_DIR)/Game/pch.h
PCH_GCH := $(BUILD_DIR)/pch.gch

CXXFLAGS := -DDEBUG -g3 -std=c++17 -Wall -Wextra -Wpedantic -Wfatal-errors -Wnon-virtual-dtor \
            -Wno-stringop-overflow -Wno-unused-parameter -Wno-sequence-point -fopenmp \
            -Wno-class-memaccess -Werror -Isrc -Iinclude -include $(PCH_SRC) \
            -fno-omit-frame-pointer -Wno-unused-but-set-variable -Wno-unused-variable
LDFLAGS := -lsfml-system -lsfml-window -lsfml-graphics -lopengl32 -lglu32 -ldbghelp -Wl,--export-all-symbols -lgomp

EXE := $(BUILD_DIR)/feraL_xperiminT

SRC_FILES := $(wildcard $(SRC_DIR)/*/*.cpp) $(wildcard $(SRC_DIR)/*/*/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SRC_FILES:.cpp=.o))

BUILD_DIRS := $(sort $(dir $(OBJ_FILES)))

ifeq ($(OS),Windows_NT)
    MKDIR = if not exist "$@" mkdir "$@"
    RM = rmdir /s /q
    RM_FILE = del /q
else
    MKDIR = mkdir -p "$@"
    RM = rm -rf
    RM_FILE = rm -f
endif

# Default goal
all: $(EXE)

$(BUILD_DIRS):
	@$(MKDIR)

-include $(OBJ_FILES:.o=.d)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIRS) $(PCH_GCH)
	@echo $(notdir $<)
	@$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(PCH_GCH): $(PCH_SRC)
	@$(CXX) $(CXXFLAGS) -x c++-header $< -o $@

build: $(BUILD_DIRS) $(PCH_GCH) $(OBJ_FILES)

$(EXE): build
	@echo Linking: ./$@
	@$(CXX) $(OBJ_FILES) -o $(EXE) $(LDFLAGS)

run: all
	./$(EXE)

clean:
	-@if exist $(BUILD_DIR) $(RM) $(BUILD_DIR)
	-@if exist $(PCH_GCH) $(RM_FILE) $(PCH_GCH)

.PHONY: all build run clean $(BUILD_DIRS)
