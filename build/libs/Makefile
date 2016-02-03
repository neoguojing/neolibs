#neolib makefile
export NEOLIB_TGT = $(LIB_TARGET_PATH)libneolib.so

$(NEOLIB_TGT):$(LIB_OBJS) 
	@echo "object files:" $(LIB_OBJS)
	#编译程序
	#$(CXX) $(TMP_OBJS) $(CXXFLAGS) -o $(NEOLIB_TGT) -lpthread; \
	#编译动态链接库
	$(CXX) $(LIB_OBJS) $(CXXFLAGS) -fPIC -shared -o $(NEOLIB_TGT) -lpthread ;
	#mv *.o $(OBJECT_PATH)
