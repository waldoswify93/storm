#ifndef STORM_STORAGE_BITVECTORHASHMAP_H_
#define STORM_STORAGE_BITVECTORHASHMAP_H_

#include <cstdint>
#include <functional>

#include "storm/storage/BitVector.h"

namespace storm {
    namespace storage {
        
        /*!
         * This class represents a hash-map whose keys are bit vectors. The value type is arbitrary. Currently, only
         * queries and insertions are supported. Also, the keys must be bit vectors with a length that is a multiple of
         * 64.
         */
        template<typename ValueType, typename Hash = std::hash<storm::storage::BitVector>>
        class BitVectorHashMap {
        public:
            class BitVectorHashMapIterator {
            public:
                /*! Creates an iterator that points to the bucket with the given index in the given map.
                 *
                 * @param map The map of the iterator.
                 * @param indexIt An iterator to the index of the bucket the iterator points to.
                 */
                BitVectorHashMapIterator(BitVectorHashMap const& map, BitVector::const_iterator indexIt);
                
                // Methods to compare two iterators.
                bool operator==(BitVectorHashMapIterator const& other);
                bool operator!=(BitVectorHashMapIterator const& other);
                
                // Methods to move iterator forward.
                BitVectorHashMapIterator& operator++(int);
                BitVectorHashMapIterator& operator++();
                
                // Method to retrieve the currently pointed-to bit vector and its mapped-to value.
                std::pair<storm::storage::BitVector, ValueType> operator*() const;
                
            private:
                // The map this iterator refers to.
                BitVectorHashMap const& map;
                
                // An iterator to the bucket this iterator points to.
                BitVector::const_iterator indexIt;
            };
            
            typedef BitVectorHashMapIterator const_iterator;
            
            /*!
             * Creates a new hash map with the given bucket size and initial size.
             *
             * @param bucketSize The size of the buckets that this map can hold. This value must be a multiple of 64.
             * @param initialSize The number of buckets that is initially available.
             * @param loadFactor The load factor that determines at which point the size of the underlying storage is
             * increased.
             */
            BitVectorHashMap(uint64_t bucketSize = 64, uint64_t initialSize = 1000, double loadFactor = 0.75);
            
            /*!
             * Searches for the given key in the map. If it is found, the mapped-to value is returned. Otherwise, the
             * key is inserted with the given value.
             *
             * @param key The key to search or insert.
             * @param value The value that is inserted if the key is not already found in the map.
             * @return The found value if the key is already contained in the map and the provided new value otherwise.
             */
            ValueType findOrAdd(storm::storage::BitVector const& key, ValueType const& value);
            
            /*!
             * Sets the given key value pain in the map. If the key is found in the map, the corresponding value is
             * overwritten with the given value. Otherwise, the key is inserted with the given value.
             *
             * @param key The key to search or insert.
             * @param value The value to set.
             */
            void setOrAdd(storm::storage::BitVector const& key, ValueType const& value);

            /*!
             * Searches for the given key in the map. If it is found, the mapped-to value is returned. Otherwise, the
             * key is inserted with the given value.
             *
             * @param key The key to search or insert.
             * @param value The value that is inserted if the key is not already found in the map.
             * @return A pair whose first component is the found value if the key is already contained in the map and
             * the provided new value otherwise and whose second component is the index of the bucket into which the key
             * was inserted.
             */
            std::pair<ValueType, std::size_t> findOrAddAndGetBucket(storm::storage::BitVector const& key, ValueType const& value);
            
            /*!
             * Sets the given key value pain in the map. If the key is found in the map, the corresponding value is
             * overwritten with the given value. Otherwise, the key is inserted with the given value.
             *
             * @param key The key to search or insert.
             * @param value The value to set.
             * @return The index of the bucket into which the key was inserted.
             */
            std::size_t setOrAddAndGetBucket(storm::storage::BitVector const& key, ValueType const& value);
            
            /*!
             * Retrieves the key stored in the given bucket (if any) and the value it is mapped to.
             *
             * @param bucket The index of the bucket.
             * @return The content and value of the named bucket.
             */
            std::pair<storm::storage::BitVector, ValueType> getBucketAndValue(std::size_t bucket) const;
            
            /*!
             * Retrieves the value associated with the given key (if any). If the key does not exist, the behaviour is
             * undefined.
             *
             * @return The value associated with the given key (if any).
             */
            ValueType getValue(storm::storage::BitVector const& key) const;
            
