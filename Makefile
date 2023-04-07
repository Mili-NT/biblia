OBJ_DIR = obj
$(shell mkdir -p $(OBJ_DIR))

OBJS = $(OBJ_DIR)/kjv_main.o \
       $(OBJ_DIR)/kjv_match.o \
       $(OBJ_DIR)/kjv_ref.o \
       $(OBJ_DIR)/kjv_render.o \
       $(OBJ_DIR)/intset.o \
       $(OBJ_DIR)/strutil.o \
       data/kjv_data.o

$(OBJ_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

CFLAGS += -Wall -Isrc/
LDLIBS += -lreadline

kjv: $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LDLIBS)

$(OBJ_DIR)/kjv_main.o: src/kjv_main.c src/kjv_config.h src/kjv_data.h src/kjv_match.h src/kjv_ref.h src/kjv_render.h src/strutil.h

$(OBJ_DIR)/kjv_match.o: src/kjv_match.h src/kjv_match.c src/kjv_config.h src/kjv_data.h src/kjv_ref.h

$(OBJ_DIR)/kjv_ref.o: src/kjv_ref.h src/kjv_ref.c src/intset.h src/kjv_data.h

$(OBJ_DIR)/kjv_render.o: src/kjv_render.h src/kjv_render.c src/kjv_config.h src/kjv_data.h src/kjv_match.h src/kjv_ref.h

$(OBJ_DIR)/intset.o: src/intset.h src/intset.c

$(OBJ_DIR)/strutil.o: src/strutil.h src/strutil.c

$(OBJ_DIR)/kjv_data.o: src/kjv_data.h data/kjv_data.c

data/kjv_data.c: data/kjv.tsv data/generate.awk src/kjv_data.h
	awk -f data/generate.awk $< > $@

.PHONY: clean
clean:
	rm -rf $(OBJS) kjv
	rm -rf data/kjv_data.c
