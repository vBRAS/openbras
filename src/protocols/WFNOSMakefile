#you can use static_libs=1/0 ,to build static lib or dynamiclib
include $(USER_ROOT)/make/makeinclude/macros.GNU
MYLIB=protocols
MYFILES =radius/CRadiusAttribute.cpp\
radius/CRadiusConfig.cpp\
radius/CRadiusConnector.cpp\
radius/CRadiusManager.cpp\
radius/CRadiusMessage.cpp\
radius/CRadiusScheme.cpp\
radius/CRadiusTransaction.cpp\

MYCPPFLAGS= -I../include -std=gnu++11
MYLDFLAGS= 
MYDLLLIB= 
include $(USER_ROOT)/make/makeinclude/allinone.GNU

