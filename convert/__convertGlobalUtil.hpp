#ifndef __CONVERT_GLOBAL_UTIL
#define __CONVERT_GLOBAL_UTIL

#include <bits/stdc++.h>
using namespace std;

typedef int Status;
#define STATUS_OKE 0
#define STATUS_ERR 1

#ifndef REPT
    #define REPT(type, i, a, b) for(type i = (a); (i) < (b); ++(i))
#endif

#ifndef REVT
    #define REVT(type, i, a, b) for(type i = (a); (i) > (b); --(i))
#endif

#ifndef REP
    #define REP(i, a, b) for(int i = (a); (i) < (b); ++(i))
#endif

#ifndef REV
    #define REV(i, a, b) for(int i = (a); (i) > (b); --(i))
#endif

template<typename... Args>
static string concat(Args const&... args) {
    stringstream ss;
    (ss << ... << args); 
    return ss.str();
}


#endif