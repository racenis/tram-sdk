
#include <framework/uid.cpp>
#include <framework/logging.cpp>
#include <framework/system.cpp>

#include <platform/file.h>

#include <map>

// testable unit
#include <framework/file.cpp>

// test framework
#include <templates/test.h>

namespace tram::Platform {
	void TryDebugging() {
		
	}
	
	void ShowErrorDialog(const char* message, const char* title) {
		
	}
}

namespace tram {

static std::map<std::string, std::string> all_contents;

class MockReader : public FileReader {
public:
	MockReader(std::string name) : name(name) {}
	~MockReader() = default;
	
	const char* GetContents() {
		if (name == "invalidfile") return nullptr;
		return all_contents[name].c_str();
	}
	
	size_t GetSize() {
		if (name == "invalidfile") return 0;
		return all_contents[name].size();
	}
	
	FileStatus GetStatus() {
		if (name == "invalidfile") return FileStatus::IO_ERROR;
		return FileStatus::READY;
	}
	
	void Yeet() {
		delete this;
	}
	
	static FileReader* GetReader(const char* path) {
		return new MockReader(path);
	}
	
	std::string name;
};

class MockWriter : public FileWriter {
public:
	MockWriter(std::string name) : name(name) {}
	~MockWriter() = default;
	
	void SetContents(const char* contents, size_t size) {
		if (name == "invalidfile") return;
		all_contents[name] += std::string_view{contents, size};
	}
	
	FileStatus GetStatus() {
		if (name == "invalidfile") return FileStatus::IO_ERROR;
		return FileStatus::READY;
	}
	
	void Yeet() {
		delete this;
	}
	
	bool Flush() {
		if (name == "invalidfile") return false;
		return true;
	}

	static MockWriter* GetWriter(const char* path) {
		return new MockWriter(path);
	}

	std::string name;	
};

FileReader* FileReader::GetReader(const char* path) {
	return new MockReader(path);
}

FileWriter* FileWriter::GetWriter(const char* path) {
	return new MockWriter(path);
}

}

std::string GetMockContents(std::string filename) {
	return tram::all_contents[filename];
}

void SetMockContents(std::string filename, std::string contents) {
	tram::all_contents[filename] = contents;
}

using namespace tram;

START_TEST

TEST_CASE("File basic closing and opening", "[file_open]") {
	File f1("testfile.txt", File::READ | File::TEXT);
	ASSERT(f1.is_open())
	
	File f2("invalidfile", File::READ | File::TEXT);
	ASSERT(!f2.is_open())
	
	File f3("writefile.txt", File::WRITE | File::TEXT);
	ASSERT(f3.is_open())
}

TEST_CASE("File binary integers", "[file1]") {
	{
		File writer("binfile.bin", File::WRITE | File::BINARY);
		ASSERT(writer.is_open())
		
		writer.write_int8(127);
		writer.write_int16(-420);
		writer.write_int32(694206969);
		writer.write_int64(-9876543210LL);
		
		writer.write_uint8(255);
		writer.write_uint16(65535);
		writer.write_uint32(4000000000U);
		writer.write_uint64(18446744073709551615ULL);
		
		ASSERT(!writer.was_error())
	}

	{
		File reader("binfile.bin", File::READ | File::BINARY);
		ASSERT(reader.is_open())

		ASSERT(reader.read_int8() == 127)
		ASSERT(reader.read_int16() == -420)
		ASSERT(reader.read_int32() == 694206969)
		ASSERT(reader.read_int64() == -9876543210LL)
		
		ASSERT(reader.read_uint8() == 255)
		ASSERT(reader.read_uint16() == 65535)
		ASSERT(reader.read_uint32() == 4000000000U)
		ASSERT(reader.read_uint64() == 18446744073709551615ULL)
		
		ASSERT(!reader.was_error())
	}
}

TEST_CASE("File binary floats", "[file2]") {
	{
		File writer("floatfile.bin", File::WRITE | File::BINARY);
		ASSERT(writer.is_open())
		
		writer.write_float32(6.7f);
		writer.write_float64(69.420);
		writer.write_float32(-420.69f);
		writer.write_float64(0.0);
		
		ASSERT(!writer.was_error())
		ASSERT(writer.flush())
	}
	
	{
		File reader("floatfile.bin", File::READ | File::BINARY);
		ASSERT(reader.is_open())
		
		float f1 = reader.read_float32();
		ASSERT(f1 == 6.7f)
		
		double d1 = reader.read_float64();
		ASSERT(d1 > 69.419 && d1 < 69.421)
		
		float f2 = reader.read_float32();
		ASSERT(f2 > -420.70f && f2 < -420.67f)
		
		double d2 = reader.read_float64();
		ASSERT(d2 == 0.0)
		
		ASSERT(!reader.was_error())
	}
}

