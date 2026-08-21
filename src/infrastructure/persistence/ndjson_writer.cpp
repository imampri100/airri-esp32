#include "infrastructure/persistence/ndjson_writer.h"

namespace irrigation
{

NdjsonWriter::NdjsonWriter(
    PersistenceStorageManager& storage,
    JsonSerializer& serializer
)
    :
    storage(storage),
    serializer(serializer)
{
}

bool NdjsonWriter::append(
    const String& path,
    const JsonDocument& document
)
{
    File file =
        storage.openAppend(path);

    if (!file)
    {
        return false;
    }

    bool result =
        serializer.serialize(
            document,
            file
        );

    file.println();

    file.close();

    return result;
}

}