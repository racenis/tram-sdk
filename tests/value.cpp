
// dependencies
#include <framework/system.cpp>
#include <framework/logging.cpp>
#include <framework/uid.cpp>
#include <framework/type.cpp>

// testable unit
#include <framework/value.h>

// test framework
#include <templates/test.h>

namespace tram::Platform {
	void TryDebugging() {
		
	}
	
	void ShowErrorDialog(const char* message, const char* title) {
		
	}
}

using namespace tram;

START_TEST

TEST_CASE("Value with no definition", "[value1]") {
	value_t value;
	ASSERT(value.GetType() == TYPE_UNDEFINED)
	ASSERT(!value)
}

TEST_CASE("Value from bool", "[value2]") {
	value_t true_val = true;
	ASSERT(true_val.IsBool())
	ASSERT(true_val.GetType() == TYPE_BOOL)
	ASSERT(true_val)
	ASSERT((bool)true_val == true)
	
	value_t false_val = false;
	ASSERT(false_val.IsBool())
	ASSERT(!false_val)
	ASSERT((bool)false_val == false)
}

TEST_CASE("Value from integer", "[value3]") {
	value_t int8_val((int8_t)127);
	ASSERT(int8_val.IsInt())
	ASSERT(int8_val.GetType() == TYPE_INT32)
	ASSERT((int8_t)int8_val == 127)
	
	value_t int16_val((int16_t)-1234);
	ASSERT(int16_val.IsInt())
	ASSERT(int16_val.GetType() == TYPE_INT32)
	ASSERT((int16_t)int16_val == -1234)
	
	value_t int32_val((int32_t)123456);
	ASSERT(int32_val.IsInt())
	ASSERT(int32_val.GetType() == TYPE_INT32)
	ASSERT((int32_t)int32_val == 123456)
	
	value_t uint8_val((uint8_t)255);
	ASSERT(uint8_val.IsInt())
	ASSERT(uint8_val.GetType() == TYPE_UINT32)
	ASSERT((uint8_t)uint8_val == 255)
	
	value_t uint16_val((uint16_t)65535);
	ASSERT(uint16_val.IsInt())
	ASSERT(uint16_val.GetType() == TYPE_UINT32)
	ASSERT((uint16_t)uint16_val == 65535)
	
	value_t uint32_val((uint32_t)4000000000U);
	ASSERT(uint32_val.IsInt())
	ASSERT(uint32_val.GetType() == TYPE_UINT32)
	ASSERT((uint32_t)uint32_val == 4000000000U)
}

TEST_CASE("Value from floats", "[value4]") {
	value_t float_val = 420.69f;
	ASSERT(float_val.IsFloat())
	ASSERT(float_val.GetType() == TYPE_FLOAT32)
	float f = (float)float_val;
	ASSERT(f == 420.69f)
}

TEST_CASE("Value from UIDs", "[value5]") {
	value_t value = (name_t)"egg roll";
	ASSERT(value.IsName())
	ASSERT(value.GetType() == TYPE_NAME)
	
	name_t retrieved = value;
	ASSERT(retrieved == "egg roll")
	
	const char* str = value;
	ASSERT(strcmp(str, "egg roll") == 0)
}

TEST_CASE("Value from strings", "[value6]") {
	const char* original = "hello world";
	value_t value = original;
	ASSERT(value.IsString())
	ASSERT(value.GetType() == TYPE_STRING)
	
	const char* retrieved = value;
	ASSERT(retrieved == original)
}

TEST_CASE("Value strings not copied", "[value7]") {
	char buffer[32] = "egg";
	value_t value = buffer;
	ASSERT(value.IsString())
	
	const char* ptr1 = value;
	ASSERT(strcmp(ptr1, "egg") == 0)
	
	strcpy(buffer, "roll");
	
	const char* ptr2 = value;
	ASSERT(strcmp(ptr2, "roll") == 0)
	ASSERT(ptr1 == ptr2)
}

TEST_CASE("Value from vec2s", "[value8]") {
	value_t value = vec2(23.0f, 419.0f);
	ASSERT(value.IsVec2())
	ASSERT(value.GetType() == TYPE_VEC2)
	
	vec2 retrieved = (vec2)value;
	ASSERT(retrieved.x == 23.0f)
	ASSERT(retrieved.y == 419.0f)
}

TEST_CASE("Value from vec3s", "[value9]") {
	value_t value = vec3(23.0f, 419.0f, 4.20f);
	ASSERT(value.IsVec3())
	ASSERT(value.GetType() == TYPE_VEC3)
	
	vec3 retrieved = (vec3)value;
	ASSERT(retrieved.x == 23.0f)
	ASSERT(retrieved.y == 419.0f)
	ASSERT(retrieved.z == 4.20f)
}

