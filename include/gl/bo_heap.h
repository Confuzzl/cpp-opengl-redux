#pragma once

#include <glad/gl.h>

#include <utility>
#include <vector>

#include "gl/gl_object.h"
#include "util.h"

struct Block {
  static constexpr GLsizei MIN_SIZE = 32;
  GLintptr offset;
  GLsizei size;

  unsigned int page : 31;
  enum struct Type : unsigned int { FREE, ALLOCATED };
  Type type : 1;

  Block *prev, *next;
  Block *prevFree, *nextFree;

  Block(const GLintptr offset, const GLsizei size, const unsigned int page,
        const Type type, Block *const prev = nullptr,
        Block *const next = nullptr, Block *const prevFree = nullptr,
        Block *const nextFree = nullptr)
      : offset{offset}, size{size}, page{page}, type{type}, prev{prev},
        next{next}, prevFree{prevFree}, nextFree{nextFree} {}
};

/*template <typename U> */ struct VBOHeap {
  using T = vert_lay::pos;

  struct Page {
    static constexpr GLsizei MAX_VERTICES = 0xffff;
    GL::VBO<T> vbo{MAX_VERTICES};

    unsigned int index;

    Block *blockList;
    Block freeListHead;
    Block *freeList;

    Page(const unsigned int index)
        : index{index}, freeListHead{-1, -1, index, Block::Type::FREE},
          freeList{&freeListHead} {
      Block *const prologue = new Block(-1, -1, index, Block::Type::ALLOCATED);
      Block *const freeBlock =
          new Block(0, MAX_VERTICES, index, Block::Type::FREE);
      Block *const epilogue = new Block(-1, -1, index, Block::Type::ALLOCATED);

      fmt::println("page {}: pro {}, free {}, epi {}", fmt::ptr(this),
                   fmt::ptr(prologue), fmt::ptr(freeBlock), fmt::ptr(epilogue));

      prologue->prev = prologue->prevFree = nullptr;
      prologue->next = prologue->nextFree = freeBlock;

      freeBlock->prev = prologue;
      freeBlock->next = epilogue;
      freeBlock->prevFree = freeBlock->nextFree = freeList;
      freeList->prevFree = freeList->nextFree = freeBlock;

      epilogue->prev = epilogue->prevFree = freeBlock;
      epilogue->next = epilogue->nextFree = nullptr;

      blockList = prologue;
    }
    ~Page() {
      Block *curr = blockList;
      while (curr) {
        Block *const next = curr->next;
        delete curr;
        curr = next;
      }
    }

    Block *tryAllocate(GLsizei count) const {
      if (count < Block::MIN_SIZE)
        count = Block::MIN_SIZE;

      Block *const head = freeList;
      for (Block *curr = freeList->nextFree; curr != head;
           curr = curr->nextFree) {
        if (curr->type != Block::Type::FREE)
          throw std::runtime_error{"block wasnt free"};

        const auto diff = curr->size - count;
        if (diff < 0)
          continue;

        if (diff < Block::MIN_SIZE) { // splinter
          // [PREV_FREE][PREV][FREE.][NEXT][NEXT_FREE]
          // [PREV_FREE][PREV][ALLOC][NEXT][NEXT_FREE]
          Block *const prevFree = curr->prevFree, *const nextFree =
                                                      curr->nextFree;
          prevFree->nextFree = nextFree;
          nextFree->prevFree = prevFree;

          curr->type = Block::Type::ALLOCATED;
          curr->nextFree = curr->prevFree = nullptr;
        } else {
          // [PREV_FREE][PREV][FREE.......][NEXT][NEXT_FREE]
          // [PREV_FREE][PREV][ALLOC][FREE][NEXT][NEXT_FREE]

          Block *const prevFree = curr->prevFree, *const nextFree =
                                                      curr->nextFree;

          Block *const newFree =
              new Block(curr->offset + count, diff, index, Block::Type::FREE,
                        curr, curr->next, prevFree, nextFree);
          prevFree->nextFree = nextFree->prevFree = newFree;

          curr->type = Block::Type::ALLOCATED;
          curr->size = count;
          curr->next = newFree;
          curr->nextFree = curr->prevFree = nullptr;
        }
        return curr;
      }

      return nullptr;
    }

    void deallocate(Block *const block) {
      if (block->type != Block::Type::ALLOCATED)
        throw std::runtime_error{"block wasnt allocated"};

      Block *const prev = block->prev, *const next = block->next;

      if (prev->type == Block::Type::FREE) {
        Block *const start = prev;
        if (next->type == Block::Type::FREE) {
          // [FREE.][ALLOC][FREE.]
          // [FREE...............]
          start->size += block->size + next->size;

          start->next = next->next;
          next->next->prev = start;

          start->nextFree = next->nextFree;
          next->nextFree->prevFree = start;

          delete block;
          delete next;
        } else {
          // [FREE.][ALLOC][ALLOC]
          // [FREE........][ALLOC]
          start->size += block->size;

          start->next = next;
          next->prev = start;

          delete block;
        }
      } else {
        if (next->type == Block::Type::FREE) {
          // [ALLOC][ALLOC][FREE.]
          // [ALLOC][FREE........]
          block->type = Block::Type::FREE;
          block->size += next->size;

          block->next = next->next;
          next->next->prev = block;

          next->prevFree->nextFree = block;
          block->prevFree = next->prevFree;
          block->nextFree = next->nextFree;
          next->nextFree->prevFree = block;

          delete next;
        } else {
          // [ALLOC][ALLOC][ALLOC]
          // [ALLOC][FREE.][ALLOC]
          block->type = Block::Type::FREE;

          // head <-> after
          // head <-> free <-> after
          Block *const after = freeList->nextFree;
          freeList->nextFree = block;
          block->prevFree = freeList;
          block->nextFree = after;
          after->prevFree = block;
        }
      }
    }

    void print() const {
      fmt::println("PAGE {}", index);
      for (const Block *curr = blockList; curr; curr = curr->next) {
        fmt::println("Block {} {{", fmt::ptr(curr));
        fmt::println("\toffset={} size={} page={} type={}", curr->offset,
                     curr->size, +curr->page,
                     static_cast<unsigned int>(curr->type));
        fmt::println("\tprev={} next={}", fmt::ptr(curr->prev),
                     fmt::ptr(curr->next));
        fmt::println("\tprevFree={} nextFree={}", fmt::ptr(curr->prevFree),
                     fmt::ptr(curr->nextFree));
        fmt::println("}}");
      }
    }
  };

  std::vector<Page> pages{};

  VBOHeap() { pages.emplace_back(0); }

  Block *allocate(const GLsizei count) {
    if (count > Page::MAX_VERTICES)
      throw std::runtime_error{
          fmt::format("cannot allocate vbo forr {} vertices max is {}", count,
                      Page::MAX_VERTICES)};

    for (Page &page : pages) {
      if (const auto block = page.tryAllocate(count)) {
        return block;
      }
    }
    auto &newPage = pages.emplace_back(static_cast<unsigned int>(pages.size()));
    if (const auto block = newPage.tryAllocate(count))
      return block;
    throw std::runtime_error{"new page failed to allocate"};
  }

  void deallocate(Block *const block) { pages[block->page].deallocate(block); }

  void print() const {
    for (const Page &page : pages) {
      page.print();
    }
  }
};

struct VBOVector {
  using T = vert_lay::pos;

  VBOHeap heap{};
  Block *storage;

  VBOVector() : VBOVector(1) {}
  VBOVector(const GLsizei count) : storage{heap.allocate(count)} {}
};