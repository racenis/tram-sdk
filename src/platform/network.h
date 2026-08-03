// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_PLATFORM_NETWORK_H
#define TRAM_SDK_PLATFORM_NETWORK_H

#include <cstdint>

namespace tram::Platform::Network {
    void SetProtocolVersion(uint32_t app_id, uint32_t version);

    enum ConnectionStatus {
        DISCONNECTED,
        CONNECTING,
        CONNECTION_FAILED,
        CONNECTED,
        HOSTING
    };

    void Connect(const char* address, uint16_t port);
    void Disconnect();
    void Host(uint16_t port);
    
    ConnectionStatus GetStatus();
    const char* GetStatusMessage();
    
    typedef uint32_t channel_t;
    typedef uint32_t connection_t;
    
    enum : channel_t {
        CHANNEL_CONTROL
    };
    
    enum : uint16_t {
        MESSAGE_CONNECT,
        MESSAGE_DISCONNECT,
        MESSAGE_ACCEPTED,
        MESSAGE_REJECTED,
        MESSAGE_PING
    };
    
    channel_t AddChannel(bool reliable);
    
    void SendMessage(channel_t channel, connection_t connection, uint16_t type, void* data, uint32_t size);
    bool ReceiveMessage(channel_t channel, connection_t& connection, uint16_t& type, void** data, uint32_t& size);
    
    bool GetConnection(connection_t& id);
    
    void Update();
}

#endif // TRAM_SDK_PLATFORM_NETWORK_H