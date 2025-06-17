CC = gcc
CLFAGS = -Wall -Wextra -Werror -pedantic -std=c11 -g -Iinclude
OUTDIR = build
CPPCHECK = cppcheck --enable=performance,unusedFunction --error-exitcode=1 --include=include
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, $(OUTDIR)/%.o, $(SRCS))
EXE = http_server

all: check $(OUTDIR) $(EXE)

$(OUTDIR):
	@mkdir -p $(OUTDIR)

check:
	$(CPPCHECK) $(SRCS)

$(EXE): $(OBJS)
	@echo "Linking $@ ..."
	$(CC) $(CLFAGS) -o $@ $^

$(OUTDIR)/%.o: src/%.c
	@echo "Compiling $< ..."
	$(CC) $(CLFAGS) -c -o $@ $<

PHONY: clean
clean:
	rm -rf $(OUTDIR) $(EXE)
