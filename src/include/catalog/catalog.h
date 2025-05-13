//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// catalog.h
//
// Identification: src/include/catalog/catalog.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <exception>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "buffer/buffer_pool_manager.h"
#include "catalog/schema.h"
#include "container/hash/hash_function.h"
#include "storage/index/b_plus_tree_index.h"
#include "storage/index/extendible_hash_table_index.h"
#include "storage/index/index.h"
#include "storage/index/stl_ordered.h"
#include "storage/index/stl_unordered.h"
#include "storage/table/table_heap.h"
#include "storage/table/tuple.h"

namespace bustub {

/**
 * Typedefs
 */
using table_oid_t = uint32_t;
using column_oid_t = uint32_t;
using index_oid_t = uint32_t;

enum class IndexType { BPlusTreeIndex, HashTableIndex, STLOrderedIndex, STLUnorderedIndex };

/**
 * The TableInfo class maintains metadata about a table.
 */
struct TableInfo {
  /**
   * Construct a new TableInfo instance.
   * @param schema The table schema
   * @param name The table name
   * @param table An owning pointer to the table heap
   * @param oid The unique OID for the table
   */
  TableInfo(Schema schema, std::string name, std::unique_ptr<TableHeap> &&table, table_oid_t oid)
      : schema_{std::move(schema)}, name_{std::move(name)}, table_{std::move(table)}, oid_{oid} {}
  /** The table schema */
  Schema schema_;
  /** The table name */
  const std::string name_;
  /** An owning pointer to the table heap */
  std::unique_ptr<TableHeap> table_;
  /** The table OID */
  const table_oid_t oid_;
};

/**
 * The IndexInfo class maintains metadata about a index.
 */
struct IndexInfo {
  /**
   * Construct a new IndexInfo instance.
   * @param key_schema The schema for the index key
   * @param name The name of the index
   * @param index An owning pointer to the index
   * @param index_oid The unique OID for the index
   * @param table_name The name of the table on which the index is created
   * @param key_size The size of the index key, in bytes
   */
  IndexInfo(Schema key_schema, std::string name, std::unique_ptr<Index> &&index, index_oid_t index_oid,
            std::string table_name, size_t key_size, bool is_primary_key, IndexType index_type)
      : key_schema_{std::move(key_schema)},
        name_{std::move(name)},
        index_{std::move(index)},
        index_oid_{index_oid},
        table_name_{std::move(table_name)},
        key_size_{key_size},
        is_primary_key_{is_primary_key},
        index_type_(index_type) {}
  /** The schema for the index key */
  Schema key_schema_;
  /** The name of the index */
  std::string name_;
  /** An owning pointer to the index */
  std::unique_ptr<Index> index_;
  /** The unique OID for the index */
  index_oid_t index_oid_;
  /** The name of the table on which the index is created */
  std::string table_name_;
  /** The size of the index key, in bytes */
  const size_t key_size_;
  /** Is primary key index? */
  bool is_primary_key_;
  /** The index type */
  IndexType index_type_;
};

/**
 * The Catalog is a non-persistent catalog that is designed for
 * use by executors within the DBMS execution engine. It handles
 * table creation, table lookup, index creation, and index lookup.
 */
class Catalog {
 public:
  /** Indicates that an operation returning a `std::shared_ptr<TableInfo>` failed */
  static inline const std::shared_ptr<TableInfo> NULL_TABLE_INFO{nullptr};

  /** Indicates that an operation returning a `std::shared_ptr<IndexInfo>` failed */
  // const std::shared_ptr<IndexInfo> NULL_INDEX_INFO{nullptr};
  static inline const std::shared_ptr<IndexInfo> NULL_INDEX_INFO{nullptr};

  /**
   * Construct a new Catalog instance.
   * @param bpm The buffer pool manager backing tables created by this catalog
   * @param lock_manager The lock manager in use by the system
   * @param log_manager The log manager in use by the system
   */
  Catalog(BufferPoolManager *bpm, LockManager *lock_manager, LogManager *log_manager)
      : bpm_{bpm}, lock_manager_{lock_manager}, log_manager_{log_manager} {}

  /**
   * Create a new table and return its metadata.
   * @param txn The transaction in which the table is being created
   * @param table_name The name of the new table, note that all tables beginning with `__` are reserved for the system.
   * @param schema The schema of the new table
   * @param create_table_heap whether to create a table heap for the new table
   * @return A shared pointer to the metadata for the table
   */
  auto CreateTable(Transaction *txn, const std::string &table_name, const Schema &schema, bool create_table_heap = true)
      -> std::shared_ptr<TableInfo> {
    if (table_names_.count(table_name) != 0) {
      return NULL_TABLE_INFO;
    }

    // Construct the table heap
    std::unique_ptr<TableHeap> table = nullptr;

    // When create_table_heap == false, it means that we're running binder tests (where no txn will be provided) or
    // we are running shell without buffer pool. We don't need to create TableHeap in this case.
    if (create_table_heap) {
      table = std::make_unique<TableHeap>(bpm_);
    } else {
      // Otherwise, create an empty heap only for binder tests
      table = TableHeap::CreateEmptyHeap(create_table_heap);
    }

    // Fetch the table OID for the new table
    const auto table_oid = next_table_oid_.fetch_add(1);

    // Construct the table information
    auto meta = std::make_shared<TableInfo>(schema, table_name, std::move(table), table_oid);

    // Update the internal tracking mechanisms
    tables_.emplace(table_oid, meta);
    table_names_.emplace(table_name, table_oid);
    index_names_.emplace(table_name, std::unordered_map<std::string, index_oid_t>{});

    return meta;
  }

