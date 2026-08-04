// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <platform/network.h>

#include <framework/logging.h>

#include <thread>
#include <vector>
#include <set>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <mutex>
#include <shared_mutex>

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#undef ERROR
#undef SendMessage

namespace tram::Platform::Network {

const char* fmt_addr(uint32_t address) {
    static thread_local char buffer[INET_ADDRSTRLEN];
    struct in_addr addrstr;
    addrstr.S_un.S_addr = address;
    inet_ntop(AF_INET, &addrstr, buffer, INET_ADDRSTRLEN);
    return buffer;
}

struct channel_info {
    bool reliable;
};

static channel_info channel_infos[128] = {{.reliable = true}}; // default channels
static channel_t channel_count = 1;

struct ReceivedMessage {
    uint16_t length;
    uint16_t type;
    char* data;
};

struct ReceivedDatagram {
    std::vector<ReceivedMessage> messages;
    uint32_t index = 0;
    
    bool operator< (const ReceivedDatagram& other) const {
        return index < other.index;
    }
};

struct ReceivedChannel {
    std::set<uint32_t> ackable;
    uint32_t received_until = 0;
    std::set<uint32_t> received_indices;
    char* clean_up = nullptr;
    std::priority_queue<ReceivedDatagram> received_datagrams;
};

struct SentDatagram {
    uint32_t index;
    uint32_t tick_last_sent;
    uint32_t tick_since_first_sent;
    int32_t length;
    char data[1500];
};

struct SentMessage {
    uint16_t type;
    uint16_t length;
    char data[1500];
};

struct SentChannel {
    std::unordered_map<uint32_t, SentDatagram> sent_datagrams;
    std::vector<SentMessage> outgoing_queue;
    uint32_t last_index = 1;
};

struct datagram_header {
    char marker[8];
    uint8_t message_count;
    uint8_t ack_count;
    uint16_t channel;
    uint16_t sender;
    char padding[2];
    uint32_t index;
};

struct message_header {
    uint16_t type;
    uint16_t length;
};

struct ack_header {
    uint16_t channel;
    uint32_t offset;
    uint32_t mask;
};

struct message_disconnect {
    uint16_t padding;
};

static uint32_t outgoing_index = 0;
const int32_t DATAGRAM_LIMIT = 1200;

class Connection {
public:
    virtual void Yeet() = 0;
    
    virtual void SendMessage(channel_t channel, uint16_t type, void* data, uint32_t size) = 0;
    virtual bool ReceiveMessage(channel_t channel, uint16_t& type, void** data, uint32_t& size) = 0;
    
    virtual void RegisterReceived(const char* data, int32_t length) = 0;
    
    virtual void SendImmediate(const SentDatagram& datagram) = 0;
    