TEST_CASE("Value from vec4s", "[value10]") {
	value_t value = vec4(23.0f, 419.0f, 4.20f, 42.1f);
	ASSERT(value.IsVec4())
	ASSERT(value.GetType() == TYPE_VEC4)
	
	vec4 retrieved = (vec4)value;
	ASSERT(retrieved.x == 23.0f)
	ASSERT(retrieved.y == 419.0f)
	ASSERT(retrieved.z == 4.20f)
	ASSERT(retrieved.w == 42.1f)
}

TEST_CASE("Value from quats", "[value11]") {
	value_t value = quat(1.0f, 0.0f, 0.0f, 0.0f);
	ASSERT(value.IsQuat())
	ASSERT(value.GetType() == TYPE_QUAT)
	
	quat retrieved = (quat)value;
	ASSERT(retrieved.w == 1.0f)
	ASSERT(retrieved.x == 0.0f)
	ASSERT(retrieved.y == 0.0f)
	ASSERT(retrieved.z == 0.0f)
}

TEST_CASE("Value copies", "[value12]") {
	value_t original = 42;
	value_t copy(original);
	
	ASSERT(copy.IsInt())
	ASSERT((int32_t)copy == 42)
	ASSERT(copy.GetType() == original.GetType())

	value_t original_float(420.69f);
	value_t copy_float(original_float);
	ASSERT(copy_float.IsFloat())
	ASSERT((float)copy_float == 420.69f)
}

TEST_CASE("Value assignments", "[value13]") {
	value_t value1 = 420;
	value_t value2 = 69;
	
	value2 = value1;
	ASSERT((int32_t)value2 == 420)
	ASSERT(value2.GetType() == value1.GetType())
	
	value_t value3 = 420.69f;
	value3 = value_t((int32_t)69);
	ASSERT(value3.IsInt())
	ASSERT((int32_t)value3 == 69)
}

TEST_CASE("Value same type comparisons", "[value14]") {
	value_t int1 = 420;
	value_t int2 = 420;
	value_t int3 = 69;
	
	ASSERT(int1 == int2)
	ASSERT(int1 != int3)
	
	value_t float1 = 4.20f;
	value_t float2 = 4.20f;
	value_t float3 = 6.9f;
	
	ASSERT(float1 == float2)
	ASSERT(float1 != float3)
	
	value_t name1 = "egg";
	value_t name2 = "egg";
	value_t name3 = "roll";

	ASSERT(name1 == name2)
	ASSERT(name1 != name3)
	
	value_t string1 = "egg";
	value_t string2 = "egg";
	value_t string3 = "roll";
	
	ASSERT(string1 == string2)
	ASSERT(string1 != string3)
}

TEST_CASE("Value comparisons with different types", "[value15]") {
	value_t int_val = 420;
	value_t float_val = 420.0f;
	value_t bool_val = true;
	value_t string_val = "egg roll";
	
	
	ASSERT(int_val == float_val)
	
	ASSERT(int_val != bool_val)
	ASSERT(int_val != string_val)
	
	ASSERT(bool_val != string_val)
	
	ASSERT(float_val != bool_val)
	ASSERT(float_val != string_val)
}

TEST_CASE("Value comparison between different ints", "[value16]") {
	value_t int8_val = (int8_t)69;
	value_t int16_val = (int16_t)69;
	value_t int32_val = (int32_t)69;
	
	ASSERT(int8_val == int16_val)
	ASSERT(int16_val == int32_val)
	ASSERT(int8_val == int32_val)
	
	value_t uint8_val = (uint8_t)69;
	ASSERT(int8_val == uint8_val)
}

TEST_CASE("Value conversions for non-bools", "[value18]") {
	value_t zero_val = 0;
	value_t nonzero_val = 420;
	ASSERT(!zero_val)
	ASSERT(nonzero_val)
	
	value_t float_zero_val = 0.0f;
	value_t float_nonzero_val = 4.20f;
	ASSERT(!float_zero_val)
	ASSERT(float_nonzero_val)
	
	value_t negative_val(-420);
	ASSERT(negative_val)
}

TEST_CASE("Value int and float conversions", "[value19]") {
	value_t int_val = 69;
	ASSERT(int_val.GetInt() == 69)
	
	value_t float_val = 420.69f;
	ASSERT(float_val.GetInt() == 420)
	
	value_t int_val2 = 420;
	ASSERT(int_val2.GetFloat() == 420.0f)
	
	value_t float_val2 = 420.69f;
	ASSERT(float_val2.GetFloat() == 420.69f)
}

