.PHONY: all

all:
	cat ../book/chap-1.md | ../../md-patcher/cmake-build-debug/md_patcher
	make st

SOURCES = st.cpp err.cpp parser.cpp invocation.cpp io.cpp
OBJECTS = $(SOURCES:.cpp=.o)

st: $(OBJECTS)
	c++ $^ -o $@
	./st test.st