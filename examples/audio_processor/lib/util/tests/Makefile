# This is meant to be run from the util/ dir
# make -f tests/Makefile

TEST_DIR ?= tests
DOCTESTHEADER_DIR = $(TEST_DIR)
TEST_SOURCES = $(TEST_DIR)/doctest.cc
TEST_SOURCES += $(wildcard *.cc)
TEST_SOURCES += $(wildcard $(TEST_DIR)/*_test.cc)
TEST_SOURCES += $(wildcard $(TEST_DIR)/*_test.cpp)
TEST_SOURCES += $(wildcard $(TEST_DIR)/*_tests.cc)
TEST_SOURCES += $(wildcard $(TEST_DIR)/*_tests.cpp)
TEST_SOURCES += $(wildcard $(TEST_DIR)/test_*.cc)
TEST_SOURCES += $(wildcard $(TEST_DIR)/test_*.cpp)

BUILDDIR = $(TEST_DIR)/build

CXXFLAGS = 	-Wall \
		 	-std=gnu++2a \
			-I. \
			-I$(TEST_DIR) \
			-Wno-unused-const-variable \
			-DTESTPROJECT \

### Boilerplate below here:

CXX = clang++
LDFLAGS = -lstdc++
OBJECTS = $(addprefix $(BUILDDIR)/, $(subst ../,,$(addsuffix .o, $(basename $(TEST_SOURCES)))))
DEPDIR := $(BUILDDIR)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$(subst ../,,$(basename $<).d)
TMPFILE = $(BUILDDIR)/runtests.out

.PHONY: all tests clean

all: $(DOCTESTHEADER_DIR)/doctest.h tests

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(info Building $<)
	@$(CC) -c $(DEPFLAGS) $(CFLAGS) $< -o $@

$(BUILDDIR)/%.o: %.cc
	@mkdir -p $(dir $@)
	$(info Building $<)
	@$(CXX) -c $(DEPFLAGS) $(CXXFLAGS) $< -o $@

$(BUILDDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(info Building $<)
	@$(CXX) -c $(DEPFLAGS) $(CXXFLAGS) $< -o $@

tests: $(BUILDDIR)/runtests
	@$(BUILDDIR)/runtests --out=$(TMPFILE) && echo "[√] Unit tests passed: $(notdir $(PWD))" || cat $(TMPFILE)

$(BUILDDIR)/runtests: $(OBJECTS)
	@$(CXX) $(LDFLAGS) -o $@ $(OBJECTS)

$(DOCTESTHEADER_DIR)/doctest.h:
	wget https://raw.githubusercontent.com/onqtam/doctest/master/doctest/doctest.h -P $(DOCTESTHEADER_DIR)/

clean:
	rm -rf $(BUILDDIR)

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(addprefix $(DEPDIR)/, $(addsuffix .d, $(basename $(TEST_SOURCES))))
$(DEPFILES):

include $(wildcard $(DEPFILES))

