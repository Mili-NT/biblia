OBJ_DIR = obj
$(shell mkdir -p $(OBJ_DIR))

OBJS = $(OBJ_DIR)/biblia_main.o \
       $(OBJ_DIR)/biblia_match.o \
       $(OBJ_DIR)/biblia_ref.o \
       $(OBJ_DIR)/biblia_render.o \
       $(OBJ_DIR)/intset.o \
       $(OBJ_DIR)/strutil.o \
       data/biblia_data.o

$(OBJ_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

CFLAGS += -Wall -g3 -Isrc/
LDLIBS += -lreadline

biblia: $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LDLIBS)

$(OBJ_DIR)/biblia_main.o: src/biblia_main.c src/biblia_config.h src/biblia_data.h src/biblia_match.h src/biblia_ref.h src/biblia_render.h src/strutil.h

$(OBJ_DIR)/biblia_match.o: src/biblia_match.h src/biblia_match.c src/biblia_config.h src/biblia_data.h src/biblia_ref.h

$(OBJ_DIR)/biblia_ref.o: src/biblia_ref.h src/biblia_ref.c src/intset.h src/biblia_data.h

$(OBJ_DIR)/biblia_render.o: src/biblia_render.h src/biblia_render.c src/biblia_config.h src/biblia_data.h src/biblia_match.h src/biblia_ref.h

$(OBJ_DIR)/intset.o: src/intset.h src/intset.c

$(OBJ_DIR)/strutil.o: src/strutil.h src/strutil.c

$(OBJ_DIR)/biblia_data.o: src/biblia_data.h data/kjv_data.c

data/biblia_data.c: data/grb.tsv data/generate.awk src/biblia_data.h
	awk -f data/generate.awk $< > $@

.PHONY: clean
clean:
	rm -rf $(OBJS) biblia
	rm -rf data/biblia_data*
all: biblia