// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/uid.h>

#include <cstring>

#include <templates/stackpool.h>
#include <templates/hashmap.h>

#include <config.h>

#include <murmur.h>

/**
 * @struct tram::UID framework/uid.h <framework/uid.h>
 * 
 * Interned string type.
 * 
 * Should intern strings in O(1) as long as the total number of interned strings
 * is lower than UID_LIMIT. 
 * In either case, interned string comparison will always be O(1).
 * If you are converting a lot of strings, it is best to do so early, because
 * the internment process, altough nearly constant time, does, in fact, take
 * some time.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/framework/uid.html
 */
 
 /**
 * @typedef UID name_t
 * Alias for UID.
 */

namespace tram {

static StackPool<char> string_pool("Name pool", UID_CHARACTER_LIMIT, {'n', 'o', 'n', 'e', '\0'});
static Hashmap<UID> string_list("Name list", UID_LIMIT, {{MurmurHash("none"), UID()}});

UID::UID(const std::string& value) {
    const char* str = value.c_str();
    *this = UID(str);
}

// TODO: make this THREAD SAFE!!
/* in order to make this thread-safe, we would need to do the following:
 * 1. make sure that Hashmap lookups are thread safe
 * 2. add a lock to the string pool
 * 3. just before the string gets added to the string pool, lock it
 * 4. double-check that string hasn't been added
 * 6. add the string as before
 * 7. the string pool, unlock it
 */

/// Interns a string.
/// This will intern the string and return a little UID that you can put in your
/// little pockets and take away with you.
/// For best results I recommend using the no_quotes() and is_empty() functions 
/// to first check whether a given string will also be a good name.
/// A string that isn't empty and doesn't need quotes is a good name.
/// @param value The string which will be interned.
/// @note Empty strings and 
UID::UID(const char* value) {
    if (!value) {
        this->key = 0;
        return;
    }
    
    uint64_t hash = MurmurHash2(value, strlen(value) + 1);
    
    // check if value is already added
    UID existing = string_list.Find(hash);
    
    if (existing) {
        if (strcmp(existing, value) != 0) {
            std::cout << "UID collision " << existing << " with " << value << std::endl; 
            abort();
        }
        
        this->key = existing.key;
        return;
    }
    
    // since we use the string "none" as the default string and since we defined
    // the bool operator on the UID to basically check whether a given string is
    // a "none", then if the user has passed a "none" string in as the parameter
    // in this method, the previous if statement will have been evaluated as
    // false, even though the string "none" is literally the only string which
    // hard-coded directly into the string table
    
    // to avoid filling up the string table with "none"s and to ensure the
    // constraint that identical strings have identical keys, in the case of
    // "none", we have bail out of this method, hence the following:
    
    // special case. very important!
    if (strcmp("none", value) == 0) return;
    
    // else add to string table
    this->key = string_pool.size();
    
    char* new_value = string_pool.AddNew(strlen(value) + 1);
    strcpy(new_value, value);
    
    string_list.Insert(hash, *this);
}

UID::operator std::string() const {
    return string_pool.begin() + key;
}

UID::operator char const*() const {
    return string_pool.begin() + key;
}

/// Checks whether the name is valid.
/// Due to bad memory handling practices, it is possible to create a name with
/// such a key, that when this name's string is looked up from the string table
/// and dereferenced, a segfault occurs.
/// This function checks whether a segfault is likely to occur when given a name
/// to check. This is done by a simple bounds check on the string table.
/// @return True, if using a name is safe.
bool UID::is_valid(const UID& value) {
    return value.key < string_pool.size();
}

/// Checks whether the name will need quotes.
/// When parsing a name from a text file, if a name has spaces in it, then it
/// will require quotes around it to be parsed in correctly.
/// This function checks whether a certain string will be able to be parsed as a
/// name correctly, without quotes.
/// @return True, if the string won't need quotes.
bool UID::no_quote(const char* str) {
    if (!str) return false;
    
    for (const char* c = str; *c != '\0'; c++) {
        if (*c == ' ') return false;
        if (*c == '"') return false;
        if (isspace(*c)) return false;
    }
    
    return true;
}

/// Checks whether a string consists of only whitespace.
/// @return True, if string contains at least a single non-whitespace character.
bool UID::is_empty(const char* str) {
    if (!str) return false;
    
    for (const char* c = str; *c != '\0'; c++) {
        if (!isspace(*c)) return false;
    }
    
    return true;
}

}