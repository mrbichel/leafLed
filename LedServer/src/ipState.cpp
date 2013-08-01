//
//  ipState.cpp
//  LedServer
//
//  Created by obscura on 01/08/13.
//
//

#include "ipState.h"

ipState::ipState()
{
    // Call update() on init
    update();
}

void ipState::update()
{
    // update function
    string output = "NOT FOUND";
    string outputWIFI = "NOT FOUND";
    struct ifaddrs *myaddrs;
    struct ifaddrs *ifa;
    struct sockaddr_in *s4;
    int status;
    
    char buf[64];
    
    status = getifaddrs(&myaddrs);
    //status = getifmaddrs(&myaddrs);
    if (status != 0)
    {
        perror("getifaddrs");
        
    }
    
    for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL) continue;
        if ((ifa->ifa_flags & IFF_UP) == 0) continue;
        
        if (ifa->ifa_addr->sa_family == AF_INET)
        {
            s4 = (struct sockaddr_in *)(ifa->ifa_addr);
            if (inet_ntop(ifa->ifa_addr->sa_family, (void *)&(s4->sin_addr), buf, sizeof(buf)) == NULL)
            {
                printf("%s: inet_ntop failed!\n", ifa->ifa_name);
            }
            else
            {
                if(ofToString(ifa->ifa_name) == "en0")
                {
                    output = ofToString(buf);
                }
                
                if(ofToString(ifa->ifa_name) == "en1")
                {
                    outputWIFI = ofToString(buf);
                }
            }
        }
    }
    
    freeifaddrs(myaddrs);
    myTextResponse = output+"-"+outputWIFI;
    
}

void ipState::draw()
{
    // draw function
    // string output = update();
    ofDrawBitmapString("LOCAL IP ADDRESSES:" , x+20, y+20);
    ofDrawBitmapString("EN0 - " + ofSplitString(myTextResponse, "-")[0] , x+20, y+40);
    ofDrawBitmapString("EN1 - " + ofSplitString(myTextResponse, "-")[1] , x+20, y+60);
}