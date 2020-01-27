ifeq ($(OSTYPE),cygwin)
	CLEANUP=rm -f
	MKDIR=mkdir -p
	TARGET_EXTENSION=out
else ifeq ($(OS),Windows_NT)
    ifeq ("$(SHELL)","/bin/sh")
	CLEANUP=rm -f
	MKDIR=mkdir -p
	TARGET_EXTENSION=exe
    else
	CLEANUP=del /F /Q
	MKDIR=mkdir
	TARGET_EXTENSION=exe
    endif
else
	CLEANUP=rm -f
	MKDIR=mkdir -p
	TARGET_EXTENSION=out
endif

PATHU = unity/src/
PATHX = unity/extras/fixture/src/
PATHS = src/
PATHI = includes/c-helper-macros/
PATHT = test/
PATHB = build/
PATHD = build/depends/
PATHO = build/objs/
PATHR = build/results/

BUILD_PATHS = $(PATHB) $(PATHD) $(PATHO) $(PATHR)

SRCT = $(wildcard $(PATHT)*.c)

COMPILE=gcc -c
LINK=gcc
DEPEND=gcc -MM -MG -MF
CFLAGS=-I. -I$(PATHU) -I$(PATHX) -I$(PATHS) -I$(PATHI)

ifdef UMM_FIRST_FIT
	CFLAGS += -DUMM_FIRST_FIT
endif

ifdef UMM_TEST_BUILD
	CFLAGS += -DUMM_TEST_BUILD -DUMM_INFO
endif

ifdef UMM_INFO
	CFLAGS += -DUMM_INFO
endif

ifdef UMM_METRICS
	CFLAGS += -DUMM_METRICS
endif

ifdef UMM_INTEGRITY
	CFLAGS += -DUMM_INTEGRITY
endif

ifdef UMM_POISON
	CFLAGS += -DUMM_POISON
endif

RESULTS = $(patsubst $(PATHT)test_%.c,$(PATHR)test_%.txt,$(SRCT))

$(PATHR)%.txt: $(PATHB)%.$(TARGET_EXTENSION)
	-./$< > $@ 2>&1

$(PATHB)test_%.$(TARGET_EXTENSION): $(PATHO)test_%.o $(PATHO)%.o $(PATHU)unity.o $(PATHX)unity_fixture.o
	$(LINK) -o $@ $^

$(PATHO)%.o:: $(PATHT)%.c
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHO)%.o:: $(PATHS)%.c
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHO)%.o:: $(PATHU)%.c $(PATHU)%.h
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHO)%.o:: $(PATHX)%.c $(PATHX)%.h
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHD)%.d:: $(PATHT)%.c
	$(DEPEND) $@ $<

$(PATHB):
	$(MKDIR) $(PATHB)

$(PATHD):
	$(MKDIR) $(PATHD)

$(PATHO):
	$(MKDIR) $(PATHO)

$(PATHR):
	$(MKDIR) $(PATHR)

clean:
	$(CLEANUP) $(PATHO)*.o
	$(CLEANUP) $(PATHB)*.$(TARGET_EXTENSION)
	$(CLEANUP) $(PATHR)*.txt

