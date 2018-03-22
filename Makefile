exe=imm_discrete

COMMFLAGS=-O3 --compiler-options -Wall -Xptxas -v
CUCC= "$(shell which nvcc)"

CUFLAGS= -arch=sm_35 ${COMMFLAGS} 
CUFLAGS+= -ccbin=g++ -Xcompiler -fopenmp

MPC= "$(shell which mpicxx)"
MPCFLAGS= -Wall -I"$(shell dirname $(CUCC))/../include" -L"$(shell dirname $(CUCC))/../lib64" -lcudart -fopenmp
MPCFLAGS+= -O3 -std=c++0x

ifeq ($(enable_monitor), 1)
	CUFLAGS+= -DENABLE_MONITORING
endif

ifeq ($(enable_check), 1)
	CUFLAGS+= -DENABLE_CHECKING
endif

ifeq ($(enable_groupby), 1)
	CUFLAGS += -DGROUBY
endif 


DEPS=	src/graph.h		\
		src/head.h		\
		src/iheap.h		\
		src/imm.h		\
		src/infgraph.h	\
		src/weibull.h

OBJS=	src/imm.o

%.o : %.cpp ${DEPS}
	${MPC} -c ${MPCFLAGS} $< -o $@

${exe}: ${OBJS} 
	${MPC} ${OBJS} ${MPCFLAGS} src/sfmt/SFMT.c -o ${exe}	

clean:
	rm -rf ${exe} src/*.o