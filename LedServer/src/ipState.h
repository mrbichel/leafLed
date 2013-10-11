//
//  ipState.h
//  LedServer
//
//  Created by obscura on 01/08/13.
//
//

#ifndef __LedServer__ipState__
#define __LedServer__ipState__

#include "ofMain.h"
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>

class ipState {
    
    
public:
    
    // methods, equivalent to specific functions of your class objects
    void update();
    void draw(); // draw method, this where you'll do the object's drawing
    
    // variables
    float x; // position
    float y;
    
    string myTextResponse;
    
    ipState(); // constructor - used to initialize an object, if no properties are passed the program sets them to the default value
    
    
};

#endif /* defined(__LedServer__ipState__) */
