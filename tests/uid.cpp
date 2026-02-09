
// testable unit
#include <framework/uid.cpp>

// test framework
#include <templates/test.h>

using namespace tram;

START_TEST

TEST_CASE("Basic UID operations", "[uid1]") {
	ASSERT(UID("asdfghjkl"))
	
	ASSERT(UID() == UID())
	ASSERT(UID() == "none")
	ASSERT(UID() == UID("none"))
	
	ASSERT(!UID())
	
	ASSERT(UID("aaa") == UID("aaa"))
	ASSERT(UID("aaa") != UID("bbb"))
	
	ASSERT(strcmp(UID("aaa"), "aaa") == 0)
	ASSERT(strcmp(UID("aaa"), "bbb") != 0)
}

TEST_CASE("Default UID value", "[uid2]") {
    UID default_uid;
    UID none_uid = "none";
    
    ASSERT(default_uid.key == none_uid.key);
    ASSERT(default_uid == none_uid);
    ASSERT(default_uid == "none");
}

TEST_CASE("UID doesn't do weird pointer stuff", "[uid3]") {
    char buffer1[] = "egg_roll";
    char buffer2[] = "egg_roll";
    
    ASSERT(buffer1 != buffer2);
    
    UID uid1(buffer1);
    UID uid2(buffer2);
    
    ASSERT(uid1.key == uid2.key);
    ASSERT(uid1 == uid2);
}

TEST_CASE("UID const char* and std::string identity", "[uid4]") {
    const char* cstr = "egg_roll";
    std::string stdstr = "egg_roll";
    
    UID uid_cstr(cstr);
    UID uid_stdstr(stdstr);
    
    ASSERT(uid_cstr.key == uid_stdstr.key);
    ASSERT(uid_cstr == uid_stdstr);
}

TEST_CASE("UID stores a proper copy", "[uid5]") {
    char buffer[] = "roll";
    
    UID uid(buffer);
    
    const char* retrieved = uid;
    
    ASSERT(std::string(retrieved) == "roll");
    
    strcpy(buffer, "egg");
    
    const char* retrieved_after = uid;
    ASSERT(std::string(retrieved_after) == "roll");
    ASSERT(std::string(retrieved_after) != "egg");
}

TEST_CASE("UID conversion to std::string", "[uid6]") {
    UID uid("egg_roll");
    
    std::string str = uid;
    
    ASSERT(str == "egg_roll");
}

TEST_CASE("UID conversion to const char*", "[uid7]") {
    UID uid = "egg_roll";
    
    const char* cstr = uid;
    
    ASSERT(std::string(cstr) == "egg_roll");
}

TEST_CASE("UID copy constructor", "[uid8]") {
    UID uid1 = "egg_roll";
    UID uid2 = uid1;
    
    ASSERT(uid1.key == uid2.key);
    ASSERT(uid1 == uid2);
    
    std::string str1 = uid1;
    std::string str2 = uid2;
    ASSERT(str1 == str2);
}

TEST_CASE("UID assignment operator", "[uid9]") {
    UID uid1 = "egg";
    UID uid2 = "roll";
    
    ASSERT(uid1.key != uid2.key);
    
    uid2 = uid1;
    
    ASSERT(uid1.key == uid2.key);
    ASSERT(uid1 == uid2);
}

TEST_CASE("UID bool comparison", "[uid10]") {
    UID valid_uid("eggroll");
    UID default_uid;
    
    ASSERT((bool)valid_uid == true);
    ASSERT((bool)default_uid == false);
}

TEST_CASE("UID no_quote() check", "[uid11]") {
    ASSERT(UID::no_quote("egg") == true);
    ASSERT(UID::no_quote("egg123") == true);
    ASSERT(UID::no_quote("under_score") == true);
    
    ASSERT(UID::no_quote("egg roll") == false);
    ASSERT(UID::no_quote("has space") == false);
    ASSERT(UID::no_quote("has\ttab") == false);
    ASSERT(UID::no_quote("has\nnewline") == false);
    
    ASSERT(UID::no_quote("has\"quote") == false);
    ASSERT(UID::no_quote("\"quoted\"") == false);
}

