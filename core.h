
/* C networking headers */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
/* end C networking headers */

/* C++ headers */
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
