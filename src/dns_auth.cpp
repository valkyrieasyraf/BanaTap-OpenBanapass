#include "dns_auth.h"
#include <algorithm>
#include <cctype>
#include <windows.h>

DNSAuthenticator::DNSAuthenticator() : initialized(false) {
    loadFromConfig(".\\dns_whitelist.ini");
    
    if (allowedDomains.empty()) {
        // Default domains if config not found
        addAllowedDomain("game.example.com");
        addAllowedDomain("api.example.com");
        addAllowedDomain("auth.example.com");
    }
    
    initialized = true;
}

void DNSAuthenticator::addAllowedDomain(const std::string& domain) {
    std::string lowerDomain = domain;
    std::transform(
        lowerDomain.begin(), 
        lowerDomain.end(), 
        lowerDomain.begin(), 
        ::tolower
    );
    
    // Check for duplicates
    if (std::find(allowedDomains.begin(), allowedDomains.end(), lowerDomain) == allowedDomains.end()) {
        allowedDomains.push_back(lowerDomain);
    }
}

void DNSAuthenticator::loadFromConfig(const char* configPath) {
    int count = GetPrivateProfileIntA("domains", "count", 0, configPath);
    
    if (count <= 0) return;
    
    for (int i = 1; i <= count; i++) {
        char keyName[32];
        char domain[256] = {0};
        sprintf_s(keyName, sizeof(keyName), "domain%d", i);
        
        GetPrivateProfileStringA("domains", keyName, "", domain, sizeof(domain), configPath);
        
        if (domain[0] != '\0') {
            addAllowedDomain(domain);
        }
    }
}

bool DNSAuthenticator::isAuthorized(const std::string& hostname) {
    std::string lowerHost = hostname;
    std::transform(
        lowerHost.begin(), 
        lowerHost.end(), 
        lowerHost.begin(), 
        ::tolower
    );
    
    for (const auto& domain : allowedDomains) {
        if (lowerHost == domain) {
            return true;
        }
        
        // Also check for subdomain matching (e.g., sub.domain.com matches domain.com)
        if (lowerHost.length() > domain.length()) {
            if (lowerHost.substr(lowerHost.length() - domain.length()) == domain &&
                lowerHost[lowerHost.length() - domain.length() - 1] == '.') {
                return true;
            }
        }
    }
    
    return false;
}

bool DNSAuthenticator::validateRequest(const std::string& requestHost) {
    if (requestHost.empty()) {
        return false;
    }
    
    // Extract hostname (remove port if present)
    std::string hostname = requestHost;
    size_t colonPos = hostname.find(':');
    
    if (colonPos != std::string::npos) {
        hostname = hostname.substr(0, colonPos);
    }
    
    // Remove whitespace
    hostname.erase(0, hostname.find_first_not_of(" \t\r\n"));
    hostname.erase(hostname.find_last_not_of(" \t\r\n") + 1);
    
    // Check if hostname is in whitelist
    return isAuthorized(hostname);
}

int DNSAuthenticator::getWhitelistCount() const {
    return allowedDomains.size();
}