    uint32_t DispatchMessages() {
        std::vector<ack_header> acks;
        for (channel_t c = 0; c < channel_count; c++) {
            std::unique_lock<std::shared_mutex> lock(*channel_mutex[c]);
            for (auto& ack : channels[c].ackable) {
                if (acks.size() && acks.back().channel == c && ack - acks.back().offset < 32) {
                    acks.back().mask |= 1 << (ack - acks.back().offset);
                } else {
                    ack_header ackh;
                    ackh.channel = c;
                    ackh.offset = ack;
                    ackh.mask = 1;
                    acks.push_back(ackh);
                }
            }
            
            channels[c].ackable.clear();
        }
        
        std::vector<SentDatagram> dispatch_queue;
        for (channel_t c = 0; c < channel_count; c++) {
            std::unique_lock<std::shared_mutex> lock(*channel_mutex[c]);
            if (!channels_out[c].outgoing_queue.size()) continue;
            SentDatagram datagram;
            datagram.index = channels_out[c].last_index++;
            datagram.tick_last_sent = GetTick();
            datagram.tick_since_first_sent = GetTick();
            
            const bool reliable = channel_infos[c].reliable;
            
            int32_t offset = 0;
            datagram_header* dheader = (datagram_header*)&datagram.data[offset];
            strcpy(dheader->marker, "TRAMSDK");
            dheader->message_count = 0;
            dheader->ack_count = 0;
            dheader->sender = outgoing_index;
            dheader->index = datagram.index;
            
            offset += sizeof(datagram_header);
            
            while (acks.size() && offset + sizeof(ack_header) < DATAGRAM_LIMIT) {
                ack_header* header = (ack_header*)&datagram.data[offset];
                *header = acks.back();
                dheader->ack_count++;
                
                acks.pop_back();
                
                offset += sizeof(ack_header);
            }
            
            std::reverse(channels_out[c].outgoing_queue.begin(), channels_out[c].outgoing_queue.end());
            
            while (channels_out[c].outgoing_queue.size() && offset + sizeof(message_header) + channels_out[c].outgoing_queue.back().length < DATAGRAM_LIMIT) {
                message_header* header = (message_header*)&datagram.data[offset];
                header->type = channels_out[c].outgoing_queue.back().type;
                header->length = channels_out[c].outgoing_queue.back().length;
                memcpy(&datagram.data[offset + sizeof(message_header)], channels_out[c].outgoing_queue.back().data, channels_out[c].outgoing_queue.back().length);
                
                dheader->message_count++;
                
                offset += sizeof(message_header) + channels_out[c].outgoing_queue.back().length;
                
                channels_out[c].outgoing_queue.pop_back();
            }
            
            std::reverse(channels_out[c].outgoing_queue.begin(), channels_out[c].outgoing_queue.end());
            
            datagram.length = offset;
            dispatch_queue.push_back(datagram);
            if (reliable) channels_out[c].sent_datagrams[datagram.index] = datagram;
        }
        
        while (acks.size()) {
            SentDatagram datagram;
            datagram.index = channels_out[0].last_index++;
            datagram.tick_last_sent = GetTick();
            datagram.tick_since_first_sent = GetTick();
            
            int32_t offset = 0;
            datagram_header* dheader = (datagram_header*)&datagram.data[offset];
            strcpy(dheader->marker, "TRAMSDK");
            dheader->message_count = 0;
            dheader->ack_count = 0;
            dheader->sender = outgoing_index;
            dheader->index = datagram.index;
            
            offset += sizeof(datagram_header);
            
            while (acks.size() && offset + sizeof(ack_header) < DATAGRAM_LIMIT) {
                ack_header* header = (ack_header*)&datagram.data[offset];
                *header = acks.back();
                dheader->ack_count++;
                
                acks.pop_back();
                
                offset += sizeof(ack_header);
            }
            
            datagram.length = offset;
            dispatch_queue.push_back(datagram);
        }
        
        for (channel_t c = 0; c < channel_count; c++) {
            std::unique_lock<std::shared_mutex> lock(*channel_mutex[c]);
            for (auto& [index, datagram] : channels_out[c].sent_datagrams) {
                if (GetTick() - datagram.tick_last_sent < 30) continue;
                datagram.tick_last_sent = GetTick();
                dispatch_queue.push_back(datagram);
            }
        }
        
        for (const auto& datagram : dispatch_queue) {
            SendImmediate(datagram);
        }
        
        return dispatch_queue.size();
    }
    
    Connection(uint32_t address, uint16_t port) : address(address), port(port) {
        for (channel_t c = 0; c < channel_count; c++) {
            channels.push_back(ReceivedChannel{});
            channels_out.push_back(SentChannel{});
            channel_mutex.push_back(new std::shared_mutex);
        }
    }
    
    virtual ~Connection() {
        for (auto mutex : channel_mutex) delete mutex;
    }
    
    std::vector<ReceivedChannel> channels;
    std::vector<SentChannel> channels_out;
    std::vector<std::shared_mutex*> channel_mutex;
    
    std::shared_mutex mutex;
    
    uint32_t last_tick_received = GetTick();
    
    uint32_t address;
    uint16_t port;
};

class UDPConnection : public Connection {
public:
    UDPConnection(uint32_t address, uint16_t port, SOCKET socket) : Connection(address, port), socket(socket) {}
    
    void Yeet() override {
        struct {
            datagram_header datagram;
            message_header message;
            message_disconnect content;
        } disconnect;
        
        strcpy(disconnect.datagram.marker, "TRAMSDK");
        disconnect.datagram.message_count = 1;
        disconnect.datagram.ack_count = 0;
        disconnect.datagram.ack_count = CHANNEL_CONTROL;
        disconnect.datagram.sender = outgoing_index;
        disconnect.datagram.index = 0;
        
        disconnect.message.type = MESSAGE_DISCONNECT;
        disconnect.message.length = sizeof(message_disconnect);
        
        disconnect.content.padding = 0;
        
        send(socket, (const char*)&disconnect, sizeof(disconnect), 0);
        
        closesocket(socket);
        
        delete this;
    }
    
    
    
