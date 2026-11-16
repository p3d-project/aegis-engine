#include "monster_generated.h"
#include <flatbuffers/flatbuffers.h>

/**
 * TODO:
 * - Modify monster example for component example
 * - Use flatbuffers code in main.cpp file
 */

int main()
{
    // Used to build the flatbuffer
    FlatBufferBuilder builder;

    // Auto-generated function emitted from `flatc` and the input
    // `monster.fbs` schema.
    auto monster = CreateMonsterDirect(builder, "Abominable Snowman", 100);

    // Finalize the buffer.
    builder.Finish(monster);

    // Get a pointer to the flatbuffer.
    const uint8_t* flatbuffer = builder.GetBufferPointer();

    // Get a view of the root monster from the flatbuffer.
    const Monster* snowman = GetMonster(flatbuffer);

    // Access the monster's fields directly.
    // ASSERT_EQ(snowman->name(), "Abominable Snowman");
    // ASSERT_EQ(snowman->health(), 100);

    delete snowman;
    snowman = nullptr;
}
