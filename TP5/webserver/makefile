CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Wpedantic \
          -Wformat=2 -Wno-unused-parameter -Wshadow -Wno-discarded-qualifiers \
          -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
          -Wredundant-decls -Wnested-externs -Wmissing-include-dirs \
			 -Wjump-misses-init -Wlogical-op -g -O3 -D_XOPEN_SOURCE=700

LFLAGS = -lssl -lcrypto -lpq
INCLUDE = -I./include

FILES = lib \
		  linked_list \
		  tree \
		  http_tree \
		  parsing \
		  logger \
		  db \
		  linpack \
		  http

FILES_SSL = ssl \
				$(FILES)

OBJECTS = $(addsuffix .o, $(FILES_SSL))
OBJECTS_NO_SSL = $(addsuffix -no-ssl.o, $(FILES))

SRC_DIR = src
BUILD_DIR = build

OBJECTS_DIR = $(addprefix $(BUILD_DIR)/, $(OBJECTS))
OBJECTS_DIR_NO_SSL = $(addprefix $(BUILD_DIR)/, $(OBJECTS_NO_SSL))

SSL = -DUSE_SSL
MAIN_OUTPUT = http
LOGGING = -DLOGGING=0

all: http

dir:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(LOGGING) $(SSL) $(CFLAGS) -c $< -o $@ $(INCLUDE)

$(BUILD_DIR)/%-no-ssl.o: $(SRC_DIR)/%.c
	$(CC) $(LOGGING) $(CFLAGS) -c $< -o $@ $(INCLUDE) $(LFLAGS)

http: dir $(OBJECTS_DIR)
	$(CC) $(LOGGING) $(SSL) $(CFLAGS) -o $(MAIN_OUTPUT) $(SRC_DIR)/main.c $(OBJECTS_DIR) $(INCLUDE) $(LFLAGS)

http-no-ssl: dir $(OBJECTS_DIR_NO_SSL)
	$(CC) $(LOGGING) $(CFLAGS) -o $(MAIN_OUTPUT) $(SRC_DIR)/main.c $(OBJECTS_DIR_NO_SSL) $(INCLUDE) $(LFLAGS)

docker-image:
	docker build -t http-server .

clean:
	rm -rf logs $(MAIN_OUTPUT) $(BUILD_DIR)