#TRAY_CFLAGS := -DTRAY_APPINDICATOR=1  -D_GLIBCXX_USE_CXX11_ABI=0 $(shell pkg-config --cflags appindicator3-0.1 gtk+-3.0)
TRAY_CPPFLAGS := -DTRAY_APPINDICATOR=1 -D_GLIBCXX_USE_CXX11_ABI=0 $(shell pkg-config --cflags appindicator3-0.1 gtk+-3.0)
TRAY_LDFLAGS := $(shell pkg-config --libs appindicator3-0.1 gtk+-3.0) -I/usr/include -lX11 -lxkbfile

INSTALL_BIN := /usr/bin
INSTALL_BASEPATH := /opt/simple_xkb_tray_icon
BIN_FNAME := simple_xkb_tray_icon

CFLAGS := -g -Wall $(TRAY_CFLAGS) -Wall -Wextra -pedantic
CPPFLAGS := -g -Wall $(TRAY_CPPFLAGS) -Wall -Wextra -pedantic
LDFLAGS := -g $(TRAY_LDFLAGS)

CC := g++

all: simple_xkb_tray_icon
simple_xkb_tray_icon: simple_xkb_tray_icon.o
	$(CC) $^ $(LDFLAGS) -o $@
	
simple_xkb_tray_icon.o: simple_xkb_tray_icon.cpp

clean:
	rm -f simple_xkb_tray_icon.o simple_xkb_tray_icon simple_xkb_tray_icon.exe

install:
	mkdir -p ${INSTALL_BASEPATH}
	install -d ${INSTALL_BASEPATH}/icons
	install -m 644 icons/*.* ${INSTALL_BASEPATH}/icons/
	install -m 755 $(BIN_FNAME) ${INSTALL_BASEPATH}/
	ln -sf ${INSTALL_BASEPATH}/$(BIN_FNAME) $(INSTALL_BIN)/$(BIN_FNAME)

uninstall:
	rm -f $(INSTALL_BIN)/$(BIN_FNAME)
	rm -rf ${INSTALL_BASEPATH}