            /*!
             * Checks if the given key is already contained in the map.
             *
             * @param key The key to search
             * @return True if the key is already contained in the map
             */
            bool contains(storm::storage::BitVector const& key) const;

            /*!
             * Retrieves an iterator to the elements of the map.
             *
             * @return The iterator.
             */
            const_iterator begin() const;

            /*!
             * Retrieves an iterator that points one past the elements of the map.
             *
             * @return The iterator.
             */
            const_iterator end() const;

            /*!
             * Retrieves the size of the map in terms of the number of key-value pairs it stores.
             *
             * @return The size of the map.
             */
            std::size_t size() const;
            
            /*!
             * Retrieves the capacity of the underlying container.
             *
             * @return The capacity of the underlying container.
             */
            std::size_t capacity() const;
            
            /*!
             * Performs a remapping of all values stored by applying the given remapping.
             *
             * @param remapping The remapping to apply.
             */
            void remap(std::function<ValueType(ValueType const&)> const& remapping);
            
        private:
            /*!
             * Retrieves whether the given bucket holds a value.
             *
             * @param bucket The bucket to check.
             * @return True iff the bucket is occupied.
             */
            bool isBucketOccupied(uint_fast64_t bucket) const;
            
            /*!
             * Searches for the bucket with the given key.
             *
             * @param key The key to search for.
             * @return A pair whose first component indicates whether the key is already contained in the map and whose
             * second component indicates in which bucket the key is stored.
             */
            std::pair<bool, std::size_t> findBucket(storm::storage::BitVector const& key) const;
            
            /*!
             * Searches for the bucket into which the given key can be inserted. If no empty bucket can be found, the
             * size of the underlying data structure is increased.
             *
             * @param key The key to search for.
             * @param increaseStorage A flag indicating whether the storage should be increased if no bucket can be found.
             * @return A tuple whose first component indicates whether the key is already contained in the map, whose
             * second component indicates in which bucket the key is supposed to be stored and whose third component is
             * an error flag indicating that the bucket could not be found (e.g. due to the restriction that the storage
             * must not be increased).
             */
            template<bool increaseStorage>
            std::tuple<bool, std::size_t, bool> findBucketToInsert(storm::storage::BitVector const& key);
            
            /*!
             * Inserts the given key-value pair without resizing the underlying storage. If that fails, this is
             * indicated by the return value.
             *
             * @param key The key to insert.
             * @param value The value to insert.
             * @return True iff the key-value pair could be inserted without resizing the storage.
             */
            bool insertWithoutIncreasingSize(storm::storage::BitVector const& key, ValueType const& value);
            
            /*!
             * Increases the size of the hash map and performs the necessary rehashing of all entries.
             */
            void increaseSize();
            
            /*!
             * Computes the next bucket in the probing sequence.
             */
            uint_fast64_t getNextBucketInProbingSequence(uint_fast64_t initialValue, uint_fast64_t currentValue, uint_fast64_t step) const;

            // The load factor determining when the size of the map is increased.
            double loadFactor;
            
            // The size of one bucket.
            uint64_t bucketSize;
            
            // The number of buckets.
            std::size_t numberOfBuckets;
            
            // The buckets that hold the elements of the map.
            storm::storage::BitVector buckets;
            
            // A bit vector that stores which buckets actually hold a value.
            storm::storage::BitVector occupied;
            
            // A vector of the mapped-to values. The entry at position i is the "target" of the key in bucket i.
            std::vector<ValueType> values;
            
            // The number of elements in this map.
            std::size_t numberOfElements;
            
            // An iterator to a value in the static sizes table.
            std::vector<std::size_t>::const_iterator currentSizeIterator;
            
            // Functor object that are used to perform the actual hashing.
            Hash hasher;
            
            // A static table that produces the next possible size of the hash table.
            static const std::vector<std::size_t> sizes;
            
#ifndef NDEBUG
            // Some performance metrics.
            mutable uint64_t numberOfInsertions;
            mutable uint64_t numberOfInsertionProbingSteps;
            mutable uint64_t numberOfFinds;
            mutable uint64_t numberOfFindProbingSteps;
#endif
        };

    }
}

#endif /* STORM_STORAGE_BITVECTORHASHMAP_H_ */
