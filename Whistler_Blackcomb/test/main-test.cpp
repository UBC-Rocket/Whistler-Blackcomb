 //main.cc
#define CATCH_CONFIG_MAIN
#include "../lib/catch.hpp"
#include "../includes/statemachine.h"
#include "../includes/teensy_com.h"



TEST_CASE("Check Test works") {
    REQUIRE(1 == 1);
}
TEST_CASE("Check Communication"){
    Voter v1 = Voter(1, true);
    Voter v2 = Voter(2, true);
    Voter v3 = Voter(3, true);

    SECTION("Check correctness of parsemessage"){
        v1.parseMessage(1, 0xD9);
        REQUIRE(v1.com1Status);
        REQUIRE(v1.com2StatusAux);
        REQUIRE(v1.kingID1 == 1);
        REQUIRE(v1.unitState1 == (FlightStates)9);
        v1.parseMessage(2, 0xE9);
        REQUIRE(v1.com2Status);
        REQUIRE(v1.com1StatusAux);
        REQUIRE(v1.kingID2 == 2);
        REQUIRE(v1.unitState2 == (FlightStates)9);

        v1.parseMessage(1, 0x27);
        REQUIRE(!v1.com1Status);
        REQUIRE(!v1.com2StatusAux);
        REQUIRE(v1.kingID1 == 2);
        REQUIRE(v1.unitState1 == (FlightStates)7);
        v1.parseMessage(2, 0x17);
        REQUIRE(!v1.com2Status);
        REQUIRE(!v1.com1StatusAux);
        REQUIRE(v1.kingID2 == 1);
        REQUIRE(v1.unitState2 == (FlightStates)7);
    };

    SECTION("Check correctness of assess kings"){
        v1.parseMessage(1, 0xE0);
        v1.parseMessage(2, 0xD0);
        v2.com1Status = false;
        v2.parseMessage(2, 0xD0);
        v3.parseMessage(1, 0xD0);
        v3.parseMessage(2, 0xA0);

        v1.assessKings();
        v2.assessKings();
        v3.assessKings();

        REQUIRE(v1.king);
        REQUIRE(!v2.king);
        REQUIRE(!v3.king);

        v1.parseMessage(1, 0xE0);
        v1.parseMessage(2, 0xE0);
        v2.com1Status = false;
        v2.parseMessage(2, 0xA0);
        v3.com1Status = false;
        v3.parseMessage(2, 0xA0);

        v1.assessKings();
        v2.assessKings();
        v3.assessKings();

        REQUIRE(!v1.king);
        REQUIRE(v2.king);
        REQUIRE(!v3.king);


        v1.parseMessage(1, 0xD0);
        v1.parseMessage(2, 0xE0);
        v2.parseMessage(1, 0xD0);
        v2.parseMessage(2, 0xA0);
        v3.com1Status = false;
        v3.parseMessage(2, 0xA0);

        v1.assessKings();
        v2.assessKings();
        v3.assessKings();

        REQUIRE(v1.king);
        REQUIRE(!v2.king);
        REQUIRE(!v3.king);

        v1.parseMessage(1, 0xE0);
        v1.parseMessage(2, 0xE0);
        v2.com1Status = false;
        v2.parseMessage(2, 0xA0);
        v3.com1Status = false;
        v3.parseMessage(2, 0xA0);

        v1.assessKings();
        v2.assessKings();
        v3.assessKings();

        v1.parseMessage(2, 0xD0);
        v1.parseMessage(1, 0xE0);
        v2.com1Status = false;
        v2.parseMessage(2, 0xD0);
        REQUIRE(v2.com2Status);
        REQUIRE(v2.kingID2 == 1);
        v3.parseMessage(1, 0xD0);
        v3.parseMessage(2, 0xA0);

        v1.assessKings();
        v2.assessKings();
        v3.assessKings();

        REQUIRE(v1.king);
        REQUIRE(!v2.king);
        REQUIRE(!v3.king);


    };
}
