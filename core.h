
/* C networking headers */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>
/* end C networking headers */

/* C++ headers */
#include <cstring>
#include <iostream>
#include <vector>
#include "question.h"
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <chrono>
/* end C++ headers */

#define DBG

#ifdef DBG
#define DEBUG(x) std::cout << x << std::endl;
#else
#define DEBUG(x)
#endif
