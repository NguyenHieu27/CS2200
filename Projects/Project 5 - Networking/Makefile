# Authored by Christopher Tam for Georgia Tech's CS 2200
TARGET = rtp-client

CC     = gcc
CFLAGS = -Wall -Wextra -Wsign-conversion -Wpointer-arith -Wcast-qual -Wwrite-strings -Wshadow -Wmissing-prototypes -Wpedantic -Wwrite-strings -g -std=gnu99

LFLAGS = -lpthread

SRCDIR = src
INCDIR = $(SRCDIR)
BINDIR = .

SUBMIT_SUFFIX = -networking
SUBMIT_FILES  = $(SRC) $(INC) Makefile rtp-server.py

SRC := $(wildcard $(SRCDIR)/*.c)
INC := $(wildcard $(INCDIR)/*.h)

INCFLAGS := $(patsubst %/,-I%,$(dir $(wildcard $(INCDIR)/.)))

.PHONY: all
all:
	@$(MAKE) release && \
	echo "$$(tput setaf 3)$$(tput bold)Note:$$(tput sgr0) this project compiled with release flags by default. To compile for debugging, please use $$(tput setaf 6)$$(tput bold)make debug$$(tput sgr0)."

.PHONY: debug
debug: CFLAGS += -ggdb -g3 -DDEBUG
debug: $(BINDIR)/$(TARGET)

.PHONY: release
release: CFLAGS += -mtune=native -O2
release: $(BINDIR)/$(TARGET)

.PHONY: clean
clean:
	@rm -f $(BINDIR)/$(TARGET)
	@rm -rf $(BINDIR)/$(TARGET).dSYM

.PHONY: check-username
check-username:
	@if [ -z "$(GT_USERNAME)" ]; then \
		echo "Before running 'make submit', please set your GT Username in the environment"; \
		echo "Run the following to set your username: \"export GT_USERNAME=<your username>\""; \
		exit 1; \
	fi

.PHONY: submit
submit: check-username
	@(tar zcfh $(GT_USERNAME)$(SUBMIT_SUFFIX).tar.gz $(SUBMIT_FILES) && \
	echo "Created submission archive $$(tput bold)$(GT_USERNAME)$(SUBMIT_SUFFIX).tar.gz$$(tput sgr0).") || \
	(echo "$$(tput bold)$$(tput setaf 1)Error:$$(tput sgr0) Failed to create submission archive." && \
	rm -f $(GT_USERNAME)$(SUBMIT_SUFFIX).tar.gz)

$(BINDIR)/$(TARGET): $(SRC) $(INC)
	@mkdir -p $(BINDIR)
	@$(CC) $(CFLAGS) $(INCFLAGS) $(SRC) -o $@ $(LFLAGS)
