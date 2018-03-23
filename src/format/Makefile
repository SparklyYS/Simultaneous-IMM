exe = format

GCC = "$(shell which g++)" 
CCFLAGS = -O3

OBJS= $(patsubst %.cpp,%.o,$(wildcard ../../lib/*.cpp)) \
			$(patsubst %.cpp,%.o,$(wildcard *.cpp))

DEPS= $(wildcard ../../lib/*.h) \
				$(wildcard *.h) \
				Makefile

%.o:%.cpp ${DEPS}
	${GCC} -c ${CCFLAGS} $< -o $@

${exe}:${OBJS}
	${GCC} ${CCFLAGS} ${OBJS} ../sfmt/SFMT.c -o ${exe}

clean:
	rm -rf ${exe} ${OBJS}
	rm -rf *.bin
