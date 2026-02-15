
// dependencies
#include <framework/system.cpp>
#include <framework/logging.cpp>
#include <framework/uid.cpp>
#include <framework/type.cpp>

// testable unit
#include <framework/settings.cpp>

// test framework
#include <templates/test.h>

namespace tram::Platform {
	void TryDebugging() {
		
	}
	
	void ShowErrorDialog(const char* message, const char* title) {
		
	}
}

using namespace tram;
using namespace tram::Settings;

static Property<int32_t> static_int = {420, "static-int", USER | MENU};
static Property<float> static_float = {-69.0f, "static-float", USER | MENU};

START_TEST

TEST_CASE("Setting bool basic", "[settings1]") {
	bool test_bool = false;
	Register(test_bool, "test_bool", USER);
	
	value_t retrieved = Get("test_bool");
	ASSERT(retrieved.IsBool())
	ASSERT((bool)retrieved == false)
	
	test_bool = true;
	value_t retrieved2 = Get("test_bool");
	ASSERT((bool)retrieved2 == true)
}

TEST_CASE("Setting float basic", "[settings2]") {
	float test_float = 420.69f;
	Register(test_float, "test_float", USER | MENU);
	
	value_t retrieved = Get("test_float");
	ASSERT(retrieved.IsFloat())
	float f = (float)retrieved;
	ASSERT(f == 420.69f)
}

TEST_CASE("Setting integer basic", "[settings3]") {
	int32_t test_int = -420;
	Register(test_int, "test_int32", APPLICATION);
	
	value_t retrieved = Get("test_int32");
	ASSERT(retrieved.IsInt())
	ASSERT((int32_t)retrieved == -420)
}

TEST_CASE("Setting unsigned integer basic", "[settings4]") {
	uint32_t test_uint = 420;
	Register(test_uint, "test_uint32", USER | APPLICATION);
	
	value_t retrieved = Get("test_uint32");
	ASSERT(retrieved.IsInt())
	ASSERT((uint32_t)retrieved == 420)
}

TEST_CASE("Setting integer value change", "[settings5]") {
	int32_t test_int = 420;
	Register(test_int, "settable_int", USER);
	
	value_t value1 = Get("settable_int");
	ASSERT((int32_t)value1 == 420)

	Set("settable_int", value_t(69));
	
	ASSERT(test_int == 69)
	
	value_t value2 = Get("settable_int");
	ASSERT((int32_t)value2 == 69)
}

TEST_CASE("Setting with different flags", "[settings6]") {
	bool setting1 = false;
	float setting2 = 420.0f;
	int32_t setting3 = -420;
	uint32_t setting4 = 69;
	
	Register(setting1, "flag_user", USER);
	Register(setting2, "flag_app", APPLICATION);
	Register(setting3, "flag_menu", MENU);
	Register(setting4, "flag_all", USER | APPLICATION | MENU);
	
	ASSERT(Flags("flag_user") == USER)
	ASSERT(Flags("flag_app") == APPLICATION)
	ASSERT(Flags("flag_menu") == MENU)
	ASSERT(Flags("flag_all") == (USER | APPLICATION | MENU))
}

TEST_CASE("Setting with no flag", "[settings7]") {
	int32_t setting = 420;
	Register(setting, "none_flag_setting", NONE);
	
	ASSERT(Flags("none_flag_setting") == NONE)
	
	value_t value = Get("none_flag_setting");
	ASSERT((int32_t)value == 420)
}

TEST_CASE("Setting lookup by user flag", "[settings8]") {
	bool setting1 = true;
	bool setting2 = false;
	bool setting3 = true;
	
	Register(setting1, "user_setting_1", USER);
	Register(setting2, "user_setting_2", USER | MENU);
	Register(setting3, "app_setting", APPLICATION);
	
	std::vector<name_t> user_settings = GetSettings(USER);
	
	bool found_setting1 = false;
	bool found_setting2 = false;
	bool found_setting3 = false;
	
	for (const auto& name : user_settings) {
		if (name == "user_setting_1") found_setting1 = true;
		if (name == "user_setting_2") found_setting2 = true;
		if (name == "app_setting") found_setting3 = true;
	}
	
	ASSERT(found_setting1)
	ASSERT(found_setting2)
	ASSERT(!found_setting3)
}