testoptions:
	make clean
	make test UMM_TEST_BUILD=1 UMM_INFO=1 UMM_FIRST_FIT=1 UMM_METRICS=1 UMM_INTEGRITY=1 UMM_POISON=1
	make clean
	make test UMM_TEST_BUILD=1 UMM_INFO=1 UMM_FIRST_FIT=1 UMM_METRICS=1 UMM_INTEGRITY=1
	make clean
	make test UMM_TEST_BUILD=1 UMM_INFO=1 UMM_FIRST_FIT=1 UMM_METRICS=1                 UMM_POISON=1
	make clean
	make test UMM_TEST_BUILD=1 UMM_INFO=1 UMM_FIRST_FIT=1 UMM_METRICS=1
	make clean
	make test UMM_TEST_BUILD=1 UMM_INFO=1 UMM_FIRST_FIT=1               UMM_INTEGRITY=1 UMM_POISON=1
	make clean
	make test UMM_TEST_BUILD=1 UMM_INFO=1 UMM_FIRST_FIT=1               UMM_INTEGRITY=1
	make clean
	make test UMM_TEST_BUILD=1 UMM_INFO=1 UMM_FIRST_FIT=1                               UMM_POISON=1
	make clean
	make test UMM_TEST_BUILD=1 UMM_INFO=1 UMM_FIRST_FIT=1
	make clean
	make test UMM_TEST_BUILD=1 UMM_INFO=1                 UMM_METRICS=1 UMM_INTEGRITY=1 UMM_POISON=1
	make clean
	make test UMM_TEST_BUILD=1 UMM_INFO=1                 UMM_METRICS=1 UMM_INTEGRITY=1
	make clean
	make test UMM_TEST_BUILD=1 UMM_INFO=1                 UMM_METRICS=1                 UMM_POISON=1
	make clean
	make test UMM_TEST_BUILD=1 UMM_INFO=1                 UMM_METRICS=1
	make clean
	make test UMM_TEST_BUILD=1 UMM_INFO=1                               UMM_INTEGRITY=1 UMM_POISON=1
	make clean
	make test UMM_TEST_BUILD=1 UMM_INFO=1                               UMM_INTEGRITY=1
	make clean
	make test UMM_TEST_BUILD=1 UMM_INFO=1                                               UMM_POISON=1
	make clean
	make test UMM_TEST_BUILD=1 UMM_INFO=1
	make clean
	make test UMM_TEST_BUILD=1            UMM_FIRST_FIT=1 UMM_METRICS=1 UMM_INTEGRITY=1 UMM_POISON=1
	make clean
	make test UMM_TEST_BUILD=1            UMM_FIRST_FIT=1 UMM_METRICS=1 UMM_INTEGRITY=1
	make clean
	make test UMM_TEST_BUILD=1            UMM_FIRST_FIT=1 UMM_METRICS=1                 UMM_POISON=1
	make clean
	make test UMM_TEST_BUILD=1            UMM_FIRST_FIT=1 UMM_METRICS=1
	make clean
	make test UMM_TEST_BUILD=1            UMM_FIRST_FIT=1               UMM_INTEGRITY=1 UMM_POISON=1
	make clean
	make test UMM_TEST_BUILD=1            UMM_FIRST_FIT=1               UMM_INTEGRITY=1 UMM_POISON=1
	make clean
	make test UMM_TEST_BUILD=1            UMM_FIRST_FIT=1                               UMM_POISON=1
	make clean
	make test UMM_TEST_BUILD=1            UMM_FIRST_FIT=1                               UMM_POISON=1
	make clean
	make test UMM_TEST_BUILD=1                            UMM_METRICS=1 UMM_INTEGRITY=1 UMM_POISON=1
	make clean
	make test UMM_TEST_BUILD=1                            UMM_METRICS=1 UMM_INTEGRITY=1
	make clean
	make test UMM_TEST_BUILD=1                            UMM_METRICS=1                 UMM_POISON=1
	make clean
	make test UMM_TEST_BUILD=1                            UMM_METRICS=1
	make clean
	make test UMM_TEST_BUILD=1                                          UMM_INTEGRITY=1 UMM_POISON=1
	make clean
	make test UMM_TEST_BUILD=1                                          UMM_INTEGRITY=1 UMM_POISON=1
	make clean
	make test UMM_TEST_BUILD=1                                                          UMM_POISON=1
	make clean
	make test UMM_TEST_BUILD=1                                                          UMM_POISON=1

.PRECIOUS: $(PATHB)test_%.$(TARGET_EXTENSION)
.PRECIOUS: $(PATHD)%.d
.PRECIOUS: $(PATHO)%.o

.PHONY: clean
.PHONY: testoptions
.PHONY: test

test: $(BUILD_PATHS) $(RESULTS)
	@echo "OSTYPE:$(OSTYPE) OS:$(OS) SHELL:$(SHELL)"
	@echo "-----------------------\nIGNORES:\n-----------------------"
	@echo `grep -s IGNORE $(PATHR)*.txt`
	@echo "-----------------------\nFAILURES:\n-----------------------"
	@echo `grep -s FAIL $(PATHR)*.txt`
	@echo "\nDONE"

