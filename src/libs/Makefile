#neolib makefile
TMP_TARGET:$(LIB_OBJS)
	@echo "generate lib objects"

sinclude $(addprefix $(OBJECT_PATH),$(LIB_SOURCES:.cpp=.d))

include $(BLD_PATH)relation.gen


