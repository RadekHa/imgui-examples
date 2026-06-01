#include "Core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace App;
using namespace std;


TEST_CASE ("Core::EventBus")
{
    using Events = variant<int, string>;
    using EventBus = EventBusImpl<Events>;

    EventBus bus;

    EventBus::Subscription subInt;

    int intCount = 0;
    int stringCount = 0;

    subInt = bus.subscribe<int> ( [&] (int) {
        ++intCount;
    });


    {
        auto subString = bus.subscribe<string> ( [&] (const string&) {
            ++stringCount;
        });


        bus.publish (42);
        bus.publish (string ("Hello"));

        bus.dispatch ();

        REQUIRE (intCount == 1);
        REQUIRE (stringCount == 1);

        bus.publish (42);
        bus.dispatch ();
        REQUIRE (intCount == 2);
    }

    subInt.unsubscribe ();
    // After the subscriptions go out of scope, they should be automatically unsubscribed.
    bus.publish (42);
    bus.publish (string ("Hello"));

    bus.dispatch ();

    REQUIRE (intCount == 2);
    REQUIRE (stringCount == 1);
}
