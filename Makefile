include Make_Setup.inc

#optional files to build

OPT_FILES=

#Optional Library Defines
ifeq ($(MYSQL),YES)
OPT_FILES += ucmysql.o
endif

#Optional Library Defines
ifeq ($(CURL),YES)
OPT_FILES += buffer_curl.o uc_curl.o
endif

#Optional Library Defines
ifeq ($(MYSQL),YES)
OPT_FILES += ucsqlite.o
endif

example : libuc.a example.o
	$(LINKXX) -o $@ $^


ucoder_json.cpp : json_parser.lex
	flex json_parser.lex

libuc.a : ucontainer.o buffer.o buffer_util.o ucoder_ini.o ucoder_bin.o \
string_util.o uc_web.o ucio.o ucoder_json.o buffer_curl.o uccontract.o \
ucdb.o $(OPT_FILES)
	rm -f libuc.a
	$(STATICLIB) $@ $^

buffer.o : buffer.h
buffer_util.o : buffer.h
buffer_curl.o : buffer.h
ucontainer.o : ucontainer.h stl_util.h
ucontract.o : uccontainer.h
ucio.o :  ucontainer.h stl_util.h buffer.h ucio.h
ucoder_ini.o : ucontainer.h buffer.h
ucoder_bin.o : ucontainer.h buffer.h
ucoder_json.o : ucontainer.h buffer.h
uc_web.o : ucontainer.h stl_util.h buffer.h ucio.h uc_web.h
ucsqlite.o : ucdb.h ucsqlite.h
ucmysql.o : ucdb.h ucmysql.h
example.o : ucontainer.h ucio.h
 
install:
	install -m644 -o root -g wheel *.h $(INSTALLDIR)/include
	install -m644 -o root -g wheel *.a $(INSTALLDIR)/lib

uninstall:
	rm -f $(INSTALLDIR)/include/buffer.h
	rm -f $(INSTALLDIR)/include/stl_util.h
	rm -f $(INSTALLDIR)/include/string_util.h 
	rm -f $(INSTALLDIR)/include/uc_web.h 
	rm -f $(INSTALLDIR)/include/ucio.h 
	rm -f $(INSTALLDIR)/include/ucdb.h 
	rm -f $(INSTALLDIR)/include/ucmysql.h
	rm -f $(INSTALLDIR)/include/ucontainer.h 
	rm -f $(INSTALLDIR)/include/ucsqlite.h 
	rm -f $(INSTALLDIR)/include/univcont.h        
	rm -f $(INSTALLDIR)/lib/libuc.a

clean :
	rm -f *.o
	rm -f *.a
	rm -f *.gcno
	rm -f example
