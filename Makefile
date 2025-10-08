FLAGS = -lX11
TARGET_DIR = ./build
SRC_DIRS = .
SRC_FILE=main.c
NAME=build

$(TARGET_DIR)/$(NAME) : $(SRC_FILE)
	mkdir -p $(TARGET_DIR)
	gcc -o $(TARGET_DIR)/$(NAME) $(SRC_FILE) ${FLAGS}
