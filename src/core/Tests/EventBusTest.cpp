#include "Core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace App;
using namespace std;

TEST_CASE ("Core::EventBus")
{
    using MyEventBus = EventBus<variant<int, string> >;

    MyEventBus bus;

    int intCount = 0;
    int stringCount = 0;

    {
        auto subInt = bus.subscribe<int> ( [&] (int) {
            ++intCount;
        });

        auto subString = bus.subscribe<string> ( [&] (const string&) {
            ++stringCount;
        });

        bus.publish (42);
        bus.publish (string ("Hello"));

        REQUIRE (intCount == 1);
        REQUIRE (stringCount == 1);
    }

    // After the subscriptions go out of scope, they should be automatically unsubscribed.
    bus.publish (42);
    bus.publish (string ("Hello"));

    REQUIRE (intCount == 1);
    REQUIRE (stringCount == 1);
}