TEST_CASE("Setting lookup by application flag", "[settings9]") {
	int32_t setting1 = 420;
	int32_t setting2 = 69;
	int32_t setting3 = 489;
	
	Register(setting1, "app_only", APPLICATION);
	Register(setting2, "app_and_menu", APPLICATION | MENU);
	Register(setting3, "user_only", USER);
	
	std::vector<name_t> app_settings = GetSettings(APPLICATION);
	
	bool found_setting1 = false;
	bool found_setting2 = false;
	bool found_setting3 = false;
	
	for (const auto& name : app_settings) {
		if (name == "app_only") found_setting1 = true;
		if (name == "app_and_menu") found_setting2 = true;
		if (name == "user_only") found_setting3 = true;
	}
	
	ASSERT(found_setting1)
	ASSERT(found_setting2)
	ASSERT(!found_setting3)
}

TEST_CASE("Setting lookup by menu flag", "[settings10]") {
	float setting1 = 420.0f;
	float setting2 = 69.0f;
	float setting3 = 489.0f;
	
	Register(setting1, "menu_setting", MENU);
	Register(setting2, "menu_user", MENU | USER);
	Register(setting3, "no_menu", USER | APPLICATION);
	
	std::vector<name_t> menu_settings = GetSettings(MENU);
	
	bool found_setting1 = false;
	bool found_setting2 = false;
	bool found_setting3 = false;
	
	for (const auto& name : menu_settings) {
		if (name == "menu_setting") found_setting1 = true;
		if (name == "menu_user") found_setting2 = true;
		if (name == "no_menu") found_setting3 = true;
	}
	
	ASSERT(found_setting1)
	ASSERT(found_setting2)
	ASSERT(!found_setting3)
}

TEST_CASE("Setting lookup by multiple flags", "[settings11]") {
	bool setting1 = true;
	bool setting2 = true;
	bool setting3 = true;
	bool setting4 = true;
	
	Register(setting1, "user_menu", USER | MENU);
	Register(setting2, "app_menu", APPLICATION | MENU);
	Register(setting3, "all_three", USER | APPLICATION | MENU);
	Register(setting4, "just_user", USER);
	
	std::vector<name_t> filtered = GetSettings(USER | MENU);
	
	bool found_setting1 = false;
	bool found_setting3 = false;
	
	for (const auto& name : filtered) {
		if (name == "user_menu") found_setting1 = true;
		if (name == "all_three") found_setting3 = true;
	}
	
	ASSERT(found_setting1)
	ASSERT(found_setting3)
}

TEST_CASE("Setting bool flag commandline parse", "[settings12]") {
	bool test_flag = false;
	Register(test_flag, "enable_flag", USER);
	
	const char* argv[] = {"program", "-enable_flag"};
	Parse(argv, 2);
	
	ASSERT(test_flag == true)
}

TEST_CASE("Setting bool commandline as value parse", "[settings13]") {
	bool flag1 = false;
	bool flag2 = true;
	
	Register(flag1, "flag_true", USER);
	Register(flag2, "flag_false", USER);
	
	const char* argv[] = {"program", "-flag_true", "true", "-flag_false", "false"};
	Parse(argv, 5);
	
	ASSERT(flag1 == true)
	ASSERT(flag2 == false)
}

TEST_CASE("Setting int commandline parse", "[settings14]") {
	int32_t test_int = 0;
	Register(test_int, "intparse", USER);
	
	const char* argv[] = {"program", "-intparse", "420"};
	Parse(argv, 3);
	
	ASSERT(test_int == 420)
}

TEST_CASE("Setting uint commandline parse", "[settings15]") {
	uint32_t test_uint = 0;
	Register(test_uint, "uintint", APPLICATION);
	
	const char* argv[] = {"program", "-uintint", "420"};
	Parse(argv, 3);
	
	ASSERT(test_uint == 420)
}

TEST_CASE("Setting float commandline parse", "[settings16]") {
	float test_float = 0.0f;
	Register(test_float, "floatfloat", USER | MENU);
	
	const char* argv[] = {"program", "-floatfloat", "420.69"};
	Parse(argv, 3);
	
	ASSERT(test_float == 420.69f)
}

TEST_CASE("Setting parse commandline negative numbers", "[settings17]") {
	int32_t negative_int = 0;
	float negative_float = 0.0f;
	
	Register(negative_int, "intneg", USER);
	Register(negative_float, "floatneg", USER);
	
	const char* argv[] = {"program", "-intneg", "-69", "-floatneg", "-420.69"};
	Parse(argv, 5);
	
	ASSERT(negative_int == -69)
	ASSERT(negative_float == -420.69f)
}

TEST_CASE("Setting multiple commandline parse", "[settings18]") {
	bool bool_val = false;
	int32_t int_val = 0;
	float float_val = 0.0f;
	uint32_t uint_val = 0;
	
	Register(bool_val, "bool_val", USER);
	Register(int_val, "int_val", USER);
	Register(float_val, "float_val", USER);
	Register(uint_val, "uint_val", USER);
	
	const char* argv[] = {
		"program",
		"-bool_val",
		"-int_val", "489",
		"-float_val", "420.69",
		"-uint_val", "69"
	};
	Parse(argv, 8);
	
	ASSERT(bool_val == true)
	ASSERT(int_val == 489)
	ASSERT(float_val == 420.69f)
	ASSERT(uint_val == 69)
}

