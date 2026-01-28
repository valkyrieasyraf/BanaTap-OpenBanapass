#pragma once
#include <vector>
#include <string>
#include <winsock2.h>
#include <algorithm>
#pragma comment(lib, "ws2_32.lib")

class DNSAuthenticator {
private:
    std::vector<std::string> allowedDomains;
    bool initialized;
    
public:
    DNSAuthenticator();
    
    // Add a domain to the whitelist
    void addAllowedDomain(const std::string& domain);
    
    // Load whitelist from configuration file
    void loadFromConfig(const char* configPath);
    
    // Check if a given hostname is authorized
    bool isAuthorized(const std::string& hostname);
    
    // Validate the request host against whitelist
    bool validateRequest(const std::string& requestHost);
    
    // Get current whitelist (for debugging)
    int getWhitelistCount() const;
};