#app makefile
export APP_TGT = $(TARGET_PATH)neolibtest.exe

$(APP_TGT):$(APP_OBJS) 
	@echo "object files:" $(APP_OBJS)
	$(CXX) $(APP_OBJS) $(LIB_OBJS) $(CXXFLAGS) -L$(LIB_TARGET_PATH) -I$(LIB_PATH) -o $(APP_TGT)
        #$(CXX) $(TMP_OBJS) $(CXXFLAGS) -L$(LIB_TARGET_PATH) -I$(PUB_INCLUDE) -l:libneolib.so -o $(APP_TGT)
	#$(CXX) $(TMP_OBJS) $(CXXFLAGS) -L$(LIB_TARGET_PATH) -I$(PUB_INCLUDE) -lneolib -Wl,-rpath=$(LIB_TARGET_PATH) -o 