TEST_CASE("UID is_empty() check", "[uid12]") {
    ASSERT(UID::is_empty("") == true);
    ASSERT(UID::is_empty(" ") == true);
    ASSERT(UID::is_empty("   ") == true);
    ASSERT(UID::is_empty("\t") == true);
    ASSERT(UID::is_empty("\n") == true);
    ASSERT(UID::is_empty(" \t\n ") == true);
    
    ASSERT(UID::is_empty("hello") == false);
    ASSERT(UID::is_empty(" text ") == false);
    ASSERT(UID::is_empty("a") == false);
}

TEST_CASE("UID is_valid() check", "[uid13]") {
    UID valid1 = "egg";
    UID valid2 = "egg_roll";
    ASSERT(UID::is_valid(valid1) == true);
    ASSERT(UID::is_valid(valid2) == true);
    
    UID default_uid;
    ASSERT(UID::is_valid(default_uid) == true);

    UID invalid_uid;
    invalid_uid.key = -1;
	ASSERT(UID::is_valid(invalid_uid) == false);
	
    invalid_uid.key = 100000000;
    ASSERT(UID::is_valid(invalid_uid) == false);
}

TEST_CASE("Multiple UID with same key", "[uid14]") {
    UID uid1 = "egg";
    UID uid2 = "egg";
    UID uid3 = "egg";
    UID uid4 = "egg";
    
    ASSERT(uid1.key == uid2.key);
    ASSERT(uid2.key == uid3.key);
    ASSERT(uid3.key == uid4.key);
    
    ASSERT(uid1 == uid2);
    ASSERT(uid2 == uid3);
    ASSERT(uid3 == uid4);
}

TEST_CASE("UID different string keys", "[uid15]") {
    const int count = 100;
    UID uids[count];

    for (int i = 0; i < count; i++) {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "string_%d", i);
        uids[i] = UID(buffer);
    }

    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            ASSERT(uids[i].key != uids[j].key);
            ASSERT(!(uids[i] == uids[j]));
        }
    }
}

TEST_CASE("UID empty string", "[uid16]") {
    UID empty1 = "";
    UID empty2 = "";
    
    ASSERT(empty1.key == empty2.key);
    ASSERT(empty1 == empty2);
    
    std::string str = empty1;
    ASSERT(str == "");
}

TEST_CASE("UID special characters", "[uid17]") {
    UID uid1 = "hello_world";
    UID uid2 = "hello-world";
    UID uid3 = "hello.world";
    UID uid4 = "hello/world";
    UID uid5 = "hello@world";
    
    ASSERT(uid1.key != uid2.key);
    ASSERT(uid2.key != uid3.key);
    ASSERT(uid3.key != uid4.key);
    ASSERT(uid4.key != uid5.key);
    
    ASSERT(std::string(uid1) == "hello_world");
    ASSERT(std::string(uid2) == "hello-world");
    ASSERT(std::string(uid3) == "hello.world");
}

TEST_CASE("UID numbers in strings", "[uid18]") {
    UID uid1 = "egg420";
    UID uid2 = "420egg";
    UID uid3 = "42069";
    
    ASSERT(uid1.key != uid2.key);
    ASSERT(uid2.key != uid3.key);
    
    ASSERT(std::string(uid1) == "egg420");
    ASSERT(std::string(uid2) == "420egg");
    ASSERT(std::string(uid3) == "42069");
}

TEST_CASE("UID long string", "[uid19]") {
    std::string long_str(500, 'x');
    long_str += "_end";
    
    UID uid(long_str.c_str());
    
    std::string retrieved = uid;
    ASSERT(retrieved == long_str);
}

TEST_CASE("UID case sensitivity", "[uid20]") {
    UID uid_lower = "egg";
    UID uid_upper = "EGG";
    UID uid_mixed = "EgG";
    
    ASSERT(uid_lower.key != uid_upper.key);
    ASSERT(uid_upper.key != uid_mixed.key);
    ASSERT(uid_lower.key != uid_mixed.key);
}

TEST_CASE("UID const char* comparison", "[uid21]") {
    UID uid = "egg_roll";
    
    ASSERT(uid == "egg_roll");
    ASSERT(!(uid == "dumpling"));
    ASSERT(!(uid == "EGG_roll"));
}

END_TEST
