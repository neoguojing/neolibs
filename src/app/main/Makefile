#app makefile

TMP_TARGET:$(APP_OBJS)
	@echo "generate app objects"

sinclude $(addprefix $(OBJECT_PATH),$(APP_SOURCES:.cpp=.d))

include $(BLD_PATH)relation.gen


