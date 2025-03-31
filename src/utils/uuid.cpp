#include "utils/uuid.h"
#include <uuid/uuid.h>

std::string generateUUID()
{
    uuid_t uuid;
    uuid_generate_random(uuid);

    char uuidStr[37] = {0};
    uuid_unparse(uuid, uuidStr);
    return uuidStr;
}