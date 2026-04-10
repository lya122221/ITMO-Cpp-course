#include "requests_handler.h"
#include "parser.h"
#include "api_client.h"
#include "cache.h"

int main() {
    Parser parser;
    Config config = parser.ParseConfig();

    APIClient api_client(config.api_key);
    Cache cache(config.max_cache_size, config.ttl_seconds);

    RequestsHandler handler(api_client, cache);
    handler.HandleRequests();

    return 0;
}