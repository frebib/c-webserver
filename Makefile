CC	 = gcc
CFLAGS = -g -Wall -Werror -std=c11 -D_GNU_SOURCE
LIBS   = -lpcre -lmagic

SRCDIR = src
OBJDIR = obj
BINDIR = bin

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

TARGET = webserver

default: all
all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(CC) -o $@ $(LIBS) $(OBJECTS)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR) $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BINDIR) $(OBJDIR)
	find . -name '*.o' -delete

.PHONY: all clean