  /**
   * Query table metadata by name.
   * @param table_name The name of the table
   * @return A (non-owning) pointer to the metadata for the table
   */
  auto GetTable(const std::string &table_name) const -> std::shared_ptr<TableInfo> {
    auto table_oid = table_names_.find(table_name);
    if (table_oid == table_names_.end()) {
      // Table not found
      return NULL_TABLE_INFO;
    }

    auto meta = tables_.find(table_oid->second);
    BUSTUB_ASSERT(meta != tables_.end(), "Broken Invariant");

    return meta->second;
  }

  /**
   * Query table metadata by OID
   * @param table_oid The OID of the table to query
   * @return A shared pointer to the metadata for the table
   */
  auto GetTable(table_oid_t table_oid) const -> std::shared_ptr<TableInfo> {
    auto meta = tables_.find(table_oid);
    if (meta == tables_.end()) {
      return NULL_TABLE_INFO;
    }

    return meta->second;
  }

  /**
   * Create a new index, populate existing data of the table and return its metadata.
   * @param txn The transaction in which the table is being created
   * @param index_name The name of the new index
   * @param table_name The name of the table
   * @param schema The schema of the table
   * @param key_schema The schema of the key
   * @param key_attrs Key attributes
   * @param keysize Size of the key
   * @param hash_function The hash function for the index
   * @return A shared pointer to the metadata of the new table
   */
  template <class KeyType, class ValueType, class KeyComparator>
  auto CreateIndex(Transaction *txn, const std::string &index_name, const std::string &table_name, const Schema &schema,
                   const Schema &key_schema, const std::vector<uint32_t> &key_attrs, std::size_t keysize,
                   HashFunction<KeyType> hash_function, bool is_primary_key = false,
                   IndexType index_type = IndexType::BPlusTreeIndex) -> std::shared_ptr<IndexInfo> {
    // Reject the creation request for nonexistent table
    if (table_names_.find(table_name) == table_names_.end()) {
      return NULL_INDEX_INFO;
    }

    // If the table exists, an entry for the table should already be present in index_names_
    BUSTUB_ASSERT((index_names_.find(table_name) != index_names_.end()), "Broken Invariant");

    // Determine if the requested index already exists for this table
    auto &table_indexes = index_names_.find(table_name)->second;
    if (table_indexes.find(index_name) != table_indexes.end()) {
      // The requested index already exists for this table
      return NULL_INDEX_INFO;
    }

    // Construct index metdata
    auto meta = std::make_unique<IndexMetadata>(index_name, table_name, &schema, key_attrs, is_primary_key);

    // Construct the index, take ownership of metadata
    // TODO(Kyle): We should update the API for CreateIndex
    // to allow specification of the index type itself, not
    // just the key, value, and comparator types

    // TODO(chi): support both hash index and btree index
    std::unique_ptr<Index> index;
    if (index_type == IndexType::HashTableIndex) {
      index = std::make_unique<ExtendibleHashTableIndex<KeyType, ValueType, KeyComparator>>(std::move(meta), bpm_,
                                                                                            hash_function);
    } else if (index_type == IndexType::BPlusTreeIndex) {
      index = std::make_unique<BPlusTreeIndex<KeyType, ValueType, KeyComparator>>(std::move(meta), bpm_);
    } else if (index_type == IndexType::STLOrderedIndex) {
      index = std::make_unique<STLOrderedIndex<KeyType, ValueType, KeyComparator>>(std::move(meta), bpm_);
    } else if (index_type == IndexType::STLUnorderedIndex) {
      index =
          std::make_unique<STLUnorderedIndex<KeyType, ValueType, KeyComparator>>(std::move(meta), bpm_, hash_function);
    } else {
      UNIMPLEMENTED("Unsupported Index Type");
    }

    // Populate the index with all tuples in table heap
    auto table_meta = GetTable(table_name);
    for (auto iter = table_meta->table_->MakeIterator(); !iter.IsEnd(); ++iter) {
      auto [meta, tuple] = iter.GetTuple();
      // we have to silently ignore the error here for a lot of reasons...
      index->InsertEntry(tuple.KeyFromTuple(schema, key_schema, key_attrs), tuple.GetRid(), txn);
    }

    // Get the next OID for the new index
    const auto index_oid = next_index_oid_.fetch_add(1);

    // Construct index information; IndexInfo takes ownership of the Index itself
    auto index_info = std::make_shared<IndexInfo>(key_schema, index_name, std::move(index), index_oid, table_name,
                                                  keysize, is_primary_key, index_type);
    // Update internal tracking
    indexes_.emplace(index_oid, index_info);
    table_indexes.emplace(index_name, index_oid);
    return index_info;
  }

