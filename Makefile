exe=imm_discrete

COMMFLAGS=-O3 --compiler-options -Wall -Xptxas -v
CUCC= "$(shell which nvcc)"

CUFLAGS= -arch=sm_35 ${COMMFLAGS} 
CUFLAGS+= -ccbin=g++ -Xcompiler -fopenmp

MPC= "$(shell which mpicxx)"
MPCFLAGS= -Wall -I"$(shell dirname $(CUCC))/../include" -L"$(shell dirname $(CUCC))"/../lib64" -lcudart -fopenmp

ifeq ($(enable_monitor), 1)
	CUFLAGS+= -DENABLE_MONITORING
endif

ifeq ($(enable_check), 1)
	CUFLAGS+= -DENABLE_CHECKING
endif

ifeq ($(enable_groupby), 1)
	CUFLAGS += -DGROUBY
endif 

#imm_continuous: src/*.cpp src/*.h
#	g++ -DCONTINUOUS src/imm.cpp -Wall -std=c++0x -O3 src/sfmt/SFMT.c  -o imm_continuous

#%.o : %.cpp ${DEPS}
#	${MPC} -c $< -Wall -std=c++0x -o $@

#${exe}: ${OBJS} 
#	${MPC} ${OBJS} -Wall -std=c++0x -O3 src/sfmt/SFMT.c -o ${exe}	


${exe}: src/*.cpp src/*.h
	${MPC} src/imm.cpp -Wall -std=c++0x -O3 src/sfmt/SFMT.c  -o ${exe}

clean:
	rm -rf ${exe} src/*.o