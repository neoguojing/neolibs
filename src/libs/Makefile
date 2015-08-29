#neolib makefile
export NEOLIB_TGT = $(LIB_TARGET_PATH)neolib
export SOURCES= $(wildcard *.cpp)

TMP_OBJS = $(patsubst %.cpp, %.o,$(SOURCES))
export OBJS = $(addprefix $(OBJECT_PATH),$(TMP_OBJS))

#.PHONY :test
#test:
#	@echo $(SOURCES)
#	@echo "object files:" $(TMP_OBJS)
#	@echo "object files:" $(OBJS)

NEOLIB_TGT:$(TMP_OBJS) 
	@echo "source files:" $(SOURCES)
	@echo "object files:" $(TMP_OBJS)
	$(CXX) $(TMP_OBJS) $(CXXFLAGS) -o $(NEOLIB_TGT) -lpthread; \
	mv *.o $(OBJECT_PATH)
		   
#sinclude $(SOURCES:.cpp=.d)
sinclude $(addprefix $(OBJECT_PATH),$(SOURCES:.cpp=.d))

#include $(BLD_PATH)relation.gen
#%d: %cpp
$(addprefix $(OBJECT_PATH),%d): %cpp
	@echo "create depend"
	$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; \
	#sed -i 's,\($*\)\.o,$(OBJECT_PATH)\1.o ,g' $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	
	#sed 's,main.o,/home/neo/workspace/neogithub/neolibs.git/trunk/build/libs/main.o ,g' < main.d > tmpmain.d


