PROJ   = WebCrawler
OBJS   = StrKit.o       \
         Log.o          \
         Configurator.o \
         MultiIo.o      \
         PluginMngr.o   \
         Hash.o         \
         BloomFilter.o  \
         Url.o          \
         UrlQueues.o    \
         Socket.o       \
         Thread.o       \
         DnsThread.o    \
         SendThread.o   \
         StatThread.o   \
         RecvThread.o   \
         WebCrawler.o   \
         Main.o
CXX    = g++
LINK   = g++
RM     = rm -rf
CFLAGS = -c -g -Wall -I. -D_DEBUG
LIBS   = -ldl -lpthread #-levent

$(PROJ): $(OBJS)
	$(LINK) $^ $(LIBS) -o ../bin/$@

.cpp.o:
	$(CXX) $(CFLAGS) $^

clean:
	$(RM) ../bin/$(PROJ) $(OBJS) *.gch