TEST_CASE("Setting commandline parse multiple order", "[settings19]") {
	bool flag1 = false;
	int32_t num1 = 0;
	bool flag2 = false;
	int32_t num2 = 0;
	
	Register(flag1, "first_flag", USER);
	Register(num1, "first_num", USER);
	Register(flag2, "second_flag", USER);
	Register(num2, "second_num", USER);
	
	const char* argv[] = {
		"program",
		"-first_num", "420",
		"-second_flag",
		"-first_flag",
		"-second_num", "69"
	};
	Parse(argv, 7);
	
	ASSERT(flag1 == true)
	ASSERT(num1 == 420)
	ASSERT(flag2 == true)
	ASSERT(num2 == 69)
}

TEST_CASE("Setting bool property wrapper", "[settings20]") {
	static Property<bool> property_flag = {false, "property_flag", USER};
	
	ASSERT((bool)property_flag == false)
	
	value_t value = Get("property_flag");
	ASSERT(value.IsBool())
	ASSERT((bool)value == false)
}

TEST_CASE("Setting int property wrapper", "[settings21]") {
	static Property<int32_t> int_prop_val = {420, "int_prop_val", USER | MENU};
	
	ASSERT((int32_t)int_prop_val == 420)
	
	value_t value = Get("int_prop_val");
	ASSERT(value.IsInt())
	ASSERT((int32_t)value == 420)
	
	ASSERT(Flags("int_prop_val") == (USER | MENU))
}

TEST_CASE("Setting float property wrapper", "[settings22]") {
	static Property<float> float_prop_val = {420.69f, "float_prop_val", APPLICATION};
	
	float f = (float)float_prop_val;
	ASSERT(f == 420.69f)
	
	value_t value = Get("float_prop_val");
	ASSERT(value.IsFloat())
}

TEST_CASE("Setting uint property wrapper", "[settings23]") {
	static Property<uint32_t> uint_prop_val = {420, "uint_prop_val", MENU};
	
	ASSERT((uint32_t)uint_prop_val == 420)
	
	value_t value = Get("uint_prop_val");
	ASSERT(value.IsInt())
	ASSERT((uint32_t)value == 420)
}

TEST_CASE("Setting property wrapper non-initializer", "[settings24]") {
	static Property<int32_t> simple_property = 69;
	
	ASSERT((int32_t)simple_property == 69)
}

TEST_CASE("Setting empty commandline parse", "[settings25]") {
	const char* argv[] = {"program"};
	Parse(argv, 1);
	ASSERT(true)
}

TEST_CASE("Setting get non-defined", "[settings26]") {
	value_t value = Get("does_not_exist");

	ASSERT(value.GetType() == TYPE_UNDEFINED)
}

TEST_CASE("Setting set non-defined", "[settings27]") {
	Set("does_not_exist", value_t(420));
	
	ASSERT(true)
}

TEST_CASE("Setting flag lookup for non-defined", "[settings28]") {
	uint32_t flags = Flags("does_not_exist");
	
	ASSERT(flags == NONE)
}

TEST_CASE("Setting commandline parse non-defined", "[settings29]") {
	bool known = false;
	Register(known, "known_setting", USER);
	
	const char* argv[] = {"program", "-unknown_setting", "value", "-known_setting"};
	Parse(argv, 4);
	
	ASSERT(known == true)
}

TEST_CASE("Setting registering multiple same name", "[settings30]") {
	int32_t first = 420;
	int32_t second = 69;
	
	Register(first, "duplicate_name", USER);
	Register(second, "duplicate_name", APPLICATION);
	
	value_t value = Get("duplicate_name");
	ASSERT(value.IsInt())
	ASSERT((int32_t)Get("duplicate_name") == first)
}

TEST_CASE("Setting commandline float parse formats", "[settings31]") {
	float float1 = 0.0f;
	float float2 = 0.0f;
	float float3 = 0.0f;
	
	Register(float1, "float1", USER);
	Register(float2, "float2", USER);
	Register(float3, "float3", USER);
	
	const char* argv[] = {
		"program",
		"-float1", "420.69",
		"-float2", ".69",
		"-float3", "420."
	};
	Parse(argv, 7);
	
	ASSERT(float1 == 420.69f)
	ASSERT(float2 == 0.69f)
	ASSERT(float3 == 420.0f)
}