TEST_CASE("Value type checking", "[value20]") {
	value_t bool_val(true);
	ASSERT(bool_val.IsBool())
	ASSERT(!bool_val.IsInt())
	ASSERT(!bool_val.IsFloat())
	
	value_t int_val = 420;
	ASSERT(int_val.IsInt())
	ASSERT(!int_val.IsBool())
	ASSERT(!int_val.IsFloat())
	
	value_t float_val = 420.69f;
	ASSERT(float_val.IsFloat())
	ASSERT(!float_val.IsInt())
	ASSERT(!float_val.IsBool())
	
	value_t name_val = (name_t)"egg";
	ASSERT(name_val.IsName())
	ASSERT(!name_val.IsString())
	
	value_t string_val = "roll";
	ASSERT(string_val.IsString())
	ASSERT(!string_val.IsName())
	
	value_t vec2_val = vec2(1.0f, 2.0f);
	ASSERT(vec2_val.IsVec2())
	ASSERT(!vec2_val.IsVec3())
	
	value_t vec3_val = vec3(1.0f, 2.0f, 3.0f);
	ASSERT(vec3_val.IsVec3())
	ASSERT(!vec3_val.IsVec2())
	ASSERT(!vec3_val.IsVec4())
	
	value_t vec4_val = vec4(1.0f, 2.0f, 3.0f, 4.0f);
	ASSERT(vec4_val.IsVec4())
	ASSERT(!vec4_val.IsVec3())
	
	value_t quat_val = quat(1.0f, 0.0f, 0.0f, 0.0f);
	ASSERT(quat_val.IsQuat())
	ASSERT(!quat_val.IsVec4())
}

TEST_CASE("Value bool conversions", "[value21]") {
	value_t int_val_zero((int32_t)0);
	ASSERT(!int_val_zero)
	
	value_t float_val_zero(0.0f);
	ASSERT(!float_val_zero)
	
	value_t int8_val_zero((int8_t)0);
	ASSERT(!int8_val_zero)
	
	value_t uint32_zero_val((uint32_t)0);
	ASSERT(!uint32_zero_val)
}

TEST_CASE("Value integer edge cases", "[value22]") {
	value_t int8_min_val((int8_t)INT8_MIN);
	value_t int8_max_val((int8_t)INT8_MAX);
	ASSERT((int8_t)int8_min_val == INT8_MIN)
	ASSERT((int8_t)int8_max_val == INT8_MAX)
	
	value_t uint8_max_val((uint8_t)UINT8_MAX);
	ASSERT((uint8_t)uint8_max_val == UINT8_MAX)
	
	value_t int32_min_val((int32_t)INT32_MIN);
	value_t int32_max_val((int32_t)INT32_MAX);
	ASSERT((int32_t)int32_min_val == INT32_MIN)
	ASSERT((int32_t)int32_max_val == INT32_MAX)
	
	ASSERT(int8_min_val)
	ASSERT(int8_max_val)
	ASSERT(uint8_max_val)
}

TEST_CASE("Value multiple assignments", "[value23]") {
	value_t value1 = 420;
	value_t value2 = 69;
	value_t value3 = 489;
	
	value3 = value2 = value1;
	
	ASSERT((int32_t)value1 == 420)
	ASSERT((int32_t)value2 == 420)
	ASSERT((int32_t)value3 == 420)
}

TEST_CASE("Value vector and quaternion components", "[value24]") {
	vec2 vec2_type = {420.0f, 69.0f};
	value_t vec2_val = vec2_type;
	vec2 vec2_copy_val = vec2_val;
	ASSERT(vec2_copy_val.x == vec2_type.x)
	ASSERT(vec2_copy_val.y == vec2_type.y)
	
	vec3 vec3_type = {420.0f, 69.0f, 489.0f};
	value_t vec3_val = vec3_type;
	vec3 vec3_copy_val = vec3_val;
	ASSERT(vec3_copy_val.x == vec3_type.x)
	ASSERT(vec3_copy_val.y == vec3_type.y)
	ASSERT(vec3_copy_val.z == vec3_type.z)
	
	vec4 vec4_type = {420.0f, 69.0f, 489.0f, 23.0f};
	value_t vec4_val = vec4_type;
	vec4 vec4_copy_val = vec4_val;
	ASSERT(vec4_copy_val.x == vec4_type.x)
	ASSERT(vec4_copy_val.y == vec4_type.y)
	ASSERT(vec4_copy_val.z == vec4_type.z)
	ASSERT(vec4_copy_val.w == vec4_type.w)
	
	quat quat_type = {420.0f, 69.0f, 489.0f, 23.0f};
	value_t quat_val = quat_type;
	quat quat_copy_val = quat_val;
	ASSERT(quat_copy_val.w == quat_type.w)
	ASSERT(quat_copy_val.x == quat_type.x)
	ASSERT(quat_copy_val.y == quat_type.y)
	ASSERT(quat_copy_val.z == quat_type.z)
}

TEST_CASE("Value negative number bool conversions", "[value25]") {
	value_t int_negative_val = -420;
	ASSERT(int_negative_val)
	
	value_t float_negative_val = -420.69f;
	ASSERT(float_negative_val)
}

END_TEST