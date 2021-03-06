#pragma once

#include <cstdint>
#include <string>
#include "commons/tinyformat.h"

using namespace std;

#define WASM_ASSERT( expr, exc_type, ... )       \
   if ( !( expr ) ) {                            \
       string str = tfm::format( __VA_ARGS__ );  \
       std::ostringstream o;                     \
       o << __FILE__ << ":" << __LINE__          \
         << ":[" << __FUNCTION__ << "], "        \
         << str;                                 \
       throw exc_type( o.str().c_str() ) ; }

#define WASM_THROW( exc_type, ... )    \
       WASM_ASSERT( false, exc_type,  __VA_ARGS__ )

#define WASM_RETHROW_EXCEPTIONS( exc_type, ... )          \
    catch( wasm::exception& e ) {                         \
         string str = tfm::format( __VA_ARGS__ );         \
         std::ostringstream o;                            \
         o << e.detail() << " , " << str;                 \
         e.msg = o.str();                                 \
         throw;                                           \
    } catch( ... ) {                                      \
         string str = tfm::format( __VA_ARGS__ );         \
         throw exc_type(str.c_str());                     \
    }

#define WASM_CAPTURE_AND_RETHROW( ... )             \
    catch( wasm::exception& e ) {                   \
       throw;                                       \
    } catch( ... ) {                                \
         string str = tfm::format( __VA_ARGS__ );   \
         throw wasm_assert_exception( str.c_str()); \
    }

namespace wasm {
    struct exception : public std::exception {
        virtual const char *what() const throw() = 0;
        virtual const char *detail() const throw() = 0;
        virtual uint32_t code() const throw() = 0;
        string msg;  
    };
}

#define WASM_DECLARE_EXCEPTION( name, _code, _what )                              \
   struct name : public wasm::exception {                                         \
      name(const char* m) { msg = string(m); }                                    \
      virtual const char* what()const throw() { return _what; }                   \
      virtual const char* detail()const throw() { return msg.c_str(); }           \
      virtual uint32_t code()const throw() { return _code; }                      \
   };

namespace wasm {
    WASM_DECLARE_EXCEPTION(wasm_exception,                       5000000, "wasm exception")
    WASM_DECLARE_EXCEPTION(abi_parse_exception,                  5000001, "abi parse exception")
    WASM_DECLARE_EXCEPTION(abi_serialization_deadline_exception, 5000002, "abi serialization deadline exception")
    WASM_DECLARE_EXCEPTION(unsupport_abi_version_exception,      5000003, "unsupport abi version exception")
    WASM_DECLARE_EXCEPTION(invalid_type_inside_abi,              5000004, "invalid type inside abi")
    WASM_DECLARE_EXCEPTION(duplicate_abi_def_exception,          5000005, "duplicate abi def exception")
    WASM_DECLARE_EXCEPTION(abi_circular_def_exception,           5000006, "abi circular def exception")
    WASM_DECLARE_EXCEPTION(transaction_exception,                5000007, "transaction exception")
    WASM_DECLARE_EXCEPTION(unpack_exception,                     5000008, "unpack exception")
    WASM_DECLARE_EXCEPTION(account_operation_exception,          5000009, "account operation exception")
    WASM_DECLARE_EXCEPTION(wasm_assert_exception,                5000010, "wasm assert exception")
    WASM_DECLARE_EXCEPTION(symbol_type_exception,                5000011, "symbol type exception")
    WASM_DECLARE_EXCEPTION(array_size_exceeds_exception,         5000012, "array size exceeds exception")
    WASM_DECLARE_EXCEPTION(pack_exception,                       5000013, "pack exception")
    WASM_DECLARE_EXCEPTION(inline_transaction_too_big,           5000014, "inline transaction too big")
    WASM_DECLARE_EXCEPTION(api_data_size_too_big_exception,      5000015, "wasm api data too big")
    WASM_DECLARE_EXCEPTION(overlapping_memory_error,             5000016, "memcpy can only accept non-aliasing pointers")
    WASM_DECLARE_EXCEPTION(unsatisfied_authorization,            5000017, "unsatisfied authorization")
    WASM_DECLARE_EXCEPTION(abort_called,                         5000018, "abort called")
    WASM_DECLARE_EXCEPTION(wasm_assert_code_exception,           5000019, "wasm assert code")
    WASM_DECLARE_EXCEPTION(wasm_execution_error,                 5000020, "wasm execution error")
    WASM_DECLARE_EXCEPTION(file_read_exception,                  5000021, "file read exception")
    WASM_DECLARE_EXCEPTION(missing_auth_exception,               5000022, "missing auth exception")
    WASM_DECLARE_EXCEPTION(fuel_fee_exception,                   5000023, "fuel fee exception")
    WASM_DECLARE_EXCEPTION(wasm_timeout_exception,               5000024, "timeout exception")
    WASM_DECLARE_EXCEPTION(asset_type_exception,                 5000025, "asset type exception")
    WASM_DECLARE_EXCEPTION(wasm_memory_exception,                5000026, "wasm memory exception")
} //wasm