TEST_CASE("File text mode reading and writing", "[file3]") {
	{
		File writer("textfile.txt", File::WRITE | File::TEXT);
		ASSERT(writer.is_open())
		
		writer.write_int32(420);
		writer.write_string("hello");
		writer.write_float32(1.5f);
		writer.write_string("world");
		
		ASSERT(!writer.was_error())
		ASSERT(writer.flush())
	}
	
	{
		File reader("textfile.txt", File::READ | File::TEXT);
		ASSERT(reader.is_open())
		ASSERT(reader.is_continue())
		
		ASSERT(reader.read_int32() == 420)
		std::string_view str1 = reader.read_string();
		ASSERT(str1 == "hello")
		
		float f = reader.read_float32();
		ASSERT(f == 1.5f)
		
		std::string_view str2 = reader.read_string();
		ASSERT(str2 == "world")
		
		ASSERT(!reader.was_error())
	}
}

TEST_CASE("File newlines and strings", "[file4]") {
	{
		File writer("multiline.txt", File::WRITE | File::TEXT);
		ASSERT(writer.is_open())
		
		writer.write_string("first line", '\0');
		writer.write_newline();
		writer.write_string("second line", '\0');
		writer.write_newline();
		writer.write_int32(420);
		writer.write_string("third", '\0');
		
		ASSERT(writer.flush())
	}
	
	{
		File reader("multiline.txt", File::READ | File::TEXT);
		ASSERT(reader.is_open())
		
		std::string_view line1 = reader.read_line();
		std::string_view line2 = reader.read_line();
		int32_t integer = reader.read_int32();
		std::string_view str = reader.read_token();

		// the writer inserts a space after each string, so we skip it here
		line1.remove_suffix(1);
		line2.remove_suffix(1);
		
		ASSERT(line1 == "first line")
		ASSERT(line2 == "second line")
		ASSERT(integer == 420)
		ASSERT(str == "third")
		
		ASSERT(!reader.was_error())
	}
}

TEST_CASE("File pause line", "[file6]") {
	SetMockContents("pausefile.txt", "line1 data1\nline2 data2\nline3 data3");
	
	File reader("pausefile.txt", File::READ | File::TEXT | File::PAUSE_LINE);
	ASSERT(reader.is_open())
	ASSERT(reader.is_continue())
	
	std::string_view tok1 = reader.read_token();
	std::string_view tok2 = reader.read_token();
	
	ASSERT(tok1 == "line1")
	ASSERT(tok2 == "data1")
	
	// parse cursor at linebreak
	ASSERT(reader.is_continue())
	ASSERT(reader.get_line() == 1)
	
	// these don't advance past linebreak
	reader.read_token();
	reader.read_token();
	
	// parse cursor at beginning of new line now
	reader.skip_linebreak();
	ASSERT(reader.get_line() == 2)
	
	std::string_view tok3 = reader.read_token();
	std::string_view tok4 = reader.read_token();
	
	ASSERT(tok3 == "line2")
	ASSERT(tok4 == "data2")
	
	ASSERT(!reader.was_error())
	
	// also doesn't advance over the linebreak
	reader.read_int32();
	
	// previous parse should have failed, due to being at linebreak (no chars left)
	ASSERT(reader.was_error())
	reader.reset_flags();
	
	reader.skip_linebreak();
	ASSERT(reader.get_line() == 3)
	
	std::string_view tok5 = reader.read_token();
	std::string_view tok6 = reader.read_token();
	
	ASSERT(tok5 == "line3")
	ASSERT(tok6 == "data3")
	
	ASSERT(!reader.was_error())
}

TEST_CASE("File errors", "[file7]") {
	SetMockContents("errorfile.txt", "420 69");
	
	File reader("errorfile.txt", File::READ | File::TEXT);
	ASSERT(reader.is_open())
	
	ASSERT(reader.read_int32() == 420)
	ASSERT(!reader.was_error())
	
	ASSERT(reader.read_int32() == 69)
	ASSERT(!reader.was_error())
	
	reader.read_int32();
	ASSERT(reader.was_error())
	ASSERT(!reader.is_continue())
	
	reader.reset_flags();
	ASSERT(!reader.was_error())
}

TEST_CASE("File invalid file handling", "[file8]") {
	File f1("invalidfile", File::READ | File::TEXT);
	ASSERT(!f1.is_open())
	
	File f2("invalidfile", File::WRITE | File::BINARY);
	ASSERT(!f2.is_open())
	
	File f3("invalidfile", File::READ | File::BINARY);
	ASSERT(!f3.is_open())
}

