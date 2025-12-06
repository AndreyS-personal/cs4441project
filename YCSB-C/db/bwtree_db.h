#ifndef YCSB_C_BWTREE_DB_H_
#define YCSB_C_BWTREE_DB_H_


//CRUD IMPLEMENTED, SCAN NOT YET

// BW-Tree configuration macros HARDCODED, MIGHT CHANGE LATER
#define DBGROUP_MAX_THREAD_NUM 64
#define BW_TREE_PAGE_SIZE 1024
#define BW_TREE_DELTA_RECORD_NUM_THRESHOLD 4
#define BW_TREE_MAX_DELTA_RECORD_NUM 64
#define BW_TREE_MIN_NODE_SIZE 64
#define BW_TREE_MAX_VARIABLE_DATA_SIZE 128
#define BW_TREE_RETRY_THRESHOLD 10
#define BW_TREE_SLEEP_TIME 10


#include "core/db.h"
#include "core/properties.h"
#include <iostream>
#include <string>
#include <vector>
#include <functional>      
#include <cstdint>              // uint64_t
#include "bw_tree/bw_tree.hpp"

namespace ycsbc {

class BwTreeDB : public DB {
 public:
  // BwTree Configuration:
  using Key = uint64_t;
  using Value = uint64_t;
  using Index = ::dbgroup::index::bw_tree::BwTreeFixLen<Key, Value>;
  using ReturnCode = ::dbgroup::index::bw_tree::ReturnCode;

  // The actual data container
  using DataBlock = std::vector<KVPair>;

  BwTreeDB() : index_{} {}

  virtual ~BwTreeDB() {
  }

  void Init() {
    // std::cout << "BwTreeDB Initialized" << std::endl;
  }

  // READ
  int Read(const std::string &table, const std::string &key,
           const std::vector<std::string> *fields,
           std::vector<KVPair> &result) {
    std::string number_str = key.substr(4);
    Key k = std::stoull(number_str);
    
    auto val_opt = index_.Read(k);

    // mess, unsure if necessary
    if (val_opt) {
      DataBlock* data = reinterpret_cast<DataBlock*>(*val_opt);
      if (data) {
          result = *data; // Copy data to result
      }
      return DB::kOK;
    }
    return DB::kErrorNoData;
  }

  // SCAN (Stub - BwTree requires specific iterator logic, not sure how), only needed for workloads e and f
  int Scan(const std::string &table, const std::string &key,
           int len, const std::vector<std::string> *fields,
           std::vector<std::vector<KVPair>> &result) {
    return DB::kOK; 
  }

  // UPDATE
  int Update(const std::string &table, const std::string &key,
             std::vector<KVPair> &values) {
    Key k = HashKey(key);

    // refactor later
    // 1. Create new data block
    DataBlock* new_data = new DataBlock(values);
    Value new_val_ptr = reinterpret_cast<Value>(new_data);

    auto rc = index_.Update(k, new_val_ptr);

    if (rc == ReturnCode::kSuccess) {
      return DB::kOK;
    } else {
      delete new_data; // Cleanup if update failed (key didn't exist)
      return DB::kErrorNoData;
    }
  }

  // INSERT
  int Insert(const std::string &table, const std::string &key,
             std::vector<KVPair> &values) {
    Key k = HashKey(key);

    // 1. Allocate data on heap
    DataBlock* data = new DataBlock(values);
    
    // 2. Cast pointer to uint64_t
    Value val_ptr = reinterpret_cast<Value>(data);

    // 3. Insert
    auto rc = index_.Insert(k, val_ptr);

    if (rc == ReturnCode::kSuccess) {
      return DB::kOK;
    } else {
      // If insert failed (duplicate key), cleanup and try Update
      delete data;
      return Update(table, key, values);
    }
  }

  // DELETE
  int Delete(const std::string &table, const std::string &key) {
    Key k = HashKey(key);
    
    // Attempt to delete
    auto rc = index_.Delete(k);

    if (rc == ReturnCode::kSuccess) {
      return DB::kOK;
    }
    return DB::kErrorNoData;
  }

 private:
  Index index_;

  // Convert string key to uint64_t hash, bw tree uses (int, int) while ycsbc uses (str, str)
  uint64_t HashKey(const std::string &key) {
    return std::hash<std::string>{}(key);
  }
};

} // namespace ycsbc

#endif // YCSB_C_BWTREE_DB_H_