    void SendMessage(channel_t channel, uint16_t type, void* data, uint32_t size) override {
        SentMessage message;
        message.length = size;
        message.type = type;
        memcpy(message.data, data, size);
        
        std::unique_lock<std::shared_mutex> lock(*channel_mutex[channel]);
        channels_out[channel].outgoing_queue.push_back(message);
    }
    
    bool ReceiveMessage(channel_t channel, uint16_t& type, void** data, uint32_t& size) override {
        std::unique_lock<std::shared_mutex> lock(*channel_mutex[channel]);
        
        // do nothing if no received datagrams
        if (!channels[channel].received_datagrams.size()) {
            return false;
        }
        
        const bool reliable = channel_infos[channel].reliable;
        
        // check if next received datagram is in sequence, otherwise we wait 
        if (reliable && channels[channel].received_datagrams.top().index != channels[channel].received_until + 1) {
            return false;
        }
        
        // check if current datagram is emptied
        if (!channels[channel].received_datagrams.top().messages.size()) {
            channels[channel].received_until = channels[channel].received_datagrams.top().index;
            channels[channel].received_datagrams.pop();
            ReceiveMessage(channel, type, data, size);
        }
        
        // clean up last message
        if (channels[channel].clean_up) {
            delete[] channels[channel].clean_up;
        }
        
        // return next message
        char* msg_data = channels[channel].received_datagrams.top().messages.back().data;
        size = channels[channel].received_datagrams.top().messages.back().length;
        type = channels[channel].received_datagrams.top().messages.back().type;
        channels[channel].clean_up = msg_data;
        *data = msg_data;
        
        return true;
    }
    
    virtual void RegisterReceived(const char* data, int32_t length) override {
        last_tick_received = GetTick();
        
        datagram_header* header = (datagram_header*)data;
        int32_t offset = sizeof(datagram_header);
        const bool reliable = channel_infos[header->channel].reliable;
        
        std::unique_lock<std::shared_mutex> lock(*channel_mutex[header->channel]);
        
        // check if datagram already received
        if (reliable && (channels[header->channel].received_until >= header->index
            || channels[header->channel].received_indices.contains(header->index))
        ) {
            Log(Severity::WARNING, System::PLATFORM, "duplicate datagram {} from {}", header->index, fmt_addr(address));
            return;
        }
        
        ReceivedDatagram datagram;
        datagram.index = header->index;
        
        // parse the acks
        for (int32_t i = 0; i < header->ack_count; i++) {
            if (offset + (int32_t)sizeof(ack_header) > length) {
                Log(Severity::WARNING, System::PLATFORM, "truncated ack from {}", fmt_addr(address));
                return;
            }
            
            ack_header* ack = (ack_header*)&data[offset];
            if (ack->channel >= channel_count) {
                Log(Severity::WARNING, System::PLATFORM, "invalid channel {} from {}", ack->channel, fmt_addr(address));
                return;
            }
            
            for (int32_t i = 0; i < 32; i++) {
                if (!(ack->mask & (1 << i))) continue;
                int32_t index = ack->offset + i;
                if (!channels_out[ack->channel].sent_datagrams.contains(index)) continue;
                channels_out[ack->channel].sent_datagrams.erase(index);
            }
            
            offset += sizeof(ack_header);
        }
        
        for (int32_t i = 0; i < header->message_count; i++) {
            if (offset + (int32_t)sizeof(message_header) > length) {
                Log(Severity::WARNING, System::PLATFORM, "truncated message from {}", fmt_addr(address));
                return;
            }
            
            message_header* msg = (message_header*)&data[offset];
            if (offset + (int32_t)sizeof(message_header) + msg->length > length) {
                Log(Severity::WARNING, System::PLATFORM, "truncated message from {}", fmt_addr(address));
                return;
            }
            
            ReceivedMessage message;
            message.length = msg->length;
            message.type = msg->type;
            message.data = new char[msg->length];
            memcpy(message.data, &data[offset + sizeof(message_header)], msg->length);
            
            datagram.messages.push_back(message);
            
            offset += sizeof(message_header) + msg->length;
        }
        
        if (reliable) {
            channels[header->channel].ackable.insert(header->index);
            channels[header->channel].received_indices.insert(header->index);
        }
    }
    
    void SendImmediate(const SentDatagram& datagram) override {
        send(socket, datagram.data, datagram.length, 0);
    }
    