  /**
   * Get the index `index_name` for table `table_name`.
   * @param index_name The name of the index for which to query
   * @param table_name The name of the table on which to perform query
   * @return A (non-owning) pointer to the metadata for the index
   */
  auto GetIndex(const std::string &index_name, const std::string &table_name) -> std::shared_ptr<IndexInfo> {
    auto table = index_names_.find(table_name);
    if (table == index_names_.end()) {
      BUSTUB_ASSERT((table_names_.find(table_name) == table_names_.end()), "Broken Invariant");
      return NULL_INDEX_INFO;
    }

    auto &table_indexes = table->second;

    auto index_meta = table_indexes.find(index_name);
    if (index_meta == table_indexes.end()) {
      return NULL_INDEX_INFO;
    }

    auto index = indexes_.find(index_meta->second);
    BUSTUB_ASSERT((index != indexes_.end()), "Broken Invariant");

    return index->second;
  }

  /**
   * Get the index `index_name` for table identified by `table_oid`.
   * @param index_name The name of the index for which to query
   * @param table_oid The OID of the table on which to perform query
   * @return A (non-owning) pointer to the metadata for the index
   */
  auto GetIndex(const std::string &index_name, const table_oid_t table_oid) -> std::shared_ptr<IndexInfo> {
    // Locate the table metadata for the specified table OID
    auto table_meta = tables_.find(table_oid);
    if (table_meta == tables_.end()) {
      // Table not found
      return NULL_INDEX_INFO;
    }

    return GetIndex(index_name, table_meta->second->name_);
  }

  /**
   * Get the index identifier by index OID.
   * @param index_oid The OID of the index for which to query
   * @return A (non-owning) pointer to the metadata for the index
   */
  auto GetIndex(index_oid_t index_oid) -> std::shared_ptr<IndexInfo> {
    auto index = indexes_.find(index_oid);
    if (index == indexes_.end()) {
      return NULL_INDEX_INFO;
    }

    return index->second;
  }

  /**
   * Get all of the indexes for the table identified by `table_name`.
   * @param table_name The name of the table for which indexes should be retrieved
   * @return A vector of std::shared_ptr<IndexInfo> for each index on the given table, empty vector
   * in the event that the table exists but no indexes have been created for it
   */
  auto GetTableIndexes(const std::string &table_name) const -> std::vector<std::shared_ptr<IndexInfo>> {
    // Ensure the table exists
    if (table_names_.find(table_name) == table_names_.end()) {
      return std::vector<std::shared_ptr<IndexInfo>>{};
    }

    auto table_indexes = index_names_.find(table_name);
    BUSTUB_ASSERT((table_indexes != index_names_.end()), "Broken Invariant");

    std::vector<std::shared_ptr<IndexInfo>> indexes{};
    indexes.reserve(table_indexes->second.size());
    for (const auto &index_meta : table_indexes->second) {
      auto index = indexes_.find(index_meta.second);
      BUSTUB_ASSERT((index != indexes_.end()), "Broken Invariant");
      indexes.push_back(index->second);
    }

    return indexes;
  }

  auto GetTableNames() -> std::vector<std::string> {
    std::vector<std::string> result;
    for (const auto &x : table_names_) {
      result.push_back(x.first);
    }
    return result;
  }

 private:
  [[maybe_unused]] BufferPoolManager *bpm_;
  [[maybe_unused]] LockManager *lock_manager_;
  [[maybe_unused]] LogManager *log_manager_;

  /** Map table identifier -> table metadata. */
  std::unordered_map<table_oid_t, std::shared_ptr<TableInfo>> tables_;

  /** Map table name -> table identifiers. */
  std::unordered_map<std::string, table_oid_t> table_names_;

  /** The next table identifier to be used. */
  std::atomic<table_oid_t> next_table_oid_{0};

  /** Map index identifier -> index metadata. */
  std::unordered_map<index_oid_t, std::shared_ptr<IndexInfo>> indexes_;

  /** Map table name -> index names -> index identifiers. */
  std::unordered_map<std::string, std::unordered_map<std::string, index_oid_t>> index_names_;

  /** The next index identifier to be used. */
  std::atomic<index_oid_t> next_index_oid_{0};
};

}  // namespace bustub

template <>
struct fmt::formatter<bustub::IndexType> : formatter<string_view> {
  template <typename FormatContext>
  auto format(bustub::IndexType c, FormatContext &ctx) const {
    string_view name;
    switch (c) {
      case bustub::IndexType::BPlusTreeIndex:
        name = "BPlusTree";
        break;
      case bustub::IndexType::HashTableIndex:
        name = "Hash";
        break;
      case bustub::IndexType::STLOrderedIndex:
        name = "STLOrdered";
        break;
      case bustub::IndexType::STLUnorderedIndex:
        name = "STLUnordered";
        break;
      default:
        name = "Unknown";
        break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};
