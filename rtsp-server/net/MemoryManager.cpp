#include "MemoryManager.h"

using namespace xop;

void *xop::Alloc(const uint32_t size)
{
	return MemoryManager::Instance().Alloc(size);
}

void xop::Free(void *ptr)
{
	return MemoryManager::Instance().Free(ptr);
}

MemoryPool::MemoryPool() = default;

MemoryPool::~MemoryPool()
{
	if (memory_) {
		free(memory_);
	}
}

void MemoryPool::Init(const uint32_t size, const uint32_t n)
{
	if (memory_) {
		return;
	}

	block_size_ = size;
	num_blocks_ = n;
	memory_ = static_cast<char *>(
		malloc(num_blocks_ * (block_size_ + sizeof(MemoryBlock))));
	head_ = reinterpret_cast<MemoryBlock *>(memory_);
	head_->block_id = 1;
	head_->pool = this;
	head_->next = nullptr;

	MemoryBlock *current = head_;
	for (uint32_t n = 1; n < num_blocks_; n++) {
		auto *next = reinterpret_cast<MemoryBlock *>(
			memory_ + n * (block_size_ + sizeof(MemoryBlock)));
		next->block_id = n + 1;
		next->pool = this;
		next->next = nullptr;

		current->next = next;
		current = next;
	}
}

void *MemoryPool::Alloc(const uint32_t size)
{
	std::lock_guard locker(mutex_);
	if (head_ != nullptr) {
		MemoryBlock *block = head_;
		head_ = head_->next;
		return reinterpret_cast<char *>(block) + sizeof(MemoryBlock);
	}

	return nullptr;
}

void MemoryPool::Free(void *ptr)
{
	if (const auto block = reinterpret_cast<MemoryBlock *>(
		    static_cast<char *>(ptr) - sizeof(MemoryBlock));
	    block->block_id != 0) {
		std::lock_guard locker(mutex_);
		block->next = head_;
		head_ = block;
	}
}

MemoryManager::MemoryManager()
{
	memory_pools_[0].Init(4096, 50);
	memory_pools_[1].Init(40960, 10);
	memory_pools_[2].Init(102400, 5);
	//memory_pools_[3].Init(204800, 2);
}

MemoryManager::~MemoryManager() = default;

MemoryManager &MemoryManager::Instance()
{
	static MemoryManager s_mgr;
	return s_mgr;
}

void *MemoryManager::Alloc(const uint32_t size)
{
	for (auto &memory_pool : memory_pools_) {
		if (size <= memory_pool.BolckSize()) {
			if (void *ptr = memory_pool.Alloc(size);
			    ptr != nullptr) {
				return ptr;
			}
			break;
		}
	}

	const auto block =
		static_cast<MemoryBlock *>(malloc(size + sizeof(MemoryBlock)));
	block->block_id = 0;
	block->pool = nullptr;
	block->next = nullptr;
	return reinterpret_cast<char *>(block) + sizeof(MemoryBlock);
}

void MemoryManager::Free(void *ptr)
{
	const auto block = reinterpret_cast<MemoryBlock *>(
		static_cast<char *>(ptr) - sizeof(MemoryBlock));

	if (MemoryPool *pool = block->pool;
	    pool != nullptr && block->block_id > 0) {
		pool->Free(ptr);
	} else {
		free(block);
	}
}