    SOCKET socket;
};

static std::vector<Connection*> connections;
static std::set<connection_t> new_connections;
static std::shared_mutex connections_mutex;

static uint32_t application_id = 489;
static uint32_t application_version = 200;

static SOCKET host_socket = INVALID_SOCKET;

static std::thread host_thread;
static std::thread client_thread;
static std::thread dispatch_thread;

static ConnectionStatus connection_status = DISCONNECTED;
static char status_message[489] = "";

struct message_accepted {
    uint16_t user_index;
};

struct message_connect {
    uint16_t padding;
};

static void dispatch_messages() {
    while (host_socket != INVALID_SOCKET) {
        connection_t connects = connections.size();
        
        uint32_t dispatched = 0;
        
        for (connection_t connect = 0; connect < connects; connect++) {
            connections_mutex.lock_shared();
            
            if (connect >= connections.size()) {
                connections_mutex.unlock_shared();
                break;
            }
            
            Connection* cn = connections[connect];
            connections_mutex.unlock_shared();
            
            dispatched += cn->DispatchMessages();
        }
        
        if (!dispatched) std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

static bool winsock_inited = false;
static bool check_init() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        return false;
    }
    
    winsock_inited = true;
    return true;
}

void SetProtocolVersion(uint32_t app_id, uint32_t version) {
    application_id = app_id;
    application_version = version;
}

void Connect(const char* address, uint16_t port) {
    if (!check_init()) return;
    
    if (connection_status == CONNECTING) {
        Log(Severity::ERROR, System::PLATFORM, "Have to wait until connection succeeds or fails, sorry I don't make the rules.");
        return; 
    }
    
    if (connection_status != DISCONNECTED || connection_status != CONNECTED) {
        Log(Severity::ERROR, System::PLATFORM, "Cannot connect before stopping hosting!");
        return; 
    }
    
    // shut down existing connections
    host_socket = INVALID_SOCKET;
    client_thread.join();
    dispatch_thread.join();
    
    connections_mutex.lock();
    for (auto connection : connections) {
        connection->Yeet();
    }
    connections.clear();
    connections_mutex.unlock();
    
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.S_un.S_addr = inet_addr(address);
    
    Log(Severity::DEFAULT, System::PLATFORM, "Connecting to {} on port {}...", address, port);
    
    host_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    (void)connect(host_socket, (sockaddr*)&addr, sizeof(addr));
    
    // initialize the connection
    Connection* connec = new UDPConnection(addr.sin_addr.S_un.S_addr, addr.sin_port, host_socket);
    connections_mutex.lock();
    connections.push_back(connec);
    new_connections.insert(0);
    connections_mutex.unlock();
    
    // send out the join message
    struct {
        datagram_header datagram;
        message_header message;
        message_connect content;
    } connect;
    
    strcpy(connect.datagram.marker, "TRAMSDK");
    connect.datagram.message_count = 1;
    connect.datagram.ack_count = 0;
    connect.datagram.channel = CHANNEL_CONTROL;
    connect.datagram.sender = outgoing_index;
    connect.datagram.index = 0;
    
    connect.message.type = MESSAGE_ACCEPTED;
    connect.message.length = sizeof(message_accepted);
    
    connect.content.padding = 420;
    
    send(host_socket, (const char*)&connect, sizeof(connect), 0);
    
    connection_status = CONNECTING;
    
    dispatch_thread = std::thread(dispatch_messages);
    client_thread = std::thread([]() {
        while (host_socket != INVALID_SOCKET) {
            char buffer[1500];

            sockaddr_in sender;
            memset(&sender, 0, sizeof(sender));
            int sender_len = sizeof(sender);

            int32_t length = recvfrom(host_socket,
                buffer,
                sizeof(buffer),
                0,
                (sockaddr*)&sender,
                &sender_len);

            if (length == SOCKET_ERROR) {
                Log(Severity::WARNING, System::PLATFORM, "client socket error ");
                continue;
            }
            
            if (length < (int32_t)sizeof(datagram_header)) {
                char address[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &sender.sin_addr, address, INET_ADDRSTRLEN);
                Log(Severity::WARNING, System::PLATFORM, "received malformed datagram from {}", address);
                continue;
            }
            
            datagram_header* header = (datagram_header*)buffer;
            if (header->marker[0] != 'T'
                || header->marker[1] != 'R'
                || header->marker[2] != 'A'
                || header->marker[3] != 'M'
                || header->marker[4] != 'S'
                || header->marker[5] != 'D'
                || header->marker[6] != 'K'
                || header->marker[7] != '\0'
            ) {
                char address[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &sender.sin_addr, address, INET_ADDRSTRLEN);
                Log(Severity::WARNING, System::PLATFORM, "received malformed datagram from {}", address);
                continue;
            }
            
            if (header->channel >= channel_count) {
                Log(Severity::WARNING, System::PLATFORM, "invalid channel {} from {}", header->channel, fmt_addr(sender.sin_addr.S_un.S_addr));
                continue;
            }
            
            
            if (connection_status == CONNECTING) {
                if (header->ack_count != 0 || header->message_count != 1 || header->channel != CHANNEL_CONTROL) {
                    Log(Severity::WARNING, System::PLATFORM, "malformed datagram from {}", fmt_addr(sender.sin_addr.S_un.S_addr));
                    continue;
                }
                
                struct {
                    datagram_header datagram;
                    message_header message;
                    message_accepted content;
                }* accept;
                
                accept = (decltype(accept))buffer;
                
                // we're just assuming that we accepted right now ig
                // TODO: implement rejections
                
                outgoing_index = accept->content.user_index;
                connection_status = CONNECTED;
            }
            
        }
    });
}

void Disconnect() {
    if (!check_init()) return;
    
    if (connection_status != CONNECTED) {
        Log(Severity::ERROR, System::PLATFORM, "Cannot disconnect if not connected.");
        return; 
    }
    
    Log(Severity::DEFAULT, System::PLATFORM, "Disconnecting from the server.");
    
    host_socket = INVALID_SOCKET;
    client_thread.join();
    dispatch_thread.join();
    
    connections_mutex.lock();
    for (auto connection : connections) {
        connection->Yeet();
    }
    connections.clear();
    connections_mutex.unlock();
    
    closesocket(host_socket);
    WSACleanup();
    winsock_inited = false;
    
    winsock_inited = false;
    host_thread.join();
    dispatch_thread.join();
}

void Host(uint16_t port) {
    if (!check_init()) return;
    
    if (connection_status != DISCONNECTED && connection_status != HOSTING) {
        Log(Severity::ERROR, System::PLATFORM, "Cannot host before disconnecting!");
        return; 
    }
    
    if (host_socket != INVALID_SOCKET) {
        Log(Severity::DEFAULT, System::PLATFORM, "Stopping the server.");
        
        closesocket(host_socket);
        WSACleanup();
        winsock_inited = false;
        
        winsock_inited = false;
        host_thread.join();
        dispatch_thread.join();
    }
    
    if (!port) {
        return;
    }

    host_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (host_socket == INVALID_SOCKET) {
        WSACleanup();
        winsock_inited = false;
        return;
    }

    outgoing_index = 0;

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(host_socket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        closesocket(host_socket);
        WSACleanup();
        winsock_inited = false;
        return;
    }
    
    Log(Severity::DEFAULT, System::PLATFORM, "Listening for connections on UDP port {}...", port);
    
    dispatch_thread = std::thread(dispatch_messages);
    host_thread = std::thread([]() {
        while (host_socket != INVALID_SOCKET) {
            char buffer[1500];

            sockaddr_in sender;
            memset(&sender, 0, sizeof(sender));
            int sender_len = sizeof(sender);

            int32_t length = recvfrom(host_socket,
                buffer,
                sizeof(buffer),
                0,
                (sockaddr*)&sender,
                &sender_len);

            if (length == SOCKET_ERROR) {
                Log(Severity::WARNING, System::PLATFORM, "host socket error ");
                continue;
            }
            
            if (length < (int32_t)sizeof(datagram_header)) {
                char address[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &sender.sin_addr, address, INET_ADDRSTRLEN);
                Log(Severity::WARNING, System::PLATFORM, "received malformed datagram from {}", address);
                continue;
            }
            
            datagram_header* header = (datagram_header*)buffer;
            if (header->marker[0] != 'T'
                || header->marker[1] != 'R'
                || header->marker[2] != 'A'
                || header->marker[3] != 'M'
                || header->marker[4] != 'S'
                || header->marker[5] != 'D'
                || header->marker[6] != 'K'
                || header->marker[7] != '\0'
            ) {
                char address[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &sender.sin_addr, address, INET_ADDRSTRLEN);
                Log(Severity::WARNING, System::PLATFORM, "received malformed datagram from {}", address);
                continue;
            }
            
            if (header->channel >= channel_count) {
                Log(Severity::WARNING, System::PLATFORM, "invalid channel {} from {}", header->channel, fmt_addr(sender.sin_addr.S_un.S_addr));
                continue;
            }
            
            if (!header->sender) {
                if (header->message_count != 1 || header->ack_count != 1 || header->channel != CHANNEL_CONTROL) {
                    char address[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &sender.sin_addr, address, INET_ADDRSTRLEN);
                    Log(Severity::WARNING, System::PLATFORM, "received malformed datagram from {}", address);
                    continue;
                }
                
                // here we could check if some user with the same ip address and
                // port hasn't already connected
                
                // we could also check IP address blacklist/whitelist
                // if server is full, password, protocol version, etc.
                
                // create a new outgoing client socket
                SOCKET new_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                (void)connect(new_socket, (sockaddr*)&sender, sizeof(sender));
                
                // allocate new client index
                connections_mutex.lock();
                
                int32_t index = -1;
                for (int32_t i = 0; i < (int32_t)connections.size(); i++) {
                    if (connections[i]) continue;
                    index = i; break;
                }
                if (index == -1) {
                    index = connections.size();
                    connections.push_back(nullptr);
                }
                
                
                // initialize the connection
                Connection* connec = new UDPConnection(sender.sin_addr.S_un.S_addr, sender.sin_port, new_socket);
                connections[index] = connec;
                new_connections.insert(index);
                
                connections_mutex.unlock();
                
                Log(Severity::DEFAULT, System::PLATFORM, "New connection from {} on port {}", fmt_addr(sender.sin_addr.S_un.S_addr), sender.sin_port);
                
                // respond with accept message. we're not using `Connection`
                // class here, as when we'll add connection rejections, we don't
                // want to have to 
                struct {
                    datagram_header datagram;
                    message_header message;
                    message_accepted content;
                } accept;
                
                strcpy(accept.datagram.marker, "TRAMSDK");
                accept.datagram.message_count = 1;
                accept.datagram.ack_count = 0;
                accept.datagram.ack_count = CHANNEL_CONTROL;
                accept.datagram.sender = outgoing_index;
                accept.datagram.index = 0;
                
                accept.message.type = MESSAGE_ACCEPTED;
                accept.message.length = sizeof(message_accepted);
                
                accept.content.user_index = index;
                
                send(new_socket, (const char*)&accept, sizeof(accept), 0);
                
                continue;
            }
            
            std::shared_lock<std::shared_mutex> lock(connections_mutex);
            if (header->sender >= connections.size()
                || connections[header->sender]->address != sender.sin_addr.S_un.S_addr
                || connections[header->sender]->port != sender.sin_port
            ) {
                char address[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &sender.sin_addr, address, INET_ADDRSTRLEN);
                Log(Severity::WARNING, System::PLATFORM, "received unconnected datagram from {}", address);
                continue;
            }
            
            connections[header->sender]->RegisterReceived(buffer, length);
        }
    });
}

ConnectionStatus GetStatus() {
    return connection_status;
}

const char* GetStatusMessage() {
    return status_message;
}

channel_t AddChannel(bool reliable) {
    std::shared_lock<std::shared_mutex> lock(connections_mutex);
    if (connections.size()) {
        Log(Severity::ERROR, System::PLATFORM, "cannot add channel if connections exist, ignoring");
        return -1;
    }
    channel_infos[channel_count].reliable = reliable;
    return channel_count++;
}

void SendMessage(channel_t channel, connection_t connection, uint16_t type, void* data, uint32_t size) {
    if (channel >= channel_count) {
        Log(Severity::ERROR, System::PLATFORM, "cannot send message to invalid channel {}", channel);
        return;
    }
    
    std::shared_lock<std::shared_mutex> lock(connections_mutex);
    if (connection >= connections.size()) {
        Log(Severity::ERROR, System::PLATFORM, "cannot send message to invalid connection {}", connection);
        return;
    }
    
    connections[connection]->SendMessage(channel, type, data, size);
}

bool ReceiveMessage(channel_t channel, connection_t& connection, uint16_t& type, void** data, uint32_t& size) {
    if (channel >= channel_count) {
        Log(Severity::ERROR, System::PLATFORM, "cannot receive message from invalid channel {}", channel);
        return false;
    }
    
    std::shared_lock<std::shared_mutex> lock(connections_mutex);
    if (connection >= connections.size()) {
        Log(Severity::ERROR, System::PLATFORM, "cannot receive message from invalid connection {}", connection);
        return false;
    }
    
    return connections[connection]->ReceiveMessage(channel, type, data, size);
}

bool GetConnection(connection_t& id) {
    std::unique_lock<std::shared_mutex> lock(connections_mutex);
    if (!new_connections.size()) return false;
    id = *new_connections.begin();
    new_connections.erase(id);
    return true;
}

void Update() {
    // TODO: check timeouts
}

}