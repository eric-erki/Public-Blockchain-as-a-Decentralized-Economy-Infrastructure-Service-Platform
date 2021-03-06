#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <chrono>

#include "tx/wasmcontracttx.h"
#include "wasm/types/inline_transaction.hpp"
#include "wasm/wasm_interface.hpp"
#include "wasm/datastream.hpp"
#include "wasm/wasm_trace.hpp"
#include "eosio/vm/allocator.hpp"
#include "persistence/cachewrapper.h"
#include "entities/receipt.h"

using namespace std;
using namespace wasm;
namespace wasm {

    typedef CNickID nick_name;
    class wasm_context;

    class wasm_context : public wasm_context_interface {

    public:
        wasm_context(CWasmContractTx &ctrl, inline_transaction &t, CCacheWrapper &cw,
                     vector <CReceipt> &receipts_in, bool mining, uint32_t depth = 0)
                : trx(t), control_trx(ctrl), database(cw), receipts(receipts_in), recurse_depth(depth) {
            reset_console();
        };

        ~wasm_context() {
            wasm_alloc.free();
        };

    public:
        void                  initialize();
        void                  execute(inline_transaction_trace &trace);
        void                  execute_one(inline_transaction_trace &trace);
        bool                  has_permission_from_inline_transaction(const permission &p);
        std::vector <uint8_t> get_code(uint64_t account);
// Console methods:
    public:
        void                      reset_console();
        std::ostringstream&       get_console_stream()       { return _pending_console_output; }
        const std::ostringstream& get_console_stream() const { return _pending_console_output; }

//virtual
    public:
        uint64_t    receiver() { return _receiver;    }
        uint64_t    contract() { return trx.contract; }
        uint64_t    action()   { return trx.action;   }

        void        execute_inline(inline_transaction t);
        bool        has_recipient    (uint64_t account  ) const;
        void        require_recipient(uint64_t recipient);
        const char* get_action_data()      { return trx.data.data(); }
        uint32_t    get_action_data_size() { return trx.data.size(); }
        bool        is_account(uint64_t account);
        void        require_auth (uint64_t account);
        void        require_auth2(uint64_t account, uint64_t permission) {}
        bool        has_authorization(uint64_t account) const;
        uint64_t    block_time() { return 0;      }
        void        exit      () { wasmif.exit(); }

        std::vector<uint64_t> get_active_producers();

        bool set_data(uint64_t contract, string k, string v) {
            CAccount contract_account;
            wasm::name contract_name = wasm::name(contract);
            WASM_ASSERT(database.accountCache.GetAccount(CNickID(contract_name.to_string()), contract_account),
                        account_operation_exception,
                        "wasm_context.set_data, contract account does not exist, contract = %s",
                        contract_name.to_string().c_str())

            return database.contractCache.SetContractData(contract_account.regid, k, v);
        }

        bool get_data(uint64_t contract, string k, string &v) {
            CAccount contract_account;
            wasm::name contract_name = wasm::name(contract);
            WASM_ASSERT(database.accountCache.GetAccount(CNickID(contract_name.to_string()), contract_account),
                        account_operation_exception,
                        "wasm_context.get_data, contract account does not exist, contract = %s",
                        contract_name.to_string().c_str())

            return database.contractCache.GetContractData(contract_account.regid, k, v);
        }

        bool erase_data(uint64_t contract, string k) {
            CAccount contract_account;
            wasm::name contract_name = wasm::name(contract);
            WASM_ASSERT(database.accountCache.GetAccount(CNickID(contract_name.to_string()), contract_account),
                        account_operation_exception,
                        "wasm_context.erase_data, contract account does not exist, contract = %s",
                        contract_name.to_string().c_str())

            return database.contractCache.EraseContractData(contract_account.regid, k);
        }

        bool contracts_console() {
            return SysCfg().GetBoolArg("-contracts_console", false) && control_trx.transaction_status == wasm::transaction_status_type::validating;
        }

        void console_append(string val) {
            _pending_console_output << val;
        }

        vm::wasm_allocator*       get_wasm_allocator() { return &wasm_alloc; }
        bool                      is_memory_in_wasm_allocator( const char* p ) { return wasm_alloc.is_in_range(p); }

        std::chrono::milliseconds get_max_transaction_duration() { return control_trx.get_max_transaction_duration(); }
        void                      update_storage_usage(uint64_t account, int64_t size_in_bytes);
        void                      pause_billing_timer ()  { control_trx.pause_billing_timer();  };
        void                      resume_billing_timer()  { control_trx.resume_billing_timer(); };

    public:
        inline_transaction&        trx;
        CWasmContractTx&           control_trx;
        CCacheWrapper&             database;
        vector<CReceipt>&          receipts;
        uint32_t                   recurse_depth;
        vector<uint64_t>           notified;
        vector<inline_transaction> inline_transactions;

        wasm::wasm_interface       wasmif;
        vm::wasm_allocator         wasm_alloc;
        uint64_t                   _receiver;

    private:
        std::ostringstream         _pending_console_output;
    };
}