TEST_CASE("Fine line counting", "[file9]") {
	SetMockContents("linefile.txt", "line 1\nline 2\nline 3\nline 4");
	
	File reader("linefile.txt", File::READ | File::TEXT);
	ASSERT(reader.is_open())
	ASSERT(reader.get_line() == 1)
	
	reader.read_line();
	ASSERT(reader.get_line() == 2)
	
	reader.read_line();
	ASSERT(reader.get_line() == 3)
	
	reader.read_line();
	ASSERT(reader.get_line() == 4)
	
	ASSERT(!reader.was_error())
}

TEST_CASE("File different size binary values", "[file10]") {
	{
		File writer("mixed.bin", File::WRITE | File::BINARY);
		ASSERT(writer.is_open())
		
		writer.write_int8(-69);
		writer.write_uint32(0xDEADBEEF);
		writer.write_float32(4.20f);
		writer.write_uint64(6942069ULL);
		writer.write_int8(-50);
		writer.write_float64(-0.001);
		
		ASSERT(writer.flush())
	}
	
	{
		File reader("mixed.bin", File::READ | File::BINARY);
		ASSERT(reader.is_open())
		
		ASSERT(reader.read_int8() == -69)
		ASSERT(reader.read_uint32() == 0xDEADBEEF)
		
		float f = reader.read_float32();
		ASSERT(f == 4.20f)
		
		ASSERT(reader.read_uint64() == 6942069ULL)
		ASSERT(reader.read_int8() == -50)
		
		double d = reader.read_float64();
		ASSERT(d == -0.001)
		
		ASSERT(!reader.was_error())
	}
}

TEST_CASE("File that is empty", "[file11]") {
	SetMockContents("emptyfile.txt", "");
	
	File reader("emptyfile.txt", File::READ | File::TEXT);
	ASSERT(reader.is_open())
	ASSERT(!reader.is_continue())
}

TEST_CASE("File reading whole lines", "[read_line]") {
	SetMockContents("linereader.txt", "first line here\nsecond line here\nthird line here");
	
	File reader("linereader.txt", File::READ | File::TEXT);
	ASSERT(reader.is_open())
	
	std::string_view line1 = reader.read_line();
	std::string_view line2 = reader.read_line();
	std::string_view line3 = reader.read_line();
	
	ASSERT(line1 == "first line here")
	ASSERT(line2 == "second line here")
	ASSERT(line3 == "third line here")
	
	ASSERT(!reader.was_error())
}

TEST_CASE("File flushing", "[file12]") {
	File writer("flushtest.txt", File::WRITE | File::TEXT);
	ASSERT(writer.is_open())
	
	writer.write_int32(111);
	writer.write_int32(222);
	writer.write_int32(333);
	
	ASSERT(!writer.was_error())
	ASSERT(writer.flush())
	
	std::string contents = GetMockContents("flushtest.txt");
	ASSERT(contents.length() > 0)
}

TEST_CASE("File boundary integers", "[file13]") {
	{
		File writer("boundary.bin", File::WRITE | File::BINARY);
		ASSERT(writer.is_open())
		
		writer.write_int8(INT8_MIN);
		writer.write_int8(INT8_MAX);
		writer.write_int16(INT16_MIN);
		writer.write_int16(INT16_MAX);
		writer.write_uint8(0);
		writer.write_uint8(UINT8_MAX);
		
		ASSERT(writer.flush())
	}
	
	{
		File reader("boundary.bin", File::READ | File::BINARY);
		ASSERT(reader.is_open())
		
		ASSERT(reader.read_int8() == INT8_MIN)
		ASSERT(reader.read_int8() == INT8_MAX)
		ASSERT(reader.read_int16() == INT16_MIN)
		ASSERT(reader.read_int16() == INT16_MAX)
		ASSERT(reader.read_uint8() == 0)
		ASSERT(reader.read_uint8() == UINT8_MAX)
		
		ASSERT(!reader.was_error())
	}
}

TEST_CASE("File text comments", "[file14]") {
	SetMockContents("commentfile.txt", 
		"sussy kraut\n"
		"4.20 69 # number\n"
		"# this is a comment\n"
		"	 # also a comment\n"
		"egg roll # comment\n");
	
	File reader("commentfile.txt", File::READ | File::TEXT);
	ASSERT(reader.is_open())
	ASSERT(reader.is_continue())
	
	std::string_view tok1 = reader.read_token();
	std::string_view tok2 = reader.read_token();
	
	ASSERT(tok1 == "sussy")
	ASSERT(tok2 == "kraut")
	
	ASSERT(reader.read_float32() == 4.20f)
	ASSERT(reader.read_int32() == 69)
	
	std::string_view tok3 = reader.read_token();
	std::string_view tok4 = reader.read_token();
	
	ASSERT(tok3 == "egg")
	ASSERT(tok4 == "roll")

	ASSERT(!reader.was_error())
	ASSERT(!reader.is_continue())
}

END_TEST