TEST_CASE("Setting commandline parse zeroes", "[settings32]") {
	int32_t int_zero = 420;
	float float_zero = 69.0f;
	
	Register(int_zero, "int_zero", USER);
	Register(float_zero, "float_zero", USER);
	
	const char* argv[] = {
		"program",
		"-int_zero", "0",
		"-float_zero", "0.0"
	};
	Parse(argv, 5);
	
	ASSERT(int_zero == 0)
	ASSERT(float_zero == 0.0f)
}

TEST_CASE("Setting setting lookup with no filter", "[settings33]") {
	bool setting1 = true;
	bool setting2 = true;
	
	Register(setting1, "setting_a", USER);
	Register(setting2, "setting_b", APPLICATION);
	
	std::vector<name_t> none_settings = GetSettings(NONE);
	
	ASSERT(!none_settings.size())
}

TEST_CASE("Setting multiple gets", "[settings34]") {
	int32_t persistent = 420;
	Register(persistent, "persistent_value", USER);
	
	value_t v1 = Get("persistent_value");
	ASSERT((int32_t)v1 == 420)
	
	value_t v2 = Get("persistent_value");
	ASSERT((int32_t)v2 == 420)
	
	value_t v3 = Get("persistent_value");
	ASSERT((int32_t)v3 == 420)
}

TEST_CASE("Setting large commandline parse", "[settings35]") {
	bool flags[10];
	int32_t values[10];
	
	for (int i = 0; i < 10; i++) {
		flags[i] = false;
		values[i] = 0;
		
		char name_buf[32];
		sprintf(name_buf, "flag_%d", i);
		Register(flags[i], name_buf, USER);
		
		sprintf(name_buf, "value_%d", i);
		Register(values[i], name_buf, USER);
	}
	
	const char* argv[] = {
		"program",
		"-flag_0", "-value_0", "0",
		"-flag_1", "-value_1", "1",
		"-flag_2", "-value_2", "2",
		"-flag_3", "-value_3", "3",
		"-flag_4", "-value_4", "4",
		"-flag_5", "-value_5", "5",
		"-flag_6", "-value_6", "6",
		"-flag_7", "-value_7", "7",
		"-flag_8", "-value_8", "8",
		"-flag_9", "-value_9", "9"
	};
	
	Parse(argv, 31);
	
	for (int i = 0; i < 10; i++) {
		ASSERT(flags[i] == true)
		ASSERT(values[i] == i)
	}
}

TEST_CASE("Setting commandline parsing before registration", "[settings36]") {
	const char* argv[] = {
		"program",
		"-late_bool",
		"-late_int", "420",
		"-late_float", "69.420"
	};
	
	Parse(argv, 6);
	
	bool late_bool = false;
	int32_t late_int = 0;
	float late_float = 0.0f;
	
	Register(late_bool, "late_bool", USER);
	Register(late_int, "late_int", USER);
	Register(late_float, "late_float", USER);
	
	ASSERT(late_bool == true)
	ASSERT(late_int == 420)
	ASSERT(late_float == 69.420f)
}

TEST_CASE("Setting commandline parsing mixed timing", "[settings37]") {
	bool early_bool = false;
	Register(early_bool, "early_setting", USER);
	
	const char* argv[] = {
		"program",
		"-early_setting",
		"-late_setting", "99"
	};
	
	Parse(argv, 4);
	
	ASSERT(early_bool == true)
	
	int32_t late_int = 0;
	Register(late_int, "late_setting", USER);
	
	ASSERT(late_int == 99)
}

TEST_CASE("Setting multiple commandline parses", "[settings38]") {
	const char* argv1[] = {"program", "-setting1", "420"};
	Parse(argv1, 3);
	
	int32_t setting1 = 0;
	Register(setting1, "setting1", USER);
	ASSERT(setting1 == 420)
	
	const char* argv2[] = {"program", "-setting2", "69"};
	Parse(argv2, 3);
	
	int32_t setting2 = 0;
	Register(setting2, "setting2", USER);
	ASSERT(setting2 == 69)
	
	ASSERT(setting1 == 420)
}

TEST_CASE("Setting static int and float property", "[settings39]") {
	ASSERT(static_int == 420)
	ASSERT((int32_t)Get("static-int") == 420)
	
	ASSERT(static_float == -69.0f)
	ASSERT((float)Get("static-float") == -69.0f)
	
	const char* argv[] = {"program", "-static-int", "69", "-static-float", "-420.0"};
	Parse(argv, 5);
	
	ASSERT(static_int == 69)
	ASSERT((int32_t)Get("static-int") == 69)
	
	ASSERT(static_float == -420.0f)
	ASSERT((float)Get("static-float") == -420.0f)
}

END_TEST