// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <extensions/file/http.h>

#include <platform/file.h>
#include <framework/logging.h>
#include <config.h>

#include <cstring>

#define CURL_STATICLIB
#include <curl/curl.h>


#undef ERROR

namespace tram::Ext::File  {

static CURL* curl;

static const int URL_LIMIT = PATH_LIMIT + 50;

static size_t ContentCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    struct std::pair<char*, size_t>* mem = (std::pair<char*, size_t>*) userp;
    
    size_t total_size = size * nmemb;

    char* ptr = (char*)realloc(mem->first, mem->second + total_size + 1);
    if (!ptr) {
        Log(Severity::WARNING, System::PLATFORM, "Not enough memory for a HTTP response!");
        return 0;
    }

    mem->first = ptr;
    memcpy(&(mem->first[mem->second]), contents, total_size);
    mem->second += total_size;
    mem->first[mem->second] = '\0';

    return total_size;
}

static size_t DiscardCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    return size * nmemb;
}

static void LogError(const char* url, CURLcode res) {
    switch (res) {
        case CURLE_COULDNT_RESOLVE_HOST:
            Log(Severity::WARNING, System::PLATFORM, "DNS resolution failed when accessing {}", url);
            break;
        case CURLE_COULDNT_CONNECT:
            Log(Severity::WARNING, System::PLATFORM, "Connection failed when accessing {}", url);
            break;
        case CURLE_OPERATION_TIMEDOUT:
            Log(Severity::WARNING, System::PLATFORM, "Request timed out when accessing {}", url);
            break;
        case CURLE_SSL_CONNECT_ERROR:
            Log(Severity::WARNING, System::PLATFORM, "SSL connection error when accessing {}", url);
            break;
        default:
            Log(Severity::WARNING, System::PLATFORM, "Network error when accessing {}\n{}", url, curl_easy_strerror(res));
            break;
    }
}

class HTTPReader : public FileReader {
public:
    HTTPReader(const char* protocol, const char* location, const char* path) {
        std::pair<char*, size_t> chunk;
        char url[URL_LIMIT];
        
        if (location) {
            snprintf(url, URL_LIMIT, "%s://%s/%s", protocol, location, path);
        } else {
            snprintf(url, URL_LIMIT, "%s://%s", protocol, path);
        }
        

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ContentCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "User-Agent: Tramway SDK/0.1.1");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        auto res = curl_easy_perform(curl);
        
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            LogError(url, res);
            return;
        }
        
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        
        if (response_code >= 400) {
            Log(Severity::INFO, System::PLATFORM, "HTTP request to {} failed: {}", url, response_code);
            return;
        }
        
        contents = chunk.first;
        length = chunk.second;
    }
    
    virtual ~HTTPReader(){
        if (contents) free(contents);
    }
    
    virtual const char* GetContents() override  {
        return contents;
    }
    
    virtual size_t GetSize() override {
        return length;
    }
    
    virtual FileStatus GetStatus() override  {
        if (contents) return FileStatus::READY;
        return FileStatus::IO_ERROR;
    }
    
    virtual void Yeet() override {
        delete this;
    }
private:
    char* contents = nullptr;
    int length = 0;
};

class HTTPWriter : public FileWriter {
public:
    HTTPWriter(const char* protocol, const char* path) {
        url = (char*)malloc(URL_LIMIT);
        snprintf(url, URL_LIMIT, "%s://%s", protocol, path);
        
        buffer = (char*)malloc(1024);
        
        allocated_size = 1024;
        buffer_size = 0;
    }
    
    ~HTTPWriter() {
        if (buffer_size) Flush();
        free(url);
        if (buffer) free(buffer);
    }
    
    void SetContents(const char* contents, size_t size) {
        if (!buffer) return;
        
        if (allocated_size < buffer_size + (int)size) {
            int new_size = allocated_size + (allocated_size >> 2);
            if (new_size < buffer_size + (int)size) {
                new_size += size;
            }
            char* new_buffer = (char*)realloc(buffer, new_size);
            if (!new_buffer) {
                flush_failed = true;
                free(buffer);
                buffer = nullptr;
                Log(Severity::ERROR, System::PLATFORM, "Error allocating {} bytes for POST request to: {}", new_size, url);
                return;
            }
            buffer = new_buffer;
            allocated_size = new_size;
        }
        
        memcpy(buffer + buffer_size, contents, size);
        
        buffer_size += size;
    }
    
    bool Flush() {
        if (!buffer) return false;
        
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)buffer_size);
        
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DiscardCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
        
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: text/plain");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        auto res = curl_easy_perform(curl);
        
        buffer_size = 0;
        curl_slist_free_all(headers);
        
        if (res != CURLE_OK) {
            LogError(url, res);
            flush_failed = true;
            return false;
        }
        
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        
        if (response_code >= 200 && response_code < 300) {
            flush_failed = false;
            return true;
        }
        
        flush_failed = true;
        Log(Severity::INFO, System::PLATFORM, "HTTP request to {} failed: {}", url, response_code);
        
        return false;
    }
   
    void Yeet() {
        delete this;
    }
    
    FileStatus GetStatus() {
        if (!buffer || flush_failed) {
            return FileStatus::IO_ERROR;
        }
        
        return FileStatus::READY;
    }
    
private:
    char* buffer = nullptr;
    
    int allocated_size = 0;
    int buffer_size = 0;
    
    char* url = nullptr;
    
    bool flush_failed = false;
};

void RegisterHTTP() {
    curl = curl_easy_init();
    
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    
    FileReader::Register("http", [](const char* location, const char* path) -> FileReader* { return new HTTPReader("http", location, path); });
    FileReader::Register("https", [](const char* location, const char* path) -> FileReader* { return new HTTPReader("https", location, path); });
    
    FileWriter::Register("http", [](const char* path) -> FileWriter* { return new HTTPWriter("http", path); });
    FileWriter::Register("https", [](const char* path) -> FileWriter* { return new HTTPWriter("https", path); });
}

}
